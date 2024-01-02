#include "../util/utility_afx.h"
#include <vector>
#include <set>
#include <map>
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
#include "../util/device_coordinate.h"
#include "dialog_river_segment.h"
// #include "../map/map_scale.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"

dialog_river_segment::dialog_river_segment
	(wxWindow *pParent,
	dynamic_map *p_map,
	map_layer **p_layer_from,
	map_layer **p_layer_to,
	long long *facility,
	long long *station)
	: wxDialog (pParent, -1, "River Segment")
{
	facility_id = facility;
	station_id = station;
	map = p_map;
	layer_from = p_layer_from;
	layer_to = p_layer_to;
	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

 	wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);
	combo_start_layer = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("'From' Layer:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	fill_layer_combo (combo_start_layer, *layer_from);
	row->Add (combo_start_layer);
	stack->Add (row);
	stack->AddSpacer (5);

 	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, -1, L"Starting point:"), 0, wxALIGN_BOTTOM);
	row->AddSpacer (15);
	edit_facility = new long_long_integer_edit (this, -1, L"");
	row->Add (edit_facility);
	stack->Add (row);
	stack->AddSpacer (25);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("'To' Layer:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	combo_end_layer = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	fill_layer_combo (combo_end_layer, *layer_to);
	row->Add (combo_end_layer);
	stack->Add (row);
	stack->AddSpacer (5);

 	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, -1, L"Ending point:"), 0, wxALIGN_BOTTOM);
	row->AddSpacer (15);
	edit_usgs_station = new long_long_integer_edit (this, -1, L"");
	row->Add (edit_usgs_station);
	stack->Add (row);

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

	edit_facility->Set (*facility_id);
	edit_usgs_station->Set (*station_id);
}

BEGIN_EVENT_TABLE(dialog_river_segment, wxDialog)
END_EVENT_TABLE()

void dialog_river_segment::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		int index;

		if ((index = combo_start_layer->GetSelection ()) != -1)
			*layer_from = map->layers [index];

		if ((index = combo_end_layer->GetSelection ()) != -1)
			*layer_to = map->layers[index];

		*facility_id = edit_facility->Get ();
		*station_id = edit_usgs_station->Get ();
	}
	wxDialog::EndModal (retCode);
}

void dialog_river_segment::fill_layer_combo
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

