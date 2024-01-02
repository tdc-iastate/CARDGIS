
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
#include "../util/card_bitmap.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
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
#include "GrainPrice.h"
// #include "timed_measurements.h"

prices_by_week::prices_by_week ()

{
}

prices_by_week::prices_by_week
	(const prices_by_week &other)
{
	copy (other);
}

prices_by_week prices_by_week::operator =
	(const prices_by_week &other)

{
	copy (other);
	return *this;
}

void prices_by_week::copy
	(const prices_by_week &other)

{
	prices = other.prices;
}

void prices_by_week::add_price
	(const short year,
	const short week,
	const double price,
	const long id,
	dynamic_string &log)

{
	std::map <long, double>::iterator existing_price;

	if ((existing_price = prices.find (year * 100 + week)) != prices.end ()) {
		log.add_formatted ("%ld\t%d\t%d\t%.6lf\n", id, year, week, price);
	}
	else
		prices.insert (std::pair <long, double> (year * 100 + week, price));
}

bool GrainPrice::read_warehouses
	(odbc_database *db,
	dynamic_map *map,
	dynamic_string &log)

{
	long id, gd_loca_id, gd_num, wh_num;
	char Company [41], address1 [33], address2 [33], city [26], state [3], zip [11], county [26], phone_num [14], gd_num_location [33], address_nopo [61];
	double lat, lon;
	map_object *warehouse;
	bool error = false;
	odbc_field_set fields;
	map_layer *layer_warehouses;

	layer_warehouses = new map_layer_wx;
	layer_warehouses->name = "Warehouses";
	layer_warehouses->type = MAP_OBJECT_POINT;
	layer_warehouses->color = RGB (255, 0, 0); // 63, 72, 204);
	layer_warehouses->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_warehouses->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
	layer_warehouses->symbol_size = 8;
	layer_warehouses->attribute_count_text = 1;
	map->layers.push_back (layer_warehouses);

	fields.table_name = "Warehouses";
	fields.add_field (1, &id, "Warehouse ID");
	fields.add_field (2, &gd_loca_id, "gd_loca_id");
	fields.add_field (3, &gd_num, "gd_num");
	fields.add_field (4, &wh_num, "wh_num");
	fields.add_field (5, Company, 41, "Company");
	fields.add_field (6, address1, 33, "address1");
	fields.add_field (7, address2, 33, "address2");
	fields.add_field (8, city, 26, "city");
	fields.add_field (9, state, 3, "state");
	fields.add_field (10, zip, 11, "zip");
	fields.add_field (11, county, 26, "county");
	fields.add_field (12, phone_num, 13, "phone_num");
	fields.add_field (13, &lat, "Latitude");
	fields.add_field (14, &lon, "Longitude");
	fields.add_field (15, gd_num_location, 33, "gd_num_location");
	fields.add_field (16, address_nopo, 61, "address_nopo");

	if (fields.open_read (db, log)) {
		if (fields.move_first (log)) {
			do {
				warehouse = layer_warehouses->create_new (MAP_OBJECT_POINT);
				warehouse->id = id;
				warehouse->attributes_text[0] = gd_num_location;
				warehouse->name = Company;
				warehouse->latitude = (long) (lat * 1.0e6);
				warehouse->longitude = (long) (lon * 1.0e6);
				layer_warehouses->objects.push_back (warehouse);
			} while (fields.move_next (log));
		}
		fields.close ();
	}
	else
		error = true;

	return !error;
}


bool GrainPrice::read_warehouse_prices
	(odbc_database *db,
	dynamic_map *map,
	dynamic_string &log)

{
	long id;
	Timestamp w11_date;
	short week;
	double price;
	bool error = false;
	odbc_field_set fields;
	std::pair <long, double> price_at_week;
	std::map <long, prices_by_week>::iterator weeks;
	dynamic_string duplicate_log;

	fields.table_name = "Warehouse Prices";
	fields.add_field (1, &id, "Warehouse ID");
	fields.add_field (2, &w11_date, "w11_date");
	fields.add_field (3, &week, "week");
	fields.add_field (4, &price, "price");

	if (fields.open_read (db, log)) {
		if (fields.move_first (log)) {
			do {
				if ((weeks = warehouse_prices.find (id)) != warehouse_prices.end ()) {
					weeks->second.add_price (w11_date.get_year (), week, price, id, duplicate_log);
				}
				else {
					prices_by_week all_prices;
					all_prices.add_price (w11_date.get_year (), week, price, id, duplicate_log);
					warehouse_prices.insert (std::pair <long, prices_by_week> (id, all_prices));
				}
			} while (fields.move_next (log));
		}
		fields.close ();
	}
	else
		error = true;

	log += "\nDuplicate Prices\n";
	log += "Warehouse ID\tYear\tWeek\tPrice\n";
	log += duplicate_log;

	return !error;
}

bool GrainPrice::read_township_prices
	(odbc_database *db,
	dynamic_map *map,
	dynamic_string &log)

{
	long id;
	short year, week;
	double price;
	bool error = false;
	odbc_field_set fields;
	std::pair <long, double> price_at_week;
	std::map <long, prices_by_week>::iterator weeks;
	dynamic_string duplicate_log;

	fields.table_name = "Township Prices";
	fields.add_field (1, &id, "orig_fid");
	fields.add_field (2, &year, "Year");
	fields.add_field (3, &week, "week");
	fields.add_field (4, &price, "price");

	if (fields.open_read (db, log)) {
		if (fields.move_first (log)) {
			do {
				if ((weeks = township_prices.find (id)) != township_prices.end ())
					weeks->second.add_price (year, week, price, id, duplicate_log);
				else {
					prices_by_week all_prices;
					all_prices.add_price (year, week, price, id, duplicate_log);
					township_prices.insert (std::pair <long, prices_by_week> (id, all_prices));
				}

			} while (fields.move_next (log));
		}
		fields.close ();
	}
	else
		error = true;

	log += "\nDuplicate Prices\n";
	log += "Township ID\tYear\tWeek\tPrice\n";
	log += duplicate_log;

	return !error;
}

bool GrainPrice::read_townships
	(odbc_database *db,
	dynamic_map *map,
	dynamic_string &log)

{
	long id, cousubfp10, cousubns10;
	char Name [26], geoid10 [11];
	double lat, lon;
	short state_fips, county_fips;
	map_object *point;
	bool error = false;
	odbc_field_set fields;
	map_layer *layer_points;

	layer_points = new map_layer_wx;
	layer_points->name = "Townships";
	layer_points->type = MAP_OBJECT_POINT;
	layer_points->color = RGB (63, 72, 204);
	layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_points->symbol = MAP_OBJECT_SYMBOL_PLUS;
	layer_points->symbol_size = 8;
	map->layers.push_back (layer_points);

	fields.table_name = "Townships";
	fields.add_field (1, &id, "orig_fid");
	fields.add_field (2, geoid10, 11, "Geoid10");
	fields.add_field (3, &state_fips, "State FIPS");
	fields.add_field (4, &county_fips, "County FIPS");
	fields.add_field (5, &cousubfp10, "cousubfp10");
	fields.add_field (6, &cousubns10, "cousubns10");
	fields.add_field (7, Name, 26, "Name");
	fields.add_field (8, &lat, "Latitude");
	fields.add_field (9, &lon, "Longitude");

	if (fields.open_read (db, log)) {
		if (fields.move_first (log)) {
			do {
				point = layer_points->create_new (MAP_OBJECT_POINT);
				point->id = id;
				point->name = Name;
				point->latitude = (long) (lat * 1.0e6);
				point->longitude = (long) (lon * 1.0e6);
				layer_points->objects.push_back (point);
			} while (fields.move_next (log));
		}
		fields.close ();
	}
	else
		error = true;

	return !error;
}

bool GrainPrice::setup
	(dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

{
	odbc_database_access db;
	odbc_database_credentials creds;
	bool error = false;

	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.set_database_filename (filename_db);
		
	// Read Marinas into new layer
	if (db.open (&creds, view, log)) {

		read_warehouses (&db, map, log);
		read_warehouse_prices (&db, map, log);
		read_townships (&db, map, log);
		read_township_prices (&db, map, log);

		db.close ();
	}
	else
		error = true;

	return !error;
}

void GrainPrice::distance_warehouse_township
	(dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

// 2017-01-03 Find nearest township for each warehouse

{
	map_layer *layer_warehouses, *layer_townships, *layer_links;
	map_object_vector *trip;
	odbc_database_access db;
	odbc_database_credentials creds;
	double distance_m, closest_m;
	map_object *closest_township;
	logical_coordinate p1, p2;
	std::vector <map_object *>::const_iterator warehouse, township;

	layer_links = new map_layer_wx;
	layer_links->name = "Links";
	layer_links->type = MAP_OBJECT_VECTOR;
	layer_links->color = RGB (192, 192, 192); // 63, 72, 204);
	layer_links->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	layer_links->symbol_size = 8;
	layer_links->attribute_count_numeric = 2;
	layer_links->attribute_count_text = 1;
	map->layers.push_back (layer_links);

	if ((layer_warehouses = map->match ("Warehouses")) != NULL) {

		if ((layer_townships = map->match ("Townships")) != NULL) {

			for (warehouse = layer_warehouses->objects.begin ();
			warehouse != layer_warehouses->objects.end ();
			++warehouse) {

				closest_m = DBL_MAX;
				closest_township = NULL;

				for (township = layer_townships->objects.begin ();
				township != layer_townships->objects.end ();
				++township) {
					p1.set ((*warehouse)->longitude, (*warehouse)->latitude);
					p2.set ((*township)->longitude, (*township)->latitude);

					distance_m = distance_meters (p1, p2);
					if (distance_m < closest_m) {
						closest_m = distance_m;
						closest_township = *township;
					}
				}
				trip = (map_object_vector *) layer_links->create_new (MAP_OBJECT_VECTOR);
				trip->id = (*warehouse)->id;
				trip->attributes_numeric[0] = (double) closest_township->id;
				trip->attributes_numeric[1] = closest_m;
				trip->attributes_text[0] = (*warehouse)->attributes_text [0];
				trip->latitude = (*warehouse)->latitude;
				trip->longitude = (*warehouse)->longitude;
				trip->destination_latitude = closest_township->latitude;
				trip->destination_longitude = closest_township->longitude;
				layer_links->objects.push_back (trip);
			}
		}
	}
}

void GrainPrice::compare_prices
	(dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

// 2017-01-03 Show weekly prices for each warehouse & closest township

{
	map_layer *layer_links;
	std::vector <map_object *>::const_iterator warehouse;
	std::map <long, prices_by_week>::const_iterator warehouse_price_set, township_price_set;
	std::map <long, double>::const_iterator price_at_warehouse, price_at_township;
	long year, week;

	if ((layer_links = map->match ("Links")) != NULL) {

		log += "\nWarehouse ID\tgd_num_location\torig_fid\tYear\tWeek\tPrice\tPrice\tDiff\n";

		for (warehouse = layer_links->objects.begin ();
		warehouse != layer_links->objects.end ();
		++warehouse) {
			warehouse_price_set = warehouse_prices.find ((*warehouse)->id);
			township_price_set = township_prices.find ((long) (*warehouse)->attributes_numeric [0]);

			// Go through prices at each location
			for (price_at_warehouse = warehouse_price_set->second.prices.begin ();
			price_at_warehouse != warehouse_price_set->second.prices.end ();
			++price_at_warehouse) {
				log.add_formatted ("%ld", (*warehouse)->id);
				log += "\t";
				log += (*warehouse)->attributes_text[0];
				log.add_formatted ("\t%ld", township_price_set->first);
	
				// warehouse_price_set is map of prices indexed by time (year * 100 + week)
				year = price_at_warehouse->first / 100;
				week = price_at_warehouse->first % 100;
				log.add_formatted ("\t%ld\t%ld\t%.6lf", year, week, price_at_warehouse->second);

				if ((price_at_township = township_price_set->second.prices.find (price_at_warehouse->first)) != township_price_set->second.prices.end ()) {
					log.add_formatted ("\t%.6lf", price_at_township->second);
					log.add_formatted ("\t%.6lf", price_at_warehouse->second - price_at_township->second);
				}
				else
					log.add_formatted ("\t\t%.6lf", price_at_warehouse->second);

				log += "\n";
			}
		}
	}
}
