
#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include <wchar.h>
#include <chrono>
#include <stack>
#include <ctime>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/message_slot.h"
#include "../util/xml_parser.h"
#include "../util/card_bitmap.h"
#include "../util/card_bitmap_png.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

#include "timed_measurements.h"
#include "river_view_map.h"

#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_field_set.h"

#include "../SamplingStations/WaterSample.h"
#include "elevation_matrix.h"

void set_e85_colors
	(color_set *colors,
	const double min,
	const double max,
	const double mean,
	const double std)

{
	dynamic_string label;
	double interval;
	int row;
	COLORREF mid_colors [7] = {
		RGB (0, 128, 0), // darker green
		RGB (0, 128, 128), // darker cyan
		RGB (0, 255, 255), // lighter cyan
		RGB (0, 128, 255), // lighter blue
		RGB (0, 0, 255), // blue
		RGB (128, 0, 128), // darker magenta
		RGB (255, 0, 255) // lighter magenta
	};

	colors->add_color (RGB (192, 192, 192), 0, min, "0.00 (no report)"); // Gray for zeros

	label.format ("%.02lf - %.02lf", min, mean - std);
	colors->add_color (RGB (0, 255, 0), min, mean - std, label.get_text_ascii ()); // Brighter green

	// Divide (mean +/- STD) by 7 to use 7 colors between
	interval = ((mean + std) - (mean - std)) / 7.0;

	for (row = 0; row < 7; ++row) {
		label.format ("%.02lf - %.02lf", min + std + (double) row * interval, min + std + (double) (row + 1) * interval);
		colors->add_color (mid_colors [row], min + std + (double) row * interval, min + std + (double) (row + 1) * interval, label.get_text_ascii ()); // darker green
	}

	colors->add_color (RGB (255, 0, 0), mean + std, max, label.get_text_ascii ()); // red
}

int river_view_map::stream_size
	(const class map_object *segment)

// Higher streamlevel is a *smaller* stream

{
	int stream_width = 10;
	switch ((int) segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL]) {
		case 1:
			stream_width *= 7;
			break;
		case 2:
			stream_width *= 6;
			break;
		case 3:
			stream_width *= 5;
			break;
		case 4:
			stream_width *= 4;
			break;
		case 5:
			stream_width *= 3;
			break;
		case 6:
			stream_width *= 2;
	}

	return stream_width;
}

double river_view_map::distance_2d_xy
	(const logical_coordinate &start,
	const logical_coordinate &end)

// pythagorean theorem

{
	return sqrt (((end.x - start.x) * (end.x - start.x))
	+ ((end.y - start.y) * (end.y - start.y)));
}

double river_view_map::distance_2d_xz
	(const logical_coordinate &start,
	const logical_coordinate &end)

// pythagorean theorem

{
	return sqrt (((end.x - start.x) * (end.x - start.x))
	+ ((end.z - start.z) * (end.z - start.z)));
}

double river_view_map::distance_3d
	(const logical_coordinate &start,
	const logical_coordinate &end)

// pythagorean theorem

{
	return sqrt (((end.x - start.x) * (end.x - start.x))
	+ ((end.y - start.y) * (end.y - start.y))
	+ ((end.z - start.z) * (end.z - start.z)));
}

river_view_map::river_view_map ()

{
	days_per_frame = 0.25;
	seconds_per_frame = 21600;
	hours_per_frame = 6.0;
	frames_per_second = 18;
	area_definition = RIVER_VIEW_AREA_SOURCE_OUTLET;

	amplification_stream = 0.5;
	amplification_station = 1.0;

	simplify = false;
}

bool river_view_map::build_stream
	(dynamic_map *map_frame,
	std::chrono::system_clock::time_point &when,
	map_layer *layer,
	std::set <long> *trail,
	std::map <long, stream_measurements> *time_changes,
	// class bounding_cube *stream_area,
	class elevation_matrix *elevations,
	interface_window *view,
	dynamic_string &log)

// Copy layer to new map

{
	bool error = false;
	map_object *segment, *copy;
	logical_coordinate segment_start, segment_end, endpoint, offset;
	long object_count = 0, count_segment_nodes = 0, count_missing_elevation = 0, count_segments = 0;
	std::set <long>::iterator trail_stop;
	std::map <long, stream_measurements>::const_iterator stream_data = time_changes->end ();
	std::map <long, timed_measurements>::const_iterator node_data;
	std::map <std::chrono::system_clock::time_point, double>::const_iterator stream_reading;
	double elevation_min, elevation_max, elevation_step;
	map_layer *river_layer;

	river_layer = map_frame->create_new (MAP_OBJECT_POINT);

	river_layer->name = "Rivers";
	river_layer->attribute_count_numeric = 1;

	// NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL
	river_layer->colors.add_color (RGB (53, 94, 0), 0.0, 1.0); // Green darkest
	river_layer->colors.add_color (RGB (0, 255, 0), 1.0, 2.0); // Green 2
	river_layer->colors.add_color (RGB (148, 189, 94), 2.0, 3.0); // Green 3
	river_layer->colors.add_color (RGB (51, 204, 102), 3.0, 4.0); // Green 4
	river_layer->colors.add_color (RGB (0, 255, 0), 4.0, 5.0); // Green lightest

	river_layer->colors.add_color (RGB (76, 25, 0), 5.0, 6.0); // Yellow scale
	river_layer->colors.add_color (RGB (153, 102, 51), 6.0, 7.0);
	river_layer->colors.add_color (RGB (179, 179, 0), 7.0, 8.0);
	river_layer->colors.add_color (RGB (230, 230, 76), 8.0, 9.0);
	river_layer->colors.add_color (RGB (255, 255, 0), 9.0, 10.0);

	river_layer->colors.add_color (RGB (40, 0, 153), 10.0, 11.0); // Blue to cyan
	river_layer->colors.add_color (RGB (35, 0, 220), 11.0, 12.0);
	river_layer->colors.add_color (RGB (0, 71, 255), 12.0, 13.0);
	river_layer->colors.add_color (RGB (0, 0, 255), 13.0, 14.0);
	river_layer->colors.add_color (RGB (0, 184, 255), 14.0, 15.0);
	river_layer->colors.add_color (RGB (0, 255, 255), 15.0, 16.0);

	river_layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;

	view->update_progress ("Path Layer", 0);

	for (trail_stop = trail->begin ();
	trail_stop != trail->end ();
	++trail_stop) {
		segment = layer->match_id (*trail_stop);
		if (segment->type == MAP_OBJECT_POLYGON) {
			// segment->check_extent (stream_area);

			if (object_count % 100 == 0) {
				view->update_progress_formatted (1, "Count %ld / %ld", ++count_segments, layer->objects.size ());
				view->update_progress_formatted (2, "Path %ld", segment->id);
			}

			if ((elevation_min = segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_MINELEVM]) != -99.98) {
				elevation_max = segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_MAXELEVM];
				elevation_step = (elevation_max - elevation_min) / (double) ((map_polygon *) segment)->node_count;

				if ((stream_data == time_changes->end ())
				|| (stream_data->first != segment->id))
					stream_data = time_changes->find (segment->id);

				copy = river_layer->create_new (segment->type);
				copy->id = segment->id;
				copy->name = segment->name;
				copy->copy (segment, river_layer);
				copy->attributes_numeric [0] = segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL];

				river_layer->objects.push_back (copy);

		
				++object_count;
			}
			else
				++count_missing_elevation;
		}
	}

	map_frame->layers.push_back (river_layer);

	log.add_formatted ("River nodes written\t%ld\n", count_segment_nodes);
	log.add_formatted ("River segments missing elevation\t%ld\n", count_missing_elevation);

	return !error;
}

bool river_view_map::build_points
	(dynamic_map *map_frame,
	std::chrono::system_clock::time_point &when,
	std::set <map_object *> *stations,
	const char *node_name_prefix,
	const double base_size,
	class elevation_matrix *elevations,
	const bool use_names,
	const bool text_labels,
	interface_window *view,
	dynamic_string &log)

// E85 points

{
	// std::vector <map_object *>::const_iterator segment;
	bool error = false;
	std::set <map_object *>::const_iterator station;
	logical_coordinate endpoint, offset, sky_point, text_position, text_rotation;
	long object_count = 0;
	timed_measurements *time_set;
	std::map <std::chrono::system_clock::time_point, double>::const_iterator time;
	map_layer *point_layer;
	map_object *point;

	point_layer = map_frame->create_new (MAP_OBJECT_POINT);

	point_layer->name = "pe85_ret";
	point_layer->attribute_count_numeric = 1;
	point_layer->include_in_legend = true;

	// Ethanol values from database
	double min = 1.079;
	double max = 4.165666667;
	double mean = 2.208469073;
	double std = 0.562583115;
	set_e85_colors (&point_layer->colors, min, max, mean, std);

	point_layer->draw_as = MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;

	view->update_progress ("Point Layer", 0);
	for (station = stations->begin ();
	station != stations->end ();
	++station) {
			if (++object_count % 100 == 0) {
				view->update_progress_formatted (1, "Count %ld / %ld", object_count, stations->size ());
				view->update_progress_formatted (2, "Point %ld", (*station)->id);
			}

			point = point_layer->create_new (MAP_OBJECT_POINT);
			point->id = (*station)->id;
			point->name = (*station)->name;
			point->latitude = (*station)->latitude;
			point->longitude = (*station)->longitude;

			point_layer->objects.push_back (point);

			point->attributes_numeric [0] = 0;

			if ((time_set = (timed_measurements *) (*station)->attributes_void) != NULL) {

				for (time = time_set->amounts_ppm.begin ();
				time != time_set->amounts_ppm.end ();
				++time) {
					if (time->first <= when) {
						point->attributes_numeric [0] = time->second;
					}
				}
			}
		}

	map_frame->layers.push_back (point_layer);

	return !error;
}

void river_view_map::simulate_flow
	(std::map <long, stream_measurements> *stream_time_changes,
	std::set <long> *trail_set,
	const long first_id,
	const long target_id,
	const router_NHD *router,
	const map_layer *layer_rivers,
	dynamic_string &log)

{
	std::set <long> flow_selections;
	std::set <long>::iterator trail_stop;

	if (first_id != 0)
		flow_selections.insert (first_id);
	else
		// pick a few segments at random and send pulses downstream
		for (trail_stop = trail_set->begin ();
		trail_stop != trail_set->end ();
		++trail_stop) {
			if (rand () % 100 == 0)
				flow_selections.insert (*trail_stop);
		}

	for (trail_stop = flow_selections.begin ();
	trail_stop != flow_selections.end ();
	++trail_stop)
		simulate_flow_stream (stream_time_changes, router, layer_rivers, *trail_stop, target_id, log);
}

void river_view_map::simulate_flow_stream
	(std::map <long, stream_measurements> *time_changes,
	const router_NHD *router,
	const map_layer *layer_rivers,
	const long start_comid,
	const long end_comid,
	dynamic_string &log)

{
	std::vector <long long> downstream_ids;
	std::vector <long long>::const_reverse_iterator trail_stop;
	std::vector <double> pulse_wave;
	std::vector <double>::const_iterator crest;
	stream_measurements stream_reading;
	timed_measurements node_reading;
	int node_index;
	std::chrono::system_clock::time_point time, base_time, pulse_time;
	double stream_width;
	map_object *segment;
	bool done;

	pulse_wave.push_back (1.0);
	pulse_wave.push_back (5.5);
	pulse_wave.push_back (12.0);
	pulse_wave.push_back (18.0);
	pulse_wave.push_back (20.0);
	pulse_wave.push_back (21.0);
	pulse_wave.push_back (22.0);
	pulse_wave.push_back (22.0);
	pulse_wave.push_back (20.75);
	pulse_wave.push_back (20.0);
	pulse_wave.push_back (19.0);
	pulse_wave.push_back (16.0);
	pulse_wave.push_back (14.0);
	pulse_wave.push_back (12.0);
	pulse_wave.push_back (8.0);
	pulse_wave.push_back (4.0);
	pulse_wave.push_back (1.0);

	router->rivers.accumulate_downstream_ordered (start_comid, &downstream_ids);
	base_time = start_date + std::chrono::seconds (seconds_per_frame * 12);
	pulse_time = base_time + std::chrono::seconds (seconds_per_frame);
	for (trail_stop = downstream_ids.rbegin (), done = false;
	!done
	&& (trail_stop != downstream_ids.rend ());
	++trail_stop) {
		// Traverse from start_comid downstream
		// initial value
		segment = layer_rivers->match_id (*trail_stop);
		stream_width = (double) stream_size (segment);

		ASSERT (segment->type == MAP_OBJECT_POLYGON);

		stream_reading.clear ();
		stream_reading.comid = *trail_stop;
		for (node_index = 0; node_index < ((map_polygon *) segment)->node_count; ++node_index) {

			node_reading.id = node_index;
			node_reading.amounts_ppm.clear ();
			node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time - std::chrono::seconds (seconds_per_frame), stream_width));

			for (time = pulse_time, crest = pulse_wave.begin ();
			crest != pulse_wave.end ();
			time += std::chrono::seconds (seconds_per_frame), ++crest)
				// 10 +/- 6.0
				node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (time, (double) stream_width + *crest * amplification_stream));

			// end value
			node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (time, stream_width));

			stream_reading.node_readings.insert (std::pair <long, timed_measurements> (node_reading.id, node_reading));

			// advance 1 frame per node
			pulse_time += std::chrono::seconds (seconds_per_frame);
		}

		time_changes->insert (std::pair <long, stream_measurements> (stream_reading.comid, stream_reading));

		if (*trail_stop == end_comid)
			// Don't go all the way to mouth of river
			done = true;
	}

}

bool river_view_map::read_sampling_data
	(const char *filename_db_readings,
	std::set <map_object *> *plotting_stations,
	const std::chrono::system_clock::time_point &first_date,
	const std::chrono::system_clock::time_point &last_date,
	long *count_samples,
	// std::map <long, timed_measurements> *time_changes,
	interface_window *view,
	dynamic_string &log)

// Read dissolved oxygen database created by samplingstations.exe

// This version reads into time_changes rather than map_objects

{
	bool error = false;
	odbc_database_access *db_odbc;
	odbc_database_credentials creds;
	std::set <map_object *>::iterator station;
	std::map <long, timed_measurements>::iterator station_readings;
	WaterSampleVector samples;
	WaterStationID rowset_ids;
	WaterSample rowset_water;
	dynamic_string filter;
	timed_measurements *stream_readings;
	// std::map <long, long> id_by_comid;
	// std::map <long, long>::iterator comid;
	std::map <long, map_object *> station_by_water_id;
	std::map <long, map_object *>::iterator station_finder;
	long record_count, count_unmatched = 0;
	std::chrono::system_clock::time_point when;

	db_odbc = new odbc_database_access;
	creds.set_database_filename (filename_db_readings);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (db_odbc->open (&creds, view, log)) {

		rowset_ids.initialize_field_set ();
		rowset_water.initialize_field_set ();

		view->update_progress ("Reading WaterSample IDs");
		record_count = 0;

		// Database has station & agency
		for (station = plotting_stations->begin ();
		station != plotting_stations->end ();
		++station) {
			// Get ID from [Station ID]
			filter.clear ();
			filter += db_odbc->identifier_quote_character_open ();
			filter += "Station";
			filter += db_odbc->identifier_quote_character_close ();
			filter += "='";
			filter += (*station)->name;
			filter += "' AND ";
			filter += db_odbc->identifier_quote_character_open ();
			filter += "Agency";
			filter += db_odbc->identifier_quote_character_close ();
			filter += "='";
			filter += (*station)->attributes_text [0];
			filter += "'";

			if (rowset_ids.field_set.open_read (db_odbc, log, filter)) {
				if (rowset_ids.field_set.move_first (log))
					// id_by_comid.insert (std::pair <long, long> (rowset_ids.ID, (*station)->id));
					station_by_water_id.insert (std::pair <long, map_object *> (rowset_ids.ID, *station));
				else
					++count_unmatched;
				rowset_ids.field_set.close ();
			}
			if ((++record_count % 1000) == 0) 
				view->update_progress_formatted  (1, "Station Count %ld unmatched %ld", record_count, count_unmatched);
		}

		view->update_progress ("WaterSample IDs read");
		log.add_formatted ("Plotting stations not found in WaterSamples ID table\t%ld\n", count_unmatched);

		record_count = 0;

		if (rowset_water.field_set.open_read (db_odbc, log)) {
			if (rowset_water.field_set.move_first (log)) {
				do {
					rowset_water.Time.set_time_point (&when);
					if ((when < first_date)
					|| (when > last_date)) {
					}
					else {
						// log.add_formatted ("%ld\t", water_station_id);
						// log += rowset_water.Time.write (5);
						// log.add_formatted ("\t%d\t%d\t%.6lf\n", (int) rowset_water.Depth, (int) rowset_water.Min, rowset_water.Amount);

						/*
						if ((comid = id_by_comid.find (rowset_water.ID)) != id_by_comid.end ()) {

							// frame_number = timestamp_to_frame_number (rowset_water.Time, *first_date);

							/*
							if (rowset_water.ID == stream_readings.id)
								stream_readings.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
							else {
								if ((station_readings = time_changes->find (comid->second)) != time_changes->end ())
									stream_readings.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
								else {
									stream_readings.clear ();
									stream_readings.id = comid->second;
									stream_readings.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
									time_changes->insert (std::pair <long, timed_measurements> (stream_readings.id, stream_readings));
								}
							}
						}
							*/
						if ((station_finder = station_by_water_id.find (rowset_water.ID)) != station_by_water_id.end ()) {
							if ((stream_readings = (timed_measurements *) station_finder->second->attributes_void) != NULL)
								stream_readings->amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
							else {
								stream_readings = new timed_measurements;
								station_finder->second->attributes_void = stream_readings;
								stream_readings->id = station_finder->second->id;
								stream_readings->amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
							}
							++count_samples;
						}
					}
					if ((++record_count % 1000) == 0) 
						view->update_progress_formatted  (1, "WaterSamples %ld", record_count);
				} while (rowset_water.field_set.move_next (log));
			}
			rowset_water.field_set.close ();
		}
		else
			error = true;

			/*
					water_station_id = rowset_ids.ID;

					stream_readings.clear ();
					stream_readings.id = (*station)->id;

					filter.format ("%cID%c=%ld", db_odbc->identifier_quote_character_open (), db_odbc->identifier_quote_character_close (), water_station_id);
					if (rowset_water.field_set.open (db_odbc, log, filter)) {
						if (rowset_water.field_set.move_first (log)) {
							do {
								if ((rowset_water.Time < *first_date)
								|| (rowset_water.Time > *last_date)) {
								}
								else {
									// log.add_formatted ("%ld\t", water_station_id);
									// log += rowset_water.Time.write (5);
									// log.add_formatted ("\t%d\t%d\t%.6lf\n", (int) rowset_water.Depth, (int) rowset_water.Min, rowset_water.Amount);


									frame_number = timestamp_to_frame_number (rowset_water.Time, *first_date);
									stream_readings.amounts_ppm.insert (std::pair <long, double> (frame_number, rowset_water.Amount));
								}
							} while (rowset_water.field_set.move_next (log));
						}
					
						rowset_water.field_set.close ();
					}

					time_changes->insert (std::pair <long, timed_measurements> (stream_readings.id, stream_readings));
				}

				rowset_ids.field_set.close ();
			}
			else
				error = true;

			// Find IDs in table [Station ID] from station/agency ?

			// read samples by ID from [Dissolved Oxygen]
		}
		*/
		log.add_formatted ("DB open.\n");
	}
	else {
		delete db_odbc;
		db_odbc = NULL;
		error = true;
	}

	return !error;
}

bool river_view_map::nodes_ahead
	(const map_object *segment,
	const int polygon_index,
	const int node_index,
	map_object **segment_ahead,
	int *polygon_index_ahead,
	int *node_index_ahead,
	std::vector <long long>::iterator *current_comid,
	std::vector <long long> *downstream_ids,
	const map_layer *river_layer)

{
	int interval = 3;
	int leftover;
	bool path_ahead = false;

	if (segment) {
		leftover = (node_index + interval) - ((map_polygon *) segment)->node_count;
		if (leftover > 0) {
			// starting on the next segment
			if (++(*current_comid) != downstream_ids->end ()) {
				*segment_ahead = river_layer->match_id (**current_comid);
				*node_index_ahead = leftover - 1;
				path_ahead = true;
			}
		}
		else {
			*segment_ahead = (map_object *) segment;
			*node_index_ahead = node_index + interval;
			path_ahead = true;
		}
	}
	return path_ahead;
}

bool river_view_map::add_polygons
	(dynamic_map *map,
	std::chrono::system_clock::time_point &when,
	map_layer *layer,
	std::vector <long> *trail,
	const bool use_layer_color,
	const class elevation_matrix *elevations,
	interface_window *view,
	dynamic_string &log)

{
	// std::vector <map_object *>::const_iterator segment;
	bool error = false;
	std::vector <long>::const_iterator trail_stop;
	std::vector <long> color_ids;
	map_object *segment;
	long object_count = 0, count_segment_nodes = 0, count_segments = 0;
	map_layer *new_layer;
	map_object *polygon;

	new_layer = map->create_new (MAP_OBJECT_POLYGON);
	new_layer->name = layer->name;
	new_layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	new_layer->color = RGB (0, 0, 0);


	if (layer->type == MAP_OBJECT_POLYGON) {
		view->update_progress ("Polygon Layer", 0);

		for (trail_stop = trail->begin ();
		trail_stop != trail->end ();
		++trail_stop) {
			segment = layer->match_id (*trail_stop);

			if (object_count % 100 == 0) {
				view->update_progress_formatted (1, "Count %ld / %ld", ++count_segments, layer->objects.size ());
				view->update_progress_formatted (2, "Path %ld", segment->id);
			}

			if (elevations
			&& (elevations->data.size () > 0)) {
				// One altitude value per polygon
			}

			polygon = new_layer->create_new (MAP_OBJECT_POLYGON);
			polygon->copy (segment, new_layer);
			new_layer->objects.push_back (polygon);
		}

		log += layer->name;
		log.add_formatted (" nodes written\t%ld\n", count_segment_nodes);
	}
	map->layers.push_back (new_layer);

	return !error;
}

bool river_view_map::read_gridfloat_header
	(dynamic_string &filename_hdr,
	int *count_columns,
	int *count_rows,
	double *ll_latitude,
	double *ll_longitude,
	double *cellsize,
	float *missing_value,
	dynamic_string &log)

{
	FILE *f_hdr;
	char data [1024];
	dynamic_string input_line;
	std::vector <dynamic_string> tokens;
	int count_read;
	bool error = false;

	if (fopen_s (&f_hdr, filename_hdr.get_text_ascii (), "r") == 0) {
		while (fgets (data, 1024, f_hdr)) {
			input_line = data;
			tokens.clear ();
			if ((count_read = input_line.tokenize (" \t\n", &tokens, true, true)) == 2) {
				if (tokens [0] == "ncols")
					*count_columns = atoi (tokens [1].get_text_ascii ());
				else
					if (tokens [0] == "nrows")
						*count_rows = atoi (tokens [1].get_text_ascii ());
					else
						if (tokens [0] == "xllcorner")
							*ll_longitude = atof (tokens [1].get_text_ascii ());
						else
							if (tokens [0] == "yllcorner")
								*ll_latitude = atof (tokens [1].get_text_ascii ());
							else
								if (tokens [0] == "cellsize")
									*cellsize = atof (tokens [1].get_text_ascii ());
								else
									if (tokens [0] == "NODATA_value")
										*missing_value = (float) atof (tokens [1].get_text_ascii ());
									else
										if ((tokens [0] == "byteorder")
										&& (tokens [1] != "LSBFIRST")) {
											error = true;
											log += "ERROR, GridFloat byte order invalid.\n";
										}
			}
			else {
				log.add_formatted ("ERROR, token count %d incorrect in HDR file.\n", count_read);
				log += input_line;
				log += "\n";
				error = true;
			}
		}
	
		fclose (f_hdr);
	}
	else {
		error = true;
		log += "ERROR, can't open HDR file \"";
		log += filename_hdr;
		log += "\n";
	}

	log.add_formatted ("Matrix size\t%d\t%d\n", *count_columns, *count_rows);
	log.add_formatted ("LL\t%.6lf\t%.6lf\n", *ll_latitude, *ll_longitude);

	return !error;
}

bool river_view_map::read_gridfloat_heatmap
	(const dynamic_string &filename_flt,
	const dynamic_string &filename_image,
	const dynamic_string &filename_dae,
	const bounding_cube &boundary,
	elevation_matrix *elevations, // keep elevations to compute elevation of streets
	const bool create_image,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	logical_coordinate coordinate, node_zero, nw_corner, normal, coordinate_2, coordinate_3, last_coordinate;
	card_bitmap_png png_image;
	std::vector <std::tuple <long, long, long>> bitmap_values;
	std::tuple <long, long, long> bitmap_value;

	// 1m x 10m x 10m, x is scaled later
	// Base point should be the center of the 'x=0' face of the
	// wall_shape->make_box (0.0, -5.0, -5.0, 1.0, 5.0, 5.0);

	filename_struct f;
	FILE *f_flt;
	dynamic_string filename;
	int count_columns, count_rows, count_read, index_column;
	double ll_latitude, ll_longitude, cellsize;
	float missing_value, *float_data;
	int combine_factor = 1;

	f.parse (&filename_flt);
	f.set_suffix (L"hdr");
	f.write_file_specification (&filename);

	// floatn39w106_1.flt
	// floatn42w094_1.flt
	// floatn43w094_1.flt
	// usgs_ned_1_n40w105_gridfloat.flt
	if (f.prefix) {
		dynamic_string prefix = f.prefix;

		int index = 0;
		if ((index = prefix.match ("n", index)) >= 0) {
			if ((prefix.get_length () >= index + 6)
			&& isdigit (prefix.get_at_ascii (index + 1))
			&& isdigit (prefix.get_at_ascii (index + 2))
			&& (prefix.get_at_ascii (index + 3) == 'w')) {
				nw_corner.y = atof (prefix.mid (index + 1).get_text_ascii ());
				nw_corner.x = atof (prefix.mid (index + 4).get_text_ascii ()) * -1.0;
			}
		}
		else {
			log += "ERROR, can't derive NW corner from FLT filename.\n";
			log += prefix;
			log += "\n";
		}
	}

	// n43w094
	// nw_corner.x = -106.0; // -94.0;
	// nw_corner.y = 39.0; // 43.0;

	if (read_gridfloat_header (filename, &count_columns, &count_rows, &ll_latitude, &ll_longitude, &cellsize, &missing_value, log)) {

		filename = filename_flt;

		if (fopen_s (&f_flt, filename.get_text_ascii (), "rb") == 0) {
			int index_row = 0, count_output_columns, count_output_rows, start_row, start_column, end_row, end_column;
			std::vector <logical_coordinate> uncombined_positions;

			float_data = new float [count_columns];

			start_row = (int) floor ((nw_corner.y - boundary.y [1]) * count_rows);
			end_row = (int) ceil ((nw_corner.y - boundary.y [0]) * count_rows);
			if (end_row >= count_rows)
				end_row = count_rows - 1;

			start_column = (int) floor ((boundary.x [0] - nw_corner.x) * count_columns);
			end_column = (int) ceil ((boundary.x [1] - nw_corner.x) * count_columns);
			if (end_column >= count_columns)
				end_column = count_columns - 1;

			count_output_columns = (end_column - start_column) + 1; // count_columns
			count_output_rows = (end_row - start_row) + 1; // count_rows

			while ((count_read = fread (float_data, sizeof (float), count_columns, f_flt)) == count_columns) {

				// make triangle strips instead ?
				if ((index_row >= start_row)
				&& (index_row <= end_row)) {
					for (index_column = start_column; index_column <= end_column; ++index_column) {

						coordinate.x = ll_longitude + (double) index_column * cellsize;
						coordinate.y = ll_latitude + (double) (count_rows - index_row) * cellsize;
						/*
						if (float_data [index_column] != missing_value)
							coordinate.z = EARTH_RADIUS_KM * 1000.0 + float_data [index_column];
						else
							coordinate.z = EARTH_RADIUS_KM * 1000.0;

						// keep unconverted coordinates to set elevations in other layers
						elevations->add (coordinate);

						if (coordinate.z < min_elevation_meters)
							min_elevation_meters = coordinate.z;
						if (coordinate.z > max_elevation_meters)
							max_elevation_meters = coordinate.z;

						if (create_image)
							bitmap_values.push_back (std::make_tuple (index_column - start_column, index_row - start_row, (long) coordinate.z));

						coordinate = geographic_to_spherical (coordinate);
						uncombined_positions.push_back (coordinate);
						*/
					}
				}
				++index_row;
			}

			if (combine_factor > 1) {
				/*
				int combined_count;
				double *combined_cells;
				combined_count = 0;
				combined_cells = new double [count_columns / combine_factor];
				shape->positions.push_back (uncombined_positions);

				for (index_column = 0; index_column < count_columns; ++index_column) {

					if (float_data [index_column] != missing_value)
						// compute each node's coordinates in meters from bottom-left corner of bounding box
						combined_cells [index_column / 12] += float_data [index_column];
				}
				++combined_count;

					memset (combined_cells, 0, sizeof (double) * (count_columns / combine_factor));
					combined_count = 0;

				delete [] combined_cells;
				*/
			}
			else {
				std::vector <logical_coordinate>::const_iterator p;

				for (p = uncombined_positions.begin ();
				p != uncombined_positions.end ();
				++p) {
					// shape->positions.push_back (*p);
				}
			}

			log.add_formatted ("Rows\t%d\t%d.\n", start_row, end_row);
			log.add_formatted ("Output Rows\t%d.\n", count_output_rows);
			log.add_formatted ("Columns\t%d\t%d.\n", start_column, end_column);
			log.add_formatted ("Output Columns\t%d.\n", count_output_columns);
			log.add_formatted ("Position\t%.6lf\t%.6lf\n", nw_corner.y + ((double) start_row / (double) count_rows), nw_corner.x + ((double) start_column / (double) count_columns));
			delete [] float_data;
	
			fclose (f_flt);
		}
		else {
			error = true;
			log += "ERROR, can't open FLT file \"";
			log += filename_flt;
			log += "\n";
		}
	}
	else
		error = true;

	return !error;
}

bool river_view_map::add_vectors
	(class dynamic_map *map_frame,
	std::chrono::system_clock::time_point &when,
	map_layer *layer,
	class bounding_cube *stream_area,
	const class elevation_matrix *elevations,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	std::vector <map_object *>::const_iterator segment;
	int node_index, last_node_index;
	logical_coordinate segment_start, segment_end, endpoint, offset;
	long object_count = 0, count_segment_nodes = 0, count_segments = 0;
	double street_width;
	map_layer *street_layer;

	street_width = 10.0;

	view->update_progress ("Path Layer", 0);

	street_layer = map_frame->create_new (MAP_OBJECT_POINT);

	street_layer->name = "TIGER";
	street_layer->attribute_count_numeric = 1;

	street_layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	map_frame->layers.push_back (street_layer);

	view->update_progress ("Path Layer", 0);

	for (segment = layer->objects.begin ();
	segment != layer->objects.end ();
	++segment) {
		if ((*segment)->overlaps_logical (stream_area)) {
			if ((*segment)->type == MAP_OBJECT_POLYGON) {

				if (object_count % 100 == 0) {
					view->update_progress_formatted (1, "Count %ld / %ld", ++count_segments, layer->objects.size ());
					view->update_progress_formatted (2, "Path %ld", (*segment)->id);
				}

				if (simplify) {
					// One map_object for the entire road
					last_node_index = ((map_polygon *) *segment)->node_count - 1;

					segment_start.x = (double) ((map_polygon *) *segment)->nodes [0] / 1.0e6;
					segment_start.y = (double) ((map_polygon *) *segment)->nodes [1] / 1.0e6;
					if (elevations)
						segment_start.z = elevations->nearest_elevation (segment_start) + 1.0;

					segment_end.x = (double) ((map_polygon *) *segment)->nodes [last_node_index * 2] / 1.0e6;
					segment_end.y = (double) ((map_polygon *) *segment)->nodes [last_node_index * 2 + 1] / 1.0e6;
					if (elevations)
						segment_end.z = elevations->nearest_elevation (segment_end) + 1.0;
				}
				else {

					for (node_index = 0;
					node_index < ((map_polygon *) *segment)->node_count - 1;
					++node_index) {
							segment_start.x = (double) ((map_polygon *) *segment)->nodes [node_index * 2] / 1.0e6;
							segment_start.y = (double) ((map_polygon *) *segment)->nodes [node_index * 2 + 1] / 1.0e6;
							if (elevations)
								segment_start.z = elevations->nearest_elevation (segment_start) + 1.0;
						}
				
					++object_count;
				}
			}
		}
	}

	log.add_formatted ("Vector nodes written\t%ld\n", count_segment_nodes);

	return !error;
}


void river_view_map::feed_streams
	(std::map <long, stream_measurements> *stream_time_changes,
	// const std::map <long, timed_measurements> *point_sampling_data,
	const map_layer *layer_storet_legacy,
	const router_NHD *router,
	const map_layer *layer_rivers,
	const bounding_cube *boundary,
	dynamic_string &log)

// Use sampling values from STORET legacy stations to create animation values for streams

{
	std::vector <map_object *>::const_iterator station;
	timed_measurements *readings;
	

	// 2016-08-31 Sample readings are stored in a timed_measurements pointer, in map_object.attributes_void
	for (station = layer_storet_legacy->objects.begin ();
	station != layer_storet_legacy->objects.end ();
	++station) {
		if ((readings = (timed_measurements *) (*station)->attributes_void) != NULL) {
			if (readings->amounts_ppm.size () > 0) {
				add_flow_stream (stream_time_changes, router, layer_rivers,
				(*station)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID],
				(*station)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX],
				(*station)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX],
				readings, boundary, log);
			}
		}
	}

	/*
	std::set <long> flow_selections;
	std::map <long, timed_measurements>::const_iterator point_reading;
	map_object *station;

	for (point_reading = point_sampling_data->begin ();
	point_reading != point_sampling_data->end ();
	++point_reading) {
		if (point_reading->second.amounts_ppm.size () > 0)
			if ((station = layer_storet_legacy->match_id (point_reading->first)) != NULL) {
				add_flow_stream (stream_time_changes, router, layer_rivers,
				station->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID],
				station->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX],
				station->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX],
				&point_reading->second, boundary, log);
			}
	}
	*/

	/*
	// find latest time
	std::map <long, timed_measurements>::const_iterator node_reading;
	std::map <long, stream_measurements>::const_iterator stream_reading;
	long latest;
	for (stream_reading = stream_time_changes->begin ();
	stream_reading != stream_time_changes->end ();
	++stream_reading)
		for (node_reading = stream_reading->second.node_readings.begin (); node_reading != stream_reading->second.node_readings.end (); ++node_reading)
			if ((latest = node_reading->second.latest_time ()) > end_time)
				end_time = latest;
	*/
}

void river_view_map::add_flow_stream
	(std::map <long, stream_measurements> *stream_time_changes,
	const router_NHD *router,
	const map_layer *layer_rivers,
	const long comid,
	const int polygon_index,
	const int point_index,
	const timed_measurements *station_reading,
	const bounding_cube *boundary,
	dynamic_string &log)

// Add station_reading to river at COMID

{
	std::vector <long long> downstream_ids;
	std::vector <long long>::const_reverse_iterator trail_stop;
	int node_index;
	std::chrono::hours time_offset_hours;
	map_object *segment;
	bool done;

	router->rivers.accumulate_downstream_ordered (comid, &downstream_ids);

	for (trail_stop = downstream_ids.rbegin (), done = false;
	!done
	&& (trail_stop != downstream_ids.rend ());
	++trail_stop) {
		// Traverse from start_comid downstream
		// initial value
		segment = layer_rivers->match_id (*trail_stop);

		if (boundary->contains (((map_polygon *) segment)->nodes [1], ((map_polygon *) segment)->nodes [0])) {
			// River segment is in map area

			if (*trail_stop == comid) {
				// segment nearest the sampling station
				if (simplify) {
					// No nodes, one object exists for the segment
					add_streamflow_at_node (stream_time_changes, segment, 0, 0, time_offset_hours, station_reading, log);
					// Increment time offset as if there were nodes anyway
					for (node_index = point_index;
					node_index < ((map_polygon *) segment)->node_count;
					++node_index)
						time_offset_hours += std::chrono::hours ((long) hours_per_frame);
				}
				else
					// Add to each node downstream from closest point
					for (node_index = point_index;
					node_index < ((map_polygon *) segment)->node_count;
					++node_index) {
						add_streamflow_at_node (stream_time_changes, segment, 0, node_index, time_offset_hours, station_reading, log);
						time_offset_hours += std::chrono::hours ((long) hours_per_frame);
					}
			}
			else {
				// Some segment downstream from the station
				if (simplify) {
					// No nodes, one object exists for the segment
					add_streamflow_at_node (stream_time_changes, segment, 0, 0, time_offset_hours, station_reading, log);
					// Increment time offset as if there were nodes anyway
					for (node_index = 0;
					node_index < ((map_polygon *) segment)->node_count;
					++node_index)
						time_offset_hours += std::chrono::hours ((long) hours_per_frame);
				}
				else
					// Add to every node
					for (node_index = 0;
					node_index < ((map_polygon *) segment)->node_count;
					++node_index) {
						add_streamflow_at_node (stream_time_changes, segment, 0, node_index, time_offset_hours, station_reading, log);
						time_offset_hours += std::chrono::hours ((long) hours_per_frame);
					}	
			}
		}
	}

}

void write_preday
	(std::map <std::chrono::system_clock::time_point, double> *amounts_ppm,
	const double stream_width,
	const double hours_per_frame,
	const std::chrono::hours &time_offset,
	std::map <std::chrono::system_clock::time_point, double>::const_iterator &station_day)

// Write a 0-reading day just ahead of a new reading day so that blender doesn't interpolate previous days to new high

{
	std::chrono::system_clock::time_point pulse_time;
	std::map <std::chrono::system_clock::time_point, double>::iterator node_day;

	pulse_time = time_offset + station_day->first - std::chrono::hours ((long) hours_per_frame);
	if ((node_day = amounts_ppm->find (pulse_time)) == amounts_ppm->end ())
		// no reading already exists for this day
		amounts_ppm->insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time, stream_width));
}

void write_decay
	(std::map <std::chrono::system_clock::time_point, double> *amounts_ppm,
	const double stream_width,
	const std::chrono::system_clock::duration &time_offset,
	const double amplification_stream,
	std::map <std::chrono::system_clock::time_point, double>::const_iterator &station_day,
	std::map <std::chrono::system_clock::time_point, double>::const_iterator *next_day)

{
	std::chrono::system_clock::duration duration;
	int decay_hours, duration_hours;
	std::chrono::system_clock::time_point pulse_time;
	double decay = 0.1;
	std::map <std::chrono::system_clock::time_point, double>::iterator node_day;

	if (next_day) {
		duration = (*next_day)->first - station_day->first;  // from day after station_day to day before next_day
		duration -= std::chrono::hours (48);

		if (duration.count () > 96) {
			// some time until next reading - impose decay 
			if (duration.count () > 240)
				duration = std::chrono::hours (240);

			duration_hours = duration.count () / 24;

			for (decay_hours = 1;
			decay_hours <= duration_hours;
			++decay_hours) {
				pulse_time = time_offset + station_day->first + std::chrono::hours (1) * decay_hours;
				if ((node_day = amounts_ppm->find (pulse_time)) != amounts_ppm->end ())
					// a reading already exists for this day
					node_day->second += (station_day->second - (station_day->second * decay * (double) decay_hours)) * amplification_stream;
				else
					amounts_ppm->insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time,
					stream_width + (station_day->second - (station_day->second * decay * (double) decay_hours)) * amplification_stream));
			}
		}
	}
	else {
		for (decay_hours = 1;
		decay_hours <= 10;
		++decay_hours) {
			pulse_time = time_offset + station_day->first + std::chrono::hours (1) * decay_hours;
			if ((node_day = amounts_ppm->find (pulse_time)) != amounts_ppm->end ())
				// a reading already exists for this day
				node_day->second += (station_day->second - (station_day->second * decay * (double) decay_hours)) * amplification_stream;
			else
				amounts_ppm->insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time,
				stream_width + (station_day->second - (station_day->second * decay * (double) decay_hours)) * amplification_stream));
		}
	}
}

void river_view_map::add_streamflow_at_node
	(std::map <long, stream_measurements> *stream_time_changes,
	const map_object *segment,
	const int polygon_index,
	const int point_index,
	const std::chrono::hours &time_offset,
	const timed_measurements *station_reading,
	dynamic_string &log)

// Add timed pulses at this single segment node, offset by distance from source

{
	std::vector <double>::const_iterator crest;
	std::map <std::chrono::system_clock::time_point, double>::const_iterator station_day, next_day;
	std::map <long, stream_measurements>::iterator stream_data;
	std::map <std::chrono::system_clock::time_point, double>::iterator node_day;
	timed_measurements node_time_reading;
	std::chrono::system_clock::time_point pulse_time, last_pulse_time;
	double stream_width;

	// pulse_time = base_time + 6;
	// Traverse from comid downstream

	stream_width = (double) stream_size (segment); // initial value
	ASSERT (segment->type == MAP_OBJECT_POLYGON);

	pulse_time = start_date + time_offset;

	if ((stream_data = stream_time_changes->find (segment->id)) != stream_time_changes->end ()) {
		// data already exists for this stream

		std::map <long, timed_measurements>::iterator previous_node_reading;

		if ((previous_node_reading = stream_data->second.node_readings.find (point_index)) != stream_data->second.node_readings.end ()) {
			// data exists for this node
			for (station_day = station_reading->amounts_ppm.begin ();
			station_day != station_reading->amounts_ppm.end ();
			++station_day) {
				pulse_time = station_day->first + time_offset;
				if ((node_day = previous_node_reading->second.amounts_ppm.find (pulse_time)) != previous_node_reading->second.amounts_ppm.end ())
					// a reading already exists for this day
					node_day->second += station_day->second * amplification_stream;
				else {
					if (std::chrono::duration_cast <std::chrono::seconds> (pulse_time - last_pulse_time).count () > 1)
						// put a 0-reading 1 day ahead of this reading to keep blender from ramping up
						write_preday (&previous_node_reading->second.amounts_ppm, stream_width, hours_per_frame, time_offset, station_day);

					previous_node_reading->second.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time, (double) stream_width + station_day->second * amplification_stream));
				}

				next_day = station_day;
				if (++next_day != station_reading->amounts_ppm.end ())
					write_decay (&previous_node_reading->second.amounts_ppm, stream_width, time_offset, amplification_stream, station_day, &next_day);
				else
					// station_day was the final reading
					write_decay (&previous_node_reading->second.amounts_ppm, stream_width, time_offset, amplification_stream, station_day, NULL);

				last_pulse_time = pulse_time;
			}
		}
		else {
			// First data for this node
			timed_measurements node_reading;
			node_reading.id = point_index;
			node_reading.amounts_ppm.clear ();

			// node_reading.amounts_ppm.insert (std::pair <long, double> (base_time, stream_width));

			for (station_day = station_reading->amounts_ppm.begin ();
			station_day != station_reading->amounts_ppm.end ();
			++station_day) {
				// Blender interpolation will gradually ramp the stream size unless we hold it down until just before the spike
				if (station_day == station_reading->amounts_ppm.begin ()) {
					pulse_time = station_day->first + time_offset - std::chrono::hours ((long) hours_per_frame);
					node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time, stream_width));
				}

				pulse_time = station_day->first + time_offset;
				node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time, stream_width + station_day->second * amplification_stream));

				next_day = station_day;
				if (++next_day != station_reading->amounts_ppm.end ())
					write_decay (&node_reading.amounts_ppm, stream_width, time_offset, amplification_stream, station_day, &next_day);
				else
					// station_day was the final reading
					write_decay (&node_reading.amounts_ppm, stream_width, time_offset, amplification_stream, station_day, NULL);
			}
			node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time + std::chrono::hours ((long) hours_per_frame), stream_width));

			// add readings for all days at tihs node
			stream_data->second.node_readings.insert (std::pair <long, timed_measurements> (point_index, node_reading));
		}
	}
	else {
		// Nothing exists yet for any node at this stream COMID
		stream_measurements stream_reading;
		timed_measurements node_reading;

		stream_reading.comid = segment->id;

		node_reading.id = point_index;

		// start value
		// node_reading.amounts_ppm.insert (std::pair <long, double> (base_time, stream_width));

		for (station_day = station_reading->amounts_ppm.begin ();
		station_day != station_reading->amounts_ppm.end ();
		++station_day) {
			// Blender interpolation will gradually ramp the stream size unless we hold it down until just before the spike
			if (station_day == station_reading->amounts_ppm.begin ()) {
				pulse_time = station_day->first + time_offset - std::chrono::hours ((long) hours_per_frame);
				node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time, stream_width));
			}

			pulse_time = station_day->first + time_offset;
			node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time, stream_width + station_day->second * amplification_stream));

			next_day = station_day;
			if (++next_day != station_reading->amounts_ppm.end ())
				write_decay (&node_reading.amounts_ppm, stream_width, time_offset, amplification_stream, station_day, &next_day);
			else
				// station_day was the final reading
				write_decay (&node_reading.amounts_ppm, stream_width, time_offset, amplification_stream, station_day, NULL);
		}

		// end value
		node_time_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time + std::chrono::hours ((long) hours_per_frame), stream_width));

		stream_reading.node_readings.insert (std::pair <long, timed_measurements> (node_time_reading.id, node_time_reading));

		stream_time_changes->insert (std::pair <long, stream_measurements> (stream_reading.comid, stream_reading));
	}

}

void river_view_map::describe_run
	(dynamic_string &log)

{
	switch (area_definition) {
		case RIVER_VIEW_AREA_DEFINED_BOX:
		case RIVER_VIEW_ALL_AREA:
			log.add_formatted ("Map Boundary\t%.6lf\t%.6lf\t%.6lf\t%.6lf\n",
			map_boundary.x [0] / 1.0e6, map_boundary.x [1] / 1.0e6,
			map_boundary.y [0] / 1.0e6, map_boundary.y [1] / 1.0e6);
			break;
		case RIVER_VIEW_AREA_SOURCE_OUTLET:
			log.add_formatted ("Start ID\t%ld\n", start_id);
			log.add_formatted ("Destination ID\t%ld\n", destination_id);
			break;
		case RIVER_VIEW_AREA_SOURCE_UPSTREAM:
			log.add_formatted ("Outlet ID\t%ld\n", destination_id);
	}

	log += "Start Date\t";
    std::time_t tt = std::chrono::system_clock::to_time_t (start_date);
	log += std::ctime(&tt);
	log += "\nEnd Date\t";
    tt = std::chrono::system_clock::to_time_t (end_date);
	log += std::ctime (&tt);
	log += "\n";
	log.add_formatted ("Amplification, station reading\t%3lf\tStream\t%.3lf\n", amplification_station, amplification_stream);
	if (simplify)
		log += "Simplified.\n";
}


void river_view_map::insert_relevant_points
	(std::set <map_object *> *plotting_stations,
	const map_layer *layer,
	interface_window *view,
	dynamic_string &log)

// copy from layer to plotting_stations if the station is going to be in view

{
	std::vector <map_object *>::const_iterator station;

	if (area_definition == RIVER_VIEW_AREA_DEFINED_BOX) {
		for (station = layer->objects.begin ();
		station != layer->objects.end ();
		++station) {
			if ((*station)->overlaps_logical (&map_boundary))
				plotting_stations->insert (*station);
		}
	}
	else
		if (area_definition == RIVER_VIEW_AREA_SOURCE_UPSTREAM) {
			// find stations near plotting_streams using fast set lookup
			for (station = layer->objects.begin ();
			station != layer->objects.end ();
			++station) {
				if (plotting_streams.find ((*station)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID]) != plotting_streams.end ())
					plotting_stations->insert (*station);
			}
		}
		else
			if (area_definition == RIVER_VIEW_ALL_AREA) {
				for (station = layer->objects.begin ();
				station != layer->objects.end ();
				++station)
					plotting_stations->insert (*station);
			}
}

void river_view_map::insert_relevant_streams
	(const map_layer *layer_rivers,
	router_NHD *router,
	class map_object **outlet,
	interface_window *view,
	dynamic_string &log)

// copy from river layer to plotting_streams if the segment is going to be in view

{
	std::vector <map_object *>::const_iterator station;
	std::map <long long, flow_network_link *>::const_iterator branch;
	std::set <long long>::iterator trail_stop;

	if ((area_definition == RIVER_VIEW_AREA_SOURCE_UPSTREAM)
	|| (area_definition == RIVER_VIEW_AREA_SOURCE_OUTLET)) {

		// Map defined by source or source & destination comids
		// Process rivers first so map_boundary can be set for others

		if ((*outlet = layer_rivers->match_id (destination_id)) != NULL) {

			// if ((branch = router->rivers.all_mouths.find (stream->id)) != router->rivers.all_mouths.end ()) {
			if ((branch = router->rivers.all_comids.find ((*outlet)->id)) != router->rivers.all_comids.end ()) {
				std::set <long long> trail_set;
				map_object *object;

				// branch->second->accumulate_upstream_single_trail_ordered (stream->id, &trail);
				branch->second->accumulate_upstream (&trail_set);

				log.add_formatted ("Accumulated trail size\t%d\n", trail_set.size ());

				// copy trail_set to trail
				for (trail_stop = trail_set.begin ();
				trail_stop != trail_set.end ();
				++trail_stop) {
					plotting_streams.insert (*trail_stop);
					if ((object = layer_rivers->match_id (*trail_stop)) != NULL)
						object->check_extent (&map_boundary);
				}
			}
			else
				log.add_formatted ("Error, no all_comids entry for comid %ld.\n", (*outlet)->id);
		}
		else
			log.add_formatted ("Error, no map_object entry for comid %ld.\n", destination_id);
	}
	else
		if (area_definition == RIVER_VIEW_AREA_DEFINED_BOX) {
			std::vector <map_object *>::const_iterator object;

			for (object = layer_rivers->objects.begin ();
			object != layer_rivers->objects.end ();
			++object) {
				// if ((*object)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL] <= 3.0)
					if ((*object)->overlaps_logical (&map_boundary))
						plotting_streams.insert ((*object)->id);
			}
		}
		else
			if (area_definition == RIVER_VIEW_ALL_AREA) {
				std::vector <map_object *>::const_iterator object;

				for (object = layer_rivers->objects.begin ();
				object != layer_rivers->objects.end ();
				++object) {
					if ((*object)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL] <= 3.0)
						plotting_streams.insert ((*object)->id);
				}
			}
}

/*
void river_view_map::create_map
	(class dynamic_map *map_frame,
	std::chrono::system_clock::time_point &when,
	dynamic_map *map_watershed,
	std::map <long, stream_measurements> *stream_time_changes,
	elevation_matrix *elevations,
	interface_window *view,
	dynamic_string &log)

{
	map_layer *layer;
	int layer_index;
	std::vector <long> counties;

	layer = (map_layer *) map_watershed->first_member ();

	layer_index = 0;
	while (layer) {
		if (render_layers [layer_index]) {
			if (layer->name == "Rivers") {
				// bounding_cube stream_area;
				if (elevations->empty ())
					// Set elevations from stream
					build_stream (map_frame, when, layer, &plotting_streams, stream_time_changes, elevations, view, log);
				else
					// elevations were set by gridfloat heatmap
					build_stream (map_frame, when, layer, &plotting_streams, stream_time_changes, NULL, view, log);
			}
			else {
				if (layer->type == MAP_OBJECT_POINT)
					build_points (map_frame, when, &plotting_objects, "F", (double) layer->symbol_size, elevations, true, true, view, log);
				else
					if (layer->type == MAP_OBJECT_POLYGON) {
						if (layer->enclosed) {
							// limit to those overlapping stream area
							std::vector <map_object *>::const_iterator county;
							counties.clear ();
							for (county = layer->objects.begin ();
							county != layer->objects.end ();
							++county) {
								if ((*county)->overlaps_logical (&map_boundary))
									counties.push_back ((*county)->id);
							}

							if (layer->name == "County Boundaries")
								add_polygons (map_frame, when, layer, &counties, false, NULL, view, log);
							else
								add_polygons (map_frame, when, layer, &counties, true, elevations, view, log);
						}
						else
							// TIGER streets
							add_vectors (map_frame, when, layer, &map_boundary, elevations, view, log);
					}
				}
		}
		layer = (map_layer *) layer->next;
		++layer_index;
	}
}
*/


bool river_view_map::set_point_data
	(std::chrono::system_clock::time_point &when,
	std::chrono::hours &interval,
	map_layer *layer,
	std::vector <map_object *> *zero_holder,
	const double base_size,
	const bool use_names,
	const bool text_labels,
	interface_window *view,
	dynamic_string &log)

// Set attributes_numeric [0] to the value in attributes_void that is relevent to when

// 

{
	// std::vector <map_object *>::const_iterator segment;
	bool error = false;
	std::vector <map_object *>::iterator station;
	logical_coordinate endpoint, offset, sky_point, text_position, text_rotation;
	long object_count = 0;
	timed_measurements *time_set;
	std::chrono::system_clock::duration elapsed;
	std::map <std::chrono::system_clock::time_point, double>::const_iterator time;

	view->update_progress ("Point Layer", 1);
	station = layer->objects.begin ();
	while (station != layer->objects.end ()) {
		if (++object_count % 100 == 0) {
			view->update_progress_formatted (1, "Count %ld / %ld", object_count, layer->objects.size ());
			view->update_progress_formatted (2, "Point %ld", (*station)->id);
		}

		(*station)->attributes_numeric [0] = 0.0;

		if ((time_set = (timed_measurements *) (*station)->attributes_void) != NULL) {

			for (time = time_set->amounts_ppm.begin ();
			time != time_set->amounts_ppm.end ();
			++time) {
				if (time->first <= when) {
					elapsed = when - time->first;
					if (elapsed <= interval)
						// More than one time may fall within interval
						// Keep the last one before when
						(*station)->attributes_numeric [0] = time->second;
				}
			}
		}

		if ((*station)->attributes_numeric [0] == 0.0) {
			// No price reported in interval, move to zero_holder
			zero_holder->push_back (*station);
			station = layer->objects.erase (station);
		}
		else
			++station;
	}

	return !error;
}

