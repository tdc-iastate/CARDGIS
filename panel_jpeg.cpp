#include "..\util\utility_afx.h"
#include <wx/wx.h>
#include <wx/sizer.h>
#include "panel_jpeg.h"

BEGIN_EVENT_TABLE(panel_jpeg, wxPanel)
// some useful events
/*
 EVT_MOTION(panel_jpeg::mouseMoved)
 EVT_LEFT_DOWN(panel_jpeg::mouseDown)
 EVT_LEFT_UP(panel_jpeg::mouseReleased)
 EVT_RIGHT_DOWN(panel_jpeg::rightClick)
 EVT_LEAVE_WINDOW(panel_jpeg::mouseLeftWindow)
 EVT_KEY_DOWN(panel_jpeg::keyPressed)
 EVT_KEY_UP(panel_jpeg::keyReleased)
 EVT_MOUSEWHEEL(panel_jpeg::mouseWheelMoved)
 */
 
// catch paint events
EVT_PAINT(panel_jpeg::paintEvent)
//Size event
EVT_SIZE(panel_jpeg::OnSize)
END_EVENT_TABLE()
 
 
// some useful events
/*
 void panel_jpeg::mouseMoved(wxMouseEvent& event) {}
 void panel_jpeg::mouseDown(wxMouseEvent& event) {}
 void panel_jpeg::mouseWheelMoved(wxMouseEvent& event) {}
 void panel_jpeg::mouseReleased(wxMouseEvent& event) {}
 void panel_jpeg::rightClick(wxMouseEvent& event) {}
 void panel_jpeg::mouseLeftWindow(wxMouseEvent& event) {}
 void panel_jpeg::keyPressed(wxKeyEvent& event) {}
 void panel_jpeg::keyReleased(wxKeyEvent& event) {}
 */
 
panel_jpeg::panel_jpeg
	(wxWindow * parent)
	: wxPanel (parent, wxID_ANY, wxDefaultPosition, wxSize (300, 400))
{
	image = NULL;
    w = -1;
    h = -1;
}

void panel_jpeg::set_image
	(class wxImage *p_image)

{
    image = p_image;
}

 
/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
 
void panel_jpeg::paintEvent(wxPaintEvent &)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}
 
/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void panel_jpeg::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}
 
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void panel_jpeg::render(wxDC&  dc)
{
	if (image) {
		int neww, newh;
		dc.GetSize( &neww, &newh );
 
		/*
		if( neww != w || newh != h ) {
		*/
			resized = wxBitmap( image->Scale( neww, newh /*, wxIMAGE_QUALITY_HIGH*/ ) );
			w = neww;
			h = newh;
			dc.DrawBitmap( resized, 0, 0, false );
			/*
		}
		else {
			dc.DrawBitmap( resized, 0, 0, false );
		}
		*/
	}
}
 
/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void panel_jpeg::OnSize(wxSizeEvent& event){
    Refresh();
    //skip the event.
    event.Skip();
}
 