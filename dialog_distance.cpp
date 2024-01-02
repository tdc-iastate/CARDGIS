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
#include <wx/radiobut.h>
#include <wx/app.h>
#include <wx/button.h>
#include "../util/dynamic_string.h"
#include "../util/filename_struct.h"
#include "../util/custom.h"
#include "../util/utility.h"
#include "../util/device_coordinate.h"
#include "../util/dialog_error_list.h"
#include "dialog_distance.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "dialog_match_attribute.h"

BEGIN_EVENT_TABLE(dialog_distance, wxDialog)
EVT_BUTTON(BUTTON_FIND_START, dialog_distance::OnFindStart)
EVT_BUTTON(BUTTON_FIND_DESTINATION, dialog_distance::OnFindDestination)
EVT_BUTTON(BUTTON_COMPUTE, dialog_distance::OnCompute)
END_EVENT_TABLE()


dialog_distance::dialog_distance
	(wxWindow *pParent,
	map_layer* p_selected_layer,
	map_layer *p_layer_selection_source,
	dynamic_map* p_map)
	: wxDialog (pParent, -1, "Distance")
{
	std::vector <map_object*>::const_iterator pick;
	selection_layer = p_selected_layer;
	layer_selection_source = p_layer_selection_source;
	start_layer = destination_layer = NULL;
	map = p_map;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );
 	wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);

	{
		wxStaticBoxSizer* object_box = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Start:"));
		combo_start_layer = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
		object_box->Add(combo_start_layer, 0, wxALL, 5);

		row = new wxBoxSizer(wxHORIZONTAL);
		row->Add(new wxStaticText(this, -1, L"ID:"), 0, wxALL, 5);
		edit_start_id = new long_integer_edit(this, -1, L"");
		row->Add(edit_start_id, 0, wxALL, 5);
		row->AddSpacer(5);
		row->Add(new wxButton(this, BUTTON_FIND_START, L"Find by Attribute"), 0, wxALL, 5);
		object_box->Add(row);

		stack->Add(object_box, 0, wxALL, 5);
	}

	stack->AddSpacer(5);

	{
		wxStaticBoxSizer* object_box = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Destination:"));
		combo_destination_layer = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
		object_box->Add(combo_destination_layer, 0, wxALL, 5);

		row = new wxBoxSizer(wxHORIZONTAL);
		row->AddSpacer(15);
		row->Add(new wxStaticText(this, -1, L"ID:"), 0, wxALL, 5);
		edit_destination_id = new long_integer_edit(this, -1, L"");
		row->Add(edit_destination_id, 0, wxALL, 5);
		row->AddSpacer(5);
		row->Add(new wxButton(this, BUTTON_FIND_DESTINATION, L"Find by Attribute"), 0, wxALL, 5);
		object_box->Add(row);

		stack->Add(object_box, 0, wxALL, 5);
	}
	stack->AddSpacer(5);

	stack->Add(new wxButton(this, BUTTON_COMPUTE, L"Compute"), 0, wxALL, 5);
	stack->AddSpacer(5);

	row = new wxBoxSizer(wxHORIZONTAL);
	row->Add(new wxStaticText(this, -1, L"Distance, km:"), 0, wxALL, 5);
	static_distance_km = new double_edit (this, wxID_ANY, L"", wxDefaultPosition, wxSize(120, 24));
	row->Add(static_distance_km, 0, wxALL, 5);
	row->AddSpacer(25);
	row->Add(new wxStaticText(this, -1, L"Distance, miles:"), 0, wxALL, 5);
	static_distance_miles = new double_edit(this, wxID_ANY, L"", wxDefaultPosition, wxSize(120, 24));
	row->Add(static_distance_miles, 0, wxALL, 5);
	stack->Add(row);

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

	if (selection_layer
	&& ((pick = selection_layer->objects.begin()) != selection_layer->objects.end()))
		edit_start_id->Set((*pick)->id);

	fill_layer_combo(combo_start_layer, layer_selection_source);
	fill_layer_combo(combo_destination_layer);
}

void dialog_distance::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {

	}
	wxDialog::EndModal (retCode);
}

void dialog_distance::OnCompute
	(wxCommandEvent&)

{
	dynamic_string log;
	long start_id, destination_id;
	class map_object* start, * destination;
	logical_coordinate start_centroid, destination_centroid;

	if ((start_id = edit_start_id->Get()) > 0) {
		start_layer = (map_layer*)combo_start_layer->GetClientData(combo_start_layer->GetSelection());

		if ((start = start_layer->match_id(start_id)) != NULL) {
			if ((destination_id = edit_destination_id->Get()) > 0) {
				destination_layer = (map_layer*)combo_destination_layer->GetClientData(combo_destination_layer->GetSelection());

				if ((destination = destination_layer->match_id(destination_id)) != NULL) {

					start->centroid(&start_centroid);
					destination->centroid(&destination_centroid);
					double distance_m = distance_meters (start_centroid, destination_centroid);

					static_distance_km->Set(distance_m);
					static_distance_miles->Set(distance_m / METERS_PER_MILE);
				}
				else
					log += "ERROR, ID not found in destination layer.\n";
			}
			else
				log += "ERROR, must enter destination ID.\n";
		}
		else
			log += "ERROR, ID not found in start layer.\n";
	}
	else
		log += "ERROR, must enter start ID.\n";

	if (log.get_length() > 0) {
		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"Compute Distance", L"", log);
		d.ShowWindowModal();
	}
}

void dialog_distance::fill_layer_combo
	(wxComboBox* combo,
	map_layer *current_choice)

{
	int index;
	std::vector <map_layer*>::const_iterator layer;

	for (layer = map->layers.begin();
	layer != map->layers.end();
	++layer) {
		index = combo->Append((*layer)->name.get_text());
		combo->SetClientData(index, *layer);

		if ((*layer) == current_choice)
			combo->SetSelection(index);
	}
}

void dialog_distance::OnFindStart
	(wxCommandEvent&)

{
	if ((start_layer = (map_layer*)combo_start_layer->GetClientData(combo_start_layer->GetSelection())) != NULL) {
		std::set <long long> new_selection;
		std::set <long long>::iterator selection;

		dialog_match_attribute dp(this, start_layer, &new_selection);
		if (dp.ShowModal() == wxID_OK) {
			if ((selection = new_selection.begin()) != new_selection.end())
				edit_start_id->Set(*selection);
		}
	}
}

void dialog_distance::OnFindDestination
	(wxCommandEvent&)

{
	if ((destination_layer = (map_layer*)combo_destination_layer->GetClientData(combo_destination_layer->GetSelection())) != NULL) {
		std::set <long long> new_selection;
		std::set <long long>::iterator selection;

		dialog_match_attribute dp(this, destination_layer, &new_selection);
		if (dp.ShowModal() == wxID_OK) {
			if ((selection = new_selection.begin()) != new_selection.end())
				edit_destination_id->Set(*selection);
		}
	}
}