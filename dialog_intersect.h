
class dialog_intersect : public wxDialog {
	public:
		enum {
			ID_MAP = 103,
			COMBO_SELECT_LAYER_1,
			COMBO_SELECT_LAYER_2,
			BUTTON_NYS
		};
		dialog_intersect
			(class wxWindow *,
			class dynamic_map *,
			class map_layer**,
			class map_layer**);

		void NewYork (wxCommandEvent&);

		class wxComboBox *combo_layer_1, *combo_layer_2;

	protected:
		class dynamic_map *map;
		class map_layer **selection_1, **selection_2;

		void fill_layer_combo
			(class wxComboBox *combo,
			const map_layer *selected);

		virtual void EndModal
			(int retCode);

		class map_layer* ag_sale_circles(dynamic_string& log);
		class map_layer* urban_areas(dynamic_string& log);

	DECLARE_EVENT_TABLE()
};
