// http://wiki.wxwidgets.org/An_image_panel
 
class panel_jpeg : public wxPanel
{
	public:
		wxImage *image;
		wxBitmap resized;
		int w, h;
 
		panel_jpeg
			(wxWindow* parent);
		void set_image
			(class wxImage *);
 
		void paintEvent(wxPaintEvent & evt);
		void paintNow();
		void OnSize(wxSizeEvent& event);
		void render(wxDC& dc);
 
    // some useful events
    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */
 
    DECLARE_EVENT_TABLE()
};
