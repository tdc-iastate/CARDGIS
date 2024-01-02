
class dialog_edit_layer : public wxDialog {
	public:
		enum {
			ID_MAP = 103,
			COMBO_SELECT_TYPE,
			BUTTON_RIVER_FTYPE,
			BUTTON_RIVER_LENGTH,
			BUTTON_RIVER_STREAMLEVEL,
			BUTTON_RIVER_MAJOR_ONLY,
			BUTTON_RIVER_DIVERGENCE,
			BUTTON_RIVER_LEVEL_PATH,
			BUTTON_RIVER_STRAHLER_ORDER,
			BUTTON_RIVER_UPHYDROSEQ,
			BUTTON_RIVER_DNHYDROSEQ,
			BUTTON_RIVER_ELEVATIONMIN,
			BUTTON_RIVER_ELEVATIONMAX,
			BUTTON_SUBBASIN_BACKGROUND,
			BUTTON_AUTORANGE,
			BUTTON_INTEGER_SCALE,
			BUTTON_SECCHI
		};
		dialog_edit_layer
			(class wxWindow *,
			class dynamic_map *,
			class map_layer *);

		class wxComboBox *combo_layer_type;
		class wxComboBox *combo_draw_as, *combo_draw_symbol;
		class wxListBox *list_column_names_numeric, *list_column_names_text;
		class wxTextCtrl *edit_layer_name, *edit_title;
		// class wxStaticText *static_object_count;
		// class integer_edit *edit_numeric_attribute_count, *edit_text_attribute_count;
		class wxCheckBox *check_show_labels_id, *check_show_labels_name, *check_separate_zero;
		class wxScrolled <panel_color_legend> *color_panel;
		class wxColourPickerCtrl *control_color;

		void OnRiverFTYPE
			(wxCommandEvent &);
		void OnRiverLength 
			(wxCommandEvent &);
		void OnRiverStreamLevel
			(wxCommandEvent &);
		void OnRiverMajorOnly
			(wxCommandEvent &);
		void OnAutoRange
			(wxCommandEvent &);
		void OnSubbasinBackground
			(wxCommandEvent &);
		void OnIntegerScale
			(wxCommandEvent &);
		void OnSecchiDepth
			(wxCommandEvent &);
		void OnRiverDivergence (wxCommandEvent &);
		void OnRiverLevelPath (wxCommandEvent &);
		void OnRiverStrahlerOrder (wxCommandEvent &);
		void OnRiverUpHydroSeq (wxCommandEvent &);
		void OnRiverDnHydroSeq (wxCommandEvent &);
		void OnRiverElevationMin (wxCommandEvent &);
		void OnRiverElevationMax (wxCommandEvent &);

	protected:
		class dynamic_map *map;
		class map_layer *layer;
		std::map <int, int> draw_as_combo_indexes;

		void show_column_names ();

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
