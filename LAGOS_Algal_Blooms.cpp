#include "../util/utility_afx.h"
#include <vector>
#include <chrono>
#include <map>
#include <set>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/state_names.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/shapefile_filter.h"
#include "LAGOS_Algal_Blooms.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

void zillow_point_callback
	(const map_object *point,
	const char delimiter,
	dynamic_string &log)

// Called by work_MapLayerProximity to write identifiers & data for Zillow Sales point

{
	// int state_fips, county_fips;

	// ID\tRow ID\tImportParc\tPropertyFullStreetAddress\tSalesDate\tStateFIPS\tCountyFIPS\tSalesPriceAmount\tLot Size ft^2\tlatitude\tlongitude
	// \tLAGOS ID\tdistance, m\t9 month chla\t12 month chla
	log.add_formatted ("%ld", point->id);
	log += delimiter;
	log += "\"";
	log += point->name; // RowID
	log += "\"";

	log += delimiter;
	log.add_formatted ("%.1lf", point->attributes_numeric [0]); // ImportParc

	/*
	// 2021-04-20 Wendong: skip these columns
	log += delimiter;
	log += point->attributes_text [1];// PropertyFullStreetAddress
	log += delimiter;
	if (point->attributes_void)
		log += ((Timestamp *) point->attributes_void)->write (TIMESTAMP_YYYY_MM_DD);// Sales Date

	state_fips = (int) point->attributes_numeric[2];
	county_fips = state_fips % 1000;
	state_fips = (state_fips - county_fips) / 1000;
	log.add_formatted ("%c%d%c%d", delimiter, state_fips, delimiter, county_fips);

	log.add_formatted ("%c%.2lf", delimiter, point->attributes_numeric [1]); // SalesPriceAmount
	log.add_formatted ("%c%.2lf", delimiter, point->attributes_numeric [3]); // LotSizeSquareFeet

	log.add_formatted ("%c%.8lf%c%.8lf", delimiter, (double) point->latitude / 1.0e6, delimiter, (double) point->longitude / 1.0e6);
	*/

	// Lagos Lake ID, distance, m, 9-mo chla, 12-mo chla
	log.add_formatted ("%c%ld%c%.8lf%c%.8lf%c%.8lf",
	delimiter, (long) point->attributes_numeric [4],
	delimiter, point->attributes_numeric [5],
	delimiter, point->attributes_numeric [6],
	delimiter, point->attributes_numeric [7]);
}

void nhd_river_callback
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log)

// Called by work_MapLayerProximity

{
	// ID\tStreamLevel\tFtype\tDistance
	log += delimiter;
	log.add_formatted ("%ld", point->id);

	// log += delimiter;
	// log += "\"";
	// log += point->name; // RowID
	// log += "\"";

	// StreamLevel
	log += delimiter;
	log.add_formatted ("%d", (int) point->attributes_numeric[NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL]);

	// NHD_FTYPE_CANALDITCH or other
	log += delimiter;
	log += "\"";
	log += NHD_FTYPE_name ((int) point->attributes_numeric[NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE]);
	log += "\"";

	log += delimiter;
	log.add_formatted ("%.6lf", distance_m);
}

void nhd_river_column_headers_callback
	(const char delimiter,
	dynamic_string &column_names)
{
	column_names.clear ();
	column_names += delimiter;
	column_names += "\"NHD ID\"";
	// column_names += delimiter;
	// column_names += "\"Name\"";
	column_names += delimiter;
	column_names += "\"Streamlevel\"";
	column_names += delimiter;
	column_names += "\"Ftype\"";
	column_names += delimiter;
	column_names += "\"Distance, m\"";
}

void nhd_lake_callback
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log)

// Called by work_MapLayerProximity to write ID, Name, distance

{
	// ID\tName\tDistance
	log += delimiter;
	log.add_formatted ("%ld", point->id);
	log += delimiter;
	log += "\"";
	log += point->name; // RowID
	log += "\"";
	log += delimiter;
	log.add_formatted ("%.6lf", distance_m);

}

void nhd_lake_column_headers_callback
	(const char delimiter,
	dynamic_string &column_names)
{
	column_names.clear ();
	column_names += delimiter;
	column_names += "\"NHD Lake ID\"";
	column_names += delimiter;
	column_names += "\"Name\"";
	column_names += delimiter;
	column_names += "\"Distance, m\"";
}

LAGOS_chla_reading::LAGOS_chla_reading ()

{
	lake_id = 0L;
	value = 0.0;
}

LAGOS_chla_reading::LAGOS_chla_reading
	(const LAGOS_chla_reading &other)

{
	copy (other);
}

LAGOS_chla_reading LAGOS_chla_reading::operator =
	(const LAGOS_chla_reading &other)
{
	if (this != &other)
		copy(other);

	return (*this);
}

void LAGOS_chla_reading::copy
	(const LAGOS_chla_reading &other)
{
	when = other.when;
	lake_id = other.lake_id;
	value = other.value;
}


LAGOS_Algal_Blooms::LAGOS_Algal_Blooms ()
{
	layer_lagos = NULL;
	layer_output = NULL;
}


LAGOS_Algal_Blooms::~LAGOS_Algal_Blooms ()
{
}

bool LAGOS_Algal_Blooms::read_shapefile
	(const dynamic_string &filename_lagos_shapefile,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

{
	importer_shapefile shapefile;

	layer_lagos = map->create_new (MAP_OBJECT_POINT);
	layer_lagos->name = "LAGOS";
	layer_lagos->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_lagos->symbol = MAP_OBJECT_SYMBOL_PLUS;
	layer_lagos->color = RGB (255, 0, 0);
	layer_lagos->include_in_legend = true;
	map->layers.push_back (layer_lagos);

	shapefile.filename_source = filename_lagos_shapefile;
	shapefile.id_field_name = "lagoslakei";
	shapefile.name_field_name = "gnis_name";
	shapefile.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	if (shapefile.import (layer_lagos, NULL, map, view, log)) {
		return true;
	}
	else
		return false;
}

bool LAGOS_Algal_Blooms::match_lakes
	(map_layer *layer_lakes,
	interface_window *view,
	dynamic_string &log)

// Find the enclosing NHD lake for each LAGOS point
// Or the nearest if none surrounding

{
	std::vector <map_object *>::iterator lagos_point, lake;
	std::set <long> lakes_linked;
	bounding_cube lake_area;
	LAGOS_Algal_Blooms lagos;
	map_object *found, *lake_copy;
	bool error = false;

	log += "\nLAGOS ID\tName\tNHD Lake ID\tName\n";
	for (lake = layer_lakes->objects.begin ();
	lake != layer_lakes->objects.end ();
	++lake) {
		(*lake)->check_extent (&lake_area);
		for (lagos_point = layer_lagos->objects.begin ();
		lagos_point != layer_lagos->objects.end ();
		++lagos_point) {
			if ((*lagos_point)->overlaps_logical (&lake_area)) {
				found = NULL;

				if ((*lake)->type == MAP_OBJECT_POLYGON) {
					if (((map_polygon *) *lake)->object_inside (*lagos_point))
						found = *lake;
				}
				else
					if ((*lake)->type == MAP_OBJECT_POLYGON_COMPLEX) {
						if (((map_polygon_complex *) *lake)->object_inside (*lagos_point))
							found = *lake;
					}
				if (found) {
					lakes_linked.insert ((*lagos_point)->id);
					log.add_formatted ("%ld", (*lagos_point)->id);
					log += "\t";
					log += (*lagos_point)->name;
					log.add_formatted ("\t%ld", (*lake)->id);
					log += "\t";
					log += (*lake)->name;
					log += "\n";

					lake_copy = layer_output->create_new (found->type);
					lake_copy->copy (found, layer_output);
					layer_output->objects.push_back (lake_copy);
				}
			}
		}
	}

	{
		device_coordinate lagos_pt;
		double distance_m;

		log += "\nLAGOS ID\tName\tLatitude\tLongitude\tNearest ID\tNearest Name\tDistance, m\n";
		for (lagos_point = layer_lagos->objects.begin ();
		lagos_point != layer_lagos->objects.end ();
		++lagos_point)
			if (lakes_linked.find ((*lagos_point)->id) == lakes_linked.end ()) {
				lagos_pt.x = (*lagos_point)->longitude;
				lagos_pt.y = (*lagos_point)->latitude;
				found = layer_lakes->find_nearest_object (lagos_pt, &distance_m);
				log.add_formatted ("%ld", (*lagos_point)->id);
				log += "\t";
				log += (*lagos_point)->name;
				log.add_formatted ("\t%.8lf\t%.8lf", ((double) (*lagos_point)->latitude / 1.0e6), ((double) (*lagos_point)->longitude / 1.0e6));

				log.add_formatted ("\t%ld", found->id);
				log += "\t";
				log += found->name;
				log.add_formatted ("\t%.3lf", distance_m);

				log += "\n";

				lake_copy = layer_output->create_new (found->type);
				lake_copy->copy (found, layer_output);
				layer_output->objects.push_back (lake_copy);
			}
	}
	return !error;
}

void LAGOS_Algal_Blooms::make_output_layer
	(dynamic_map *map,
	class map_layer *layer_lakes)

{
	layer_output = map->create_new (MAP_OBJECT_POLYGON);
	layer_output->name = "LAGOS-NHD Lakes";
	layer_output->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	layer_output->color = RGB (255, 0, 255);
	layer_output->attribute_count_numeric = layer_lakes->attribute_count_numeric;
	layer_output->attribute_count_text = layer_lakes->attribute_count_text;
	layer_output->attributes_void_usage = layer_lakes->attributes_void_usage;
	layer_output->data_attribute_index = layer_lakes->data_attribute_index;
	layer_output->column_names_numeric = layer_lakes->column_names_numeric;
	layer_output->column_names_text = layer_lakes->column_names_text;
	map->layers.push_back (layer_output);
}

bool LAGOS_Algal_Blooms::write_output_layer
	(const dynamic_string &filename_output,
	interface_window *view,
	dynamic_string &log)

{
	importer_shapefile shp_exporter;

	shp_exporter.filename_source = filename_output;

	return shp_exporter.write_from_map_layer (layer_output, NULL, false, view, log);
}

bool LAGOS_Algal_Blooms::read_chla
	(const dynamic_string &filename_lagos_shapefile,
	interface_window *view,
	dynamic_string &log)

// "I:\TDC\LAGOS\Zillow-LAGOS\LAGOS.csv"

{
	FILE *f;
	dynamic_string local_filename = filename_lagos_shapefile, data;
	char buffer[1024];
	LAGOS_chla_reading chla;
	std::vector <dynamic_string> tokens;
	std::map <long, std::vector <LAGOS_chla_reading>>::iterator lake_vector, previous_lookup;

	if (fopen_s (&f, local_filename.get_text_ascii (), "r") == 0) {
		// record_id	lagoslakeid	sampledate	chla	lake_year
		// 1	-1212	6/17/1999	8	-1212_1999
		// 2	-1212	6/30/1999	34	-1212_1999
		// 3	-1212	7/18/1999	125	-1212_1999
		// 4	-1212	8/2/1999	23	-1212_1999
		if (fgets (buffer, 1024, f)) {

			previous_lookup = chla_by_lake.end ();

			while (fgets (buffer, 1025, f)) {
				data = buffer;
				data.tokenize (",", &tokens, true, false);

				if (tokens.size () == 5) {
					chla.lake_id = atol (tokens[1].get_text_ascii ());
					chla.when.parse (tokens[2].get_text_ascii (), TIMESTAMP_M_D_YYYY);
					chla.value = atof (tokens[3].get_text_ascii ());

					if ((previous_lookup != chla_by_lake.end ())
					&& (previous_lookup->first == chla.lake_id))
						previous_lookup->second.push_back (chla);
					else
						if ((lake_vector = chla_by_lake.find (chla.lake_id)) != chla_by_lake.end ()) {
							lake_vector->second.push_back (chla);
							previous_lookup = lake_vector;
						}
						else {
							std::vector <LAGOS_chla_reading> new_vector;
							new_vector.push_back (chla);
							chla_by_lake.insert (std::pair <long, std::vector <LAGOS_chla_reading>> (chla.lake_id, new_vector));
							previous_lookup = chla_by_lake.end ();
						}
				}
				else {
					log += "ERROR, unable to parse chla data \"";
					log += buffer;
					log += "\"\n";
				}
			}
		}

		fclose (f);
		return true;
	}
	else {
		log += "ERROR, can't open chla file \"";
		log += filename_lagos_shapefile;
		log += "\".\n";
		return false;
	}
}

void LAGOS_Algal_Blooms::dump_chla
	(dynamic_string &log)

{
	std::map <long, std::vector <LAGOS_chla_reading>>::iterator lake_vector;
	std::vector <LAGOS_chla_reading>::iterator reading;
	map_object *lagos_lake;
	dynamic_string report;

	for (lake_vector = chla_by_lake.begin ();
	lake_vector != chla_by_lake.end ();
	++lake_vector) {
		report.add_formatted ("%ld", lake_vector->first);

		if ((lagos_lake = layer_lagos->match_id (lake_vector->first)) != NULL) {
			report += "\t";
			report += lagos_lake->name;
		}
		else
			log.add_formatted ("ERROR, no LAGOS layer objects for lake ID %ld.\n", lake_vector->first);

		report += "\n";

		for (reading = lake_vector->second.begin ();
		reading != lake_vector->second.end ();
		++reading) {
			report += "\t";
			report += reading->when.write ();
			report.add_formatted ("\t%.6lf\n", reading->value);
		}
	}

	log += "\n";
	log += report;
}

double LAGOS_Algal_Blooms::average_across_months
	(std::vector <LAGOS_chla_reading> *lake_vector,
	const Timestamp *sale_date,
	const int number_of_months)

// Average of readings across number_of_months

{
	std::vector <LAGOS_chla_reading>::iterator reading;
	Timestamp start_of_months;
	double sum = 0.0;
	int count = 0;

	start_of_months = *sale_date;
	for (int i = 0; i < number_of_months; ++i)
		start_of_months.decrement_month ();

	for (reading = lake_vector->begin ();
	reading != lake_vector->end ();
	++reading) {
		if ((reading->when <= *sale_date)
		&& (reading->when >= start_of_months)) {
			sum += reading->value;
			++count;
		}
	}

	if (count > 0)
		return sum / (double) count;
	else
		return 0.0;
}

void LAGOS_Algal_Blooms::compute_chla_averages
	(class map_layer *zillow_sales,
	class interface_window *view,
	dynamic_string &log)

// Wendong: compute 9-month and 12-month averages for each zillow sale date

{
	int index_lagos_lake, index_distance_meters, index_9, index_12;
	std::vector <map_object *>::iterator sale;
	map_object *lagos_lake;
	std::map <long, std::vector <LAGOS_chla_reading>>::iterator lake_vector;
	device_coordinate sale_location;
	double distance_meters;
	Timestamp *sale_date;

	view->update_progress ("Computing CHLA Averages", 0);

	index_lagos_lake = zillow_sales->attribute_count_numeric;
	index_distance_meters = index_lagos_lake + 1;
	index_9 = index_lagos_lake + 2;
	index_12 = index_lagos_lake + 3;

	zillow_sales->resize (index_12 + 1, zillow_sales->attribute_count_text);

	for (sale = zillow_sales->objects.begin ();
	sale != zillow_sales->objects.end ();
	++sale) {
		sale_date = (Timestamp *) (*sale)->attributes_void;
		(*sale)->centroid (&sale_location.y, &sale_location.x);
		if ((lagos_lake = layer_lagos->find_nearest_object (sale_location, &distance_meters)) != NULL) {

			view->update_progress (lagos_lake->attributes_text [0], 1);

			(*sale)->attributes_numeric[index_lagos_lake] = (double) lagos_lake->id;
			(*sale)->attributes_numeric[index_distance_meters] = distance_meters;

			if (sale_date) {
				if ((lake_vector = chla_by_lake.find (lagos_lake->id)) != chla_by_lake.end ()) {
					(*sale)->attributes_numeric [index_9] = average_across_months (&lake_vector->second, sale_date, 9);
					(*sale)->attributes_numeric [index_12] = average_across_months (&lake_vector->second, sale_date, 12);
				}
				else
					log.add_formatted ("ERROR, no chla data for lake %ld.\n", lagos_lake->id);
			}
		}
	}


}
