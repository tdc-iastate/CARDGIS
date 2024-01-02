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
#include "dialog_read_TIGER.h"

dialog_read_TIGER::dialog_read_TIGER
	(wxWindow *pParent,
	dynamic_string *p_path,
	class bounding_cube *p_current_extent,
	std::vector <dynamic_string> *street_types,
	bool *open_street_type)
	: wxDialog (pParent, -1, "Read TIGER streets")
{
	int layer_index, layer_count;
	wxCheckBox *checkbox;

	tiger_path = p_path;
	current_extent = p_current_extent;
	street_type_names = street_types;
	read_street_type = open_street_type;
	layer_count = street_types->size ();

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
		checkbox = new wxCheckBox (this, wxID_ANY, (*street_type_names) [layer_index].get_text_ascii (), wxDefaultPosition, wxDefaultSize);
		checkbox_layer.push_back (checkbox);
		row->Add (checkbox, 0);
		row->AddSpacer (15);
	}

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);
	for (layer_index = 0; layer_index < layer_count; ++layer_index)
		checkbox_layer [layer_index]->SetValue (read_street_type [layer_index]);
}

BEGIN_EVENT_TABLE(dialog_read_TIGER, wxDialog)
	// EVT_CHECKBOX (CHECKBOX_OTHER_FACILITIES, dialog_read_TIGER::OnCheckOtherFacilities)
END_EVENT_TABLE()

/*
void dialog_read_TIGER::OnCheckOtherFacilities
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

void dialog_read_TIGER::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		for (int layer_index = 0; layer_index < checkbox_layer.size (); ++layer_index)
			read_street_type [layer_index] = checkbox_layer [layer_index]->GetValue ();
	}
	wxDialog::EndModal (retCode);
}

