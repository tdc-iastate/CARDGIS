#include "../util/utility_afx.h"
#include <vector>
#include <set>
#include <map>
#include <chrono>
#include <wx/stattext.h>
#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/filedlg.h>
#include <wx/clipbrd.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/listbox.h>
#include <wx/app.h>
#include <wx/button.h>
#include "../util/dynamic_string.h"
#include "../util/custom.h"
#include "../util/filename_struct.h"
#include "../util/listbox_files.h"
#include "../util/device_coordinate.h"
#include "../util/dialog_error_list.h"

#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_database_mysql.h"
#include "../odbc/odbc_database_sql_server.h"
#include "../odbc/odbc_field_set.h"

#include "dialog_read_layer_data.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

dialog_read_layer_data::dialog_read_layer_data
	(wxWindow *pParent,
	dynamic_string *filename,
	dynamic_string *p_table_name,
	BYTE *p_import_format,
	dynamic_string *p_filter_id,
	std::vector <dynamic_string> *p_columns)
	: wxDialog (pParent, -1, "Read Layer Data")
{
	dynamic_string filename_VAA, filename_network, filename_usgs, filename_storet_legacy, filename_storet_modern;

	filename_points = filename;
	table_name = p_table_name;
	import_format = p_import_format;
	column_names = p_columns;
	filter_id = p_filter_id;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxSize regular_size, wide_size;
	regular_size.x = 250;
	regular_size.y = 24;
	wide_size.x = 360;
	wide_size.y = 24;

	stack->AddSpacer (10);
	
 	wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxButton (this, ID_BUTTON_FILENAME, wxT("Set")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	static_filename = new wxStaticText (this, wxID_ANY, filename_points->get_text (), wxDefaultPosition, wxSize (500, 24));
	row->Add (static_filename);
	stack->Add (row);
	stack->AddSpacer (15);

	wxString format_names [3] = {"CSV", "Upstream Counties", "ODBC"};

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Format")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	combo_format = new wxComboBox (this, COMBO_FORMAT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, format_names, wxCB_DROPDOWN | wxCB_READONLY);
	row->Add (combo_format);
	stack->Add (row);

	stack->AddSpacer (15);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Filter ID")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	edit_filter_id = new wxTextCtrl (this, wxID_ANY, wxEmptyString, wxDefaultPosition);
	row->Add (edit_filter_id);
	stack->Add (row);

	stack->AddSpacer (15);

	row = new wxBoxSizer (wxHORIZONTAL);
	static_format = new wxStaticText (this, wxID_ANY, filename_points->get_text (), wxDefaultPosition, wxSize (600, 96), wxALIGN_LEFT);
	row->Add (static_format);
	stack->Add (row);
	stack->AddSpacer (5);
	row = new wxBoxSizer (wxHORIZONTAL);
	edit_preview = new wxTextCtrl (this, wxID_ANY, filename_points->get_text (), wxDefaultPosition, wxSize (600, 300), wxTE_MULTILINE | wxTE_READONLY);
	row->Add (edit_preview);
	stack->Add (row);
	stack->AddSpacer (10);

	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	edit_filter_id->SetLabel (filter_id->get_text ());
	combo_format->SetSelection (*import_format - 1);
	preview_format (*import_format);
	SetSizerAndFit(stack);
}

BEGIN_EVENT_TABLE(dialog_read_layer_data, wxDialog)
	EVT_COMBOBOX (COMBO_FORMAT, dialog_read_layer_data::OnFormatSelect)
	EVT_BUTTON (ID_BUTTON_FILENAME, dialog_read_layer_data::OnBrowseFile)
END_EVENT_TABLE()

void dialog_read_layer_data::OnFormatSelect
	(wxCommandEvent &)

{

	int index = combo_format->GetSelection ();
	preview_format (index + 1);
}

void dialog_read_layer_data::preview_format
	(const BYTE format)

{
	dynamic_string five_lines [5], log, text_preview;
	std::vector <dynamic_string>::iterator column;
	int index;

	if (format == 1) {
		static_format->SetLabel ("CSV");
		if (preview_5_lines (five_lines, log)) {

			for (column = column_names->begin ();
			column != column_names->end ();
			++column) {
				if (column != column_names->begin ())
					text_preview += ",";
				text_preview += "\"";
				text_preview += *column;
				text_preview += "\"";
			}
			text_preview += "\r\n";

			for (index = 1; index < 5; ++index)
				if (five_lines[index].get_length () > 0) {
					text_preview += five_lines[index];
					text_preview += "\r\n";
				}
			edit_preview->SetLabel (text_preview.get_text ());
		}
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Point File Preview", L"", log);
		d.ShowWindowModal ();
	}
	else
		if (format == 3)
			preview_odbc ();
		else {
			static_format->SetLabel ("Upstream Counties");
			edit_preview->SetLabel ("");
		}
}

void dialog_read_layer_data::preview_odbc ()

{
	// ERROR, Native=-1907 SQL state "42000".
	// [Microsoft][ODBC Microsoft Access Driver] Record(s) cannot be read; no read permission on 'MSysObjects'.
	/*
	odbc_field_set results;
	dynamic_string sql, log;

	odbc_database_access *db_layer;
	db_layer = new odbc_database_access;
	odbc_database_credentials creds;
	creds.set_database_filename (*filename_points);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (db_layer->open (&creds, NULL, log)) {

		sql = "select MSysObjects.Name, MSysObjects.Type from MSysObjects where ((MSysObjects.Type)=1) ORDER BY MSysObjects.Name;";

		if (db_layer->execute (sql, log)) {

		}

		db_layer->close ();
		delete db_layer;
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"ODBC Preview", L"", log);
	d.ShowWindowModal ();
		*/
}

bool dialog_read_layer_data::preview_5_lines
	(dynamic_string five_lines [],
	dynamic_string &log)

{
	bool error = false;
	FILE *file_csv;
	char data [4096];
	int index = 0;

	if (fopen_s (&file_csv, filename_points->get_text_ascii (), "r") == 0) {

		// facid,latitude,longitude,latitude3,longitude3
		// 100001002,38.60166666666667,75.07138888888889,38.602,75.071
		// 100002001,38.7,75.61666666666666,38.7,75.617

		if (fgets (data, 4096, file_csv)) {
			five_lines [index] = data;
			five_lines [index].trim_right ();
			five_lines [index].tokenize (",", column_names, true, false);
			++index;

			while ((index < 5)
			&& fgets (data, 4096, file_csv)) {
				five_lines [index] = data;
				five_lines [index].trim_right ();
				++index;
			}
		}

		fclose (file_csv);
	}
	else {
		error = true;
		log += "ERROR, can't open point file \"";
		log += *filename_points;
		log += "\"\n";
	}

	return !error;
}

void dialog_read_layer_data::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		*import_format = combo_format->GetSelection () + 1;
		*filter_id = edit_filter_id->GetValue ().ToAscii ();
	}
	wxDialog::EndModal (retCode);
}

void dialog_read_layer_data::OnBrowseFile
	(wxCommandEvent &)

{
	if (*import_format == 1) {
		if (get_read_path (filename_points, L"Data Files (*.csv)|*.csv", NULL))
			static_filename->SetLabel (filename_points->get_text ());
	}
	else {
		if (get_read_path (filename_points, L"Database Files (*.mdb;*.accdb)|*.mdb;*.accdb", NULL))
			static_filename->SetLabel (filename_points->get_text ());
	}
}

