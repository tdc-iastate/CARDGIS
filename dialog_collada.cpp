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
#include "../util/filename_struct.h"
#include "../util/custom.h"
#include "../util/edit_time_point.h"
#include "../util/Timestamp.h"
#include "../util/device_coordinate.h"
#include "dialog_collada.h"
// #include "../map/map_scale.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"

#include "../Collada/camera_time.h"
#include "../Collada/collada_animation.h"
#include "../Collada/collada_object.h"
#include "../Collada/collada_effect.h"
#include "../Collada/collada_material.h"
#include "../Collada/collada_camera.h"
#include "../Collada/collada_light.h"
#include "../Collada/collada_scene.h"
#include "timed_measurements.h"
#include "river_view_map.h"
#include "river_view_collada.h"

BEGIN_EVENT_TABLE(dialog_collada, wxDialog)
	EVT_BUTTON (ID_EDIT_PATH, dialog_collada::OnBrowseOutputFile)
	EVT_BUTTON (BUTTON_ROYAL_GORGE, dialog_collada::OnSetRoyalGorge)
	EVT_BUTTON (BUTTON_AMES, dialog_collada::OnSetAmes)
	EVT_BUTTON (BUTTON_RACCOON, dialog_collada::OnSetRaccoon)
	EVT_BUTTON (BUTTON_E85, dialog_collada::OnSetE85)
	EVT_BUTTON (BUTTON_SELECTION_BOUNDS, dialog_collada::OnSetSelectionBounds)
END_EVENT_TABLE()

dialog_collada::dialog_collada
	(wxWindow *pParent,
	dynamic_string *p_filename,
	dynamic_map *p_map,
	bool *p_gridfloat_3dep,
	bool *p_boundary_box,
	bool *p_latlon_grid,
	bool *p_light,
	class river_view_collada *p_river_viewer)
	: wxDialog (pParent, -1, "Collada Export")
{
	filename_dae = p_filename;
	map = p_map;
	gridfloat_3dep = p_gridfloat_3dep;
	boundary_box = p_boundary_box;
	latlon_grid = p_latlon_grid;
	light = p_light;
	river_view = p_river_viewer;
	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

 	wxBoxSizer *row;
	std::vector <map_layer*>::const_iterator layer;
	int layer_index, layer_count = map->layers.size ();
	dynamic_string layer_description;

	row = new wxBoxSizer (wxHORIZONTAL);
	button_browse_output_file = new wxButton (this, ID_EDIT_PATH, wxT("Set Output Filename"));
	row->Add (button_browse_output_file, 0, wxALL, 5);
	row->AddSpacer (10);
	static_output_file = new wxStaticText (this, STATIC_FILENAME, wxT(""), wxDefaultPosition, wxSize (500, 24));
	row->Add (static_output_file, 0, wxALL, 5);
	stack->Add (row);

	stack->AddSpacer (10);

	wxGridSizer *grid = new wxGridSizer (3, wxSize (4, 4));
	grid->Add (new wxStaticText (this, wxID_ANY, L"Name", wxDefaultPosition, wxDefaultSize));
	grid->Add (new wxStaticText (this, wxID_ANY, L"Render", wxDefaultPosition, wxDefaultSize));
	grid->Add (new wxStaticText (this, wxID_ANY, L"Symbol size (m)", wxDefaultPosition, wxDefaultSize));

	for (layer = map->layers.begin(), layer_index = 0;
	layer != map->layers.end();
	++layer, ++layer_index) {
		layer_description = (*layer)->name;
		grid->Add (new wxStaticText (this, wxID_ANY, layer_description.get_text (), wxDefaultPosition, wxDefaultSize));

		checkbox_layers = new wxCheckBox (this, wxID_ANY, L"", wxDefaultPosition, wxDefaultSize);
		checkbox_layer.push_back (checkbox_layers);
		grid->Add (checkbox_layers);

		edit_symbol_size = new double_edit (this, wxID_ANY, L"0.0", wxDefaultPosition, wxDefaultSize);
		edit_layer.push_back (edit_symbol_size);
		grid->Add (edit_symbol_size);

		// static_text = new wxStaticText (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize (350, 25));
		// static_layer.push_back (static_text);
		// row->Add (static_text);
	}
	stack->Add (grid);

	wxString area_types [NUMBER_OF_RIVER_VIEW_AREA_TYPES];
	area_types [0] = "Box Area";
	area_types [1] = "Source and Outlet";
	area_types [2] = "Source and Upstream";
	area_types [3] = "All in Area";

	row = new wxBoxSizer (wxHORIZONTAL);
	combo_area_definition = new wxComboBox (this, wxID_ANY, L"", wxDefaultPosition, wxDefaultSize, NUMBER_OF_RIVER_VIEW_AREA_TYPES, area_types, wxCB_READONLY);
	row->Add (combo_area_definition, 0, wxALL, 5);
	stack->Add (row);

	wxString camera_types [NUMBER_OF_COLLADA_CAMERA_TYPES];
	camera_types [0] = "None";
	camera_types [1] = "Fixed";
	camera_types [2] = "Orbit";
	camera_types [3] = "Space Approach";
	camera_types [4] = "Follow Stream";

	row = new wxBoxSizer (wxHORIZONTAL);
	combo_camera_type = new wxComboBox (this, wxID_ANY, L"", wxDefaultPosition, wxDefaultSize, NUMBER_OF_COLLADA_CAMERA_TYPES, camera_types, wxCB_READONLY);
	row->Add (combo_camera_type, 0, wxALL, 5);
	stack->Add (row);

	{
		wxStaticBoxSizer *outline_box = new wxStaticBoxSizer (wxVERTICAL, this, wxT("Source & Outlet:"));
 		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, -1, L"Source COMID:"), 0, wxALL, 5);
		row->AddSpacer (15);
		edit_facility = new long_integer_edit (this, -1, L"");
		row->Add (edit_facility, 0, wxALL, 5);
		outline_box->Add (row);

 		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, -1, L"Outlet COMID:"), 0, wxALL, 5);
		row->AddSpacer (15);
		edit_usgs_station = new long_integer_edit (this, -1, L"");
		row->Add (edit_usgs_station, 0, wxALL, 5);
		outline_box->Add (row);
		stack->Add (outline_box);
	}

	{
		wxStaticBoxSizer *outline_box = new wxStaticBoxSizer (wxVERTICAL, this, wxT("Map Boundaries:"));
 		row = new wxBoxSizer (wxHORIZONTAL);
		row->AddSpacer (100);
		row->Add (new wxStaticText (this, -1, L"North:"), 0, wxALL, 5);
		edit_boundary_north = new double_edit (this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
		row->Add (edit_boundary_north, 0, wxALL, 5);
		outline_box->Add (row);

 		row = new wxBoxSizer (wxHORIZONTAL);
		row->AddSpacer (25);
		row->Add (new wxStaticText (this, -1, L"West:"), 0, wxALL, 5);
		edit_boundary_west = new double_edit (this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
		row->Add (edit_boundary_west, 0, wxALL, 5);
		row->Add (new wxStaticText (this, -1, L"East:"), 0, wxALL, 5);
		edit_boundary_east = new double_edit (this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
		row->Add (edit_boundary_east, 0, wxALL, 5);
		outline_box->Add (row);

 		row = new wxBoxSizer (wxHORIZONTAL);
		row->AddSpacer (100);
		row->Add (new wxStaticText (this, -1, L"South:"), 0, wxALL, 5);
		edit_boundary_south = new double_edit (this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
		row->Add (edit_boundary_south, 0, wxALL, 5);
		outline_box->Add (row);

		stack->Add (outline_box);
	}

 	row = new wxBoxSizer (wxHORIZONTAL);
	check_gridfloat = new wxCheckBox (this, wxID_ANY, "3DEP gridfloat", wxDefaultPosition, wxDefaultSize);
	row->Add (check_gridfloat, 0, wxALL, 5);
	stack->Add (row);

 	row = new wxBoxSizer (wxHORIZONTAL);
	check_boundary = new wxCheckBox (this, wxID_ANY, "Bounding box", wxDefaultPosition, wxDefaultSize);
	row->Add (check_boundary, 0, wxALL, 5);
	stack->Add (row);

 	row = new wxBoxSizer (wxHORIZONTAL);
	check_latlon_grid = new wxCheckBox (this, wxID_ANY, "Lat/Lon Grid", wxDefaultPosition, wxDefaultSize);
	row->Add (check_latlon_grid, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	check_light = new wxCheckBox (this, wxID_ANY, "Set Light", wxDefaultPosition, wxDefaultSize);
	row->Add (check_light, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, -1, L"Amplification, station reading:"), 0, wxALL, 5);
	edit_station_amplification = new double_edit (this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
	row->Add (edit_station_amplification, 0, wxALL, 5);
	row->AddSpacer (20);
	row->Add (new wxStaticText (this, -1, L"stream reading:"), 0, wxALL, 5);
	edit_stream_amplification = new double_edit (this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
	row->Add (edit_stream_amplification, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	check_simplify = new wxCheckBox (this, wxID_ANY, "Simplify", wxDefaultPosition, wxDefaultSize);
	row->Add (check_simplify, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	button_selection_bounds = new wxButton (this, BUTTON_SELECTION_BOUNDS, wxT("Selection"));
	row->Add (button_selection_bounds, 0, wxALL, 5);
	row->AddSpacer (15);
	button_ames = new wxButton (this, BUTTON_AMES, wxT("Ames"));
	row->Add (button_ames, 0, wxALL, 5);
	row->AddSpacer (15);

	row->Add (new wxButton (this, BUTTON_RACCOON, wxT("Raccoon River")), 0, wxALL, 5);
	row->AddSpacer (15);

	button_royal_gorge = new wxButton (this, BUTTON_ROYAL_GORGE, wxT("Royal Gorge"));
	row->Add (button_royal_gorge, 0, wxALL, 5);

	button_e85 = new wxButton (this, BUTTON_E85, wxT("E85"));
	row->Add (button_e85, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, -1, L"Start Date:"), 0, wxALL, 5);
	edit_start_date = new edit_time_point (this, wxID_ANY, L"2000-01-01", wxDefaultPosition, wxDefaultSize);
	row->Add (edit_start_date);
	row->Add (new wxStaticText (this, -1, L"End Date:"), 0, wxALL, 5);
	edit_end_date = new edit_time_point (this, wxID_ANY, L"2004-12-31", wxDefaultPosition, wxDefaultSize);
	row->Add (edit_end_date);
	stack->Add (row);


	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

	static_output_file->SetLabel (filename_dae->get_text ());
	edit_facility->Set (river_view->start_id);
	edit_usgs_station->Set (river_view->destination_id);
	combo_area_definition->SetSelection (river_view->area_definition - 1);
	combo_camera_type->SetSelection (river_view->camera_type);
	check_boundary->SetValue (*boundary_box);
	edit_boundary_north->Set (river_view->map_boundary.y [1], 1);
	edit_boundary_south->Set (river_view->map_boundary.y [0], 1);
	edit_boundary_east->Set (river_view->map_boundary.x [1], 1);
	edit_boundary_west->Set (river_view->map_boundary.x [0], 1);
	check_simplify->SetValue (river_view->simplify);
	check_gridfloat->SetValue (*gridfloat_3dep);
	check_latlon_grid->SetValue (*latlon_grid);
	check_light->SetValue (*light);
	edit_station_amplification->Set (river_view->amplification_station);
	edit_stream_amplification->Set (river_view->amplification_stream);
	edit_start_date->Set (river_view->start_date);
	edit_end_date->Set (river_view->end_date);

	for (layer_index = 0; layer_index < layer_count; ++layer_index) {
		checkbox_layer [layer_index]->SetValue (river_view->render_layers [layer_index]);
		edit_layer [layer_index]->Set (river_view->layer_symbol_size [layer_index]);
	}
}

void dialog_collada::OnSetSelectionBounds
	(wxCommandEvent &)

{
	map_layer *selection_layer;
	bounding_cube selection_boundary;
	
	if ((selection_layer = map->match ("Selection")) != NULL) {
		selection_layer->check_extent (&selection_boundary);
		edit_boundary_east->Set (selection_boundary.x [1], 1);
		edit_boundary_west->Set (selection_boundary.x [0], 1);
		edit_boundary_south->Set (selection_boundary.y [0], 1);
		edit_boundary_north->Set (selection_boundary.y [1], 1);
	}
}

void dialog_collada::OnSetAmes
	(wxCommandEvent &)

{
	edit_boundary_east->Set (-93574000.0, 1);
	edit_boundary_west->Set (-93712500.0, 1);
	edit_boundary_south->Set (42000100.0, 1);
	edit_boundary_north->Set (42076110.0, 1);
}

void dialog_collada::OnSetRaccoon
	(wxCommandEvent &)

{
	edit_usgs_station->Set (6610140);
	combo_area_definition->SetSelection (RIVER_VIEW_AREA_SOURCE_UPSTREAM - 1);
	// Boost amps for large-area view
	edit_station_amplification->Set (5.0);
	edit_stream_amplification->Set (3.0);
}

void dialog_collada::OnSetE85
	(wxCommandEvent &)

{
	/* 
	// Des Moines
	edit_boundary_east->Set (-93500000.0, 1);
	edit_boundary_west->Set (-93833000.0, 1);
	edit_boundary_north->Set (41683300.0, 1);
	edit_boundary_south->Set (41500000.0, 1);
	*/

	/*
	edit_boundary_east->Set (-92902884.0, 1);
	edit_boundary_west->Set (-94678125.0, 1);
	edit_boundary_north->Set (42448803.0, 1);
	edit_boundary_south->Set (41500000.0, 1);
	*/
	int layer_index;
	dynamic_string layer_label;
	std::vector <map_layer*>::const_iterator layer;

	for (layer = map->layers.begin(), layer_index = 0;
	layer != map->layers.end();
	++layer, ++layer_index) {
		if ((*layer)->name == "E85 Prices") {
			(*layer)->symbol_size = 2500;
			layer_label = (*layer)->name;
			layer_label.add_formatted (" SymSize=%d", (*layer)->symbol_size);
			checkbox_layer [layer_index]->SetLabel (layer_label.get_text_ascii ());
		}
		else
			if ((*layer)->name == "Stations Not Reporting") {
				(*layer)->symbol_size = 2500;
				layer_label = (*layer)->name;
				layer_label.add_formatted (" SymSize=%d", (*layer)->symbol_size);
				checkbox_layer [layer_index]->SetLabel (layer_label.get_text_ascii ());
			}
	}

	// edit_usgs_station->Set (22254207);
	combo_area_definition->SetSelection (RIVER_VIEW_ALL_AREA - 1);
	// Boost amps for large-area view
	edit_station_amplification->Set (5000.0);
	// edit_stream_amplification->Set (3.0);
	edit_start_date->SetValue (L"2013-01-01");
	edit_end_date->SetValue (L"2016-06-30");
}

void dialog_collada::OnSetRoyalGorge
	(wxCommandEvent &)

{
	edit_boundary_east->Set (-105275000.0, 1);
	edit_boundary_west->Set (-105366000.0, 1);
	edit_boundary_south->Set (38441000.0, 1);
	edit_boundary_north->Set (38478000.0, 1);
}

void dialog_collada::OnBrowseOutputFile
	(wxCommandEvent &)
{
	wxFileDialog* openFileDialog = new wxFileDialog( this, _("Open file"), "", "", "Collada File (*.dae)|*.dae", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	filename_struct f;
	dynamic_string text;

	f.parse (filename_dae);
 
	f.write_path (&text);
	openFileDialog->SetDirectory (text.get_text_ascii ());

	text = f.filename;
	openFileDialog->SetFilename (text.get_text_ascii ());
	if ( openFileDialog->ShowModal() == wxID_OK ) {

		*filename_dae = openFileDialog->GetDirectory().ToAscii ();
		*filename_dae += PATH_SEPARATOR;
		*filename_dae += openFileDialog->GetFilename().ToAscii ();
		static_output_file->SetLabel (filename_dae->get_text ());
	}
}

void dialog_collada::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		for (int layer_index = 0; layer_index < map->layers.size (); ++layer_index) {
			river_view->render_layers [layer_index] = checkbox_layer [layer_index]->GetValue ();
			river_view->layer_symbol_size [layer_index] = edit_layer [layer_index]->Get ();
		}

		river_view->start_id = edit_facility->Get ();
		river_view->destination_id = edit_usgs_station->Get ();
		river_view->area_definition = combo_area_definition->GetSelection () + 1;
		river_view->camera_type = combo_camera_type->GetSelection ();

		river_view->amplification_station = edit_station_amplification->Get ();
		river_view->amplification_stream = edit_stream_amplification->Get ();

		*boundary_box = check_boundary->GetValue ();
		river_view->map_boundary.y [1] = edit_boundary_north->Get ();
		river_view->map_boundary.y [0] = edit_boundary_south->Get ();
		river_view->map_boundary.x [1] = edit_boundary_east->Get ();
		river_view->map_boundary.x [0] = edit_boundary_west->Get ();

		river_view->simplify = check_simplify->GetValue ();
		*gridfloat_3dep = check_gridfloat->GetValue ();
		*latlon_grid = check_latlon_grid->GetValue ();
		*light = check_light->GetValue ();

		river_view->start_date = edit_start_date->Get ();
		river_view->end_date = edit_end_date->Get ();
	}
	wxDialog::EndModal (retCode);
}

