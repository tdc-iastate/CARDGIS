
class dialog_read_area : public wxDialog {
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
		dialog_read_area
			(class wxWindow *,
			const int number_of_layers,
			bool *p_read_layers,
			bool *p_read_timed_data,
			dynamic_string *p_layer_names,
			dynamic_string *p_layer_paths);

		wxButton *button_ok, *button_edit_path;
		std::vector <wxCheckBox *> checkbox_layer;
		std::vector <wxCheckBox *> checkbox_timed_data;
		std::vector <wxStaticText *> static_layer;
		wxCheckBox *check_read_divergence, *check_read_network, *check_read_facilities, *check_read_usgs, *check_read_storet_legacy, *check_read_storet_modern, *check_other_facilities;
		wxStaticText *static_divergence, *static_network, *static_facilities, *static_facilities_other, *static_usgs, *static_storet_legacy, *static_storet_modern;

	protected:
		int layer_count;
		bool *read_layers, *read_timed_data;
		dynamic_string *layer_names, *layer_paths;
		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
