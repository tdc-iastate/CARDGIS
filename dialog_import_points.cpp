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
#include <wx/grid.h>
#include "../util/dynamic_string.h"
#include "../util/custom.h"
#include "../util/filename_struct.h"
#include "../util/listbox_files.h"
#include "../util/device_coordinate.h"
#include "../util/dialog_error_list.h"
#include "../util/Ledger.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "dialog_import_points.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

dialog_import_points::dialog_import_points
	(wxWindow *pParent,
	dynamic_string *filename,
	const dynamic_string& match_id_name,
	dynamic_map *p_map,
	BYTE *p_import_format,
	bool *p_normalize_longitude,
	bool *p_numeric_id,
	Ledger *p_preview,
	std::set <int> *p_text_columns,
	std::set <int> *p_skip_columns,
	int *p_id_column_index,
	int *p_lat_column_index,
	int *p_lon_column_index)
	: wxDialog (pParent, -1, "Read Points")
{
	dynamic_string filename_VAA, filename_network, filename_usgs, filename_storet_legacy, filename_storet_modern;

	filename_points = filename;
	id_column_name = match_id_name;
	map = p_map;
	import_format = p_import_format;
	normalize_longitude = p_normalize_longitude;
	numeric_id = p_numeric_id;
	preview = p_preview;
	text_columns = p_text_columns;
	skip_columns = p_skip_columns;
	id_column_index = p_id_column_index;
	lat_column_index = p_lat_column_index;
	lon_column_index = p_lon_column_index;

	fixed_font = new wxFont(11, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxSize regular_size, wide_size;
	regular_size.x = 250;
	regular_size.y = 24;
	wide_size.x = 360;
	wide_size.y = 24;

	stack->AddSpacer (10);
	
 	wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxButton (this, ID_BUTTON_FILENAME_POINTS, wxT("Set")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	static_filename = new wxStaticText (this, wxID_ANY, filename_points->get_text (), wxDefaultPosition, wxSize (600, 24));
	row->Add (static_filename);
	stack->Add (row);
	stack->AddSpacer (10);

	wxString format_names [11] = {"Facilities", "By COMID", "USGS/Storet Modern", "STORET Legacy Old", "STORET Legacy New", "STORET Legacy 2018", "Shapefile", "Specified", "ID-Lat-Long", "USGS 2018", "Automatic"};

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Format")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	combo_format = new wxComboBox (this, COMBO_FORMAT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 11, format_names, wxCB_DROPDOWN | wxCB_READONLY);
	row->Add (combo_format);
	stack->Add (row);
	stack->AddSpacer (10);

	row = new wxBoxSizer (wxHORIZONTAL);
	static_format = new wxStaticText (this, wxID_ANY, filename_points->get_text (), wxDefaultPosition, wxSize (600, 96), wxALIGN_LEFT);
	row->Add (static_format);
	stack->Add (row);
	stack->AddSpacer (5);

	row = new wxBoxSizer (wxHORIZONTAL);
	edit_file_view = new wxTextCtrl (this, wxID_ANY, filename_points->get_text (), wxDefaultPosition, wxSize (800, 300), wxTE_MULTILINE | wxTE_READONLY);
	edit_file_view->SetFont(*fixed_font);
	row->Add (edit_file_view);
	stack->Add (row);
	stack->AddSpacer (10);

	wxStaticBoxSizer *grid_outline = new wxStaticBoxSizer (wxVERTICAL, this, "Preview");
	grid_preview = new wxGrid (this, GRID_PREVIEW, wxDefaultPosition, wxSize (800, 240));
	grid_preview->CreateGrid (0, 0);
	grid_outline->Add (grid_preview, wxALL, 5);
	// button_copy_grid = new wxButton (this, BUTTON_COPY_GRID, wxT("Copy"));
	// grid_outline->Add (button_copy_grid, 0, wxALL, 5);
	stack->Add (grid_outline, 0, wxALL, 5);
	stack->AddSpacer (10);


	row = new wxBoxSizer (wxHORIZONTAL);
	check_normalize_longitude = new wxCheckBox (this, wxID_ANY, L"Normalize longitude");
	row->Add (check_normalize_longitude);
	row->AddSpacer (25);
	check_numeric_id = new wxCheckBox (this, wxID_ANY, L"Numeric ID");
	row->Add (check_numeric_id);
	stack->Add (row);
	stack->AddSpacer (10);


	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	check_normalize_longitude->SetValue (*normalize_longitude);
	check_numeric_id->SetValue (*numeric_id);
	combo_format->SetSelection (*import_format - 1);
	setup_format (*import_format);
	show_file();
	show_ledger ();
	SetSizerAndFit(stack);
}

dialog_import_points::~dialog_import_points()

{
	if (fixed_font)
		delete fixed_font;

}

BEGIN_EVENT_TABLE(dialog_import_points, wxDialog)
	EVT_COMBOBOX (COMBO_FORMAT, dialog_import_points::OnFormatSelect)
	EVT_BUTTON (ID_BUTTON_FILENAME_POINTS, dialog_import_points::OnBrowsePointFile)
END_EVENT_TABLE()

void dialog_import_points::OnFormatSelect
	(wxCommandEvent &)

{

	int index = combo_format->GetSelection ();
	setup_format (index + 1);
	show_ledger ();
}

/*
void dialog_import_points::match_column_names ()

{
	std::vector <dynamic_string>::iterator column;
	int column_index;

	for (column = preview->column_names.begin (), column_index = 0;
	column != preview->column_names.end ();
	++column, ++column_index) {
		if (column->equals_insensitive ("ID")
		|| (column->match_insensitive ("Codido") == 0)) // Spanish "Coded" from I:\TDC\BHO\Received 2021-07-26\Water_Reservoirs.csv
			*id_column_index = column_index;

		if (column->equals_insensitive ("Latitude")
		|| column->equals_insensitive ("Lat"))
			*lat_column_index = column_index;

		if (column->equals_insensitive ("Longitude")
		|| column->equals_insensitive ("Lon"))
			*lon_column_index = column_index;
	}
}
*/

void dialog_import_points::show_file()
{
	std::vector <dynamic_string>::const_iterator raw_line;
	dynamic_string text_preview;

	for (raw_line = raw_text.begin ();
	raw_line != raw_text.end ();
	++raw_line)
		text_preview += *raw_line;

	text_preview.convert_linefeeds_for_CEdit();
	edit_file_view->SetLabel(text_preview.get_text());
}

void dialog_import_points::setup_format
	(const BYTE format)

{
	dynamic_string log, text_preview;
	std::vector <dynamic_string>::iterator column;
	std::vector <std::vector <dynamic_string>>::iterator row;
	// int index, column_index;

	if (preview_position_file (*filename_points, format, "", 10, preview, id_column_index, lat_column_index, lon_column_index, text_columns, &raw_text, log)) {
		switch (format) {
			case CSV_POINT_FORMAT_FACILITIES:
				static_format->SetLabel ("facid,latitude,longitude,latitude3,longitude3\n100001002,38.60166666666667,75.07138888888889,38.602,75.071\n100002001,38.7,75.61666666666666,38.7,75.617\n100003001,38.5,75.5,38.5,75.5");
				break;
			case CSV_POINT_FORMAT_COMID:
				static_format->SetLabel ("comid\n20228401\n948090648\n18224411");
				break;
			case CSV_POINT_FORMAT_USGS:
				static_format->SetLabel ("station,stationname,longitude,latitude,huc,rchname,fips,primarytypename,stationid,nasqan1,...\nAR001-331052093543302,18S27W21BB1,-93.909348,33.18124,11140304,,5091,Stream,,,,,,,,,\nAR001-331500092390001,17s15w16AA1,-92.650154,33.25013,8040201,EL DORADO WATER UTILITY G,5139,Stream,,,,,,,,,\nAR001-331701093152601,6169 MAGNOLIA WW,-93.257393,33.283737,11140203,6169 MAGNOLIA WW,5027,Stream,,,,,,,,,");
				break;
			case CSV_POINT_FORMAT_STORET_LEGACY_OLD:
				static_format->SetLabel ("station,agency,state,county,fips,stationname,agencyname,description,latitude,longitude,huc,...\n0-0-SPK,11113300,ma,essex,25009,spicket river / nort,USEPA REGION 1,bridge on ,42.734722,-71.20138900000001,1070002,...\n0-3-OSS,11113300,nh,carroll,33003,ossipee r. rt. 153 b,USEPA REGION 1,none,43.797222,-71.056944,1060002,...");
				break;
			case CSV_POINT_FORMAT_STORET_LEGACY_NEW:
				static_format->SetLabel ("station,agency,statename_stn,countyname_stn,agencyname_stn,description_stn,latitude_stn,longitude_stn,huc_stn,rchmilesegment_stn,milesupreach_stn,rchonoff_stn,rchname_stn,stationdepth_stn,type,type2legacy,depthunits_stn,dam,res");
				break;
			case CSV_POINT_FORMAT_SPECIFIED:
				static_format->SetLabel ("<specified>");
				break;
			case CSV_POINT_FORMAT_ID_LAT_LONG:
				static_format->SetLabel ("<ID,Latitude,Longitude>");
				break;
			case CSV_POINT_FORMAT_AUTOMATIC:
				text_preview.format("<automatic: %d numeric, %d text>", preview->column_names.size () - text_columns->size (), text_columns->size());
				static_format->SetLabel (text_preview.get_text ());
		}

		/*
		for (column = preview->column_names.begin(), index = 0;
		column != preview->column_names.end ();
		++column, ++index) {
			if (column != preview->column_names.begin ())
				text_preview += ",";
			if (index == *id_column_index)
				text_preview += "**ID**";
			else
				if (index == *lat_column_index)
					text_preview += "**LATITUDE**";
				else
					if (index == *lon_column_index)
						text_preview += "**LONGITUDE**";
					else {
						text_preview += "\"";
						text_preview += *column;
						text_preview += "\"";
					}
			if (text_columns->find (index) != text_columns->end ())
				text_preview += "(text)";
			else
				text_preview += "(num)";
		}
		text_preview += "\n";

		for (row = preview->rows.begin (), index = 0;
		row != preview->rows.end ();
		++row, ++index) {
			for (column = (*row).begin (), column_index = 0;
			column != (*row).end ();
			++column, ++column_index) {
				text_preview += " - ";
				text_preview += *column;
			}
			text_preview += "\n";
		}
		text_preview.convert_linefeeds_for_CEdit();
		edit_file_view->SetLabel (text_preview.get_text ());
		*/
	}
	else {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Point File Preview", L"", log);
		d.ShowWindowModal ();
	}
}


void dialog_import_points::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		*import_format = combo_format->GetSelection () + 1;
		*normalize_longitude = check_normalize_longitude->GetValue ();
		*numeric_id = check_numeric_id->GetValue ();
		// *column_names = preview.column_names;
	}
	wxDialog::EndModal (retCode);
}

void dialog_import_points::OnBrowsePointFile
	(wxCommandEvent &)

{
	if (get_read_path (filename_points, L"Point Files (*.csv)|*.csv", NULL))
		static_filename->SetLabel (filename_points->get_text ());
}

void dialog_import_points::set_columns (int *column_count)

{
	std::vector <std::vector <dynamic_string>>::const_iterator row;
	std::vector <dynamic_string>::const_iterator title;
	int column_index;

	if ((row = preview->rows.begin ()) != preview->rows.end ()) {
		// grid_preview_output->SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		if (preview->column_names.size () > 0)
			*column_count = preview->column_names.size ();
		else
			*column_count = (*row).size ();
		grid_preview->AppendCols (*column_count);

		if (preview->column_names.size () > 0) {
			title = preview->column_names.begin ();
			column_index = 0;
			while (title != preview->column_names.end ()) {
				grid_preview->SetColLabelValue (column_index, title->get_text ()); // , text_color, background_color);
				++title;
				++column_index;
			}
		}
	}
}

void dialog_import_points::show_ledger ()

{
	std::vector <std::vector <dynamic_string>>::iterator row;
	std::vector <dynamic_string>::iterator column, row_label;
	int grid_row_index, column_index, column_count;
	// const int records_per_display = 10;
	dynamic_string row_header;
	wxSize row_label_width, widest_row_label;

	if (grid_preview->GetNumberRows () > 0)
		grid_preview->DeleteRows (0, grid_preview->GetNumberRows ());
	if (grid_preview->GetNumberCols () > 0)
		grid_preview->DeleteCols (0, grid_preview->GetNumberCols ());

	set_columns (&column_count);

	grid_preview->AppendRows (preview->rows.size ());

	for (row = preview->rows.begin (), grid_row_index = 0, row_label = preview->row_names.begin ();
	// (grid_row_index < records_per_display)
	row != preview->rows.end ();
	++row, ++grid_row_index, ++row_label) {

		//  if (skip_lines.find (grid_row_index) != skip_lines.end ())
		//	skip_row = true;
		// else
		// 	skip_row = false;

		if (row_label->get_length () > 0)
			grid_preview->SetRowLabelValue (grid_row_index, row_label->get_text ()); // , text_color, background_color);

		for (column = (*row).begin (), column_index = 0;
		column != (*row).end ();
		++column, ++column_index) {				
			grid_preview->SetCellValue (grid_row_index, column_index, column->get_text ());

			if (column_index == *id_column_index)
				grid_preview->SetCellTextColour (grid_row_index, column_index, *wxRED);
			else
				if (column_index == *lat_column_index)
					grid_preview->SetCellTextColour (grid_row_index, column_index, *wxGREEN);
				else
					if (column_index == *lon_column_index)
						grid_preview->SetCellTextColour (grid_row_index, column_index, *wxBLUE);
					else
						if (text_columns->find (column_index) != text_columns->end ())
							grid_preview->SetCellTextColour (grid_row_index, column_index, *wxCYAN);
		}
		// if (*column_of_interest >= 0)
		//	grid_preview->SetCellTextColour (grid_row_index, *column_of_interest, *wxRED);
	}
	grid_preview->SetRowLabelSize (wxGRID_AUTOSIZE);
	// grid_preview->AutoSize ();
}

