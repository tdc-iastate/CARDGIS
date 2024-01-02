
class dialog_read_TIGER : public wxDialog {
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
		dialog_read_TIGER
			(class wxWindow *,
			dynamic_string *p_path,
			class bounding_cube *p_current_extent,
			std::vector <dynamic_string> *street_types,
			bool *open_street_type);

		std::vector <wxCheckBox *> checkbox_layer;
		wxButton *button_ok, *button_edit_path;
		wxStaticText *static_divergence, *static_network, *static_facilities, *static_facilities_other, *static_usgs, *static_storet_legacy, *static_storet_modern;

	protected:
		// int layer_count;
		bool *read_street_type;
		dynamic_string *tiger_path;
		std::vector <dynamic_string> *street_type_names;
		class bounding_cube *current_extent;

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
