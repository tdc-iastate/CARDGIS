	
class dialog_distance : public wxDialog {
	public:
		enum {
			ID_FILENAME_OUTPUT_VARIABLES = 103,
			BUTTON_FIND_START,
			BUTTON_FIND_DESTINATION,
			BUTTON_COMPUTE
		};
		dialog_distance
			(class wxWindow *,
			class map_layer* p_selected_layer,
			class map_layer* p_layer_selection_source,
			class dynamic_map *p_map);

		void OnFindStart
			(wxCommandEvent&);
		void OnFindDestination
			(wxCommandEvent&);
		void OnCompute
			(wxCommandEvent&);

		double_edit * static_distance_km, * static_distance_miles;
		long_integer_edit *edit_start_id, *edit_destination_id;
		class wxComboBox *combo_start_layer, *combo_destination_layer;

	protected:
		class map_layer *selection_layer, *layer_selection_source, *start_layer, *destination_layer;
		class dynamic_map* map;
		void fill_layer_combo
			(wxComboBox* combo,
			class map_layer *current_choice = NULL);

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
