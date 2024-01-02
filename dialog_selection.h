
class dialog_selection : public wxDialog {
	public:
		class dynamic_map map;
		map_layer *layer_selection;

		enum {
			ID_MAP = 103,
			COMBO_SELECT_LAYER,
			ID_STATIC_MAP_STATUS
		};
		dialog_selection
			(class wxWindow *,
			class flow_network *);

		void create_selection_layer
			(const class map_layer *);
		class map_layer *create_map_layer
			(const class map_layer *);

		void OnSelectLayer
			(wxCommandEvent &);

		class panel_shapefile *panel_river;
		class wxStaticText *static_map_status;
		class wxComboBox *combo_select_layer;

		void set_map ();
	protected:
		class flow_network *river_network;
		// class interface_window_wx *status_window; // encapsulated pointer to static_map_status

		void describe_segment
			(class map_polygon *line,
			double *stream_length_km,
			dynamic_string &log);

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
