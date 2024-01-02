#pragma once

#include "Resource.h"

class app_CARDGIS: public wxApp {
	public:
		dynamic_string AppName;
		int starting_field_id;
		dynamic_string filename_nhd_root, filename_nhd_hr_root, filename_other_root, filename_census_root, filename_TIGER_root, project_root;
		int current_shapefile_index; // index to RecentlyOpenedShapefiles
		int multithread_count;
		bool use_gl;

		std::vector <odbc_database_credentials> db_credentials;
		bool get_credentials
			(const char *filename,
			const BYTE type,
			class odbc_database_credentials *);
		void add_credentials
			(const class odbc_database_credentials &);
		std::vector <odbc_database_credentials>::iterator match_credentials
			(const dynamic_string &filename,
			const BYTE type);

		void load_configuration ();
		void save_configuration ();

		virtual bool OnInit();
		virtual int OnExit ();
};


DECLARE_APP(app_CARDGIS)