
class dialog_river_segment : public wxDialog {
	public:
		enum {
			ID_FILENAME_OUTPUT_VARIABLES = 103,
			ID_BUTTON_FILENAME_OUTPUT_VARIABLES,
			ID_EDIT_PATH,
			ID_LIST_FILES
		};
		dialog_river_segment
			(class wxWindow *,
			class dynamic_map *p_map,
			class map_layer **p_layer_from,
			class map_layer **p_layer_to,
			long long *facility,
			long long *station);

		long_long_integer_edit *edit_facility, *edit_usgs_station;
		wxStaticText *static_station_layer;
		class wxComboBox *combo_start_layer, *combo_end_layer;

	protected:
		long long *facility_id, *station_id;
		class dynamic_map *map;
		class map_layer **layer_from, **layer_to;

		void fill_layer_combo
			(class wxComboBox *combo,
			const class map_layer *selected);

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
