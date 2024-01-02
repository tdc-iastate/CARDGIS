
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
#include "../util/matrix.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"
#include "../Collada/camera_time.h"
#include "../Collada/collada_animation.h"
#include "../Collada/collada_object.h"
#include "../Collada/collada_effect.h"
#include "../Collada/collada_material.h"
#include "../Collada/collada_camera.h"
#include "../Collada/collada_light.h"
#include "../Collada/xml_command_collada.h"
#include "../Collada/xml_configuration_collada.h"
#include "../Collada/collada_scene.h"

#include "timed_measurements.h"

#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_field_set.h"

#include "../SamplingStations/WaterSample.h"

timed_measurements::timed_measurements()

{
	id = 0;
}

timed_measurements::timed_measurements
	(const timed_measurements &other)
{
	copy(other);
}

timed_measurements timed_measurements::operator =
	(const timed_measurements &other)
{
	if (this != &other)
		copy(other);

	return (*this);
}

void timed_measurements::copy
	(const timed_measurements &other)
{
	id = other.id;
	amounts_ppm = other.amounts_ppm;
}

void timed_measurements::clear ()
{
	id = 0;
	amounts_ppm.clear ();
}

void timed_measurements::describe
	(dynamic_string &log)

{
	std::map <std::chrono::system_clock::time_point, double>::const_iterator amount;
    Timestamp when;

	log.add_formatted ("Measurement ID\t%ld\n", id);

	for (amount = amounts_ppm.begin ();
	amount != amounts_ppm.end ();
	++amount) {
		log += "\t";
		when.set_from_time_point (amount->first);
		log += when.write (TIMESTAMP_YYYY_MM_DD);
		log.add_formatted ("\t%.3lf\n", amount->second);
	}
}

std::chrono::system_clock::time_point timed_measurements::latest_time () const

{
	std::map <std::chrono::system_clock::time_point, double>::const_iterator amount;
	std::chrono::system_clock::time_point last_time;

	amount = amounts_ppm.begin ();
	last_time = amount->first;
	while (amount != amounts_ppm.end ()) {
		if (amount->first > last_time)
			last_time = amount->first;
		++amount;
	}

	return last_time;
}

void timed_measurements::statistics
	(double *lowest,
	double *highest) const

{
	std::map <std::chrono::system_clock::time_point, double>::const_iterator amount;
	// std::chrono::seconds interval;

	amount = amounts_ppm.begin ();
	*lowest = *highest = amount->second;
	while (amount != amounts_ppm.end ()) {
		if (amount->second > *highest)
			*highest = amount->second;
		if (amount->second < *lowest)
			*lowest = amount->second;
		++amount;
	}

}

stream_measurements::stream_measurements()

{
	comid = 0;
}

stream_measurements::stream_measurements
	(const stream_measurements &other)
{
	copy(other);
}

stream_measurements stream_measurements::operator =
	(const stream_measurements &other)
{
	if (this != &other)
		copy(other);

	return (*this);
}

void stream_measurements::copy
	(const stream_measurements &other)
{
	comid = other.comid;
	node_readings = other.node_readings;
}

void stream_measurements::clear ()
{
	comid = 0;
	node_readings.clear ();
}

bool read_sampling_data
	(const char *filename_db_readings,
	class map_layer *layer_stations,
	interface_window *view,
	dynamic_string &log)

// Read dissolved oxygen database created by samplingstations.exe

// This version reads into map_objects

{
	bool error = false;
	odbc_database_access *db_odbc;
	odbc_database_credentials creds;
	WaterSampleVector samples;
	WaterStationID rowset_ids;
	WaterSample rowset_water;
	dynamic_string filter;
	timed_measurements *stream_readings;
	std::map <long, long> id_by_station_id;
	std::map <long, long>::iterator comid;
	long record_count, count_unmatched = 0;
	std::chrono::system_clock::time_point when;
	map_object *sampling_station;

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
		// Build a map (by station name) of maps (by agency name) of ids
		std::map <dynamic_string, WaterStationAgencies> ids_by_station_name;
		WaterStationAgencies agencies;
		std::vector <map_object *>::iterator map_point;
		std::map <dynamic_string, WaterStationAgencies>::iterator id_table_entry;
		std::map <dynamic_string, long>::iterator agency_entry;

		if (rowset_ids.field_set.open_read (db_odbc, log)) {
			if (rowset_ids.field_set.move_first (log)) {
				do {
					if ((id_table_entry = ids_by_station_name.find (rowset_ids.Station)) != ids_by_station_name.end ())
						id_table_entry->second.ids_by_agency.insert (std::pair <dynamic_string, long> (rowset_ids.Agency, rowset_ids.ID));
					else {
						agencies.clear ();
						agencies.ids_by_agency.insert (std::pair <dynamic_string, long> (rowset_ids.Agency, rowset_ids.ID));
						ids_by_station_name.insert (std::pair <dynamic_string, WaterStationAgencies> (rowset_ids.Station, agencies));
					}
					if ((++record_count % 1000) == 0) 
						view->update_progress_formatted  (1, "Reading Station ID Table %ld", record_count);
				} while (rowset_ids.field_set.move_next (log));
			}
			rowset_ids.field_set.close ();

			// Convert map of maps to a single map: map_object ID by WaterStationID
			// by matching station name & agency name from map_object
			view->update_progress ("Building Station ID map", 0);
			record_count = 0;
			for (map_point = layer_stations->objects.begin ();
			map_point != layer_stations->objects.end ();
			++map_point) {
				if ((id_table_entry = ids_by_station_name.find ((*map_point)->name)) != ids_by_station_name.end ()) {
					if ((agency_entry = id_table_entry->second.ids_by_agency.find ((*map_point)->attributes_text [0])) != id_table_entry->second.ids_by_agency.end ()) {
						id_by_station_id.insert (std::pair <long, long> (agency_entry->second, (*map_point)->id));
					}
					else
						++count_unmatched;
				}
				else
					++count_unmatched;
				if ((++record_count % 1000) == 0) 
					view->update_progress_formatted  (1, "%ld, %ld unmatched", record_count, count_unmatched);
			}
		}
		else
			error = true;

		ids_by_station_name.clear ();

		view->update_progress ("WaterSample IDs read");
		log.add_formatted ("Plotting stations not found in WaterSamples ID table\t%ld\n", count_unmatched);

		record_count = 0;

		if (rowset_water.field_set.open_read (db_odbc, log)) {
			if (rowset_water.field_set.move_first (log)) {
				do {
					rowset_water.Time.set_time_point (&when);

					if ((comid = id_by_station_id.find (rowset_water.ID)) != id_by_station_id.end ()) {

						if ((sampling_station = layer_stations->match_id (comid->second)) != NULL) {

							if ((stream_readings = (timed_measurements *) sampling_station->attributes_void) != NULL)
								stream_readings->amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
							else {
								stream_readings = new timed_measurements;
								sampling_station->attributes_void = stream_readings;
								stream_readings->id = rowset_water.ID;
								stream_readings->amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
							}
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

			log.add_formatted ("DB open.\n");
	}
	else {
		delete db_odbc;
		db_odbc = NULL;
		error = true;
	}

	return !error;
}

