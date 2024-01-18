

#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include <stack>
#include <algorithm>
#include <sys/timeb.h>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/shapefile_filter.h"
#include "../map/projection_utm.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "color_set_histogram.h"
#include "../cardgis_console/string_grid.h"
#include "../cardgis_console/router_NHD.h"
// #include "MinkowskiBuffer.h"
// #include "../clipper_ver6.2.1/cpp/clipper.hpp"
#include "../clipper_ver6.4.2/cpp/clipper.hpp"
#include "ClipperBuffer.h"

#include <GeographicLib/PolygonArea.hpp>
#include <GeographicLib/DMS.hpp>
#include <GeographicLib/Utility.hpp>
#include <GeographicLib/GeoCoords.hpp>
#include <GeographicLib/Ellipsoid.hpp>
#include "../subbasin_effect/thread_manager.h"
#include "thread_work_intersect.h"

// For lat/long conversion, double coordinate are divided by 1.0e6 when converted to long
// This can overflow ClipperLib::CInt
// For utm, divide by this number
const double UTM_METERS_FACTOR = 1.0e6;

void area_state_overlaps
	(const int area_number,
	std::set <int> *state_set)

{
	switch (area_number) {
		case NHDAREA_01: // New England
			state_set->insert (9); // Connecticut
			state_set->insert (23); // Maine
			state_set->insert (25); // Massachusetts
			state_set->insert (33); // New Hampshire
			state_set->insert (36); // New York
			state_set->insert (44); // Rhode Island
			state_set->insert (50); // Vermont
			break;
		case NHDAREA_02: // Chesapeake
			state_set->insert (9); // Connecticut
			state_set->insert (10); // Delaware
			state_set->insert (24); // Maryland
			state_set->insert (25); // Massachusetts
			state_set->insert (34); // New Jersey
			state_set->insert (36); // New York
			state_set->insert (42); // Pennsylvania
			state_set->insert (50); // Vermont
			state_set->insert (51); // Virginia
			state_set->insert (54); // West Virginia
			break;
		case NHDAREA_03N: // 3N Carolinas
			state_set->insert (13); // Georgia
			state_set->insert (37); // North Carolina
			state_set->insert (45); // South Carolina
			state_set->insert (51); // Virginia
			break;
		case NHDAREA_03S: // 3S Florida
			state_set->insert (12); // Florida
			state_set->insert (13); // Georgia
			break;
		case NHDAREA_03W: // 3W Alabama
			state_set->insert (1); // Alabama
			state_set->insert (12); // Florida
			state_set->insert (13); // Georgia
			state_set->insert (28); // Mississippi
			state_set->insert (47); // Tennessee
			break;
		case NHDAREA_04: // 4 Michigan
			state_set->insert (17); // Illinois
			state_set->insert (18); // Indiana
			state_set->insert (26); // Michigan
			state_set->insert (27); // Minnesota
			state_set->insert (55); // Wisconsin
			state_set->insert (36); // New York
			state_set->insert (39); // Ohio
			state_set->insert (42); // Pennsylvania
			break;
		case NHDAREA_05: // 5 Ohio
			state_set->insert (17); // Illinois
			state_set->insert (18); // Indiana
			state_set->insert (21); // Kentucky
			state_set->insert (24); // Maryland
			state_set->insert (36); // New York
			state_set->insert (37); // North Carolina
			state_set->insert (39); // Ohio
			state_set->insert (42); // Pennsylvania
			state_set->insert (47); // Tennessee
			state_set->insert (51); // Virginia
			state_set->insert (54); // West Virginia
			break;
		case NHDAREA_06: // 6 Tennessee
			state_set->insert (1); // Alabama
			state_set->insert (13); // Georgia
			state_set->insert (21); // Kentucky
			state_set->insert (28); // Mississippi
			state_set->insert (37); // North Carolina
			state_set->insert (45); // South Carolina
			state_set->insert (47); // Tennessee
			state_set->insert (51); // Virginia
			break;
		case NHDAREA_07: // 7 UMRB
			state_set->insert (17); // Illinois
			state_set->insert (18); // Indiana
			state_set->insert (19); // iowa
			state_set->insert (26); // Michigan
			state_set->insert (27); // Minnesota
			state_set->insert (29); // Missouri
			state_set->insert (46); // South Dakota
			state_set->insert (55); // Wisconsin
			break;
		case NHDAREA_08: // 8 Lower Mississippi
			state_set->insert (5); // Arkansas
			state_set->insert (17); // Illinois
			state_set->insert (21); // Kentucky
			state_set->insert (22); // Louisianna
			state_set->insert (28); // Mississippi
			state_set->insert (29); // Missouri
			state_set->insert (47); // Tennessee
			break;
		case NHDAREA_09: // 9 North Dakota
			state_set->insert (27); // Minnesota
			state_set->insert (38); // North Dakota
			state_set->insert (46); // South Dakota
			break;
		case NHDAREA_10L: // 10L Missouri Lower
			state_set->insert (8); // Colorado
			state_set->insert (19); // iowa
			state_set->insert (20); // Kansas
			state_set->insert (27); // Minnesota
			state_set->insert (29); // Missouri
			state_set->insert (31); // Nebraska
			state_set->insert (56); // Wyoming
			break;
		case NHDAREA_10U: // 10U Missouri Upper
			state_set->insert (16); // Idaho
			state_set->insert (19); // iowa
			state_set->insert (27); // Minnesota
			state_set->insert (30); // Montana
			state_set->insert (31); // Nebraska
			state_set->insert (38); // North Dakota
			state_set->insert (46); // South Dakota
			state_set->insert (56); // Wyoming
			break;
		case NHDAREA_11: // 11 Arkansas River
			state_set->insert (5); // Arkansas
			state_set->insert (8); // Colorado
			state_set->insert (20); // Kansas
			state_set->insert (22); // Louisianna
			state_set->insert (29); // Missouri
			state_set->insert (35); // New Mexico
			state_set->insert (40); // Oklahoma
			state_set->insert (48); // Texas
			break;
		case NHDAREA_12: // 12 Texas Gulf Coast
			state_set->insert (22); // Louisianna
			state_set->insert (48); // Texas
			state_set->insert (35); // New Mexico
			break;
		case NHDAREA_13: // 13 SW Texas
			state_set->insert (8); // Colorado
			state_set->insert (35); // New Mexico
			state_set->insert (48); // Texas
			// *utm_zone = 13;
			// *utm_letter = 'R';
			break;
		case NHDAREA_14: // 14 Colorado Upper
			state_set->insert (4); // Arizona
			state_set->insert (8); // Colorado
			state_set->insert (35); // New Mexico
			state_set->insert (49); // Utah
			state_set->insert (56); // Wyoming
			// *utm_zone = 12;
			// *utm_letter = 'S';
			break;
		case NHDAREA_15: // 15 Colorado Lower
			state_set->insert (4); // Arizona
			state_set->insert (6); // California
			state_set->insert (32); // Nevada
			state_set->insert (35); // New Mexico
			state_set->insert (49); // Utah
			break;
		case NHDAREA_16: // 16 Nevada-Utah
			state_set->insert (6); // California
			state_set->insert (16); // Idaho
			state_set->insert (32); // Nevada
			state_set->insert (41); // Oregon
			state_set->insert (49); // Utah
			state_set->insert (56); // Wyoming
			break;
		case NHDAREA_17: // 17 Washington
			state_set->insert (6); // California
			state_set->insert (16); // Idaho
			state_set->insert (30); // Montana
			state_set->insert (32); // Nevada
			state_set->insert (41); // Oregon
			state_set->insert (49); // Utah
			state_set->insert (53); // Washington
			state_set->insert (56); // Wyoming
			break;
		case NHDAREA_18: // 18 California
			state_set->insert (6); // California
			state_set->insert (32); // Nevada
			state_set->insert (41); // Oregon
	}
}

double area_m2
	(const ClipperLib::Path *path,
	const BYTE linetype)

{
	GeographicLib::Math::real a, f, perimeter, area = 0.0;
	bool polyline = false, reverse = false, sign = true;
    // enum { GEODESIC, EXACT, AUTHALIC, RHUMB };
	// int linetype = GEODESIC;
	int precision = 6;

	a = GeographicLib::Constants::WGS84_a();
	f = GeographicLib::Constants::WGS84_f();
    const GeographicLib::Ellipsoid ellip(a, f);

	GeographicLib::GeoCoords p;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	unsigned num;
	// Max precision = 10: 0.1 nm in distance, 10^-15 deg (= 0.11 nm),
	// 10^-11 sec (= 0.3 nm).
	// Parens around std::min and std::max so that windef.h version doesn't interfere
	precision = (std::min) (10 + GeographicLib::Math::extra_digits(), (std::max) (0, precision));

	switch (linetype) {
		case GEOGRAPHICLIB_LINETYPE_GEODESIC:
			{
				const GeographicLib::Geodesic geod(a, f);
				GeographicLib::PolygonArea poly(geod, polyline);
				for (path_point = path->begin ();
				path_point != path->end ();
				++path_point) {
					p.Reset ((double) path_point->Y / 1.0e6, (double) path_point->X / 1.0e6, GeographicLib::UTMUPS::STANDARD);
					poly.AddPoint(p.Latitude(), p.Longitude());
				}
				poly.Compute(reverse, sign, perimeter, area); // geodesic + authalic
			}
			break;
		case GEOGRAPHICLIB_LINETYPE_EXACT:
			{
				const GeographicLib::GeodesicExact geode(a, f);
				GeographicLib::PolygonAreaExact polye (geode, polyline);

				for (path_point = path->begin ();
				path_point != path->end ();
				++path_point) {
					p.Reset ((double) path_point->Y / 1.0e6, (double) path_point->X / 1.0e6, GeographicLib::UTMUPS::STANDARD);
					polye.AddPoint(p.Latitude(), p.Longitude());
				}
				num =  polye.Compute (reverse, sign, perimeter, area);
			}
			break;
		case GEOGRAPHICLIB_LINETYPE_AUTHALIC:
			{
				const GeographicLib::Geodesic geod(a, f);
				GeographicLib::PolygonArea poly(geod, polyline);
				for (path_point = path->begin ();
				path_point != path->end ();
				++path_point) {
					p.Reset ((double) path_point->Y / 1.0e6, (double) path_point->X / 1.0e6, GeographicLib::UTMUPS::STANDARD);
					poly.AddPoint (ellip.AuthalicLatitude(p.Latitude()), p.Longitude());
				}
				poly.Compute(reverse, sign, perimeter, area); // geodesic + authalic
			}
			break;
		case GEOGRAPHICLIB_LINETYPE_RHUMB:
			{
				const GeographicLib::Rhumb rhumb(a, f);
				GeographicLib::PolygonAreaRhumb polyr(rhumb, polyline);

				for (path_point = path->begin ();
				path_point != path->end ();
				++path_point) {
					p.Reset ((double) path_point->Y / 1.0e6, (double) path_point->X / 1.0e6, GeographicLib::UTMUPS::STANDARD);
					polyr.AddPoint(p.Latitude(), p.Longitude());
				}
				polyr.Compute(reverse, sign, perimeter, area);
			}
	}

	// log += "\nPerimeter\t";
	// log += GeographicLib::Utility::str(perimeter, precision).c_str ();
	// log += "\nArea\t";
	// log += GeographicLib::Utility::str(area, std::max (0, precision - 5)).c_str ();
	// log += "\n";

	return area;
}

bool read_census_tracts
	(const dynamic_string &path_shapefile,
	dynamic_map *map_watershed,
	map_layer *layer_census,
	std::set <int> *state_fips,
	interface_window *update_display,
	dynamic_string &log)

{
	bool error = false;
	importer_shapefile importer;
	std::set <int>::const_iterator state;
	shapefile_filter *filter;
	update_display->update_progress ("Reading Shapefile");

	importer.filename_source = path_shapefile;
	// importer.id_field_name = "AREAKEY";
	importer.name_field_name = "AREAKEY";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = true;
	// 2018-04-20 Can't add AREAKEY to column_names_text
	// because that value goes in "NAME", so attribute_count_text is 0
	// layer_census->column_names_text.push_back ("AREAKEY");

	for (state = state_fips->begin ();
	state != state_fips->end ();
	++state) {
		filter = new shapefile_filter;
		filter->type = SHAPEFILE_FILTER_MASK;
		// 01003011000
		filter->field_name = "AREAKEY";
		// filter->pattern = "08#########"; // Colorado
		filter->pattern.format ("%02d#########", *state);
		importer.filters.push_back (filter);
	}

	if (!importer.import (layer_census, NULL, map_watershed, update_display, log))
		error = true;

	map_watershed->set_extent ();

	log += "Census layer source : ";
	log += importer.filename_source;
	log += "\n";
	log.add_formatted ("Census layer size : %d\n", layer_census->objects.size ());

	return !error;
}

bool read_PlaceTract
	(const dynamic_string &path_shapefile,
	dynamic_map *map_watershed,
	map_layer *layer_polygons,
	std::set <int> *state_fips,
	interface_window *update_display,
	dynamic_string &log)

{
	bool error = false;
	importer_shapefile importer;
	std::set <int>::const_iterator state;
	shapefile_filter *filter;
	update_display->update_progress ("Reading Shapefile");

	importer.filename_source = path_shapefile;
	importer.id_field_name = "FID_tracts";
	importer.name_field_name = "AREAKEY";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = true;
	// 2018-04-20 Can't add AREAKEY to column_names_text
	// because that value goes in "NAME", so attribute_count_text is 0
	// layer_census->column_names_text.push_back ("AREAKEY");

	for (state = state_fips->begin ();
	state != state_fips->end ();
	++state) {
		filter = new shapefile_filter;
		filter->type = SHAPEFILE_FILTER_MASK;
		// 01003011000
		filter->field_name = "AREAKEY";
		// filter->pattern = "08#########"; // Colorado
		filter->pattern.format ("%02d#########", *state);
		importer.filters.push_back (filter);
	}

	if (!importer.import (layer_polygons, NULL, map_watershed, update_display, log))
		error = true;

	map_watershed->set_extent ();

	log += "PlaceTract layer source : ";
	log += importer.filename_source;
	log += "\n";
	log.add_formatted ("PlaceTract layer size : %d\n", layer_polygons->objects.size ());

	return !error;
}

bool read_PlaceTract_clipint
	(const dynamic_string &path_shapefile,
	dynamic_map *map_watershed,
	map_layer *layer_polygons,
	std::set <int> *state_fips,
	std::chrono::system_clock::time_point *time_after_shp,
	interface_window *update_display,
	dynamic_string &log)

// 2018-05-21 Dave Keiser "I first clipped the tract file with the place shapefile and then intersected the files."

{
	bool error = false;
	importer_shapefile importer;
	std::set <int>::const_iterator state;
	shapefile_filter *filter;
	update_display->update_progress ("Reading Shapefile");

	importer.filename_source = path_shapefile;
	importer.name_field_name = "AREAKEY";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = true;
	// 2018-04-20 Can't add AREAKEY to column_names_text
	// because that value goes in "NAME", so attribute_count_text is 0
	// layer_census->column_names_text.push_back ("AREAKEY");

	for (state = state_fips->begin ();
	state != state_fips->end ();
	++state) {
		filter = new shapefile_filter;
		filter->type = SHAPEFILE_FILTER_MASK;
		// 01003011000
		filter->field_name = "AREAKEY";
		// filter->pattern = "08#########"; // Colorado
		filter->pattern.format ("%02d#########", *state);
		importer.filters.push_back (filter);
	}

	if (!importer.import (layer_polygons, NULL, map_watershed, update_display, log, time_after_shp))
		error = true;

	map_watershed->set_extent ();

	log += "PlaceTract layer source : ";
	log += importer.filename_source;
	log += "\n";
	log.add_formatted ("PlaceTract layer size : %d\n", layer_polygons->objects.size ());

	return !error;
}



bool read_county_boundaries
	(const dynamic_string &path_shapefile,
	dynamic_map *map_watershed,
	map_layer *layer_county,
	std::set <int> *state_fips,
	interface_window *update_display,
	dynamic_string &log)

// Census TIGER county boundaries 2012

// Called by frame_CARDGIS.OnOpenArea, OnRouteCountiesRiver, OnListCountiesUpstream, OnCountiesUpstreamStrahler, OnMatchAFIDA

{
	bool error = false;
	importer_shapefile importer;
	std::set <int>::const_iterator state;
	shapefile_filter *filter;
	update_display->update_progress ("Reading Shapefile");

	importer.filename_source = path_shapefile;
	importer.id_field_name = "GEOID";
	importer.name_field_name = "NAME";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;

	if (state_fips)
		for (state = state_fips->begin ();
		state != state_fips->end ();
		++state) {
			filter = new shapefile_filter;
			filter->type = SHAPEFILE_FILTER_MASK;
			// 01003011000
			filter->field_name = importer.id_field_name;
			// filter->pattern = "08###"; // Colorado
			filter->pattern.format ("%02d###", *state);
			importer.filters.push_back (filter);
		}

	if (!importer.import (layer_county, NULL, map_watershed, update_display, log))
		error = true;

	map_watershed->set_extent ();

	log += "County boundary layer source\t";
	log += importer.filename_source;
	log += "\n";
	log.add_formatted ("County boundary layer size\t%d\n", layer_county->objects.size ());

	return !error;
}

bool work_combine_polygons
	(const map_layer *buffer_partial,
	map_layer *layer_combined)

{
	ClipperLib::Clipper co = ClipperLib::Clipper();
	ClipperLib::Path p;
	ClipperLib::Paths subject, clip, solution, collective;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	map_polygon *outline;
	std::vector <map_object *>::const_iterator segment;
	int index;

	// Combine the first two segments, 
	// then add each subsequent segment

	for (segment = buffer_partial->objects.begin ();
	segment != buffer_partial->objects.end ();
	++segment) {

		subject.clear ();
		clip.clear ();
		p.clear ();
		p.reserve (((map_polygon *) (* segment))->node_count);

		if (segment == buffer_partial->objects.begin ()) {
			// Combine the first 2 segments
			std::vector <map_object *>::const_iterator segment_2;

			for (index = 0;
			index < ((map_polygon *) *segment)->node_count;
			++index)
				p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((map_polygon *) *segment)->nodes [index * 2],  (ClipperLib::cInt) ((map_polygon *) *segment)->nodes [index * 2 + 1]));
			subject.push_back (p);

			segment_2 = segment;
			++segment_2;

			for (index = 0;
			index < ((map_polygon *) *segment_2)->node_count;
			++index)
				p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((map_polygon *) *segment_2)->nodes [index * 2],  (ClipperLib::cInt) ((map_polygon *) *segment_2)->nodes [index * 2 + 1]));
			clip.push_back (p);
			segment = segment_2;

			co.Clear ();
			co.AddPaths (subject, ClipperLib::ptSubject, true);
			co.AddPaths (clip, ClipperLib::ptClip, true);
			collective.clear();
			co.Execute (ClipperLib::ctIntersection, collective, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
		}
		else {
			// Add each remaining segments to collective
			for (index = 0;
			index < ((map_polygon *) *segment)->node_count;
			++index)
				p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((map_polygon *) *segment)->nodes [index * 2],  (ClipperLib::cInt) ((map_polygon *) *segment)->nodes [index * 2 + 1]));
			clip.push_back (p);

			co.Clear ();
			co.AddPaths (collective, ClipperLib::ptSubject, true);
			co.AddPaths (clip, ClipperLib::ptClip, true);
			solution.clear();
			co.Execute (ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

			collective = solution;
		}
	}

	for (paths_path = collective.begin ();
	paths_path != collective.end ();
	++paths_path) {
		outline = (map_polygon *) layer_combined->create_new (MAP_OBJECT_POLYGON);
		outline->id = 1;
		outline->node_count = (long) paths_path->size ();
		outline->nodes = new long [outline->node_count * 2];
		for (path_point = paths_path->begin (), index = 0;
		path_point != paths_path->end ();
		++path_point, ++index) {
			outline->nodes [index * 2] = (long) path_point->X;
			outline->nodes [index * 2 + 1] = (long) path_point->Y;
		}
		layer_combined->objects.push_back (outline);
	}
	return true;
}

void bisection_definition::create_map_object
	(map_layer *layer,
	dynamic_string &log) const

{
	map_polygon *line;

	// line = new map_polygon_wx;
	line = (map_polygon *) layer->create_new (MAP_OBJECT_POLYGON);
	line->id = 1;
	line->node_count = 2;
	line->nodes = new long [line->node_count * 2];
	line->nodes [0] = (long) (end_0.x * 1.0e6);
	line->nodes [1] = (long) (end_0.y * 1.0e6);
	line->nodes [2] = (long) (end_1.x * 1.0e6);
	line->nodes [3] = (long) (end_1.y * 1.0e6);

	line->dump (layer, log);
	layer->objects.push_back (line);
}

void apply_offset
	(const int quadrant,
	logical_coordinate *end_0,
	logical_coordinate *end_1,
	const double theta,
	const double radius,
	const logical_coordinate &center_point,
	logical_coordinate *new_point)

// Left side of 0->1 vector is upstream
// Right side is downstream

{
	switch (quadrant) {
		case 1:
			// new_point.x > 0
			// new_point.y < 0
			new_point->x = sin (theta) * radius;
			new_point->y = cos (theta) * radius;

			end_0->x = center_point.x - new_point->x;
			end_0->y = center_point.y - new_point->y;
			end_1->x = center_point.x + new_point->x;
			end_1->y = center_point.y + new_point->y;
			break;
		case 2:
			// new_point.x > 0
			// new_point.y < 0
			new_point->x = sin (theta) * radius;
			new_point->y = cos (theta) * radius;
			end_0->x = center_point.x - new_point->x;
			end_0->y = center_point.y - new_point->y;
			end_1->x = center_point.x + new_point->x;
			end_1->y = center_point.y + new_point->y;
			break;
		case 3:
			// new_point.x < 0
			// new_point.y < 0
			new_point->x = cos (theta) * radius;
			new_point->y = sin (theta) * radius;
			end_0->x = center_point.x + new_point->x;
			end_0->y = center_point.y - new_point->y;
			end_1->x = center_point.x - new_point->x;
			end_1->y = center_point.y + new_point->y;
			break;
		case 4:
			// new_point.x > 0
			// new_point.y < 0
			new_point->x = cos (theta) * radius;
			new_point->y = sin (theta) * radius;
			end_0->x = center_point.x - new_point->x;
			end_0->y = center_point.y + new_point->y;
			end_1->x = center_point.x + new_point->x;
			end_1->y = center_point.y - new_point->y;
	}
}

void bisection_definition::create_bisection_2pt
	(const logical_coordinate upstream_first [2],
	const double radius_average,
	double *theta,
	const bool verbose,
	dynamic_string &log)

// points must be in order 
// Left side of 0->1 vector is upstream
// Right side is downstream

// Perpendicular computed by adding PI/2

{
	logical_coordinate diff1, new_point;
	int quadrant;

	center_point = upstream_first [1];

	// find angle on the left side going up
	diff1.x = center_point.x - upstream_first [0].x;
	diff1.y = center_point.y - upstream_first [0].y;
			
	if (diff1.x != 0.0)
		// 2016-02-23 atan has issues with quadrants
		*theta = atan (diff1.x / diff1.y);
	else
		// vertical
		*theta = 0;

	// normalize thetas with quadrant
	find_quadrant (diff1, theta);
//	quadrant = ((int) floor ((*theta + (2 * PI)) / (PI / 2.0)) % 4) + 1;

//	apply_offset (quadrant, &end_0, &end_1, *theta, radius_average, center_point, &new_point);

	// perpendicular
	*theta += PI / 2.0;

	// normalize
	if (*theta < 0)
		*theta += 2.0 * PI;
	else
		*theta = fmod (*theta, 2.0 * PI);

	new_point.x = sin (*theta) * radius_average;
	new_point.y = cos (*theta) * radius_average;
	/*
	end_0.x = center_point.x - new_point.x;
	end_0.y = center_point.y + new_point.y;
	end_1.x = center_point.x + new_point.x;
	end_1.y = center_point.y - new_point.y;
	*/
	end_0.x = center_point.x + new_point.x;
	end_0.y = center_point.y + new_point.y;
	end_1.x = center_point.x - new_point.x;
	end_1.y = center_point.y - new_point.y;
	quadrant = 0;

	if (verbose) {
		log += "\nBisection\n";
		log.add_formatted ("0\t%.8lf\t%.8lf\n", upstream_first [0].x, upstream_first [0].y);
		log.add_formatted ("1\t%.8lf\t%.8lf\n", upstream_first [1].x, upstream_first [1].y);
		log.add_formatted ("Radius\t%.8lf\n", radius_average);
		log.add_formatted ("Diff\t%.8lf\t%.8lf\n", diff1.x, diff1.y);
		log.add_formatted ("Angle\t%.4lf\t%d\n", *theta, quadrant);
		log.add_formatted ("Newdiff\t%.8lf\t%.8lf\n", new_point.x, new_point.y);
		log.add_formatted ("Result\t%.8lf\t%.8lf\tFacing Upstream Left\n", end_0.x, end_0.y);
		log.add_formatted ("\t%.8lf\t%.8lf\tFacing Upstream Right\n", end_1.x, end_1.y);
	}
}

void bisection_definition::create_bisection_3pt
	(const logical_coordinate upstream_first [3],
	const double radius_average,
	double *mid_theta,
	const bool verbose,
	dynamic_string &log)

// points must be in order 0 is upstream, highest # is downstream

// Left side of 0->1 vector is upstream
// Right side is downstream

// Perpendicular computed by adding PI/2

{
	double theta [2];
	int quadrant;
	logical_coordinate diff1, diff2, new_point;

	center_point = upstream_first [1];

	// find angle on the left side going up
	diff1.x = center_point.x - upstream_first [0].x;
	diff1.y = center_point.y - upstream_first [0].y;
	diff2.x = upstream_first [2].x - center_point.x;
	diff2.y = upstream_first [2].y - center_point.y;
			
	if (diff1.x != 0.0)
		theta [0] = atan (diff1.x / diff1.y);
	else
		// vertical
		theta [0] = 0;

	if (diff2.x != 0.0)
		theta [1] = atan (diff2.x / diff2.y);
	else
		// vertical
		theta [1] = 0;

	// normalize thetas with quadrant
	find_quadrant (diff1, &theta [0]);
	find_quadrant (diff2, &theta [1]);

	/*
	*mid_theta = (theta [0] + theta [1]) / 2;
	quadrant = ((int) floor ((*mid_theta + (2 * PI)) / (PI / 2.0)) % 4) + 1;

	// radius = offset_longitude_meters (center_point, buffer_radius_m);

	apply_offset (quadrant, &end_0, &end_1, *mid_theta, radius_average, center_point, &new_point);
	*/

	*mid_theta = (theta [0] + theta [1]) / 2;
	if (abs (theta [1] - theta [0]) > PI)
		// 2016-02-24 Thetas are 306 & 10. 
		// Mathematical average is 159 but that reverses course
		// We need a direction that's between the two thetas
		// Could use average of 306 & 370
		*mid_theta += PI;

	// perpendicular
	*mid_theta += PI / 2.0;

	// normalize
	if (*mid_theta < 0)
		*mid_theta += 2.0 * PI;
	else
		*mid_theta = fmod (*mid_theta, 2.0 * PI);

	new_point.x = sin (*mid_theta) * radius_average;
	new_point.y = cos (*mid_theta) * radius_average;
	end_0.x = center_point.x + new_point.x;
	end_0.y = center_point.y + new_point.y;
	end_1.x = center_point.x - new_point.x;
	end_1.y = center_point.y - new_point.y;
	quadrant = 0;

	if (verbose) {
		log += "\nBisection\n";
		log.add_formatted ("0\t%.8lf\t%.8lf\n", upstream_first [0].x, upstream_first [0].y);
		log.add_formatted ("1\t%.8lf\t%.8lf\n", upstream_first [1].x, upstream_first [1].y);
		log.add_formatted ("2\t%.8lf\t%.8lf\n", upstream_first [2].x, upstream_first [2].y);
		log.add_formatted ("Radius\t%.8lf\n", radius_average);
		log.add_formatted ("Diff1\t%.8lf\t%.8lf\n", diff1.x, diff1.y);
		log.add_formatted ("Diff2\t%.8lf\t%.8lf\n", diff2.x, diff2.y);
		log.add_formatted ("Angles\t%.4lf\t%.4lf\t%.4lf\t%d\n", theta [0], theta [1], *mid_theta, quadrant);
		log.add_formatted ("Newdiff\t%.8lf\t%.8lf\n", new_point.x, new_point.y);
		log.add_formatted ("Result\t%.8lf\t%.8lf\tFacing Upstream Left\n", end_0.x, end_0.y);
		log.add_formatted ("\t%.8lf\t%.8lf\tFacing Upstream Right\n", end_1.x, end_1.y);
	}
}

bool bisection_definition::set_upstream_downstream
	(const logical_coordinate &intersection) const

// Returns a boolean value for the section of buffer we're leaving as we cross the intersection

// Buffer polygons must have a counter-clockwise order

// 'Left' side of cutoff line (0->1) is always upstream
// 'Right' side of cutoff line (0->1) is always downstream

// If intersection is between 0 and river, crossing from upstream to downstream
// If intersection is between river and 1, crossing from downstream to upstream

{
	if (end_0.x < end_1.x) {
		if (intersection.x < center_point.x)
			// intersection is between 0 and river
			return true;
		else
			// intersection is between river and 1
			return false;
	}
	else
		if (end_0.x > end_1.x) {
			if (intersection.x > center_point.x)
				// intersection is between 0 and river
				return true;
			else
				// intersection is between river and 1
				return false;
		}
		else
			// vertical line, look at Y instead of X
			if (end_0.y < end_1.y) {
				if (intersection.y < center_point.y)
					// intersection is between 0 and river
					return true;
				else
					// intersection is between river and 1
					return false;
			}
			else
				if (intersection.y > center_point.y)
					// intersection is between 0 and river
					return true;
				else
					// intersection is between river and 1
					return false;
}

bool ClipperBuffer::create_threaded
	(const map_layer *river,
	map_layer *layer_buffer,
	const int thread_count,
	const double buffer_radius_m,
	const bool endtype_closed_polygon,
	interface_window *view,
	dynamic_string &log)

{
	int segment_index;
	std::vector <map_object *>::const_iterator segment;
	map_object *copy;
	_timeb start, end;
	float elapsed_milliseconds;
	double offset_radius_average;
	logical_coordinate offset_point;
	thread_manager threader;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator completed_job;
	thread_work_create_buffer *job;
	thread_manager_statistics stats;
	bool error = false;

	segment = river->objects.begin ();

	{
		double offset_radius_x, offset_radius_y;
		long latitude, longitude;

		(*segment)->centroid (&latitude, &longitude);
		offset_point.set (longitude, latitude);
		offset_radius_x = offset_longitude_meters (offset_point, buffer_radius_m) * 1.0e6;
		offset_radius_y = offset_latitude_meters (offset_point, buffer_radius_m) * 1.0e6;
		offset_radius_average = (offset_radius_x + offset_radius_y) / 2.0;
	}

	// 2022-01-12 Negative offset for polygon reduction
	if ((buffer_radius_m < 0.0)
	&& (offset_radius_average > 0.0))
		offset_radius_average *= -1.0;

	/*
	segment_size = (int) river->objects.size () / thread_count;
	if (segment_size * thread_count != (int) river->objects.size ())
		segment_size += 1;
	*/

	for (segment = river->objects.begin (), segment_index = 0;
	segment != river->objects.end ();
	++segment, ++segment_index) {
		job = new thread_work_create_buffer;
		job->id = segment_index + 1;
		job->work_function = &work_create_buffer;
		job->polygon = *segment;
		job->offset_radius_average = offset_radius_average;
		job->endtype_closed = endtype_closed_polygon;
		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	_ftime_s (&start);

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = thread_wait_interval_ms; // 5;
	stats.reset (thread_count);
	threader.statistics = &stats;

	view->update_progress ("Starting threads", 0);
	threader.run_tiny (&jobs, view, log);

	view->update_progress ("Adding thread layers to map layer", 0);

	int object_count, object_number;
	for (completed_job = jobs.begin ();
	completed_job != jobs.end ();
	++completed_job) {
		view->update_progress_formatted (1, "Job %ld", completed_job->first);


		object_count = ((thread_work_create_buffer *) completed_job->second)->layer_buffer.objects.size ();
		for (segment = ((thread_work_create_buffer *) completed_job->second)->layer_buffer.objects.begin (), object_number = 1;
		segment != ((thread_work_create_buffer *) completed_job->second)->layer_buffer.objects.end ();
		++segment, ++object_number) {
			view->update_progress_formatted (2, "Segment %ld %d/%d", (*segment)->id, object_number, object_count);
			copy = layer_buffer->create_new ((*segment)->type);
			copy->copy (*segment, layer_buffer);
			layer_buffer->objects.push_back (copy);
		}

		log += completed_job->second->log;
		completed_job->second->log.clear ();
	}


	log.add_formatted ("Number of segments to buffer\t%d\n", river->objects.size ());
	log.add_formatted ("Number of buffers\t%d\n", layer_buffer->objects.size ());

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

bool ClipperBuffer::create
	(const map_layer *river,
	map_layer *layer_buffer,
	interface_window *view,
	dynamic_string &)

{
	std::vector <map_object *> ::const_iterator segment;
	int index;
	ClipperLib::Path p;
	ClipperLib::Paths all_p, solution;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	map_polygon *outline;
	long count;
	logical_coordinate offset_point;
	double buffer_radius_m = 1000.0;
	double offset_radius_x, offset_radius_y, offset_radius_average;
	ClipperLib::ClipperOffset co = ClipperLib::ClipperOffset();

	segment = river->objects.begin ();
	offset_point.set (((map_polygon *) (* segment))->nodes [0], ((map_polygon *) (* segment))->nodes [1]);
	offset_radius_x = offset_longitude_meters (offset_point, buffer_radius_m) * 1.0e6;
	offset_radius_y = offset_latitude_meters (offset_point, buffer_radius_m) * 1.0e6;
	offset_radius_average = (offset_radius_x + offset_radius_y) / 2.0;

	view->update_progress ("Copying Points", 1);

	count = 0;

	// Fill paths from river layer
	for (segment = river->objects.begin ();
	segment != river->objects.end ();
	++segment) {
		// if (((*segment)->id == 20800677)
		// || ((*segment)->id == 20800675))
		p.clear ();
		p.reserve (((map_polygon *) (* segment))->node_count);

		for (index = 0;
		index < ((map_polygon *) (* segment))->node_count;
		++index)
			p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((map_polygon *) (* segment))->nodes [index * 2],  (ClipperLib::cInt) ((map_polygon *) (* segment))->nodes [index * 2 + 1]));

		all_p.push_back (p);

		co.Clear ();
		co.AddPaths (all_p, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etOpenRound);
		solution.clear();
		co.Execute (solution, offset_radius_average);

		for (paths_path = solution.begin ();
		paths_path != solution.end ();
		++paths_path) {
			outline = (map_polygon *) layer_buffer->create_new (MAP_OBJECT_POLYGON);
			outline->id = ++count;
			outline->node_count = (long) paths_path->size ();
			outline->nodes = new long [outline->node_count * 2];
			for (path_point = paths_path->begin (), index = 0;
			path_point != paths_path->end ();
			++path_point, ++index) {
				outline->nodes [index * 2] = (long) path_point->X;
				outline->nodes [index * 2 + 1] = (long) path_point->Y;
			}
			layer_buffer->objects.push_back (outline);
		}
		if (count % 100 == 0)
			view->update_progress_formatted (2, "Buffers created %ld", count);
	}

	/*
	view->update_progress ("Offsetting", 1);

	ClipperLib::ClipperOffset co = ClipperLib::ClipperOffset();
	co.AddPaths (all_p, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etOpenRound);
	solution.clear();
	co.Execute (solution, offset_radius_average);

	view->update_progress ("Copying Points", 1);

	// Fill buffer layer from solution
	for (paths_path = solution.begin (), count = 0;
	paths_path != solution.end ();
	++paths_path) {
		outline = (map_polygon *) layer_buffer->create_new (MAP_OBJECT_POLYGON);
		outline->id = ++count;
		outline->node_count = paths_path->size ();
		outline->nodes = new long [outline->node_count * 2];
		for (path_point = paths_path->begin (), index = 0;
		path_point != paths_path->end ();
		++path_point, ++index) {
			outline->nodes [index * 2] = (long) path_point->X;
			outline->nodes [index * 2 + 1] = (long) path_point->Y;
		}
		layer_buffer->objects.push_back (outline);
		outline->dump (layer_buffer, log);
	}
	// solution.begin ();
	return solution.size() == 2;
	*/

	return true;
}

	
bool ClipperBuffer::merge_threaded
	(const map_layer *layer_buffer,
	map_layer *combined_buffer,
	const int thread_count,
	interface_window *view,
	dynamic_string &log)

// combine each polygon into a single large polygon

{
	int segment_size;
	int thread_index, segment_index, complete_count;
	std::future <bool> *threads;
	map_layer *workload, *results;
	std::vector <map_object *>::const_iterator segment;
	bool finished, *done;
	map_object *copy;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false;

	segment = layer_buffer->objects.begin ();

	segment_size = (int) layer_buffer->objects.size () / thread_count;
	if (segment_size * thread_count != (int) layer_buffer->objects.size ())
		segment_size += 1;

	_ftime_s (&start);

	workload = new map_layer [thread_count];
	results = new map_layer [thread_count];
	threads = new std::future <bool> [thread_count];
	done = new bool [thread_count];

	view->update_progress ("Starting threads", 0);
	for (thread_index = 0; thread_index < thread_count; ++thread_index) {
		done [thread_index] = false;

		workload [thread_index].type = layer_buffer->type;
		workload [thread_index].attribute_count_numeric = layer_buffer->attribute_count_numeric;
		workload [thread_index].attribute_count_text = layer_buffer->attribute_count_text;

		for (segment_index = 0;
		(segment != layer_buffer->objects.end ())
		&& (segment_index < segment_size);
		++segment_index) {
			copy = workload [thread_index].create_new ((*segment)->type);
			copy->copy (*segment, &workload [thread_index]);
			// if (copy->id == 341090001)
			workload [thread_index].objects.push_back (copy);
			++segment;
		}

		threads [thread_index] = std::async (std::launch::async, work_combine_polygons, &workload [thread_index],
		&results [thread_index]);
	}

	view->update_progress ("Waiting for threads", 0);
	finished = false;
	complete_count = 0;
	while (!finished) {
		finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			if (!done [thread_index]) {
				finished = false;
				if (threads [thread_index].wait_for (std::chrono::milliseconds (100)) == std::future_status::ready) {
					done [thread_index] = true;
					++complete_count;
					view->update_progress_formatted (1, "%d of %d complete", complete_count, thread_count);
				}
			}
		}
	}

	delete [] threads;
	delete [] workload;

	view->update_progress ("Adding thread layers to map layer", 0);
	for (thread_index = 0; thread_index < thread_count; ++thread_index) {
		for (segment = results [thread_index].objects.begin ();
		segment != results [thread_index].objects.end ();
		++segment) {
			copy = combined_buffer->create_new ((*segment)->type);
			copy->copy (*segment, layer_buffer);
			combined_buffer->objects.push_back (copy);
		}
	}

	delete [] results;

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

bool ClipperBuffer::merge_segment
	(map_polygon *segment,
	ClipperLib::Paths *collective)

// Add this segment to collective

{
	int index;
	ClipperLib::Path p;
	ClipperLib::Clipper co = ClipperLib::Clipper();
	ClipperLib::Paths clip, solution;

	// Add each remaining segments to collective
	for (index = 0;
	index < segment->node_count;
	++index)
		p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) segment->nodes [index * 2],  (ClipperLib::cInt) segment->nodes [index * 2 + 1]));
	clip.push_back (p);

	co.Clear ();
	co.AddPaths (*collective, ClipperLib::ptSubject, true);
	co.AddPaths (clip, ClipperLib::ptClip, true);
	solution.clear();
	co.Execute (ClipperLib::ctUnion, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	if (solution.size () > 0) {
		// if no overlap, just keep collective as-is
		*collective = solution;
		return true;
	}
	else
		return false;
}

bool ClipperBuffer::merge
	(const map_layer *layer_buffer,
	map_layer *layer_combined,
	interface_window *view,
	dynamic_string &log)

// combine each polygon into a single large polygon

{
	std::vector <map_object *>::const_iterator segment;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false;
	ClipperLib::Clipper co = ClipperLib::Clipper();
	ClipperLib::Path p;
	ClipperLib::Paths subject, clip, solution, collective;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	map_polygon *outline;
	int index;
	long count_combined = 0;

	_ftime_s (&start);

	view->update_progress ("Starting intersect", 0);

	// Combine the first two segments, 
	// then add each subsequent segment

	for (segment = layer_buffer->objects.begin ();
	segment != layer_buffer->objects.end ();
	++segment) {

		subject.clear ();
		clip.clear ();
		p.clear ();
		p.reserve (((map_polygon *) (* segment))->node_count);

		if (++count_combined == 1) {
			// Combine the first 2 segments
			std::vector <map_object *>::const_iterator segment_2;

			for (index = 0;
			index < ((map_polygon *) *segment)->node_count;
			++index)
				p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((map_polygon *) *segment)->nodes [index * 2],  (ClipperLib::cInt) ((map_polygon *) *segment)->nodes [index * 2 + 1]));
			subject.push_back (p);

			segment_2 = segment;
			++segment_2;

			for (index = 0;
			index < ((map_polygon *) *segment_2)->node_count;
			++index)
				p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((map_polygon *) *segment_2)->nodes [index * 2],  (ClipperLib::cInt) ((map_polygon *) *segment_2)->nodes [index * 2 + 1]));
			clip.push_back (p);
			segment = segment_2;

			co.Clear ();
			co.AddPaths (subject, ClipperLib::ptSubject, true);
			co.AddPaths (clip, ClipperLib::ptClip, true);
			collective.clear();
			co.Execute (ClipperLib::ctUnion, collective, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
		}
		else
			merge_segment ((map_polygon *) *segment, &collective);
	}

	for (paths_path = collective.begin ();
	paths_path != collective.end ();
	++paths_path) {
		outline = (map_polygon *) layer_combined->create_new (MAP_OBJECT_POLYGON);
		outline->id = 1;
		outline->node_count = (long) paths_path->size ();
		outline->nodes = new long [outline->node_count * 2];
		for (path_point = paths_path->begin (), index = 0;
		path_point != paths_path->end ();
		++path_point, ++index) {
			outline->nodes [index * 2] = (long) path_point->X;
			outline->nodes [index * 2 + 1] = (long) path_point->Y;
		}
		layer_combined->objects.push_back (outline);
	}


	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

void ClipperBuffer::add_to_path
	(ClipperLib::Path *p,
	const long x,
	const long y) const

{
	/*
	if (UTMZoneNumber != 0) {
		double northing, easting;
		ClipperLib::cInt northing_long, easting_long;

		LLtoUTM (23, UTMZoneNumber, (double) y / 1.0e6, (double) x / 1.0e6, northing, easting);
		ASSERT ((northing > 0.0) && (easting > 0.0));
		northing_long = (ClipperLib::cInt) (northing * UTM_METERS_FACTOR);
		easting_long = (ClipperLib::cInt) (easting * UTM_METERS_FACTOR);
		p->push_back (ClipperLib::IntPoint (easting_long, northing_long));
	}
	else
	*/
		p->push_back (ClipperLib::IntPoint((ClipperLib::cInt) x,  (ClipperLib::cInt) y));
}

void ClipperBuffer::add_to_path
	(ClipperLib::Path *p,
	const device_coordinate &point) const

{
	p->push_back (ClipperLib::IntPoint((ClipperLib::cInt) point.x,  (ClipperLib::cInt) point.y));
}

double ClipperBuffer::average_offset
	(const map_layer *layer_rivers,
	const long comid,
	const double buffer_radius_m,
	dynamic_string &log) const

// Compute radius for this location

{
	map_object *river;
	double offset_radius_x, offset_radius_y;
	logical_coordinate offset_point;

	double offset_radius_average = 0.0;

	// Set offset_radius_average from specified segment
	if ((river = layer_rivers->match_id (comid)) != NULL) {

		if (river->type == MAP_OBJECT_POLYGON) {
			offset_point.set (((map_polygon *) river)->nodes [0], ((map_polygon *) river)->nodes [1]);
			offset_radius_x = offset_longitude_meters (offset_point, buffer_radius_m) * 1.0e6;
			offset_radius_y = offset_latitude_meters (offset_point, buffer_radius_m) * 1.0e6;
			offset_radius_average = (offset_radius_x + offset_radius_y) / 2.0;
		}
		else {
			map_polygon *polygon;
			int polygon_index = 0;
			polygon = &((map_polygon_complex *) river)->polygons [polygon_index];
			offset_point.set (polygon->nodes [0], polygon->nodes [1]);
			offset_radius_x = offset_longitude_meters (offset_point, buffer_radius_m) * 1.0e6;
			offset_radius_y = offset_latitude_meters (offset_point, buffer_radius_m) * 1.0e6;
			offset_radius_average = (offset_radius_x + offset_radius_y) / 2.0;
		}
	}
	else
		log.add_formatted ("ERROR, no river segment for comid %ld.\n", comid);

	return offset_radius_average;
}

bool ClipperBuffer::create_paths
	(const map_layer *layer_rivers,
	map_layer *layer_river_points,
	std::vector <long long> *upstream_ids,  // Order is ascending upstream
	const double length_km,
	ClipperLib::Path *p,
	dynamic_string &log) const

// Accumulate river segment points until at or just above length_km is reached

{
	int index, polygon_index;
	std::vector <long long>::const_iterator id;
	map_object *river;
	map_polygon *polygon, *upstream_map_object = NULL;
	double length_m = 0.0;
	bool done = false, error = false;
	logical_coordinate offset_point;
	long last_latitude = 0, last_longitude = 0;

	length_m = 0.0; // upstream_within_segment_m has already been subtracted from length_km

	// Add each remaining segments to collective.

	if ((id = upstream_ids->begin ()) != upstream_ids->end ()) {
		// First entry in upstream_ids is closest segment, so don't add points from it.
		// upstream points for closest segment are already in path
		++id;
		while (!done
		&& (id != upstream_ids->end ())) {
			if ((river = layer_rivers->match_id (*id)) != NULL) {

				// 2016-03-24 put duplicate segments in layer_river_points so they know whether they're upstream or downstream
				if (layer_river_points) {
					upstream_map_object = (map_polygon *) layer_river_points->create_new (MAP_OBJECT_POLYGON);
					upstream_map_object->id = river->id;
					upstream_map_object->attributes_numeric [0] = 1.0;
					layer_river_points->objects.push_back (upstream_map_object);
				}

				if (river->type == MAP_OBJECT_POLYGON) {

					// working upstream from highest node# towards 0
					for (index = ((map_polygon *) river)->node_count - 1;
					!done && (index >= 0);
					--index) {

						// Aaron, 2018-08-28: Check length first in case we're already beyond it (negative length_km)
						if (length_m >= (length_km * 1000.0))
							done = true;
						else {
							if (index < (((map_polygon *) river)->node_count - 1))
								length_m += distance_meters (last_latitude, last_longitude, ((map_polygon *) river)->nodes [index * 2 + 1], ((map_polygon *) river)->nodes [index * 2]);

							add_to_path (p, ((map_polygon *) river)->nodes [index * 2], ((map_polygon *) river)->nodes [index * 2 + 1]);

							if (upstream_map_object)
								upstream_map_object->add_point (((map_polygon *) river)->nodes [index * 2 + 1], ((map_polygon *) river)->nodes [index * 2]);

							last_latitude = ((map_polygon *) river)->nodes [index * 2 + 1];
							last_longitude = ((map_polygon *) river)->nodes [index * 2];
						}
					}
				}
				else
					// complex polygon
					// Polygons are number 0..n from upstream to downstream
					for (polygon_index = ((map_polygon_complex *) river)->polygon_count - 1;
					!done
					&& (polygon_index >= 0);
					--polygon_index) {

						polygon = &((map_polygon_complex *) river)->polygons [polygon_index];

						for (index = (polygon->node_count - 1);
						!done && (index >= 0);
						--index) {
							// Aaron, 2018-08-28: Check length first in case we're already beyond it (negative length_km)
							if (length_m >= (length_km * 1000.0))
								done = true;
							else {
								if (index < (polygon->node_count - 1))
									length_m += distance_meters (last_latitude, last_longitude, polygon->nodes [index * 2 + 1], polygon->nodes [index * 2]);

								add_to_path (p, polygon->nodes [index * 2], polygon->nodes [index * 2 + 1]);

								if (upstream_map_object)
									upstream_map_object->add_point (polygon->nodes [index * 2 + 1], polygon->nodes [index * 2]);

								last_latitude = polygon->nodes [index * 2 + 1];
								last_longitude = polygon->nodes [index * 2];
							}
						}

					}
			}
			else {
				error = true;
				log.add_formatted ("ERROR, segment id %ld not found in river layer in ClipperBuffer.create_paths.\n", *id);
			}
			++id;
		}
	}
	else {
		log += "ERROR, no upstream ids.  offset_radius_average not computed.\n";
		error = true;
	}
	return !error;
}

bool ClipperBuffer::add_upstream_within_segment
	(map_object *river,
	const int nearest_polygon_index,
	const int nearest_point_index,
	double *perimeter_meters,
	const double limit_meters,
	ClipperLib::Path *p,
	map_layer *layer_river_points,
	dynamic_string &log) const

 // nodes are ordered low-to-high in direction of water flow

 // Add points to p from nearest point upward towards point 0
// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (0, nearest_point_index, &upstream_within_segment_m, log);

{
	int point_index;
	bool error = false;
	device_coordinate point1, point2;
	map_polygon *upstream_map_object = NULL;
	*perimeter_meters = 0;

	if (layer_river_points) {
		upstream_map_object = (map_polygon *) layer_river_points->create_new (MAP_OBJECT_POLYGON);
		upstream_map_object->id = river->id;
		upstream_map_object->attributes_numeric [0] = 1.0;
		layer_river_points->objects.push_back (upstream_map_object);
	}

	if (river->type == MAP_OBJECT_POLYGON) {
		if (nearest_point_index < ((map_polygon *) river)->node_count) {

			// 2016-10-11 Must add point_index 0 to upstream_map_object
			point1 = ((map_polygon *) river)->point_at_index (nearest_point_index);
			add_to_path (p, point1);
			if (upstream_map_object)
				upstream_map_object->add_point (point1.y, point1.x);

			for (point_index = nearest_point_index;
			(point_index > 0)
			&& (*perimeter_meters < limit_meters);
			--point_index) {

				point1 = ((map_polygon *) river)->point_at_index (point_index);
				point2 = ((map_polygon *) river)->point_at_index (point_index - 1);
				*perimeter_meters += distance_meters (point1, point2);
				add_to_path (p, point2);
				if (upstream_map_object)
					upstream_map_object->add_point (point2.y, point2.x);
			}
		}
		else {
			log.add_formatted ("ERROR, ID %ld perimeter index too high: %d / %d.\n", river->id, nearest_point_index, ((map_polygon *) river)->total_number_of_points ());
			error = true;
		}
	}
	else {
		int polygon_index;
		map_polygon *polygon;

		// add points from nearerst_polygon_index
		polygon_index = nearest_polygon_index;
		polygon = &((map_polygon_complex *) river)->polygons [polygon_index];

		// 2016-10-11 Must add point_index 0 to upstream_map_object
		// add_to_path (p, polygon->point_at_index (nearest_point_index));
		point1 = polygon->point_at_index (nearest_point_index);
		add_to_path (p, point1);
		if (upstream_map_object)
			upstream_map_object->add_point (point1.y, point1.x);

		for (point_index = nearest_point_index;
		(point_index > 0)
		&& (*perimeter_meters < limit_meters);
		--point_index) {
			// distance from nodes at point_index to nodes at (point_index - 1)
			point1 = polygon->point_at_index (point_index);
			point2 = polygon->point_at_index (point_index - 1);
			*perimeter_meters += distance_meters (point1, point2);

			add_to_path (p, point2);
			if (upstream_map_object)
				upstream_map_object->add_point (point2.y, point2.x);
		}

		// add points from polygons between nearest_polygon_index and polygon index 0
		for (polygon_index = nearest_polygon_index - 1;
		(polygon_index >= 0)
		&& (*perimeter_meters < limit_meters);
		--polygon_index) {
			polygon = &((map_polygon_complex *) river)->polygons [polygon_index];
			add_to_path (p, polygon->point_at_index (0));
			for (point_index = polygon->node_count - 1;
			(point_index > 0)
			&& (*perimeter_meters < limit_meters);
			--point_index) {
				point1 = polygon->point_at_index (point_index);
				point2 = polygon->point_at_index (point_index - 1);
				*perimeter_meters += distance_meters (point1, point2);

				add_to_path (p, point2);
				if (upstream_map_object)
					upstream_map_object->add_point (point2.y, point2.x);
			}
		}

	}

	return !error;
}

bool ClipperBuffer::add_downstream_within_segment
	(map_object *river,
	const int nearest_polygon_index,
	const int nearest_point_index,
	double *distance_downstream_m,
	const double limit_meters,
	ClipperLib::Path *p,
	map_layer *layer_river_points,
	dynamic_string &log) const

// Add points to p from nearest point downstream to limit or end
// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (nearest_point_index, ((map_polygon *) nearest_segment->second)->total_number_of_points () - 1, &downstream_within_segment_m, log);

{
	int point_set;
	bool error = false;
	map_polygon *downstream_map_object = NULL;
	*distance_downstream_m = 0;
	device_coordinate point1, point2;

	if (layer_river_points) {
		downstream_map_object = (map_polygon *) layer_river_points->create_new (MAP_OBJECT_POLYGON);
		downstream_map_object->id = river->id;
		downstream_map_object->attributes_numeric [0] = 2.0;
		layer_river_points->objects.push_back (downstream_map_object);
	}

	if (river->type == MAP_OBJECT_POLYGON) {

		// 2016-10-11 Must add point_index 0 to downstream_map_object
		// add_to_path (p, ((map_polygon *) river)->point_at_index (nearest_point_index));
		point1 = ((map_polygon *) river)->point_at_index (nearest_point_index);
		add_to_path (p, point1);
		if (downstream_map_object)
			downstream_map_object->add_point (point1.y, point1.x);

		for (point_set = nearest_point_index;
		(point_set < (((map_polygon *) river)->node_count - 1))
		&& (*distance_downstream_m < limit_meters);
		++point_set) {
			point1 = ((map_polygon *) river)->point_at_index (point_set);
			point2 = ((map_polygon *) river)->point_at_index (point_set + 1);
			*distance_downstream_m += distance_meters (point1, point2);
			add_to_path (p, point2);
			if (downstream_map_object)
				downstream_map_object->add_point (point2.y, point2.x);
		}
	}
	else {
		int polygon_index;
		map_polygon *polygon;

		// add points starting at nearest point in nearest polygon
		polygon_index = nearest_polygon_index;
		polygon = &((map_polygon_complex *) river)->polygons [polygon_index];

		// 2016-10-11 Must add point_index 0 to downstream_map_object
		// add_to_path (p, polygon->point_at_index (nearest_point_index));
		point1 = polygon->point_at_index (nearest_point_index);
		add_to_path (p, point1);
		if (downstream_map_object)
			downstream_map_object->add_point (point1.y, point1.x);

		for (point_set = nearest_point_index;
		(point_set < (polygon->node_count - 1))
		&& (*distance_downstream_m < limit_meters);
		++point_set) {
			point1 = polygon->point_at_index (point_set);
			point2 = polygon->point_at_index (point_set + 1);
			*distance_downstream_m += distance_meters (point1, point2);
			add_to_path (p, point2);
			if (downstream_map_object)
				downstream_map_object->add_point (point2.y, point2.x);
		}

		// add points in remaining polygons
		for (polygon_index = nearest_polygon_index + 1;
		(polygon_index < ((map_polygon_complex *) river)->polygon_count)
		&& (*distance_downstream_m < limit_meters);
		++polygon_index) {
			polygon = &((map_polygon_complex *) river)->polygons [polygon_index];
			add_to_path (p, polygon->point_at_index (0));
			for (point_set = 0;
			(point_set < (polygon->node_count - 1))
			&& (*distance_downstream_m < limit_meters);
			++point_set) {
				point1 = polygon->point_at_index (point_set);
				point2 = polygon->point_at_index (point_set + 1);
				*distance_downstream_m += distance_meters (point1, point2);
				add_to_path (p, point2);

				if (downstream_map_object)
					downstream_map_object->add_point (point2.y, point2.x);
			}
		}
	}

	return !error;
}

bool ClipperBuffer::look_downstream_within_segment
	(const map_object *river,
	const int nearest_polygon_index,
	const int nearest_point_index,
	const double limit_meters,
	double *distance_downstream_m,
	int *polygon_index,
	int *point_index,
	dynamic_string &log) const

// Same as add_upstream_within_segment, but doesn't create clipper path

{
	int point_set;
	bool error = false;
	*distance_downstream_m = 0;
	device_coordinate point1, point2;

	if (river->type == MAP_OBJECT_POLYGON) {

		for (point_set = nearest_point_index;
		(point_set < (((map_polygon *) river)->node_count - 1))
		&& (*distance_downstream_m < limit_meters);
		++point_set) {
			point1 = ((map_polygon *) river)->point_at_index (point_set);
			point2 = ((map_polygon *) river)->point_at_index (point_set + 1);
			*distance_downstream_m += distance_meters (point1, point2);
		}
	}
	else {
		int poly_index;
		map_polygon *polygon;

		// add points starting at nearest point in nearest polygon
		poly_index = nearest_polygon_index;
		polygon = &((map_polygon_complex *) river)->polygons [poly_index];

		for (point_set = nearest_point_index;
		(point_set < (polygon->node_count - 1))
		&& (*distance_downstream_m < limit_meters);
		++point_set) {
			point1 = polygon->point_at_index (point_set);
			point2 = polygon->point_at_index (point_set + 1);
			*distance_downstream_m += distance_meters (point1, point2);
		}

		// add points in remaining polygons
		for (poly_index = nearest_polygon_index + 1;
		(poly_index < ((map_polygon_complex *) river)->polygon_count)
		&& (*distance_downstream_m < limit_meters);
		++poly_index) {
			polygon = &((map_polygon_complex *) river)->polygons [poly_index];

			for (point_set = 0;
			(point_set < (polygon->node_count - 1))
			&& (*distance_downstream_m < limit_meters);
			++point_set) {
				point1 = polygon->point_at_index (point_set);
				point2 = polygon->point_at_index (point_set + 1);
				*distance_downstream_m += distance_meters (point1, point2);
			}
		}
	}

	return !error;
}

bool ClipperBuffer::look_upstream_within_segment
	(const map_object *river,
	const int nearest_polygon_index,
	const int nearest_point_index,
	const double limit_km,
	double *distance_within_segment_m,
	int *limit_polygon_index,
	int *limit_point_index,
	dynamic_string &log) const

 // Same as add_upstream_within_segment, but doesn't create clipper path

{
	int point_set;
	bool limit_reached = false;
	device_coordinate point1, point2;
	*distance_within_segment_m = 0;

	if (river->type == MAP_OBJECT_POLYGON) {
		if (nearest_point_index < ((map_polygon *) river)->node_count) {

			for (point_set = nearest_point_index;
			(point_set > 0)
			&& (*distance_within_segment_m < (limit_km * 1000.0));
			--point_set) {
				point1 = ((map_polygon *) river)->point_at_index (point_set);
				point2 = ((map_polygon *) river)->point_at_index (point_set - 1);
				*distance_within_segment_m += distance_meters (point1, point2);
			}

			if (*distance_within_segment_m >= (limit_km * 1000.0)) {
				limit_reached = true;
				*limit_polygon_index = 0;
				*limit_point_index = point_set - 1;
			}
		}
	}
	else {
		int polygon_index;
		map_polygon *polygon;

		// add points from nearerst_polygon_index
		polygon_index = nearest_polygon_index;
		polygon = &((map_polygon_complex *) river)->polygons [polygon_index];

		for (point_set = nearest_point_index;
		(point_set > 0)
		&& (*distance_within_segment_m < (limit_km * 1000.0));
		--point_set) {
			point1 = polygon->point_at_index (point_set);
			point2 = polygon->point_at_index (point_set - 1);
			*distance_within_segment_m += distance_meters (point1, point2);
		}

		if (*distance_within_segment_m >= (limit_km * 1000.0)) {
			limit_reached = true;
			*limit_polygon_index = 0;
			*limit_point_index = point_set - 1;
		}
		else {
			// traverse points from polygons between nearest_polygon_index and polygon index 0
			for (polygon_index = nearest_polygon_index - 1;
			(polygon_index >= 0)
			&& !limit_reached;
			--polygon_index) {
				polygon = &((map_polygon_complex *) river)->polygons [polygon_index];
				for (point_set = polygon->node_count - 1;
				(point_set >= 0)
				&& (*distance_within_segment_m < (limit_km * 1000.0));
				--point_set) {

					point1 = polygon->point_at_index (point_set);
					point2 = polygon->point_at_index (point_set - 1);
					*distance_within_segment_m += distance_meters (point1, point2);
				}

				if (*distance_within_segment_m >= (limit_km * 1000.0)) {
					limit_reached = true;
					*limit_polygon_index = 0;
					*limit_point_index = point_set - 1;
				}
			}
		}

	}

	return limit_reached;
}

/*
bool ClipperBuffer::duplicate_closest_segment
	(const map_layer *layer_rivers,
	map_layer *layer_river_points,
	const long comid) const

// List of downstream points ends with nearest river segment to facility 
// Called by create_paths_reverse to add a map_object with points from nearest river segment to facility to layer_river_points

{
	map_object *river;
	map_polygon *downstream_map_object;
	int index;

	if ((river = layer_rivers->match_id (comid)) != NULL) {

		if (river->type == MAP_OBJECT_POLYGON) {
			downstream_map_object = (map_polygon *) layer_river_points->create_new (MAP_OBJECT_POLYGON);
			downstream_map_object->id = river->id;
			downstream_map_object->attributes_numeric [0] = 2.0;
			for (index = 0;
			index < ((map_polygon *) river)->node_count;
			++index)
				downstream_map_object->add_point (((map_polygon *) river)->nodes [index * 2 + 1], ((map_polygon *) river)->nodes [index * 2]);
			layer_river_points->objects.push_back (downstream_map_object);
		}
		else {
			int polygon_index;
			map_object *polygon;
			downstream_map_object = (map_polygon *) layer_river_points->create_new (MAP_OBJECT_POLYGON_COMPLEX);

			downstream_map_object->id = river->id;
			downstream_map_object->attributes_numeric [0] = 2.0;

			// complex polygon
			for (polygon_index = 0;
			polygon_index < ((map_polygon_complex *) river)->polygon_count;
			++polygon_index) {
				polygon = &((map_polygon_complex *) river)->polygons [polygon_index];
				((map_polygon_complex *) downstream_map_object)->add_polygon_coordinates_only ((map_polygon *) polygon, layer_river_points);
			}

			layer_river_points->objects.push_back (downstream_map_object);
		}
	}
}
*/

bool ClipperBuffer::create_paths_reverse
	(const map_layer *layer_rivers,
	map_layer *layer_river_points,
	const std::vector <long long> *downstream_ids_upward,
	// const double buffer_radius_m,
	const double length_km,
	ClipperLib::Path *p,
	dynamic_string &log) const

// Accumulate river segment points until at or just above length_km is reached

{
	int index, polygon_index;
	std::vector <long long>::const_reverse_iterator id;
	map_object *river;
	map_polygon *polygon, *downstream_map_object = NULL;
	double length_m = 0.0;
	bool done = false, error = false;
	long last_latitude = 0, last_longitude = 0;
	logical_coordinate offset_point;

	length_m = 0.0; // downstream_within_segment_m has already been subtracted from length_km

	// Add each remaining segments to collective

	for (id = downstream_ids_upward->rbegin ();
	!done
	&& (id != downstream_ids_upward->rend ());
	++id) {

		if (id == downstream_ids_upward->rbegin ()) {
			// First segment is nearest to facility, and we've already computed downstream_within_segment_m
			// so don't add to p
		}
		else {
			if ((river = layer_rivers->match_id (*id)) != NULL) {

				// 2016-03-24 put duplicate segments in layer_river_points so they know whether they're upstream or downstream
				if (layer_river_points) {
					downstream_map_object = (map_polygon *) layer_river_points->create_new (MAP_OBJECT_POLYGON);
					downstream_map_object->id = river->id;
					downstream_map_object->attributes_numeric [0] = 2.0;
					layer_river_points->objects.push_back (downstream_map_object);
				}

				if (river->type == MAP_OBJECT_POLYGON) {

					for (index = 0;
					!done && (index < ((map_polygon *) river)->node_count);
					++index) {
						// Aaron, 2018-08-28: Check length first in case we're already beyond it (negative length_km)
						if (length_m >= (length_km * 1000.0))
							done = true;
						else {
							if (index > 0)
								length_m += distance_meters(last_latitude, last_longitude, ((map_polygon *)river)->nodes[index * 2 + 1], ((map_polygon *)river)->nodes[index * 2]);

							add_to_path(p, ((map_polygon *)river)->nodes[index * 2], ((map_polygon *)river)->nodes[index * 2 + 1]);

							if (downstream_map_object)
								downstream_map_object->add_point(((map_polygon *)river)->nodes[index * 2 + 1], ((map_polygon *)river)->nodes[index * 2]);

							last_latitude = ((map_polygon *)river)->nodes[index * 2 + 1];
							last_longitude = ((map_polygon *)river)->nodes[index * 2];
						}
					}
				}
				else
					// complex polygon
					for (polygon_index = 0;
					!done
					&& (polygon_index < ((map_polygon_complex *) river)->polygon_count);
					++polygon_index) {
						polygon = &((map_polygon_complex *) river)->polygons [polygon_index];

						for (index = 0;
						!done && (index < polygon->node_count);
						++index) {
							// Aaron, 2018-08-28: Check length first in case we're already beyond it (negative length_km)
							if (length_m >= (length_km * 1000.0))
								done = true;
							else {
								if (index > 0)
									length_m += distance_meters (last_latitude, last_longitude, polygon->nodes[index * 2 + 1], polygon->nodes[index * 2]);

								add_to_path (p, polygon->nodes[index * 2], polygon->nodes[index * 2 + 1]);

								if (downstream_map_object)
									downstream_map_object->add_point (polygon->nodes[index * 2 + 1], polygon->nodes[index * 2]);

								last_latitude = polygon->nodes[index * 2 + 1];
								last_longitude = polygon->nodes[index * 2];

							}
						}
					}
			}
			else {
				error = true;
				log.add_formatted ("ERROR, segment id %ld not found in river layer in ClipperBuffer.create_paths_reverse.\n", *id);
			}
		}
	}
	// new_path->push_back (p);
	return !error;
}

bool ClipperBuffer::Paths_to_map_layer
	(const ClipperLib::Paths *trail,
	const long facility_id,
	const double buffer_width,
	const std::vector <dynamic_string> *tract_id_names,
	const std::vector <dynamic_string>* tract_names,
	map_layer *layer_buffer,
	const bool compute_area,
	interface_window *view,
	dynamic_string &log)

// Create map_layer polygons for each ClipperLib::Paths
// Assign id names to attributes_text [0]

// Closest point to facility is node_index 0 of first path
// This becomes node_index [0] of map_object with ID 1

{
	bool error = false;
	long count = 0;
	int index, area_attribute_index = 0;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector <dynamic_string>::const_iterator tract_id, tract_name;
	dynamic_string inconstant_name;
	map_polygon *outline;

	if ((tract_id_source_1 == CLIPPED_ID_SOURCE_ID)
	|| (tract_id_source_1 == CLIPPED_ID_SOURCE_ID_AND_NAME)
	|| (tract_id_source_1 == CLIPPED_ID_SOURCE_NUMERIC_ATTRIBUTE)
	|| (tract_id_source_1 == CLIPPED_ID_SOURCE_THREE_NAMES))
		++area_attribute_index;
	if (buffer_width_output_index != -1)
		++area_attribute_index;

	if ((tract_id_source_2 == CLIPPED_ID_SOURCE_ID)
	|| (tract_id_source_2 == CLIPPED_ID_SOURCE_ID_AND_NAME)
	|| (tract_id_source_2 == CLIPPED_ID_SOURCE_NUMERIC_ATTRIBUTE))
		++area_attribute_index;

	if (view)
		view->update_progress ("Copying Points", 1);

	if (tract_id_names)
		tract_id = tract_id_names->begin ();
	if (tract_names)
		tract_name = tract_names->begin();

	for (paths_path = trail->begin ();
	paths_path != trail->end ();
	++paths_path) {
		outline = (map_polygon *) layer_buffer->create_new (MAP_OBJECT_POLYGON);
		outline->attributes_numeric [0] = facility_id;
		if (buffer_width_output_index != -1)
			outline->attributes_numeric [buffer_width_output_index] = buffer_width;
		if (tract_id_names) {
			if ((tract_id_source_2 == CLIPPED_ID_SOURCE_ID)
			|| (tract_id_source_2 == CLIPPED_ID_SOURCE_ID_AND_NAME)) {
				inconstant_name = *tract_id;
				outline->attributes_numeric[1] = atof (inconstant_name.get_text_ascii ());
			}
			else
				outline->attributes_text[0] = *tract_id;
			++tract_id;
		}
		if (tract_names
		&& (tract_id_source_2 == CLIPPED_ID_SOURCE_ID_AND_NAME)) {
			// 2023-08-16 Need city name along with ID
			if (tract_id_source_1 == CLIPPED_ID_SOURCE_ID_AND_NAME)
				outline->attributes_text[1] = *tract_name;
			else
				outline->attributes_text[0] = *tract_name;
			++tract_name;
		}

		if (compute_area) {
			outline->attributes_numeric [area_attribute_index] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
			outline->attributes_numeric [area_attribute_index + 1] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);
			outline->attributes_numeric [area_attribute_index + 2] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
			outline->attributes_numeric [area_attribute_index + 3] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_RHUMB);
		}

		outline->id = ++count;
		outline->node_count = (long) paths_path->size ();
		outline->nodes = new long [outline->node_count * 2];

		for (path_point = paths_path->begin (), index = 0;
		path_point != paths_path->end ();
		++path_point, ++index) {
			/*
			if (UTMZoneNumber != 0) {
				char UTMZone [12];
				sprintf_s (UTMZone, 12, "%d%c", UTMZoneNumber, UTMLetter);

				northing = (double) path_point->X / UTM_METERS_FACTOR;
				easting = (double) path_point->Y / UTM_METERS_FACTOR;
				UTMtoLL (23, northing, easting, UTMZone, latitude, longitude);
				outline->nodes [index * 2 + 1] = (long) (latitude * 1.0e6);
				outline->nodes [index * 2] = (long) (longitude * 1.0e6);
			}
			else {
			*/
			outline->nodes [index * 2] = (long) path_point->X;
			outline->nodes [index * 2 + 1] = (long) path_point->Y;
		}
		layer_buffer->objects.push_back (outline);
	}

	return !error;
}

bool ClipperBuffer::Paths_to_map_layer
	(const ClipperLib::Paths *trail,
	long *last_clip_id,
	const long facility_id,
	const double numeric_attribute,
	const dynamic_string *text_attribute,
	map_layer *layer_buffer,
	std::vector <map_object *> *clips_added,
	const bool compute_area,
	interface_window *view,
	dynamic_string &log)

// Version called from FarmlandFinder.clip_buffers.  Repeated calls output to the same layer, so last_clip_id tracks unique IDs
// Create map_layer polygons for each ClipperLib::Paths

// Writes one string with all source_ids to attributes_text [0]


{
	bool error = false;
	long count = 0;
	int index, area_attribute_index = 0;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	dynamic_string inconstant_name;
	map_polygon *outline;

	if ((tract_id_source_1 == CLIPPED_ID_SOURCE_ID)
	|| (tract_id_source_1 == CLIPPED_ID_SOURCE_NUMERIC_ATTRIBUTE)
	|| (tract_id_source_1 == CLIPPED_ID_SOURCE_THREE_NAMES))
		++area_attribute_index;
	if (buffer_width_output_index != -1)
		++area_attribute_index;

	if ((tract_id_source_2 == CLIPPED_ID_SOURCE_ID)
	|| (tract_id_source_2 == CLIPPED_ID_SOURCE_NUMERIC_ATTRIBUTE))
		++area_attribute_index;

	if (view)
		view->update_progress ("Copying Points", 1);

	for (paths_path = trail->begin ();
	paths_path != trail->end ();
	++paths_path) {
		outline = (map_polygon *) layer_buffer->create_new (MAP_OBJECT_POLYGON);
		outline->attributes_numeric [0] = facility_id;
		if (buffer_width_output_index != -1)
			outline->attributes_numeric [buffer_width_output_index] = numeric_attribute;

		// Same list of source buffer ids written to each polygon's text_attribute field
		if (text_attribute)
			outline->attributes_text [0] = *text_attribute;

		if (compute_area) {
			outline->attributes_numeric [area_attribute_index] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
			outline->attributes_numeric [area_attribute_index + 1] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);
			outline->attributes_numeric [area_attribute_index + 2] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
			outline->attributes_numeric [area_attribute_index + 3] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_RHUMB);
		}

		outline->id = ++count + *last_clip_id;

		// 2021-08-25 Clipped powerline buffers don't have a last point that coincides with first point, so are not actually enclosed
		// area_m2 must be called without these duplicates
		// if (layer_buffer->enclosed)
		if (layer_buffer->enclosure == MAP_POLYGON_ADD_LAST_SEGMENT)
			outline->node_count = (long) paths_path->size () + 1;
		else
			outline->node_count = (long) paths_path->size ();

		outline->nodes = new long [outline->node_count * 2];

		for (path_point = paths_path->begin (), index = 0;
		path_point != paths_path->end ();
		++path_point, ++index) {
			outline->nodes [index * 2] = (long) path_point->X;
			outline->nodes [index * 2 + 1] = (long) path_point->Y;
		}

		// if (layer_buffer->enclosed) {
		if (layer_buffer->enclosure == MAP_POLYGON_ADD_LAST_SEGMENT) {
			// Duplicate first point at the end 
			path_point = paths_path->begin ();
			outline->nodes [index * 2] = (long) path_point->X;
			outline->nodes [index * 2 + 1] = (long) path_point->Y;
		}

		clips_added->push_back (outline);
		layer_buffer->objects.push_back (outline);
	}

	*last_clip_id += count;

	return !error;
}

bool ClipperBuffer::map_object_to_Path
	(const map_object *soil,
	ClipperLib::Paths *trail,
	dynamic_string &log)

// Create ClipperLib::Path from map_object

// 2021-07-06 First and last points of polygons must not coincide

{
	bool error = false;
	int index;
	ClipperLib::Path p;

	p.clear ();
	if (soil->type == MAP_OBJECT_POLYGON) {

		for (index = 0;
		index < ((map_polygon *) soil)->node_count;
		++index)
			add_to_path (&p, ((map_polygon *) soil)->nodes[index * 2], ((map_polygon *) soil)->nodes[index * 2 + 1]);

		trail->push_back (p);
	}
	else
		if (soil->type == MAP_OBJECT_POLYGON_COMPLEX) {
			map_polygon *sub_polygon;
			int polygon_index;

			for (polygon_index = 0;
			polygon_index < ((map_polygon_complex *) soil)->polygon_count;
			++polygon_index) {
				sub_polygon = &((map_polygon_complex *) soil)->polygons [polygon_index];
				p.clear ();
				p.reserve (sub_polygon->node_count);

				for (index = 0;
				index < sub_polygon->node_count;
				++index)
					add_to_path (&p, sub_polygon->nodes[index * 2], sub_polygon->nodes[index * 2 + 1]);

				trail->push_back (p);
			}
		}

	return !error;
}

bool ClipperBuffer::map_layer_to_Paths
	(const map_layer *layer,
	ClipperLib::Paths *trail,
	interface_window *view,
	dynamic_string &log)

// Create ClipperLib::Paths from map_layer

{
	bool error = false;
	std::vector <map_object *>::const_iterator soil;

	view->update_progress ("Copying Points", 1);

	for (soil = layer->objects.begin ();
	soil != layer->objects.end ();
	++soil)
		map_object_to_Path (*soil, trail, log);

	return !error;
}

/*
bool ClipperBuffer::create_buffer_map_layer
	(const ClipperLib::Paths *trail,
	map_layer *layer_buffer,
	const double offset_radius_average,
	const long new_id,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	_timeb start, end;
	float elapsed_milliseconds;
	int index;
	ClipperLib::Path p;
	ClipperLib::Paths all_p, solution;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	map_polygon *outline;

	ClipperLib::ClipperOffset co = ClipperLib::ClipperOffset();

	_ftime_s (&start);

	view->update_progress ("Copying Points", 1);

	co.Clear ();
	co.AddPaths (*trail, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etOpenRound);
	solution.clear();
	co.Execute (solution, offset_radius_average);

	for (paths_path = solution.begin ();
	paths_path != solution.end ();
	++paths_path) {
		outline = (map_polygon *) layer_buffer->create_new (MAP_OBJECT_POLYGON);
		outline->id = new_id;
		outline->node_count = paths_path->size ();
		outline->nodes = new long [outline->node_count * 2];
		for (path_point = paths_path->begin (), index = 0;
		path_point != paths_path->end ();
		++path_point, ++index) {
			outline->nodes [index * 2] = (long) path_point->X;
			outline->nodes [index * 2 + 1] = (long) path_point->Y;
		}
		layer_buffer->objects.push_back (outline);
	}

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}
*/

/*
bool ClipperBuffer::process_treatment_facility
	(const long nearest_comid,
	const double length_stream_km,
	const int UTMZoneNumber,
	const class map_layer *layer_rivers,
	const class map_layer *layer_census_tracts,
	const class router_NHD *router,
	class interface_window *view,
	dynamic_string &log)

// Creates a series of buffers of increasing radius and specified length around a single facility
// Does not put resulting buffers into a layer

{
	flow_network_link *link;
	std::vector <long> downstream_ids, upstream_ids;
	int index_miles;
	long level_path_id;
	double offset_radius_average_1_mile, offset_radius_average;
	double buffer_radius_m = 1.0 * METERS_PER_MILE;
	map_object *polygon;
	bool error = false;
	ClipperLib::Paths paths_buffers, paths_clipped;
	std::vector <long> tract_ids;
	std::vector <dynamic_string> tract_id_names;

	view->update_progress ("Accumulate downstream", 0);
	// router->rivers.accumulate_downstream (nearest_comid, &downstream_ids);
	router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

	view->update_progress ("Accumulate upstream", 0);
	if ((polygon = layer_rivers->match_id (nearest_comid)) != NULL) {
		level_path_id = (long) polygon->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];
		if ((link = router->rivers.match_comid (nearest_comid)) != NULL)
			link->accumulate_upstream_LevelPath_ordered (level_path_id, &router->river_layer_index, &upstream_ids);
		else {
			error = true;
			log.add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
		}
	}
	else {
		error = true;
		log.add_formatted ("ERROR, can't find map_object for COMID %ld.\n", nearest_comid);
	}

	ClipperLib::Paths path_downstream;

	// Compute offset_radius_average for 1 mile, then multiply it for each of the others

	view->update_progress ("Create Paths Upstream", 0);
	// upstream_ids are in order upstream
	// so don't use reverse_iterator
	// discard ids beyond length_stream_km
	create_paths (layer_rivers, &upstream_ids, &offset_radius_average_1_mile, buffer_radius_m, length_stream_km, UTMZoneNumber, &path_downstream, log);
	view->update_progress ("Create Paths Downstream", 0);
	create_paths_reverse (layer_rivers, &downstream_ids, &offset_radius_average_1_mile, buffer_radius_m, length_stream_km, UTMZoneNumber, &path_downstream, log);

	view->update_progress ("Create buffers", 0);
	for (index_miles = 1;
	index_miles <= 10;
	++index_miles) {
		offset_radius_average = offset_radius_average_1_mile * (double) index_miles;

		view->update_progress_formatted (1, "buffer %d miles", index_miles);

		// clipper.create_threaded (panel_watershed->layer_selection, layer_buffer, 30, buffer_radius_m, &view, log);
		create_buffer (&path_downstream, &paths_buffers, offset_radius_average, log);

		// intersect solution with any overlapping census polygons
		intersect_overlapping (&paths_buffers, layer_census_tracts, &paths_clipped, &tract_id_names, UTMZoneNumber, view, log);
		// paths_clipped are identified by tract_id_names and index_miles
	}

	return !error;
}
*/

void ClipperBuffer::set_tract_id
	(std::vector <dynamic_string>* tract_ids,
	std::vector <dynamic_string>* tract_names,
	const map_object *tract) const

{
	switch (tract_id_source_2) {
		case CLIPPED_ID_SOURCE_ID:
			{
				dynamic_string name;
				name.format ("%lld", tract->id);
				tract_ids->push_back (name);
			}
			break;
		case CLIPPED_ID_SOURCE_ID_AND_NAME:
		{
			dynamic_string name;
			name.format("%lld", tract->id);
			tract_ids->push_back(name);
			tract_names->push_back(tract->name);
		}
		break;
		case CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE:
			tract_ids->push_back (tract->attributes_text [tract_index_source_2]);
			break;
		case CLIPPED_ID_SOURCE_NAME:
			// 2017-07-12 Zipcodes (clipped by SSURGO) use name for ID
			// 2018-04-11 Census tract AREAKEY field too big for long ID, so it's in map_object.name
			tract_ids->push_back (tract->name);
			break;
		case CLIPPED_ID_SOURCE_THREE_NAMES:
			// Polygon union of census and place shapefiles
			// Union FID, Census Tract, Place
			{
				dynamic_string name;
				name.format ("%ld", tract->id);
				name += "\t";
				name += tract->name;
				name += "\t";
				name += tract->attributes_text[tract_index_source_2]; // PLCIDFP00

				tract_ids->push_back (name);
			}
			break;
		// default:
			// tract_id_names->push_back ((*tract)->name);
	}
}
	
bool ClipperBuffer::intersect_overlapping
	(const ClipperLib::Paths *buffers,
	const map_layer *layer_census_tracts,
	ClipperLib::Paths *clipped_tracts,
	std::vector <dynamic_string>* tract_ids,
	std::vector <dynamic_string>* tract_names,
	// std::vector <double> *tract_areas,
	interface_window *view,
	dynamic_string &log) const

// For each census tract that overlaps the area of the solution
// create an intersection

{
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector <map_object *>::const_iterator tract;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	bounding_cube solution_area;
	ClipperLib::Clipper c_intersect = ClipperLib::Clipper ();
	ClipperLib::Paths tract_path, clipped_tract;
	ClipperLib::Path p;
	int index;
	bool error = false;

	// Find the overall area of the solution
	for (paths_path = buffers->begin ();
	paths_path != buffers->end ();
	++paths_path) {
		for (path_point = paths_path->begin ();
		path_point != paths_path->end ();
		++path_point) {

			if ((long) path_point->X < solution_area.x [0])
				solution_area.x [0] = (long) path_point->X;
			if ((long) path_point->X > solution_area.x [1])
				solution_area.x [1] = (long) path_point->X;

			if ((long) path_point->Y < solution_area.y [0])
				solution_area.y [0] = (long) path_point->Y;
			if ((long) path_point->Y > solution_area.y [1])
				solution_area.y [1] = (long) path_point->Y;
		}
	}

	if (view)
		view->update_progress ("Clipping Tracts", 1);

	for (tract = layer_census_tracts->objects.begin ();
	tract != layer_census_tracts->objects.end ();
	++tract) {

		if ((*tract)->overlaps_logical (&solution_area)) {

			if (view) {
				if (tract_id_source_1 == CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE)
					view->update_progress ((*tract)->attributes_text[tract_index_source_1], 2);
				else
					if (tract_id_source_1 == CLIPPED_ID_SOURCE_NAME)
						// 2017-07-12 Zipcodes (clipped by SSURGO) use name for ID
						view->update_progress ((*tract)->name, 2);
			}


			// create a ClipperLib::Paths of the tract outline
			tract_path.clear ();
			if ((*tract)->type == MAP_OBJECT_POLYGON) {

				p.clear ();
				p.reserve (((map_polygon *) (* tract))->node_count);

				for (index = 0;
				index < ((map_polygon *) *tract)->node_count;
				++index)
					add_to_path (&p, ((map_polygon *) *tract)->nodes [index * 2], ((map_polygon *) *tract)->nodes [index * 2 + 1]);
					// p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((map_polygon *) *tract)->nodes [index * 2],  (ClipperLib::cInt) ((map_polygon *) *tract)->nodes [index * 2 + 1]));
				tract_path.push_back (p);
			}
			else
				if ((*tract)->type == MAP_OBJECT_POLYGON_COMPLEX) {
					map_polygon *sub_polygon;
					int polygon_index;

					for (polygon_index = 0;
					polygon_index < ((map_polygon_complex *) *tract)->polygon_count;
					++polygon_index) {
						sub_polygon = &((map_polygon_complex *) *tract)->polygons [polygon_index];
						p.clear ();
						p.reserve (sub_polygon->node_count);

						for (index = 0;
						index < sub_polygon->node_count;
						++index)
							add_to_path (&p, sub_polygon->nodes [index * 2], sub_polygon->nodes [index * 2 + 1]);
							// p.push_back (ClipperLib::IntPoint((ClipperLib::cInt) sub_polygon->nodes [index * 2],  (ClipperLib::cInt) sub_polygon->nodes [index * 2 + 1]));
						tract_path.push_back (p);
					}
				}

			c_intersect.Clear ();
			c_intersect.AddPaths (*buffers, ClipperLib::ptSubject, true);
			c_intersect.AddPaths (tract_path, ClipperLib::ptClip, true);
			// clipped_tracts->clear();
			clipped_tract.clear ();
			c_intersect.Execute (ClipperLib::ClipType::ctIntersection, clipped_tract);
			// Execute calls resize on the results set, so copy them to clipped_tracts

			for (paths_path = clipped_tract.begin ();
			paths_path != clipped_tract.end ();
			++paths_path) {
				clipped_tracts->push_back (*paths_path);
				set_tract_id (tract_ids, tract_names, *tract);
			}
		}
	}

	return !error;
}

bool ClipperBuffer::compute_polygon_area
	(const ClipperLib::Paths *trail,
	map_layer *layer_buffer,
	const int attribute_index,
	const byte geographiclib_linetype,
	interface_window *view,
	dynamic_string &log)

// Compute an area field in map_object

{
	bool error = false;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector <map_object *>::iterator polygon;
		
	view->update_progress ("Computing Area", 1);

	for (paths_path = trail->begin (), polygon = layer_buffer->objects.begin ();
	(paths_path != trail->end ())
	&& (polygon != layer_buffer->objects.end ());
	++paths_path, ++polygon) {
		(*polygon)->attributes_numeric [attribute_index] = area_m2 (&*paths_path, geographiclib_linetype);
	}

	return !error;
}

bool ClipperBuffer::create_buffer
	(const ClipperLib::Paths *trail,
	ClipperLib::Paths *solution,
	const double offset_radius_average,
	dynamic_string &log) const

{
	bool error = false;
	ClipperLib::Path p;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	std::vector < ClipperLib::Path >::const_iterator paths_path;

	ClipperLib::ClipperOffset co = ClipperLib::ClipperOffset();

	// view->update_progress ("Copying Points", 1);

	co.Clear ();
	co.AddPaths (*trail, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etOpenRound);
	solution->clear();
	co.Execute (*solution, offset_radius_average);

	return !error;
}

bool ClipperBuffer::find_bisection_line
	(class map_object *segment,
	const int nearest_polygon_index,
	const int nearest_point_index,
	const long next_comid_up,
	const long next_comid_down,
	const router_NHD *router,
	const double radius_average,
	bisection_definition *result,
	double *theta,
	std::set <long long> *two_point_instances,
	const bool verbose,
	dynamic_string &log) const

// Given the point nearest the facility, find the next point up and downstream
// Compute a 'perpendicular' line definition bisecting right and left sides

// river segment node ids are in order downstream

{
	bool error = false;
	logical_coordinate river_points [3];
	bool three_point = true;
	int node_index_adjacent;

	ASSERT (segment->type == MAP_OBJECT_POLYGON);

	if (nearest_point_index == 0) {
		// get next segment upstream
		std::map <long long, map_object *>::const_iterator adjacent;
		if ((adjacent = router->river_layer_index.find (next_comid_up)) != router->river_layer_index.end ()) {
			node_index_adjacent = ((map_polygon *) adjacent->second)->node_count - 1;
			// Most-downstream point of adjacent is a match for Most-upstream point [0] of segment
			ASSERT (((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2] == ((map_polygon *) segment)->nodes [0]);
			// Move on to 2nd point in adjacent
			node_index_adjacent -= 1;

			river_points [0].x = (double) ((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2] / 1.0e6;
			river_points [0].y = (double) ((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2 + 1] / 1.0e6;

			river_points [2].x = (double) ((map_polygon *) segment)->nodes [(nearest_point_index + 1) * 2] / 1.0e6;
			river_points [2].y = (double) ((map_polygon *) segment)->nodes [(nearest_point_index + 1) * 2 + 1] / 1.0e6;

			if (verbose) {
				log.add_formatted ("Next Upstream\t%lld\n", next_comid_up);
				log.add_formatted ("%d\t%.8lf\t%.8lf\n", node_index_adjacent,  ((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2] / 1.0e6, (double) ((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2 + 1] / 1.0e6);
			}
		}
		else {
			three_point = false;

			river_points [0].x = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2] / 1.0e6;
			river_points [0].y = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
			river_points [1].x = (double) ((map_polygon *) segment)->nodes [(nearest_point_index + 1) * 2] / 1.0e6;
			river_points [1].y = (double) ((map_polygon *) segment)->nodes [(nearest_point_index + 1) * 2 + 1] / 1.0e6;
		}
	}
	else
		if (nearest_point_index == segment->total_number_of_points () - 1) {
			// get next segment downstream
			std::map <long long, map_object *>::const_iterator adjacent;
			if ((adjacent = router->river_layer_index.find (next_comid_down)) != router->river_layer_index.end ()) {
				node_index_adjacent = 0;
				// Most-upstream point [0] of adjacent is same as Most-downstream point of segment
				ASSERT (((map_polygon *) adjacent->second)->nodes [0] == ((map_polygon *) segment)->nodes [nearest_point_index * 2]);
				// Move on to 2nd point in adjacent
				node_index_adjacent = 1;
				river_points [2].x = (double) ((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2] / 1.0e6;
				river_points [2].y = (double) ((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2 + 1] / 1.0e6;

				river_points [0].x = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2] / 1.0e6;
				river_points [0].y = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2 + 1] / 1.0e6;

				if (verbose) {
					log.add_formatted ("Next Downstream\t%lld\n", next_comid_up);
					log.add_formatted ("%d\t%.8lf\t%.8lf\n", node_index_adjacent, ((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2] / 1.0e6, (double) ((map_polygon *) adjacent->second)->nodes [node_index_adjacent * 2 + 1] / 1.0e6);
				}
			}
			else {
				// points are in order 0,1 upstream to downstream
				three_point = false;

				river_points [0].x = (double) ((map_polygon *) segment)->nodes [(nearest_point_index - 1) * 2] / 1.0e6;
				river_points [0].y = (double) ((map_polygon *) segment)->nodes [(nearest_point_index - 1) * 2 + 1] / 1.0e6;
				river_points [1].x = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2] / 1.0e6;
				river_points [1].y = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
			}
		}
		else {
			// next point downstream is in this COMID
			river_points [0].x = (double) ((map_polygon *) segment)->nodes [(nearest_point_index - 1) * 2] / 1.0e6;
			river_points [0].y = (double) ((map_polygon *) segment)->nodes [(nearest_point_index - 1) * 2 + 1] / 1.0e6;
			river_points [2].x = (double) ((map_polygon *) segment)->nodes [(nearest_point_index + 1) * 2] / 1.0e6;
			river_points [2].y = (double) ((map_polygon *) segment)->nodes [(nearest_point_index + 1) * 2 + 1] / 1.0e6;
		}

	if (three_point) {
		river_points [1].x = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2] / 1.0e6;
		river_points [1].y = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
		// compute perpendicular line from 3 points
		result->create_bisection_3pt (river_points, radius_average, theta, verbose, log);
	}
	else {
		result->create_bisection_2pt (river_points, radius_average, theta, verbose, log);

		// Track the times this happens
		two_point_instances->insert (segment->id);
	}

	return !error;
}

inline double Dot
	(const logical_coordinate a,
	const logical_coordinate b)
{
	return (a.x * b.x) + (a.y * b.y);
}

inline double PerpDot
	(const logical_coordinate a,
	const logical_coordinate b)

{
	return (a.y * b.x) - (a.x * b.y);
}

bool LineCollision
	(const logical_coordinate &A1,
	const logical_coordinate &A2,
    const logical_coordinate &B1,
	const logical_coordinate &B2,
	logical_coordinate * out = 0)

// http://www.cplusplus.com/forum/beginner/49408/

{
	logical_coordinate diff_a, diff_b;
	diff_a.x = A2.x - A1.x;
	diff_a.y = A2.y - A1.y;

	diff_b.x = B2.x - B1.x;
	diff_b.y = B2.y - B1.y;

    double f = PerpDot (diff_a, diff_b);
    if (f == 0)      // lines are parallel
        return false;
    
    logical_coordinate c;
	c.x = B2.x - A2.x;
	c.y = B2.y - A2.y;

    double aa = PerpDot (diff_a, c);
    double bb = PerpDot (diff_b, c);

    if(f < 0) {
        if (aa > 0)     return false;
        if (bb > 0)     return false;
        if (aa < f)     return false;
        if (bb < f)     return false;
    }
    else {
        if (aa < 0)     return false;
        if (bb < 0)     return false;
        if (aa > f)     return false;
        if (bb > f)     return false;
    }

    if (out) {
        double fraction = 1.0 - (aa / f);
		out->x = ((B2.x - B1.x) * fraction) + B1.x;
		// out->y = ((A2.y - A1.y) * fraction) + A1.y;
		out->y = ((B2.y - B1.y) * fraction) + B1.y;
	}

    return true;
}

bool ClipperBuffer::cut_buffer
	(const long facility_id,
	const double buffer_size_miles,
	std::map <double, long> *truncation_error_histogram,
	const bisection_definition *bisection,
	const ClipperLib::Paths *buffers_up_and_down,
	ClipperLib::Paths *buffers_upstream,
	ClipperLib::Paths *buffers_downstream,
	dynamic_string &log) const

// Cut buffers_up_and_down at bisection, creating buffers_upstream and buffers_downstream

// 2018-04-13 previously called 'truncate_buffers'

{
	bool error = false;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector < ClipperLib::IntPoint >::const_iterator p;
	logical_coordinate A1, A2, intersection;
	int count_intersections = 0, source_polygon_index;
	std::vector < ClipperLib::IntPoint > path_1, path_2;
	std::vector <bool> path_is_upstream;
	/*
#ifdef _DEBUG
	log += "Index\tLat\tLon\tCount\tUpsize\tDownsize\n";
#endif
	*/

	for (paths_path = buffers_up_and_down->begin ();
	paths_path != buffers_up_and_down->end ();
	++paths_path) {
		for (p = paths_path->begin (), source_polygon_index = 0;
		p != paths_path->end ();
		++p, ++source_polygon_index) {
			if (p == paths_path->begin ()) {
				A2.set ((long) p->X, (long) p->Y);
				path_1.push_back (*p);
			}
			else {
				A1.set ((long) p->X, (long) p->Y);
				if (LineCollision (A1, A2, bisection->end_0, bisection->end_1, &intersection)) {
					path_is_upstream.push_back (bisection->set_upstream_downstream (intersection));

					++count_intersections;
					path_1.push_back (ClipperLib::IntPoint ((long) (intersection.x * 1.0e6), (long) (intersection.y * 1.0e6)));
					path_2.push_back (ClipperLib::IntPoint ((long) (intersection.x * 1.0e6), (long) (intersection.y * 1.0e6)));
					/*
#ifdef _DEBUG
					log.add_formatted ("%d\t%.8lf\t%.8lf\t%d\t%d\t%d\n", source_polygon_index, intersection.x, intersection.y, count_intersections, path_1.size (), path_2.size ());
#endif
					*/
				}
				else {
					if (count_intersections % 2 == 0)
						path_1.push_back (*p);
					else
						path_2.push_back (*p);
				}

				A2 = A1;
			}	
		}
		// Connect last point to first

		p = paths_path->begin ();
		source_polygon_index = 0;
		A1 = A2;
		A2.set ((long) p->X, (long) p->Y);
		if (LineCollision (A1, A2, bisection->end_0, bisection->end_1, &intersection)) {
			path_is_upstream.push_back (bisection->set_upstream_downstream (intersection));
			++count_intersections;
			path_1.push_back (ClipperLib::IntPoint ((long) (intersection.x * 1.0e6), (long) (intersection.y * 1.0e6)));
			path_2.push_back (ClipperLib::IntPoint ((long) (intersection.x * 1.0e6), (long) (intersection.y * 1.0e6)));
			/*
#ifdef _DEBUG
			log.add_formatted ("%d\t%.8lf\t%.8lf\t%d\t%d\t%d\n", source_polygon_index, intersection.x, intersection.y, count_intersections, path_1.size (), path_2.size ());
#endif
			*/
		}
		else {
			// don't need to push first point again
			// path_down.push_back (*p);
		}
	}
	if (count_intersections != 2) {
		log.add_formatted ("Error, intersection count is %d in ClipperBuffer.cut_buffers (Facility %ld, buffer size %.2lf miles).\n", count_intersections, facility_id, buffer_size_miles);
		error = true;
		std::map <double, long>::iterator histogram_entry;

		if ((histogram_entry = truncation_error_histogram->find (buffer_size_miles)) != truncation_error_histogram->end ())
			histogram_entry->second += 1;
		else
			truncation_error_histogram->insert (std::pair <double, long> (buffer_size_miles, 1));
	}
	else {
		/*
#ifdef _DEBUG
		log += "\n";
#endif
		*/
		if (path_is_upstream [1]) {
			buffers_upstream->push_back (path_2);
			buffers_downstream->push_back (path_1);
		}
		else {
			buffers_upstream->push_back (path_1);
			buffers_downstream->push_back (path_2);
		}
	}

	/*
	{
		// "when open paths are passed to a Clipper object, the user must use a PolyTree object as the solution parameter, otherwise an exception will be raised."
		ClipperLib::Clipper c_intersect = ClipperLib::Clipper ();
		ClipperLib::PolyTree clipped_buffers;

		// only returns 2 points
		c_intersect.Clear ();
		c_intersect.AddPaths (*buffers_up_and_down, ClipperLib::ptClip, true);
		c_intersect.AddPaths (bisection->vector, ClipperLib::ptSubject, false); // open paths must be subject

		c_intersect.Execute (ClipperLib::ClipType::ctIntersection, clipped_buffers);

		ASSERT (clipped_buffers.Childs [0]->Contour.size () == 2);

		// class PolyNode;
		// typedef std::vector< PolyNode* > PolyNodes;
		ClipperLib::PolyNodes pn;
		ClipperLib::PolyNodes::iterator pc;
		ClipperLib::PolyNode *node;

		// PolyNodes Childs;
		pn = clipped_buffers.Childs;
		pc = pn.begin ();
		node = *pc;

		path_buffers_upstream->push_back (node->Contour);
	//	path_buffers_downstream->push_back ((*pc)->Contour [1]);
	}
	*/

	return !error;
}

void nearest_up
	(const flow_network_link *nearest_link,
	const long long level_path_id,
	std::vector <long long> *upstream_ids,
	long long *next_comid_up,
	const router_NHD *router)

// For comid related to nearest_link, find comids of next segment up & down along level path id


{

	nearest_link->accumulate_upstream_LevelPath_ordered (level_path_id, &router->river_layer_index, upstream_ids);

	if (upstream_ids->size () > 0) {
		// upstream_ids start with nearest_comid, and are listed in order upstream
		// first member is nearest_comid
		if (nearest_link->id == (*upstream_ids) [0]) {
			if (upstream_ids->size () > 1)
				*next_comid_up = (*upstream_ids) [1];
			else
				*next_comid_up = -1;
		}
		else
			*next_comid_up = (*upstream_ids) [0];
	}
	else
		*next_comid_up = -1;
}

void nearest_down
	(const flow_network_link *nearest_link,
	std::vector <long long> *downstream_ids,
	long long *next_comid_down,
	const router_NHD *router)

// For comid related to nearest_link, find comids of next segment up & down along level path id


{
	router->rivers.accumulate_downstream_ordered (nearest_link->id, downstream_ids);

	if (downstream_ids->size () > 0) {
		// downstream_ids start with nearest_comid, and are listed in order downstream
		// last member is nearest_comid
		int downstream_index = (int) downstream_ids->size () - 1;
		if (nearest_link->id == (*downstream_ids) [downstream_index]) {
			if (downstream_ids->size () > 1)
				*next_comid_down = (*downstream_ids) [downstream_index - 1];
			else
				*next_comid_down = -1;
		}
		else
			*next_comid_down = (*downstream_ids) [downstream_index];
	}
	else
		*next_comid_down = -1;
}

bool work_buffer_clip
	(const double length_stream_km,
	const double minimum_buffer_radius_miles,
	const double maximum_buffer_radius_miles,
	const double cut_line_length_miles,
	const map_object *facility,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const ClipperBuffer *clipper,
	const router_NHD *router,
	bool *error,
	std::map <double, long> *truncation_error_histogram,
	std::set <long long> *two_point_instances,
	dynamic_string *table,
	dynamic_string *log)

// Creates buffer upstream and downstream from facility,
// using a bisection line through the nearest river point

{
	long long nearest_comid, level_path_id, next_comid_up, next_comid_down;
	std::map <long long, map_object *>::const_iterator nearest_segment;
	int nearest_polygon_index, nearest_point_index, index_area;
	flow_network_link *nearest_link;
	double offset_radius_average_1_mile, offset_radius_average, upstream_within_segment_m, downstream_within_segment_m, theta, index_miles;
	ClipperLib::Path p;
	ClipperLib::Paths path_up_and_down, buffer_up_and_down, buffers_up, buffers_down, paths_clipped_up, paths_clipped_down;
	double radius_average_25km; 
	bisection_definition bisection;
	std::vector <long long> downstream_ids, upstream_ids;
	double area_values [4];
	std::map <double, long>::iterator histogram_entry;

	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector <dynamic_string>::const_iterator tract_id_name;
	std::vector <dynamic_string> tract_id_names_up, tract_id_names_down;

	nearest_comid = (long long) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
	nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
	nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];

	nearest_segment = router->river_layer_index.find (nearest_comid);
	level_path_id = (long long) nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

	// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (0, nearest_point_index, &upstream_within_segment_m, *log);
	// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (nearest_point_index, ((map_polygon *) nearest_segment->second)->total_number_of_points () - 1, &downstream_within_segment_m, *log);

	{
		logical_coordinate center_point;
		double offset_radius_x, offset_radius_y;
		double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE;
		// length_stream_km = bisect_radius_m / 1000.0;

		center_point.x = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
		center_point.y = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
		offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
		offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
		radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
	}

	// Only evaluate if within 10km of river
	if (facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M] < (5.0 * METERS_PER_MILE)) {
		if (nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
				nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
				nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);

				clipper->find_bisection_line (nearest_segment->second, nearest_polygon_index, nearest_point_index, next_comid_up, next_comid_down, router, radius_average_25km, &bisection, &theta, two_point_instances, false, *log);

				// computed distance from nearest point in nearest segment up & down

				// discard ids beyond length_downstream_km

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper->average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, *log);
				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				p.clear ();
				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				clipper->add_upstream_within_segment ((map_polygon *) nearest_segment->second, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, NULL, *log);
				clipper->create_paths (layer_rivers, NULL, &upstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, *log);
				path_up_and_down.push_back (p);

				p.clear ();
				clipper->add_downstream_within_segment ((map_polygon *) nearest_segment->second, nearest_polygon_index, nearest_point_index, &downstream_within_segment_m, length_stream_km * 1000.0, &p, NULL, *log);
				clipper->create_paths_reverse (layer_rivers, NULL, &downstream_ids, length_stream_km - (downstream_within_segment_m / 1000.0), &p, *log);
				path_up_and_down.push_back (p);

				// clipper->create_paths (layer_rivers, upstream_within_segment_m, &upstream_ids, &offset_radius_average_1_mile, METERS_PER_MILE, length_stream_km - (upstream_within_segment_m / 1000.0), 0, &path_up_and_down, *log);
				// clipper->create_paths_reverse (layer_rivers, downstream_within_segment_m, &downstream_ids, &offset_radius_average_1_mile, METERS_PER_MILE, length_stream_km - (downstream_within_segment_m / 1000.0), 0, &path_up_and_down, *log);

				for (index_miles = minimum_buffer_radius_miles;
				index_miles <= maximum_buffer_radius_miles;
				++index_miles) {
					offset_radius_average = offset_radius_average_1_mile * index_miles;

					clipper->create_buffer (&path_up_and_down, &buffer_up_and_down, offset_radius_average, *log);
					buffers_up.clear ();
					buffers_down.clear ();
					paths_clipped_up.clear ();
					paths_clipped_down.clear ();
					tract_id_names_up.clear ();
					tract_id_names_down.clear ();

					// Use a line perpedicular to the river at cutoff point to square the ends of buffers
					// This will eliminate overlap

					if (!clipper->cut_buffer (facility->id, index_miles, truncation_error_histogram, &bisection, &buffer_up_and_down, &buffers_up, &buffers_down, *log))
						*error = true;
					else {

						clipper->intersect_overlapping (&buffers_up, layer_census_tracts, &paths_clipped_up, &tract_id_names_up, NULL, NULL, *log);
						clipper->intersect_overlapping (&buffers_down, layer_census_tracts, &paths_clipped_down, &tract_id_names_down, NULL, NULL, *log);

						for (paths_path = paths_clipped_up.begin (), tract_id_name = tract_id_names_up.begin ();
						paths_path != paths_clipped_up.end ();
						++paths_path, ++tract_id_name) {
							area_values [0] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
							area_values [1] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);
							area_values [2] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
							area_values [3] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_RHUMB);
							table->add_formatted ("%ld", facility->id);
							table->add_formatted ("\t%.2lf", index_miles);
							*table += "\t";
							*table += *tract_id_name;
							table->add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tUp\n",
							area_values [0], area_values [1], area_values [2], area_values [3]);

							for (index_area = 0; index_area < 4; ++index_area)
								if (area_values [index_area] < 0.0) {								
									if ((histogram_entry = truncation_error_histogram->find (-index_miles)) != truncation_error_histogram->end ())
										histogram_entry->second += 1;
									else
										truncation_error_histogram->insert (std::pair <double, long> (-index_miles, 1));
								}
						}

						for (paths_path = paths_clipped_down.begin (), tract_id_name = tract_id_names_down.begin ();
						paths_path != paths_clipped_down.end ();
						++paths_path, ++tract_id_name) {

							area_values [0] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
							area_values [1] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);
							area_values [2] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
							area_values [3] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_RHUMB);

							table->add_formatted ("%ld", facility->id);
							table->add_formatted ("\t%.2lf", index_miles);
							*table += "\t";
							*table += *tract_id_name;
							table->add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tDown\n",
							area_values [0], area_values [1], area_values [2], area_values [3]);

							for (index_area = 0; index_area < 4; ++index_area)
								if (area_values [index_area] < 0.0) {								
									if ((histogram_entry = truncation_error_histogram->find (-index_miles)) != truncation_error_histogram->end ())
										histogram_entry->second += 1;
									else
										truncation_error_histogram->insert (std::pair <double, long> (-index_miles, 1));
								}
						}
					}
				}
			}
			else {
				log->add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
				*error = true;
			}

		}
		else
			log->add_formatted ("%ld\tDivergence.\n", nearest_comid);
	}

	return (*error == false);
}

void ClipperBuffer::dump_paths
	(ClipperLib::Paths *trail,
	dynamic_string &log) const

{
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	std::vector < ClipperLib::Path >::const_iterator paths_path;

	for (paths_path = trail->begin ();
	paths_path != trail->end ();
	++paths_path) {
		log += "Path\n";
		for (path_point = paths_path->begin ();
		path_point != paths_path->end ();
		++path_point)
			log.add_formatted ("\t%.6ld\t%.6ld\n", (long) path_point->X, (long) path_point->Y);
	}
}

bool work_buffer_upstream
	(const double buffer_width_miles,
	const double start_upstream_km,
	const double length_stream_km,
	const double cut_line_length_miles,
	const map_object *facility,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const ClipperBuffer *clipper,
	const router_NHD *router,
	long *count_skip_upstream_distance,
	bool *error,
	dynamic_string *table,
	dynamic_string *log)

{
	long long nearest_comid, level_path_id, next_comid_up;
	std::map <long long, map_object *>::const_iterator facility_segment;
	int nearest_polygon_index, nearest_point_index;
	flow_network_link *nearest_link;
	map_object *starting_segment;
	double offset_radius_average_1_mile, offset_radius_average, upstream_within_segment_m;
	ClipperLib::Path p;
	ClipperLib::Paths path_up_and_down, buffers_up, paths_clipped_up;
	double radius_average_25km; 
	std::vector <long long> upstream_ids;
	double area_values [4];
	bool skip = false, verbose = false;

	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector <dynamic_string>::const_iterator tract_id_name;
	std::vector <dynamic_string> tract_id_names_up, tract_id_names_down;

	nearest_comid = (long long) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
	nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
	nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];

	facility_segment = router->river_layer_index.find (nearest_comid);
	level_path_id = (long long) facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

	// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (0, nearest_point_index, &upstream_within_segment_m, *log);
	// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (nearest_point_index, ((map_polygon *) nearest_segment->second)->total_number_of_points () - 1, &downstream_within_segment_m, *log);

	{
		logical_coordinate center_point;
		double offset_radius_x, offset_radius_y;
		double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE;
		// length_stream_km = bisect_radius_m / 1000.0;

		center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
		center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
		offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
		offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
		radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
	}

	// Only evaluate if within 10km of river
	if (facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M] < (5.0 * METERS_PER_MILE)) {
		if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
				nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);

				skip = false;
				if (start_upstream_km > 0.0) {
					int starting_polygon_index, starting_point_index;

					/*
					if (facility->id == 330100003)
						verbose = true;
					else
						verbose = false;
					*/

					if (clipper->find_start_point_upstream (facility_segment->second, nearest_polygon_index, nearest_point_index, layer_rivers, &upstream_ids, start_upstream_km,
					&starting_segment, &starting_polygon_index, &starting_point_index, verbose, *log)) {

						nearest_comid = starting_segment->id;
						nearest_polygon_index = starting_polygon_index;
						nearest_point_index = starting_point_index;

						// rebuild upstream_ids starting at starting_segment
						upstream_ids.clear ();
						if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
							nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
						else {
							log->add_formatted ("ERROR, can't find flow_network_link for new starting comid %ld.\n", nearest_comid);
							skip = true;
						}
					}
					else {
						skip = true;
						*count_skip_upstream_distance += 1;
					}
				}
				else
					starting_segment = facility_segment->second;

				if (!skip) {
					// computed distance from nearest point in nearest segment up & down

					// discard ids beyond length_downstream_km

					// Compute offset_radius_average for 1 mile, then multiply it for each of the others
					offset_radius_average_1_mile = clipper->average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, *log);
					// upstream_ids are in order upstream
					// so don't use reverse_iterator
					p.clear ();
					// upstream_ids are in order upstream
					// so don't use reverse_iterator
					clipper->add_upstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, NULL, *log);

					clipper->create_paths (layer_rivers, NULL, &upstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, *log);
					path_up_and_down.push_back (p);

					if (verbose) {
						// List upstream ids
						std::vector <long long>::const_iterator stream;
						*log += "\nUpstream\t";
						for (stream = upstream_ids.begin ();
						stream != upstream_ids.end ();
						++stream)
							log->add_formatted (" %ld", *stream);
						*log += "\n";

						clipper->dump_paths (&path_up_and_down, *log);
					}

					/*
					for (index_miles = 1;
					index_miles <= 10;
					++index_miles) {
						buffer_id = (long) index_miles;
					*/
					offset_radius_average = offset_radius_average_1_mile * buffer_width_miles;

					clipper->create_buffer (&path_up_and_down, &buffers_up, offset_radius_average, *log);
					paths_clipped_up.clear ();
					tract_id_names_up.clear ();
					tract_id_names_down.clear ();

					// Use a line perpedicular to the river at cutoff point to square the ends of buffers
					// This will eliminate overlap

					clipper->intersect_overlapping (&buffers_up, layer_census_tracts, &paths_clipped_up, &tract_id_names_up, NULL, NULL, *log);

					for (paths_path = paths_clipped_up.begin (), tract_id_name = tract_id_names_up.begin ();
					paths_path != paths_clipped_up.end ();
					++paths_path, ++tract_id_name) {
						area_values [0] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
						area_values [1] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);
						area_values [2] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
						area_values [3] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_RHUMB);
						table->add_formatted ("%ld", facility->id);
						table->add_formatted ("\t%.2lf", buffer_width_miles);
						*table += "\t";
						*table += *tract_id_name;
						table->add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tUp\n",
						area_values [0], area_values [1], area_values [2], area_values [3]);
					}
				}
			}
			else {
				log->add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
				*error = true;
			}

		}
		else
			log->add_formatted ("%ld\tDivergence.\n", nearest_comid);
	}

	return (*error == false);
}

bool subtract_inside_areas
	(std::map <dynamic_string, double *> *outside_area,
	ClipperLib::Paths *paths_clipped_inside,
	std::vector <dynamic_string> *tract_id_names_inside,
	ClipperLib::Paths *paths_clipped_outside,
	std::vector <dynamic_string> *tract_id_names_outside,
	dynamic_string &log,
	const bool verbose)

// Fill map area_inside with areas of each clipped Path indexed by census tract name
// Subtract from outside areas

{
	std::pair <std::map <dynamic_string, double *>::iterator, bool> insert_result;
	std::map <dynamic_string, double *> area_inside;
	std::map <dynamic_string, double *>::iterator tract_area, inside_area;
	std::vector < ClipperLib::Path >::const_iterator clipped_path;
	std::vector <dynamic_string>::const_iterator tract_name;
	bool error = false;

	// Compute total area by Census Tract ID of all polygons in paths_clipped_inside
	for (clipped_path = paths_clipped_inside->begin (), tract_name = tract_id_names_inside->begin ();
	clipped_path != paths_clipped_inside->end ();
	++clipped_path, ++tract_name) {
		if (verbose) {
			if (*tract_name == "23019031300") {
				std::vector < ClipperLib::IntPoint >::const_iterator path_point;

				log += "\n";
				for (path_point = clipped_path->begin ();
				path_point != clipped_path->end ();
				++path_point) {
					log.add_formatted ("\t%.8lf\t%.8lf\n", (double) path_point->Y / 1.0e6, (double) path_point->X / 1.0e6);
				}
				log += "\n";
			}
		}

		if ((tract_area = area_inside.find (*tract_name)) == area_inside.end ()) {
			insert_result = area_inside.insert (std::pair <dynamic_string, double *> (*tract_name, new double [4]));

			insert_result.first->second [0] = area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
			insert_result.first->second [1] = area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_EXACT);
			insert_result.first->second [2] = area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
			insert_result.first->second [3] = area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_RHUMB);
		}
		else {
			tract_area->second [0] += area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
			tract_area->second [1] += area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_EXACT);
			tract_area->second [2] += area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
			tract_area->second [3] += area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_RHUMB);
		}
	}

	// Compute total area by Census Tract ID of all polygons in paths_clipped_outside
	for (clipped_path = paths_clipped_outside->begin (), tract_name = tract_id_names_outside->begin ();
	clipped_path != paths_clipped_outside->end ();
	++clipped_path, ++tract_name) {

		if ((tract_area = outside_area->find (*tract_name)) == outside_area->end ()) {
			insert_result = outside_area->insert (std::pair <dynamic_string, double *> (*tract_name, new double [4]));

			insert_result.first->second [0] = area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
			insert_result.first->second [1] = area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_EXACT);
			insert_result.first->second [2] = area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
			insert_result.first->second [3] = area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_RHUMB);
		}
		else {
			insert_result.first->second [0] += area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
			insert_result.first->second [1] += area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_EXACT);
			insert_result.first->second [2] += area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
			insert_result.first->second [3] += area_m2 (&*clipped_path, GEOGRAPHICLIB_LINETYPE_RHUMB);
		}
	}

	/*
	if (verbose) {
		if ((tract_area = outside_area->find ("23019031300")) != outside_area->end ()) {
			log += "Outside\t";
			log += tract_area->first;
			log.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tUp\n",
			tract_area->second [0], tract_area->second [1], tract_area->second [2], tract_area->second [3]);
		}

		if ((tract_area = area_inside.find ("23019031300")) != area_inside.end ()) {
			log += "Inside\t";
			log += tract_area->first;
			log.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tUp\n",
			tract_area->second [0], tract_area->second [1], tract_area->second [2], tract_area->second [3]);
		}
	}
	*/

	// Subtract any matching inside areas from outside areas
	for (tract_area = outside_area->begin ();
	tract_area != outside_area->end ();
	++tract_area) {
		if ((inside_area = area_inside.find (tract_area->first)) != area_inside.end ()) {
			// Same facility, same buffer_width, same census tract
			tract_area->second [0] -= inside_area->second [0];
			tract_area->second [1] -= inside_area->second [1];
			tract_area->second [2] -= inside_area->second [2];
			tract_area->second [3] -= inside_area->second [3];
		}
	}

	for (inside_area = area_inside.begin (); inside_area != area_inside.end (); ++inside_area)
		delete [] inside_area->second;

	return !error;
}

bool work_buffer_upstream_difference
	(const double buffer_width_miles,
	const double length_stream_km,
	const double start_upstream_km,
	const double cut_line_length_miles,
	const map_object *facility,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const ClipperBuffer *clipper,
	const router_NHD *router,
	bool *error,
	dynamic_string *table,
	dynamic_string *log)

// Compute buffer sizes between start and length at mile width

// Compute 0-start_upstream_km
// Compute 0-length_stream_km
// Subtract to find difference

{
	long long nearest_comid, level_path_id, next_comid_up;
	std::map <long long, map_object *>::const_iterator facility_segment;
	int nearest_polygon_index, nearest_point_index;
	flow_network_link *nearest_link;
	map_object *starting_segment;
	double offset_radius_average_1_mile, offset_radius_average, upstream_within_segment_m;
	ClipperLib::Path p;
	ClipperLib::Paths paths_inside, paths_outside, buffers_inside, buffers_outside, paths_clipped_inside, paths_clipped_outside;
	double radius_average_25km; 
	std::vector <long long> upstream_ids;
	std::map <dynamic_string, double *> outside_areas;
	std::map <dynamic_string, double *>::iterator outside_area;
	bool verbose = false;
	std::vector < ClipperLib::Path >::const_iterator clip_outside;
	std::vector < ClipperLib::Path >::const_iterator clipped_path;
	std::vector <dynamic_string>::const_iterator tract_name_outside;
	std::vector <dynamic_string> tract_id_names_inside, tract_id_names_outside;

	nearest_comid = (long long) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
	nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
	nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];

	facility_segment = router->river_layer_index.find (nearest_comid);
	level_path_id = (long long) facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

	{
		logical_coordinate center_point;
		double offset_radius_x, offset_radius_y;
		double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE;
		// length_stream_km = bisect_radius_m / 1000.0;

		center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
		center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
		offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
		offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
		radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
	}
	// if (facility->id == 230026001)
	//	verbose = true;

	// Only evaluate if within 10km of river
	if (facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M] < (5.0 * METERS_PER_MILE)) {
		if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
				nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);

				starting_segment = facility_segment->second;

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper->average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, *log);
				offset_radius_average = offset_radius_average_1_mile * buffer_width_miles;

				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				p.clear ();
				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				clipper->add_upstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, start_upstream_km * 1000.0, &p, NULL, *log);
				clipper->create_paths (layer_rivers, NULL, &upstream_ids, start_upstream_km - (upstream_within_segment_m / 1000.0), &p, *log);
				paths_inside.push_back (p);

				p.clear ();
				clipper->add_upstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, NULL, *log);
				clipper->create_paths (layer_rivers, NULL, &upstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, *log);
				paths_outside.push_back (p);

				if (verbose) {
					// List upstream ids
					std::vector <long long>::const_iterator stream;
					*log += "\nUpstream\t";
					for (stream = upstream_ids.begin ();
					stream != upstream_ids.end ();
					++stream)
						log->add_formatted (" %lld", *stream);
					*log += "\n";

					*log += "Inside\n";
					clipper->dump_paths (&paths_inside, *log);
					*log += "Outside\n";
					clipper->dump_paths (&paths_outside, *log);
				}

				clipper->create_buffer (&paths_inside, &buffers_inside, offset_radius_average, *log);
				clipper->create_buffer (&paths_outside, &buffers_outside, offset_radius_average, *log);

				paths_clipped_inside.clear ();
				paths_clipped_outside.clear ();
				tract_id_names_inside.clear ();
				tract_id_names_outside.clear ();

				// Compute intersection
				clipper->intersect_overlapping (&buffers_inside, layer_census_tracts, &paths_clipped_inside, &tract_id_names_inside, NULL, NULL, *log);
				clipper->intersect_overlapping (&buffers_outside, layer_census_tracts, &paths_clipped_outside, &tract_id_names_outside, NULL, NULL, *log);

				// multiple polygons can exist for each census tract id
				// Create a map of outside areas by tract id
				subtract_inside_areas (&outside_areas, &paths_clipped_inside, &tract_id_names_inside, &paths_clipped_outside, &tract_id_names_outside, *table, verbose);

				for (outside_area = outside_areas.begin ();
				outside_area != outside_areas.end ();
				++outside_area) {
					if ((outside_area->second [0] != 0.0)
					// || (outside_area->second [1] != 0.0) // EXACT often comes out as "-1.#IND" when the other 3 are 0
					|| (outside_area->second [2] != 0.0)
					|| (outside_area->second [3] != 0.0)) {
						table->add_formatted ("%lld", facility->id);
						table->add_formatted ("\t%.1lf", buffer_width_miles);
						*table += "\t";
						*table += outside_area->first;
						table->add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tUp\n",
						outside_area->second [0], outside_area->second [1], outside_area->second [2], outside_area->second [3]);
					}
					delete [] outside_area->second;
				}
				outside_areas.clear ();
			}
			else {
				log->add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
				*error = true;
			}
		}
		else
			log->add_formatted ("%ld\tDivergence.\n", nearest_comid);
	}

	return (*error == false);
}

bool work_buffer_downstream_difference
	(const double buffer_width_miles,
	const double length_stream_km,
	const double start_upstream_km,
	const double cut_line_length_miles,
	const map_object *facility,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const ClipperBuffer *clipper,
	const router_NHD *router,
	bool *error,
	dynamic_string *table,
	dynamic_string *log)

// Compute buffer sizes between start and length at mile width

// Compute 0-start_upstream_km
// Compute 0-length_stream_km
// Subtract to find difference

{
	long long nearest_comid, next_comid_up;
	std::map <long long, map_object *>::const_iterator facility_segment;
	int nearest_polygon_index, nearest_point_index;
	flow_network_link *nearest_link;
	map_object *starting_segment;
	double offset_radius_average_1_mile, offset_radius_average, upstream_within_segment_m;
	ClipperLib::Path p;
	ClipperLib::Paths paths_inside, paths_outside, buffers_inside, buffers_outside, paths_clipped_inside, paths_clipped_outside;
	double radius_average_25km; 
	std::vector <long long> downstream_ids;
	std::map <dynamic_string, double *> outside_areas;
	std::map <dynamic_string, double *>::iterator outside_area;
	bool verbose = false;
	std::vector < ClipperLib::Path >::const_iterator clip_outside;
	std::vector < ClipperLib::Path >::const_iterator clipped_path;
	std::vector <dynamic_string>::const_iterator tract_name_outside;
	std::vector <dynamic_string> tract_id_names_inside, tract_id_names_outside;

	nearest_comid = (long long) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
	nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
	nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];

	facility_segment = router->river_layer_index.find (nearest_comid);

	{
		logical_coordinate center_point;
		double offset_radius_x, offset_radius_y;
		double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE;
		// length_stream_km = bisect_radius_m / 1000.0;

		center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
		center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
		offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
		offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
		radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
	}
	// if (facility->id == 230026001)
	//	verbose = true;

	// Only evaluate if within 10km of river
	if (facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M] < (5.0 * METERS_PER_MILE)) {
		if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
				nearest_down (nearest_link, &downstream_ids, &next_comid_up, router);

				starting_segment = facility_segment->second;

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper->average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, *log);
				offset_radius_average = offset_radius_average_1_mile * buffer_width_miles;

				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				p.clear ();
				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				clipper->add_downstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, start_upstream_km * 1000.0, &p, NULL, *log);
				clipper->create_paths_reverse (layer_rivers, NULL, &downstream_ids, start_upstream_km - (upstream_within_segment_m / 1000.0), &p, *log);
				paths_inside.push_back (p);

				p.clear ();
				clipper->add_downstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, NULL, *log);
				clipper->create_paths_reverse (layer_rivers, NULL, &downstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, *log);
				paths_outside.push_back (p);

				if (verbose) {
					// List upstream ids
					std::vector <long long>::const_iterator stream;
					*log += "\nDownstream\t";
					for (stream = downstream_ids.begin ();
					stream != downstream_ids.end ();
					++stream)
						log->add_formatted (" %lld", *stream);
					*log += "\n";

					*log += "Inside\n";
					clipper->dump_paths (&paths_inside, *log);
					*log += "Outside\n";
					clipper->dump_paths (&paths_outside, *log);
				}

				clipper->create_buffer (&paths_inside, &buffers_inside, offset_radius_average, *log);
				clipper->create_buffer (&paths_outside, &buffers_outside, offset_radius_average, *log);

				paths_clipped_inside.clear ();
				paths_clipped_outside.clear ();
				tract_id_names_inside.clear ();
				tract_id_names_outside.clear ();

				// Compute intersection
				clipper->intersect_overlapping (&buffers_inside, layer_census_tracts, &paths_clipped_inside, &tract_id_names_inside, NULL, NULL, *log);
				clipper->intersect_overlapping (&buffers_outside, layer_census_tracts, &paths_clipped_outside, &tract_id_names_outside, NULL, NULL, *log);

				// multiple polygons can exist for each census tract id
				// Create a map of outside areas by tract id
				subtract_inside_areas (&outside_areas, &paths_clipped_inside, &tract_id_names_inside, &paths_clipped_outside, &tract_id_names_outside, *table, verbose);

				for (outside_area = outside_areas.begin ();
				outside_area != outside_areas.end ();
				++outside_area) {
					if ((outside_area->second [0] != 0.0)
					|| (outside_area->second [1] != 0.0)
					|| (outside_area->second [2] != 0.0)
					|| (outside_area->second [3] != 0.0)) {
						table->add_formatted ("%ld", facility->id);
						table->add_formatted ("\t%.1lf", buffer_width_miles);
						*table += "\t";
						*table += outside_area->first;
						table->add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tDown\n",
						outside_area->second [0], outside_area->second [1], outside_area->second [2], outside_area->second [3]);
					}
					delete [] outside_area->second;
				}
				outside_areas.clear ();
			}
			else {
				log->add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
				*error = true;
			}
		}
		else
			log->add_formatted ("%ld\tDivergence.\n", nearest_comid);
	}

	return (*error == false);
}

bool ClipperBuffer::buffer_stream_threaded
	(const map_layer *layer_facilities,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const router_NHD *router,
	const double length_stream_miles,
	const double cut_line_length_miles,
	const double minimum_buffer_radius_miles,
	const double maximum_buffer_radius_miles,
	const int thread_count,
	FILE *output_table,
	interface_window *view,
	dynamic_string &log)

// Create buffers around layer_facilities and intersect with layer_census_tracts

// Creates a series of buffers with radii from 1 to 10 miles

// 2018-04-13 used to be called 'intersect_threaded'


{
	int count_all_points, count_points_complete;
	int thread_index;
	std::future <bool> *threads;
	BYTE *thread_status_array;
	dynamic_string *results, *table, thread_update, update;
	std::vector <map_object *>::const_iterator last_facility_started;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false, queue_complete, all_threads_finished, *errors;
	std::future_status thread_status;
	long count_ok = 0, count_fail = 0, count_skip = 0;
	std::map <double, long> *truncation_error_histogram, all_truncation_errors;
	std::map <double, long>::iterator histogram_entry, all_histogram_entry;
	std::set <long long> *two_point_instances, all_two_point_instances;
	std::set <long long>::iterator two_point_entry;
	double length_stream_km;

	_ftime_s (&start);

	results = new dynamic_string [thread_count];
	table = new dynamic_string [thread_count];
	threads = new std::future <bool> [thread_count];
	thread_status_array = new BYTE [thread_count];
	errors = new bool [thread_count];
	memset (errors, 0, sizeof (bool) * thread_count);
	truncation_error_histogram = new std::map <double, long> [thread_count];
	two_point_instances = new std::set <long long> [thread_count];

	fprintf (output_table, "Facility\tBuffer Radius, miles\tCensus Tract ID\tGeodesic\tExact\tAuthalic\tRhumb\n");
	length_stream_km = ((double) length_stream_miles * METERS_PER_MILE) / 1000.0;

	log.add_formatted ("Buffer length\t%.2lf\t%.2lf\n", length_stream_miles, length_stream_km);

	view->update_progress ("Starting threads", 0);
	thread_index = 0;
	memset (thread_status_array, 0, sizeof (BYTE) * thread_count);
	last_facility_started = layer_facilities->objects.begin ();
	count_all_points = (int) layer_facilities->objects.size ();
	for (thread_index = 0;
	(thread_index < thread_count)
	&& (last_facility_started != layer_facilities->objects.end ());
	++thread_index, ++last_facility_started) {
		threads [thread_index] = std::async (std::launch::async, work_buffer_clip, length_stream_km, minimum_buffer_radius_miles, maximum_buffer_radius_miles, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router, &errors [thread_index],
		&truncation_error_histogram [thread_index],
		&two_point_instances [thread_index],
		&table [thread_index], &results [thread_index]);
		thread_status_array [thread_index] = THREAD_STATUS_RUNNING;
	}

	view->update_progress ("Waiting for threads", 0);
	queue_complete = false;
	all_threads_finished = false;
	count_points_complete = 0;
	while (!queue_complete || !all_threads_finished) {
		all_threads_finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			// Can't distinguish ready from long-since-finished, so thread_status_array has to be used to find out if all threads are complete
			if (thread_status_array [thread_index] == THREAD_STATUS_RUNNING) {
				thread_update.clear ();
				update.clear ();
				// Smaller wait time (milliseconds) produces higher cpu load becuase less time is wasted between job starts
				// 100, 50, 25
				if ((thread_status = threads [thread_index].wait_for (std::chrono::milliseconds (10))) == std::future_status::ready) {
					// std::future_status::ready can mean we thread just completed, or it's been done awhile
					if (errors [thread_index])
						++count_fail;

					if (table [thread_index].get_length () > 0) {
						++count_ok;
						if (output_table)
							fprintf (output_table, "%s", table [thread_index].get_text_ascii ());
						else
							log += table [thread_index];
					}
					else
						++count_skip; // Out of area or divergence

					log += results [thread_index];
					errors [thread_index] = false;
					table [thread_index].clear ();
					results [thread_index].clear ();
					++count_points_complete;
					update.format ("%d of %d complete, thread %d/%d.", count_points_complete, count_all_points, thread_index + 1, thread_count);
					thread_status_array [thread_index] = THREAD_STATUS_COMPLETE;

					if (!queue_complete) {
						if (last_facility_started != layer_facilities->objects.end ()) {
							// view->update_progress_formatted (0, "Starting in thread %d/%d", thread_index + 1, thread_count);
							threads [thread_index] = std::async (std::launch::async, work_buffer_clip, length_stream_km, minimum_buffer_radius_miles, maximum_buffer_radius_miles, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router,
							&errors [thread_index],
							&truncation_error_histogram [thread_index],
							&two_point_instances [thread_index],
							&table [thread_index], &results [thread_index]);
							thread_status_array [thread_index] = THREAD_STATUS_RUNNING;

							if (!threads [thread_index].valid ())
								update.add_formatted (" Thread %d/%d invalid.", thread_index + 1, thread_count);
							else
								update.add_formatted (" Thread %d/%d starting %ld.", thread_index + 1, thread_count, (*last_facility_started)->id);
							++last_facility_started;
						}
						else {
							update.add_formatted (" All facilities complete or running.");
							queue_complete = true;
						}
					}
					else
						update.add_formatted (" Thread %d will not restart.", thread_index + 1);

					// If something happened, write thread status for all threads
					write_thread_status (thread_status_array, thread_count, thread_update);
					thread_update += " : ";
					thread_update += update;
					view->update_progress (thread_update, 1);
				}
				else
					all_threads_finished = false;
			}
		}
		view->write_queued ();
	}

	delete [] thread_status_array;
	delete [] threads;

	if (!output_table) {
		view->update_progress ("Adding results to log", 0);
		for (thread_index = 0; thread_index < thread_count; ++thread_index) {
			log += results [thread_index];
			log += table [thread_index];
		}
	}

	for (thread_index = 0; thread_index < thread_count; ++thread_index) {

		for (histogram_entry = truncation_error_histogram [thread_index].begin ();
		histogram_entry != truncation_error_histogram [thread_index].end ();
		++histogram_entry) {
			if ((all_histogram_entry = all_truncation_errors.find (histogram_entry->first)) != all_truncation_errors.end ())
				all_histogram_entry->second += histogram_entry->second;
			else
				all_truncation_errors.insert (std::pair <double, long> (histogram_entry->first, histogram_entry->second));
		}

		for (two_point_entry = two_point_instances [thread_index].begin ();
		two_point_entry != two_point_instances [thread_index].end ();
		++two_point_entry)
			all_two_point_instances.insert (*two_point_entry);
	}

	log.add_formatted ("Count facilities\t%d\n", layer_facilities->objects.size ());
	log.add_formatted ("Count threads completed\t%d\n", count_points_complete);
	log.add_formatted ("Count ok\t%ld\n", count_ok);
	log.add_formatted ("Count skipped\t%ld\n", count_skip);
	log.add_formatted ("Count fail\t%ld\n", count_fail);
	{
		log.add_formatted ("Two Point Instances", layer_facilities->objects.size ());
		for (two_point_entry = all_two_point_instances.begin ();
		two_point_entry != all_two_point_instances.end ();
		++two_point_entry)
			log.add_formatted ("\t%lld", *two_point_entry);
		log += "\n";
	}

	for (histogram_entry = all_truncation_errors.begin ();
	histogram_entry != all_truncation_errors.end ();
	++histogram_entry)
		if (histogram_entry->first < 0)
			// negative areas
			log.add_formatted ("Negative %d miles\t%ld\n", histogram_entry->first, histogram_entry->second);
		else
			log.add_formatted ("Error %d miles\t%ld\n", histogram_entry->first, histogram_entry->second);

	delete [] two_point_instances;
	delete [] truncation_error_histogram;
	delete [] errors;
	delete [] results;
	delete [] table;

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

void ClipperBuffer::write_area_table
	(map_layer *layer_clipped,
	const char *direction,
	dynamic_string &log)

{
	if (tract_id_source_1 == CLIPPED_ID_SOURCE_THREE_NAMES) {
		log += "\n";
		log += layer_1_id_column_name;
		log += "\tPlaceTract";
		log += "\t";
		log += layer_2_id_column_name;
		log += "\tPLCIDFP00";
	}
	else {
		log += "\n";
		log += layer_1_id_column_name;

		if (tract_id_source_1 == CLIPPED_ID_SOURCE_ID_AND_NAME) {
			log += "\t";
			log += layer_1_name_column_name;
		}

		log += "\t";
		log += layer_2_id_column_name;

		if (tract_id_source_2 == CLIPPED_ID_SOURCE_ID_AND_NAME) {
			// 2023-08-16 Ag sales & Urban areas, write city name
			log += "\t";
			log += layer_2_name_column_name;
		}
	}

	if ((tract_id_source_2 != CLIPPED_ID_SOURCE_ID)
	&& (tract_id_source_2 != CLIPPED_ID_SOURCE_ID_AND_NAME))
		log += "\tBuffer, miles";

	log += "\t";
	log += layer_clipped->column_names_numeric [2];
	log += "\t";
	log += layer_clipped->column_names_numeric [3];
	log += "\t";
	log += layer_clipped->column_names_numeric [4];
	log += "\t";
	log += layer_clipped->column_names_numeric [5];
	log += "\tDirection\n";

	// summary table
	std::vector <map_object *>::const_iterator clip;
	for (clip = layer_clipped->objects.begin ();
	clip != layer_clipped->objects.end ();
	++clip) {
		log.add_formatted ("%ld", (long) (*clip)->attributes_numeric [0]);
		log += "\t";

		if (tract_id_source_1 == CLIPPED_ID_SOURCE_ID_AND_NAME)
			// 2023-08-16 Ag sales & Urban areas, write city name
			log += (*clip)->attributes_text[0];

		if ((tract_id_source_2 == CLIPPED_ID_SOURCE_ID)
		|| (tract_id_source_2 == CLIPPED_ID_SOURCE_ID_AND_NAME))
			log.add_formatted("%ld", (long) (*clip)->attributes_numeric[1]);
		else {
			log += (*clip)->attributes_text [0];
			log.add_formatted("\t%.2lf", (*clip)->attributes_numeric[1]);
		}

		if (tract_id_source_2 == CLIPPED_ID_SOURCE_ID_AND_NAME) {
			log += "\t";
			if (tract_id_source_1 != CLIPPED_ID_SOURCE_ID_AND_NAME)
				// 2023-08-16 Ag sales & Urban areas, write city name
				log += (*clip)->attributes_text [0];
			else
				log += (*clip)->attributes_text[1];
		}

		log.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t",
		(*clip)->attributes_numeric [2],
		(*clip)->attributes_numeric [3],
		(*clip)->attributes_numeric [4],
		(*clip)->attributes_numeric [5]);
		log += direction;
		log += "\n";
	}
}

void ClipperBuffer::write_area_table_group_by_1
	(const map_layer* layer_1,
	const map_layer* layer_clipped,
	dynamic_string& log)

// Area table with all polygons in layer_1 summed to single rows

{
	std::vector <map_object*>::const_iterator polygon, clip;
	double areas [4];
	std::set <dynamic_string> city_names;
	std::set <dynamic_string>::iterator city_name;
	dynamic_string clean_city_name;

	log += "\n";
	log += layer_1_id_column_name;

	if (tract_id_source_1 == CLIPPED_ID_SOURCE_ID_AND_NAME) {
		log += "\t";
		log += layer_1_name_column_name;
	}

	log += "\tArea, acres\tUrban % of Area";

	log += "\t";
	log += layer_clipped->column_names_numeric[2];
	/*
	log += "\t";
	log += layer_clipped->column_names_numeric[3];
	log += "\t";
	log += layer_clipped->column_names_numeric[4];
	log += "\t";
	log += layer_clipped->column_names_numeric[5];
	*/

	log += "\tCities\n";

	// 2023-08-17 NYS ag sale field circle has radius,meters in [3] and area,acres in [4]
	ASSERT (layer_1->attribute_count_numeric == 4);

	for (polygon = layer_1->objects.begin();
	polygon != layer_1->objects.end();
	++polygon) {
		memset (areas, 0, sizeof (double) * 4);
		city_names.clear ();

		for (clip = layer_clipped->objects.begin();
		clip != layer_clipped->objects.end();
		++clip) {
			if ((*clip)->attributes_numeric [0] == (double) (*polygon)->id) {
				areas[0] += (*clip)->attributes_numeric[2];
				areas[1] += (*clip)->attributes_numeric[3];
				areas[2] += (*clip)->attributes_numeric[4];
				areas[3] += (*clip)->attributes_numeric[5];

				if (tract_id_source_1 == CLIPPED_ID_SOURCE_ID_AND_NAME)
					clean_city_name = (*clip)->attributes_text[1];
				else
					clean_city_name = (*clip)->attributes_text[0];

				clean_city_name.trim_right ();
				if (city_names.find (clean_city_name) == city_names.end ())
					city_names.insert (clean_city_name);
			}
		}

		if (city_names.size () > 0) {
			log.add_formatted("%ld", (*polygon)->id);

			if (tract_id_source_1 == CLIPPED_ID_SOURCE_ID_AND_NAME) {
				log += "\t";
				log += (*polygon)->name;
			}

			// Area of polygon
			log.add_formatted("\t%.6lf", (*polygon)->attributes_numeric[3]);

			// Percent of polygon that is overlapped by urban area
			if ((*polygon)->attributes_numeric[3] != 0.0)
				log.add_formatted ("\t%.6lf", (areas [0] / ((*polygon)->attributes_numeric[3] * M2_PER_ACRE)) * 100.0);
			else
				log += "\t";

			log.add_formatted("\t%.4lf", areas[0]);
			// log.add_formatted("\t%.4lf\t%.4lf\t%.4lf\t", areas[1], areas[2], areas[3]);

			log += "\t";
			for (city_name = city_names.begin ();
			city_name != city_names.end ();
			++city_name) {
				if (city_name != city_names.begin ())
					log += ", ";
				log += *city_name;
			}
			log += "\n";
		}
	}
}

bool ClipperBuffer::make_circle
	(map_object *segment,
	const int nearest_polygon_index,
	const int nearest_point_index,
	const long next_comid_up,
	const long next_comid_down,
	const class router_NHD *router,
	const double radius_average,
	ClipperLib::Paths *buffers_up,
	ClipperLib::Paths *buffers_down,
	std::set <long long> *two_point_instances,
	dynamic_string &log) const

// ClipperLib creates about 290 points in 180 degrees on the rounded end of a 1-mile buffer

{
	bool error = false;
	bisection_definition cut_line;
	double theta, circle_angle, theta_increment = PI / 180.0;
	int division;
	logical_coordinate center_point, offset_point;
	ClipperLib::Path p_up, p_down;

	if (find_bisection_line (segment, nearest_polygon_index, nearest_point_index, next_comid_up, next_comid_down, router, radius_average, &cut_line, &theta, two_point_instances, false, log)) {
		// compute lat & long for points along two semicircles
		center_point.x = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2] / 1.0e6;
		center_point.y = (double) ((map_polygon *) segment)->nodes [nearest_point_index * 2 + 1] / 1.0e6;

		p_up.clear ();
		p_down.clear ();

		// Theta is angle to left-hand vector from center point.
		// Clockwise from it is upstream, CCW is downstream

		for (division = 0; division <= 180; ++division) {
			// clockwise
			circle_angle = theta + (theta_increment * (double) division);

			if (circle_angle < 0)
				circle_angle += 2 * PI;
			else
				circle_angle = fmod (circle_angle, 2.0 * PI);

			/*
			quadrant = ((int) floor ((circle_angle + (2 * PI)) / (PI / 2.0)) % 4) + 1;

			apply_offset (quadrant, &point_up, &point_down, circle_angle, radius_meters, center_point, &offset_point);

			p_up.push_back (ClipperLib::IntPoint((ClipperLib::cInt) (point_up.x * 1.0e6),  (ClipperLib::cInt) (point_up.y * 1.0e6)));
			p_down.push_back (ClipperLib::IntPoint((ClipperLib::cInt) (point_down.x * 1.0e6),  (ClipperLib::cInt) (point_down.y * 1.0e6)));
			*/

			offset_point.x = sin (circle_angle) * radius_average;
			offset_point.y = cos (circle_angle) * radius_average;

			p_up.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((center_point.x + offset_point.x) * 1.0e6),  (ClipperLib::cInt) ((center_point.y + offset_point.y) * 1.0e6)));
			p_down.push_back (ClipperLib::IntPoint((ClipperLib::cInt) ((center_point.x - offset_point.x) * 1.0e6),  (ClipperLib::cInt) ((center_point.y - offset_point.y) * 1.0e6)));

			/*
#ifdef _DEBUG
			log.add_formatted ("%d\t%.4lf\t%.8lf\t%.8lf\t%.8lf\t%.8lf\n", division, circle_angle, offset_point.x, offset_point.y, offset_point.x + center_point.x, offset_point.y + center_point.y);
#endif
			*/
		}
		buffers_up->push_back (p_up);
		buffers_down->push_back (p_down);

	}

	return !error;
}

bool work_concentric_circles
	(const map_object *facility,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const ClipperBuffer *clipper,
	const router_NHD *router,
	std::set <long long> *two_point_instances,
	bool *error,
	dynamic_string *table,
	dynamic_string *log)

{
	long long nearest_comid, level_path_id, buffer_id, next_comid_up, next_comid_down;
	std::map <long long, map_object *>::const_iterator nearest_segment;
	int nearest_polygon_index, nearest_point_index;
	flow_network_link *nearest_link;
	std::vector <long long> downstream_ids, upstream_ids;
	double offset_radius_average_1_mile, upstream_within_segment_m, downstream_within_segment_m;
	ClipperLib::Paths buffers_up, buffers_down, paths_clipped_up, paths_clipped_down;
	std::set <long long> index_miles_run;
	std::set <long long>::iterator index_miles;

	index_miles_run.insert (1);
	index_miles_run.insert (2);
	index_miles_run.insert (3);
	index_miles_run.insert (4);
	index_miles_run.insert (5);
	index_miles_run.insert (6);
	index_miles_run.insert (7);
	index_miles_run.insert (8);
	index_miles_run.insert (9);
	index_miles_run.insert (10);
	index_miles_run.insert (15);
	index_miles_run.insert (20);
	index_miles_run.insert (25);

	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector <dynamic_string>::const_iterator tract_id_name;
	std::vector <dynamic_string> tract_id_names_up, tract_id_names_down;

	nearest_comid = (long long) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
	nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
	nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];

	nearest_segment = router->river_layer_index.find (nearest_comid);
	level_path_id = (long long) nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

	((map_polygon *) nearest_segment->second)->perimeter_between_meters (0, nearest_point_index, &upstream_within_segment_m, *log);
	((map_polygon *) nearest_segment->second)->perimeter_between_meters (nearest_point_index, ((map_polygon *) nearest_segment->second)->total_number_of_points () - 1, &downstream_within_segment_m, *log);

	// Only evaluate if within 5 miles of river
	if (facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M] < (5.0 * METERS_PER_MILE)) {
		if (nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			{
				// Compute radius for 1 mile
				logical_coordinate center_point;
				double offset_radius_x, offset_radius_y;
				double buffer_radius_m = 1.0 * METERS_PER_MILE;

				center_point.x = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
				center_point.y = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
				offset_radius_x = offset_longitude_meters (center_point, buffer_radius_m);
				offset_radius_y = offset_latitude_meters (center_point, buffer_radius_m);
				offset_radius_average_1_mile = (offset_radius_x + offset_radius_y) / 2.0;
			}

			if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
				nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
				nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);

				for (index_miles = index_miles_run.begin ();
				index_miles != index_miles_run.end ();
				++index_miles) {
					buffer_id = *index_miles;

					buffers_up.clear ();
					buffers_down.clear ();

					// Create two semi-circles bisected at line to nearest river point
					clipper->make_circle (nearest_segment->second, nearest_polygon_index, nearest_point_index, next_comid_up, next_comid_down, router,
					offset_radius_average_1_mile * (double) *index_miles, &buffers_up, &buffers_down, two_point_instances, *log);

					clipper->intersect_overlapping (&buffers_up, layer_census_tracts, &paths_clipped_up, &tract_id_names_up, NULL, NULL, *log);
					clipper->intersect_overlapping (&buffers_down, layer_census_tracts, &paths_clipped_down, &tract_id_names_down, NULL, NULL, *log);

					for (paths_path = paths_clipped_up.begin (), tract_id_name = tract_id_names_up.begin ();
					paths_path != paths_clipped_up.end ();
					++paths_path, ++tract_id_name) {

						table->add_formatted ("%ld", facility->id);
						table->add_formatted ("\t%ld", buffer_id);
						*table += "\t";
						*table += *tract_id_name;
						table->add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tUp\n",
						area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC),
						area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT),
						area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC),
						area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_RHUMB));
					}

					for (paths_path = paths_clipped_down.begin (), tract_id_name = tract_id_names_down.begin ();
					paths_path != paths_clipped_down.end ();
					++paths_path, ++tract_id_name) {

						table->add_formatted ("%ld", facility->id);
						table->add_formatted ("\t%ld", buffer_id);
						*table += "\t";
						*table += *tract_id_name;
						table->add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tDown\n",
						area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC),
						area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT),
						area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC),
						area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_RHUMB));
					}
				}
			}
			else {
				log->add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
				*error = true;
			}
		}
		else
			log->add_formatted ("%ld\tDivergence.\n", nearest_comid);
	}
	else
		log->add_formatted ("%ld\tOutside Area (%.2lf).\n", nearest_comid, facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M]);

	return (*error == false);
}

bool ClipperBuffer::intersect_circles_threaded
	(const map_layer *layer_facilities,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const router_NHD *router,
	const int thread_count,
	FILE *output_table,
	interface_window *view,
	dynamic_string &log)

{
	int count_all_points, count_points_complete;
	int thread_index;
	std::future <bool> *threads;
	BYTE *thread_status_array;
	dynamic_string *results, *table, thread_update, update;
	std::vector <map_object *>::const_iterator last_facility_started;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false, queue_complete, all_threads_finished, *errors;
	std::future_status thread_status;
	long count_ok = 0, count_fail = 0, count_skip = 0;
	std::set <long long> *two_point_instances, all_two_point_instances;
	std::set <long long>::iterator two_point_entry;

	_ftime_s (&start);

	results = new dynamic_string [thread_count];
	table = new dynamic_string [thread_count];
	threads = new std::future <bool> [thread_count];
	thread_status_array = new BYTE [thread_count];
	errors = new bool [thread_count];
	memset (errors, 0, sizeof (bool) * thread_count);
	two_point_instances = new std::set <long long> [thread_count];

	fprintf (output_table, "Facility\tBuffer Radius, miles\tCensus Tract ID\tGeodesic\tExact\tAuthalic\tRhumb\n");

	view->update_progress ("Starting threads", 0);
	thread_index = 0;
	memset (thread_status_array, 0, sizeof (BYTE) * thread_count);
	last_facility_started = layer_facilities->objects.begin ();
	count_all_points = (int) layer_facilities->objects.size ();
	for (thread_index = 0;
	(thread_index < thread_count)
	&& (last_facility_started != layer_facilities->objects.end ());
	++thread_index, ++last_facility_started) {
		threads [thread_index] = std::async (std::launch::async, work_concentric_circles, *last_facility_started, layer_rivers, layer_census_tracts, this, router,
		&two_point_instances [thread_index],
		&errors [thread_index], &table [thread_index], &results [thread_index]);
		thread_status_array [thread_index] = THREAD_STATUS_RUNNING;
	}

	view->update_progress ("Waiting for threads", 0);
	queue_complete = false;
	all_threads_finished = false;
	count_points_complete = 0;
	while (!queue_complete || !all_threads_finished) {
		all_threads_finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			// Can't distinguish ready from long-since-finished, so thread_status_array has to be used to find out if all threads are complete
			if (thread_status_array [thread_index] == THREAD_STATUS_RUNNING) {
				thread_update.clear ();
				update.clear ();
				// Smaller wait time (milliseconds) produces higher cpu load becuase less time is wasted between job starts
				// 100, 50, 25
				if ((thread_status = threads [thread_index].wait_for (std::chrono::milliseconds (10))) == std::future_status::ready) {
					// std::future_status::ready can mean we thread just completed, or it's been done awhile
					if (errors [thread_index])
						++count_fail;
					if (table [thread_index].get_length () > 0) {
						++count_ok;
						if (output_table)
							fprintf (output_table, "%s", table [thread_index].get_text_ascii ());
						else
							log += table [thread_index];
					}
					else
						++count_skip; // Out of area or divergence

					log += results [thread_index];
					errors [thread_index] = false;
					table [thread_index].clear ();
					results [thread_index].clear ();
					++count_points_complete;
					update.format ("%d of %d complete, thread %d/%d.", count_points_complete, count_all_points, thread_index + 1, thread_count);
					thread_status_array [thread_index] = THREAD_STATUS_COMPLETE;

					if (!queue_complete) {
						if (last_facility_started != layer_facilities->objects.end ()) {
							// view->update_progress_formatted (0, "Starting in thread %d/%d", thread_index + 1, thread_count);
							threads [thread_index] = std::async (std::launch::async, work_concentric_circles, *last_facility_started, layer_rivers, layer_census_tracts, this, router,
							&two_point_instances [thread_index],
							&errors [thread_index], &table [thread_index], &results [thread_index]);
							thread_status_array [thread_index] = THREAD_STATUS_RUNNING;

							if (!threads [thread_index].valid ())
								update.add_formatted (" Thread %d/%d invalid.", thread_index + 1, thread_count);
							else
								update.add_formatted (" Thread %d/%d starting %ld.", thread_index + 1, thread_count, (*last_facility_started)->id);
							++last_facility_started;
						}
						else {
							update.add_formatted (" All facilities complete or running.");
							queue_complete = true;
						}
					}
					else
						update.add_formatted (" Thread %d will not restart.", thread_index + 1);

					// If something happened, write thread status for all threads
					write_thread_status (thread_status_array, thread_count, thread_update);
					thread_update += " : ";
					thread_update += update;
					view->update_progress (thread_update, 1);
				}
				else
					all_threads_finished = false;
			}
		}
		view->write_queued ();
	}

	delete [] thread_status_array;
	delete [] threads;

	if (!output_table) {
		view->update_progress ("Adding results to log", 0);
		for (thread_index = 0; thread_index < thread_count; ++thread_index) {
			log += results [thread_index];
			log += table [thread_index];

			for (two_point_entry = two_point_instances [thread_index].begin ();
			two_point_entry != two_point_instances [thread_index].end ();
			++two_point_entry)
				all_two_point_instances.insert (*two_point_entry);
		}
	}
	log.add_formatted ("Count facilities\t%d\n", layer_facilities->objects.size ());
	log.add_formatted ("Count threads completed\t%d\n", count_points_complete);
	log.add_formatted ("Count ok\t%ld\n", count_ok);
	log.add_formatted ("Count skipped\t%ld\n", count_skip);
	log.add_formatted ("Count fail\t%ld\n", count_fail);

	{
		log.add_formatted ("Two Point Instances", layer_facilities->objects.size ());
		for (two_point_entry = all_two_point_instances.begin ();
		two_point_entry != all_two_point_instances.end ();
		++two_point_entry)
			log.add_formatted ("\t%ld", *two_point_entry);
		log += "\n";
	}

	delete [] two_point_instances;
	delete [] errors;
	delete [] results;
	delete [] table;

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

void add_grid_cell
	(map_layer *layer_grid,
	const logical_coordinate &grid_size,
	const double x,
	const double y,
	const int grid_x,
	const int grid_y,
	const long id)

{
	map_object *cell;

	cell = layer_grid->create_new (MAP_OBJECT_POLYGON);
	cell->id = id;
	cell->attributes_numeric [0] = (double) grid_x;
	cell->attributes_numeric [1] = (double) grid_y;
	cell->attributes_numeric [2] = 0.0;
	((map_polygon *) cell)->add_point ((long) (y * 1.0e6), (long) (x * 1.0e6));
	((map_polygon *) cell)->add_point ((long) (y * 1.0e6), (long) ((x + grid_size.x) * 1.0e6));
	((map_polygon *) cell)->add_point ((long) ((y + grid_size.y) * 1.0e6), (long) ((x + grid_size.x) * 1.0e6));
	((map_polygon *) cell)->add_point ((long) ((y + grid_size.y) * 1.0e6), (long) (x * 1.0e6));
	layer_grid->objects.push_back (cell);
}

bool ClipperBuffer::create_grid
	(map_layer *layer_grid,
	const double grid_size_miles,
	const double length_stream_miles,
	map_object *river,
	const int nearest_polygon_index,
	const int nearest_point_index,
	dynamic_string &log) const

{
	// Center a grid at nearest point to facility
	// Try to cover area of largest buffer, += length_stream_miles
	int x, y, grid_radius = (int) (length_stream_miles / grid_size_miles);
	logical_coordinate center;
	bool error = false;
	long id = 0;

	logical_coordinate grid_size;

	if (river->type == MAP_OBJECT_POLYGON)
		center.set (((map_polygon *) river)->nodes [nearest_point_index * 2], ((map_polygon *) river)->nodes [nearest_point_index * 2 + 1]);
	else
		if (river->type == MAP_OBJECT_POLYGON_COMPLEX) {
			map_polygon *polygon = &((map_polygon_complex *) river)->polygons [nearest_polygon_index];
			center.set (polygon->nodes [nearest_point_index * 2], polygon->nodes [nearest_point_index * 2 + 1]);
		}

	grid_size.x = offset_longitude_meters (center, grid_size_miles * METERS_PER_MILE);
	grid_size.y = offset_latitude_meters (center, grid_size_miles * METERS_PER_MILE);

	log.add_formatted ("Center\t%.6lf\t%.6lf\n", center.x, center.y);
	log.add_formatted ("Grid size\t%.6lf\t%.6lf\n", grid_size.x, grid_size.y);
	log.add_formatted ("Grid width, m\t%.6lf\n", distance_meters (center.y, center.x, center.y, center.x + grid_size.x));
	for (x = 0; x < grid_radius; ++x) {
		for (y = 0; y < grid_radius; ++y) {

			add_grid_cell (layer_grid, grid_size, center.x - ((double) x * grid_size.x), center.y - ((double) y * grid_size.y), x, y, ++id);
			add_grid_cell (layer_grid, grid_size, center.x + ((double) x * grid_size.x), center.y + ((double) y * grid_size.y), x, y, ++id);

			add_grid_cell (layer_grid, grid_size, center.x - ((double) x * grid_size.x), center.y + ((double) y * grid_size.y), x, y, ++id);
			add_grid_cell (layer_grid, grid_size, center.x + ((double) x * grid_size.x), center.y - ((double) y * grid_size.y), x, y, ++id);
		}
	}

	return !error;
}

bool ClipperBuffer::find_start_point_upstream
	(const map_object *nearest_segment,
	const int nearest_polygon_index,
	const int nearest_point_index,
	const map_layer *layer_rivers,
	std::vector <long long> *upstream_ids,
	const double start_upstream_km,
	map_object **new_segment,
	int *new_polygon_index,
	int *new_point_index,
	const bool verbose,
	dynamic_string &log) const

// Find the point upstream at just >= start_upstream_km 

{
	double distance_within_segment_m, distance_overall_m = 0.0;
	bool error = false, done = false;
	std::vector <long long> traversed_segments;
	std::vector <double> traversed_length;

	if (look_upstream_within_segment ((map_polygon *) nearest_segment, nearest_polygon_index, nearest_point_index, start_upstream_km, &distance_within_segment_m, new_polygon_index, new_point_index, log)) {
		done = true;
		*new_segment = (map_object *) nearest_segment;
	}
	else {
		// traverse upstream_ids until length is reached

		int index, polygon_index;
		std::vector <long long>::const_iterator id;
		map_object *river;
		long last_latitude = 0, last_longitude = 0;
		double segment_length, node_length;

		distance_overall_m = distance_within_segment_m;

		traversed_length.push_back (distance_within_segment_m);
		traversed_segments.push_back (nearest_segment->id);

		if ((id = upstream_ids->begin ()) != upstream_ids->end ()) {
			// First entry in upstream_ids is closest segment, so don't add points from it.
			// upstream points for closest segment are already in path
			++id;
			while (!done
			&& (id != upstream_ids->end ())) {
				if ((river = layer_rivers->match_id (*id)) != NULL) {
					segment_length = 0.0;
					if (river->type == MAP_OBJECT_POLYGON) {

						// working upstream from highest node# towards 0
						for (index = ((map_polygon *) river)->node_count - 1;
						!done && (index >= 0);
						--index) {

							if (index < (((map_polygon *) river)->node_count - 1))
								node_length = distance_meters (last_latitude, last_longitude, ((map_polygon *) river)->nodes [index * 2 + 1], ((map_polygon *) river)->nodes [index * 2]);
							else
								node_length = 0;
							segment_length += node_length;
							distance_overall_m += node_length;

							if (distance_overall_m >= (start_upstream_km * 1000.0)) {
								done = true;
								*new_segment = river;
								*new_polygon_index = 0;
								*new_point_index = index;
							}
							else {
								last_latitude = ((map_polygon *) river)->nodes [index * 2 + 1];
								last_longitude = ((map_polygon *) river)->nodes [index * 2];
							}
						}
					}
					else {
						map_polygon *polygon;

						// complex polygon
						// polygon [0] is most upstream
						for (polygon_index = ((map_polygon_complex *) river)->polygon_count - 1;
						!done
						&& (polygon_index >= 0);
						--polygon_index) {

							polygon = &((map_polygon_complex *) river)->polygons [polygon_index];

							for (index = (polygon->node_count - 1);
							!done && (index >= 0);
							--index) {
								if (index < (polygon->node_count - 1))
									node_length = distance_meters (last_latitude, last_longitude, polygon->nodes [index * 2 + 1], polygon->nodes [index * 2]);
								else
									node_length = 0;

								segment_length += node_length;
								distance_overall_m += node_length;

								if (distance_overall_m >= (start_upstream_km * 1000.0)) {
									done = true;
									*new_segment = river;
									*new_polygon_index = polygon_index;
									*new_point_index = index;
								}
								else {
									last_latitude = polygon->nodes [index * 2 + 1];
									last_longitude = polygon->nodes [index * 2];
								}
							}
						}
					}
					traversed_length.push_back (segment_length);
					traversed_segments.push_back (*id);
				}
				else {
					error = true;
					log.add_formatted ("ERROR, segment id %ld not found in river layer in ClipperBuffer.find_start_point.\n", *id);
				}
				++id;
			}
		}
		else {
			log += "ERROR, no upstream ids.  offset_radius_average not computed.\n";
			error = true;
		}
	}

	if (verbose) {
		std::vector <long long>::const_iterator seg;
		std::vector <double>::const_iterator distance;

		for (seg = traversed_segments.begin (), distance = traversed_length.begin ();
		seg != traversed_segments.end ();
		++seg, ++distance)
			log.add_formatted ("%lld\t%.3lf\n", *seg, *distance);

		log.add_formatted ("Distance up nearest segment\t%.2lf\tm\n", distance_within_segment_m);
		log.add_formatted ("Distance up overall\t%.2lf\tm\n", distance_overall_m);
		log.add_formatted ("Start COMID\t%lld\n", (*new_segment)->id);
		log.add_formatted ("Start Point\t%d\t%d\n", *new_polygon_index, *new_point_index);
	}

	if (!done) {
		log.add_formatted ("Never reached start point upstream from segment %lld.\n", nearest_segment->id);
		error = true;
	}

	return !error;
}

bool ClipperBuffer::find_start_point_downstream
	(const map_object *nearest_segment,
	const int nearest_polygon_index,
	const int nearest_point_index,
	const map_layer *layer_rivers,
	std::vector <long long> *downstream_ids,
	const double start_downstream_km,
	map_object **new_segment,
	int *new_polygon_index,
	int *new_point_index,
	const bool verbose,
	dynamic_string &log) const

// Find the point upstream at just >= start_upstream_km 

{
	double distance_within_segment_m, distance_overall_m = 0.0;
	bool error = false, done = false;
	std::vector <long long> traversed_segments;
	std::vector <double> traversed_length;

	if (look_downstream_within_segment ((map_polygon *) nearest_segment, nearest_polygon_index, nearest_point_index, start_downstream_km, &distance_within_segment_m, new_polygon_index, new_point_index, log)) {
		done = true;
		*new_segment = (map_object *) nearest_segment;
	}
	else {
		// traverse upstream_ids until length is reached

		int index, polygon_index;
		std::vector <long long>::const_iterator id;
		map_object *river;
		long last_latitude = 0, last_longitude = 0;
		double segment_length, node_length;

		distance_overall_m = distance_within_segment_m;

		traversed_length.push_back (distance_within_segment_m);
		traversed_segments.push_back (nearest_segment->id);

		if ((id = downstream_ids->begin ()) != downstream_ids->end ()) {
			// First entry in upstream_ids is closest segment, so don't add points from it.
			// upstream points for closest segment are already in path
			++id;
			while (!done
			&& (id != downstream_ids->end ())) {
				if ((river = layer_rivers->match_id (*id)) != NULL) {
					segment_length = 0.0;
					if (river->type == MAP_OBJECT_POLYGON) {

						// working downstream from 0 towards highest node#
						for (index = 0;
						!done && (index < ((map_polygon *) river)->node_count);
						++index) {

							if (index > 0)
								node_length = distance_meters (last_latitude, last_longitude, ((map_polygon *) river)->nodes [index * 2 + 1], ((map_polygon *) river)->nodes [index * 2]);
							else
								node_length = 0;
							segment_length += node_length;
							distance_overall_m += node_length;

							if (distance_overall_m >= (start_downstream_km * 1000.0)) {
								done = true;
								*new_segment = river;
								*new_polygon_index = 0;
								*new_point_index = index;
							}
							else {
								last_latitude = ((map_polygon *) river)->nodes [index * 2 + 1];
								last_longitude = ((map_polygon *) river)->nodes [index * 2];
							}
						}
					}
					else {
						map_polygon *polygon;

						// complex polygon
						// polygon [0] is most upstream
						for (polygon_index = ((map_polygon_complex *) river)->polygon_count - 1;
						!done
						&& (polygon_index >= 0);
						--polygon_index) {

							polygon = &((map_polygon_complex *) river)->polygons [polygon_index];

							for (index = 0;
							!done && (index < polygon->node_count);
							++index) {
								if (index > 0)
									node_length = distance_meters (last_latitude, last_longitude, polygon->nodes [index * 2 + 1], polygon->nodes [index * 2]);
								else
									node_length = 0;

								segment_length += node_length;
								distance_overall_m += node_length;

								if (distance_overall_m >= (start_downstream_km * 1000.0)) {
									done = true;
									*new_segment = river;
									*new_polygon_index = polygon_index;
									*new_point_index = index;
								}
								else {
									last_latitude = polygon->nodes [index * 2 + 1];
									last_longitude = polygon->nodes [index * 2];
								}
							}
						}
					}
					traversed_length.push_back (segment_length);
					traversed_segments.push_back (*id);
				}
				else {
					error = true;
					log.add_formatted ("ERROR, segment id %lld not found in river layer in ClipperBuffer.find_start_point.\n", *id);
				}
				++id;
			}
		}
		else {
			log += "ERROR, no downstream ids.  offset_radius_average not computed.\n";
			error = true;
		}
	}

	if (verbose) {
		std::vector <long long>::const_iterator seg;
		std::vector <double>::const_iterator distance;

		for (seg = traversed_segments.begin (), distance = traversed_length.begin ();
		seg != traversed_segments.end ();
		++seg, ++distance)
			log.add_formatted ("%lld\t%.3lf\n", *seg, *distance);

		log.add_formatted ("Distance down nearest segment\t%.2lf\tm\n", distance_within_segment_m);
		log.add_formatted ("Distance down overall\t%.2lf\tm\n", distance_overall_m);
		log.add_formatted ("Start COMID\t%lld\n", (*new_segment)->id);
		log.add_formatted ("Start Point\t%d\t%d\n", *new_polygon_index, *new_point_index);
	}

	if (!done) {
		log.add_formatted ("Never reached start point downstream from segment %lld.\n", nearest_segment->id);
		error = true;
	}

	return !error;
}

void ClipperBuffer::set_column_headers
	(FILE *output_table) const
{
	if (tract_id_source_1 == CLIPPED_ID_SOURCE_THREE_NAMES)
		fprintf (output_table, "Facility\tBuffer Radius, miles\tPlaceTract ID\tCensus Tract ID\tPlace ID");
	else
		fprintf (output_table, "Facility\tBuffer Radius, miles\tCensus Tract ID");

	fprintf (output_table, "\tGeodesic\tExact\tAuthalic\tRhumb\n");
}

bool ClipperBuffer::buffer_upstream_threaded
	(const map_layer *layer_facilities,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const router_NHD *router,
	const double buffer_width_miles,
	const double start_stream_miles,
	const double length_stream_miles,
	const double cut_line_length_miles,
	const int thread_count,
	const bool write_column_headers,
	FILE *output_table,
	interface_window *view,
	dynamic_string &log)

// combine each polygon into a single large polygon

// 2018-04-13 previously called 'intersect_upstream_threaded'

{
	int count_all_points, count_points_complete;
	int thread_index;
	std::future <bool> *threads;
	BYTE *thread_status_array;
	dynamic_string *results, *table, thread_update, update;
	std::vector <map_object *>::const_iterator last_facility_started;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false, queue_complete, all_threads_finished, *errors;
	std::future_status thread_status;
	long count_ok = 0, count_fail = 0, count_skip = 0, count_skip_upstream_distance = 0, *count_skip_upstream;
	double length_stream_km, start_stream_km;

	_ftime_s (&start);

	results = new dynamic_string [thread_count];
	table = new dynamic_string [thread_count];
	threads = new std::future <bool> [thread_count];
	thread_status_array = new BYTE [thread_count];
	errors = new bool [thread_count];
	count_skip_upstream = new long [thread_count];
	memset (errors, 0, sizeof (bool) * thread_count);
	memset (count_skip_upstream, 0, sizeof (long) * thread_count);

	if (write_column_headers)
		set_column_headers (output_table);

	length_stream_km = ((double) length_stream_miles * METERS_PER_MILE) / 1000.0;
	start_stream_km = ((double) start_stream_miles * METERS_PER_MILE) / 1000.0;

	log.add_formatted ("Buffer length\t%.2lf\t%.2lf\n", length_stream_miles, length_stream_km);
	log.add_formatted ("Buffer start upstream\t%.2lf\t%.2lf\n", start_stream_miles, start_stream_km);

	view->update_progress ("Starting threads", 0);
	thread_index = 0;
	memset (thread_status_array, 0, sizeof (BYTE) * thread_count);
	last_facility_started = layer_facilities->objects.begin ();
	count_all_points = (int) layer_facilities->objects.size ();
	for (thread_index = 0;
	(thread_index < thread_count)
	&& (last_facility_started != layer_facilities->objects.end ());
	++thread_index, ++last_facility_started) {
		threads [thread_index] = std::async (std::launch::async, work_buffer_upstream, buffer_width_miles, start_stream_km, length_stream_km, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router, &count_skip_upstream [thread_index], &errors [thread_index],
		&table [thread_index], &results [thread_index]);
		thread_status_array [thread_index] = THREAD_STATUS_RUNNING;
	}

	view->update_progress ("Waiting for threads", 0);
	queue_complete = false;
	all_threads_finished = false;
	count_points_complete = 0;
	while (!queue_complete || !all_threads_finished) {
		all_threads_finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			// Can't distinguish ready from long-since-finished, so thread_status_array has to be used to find out if all threads are complete
			if (thread_status_array [thread_index] == THREAD_STATUS_RUNNING) {
				thread_update.clear ();
				update.clear ();
				// Smaller wait time (milliseconds) produces higher cpu load becuase less time is wasted between job starts
				// 100, 50, 25
				if ((thread_status = threads [thread_index].wait_for (std::chrono::milliseconds (10))) == std::future_status::ready) {
					// std::future_status::ready can mean we thread just completed, or it's been done awhile
					if (errors [thread_index])
						++count_fail;

					if (table [thread_index].get_length () > 0) {
						++count_ok;
						if (output_table)
							fprintf (output_table, "%s", table [thread_index].get_text_ascii ());
						else
							log += table [thread_index];
					}
					else
						++count_skip; // Out of area or divergence

					log += results [thread_index];
					errors [thread_index] = false;
					table [thread_index].clear ();
					results [thread_index].clear ();
					++count_points_complete;
					update.format ("%d of %d complete, thread %d/%d.", count_points_complete, count_all_points, thread_index + 1, thread_count);
					thread_status_array [thread_index] = THREAD_STATUS_COMPLETE;

					if (!queue_complete) {
						if (last_facility_started != layer_facilities->objects.end ()) {
							// view->update_progress_formatted (0, "Starting in thread %d/%d", thread_index + 1, thread_count);
							threads [thread_index] = std::async (std::launch::async, work_buffer_upstream, buffer_width_miles, start_stream_km, length_stream_km, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router,
							&count_skip_upstream [thread_index],
							&errors [thread_index],
							&table [thread_index], &results [thread_index]);
							thread_status_array [thread_index] = THREAD_STATUS_RUNNING;

							if (!threads [thread_index].valid ())
								update.add_formatted (" Thread %d/%d invalid.", thread_index + 1, thread_count);
							else
								update.add_formatted (" Thread %d/%d starting %ld.", thread_index + 1, thread_count, (*last_facility_started)->id);
							++last_facility_started;
						}
						else {
							update.add_formatted (" All facilities complete or running.");
							queue_complete = true;
						}
					}
					else
						update.add_formatted (" Thread %d will not restart.", thread_index + 1);

					// If something happened, write thread status for all threads
					write_thread_status (thread_status_array, thread_count, thread_update);
					thread_update += " : ";
					thread_update += update;
					view->update_progress (thread_update, 1);
				}
				else
					all_threads_finished = false;
			}
		}
		view->write_queued ();
	}

	delete [] thread_status_array;
	delete [] threads;

	if (!output_table) {
		view->update_progress ("Adding results to log", 0);
		for (thread_index = 0; thread_index < thread_count; ++thread_index) {
			log += results [thread_index];
			log += table [thread_index];
			count_skip_upstream_distance += count_skip_upstream [thread_index];
		}
	}

	log.add_formatted ("Count facilities\t%d\n", layer_facilities->objects.size ());
	log.add_formatted ("Count threads completed\t%d\n", count_points_complete);
	log.add_formatted ("Count ok\t%ld\n", count_ok);
	log.add_formatted ("Count skipped\t%ld\n", count_skip);
	log.add_formatted ("Count skipped for upstream distance\t%ld\n", count_skip_upstream_distance);
	log.add_formatted ("Count fail\t%ld\n", count_fail);

	delete [] count_skip_upstream;
	delete [] errors;
	delete [] results;
	delete [] table;

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

bool ClipperBuffer::buffer_upstream_threaded_difference
	(const map_layer *layer_facilities,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const router_NHD *router,
	const double buffer_width_miles,
	const double length_stream_miles,
	const double start_stream_miles,
	const double cut_line_length_miles,
	const int thread_count,
	FILE *output_table,
	interface_window *view,
	dynamic_string &log)

// 2016-05-17 Find area between start_stream_miles and length_stream_miles

// 2018-04-13 previously called 'intersect_upstream_threaded_difference

{
	int count_all_points, count_points_complete;
	int thread_index;
	std::future <bool> *threads;
	BYTE *thread_status_array;
	dynamic_string *results, *table, thread_update, update;
	std::vector <map_object *>::const_iterator last_facility_started;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false, queue_complete, all_threads_finished, *errors;
	std::future_status thread_status;
	long count_ok = 0, count_fail = 0, count_skip = 0;
	double length_stream_km, length_start_km;

	_ftime_s (&start);

	results = new dynamic_string [thread_count];
	table = new dynamic_string [thread_count];
	threads = new std::future <bool> [thread_count];
	thread_status_array = new BYTE [thread_count];
	errors = new bool [thread_count];
	memset (errors, 0, sizeof (bool) * thread_count);

	fprintf (output_table, "Facility\tBuffer Radius, miles\tCensus Tract ID\tGeodesic\tExact\tAuthalic\tRhumb\n");
	length_stream_km = ((double) length_stream_miles * METERS_PER_MILE) / 1000.0;
	length_start_km = ((double) start_stream_miles * METERS_PER_MILE) / 1000.0;

	log.add_formatted ("Buffer width, miles\t%.1lf\n", buffer_width_miles);
	log.add_formatted ("Buffer length\t%.2lf\t%.2lf\n", length_stream_miles, length_stream_km);
	log.add_formatted ("Buffer start upstream\t%.2lf\t%.2lf\n", start_stream_miles, length_start_km);

	view->update_progress ("Starting threads", 0);
	thread_index = 0;
	memset (thread_status_array, 0, sizeof (BYTE) * thread_count);
	last_facility_started = layer_facilities->objects.begin ();
	count_all_points = (int) layer_facilities->objects.size ();
	for (thread_index = 0;
	(thread_index < thread_count)
	&& (last_facility_started != layer_facilities->objects.end ());
	++thread_index, ++last_facility_started) {
		threads [thread_index] = std::async (std::launch::async, work_buffer_upstream_difference, buffer_width_miles, length_stream_km, length_start_km, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router, &errors [thread_index],
		&table [thread_index], &results [thread_index]);
		thread_status_array [thread_index] = THREAD_STATUS_RUNNING;
	}

	view->update_progress ("Waiting for threads", 0);
	queue_complete = false;
	all_threads_finished = false;
	count_points_complete = 0;
	while (!queue_complete || !all_threads_finished) {
		all_threads_finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			// Can't distinguish ready from long-since-finished, so thread_status_array has to be used to find out if all threads are complete
			if (thread_status_array [thread_index] == THREAD_STATUS_RUNNING) {
				thread_update.clear ();
				update.clear ();
				// Smaller wait time (milliseconds) produces higher cpu load becuase less time is wasted between job starts
				// 100, 50, 25
				if ((thread_status = threads [thread_index].wait_for (std::chrono::milliseconds (10))) == std::future_status::ready) {
					// std::future_status::ready can mean we thread just completed, or it's been done awhile
					if (errors [thread_index])
						++count_fail;

					if (table [thread_index].get_length () > 0) {
						++count_ok;
						if (output_table)
							fprintf (output_table, "%s", table [thread_index].get_text_ascii ());
						else
							log += table [thread_index];
					}
					else
						++count_skip; // Out of area or divergence

					log += results [thread_index];
					errors [thread_index] = false;
					table [thread_index].clear ();
					results [thread_index].clear ();
					++count_points_complete;
					update.format ("%d of %d complete, thread %d/%d.", count_points_complete, count_all_points, thread_index + 1, thread_count);
					thread_status_array [thread_index] = THREAD_STATUS_COMPLETE;

					if (!queue_complete) {
						if (last_facility_started != layer_facilities->objects.end ()) {
							// view->update_progress_formatted (0, "Starting in thread %d/%d", thread_index + 1, thread_count);
							threads [thread_index] = std::async (std::launch::async, work_buffer_upstream_difference, buffer_width_miles, length_stream_km, length_start_km, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router,
							&errors [thread_index],
							&table [thread_index], &results [thread_index]);
							thread_status_array [thread_index] = THREAD_STATUS_RUNNING;

							if (!threads [thread_index].valid ())
								update.add_formatted (" Thread %d/%d invalid.", thread_index + 1, thread_count);
							else
								update.add_formatted (" Thread %d/%d starting %ld.", thread_index + 1, thread_count, (*last_facility_started)->id);
							++last_facility_started;
						}
						else {
							update.add_formatted (" All facilities complete or running.");
							queue_complete = true;
						}
					}
					else
						update.add_formatted (" Thread %d will not restart.", thread_index + 1);

					// If something happened, write thread status for all threads
					write_thread_status (thread_status_array, thread_count, thread_update);
					thread_update += " : ";
					thread_update += update;
					view->update_progress (thread_update, 1);
				}
				else
					all_threads_finished = false;
			}
		}
		view->write_queued ();
	}

	delete [] thread_status_array;
	delete [] threads;

	if (!output_table) {
		view->update_progress ("Adding results to log", 0);
		for (thread_index = 0; thread_index < thread_count; ++thread_index) {
			log += results [thread_index];
			log += table [thread_index];
		}
	}

	log.add_formatted ("Count facilities\t%d\n", layer_facilities->objects.size ());
	log.add_formatted ("Count threads completed\t%d\n", count_points_complete);
	log.add_formatted ("Count ok\t%ld\n", count_ok);
	log.add_formatted ("Count skipped\t%ld\n", count_skip);
	// log.add_formatted ("Count skipped for upstream distance\t%ld\n", count_skip_upstream_distance);
	log.add_formatted ("Count fail\t%ld\n", count_fail);

	delete [] errors;
	delete [] results;
	delete [] table;

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

bool work_buffer_downstream
	(const double buffer_width_miles,
	const double length_stream_km,
	const double cut_line_length_miles,
	const map_object *facility,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const ClipperBuffer *clipper,
	const router_NHD *router,
	bool *error,
	dynamic_string *table,
	dynamic_string *log)

{
	long long nearest_comid, level_path_id, next_comid_down;
	std::map <long long, map_object *>::const_iterator nearest_segment;
	int nearest_polygon_index, nearest_point_index;
	flow_network_link *nearest_link;
	double offset_radius_average_1_mile, offset_radius_average, downstream_within_segment_m;
	ClipperLib::Path p;
	ClipperLib::Paths paths_down, buffers_down, paths_clipped;
	double radius_average_25km; 
	std::vector <long long> downstream_ids;
	double area_values [4];

	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector <dynamic_string>::const_iterator tract_id_name;
	std::vector <dynamic_string> tract_id_names_down;

	nearest_comid = (long long) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
	nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
	nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];

	nearest_segment = router->river_layer_index.find (nearest_comid);
	level_path_id = (long long) nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

	// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (0, nearest_point_index, &upstream_within_segment_m, *log);
	// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (nearest_point_index, ((map_polygon *) nearest_segment->second)->total_number_of_points () - 1, &downstream_within_segment_m, *log);

	{
		logical_coordinate center_point;
		double offset_radius_x, offset_radius_y;
		double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE;
		// length_stream_km = bisect_radius_m / 1000.0;

		center_point.x = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
		center_point.y = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
		offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
		offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
		radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
	}

	// Only evaluate if within 10km of river
	if (facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M] < (5.0 * METERS_PER_MILE)) {
		if (nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
				nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);

				// computed distance from nearest point in nearest segment up & down

				// discard ids beyond length_downstream_km

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper->average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, *log);

				p.clear ();
				clipper->add_downstream_within_segment ((map_polygon *) nearest_segment->second, nearest_polygon_index, nearest_point_index, &downstream_within_segment_m, length_stream_km * 1000.0, &p, NULL, *log);
				clipper->create_paths_reverse (layer_rivers, NULL, &downstream_ids, length_stream_km - (downstream_within_segment_m / 1000.0), &p, *log);
				paths_down.push_back (p);

				/*
				for (index_miles = 1;
				index_miles <= 10;
				++index_miles) {
					buffer_id = (long) index_miles;
					*/
				offset_radius_average = offset_radius_average_1_mile * buffer_width_miles;

				clipper->create_buffer (&paths_down, &buffers_down, offset_radius_average, *log);
				paths_clipped.clear ();
				tract_id_names_down.clear ();

				clipper->intersect_overlapping (&buffers_down, layer_census_tracts, &paths_clipped, &tract_id_names_down, NULL, NULL, *log);

				for (paths_path = paths_clipped.begin (), tract_id_name = tract_id_names_down.begin ();
				paths_path != paths_clipped.end ();
				++paths_path, ++tract_id_name) {

					area_values [0] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);
					area_values [1] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);
					area_values [2] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_AUTHALIC);
					area_values [3] = area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_RHUMB);

					table->add_formatted ("%ld", facility->id);
					table->add_formatted ("\t%.2lf", buffer_width_miles);
					*table += "\t";
					*table += *tract_id_name;
					table->add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tDown\n",
					area_values [0], area_values [1], area_values [2], area_values [3]);
				}
			}
			else {
				log->add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
				*error = true;
			}

		}
		else
			log->add_formatted ("%ld\tDivergence.\n", nearest_comid);
	}

	return (*error == false);
}

bool ClipperBuffer::buffer_downstream_threaded
	(const map_layer *layer_facilities,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const router_NHD *router,
	const double buffer_width_miles,
	const double start_stream_miles,
	const double length_stream_miles,
	const double cut_line_length_miles,
	const int thread_count,
	const bool write_column_headers,
	FILE *output_table,
	interface_window *view,
	dynamic_string &log)

// combine each polygon into a single large polygon

// 2018-04-13 previously called 'intersect_downstream_threaded'

{
	int count_all_points, count_points_complete;
	int thread_index;
	std::future <bool> *threads;
	BYTE *thread_status_array;
	dynamic_string *results, *table, thread_update, update;
	std::vector <map_object *>::const_iterator last_facility_started;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false, queue_complete, all_threads_finished, *errors;
	std::future_status thread_status;
	long count_ok = 0, count_fail = 0, count_skip = 0;
	double start_stream_km, length_stream_km;

	_ftime_s (&start);

	results = new dynamic_string [thread_count];
	table = new dynamic_string [thread_count];
	threads = new std::future <bool> [thread_count];
	thread_status_array = new BYTE [thread_count];
	errors = new bool [thread_count];
	memset (errors, 0, sizeof (bool) * thread_count);

	if (write_column_headers)
		set_column_headers (output_table);

	start_stream_km = ((double) start_stream_miles * METERS_PER_MILE) / 1000.0;
	length_stream_km = ((double) length_stream_miles * METERS_PER_MILE) / 1000.0;

	log.add_formatted ("Buffer length\t%.2lf\t%.2lf\n", length_stream_miles, length_stream_km);

	view->update_progress ("Starting threads", 0);
	thread_index = 0;
	memset (thread_status_array, 0, sizeof (BYTE) * thread_count);
	last_facility_started = layer_facilities->objects.begin ();
	count_all_points = (int) layer_facilities->objects.size ();
	for (thread_index = 0;
	(thread_index < thread_count)
	&& (last_facility_started != layer_facilities->objects.end ());
	++thread_index, ++last_facility_started) {
		threads [thread_index] = std::async (std::launch::async, work_buffer_downstream, buffer_width_miles, length_stream_km, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router, &errors [thread_index],
		&table [thread_index], &results [thread_index]);
		thread_status_array [thread_index] = THREAD_STATUS_RUNNING;
	}

	view->update_progress ("Waiting for threads", 0);
	queue_complete = false;
	all_threads_finished = false;
	count_points_complete = 0;
	while (!queue_complete || !all_threads_finished) {
		all_threads_finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			// Can't distinguish ready from long-since-finished, so thread_status_array has to be used to find out if all threads are complete
			if (thread_status_array [thread_index] == THREAD_STATUS_RUNNING) {
				thread_update.clear ();
				update.clear ();
				// Smaller wait time (milliseconds) produces higher cpu load becuase less time is wasted between job starts
				// 100, 50, 25
				if ((thread_status = threads [thread_index].wait_for (std::chrono::milliseconds (10))) == std::future_status::ready) {
					// std::future_status::ready can mean we thread just completed, or it's been done awhile
					if (errors [thread_index])
						++count_fail;

					if (table [thread_index].get_length () > 0) {
						++count_ok;
						if (output_table)
							fprintf (output_table, "%s", table [thread_index].get_text_ascii ());
						else
							log += table [thread_index];
					}
					else
						++count_skip; // Out of area or divergence

					log += results [thread_index];
					errors [thread_index] = false;
					table [thread_index].clear ();
					results [thread_index].clear ();
					++count_points_complete;
					update.format ("%d of %d complete, thread %d/%d.", count_points_complete, count_all_points, thread_index + 1, thread_count);
					thread_status_array [thread_index] = THREAD_STATUS_COMPLETE;

					if (!queue_complete) {
						if (last_facility_started != layer_facilities->objects.end ()) {
							// view->update_progress_formatted (0, "Starting in thread %d/%d", thread_index + 1, thread_count);
							threads [thread_index] = std::async (std::launch::async, work_buffer_downstream, buffer_width_miles, length_stream_km, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router,
							&errors [thread_index],
							&table [thread_index], &results [thread_index]);
							thread_status_array [thread_index] = THREAD_STATUS_RUNNING;

							if (!threads [thread_index].valid ())
								update.add_formatted (" Thread %d/%d invalid.", thread_index + 1, thread_count);
							else
								update.add_formatted (" Thread %d/%d starting %ld.", thread_index + 1, thread_count, (*last_facility_started)->id);
							++last_facility_started;
						}
						else {
							update.add_formatted (" All facilities complete or running.");
							queue_complete = true;
						}
					}
					else
						update.add_formatted (" Thread %d will not restart.", thread_index + 1);

					// If something happened, write thread status for all threads
					write_thread_status (thread_status_array, thread_count, thread_update);
					thread_update += " : ";
					thread_update += update;
					view->update_progress (thread_update, 1);
				}
				else
					all_threads_finished = false;
			}
		}
		view->write_queued ();
	}

	delete [] thread_status_array;
	delete [] threads;

	if (!output_table) {
		view->update_progress ("Adding results to log", 0);
		for (thread_index = 0; thread_index < thread_count; ++thread_index) {
			log += results [thread_index];
			log += table [thread_index];
		}
	}

	log.add_formatted ("Count facilities\t%d\n", layer_facilities->objects.size ());
	log.add_formatted ("Count threads completed\t%d\n", count_points_complete);
	log.add_formatted ("Count ok\t%ld\n", count_ok);
	log.add_formatted ("Count skipped\t%ld\n", count_skip);
	log.add_formatted ("Count fail\t%ld\n", count_fail);

	delete [] errors;
	delete [] results;
	delete [] table;

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

bool ClipperBuffer::buffer_downstream_threaded_difference
	(const map_layer *layer_facilities,
	const map_layer *layer_rivers,
	const map_layer *layer_census_tracts,
	const router_NHD *router,
	const double buffer_width_miles,
	const double length_stream_miles,
	const double start_miles,
	const double cut_line_length_miles,
	const int thread_count,
	FILE *output_table,
	interface_window *view,
	dynamic_string &log)

// combine each polygon into a single large polygon

// 2018-04-13 previously called 'intersect_downstream_threaded_difference'

{
	int count_all_points, count_points_complete;
	int thread_index;
	std::future <bool> *threads;
	BYTE *thread_status_array;
	dynamic_string *results, *table, thread_update, update;
	std::vector <map_object *>::const_iterator last_facility_started;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false, queue_complete, all_threads_finished, *errors;
	std::future_status thread_status;
	long count_ok = 0, count_fail = 0, count_skip = 0;
	double length_stream_km, start_km;

	_ftime_s (&start);

	results = new dynamic_string [thread_count];
	table = new dynamic_string [thread_count];
	threads = new std::future <bool> [thread_count];
	thread_status_array = new BYTE [thread_count];
	errors = new bool [thread_count];
	memset (errors, 0, sizeof (bool) * thread_count);

	fprintf (output_table, "Facility\tBuffer Radius, miles\tCensus Tract ID\tGeodesic\tExact\tAuthalic\tRhumb\n");
	length_stream_km = ((double) length_stream_miles * METERS_PER_MILE) / 1000.0;
	start_km = ((double) start_miles * METERS_PER_MILE) / 1000.0;

	log.add_formatted ("Buffer width, miles\t%.1lf\n", buffer_width_miles);
	log.add_formatted ("Buffer length Inside\t%.2lf\t%.2lf\n", start_miles, start_km);
	log.add_formatted ("Buffer length Outside\t%.2lf\t%.2lf\n", length_stream_miles, length_stream_km);

	view->update_progress ("Starting threads", 0);
	thread_index = 0;
	memset (thread_status_array, 0, sizeof (BYTE) * thread_count);
	last_facility_started = layer_facilities->objects.begin ();
	count_all_points = (int) layer_facilities->objects.size ();
	for (thread_index = 0;
	(thread_index < thread_count)
	&& (last_facility_started != layer_facilities->objects.end ());
	++thread_index, ++last_facility_started) {
		threads [thread_index] = std::async (std::launch::async, work_buffer_downstream_difference, buffer_width_miles, length_stream_km, start_km, cut_line_length_miles,
		*last_facility_started, layer_rivers, layer_census_tracts, this, router, &errors [thread_index],
		&table [thread_index], &results [thread_index]);
		thread_status_array [thread_index] = THREAD_STATUS_RUNNING;
	}

	view->update_progress ("Waiting for threads", 0);
	queue_complete = false;
	all_threads_finished = false;
	count_points_complete = 0;
	while (!queue_complete || !all_threads_finished) {
		all_threads_finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			// Can't distinguish ready from long-since-finished, so thread_status_array has to be used to find out if all threads are complete
			if (thread_status_array [thread_index] == THREAD_STATUS_RUNNING) {
				thread_update.clear ();
				update.clear ();
				// Smaller wait time (milliseconds) produces higher cpu load becuase less time is wasted between job starts
				// 100, 50, 25
				if ((thread_status = threads [thread_index].wait_for (std::chrono::milliseconds (10))) == std::future_status::ready) {
					// std::future_status::ready can mean we thread just completed, or it's been done awhile
					if (errors [thread_index])
						++count_fail;

					if (table [thread_index].get_length () > 0) {
						++count_ok;
						if (output_table)
							fprintf (output_table, "%s", table [thread_index].get_text_ascii ());
						else
							log += table [thread_index];
					}
					else
						++count_skip; // Out of area or divergence

					log += results [thread_index];
					errors [thread_index] = false;
					table [thread_index].clear ();
					results [thread_index].clear ();
					++count_points_complete;
					update.format ("%d of %d complete, thread %d/%d.", count_points_complete, count_all_points, thread_index + 1, thread_count);
					thread_status_array [thread_index] = THREAD_STATUS_COMPLETE;

					if (!queue_complete) {
						if (last_facility_started != layer_facilities->objects.end ()) {
							// view->update_progress_formatted (0, "Starting in thread %d/%d", thread_index + 1, thread_count);
							threads [thread_index] = std::async (std::launch::async, work_buffer_downstream_difference, buffer_width_miles, length_stream_km, start_km, cut_line_length_miles, *last_facility_started, layer_rivers, layer_census_tracts, this, router,
							&errors [thread_index],
							&table [thread_index], &results [thread_index]);
							thread_status_array [thread_index] = THREAD_STATUS_RUNNING;

							if (!threads [thread_index].valid ())
								update.add_formatted (" Thread %d/%d invalid.", thread_index + 1, thread_count);
							else
								update.add_formatted (" Thread %d/%d starting %ld.", thread_index + 1, thread_count, (*last_facility_started)->id);
							++last_facility_started;
						}
						else {
							update.add_formatted (" All facilities complete or running.");
							queue_complete = true;
						}
					}
					else
						update.add_formatted (" Thread %d will not restart.", thread_index + 1);

					// If something happened, write thread status for all threads
					write_thread_status (thread_status_array, thread_count, thread_update);
					thread_update += " : ";
					thread_update += update;
					view->update_progress (thread_update, 1);
				}
				else
					all_threads_finished = false;
			}
		}
		view->write_queued ();
	}

	delete [] thread_status_array;
	delete [] threads;

	if (!output_table) {
		view->update_progress ("Adding results to log", 0);
		for (thread_index = 0; thread_index < thread_count; ++thread_index) {
			log += results [thread_index];
			log += table [thread_index];
		}
	}

	log.add_formatted ("Count facilities\t%d\n", layer_facilities->objects.size ());
	log.add_formatted ("Count threads completed\t%d\n", count_points_complete);
	log.add_formatted ("Count ok\t%ld\n", count_ok);
	log.add_formatted ("Count skipped\t%ld\n", count_skip);
	log.add_formatted ("Count fail\t%ld\n", count_fail);

	delete [] errors;
	delete [] results;
	delete [] table;

	_ftime_s (&end);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}

bool read_tiger_streets
	(const dynamic_string &path_shapefile,
	dynamic_map *map_watershed,
	map_layer *layer_tiger,
	std::set <int> *state_fips,
	const bool major_roads_only,
	interface_window *update_display,
	dynamic_string &log)

// Census TIGER county street shapefiles

{
	bool error = false;
	importer_shapefile importer;
	std::set <int>::const_iterator state;
	dynamic_string state_folder;
	int county_fips;
	std::vector <dynamic_string> all_county_folders;
	std::vector <dynamic_string>::const_iterator county_folder;
	update_display->update_progress ("Reading TIGER Shapefiles");

	if (major_roads_only) {
		shapefile_filter *filter;

		// Interstate
		filter = new shapefile_filter;
		filter->type = SHAPEFILE_FILTER_MASK;
		filter->field_name = "RTTYP";
		filter->field_number = 3;
		filter->pattern = "I";
		importer.filters.push_back (filter);

		// US Highway
		filter = new shapefile_filter;
		filter->type = SHAPEFILE_FILTER_MASK;
		filter->field_name = "RTTYP";
		filter->field_number = 3;
		filter->pattern = "U";
		importer.filters.push_back (filter);

		// State Highway
		filter = new shapefile_filter;
		filter->type = SHAPEFILE_FILTER_MASK;
		filter->field_name = "RTTYP";
		filter->field_number = 3;
		filter->pattern = "S";
		importer.filters.push_back (filter);
	}

	for (state = state_fips->begin ();
	!error
	&& (state != state_fips->end ());
	++state) {
		state_folder = path_shapefile;
		state_folder.add_formatted ("%d\\", *state);

		if (list_of_matching_filenames ("tl_2015_*", state_folder, true, &all_county_folders, log)) {
			for (county_folder = all_county_folders.begin ();
			!error
			&& (county_folder != all_county_folders.end ());
			++county_folder) {
				if (county_folder->right (3) != "zip") {
					// "R:\Census\TIGER 2015\Roads\19\tl_2015_19169_roads\"
					dynamic_string d = county_folder->mid (state_folder.get_length () + 10);
					county_fips = atoi (county_folder->mid (state_folder.get_length () + 10).get_text_ascii ());
					importer.filename_source = *county_folder;
					importer.filename_source += PATH_SEPARATOR;
					importer.filename_source.add_formatted ("tl_2015_%02d%03d_roads.shp", *state, county_fips);
					importer.id_field_name = "LINEARID";
					importer.name_field_name = "FULLNAME";
					importer.take_dbf_columns = false;

					importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
					importer.normalize_longitude = false;
					if (!importer.import (layer_tiger, NULL, map_watershed, update_display, log))
						error = true;
				}
			}

		}
	}


	map_watershed->set_extent ();

	log.add_formatted ("TIGER layer size : %d\n", layer_tiger->objects.size ());

	return !error;
}


bool ClipperBuffer::intersect_threaded
	(const map_layer *layer_1,
	const map_layer *layer_2,
	map_layer *layer_intersected,
	const int thread_count,
	interface_window *view,
	dynamic_string &log) const

// For each census tract that overlaps the area of the solution
// create an intersection

{
	int job_index;
	map_layer *layer_results;
	std::vector <map_object *>::const_iterator segment;
	map_object *copy;
	_timeb start, end;
	float elapsed_milliseconds;
	bool error = false;
	thread_manager threader;
	std::map <long, thread_work *> jobs;
	thread_work_intersect *job;
	thread_manager_statistics stats;
	long id = 0;

	threader.thread_count = thread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = thread_wait_interval_ms; // 5;
	stats.reset (thread_count);
	threader.statistics = &stats;

	_ftime_s (&start);

	layer_results = new map_layer [layer_1->objects.size ()];

	view->update_progress ("Starting threads", 1);
	for (job_index = 0, segment = layer_1->objects.begin ();
	// (job_index < 2000)
	segment != layer_1->objects.end ();
	++segment, ++job_index) {
		layer_results [job_index].name.format ("Thread Results %d", job_index);
		layer_results [job_index].attribute_count_numeric = layer_intersected->attribute_count_numeric; // Space for area fields in ClipperBuffer.Paths_to_map_layer
		layer_results [job_index].attribute_count_text = layer_intersected->attribute_count_text;
		layer_results [job_index].initialize_attributes = true;

		job = new thread_work_intersect;
		job->id = job_index + 1;
		job->work_function = &work_intersect;
		job->polygon = *segment;
		job->layer_2 = layer_2;
		job->layer_results = &layer_results [job_index];
		job->tract_id_source_1 = tract_id_source_1;
		job->tract_index_source_1 = tract_index_source_1;
		job->tract_id_source_2 = tract_id_source_2;
		job->tract_index_source_2 = tract_index_source_2;
		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	if (thread_run_tiny)
		threader.run_tiny (&jobs, view, log);
	else
		threader.run (&jobs, view, log);

	view->update_progress ("Adding thread layers to map layer", 0);
	id = 0;
	for (job_index = 0; job_index < layer_1->objects.size (); ++job_index) {
		for (segment = layer_results [job_index].objects.begin ();
		segment != layer_results [job_index].objects.end ();
		++segment) {
			copy = layer_intersected->create_new ((*segment)->type);
			copy->copy (*segment, layer_intersected);
			copy->set_extent (); // set min_x, max_x, min_y, max_y
			copy->id = ++id;  // Give unique ID to polygons from threads
			layer_intersected->objects.push_back (copy);
		}
	}

	for (job_index = 0; job_index < layer_1->objects.size(); ++job_index)
		layer_results [job_index].clear_objects ();

	delete [] layer_results;

	_ftime_s (&end);

	stats.write_time_summary (log);

	elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
	log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

	return !error;
}


bool ClipperBuffer::cut_and_accumulate
	(map_object *input_polygon_1,
	ClipperLib::Paths *paths_polygon_2,
	ClipperLib::Paths *final_cuts,
	std::vector <long long> *polygon_1_ids,
	int *count_contributing,
	const long polygon_2_id,
	const bool verbose,
	dynamic_string &log)

// Called by FarmlandFinder.clip_buffers
// Clip one polygon against another (powerline buffer vs FarmlandFinder sale)
// Accumulates clips by Union with the previously clipped

{
	ClipperLib::Paths paths_powerline_buffer, clipped_region, reclipped;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	ClipperLib::Clipper c_intersect = ClipperLib::Clipper ();

	map_object_to_Path (input_polygon_1, &paths_powerline_buffer, log);

	c_intersect.Clear ();
	c_intersect.AddPaths (paths_powerline_buffer, ClipperLib::ptSubject, true);
	c_intersect.AddPaths (*paths_polygon_2, ClipperLib::ptClip, true);
	clipped_region.clear ();
	c_intersect.Execute (ClipperLib::ClipType::ctIntersection, clipped_region);

	if (clipped_region.size () > 0) {
		*count_contributing += 1;

		if (tract_id_source_1 == CLIPPED_ID_SOURCE_NUMERIC_ATTRIBUTE)
			polygon_1_ids->push_back (input_polygon_1->attributes_numeric [0]);
		else
			if (tract_id_source_1 == CLIPPED_ID_SOURCE_ID)
				// 2021-01-03 Clipping SWAT polygons vs HUC12
				polygon_1_ids->push_back (input_polygon_1->id);

		// If previous polygons were clipped, clip this output against them
		if (verbose)
			log.add_formatted ("\t%lld\t%d\t%d\t%d", polygon_2_id, *count_contributing, final_cuts->size (), clipped_region.size ());

		if (final_cuts->size () > 0) {
			c_intersect.Clear ();
			c_intersect.AddPaths (clipped_region, ClipperLib::ptSubject, true);
			c_intersect.AddPaths (*final_cuts, ClipperLib::ptClip, true);
			reclipped.clear ();
			c_intersect.Execute (ClipperLib::ClipType::ctUnion, reclipped);
			*final_cuts = reclipped;

			if (verbose)
				log.add_formatted ("\t%d\n", final_cuts->size ());
		}
		else {
			// This output becomes paths_final
			*final_cuts = clipped_region;
			if (verbose)
				log.add_formatted ("\t%d\n", final_cuts->size ());
		}
	}

	return true;
}

bool ClipperBuffer::subtract
	(ClipperLib::Paths *paths_subject,
	ClipperLib::Paths *paths_clip,
	ClipperLib::Paths *differences,
	const bool verbose,
	dynamic_string &log)

// Called by FarmlandFinder.clip_buffers
// Subtract input_polygon_1 from paths_polygon_2

{
	ClipperLib::Clipper c_intersect = ClipperLib::Clipper ();

	c_intersect.Clear ();
	c_intersect.AddPaths (*paths_subject, ClipperLib::ptSubject, true);
	c_intersect.AddPaths (*paths_clip, ClipperLib::ptClip, true);
	differences->clear ();
	c_intersect.Execute (ClipperLib::ClipType::ctDifference, *differences);

	if (verbose) {
		std::vector < ClipperLib::Path >::const_iterator paths_path;
		std::vector < ClipperLib::IntPoint >::const_iterator path_point;
		int index;

		for (paths_path = differences->begin ();
		paths_path != differences->end ();
		++paths_path) {
			log += "path\n";
			for (path_point = paths_path->begin (), index = 0;
			path_point != paths_path->end ();
			++path_point, ++index) {
				log.add_formatted ("%d\t%ld\t%ld\n", index, (long) path_point->X, (long) path_point->Y);
			}
		}
	}

	return true;
}

void ClipperBuffer::add_area_attribute
	(map_layer *layer,
	dynamic_string &log)

// Sets next numeric_attribute to km^2

{
	ClipperLib::Paths trail;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector <map_object *>::const_iterator polygon;
	double object_area_m2;
	int area_attibute_index;

	// Add and fill an area field
	area_attibute_index = layer->attribute_count_numeric;
	layer->resize (layer->attribute_count_numeric + 1, layer->attribute_count_text);
	layer->column_names_numeric.push_back ("Area, km^2");

	for (polygon = layer->objects.begin ();
	polygon != layer->objects.end ();
	++polygon) {
		trail.clear ();
		map_object_to_Path (*polygon, &trail, log);

		object_area_m2 = 0.0;

		for (paths_path = trail.begin ();
		paths_path != trail.end ();
		++paths_path)
			object_area_m2 += area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);

		(*polygon)->attributes_numeric[area_attibute_index] = std::abs (object_area_m2 / 1.0e6);
	}

}