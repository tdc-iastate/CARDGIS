
class dialog_river : public wxDialog {
	public:
		class dynamic_map *map;
		bool use_gl;
		map_layer *streams, *other_streams, *layer_point_from, *layer_point_to;

		enum {
			ID_MAP = 103,
			COMBO_SELECT_LAYER,
			ID_STATIC_MAP_STATUS
		};
		dialog_river
			(class wxWindow *,
			class dynamic_map *,
			const bool opengl,
			class flow_network *,
			class color_set *);
		virtual ~dialog_river ();

		void create_map_layers
			(const class map_layer *layer_from,
			const class map_layer *layer_to);
		void describe_all
			(const bool upstream,
			dynamic_string &);

		void OnSelectLayer
			(wxCommandEvent &);

		class panel_shapefile *panel_river;
		class wxStaticText *static_map_status;
		class wxComboBox *combo_select_layer;

		void set_map ();
	protected:
		class flow_network *river_network;
		class color_set *colors;
		// class interface_window_wx *status_window; // encapsulated pointer to static_map_status
		class dynamic_map *frame_map;

		void describe_segment
			(class map_object *line,
			double *stream_length_km,
			dynamic_string &log);
		void describe_segment_points
			(class map_object *line,
			const int nearest_point_index,
			double *stream_length_km,
			dynamic_string &log);

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
