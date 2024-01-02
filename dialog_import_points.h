
class dialog_import_points : public wxDialog {
	public:
		enum {
			ID_BUTTON_FILENAME_POINTS,
			COMBO_FORMAT,
			GRID_PREVIEW
		};
		dialog_import_points
			(class wxWindow *,
			dynamic_string *filename,
			const dynamic_string & match_id_name, // "" to look for defaults "ID" or "Codido"
			class dynamic_map *p_map,
			BYTE *p_import_format,
			bool *p_normalize_longitude,
			bool *p_numeric_id,
			class Ledger *p_preview,
			std::set <int> *p_text_columns,
			std::set <int> *p_skip_columns,
			int *p_id_column_index,
			int *p_lat_column_index,
			int *p_lon_column_index);
		virtual ~dialog_import_points();

		wxComboBox *combo_format;
		wxStaticText *static_filename, *static_format;
		wxTextCtrl *edit_file_view;
		class wxGrid *grid_preview;
		wxCheckBox *check_normalize_longitude, *check_numeric_id;
		wxFont* fixed_font;

		void OnFormatSelect (wxCommandEvent &);
		void OnBrowsePointFile (wxCommandEvent &);

	protected:
		dynamic_string *filename_points, id_column_name;
		dynamic_map *map;
		BYTE *import_format;
		bool *normalize_longitude, *numeric_id;
		std::vector <dynamic_string>* column_names, raw_text;
		std::set <int> *text_columns, *skip_columns;
		int *id_column_index, *lat_column_index, *lon_column_index;
		class Ledger *preview;

		void show_file ();
		void set_columns (int *column_count);
		void setup_format
			(const BYTE format);
		void show_ledger ();

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
