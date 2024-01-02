#include "../util/utility_afx.h"
#include <vector>
#include <mutex>
#include <set>
#include <deque>
#include <map>
#include <list>
#include <thread>
#include <future> // Requires "-std=c++11" option for g++
#include <sys/timeb.h>
#include "../util/dynamic_string.h"
#include "../util/Timestamp.h"
#include "../util/message_slot.h"
#include "../util/interface_window.h"
#include "../util/interface_window_wx.h"
#include "../util/dialog_error_list.h"
#include "../util/device_coordinate.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "PolygonMatch.h"
#include "../clipper_ver6.4.2/cpp/clipper.hpp"
#include "ClipperBuffer.h"
#include "../subbasin_effect/thread_manager.h"

const double CENTROID_NEARNESS_M = 25000.0;

class thread_match_polygon : public thread_work {
	public:
		class map_object *start_object;
		class map_layer *target_layer;
		char output_delimiter;
		FILE *output_file;
		dynamic_string output_table;
		int huc12_attribute_index;
};

double overlap_area
	(ClipperBuffer *clipper,
	ClipperLib::Paths *swat_paths,
	map_object *p2,
	dynamic_string &log)

// Produce a number that rates the area overlap between two bounding polygons

{
	ClipperLib::Paths result;
	std::vector <long long> p1_ids;
	int count_contributing = 0;
	double total_area_m2 = 0;
	std::vector < ClipperLib::Path >::const_iterator paths_path;

	clipper->tract_id_source_1 = CLIPPED_ID_SOURCE_ID;
	clipper->cut_and_accumulate (p2, swat_paths, &result, &p1_ids, &count_contributing,
	p2->id,
	false, // ((*sale)->id == 11359),
	log);

	for (paths_path = result.begin ();
	paths_path != result.end ();
	++paths_path)
		total_area_m2 += std::abs (area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT));

	return total_area_m2;
}

PolygonMatch::PolygonMatch ()

{
	output_delimiter = ',';
	thread_count = 8;
	layer_1 = layer_2 = NULL;
	huc12_attribute_index = 0;
}


map_object *find_match
	(map_object *swat_huc,
	map_layer *layer_2,
	double *nearest_distance,
	double *highest_area,
	double *swat_area_m2,
	dynamic_string &log)

{
	std::vector <map_object *>::const_iterator huc12;
	device_coordinate centroid;
	int nearest_index, nearest_polygon_index;
	double distance_m, matching_area;
	map_object *found = NULL;
	ClipperBuffer clipper;
	ClipperLib::Paths swat_paths;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	bounding_cube swat_huc_area;

	swat_huc->centroid (&centroid.y, &centroid.x);
	swat_huc->check_extent (&swat_huc_area);

	clipper.map_object_to_Path (swat_huc, &swat_paths, log);

	*swat_area_m2 = 0.0;
	for (paths_path = swat_paths.begin ();
	paths_path != swat_paths.end ();
	++paths_path)
		*swat_area_m2 += std::abs (area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT));

	*highest_area = -DBL_MAX;
	*nearest_distance = DBL_MAX;
	found = NULL;

	for (huc12 = layer_2->objects.begin ();
	huc12 != layer_2->objects.end ();
	++huc12) {

		if ((*huc12)->overlaps_logical (&swat_huc_area)) {
			if ((*huc12)->type == MAP_OBJECT_POLYGON_COMPLEX)
				// 2020-11-12 some NHD lakes are complex
				distance_m = ((map_polygon_complex *) (*huc12))->closest_point_meters (centroid, &nearest_polygon_index, &nearest_index);
			else
				distance_m = (*huc12)->closest_point_meters (centroid, &nearest_index);

			if (distance_m < CENTROID_NEARNESS_M) {
				matching_area = overlap_area (&clipper, &swat_paths, *huc12, log);
				if (matching_area > *highest_area) {
					*highest_area = matching_area;
					*nearest_distance = distance_m;
					found = *huc12;
				}
			}
		}
	}
	return found;
}

bool PolygonMatch::run
	(dynamic_string &filename_output,
	dynamic_string &log)

{
	// Match SWAT run hucs to HUC12 hucs
	std::vector <map_object *>::iterator swat_huc;
	std::vector <map_object *>::const_iterator huc12;
	device_coordinate centroid;
	bool error = false;
	double nearest_distance, highest_area, swat_area_m2;
	map_object *found = NULL;
	ClipperBuffer clipper;
	ClipperLib::Paths swat_paths;

	FILE *output_file;

	if (fopen_s (&output_file, filename_output.get_text_ascii (), "w") == 0) {
		fprintf (output_file, "SWAT HUC\tHUC12\tCenter Distance\tArea Overlap\n");

		for (swat_huc = layer_1->objects.begin ();
		swat_huc != layer_1->objects.end ();
		++swat_huc) {

			found = find_match (*swat_huc, layer_2, &nearest_distance, &highest_area, &swat_area_m2, log);

			if (found)
				fprintf (output_file, "%lld\t%lld\t%.6lf\t%.6lf\n", (*swat_huc)->id, found->id, nearest_distance, highest_area);
			else
				fprintf (output_file, "%lld\tNA\n", (*swat_huc)->id);

		}
		fclose (output_file);
	}

	return !error;
}

bool work_match_polygon
	(thread_work *base_job)

{
	thread_match_polygon *job = (thread_match_polygon *) base_job;
	dynamic_string output_line;
	map_object *found;
	double nearest_distance, highest_area, swat_area_m2;

    job->start = std::chrono::system_clock::now();

	found = find_match (job->start_object, job->target_layer, &nearest_distance, &highest_area, &swat_area_m2, job->log);

	if (found) {
		output_line.format ("%lld", job->start_object->id);

		output_line += job->output_delimiter;
		output_line += found->attributes_text[job->huc12_attribute_index]; // TOHUC

		output_line.add_formatted ("%c%.6lf%c%.6lf%c%.6lf",
		job->output_delimiter, nearest_distance, job->output_delimiter, highest_area, job->output_delimiter, swat_area_m2);

		output_line.add_formatted ("%c%.6lf", job->output_delimiter, (swat_area_m2 - highest_area) / swat_area_m2);

		output_line += "\n";

		if (job->output_file)
			fwrite (output_line.get_text_ascii (), 1, output_line.get_length (), job->output_file);
		else
			job->output_table += output_line;
	}
	else {
		output_line.format ("%lld", job->start_object->id);
		output_line.add_formatted ("%c%c%c%c%.6lf", job->output_delimiter, job->output_delimiter, job->output_delimiter, job->output_delimiter, swat_area_m2);
		output_line.add_formatted ("%c1.0", job->output_delimiter);
		output_line += "\n";

		if (job->output_file)
			fwrite (output_line.get_text_ascii (), 1, output_line.get_length (), job->output_file);
		else
			job->output_table += output_line;
	}
	base_job->status = THREAD_WORK_STATUS_COMPLETE;

	job->end = std::chrono::system_clock::now();
	return job->status == THREAD_WORK_STATUS_COMPLETE;
}

bool PolygonMatch::run_threaded
	(dynamic_string &filename_output,
	interface_window *view,
	dynamic_string &log)

{
	thread_manager threader;
	thread_manager_statistics stats;
	std::vector <map_object *>::iterator swat_huc;
	std::map<long, thread_work *> jobs;
	thread_match_polygon *job;
	int job_count, limit = 1000;
	bool error = false;

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1;
	threader.pause_time_minimum_seconds = 0;
	threader.pause_time_maximum_seconds = 1;
	stats.reset(threader.thread_count);
	threader.statistics = &stats;

	FILE *output_file;

	if (fopen_s (&output_file, filename_output.get_text_ascii (), "w") == 0) {
		fprintf (output_file, "SWAT HUC%cHUC12%cCenter Distance%cArea Overlap%cArea%cFraction Difference\n", output_delimiter, output_delimiter, output_delimiter, output_delimiter, output_delimiter);

		for (swat_huc = layer_1->objects.begin (), job_count = 0;
		swat_huc != layer_1->objects.end ();
		// && (job_count < limit);
		++swat_huc, ++job_count) {
			job = new thread_match_polygon;
			job->id = (*swat_huc)->id;
			job->start_object = *swat_huc;
			job->target_layer = layer_2;
			job->work_function = &work_match_polygon;
			job->output_delimiter = output_delimiter;
			job->output_file = output_file;
			job->huc12_attribute_index = huc12_attribute_index;

			job->description.format ("Run %ld", job->id);
			jobs.insert (std::pair<long, thread_work *> (job->id, job));
		}

		view->update_progress ("Starting threads");
		threader.run_tiny (&jobs, view, log);

		fclose (output_file);
	}
	else {
		error = true;
		log += "ERROR, can't open output file \"";
		log += filename_output;
		log += "\".\n";
	}

	return !error;
}