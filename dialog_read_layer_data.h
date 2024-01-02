
class dialog_read_layer_data : public wxDialog {
	public:
		enum {
			ID_BUTTON_FILENAME,
			COMBO_FORMAT
		};
		dialog_read_layer_data
			(class wxWindow *,
			dynamic_string *filename,
			dynamic_string *p_table_name,
			BYTE *p_import_format,
			dynamic_string *p_filter_id,
			std::vector <dynamic_string> *);

		wxComboBox *combo_format;
		wxStaticText *static_filename, *static_format;
		wxTextCtrl *edit_preview, *edit_filter_id;
		wxCheckBox *check_normalize_longitude;

		void OnFormatSelect (wxCommandEvent &);
		void OnBrowseFile (wxCommandEvent &);

	protected:
		dynamic_string *filename_points, *table_name, *filter_id;
		BYTE *import_format;
		std::vector <dynamic_string> *column_names;
		// std::set <int> *text_columns, *skip_columns;

		bool preview_5_lines
			(dynamic_string five_lines [],
			dynamic_string &log);
		void preview_format
			(const BYTE format);
		void preview_odbc ();

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
