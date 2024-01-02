 
class window_legend : public wxWindow {
	DECLARE_EVENT_TABLE()
 
	public:
		enum {
			WINDOW_LEGEND = 10001
		};
		window_legend
			(wxWindow* parent,
			class color_set_histogram *,
			double *e_max,
			wxWindowID id = WINDOW_LEGEND,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxSize (100, 25));
 
		virtual ~window_legend();
 

		void OnSize( wxSizeEvent& event );
		void OnPaint( wxPaintEvent& event );
		void OnEraseBackground( wxEraseEvent& event );
	protected:
		class color_set_histogram *colors;
		double *elevation_max;

		void draw_legend_text
			(wxDC &dc,
			const int left,
			const int bottom,
			const int segment_height);
		void draw_text_entry
			(wxDC &dc,
			const int left,
			const int top,
			class color_set_definition *color_range);
};
 
