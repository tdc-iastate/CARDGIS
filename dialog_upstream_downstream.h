class dialog_upstream_downstream : public wxDialog {
	// Layer
	public:
		enum {
			BUTTON_BROWSE_SHAPEFILE = 101,
			CHECK_FLIP_LATITUDE,
			EDIT_CENTRAL_MERIDIAN,
			CHECK_HAS_NAME,
			CHECK_HAS_ID,
			RADIO_GIVEN_LAYER_COLUMN,
			RADIO_GIVEN_LAYER_NAME,
			EDIT_LAYER_COLUMN,
			EDIT_LAYER_NAME,
			COMBO_SELECT_LAYER,
			STATIC_OUTPUT_FILENAME
		};

		dialog_upstream_downstream
			(wxWindow *pParent,
			class dynamic_map *,
			bool *p_write_output,
			dynamic_string *p_filename_out,
			bool *p_save_to_layer,
			class map_layer **p_map_layer);

		class wxComboBox *combo_layer;
		wxCheckBox *check_write_output, *check_add_to_layer;
		wxStaticText *static_output_file;
		wxButton *button_browse_output_file;

		void OnBrowseOutputFile (wxCommandEvent &);
		void OnIdcancel (wxCommandEvent &);
		void OnSelChangePreset
			(wxCommandEvent &);

	protected:
		class dynamic_map *map;
		dynamic_string *filename_output;
		map_layer **save_map_layer;
		bool *write_output, *save_to_layer;

		void enable_windows ();

		virtual void EndModal
			(int retCode);

		void fill_layer_combo
			(class wxComboBox *combo,
			const map_layer *selected);

		DECLARE_EVENT_TABLE()
};

