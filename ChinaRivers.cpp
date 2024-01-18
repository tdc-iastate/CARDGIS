#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <thread>
#include <future>
#include <stack>
#include <chrono>
#include <wx/frame.h>
#include <wx/notebook.h>
#include <wx/filename.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/dialog.h>
// #include <wx/log.h>
#include <wx/app.h>
#include <wx/combobox.h>
#include <wx/listbox.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/stopwatch.h>
#include <wx/sizer.h>
#include <sys/timeb.h>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/listbox_files.h"
#include "../util/interface_window_wx.h"
#include "../util/device_coordinate.h"
#include "../util/dialog_error_list.h"
#include "../util/dialog_prompt.h"
#include "../util/card_bitmap.h"
#include "../util/custom.h"
#include "../util/RecentlyOpened.h"
#include "../util/message_slot.h"
#include "../util/xml_parser.h"
#include "../util/arbitrary_counter.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/dynamic_map_gl2.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/shapefile_filter.h"
// wx setup.h must appear before odbc_inst.h or error with HAVE_UNISTD_H
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_database_mysql.h"
#include "../odbc/odbc_database_sql_server.h"
#include "../odbc/odbc_field_set.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "color_set_histogram.h"
#include "../cardgis_console/string_grid.h"
#include "../cardgis_console/router_NHD.h"
#include "../subbasin_effect/thread_manager.h"
#include "ChinaRivers.h"

const long POSITION_TOLERANCE = 4; // sum of x & y differences

#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/graph/graphviz.hpp>
#include "astar_router.h"

bool work_ChinaRouting
	(thread_work *job_ptr)

{
	std::vector <map_object *>::const_iterator county;
	thread_work_ChinaRouting *job = (thread_work_ChinaRouting *) job_ptr;

	/*
	job->router.end_object = job->layer_counties->match_id (1390);
	if (job->router.route (job->layer_rivers, job->log))
		job->router.write_route_line (job->log);
	*/
	for (county = job->layer_counties->objects.begin ();
	county != job->layer_counties->objects.end ();
	++county) {
		job->router.end_object = *county;
		if (job->router.route_directional (job->layer_rivers, job->river_layer_index, job->log))
			job->router.write_route_line (job->log);
	}

	job->status = THREAD_WORK_STATUS_COMPLETE;
	return true;
}

ChinaRouter::ChinaRouter ()

{
	start_object = end_object = NULL;
	start_point_index = end_point_index = -1;
	distance_start_m = distance_m = distance_end_m = elevation_start = elevation_end = 0.0;
	upstream = false;
}

ChinaLinkPoint::ChinaLinkPoint ()

{
	id = -1;
}

ChinaLinkPoint::ChinaLinkPoint
	(const ChinaLinkPoint &other)

{
	copy (other);
}

ChinaLinkPoint ChinaLinkPoint::operator =
	(const ChinaLinkPoint &other)
{
	if (this != &other)
		copy(other);

	return (*this);
}

void ChinaLinkPoint::copy
	(const ChinaLinkPoint &other)
{
	id = other.id;
	sources = other.sources;
	sinks = other.sinks;
}

void ChinaRivers::add_link_points
	(map_object *segment,
	map_layer *layer_points,
	const int level_number,
	long *new_point_id,
	dynamic_string &log)

{
	device_coordinate position_1, position_2;
	map_object *point, *old_point;
	long point_id, distance;

	// FNODE_
	if ((point_id = (long) segment->attributes_numeric[CHINA_ROUTER_INDEX_FNODE]) > 0.0) {
		position_1 = segment->first_point ();
		old_point = layer_points->find_nearest_object_fast (position_1, &distance);
		if (distance == 0)
			old_point->attributes_text[0].add_formatted ("\t%d:%ld %ldF", level_number, point_id, segment->id);
		else {
			point = layer_points->create_new (MAP_OBJECT_POINT);
			point->id = *new_point_id;
			// point->attributes_numeric[level_number - 1] = (double) segment->id;
			point->attributes_text[0].add_formatted ("\t%d:%ld %ldF", level_number, point_id, segment->id);
			position_1 = segment->first_point ();
			point->longitude = position_1.x;
			point->latitude = position_1.y;
			layer_points->objects.push_back (point);
			*new_point_id += 1;
		}
	}
	// TNODE_
	if ((point_id = (long) segment->attributes_numeric[CHINA_ROUTER_INDEX_TNODE]) > 0.0) {
		position_2 = segment->last_point ();

		old_point = layer_points->find_nearest_object_fast (position_2, &distance);
		if (distance == 0)
			old_point->attributes_text[0].add_formatted ("\t%d:%ld %ldT", level_number, point_id, segment->id);
		else {
			point = layer_points->create_new (MAP_OBJECT_POINT);
			point->id = *new_point_id;
			point->attributes_text[0].add_formatted ("\t%d:%ld %ldT", level_number, point_id, segment->id);
			position_1 = segment->first_point ();
			point->longitude = position_2.x;
			point->latitude = position_2.y;
			layer_points->objects.push_back (point);
			*new_point_id += 1;
		}
	}

}

void ChinaRivers::check_node_order
	(map_object *copy,
	map_layer *layer_points,
	const int level_number,
	long *new_point_id,
	dynamic_string &log)

// Find link points by location, use elevation to check From/To and node order

{
	device_coordinate position_1, position_2;
	map_object *point;
	double start_elevation = DBL_MAX, end_elevation = DBL_MAX;
	int index;
	long point_id;

	if ((point_id = copy->attributes_numeric[CHINA_ROUTER_INDEX_FNODE]) != NULL) {
		point = layer_points->match_id (point_id);
		start_elevation = point->attributes_numeric[CHINA_LINK_POINT_ELEVATION];
		copy->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_START] = start_elevation;
	}

	if ((point_id = copy->attributes_numeric[CHINA_ROUTER_INDEX_TNODE]) != NULL) {
		point = layer_points->match_id (point_id);
		end_elevation = point->attributes_numeric[CHINA_LINK_POINT_ELEVATION];
		copy->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_END] = end_elevation;
	}

	/*
	// Average elevation of source points
	for (index = 0; index < (int) copy->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT]; ++index) {
		point_id = ((long *) copy->attributes_void) [index];
		point = layer_points->match_id (point_id);
		start_elevation += point->attributes_numeric[0];
	}
	if (copy->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] > 0.0) {
		start_elevation /= copy->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT];
		copy->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_START] = start_elevation;
	}

	// Average elevation of sink points
	while (index < (int) (copy->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] + copy->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT])) {
		point_id = ((long *) copy->attributes_void) [index];
		point = layer_points->match_id (point_id);
		end_elevation += point->attributes_numeric[0];
		++index;
	}
	if (copy->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT] > 0.0) {
		end_elevation /= copy->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT];
		copy->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_END] = end_elevation;
	}
	*/

	if ((start_elevation < end_elevation)
	&& (start_elevation != DBL_MAX)
	&& (end_elevation != DBL_MAX)) {
		std::map <long, ChinaLinkPoint>::iterator new_from, new_to;
		long temp_point_link_id;
		// point1 -> point2 is flowing uphill
		copy->reverse_nodes ();

		// Swap Point link IDs
		temp_point_link_id = (long) copy->attributes_numeric [CHINA_ROUTER_INDEX_FNODE];
		copy->attributes_numeric [CHINA_ROUTER_INDEX_FNODE] = copy->attributes_numeric [CHINA_ROUTER_INDEX_TNODE];
		copy->attributes_numeric [CHINA_ROUTER_INDEX_TNODE] = (double) temp_point_link_id;

		// all point_links that were sources now become sinks
		for (index = 0; index < (int) copy->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT]; ++index) {
			point_id = ((long *) copy->attributes_void)[index];
			new_to = point_links.find (point_id);
			new_to->second.remove_sink (copy->id);
			new_to->second.add_source (copy->id);
		}

		// all point_links that were sinks now become sources
		while (index < (int) (copy->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] + copy->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT])) {
			point_id = ((long *) copy->attributes_void)[index];
			new_from = point_links.find (point_id);
			new_from->second.remove_source (copy->id);
			new_from->second.add_sink (copy->id);
			++index;
		}

		// Swap Elevations
		copy->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_START] = end_elevation;
		copy->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_END] = start_elevation;

		log.add_formatted ("Reversing segment %ld level %d.\n", copy->id, level_number);
	}
}

void ChinaRivers::set_point_links
	(map_object *segment,
	map_object *copy,
	map_layer *layer_points,
	const int level_number,
	long *new_point_id,
	dynamic_string &log)

// Find link points by location, use elevation to check From/To and node order

{
	device_coordinate position_1, position_2;
	map_object *point_1 = NULL, *point_2 = NULL;
	long fnode_id, tnode_id, distance;

	std::map <long, ChinaLinkPoint>::iterator link_point;

	// FNODE_
	if ((fnode_id = (long) segment->attributes_numeric[0]) > 0.0) {
		position_1 = segment->first_point ();
		point_1 = layer_points->find_nearest_object_fast (position_1, &distance);
		if (distance != 0)
			point_1 = NULL;
	}
	// TNODE_
	if ((tnode_id = (long) segment->attributes_numeric[1]) > 0.0) {
		position_2 = segment->last_point ();

		point_2 = layer_points->find_nearest_object_fast (position_2, &distance);
		if (distance != 0)
			point_2 = NULL;
	}

	if (point_1 && point_2) {
		link_point = point_links.find (point_1->id);
		link_point->second.add_sink (level_number * 100000 + segment->id);
		link_point = point_links.find (point_2->id);
		link_point->second.add_source (level_number * 100000 + segment->id);
	}
	else
		if (point_1) {
			link_point = point_links.find (point_1->id);
			link_point->second.add_sink (level_number * 100000 + segment->id);
		}
		else
			if (point_2) {
				link_point = point_links.find (point_2->id);
				link_point->second.add_source (level_number * 100000 + segment->id);
			}
}

map_layer *ChinaRivers::create_point_layer
	(dynamic_map *map_watershed)

{
	map_layer *layer_points;
	layer_points = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_points->symbol = MAP_OBJECT_SYMBOL_SQUARE;
	layer_points->initialize_attributes = true; // clear all attributes as polygons are created
	layer_points->attribute_count_text = 1;
	layer_points->name = "Link Points";
	layer_points->color = RGB (0, 0, 0);
	layer_points->column_names_text.push_back ("Sources");
	map_watershed->layers.push_back (layer_points);

	return layer_points;
}

map_layer *ChinaRivers::import_point_layer
	(const dynamic_string &filename,
	dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

{
	map_layer *layer_points;
	importer_shapefile importer;
	std::chrono::system_clock::time_point time_after_shp_base;

	layer_points = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;
	layer_points->data_attribute_index = 0;
	layer_points->symbol = MAP_OBJECT_SYMBOL_SQUARE;
	layer_points->initialize_attributes = true; // clear all attributes as polygons are created
	layer_points->attribute_count_numeric = 4; // Elevation, CHINA_LINK_POINT_SOURCE_COUNT, CHINA_LINK_POINT_SINKS_COUNT
	layer_points->attribute_count_text = 1;
	layer_points->attributes_void_usage = MAP_LAYER_USAGE_VOID_SOURCES_SINKS;
	layer_points->name = "Link Points";
	layer_points->color = RGB (0, 0, 0);
	// layer_points->column_names_numeric.push_back ("Elevation");
	// layer_points->column_names_text.push_back ("Sources");
	map_watershed->layers.push_back(layer_points);

	importer.filename_source = filename;
	importer.id_field_name = "ID";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = true;

	if (importer.import (layer_points, NULL, map_watershed, view, log, &time_after_shp_base)) {
		layer_points->resize (4, 1);
		map_watershed->autorange (layer_points, 10, false, false, log);
		return layer_points;
	}
	else
		return NULL;
}

map_layer *ChinaRivers::build_point_layer
	(dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// Read 5 (6) layers for first time, build layer of end points

{
	map_layer *layer_level, *layer_points;
	std::vector <dynamic_string> five_levels;
	std::vector <dynamic_string>::iterator level;
	COLORREF colors[6];
	std::vector <map_object *>::iterator segment;
	bool error = false;
	long point_id;
	int level_number;
	importer_shapefile importer;
	std::chrono::system_clock::time_point time_after_shp_base, time_after_shp;

	five_levels.push_back ("hyd1_4l.shp");
	five_levels.push_back ("hyd1_4p.shp");
	five_levels.push_back ("hyd2_4l.shp");
	five_levels.push_back ("hyd2_4p.shp");
	five_levels.push_back ("River4_polyline.shp");
	five_levels.push_back ("River5_polyline.shp");

	// Default color for rivers is green
	colors[0] = RGB (255, 0, 0);
	colors[1] = RGB (255, 128, 0);
	colors[2] = RGB (0, 0, 255);
	colors[3] = RGB (255, 0, 255);
	colors[4] = RGB (128, 128, 128);
	colors[5] = RGB (0, 255, 255);

	layer_points = create_point_layer (map_watershed);

	point_id = 1;

	for (level = five_levels.begin (), level_number = 1;
	level != five_levels.end ();
	++level, ++level_number) {
		layer_level = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_level->enclosure = MAP_POLYGON_LAST_OVERLAPS_FIRST; // Don't draw line from first node to last
		layer_level->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_level->initialize_attributes = true; // clear all attributes as polygons are created
		layer_level->name = *level;
		layer_level->color = colors[level_number - 1];
		map_watershed->layers.push_back(layer_level);

		importer.filename_source = filename_china_root;
		importer.filename_source += *level;
		importer.name_field_name = "NAME";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		if (!importer.import (layer_level, NULL, map_watershed, view, log, &time_after_shp_base))
			error = true;

		if (level_number == 6) {
			/*
			map_polygon *object;
			object = (map_polygon *) layer_level->match_id (671);
			object->reverse_nodes ();
			object = (map_polygon *) layer_level->match_id (704);
			object->reverse_nodes ();
			object = (map_polygon *) layer_level->match_id (761);
			object->reverse_nodes ();
			*/
		}

		for (segment = layer_level->objects.begin ();
		segment != layer_level->objects.end ();
		++segment)
			add_link_points (*segment, layer_points, level_number, &point_id, log);
	}
	log.add_formatted ("Point layer size : %d\n", layer_points->objects.size ());

	return layer_points;
}

void ChinaLinkPoint::add_sink
	(const long segment)

// point drains to segment
// original_point_id

{
	sinks.push_back (segment);
}

void ChinaLinkPoint::add_source
	(const long segment)

// segment drains to point

{
	sources.push_back (segment);
}

void ChinaLinkPoint::remove_sink
	(const long segment)

{
	std::vector <long>::iterator s;
	std::vector <long> temp;

	for (s = sinks.begin ();
	s != sinks.end ();
	++s)
		if (*s != segment)
			temp.push_back (*s);
	sinks = temp;
}

void ChinaLinkPoint::remove_source
	(const long segment)

{
	std::vector <long>::iterator s;
	std::vector <long> temp;

	for (s = sources.begin ();
	s != sources.end ();
	++s)
		if (*s != segment)
			temp.push_back (*s);
	sources = temp;
}

map_layer *ChinaRivers::import_point_layer
	(dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// Read 5 (6) layers for second time, use layer of end points (now with Z elevation data)
// to correct From/To node order

{
	map_layer *layer_points;
	// bool error = false;
	std::vector<map_object *>::const_iterator highway;
	ChinaLinkPoint link;

	// layer_points = crivers.import_point_layer ("H:\\scratch\\China Rivers\\LinkPoints_GCS_Beijing_1954.shp", map_watershed, &view, log);
	layer_points = import_point_layer ("H:\\scratch\\China Rivers\\Link Points.shp", map_watershed, view, log);

	/*
	if (false) {
		// Find duplicate point near 5402
		long distance, closest_distance;
		map_object *pt5402, *found;
		device_coordinate pt5402_position;
		std::vector<map_object *>::const_iterator highway;

		pt5402 = layer_points->match_id (5402);
		pt5402_position.x = pt5402->longitude;
		pt5402_position.y = pt5402->latitude;

		closest_distance = 99999;

		for(highway = layer_points->objects.begin();
		highway != layer_points->objects.end();
		++highway) {
			if ((*highway)->id != 5402) {
				distance = (*highway)->closest_distance (pt5402_position);
				if (distance < closest_distance) {
					closest_distance = distance;
					found = *highway;
				}
			}
		}
		log.add_formatted ("5402 match %ld\t%ld\t%ld\n", found->id, found->latitude, found->longitude);
	}
	*/

	log.add_formatted ("Point layer size : %d\n", layer_points->objects.size ());

	return layer_points;
}

map_layer *ChinaRivers::read_counties
	(const dynamic_string &filename,
	dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// Polygons of Chinese county outlines

{
	map_layer *layer_counties;
	bool error = false;
	importer_shapefile importer;
	std::chrono::system_clock::time_point time_after_shp_base;

	layer_counties = map_watershed->create_new (MAP_OBJECT_POLYGON);
	layer_counties->enclosure = MAP_POLYGON_LAST_OVERLAPS_FIRST; // Don't draw line from first node to last
	layer_counties->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_counties->initialize_attributes = true; // clear all attributes as polygons are created
	layer_counties->name = "Counties";
	layer_counties->color = RGB (192, 192, 192);
	map_watershed->layers.push_back(layer_counties);

	importer.filename_source = filename;
	importer.id_field_name = "BOUNT_ID";
	importer.name_field_name = "NAME99";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = true;

	if (!importer.import (layer_counties, NULL, map_watershed, view, log, &time_after_shp_base))
		error = true;
	else
		log.add_formatted ("County layer size : %d\n", layer_counties->objects.size ());

	if (!error)
		return layer_counties;
	else
		return NULL;
}

bool ChinaRivers::read_5_levels
	(dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// Read 5 (6) layers for second time, use layer of end points (now with Z elevation data)
// to correct From/To node order

{
	map_layer *layer_level;
	std::vector <dynamic_string> five_levels;
	std::vector <dynamic_string>::iterator level;
	COLORREF colors[6];
	std::vector <map_object *>::iterator segment;
	bool error = false;
	long point_id;
	int level_number;
	importer_shapefile importer;
	std::chrono::system_clock::time_point time_after_shp_base, time_after_shp;

	five_levels.push_back ("hyd1_4l.shp");
	five_levels.push_back ("hyd1_4p.shp");
	five_levels.push_back ("hyd2_4l.shp");
	five_levels.push_back ("hyd2_4p.shp");
	five_levels.push_back ("River4_polyline.shp");
	five_levels.push_back ("River5_polyline.shp");

	// Default color for rivers is green
	colors[0] = RGB (255, 0, 0);
	colors[1] = RGB (255, 128, 0);
	colors[2] = RGB (0, 0, 255);
	colors[3] = RGB (255, 0, 255);
	colors[4] = RGB (128, 128, 128);
	colors[5] = RGB (0, 255, 255);

	point_id = 1;

	for (level = five_levels.begin (), level_number = 1;
	level != five_levels.end ();
	++level, ++level_number) {

		layer_level = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_level->enclosure = MAP_POLYGON_LAST_OVERLAPS_FIRST; // Don't draw line from first node to last
		layer_level->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_level->initialize_attributes = true; // clear all attributes as polygons are created
		layer_level->name = *level;
		layer_level->color = colors[level_number - 1];
		map_watershed->layers.push_back(layer_level);

		importer.filename_source = filename_china_root;
		importer.filename_source += *level;
		importer.name_field_name = "NAME";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		if (!importer.import (layer_level, NULL, map_watershed, view, log, &time_after_shp_base))
			error = true;
		else
			log.add_formatted ("Level %d layer size : %d\n", level_number, layer_level->objects.size ());
	}

	return !error;
}

bool ChinaRivers::consolidate_points
	(dynamic_map *map_watershed,
	map_layer *layer_points,
	dynamic_string &log)

// eliminate duplicate points
{
	std::vector <map_object *>::iterator point1, point2;
	std::map <long, ChinaLinkPoint>::iterator link_point_1, link_point_2;
	std::vector <std::pair <long, long>>::iterator segments;
	std::map <long, long> targets;
	std::map <long, long>::iterator victim;
	ChinaLinkPoint link;
	long diff_x, diff_y;
	bool error = false;

	log.add_formatted ("Point layer size\t%d\n", layer_points->objects.size ());
	log.add_formatted ("Position tolerance (x + y)\t+/-%d\n", POSITION_TOLERANCE);

	for (point1 = layer_points->objects.begin ();
	point1 != layer_points->objects.end ();
	++point1)
		for (point2 = layer_points->objects.begin ();
		point2 != layer_points->objects.end ();
		++point2) {
			if (*point1 != *point2) {
				diff_x = abs ((*point2)->longitude - (*point1)->longitude);
				diff_y = abs ((*point2)->latitude - (*point1)->latitude);
				if ((diff_x + diff_y) <= POSITION_TOLERANCE) {

					// point2 will be subsumed into point1
					// unless we've already chosen to do the inverse
					if ((victim = targets.find ((*point1)->id)) != targets.end ()) {
						if (victim->second != (*point2)->id)
							// point1 is already destined to be subsumed to a 3rd point
							targets.insert (std::pair <long, long> ((*point2)->id, victim->second));
					}
					else
						if (targets.find ((*point2)->id) == targets.end ())
							targets.insert (std::pair <long, long> ((*point2)->id, (*point1)->id));
				}
			}
		}

	// for all targets, change segment FROM_ and TO_ fields 
	/*
	{
		std::map <long, long>::iterator ghost;
		std::vector <map_object *>::iterator segment;
		map_layer *layer_level, *first_layer_level = map_watershed->match ("hyd1_4l.shp");
		int level_number;
		long from, to;

		layer_level = first_layer_level;
		for (level_number = 1;
		level_number <= 6;
		++level_number) {
			for (segment = layer_level->objects.begin ();
			segment != layer_level->objects.end ();
			++segment) {
				from = (long) (*segment)->attributes_numeric[0];
				if ((ghost = targets.find (from)) != targets.end ())
					(*segment)->attributes_numeric[0] = ghost->second;

				to = (long) (*segment)->attributes_numeric[1];
				if ((ghost = targets.find (to)) != targets.end ())
					(*segment)->attributes_numeric[1] = ghost->second;
			}

		}
	}
	*/

	// Purge targets
	purge_targets (layer_points, &targets);

	// Create link_points
	for (point1 = layer_points->objects.begin ();
	point1 != layer_points->objects.end ();
	++point1) {
		link.id = (*point1)->id;
		point_links.insert (std::pair <long, ChinaLinkPoint> ((*point1)->id, link));
	}

	log.add_formatted ("Point layer size after consolidation\t%d\n", layer_points->objects.size ());

	return !error;
}


bool ChinaRivers::copy_to_rivers
	(dynamic_map *map_watershed,
	map_layer *layer_rivers,
	map_layer *layer_points,
	interface_window *view,
	dynamic_string &log)

// Uses attributes_void of river segments to hold sink and source point ids

{
	std::vector <map_layer *>::iterator layer, layer_level;
	std::vector <map_object *>::iterator segment;
	bool error = false;
	long point_id;
	int level_number;
	importer_shapefile importer;
	std::chrono::system_clock::time_point time_after_shp_base, time_after_shp;

	point_id = 1;

	// Set the iterator to the first layer level
	for (layer = map_watershed->layers.begin(), layer_level = map_watershed->layers.end ();
	(layer != map_watershed->layers.end ())
	&& (layer_level == map_watershed->layers.end());
	++layer) {
		if ((*layer)->name == "hyd1_4l.shp")
			layer_level = layer;
	}

	for (level_number = 1;
	level_number <= 6;
	++level_number) {

		// copy map_objects from layer_levels to layer_Rivers
		map_object *copy;
		for (segment = (*layer_level)->objects.begin ();
		segment != (*layer_level)->objects.end ();
		++segment) {

			copy = layer_rivers->create_new ((*segment)->type);
			copy->copy_without_attributes (*segment);
			copy->id = level_number * 100000 + (*segment)->id;

			// MAP_LAYER_USAGE_VOID_SINKS_SOURCES
			// attach_segments_to_points uses attributes_void for POINT ids
			copy->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] = copy->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT] = 0.0;

			// 2019-11-19 HYD2_4L is most finely segmented
			// Skip HYD1_4L and the two 'P' layers
			if ((level_number == 3) || (level_number > 4))
				layer_rivers->objects.push_back (copy);

			attach_segment_to_points (copy, layer_points, log);
			check_node_order (copy, layer_points, level_number, &point_id, log);
			// set_point_links (*segment, copy, layer_points, level_number, &point_id, log);

			/*
			// Clear attributes_void, that will now be used for segment sink and source
			delete[] copy->attributes_void;
			copy->attributes_void = NULL;
			copy->attributes_numeric[0] = copy->attributes_numeric[1] = 0.0;
			*/

			if ((level_number != 3) && (level_number <= 4))
				delete copy;
			else
				copy->attributes_numeric[CHINA_ROUTER_INDEX_LENGTH_KM] = copy->length_km ();
		}
		++layer_level;
	}
	log.add_formatted ("CHD layer size : %d\n", layer_rivers->objects.size ());

	return !error;
}

void ChinaRivers::attach_segment_to_points 
	(map_object *segment,
	map_layer *layer_points,
	dynamic_string &log)

{
	std::vector <map_object *>::iterator point;
	long diff_x, diff_y;
	device_coordinate start_position, end_position;
	std::map <long, ChinaLinkPoint>::iterator link_point;

	start_position = segment->first_point ();
	end_position = segment->last_point ();

	for (point = layer_points->objects.begin ();
	point != layer_points->objects.end ();
	++point) {
		diff_x = abs (start_position.x - (*point)->longitude);
		diff_y = abs (start_position.y - (*point)->latitude);
		if ((diff_x + diff_y) <= POSITION_TOLERANCE) {
			link_point = point_links.find ((*point)->id);
			// Segment is sink to link_point and point
			link_point->second.add_sink (segment->id);
			add_segment_source (*point, segment->id);  // attach to link point map object
			segment->attributes_numeric[CHINA_ROUTER_INDEX_FNODE] = (long) (*point)->id;
		}

		diff_x = abs (end_position.x - (*point)->longitude);
		diff_y = abs (end_position.y - (*point)->latitude);
		if ((diff_x + diff_y) <= POSITION_TOLERANCE) {
			link_point = point_links.find ((*point)->id);
			// Segment is source to link_point and point
			link_point->second.add_source (segment->id);
			add_segment_sink (*point, segment->id);
			segment->attributes_numeric[CHINA_ROUTER_INDEX_TNODE] = (long) (*point)->id;
		}
	}

}

bool check_overlap
	(class map_object *lake,
	const int lake_level,
	class map_object *shoreline,
	const int shoreline_level,
	dynamic_string &log)

{
	int node_index_1, node_index_2, polygon_index_2, match_count;

	// is shoreline a subset of lake?
	ASSERT (shoreline->type == MAP_OBJECT_POLYGON);
	match_count = 0;

	for (node_index_1 = 0;
	node_index_1 < ((map_polygon *) shoreline)->node_count;
	++node_index_1) {
		if (lake->type == MAP_OBJECT_POLYGON) {
			for (node_index_2 = 0;
			node_index_2 < ((map_polygon *) lake)->node_count;
			++node_index_2) {
				if ((((map_polygon *) shoreline)->nodes[node_index_1 * 2] == ((map_polygon *) lake)->nodes[node_index_2 * 2])
				&& (((map_polygon *) shoreline)->nodes[node_index_1 * 2 + 1] == ((map_polygon *) lake)->nodes[node_index_2 * 2 + 1])) {
					++match_count;
				}
			}
		}
		else {
			for (polygon_index_2 = 0;
			polygon_index_2 < ((map_polygon_complex *) lake)->polygon_count;
			++polygon_index_2) {
				for (node_index_2 = 0;
				node_index_2 < ((map_polygon_complex *) lake)->polygons [polygon_index_2].node_count;
				++node_index_2) {
					if ((((map_polygon *) shoreline)->nodes[node_index_1 * 2] == ((map_polygon_complex *) lake)->polygons [polygon_index_2].nodes[node_index_2 * 2])
					&& (((map_polygon *) shoreline)->nodes[node_index_1 * 2 + 1] == ((map_polygon_complex *) lake)->polygons [polygon_index_2].nodes[node_index_2 * 2 + 1])) {
						++match_count;
					}
				}
			}
		}
	}
	if (match_count > 1) {
		if (match_count >= ((map_polygon *) shoreline)->node_count) {
			log.add_formatted ("\t%d\t%ld\t%d\t%ld\tALL\t%d\n", lake_level, lake->id,
			shoreline_level, shoreline->id,
			((map_polygon *) shoreline)->node_count);

			return true;
		}
		else
			log.add_formatted ("\t%d\t%ld\t%d\t%ld\t%d\t%d\n", lake_level, lake->id,
			shoreline_level, shoreline->id,
			match_count, ((map_polygon *) shoreline)->node_count);
	}
	return false;
}

bool ChinaRivers::find_duplicates
	(dynamic_map *map_watershed,
	map_layer *layer_rivers,
	interface_window *view,
	dynamic_string &log)

// Layer hyd2_4l overlaps lakes in layer_hyd_4p but provides perimeter breaks at the outlet

// Among the named lake polygons in HYD1_4p and HYD2_4p, 
// Find duplicates in layer hyd2_4l
// Eliminate the former for the latter

{
	map_layer *layer_2_4l, *layer_1_4p, *layer_2_4p, *layer_1_4l, *layer_4, *layer_5;
	std::vector <dynamic_string>::iterator level;
	std::vector <map_object *>::iterator lake, shoreline;
	std::map <long, long> duplicates;
	std::set <long>::iterator target;
	bool error = false;
	bounding_cube lake_space;

	layer_2_4l = map_watershed->match ("hyd2_4l.shp");

	layer_1_4l = map_watershed->match ("hyd1_4l.shp");
	layer_1_4p = map_watershed->match ("hyd1_4p.shp");
	layer_2_4p = map_watershed->match ("hyd2_4p.shp");
	layer_4 = map_watershed->match ("River4_polyline");
	layer_5 = map_watershed->match ("River5_polyline");

	// hyd1_4p vs hyd2_4l
	for (lake = layer_1_4p->objects.begin ();
	lake != layer_1_4p->objects.end ();
	++lake) {
		// if ((*lake)->attributes_text [0] != "") {
			// HYD2_4L ID 924 and 3 others match HYD2_4P ID 135 L461 
			lake_space.clear ();
			(*lake)->check_extent (&lake_space);
			for (shoreline = layer_2_4l->objects.begin ();
			shoreline != layer_2_4l->objects.end ();
			++shoreline) {
				if ((*shoreline)->overlaps_logical (&lake_space))
					if (check_overlap (*lake, 2, *shoreline, 3, log))
						// discard river version of lake
						duplicates.insert (std::pair <long, long> (2 * 100000 + (*lake)->id, 3 * 100000 + (*shoreline)->id));
			}
		// }
	}

	// hyd2_4p vs hyd2_4l
	for (lake = layer_2_4p->objects.begin ();
	lake != layer_2_4p->objects.end ();
	++lake) {
		// if ((*lake)->attributes_text [0] != "") {
			lake_space.clear ();
			(*lake)->check_extent (&lake_space);
			for (shoreline = layer_2_4l->objects.begin ();
			shoreline != layer_2_4l->objects.end ();
			++shoreline) {
				if ((*shoreline)->overlaps_logical (&lake_space))
					if (check_overlap (*lake, 4, *shoreline, 3, log))
						// discard river version of lake
						duplicates.insert (std::pair <long, long> (4 * 100000 + (*lake)->id, 3 * 100000 + (*shoreline)->id));
			}
		// }
	}

	// hyd1_4l vs hyd2_4l
	for (lake = layer_1_4l->objects.begin ();
	lake != layer_1_4l->objects.end ();
	++lake) {
		lake_space.clear ();
		(*lake)->check_extent (&lake_space);
		for (shoreline = layer_2_4l->objects.begin ();
		shoreline != layer_2_4l->objects.end ();
		++shoreline) {
			if ((*shoreline)->overlaps_logical (&lake_space))
				if (check_overlap (*lake, 1, *shoreline, 3, log))
					// discard river version of lake
					duplicates.insert (std::pair <long, long> (1 * 100000 + (*lake)->id, 3 * 100000 + (*shoreline)->id));
		}
	}

	// River4_polyline vs hyd2_4l
	for (lake = layer_2_4p->objects.begin ();
	lake != layer_2_4p->objects.end ();
	++lake) {
		lake_space.clear ();
		(*lake)->check_extent (&lake_space);
		for (shoreline = layer_2_4l->objects.begin ();
		shoreline != layer_2_4l->objects.end ();
		++shoreline) {
			if ((*shoreline)->overlaps_logical (&lake_space))
				if (check_overlap (*lake, 5, *shoreline, 3, log))
					// discard river version of lake
					duplicates.insert (std::pair <long, long> (5 * 100000 + (*lake)->id, 3 * 100000 + (*shoreline)->id));
		}
	}

	purge_targets (layer_rivers, &duplicates);

	log.add_formatted ("Duplicates removed\t%d\n", duplicates.size ());

	return !error;
}

void ChinaRivers::purge_targets
	(map_layer *layer,
	std::map <long, long> *targets)

// Delete targets or Save only targets

{
	std::vector <map_object *> saved;
	std::vector <map_object *>::iterator shoreline;
	std::map <long, long>::iterator victim;

	victim = targets->find (2359);

	for (shoreline = layer->objects.begin ();
	shoreline != layer->objects.end ();
	++shoreline) {
		// Delete if among targets
		if ((victim = targets->find ((*shoreline)->id)) == targets->end ())
			saved.push_back (*shoreline);
		else
			delete *shoreline;
	}
	layer->objects.clear ();
	for (shoreline = saved.begin ();
	shoreline != saved.end ();
	++shoreline)
		layer->objects.push_back (*shoreline);

}

void ChinaRivers::set_segment_to
	(map_layer *layer_rivers,
	router_NHD *router,
	dynamic_string &log)

// for records where point_sinks has only 1 entry, find any source segments and set "TO"
// value in DNHYDROSEQ 

{
	std::map <long, ChinaLinkPoint>::iterator link;
	std::vector <long>::iterator segment_point;
	map_object *sink_segment, *source_segment;

	for (link = point_links.begin (); link != point_links.end (); ++link) {
		if (link->second.sinks.size () == 1) {
			segment_point = link->second.sinks.begin ();
			sink_segment = layer_rivers->match_id (*segment_point);
			ASSERT (sink_segment != NULL);

			for (segment_point = link->second.sources.begin (); segment_point != link->second.sources.end (); ++segment_point) {
				source_segment = layer_rivers->match_id (*segment_point);
				ASSERT (source_segment != NULL);
				source_segment->attributes_numeric[NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ] = link->first;

				router->all_from.push_back (source_segment->id);
				router->all_to.push_back (sink_segment->id);
			}
		}
	}
}

void ChinaRivers::dump_point_maps
	(dynamic_string &log) const

{
	std::map <long, ChinaLinkPoint>::const_iterator point;
	std::vector <long>::const_iterator segment_point;

	log += "\n";
	log += "Point Links\n";
	log += "\n";
	for (point = point_links.begin (); point != point_links.end (); ++point) {
		log.add_formatted ("%ld", point->first);
		if ((point->second.sources.size () > 0)
		|| (point->second.sinks.size () > 0)) {
			log += "\tSources";
			for (segment_point = point->second.sources.begin (); segment_point != point->second.sources.end (); ++segment_point) {
				log.add_formatted ("\t%ld", *segment_point);
			}
			log += "\n\tSinks";
			for (segment_point = point->second.sinks.begin (); segment_point != point->second.sinks.end (); ++segment_point) {
				log.add_formatted ("\t%ld", *segment_point);
			}
		}
		log += "\n";
	}
}

void ChinaRivers::write_segment_elevations
	(map_layer *layer_rivers,
	map_layer *layer_points,
	dynamic_string &log) const

// 2020-01-24 Nieyan: want list of elevations by segment ID

{
	// std::map <long, ChinaLinkPoint>::iterator link;
	std::vector <long>::iterator segment_point;
	std::vector <map_object *>::const_iterator segment;
	map_object *point;

	log += "\n";
	log += "Segment Elevations\n";
	log += "\n";

	for (segment = layer_rivers->objects.begin ();
	segment != layer_rivers->objects.end ();
	++segment) {
		log.add_formatted ("%ld", (*segment)->id);
		// if ((link = point_links.find ((*segment)->attributes_numeric[CHINA_ROUTER_INDEX_FNODE])) != point_links.end ())
		if ((point = layer_points->match_id ((*segment)->attributes_numeric[CHINA_ROUTER_INDEX_FNODE])) != NULL) {
			log.add_formatted ("\t%ld\t%.4lf", point->id, point->attributes_numeric[CHINA_LINK_POINT_ELEVATION]);
		}

		if ((point = layer_points->match_id ((*segment)->attributes_numeric[CHINA_ROUTER_INDEX_TNODE])) != NULL) {
			log.add_formatted ("\t%ld\t%.4lf", point->id, point->attributes_numeric[CHINA_LINK_POINT_ELEVATION]);
		}
		log += "\n";
	}
}

void ChinaRivers::add_segment_source
	(map_object *segment,
	const long sink_segment_id)

// checks & prevents duplication

// MAP_LAYER_USAGE_VOID_SOURCES_SINKS

{
	int index, found;

	for (index = 0, found = -1;
	(found == -1)
	&& (index < (int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT]);
	++index)
		if (((long *) segment->attributes_void) [index] == sink_segment_id)
			found = index;

	if (found == -1) {
		// Preserve sinks, add new source
		long *temp_void = (long *) segment->attributes_void;
		segment->attributes_void = new long[(int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT]
		+ (int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT] + 1];

		// Copy existing sources to first part of attributes_void
		if (segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] > 0.0)
			memcpy (segment->attributes_void, temp_void, sizeof (long) * (int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT]);
		// Add new source before second part
		((long *) segment->attributes_void) [(int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT]] = sink_segment_id;
		// Copy existing sinks to second part of attributes_void
		index = segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT];
		if (segment->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT] > 0.0)
			memcpy (&((long *) segment->attributes_void) [index + 1], &temp_void [index], sizeof (long) * (int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT]);
		segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] += 1;

		if ((segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] + segment->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT]) > 1.0)
			delete[] temp_void;
	}
}

void ChinaRivers::add_segment_sink
	(map_object *segment,
	const long sink_segment_id)

// checks & prevents duplication

// MAP_LAYER_USAGE_VOID_SOURCES_SINKS

{
	int index, found, total_count;

	total_count = (int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] + (int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT];

	for (index = (int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT], found = -1;
	(found == -1)
	&& (index < total_count);
	++index)
		if (((long *) segment->attributes_void) [index] == sink_segment_id)
			found = index;

	if (found == -1) {
		// Preserve sources, add new sink
		long *temp_void = (long *) segment->attributes_void;
		segment->attributes_void = new long[(int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] + (int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT] + 1];

		// Copy existing sources and sinks attributes_void
		if (total_count > 0)
			memcpy (segment->attributes_void, temp_void, sizeof (long) * ((int) segment->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] + (int) segment->attributes_numeric [CHINA_ROUTER_INDEX_SINKS_COUNT]));

		// Add new sink at end of second part
		((long *) segment->attributes_void) [total_count] = sink_segment_id;

		segment->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT] += 1;

		if (total_count > 0)
			delete[] temp_void;
	}
}

bool ChinaRivers::build_connection_map
	(dynamic_map *map_watershed,
	map_layer *layer_rivers,
	map_layer *layer_points,
	dynamic_string &log)

// Find all connections for every river segment

{
	map_object *segment_1, *segment_2;
	std::vector <map_object *>::iterator point;
	std::map <long, ChinaLinkPoint>::iterator link_point;
	std::vector <long>::iterator segment_1_id, segment_2_id;
	bool error = false;

	for (point = layer_points->objects.begin ();
	point != layer_points->objects.end ();
	++point) {
		if ((link_point = point_links.find ((*point)->id)) != point_links.end ()) {

			// 50021 connects to 300994 and 300996, but both of those drain the wrong way
			// because of apparently invalid elevations at their other ends

			for (segment_1_id = link_point->second.sources.begin ();
			segment_1_id != link_point->second.sources.end ();
			++segment_1_id) {
				if ((segment_1 = layer_rivers->match_id (*segment_1_id)) != NULL) {
					for (segment_2_id = link_point->second.sources.begin ();
					segment_2_id != link_point->second.sources.end ();
					++segment_2_id) {
						if (*segment_1_id != *segment_2_id) {
							if ((segment_2 = layer_rivers->match_id (*segment_2_id)) != NULL) {
								// point_link sources are all connections from the downstream end of segments
								// Make every segment listed for point_link a sink for every other one
								add_segment_sink (segment_1, *segment_2_id);
							}
							else
								log.add_formatted ("Point link %ld source segment %ld not found\n", link_point->first, *segment_2_id);
						}
					}

					// point_link sinks should also become sinks to every source
					for (segment_2_id = link_point->second.sinks.begin ();
					segment_2_id != link_point->second.sinks.end ();
					++segment_2_id) {
						if (*segment_1_id != *segment_2_id) {
							if ((segment_2 = layer_rivers->match_id (*segment_2_id)) != NULL) {
								// point_link sinks are all connections from the upstream end of segments
								// Make every segment listed for point_link a source for every other one
								add_segment_sink (segment_1, *segment_2_id);
							}
							else
								log.add_formatted ("Point link %ld sink segment %ld not found\n", link_point->first, *segment_2_id);
						}
					}
				}
				else
					log.add_formatted ("Point link %ld source segment %ld not found\n", link_point->first, *segment_1_id);
			}

			for (segment_1_id = link_point->second.sinks.begin ();
			segment_1_id != link_point->second.sinks.end ();
			++segment_1_id) {
				if ((segment_1 = layer_rivers->match_id (*segment_1_id)) != NULL) {
					for (segment_2_id = link_point->second.sinks.begin ();
					segment_2_id != link_point->second.sinks.end ();
					++segment_2_id) {
						if (*segment_1_id != *segment_2_id) {
							if ((segment_2 = layer_rivers->match_id (*segment_2_id)) != NULL) {
								// point_link sinks are all connections from the upstream end of segments
								// Make every segment listed for point_link a source for every other one
								add_segment_source (segment_1, *segment_2_id);
							}
							else
								log.add_formatted ("Point link %ld sink segment %ld not found\n", link_point->first, *segment_2_id);
						}
					}

					// point_link sources should also become sources to every sink
					for (segment_2_id = link_point->second.sources.begin ();
					segment_2_id != link_point->second.sources.end ();
					++segment_2_id) {
						if (*segment_1_id != *segment_2_id) {
							if ((segment_2 = layer_rivers->match_id (*segment_2_id)) != NULL) {
								// point_link sinks are all connections from the upstream end of segments
								// Make every segment listed for point_link a source for every other one
								add_segment_source (segment_1, *segment_2_id);
							}
							else
								log.add_formatted ("Point link %ld source segment %ld not found\n", link_point->first, *segment_2_id);
						}
					}				}
				else
					log.add_formatted ("Point link %ld sink segment %ld not found\n", link_point->first, *segment_1_id);
			}
		}
	}

	return !error;
}

bool is_source
	(const long id,
	map_object *segment)

// True if segment's sources include id

{
	for (int index = 0; index < (int) segment->attributes_numeric[0]; ++index)
		if (((long *) segment->attributes_void)[index] == id)
			return true;

	return false;
}

bool seek
	(map_object *start,
	map_object *destination,
	const bool headed_downstream,
	std::vector <map_object *> *path,
	std::set <long> *visited,
	std::set <long> *failures,
	double *distance_m,
	const std::map <long, map_object *> *all_segments)

// Returns distance including length of start but not finish

// Does not distinguish between upstream and downstream in failure set

{
	bool found = false;
	visited->insert (start->id);
	path->push_back (start);

	if (start == destination)
		found = true;
	else {
		int index;
		long next_id;
		std::map <long, map_object *>::const_iterator next_segment;
		double best_distance, sub_distance, perimeter;
		std::vector <map_object *> best_path;
		std::vector <map_object *>::iterator segment;
		std::set <long> best_visited;
		std::set <long>::iterator visit;
		std::vector <map_object *> sub_path;
		std::set <long> sub_visited;
		dynamic_string local_log;

		((map_polygon *) start)->perimeter_between_meters (0, ((map_polygon *) start)->total_number_of_points () - 1, &perimeter, local_log);
		*distance_m += perimeter;
		best_distance = DBL_MAX;

		if (headed_downstream) {
			// we came here through node 0. Depart via highest node index (sinks)

			for (index = (int) start->attributes_numeric[0];
			index < (int) (start->attributes_numeric[0] + start->attributes_numeric[1]);
			++index) {
				next_id = ((long *) start->attributes_void)[index];


				if ((visited->find (next_id) == visited->end ())
				&& (failures->find (next_id) == failures->end ())) {

					sub_path.clear ();
					sub_visited = *visited;
					sub_distance = 0.0;
					next_segment = all_segments->find (next_id);
					if (seek (next_segment->second, destination, is_source (start->id, next_segment->second), &sub_path, &sub_visited,
					failures, &sub_distance, all_segments)) {

						if (sub_distance < best_distance) {
							best_distance = sub_distance;
							best_path = sub_path;
							best_visited = sub_visited;
						}
						found = true;
					}
					else
						for (visit = sub_visited.begin (); visit != sub_visited.end (); ++visit)
							failures->insert (*visit);
				}
			}
		}
		else {
			// we entered start via highest node_index.  Depart via node 0 (sources)
			for (index = 0;
			index < (int) start->attributes_numeric[0];
			++index) {
				next_id = ((long *) start->attributes_void)[index];
				if ((visited->find (next_id) == visited->end ())
				&& (failures->find (next_id) == failures->end ())) {
					sub_path.clear ();
					sub_visited = *visited;
					sub_distance = 0.0;
					next_segment = all_segments->find (next_id);
					if (seek (next_segment->second, destination, is_source (start->id, next_segment->second), &sub_path, &sub_visited, failures, &sub_distance, all_segments)) {
						if (sub_distance < best_distance) {
							best_distance = sub_distance;
							best_path = sub_path;
							best_visited = sub_visited;
						}
						found = true;
					}
					else
						for (visit = sub_visited.begin (); visit != sub_visited.end (); ++visit)
							failures->insert (*visit);
				}
			}
		}
		if (found) {
			*distance_m += best_distance;
			for (segment = best_path.begin (); segment != best_path.end (); ++segment)
				path->push_back (*segment);
			for (visit = best_visited.begin (); visit != best_visited.end (); ++visit)
				visited->insert (*visit);
		}

	}
	return found;
}

bool next_segment_open
	(const long from_id,
	map_object *next_segment,
	std::set <long> visited [2],
	std::set <long> failures [2],  // [0] for upstream, [1] for downstream
	bool *next_segment_downstream)

// next_segment is open if we haven't traversed or failed when going through it before in the same direction 
// Direction is determined by whether from_id is a source or sink
	
{
	if ((visited [0].find (next_segment->id) == visited [0].end ())
	&& (visited[1].find (next_segment->id) == visited[1].end ())) {
		// No reason ever to visit twice on a single path

		// Check against failures found by other routes
		// But only if failure matched this intended direction
		*next_segment_downstream = is_source (from_id, next_segment);
		if (*next_segment_downstream) {
			// start is listed as a source to next_segment, so we're about to traverse next_segment downstream
			if (failures[1].find (next_segment->id) == failures[1].end ())
				return true;
		}
		else {
			// start is listed as a sink to next_segment, so we're about to traverse next_segment upstream
			if (failures[0].find (next_segment->id) == failures[0].end ())
				return true;
		}
	}
	return false;
}

bool seek_directional
	(map_object *start,
	map_object *destination,
	const bool headed_downstream,
	std::vector <map_object *> *path,
	std::set <long> visited [2],  // [0] for upstream, [1] for downstream
	std::set <long> failures [2],  // [0] for upstream, [1] for downstream
	double *distance_m,
	const std::map <long, map_object *> *all_segments)

// Returns distance including length of start but not finish
// Distinguishes between failure going upstream and downstream

{
	bool found = false;
	if (headed_downstream)
		visited [1].insert (start->id);
	else
		visited [0].insert (start->id);

	path->push_back (start);

	if (start == destination)
		found = true;
	else {
		int index;
		long next_id;
		std::map <long, map_object *>::const_iterator next_segment;
		double best_distance, sub_distance, perimeter;
		std::vector <map_object *> best_path;
		std::vector <map_object *>::iterator segment;
		std::set <long>::iterator visit;
		std::vector <map_object *> sub_path;
		std::set <long> sub_visited [2];
		dynamic_string local_log;
		bool next_segment_downstream;

		((map_polygon *) start)->perimeter_between_meters (0, ((map_polygon *) start)->total_number_of_points () - 1, &perimeter, local_log);
		*distance_m += perimeter;
		best_distance = DBL_MAX;

		if (headed_downstream) {
			// we came here through node 0. Depart via highest node index (sinks)

			for (index = (int) start->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT];
			index < (int) (start->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT] + start->attributes_numeric[CHINA_ROUTER_INDEX_SINKS_COUNT]);
			++index) {
				next_id = ((long *) start->attributes_void)[index];

				next_segment = all_segments->find (next_id);
				// Have we previously visited next_segment ?
				// Or was it a previous failure (in the same direction we're moving) ?
				if (next_segment_open (start->id, next_segment->second, visited, failures, &next_segment_downstream)) {

					sub_visited [0] = visited [0];
					sub_visited [1] = visited [1];
					sub_distance = 0.0;
					if (seek_directional (next_segment->second, destination, next_segment_downstream, &sub_path, sub_visited,
					failures, &sub_distance, all_segments)) {

						if (sub_distance < best_distance) {
							best_distance = sub_distance;
							best_path = sub_path;
						}
						found = true;
					}
					else {
						// Only add the parts of sub_visited that are beyond visited
						for (visit = sub_visited [0].begin (); visit != sub_visited [0].end (); ++visit)
							if (visited [0].find (*visit) == visited [0].end ())
								failures [0].insert (*visit);
						for (visit = sub_visited [1].begin (); visit != sub_visited [1].end (); ++visit)
							if (visited [1].find (*visit) == visited [1].end ())
								failures [1].insert (*visit);
					}
				}
			}
		}
		else {
			// we entered start via highest node_index.  Depart via node 0 (sources)
			for (index = 0;
			index < (int) start->attributes_numeric[CHINA_ROUTER_INDEX_SOURCES_COUNT];
			++index) {
				next_id = ((long *) start->attributes_void)[index];

				next_segment = all_segments->find (next_id);

				// Have we previously visited next_segment ?
				// Or was it a previous failure (in the same direction we're moving) ?
				if (next_segment_open (start->id, next_segment->second, visited, failures, &next_segment_downstream)) {

					sub_visited [0] = visited [0];
					sub_visited [1] = visited [1];
					sub_distance = 0.0;

					if (seek_directional (next_segment->second, destination, next_segment_downstream,
					&sub_path, sub_visited, failures, &sub_distance, all_segments)) {
						if (sub_distance < best_distance) {
							best_distance = sub_distance;
							best_path = sub_path;
						}
						found = true;
					}
					else {
						// Only add the parts of sub_visited that are beyond visited
						for (visit = sub_visited [0].begin (); visit != sub_visited [0].end (); ++visit)
							if (visited [0].find (*visit) == visited [0].end ())
								failures [0].insert (*visit);
						for (visit = sub_visited [1].begin (); visit != sub_visited [1].end (); ++visit)
							if (visited [1].find (*visit) == visited [1].end ())
								failures [1].insert (*visit);
					}
				}
			}
		}
		if (found) {
			*distance_m += best_distance;
			for (segment = best_path.begin (); segment != best_path.end (); ++segment)
				path->push_back (*segment);
			// Don't pass visited back down, other routes may need to try these branches
		}

	}
	return found;
}

bool ChinaRouter::route
	(map_layer *layer_rivers,
	const std::map <long, map_object *> *river_layer_index,
	dynamic_string &log)

{
	std::set <long> visited, failures;
	bool found = false;
	device_coordinate start_position, end_position;
	map_object *start_segment, *end_segment;
	double first_segment_total_perimeter_m;
	long gap;

	start_object->centroid (&start_position.y, &start_position.x);
	start_segment = layer_rivers->find_nearest_object_fast (start_position, &gap);
	start_segment->closest_point_meters (start_position, &start_point_index);

	end_object->centroid (&end_position.y, &end_position.x);
	end_segment = layer_rivers->find_nearest_object_fast (end_position, &gap);
	end_segment->closest_point_meters (end_position, &end_point_index);

	log.add_formatted ("\t%ld to %ld\n", start_segment->id, end_segment->id);

	// try to go upstream from start
	path.clear ();
	distance_m = 0.0;
	if (seek (start_segment, end_segment, false, &path, &visited, &failures, &distance_m, river_layer_index)) {
		found = true;
		upstream = true;
		elevation_start = start_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_START];
		((map_polygon *) start_segment)->perimeter_between_meters (0, ((map_polygon *) start_segment)->total_number_of_points () - 1, &first_segment_total_perimeter_m, log);
		((map_polygon *) start_segment)->perimeter_between_meters (0, start_point_index, &distance_start_m, log);
		elevation_end = end_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_END];
		((map_polygon *) end_segment)->perimeter_between_meters (end_point_index, ((map_polygon *) end_segment)->total_number_of_points () - 1, &distance_end_m, log);

		// Seek returns distance including first segment (entire length) but not last
		distance_m -= first_segment_total_perimeter_m;
	}
	else {
		// try to go downstream from start
		visited.clear ();
		path.clear ();
		distance_m = 0.0;
		if (seek (start_segment, end_segment, false, &path, &visited, &failures, &distance_m, river_layer_index)) {
			upstream = false;
			elevation_start = start_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_END];
			((map_polygon *) start_segment)->perimeter_between_meters (0, ((map_polygon *) start_segment)->total_number_of_points () - 1, &first_segment_total_perimeter_m, log);
			((map_polygon *) start_segment)->perimeter_between_meters (start_point_index, ((map_polygon *) start_segment)->total_number_of_points () - 1, &distance_start_m, log);
			elevation_end = end_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_START];
			((map_polygon *) end_segment)->perimeter_between_meters (0, end_point_index, &distance_end_m, log);

			// Seek returns distance including first segment (entire length) but not last
			distance_m -= first_segment_total_perimeter_m;
		}
	}

	return found;
}

bool ChinaRouter::route_directional
	(map_layer *layer_rivers,
	const std::map <long, map_object *> *river_layer_index,
	dynamic_string &log)

// Calls seek_directional so that failure when visiting a segment upstream
// doesn't prevent trying the same segment headed downstream

{
	std::set <long> visited [2], failures[2];
	bool found = false;
	device_coordinate start_position, end_position;
	map_object *start_segment, *end_segment;
	double first_segment_total_perimeter_m;
	long gap;

	start_object->centroid (&start_position.y, &start_position.x);
	start_segment = layer_rivers->find_nearest_object_fast (start_position, &gap);
	start_segment->closest_point_meters (start_position, &start_point_index);

	end_object->centroid (&end_position.y, &end_position.x);
	end_segment = layer_rivers->find_nearest_object_fast (end_position, &gap);
	end_segment->closest_point_meters (end_position, &end_point_index);

	// try to go upstream from start
	path.clear ();
	distance_m = 0.0;
	if (seek_directional (start_segment, end_segment, false, &path, visited, failures,
	&distance_m, river_layer_index)) {
		found = true;
		upstream = true;
		elevation_start = start_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_START];
		((map_polygon *) start_segment)->perimeter_between_meters (0, ((map_polygon *) start_segment)->total_number_of_points () - 1, &first_segment_total_perimeter_m, log);
		((map_polygon *) start_segment)->perimeter_between_meters (0, start_point_index, &distance_start_m, log);
		elevation_end = end_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_END];
		((map_polygon *) end_segment)->perimeter_between_meters (end_point_index, ((map_polygon *) end_segment)->total_number_of_points () - 1, &distance_end_m, log);

		// Seek returns distance including first segment (entire length) but not last
		distance_m -= first_segment_total_perimeter_m;
	}
	else {
		// try to go downstream from start
		visited [0].clear ();
		visited [1].clear ();
		path.clear ();
		distance_m = 0.0;
		if (seek_directional (start_segment, end_segment, false, &path, visited, failures, &distance_m, river_layer_index)) {
			upstream = false;
			elevation_start = start_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_END];
			((map_polygon *) start_segment)->perimeter_between_meters (0, ((map_polygon *) start_segment)->total_number_of_points () - 1, &first_segment_total_perimeter_m, log);
			((map_polygon *) start_segment)->perimeter_between_meters (start_point_index, ((map_polygon *) start_segment)->total_number_of_points () - 1, &distance_start_m, log);
			elevation_end = end_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_START];
			((map_polygon *) end_segment)->perimeter_between_meters (0, end_point_index, &distance_end_m, log);

			// Seek returns distance including first segment (entire length) but not last
			distance_m -= first_segment_total_perimeter_m;
		}
	}

	return found;
}



void ChinaRouter::write_route_line
	(dynamic_string &log)

{
	std::vector <map_object *>::iterator road;

	log.add_formatted ("%ld\t%ld", start_object->id, end_object->id);
	log.add_formatted ("\t%.4lf", distance_start_m / 1000.0);
	log.add_formatted ("\t%.4lf", distance_m / 1000.0);
	log.add_formatted ("\t%.4lf", distance_end_m / 1000.0);
	log.add_formatted ("\t%.4lf", (distance_start_m + distance_m + distance_end_m) / 1000.0);
	log.add_formatted ("\t%.4lf", elevation_start);
	log.add_formatted ("\t%.4lf", elevation_end);

	for (road = path.begin ();
	road != path.end ();
	++road)
		log.add_formatted ("\t%ld", (*road)->id);
	log += "\n";
}

bool ChinaRivers::route_all
	(map_layer *layer_stations,
	map_layer *layer_counties,
	map_layer *layer_rivers,
	interface_window *view,
	dynamic_string &log)

{
	std::vector <map_object *>::const_iterator station, county;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator i_job;
	thread_work_ChinaRouting *job;
	thread_manager threader;
	thread_manager_statistics stats;
	int thread_count = 25;
	long job_count;
	bool error = false;

	job_count = 0;

	for (station = layer_stations->objects.begin ();
	// (jobs.size () < 10)
	station != layer_stations->objects.end ();
	++station) {

		job = new thread_work_ChinaRouting;
		job->work_function = &work_ChinaRouting;
		job->router.start_object = (*station);
		job->layer_counties = layer_counties;
		job->layer_rivers = layer_rivers;
		job->river_layer_index = &river_layer_index;

		job->id = ++job_count;
		job->description.format ("%ld Station %ld", job->id, job->router.start_object->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 5;
	stats.reset (thread_count);
	threader.statistics = &stats;

	view->set_data (&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
	threader.run (&jobs, view, log);
	// threader.run_tiny (&jobs, view, log);
	view->set_data (NULL);

	view->update_status_bar ("Threader run complete");

	log += "Start\tEnd\tStart Distance km\tDistance Between km\tEnd Distance km\tTotal Distance km\tStart Elevation\tEnd Elevation\tSegment";
	log += "\n";

	for (i_job = jobs.begin ();
	i_job != jobs.end ();
	++i_job) {
		log += i_job->second->log;
		delete i_job->second;
	}

	return !error;
}
