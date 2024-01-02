
// Aaron Thune

//A container for holding our wxGLCanvas
class panel_shapefile_gl : public panel_shapefile {

	public:
		//Constructor
		panel_shapefile_gl(wxWindow* parent, class dynamic_map *map, class interface_window* intf_window, const wxSize& size = wxDefaultSize, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
						   long style = 0L, const wxString& name = L"GLContainer");
		//Destructor
		virtual ~panel_shapefile_gl();

		typedef panel_shapefile super;

		void create_selection_layer(const class map_layer *source_layer);
		void render(interface_window* intf_window);
		void redraw(const dynamic_string& source);
		bool replace_selection(std::set <long>* upstream_ids, dynamic_string &log);

		void SetCurrent(wxKeyEvent& event);

	protected:
		//Instance of our wxGLCanvas, our canvas that can interface with OpenGL
		mapview_gl* gl_canvas;

		void OnPaint(wxPaintEvent& event);
		void OnLeftUp(wxMouseEvent& event);


		//Defines all events and handler functions for those events in this window
		DECLARE_EVENT_TABLE();
};