#include "../util/utility_afx.h"
#include <vector>
#include <set>
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
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "dialog_read_area.h"

dialog_read_area::dialog_read_area
	(wxWindow *pParent,
	const int number_of_layers,
	bool *p_read_layers,
	bool *p_read_timed_data,
	dynamic_string *p_layer_names,
	dynamic_string *p_layer_paths)
	: wxDialog (pParent, -1, "Read Area Files")
{
	int layer_index;
	wxCheckBox *checkbox;
	wxStaticText *static_text;

	layer_count = number_of_layers;
	read_layers = p_read_layers;
	layer_names = p_layer_names;
	read_timed_data = p_read_timed_data;
	layer_paths = p_layer_paths;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxSize regular_size, wide_size;
	regular_size.x = 250;
	regular_size.y = 24;
	wide_size.x = 360;
	wide_size.y = 24;

	stack->AddSpacer (10);
	
 	wxBoxSizer *row;

	for (layer_index = 0; layer_index < layer_count; ++layer_index) {
		row = new wxBoxSizer (wxHORIZONTAL);
		checkbox = new wxCheckBox (this, wxID_ANY, layer_names [layer_index].get_text_ascii (), wxDefaultPosition, wxDefaultSize);
		checkbox_layer.push_back (checkbox);
		row->Add (checkbox, 0);
		row->AddSpacer (15);

		checkbox = new wxCheckBox (this, wxID_ANY, L"Timed Data", wxDefaultPosition, wxDefaultSize);
		checkbox_timed_data.push_back (checkbox);
		row->Add (checkbox, 0);
		row->AddSpacer (15);

		static_text = new wxStaticText (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize (450, 25));
		static_layer.push_back (static_text);
		row->Add (static_text);
		stack->Add (row);
		stack->AddSpacer (10);

	}

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

	for (layer_index = 0; layer_index < layer_count; ++layer_index) {
		static_layer [layer_index]->SetLabel (layer_paths [layer_index].get_text ());
		checkbox_layer [layer_index]->SetValue (read_layers [layer_index]);
		checkbox_timed_data [layer_index]->SetValue (read_timed_data [layer_index]);
	}
}

BEGIN_EVENT_TABLE(dialog_read_area, wxDialog)
	// EVT_CHECKBOX (CHECKBOX_OTHER_FACILITIES, dialog_read_area::OnCheckOtherFacilities)
END_EVENT_TABLE()

/*
void dialog_read_area::OnCheckOtherFacilities
	(wxCommandEvent &)

{
	if (check_other_facilities->GetValue ()) {
		filename_facilities = path_other_root;
		filename_facilities += area_name;
		filename_facilities += "-Facilities.bin";
		static_facilities->SetLabel (filename_facilities.get_text ());
	}
	else {
		filename_facilities = path_nhd_root;
		filename_facilities += area_name;
		filename_facilities += "-Facilities.bin";
		static_facilities->SetLabel (filename_facilities.get_text ());
	}
}
*/

void dialog_read_area::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		for (int layer_index = 0; layer_index < layer_count; ++layer_index) {
			read_layers [layer_index] = checkbox_layer [layer_index]->GetValue ();
			read_timed_data [layer_index] = checkbox_timed_data [layer_index]->GetValue ();
		}
	}
	wxDialog::EndModal (retCode);
}

