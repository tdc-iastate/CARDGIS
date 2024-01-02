
class dialog_project : public wxDialog {
	public:
		enum {
			ID_FILENAME_OUTPUT_VARIABLES = 103,
			ID_BUTTON_FILENAME_OUTPUT_VARIABLES,
			ID_READ_USGS_BIN,
			ID_READ_STORET_LEGACY,
			ID_READ_STORET_MODERN,
			ID_LIST_FILES,
			ID_EDIT_PATH
		};
		dialog_project
			(class wxWindow *,
			int *p_first_record,
			int *p_last_record,
			int *cpu_count,
			int *p_first_state_fips,
			int *p_last_state_fips,
			const dynamic_string &p_path_description = "",
			dynamic_string *p_path = NULL);

		std::vector <wxCheckBox *> checkbox_layer;
		integer_edit *edit_first_record, *edit_last_record, *edit_number_of_threads, *edit_first_state_fips, *edit_last_state_fips;
		wxButton *button_ok;
		class listbox_files *list_files;
		wxButton *button_edit_path;

	protected:
		int *first_record, *last_record, *number_of_threads, *first_state_fips, *last_state_fips;
		dynamic_string path_description, *path;
		void OnEditPath (wxCommandEvent &);
		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
