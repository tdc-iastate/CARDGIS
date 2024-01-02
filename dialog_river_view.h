
class dialog_river_view : public wxDialog {
	public:
		enum {
			STATIC_FILENAME = 103,
			ID_EDIT_PATH,
			ID_LIST_FILES,
			BUTTON_ROYAL_GORGE,
			BUTTON_AMES,
			BUTTON_RACCOON,
			BUTTON_E85,
			BUTTON_SELECTION_BOUNDS
		};
		dialog_river_view
			(class wxWindow *,
			dynamic_string *filename,
			class dynamic_map *p_map,
			bool *p_gridfloat_3dep,
			bool *p_boundary_box,
			class river_view_map *);
		void OnBrowseOutputFile (wxCommandEvent &);
		void OnSetRoyalGorge (wxCommandEvent &);
		void OnSetAmes (wxCommandEvent &);
		void OnSetRaccoon (wxCommandEvent &);
		void OnSetE85 (wxCommandEvent &);
		void OnSetSelectionBounds (wxCommandEvent &);

		long_integer_edit *edit_facility, *edit_usgs_station;
		double_edit *edit_boundary_west, *edit_boundary_east, *edit_boundary_north, *edit_boundary_south, *edit_stream_amplification, *edit_station_amplification;
		wxCheckBox *check_boundary, *check_simplify, *check_gridfloat;
		wxComboBox *combo_area_definition;
		std::vector <wxCheckBox *> checkbox_layer;
		std::vector <wxStaticText *> static_layer;
		wxButton *button_browse_output_file, *button_royal_gorge, *button_ames, *button_raccoon, *button_selection_bounds, *button_e85;
		wxStaticText *static_output_file;
		class edit_time_point *edit_start_date, *edit_end_date;

	protected:
		dynamic_string *filename_dae;
		bool *boundary_box, *gridfloat_3dep;
		class river_view_map *river_view;
		class dynamic_map *map;
		std::vector <bool> *read_timed_data;

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
