
class dialog_open_database : public wxDialog {
	public:
		enum {
			ID_DB_NAME = 103,
			ID_DRIVER_NAME = 104,
			ID_HOSTNAME = 105,
			ID_USERNAME = 106,
			ID_PASSWORD = 107,
			ID_SOCKET,
			ID_LIST_DATABASE_TYPE,
			ID_CONNECTION_STRING,
			ID_BUTTON_PREVIEW,
			ID_BUTTON_NEW_CONNECTION,
			ID_BUTTON_DELETE_CONNECTION,
			ID_COMBO_CREDENTIALS
		};
		dialog_open_database
			(class wxWindow *,
			std::vector <odbc_database_credentials> *,
			class odbc_database_credentials *);

		wxComboBox *combo_database_type, *combo_credentials;
		wxTextCtrl *edit_driver_name, *edit_hostname, *edit_username, *edit_password, *edit_dbname, *edit_connection_string, *edit_socket;
		wxButton *button_ok, *button_preview, *button_new, *button_delete;

		void OnPreviewConnectionString
			(wxCommandEvent &);
		void OnNewConnection
			(wxCommandEvent &);
		void OnDeleteConnection
			(wxCommandEvent &);
		void OnSelChangeDBType
			(wxCommandEvent &);
	protected:
		std::vector <odbc_database_credentials> *db_credentials;
		odbc_database_credentials *credentials;

		virtual void EndModal
			(int retCode);
		void fill_cred_combo ();
		int find_cred_index
			(const class odbc_database_credentials *cred);
		void set_credentials ();
		void show_selected_db ();
	DECLARE_EVENT_TABLE()
};
