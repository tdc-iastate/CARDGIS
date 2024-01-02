#include "../util/utility_afx.h"
#include <vector>
#include <map>
#include <set>
#include "../util/dynamic_string.h"
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "window_legend.h"
#include "../map/color_set.h"
#include "color_set_histogram.h"
 
BEGIN_EVENT_TABLE( window_legend, wxWindow )
	EVT_SIZE( window_legend::OnSize )
	EVT_PAINT( window_legend::OnPaint )
	EVT_ERASE_BACKGROUND( window_legend::OnEraseBackground )
END_EVENT_TABLE()
 
 
window_legend::window_legend
	(wxWindow* parent,
	color_set_histogram *p_colors,
	double *e_max,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size)
	: wxWindow (parent, id, pos, size)
{
	colors = p_colors;
	elevation_max = e_max;
}
 
window_legend::~window_legend()
{
}

void window_legend::OnPaint
	(wxPaintEvent&)
{
	wxBufferedPaintDC dc(this);
	std::vector <color_set_definition>::const_iterator color_range;
	wxRect clientRect = GetClientRect(), color_rect, histogram_rect;
	int y, range_count, effective_bottom;
	double ratio;
	int segment_height;
	wxBrush brush;
	wxPen pen;
	long denominator, max_count;

	dc.DrawRectangle (clientRect);

	color_rect = clientRect;
	color_rect.SetWidth (35);
	histogram_rect = clientRect;
	histogram_rect.SetLeft (histogram_rect.GetRight () - 55);
	histogram_rect.SetWidth (50);

	denominator = colors->total_count (&max_count);

	if (colors->colors.size () > 0) {
		// segment_width = color_rect.GetWidth () / colors->size ();
		if (*elevation_max != 0.0) {
			color_range = colors->colors.begin ();
			ratio = color_rect.GetHeight () / (*elevation_max - color_range->ge);
		}
		else
			ratio = color_rect.GetHeight () / colors->total_range ();

		// colors are set by lidar.prescan_color_range, called from panel_LIDAR_image.set_position
		color_range = colors->colors.begin ();
		segment_height = (color_rect.GetBottom () - color_rect.GetTop ()) / colors->colors.size ();
		effective_bottom = color_rect.GetTop () + segment_height * colors->colors.size ();
		y = effective_bottom;
		range_count = 0;
		while (color_range != colors->colors.end ()) {

			// ratio is wrong if min & max override the color range ge or lt
			// segment_height = (int) ((color_range->lt - color_range->ge) * ratio + 0.5);

			brush.SetColour (color_range->color);
			pen.SetColour (color_range->color);
			dc.SetBrush (brush);
			dc.SetPen (pen);
			dc.DrawRectangle (color_rect.GetLeft (), y - segment_height, color_rect.GetWidth (), y);
			if (color_range->count > 0) {
				dc.DrawRectangle (histogram_rect.GetLeft (),
				y - segment_height,
				(color_range->count * (long) histogram_rect.GetWidth ()) / max_count,
				segment_height);
			}
			dc.SetPen(wxNullPen);
			dc.SetBrush (wxNullBrush);

			++range_count;
			y -= segment_height;
			++color_range;
		}
		draw_legend_text (dc, color_rect.GetWidth () + 4, effective_bottom, segment_height);
	}
}
 
void window_legend::draw_text_entry
	(wxDC &dc,
	const int left,
	const int top,
	color_set_definition *color_range)

{
	dynamic_string entry;

	// entry.format ("%.1lfm <=x< %.1lfm", color_range->ge, color_range->lt);
	entry.format ("%.1lfm..%.1lfm", color_range->ge, color_range->lt);
	dc.DrawText (entry.get_text_ascii (), left, top);
}

void window_legend::draw_legend_text
	(wxDC &dc,
	const int left,
	const int bottom,
	const int segment_height)
{
	std::vector <color_set_definition>::const_iterator color_range;
	int segment_base, y, index, color_count;
	std::set <int> drawn;

	wxFont label_font (9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	dc.SetFont (label_font);
	dc.SetTextForeground (RGB (0, 0, 0));
	dc.SetTextBackground (RGB (255, 255, 255));
	dc.SetBackgroundMode (wxBG_STYLE_TRANSPARENT);

	wxCoord xExtent, yExtent;
	dc.GetTextExtent ("Ag", &xExtent, &yExtent);

	color_count = colors->colors.size ();
	drawn.insert (0);
	drawn.insert (color_count / 4);
	drawn.insert (color_count / 2);
	drawn.insert ((color_count * 3) / 4);
	drawn.insert (color_count - 1);

	color_range = colors->colors.begin ();
	index = 0;
	segment_base = bottom;
	while (color_range != colors->colors.end ()) {

		// ratio is not relevent if color_range has been overridden by min/max
		// set y by # of segments
		// segment_height = (int) ((color_range->lt - color_range->ge) * ratio + 0.5);

		if (drawn.find (index) != drawn.end ()) {
			if (index == 0)
				// First color, bottom, lowest value
				y = bottom - yExtent;
			else
				if (index == color_count - 1)
					// Last color, top, highest value
					y = segment_base - segment_height + yExtent;
				else
					y = segment_base - segment_height;

			draw_text_entry (dc, left, y, (color_set_definition *) &*color_range);
		}
		segment_base -= segment_height;

		++index;
		++color_range;
	}
}
 
void window_legend::OnEraseBackground
	( wxEraseEvent& )
{
}
 
void window_legend::OnSize( wxSizeEvent&)
{
Refresh();
}
