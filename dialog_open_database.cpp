#include "../util/utility_afx.h"
#include <vector>
#include "../util/dynamic_string.h"
#include <set>
#include <wx/stattext.h>
#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/filedlg.h>
#include <wx/clipbrd.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/app.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include "../util/dialog_error_list.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_database_sql_server.h"
#include "../odbc/odbc_database_mysql.h"
#include "dialog_open_database.h"

void add_static_edit
    (const wxString &static_label,
    const int edit_id,
    wxTextCtrl **edit_control,
    const wxSize &edit_size,
    wxWindow *parent,
    wxGridSizer *grid,
	const wxString &comment_text,
	const bool password_style = false)

{

	// wxBoxSizer *row = new wxBoxSizer (wxHORIZONTAL);
	grid->Add (new wxStaticText (parent, -1, static_label), 0, wxALIGN_BOTTOM);
	if (password_style)
	    *edit_control = new wxTextCtrl (parent, edit_id, wxEmptyString, wxDefaultPosition, edit_size, wxTE_PASSWORD);
	else
	    *edit_control = new wxTextCtrl (parent, edit_id, wxEmptyString, wxDefaultPosition, edit_size);
	grid->Add (*edit_control, 0, wxLEFT | wxALIGN_BOTTOM, 6);
	grid->Add (new wxStaticText (parent, -1, comment_text), 0, wxALIGN_BOTTOM);
}

dialog_open_database::dialog_open_database
	(wxWindow *pParent,
	std::vector <odbc_database_credentials> *p_cred_list,
	class odbc_database_credentials *p_cred)
	: wxDialog (pParent, -1, "Open database")
{
	db_credentials = p_cred_list;
	credentials = p_cred;

    wxBoxSizer *stack = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *row;
    wxFlexGridSizer *grid = new wxFlexGridSizer (12, 3, 1, 2);

	wxSize regular_size, wide_size;
	regular_size.x = 250;
	regular_size.y = 24;
	wide_size.x = 450;
	wide_size.y = 24;

	// Credential Combo
	combo_credentials = new wxComboBox (this, ID_COMBO_CREDENTIALS, wxEmptyString, wxDefaultPosition, wide_size, 0, NULL);
	stack->Add (combo_credentials);

	// Buttons to add/delete credentials
    row = new wxBoxSizer (wxHORIZONTAL);
	button_new = new wxButton (this, ID_BUTTON_NEW_CONNECTION, _("Connect to New Database"), wxDefaultPosition, wxDefaultSize);
	row->Add (button_new);
	button_delete = new wxButton (this, ID_BUTTON_DELETE_CONNECTION, _("Remove Connection"), wxDefaultPosition, wxDefaultSize);
	row->Add (button_delete);
	stack->Add (row);
	stack->AddSpacer (4);

	// DBType
    row = new wxBoxSizer (wxHORIZONTAL);
	wxString database_type_names [ODBC_NUMBER_OF_DATABASE_TYPES];
	database_type_names [0] = "Access";
	database_type_names [1] = "MySQL";
	database_type_names [2] = "SQL Server";
	database_type_names [3] = "Firebird";
	combo_database_type = new wxComboBox (this, ID_LIST_DATABASE_TYPE, wxEmptyString, wxDefaultPosition, wxDefaultSize, ODBC_NUMBER_OF_DATABASE_TYPES, database_type_names);
	row->Add (new wxStaticText (this, -1, "Database Type"), 0, wxALIGN_BOTTOM);
	row->Add (combo_database_type, 0, wxLEFT | wxALIGN_BOTTOM, 6);
	stack->Add (row);

    add_static_edit (wxT("DB Name:"), ID_DB_NAME, &edit_dbname, regular_size, this, grid, L"-");
    add_static_edit (wxT("Driver Name:"), ID_DRIVER_NAME, &edit_driver_name, regular_size, this, grid, L"-");
	add_static_edit (wxT("Hostname:"), ID_HOSTNAME, &edit_hostname, regular_size, this, grid, L".\\SQLEXPRESS");
	add_static_edit (wxT("User Name:"), ID_USERNAME, &edit_username, regular_size, this, grid, L"-");
	add_static_edit (wxT("Password:"), ID_PASSWORD, &edit_password, regular_size, this, grid, L"-", true);
	add_static_edit (wxT("Socket:"), ID_SOCKET, &edit_socket, regular_size, this, grid, L"-");
	stack->Add (grid);
	stack->AddSpacer (4);

	button_preview = new wxButton (this, ID_BUTTON_PREVIEW, _("Preview Connection String"), wxDefaultPosition, wxDefaultSize);
	stack->Add (button_preview);
    edit_connection_string = new wxTextCtrl (this, ID_CONNECTION_STRING, wxEmptyString, wxDefaultPosition, wide_size);
	stack->Add (edit_connection_string);

	stack->Add (100, 50);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);


	fill_cred_combo ();
	show_selected_db ();
}

BEGIN_EVENT_TABLE(dialog_open_database, wxDialog)
	EVT_BUTTON(ID_BUTTON_PREVIEW, dialog_open_database::OnPreviewConnectionString)
	EVT_COMBOBOX (ID_LIST_DATABASE_TYPE, dialog_open_database::OnSelChangeDBType)
	EVT_BUTTON(ID_BUTTON_NEW_CONNECTION, dialog_open_database::OnNewConnection)
	EVT_BUTTON(ID_BUTTON_DELETE_CONNECTION, dialog_open_database::OnDeleteConnection)
END_EVENT_TABLE()

void dialog_open_database::OnSelChangeDBType
	(wxCommandEvent &)

{
	BYTE type = (BYTE) combo_database_type->GetSelection () + 1;
	switch (type) {
		case ODBC_ACCESS:
			edit_driver_name->SetValue (ODBC_ACCESS_DEFAULT_DRIVER);
			edit_hostname->SetValue (".");
			break;
		case ODBC_SQLSERVER:
			edit_driver_name->SetValue (ODBC_SQLSERVER_DEFAULT_DRIVER);
			edit_hostname->SetValue (".\\SQLEXPRESS");
			break;
		case ODBC_MYSQL:
			edit_driver_name->SetValue (ODBC_MYSQL_DEFAULT_DRIVER);
			edit_hostname->SetValue ("localhost");
	}
}

void dialog_open_database::set_credentials ()

{
	dynamic_string ud;

	credentials->type = (BYTE) combo_database_type->GetSelection () + 1;

	ud = edit_driver_name->GetValue ().ToAscii ();
	credentials->set_driver_name (ud);

	ud = edit_hostname->GetValue ().ToAscii ();
	credentials->set_hostname (ud);

	ud = edit_username->GetValue ().ToAscii ();
	credentials->set_username (ud);

	ud = edit_password->GetValue ().ToAscii ();
	credentials->set_password (ud);

	credentials->database_name = edit_dbname->GetValue ().ToAscii ();
}

void dialog_open_database::OnNewConnection
	(wxCommandEvent &)

{
	odbc_database_credentials new_creds;

	if (db_credentials->size () > 0) {
		new_creds.type = (*db_credentials) [db_credentials->size () - 1].type;
		new_creds.hostname = (*db_credentials) [db_credentials->size () - 1].hostname;
		db_credentials->push_back (new_creds);
		credentials->copy ((*db_credentials) [db_credentials->size () - 1]);
	}
	else {
		db_credentials->push_back (new_creds);
		credentials->copy ((*db_credentials) [db_credentials->size () - 1]);
	}

	show_selected_db ();
}

void dialog_open_database::fill_cred_combo ()

{
	int index;
	dynamic_string log, entry;
	std::vector <odbc_database_credentials>::const_iterator credential;

	combo_credentials->Clear ();
	for (credential = db_credentials->begin (), index = 0;
	credential != db_credentials->end ();
	++credential, ++index) {
		entry = "\"";
		entry += credential->database_name;
		entry += "\" ";
		entry += database_type_name (credential->type);
		combo_credentials->Append (entry.get_text_ascii ());
		if (*credential == *credentials)
			combo_credentials->SetSelection (index);
	}
}

int dialog_open_database::find_cred_index
	(const odbc_database_credentials *cred)

{
	int target_index = -1, index;
	std::vector <odbc_database_credentials>::const_iterator credential;

	for (credential = db_credentials->begin (), index = 0;
	credential != db_credentials->end ();
	++credential, ++index) {
		if (*credential == *cred)
			target_index = index;
	}

	return target_index;
}

void dialog_open_database::OnDeleteConnection
	(wxCommandEvent &)

{
	int target_index;

	target_index = find_cred_index (credentials);
	if (target_index != -1) {
		dynamic_string prompt;

		prompt.format ("Delete connection to DB \"%s\" ?", credentials->database_name.get_text ());
		if (wxMessageBox (prompt.get_text_ascii (),  wxT("Confirm Delete"), wxYES_NO) == wxYES) {

			db_credentials->erase (db_credentials->begin () + target_index);
			credentials->copy ((*db_credentials) [db_credentials->size () - 1]);
			fill_cred_combo ();
			show_selected_db ();
		}
	}
	else {
		// fields on screen are not in vector yet, so just show last member
		credentials->copy ((*db_credentials) [db_credentials->size () - 1]);
		fill_cred_combo ();
		show_selected_db ();
	}
}

void dialog_open_database::OnPreviewConnectionString
	(wxCommandEvent &)

{
	set_credentials ();

	odbc_database *preview_db;
	dynamic_string connection_string;

	switch (credentials->type) {
		case ODBC_ACCESS:
			preview_db = new odbc_database_access;
			break;
		case ODBC_SQLSERVER:
			preview_db = new odbc_database_sql_server;
			break;
		case ODBC_MYSQL:
			preview_db = new odbc_database_mysql;
			break;
		default:
			preview_db = NULL;
	}
	if (preview_db) {
		preview_db->make_connection_string (credentials, true, connection_string);
		edit_connection_string->SetValue (connection_string.get_text ());
		delete preview_db;
	}
}

void dialog_open_database::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {

		credentials->type = combo_database_type->GetSelection () + 1;
		credentials->database_name = edit_dbname->GetValue ().ToAscii ();
		credentials->driver_name = edit_driver_name->GetValue ().ToAscii ();
		credentials->hostname = edit_hostname->GetValue ().ToAscii ();
		credentials->username = edit_username->GetValue ().ToAscii ();
		credentials->socket = edit_socket->GetValue ().ToAscii ();

		dynamic_string p;
		p = edit_password->GetValue ().ToAscii ();
		credentials->set_password (p.get_text ());
	}
	wxDialog::EndModal (retCode);
}

void dialog_open_database::show_selected_db ()

{
	combo_database_type->SetSelection (credentials->type - 1);
	edit_dbname->SetValue (credentials->database_name.get_text ());
	edit_driver_name->SetValue (credentials->driver_name.get_text ());
	edit_hostname->SetValue (credentials->hostname.get_text ());
	edit_username->SetValue (credentials->username.get_text ());
	edit_socket->SetValue (credentials->socket.get_text ());
	// edit_password->SetValue (credentials->password);
}

