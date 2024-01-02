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
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "dialog_buffer.h"
// #include "../i_swat_database/SWATDeclarations.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

dialog_buffer::dialog_buffer
	(wxWindow *pParent,
	dynamic_map *p_map,
	map_layer **p_facility_layer,
	map_object **p_facility,
	map_layer **p_census_layer,
	std::vector <double> *p_buffer_widths,
	double *p_start_upstream_miles,
	double *p_length_upstream_miles,
	double *p_start_downstream_miles,
	double *p_length_downstream_miles,
	double *p_cut_line_length_miles)
	: wxDialog (pParent, -1, "Buffer")
{
	dynamic_string filename_VAA, filename_network, filename_usgs, filename_storet_legacy, filename_storet_modern;

	map = p_map;
	facility_layer = p_facility_layer;
	facility = p_facility;
	census_layer = p_census_layer;
	buffer_widths = p_buffer_widths;
	start_upstream_miles = p_start_upstream_miles;
	length_upstream_miles = p_length_upstream_miles;
	start_downstream_miles = p_start_downstream_miles;
	length_downstream_miles = p_length_downstream_miles;
	cut_line_length_miles = p_cut_line_length_miles;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	stack->AddSpacer (10);

 	wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);
	combo_facility_layer = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_DROPDOWN);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("'From' Layer:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	fill_layer_combo (combo_facility_layer, *facility_layer);
	row->Add (combo_facility_layer);
	stack->Add (row);
	stack->AddSpacer (5);

	row = new wxBoxSizer (wxHORIZONTAL);
	combo_polygon_layer = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_DROPDOWN);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Census Layer:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	fill_layer_combo (combo_polygon_layer, *census_layer);
	row->Add (combo_polygon_layer);
	stack->Add (row);
	stack->AddSpacer (5);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, -1, L"Starting point:"), 0, wxALIGN_BOTTOM);
	row->AddSpacer (15);
	edit_facility = new long_integer_edit (this, -1, L"");
	row->Add (edit_facility);
	stack->Add (row);
	stack->AddSpacer (25);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, L"Widths, miles:", wxDefaultPosition));
	edit_buffer_width = new wxTextCtrl (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	row->Add (edit_buffer_width, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, L"Start upstream, miles:", wxDefaultPosition));
	edit_start_upstream = new double_edit (this, wxID_ANY, wxEmptyString);
	row->Add (edit_start_upstream, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, L"Length upstream, miles:", wxDefaultPosition));
	edit_length_upstream = new double_edit (this, wxID_ANY, wxEmptyString);
	row->Add (edit_length_upstream, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, L"Start downstream, miles:", wxDefaultPosition));
	edit_start_downstream = new double_edit (this, wxID_ANY, wxEmptyString);
	row->Add (edit_start_downstream, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, L"Length downstream, miles:", wxDefaultPosition));
	edit_length_downstream = new double_edit (this, wxID_ANY, wxEmptyString);
	row->Add (edit_length_downstream, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, L"Cut line length, miles:", wxDefaultPosition));
	edit_cut_line_length = new double_edit (this, wxID_ANY, wxEmptyString);
	row->Add (edit_cut_line_length, 0, wxALL, 5);
	stack->Add (row);


	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));
	
	if (*facility)
		edit_facility->Set ((*facility)->id);

	std::vector <double>::iterator width;
	dynamic_string text;
	for (width = buffer_widths->begin (); width != buffer_widths->end (); ++width)
		text.add_formatted (" %.2lf", *width);
	edit_buffer_width->SetValue (text.get_text ());

	edit_start_upstream->Set (*start_upstream_miles);
	edit_length_upstream->Set (*length_upstream_miles);
	edit_start_downstream->Set (*start_downstream_miles);
	edit_length_downstream->Set (*length_downstream_miles);
	edit_cut_line_length->Set (*cut_line_length_miles);

	SetSizerAndFit(stack);
}

BEGIN_EVENT_TABLE(dialog_buffer, wxDialog)
END_EVENT_TABLE()

void dialog_buffer::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		int index;
		long facility_id;

		if ((index = combo_facility_layer->GetSelection ()) != -1)
			*facility_layer = map->layers [index];

		facility_id = edit_facility->Get ();
		*facility = (*facility_layer)->match_id (facility_id);

		if ((index = combo_polygon_layer->GetSelection ()) != -1)
			*census_layer = map->layers [index];

		{
			dynamic_string text;
			std::vector <dynamic_string> tokens;
			std::vector <dynamic_string>::iterator token;

			buffer_widths->clear ();
			text = edit_buffer_width->GetValue ().ToAscii ();

			text.tokenize (" \t", &tokens, true, true);
			for (token = tokens.begin ();
			token != tokens.end ();
			++token)
				buffer_widths->push_back (atof (token->get_text_ascii ()));
		}

		*start_upstream_miles = edit_start_upstream->Get ();
		*length_upstream_miles = edit_length_upstream->Get ();
		*start_downstream_miles = edit_start_downstream->Get ();
		*length_downstream_miles = edit_length_downstream->Get ();
		*cut_line_length_miles = edit_cut_line_length->Get ();

	}
	wxDialog::EndModal (retCode);
}

void dialog_buffer::fill_layer_combo
	(wxComboBox *combo,
	const map_layer *selected)

{
	int index;
	std::vector <map_layer*>::const_iterator layer;

	for (layer = map->layers.begin();
	layer != map->layers.end();
	++layer) {
		index = combo->Append ((*layer)->name.get_text_ascii (), (void *) *layer);
		if ((*layer) == selected)
			combo->SetSelection (index);
	}
}

