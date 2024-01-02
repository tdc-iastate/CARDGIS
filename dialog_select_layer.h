
class dialog_select_layer : public wxDialog {
	public:
		enum {
			ID_MAP = 103,
			COMBO_SELECT_LAYER
		};
		dialog_select_layer
			(class wxWindow *,
			const wxString& prompt,
			class dynamic_map *,
			class map_layer **);

		class wxComboBox *combo_layer;

	protected:
		class dynamic_map *map;
		class map_layer **selection;

		void fill_layer_combo
			(class wxComboBox *combo,
			const map_layer *selected);

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
