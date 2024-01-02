// wxSide.cpp : Defines the entry point for the application.
//

#include "../util/utility_afx.h"
#include <vector>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/device_coordinate.h"
#include "../util/bounding_cube.h"
#include <map>
#include <set>
#include <wx/app.h>
#include "wx/frame.h"
#include "wx/wizard.h"
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/init.h>
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_field_set.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/map_scale.h"
#include "app_CARDGIS.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "frame_CARDGIS.h"

const int MAX_DB_CREDENTIALS = 40;

IMPLEMENT_APP(app_CARDGIS)

bool app_CARDGIS::OnInit()
{
	starting_field_id = 0;
	multithread_count = 4;
	use_gl = false;

	filename_nhd_root = "R:\\NHDPlusv2\\";
	filename_nhd_hr_root = "R:\\NHDPlus_HR\\";
	filename_other_root = "r:\\DKeiser\\Other\\";
	filename_census_root = "R:\\DKeiser\\census\\";
	filename_TIGER_root = "R:\\Census\\TIGER 2015\\";
	project_root = "f:\\scratch\\"; // For whatever current project

	AppName = "CARDGIS";
	if ( !wxApp::OnInit() )
        return false;

	current_shapefile_index = -1;

	load_configuration ();

#ifndef _DEBUG
	// 2014-06-11 Override TIFFReadDirectory's warnings about GEOTIFF tags
	wxLog wlog;
	wlog.SetLogLevel (wxLOG_Error);
#endif

	frame_CARDGIS *frame = new frame_CARDGIS ( AppName.get_text (), wxPoint(50, 50), wxSize(wxSystemSettings::GetMetric (wxSYS_SCREEN_X), wxSystemSettings::GetMetric (wxSYS_SCREEN_Y)), this );
    frame->Show(true);
	frame->Maximize ();

	// frame->position = starting_field_id;

    // SetTopWindow(frame);

    /*
	wizard_control_record wizard (frame);
    wizard.RunWizard(wizard.GetFirstPage());
	*/

    return true;
}

int app_CARDGIS::OnExit ()

{
	save_configuration ();

#ifdef _WEBVIEWCHROMIUM
	wxWebViewChromium::Shutdown ();
#endif

	return wxApp::OnExit ();
}

void app_CARDGIS::load_configuration ()

{
	wxString str;
	wxConfig *config = new wxConfig (AppName.get_text_wide ());

	config->Read ("FieldID", &starting_field_id);

	config->Read ("ShapefileIndex", &current_shapefile_index);

	config->Read ("ThreadCount", &multithread_count);

	config->Read ("Use_GL", &use_gl);

    if (config->Read("NHD_root_filename", &str) ) {
        filename_nhd_root = str.ToAscii ();
		str.clear ();
	}
	if (config->Read("NHD_HR_root_filename", &str)) {
		filename_nhd_hr_root = str.ToAscii();
		str.clear();
	}
	if (config->Read("Other_root_filename", &str) ) {
        filename_other_root = str.ToAscii ();
		str.clear ();
	}

    if (config->Read("Census_root_filename", &str) ) {
        filename_census_root = str.ToAscii ();
		str.clear ();
	}

    if (config->Read("TIGER_root_filename", &str) ) {
        filename_TIGER_root = str.ToAscii ();
		str.clear ();
	}

	if (config->Read("Project_Root", &str) ) {
        project_root = str.ToAscii ();
		str.clear ();
	}


	/*
	int registry_thread_count, thread_index, version_index;
	dynamic_string entry;

    if (config->Read("filename_swat_output_variables", &str) ) {
        filename_swat_output_variables = str.ToAscii ();
		str.clear ();
	}

	if (!config->Read ("Threads", &registry_thread_count))
		registry_thread_count = 1;

	for (thread_index = 0; thread_index < registry_thread_count; ++thread_index) {
		for (version_index = 0; version_index < NUMBER_OF_SWAT_VERSIONS; ++version_index) {
			entry = SWAT_VERSION_NAME [version_index];
			entry.add_formatted ("-%d", thread_index);
			if (config->Read(entry.get_text (), &str)) {

				if (number_of_threads > thread_index) {
					// change existing path
					swat_locations [version_index] [thread_index] = str.ToAscii ();
				}
				else
					// add new path
					entry = str.ToAscii ();
					swat_locations [version_index].push_back (entry);
			}
		}
	}
	number_of_threads = registry_thread_count;

	if (config->Read ("Version", &config_int))
		SWATVersion = (short) config_int;
	if (config->Read ("Revision", &config_int))
		SWATRevision = config_int;
	*/

	delete config;
}

void app_CARDGIS::save_configuration ()

{
	wxString str_entry;
	wxConfig *config = new wxConfig (AppName.get_text_wide ());
	config->Write ("FieldID", starting_field_id);

	config->Write ("ShapefileIndex", current_shapefile_index);

	config->Write ("ThreadCount", multithread_count);
	config->Write ("Use_GL", use_gl);

	str_entry = filename_nhd_root.get_text_ascii();
	config->Write("NHD_root_filename", str_entry);

	str_entry = filename_nhd_hr_root.get_text_ascii();
	config->Write("NHD_HR_root_filename", str_entry);

	str_entry = filename_other_root.get_text_ascii ();
    config->Write ("Other_root_filename", str_entry);

    str_entry = filename_census_root.get_text_ascii ();
    config->Write ("Census_root_filename", str_entry);

	str_entry = filename_TIGER_root.get_text_ascii ();
    config->Write ("TIGER_root_filename", str_entry);

	str_entry = project_root.get_text_ascii ();
	config->Write ("Project_Root", str_entry);

	delete config;
}

bool app_CARDGIS::get_credentials
	(const char *filename,
	const BYTE type,
	class odbc_database_credentials *creds)

{
	std::vector <odbc_database_credentials>::iterator it;

	it = match_credentials (filename, type);
	if (it != db_credentials.end ()) {
		creds->copy (*it);
		return true;
	}
	else
		return false;
}

std::vector <odbc_database_credentials>::iterator app_CARDGIS::match_credentials
	(const dynamic_string &filename,
	const BYTE type)

{
	std::vector <odbc_database_credentials>::iterator it, found;
	found = db_credentials.end ();

	for (it = db_credentials.begin ();
	(found == db_credentials.end ())
	&& (it != db_credentials.end ());
	++it) {
		if ((it->database_name == filename)
		&& (it->type == type))
			found = it;
	}

	return found;
}

void app_CARDGIS::add_credentials
	(const class odbc_database_credentials &creds)

{
	std::vector <odbc_database_credentials>::iterator it;

	it = match_credentials (creds.database_name, creds.type);

	if (it != db_credentials.end ()) {
		// update existing creds
		it->copy (creds);
	}
	else {
		if (db_credentials.size () > MAX_DB_CREDENTIALS)
			// remove oldest one
			db_credentials.erase (db_credentials.begin ());
		db_credentials.push_back (creds);
	}
}
