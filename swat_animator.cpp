
#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include <stack>
#include <chrono>
#include <sys/timeb.h>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/listbox_files.h"
#include "../util/interface_window_wx.h"
#include "../util/device_coordinate.h"
#include "../util/arbitrary_counter.h"
#include "../util/autorange_histogram.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_field_set.h"

#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
// #include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/shapefile_filter.h"
#include "swat_animator.h"
#include "panel_shapefile.h"

swat_animator::swat_animator ()
{
	input_source = SWAT_ANIMATOR_INPUT_CSV;
}

swat_animator::~swat_animator ()

{
}

bool swat_animator::read_data
	(dynamic_string &filename_data,
	const dynamic_string &match_column_name,
	const BYTE y_m_d,
	std::map <Timestamp, std::map <long, double>> *map_data_by_time,
	arbitrary_count *ranger,
	autorange_histogram *histogram,
	map_layer *layer,
	dynamic_string &log)

// read all values by location & date for the selected column

// ID,Year,Month,day,Data1,Data2,Data3

{
	bool error = false;
	std::vector <dynamic_string> columns;
	FILE *file_csv;
	int column_index, draw_column_index = -1;
	dynamic_string data_line;
	std::vector <dynamic_string> column_names;
	double value;
	long id;
	Timestamp when;
	std::map <long, double> place_data;
	std::map <Timestamp, std::map <long, double>>::iterator other_place_data;
	std::vector <dynamic_string>::iterator column;

	if (fopen_s (&file_csv, filename_data.get_text_ascii (), "r") == 0) {
		char buffer[1024];
		if (fgets (buffer, 1024, file_csv)) {
			// read column names
			column_index = 0;
			data_line = buffer;
			data_line.tokenize (",\t\n", &column_names, true, false);

			column = column_names.begin ();
			// First colummns are ID,Year,Month,day
			++column;
			if (y_m_d >= 2)
				++column;
			if (y_m_d >= 3)
				++column;
			++column;

			column_index = 1 + y_m_d;
			while (column != column_names.end ()) {
				layer->column_names_numeric.push_back (*column);
				if (*column == match_column_name)
					draw_column_index = column_index;
				++column;
				++column_index;
			}

			if (draw_column_index != -1) {
				while (fgets (buffer, 1024, file_csv)) {
					data_line = buffer;
					data_line.tokenize (",\t\n", &columns, true, false);
					if (columns.size () > 3) {
						column = columns.begin ();
						id = atol (column->get_text_ascii ());
						++column;
						when.set_year (atoi (column->get_text_ascii ()));
						++column;
						if (y_m_d >= 2) {
							when.set_month (atoi (column->get_text_ascii ()));
							++column;
						}
						if (y_m_d >= 3) {
							when.set_day (atoi (column->get_text_ascii ()));
							++column;
						}

						value = atof (columns[draw_column_index].get_text_ascii ());

						if (value != 0.0) {
							if (when < earliest_date)
								earliest_date = when;
							if (when > latest_date)
								latest_date = when;

							ranger->accumulate (value);

							// histogram tracks each point for color pallette divisions
							histogram->absorb_id (id, value, log);

							if ((other_place_data = map_data_by_time->find (when)) != map_data_by_time->end ())
								other_place_data->second.insert (std::pair <long, double> (id, value));
							else {
								// first occurance of when
								place_data.insert (std::pair <long, double> (id, value));
								map_data_by_time->insert (std::pair <Timestamp, std::map <long, double>> (when, place_data));
							}
						}
					}
					else {
						error = true;
						log.add_formatted ("ERROR too few columns in swat_animator.preread_data_ranges\n");
					}
				}
			}
			else {
				error = true;
				log += "ERROR, selected column name \"";
				log += match_column_name;
				log += "\" not found in file.\n";
			}
		}
		fclose (file_csv);

	}
	else {
		log += "ERROR, unable to open csv file \"";
		log += filename_data;
		log += "\".\n";
	}
	return !error;
}

bool swat_animator::read_data_odbc
	(dynamic_string &filename_data,
	const dynamic_string &match_column_name,
	std::map <int, std::map <long, double>> *map_data_by_input_level,
	arbitrary_count *ranger,
	autorange_histogram *histogram,
	map_layer *layer,
	dynamic_string &log)

// see read_layer_data_odbc in frame_CARDGIS

{
	bool error = false;
	std::vector <dynamic_string> columns;
	std::vector <dynamic_string>::iterator column;
	long source_subbasin, impacted_subbasin;
	char source_huc[13], impacted_huc[13];
	double Delta, data, flow [2], p [2];
	int input_level;
	std::map <long, double> place_data;
	std::map <int, std::map <long, double>>::iterator other_place_data;
	dynamic_string filter;

	odbc_field_set results;

	odbc_database_access *db_layer;
	db_layer = new odbc_database_access;
	odbc_database_credentials creds;
	creds.set_database_filename (filename_data);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (db_layer->open (&creds, NULL, log)) {
		results.table_name = "Phosphorus PS Ramp";

		filter += db_layer->identifier_quote_character_open ();
		filter += "Source Subbasin";
		filter += db_layer->identifier_quote_character_close ();
		filter += " = ";
		filter += "1010";

		// 2018-09-25 subbasin_effect ramp run
		results.add_field (1, &source_subbasin, "Source Subbasin");
		results.add_field (2, &impacted_subbasin, "Impacted Subbasin");
		results.add_field (3, &Delta, "Phosphorus Change - tons per year");
		results.add_field (4, source_huc, 13, "Source HUC12");
		results.add_field (5, impacted_huc, 13, "Impacted HUC12");
		results.add_field (6, &flow [0], "Baseline Flow Out - cms");
		results.add_field (7, &flow [1], "Experiment Flow Out - cms");
		results.add_field (8, &p [0], "Baseline Phosphorus - tons per year");
		results.add_field (9, &p [1], "Experiment Phosphorus - tons per year");

		if (results.open_read (db_layer, log, filter)) {
			if (results.move_first (log)) {
				do {
					data = p[1] - p[0];
					input_level = (int) Delta;

					ranger->accumulate (data);

					// histogram tracks each point for color pallette divisions
					histogram->absorb_id (impacted_subbasin, data, log);

					if ((other_place_data = map_data_by_input_level->find (input_level)) != map_data_by_input_level->end ())
						other_place_data->second.insert (std::pair <long, double> (impacted_subbasin, data));
					else {
						// first occurance of input_level
						place_data.clear ();
						place_data.insert (std::pair <long, double> (impacted_subbasin, data));
						map_data_by_input_level->insert (std::pair <int, std::map <long, double>> (input_level, place_data));
					}
				} while (results.move_next (log));
			}
			results.close ();
		}
		else
			error = true;

		db_layer->close ();
		delete db_layer;
	}
	else {
		error = true;
		log += "ERROR, unable to open database file \"";
		log += filename_data;
		log += "\".\n";
	}
	return !error;
}

bool swat_animator::read_daily_hsl
	(dynamic_string &filename_data,
	const dynamic_string &match_column_name,
	const BYTE y_m_d,
	std::map <Timestamp, std::map <long, double>> *map_data_by_time,
	arbitrary_count *ranger,
	autorange_histogram *histogram,
	map_layer *layer,
	dynamic_string &log)

// read HSL daily values from DailySHYD.out processed by wxMap to show only ROUTE nodes downstream from target subbasin

{
	bool error = false;
	std::vector <dynamic_string> columns;
	FILE *file_csv;
	dynamic_string data_line;
	double baseline_water, experiment_water, baseline_varoute, experiment_varoute, baseline_shyd, experiment_shyd, day_value = 0.0;
	long hsl, reach;
	Timestamp when;
	std::map <long, double> place_data;
	std::map <Timestamp, std::map <long, double>>::iterator other_place_data;
	std::vector <dynamic_string>::iterator column;

	if (fopen_s (&file_csv, filename_data.get_text_ascii (), "r") == 0) {
		char buffer[1024];

		if (fgets (buffer, 1024, file_csv)) {
			// colummns are HSL,Reach,YYYY-MM-DD, baseline varoute NO3, experiment varoute NO3, baseline SHYD no3, experiment varoute no3

			while (fgets (buffer, 1024, file_csv)) {
				data_line = buffer;
				data_line.tokenize (",\t\n", &columns, true, false);
				if (columns.size () > 3) {
					column = columns.begin ();
					hsl = atol (column->get_text_ascii ());
					++column;
					reach = atol (column->get_text_ascii ());
					++column;
					when.set_year (atoi (column->get_text_ascii ()));
					++column;
					if (y_m_d >= 2) {
						when.set_month (atoi (column->get_text_ascii ()));
						++column;
					}
					if (y_m_d >= 3) {
						when.set_day (atoi (column->get_text_ascii ()));
						++column;
					}

					baseline_water = atof (column->get_text_ascii ());
					++column;
					experiment_water = atof (column->get_text_ascii ());
					++column;
					baseline_varoute = atof (column->get_text_ascii ());
					++column;
					experiment_varoute = atof (column->get_text_ascii ());
					// shyd no3
					++column;
					baseline_shyd = atof (column->get_text_ascii ());
					++column;
					experiment_shyd = atof (column->get_text_ascii ());

					if (match_column_name.match_insensitive ("Baseline Varoute water") != -1)
						day_value = baseline_varoute;
					else {
						if (show_diffs) {
							if (match_column_name.match_insensitive ("Baseline Varoute NO3") != -1)
								day_value = experiment_varoute - baseline_varoute;
							else
								if (match_column_name.match_insensitive ("Baseline SHYD no3") != -1)
									day_value = experiment_shyd - baseline_shyd;
						}
						else {
							// comparison of VAROUTE vs baseline
							if (match_column_name.match_insensitive ("Baseline Varoute no3") != -1)
								day_value = baseline_varoute;
							else
								if (match_column_name.match_insensitive ("Baseline SHYD no3") != -1)
									day_value = baseline_shyd;
						}
					}

					if (when < earliest_date)
						earliest_date = when;
					if (when > latest_date)
						latest_date = when;

					ranger->accumulate (day_value);

					// histogram tracks each point for color pallette divisions
					histogram->absorb_id (reach, day_value, log);

					if ((other_place_data = map_data_by_time->find (when)) != map_data_by_time->end ())
						other_place_data->second.insert (std::pair <long, double> (reach, day_value));
					else {
						// first occurance of when
						place_data.insert (std::pair <long, double> (reach, day_value));
						map_data_by_time->insert (std::pair <Timestamp, std::map <long, double>> (when, place_data));
					}
				}
				else {
					error = true;
					log += "ERROR, selected column name \"";
					log += match_column_name;
					log += "\" not found in file.\n";
				}
			}
		}
		fclose (file_csv);

	}
	else {
		log += "ERROR, unable to open csv file \"";
		log += filename_data;
		log += "\".\n";
	}
	return error;
}

void swat_animator::set_colors_no3
	(map_layer *layer)

// use range of all years & months so colors don't change between images

{
	int number_of_divisions;
	// double interval, last_interval;
	COLORREF *pallette;
	bool monthly = false;

	number_of_divisions = 10;

	// pallette [2] = RGB (255, 127, 39); // orange

	if (monthly)
		pallette = make_palette_10 ();
	else {
		pallette = new COLORREF[number_of_divisions];
		pallette[0] = RGB (0, 255, 0); // Green
		pallette[1] = RGB (192, 192, 192); // gray 0 value
		pallette[2] = RGB (0, 0, 255); // Blue
		pallette[3] = RGB (0, 128, 255); // Medium blue
		pallette[4] = RGB (0, 255, 255); // Cyan
		pallette[5] = RGB (255, 255, 0); // yellow
		pallette[6] = RGB (255, 128, 128); // pink
		pallette[7] = RGB (255, 0, 0); // red
		pallette[8] = RGB (191, 0, 0); /// slightly dark red
		pallette[9] = RGB (128, 0, 0); // dark red
	}

	layer->colors.clear ();

	if (monthly) {
		// Monthly NO3 values are 0, 400, 500, 600, 800, 1000, 10000, 100000
		layer->colors.add_color (pallette[0], 0.0, 0.1, "0");
		layer->colors.add_color (pallette[1], 400.0, 400.1, "400");
		layer->colors.add_color (pallette[2], 500.0, 500.1, "500");
		layer->colors.add_color (pallette[3], 580.0, 580.1, "580");
		layer->colors.add_color (pallette[4], 600.0, 600.1, "600");
		layer->colors.add_color (pallette[5], 700.0, 700.1, "700");
		layer->colors.add_color (pallette[6], 800.0, 800.1, "800");
		layer->colors.add_color (pallette[7], 1000.0, 1000.1, "1,000");
		layer->colors.add_color (pallette[8], 10000.0, 10000.1, "10,000");
		layer->colors.add_color (pallette[9], 100000.0, 100000.1, "100,000");
	}
	else {

		// 40 unique values: 10000, 1000, 200, 120, 110, 100, 90, 80, 70, 60,...1, .100006, 0, -20, -30
		layer->colors.add_color (pallette[0], -30.0, -0.1, "-30..-0.1");
		layer->colors.add_color (pallette[1], 0.0, 0.1, "0");
		layer->colors.add_color (pallette[2], 0.1, 10.0, "0.1..10");
		layer->colors.add_color (pallette[3], 10.0, 20.0, "10..20");
		layer->colors.add_color (pallette[4], 20.0, 30.0, "20..30");
		layer->colors.add_color (pallette[5], 30.0, 100.0, "30..100");
		layer->colors.add_color (pallette[6], 100.0, 120.0, "100..120");
		layer->colors.add_color (pallette[7], 200.0, 200.1, "200");
		layer->colors.add_color (pallette[8], 1000.0, 1000.1, "1,000");
		layer->colors.add_color (pallette[9], 10000.0, 10000.1, "10,000");
	}

	/*

	// 0.0 values use first color, then divide up the range from nonzero_min to max
	interval = (ranger.get_max () - ranger.get_nonzero_min ()) / ((double) count - 1.0);

	last_interval = ranger.get_nonzero_min () - 0.001;
	layer->colors.add_color (pallette [0], 0.0, last_interval, "0");

	for (index = 1; index < count; ++index) {
		if (index < count - 1)
			layer->colors.add_color (pallette[index], last_interval, last_interval + interval);
		else
			layer->colors.add_color (pallette[index], last_interval, last_interval + interval + 1.0);
		last_interval += interval;
	}
	layer->colors->make_labels ();
	*/
	delete[] pallette;

}

bool swat_animator::render_frame
	(const Timestamp &when,
	const BYTE y_m_d,
	std::map <Timestamp, std::map <long, double>> *map_data,
	dynamic_map *map,
	map_layer *layer,
	dynamic_string &log)

// Set the polygons in the map_layer to values for this date

{
	std::map <Timestamp, std::map <long, double>>::const_iterator data_time;
	std::map <long, double>::const_iterator frame_value;
	bool error = false;
	map_object *polygon;
	std::vector <map_object *>::iterator object;

	// clear all existing polygon values
	for (object = layer->objects.begin ();
	object != layer->objects.end ();
	++object)
		(*object)->attributes_numeric[layer->data_attribute_index] = 0.0;

	if ((data_time = map_data->find (when)) != map_data->end ()) {

		for (frame_value = data_time->second.begin ();
		frame_value != data_time->second.end ();
		++frame_value) {
			if ((polygon = layer->match_id (frame_value->first)) != NULL)
				polygon->attributes_numeric[layer->data_attribute_index] = frame_value->second;
		}
	}

	if (y_m_d == 2)
		map->title.add_formatted (" %04d-%02d", when.get_year (), when.get_month ());
	else
		map->title.add_formatted (" %04d-%02d-%02d", when.get_year (), when.get_month (), when.get_day ());

	return !error;
}

bool swat_animator::render_frame_polygon_data
	(const std::map <long, double> *average_by_subbasin,
	dynamic_map *map,
	map_layer *layer,
	dynamic_string &log)

// Set the polygons in the map_layer to values for this date

{
	std::map <long, double>::const_iterator subbasin_sum;
	bool error = false;
	map_object *polygon;
	std::vector <map_object *>::iterator object;

	// clear all existing polygon values
	for (object = layer->objects.begin ();
	object != layer->objects.end ();
	++object)
		(*object)->attributes_numeric[layer->data_attribute_index] = 0.0;

	// divide by # of years
	for (subbasin_sum = average_by_subbasin->begin ();
	subbasin_sum != average_by_subbasin->end ();
	++subbasin_sum) {
		if ((polygon = layer->match_id (subbasin_sum->first)) != NULL)
			polygon->attributes_numeric[layer->data_attribute_index] = subbasin_sum->second;
	}

	return !error;
}

void swat_animator::set_layer_pallette
	(class map_layer *layer,
	arbitrary_count *ranger,
	autorange_histogram *histogram,
	dynamic_string &log)

{
	// bool pallette_near_zero_weighting;

	ranger->write (log);
	histogram->set (10, AUTORANGE_BIN_EQUAL_POINT_COUNT, true, true, log);

	// set_colors_no3 (layer);
	layer->colors.clear ();

	// layer->colors->autorange (ranger, 10, true, true, log);
	layer->colors.autorange (histogram, log);

	/*
	if (ranger->min < 0.0) {
		COLORREF *use_pallette;
		use_pallette = make_pallette_10 ();
		// Change second color to bright green for negative values
		use_pallette[1] = RGB (0, 255, 0);

		// Set layer colors to 10 ranges
	// produce a histogram of 10 range bins, use it to set colors
		layer->colors.add_color_set_label (use_pallette[0], 0.0, 0.00000125);
		layer->colors.add_color_set_label (use_pallette[1], 0.00000125, 0.0001);
		layer->colors.add_color_set_label (use_pallette[2], 0.0001, 0.0002);
		layer->colors.add_color_set_label (use_pallette[3], 0.0002, 0.0005);
		layer->colors.add_color_set_label (use_pallette[4], 0.0005, 0.001);
		layer->colors.add_color_set_label (use_pallette[5], 0.001, 0.05);
		layer->colors.add_color_set_label (use_pallette[6], 0.05, 0.1);
		layer->colors.add_color_set_label (use_pallette[7], 0.1, 0.05);
		layer->colors.add_color_set_label (use_pallette[8], 1.0, 3.0);
		layer->colors.add_color_set_label (use_pallette[9], 3.0, 3.65);
		layer->colors->dump (log);
		delete[] use_pallette;
	}

	if (ranger->max > 0.0)
		pallette_near_zero_weighting = true;
	else
		pallette_near_zero_weighting = false;
	*/
}

bool swat_animator::draw_map_on_bitmap
	(wxBitmap &bitmap,
	const wxRect &box,
	dynamic_map *map,
	dynamic_string &error_message)

// Get the map to render itself into bitmap

{
	wxMemoryDC memory_dc;
	bool error = false;
	device_coordinate legend_corner;

	legend_corner.x = box.GetLeft ();
	legend_corner.y = box.GetTop ();

	if (override_logical_window.x[0] != DBL_MAX) {
		map->logical_extent.x [0]  = override_logical_window.x [0] * 1.0e6;
		map->logical_extent.x [1]  = override_logical_window.x [1] * 1.0e6;
		map->logical_extent.y [0]  = override_logical_window.y [0] * 1.0e6;
		map->logical_extent.y [1]  = override_logical_window.y [1] * 1.0e6;
	}

	// memory device context
	memory_dc.SelectObject (bitmap);

	// Fill with white background
	memory_dc.SetBrush (*wxWHITE_BRUSH);
	memory_dc.DrawRectangle (box);

	interface_window_wx view;
	map_scale scale_box, *old_scale;

	view.dc = &memory_dc;

	scale_box.set (NULL, box, &map->logical_extent, 0, SHAPEFILE_PROJECTED_LAT_LONG);

	old_scale = map->scale;
	map->scale = &scale_box;

	map->draw (&view, error_message);
	map->draw_legend (&view, legend_corner, error_message);

	// add_label (&memory_dc, &draw_box);
	memory_dc.SelectObject (wxNullBitmap);

	map->scale = old_scale;

	return !error;
}

/*
bool swat_animator::draw_map_on_image
	(wxImage *png_image,
	const wxRect &image_size,
	dynamic_map *map,
	dynamic_string &error_message)

// Called from page_prevalence for saving to file

{
	wxBitmap bitmap;
	bool error = false;

	if (bitmap.Create (image_size.GetWidth (), image_size.GetHeight ())) {
		if (draw_map_on_bitmap (bitmap, image_size, map, error_message))
			*png_image = bitmap.ConvertToImage();
		else
			error = true;

	}
	else {
		error = true;
		error_message += "ERROR, bitmap create failed.\n";
	}

	return !error;
}
*/

void swat_animator::make_bins
	(const int bin_count,
	const std::map <Timestamp, std::map <long, double>> *map_data)

{
	std::map <Timestamp, std::map <long, double>>::const_iterator date;
	std::map <long, double>::const_iterator point;
	int bin_size, bin_index, total_count = 0;

	for (date = map_data->begin ();
	date != map_data->end ();
	++date)
		total_count += date->second.size ();

	bin_size = total_count / bin_count;
	bin_index = 0;
	for (date = map_data->begin ();
	date != map_data->end ();
	++date)
		for (point = date->second.begin ();
		point != date->second.end ();
		++point) {
		}


	// sort map_data by values

	// for each bin #: need a set of ids and a GE value and a LE value
	// change ids, ge & le as necessary to equalize points



	for (date = map_data->begin ();
	date != map_data->end ();
	++date)
		for (point = date->second.begin ();
		point != date->second.end ();
		++point) {
			point->second;
		}
}

bool swat_animator::animate_polygons_by_level
	(std::map <int, std::map <long, double>> *data_by_input_level,
	const dynamic_string &output_path,
	dynamic_map *map,
	map_layer *layer,
	interface_window *view,
	dynamic_string &log)

// 1 image per input_level, a number ranging 10..250
// 1 value per polygon per image

{
	bool error = false;
	dynamic_string filename_output;
	std::map <int, std::map <long, double>>::const_iterator place_data;

	wxRect image_size;
	image_size.SetPosition (wxPoint (0, 0));
	image_size.SetSize (wxSize (1920, 1080));

	for (place_data = data_by_input_level->begin ();
	place_data != data_by_input_level->end ();
	++place_data) {
		view->update_progress_formatted (1, "Level %d", place_data->first);

		wxImage image_png;
		wxBitmap bitmap;

		if (bitmap.Create (image_size.GetWidth (), image_size.GetHeight ())) {

			map->title = target_column_name_1;
			map->title.add_formatted (" Level %d", place_data->first);

			if (render_frame_polygon_data (&place_data->second, map, layer, log)) {

				if (draw_map_on_bitmap (bitmap, image_size, map, log)) {

					filename_output = output_path;
					filename_output.add_formatted ("Level%d.png", place_data->first);

					image_png = bitmap.ConvertToImage ();
					if (!image_png.SaveFile (filename_output.get_text (), wxBITMAP_TYPE_PNG)) {
						log += "ERROR, can't save PNG image to \"";
						log += filename_output;
						log += "\".\n";
						error = true;
					}
				}
				else {
					error = true;
					log + "ERROR, cant' draw bitmap.\n";
				}
			}
			else
				error = true;
		}
	}
	return !error;
}

bool swat_animator::process
	(dynamic_string &filename_data,
	const dynamic_string &output_path,
	const BYTE y_m_d,
	dynamic_map *map,
	map_layer *layer,
	panel_shapefile *panel_watershed,
	interface_window *view,
	dynamic_string &log)

{
	dynamic_string filename_output, data_line;
	bool error = false;
	std::map <Timestamp, std::map <long, double>> data_by_timestamp;
	std::map <int, std::map <long, double>> data_by_input_level;
	autorange_histogram histogram_1;

	wxImage::AddHandler (new wxPNGHandler);

	// File has to be sorted by year,month (not ID)
	add_target_highlight (map, target_subbasin, layer);

	switch (input_source) {
		case SWAT_ANIMATOR_INPUT_CSV:
			read_data (filename_data, target_column_name_1, y_m_d, &data_by_timestamp, &ranger_1, &histogram_1, layer, log);
			break;
		case SWAT_ANIMATOR_INPUT_HSL:
			read_daily_hsl (filename_data, target_column_name_1, y_m_d, &data_by_timestamp, &ranger_1, &histogram_1, layer, log);
			break;
		case SWAT_ANIMATOR_INPUT_ODBC:
			read_data_odbc (filename_data, target_column_name_1, &data_by_input_level, &ranger_1, &histogram_1, layer, log);
		//	break;
		// case SWAT_ANIMATOR_INPUT_REACH:
			// read_output_rch (filename_data, target_column_name_1, &data_by_input_level, &ranger_1, &histogram_1, layer, view, log);
	}

	layer->resize (layer->attribute_count_numeric + 1, layer->attribute_count_text);
	layer->data_attribute_index = layer->attribute_count_numeric - 1;
	layer->draw_as = MAP_OBJECT_DRAW_FILL_AND_OUTLINE;
	layer->include_in_legend = true;

	set_layer_pallette (layer, &ranger_1, &histogram_1, log);

	if (input_source == SWAT_ANIMATOR_INPUT_ODBC)
		animate_polygons_by_level (&data_by_input_level, output_path, map, layer, view, log);
	else {
		std::map <Timestamp, std::map <long, double>>::const_iterator place_data;

		wxRect image_size;
		image_size.SetPosition (wxPoint (0, 0));
		image_size.SetSize (wxSize (1920, 1080));

		for (place_data = data_by_timestamp.begin ();
		place_data != data_by_timestamp.end ();
		++place_data) {
			view->update_progress_formatted (1, "Year %d Month %d day %d", place_data->first.get_year (), place_data->first.get_month (), place_data->first.get_day ());
			wxImage image_png;
			wxBitmap bitmap;

			if (bitmap.Create (image_size.GetWidth (), image_size.GetHeight ())) {

				map->title = target_column_name_1;
				if (render_frame (place_data->first, y_m_d, &data_by_timestamp, map, layer, log)) {

					if (draw_map_on_bitmap (bitmap, image_size, map, log)) {

						filename_output = output_path;
						filename_output.add_formatted ("%04d%02d%02d.png", place_data->first.get_year (), place_data->first.get_month (), place_data->first.get_day ());

						image_png = bitmap.ConvertToImage ();
						if (!image_png.SaveFile (filename_output.get_text (), wxBITMAP_TYPE_PNG)) {
							log += "ERROR, can't save PNG image to \"";
							log += filename_output;
							log += "\".\n";
							error = true;
						}
					}
					else {
						error = true;
						log + "ERROR, cant' draw bitmap.\n";
					}
				}
				else
					error = true;
			}
		}
	}

	return !error;
}

void swat_animator::add_target_highlight
	(dynamic_map *map,
	const long target_polygon,
	map_layer *data_layer)

{
	map_layer *highlight_layer;
	std::vector <map_object *>::iterator object;

	highlight_layer = map->create_new (MAP_OBJECT_POLYGON);
	highlight_layer->copy (data_layer);
	highlight_layer->name = "Target";

	highlight_layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	highlight_layer->include_in_legend = true;
	// highlight_layer->colors.add_color (RGB (0, 0, 0), 0.0, 1.0);
	highlight_layer->colors.add_color (RGB (0, 255, 0), 1.0, 2.0);
	highlight_layer->line_width = 2;

	highlight_layer->data_attribute_index = 0;

	for (object = highlight_layer->objects.begin ();
	object != highlight_layer->objects.end ();
	++object) {
		if ((*object)->id == target_polygon)
			(*object)->attributes_numeric[highlight_layer->data_attribute_index] = 1.0;
		else
			(*object)->attributes_numeric[highlight_layer->data_attribute_index] = 0.0;
	}

	map->layers.push_back (highlight_layer);
}

bool swat_animator::process_side_by_side
	(dynamic_string &filename_data_1,
	dynamic_string &filename_data_2,
	const BYTE y_m_d,
	const bool unified_scale,
	const dynamic_string &output_path,
	dynamic_map *local_map_1,
	dynamic_map *local_map_2,
	map_layer *layer_1,
	map_layer *layer_2,
	panel_shapefile *panel_watershed,
	interface_window *view,
	dynamic_string &log)

{
	dynamic_string filename_output, data_line;
	bool error = false;
	std::map <Timestamp, std::map <long, double>> map_data_1, map_data_2;
	Timestamp when;
	autorange_histogram histogram_1, histogram_2;
	arbitrary_count *use_ranger;
	autorange_histogram *use_histogram;

	wxImage::AddHandler(new wxPNGHandler);

	earliest_date.set (9999, 12, 31, 0, 0, 0);
	latest_date.set (1, 1, 1, 0, 0, 0);

	// File has to be sorted by year,month (not ID)

	add_target_highlight (local_map_1, target_subbasin, layer_1);
	add_target_highlight (local_map_2, target_subbasin, layer_2);

	view->update_progress ("Reading Data 1", 0);

	if (input_source == SWAT_ANIMATOR_INPUT_HSL)
		read_daily_hsl (filename_data_1, target_column_name_1, y_m_d, &map_data_1, &ranger_1, &histogram_1, layer_1, log);
	else
		read_data (filename_data_1, target_column_name_1, y_m_d, &map_data_1, &ranger_1, &histogram_1, layer_1, log);

	view->update_progress ("Reading Data 2", 0);

	if (unified_scale) {
		use_ranger = &ranger_1;
		use_histogram = &histogram_1;
	}
	else {
		use_ranger = &ranger_2;
		use_histogram = &histogram_2;
	}
	if (input_source == SWAT_ANIMATOR_INPUT_HSL)
		read_daily_hsl (filename_data_2, target_column_name_2, y_m_d, &map_data_2, use_ranger, use_histogram, layer_2, log);
	else
		read_data (filename_data_2, target_column_name_2, y_m_d, &map_data_2, use_ranger, use_histogram, layer_2, log);

	layer_1->resize (layer_1->attribute_count_numeric + 1, layer_1->attribute_count_text);
	layer_1->data_attribute_index = layer_1->attribute_count_numeric - 1;
	layer_1->draw_as = MAP_OBJECT_DRAW_FILL_AND_OUTLINE;
	layer_1->include_in_legend = true;

	layer_2->resize (layer_2->attribute_count_numeric + 1, layer_2->attribute_count_text);
	layer_2->data_attribute_index = layer_2->attribute_count_numeric - 1;
	layer_2->draw_as = MAP_OBJECT_DRAW_FILL_AND_OUTLINE;
	layer_2->include_in_legend = true;

	set_layer_pallette (layer_1, &ranger_1, &histogram_1, log);
	if (unified_scale)
		layer_2->colors = layer_1->colors;
	else
		set_layer_pallette (layer_2, &ranger_2, &histogram_2, log);

	wxRect image_size_left, image_size_right, total_image_size;
	image_size_left.SetPosition (wxPoint (0, 0));
	image_size_left.SetSize (wxSize (960, 1080));
	image_size_right.SetPosition (wxPoint (960, 0));
	image_size_right.SetSize (wxSize (960, 1080));
	total_image_size.SetSize (wxSize (1920, 1080));

	view->update_progress ("Building Maps", 0);
	when = earliest_date;
	while (when <= latest_date) {

		if (y_m_d == 2)
			view->update_progress_formatted (1, "Year %d Month %d", when.get_year (), when.get_month ());
		else
			view->update_progress_formatted (1, "Year %d Month %d day %d", when.get_year (), when.get_month (), when.get_day ());
		wxImage image_png;
		wxBitmap bitmap;

		if (bitmap.Create (total_image_size.GetWidth (), total_image_size.GetHeight ())) {
			local_map_1->title = title_text_1;
			local_map_2->title = title_text_2;
			if (render_frame (when, y_m_d, &map_data_1, local_map_1, layer_1, log)
			&& render_frame (when, y_m_d, &map_data_2, local_map_2, layer_2, log)) {

				if (draw_map_on_bitmap (bitmap, image_size_left, local_map_1, log)
				&& draw_map_on_bitmap (bitmap, image_size_right, local_map_2, log)) {

					filename_output = output_path;
					if (y_m_d == 2)	
						filename_output.add_formatted ("%04d-%02d.png", (int) when.get_year (), (int) when.get_month ());
					else
						filename_output.add_formatted ("%04d%02d%02d.png", when.get_year (), when.get_month (), when.get_day ());

					image_png = bitmap.ConvertToImage ();
					if (!image_png.SaveFile (filename_output.get_text (), wxBITMAP_TYPE_PNG)) {
						error = true;
						log += "ERROR, can't save PNG image to \"";
						log += filename_output;
						log += "\".\n";
					}
				}
				else {
					log += "ERROR, cant' draw bitmaps.\n";
					error = true;
				}
			}
			else
				error = true;
		}
		else {
			error = true;
			log += "ERROR, bitmap create failed.\n";
		}
		if (y_m_d == 2)
			when.increment_month ();
		else
			when.increment_day ();
	}

	return !error;
}

bool swat_animator::process_image_side_by_side
	(dynamic_string &filename_data_1,
	dynamic_string &filename_data_2,
	const BYTE y_m_d,
	const bool unified_scale,
	const bool monthly_average,
	const dynamic_string &output_path,
	dynamic_map *local_map_1,
	dynamic_map *local_map_2,
	map_layer *layer_1,
	map_layer *layer_2,
	panel_shapefile *panel_watershed,
	interface_window *view,
	dynamic_string &log)

// Summarize to a single ave annual value per subbasin

{
	dynamic_string filename_output, data_line;
	bool error = false;
	std::map <Timestamp, std::map <long, double>> map_data_by_time_1, map_data_by_time_2;
	int denominator;
	autorange_histogram histogram_1, histogram_2;
	std::map <long, double> average_by_subbasin_1, average_by_subbasin_2;

	wxImage::AddHandler(new wxPNGHandler);

	earliest_date.set (9999, 12, 31, 0, 0, 0);
	latest_date.set (1, 1, 1, 0, 0, 0);

	// File has to be sorted by year,month (not ID)

	add_target_highlight (local_map_1, target_subbasin, layer_1);
	add_target_highlight (local_map_2, target_subbasin, layer_2);

	view->update_progress ("Reading Data 1", 0);
	read_data (filename_data_1, target_column_name_1, y_m_d, &map_data_by_time_1, &ranger_1, &histogram_1, layer_1, log);
	view->update_progress ("Reading Data 2", 0);
	if (unified_scale)
		read_data (filename_data_2, target_column_name_2, y_m_d, &map_data_by_time_1, &ranger_1, &histogram_1, layer_2, log);
	else
		read_data (filename_data_2, target_column_name_2, y_m_d, &map_data_by_time_2, &ranger_2, &histogram_2, layer_2, log);

	denominator = latest_date.get_year () - earliest_date.get_year () + 1;
	if (monthly_average)
		denominator *= 12;

	if (unified_scale) {
		ranger_1.clear ();
		histogram_1.clear ();
		compute_average (&map_data_by_time_1, denominator, monthly_average, &ranger_1, &histogram_1, &average_by_subbasin_1, log);
		compute_average (&map_data_by_time_2, denominator, monthly_average, &ranger_1, &histogram_1, &average_by_subbasin_2, log);
	}
	else {
		ranger_1.clear ();
		ranger_2.clear ();
		histogram_1.clear ();
		histogram_2.clear ();
		compute_average (&map_data_by_time_1, denominator, monthly_average, &ranger_1, &histogram_1, &average_by_subbasin_1, log);
		compute_average (&map_data_by_time_2, denominator, monthly_average, &ranger_2, &histogram_2, &average_by_subbasin_2, log);
	}

	layer_1->resize (layer_1->attribute_count_numeric + 1, layer_1->attribute_count_text);
	layer_1->data_attribute_index = layer_1->attribute_count_numeric - 1;
	layer_1->draw_as = MAP_OBJECT_DRAW_FILL_AND_OUTLINE;
	layer_1->include_in_legend = true;

	layer_2->resize (layer_2->attribute_count_numeric + 1, layer_2->attribute_count_text);
	layer_2->data_attribute_index = layer_2->attribute_count_numeric - 1;
	layer_2->draw_as = MAP_OBJECT_DRAW_FILL_AND_OUTLINE;
	layer_2->include_in_legend = true;

	set_layer_pallette (layer_1, &ranger_1, &histogram_1, log);
	if (unified_scale)
		layer_2->colors = layer_1->colors;
	else
		set_layer_pallette (layer_2, &ranger_2, &histogram_2, log);

	wxRect image_size_left, image_size_right, total_image_size;
	image_size_left.SetPosition (wxPoint (0, 0));
	image_size_left.SetSize (wxSize (960, 1080));
	image_size_right.SetPosition (wxPoint (960, 0));
	image_size_right.SetSize (wxSize (960, 1080));
	total_image_size.SetSize (wxSize (1920, 1080));

	view->update_progress ("Building Maps", 0);
	wxImage image_png;
	wxBitmap bitmap;

	if (bitmap.Create (total_image_size.GetWidth (), total_image_size.GetHeight ())) {
		local_map_1->title = title_text_1;
		local_map_2->title = title_text_2;
		if (render_frame_polygon_data (&average_by_subbasin_1, local_map_1, layer_1, log)
		&& render_frame_polygon_data (&average_by_subbasin_2, local_map_2, layer_2, log)) {

			if (draw_map_on_bitmap (bitmap, image_size_left, local_map_1, log)
			&& draw_map_on_bitmap (bitmap, image_size_right, local_map_2, log)) {

				filename_output = output_path;
				filename_output += output_filename_prefix;
				filename_output += ".png";

				image_png = bitmap.ConvertToImage ();
				if (!image_png.SaveFile (filename_output.get_text (), wxBITMAP_TYPE_PNG)) {
					error = true;
					log += "ERROR, can't save PNG image to \"";
					log += filename_output;
					log += "\".\n";
				}
			}
			else {
				log += "ERROR, cant' draw bitmaps.\n";
				error = true;
			}
		}
		else
			error = true;
	}
	else {
		error = true;
		log += "ERROR, bitmap create failed.\n";
	}

	return !error;
}

bool swat_animator::process_image
	(dynamic_string &filename_data,
	const BYTE y_m_d,
	const bool monthly_average,
	const dynamic_string &output_path,
	dynamic_map *local_map_1,
	map_layer *layer_1,
	panel_shapefile *panel_watershed,
	interface_window *view,
	dynamic_string &log)

// Summarize to a single ave annual value per subbasin

{
	dynamic_string filename_output, data_line;
	bool error = false;
	std::map <Timestamp, std::map <long, double>> map_data_by_time_1;
	int denominator;
	autorange_histogram histogram_1;
	std::map <long, double> average_by_subbasin_1;

	wxImage::AddHandler(new wxPNGHandler);

	earliest_date.set (9999, 12, 31, 0, 0, 0);
	latest_date.set (1, 1, 1, 0, 0, 0);

	// File has to be sorted by year,month (not ID)

	add_target_highlight (local_map_1, target_subbasin, layer_1);

	view->update_progress ("Reading Data", 0);
	read_data (filename_data, target_column_name_1, y_m_d, &map_data_by_time_1, &ranger_1, &histogram_1, layer_1, log);

	denominator = latest_date.get_year () - earliest_date.get_year () + 1;
	if (monthly_average)
		denominator *= 12;

	ranger_1.clear ();
	histogram_1.clear ();
	compute_average (&map_data_by_time_1, denominator, monthly_average, &ranger_1, &histogram_1, &average_by_subbasin_1, log);

	layer_1->resize (layer_1->attribute_count_numeric + 1, layer_1->attribute_count_text);
	layer_1->data_attribute_index = layer_1->attribute_count_numeric - 1;
	layer_1->draw_as = MAP_OBJECT_DRAW_FILL_AND_OUTLINE;
	layer_1->include_in_legend = true;

	set_layer_pallette (layer_1, &ranger_1, &histogram_1, log);

	wxRect total_image_size;
	total_image_size.SetPosition (wxPoint (0, 0));
	total_image_size.SetSize (wxSize (1920, 1080));

	view->update_progress ("Building Maps", 0);
	wxImage image_png;
	wxBitmap bitmap;

	if (bitmap.Create (total_image_size.GetWidth (), total_image_size.GetHeight ())) {
		local_map_1->title = title_text_1;
		if (render_frame_polygon_data (&average_by_subbasin_1, local_map_1, layer_1, log)) {

			if (draw_map_on_bitmap (bitmap, total_image_size, local_map_1, log)) {

				filename_output = output_path;
				filename_output += output_filename_prefix;
				filename_output += ".png";

				image_png = bitmap.ConvertToImage ();
				if (!image_png.SaveFile (filename_output.get_text (), wxBITMAP_TYPE_PNG)) {
					error = true;
					log += "ERROR, can't save PNG image to \"";
					log += filename_output;
					log += "\".\n";
				}
			}
			else {
				log += "ERROR, cant' draw bitmaps.\n";
				error = true;
			}
		}
		else
			error = true;
	}
	else {
		error = true;
		log += "ERROR, bitmap create failed.\n";
	}

	return !error;
}

bool swat_animator::compute_average
	(std::map <Timestamp, std::map <long, double>> *map_data_by_time,
	const int denominator,
	const bool monthly_average,
	class arbitrary_count *ranger,
	class autorange_histogram *histogram,
	std::map <long, double> *average_by_subbasin,
	dynamic_string &log)

// Clear ranger & histogram, re-fill them from average annual values

{
	std::map <Timestamp, std::map <long, double>>::const_iterator data_by_time;
	std::map <long, double>::const_iterator data_by_subbasin;
	std::map <long, double>::iterator subbasin_sum;
	std::map <long, int> count_by_subbasin;
	std::map <long, int>::iterator count;
	bool error = false;

	log += "\n";
	log += "Subbasin\tYear\tMonth\tValue\n";
	for (data_by_time = map_data_by_time->begin ();
	data_by_time != map_data_by_time->end ();
	++data_by_time) {

		for (data_by_subbasin = data_by_time->second.begin ();
		data_by_subbasin != data_by_time->second.end ();
		++data_by_subbasin) {
			log.add_formatted ("%ld\t%d\t%d\t%.3lf\n", data_by_subbasin->first,
			data_by_time->first.get_year (), data_by_time->first.get_month (),
			data_by_subbasin->second);

			if ((subbasin_sum = average_by_subbasin->find (data_by_subbasin->first)) != average_by_subbasin->end ())
				subbasin_sum->second += data_by_subbasin->second;
			else
				average_by_subbasin->insert (std::pair <long, double> (data_by_subbasin->first, data_by_subbasin->second));

			if ((count = count_by_subbasin.find (data_by_subbasin->first)) != count_by_subbasin.end ())
				count->second += 1;
			else
				count_by_subbasin.insert (std::pair <long, long> (data_by_subbasin->first, 1));
		}

	}

	log += "\n";
	log += "Subbasin\tAve Value\tCount\n";
	// divide by # of years
	for (subbasin_sum = average_by_subbasin->begin ();
	subbasin_sum != average_by_subbasin->end ();
	++subbasin_sum) {
		if (monthly_average) {
			if ((count = count_by_subbasin.find (subbasin_sum->first)) != count_by_subbasin.end ())
				subbasin_sum->second = subbasin_sum->second / (double) count->second;
		}
		else
			subbasin_sum->second = subbasin_sum->second / (double) denominator;

		log.add_formatted ("%ld\t%.3lf", subbasin_sum->first, subbasin_sum->second);
		if ((count = count_by_subbasin.find (subbasin_sum->first)) != count_by_subbasin.end ())
			log.add_formatted ("\t%d", count->second);
		log += "\n";

		ranger->accumulate (subbasin_sum->second);

		// histogram tracks each point for color pallette divisions
		histogram->absorb_id (subbasin_sum->first, subbasin_sum->second, log);
	}
	log += "\n";


	return !error;
}

