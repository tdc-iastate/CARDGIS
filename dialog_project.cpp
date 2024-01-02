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
#include "dialog_project.h"

dialog_project::dialog_project
	(wxWindow *pParent,
	int *p_first_record,
	int *p_last_record,
	int *cpu_count,
	int *p_first_state_fips,
	int *p_last_state_fips,
	const dynamic_string &p_path_description,
	dynamic_string *p_path)
	: wxDialog (pParent, -1, "Project")
{

	first_record = p_first_record;
	last_record = p_last_record;
	number_of_threads = cpu_count;
	first_state_fips = p_first_state_fips;
	last_state_fips = p_last_state_fips;
	path_description = p_path_description;
	path = p_path;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxSize regular_size, wide_size;
	regular_size.x = 250;
	regular_size.y = 24;
	wide_size.x = 360;
	wide_size.y = 24;

	stack->AddSpacer (10);
	
 	wxBoxSizer *row;

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("First Record:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	edit_first_record = new integer_edit (this, wxID_ANY);
	row->Add (edit_first_record, 0, wxALIGN_BOTTOM);
	row->AddSpacer (15);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Last Record:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	edit_last_record = new integer_edit (this, wxID_ANY);
	row->Add (edit_last_record, 0, wxALIGN_BOTTOM);
	stack->Add (row);

	if (first_state_fips != NULL) {
		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, wxID_ANY, wxT ("First State FIPS:")), 0, wxALIGN_BOTTOM);
		row->AddSpacer (10);
		edit_first_state_fips = new integer_edit (this, wxID_ANY);
		row->Add (edit_first_state_fips, 0, wxALIGN_BOTTOM);
		row->AddSpacer (15);
		row->Add (new wxStaticText (this, wxID_ANY, wxT ("Last State FIPS:")), 0, wxALIGN_BOTTOM);
		row->AddSpacer (10);
		edit_last_state_fips = new integer_edit (this, wxID_ANY);
		row->Add (edit_last_state_fips, 0, wxALIGN_BOTTOM);
		stack->Add (row);
	}

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("# of threads:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	edit_number_of_threads = new integer_edit (this, wxID_ANY);
	row->Add (edit_number_of_threads, 0, wxALIGN_BOTTOM);
	stack->Add (row);

	if (path) {
		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, -1, L"Files:"), 0, wxALL, 5);
		list_files = new listbox_files (this, ID_LIST_FILES, wxDefaultPosition, wxSize (wide_size.x, 72));
		row->Add (list_files, 0, wxALL, 5);
		button_edit_path = new wxButton (this, ID_EDIT_PATH, _ ("Edit"), wxDefaultPosition, wxDefaultSize);
		row->Add (button_edit_path, 0, wxALL, 5);
		stack->Add (row);
	}
	else
		list_files = NULL;

	stack->AddSpacer (25);

	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

	edit_first_record->Set (*first_record);
	edit_last_record->Set (*last_record);
	edit_number_of_threads->Set (*number_of_threads);
	if (first_state_fips) {
		edit_first_state_fips->Set (*first_state_fips);
		edit_last_state_fips->Set (*last_state_fips);
	}
	if (path) {
		list_files->add_path (path_description.get_text_ascii (), path);
		list_files->show_filenames ();
	}
}

BEGIN_EVENT_TABLE(dialog_project, wxDialog)
	EVT_BUTTON(ID_EDIT_PATH, dialog_project::OnEditPath)
	// EVT_CHECKBOX (CHECKBOX_OTHER_FACILITIES, dialog_project::OnCheckOtherFacilities)
END_EVENT_TABLE()

void dialog_project::OnEditPath
	(wxCommandEvent &)

// Click of Edit button intended for listbox_filenames

{
	wxCommandEvent e (wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, ID_LIST_FILES);
	list_files->ProcessWindowEvent (e);
}

void dialog_project::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		*first_record = edit_first_record->Get ();
		*last_record = edit_last_record->Get ();
		*number_of_threads = edit_number_of_threads->Get ();
		if (first_state_fips) {
			*first_state_fips = edit_first_state_fips->Get ();
			*last_state_fips = edit_last_state_fips->Get ();
		}
	}
	wxDialog::EndModal (retCode);
}

