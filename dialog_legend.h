
class panel_color_legend : public wxPanel {
	DECLARE_EVENT_TABLE()
	public:
		class dynamic_map *map;
		class color_set *colors;

		panel_color_legend ();
		panel_color_legend
			(wxWindow *,
			const wxWindowID id,
			// class color_set *,
			const wxPoint &position,
			const wxSize &size);
		void OnPaint
			(wxPaintEvent&);
	protected:
		void draw_map_colors ();
};


class dialog_legend : public wxDialog {
	public:
		enum {
			ID_MAP = 103,
			COMBO_SELECT_LAYER
		};
		dialog_legend
			(class wxWindow *,
			class dynamic_map *,
			class map_layer *);

		class wxStaticText *static_layer_name;
		class wxScrolled <panel_color_legend> *color_panel;
		// class panel_color_legend *color_panel;

	protected:
		class dynamic_map *map;
		class map_layer *selection_layer;
		class color_set *colors;
		int color_height;

		void show_colors ();

		virtual void EndModal
			(int retCode);

	DECLARE_EVENT_TABLE()
};
