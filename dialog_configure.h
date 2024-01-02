
class dialog_configure : public wxDialog {
	public:
		enum {
			ID_FILENAME_OUTPUT_VARIABLES = 103,
			ID_BUTTON_FILENAME_OUTPUT_VARIABLES,
			ID_EDIT_PATH,
			ID_LIST_FILES,
			BUTTON_M,
			BUTTON_R
		};
		dialog_configure
			(class wxWindow *,
			class app_CARDGIS *);

		wxButton *button_ok, *button_edit_path;
		wxTextCtrl *edit_filename_output_variables;
		listbox_files *list_files;
		integer_edit *edit_thread_count;
		wxCheckBox *check_use_GL;

		void OnEditPath
			(wxCommandEvent &);
		void OnM
			(wxCommandEvent &);
		void OnR
			(wxCommandEvent &);

	protected:
		class app_CARDGIS *app;
		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
