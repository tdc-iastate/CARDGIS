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
#include "../util/dialog_error_list.h"
#include "../util/device_coordinate.h"
#include "dialog_match_attribute.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"

BEGIN_EVENT_TABLE(dialog_match_attribute, wxDialog)
	EVT_BUTTON (RADIO_TEXT, dialog_match_attribute::OnText)
	EVT_BUTTON (RADIO_NUMERIC, dialog_match_attribute::OnNumeric)
END_EVENT_TABLE()


dialog_match_attribute::dialog_match_attribute
	(wxWindow *pParent,
	map_layer *p_selected_layer,
	std::set <long long>* p_new_selection)
	: wxDialog (pParent, -1, "Match Attribute")
{
	new_selection = p_new_selection;
	selected_layer = p_selected_layer;
	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

 	wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);

	static_layer_name = new wxStaticText (this, wxID_ANY, L"", wxDefaultPosition, wxSize (400, 24));
	row->Add (static_layer_name, 0, wxALL, 5);
	stack->Add (row);
	stack->AddSpacer (5);

 	row = new wxBoxSizer (wxHORIZONTAL);
	radio_text = new wxRadioButton (this, RADIO_TEXT, wxT ("Text"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	row->Add (radio_text, 0, wxALL, 5);
	row->AddStretchSpacer ();
	combo_attributes_text = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	row->Add (combo_attributes_text, 0, wxALL, 5);
	stack->Add (row);
	stack->AddSpacer (5);

	row = new wxBoxSizer (wxHORIZONTAL);
	radio_numeric = new wxRadioButton (this, RADIO_TEXT, wxT ("Numeric"));
	row->Add (radio_numeric, 0, wxALL, 5);
	row->AddStretchSpacer ();
	combo_attributes_numeric = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	row->Add (combo_attributes_numeric, 0, wxALL, 5);
	stack->Add (row);
	stack->AddSpacer (20);


 	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, -1, L"Value:"), 0, wxALL, 5);
	row->AddSpacer (15);
	edit_value = new wxTextCtrl (this, wxID_ANY, L"");
	row->Add (edit_value, 0, wxALL, 5);
	stack->Add (row);

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

	static_layer_name->SetLabel (selected_layer->name.get_text ());
	fill_attribute_combo (combo_attributes_text, &selected_layer->column_names_text);
	fill_attribute_combo (combo_attributes_numeric, &selected_layer->column_names_numeric);
}

void dialog_match_attribute::OnText (wxCommandEvent &)

{
	// ON_BN_CLICKED is sent at least once as dialog is initiailized, 
	// so check that a real click has occured with GetCheck
	if (radio_text->GetValue () == true) {
		combo_attributes_text->Enable (true);
		combo_attributes_numeric->Enable (false);
	}
}

void dialog_match_attribute::OnNumeric (wxCommandEvent &)

{
	// ON_BN_CLICKED is sent at least once as dialog is initiailized, 
	// so check that a real click has occured with GetCheck
	if (radio_numeric->GetValue () == true) {
		combo_attributes_text->Enable (false);
		combo_attributes_numeric->Enable (true);
	}
}

bool dialog_match_attribute::find_target
	(const int numeric_index,
	const int text_index,
	dynamic_string &match_value,
	dynamic_string &log)

{
	std::vector <map_object*>::const_iterator polygon;

	SetCursor(*wxHOURGLASS_CURSOR);

	if (text_index != -1) {
		if (selected_layer->attribute_count_text > text_index) {
			for (polygon = selected_layer->objects.begin();
			polygon != selected_layer->objects.end();
			++polygon) {
				if ((*polygon)->attributes_text[text_index] == match_value)
					new_selection->insert((*polygon)->id);
			}
		}
		else
			log += "ERROR, text attribute index invalid.\n";
	}
	else
		if (numeric_index != -1) {
			double match_double;
			match_double = atof(match_value.get_text_ascii());

			if (selected_layer->attribute_count_numeric > numeric_index) {
				for (polygon = selected_layer->objects.begin();
					polygon != selected_layer->objects.end();
					++polygon) {
					if ((*polygon)->attributes_numeric[numeric_index] == match_double)
						new_selection->insert((*polygon)->id);
				}
			}
			else
				log += "ERROR, numeric attribute index invalid.\n";
		}

	SetCursor(*wxSTANDARD_CURSOR);

	if (new_selection->size() > 0)
		return true;
	else {
		log += "ERROR, can't find value \"";
		log += match_value;
		if (text_index != -1)
			log.add_formatted("\" in text attribute column [%d].\n", text_index);
		else
			log.add_formatted("\" in numeric attribute column [%d].\n", numeric_index);
		return false;
	}

}

void dialog_match_attribute::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		int numeric_index, text_index;
		dynamic_string match_value, log;

		if (radio_text->GetValue () == true) {
			text_index = combo_attributes_text->GetCurrentSelection ();
			numeric_index = -1;
		}
		else {
			text_index = -1;
			numeric_index = combo_attributes_numeric->GetCurrentSelection ();
		}

		match_value = edit_value->GetValue ().ToAscii ();

		if (find_target(numeric_index, text_index, match_value, log))
			wxDialog::EndModal(retCode);
		else {
			log.convert_linefeeds_for_CEdit();
			dialog_error_list d(this, L"Match Attribute", L"", log);
			d.ShowWindowModal();
		}
	}
	else
		wxDialog::EndModal(retCode);
}

void dialog_match_attribute::fill_attribute_combo
	(wxComboBox *combo,
	const std::vector <dynamic_string> *column_names)

{
	int index;
	std::vector <dynamic_string>::const_iterator column;

	for (column = column_names->begin ();
	column != column_names->end ();
	++column) {
		index = combo->Append (column->get_text ());
		if (index == 0)
			combo->SetSelection (index);
	}
}
