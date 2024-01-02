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
#include "dialog_configure.h"
#include "app_CARDGIS.h"

void add_edit_path
    (const wxString &static_label,
    const int edit_id,
    wxTextCtrl **edit_control,
    const wxSize &edit_size,
	wxButton *edit_path_button,
    wxWindow *parent,
    wxGridSizer *grid)

{

	// wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);
	grid->Add (new wxStaticText (parent, -1, static_label), 0, wxALIGN_BOTTOM);
    *edit_control = new wxTextCtrl (parent, edit_id, wxEmptyString, wxDefaultPosition, edit_size); //, wxPoint(15, 30), regular_size);
	grid->Add (*edit_control, 0, wxLEFT | wxALIGN_BOTTOM, 6);
	grid->Add (edit_path_button);
}

dialog_configure::dialog_configure
	(wxWindow *pParent,
	app_CARDGIS *p_app)
	: wxDialog (pParent, -1, "Configuration")
{
	app = p_app;
	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxSize regular_size, wide_size;
	regular_size.x = 450;
	regular_size.y = 24;
	wide_size.x = 360;
	wide_size.y = 24;
	
 	wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, -1, L"Files:"), 0, wxALL, 5);
	list_files = new listbox_files (this, ID_LIST_FILES, wxDefaultPosition, wxSize (wide_size.x, 72));
	row->Add (list_files, 0, wxALL, 5);
	button_edit_path = new wxButton (this, ID_EDIT_PATH, _("Edit"), wxDefaultPosition, wxDefaultSize);
	row->Add (button_edit_path, 0, wxALL, 5);
	stack->Add (row);
	stack->AddSpacer (10);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, -1, L"Thread Count:"), 0, wxALL, 5);
	row->AddSpacer (25);
	edit_thread_count = new integer_edit (this, wxID_ANY);
	row->Add (edit_thread_count, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxButton (this, BUTTON_M, _("m:\\"), wxDefaultPosition, wxDefaultSize));
	row->AddSpacer (5);
	row->Add (new wxButton (this, BUTTON_R, _("r:\\"), wxDefaultPosition, wxDefaultSize));
	stack->Add (row, 0, wxALL, 5);

	row = new wxBoxSizer (wxHORIZONTAL);
	check_use_GL = new wxCheckBox (this, wxID_ANY, L"Use GL");
	row->Add (check_use_GL);
	stack->Add (row, 0, wxALL, 5);


	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL), 0, wxALL, 5);

	SetSizerAndFit(stack);

	edit_thread_count->Set (app->multithread_count);
	check_use_GL->SetValue (app->use_gl);

	list_files->add_path ("NHD Root", &app->filename_nhd_root);
	list_files->add_path ("Census Root", &app->filename_census_root);
	list_files->add_path ("TIGER Root", &app->filename_TIGER_root);
	// list_files->add_path ("Other Root", &app->filename_other_root);
	list_files->add_path ("Project Root", &app->project_root);
	list_files->show_filenames ();
}

BEGIN_EVENT_TABLE(dialog_configure, wxDialog)
	EVT_BUTTON(ID_EDIT_PATH, dialog_configure::OnEditPath)
	EVT_BUTTON(BUTTON_M, dialog_configure::OnM)
	EVT_BUTTON(BUTTON_R, dialog_configure::OnR)
END_EVENT_TABLE()

void dialog_configure::OnEditPath
	(wxCommandEvent &)

// Click of Edit button intended for listbox_filenames

{
	wxCommandEvent e (wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, ID_LIST_FILES);
	list_files->ProcessWindowEvent (e);
	/*
	wxFileDialog openFileDialog (this, _("Open TIFF file"), "", "", "TIFF files (*.TIF)|*.TIF", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	openFileDialog.SetPath (app->path_cdl.get_text_wide ());
    if (openFileDialog.ShowModal() != wxID_CANCEL) {
		filename_struct f;
		dynamic_string new_path;
		f.parse (openFileDialog.GetPath().ToAscii ());
		f.write_path (&new_path);
		//edit_path_cdl->SetValue (new_path.get_text ());
    }
    else
        return;     // the user changed idea...
	*/
}

void dialog_configure::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		app->multithread_count = edit_thread_count->Get ();
		app->use_gl = check_use_GL->GetValue ();
	}
	wxDialog::EndModal (retCode);
}

void dialog_configure::OnM
	(wxCommandEvent &)

{
	filename_struct path;
	path.parse (&app->filename_nhd_root);
	path.drive_letter = L'M';
	path.write_file_specification (&app->filename_nhd_root);

	path.parse (&app->filename_census_root);
	path.drive_letter = L'M';
	path.write_file_specification (&app->filename_census_root);

	path.parse (&app->filename_TIGER_root);
	path.drive_letter = L'M';
	path.write_file_specification (&app->filename_TIGER_root);

	path.parse (&app->filename_other_root);
	path.drive_letter = L'M';
	path.write_file_specification (&app->filename_other_root);

	list_files->reload ();
	list_files->show_filenames ();
}

void dialog_configure::OnR
	(wxCommandEvent &)

{
	filename_struct path;

	path.parse (&app->filename_nhd_root);
	path.drive_letter = L'R';
	path.write_file_specification (&app->filename_nhd_root);

	path.parse (&app->filename_census_root);
	path.drive_letter = L'R';
	path.write_file_specification (&app->filename_census_root);

	path.parse (&app->filename_TIGER_root);
	path.drive_letter = L'R';
	path.write_file_specification (&app->filename_TIGER_root);

	path.parse (&app->filename_other_root);
	path.drive_letter = L'R';
	path.write_file_specification (&app->filename_other_root);

	list_files->reload ();
	list_files->show_filenames ();
}
