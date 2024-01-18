
#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <stack>
#include <chrono>
#include <sys/timeb.h>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/bounding_cube.h"
#include "../util/card_bitmap.h"
#include "../util/card_bitmap_tiff.h"
#include "../util/autorange_histogram.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
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
#include "LakeErie.h"

const BYTE RECENTER_GRID_LEFT = 1;
const BYTE RECENTER_GRID_TOP = 2;
const BYTE RECENTER_GRID_RIGHT = 3;
const BYTE RECENTER_GRID_BOTTOM = 4;

void matrix_averager::add_to
	(const long row,
	const short column,
	const double value)

{
	std::map <long, std::map <short, std::pair <long, double>>>::iterator row_iterator;
	std::map <short, std::pair <long, double>>::iterator in_row;

	all_columns.insert (column);

	if ((row_iterator = data.find (row)) != data.end ()) {
		if ((in_row = row_iterator->second.find (column)) != row_iterator->second.end ()) {
			in_row->second.first += 1;
			in_row->second.second += value;
		}
		else
			row_iterator->second.insert (std::pair <short, std::pair <long, double>> (column, std::pair <long, double> (1, value)));
	}
	else {
		std::map <short, std::pair <long, double>> new_row;
		new_row.insert (std::pair <short, std::pair <long, double>> (column, std::pair <long, double> (1, value)));
		data.insert (std::pair <long, std::map <short, std::pair <long, double>>> (row, new_row));
	}
}

void matrix_averager::write
	(dynamic_string &report) const

{
	std::map <long, std::map <short, std::pair <long, double>>>::const_iterator row_iterator;
	std::map <short, std::pair <long, double>>::const_iterator in_row;
	std::set <short>::const_iterator column;

	report += "Average Boating Distance by Region\n";

	report += "Zip Code";
	for (column = all_columns.begin ();
	column != all_columns.end ();
	++column)
		report.add_formatted ("\t%d", (int) *column);
	report += "\n";

	for (row_iterator = data.begin ();
	row_iterator != data.end ();
	++row_iterator) {
		report.add_formatted ("%ld", row_iterator->first);

		for (column = all_columns.begin ();
		column != all_columns.end ();
		++column) {
			report += "\t";
			if ((in_row = row_iterator->second.find (*column)) != row_iterator->second.end ())
				report.add_formatted ("%lg", in_row->second.second / (double) in_row->second.first);
		}
		report += "\n";
	}
}

void recenter
	(map_layer *layer_grid,
	map_layer *layer_grid_centroids,
	const long id,
	const BYTE side,
	dynamic_string &log)

// Change indicated side, based on distance from center to opposite side

// Grid box nodes are 0,1,2,3 starting at the top left clockwise
// nodes array has longitude, then latitude

{
	map_object *center;
	map_polygon *box;
	long radius, new_value;

	if ((center = layer_grid_centroids->match_id (id)) != NULL) {
		if ((box = (map_polygon *) layer_grid->match_id (id)) != NULL) {
			switch (side) {
				case RECENTER_GRID_TOP:
					// bottom is ok, use this distance to set top
					radius = center->latitude - box->nodes [2 * 2 + 1];
					new_value = center->latitude + radius;
					box->nodes [0 * 2 + 1] = new_value;
					box->nodes [1 * 2 + 1] = new_value;
					break;
				case RECENTER_GRID_BOTTOM:
					// top is ok, use this distance to set bottom
					radius = box->nodes [0 * 2 + 1] - center->latitude;
					new_value = center->latitude - radius;
					box->nodes [2 * 2 + 1] = new_value;
					box->nodes [3 * 2 + 1] = new_value;
					break;
				case RECENTER_GRID_LEFT:
					// right is ok, use this distance to set left
					radius = box->nodes [2 * 2] - center->longitude;
					new_value = center->longitude - radius;
					box->nodes [0 * 2] = new_value;
					box->nodes [3 * 2] = new_value;
					break;
				case RECENTER_GRID_RIGHT:
					// left is ok, use this distance to set right
					radius = center->longitude - box->nodes [0 * 2];
					new_value = center->longitude + radius;
					box->nodes [1 * 2] = new_value;
					box->nodes [2 * 2] = new_value;
			}
		}
		else
			log.add_formatted ("ERROR, recenter can't find grid box %ld.\n", id);
	}
	else
		log.add_formatted ("ERROR, recenter can't find grid point %ld.\n", id);
}

void recenter
	(map_layer *layer_grid,
	map_layer *layer_grid_centroids,
	const long id,
	const long id_adjacent,
	const BYTE side,
	dynamic_string &log)

// Change indicated side (based on distance from center to opposite side), and then change adjacent box to match

// Grid box nodes are 0,1,2,3 starting at the top left clockwise
// nodes array has longitude, then latitude

{
	recenter (layer_grid, layer_grid_centroids, id, side, log);

	map_polygon *box, *box_adjacent;

	if ((box_adjacent = (map_polygon *) layer_grid->match_id (id_adjacent)) != NULL) {
		if ((box = (map_polygon *) layer_grid->match_id (id)) != NULL) {
			switch (side) {
				case RECENTER_GRID_TOP:
					// top of box was set, so use this value for bottom of adjacent box
					box_adjacent->nodes [2 * 2 + 1] = box->nodes [0 * 2 + 1];
					box_adjacent->nodes [3 * 2 + 1] = box->nodes [0 * 2 + 1];
					break;
				case RECENTER_GRID_BOTTOM:
					// bottom of box was set, so use this value for top of adjacent box
					box_adjacent->nodes [0 * 2 + 1] = box->nodes [2 * 2 + 1];
					box_adjacent->nodes [1 * 2 + 1] = box->nodes [2 * 2 + 1];
					break;
				case RECENTER_GRID_LEFT:
					// left of box was set, so use this value for right of adjacent box
					box_adjacent->nodes [1 * 2] = box->nodes [0 * 2];
					box_adjacent->nodes [2 * 2] = box->nodes [0 * 2];
					break;
				case RECENTER_GRID_RIGHT:
					// right of box was set, so use this value for left of adjacent box
					box_adjacent->nodes [0 * 2] = box->nodes [1 * 2];
					box_adjacent->nodes [3 * 2] = box->nodes [1 * 2];
			}
		}
		else
			log.add_formatted ("ERROR, recenter can't find grid box %ld.\n", id);
	}
	else
		log.add_formatted ("ERROR, recenter can't find adjacent grid box %ld.\n", id);
}

void LakeErie::assign_grids
	(dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

// 2016-11-03 Assign a grid ID to secchi readings

{
	map_layer *layer_grid, *layer_secchi;
	std::vector <map_object *>::iterator secchi, grid_object;
	std::set <long> matching_grids;
	std::set <long>::iterator match;
	map_polygon *grid_box;
	int grid_column_index = 4;

	if ((layer_grid = map->match ("Erie Grid")) != NULL) {
		if ((layer_secchi = map->match ("I:\\TDC\\LakeErie\\Secchi Depths\\Ohio_Lake_Erie_Water_Clarity_2011_2013.csv")) != NULL) {

			{
				// Confirm column number for Grid ID
				std::vector <dynamic_string>::iterator column_name;
				int column_index;

				for (column_name = layer_secchi->column_names_numeric.begin (), column_index = 0;
				column_name != layer_secchi->column_names_numeric.end ();
				++column_name, ++column_index)
					if (*column_name == "GridID") {
						grid_column_index = column_index;
						log.add_formatted ("Grid ID column is attributes_numeric [%d].\n", grid_column_index);
					}
			}

			for (secchi = layer_secchi->objects.begin ();
			secchi != layer_secchi->objects.end ();
			++secchi) {
				matching_grids.clear ();
				for (grid_object = layer_grid->objects.begin ();
				grid_object != layer_grid->objects.end ();
				++grid_object) {
					grid_box = (map_polygon *) *grid_object;
					if (grid_box->object_inside (*secchi))
						matching_grids.insert (grid_box->id);
				}

				if (matching_grids.size () == 0)
					log.add_formatted ("Error, Secchi %ld not inside grid.\n", (*secchi)->id);
				else
					if (matching_grids.size () > 1) {
						log.add_formatted ("Error, Secchi %ld in multiple grids.\n", (*secchi)->id);
						for (match = matching_grids.begin ();
						match != matching_grids.end ();
						++match)
							log.add_formatted ("\t%ld\n", *match);
					}
					else {
						match = matching_grids.begin ();
						(*secchi)->attributes_numeric [grid_column_index] = *match;
					}
			}
		}
	}
}

map_layer *LakeErie::read_contour_shapefile
	(dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

{
	importer_shapefile importer;
	map_layer *layer_contour = new map_layer_wx;
	layer_contour->name = "Erie Contours";
	layer_contour->type = MAP_OBJECT_POLYGON;
	layer_contour->color = RGB (192, 192, 192); // 63, 72, 204);
	layer_contour->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	map->layers.push_back (layer_contour);

	importer.filename_source = "I:\\TDC\\LakeErie\\Lake_Erie_Fishing_Grid_Map\\Lake_Erie_Contours.shp";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.longitude_central_meridian = 0;
	// 2020-04-03 Need normalized longitude to align with lake Erie geoTIFF
	importer.normalize_longitude = true;
	if (importer.import (layer_contour, NULL, map, view, log))
		return layer_contour;
	else {
		delete layer_contour;
		return NULL;
	}
}

map_layer *LakeErie::read_grid_shapefile
	(dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

{
	importer_shapefile importer;

	map_layer *layer_erie = map->create_new (MAP_OBJECT_POLYGON);
	layer_erie->name = "Erie Grid";
	layer_erie->initialize_attributes = true; // clear all attributes as polygons are created
	layer_erie->attribute_count_numeric = 1; // importer.data_field_names_numeric.size ();
	layer_erie->color = RGB (255, 0, 255);
	layer_erie->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	map->layers.push_back (layer_erie);

	importer.id_field_name = "ID";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.filename_source = "I:\\TDC\\LakeErie\\Lake_Erie_Fishing_Grid_Map\\Lake Erie Grid.shp";
	importer.take_dbf_columns = true;
	if (importer.import (layer_erie, NULL, map, view, log))
		return layer_erie;
	else {
		delete layer_erie;
		return NULL;
	}
}

void LakeErie::read
	(dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

// 2016-09-07 Shapefile from Wedong of Lake Erie fishing grid centerpoints

// Create the grid around them.  Show on map with contour & margins

// Grid is more-or-less columns 1 to 28 and rows 1(00)..10(00)

// See illustration in "I:\TDC\LakeErie\Lake_Erie_Fishing_Grid_ODNR.pdf"

{
	map_layer *layer_contour, *layer_coastal_margin, *layer_grid_centroids, *layer_grid, *layer_grid_points;
	importer_shapefile importer;

	layer_contour = read_contour_shapefile (map, view, log);

	if (layer_contour) {
		layer_coastal_margin = new map_layer_wx;
		layer_coastal_margin->name = "Erie Coastal Margin";
		layer_coastal_margin->type = MAP_OBJECT_POLYGON;
		layer_coastal_margin->color = RGB (255, 128, 64);
		layer_coastal_margin->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		map->layers.push_back (layer_coastal_margin);

		importer.filename_source = "I:\\TDC\\LakeErie\\Lake_Erie_Fishing_Grid_Map\\lake_erie_coastal_margin.shp";
		importer.projection = SHAPEFILE_PROJECTED_UTM;
		importer.longitude_central_meridian = -81;
		importer.normalize_longitude = false;
		importer.id_field_name = "id";

		if (importer.import (layer_coastal_margin, NULL, map, view, log)) {
			layer_grid_centroids = new map_layer_wx;
			layer_grid_centroids->name = "Grid Centroids";
			layer_grid_centroids->type = MAP_OBJECT_POINT;
			layer_grid_centroids->color = RGB (0, 255, 0);
			layer_grid_centroids->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			layer_grid_centroids->symbol = MAP_OBJECT_SYMBOL_PLUS;
			layer_grid_centroids->draw_labels_id = true;
			map->layers.push_back (layer_grid_centroids);

			importer.filename_source = "I:\\TDC\\LakeErie\\Lake_Erie_Fishing_Grid_Map\\Grid_centroids.shp";
			importer.projection = SHAPEFILE_PROJECTED_UTM;
			importer.longitude_central_meridian = -81;
			importer.normalize_longitude = false;
			importer.id_field_name = "grid";
			if (importer.import (layer_grid_centroids, NULL, map, view, log)) {

				layer_grid = map->create_new (MAP_OBJECT_POLYGON);
				layer_grid->name = "Erie Grid";
				layer_grid->initialize_attributes = true; // clear all attributes as polygons are created
				layer_grid->attribute_count_numeric = 1; // importer.data_field_names_numeric.size ();
				layer_grid->color = RGB (255, 0, 255);
				layer_grid->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
				map->layers.push_back (layer_grid);

				layer_grid_points = map->create_new (MAP_OBJECT_POINT);
				layer_grid_points->name = "Erie Grid Points";
				layer_grid_points->color = RGB (0, 0, 255);
				layer_grid_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
				layer_grid_points->symbol = MAP_OBJECT_SYMBOL_POINT;
				layer_grid_points->attribute_count_numeric = 1;
				layer_grid_points->column_names_numeric.push_back ("Grid ID");
				layer_grid_points->initialize_attributes = true;
				map->layers.push_back (layer_grid_points);

				// Create grid from centroids
				std::vector <map_object *>::iterator point; 
				map_object *center1, *new_point;
				map_polygon *new_box;
				long point_id = 0;
				double lat, lon;
				// long distance;
				// device_coordinate centerpoint;
				int row_index, column_index, point_row, point_column;

				// Grid is described as every 10 minutes of arc
				// Halfway between 609 and 709 is 41.998076
				// 3 * 10 seconds north of this is 41.3314092
				for (lat = 41.3314092, row_index = 1000;
				lat <= 43.0; // 42.9980762
				lat += 0.1666667, row_index -= 100) {
					// Halfway between 708 & 709 is -82.1666465.
					// 8 * 10 seconds west of this is -83.4999801
					for (lon = -83.4999801, column_index = 1;
					lon <= -78.5; // -78.62941067
					lon += 0.1666667, ++column_index) {

						if ((center1 = layer_grid_centroids->match_id (row_index + column_index)) != NULL) {
							new_box = (map_polygon *) layer_grid->create_new (MAP_OBJECT_POLYGON);
							new_box->id = row_index + column_index;
							new_box->add_point ((long) (lat * 1.0e6), (long) (lon * 1.0e6));
							new_box->add_point ((long) (lat * 1.0e6), (long) ((lon + 0.1666667) * 1.0e6));
							new_box->add_point ((long) ((lat + 0.1666667) * 1.0e6), (long) ((lon + 0.1666667) * 1.0e6));
							new_box->add_point ((long) ((lat + 0.1666667) * 1.0e6), (long) (lon * 1.0e6));
							layer_grid->objects.push_back (new_box);

							// grid point every 20 seconds, 0.005555556 degrees, 30 per square
							// Each tagged with surrounding grid id
							for (point_row = 0; point_row < 30; ++point_row) {
								for (point_column = 0; point_column < 30; ++point_column) {
									new_point = layer_grid_points->create_new (MAP_OBJECT_POINT);
									new_point->id = ++point_id;
									new_point->attributes_numeric [0] = new_box->id;
									new_point->latitude = (long) ((lat + point_row * 0.005555556 +  0.002777778) * 1.0e6);
									new_point->longitude = (long) ((lon + point_column * 0.005555556 +  0.002777778) * 1.0e6);
									layer_grid_points->objects.push_back (new_point);
								}
							}
						}
					}
				}

				/*
				map_object *center2;
				long latitude [11], longitude [29];

				// Above 100
				// The latitude between 125/225 is 42794366
				// This is 51800 below 125 center.  This far above 125 center is 42897966
				latitude [0] = 42897966;

				// Latitude 100-200
				center1 = layer_grid_centroids->match_id (124);
				center2 = layer_grid_centroids->match_id (224);
				latitude [1] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// Latitude 200-300
				center1 = layer_grid_centroids->match_id (224);
				center2 = layer_grid_centroids->match_id (324);
				latitude [2] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// Latitude 300-400
				center1 = layer_grid_centroids->match_id (322);
				center2 = layer_grid_centroids->match_id (422);
				latitude [3] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// Latitude 400-500
				center1 = layer_grid_centroids->match_id (415);
				center2 = layer_grid_centroids->match_id (515);
				latitude [4] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// Latitude 500-600
				center1 = layer_grid_centroids->match_id (512);
				center2 = layer_grid_centroids->match_id (612);
				latitude [5] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// Latitude 600-700
				center1 = layer_grid_centroids->match_id (609);
				center2 = layer_grid_centroids->match_id (709);
				latitude [6] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// latitude 700-800
				center1 = center2;
				center2 = layer_grid_centroids->match_id (809);
				latitude [7] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// latitude 800-900
				center1 = center2;
				center2 = layer_grid_centroids->match_id (909);
				latitude [8] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// latitude 900-1000
				center1 = layer_grid_centroids->match_id (907);
				center2 = layer_grid_centroids->match_id (1007);
				latitude [9] = (center1->latitude - center2->latitude) / 2 + center2->latitude;

				// Below 1000
				// The latitude between 907/1007 is 42794366
				// This is 51800 below 125 center.  This far above 125 center is 42897966
				latitude [10] = 41370000;

				// Longitude left of 701 & 801
				// The longitude between 801/802 is -83380594
				// This is 82150 to the right of 801 center.  This far left of 801 center is -83462744
				longitude [0] = -83462744;

				for (column_index = 1;
				column_index < 16;
				++column_index) {
					// longitude 701-702 through  715-716
					center1 = layer_grid_centroids->match_id (700 + column_index);
					center2 = layer_grid_centroids->match_id (700 + column_index + 1);
					longitude [column_index] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;
				}

				// longitude 616-617
				center1 = layer_grid_centroids->match_id (616);
				center2 = layer_grid_centroids->match_id (617);
				longitude [16] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;

				// longitude 617-618
				center1 = layer_grid_centroids->match_id (617);
				center2 = layer_grid_centroids->match_id (618);
				longitude [17] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;

				// longitude 618-619
				center1 = layer_grid_centroids->match_id (618);
				center2 = layer_grid_centroids->match_id (619);
				longitude [18] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;

				// longitude 619-620
				center1 = layer_grid_centroids->match_id (619);
				center2 = layer_grid_centroids->match_id (620);
				longitude [19] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;

				// longitude 420-421
				center1 = layer_grid_centroids->match_id (420);
				center2 = layer_grid_centroids->match_id (421);
				longitude [20] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;

				// longitude 321-322
				center1 = layer_grid_centroids->match_id (321);
				center2 = layer_grid_centroids->match_id (322);
				longitude [21] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;

				// longitude 322-323
				center1 = layer_grid_centroids->match_id (322);
				center2 = layer_grid_centroids->match_id (323);
				longitude [22] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;

				for (column_index = 23;
				column_index < 28;
				++column_index) {
					// longitude 223-224 through 227-228
					center1 = layer_grid_centroids->match_id (200 + column_index);
					center2 = layer_grid_centroids->match_id (200 + column_index + 1);
					longitude [column_index] =  (center2->longitude - center1->longitude) / 2 + center1->longitude;
				}

				// longitude after 228
				longitude [28] = -78860000;

				for (column_index = 0;
				column_index < 29;
				++column_index) {
					for (row_index = 1;
					row_index < 11;
					++row_index) {
						if ((center1 = layer_grid_centroids->match_id ((row_index * 100) + column_index + 1)) != NULL) {

							// 4 new points in polygon
							new_box = (map_polygon *) layer_grid->create_new (MAP_OBJECT_POLYGON);
							new_box->id = center1->id;
							new_box->add_point (latitude [row_index - 1], longitude [column_index]);
							new_box->add_point (latitude [row_index - 1], longitude [column_index + 1]);
							new_box->add_point (latitude [row_index], longitude [column_index + 1]);
							new_box->add_point (latitude [row_index], longitude [column_index]);
							layer_grid->objects.push_back (new_box);
						}
					}
				}

				// Individual adjustments based on visual inspection

				recenter (layer_grid, layer_grid_centroids, 127, RECENTER_GRID_TOP, log);
				recenter (layer_grid, layer_grid_centroids, 128, RECENTER_GRID_TOP, log);
				recenter (layer_grid, layer_grid_centroids, 128, RECENTER_GRID_RIGHT, log);
				recenter (layer_grid, layer_grid_centroids, 228, RECENTER_GRID_RIGHT, log);

				// 411 must be wider, so set it first
				recenter (layer_grid, layer_grid_centroids, 411, 410, RECENTER_GRID_LEFT, log);
				recenter (layer_grid, layer_grid_centroids, 410, RECENTER_GRID_LEFT, log);

				recenter (layer_grid, layer_grid_centroids, 410, RECENTER_GRID_TOP, log);
				recenter (layer_grid, layer_grid_centroids, 411, RECENTER_GRID_TOP, log);
				recenter (layer_grid, layer_grid_centroids, 222, RECENTER_GRID_TOP, log);

				recenter (layer_grid, layer_grid_centroids, 620, 621, RECENTER_GRID_RIGHT, log);
				recenter (layer_grid, layer_grid_centroids, 621, 622, RECENTER_GRID_RIGHT, log);
				recenter (layer_grid, layer_grid_centroids, 622, RECENTER_GRID_RIGHT, log);

				recenter (layer_grid, layer_grid_centroids, 717, 718, RECENTER_GRID_RIGHT, log);
				recenter (layer_grid, layer_grid_centroids, 718, 719, RECENTER_GRID_RIGHT, log);
				recenter (layer_grid, layer_grid_centroids, 719, RECENTER_GRID_RIGHT, log);
				*/

				map->set_extent ();

			}
		}
	}
}

bool LakeErie::read_marina_layer
	(odbc_database *db,
	dynamic_map *map,
	dynamic_string &log)

{
	long id;
	char Name [41];
	double lat, lon;
	map_object *marina;
	bool error = false;
	odbc_field_set fields;
	map_layer *layer_marinas;

	layer_marinas = new map_layer_wx;
	layer_marinas->name = "Lake Erie Marinas";
	layer_marinas->type = MAP_OBJECT_POINT;
	layer_marinas->color = RGB (255, 0, 0); // 63, 72, 204);
	layer_marinas->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_marinas->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
	layer_marinas->symbol_size = 8;
	map->layers.push_back (layer_marinas);

	fields.table_name = "Marinas";
	fields.add_field (1, &id, "Marina ID");
	fields.add_field (2, Name, 41, "Name");
	fields.add_field (3, &lat, "Latitude");
	fields.add_field (4, &lon, "Longitude");

	if (fields.open_read (db, log)) {
		if (fields.move_first (log)) {
			do {
				marina = layer_marinas->create_new (MAP_OBJECT_POINT);
				marina->id = id;
				marina->name = Name;
				marina->latitude = (long) (lat * 1.0e6);
				marina->longitude = (long) (lon * 1.0e6);
				layer_marinas->objects.push_back (marina);
			} while (fields.move_next (log));
		}
		fields.close ();
	}
	else
		error = true;

	return !error;
}

bool LakeErie::read_marina_grid_pairs
	(odbc_database *db,
	std::vector <std::pair <long, long>> *marina_grids,
	dynamic_string &log)

{
	long id, grid_id;
	bool error = false;
	odbc_field_set fields;
	double bd;

	fields.table_name = "Marina to Grid";
	fields.add_field (1, &id, "Marina ID");
	fields.add_field (2, &grid_id, "ODNR Grid");
	fields.add_field (3, &bd, "Boating Distance miles");

	if (fields.open_read (db, log, L"", L"Marina ID")) {
		if (fields.move_first (log)) {
			do {
				marina_grids->push_back (std::pair <long, long> (id, grid_id));
			} while (fields.move_next (log));
		}
		fields.close ();
	}
	else
		error = true;

	log.add_formatted ("Marina grid pairs read\t%d.\n", marina_grids->size ());

	return !error;
}

bool LakeErie::report_angler_trips_by_gridcode
	(interface_window *view,
	dynamic_string &log)

// 2016-11-21 Make 3 tables for 3 seasons of angler counts by ODNR grid

{
	odbc_database_access db;
	odbc_database_credentials creds;
	std::map <long, std::map <long, long>> grid_totals [3];
	std::map <long, std::map <long, long>>::iterator grid_total_iterator;
	std::map <long, long> id_totals;
	std::map <long, long>::iterator id_total_iterator;
	std::set <long> all_odnr_ids;
	bool error = false;
	char *season_name [3] = { "Spring", "Summer", "Fall" };

	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.set_database_filename (filename_marina_db);	

	// Wendong: add columns for ODNR grid IDs (that have no data)
	all_odnr_ids.insert (607);
	all_odnr_ids.insert (608);
	all_odnr_ids.insert (609);
	all_odnr_ids.insert (610);
	all_odnr_ids.insert (612);
	all_odnr_ids.insert (707);
	all_odnr_ids.insert (708);
	all_odnr_ids.insert (709);

	if (db.open (&creds, view, log)) {

		long NewID, SurveyGrid, ODNRGrid;
		short season, count;
		odbc_field_set fields;
		dynamic_string order;

		fields.table_name = "Angler Trips";
		fields.add_field (1, &NewID, "New ID");
		fields.add_field (2, &SurveyGrid , "Survey Grid");
		fields.add_field (3, &ODNRGrid, "ODNR Grid");
		fields.add_field (4, &season, "Season");
		fields.add_field (5, &count, "Count");

		if (fields.open_read (&db, log)) {
			if (fields.move_first (log)) {
				do {
					if ((grid_total_iterator = grid_totals [season - 1].find (NewID)) != grid_totals [season - 1].end ()) {
						// entries exist for NewID
						if ((id_total_iterator = grid_total_iterator->second.find (ODNRGrid)) != grid_total_iterator->second.end ()) {
							// Entry exists for NewID & ODNRGrid
							id_total_iterator->second += count;
						}
						else {
							// Nothing for this ODNRGrid yet
							grid_total_iterator->second.insert (std::pair <long, long> (ODNRGrid, (long) count));
						}
					}
					else {
						id_totals.clear ();
						id_totals.insert (std::pair <long, long> (ODNRGrid, (long) count));
						grid_totals [season - 1].insert (std::pair <long, std::map <long, long>> (NewID, id_totals));
					}
					all_odnr_ids.insert (ODNRGrid);
				} while (fields.move_next (log));
			}
			fields.close ();
		}
		else
			error = true;
		db.close ();


		std::set <long>::iterator odnr_iterator;
		for (season = 1; season <= 3; ++season) {
			log += "\n";
			log += season_name [season - 1];
			log += "\n";

			// Column headings
			log += "New ID";
			for (odnr_iterator = all_odnr_ids.begin ();
			odnr_iterator != all_odnr_ids.end ();
			++odnr_iterator)
				log.add_formatted ("\t%ld", *odnr_iterator);
			log += "\n";
			
			for (grid_total_iterator = grid_totals [season - 1].begin ();
			grid_total_iterator != grid_totals [season - 1].end ();
			++grid_total_iterator) {
				log.add_formatted ("%ld", grid_total_iterator->first);

				for (odnr_iterator = all_odnr_ids.begin ();
				odnr_iterator != all_odnr_ids.end ();
				++odnr_iterator) {
					log += "\t";
					if ((id_total_iterator = grid_total_iterator->second.find (*odnr_iterator)) != grid_total_iterator->second.end ())
						log.add_formatted ("%ld", id_total_iterator->second);
					else
						log += "0";
				}
				log += "\n";
			}
			
		}

	}
	else
		error = true;

	return !error;
}

bool LakeErie::report_grid_counts
	(const map_layer *layer_grid,
	const card_bitmap_tiff *tiff_image,
	dynamic_string *filename_grid_report,
	dynamic_string *filename_point_report,
	dynamic_string &log)

{
	int y, byte_width, line_index, color_index, pixel_value;
	unsigned char *line_ptr;
	bool error = false;
	std::vector <RGBQUAD>::const_iterator color_quad;
	map_object *square;
	std::map <long, autorange_histogram> histograms_by_grid;
	std::map <long, autorange_histogram>::iterator histogram;
	autorange_histogram histograms_outside;
	logical_coordinate coordinate, grid_center;
	long grid_center_x, grid_center_y;
	bounding_cube coordinate_hint;
	dynamic_string report_text;
	FILE *point_report_file, *grid_report_file;

	if (fopen_s (&point_report_file, filename_point_report->get_text_ascii (), "w") == 0) {
		fprintf (point_report_file, "x,y,longitude,latitude,value,grid,distance to center km\n");

		byte_width = tiff_image->Width () * 3;
		for (y = 0; y < tiff_image->Height (); ++y) {
			line_ptr = tiff_image->GetLinePtr (y);
			for (line_index = 0; line_index < byte_width; line_index += 3) {
				// TIFFGetField provided color maps, index by 0..255, which were saved in color_quads
				// then converted to RGB.  Convert back to get 0.255 index value
				// color = RGB (*ptr, *(ptr+1), *(ptr+2));
				for (color_quad = tiff_image->color_quads.begin (), color_index = 0, pixel_value = -1;
					(pixel_value == -1)
				&& (color_quad != tiff_image->color_quads.end ());
				++color_quad, ++color_index) {
					if ((color_quad->rgbRed == line_ptr[line_index + 2])
					&& (color_quad->rgbGreen == line_ptr[line_index + 1])
					&& (color_quad->rgbBlue == line_ptr[line_index]))
						pixel_value = color_index;
				}
				if (pixel_value != -1) {

					// Find surrounding Grid
					tiff_image->center_position (device_coordinate (line_index / 3, y), &coordinate);
					if (coordinate.x < 0.0)
						coordinate.x = 360.0 + coordinate.x;
					coordinate_hint.x[0] = coordinate.x - 0.0005;
					coordinate_hint.y[0] = coordinate.y - 0.0005;
					coordinate_hint.x[1] = coordinate_hint.x[0] + 0.001;
					coordinate_hint.y[1] = coordinate_hint.y[0] + 0.001;

					if ((square = layer_grid->find_layer_object_surrounding (coordinate, &coordinate_hint)) != NULL) {

						square->centroid (&grid_center_y, &grid_center_x);
						grid_center.x = (double) grid_center_x / 1.0e6;
						grid_center.y = (double) grid_center_y / 1.0e6;

						report_text.format ("%d,%d,%.6lf,%.6lf,%d,%ld,%.6lf\n", line_index / 3, y, coordinate.x, coordinate.y,
						pixel_value, square->id, distance_meters (coordinate, grid_center) / 1000.0);
					}
					else
						report_text.format ("%d,%d,%.6lf,%.6lf,%d,%ld,\n", line_index / 3, y, coordinate.x, coordinate.y, pixel_value, -1);

					fwrite (report_text.get_text_ascii (), report_text.get_length (), 1, point_report_file);

					if (square) {
						if ((histogram = histograms_by_grid.find (square->id)) != histograms_by_grid.end ())
							histogram->second.absorb_count ((double) pixel_value, 1);
						else {
							autorange_histogram hist;
							hist.absorb_count ((double) pixel_value, 1);
							histograms_by_grid.insert (std::pair <long, autorange_histogram> (square->id, hist));
						}
					}
					else
						histograms_outside.absorb_count ((double) pixel_value, 1);
				}
				else {
					error = true;
					log.add_formatted ("ERROR at %d,%d, unknown color %d-%d-%d\n", line_index / 3, y,
						(int) line_ptr[line_index], (int) line_ptr[line_index + 1], (int) line_ptr[line_index + 2]);
				}
			}
		}

		if (fopen_s (&grid_report_file, filename_grid_report->get_text_ascii (), "w") == 0) {
			int bin_index;
			report_text = "ID,Value,Count\n";
			fwrite (report_text.get_text_ascii (), report_text.get_length (), 1, grid_report_file);
			histograms_outside.set_unique (log);
			for (bin_index = 0; bin_index < histograms_outside.number_of_bins; ++bin_index) {
				report_text.format ("%d,%lg,%ld\n", -1, histograms_outside.bins[bin_index].ge, histograms_outside.bins[bin_index].total_count ());
				fwrite (report_text.get_text_ascii (), report_text.get_length (), 1, grid_report_file);
			}


			for (histogram = histograms_by_grid.begin (); histogram != histograms_by_grid.end (); ++histogram) {
				histogram->second.set_unique (log);
				for (bin_index = 0; bin_index < histogram->second.number_of_bins; ++bin_index) {
					report_text.format ("%d,%lg,%ld\n", histogram->first, histogram->second.bins[bin_index].ge, histogram->second.bins[bin_index].total_count ());
					fwrite (report_text.get_text_ascii (), report_text.get_length (), 1, grid_report_file);
				}
			}
			fclose (grid_report_file);
		}
		else {
			error = true;
			log += "ERROR, can't open grid report file \"";
			log += *filename_grid_report;
			log += "\".\n";
		}
		
		fclose (point_report_file);
	}
	else {
		error = true;
		log += "ERROR, can't open point report file \"";
		log += *filename_point_report;
		log += "\".\n";
	}

	return !error;
}


void LakeErie::marina_distance
	(dynamic_map *map,
	const bool all_combinations,
	interface_window *view,
	dynamic_string &log)

// 2016-11-09 Read Marina layer and list of Marina->Grid pairs
// Compute distance from each marina to center of each grid
// But only for those pairs

{
	map_layer *layer_grid, *layer_marinas, *layer_boat_trips;
	map_object_vector *trip;
	odbc_database_access db;
	odbc_database_credentials creds;
	std::vector <std::pair <long, long>> marina_grids;
	std::vector <std::pair <long, long>>::iterator mg_pair;
	logical_coordinate p1, p2;
	map_object *grid;

	// 2016-12-14 produce a table in sorted order of distances to each grid cell from each marina
	std::set <long> grid_ids;
	std::set <long>::const_iterator grid_number;
	std::map <long, std::map <long, double>> table;
	std::map <long, double> marina_to_grid_miles;
	std::map <long, std::map <long, double>>::iterator table_row;
	std::map <long, double>::const_iterator table_cell;
	std::vector <map_object *>::const_iterator grid_object;

	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.set_database_filename (filename_marina_db);
		
	// Read Marinas into new layer
	if (db.open (&creds, view, log)) {

		read_marina_layer (&db, map, log);
		if (!all_combinations)
			read_marina_grid_pairs (&db, &marina_grids, log);

		db.close ();
	}

	layer_boat_trips = new map_layer_wx;
	layer_boat_trips->name = "Boat Trips";
	layer_boat_trips->type = MAP_OBJECT_VECTOR;
	layer_boat_trips->color = RGB (0, 127, 255); // 63, 72, 204);
	layer_boat_trips->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	layer_boat_trips->symbol_size = 8;
	map->layers.push_back (layer_boat_trips);

	if ((layer_grid = map->match ("Grid Centroids")) != NULL) {

		// Keep grid IDs in set for table column names
		for (grid_object = layer_grid->objects.begin ();
		grid_object != layer_grid->objects.end ();
		++grid_object)
			grid_ids.insert ((*grid_object)->id);

		if ((layer_marinas = map->match ("Lake Erie Marinas")) != NULL) {

			if (all_combinations) {
				std::vector <map_object *>::const_iterator marina;

				for (marina = layer_marinas->objects.begin ();
				marina != layer_marinas->objects.end ();
				++marina) {
					marina_to_grid_miles.clear ();

					for (grid_object = layer_grid->objects.begin ();
					grid_object != layer_grid->objects.end ();
					++grid_object) {
							p1.set ((*marina)->longitude, (*marina)->latitude);
							p2.set ((*grid_object)->longitude, (*grid_object)->latitude);

							marina_to_grid_miles.insert (std::pair <long, double> ((*grid_object)->id, distance_meters (p1, p2) / METERS_PER_MILE));

							trip = (map_object_vector *) layer_boat_trips->create_new (MAP_OBJECT_VECTOR);
							trip->id = (*marina)->id * 10000 + (*grid_object)->id;
							trip->latitude = (*marina)->latitude;
							trip->longitude = (*marina)->longitude;
							trip->destination_latitude = (*grid_object)->latitude;
							trip->destination_longitude = (*grid_object)->longitude;
							layer_boat_trips->objects.push_back (trip);
					}
					table.insert (std::pair <long, std::map <long, double>> ((*marina)->id, marina_to_grid_miles));
				}
			}
			else {

				map_object *marina;
				std::map <long, double> *row_pointer;

				marina_to_grid_miles.clear ();

				for (mg_pair = marina_grids.begin ();
				mg_pair != marina_grids.end ();
				++mg_pair) {
					if ((marina = layer_marinas->match_id (mg_pair->first)) != NULL) {
						if ((grid = layer_grid->match_id (mg_pair->second)) != NULL) {
							// log.add_formatted ("%ld\t%ld", marina->id, grid->id);
							p1.set (marina->longitude, marina->latitude);
							p2.set (grid->longitude, grid->latitude);
							// log.add_formatted ("\t%.8lf\n", distance_meters (p1, p2) / METERS_PER_MILE);


							if ((table_row = table.find (marina->id)) != table.end ())
								row_pointer = &table_row->second;
							else {
								std::pair <std::map <long, std::map <long, double>>::iterator, bool> result;
								result = table.insert (std::pair <long, std::map <long, double>> (marina->id, marina_to_grid_miles));

								// std::map <long, std::map <long, double>>::iterator result_1;
								// result_1 = result.first;
								// row_pointer = &result_1->second;
								row_pointer = &(result.first)->second;
							}

							row_pointer->insert (std::pair <long, double> (grid->id, distance_meters (p1, p2) / METERS_PER_MILE));

							trip = (map_object_vector *) layer_boat_trips->create_new (MAP_OBJECT_VECTOR);
							trip->id = marina->id * 10000 + grid->id;
							trip->latitude = marina->latitude;
							trip->longitude = marina->longitude;
							trip->destination_latitude = grid->latitude;
							trip->destination_longitude = grid->longitude;
							layer_boat_trips->objects.push_back (trip);
						}
						else
							log.add_formatted ("ERROR, can't find grid object %ld.\n", mg_pair->second);
					}
					else
						// ID 10 missing.  Wendong: "skip it"
						log.add_formatted ("ERROR, can't find marina layer object %ld.\n", mg_pair->first);
				}
			}

			// Write table

			// column headers
			for (grid_number = grid_ids.begin ();
			grid_number != grid_ids.end ();
			++grid_number)
				log.add_formatted ("\t%ld", *grid_number);
			log += "\n";

			for (table_row = table.begin ();
			table_row != table.end ();
			++table_row) {
				log.add_formatted ("%ld", table_row->first);
				table_cell = table_row->second.begin ();
				for (grid_number = grid_ids.begin ();
				(grid_number != grid_ids.end ())
				&& (table_cell != table_row->second.end ());
				++grid_number) {
					while (*grid_number < table_cell->first) {
						log += "\t";
						++grid_number;
					}
					if (*grid_number == table_cell->first) {
						log.add_formatted ("\t%.8lf", table_cell->second);
						++table_cell;
					}
				}
				log += "\n";
			}
		}
	}
}

void LakeErie::report_relevent_travel
	(dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

// 2016-11-09 Read Marina layer and list of Marina->Grid pairs
// Compute distance from each marina to center of each grid
// But only for those pairs

// Reports are saved to "Travel Distances.xlsx"

{
	map_layer *layer_grid, *layer_marinas;
	odbc_database_access db;
	odbc_database_credentials creds;
	std::vector <std::pair <long, long>> marina_grids;
	std::vector <std::pair <long, long>>::iterator mg_pair;
	logical_coordinate p1, p2;
	map_object *grid;
	std::map <long, std::map <long, double>> zip_marina_miles;
	std::map <long, short> grid_regions;
	dynamic_string report_marina_id, report_boating_distance, report_driving_distance;

	// 2016-12-14 produce a table in sorted order of distances to each grid cell from each marina
	std::map <long, int> grid_id_counts;
	std::map <long, int>::iterator grid_number;
	std::map <long, std::map <long, double>> grid_marina_table;
	std::map <long, std::map <long, double>>::iterator grid_row;
	std::vector <map_object *>::const_iterator grid_object;
	// sum boating distance by zip and region
	matrix_averager zip_region_boating_distance;
	std::set <long> odnr_grids_not_matched;

	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.set_database_filename (filename_marina_db);
		
	// Read Marinas into new layer, also relevent marina-grid pairs
	if (db.open (&creds, view, log)) {

		read_marina_layer (&db, map, log);
		read_marina_grid_pairs (&db, &marina_grids, log);

		// read Zip to Marina
		read_zip_to_marina (&db, &zip_marina_miles, log);

		// Read region # for survey grids
		read_grid_regions (&db, &grid_regions, log);

		db.close ();
	}

	if ((layer_grid = map->match ("Grid Centroids")) != NULL) {

		// Keep grid IDs in set for table column names
		for (grid_object = layer_grid->objects.begin ();
		grid_object != layer_grid->objects.end ();
		++grid_object)
			grid_id_counts.insert (std::pair <long, int> ((*grid_object)->id, 0));

		if ((layer_marinas = map->match ("Lake Erie Marinas")) != NULL) {

			map_object *marina;
			std::map <long, double> *row_pointer;

			for (mg_pair = marina_grids.begin ();
			mg_pair != marina_grids.end ();
			++mg_pair) {

				if ((marina = layer_marinas->match_id (mg_pair->first)) != NULL) {
					if ((grid = layer_grid->match_id (mg_pair->second)) != NULL) {
						p1.set (marina->longitude, marina->latitude);
						p2.set (grid->longitude, grid->latitude);

						if ((grid_row = grid_marina_table.find (grid->id)) != grid_marina_table.end ())
							row_pointer = &grid_row->second;
						else {
							std::map <long, double> marina_to_grid_miles;
							std::pair <std::map <long, std::map <long, double>>::iterator, bool> result;
							result = grid_marina_table.insert (std::pair <long, std::map <long, double>> (grid->id, marina_to_grid_miles));

							grid_number = grid_id_counts.find (grid->id);
							grid_number->second += 1;

							row_pointer = &(result.first)->second;
						}

						row_pointer->insert (std::pair <long, double> (marina->id, distance_meters (p1, p2) / METERS_PER_MILE));

					}
					else
						log.add_formatted ("ERROR, can't find grid object %ld.\n", mg_pair->second);
				}
				else
					// ID 10 missing.  Wendong: "skip it"
					log.add_formatted ("ERROR, can't find marina layer object %ld.\n", mg_pair->first);
			}


			// Create a new map with only the nearest marinas to each zip
			std::map <long, double>::const_iterator boating_distance;
			std::map <long, std::map <long, double>>::iterator zip;
			long shortest_driving_marina;
			double shortest_distances[2];

			std::map <long, double>::iterator miles_driving;
			std::map <long, short>::iterator region;

			// column headers
			report_marina_id += "Zip";
			for (grid_number = grid_id_counts.begin ();
			grid_number != grid_id_counts.end ();
			++grid_number)
				if (grid_number->second > 0)
					report_marina_id.add_formatted ("\t%ld", grid_number->first);
			report_marina_id += "\n";

			report_boating_distance = report_driving_distance = report_marina_id;

			for (zip = zip_marina_miles.begin ();
			zip != zip_marina_miles.end ();
			++zip) {

				report_marina_id.add_formatted ("%ld", zip->first);
				report_boating_distance.add_formatted ("%ld", zip->first);
				report_driving_distance.add_formatted ("%ld", zip->first);

				for (grid_number = grid_id_counts.begin ();
				grid_number != grid_id_counts.end ();
				++grid_number) {
					if (grid_number->second > 0) {
						report_marina_id += "\t";
						report_boating_distance += "\t";
						report_driving_distance += "\t";
						if ((grid_row = grid_marina_table.find (grid_number->first)) != grid_marina_table.end ()) {

							// For each zipcode + grid, find nearest marina (to zip) among the 3-5 associated with grid
							// This gives us zip + marina + grid
							shortest_driving_marina = 0;
							shortest_distances[0] = shortest_distances[1] = DBL_MAX;

							for (boating_distance = grid_row->second.begin ();
							boating_distance != grid_row->second.end ();
							++boating_distance) {
								// Lookup driving distance from zip to this marina
								if ((miles_driving = zip->second.find (boating_distance->first)) != zip->second.end ())
									if (miles_driving->second < shortest_distances[0]) {
										shortest_driving_marina = boating_distance->first;
										shortest_distances[0] = miles_driving->second;
										shortest_distances[1] = boating_distance->second;
									}
							}

							// Best marina for this zip/grid is shortest.first
							if (shortest_driving_marina != 0) {
								report_marina_id.add_formatted ("%ld", shortest_driving_marina);
								report_driving_distance.add_formatted ("%lg", shortest_distances[0]);
								report_boating_distance.add_formatted ("%lg", shortest_distances[1]);

								// if (grid_number->first > 518)
								// 2016-12-16 Wendong: skip 516, 517, 518 since we don't have a region code for them
								if ((region = grid_regions.find (grid_number->first)) != grid_regions.end ())
									zip_region_boating_distance.add_to (zip->first, region->second, shortest_distances[1]);
								else {
									zip_region_boating_distance.add_to (zip->first, 0, shortest_distances[1]);
									odnr_grids_not_matched.insert (grid_number->first);
								}
							}
						}
					}
				}
				report_marina_id += "\n";
				report_boating_distance += "\n";
				report_driving_distance += "\n";
			}
		}
	}

	log += "Closest Relevant Marina ID\n";
	log += report_marina_id;
	log += "\n";
	log += "Boating Distance for Marina with Shortest Relevant Driving Distance\n";
	log += report_boating_distance;
	log += "\n";
	log += "Driving Distance to Closest Relevant Marina\n";
	log += report_driving_distance;

	log += "\n";
	zip_region_boating_distance.write (log);

	log += "\nUnmatched Grids\n";
	std::set <long>::iterator unmatched_grid;
	for (unmatched_grid = odnr_grids_not_matched.begin ();
	unmatched_grid != odnr_grids_not_matched.end ();
	++unmatched_grid)
		log.add_formatted ("\t%ld", *unmatched_grid);
}

bool LakeErie::read_zip_to_marina
	(odbc_database *db,
	std::map <long, std::map <long, double>> *zip_marina_miles,
	dynamic_string &log)

{
	long zip_id, marina_id;
	bool error = false;
	odbc_field_set fields;
	double distance_miles;
	char zip_address[256], marina_address[256], travel_time[256];
	std::map <long, std::map <long, double>>::iterator zip;

	fields.table_name = "Zip to Marina";
	fields.add_field (1, &zip_id, "Zip Code");
	fields.add_field (2, &marina_id, "Marina ID");
	fields.add_field (3, zip_address, 256, "Zip Address");
	fields.add_field (4, marina_address, 256, "Marina Address");
	fields.add_field (5, &distance_miles, "Distance Miles");
	fields.add_field (6, travel_time, 256, "Travel Time");

	if (fields.open_read (db, log)) {
		if (fields.move_first (log)) {
			do {

				if ((zip = zip_marina_miles->find (zip_id)) != zip_marina_miles->end ())
					zip->second.insert (std::pair <long, double> (marina_id, distance_miles));
				else {
					std::map <long, double> marina_miles;
					marina_miles.insert (std::pair <long, double> (marina_id, distance_miles));
					zip_marina_miles->insert (std::pair <long, std::map <long, double>> (zip_id, marina_miles));
				}
			} while (fields.move_next (log));
		}
		fields.close ();
	}
	else
		error = true;

	log.add_formatted ("Zip-Marina distances read\t%d.\n", zip_marina_miles->size ());

	return !error;
}

bool LakeErie::read_grid_regions
	(odbc_database *db,
	std::map <long, short> *grid_regions,
	dynamic_string &log)

{
	long survey_grid, grid_id;
	short district, region;
	bool error = false;
	odbc_field_set fields;
	char subdistrict[6];
	std::map <long, std::map <long, double>>::iterator zip;

	fields.table_name = "Survey Grid Codes";
	fields.add_field (1, &survey_grid, "Survey Grid");
	fields.add_field (2, &grid_id, "ODNR Grid");
	fields.add_field (3, &district, "ODNR District");
	fields.add_field (4, subdistrict, 6, "ODNR SubDistrict");
	fields.add_field (5, &region, "ODNR Region");

	if (fields.open_read (db, log)) {
		if (fields.move_first (log)) {
			do {
				grid_regions->insert (std::pair <long, short> (grid_id, region));
			} while (fields.move_next (log));
		}
		fields.close ();
	}
	else
		error = true;

	log.add_formatted ("Grid Regions read\t%d.\n", grid_regions->size ());

	return !error;
}

