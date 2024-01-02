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
#include "../util/interface_window.h"
#include "../util/filename_struct.h"
#include "../util/listbox_files.h"
#include "../util/device_coordinate.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "dialog_route_points.h"
#include "panel_shapefile.h"
// #include "../i_swat_database/SWATDeclarations.h"
// #include "app_nhd_view.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

dialog_route_points::dialog_route_points
	(wxWindow *pParent,
	dynamic_map *p_map,
	map_layer **from,
	map_layer **to,
	bool *output_file,
	dynamic_string *p_filename,
	bool *p_clip_1000m,
	char *p_output_delimiter,
	bool *p_write_station_name)
	: wxDialog (pParent, -1, "Routing Point Layers")
{
	map = p_map;
	layer_from = from;
	layer_to = to;
	clip_1000m = p_clip_1000m;
	write_output_file = output_file;
	filename_output = p_filename;
	output_delimiter = p_output_delimiter;
	write_station_name = p_write_station_name;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer * row = new wxBoxSizer (wxHORIZONTAL);
	combo_start_layer = new wxComboBox (this, COMBO_SELECT_LAYER_START, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("'From' Layer:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	fill_layer_combo (combo_start_layer, *layer_from);
	combo_start_layer->SetSelection (0);
	row->Add (combo_start_layer);
	stack->Add (row);
	stack->AddSpacer (15);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("'To' Layer:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	combo_end_layer = new wxComboBox (this, COMBO_SELECT_LAYER_FINISH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	fill_layer_combo (combo_end_layer, *layer_to);
	combo_end_layer->SetSelection (0);
	row->Add (combo_end_layer);
	stack->Add (row);

	stack->AddSpacer (10);
	row = new wxBoxSizer (wxHORIZONTAL);
	check_output_file = new wxCheckBox (this, wxID_ANY, L"Write directly to output file");
	row->Add (check_output_file);
	stack->Add (row);

	wxString delimiters [2];
	delimiters [0] = "Comma";
	delimiters [1] = "Tab";

	stack->AddSpacer (10);
	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Delimiter:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	combo_delimiter = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, delimiters, wxCB_READONLY);
	row->Add (combo_delimiter);
	stack->Add (row);

	stack->AddSpacer (10);
	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxButton (this, ID_BUTTON_FILENAME, wxT("Set")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	static_filename = new wxStaticText (this, wxID_ANY, filename_output->get_text (), wxDefaultPosition, wxSize (500, 24));
	row->Add (static_filename);
	stack->Add (row);
	stack->AddSpacer (10);

	stack->AddSpacer (10);
	row = new wxBoxSizer (wxHORIZONTAL);
	check_write_station_name = new wxCheckBox (this, wxID_ANY, L"Write station names");
	row->Add (check_write_station_name);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	check_1000m = new wxCheckBox (this, wxID_ANY, "Ignore points > 1000m from rivers", wxDefaultPosition, wxDefaultSize);
	row->Add (check_1000m);
	stack->Add (row);

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));
	SetSizerAndFit(stack);

	check_1000m->SetValue (*clip_1000m);
	check_output_file->SetValue (*write_output_file);
	static_filename->SetLabel (filename_output->get_text ());
	check_write_station_name->SetValue (*write_station_name);
	if (*output_delimiter == ',')
		combo_delimiter->SetSelection (0);
	else
		combo_delimiter->SetSelection (1);
}

BEGIN_EVENT_TABLE(dialog_route_points, wxDialog)
	EVT_BUTTON (ID_BUTTON_FILENAME, dialog_route_points::OnBrowseOutputFile)
END_EVENT_TABLE()

void dialog_route_points::fill_layer_combo
	(wxComboBox *combo,
	const map_layer *selected)

{
	int index;
	std::vector <map_layer*>::const_iterator layer;

	for (layer = map->layers.begin();
	layer != map->layers.end();
	++layer) {
		index = combo->Append ((*layer)->name.get_text_ascii (), (void *) *layer);
		if (*layer == selected)
			combo->SetSelection (index);
	}
}

void dialog_route_points::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		int index;

		if ((index = combo_start_layer->GetSelection ()) != -1)
			*layer_from = map->layers [index];

		if ((index = combo_end_layer->GetSelection ()) != -1)
			*layer_to = map->layers[index];
		*clip_1000m = check_1000m->GetValue ();
		*write_output_file = check_output_file->GetValue ();
		*write_station_name = check_write_station_name->GetValue ();
		if (combo_delimiter->GetSelection () == 0)
			*output_delimiter = ',';
		else
			*output_delimiter = '\t';
	}
	wxDialog::EndModal (retCode);
}


void dialog_route_points::OnBrowseOutputFile
	(wxCommandEvent &)

{
	if (get_read_path (filename_output, L"Output Files (*.tsv)|*.tsv", NULL))
		static_filename->SetLabel (filename_output->get_text ());
}

