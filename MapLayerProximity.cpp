#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include <stack>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/arbitrary_counter.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/shapefile_filter.h"
#include "../subbasin_effect/thread_manager.h"
#include "MapLayerProximity.h"

void default_base_point_callback
	(const map_object* point,
	const char delimiter,
	dynamic_string& log)

{
	log.add_formatted("%ld", point->id);

	log += delimiter;
	log += "\"";
	log += point->name; // RowID
	log += "\"";
}


void proximity_id_name_callback // FarmlandFinder_point_callback
	(const map_object* point,
	const char delimiter,
	dynamic_string& log)

	// 2021-07-08 Called by work_MapLayerProximity to write identifiers & data for Grain Location point

{
	// int state_fips, county_fips;

	// ID\tSales Location
	log.add_formatted("%ld", point->id);
	log += delimiter;
	log += "\"";
	log += point->name;
	log += "\"";
}

void proximity_id_name_distance_column_headers_callback
	(const char delimiter,
	dynamic_string& column_names)

// ProximityLayer.write_column_headers

{
	// column_names.clear();
	// column_names += delimiter;
	// column_names += "\"Grain Location Number\"";
	column_names += delimiter;
	column_names += "\"ID\"";
	column_names += delimiter;
	column_names += "\"Name\"";
	column_names += delimiter;
	column_names += "\"Distance, m\"";
}

void proximity_id_name_distance_callback
	(const map_object* point,
	const double distance_m,
	const char delimiter,
	dynamic_string& log)

// 2023-10-09 id, name, distance

{
	log += delimiter;
	log.add_formatted("%ld", point->id);
	log += delimiter;
	log += "\"";
	log += point->name;
	log += "\"";
	log += delimiter;
	// log.add_formatted("%lg", distance_m);
	log.add_formatted("%.2lf", distance_m);
}

void address_point_callback
	(const map_object *point,
	const char delimiter,
	dynamic_string &log)

// 2022-01-03 Address points to lake centers

{
	log.add_formatted ("%ld", point->id);

	log += delimiter;
	log += "\"";
	log += point->name; // RowID
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [7]; // Address1
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [8]; // Address2
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [9]; // City
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [10]; // State
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [11]; // Zip
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [12]; // Zip4
	log += "\"";
}

void lake_centroid_callback
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log)

// 2022-01-03 Address points to lake centers

{
	log += delimiter;
	log.add_formatted ("%ld", point->id);

	log += delimiter;
	log += "\"";
	log += point->name; // RowID
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [0]; // County
	log += "\"";

	log += delimiter;
	log.add_formatted ("%lg", distance_m);
}

void lake_centroid_callback_headers
	(const char delimiter,
	dynamic_string &log)

// 2022-01-03 Address points to lake centers

{
	if (delimiter == '\t')
		log += "\tLakeID\tName\tCounty\tDistance,m";
	else
		log += ",\"LakeID\",\"Name\",\"County\", \"Distance m\"";
}

bool work_computed_proximity
	(thread_work *job)

// Fill ProximityResult with distances to nearest in each layer

{
	bool error = false;
	map_object *feature;
	MapLayerProximity *proximator = ((job_MapLayerProximity*) job)->proximator;
	std::vector <ProximityLayer>::iterator other_layer;
	ProximityLayerResult *results = &((job_MapLayerProximity*) job)->result;
	std::pair <map_layer *, ProximityResult> layer_result;
	double distance_meters;
	device_coordinate base_point;
	int point_index;

	((job_MapLayerProximity *) job)->base_layer_point->centroid (&base_point);

	for (other_layer = proximator->proximity_layers.begin ();
	other_layer != proximator->proximity_layers.end ();
	++other_layer) {
		layer_result.first = other_layer->layer;

		if ((feature = other_layer->layer->find_nearest_object (base_point, &distance_meters)) != NULL) {

			layer_result.second.object = feature;

			if (feature->type == MAP_OBJECT_POLYGON_COMPLEX) {
				// 2022-03-08 (frame_CARDGIS.On_WindDistances) Ignore inner polygons when finding distance to surrounding polygon
				// Those inner polygons are found for another column of output table
				// if (((map_polygon_complex *) feature)->polygon_count > 0)
				layer_result.second.distance_m = ((map_polygon_complex *) feature)->polygons[0].closest_point_meters (base_point, &point_index);
			}
			else
				layer_result.second.distance_m = distance_meters;
		}
		else
			layer_result.second.clear ();
		results->layers.push_back (layer_result);
	}

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		// job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool work_computed_enclosure
	(thread_work *job)

// Fill ProximityResult with distances to any enclosing objects in proximity_layers

{
	bool error = false;
	map_object *feature;
	MapLayerProximity *proximator = ((job_MapLayerProximity *) job)->proximator;
	std::vector <ProximityLayer>::iterator other_layer;
	ProximityLayerResult *results = &((job_MapLayerProximity *) job)->result;
	std::pair <map_layer *, ProximityResult> layer_result;
	device_coordinate base_point;
	bounding_cube coordinate_hint;
	int point_index;

	((job_MapLayerProximity *) job)->base_layer_point->centroid (&base_point);

	coordinate_hint.x[0] = base_point.x - 0.0005;
	coordinate_hint.y[0] = base_point.y - 0.0005;
	coordinate_hint.x[1] = coordinate_hint.x[0] + 0.001;
	coordinate_hint.y[1] = coordinate_hint.y[0] + 0.001;

	for (other_layer = proximator->proximity_layers.begin ();
	other_layer != proximator->proximity_layers.end ();
	++other_layer) {
		layer_result.first = other_layer->layer;

		if ((feature = other_layer->layer->find_layer_object_surrounding (base_point, &coordinate_hint)) != NULL) {
			layer_result.second.object = feature;
			if (feature->type == MAP_OBJECT_POLYGON_COMPLEX) {
				// 2022-03-08 (frame_CARDGIS.On_WindDistances) Ignore inner polygons when finding distance to surrounding polygon
				// Those inner polygons are found for another column of output table
				// if (((map_polygon_complex *) feature)->polygon_count > 0)
				layer_result.second.distance_m = ((map_polygon_complex *) feature)->polygons [0].closest_point_meters (base_point, &point_index);
			}
			else
				layer_result.second.distance_m = feature->closest_point_meters (base_point, &point_index);
		}
		else
			layer_result.second.clear ();
		results->layers.push_back (layer_result);
	}

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		// job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool work_MapLayerProximity_file
	(thread_work *job)

// Version that writes to file

{
	bool error = false;
	map_object *feature;
	MapLayerProximity *proximator = ((job_MapLayerProximity *) job)->proximator;
	std::vector <ProximityLayer>::iterator other_layer;
	FILE *output_file = (* ((job_MapLayerProximity*)job)->output_files) [job->thread_directory_index];
	double distance_meters;
	device_coordinate base_point;
	dynamic_string output_line;
	int distance_index = ((job_MapLayerProximity*)job)->add_to_base_shapefile_index;

	proximator->write_point_function (((job_MapLayerProximity *) job)->base_layer_point, proximator->delimiter, output_line);

	((job_MapLayerProximity *) job)->base_layer_point->centroid (&base_point.y, &base_point.x);

	for (other_layer = proximator->proximity_layers.begin ();
	other_layer != proximator->proximity_layers.end ();
	++other_layer) {
		if ((feature = other_layer->layer->find_nearest_object (base_point, &distance_meters, ((job_MapLayerProximity*)job)->within_check > 0)) != NULL) {

			if ((((job_MapLayerProximity*)job)->within_check == PROXIMITY_WITHIN_POLYGON_ZERO)
			&& (distance_meters < 0.0))
				distance_meters = 0.0;

			if (other_layer->write_layer_point)
				// callback function for NHD lakes or rivers
				other_layer->write_layer_point (feature, distance_meters, proximator->delimiter, output_line);
			else
				output_line.add_formatted ("%c%ld%c%.6lf", proximator->delimiter, feature->id, proximator->delimiter, distance_meters);
			if (distance_index >= 0)
				((job_MapLayerProximity*)job)->base_layer_point->attributes_numeric [distance_index] = distance_meters;
		}
		else {
			output_line += proximator->delimiter;
			output_line += proximator->delimiter;
		}
	}
	// output_line.add_formatted ("%c%d", proximator->delimiter, job->thread_directory_index);
	output_line += "\n";
	fwrite (output_line.get_text_ascii (), output_line.get_length (), 1, output_file);

	// job->log.add_formatted("%d\t", ((job_MapLayerProximity*)job)->base_layer_point->id);
	// job->log += ((job_MapLayerProximity*)job)->base_layer_point->name;
	// job->log.add_formatted("\t%d\n", job->thread_directory_index);

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		// job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool work_MapLayerProximity_log
	(thread_work *job)

// Version that doesn't write to file

{
	bool error = false;
	map_object *feature;
	MapLayerProximity *proximator = ((job_MapLayerProximity *) job)->proximator;
	std::vector <ProximityLayer>::iterator other_layer;
	double distance_meters;
	device_coordinate base_point;
	dynamic_string output_line;
	int distance_index = ((job_MapLayerProximity*)job)->add_to_base_shapefile_index;

	proximator->write_point_function (((job_MapLayerProximity *) job)->base_layer_point, proximator->delimiter, output_line);

	((job_MapLayerProximity *) job)->base_layer_point->centroid (&base_point.y, &base_point.x);

	for (other_layer = proximator->proximity_layers.begin ();
	other_layer != proximator->proximity_layers.end ();
	++other_layer) {
		if ((feature = other_layer->layer->find_nearest_object (base_point, &distance_meters, ((job_MapLayerProximity*)job)->within_check > 0)) != NULL) {
			if (other_layer->write_layer_point)
				// callback function for NHD lakes or rivers
				other_layer->write_layer_point (feature, distance_meters, proximator->delimiter, output_line);
			else
				output_line.add_formatted ("%c%ld%c%.6lf", proximator->delimiter, feature->id, proximator->delimiter, distance_meters);

			if (((job_MapLayerProximity*)job)->add_to_base_shapefile_index >= 0)
				((job_MapLayerProximity*)job)->base_layer_point->attributes_numeric[distance_index] = distance_meters;
		}
		else {
			output_line += proximator->delimiter;
			output_line += proximator->delimiter;
		}
		++distance_index;
	}
	output_line += "\n";
	job->log += output_line;

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool distance_sorter
	(const std::pair <map_object *, double> &first,
	const std::pair <map_object *, double> &second)

{
	if (first.second < second.second)
		return true;
	else
		return false;
}

void find_objects_by_distance
	(const device_coordinate &logical_point,
	const map_layer *search_layer,
	std::vector <std::pair <map_object *, double>> *objects_and_distances)

// Fill a vector of objects and distances

{
	int nearest_index, nearest_polygon_index;
	double distance_m;
	std::vector<map_object *>::const_iterator highway;
	std::pair <map_object *, double> object_distance;

	if (search_layer->type == MAP_OBJECT_POINT) {
		// improve performance a bit by not repeatedly checking for complex polygons
		// Call distance_meters directly
		for (highway = search_layer->objects.begin ();
		highway != search_layer->objects.end ();
		++highway) {

			distance_m = distance_meters (logical_point.y, logical_point.x, (*highway)->latitude, (*highway)->longitude);
			objects_and_distances->push_back (std::pair <map_object *, double> (*highway, distance_m));
		}
	}
	else {
		for (highway = search_layer->objects.begin ();
		highway != search_layer->objects.end ();
		++highway) {

			if ((*highway)->type == MAP_OBJECT_POLYGON_COMPLEX)
				// 2020-11-12 some NHD lakes are complex
				distance_m = ((map_polygon_complex *) (*highway))->closest_point_meters (logical_point, &nearest_polygon_index, &nearest_index);
			else
				distance_m = (*highway)->closest_point_meters (logical_point, &nearest_index);

			objects_and_distances->push_back (std::pair <map_object *, double> (*highway, distance_m));
		}
	}
}

bool work_MapLayerProximity_list_file
	(thread_work *job)

// Version that writes to file

{
	bool error = false;
	MapLayerProximity *proximator = ((job_MapLayerProximity *) job)->proximator;
	std::vector <ProximityLayer>::iterator other_layer;
	FILE* output_file = (*((job_MapLayerProximity*)job)->output_files)[job->thread_directory_index];
	device_coordinate base_point;
	dynamic_string output_line;
	std::vector <std::pair <map_object *, double>> objects_and_distances;
	std::vector <std::pair <map_object *, double>>::iterator object_iterator;
	int object_index;

	((job_MapLayerProximity *) job)->base_layer_point->centroid (&base_point.y, &base_point.x);

	for (other_layer = proximator->proximity_layers.begin ();
	other_layer != proximator->proximity_layers.end ();
	++other_layer) {
		objects_and_distances.clear ();
		find_objects_by_distance (base_point, other_layer->layer, &objects_and_distances);

		std::sort (objects_and_distances.begin (), objects_and_distances.end (), distance_sorter);

		for (object_iterator = objects_and_distances.begin (), object_index = 0;
		(object_iterator != objects_and_distances.end ())
		&& (object_index < ((job_MapLayerProximity *) job)->list_size);
		++object_iterator, ++object_index) {
			proximator->write_point_function (((job_MapLayerProximity *) job)->base_layer_point, proximator->delimiter, output_line);

			if (other_layer->write_layer_point)
				// callback function for NHD lakes or rivers
				other_layer->write_layer_point (object_iterator->first, object_iterator->second, proximator->delimiter, output_line);
			else
				output_line.add_formatted ("%c%ld%c%.6lf", proximator->delimiter, object_iterator->first->id, proximator->delimiter, object_iterator->second);

			// output_line.add_formatted ("%c%d", proximator->delimiter, object_index);

			output_line += "\n";
		}
	}
	fwrite (output_line.get_text_ascii (), output_line.get_length (), 1, output_file);


	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		// job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool work_MapLayerProximity_list_log
	(thread_work *job)

// Version that doesn't write to file

{
	bool error = false;
	MapLayerProximity *proximator = ((job_MapLayerProximity *) job)->proximator;
	std::vector <ProximityLayer>::iterator other_layer;
	device_coordinate base_point;
	dynamic_string output_line;
	std::vector <std::pair <map_object *, double>> objects_and_distances;
	std::vector <std::pair <map_object *, double>>::iterator object_iterator;
	int object_index;

	((job_MapLayerProximity *) job)->base_layer_point->centroid (&base_point.y, &base_point.x);

	for (other_layer = proximator->proximity_layers.begin ();
	other_layer != proximator->proximity_layers.end ();
	++other_layer) {
		objects_and_distances.clear ();
		find_objects_by_distance (base_point, other_layer->layer, &objects_and_distances);

		std::sort (objects_and_distances.begin (), objects_and_distances.end (), distance_sorter);

		for (object_iterator = objects_and_distances.begin (), object_index = 0;
		(object_iterator != objects_and_distances.end ())
		&& (object_index < ((job_MapLayerProximity *) job)->list_size);
		++object_iterator, ++object_index) {

			proximator->write_point_function (((job_MapLayerProximity *) job)->base_layer_point, proximator->delimiter, output_line);

			if (other_layer->write_layer_point)
				// callback function for NHD lakes or rivers
				other_layer->write_layer_point (object_iterator->first, object_iterator->second, proximator->delimiter, output_line);
			else
				output_line.add_formatted ("%c%ld%c%.6lf", proximator->delimiter, object_iterator->first->id, proximator->delimiter, object_iterator->second);

			output_line += "\n";
		}
	}
	job->log += output_line;

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool work_MapLayerProximity_file_radius
	(thread_work *job)

// Version that writes to file

{
	bool error = false;
	MapLayerProximity *proximator = ((job_MapLayerProximity *) job)->proximator;
	std::vector <ProximityLayer>::iterator other_layer;
	FILE* output_file = (*((job_MapLayerProximity*)job)->output_files)[job->thread_directory_index];
	device_coordinate base_point;
	logical_coordinate sale_location, spill_location;
	dynamic_string output_line;
	std::vector <map_object *>::const_iterator spill;
	double distance_m, offset_10_plus;
	bounding_cube ten_mile_box;
	int closest_point_index;

	((job_MapLayerProximity *) job)->base_layer_point->centroid (&base_point.y, &base_point.x);

	sale_location.x = (double) base_point.x / 1.0e6;
	sale_location.y = (double) base_point.y / 1.0e6;

	// ten_mile_box needs to be 20 x 20 miles
	offset_10_plus = offset_longitude_meters (sale_location, proximator->cutoff_radius_m_plus);
	ten_mile_box.x [0] = (sale_location.x - offset_10_plus) * 1.0e6;
	ten_mile_box.x [1] = (sale_location.x + offset_10_plus) * 1.0e6;
	offset_10_plus = offset_latitude_meters (sale_location, proximator->cutoff_radius_m_plus);
	ten_mile_box.y [0] = (sale_location.y - offset_10_plus) * 1.0e6;
	ten_mile_box.y [1] = (sale_location.y + offset_10_plus) * 1.0e6;

	for (other_layer = proximator->proximity_layers.begin ();
	other_layer != proximator->proximity_layers.end ();
	++other_layer) {
		for (spill = other_layer->layer->objects.begin ();
		spill != other_layer->layer->objects.end ();
		++spill) {
			// Write to output only if within 10mi cutoff
			if ((*spill)->overlaps_logical (&ten_mile_box)) {
				spill_location.x = (double) (*spill)->longitude / 1.0e6;
				spill_location.y = (double) (*spill)->latitude / 1.0e6;
				// 2022-04-07 Polygons must use closest_point_meter instead of centroid to find distance.
				if ((distance_m = (*spill)->closest_point_meters(base_point, &closest_point_index)) <= proximator->cutoff_radius_m) {
						proximator->write_point_function (((job_MapLayerProximity *) job)->base_layer_point, proximator->delimiter, output_line);
					if (other_layer->write_layer_point)
						// callback function for NHD lakes or rivers
						other_layer->write_layer_point (*spill, distance_m, proximator->delimiter, output_line);
					else
						output_line.add_formatted ("%c%ld%c%.6lf", proximator->delimiter, (*spill)->id, proximator->delimiter, distance_m);
					output_line += "\n";
				}
			}
		}
	}

	if (output_line.get_length () > 0)
		fwrite (output_line.get_text_ascii (), output_line.get_length (), 1, output_file);

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		// job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool work_MapLayerProximity_log_radius
	(thread_work *job)

{
	bool error = false;
	MapLayerProximity *proximator = ((job_MapLayerProximity *) job)->proximator;
	std::vector <ProximityLayer>::iterator other_layer;
	device_coordinate base_point;
	logical_coordinate sale_location, spill_location;
	dynamic_string output_line;
	std::vector <map_object *>::const_iterator spill;
	double distance_m, offset_10_plus;
	bounding_cube ten_mile_box;
	int closest_point_index;

	((job_MapLayerProximity*)job)->base_layer_point->centroid(&base_point.y, &base_point.x);

	((job_MapLayerProximity*)job)->match_count = 0;
	sale_location.x = (double) base_point.x / 1.0e6;
	sale_location.y = (double) base_point.y / 1.0e6;

	// ten_mile_box needs to be 20 x 20 miles
	offset_10_plus = offset_longitude_meters (sale_location, proximator->cutoff_radius_m_plus);
	ten_mile_box.x [0] = (sale_location.x - offset_10_plus) * 1.0e6;
	ten_mile_box.x [1] = (sale_location.x + offset_10_plus) * 1.0e6;
	offset_10_plus = offset_latitude_meters (sale_location, proximator->cutoff_radius_m_plus);
	ten_mile_box.y [0] = (sale_location.y - offset_10_plus) * 1.0e6;
	ten_mile_box.y [1] = (sale_location.y + offset_10_plus) * 1.0e6;

	for (other_layer = proximator->proximity_layers.begin ();
	other_layer != proximator->proximity_layers.end ();
	++other_layer) {
		for (spill = other_layer->layer->objects.begin ();
		spill != other_layer->layer->objects.end ();
		++spill) {
			// Write to output only if within 10mi cutoff
			if ((*spill)->overlaps_logical (&ten_mile_box)) {
				// 2022-04-07 Polygons must use closest_point_meter instead of centroid to find distance.
				if ((distance_m = (*spill)->closest_point_meters (base_point, &closest_point_index)) <= proximator->cutoff_radius_m) {
					proximator->write_point_function(((job_MapLayerProximity*)job)->base_layer_point, proximator->delimiter, output_line);
					if (other_layer->write_layer_point)
						// callback function for NHD lakes or rivers
						other_layer->write_layer_point(*spill, distance_m, proximator->delimiter, output_line);
					else
						output_line.add_formatted("%c%ld%c%.6lf", proximator->delimiter, (*spill)->id, proximator->delimiter, distance_m);
					output_line += "\n";
					((job_MapLayerProximity*)job)->match_count += 1;
				}
			}
		}
	}

	job->log += output_line;

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		// job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

ProximityLayer::ProximityLayer ()

{
	layer = NULL;
	write_layer_point = NULL;
	write_column_headers = NULL;
}

ProximityLayer::ProximityLayer
	(const ProximityLayer &other)
{
	copy (other);
}

ProximityLayer ProximityLayer::operator =
	(const ProximityLayer &other)

{
	copy (other);
	return *this;
}

void ProximityLayer::copy
	(const ProximityLayer &other)

{
	layer = other.layer;
	write_layer_point = other.write_layer_point;
	write_column_headers = other.write_column_headers;
}

ProximityResult::ProximityResult ()

{
	object = NULL;
	distance_m = 0.0;
}

ProximityResult::ProximityResult
	(const ProximityResult &other)
{
	copy (other);
}

ProximityResult ProximityResult::operator =
	(const ProximityResult &other)

{
	copy (other);
	return *this;
}

void ProximityResult::copy
	(const ProximityResult &other)

{
	object = other.object;
	distance_m = other.distance_m;
}


ProximityLayerResult::ProximityLayerResult ()

{
}

ProximityLayerResult::ProximityLayerResult
	(const ProximityLayerResult &other)
{
	copy (other);
}

ProximityLayerResult ProximityLayerResult::operator =
	(const ProximityLayerResult &other)

{
	copy (other);
	return *this;
}

void ProximityLayerResult::copy
	(const ProximityLayerResult &other)

{
	layers = other.layers;
}

MapLayerProximity::MapLayerProximity ()

{
	thread_count = 12;
	delimiter = '\t';
	cutoff_radius_miles = 8.0;
	write_point_function = &default_base_point_callback;
}

bool MapLayerProximity::open_thread_files
	(std::vector <FILE*>* thread_files,
	std::vector <dynamic_string>* thread_filenames,
	dynamic_string &log)

// 2022-07-15 Some records in large Zillow-LAGOS job overwrote each other.
// Open a unique output file for one thread

{
	FILE* f = NULL;
	filename_struct filename;
	bool error = false;
	std::vector <ProximityLayer>::iterator layer;
	int thread_index;
	dynamic_string thread_output_filename;

	for (thread_index = 0;
	!error
	&& (thread_index < thread_count);
	++thread_index) {
		// Add thread number to filename prefix
		filename.parse(&output_filename);
		thread_output_filename = filename.prefix;
		thread_output_filename.add_formatted("-%d", thread_index + 1);
		filename.set_prefix(thread_output_filename.get_text());
		filename.write_file_specification(&thread_output_filename);

		if (fopen_s(&f, thread_output_filename.get_text_ascii(), "w") == 0) {
			thread_files->push_back (f);
			thread_filenames->push_back (thread_output_filename);
		}
		else {
			error = true;
			log += "ERROR, can't open output file \"";
			log += thread_output_filename;
			log += "\".\n";
		}
	}

	return !error;
}

bool MapLayerProximity::append_thread_files
	(std::vector <FILE *> *thread_files,
	std::vector <dynamic_string>* thread_filenames,
	dynamic_string &log)
{
	FILE *f, *thread_read;
	bool error = false;
	dynamic_string column_names;
	std::vector <FILE*>::iterator thread_file;
	std::vector <dynamic_string>::iterator thread_filename;
	std::vector <ProximityLayer>::iterator layer;

	if (fopen_s(&f, output_filename.get_text_ascii(), "w") == 0) {

		fwrite(base_layer_column_headers.get_text_ascii(), base_layer_column_headers.get_length(), 1, f);
		for (layer = proximity_layers.begin();
		layer != proximity_layers.end();
		++layer) {
			if (layer->write_column_headers) {
				// callback function for NHD lakes or rivers
				layer->write_column_headers(delimiter, column_names);
				fwrite(column_names.get_text_ascii(), column_names.get_length(), 1, f);
			}
			else {
				fprintf(f, "%c\"Nearest ", delimiter);
				fprintf(f, "%s\"", layer->layer->name.get_text_ascii());
				fprintf(f, "%c\"distance, m\"", delimiter);
			}
		}
		fprintf(f, "\n");

		BYTE buffer [2048];
		int buffer_size;
		filename_struct fs;
		dynamic_string target_folder, target_filename;

		for (thread_file = thread_files->begin (), thread_filename = thread_filenames->begin ();
		thread_file != thread_files->end ();
		++thread_file, ++thread_filename) {
			fclose(*thread_file);

			if (fopen_s(&thread_read, thread_filename->get_text_ascii(), "r") == 0) {
				while ((buffer_size = fread (buffer, 1, 2048, thread_read)) > 0)
					fwrite (buffer, 1, buffer_size, f);
				fclose (thread_read);
			}

			fs.parse (&*thread_filename);
			fs.write_path (&target_folder);
			fs.write_filename (&target_filename);
			remove_file (target_folder, target_filename, &error, log);
		}

		fclose (f);
	}
	else {
		error = true;
		log += "ERROR, can't open output file \"";
		log += output_filename;
		log += "\".\n";
	}
	return !error;
}

void MapLayerProximity::write_proximity_table
	(const int within_check,
	const int add_to_base_shapefile_index,
	interface_window *view,
	dynamic_string &log)

{
	std::vector <map_object *>::iterator sale;
	thread_manager threader;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator job_iterator;
	job_MapLayerProximity *job;
	std::vector <ProximityLayer>::iterator layer;
	thread_manager_statistics stats;
	int job_index;
	dynamic_string progress, column_names;
	std::vector <FILE*> thread_files;
	std::vector <dynamic_string> thread_filenames;
	bool error = false;

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1; // 5;
	stats.reset (threader.thread_count);
	threader.statistics = &stats;

	if (output_filename.get_length () > 0) {
		if (!open_thread_files (&thread_files, &thread_filenames, log))
			error = true;
	}

	job_index = 0;
	for (sale = base_layer->objects.begin ();
	!error
	&& (sale != base_layer->objects.end ());
	++sale) {
		job = new job_MapLayerProximity;
		job->id = ++job_index;
		if (output_filename.get_length() > 0) {
			job->work_function = &work_MapLayerProximity_file;
			job->output_files = &thread_files;
			job->thread_count = thread_count;
		}
		else
			job->work_function = &work_MapLayerProximity_log;
		job->base_layer_point = *sale;
		job->proximator = this;
		job->view = view;
		job->within_check = within_check;
		job->add_to_base_shapefile_index = add_to_base_shapefile_index;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	if (!error) {
		view->set_data (&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
		// threader.run (&jobs, view, log);
		threader.run_tiny (&jobs, view, log);
		view->set_data (NULL);
		view->update_status_bar ("Threader run complete");
	}

	if (output_filename.get_length() > 0) {
		append_thread_files (&thread_files, &thread_filenames, log);
		for (job_iterator = jobs.begin();
		job_iterator != jobs.end ();
		++job_iterator) {
			log += job_iterator->second->log;
			job_iterator->second->log.clear();
		}
	}
	else {
		log += base_layer_column_headers;
		for (layer = proximity_layers.begin ();
		layer != proximity_layers.end ();
		++layer) {
			if (layer->write_column_headers) {
				// callback function for NHD lakes or rivers
				layer->write_column_headers (delimiter, column_names);
				log += column_names;
			}
			else {
				log.add_formatted ("%cNearest ", delimiter);
				log += layer->layer->name;
				log.add_formatted ("%cdistance, m", delimiter);
			}
		}
		log += "\n";

		for (job_iterator = jobs.begin ();
		job_iterator != jobs.end ();
		++job_iterator) {
			progress.format ("Writing output job %ld.\n", job_iterator->first);
			view->update_scroll (progress);
			log += job_iterator->second->log;
			job_iterator->second->log.clear ();
		}
	}

	// stats.write_time_summary (log);
}

bool MapLayerProximity::write_proximity_table_within_radius
	(interface_window *view,
	dynamic_string &log)

// 2021-05-05 Only writes text to output if distance < cutoff_radius_miles
// 2022-04-08 This will be called multiple times by frame_CARDGIS.OnZillowPowerlineTable

{
	std::vector <map_object *>::iterator sale;
	thread_manager threader;
	std::vector <ProximityLayer>::iterator layer;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator job_iterator;
	job_MapLayerProximity *job;
	thread_manager_statistics stats;
	int thread_index, output_row_count;
	dynamic_string progress, column_names;
	std::vector <FILE *> thread_files;
	std::vector <dynamic_string> thread_filenames;
	bool error = false;

	if (output_filename.get_length() > 0) {
		if (!open_thread_files(&thread_files, &thread_filenames, log))
			error = true;
	}

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1; // 5;
	threader.pause_after_threads = false;
	stats.reset (threader.thread_count);
	threader.statistics = &stats;

	cutoff_radius_m_plus = (cutoff_radius_miles + 0.01) * METERS_PER_MILE; // a little extra so precision of distance meters is more important than offset_longitude_meters/offset_latitude_meters
	cutoff_radius_m = cutoff_radius_miles * METERS_PER_MILE;

	thread_index = 0;
	for (sale = base_layer->objects.begin ();
	sale != base_layer->objects.end ();
	++sale) {
		job = new job_MapLayerProximity;
		job->id = ++thread_index;

		if (output_filename.get_length () > 0) {
			job->work_function = &work_MapLayerProximity_file_radius;
			job->output_files = &thread_files;
		}
		else
			job->work_function = &work_MapLayerProximity_log_radius;
		job->base_layer_point = *sale;
		job->proximator = this;
		job->view = view;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	if (!error) {
		view->set_data (&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
		// threader.run (&jobs, view, log);
		threader.run_tiny (&jobs, view, log);
		view->set_data (NULL);
		view->update_status_bar ("Threader run complete");
	}

	if (output_filename.get_length() > 0) {
		append_thread_files(&thread_files, &thread_filenames, log);
		for (job_iterator = jobs.begin();
			job_iterator != jobs.end();
			++job_iterator) {
			log += job_iterator->second->log;
			job_iterator->second->log.clear();
		}
	}
	else {
		// Write each thread's output to file
		FILE *f;
		if (fopen_s (&f, output_filename.get_text_ascii (), "w") == 0) {
			fwrite (base_layer_column_headers.get_text_ascii (), base_layer_column_headers.get_length (), 1, f);
			for (layer = proximity_layers.begin ();
			layer != proximity_layers.end ();
			++layer) {
				if (layer->write_column_headers) {
					// callback function for NHD lakes or rivers
					layer->write_column_headers (delimiter, column_names);
					fwrite (column_names.get_text_ascii (), column_names.get_length (), 1, f);
				}
				else {
					fprintf (f, "%c\"Nearest ", delimiter);
					fprintf (f, "%s\"", layer->layer->name.get_text_ascii ());
					fprintf (f, "%c\"distance, m\"", delimiter);
				}
			}
			fprintf (f, "\n");

			output_row_count = 0;
			for (job_iterator = jobs.begin ();
			// (output_row_count < 1000)
			job_iterator != jobs.end ();
			++job_iterator) {
				progress.format ("Writing output job %ld.\n", job_iterator->first);
				view->update_scroll (progress);
				fwrite (job_iterator->second->log.get_text_ascii (), job_iterator->second->log.get_length (), 1, f);
				job_iterator->second->log.clear ();
				output_row_count += ((job_MapLayerProximity *) job_iterator->second)->match_count;
			}
			fclose (f);
		}
		else {
			error = true;
			log += "ERROR, can't open output file \"";
			log += output_filename;
			log += "\".\n";
		}
	}
	// stats.write_time_summary (log);
	return !error;
}

ProximityLayer *MapLayerProximity::read_proximity_shapefile
	(const dynamic_string &path_proximity,
	const dynamic_string &filename_proximity,
	const dynamic_string &layer_name,
	const dynamic_string &id_field_name,
	const dynamic_string &name_field_name,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &error_message)

// Read shapefile & add to map
// Copied from FarmlandFinder

{
	map_layer* layer_roads;
	importer_shapefile shapefile;
	ProximityLayer proxy;
	int color_index = map->layers.size();
	COLORREF* proximity_pallette;

	proximity_pallette = make_palette_10 ();

	layer_roads = map->create_new (MAP_OBJECT_POLYGON);
	layer_roads->name = layer_name;
	layer_roads->color = proximity_pallette [color_index];
	map->layers.push_back (layer_roads);

	shapefile.filename_source = path_proximity;
	shapefile.filename_source += filename_proximity;

	if (id_field_name.get_length () > 0)
		shapefile.id_field_name = id_field_name;

	if (name_field_name.get_length () > 0)
		shapefile.name_field_name = name_field_name;

	shapefile.projection_from_prj_file = true;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	delete [] proximity_pallette;

	view->update_progress (layer_name, 1);
	if (shapefile.import (layer_roads, NULL, map, view, error_message)) {

		if (layer_roads->type == MAP_OBJECT_POINT) {
			layer_roads->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			layer_roads->symbol = MAP_OBJECT_SYMBOL_PLUS;
		}
		else
			layer_roads->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;

		proxy.layer = layer_roads;
		proximity_layers.push_back (proxy);
		return &proximity_layers[proximity_layers.size() - 1];
	}
	else
		return NULL;
}

/*
bool MapLayerProximity::read_proximity_shapefile
	(const dynamic_string& path_proximity,
	const dynamic_string& filename_proximity,
	const dynamic_string& id_field_name,
	const dynamic_string& name_field_name,
	void (*p_write_layer_point) (const class map_object*, const double distance_m, const char delimiter, dynamic_string&),
	void (*p_write_column_headers) (const char delimiter, dynamic_string&),
	map_layer* layer_roads,
	dynamic_map *map,
	interface_window* view,
	dynamic_string& error_message)

// 2022-04-08 Version that adds to existing map layer
// Read shapefile & add to map
// Copied from FarmlandFinder

{
	importer_shapefile shapefile;
	ProximityLayer proxy;

	shapefile.filename_source = path_proximity;
	shapefile.filename_source += filename_proximity;

	if (id_field_name.get_length() > 0) {
		shapefile.id_field_name = id_field_name;
		shapefile.has_id = true;
	}
	else
		shapefile.has_id = false;

	if (name_field_name.get_length() > 0) {
		shapefile.name_field_name = name_field_name;
		shapefile.has_name = true;
	}
	else
		shapefile.has_name = false;
	shapefile.projection = SHAPEFILE_PROJECTED_READPRJ;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	view->update_progress(filename_proximity, 1);
	if (shapefile.import(layer_roads, map, view, error_message)) {

		if (layer_roads->type == MAP_OBJECT_POINT) {
			layer_roads->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			layer_roads->symbol = MAP_OBJECT_SYMBOL_PLUS;
		}
		else
			layer_roads->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;

		proxy.layer = layer_roads;
		proxy.write_layer_point = p_write_layer_point;
		proxy.write_column_headers = p_write_column_headers;
		proximity_layers.push_back(proxy);
		return true;
	}
	else
		return false;
}
*/

ProximityLayer *MapLayerProximity::read_proximity_csv
	(const dynamic_string &filename_proximity,
	const dynamic_string &layer_name,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

// Read CSV, create layer & add to map

{
	map_layer *layer_points;
	map_object *point;
	dynamic_string inconstant_filename = filename_proximity, data;
	std::vector <dynamic_string> tokens;
	char buffer[8192];
	ProximityLayer proxy;
	FILE *f;
	bool error = false;
	int point_count;

	layer_points = map->create_new (MAP_OBJECT_POINT);
	layer_points->name = layer_name;
	layer_points->color = RGB (0, 255, 0);
	layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_points->symbol = MAP_OBJECT_SYMBOL_PLUS;
	layer_points->attribute_count_text = 1;
	layer_points->column_names_text.push_back ("Location Name");
	layer_points->initialize_attributes = true;
	map->layers.push_back (layer_points);

	view->update_progress (layer_name, 1);
	if (fopen_s (&f, inconstant_filename.get_text_ascii (), "r") == 0) {
		if (fgets (buffer, 8192, f)) {
			point_count = 0;
			while (fgets (buffer, 8192, f)
			&& !error) {
				data = buffer;
				data.tokenize (",\n", &tokens, true, false);

				// ID,Name,Address,City,State,ZIP,USDA Federal Liscense (F),Latitude,Longitude
				// GW_1,"ADM Alliance Nutrition, Inc.",1300 W Locust,Springfield,MO,65803,,37.226907,-93.3088351
				// GW_2,ADM Grain Company,#1 East Grand Avenue,St. Louis,MO,63147,,38.6786287,-90.1935591

				if (tokens.size () == 9) {
					point = layer_points->create_new (MAP_OBJECT_POINT);
					point->id = ++point_count;
					point->name = tokens[0];
					point->attributes_text[0] = tokens[1];
					point->latitude = long (atof (tokens[7].get_text_ascii ()) * 1.0e6);
					point->longitude = long (atof (tokens[8].get_text_ascii ()) * 1.0e6);
					layer_points->objects.push_back (point);
				}
				else {
					error = true;
					log += "ERROR, invalid format in file \"";
					log += filename_proximity;
					log += "\"\n";
				}
			}
		}
		else
			error = true;
		fclose (f);

		proxy.layer = layer_points;
		proximity_layers.push_back (proxy);
		return &proximity_layers [proximity_layers.size () - 1];
	}
	else
		return NULL;
}

ProximityLayer *MapLayerProximity::read_proximity_csv_2
	(const dynamic_string &filename_proximity,
	const dynamic_string &layer_name,
	const long max_lake_id, // 2022-01-11 Yongjie: omit lakes 140..142 
	dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

// 2022-01-03 Read CSV file exported from "I:\TDC\Lake Proximity\Lake139.xlsx"
// ID, Name, County, Lat, Long

{
	map_layer *layer_points;
	map_object *point;
	dynamic_string inconstant_filename = filename_proximity, data;
	std::vector <dynamic_string> tokens;
	char buffer[8192];
	ProximityLayer proxy;
	FILE *f;
	bool error = false;
	int point_count;
	long lake_id;

	layer_points = map->create_new (MAP_OBJECT_POINT);
	layer_points->name = layer_name;
	layer_points->color = RGB (0, 255, 0);
	layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_points->symbol = MAP_OBJECT_SYMBOL_PLUS;
	layer_points->attribute_count_text = 1;
	layer_points->column_names_text.push_back ("County");
	layer_points->initialize_attributes = true;
	map->layers.push_back (layer_points);

	view->update_progress (layer_name, 1);
	if (fopen_s (&f, inconstant_filename.get_text_ascii (), "r") == 0) {
		if (fgets (buffer, 8192, f)) {
			point_count = 0;
			while (fgets (buffer, 8192, f)
			&& !error) {
				data = buffer;
				data.tokenize (",\n", &tokens, true, false);

					// lakeid	Lakename	County	Lat	Lon
					// 1	Ada Hayden Lake	Story	42.07232	-93.6251
					// 2	Arbor Lake	Poweshiek	41.73173	-92.7321


				if (tokens.size () == 5) {
					lake_id = atol (tokens[0].get_text_ascii ());
					if (lake_id <= max_lake_id) {
						point = layer_points->create_new (MAP_OBJECT_POINT);
						point->id = lake_id;
						point->name = tokens[1];
						point->attributes_text[0] = tokens[2];
						point->latitude = long (atof (tokens[3].get_text_ascii ()) * 1.0e6);
						point->longitude = long (atof (tokens[4].get_text_ascii ()) * 1.0e6);
						layer_points->objects.push_back (point);
					}
					else {
						log.add_formatted ("Skipping lake %ld \"", lake_id);
						log += tokens[1];
						log += "\".\n";
					}
				}
				else {
					error = true;
					log += "ERROR, invalid format in file \"";
					log += filename_proximity;
					log += "\"\n";
				}
			}
		}
		else
			error = true;
		fclose (f);

		proxy.layer = layer_points;
		proxy.write_layer_point = &lake_centroid_callback;
		proxy.write_column_headers = &lake_centroid_callback_headers;
		proximity_layers.push_back (proxy);
		return &proximity_layers [proximity_layers.size () - 1];
	}
	else
		return NULL;
}

bool MapLayerProximity::divide_urban_areas
	(const dynamic_string &layer_name,
	dynamic_map *map,
	dynamic_string &log)

// Wendong: use field from DBF to divide urban areas into UA and UC

// Field "UATYP10", value U is associated with Urbanized Area, C is Urban Cluster

{
	importer_shapefile shapefile;
	std::vector <ProximityLayer>::iterator layer, ua_layer;
	std::vector <map_object *> ua, uc;
	std::vector <map_object *>::iterator town;
	bool error = false;

	ua_layer = proximity_layers.end ();

	for (layer = proximity_layers.begin ();
	(ua_layer == proximity_layers.end ())
	&& (layer != proximity_layers.end ());
	++layer)
		if (layer->layer->name == layer_name)
			ua_layer = layer;

	if (ua_layer != proximity_layers.end ()) {
		ProximityLayer uc_layer;
		uc_layer.layer = map->create_new (MAP_OBJECT_POLYGON);
		uc_layer.layer->name = "Urban Clusters";
		uc_layer.layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
		uc_layer.layer->color = RGB (0, 127, 255);
		map->layers.push_back (uc_layer.layer);
		proximity_layers.push_back (uc_layer);

		for (town = ua_layer->layer->objects.begin ();
		town != ua_layer->layer->objects.end ();
		++town)
			if ((*town)->attributes_text[4] == "U")
				ua.push_back (*town);
			else
				if ((*town)->attributes_text[4] == "C")
					uc.push_back (*town);
				else {
					log.add_formatted ("ERROR, urban id %ld has invalid type\n", (*town)->id);
					error = true;
				}

		ua_layer->layer->objects.clear ();

		// ua back into ua_layer
		for (town = ua.begin ();
		town != ua.end ();
		++town)
			ua_layer->layer->objects.push_back (*town);

		// uc into uc_layer
		for (town = uc.begin ();
		town != uc.end ();
		++town)
			uc_layer.layer->objects.push_back (*town);

	}
	else
		error = true;
	return !error;
}

bool MapLayerProximity::read_state_shapefiles
	(const dynamic_string &path_proximity,
	const dynamic_string &layer_name,
	const dynamic_string &id_field_name,
	const dynamic_string &name_field_name,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &error_message)

// Read all shapefiles in folder into one layer & add to map

// Each import uses layer_state so that column names aren't added 50 times

{
	map_layer *layer_state, *layer_all_states = NULL;
	std::vector <dynamic_string> filenames;
	std::vector <map_object *>::iterator point;
	std::vector <dynamic_string>::iterator filename_shp;
	ProximityLayer proxy;
	int file_count = 0;

	if (list_of_matching_filenames ("*.shp", path_proximity, true, &filenames, error_message)) {

		for (filename_shp = filenames.begin ();
		filename_shp != filenames.end ();
		++filename_shp) {
			importer_shapefile shapefile;

			layer_state = map->create_new (MAP_OBJECT_POLYGON);
			layer_state->name = layer_name;

			shapefile.filename_source = *filename_shp;
			if (id_field_name.get_length () > 0)
				shapefile.id_field_name = id_field_name;

			if (name_field_name.get_length () > 0)
				shapefile.name_field_name = name_field_name;

			shapefile.projection_from_prj_file = true;
			shapefile.normalize_longitude = false;
			shapefile.take_dbf_columns = true;

			view->update_progress (layer_name, 1);
			if (shapefile.import (layer_state, NULL, map, view, error_message)) {
				if (++file_count == 1) {
					layer_all_states = map->create_new (MAP_OBJECT_POLYGON);
					layer_all_states->name = layer_name;
					map->layers.push_back (layer_all_states);
					proxy.layer = layer_all_states;
					proximity_layers.push_back (proxy);
				}

				// move all objects from imported layer_state to layer_roads
				for (point = layer_state->objects.begin ();
				point != layer_state->objects.end ();
				++point)
					layer_all_states->objects.push_back (*point);
				layer_state->objects.clear ();

				if (file_count == 1) {
					layer_all_states->copy (layer_state); // Type, attribute counts, column_names

					if (layer_state->type == MAP_OBJECT_POINT) {
						layer_all_states->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
						layer_all_states->symbol = MAP_OBJECT_SYMBOL_PLUS;
					}
					else
						layer_all_states->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
				}
			}

			delete layer_state;
		}

		return true;
	}
	else
		return false;
}

void MapLayerProximity::write_proximity_table_list_closest
	(const int list_size,
	interface_window *view,
	dynamic_string &log)

{
	std::vector <map_object *>::iterator sale;
	thread_manager threader;
	std::vector <ProximityLayer>::iterator layer;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator job_iterator;
	job_MapLayerProximity *job;
	thread_manager_statistics stats;
	int thread_index, job_count;
	dynamic_string progress, column_names;
	std::vector <FILE *> thread_files;
	std::vector <dynamic_string> thread_filenames;
	bool error = false;

	job_count = 100;
	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1; // 5;
	stats.reset (threader.thread_count);
	threader.statistics = &stats;

	if (output_filename.get_length() > 0) {
		if (!open_thread_files(&thread_files, &thread_filenames, log))
			error = true;
	}

	thread_index = 0;
	for (sale = base_layer->objects.begin ();
	sale != base_layer->objects.end ();
	++sale) {
		job = new job_MapLayerProximity;
		job->id = ++thread_index;
		if (output_filename.get_length() > 0) {
			job->work_function = &work_MapLayerProximity_list_file;
			job->output_files = &thread_files;
		}
		else
			job->work_function = &work_MapLayerProximity_list_log;
		job->list_size = list_size;
		job->base_layer_point = *sale;
		job->proximator = this;
		job->view = view;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	if (!error) {
		view->set_data (&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
		// threader.run (&jobs, view, log);
		threader.run_tiny (&jobs, view, log);
		view->set_data (NULL);
		view->update_status_bar ("Threader run complete");
	}

	if (output_filename.get_length() > 0) {
		append_thread_files(&thread_files, &thread_filenames, log);
		for (job_iterator = jobs.begin();
			job_iterator != jobs.end();
			++job_iterator) {
			log += job_iterator->second->log;
			job_iterator->second->log.clear();
		}
	}
	else {
		log += base_layer_column_headers;
		for (layer = proximity_layers.begin ();
		layer != proximity_layers.end ();
		++layer) {
			if (layer->write_column_headers) {
				// callback function for NHD lakes or rivers
				layer->write_column_headers (delimiter, column_names);
				log += column_names;
			}
			else {
				log.add_formatted ("%cNearest ", delimiter);
				log += layer->layer->name;
				log.add_formatted ("%cdistance, m", delimiter);
			}
		}
		log += "\n";

		for (job_iterator = jobs.begin ();
		job_iterator != jobs.end ();
		++job_iterator) {
			progress.format ("Writing output job %ld.\n", job_iterator->first);
			view->update_scroll (progress);
			log += job_iterator->second->log;
			job_iterator->second->log.clear ();
		}
	}

	// stats.write_time_summary (log);
}

void MapLayerProximity::compute_proximity
	(std::map <long, ProximityLayerResult> *all_results,
	interface_window *view,
	dynamic_string &log)

// Fill all_results with objects nearest to base_layer centroids 

{
	std::vector <map_object *>::iterator farm;
	thread_manager threader;
	std::vector <ProximityLayer>::iterator layer;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator job_iterator;
	job_MapLayerProximity *job;
	thread_manager_statistics stats;
	int job_count;
	dynamic_string progress, column_names;
	bool error = false;

	job_count = 100;
	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1; // 5;
	stats.reset (threader.thread_count);
	threader.statistics = &stats;

	for (farm = base_layer->objects.begin ();
	farm != base_layer->objects.end ();
	++farm) {
		job = new job_MapLayerProximity;
		job->id = (*farm)->id;
		job->work_function = &work_computed_proximity;
		job->base_layer_point = *farm;
		job->proximator = this;
		job->view = view;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	if (!error) {
		view->set_data (&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
		// threader.run (&jobs, view, log);
		threader.run_tiny (&jobs, view, log);
		view->set_data (NULL);
		view->update_status_bar ("Threader run complete");
	}

	for (job_iterator = jobs.begin ();
	job_iterator != jobs.end ();
	++job_iterator) {
		job = (job_MapLayerProximity *) job_iterator->second;
		all_results->insert (std::pair <long, ProximityLayerResult> (job->base_layer_point->id, job->result));
		log += job_iterator->second->log;
		delete job_iterator->second;
	}

	// stats.write_time_summary (log);
}

void MapLayerProximity::compute_enclosure
	(std::map <long, ProximityLayerResult> *all_results,
	interface_window *view,
	dynamic_string &log)

// Fill all_results with any boundaries that enclose base_layer centroids 

{
	std::vector <map_object *>::iterator farm;
	thread_manager threader;
	std::vector <ProximityLayer>::iterator layer;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator job_iterator;
	job_MapLayerProximity *job;
	thread_manager_statistics stats;
	int job_count;
	dynamic_string progress, column_names;
	bool error = false;

	job_count = 100;
	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1; // 5;
	stats.reset (threader.thread_count);
	threader.statistics = &stats;

	for (farm = base_layer->objects.begin ();
	farm != base_layer->objects.end ();
	++farm) {
		job = new job_MapLayerProximity;
		job->id = (*farm)->id;
		job->work_function = &work_computed_enclosure;
		job->base_layer_point = *farm;
		job->proximator = this;
		job->view = view;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	if (!error) {
		view->set_data (&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
		// threader.run (&jobs, view, log);
		threader.run_tiny (&jobs, view, log);
		view->set_data (NULL);
		view->update_status_bar ("Threader run complete");
	}

	for (job_iterator = jobs.begin ();
	job_iterator != jobs.end ();
	++job_iterator) {
		job = (job_MapLayerProximity *) job_iterator->second;
		all_results->insert (std::pair <long, ProximityLayerResult> (job->base_layer_point->id, job->result));
		log += job_iterator->second->log;
		delete job_iterator->second;
	}

	// stats.write_time_summary (log);
}

bool MapLayerProximity::read_shapefile
	(const dynamic_string& path_shapefiles,
	const dynamic_string& filename_shapefile,
	const dynamic_string& id_field_name,
	const dynamic_string& name_field_name,
	class map_layer* layer,
	class dynamic_map* map,
	class interface_window* view,
	dynamic_string& error_message)
{
	importer_shapefile shapefile;

	shapefile.filename_source = path_shapefiles;
	shapefile.filename_source += filename_shapefile;

	if (id_field_name.get_length() > 0)
		shapefile.id_field_name = id_field_name;

	if (name_field_name.get_length() > 0)
		shapefile.name_field_name = name_field_name;

	shapefile.projection_from_prj_file = true;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	view->update_progress(filename_shapefile, 1);
	if (shapefile.import(layer, NULL, map, view, error_message)) {

		if (layer->type == MAP_OBJECT_POINT) {
			layer->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			layer->symbol = MAP_OBJECT_SYMBOL_PLUS;
		}
		else
			layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;

		return true;
	}
	return false;
}

