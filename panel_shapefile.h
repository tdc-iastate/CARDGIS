
wxDECLARE_EVENT (MESSAGE_PANEL_SHAPEFILE_SELECT, wxCommandEvent);
wxDECLARE_EVENT (MESSAGE_PANEL_MEASURE_DISTANCE, wxCommandEvent); // ClientData contains allocated device_coordinate that must be deleted

// used by this and parent class if it responds to these on right-click
const int wxID_CLOSEST_SEGMENT = 495;
const int wxID_SELECT_UP = 496;
const int wxID_SELECT_DOWN = 497;
const int wxID_LIST_UP = 498;
const int wxID_LIST_DOWN = 499;
const int MENU_ADD_SELECTION_TO_LAYER = 500;

const int PANEL_SHAPEFILE_MODE_RESTING = 0;
const int PANEL_SHAPEFILE_MODE_ZOOM = 1;
const int PANEL_SHAPEFILE_MODE_DISTANCE = 2;

class panel_shapefile : public wxPanel {
	public:
		enum {
			wxID_DESCRIBE_SELECTION = 501
		};
		int pending_mode;
		bool draw_point_indexes, use_gl;
		int refresh_count, update_count, redraw_count, render_count, paint_count;
		dynamic_string redraw_source;

		device_coordinate recent_click; // Set by measure_distance.  Stores click position
		
		bool pause_rendering;
		class map_layer *layer_selection_source, *layer_selection;
		panel_shapefile
			(class wxWindow *,
			class dynamic_map *,
			class wxStatusBar *,
			const wxSize &size);
		virtual ~panel_shapefile ();

		void OnContext (wxContextMenuEvent &);
		void OnLeftUp (wxMouseEvent &);
		void OnDescribeSelection
			(wxMenuEvent &);

		void change_selection_layer
			(class map_layer *new_selection_target);

		virtual void redraw
			(const dynamic_string &);
		virtual void Refresh ();
		virtual void Update ();
		void change_layers ();
	    void OnPaint(wxPaintEvent &);

		bool add_to_selection
			(const long long id,
			dynamic_string &log);
		void erase_selection
			(class interface_window *window);

		bool draw_bitmap
			(wxImage *,
			const wxSize &image_size,
			dynamic_string &);

		bool select_id
			(const long long id);

		bool replace_selection
			(std::set <long long> *,
			dynamic_string &log);
		void start_zoom ();
		void start_measure ();
		void zoom_out ();
		void zoom_in
			(const device_coordinate &device_point);
		void measure_distance
			(const device_coordinate &);
		void zoom_selection ();

	protected:
		class dynamic_map *map;
		map_scale scale_screen;
		bounding_cube zoomed_logical_extent;
		wxStatusBar *window_status_display;
		class interface_window *gl_interface_window;

		class interface_window *create_interface
			(const bool with_dc,
			const bool paint_context = false);
		void cleanup_dc (class interface_window *);

		void create_selection_layer
			(const class map_layer *source_layer);

		bool make_dib_image
			(wxBitmap &bitmap,
			wxRect &box,
			dynamic_string &error_message);

		void render (class interface_window *);
		void selection_layer_attributes ();

		DECLARE_EVENT_TABLE()
};

