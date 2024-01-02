
class dialog_map_layers : public wxDialog {
	public:
		enum {
			STATIC_FILENAME = 103,
			ID_EDIT_PATH,
			ID_LIST_FILES,
			BUTTON_ROYAL_GORGE,
			BUTTON_AMES,
			BUTTON_RACCOON,
			BUTTON_SELECTION_BOUNDS
		};
		dialog_map_layers
			(class wxWindow *,
			class dynamic_map *p_map);
		void OnSetRoyalGorge (wxCommandEvent &);
		void OnSetAmes (wxCommandEvent &);
		void OnSetRaccoon (wxCommandEvent &);

		std::vector <wxComboBox *> combo_render;
		std::vector <wxCheckBox *> checkbox_layer_visible;
		std::vector <wxStaticText *> static_layer;
		wxButton *button_royal_gorge, *button_ames, *button_raccoon;

	protected:
		class dynamic_map *map;

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
