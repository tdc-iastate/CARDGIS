
class dialog_route_points : public wxDialog {
	public:
		enum {
			ID_MAP = 103,
			COMBO_SELECT_LAYER_START,
			COMBO_SELECT_LAYER_FINISH,
			ID_STATIC_MAP_STATUS,
			ID_BUTTON_FILENAME
		};
		dialog_route_points
			(class wxWindow *,
			class dynamic_map *,
			class map_layer **from,
			class map_layer **to,
			bool *output_file,
			dynamic_string *p_filename,
			bool *p_clip_1000m,
			char *p_output_delimiter,
			bool *p_write_station_name);

		class wxComboBox *combo_start_layer, *combo_end_layer, *combo_delimiter;
		class wxCheckBox *check_1000m;
		class wxCheckBox *check_output_file, *check_write_station_name;
		class wxStaticText *static_filename;
		void OnBrowseOutputFile (wxCommandEvent &);

	protected:
		class dynamic_map *map;
		class map_layer **layer_from, **layer_to;
		class interface_window_wx *status_window; // encapsulated pointer to static_map_status
		bool *clip_1000m;
		bool *write_output_file;
		char *output_delimiter;
		bool *write_station_name;
		dynamic_string *filename_output;

		void fill_layer_combo
			(class wxComboBox *combo,
			const map_layer *selected);

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
