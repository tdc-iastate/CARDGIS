
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
#include "../util/device_coordinate.h"
#include "../util/bounding_cube.h"
#include "../util/xml_parser.h"
#include "../util/arbitrary_counter.h"
#include "../util/state_names.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/shapefile_filter.h"
#include "ZillowSales.h"
#include "../subbasin_effect/thread_manager.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"
#include "CountyMaster.h"

void zillow_point_callback_3
	(const map_object *point,
	const char delimiter,
	dynamic_string &log)

// Called by work_MapLayerProximity to write identifiers & data for Zillow Sales point

{
	// ID\tRow ID\tTrans ID\tImportParcelID
	log.add_formatted ("%ld", point->id);

	log += delimiter;
	log += "\"";
	log += point->name; // RowID
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [0]; // Trans ID
	log += "\"";

	log += delimiter;
	log.add_formatted ("%.1lf", point->attributes_numeric [0]); // ImportParc
}

class thread_nearest_water : public thread_work {
public:
	class map_object *site;
	bool site_is_zillow_sale;
	class map_layer *layer_rivers, *layer_lakes;
	dynamic_string output_table;
};

map_object *closest_water
	(map_object *site,
	map_layer *layer_rivers,
	map_layer *layer_lakes,
	double *distance_m,
	bool *result_is_lake)

{
	device_coordinate house_point;
	map_object *river, *lake;
	double nearest_river_m, nearest_lake_m;

	house_point.x = site->longitude;
	house_point.y = site->latitude;
	river = layer_rivers->find_nearest_object (house_point, &nearest_river_m);
	lake = layer_lakes->find_nearest_object (house_point, &nearest_lake_m);

	if (nearest_river_m < nearest_lake_m) {
		*result_is_lake = false;
		*distance_m = nearest_river_m;
		return river;
	}
	else {
		*result_is_lake = true;
		*distance_m = nearest_lake_m;
		return lake;
	}
}

bool work_find_water
	(thread_work *base_job)

{
	thread_nearest_water *job = (thread_nearest_water *) base_job;
	bool result_is_lake;
	double distance_m;
	map_object *water;

    job->start = std::chrono::system_clock::now();

	water = closest_water (job->site, job->layer_rivers, job->layer_lakes, &distance_m, &result_is_lake);

	if (job->site_is_zillow_sale) {
		// "House ID\tName\tImportParc\tDate\tWater ID\tWater Name\tDistance, m\tArea, km^2\tStream Level\tRiver or Lake\n";
		job->output_table.add_formatted ("%ld", job->site->id);
		job->output_table += "\t";
		job->output_table += job->site->name;
		job->output_table.add_formatted ("\t%.1lf", job->site->attributes_numeric[5]); // ImportParc
		job->output_table += "\t";
		job->output_table += job->site->attributes_text[0]; // Date
	}
	else {
		// "Outbreak ID\tOutbreak Name\tDate\tWater ID\tWater Name\tDistance, m\tArea, km^2\tStream Level\tRiver or Lake\n";
		job->output_table.add_formatted ("%ld", job->site->id);
		job->output_table += "\t";
		job->output_table += job->site->name;
		job->output_table += "\t";
		job->output_table += job->site->attributes_text[1];
	}

	job->output_table.add_formatted ("\t%ld", water->id);
	job->output_table += "\t";
	job->output_table += water->name;
	job->output_table.add_formatted ("\t%.6lf", distance_m);

	if (result_is_lake) {
		job->output_table.add_formatted ("\t%.6lf", water->attributes_numeric [0]);
		job->output_table += "\t";
		job->output_table += "\tLake";
	}
	else {
		job->output_table += "\t";
		// NHD_FTYPE_CANALDITCH or other
		job->output_table.add_formatted ("\t%d", (int) water->attributes_numeric[NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL]);
		job->output_table += "\t";
		job->output_table += NHD_FTYPE_name ((int) water->attributes_numeric[NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE]);
	}
	job->output_table += "\n";
	job->status = THREAD_WORK_STATUS_COMPLETE;

	job->end = std::chrono::system_clock::now();
	return job->status == THREAD_WORK_STATUS_COMPLETE;
}

class thread_nearest_outbreak : public thread_work {
public:
	class map_object *house;
	class map_layer *layer_ecoli;
	dynamic_string output_table;
};

bool work_find_outbreak
	(thread_work *base_job)

{
	thread_nearest_outbreak *job = (thread_nearest_outbreak *) base_job;
	map_object *outbreak;
	double nearest_outbreak_m;
	device_coordinate house_point;
	long interval_days;
	std::vector <map_object *>::iterator ecoli;
	std::map <long, long> intervals_by_id;
	std::map <long, long>::iterator days_between;
	map_layer outbreaks_in_time;

    job->start = std::chrono::system_clock::now();

	house_point.x = job->house->longitude;
	house_point.y = job->house->latitude;

	// Create vector of those outbreaks that fit time window
	for (ecoli = job->layer_ecoli->objects.begin ();
	ecoli != job->layer_ecoli->objects.end ();
	++ecoli) {
		if (((Timestamp *) job->house->attributes_void)->after ((Timestamp *) (*ecoli)->attributes_void)) {
			((Timestamp *) job->house->attributes_void)->get_interval_days (*((Timestamp *) (*ecoli)->attributes_void), &interval_days);
			if (interval_days <= 365) {
				outbreaks_in_time.objects.push_back (*ecoli);
				intervals_by_id.insert (std::pair <long, long> ((*ecoli)->id, interval_days));
			}
		}
	}

	ASSERT (intervals_by_id.size () == outbreaks_in_time.objects.size ());

	// Find closest among outbreaks_in_time
	if (outbreaks_in_time.objects.size () > 0) {
		outbreak = outbreaks_in_time.find_nearest_object (house_point, &nearest_outbreak_m);

		job->output_table.add_formatted ("%ld", job->house->id);
		job->output_table += "\t";
		job->output_table += job->house->name;
		job->output_table.add_formatted ("\t%.1lf", job->house->attributes_numeric [5]); // ImportParc
		job->output_table += "\t";
		job->output_table += job->house->attributes_text[0]; // Date

		job->output_table.add_formatted ("\t%ld", outbreak->id);
		job->output_table += "\t";
		job->output_table += outbreak->name;
		job->output_table += "\t";
		job->output_table += outbreak->attributes_text[1];
		days_between = intervals_by_id.find (outbreak->id);
		job->output_table.add_formatted ("\t%ld", days_between->second);
		job->output_table.add_formatted ("\t%.4lf", nearest_outbreak_m);

		job->output_table += "\n";

		outbreaks_in_time.objects.clear (); // ~map_layer would try to delete the map_objects, so clear pointers instead
	}
	job->status = THREAD_WORK_STATUS_COMPLETE;

	job->end = std::chrono::system_clock::now();
	return job->status == THREAD_WORK_STATUS_COMPLETE;
}

class thread_outbreak_5mile : public thread_work {
public:
	class map_object *outbreak;
	class map_layer *layer_zillow[ZILLOW_YEAR_COUNT];
	dynamic_string output_table;
	char delimiter;
};

bool work_outbreak_5mile
	(thread_work *base_job)

{
	thread_outbreak_5mile *job = (thread_outbreak_5mile *) base_job;
	double distance_m, distance_miles;
	std::vector <map_object *>::iterator house;
	std::vector <double> distances;
	std::vector <double>::iterator miles_to_house;
	map_layer nearby_houses;
	int year_index; // , construction_year;

    job->start = std::chrono::system_clock::now();

	for (year_index = 0; year_index < ZILLOW_YEAR_COUNT; ++year_index)
		if (job->layer_zillow[year_index]) {
			for (house = job->layer_zillow[year_index]->objects.begin ();
			house != job->layer_zillow[year_index]->objects.end ();
			++house) {
				distance_m = distance_meters ((*house)->latitude, (*house)->longitude, job->outbreak->latitude, job->outbreak->longitude);
				distance_miles = distance_m / METERS_PER_MILE;
				if (distance_miles <= 5.0) {
					nearby_houses.objects.push_back (*house);
					distances.push_back (distance_miles);
				}
			}
		}

	ASSERT (distances.size () == nearby_houses.objects.size ());

	// Find closest among outbreaks_in_time
	if (nearby_houses.objects.size () > 0) {
		// "Outbreak ID\tOutbreak Name\tDate\tHouse ID\tName\tImportParc\tSale Date\tYear Built\tDistance, m\n"

		for (house = nearby_houses.objects.begin (), miles_to_house = distances.begin ();
		house != nearby_houses.objects.end () && miles_to_house != distances.end ();
		++house, ++miles_to_house) {
			// construction_year = (int) (*house)->attributes_numeric[16];

			job->output_table.add_formatted ("%ld", job->outbreak->id);
			job->output_table += job->delimiter;
			job->output_table += "\"";
			job->output_table += job->outbreak->name;
			job->output_table += "\"";
			job->output_table += job->delimiter;
			job->output_table += "\"";
			job->output_table += job->outbreak->attributes_text[1];
			job->output_table += "\"";

			job->output_table += job->delimiter;
			job->output_table.add_formatted ("%ld", (*house)->id);
			job->output_table += job->delimiter;
			job->output_table += (*house)->name;
			job->output_table.add_formatted ("%c%.1lf", job->delimiter, (*house)->attributes_numeric[5]); // ImportParc
			job->output_table += job->delimiter;
			job->output_table += "\"";
			job->output_table += (*house)->attributes_text[0]; // Date
			job->output_table += "\"";
			job->output_table.add_formatted ("%c%.1lf", job->delimiter, (*house)->attributes_numeric[15]); // YearBuilt
			job->output_table.add_formatted ("%c%.6lf", job->delimiter, *miles_to_house);

			job->output_table += "\n";
		}

		nearby_houses.objects.clear (); // ~map_layer would try to delete the map_objects, so clear pointers instead
	}
	job->status = THREAD_WORK_STATUS_COMPLETE;

	job->end = std::chrono::system_clock::now();
	return job->status == THREAD_WORK_STATUS_COMPLETE;
}

class thread_sale_1000m : public thread_work {
public:
	class map_object *house;
	class map_layer *layer_ecoli, *layer_rivers, *layer_lakes;
	dynamic_string output_table;
};

bool work_sale_1000m
	(thread_work *base_job)

// Find nearest NHD water to house sale
// From there, find all outbreaks within 1000m

{
	thread_sale_1000m *job = (thread_sale_1000m *) base_job;
	double distance_m, nearest_distance_m;
	std::vector <map_object *>::iterator outbreak;
	std::vector <double> distances;
	std::vector <long> days_between;
	std::vector <double>::iterator distance;
	map_layer nearby_ecoli;
	std::vector <long>::iterator interval_days;
	long days, shortest_days, water_latitude, water_longitude;
	map_object *water, *nearest_outbreak, *most_recent_outbreak;
	bool result_is_lake;

    job->start = std::chrono::system_clock::now();

	water = closest_water (job->house, job->layer_rivers, job->layer_lakes, &distance_m, &result_is_lake);
	water->centroid (&water_latitude, &water_longitude);

	shortest_days = MAXLONG;
	most_recent_outbreak = NULL;
	nearest_distance_m = DBL_MAX;
	nearest_outbreak = NULL;

	for (outbreak = job->layer_ecoli->objects.begin ();
	outbreak != job->layer_ecoli->objects.end ();
	++outbreak) {
		distance_m = distance_meters ((*outbreak)->latitude, (*outbreak)->longitude, water_latitude, water_longitude);
		if (distance_m <= 1000.0) {
			nearby_ecoli.objects.push_back (*outbreak);
			distances.push_back (distance_m);

			((Timestamp *) job->house->attributes_void)->get_interval_days (*((Timestamp *) (*outbreak)->attributes_void), &days);
			days_between.push_back (days);

			if (days >= 0) {
				if (distance_m < nearest_distance_m) {
					nearest_distance_m = distance_m;
					nearest_outbreak = *outbreak;
				}

				if (days < shortest_days) {
					shortest_days = days;
					most_recent_outbreak = *outbreak;
				}
			}
		}
	}

	ASSERT (distances.size () == nearby_ecoli.objects.size ());
	ASSERT (days_between.size () == nearby_ecoli.objects.size ());

	// Find closest among outbreaks_in_time
	if (nearby_ecoli.objects.size () > 0) {
		// "House ID\tName\tImportParc\tSale Date\tYear Built\tWater ID\tWater Name\tDistance, m\tArea, km^2\tStream Level\tRiver or Lake\tOutbreak ID\tOutbreak Name\tDate\tDistance, m\tInterval, days\tNearest\tMost Recent"

		for (outbreak = nearby_ecoli.objects.begin (), distance = distances.begin (), interval_days = days_between.begin ();
		outbreak != nearby_ecoli.objects.end () && distance != distances.end () && interval_days != days_between.end ();
		++outbreak, ++distance, ++interval_days) {

			// House
			job->output_table.add_formatted ("%ld", job->house->id);
			job->output_table += "\t";
			job->output_table += job->house->name;
			job->output_table.add_formatted ("\t%.1lf", job->house->attributes_numeric[5]); // ImportParc
			job->output_table += "\t";
			job->output_table += job->house->attributes_text[0]; // Date
			job->output_table.add_formatted ("\t%.1lf", job->house->attributes_numeric[16]); // YearBuilt

			// Water
			job->output_table.add_formatted ("\t%ld", water->id);
			job->output_table += "\t";
			job->output_table += water->name;
			job->output_table.add_formatted ("\t%.6lf", distance_m);

			if (result_is_lake) {
				job->output_table.add_formatted ("\t%.6lf", water->attributes_numeric [0]);
				job->output_table += "\t";
				job->output_table += "\tLake";
			}
			else {
				job->output_table += "\t";
				// NHD_FTYPE_CANALDITCH or other
				job->output_table.add_formatted ("\t%d", (int) water->attributes_numeric[NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL]);
				job->output_table += "\t";
				job->output_table += NHD_FTYPE_name ((int) water->attributes_numeric[NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE]);
			}

			// outbreak
			job->output_table += "\t";
			job->output_table.add_formatted ("%ld", (*outbreak)->id);
			job->output_table += "\t";
			job->output_table += (*outbreak)->name;
			job->output_table += "\t";
			job->output_table += (*outbreak)->attributes_text[1];

			job->output_table.add_formatted ("\t%.6lf", *distance);

			job->output_table.add_formatted ("\t%ld", *interval_days);

			if ((*outbreak) == nearest_outbreak)
				job->output_table += "\t1";
			else
				job->output_table += "\t0";

			if ((*outbreak) == most_recent_outbreak)
				job->output_table += "\t1";
			else
				job->output_table += "\t0";

			job->output_table += "\n";
		}

		nearby_ecoli.objects.clear (); // ~map_layer would try to delete the map_objects, so clear pointers instead
	}
	job->status = THREAD_WORK_STATUS_COMPLETE;

	job->end = std::chrono::system_clock::now();
	return job->status == THREAD_WORK_STATUS_COMPLETE;
}

ZillowSales::ZillowSales ()

{
	thread_count = 12;
	for (int index = 0; index < 10; ++index)
		layer_zillow[index] = NULL;
}

ZillowSales::~ZillowSales ()

{
	// layer_zillow are list_members, will be deleted there
}

void ZillowSales::make_colors
	(color_set *zillow_colors)

{
	COLORREF *pallette = make_palette_10 ();
	zillow_colors->clear ();

	zillow_colors->add_color (pallette[0], 2009.0, 2010.0, "2009");
	zillow_colors->add_color (pallette[2], 2010.0, 2011.0, "2010");
	zillow_colors->add_color (pallette[3], 2011.0, 2012.0, "2011");
	zillow_colors->add_color (pallette[4], 2012.0, 2013.0, "2012");
	zillow_colors->add_color (pallette[5], 2013.0, 2014.0, "2013");
	zillow_colors->add_color (pallette[6], 2014.0, 2015.0, "2014");
	zillow_colors->add_color (pallette[7], 2015.0, 2016.0, "2015");
	zillow_colors->add_color (pallette[8], 2016.0, 2017.0, "2016");
	zillow_colors->add_color (pallette[9], 2017.0, 2018.0, "2017");

	delete[] pallette;
}

bool ZillowSales::load_zillow_sales
	(const dynamic_string &path_zillow,
	const int first_year,  // -1 for all
	const int last_year,
	const bool add_timestamp,
	dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// Read 10 shapfiles for 2009..2018

{
	int year_index;
	dynamic_string filename_shapefile;
	bool error = false;

	for (year_index = 0;
	// year_index < 1;
	year_index < ZILLOW_YEAR_COUNT;
	++year_index) {
		if ((first_year == -1)
		|| ((first_year <= (year_index + ZILLOW_FIRST_YEAR))
		&& (last_year >= (year_index + ZILLOW_FIRST_YEAR)))) {
			importer_shapefile importer;

			filename_shapefile = path_zillow;
			filename_shapefile.add_formatted ("MI_HousingPoints_%d.shp", year_index + ZILLOW_FIRST_YEAR);
			view->update_progress (filename_shapefile, 1);

			layer_zillow[year_index] = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_zillow[year_index]->name.format ("Zillow %d", year_index + 2009);
			layer_zillow[year_index]->type = MAP_OBJECT_POINT;
			// layer_zillow[year_index]->color = RGB (year_index * 12, year_index * 12, year_index * 12);
			layer_zillow[year_index]->symbol = MAP_OBJECT_SYMBOL_POINT;
			layer_zillow[year_index]->draw_as = MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;
			layer_zillow[year_index]->data_attribute_index = 2; // "SalesYear"
			layer_zillow[year_index]->draw_labels_id = false;
			make_colors (&layer_zillow[year_index]->colors);
			map_watershed->layers.push_back (layer_zillow[year_index]);

			importer.filename_source = filename_shapefile;
			importer.has_id = true;
			importer.id_field_name = "OBJECTID";
			importer.has_name = true;
			importer.name_field_name = "RowID_";
			importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			if (!importer.import (layer_zillow[year_index], NULL, map_watershed, view, log))
				error = true;

			if (add_timestamp) {
				std::vector <map_object *>::iterator sale;

				// Add a timestamp
				layer_zillow[year_index]->attributes_void_usage = MAP_LAYER_USAGE_VOID_TIMESTAMP;

				for (sale = layer_zillow [year_index]->objects.begin ();
				sale != layer_zillow [year_index]->objects.end ();
				++sale) {
					(*sale)->attributes_void = new Timestamp [1];
					((Timestamp *) ((*sale)->attributes_void))->parse ((*sale)->attributes_text[0].get_text_ascii (), TIMESTAMP_YYYYMMDD);
				}

				// layer_zillow[year_index]->objects.resize (500);
			}
		}
	}

	return !error;
}

bool ZillowSales::load_EColi_outbreaks
	(const dynamic_string &filename_ecoli,
	dynamic_map *map_watershed,
	const int record_limit,
	interface_window *view,
	dynamic_string &log)

// Read 10 shapfiles for 2009..2018

{
	bool error = false;
	importer_shapefile importer;

	view->update_progress (filename_ecoli, 1);

	layer_ecoli = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_ecoli->name = "EColi Outbreaks";
	layer_ecoli->type = MAP_OBJECT_POINT;
	layer_ecoli->color = RGB (255, 0, 0);
	layer_ecoli->symbol = MAP_OBJECT_SYMBOL_PLUS;
	layer_ecoli->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_ecoli->draw_labels_id = false;
	map_watershed->layers.push_back (layer_ecoli);

	importer.filename_source = filename_ecoli;
	importer.has_id = false;
	importer.has_name = true;
	importer.name_field_name = "Site_descr";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = true;

	if (!importer.import (layer_ecoli, NULL, map_watershed, view, log))
		error = true;
	else {
		std::vector <map_object *>::iterator outbreak;

		if (record_limit != -1)
			layer_ecoli->objects.resize (record_limit);

		// Add a timestamp
		layer_ecoli->attributes_void_usage = MAP_LAYER_USAGE_VOID_TIMESTAMP;
		for (outbreak = layer_ecoli->objects.begin ();
		outbreak != layer_ecoli->objects.end ();
		++outbreak) {
			(*outbreak)->attributes_void = new Timestamp [1];
			((Timestamp *) ((*outbreak)->attributes_void))->parse ((*outbreak)->attributes_text[1].get_text_ascii (), TIMESTAMP_YYYYMMDD);
		}
	}

	return !error;
}

bool ZillowSales::nearest_water_to_sale
	(map_layer *layer_rivers,
	map_layer *layer_nhd_lakes,
	interface_window *view,
	dynamic_string &log)

// For each sales record, find nearest river or water body

{
	bool error = false;
	std::vector <map_object *>::iterator house;
	int year_index;

	_timeb start, end;
	float elapsed_milliseconds;
	thread_manager threader;
	std::map<long, thread_work *> jobs;
	thread_nearest_water *job;
	std::map<long, thread_work *>::iterator completed_job;
	thread_manager_statistics stats;
	int house_count = 0;

	_ftime_s (&start);

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1;
	threader.pause_time_minimum_seconds = 0;
	threader.pause_time_maximum_seconds = 1;
	stats.reset(threader.thread_count);
	threader.statistics = &stats;

	for (year_index = 0;
	year_index < ZILLOW_YEAR_COUNT;
	++year_index) {
		if (layer_zillow[year_index]) {
			for (house = layer_zillow[year_index]->objects.begin ();
			house != layer_zillow[year_index]->objects.end ();
			++house) {
				job = new thread_nearest_water;
				job->id = ++house_count;
				job->site = *house;
				job->site_is_zillow_sale = true;
				job->layer_rivers = layer_rivers;
				job->layer_lakes = layer_nhd_lakes;
				job->work_function = &work_find_water;

				job->description.format ("Run %ld", job->id);
				jobs.insert (std::pair<long, thread_work *> (job->id, job));
			}
		}
	}

	log.add_formatted ("\t%d\tpoints\n", (int) jobs.size ());
	log.add_formatted ("\t%d\thouses\n", house_count);

	log += "House ID\tName\tImportParc\tDate\tWater ID\tWater Name\tDistance, m\tArea, km^2\tStream Level\tRiver or Lake\n";

	view->update_progress("Starting threads");
	threader.run_tiny (&jobs, view, log);
	
	view->update_progress ("Adding Logs", 0);

	for (completed_job = jobs.begin ();
	completed_job != jobs.end ();
	++completed_job) {
		log += ((thread_nearest_water *) completed_job->second)->output_table;
		((thread_nearest_water *) completed_job->second)->output_table.clear ();
		log += completed_job->second->log;
		completed_job->second->log.clear ();
	}

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);


	return !error;
}

bool ZillowSales::nearest_outbreak
	(interface_window *view,
	dynamic_string &log)

// For each sales record, find nearest river or water body

{
	bool error = false;
	std::vector <map_object *>::iterator house;
	int year_index;

	_timeb start, end;
	float elapsed_milliseconds;
	thread_manager threader;
	std::map<long, thread_work *> jobs;
	thread_nearest_outbreak *job;
	std::map<long, thread_work *>::iterator completed_job;
	thread_manager_statistics stats;
	int house_count = 0;

	_ftime_s (&start);

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1;
	threader.pause_time_minimum_seconds = 0;
	threader.pause_time_maximum_seconds = 1;
	stats.reset(threader.thread_count);
	threader.statistics = &stats;

	for (year_index = 0;
	year_index < ZILLOW_YEAR_COUNT;
	++year_index) {
		if (layer_zillow[year_index]) {
			for (house = layer_zillow[year_index]->objects.begin ();
			house != layer_zillow[year_index]->objects.end ();
			++house) {
				job = new thread_nearest_outbreak;
				job->id = ++house_count;
				job->house = *house;
				job->layer_ecoli = layer_ecoli;
				job->work_function = &work_find_outbreak;

				job->description.format ("Run %ld", job->id);
				jobs.insert (std::pair<long, thread_work *> (job->id, job));
			}
		}
	}

	log.add_formatted ("\t%d\tpoints\n", (int) jobs.size ());
	log.add_formatted ("\t%d\thouses\n", house_count);

	log += "House ID\tName\tImportParc\tDate\tOutbreak ID\tOutbreak Name\tDate\tInterval, days\tDistance, m\n";

	view->update_progress("Starting threads");
	threader.run_tiny (&jobs, view, log);
	
	view->update_progress ("Adding Logs", 0);

	for (completed_job = jobs.begin ();
	completed_job != jobs.end ();
	++completed_job) {
		log += ((thread_nearest_outbreak *) completed_job->second)->output_table;
		((thread_nearest_outbreak *) completed_job->second)->output_table.clear ();
		log += completed_job->second->log;
		completed_job->second->log.clear ();
	}

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);


	return !error;
}

bool ZillowSales::homes_in_5_miles
	(dynamic_map *map_watershed,
	dynamic_string &filename_output,
	const char &delimiter,
	interface_window *view,
	dynamic_string &log)

// For each outbreak, list home sales within 5 miles

{
	bool error = false;
	std::vector <map_object *>::iterator house;
	int year_index;

	_timeb start, end;
	float elapsed_milliseconds;
	thread_manager threader;
	std::map<long, thread_work *> jobs;
	thread_outbreak_5mile *job;
	std::map<long, thread_work *>::iterator completed_job;
	thread_manager_statistics stats;
	int house_count = 0;

	_ftime_s (&start);

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1;
	threader.pause_time_minimum_seconds = 0;
	threader.pause_time_maximum_seconds = 1;
	stats.reset(threader.thread_count);
	threader.statistics = &stats;

	for (house = layer_ecoli->objects.begin ();
	house != layer_ecoli->objects.end ();
	++house) {
		job = new thread_outbreak_5mile;
		job->id = ++house_count;
		job->outbreak = *house;
		for (year_index = 0;
		year_index < ZILLOW_YEAR_COUNT;
		++year_index)
			job->layer_zillow[year_index] = layer_zillow[year_index];
		job->work_function = &work_outbreak_5mile;
		job->delimiter = delimiter;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair<long, thread_work *> (job->id, job));
	}

	log.add_formatted ("\t%d\tpoints\n", (int) jobs.size ());
	log.add_formatted ("\t%d\thouses\n", house_count);

	view->update_progress("Starting threads");
	threader.run_tiny (&jobs, view, log);
	
	view->update_progress ("Adding Logs", 0);

	FILE *f;

	if ((fopen_s (&f, filename_output.get_text_ascii (), "w")) == 0) {

		fprintf (f, "\"Outbreak ID\"%c\"Outbreak Name\"%c\"Date\"%c\"House ID\"%c\"RowID_\"%c\"ImportParc\"%c\"Sale Date\"%c\"",
		delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
		// 16th numeric field is YearBuilt
		fwrite (layer_zillow [0]->column_names_numeric [15].get_text_ascii (), 1, layer_zillow [0]->column_names_numeric [15].get_length (), f);
		fprintf (f, "\"%c\"Distance, m\"\n", delimiter);
		for (completed_job = jobs.begin ();
		completed_job != jobs.end ();
		++completed_job) {
			fwrite (((thread_outbreak_5mile *) completed_job->second)->output_table.get_text_ascii (), 1, ((thread_outbreak_5mile *) completed_job->second)->output_table.get_length (), f);
			((thread_outbreak_5mile *) completed_job->second)->output_table.clear ();
			log += completed_job->second->log;
			completed_job->second->log.clear ();
		}
		fclose (f);
	}

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);


	return !error;
}

bool ZillowSales::nearest_water_to_outbreak
	(map_layer *layer_rivers,
	map_layer *layer_nhd_lakes,
	interface_window *view,
	dynamic_string &log)

// For each outbreak, find nearest river or water body

{
	bool error = false;
	std::vector <map_object *>::iterator outbreak;

	_timeb start, end;
	float elapsed_milliseconds;
	thread_manager threader;
	std::map<long, thread_work *> jobs;
	thread_nearest_water *job;
	std::map<long, thread_work *>::iterator completed_job;
	thread_manager_statistics stats;
	int house_count = 0;

	_ftime_s (&start);

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1;
	threader.pause_time_minimum_seconds = 0;
	threader.pause_time_maximum_seconds = 1;
	stats.reset(threader.thread_count);
	threader.statistics = &stats;

	for (outbreak = layer_ecoli->objects.begin ();
	outbreak != layer_ecoli->objects.end ();
	++outbreak) {
		job = new thread_nearest_water;
		job->id = ++house_count;
		job->site = *outbreak;
		job->site_is_zillow_sale = false;
		job->layer_rivers = layer_rivers;
		job->layer_lakes = layer_nhd_lakes;
		job->work_function = &work_find_water;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair<long, thread_work *> (job->id, job));
	}

	log.add_formatted ("\t%d\tpoints\n", (int) jobs.size ());
	log.add_formatted ("\t%d\thouses\n", house_count);

	log += "Outbreak ID\tOutbreak Name\tDate\tWater ID\tWater Name\tDistance, m\tArea, km^2\tStream Level\tRiver or Lake\n";

	view->update_progress("Starting threads");
	threader.run_tiny (&jobs, view, log);
	
	view->update_progress ("Adding Logs", 0);

	for (completed_job = jobs.begin ();
	completed_job != jobs.end ();
	++completed_job) {
		log += ((thread_nearest_water *) completed_job->second)->output_table;
		((thread_nearest_water *) completed_job->second)->output_table.clear ();
		log += completed_job->second->log;
		completed_job->second->log.clear ();
	}

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

bool ZillowSales::outbreaks_in_1000m
	(map_layer *layer_rivers,
	map_layer *layer_nhd_lakes,
	dynamic_map *map_watershed,
	dynamic_string &filename_output,
	interface_window *view,
	dynamic_string &log)

// For each sale, list outbreaks in 1000m

{
	bool error = false;
	std::vector <map_object *>::iterator sale, water;
	int year_index;

	_timeb start, end;
	float elapsed_milliseconds;
	thread_manager threader;
	std::map<long, thread_work *> jobs;
	thread_sale_1000m *job;
	std::map<long, thread_work *>::iterator completed_job;
	thread_manager_statistics stats;
	int house_count = 0;

	_ftime_s (&start);

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1;
	threader.pause_time_minimum_seconds = 0;
	threader.pause_time_maximum_seconds = 1;
	stats.reset(threader.thread_count);
	threader.statistics = &stats;

	for (year_index = 0;
	year_index < ZILLOW_YEAR_COUNT;
	++year_index) {
		if (layer_zillow[year_index] != NULL) {
			for (sale = layer_zillow[year_index]->objects.begin ();
			sale != layer_zillow[year_index]->objects.end ();
			++sale) {
				job = new thread_sale_1000m;
				job->id = ++house_count;
				job->house = *sale;
				job->layer_ecoli = layer_ecoli;
				job->layer_rivers = layer_rivers;
				job->layer_lakes = layer_nhd_lakes;
				job->work_function = &work_sale_1000m;

				job->description.format ("Run %ld", job->id);
				jobs.insert (std::pair<long, thread_work *> (job->id, job));
			}
		}
	}

	log.add_formatted ("\t%d\tpoints\n", (int) jobs.size ());
	log.add_formatted ("\t%d\thouses\n", house_count);

	view->update_progress("Starting threads");
	threader.run_tiny (&jobs, view, log);
	
	view->update_progress ("Adding Logs", 0);

	FILE *f;

	if ((fopen_s (&f, filename_output.get_text_ascii (), "w")) == 0) {

		fprintf (f, "House ID\tName\tImportParc\tSale Date\tYear Built\tWater ID\tWater Name\tDistance Sale to Water, m\tArea, km^2\tStream Level\tRiver or Lake\tOutbreak ID\tOutbreak Name\tDate\tDistance Water to Outbreak, m\tInterval, days\tNearest\tMost Recent\n");
		for (completed_job = jobs.begin ();
		completed_job != jobs.end ();
		++completed_job) {
			fwrite (((thread_sale_1000m *) completed_job->second)->output_table.get_text_ascii (), 1, ((thread_sale_1000m *) completed_job->second)->output_table.get_length (), f);
			((thread_sale_1000m *) completed_job->second)->output_table.clear ();
			log += completed_job->second->log;
			completed_job->second->log.clear ();
		}
		fclose (f);
	}

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

bool ZillowSales::read_csv
	(const dynamic_string &filename_zillow_csv,
	const int first_record,
	const int last_record,
	CountyMaster *counties,
	dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// "I:\TDC\LAGOS\Zillow-LAGOS\append_all_10miles.csv" has 3.25 million records

// Read and create map layer

{
	FILE *f;
	// color_set *zillow_colors = make_colors ();
	dynamic_string local_filename = filename_zillow_csv, data;
	char buffer[4096];
	std::vector <dynamic_string> tokens;
	map_object *point;
	int year_index = 0, state_fips, county_fips;
	long point_count = 0;
	Timestamp *sales_date;

	layer_zillow[year_index] = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_zillow[year_index]->name.format ("Zillow %d", year_index + 2009);
	layer_zillow[year_index]->type = MAP_OBJECT_POINT;
	// layer_zillow[year_index]->color = RGB (year_index * 12, year_index * 12, year_index * 12);
	layer_zillow[year_index]->symbol = MAP_OBJECT_SYMBOL_POINT;
	layer_zillow[year_index]->draw_as = MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;
	layer_zillow[year_index]->attribute_count_numeric = 4; // ImportParc,SalesPriceAmount,FIPS, LotSizeSquareFeet
	layer_zillow[year_index]->attribute_count_text = 2; // "SalesDate", "PropertyFullStreetAddress"
	layer_zillow [year_index]->data_attribute_index = 1; // SalesPriceAmount
	layer_zillow[year_index]->attributes_void_usage = MAP_LAYER_USAGE_VOID_TIMESTAMP;
	layer_zillow [year_index]->draw_labels_id = false;
	make_colors (&layer_zillow [year_index]->colors);
	map_watershed->layers.push_back (layer_zillow[year_index]);

	if (fopen_s (&f, local_filename.get_text_ascii (), "r") == 0) {
		// RowID,importparc,lat,lon,TransId,FIPS,State,County,RecordingDate,SalesPriceAmount,BatchID,BKFSPID,PropertyAddressLatitude,PropertyAddressLongitude,ImportParcelID,PropertyFullStreetAddress,PropertyCity,PropertyState,PropertyZip,LotSizeSquareFeet,_merge,group
		// 0000156F-7891-E911-80C7-3863BB43E813,71295852,45.24403,-93.391548,221283366,27003,MN,ANOKA,1996-04-23,132500,1000800350,11236,45.244034,-93.391548,71295852,4940 150TH LN NW,RAMSEY,MN,55303,,matched (3),120181
		// 0000156F-7891-E911-80C7-3863BB43E813,71295852,45.24403,-93.391548,221345268,27003,MN,ANOKA,2003-02-10,202500,1000800350,71417,45.244034,-93.391548,71295852,4940 150TH LN NW,RAMSEY,MN,55303,,matched (3),120181
		// 00001C6F-7891-E911-80C7-3863BB43E813,71227200,45.20512,-93.240608,465164728,27003,MN,ANOKA,2019-06-13,259560,1000800350,225607,45.205124,-93.240608,71227200,12926 BUCHANAN ST NE,BLAINE,MN,55434,,matched (3),120184
		// 00002B6F-7891-E911-80C7-3863BB43E813,71276940,45.15958,-93.248428,408199302,27003,MN,ANOKA,2010-11-08,45000,1000800350,138977,45.159584,-93.248428,71276940,901 104TH LN NE,BLAINE,MN,55434,10800,matched (3),120190
		// 00002B6F-7891-E911-80C7-3863BB43E813,71276940,45.15958,-93.248428,221403744,27003,MN,ANOKA,2010-12-14,45000,1000800350,139545,45.159584,-93.248428,71276940,901 104TH LN NE,BLAINE,MN,55434,10800,matched (3),120190
		// 00002B6F-7891-E911-80C7-3863BB43E813,71276940,45.15958,-93.248428,221407572,27003,MN,ANOKA,2011-09-12,230000,1000800350,143835,45.159584,-93.248428,71276940,901 104TH LN NE,BLAINE,MN,55434,10800,matched (3),120190
		if (fgets (buffer, 4096, f)) {

			while (fgets (buffer, 4096, f)) {
				data = buffer;
				data.tokenize (",", &tokens, true, false);

				if (tokens.size () == 22) {
					++point_count;
					if ((point_count >= first_record)
					&& (point_count <= last_record)) {
						point = layer_zillow[year_index]->create_new (MAP_OBJECT_POINT);
						point->id = point_count;
						point->name = tokens[0];
						point->latitude = (long) (atof (tokens[2].get_text_ascii ()) * 1.0e6);
						point->longitude = (long) (atof (tokens[3].get_text_ascii ()) * 1.0e6);
						point->attributes_numeric[0] = atof (tokens[1].get_text_ascii ()); // ImportParc
						point->attributes_numeric[1] = atof (tokens[9].get_text_ascii ()); // SalesPriceAmount
						if (tokens[8].get_length () > 0) {
							sales_date = new Timestamp;
							sales_date->parse (tokens[8].get_text_ascii (), TIMESTAMP_YYYY_MM_DD); // Recording date
							point->attributes_void = sales_date;
						}
						point->attributes_text[0] = tokens[8]; // Sales Year
						point->attributes_text[1] = tokens[15]; // PropertyFullStreetAddress

						if (tokens[6].get_length () > 0) {
							state_fips = state_FIPS_from_abbreviation (tokens[6].get_text_ascii ()); // State
							if (tokens[7].get_length () > 0)
								county_fips = counties->get_FIPS (state_fips, tokens[7].get_text_ascii ()); // County
							else
								county_fips = -1;

							point->attributes_numeric[2] = (double) (state_fips * 1000);
							if (county_fips == -1) {
								log += "Unknown county \"";
								log += tokens[7];
								log.add_formatted ("\" State %d\n", state_fips);
							}
							else
								point->attributes_numeric[2] += (double) county_fips;
						}

						// LotSizeSquareFeet
						if (tokens[19].get_length () > 0)
							point->attributes_numeric[3] = atof (tokens[19].get_text_ascii ());
						else
							point->attributes_numeric[3] = 0.0;

						layer_zillow[year_index]->objects.push_back (point);
					}
				}
				else {
					log += "ERROR, unable to parse Zillow CSV data \"";
					log += buffer;
					log += "\"\n";
				}
			}
		}

		fclose (f);
		return true;
	}
	else {
		log += "ERROR, can't open Zillow CSV file \"";
		log += filename_zillow_csv;
		log += "\".\n";
		return false;
	}
}

bool ZillowSales::read_csv_state
	(const dynamic_string &filename_zillow_csv,
	CountyMaster *counties,
	dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// "I:\TDC\Pipeline_Accidents\housing_transactions\current_assess_transaction_merge_2.csv"

// Yet another column order

// Read and create map layer

{
	FILE *f;
	// color_set *zillow_colors = make_colors ();
	dynamic_string local_filename = filename_zillow_csv, data;
	char buffer[4096];
	std::vector <dynamic_string> tokens;
	map_object *point;
	int year_index = 0, state_fips, county_fips;
	long point_count = 0;
	Timestamp *sales_date;

	layer_zillow[year_index] = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_zillow[year_index]->name.format ("Zillow %d", year_index + 2009);
	layer_zillow[year_index]->type = MAP_OBJECT_POINT;
	// layer_zillow[year_index]->color = RGB (year_index * 12, year_index * 12, year_index * 12);
	layer_zillow[year_index]->symbol = MAP_OBJECT_SYMBOL_POINT;
	layer_zillow[year_index]->draw_as = MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;
	layer_zillow[year_index]->attribute_count_numeric = 4; // ImportParc,SalesPriceAmount,FIPS, LotSizeSquareFeet
	layer_zillow[year_index]->attribute_count_text = 2; // "SalesDate", "PropertyFullStreetAddress"
	layer_zillow [year_index]->data_attribute_index = 1; // SalesPriceAmount
	layer_zillow[year_index]->attributes_void_usage = MAP_LAYER_USAGE_VOID_TIMESTAMP;
	layer_zillow [year_index]->draw_labels_id = false;
	make_colors (&layer_zillow [year_index]->colors);
	map_watershed->layers.push_back (layer_zillow[year_index]);

	if (fopen_s (&f, local_filename.get_text_ascii (), "r") == 0) {
		// TransId	FIPS	State	County	RecordingDate	SalesPriceAmount	BatchID	BKFSPID	PropertyAddressLatitude	PropertyAddressLongitude	ImportParcelID	RowID	PropertyFullStreetAddress	PropertyCity	PropertyState	PropertyZip	LotSizeSquareFeet
		// 30789371	2013	AK	ALEUTIANS EAST	4/17/2008	0	1000850325	2298			2968085	F747FC4C-569A-E511-80BF-3863BB43AC67					
		// 30788953	2013	AK	ALEUTIANS EAST	9/22/2004	0	1000850325	1880			2968098	3251FC4C-569A-E511-80BF-3863BB43AC67					
		// ...
		// 30939662	2020	AK	ANCHORAGE	2/3/2015	636669	1001245863	359871	61.216877	-149.90524	2973935	EF602D00-0000-0000-07E4-000000000000	540  Coastal Pl	Anchorage	AK	99501	7414
		// 422728584	2020	AK	ANCHORAGE	11/13/2017	11750000	1001245863	440162	61.217457	-149.90016	2974055	67612D00-0000-0000-07E4-000000000000	900 W 5th Ave	Anchorage	AK	99501	16262

		if (fgets (buffer, 4096, f)) {

			while (fgets (buffer, 4096, f)) {
				data = buffer;
				data.tokenize (",", &tokens, true, false);

				if (tokens.size () == 17) {
					++point_count;
					// if ((point_count >= first_record)
					// && (point_count <= last_record))
					point = layer_zillow[year_index]->create_new (MAP_OBJECT_POINT);
					point->id = point_count;
					point->name = tokens[11]; // RowID
					point->latitude = (long) (atof (tokens[2].get_text_ascii ()) * 1.0e6);
					point->longitude = (long) (atof (tokens[3].get_text_ascii ()) * 1.0e6);
					point->attributes_numeric[0] = atof (tokens[1].get_text_ascii ()); // ImportParc
					point->attributes_numeric[1] = atof (tokens[9].get_text_ascii ()); // SalesPriceAmount
					if (tokens[8].get_length () > 0) {
						sales_date = new Timestamp;
						sales_date->parse (tokens[8].get_text_ascii (), TIMESTAMP_YYYY_MM_DD); // Recording date
						point->attributes_void = sales_date;
					}
					point->attributes_text[0] = tokens[8]; // Sales Year
					point->attributes_text[1] = tokens[15]; // PropertyFullStreetAddress

					if (tokens[6].get_length () > 0) {
						state_fips = state_FIPS_from_abbreviation (tokens[6].get_text_ascii ()); // State
						if (tokens[7].get_length () > 0)
							county_fips = counties->get_FIPS (state_fips, tokens[7].get_text_ascii ()); // County
						else
							county_fips = -1;

						point->attributes_numeric[2] = (double) (state_fips * 1000);
						if (county_fips == -1) {
							log += "Unknown county \"";
							log += tokens[7];
							log.add_formatted ("\" State %d\n", state_fips);
						}
						else
							point->attributes_numeric[2] += (double) county_fips;
					}

					// LotSizeSquareFeet
					if (tokens[19].get_length () > 0)
						point->attributes_numeric[3] = atof (tokens[19].get_text_ascii ());
					else
						point->attributes_numeric[3] = 0.0;

					layer_zillow[year_index]->objects.push_back (point);
				}
				else {
					log += "ERROR, unable to parse Zillow CSV data \"";
					log += buffer;
					log += "\"\n";
				}
			}
		}

		fclose (f);
		return true;
	}
	else {
		log += "ERROR, can't open Zillow CSV file \"";
		log += filename_zillow_csv;
		log += "\".\n";
		return false;
	}
}

bool ZillowSales::read_csv_format_3
(const dynamic_string &path_zillow_csv,
const int state_fips,
const int first_record,
const int last_record,
CountyMaster *counties,
dynamic_map *map_watershed,
interface_window *view,
dynamic_string &log)

// "I:\TDC\Pipeline_Accidents\housing_transactions\"

// Read and create map layer

{
	FILE *f;
	// color_set *zillow_colors = make_colors ();
	dynamic_string filename_zillow_csv, data;
	char buffer[4096];
	std::vector <dynamic_string> tokens;
	map_object *point;
	int year_index = 0;
	long point_count = 0;
	Timestamp *sales_date;
	bool skip;

	filename_zillow_csv = path_zillow_csv;
	filename_zillow_csv.add_formatted ("current_assess_transaction_merge_%d.csv", state_fips);

	layer_zillow[year_index] = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_zillow[year_index]->name.format ("Zillow Sales"); // , year_index + 2009);
	layer_zillow[year_index]->type = MAP_OBJECT_POINT;
	// layer_zillow[year_index]->color = RGB (year_index * 12, year_index * 12, year_index * 12);
	layer_zillow[year_index]->symbol = MAP_OBJECT_SYMBOL_POINT;
	layer_zillow[year_index]->draw_as = MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;
	layer_zillow[year_index]->attribute_count_numeric = 2; // ImportParcelID,SalesPriceAmount,
	layer_zillow[year_index]->attribute_count_text = 1; // TransID
	layer_zillow[year_index]->data_attribute_index = 1; // SalesPriceAmount
	layer_zillow[year_index]->attributes_void_usage = MAP_LAYER_USAGE_VOID_TIMESTAMP;
	layer_zillow[year_index]->draw_labels_id = false;
	make_colors (&layer_zillow[year_index]->colors);
	map_watershed->layers.push_back (layer_zillow[year_index]);

	if (file_exists (filename_zillow_csv.get_text_ascii ())) {
		if (fopen_s (&f, filename_zillow_csv.get_text_ascii (), "r") == 0) {
			// TransId	RecordingDate	SalesPriceAmount	BatchID	BKFSPID	latitude	longitude	ImportParcelID	RowID
			// 30887151	2/5/2009	810566	1001245863	306691	61.217278	-149.90594	2973930	EA602D00-0000-0000-07E4-000000000000
			// 30939662	2/3/2015	636669	1001245863	359871	61.216877	-149.90524	2973935	EF602D00-0000-0000-07E4-000000000000
			// 422728584	11/13/2017	11750000	1001245863	440162	61.217457	-149.90016	2974055	67612D00-0000-0000-07E4-000000000000
			// 432165428	4/10/2018	411158	1001245863	442156	61.219051	-149.90062	2974087	87612D00-0000-0000-07E4-000000000000
			if (fgets (buffer, 4096, f)) {

				while (fgets (buffer, 4096, f)) {
					if (buffer[strlen (buffer) - 1] == '\n')
						buffer[strlen (buffer) - 1] = '\0';
					data = buffer;
					data.tokenize (",", &tokens, true, false);

					if (tokens.size () == 9) {
						++point_count;

						skip = false;
						if (point_count >= first_record) {
							if ((last_record != -1)
							&& (point_count > last_record))
								skip = true;
						}
						else
							skip = true;
						if (!skip) {
							point = layer_zillow[year_index]->create_new (MAP_OBJECT_POINT);
							point->id = point_count;
							point->attributes_text[0] = tokens[0].get_text_ascii (); // TransID
							point->name = tokens[8]; // RowID
							point->latitude = (long) (atof (tokens[5].get_text_ascii ()) * 1.0e6);
							point->longitude = (long) (atof (tokens[6].get_text_ascii ()) * 1.0e6);
							point->attributes_numeric[0] = atof (tokens[7].get_text_ascii ()); // ImportParcelID
							point->attributes_numeric[1] = atof (tokens[2].get_text_ascii ()); // SalesPriceAmount
							if (tokens[1].get_length () > 0) {
								sales_date = new Timestamp;
								sales_date->parse (tokens[1].get_text_ascii (), TIMESTAMP_YYYY_MM_DD); // Recording date
								point->attributes_void = sales_date;
							}
							// point->attributes_text[0] = tokens[8]; // Sales Year
							// point->attributes_text[1] = tokens[15]; // PropertyFullStreetAddress
							layer_zillow[year_index]->objects.push_back (point);
						}
					}
					else {
						log += "ERROR, unable to parse Zillow CSV data \"";
						log += buffer;
						log += "\"\n";
					}
				}
			}

			fclose (f);
			return true;
		}
		else {
			log += "ERROR, can't open Zillow CSV file \"";
			log += filename_zillow_csv;
			log += "\".\n";
			return false;
		}
	}
	else
		return true;
}


