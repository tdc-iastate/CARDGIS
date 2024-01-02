#include "../util/utility_afx.h"
#include <set>
#include <mutex>
#include "../util/device_coordinate.h"
#include "../util/dynamic_string.h"
#include "../util/dialog_error_list.h"
#include "../util/interface_window.h"
#include "../util/interface_window_gl.h"
#include "../util/list.h"
#include <wx/panel.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <wx/glcanvas.h>
#include "../map/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/mapview_gl.h"
#include "../map/dynamic_map_gl.h"
#include "../map/map_scale.h"
#include "../map/color_set.h"
#include "panel_shapefile.h"
#include "panel_shapefile_gl.h"

BEGIN_EVENT_TABLE(panel_shapefile_gl, panel_shapefile)
	//EVT_KEY_DOWN(panel_shapefile_gl::SetCurrent)
END_EVENT_TABLE()

panel_shapefile_gl::panel_shapefile_gl(wxWindow* parent, class dynamic_map* map, class interface_window* intf_window, const wxSize& size, wxWindowID id, const wxPoint& pos, long style, const wxString& name) 
									: panel_shapefile(parent, map, intf_window, size)
{
	//OpenGL display attributes
	wxGLAttributes dispAttrs;
	dispAttrs.PlatformDefaults().RGBA().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(0).EndList(); //Setting attributes of our GL window

	bool displaySupported = wxGLCanvas::IsDisplaySupported(dispAttrs);

	if (displaySupported)
	{
		//Our canvas that this class is serving as a container for
		this->gl_canvas = new mapview_gl(this, dispAttrs, size);
	}

	// tdc 2018-11-09
	// ((interface_window_gl *)this->intf_window)->gldc = new mapview_gl_context(gl_canvas);
	((interface_window_gl *) window_status_display)->gldc = new mapview_gl_context(gl_canvas);
}

panel_shapefile_gl::~panel_shapefile_gl()
{
	delete gl_canvas;
}

void panel_shapefile_gl::create_selection_layer(const class map_layer *source_layer)
{
	//Add Selection layer last to draw on top
	layer_selection_source = (map_layer *)source_layer;

	if (layer_selection != NULL) {
		map->remove(layer_selection);
	}

	layer_selection = new map_layer_gl;
	layer_selection->name = "Selection";
	layer_selection->color = RGB(255, 0, 0);

	if (layer_selection->type == MAP_OBJECT_POINT)
	{
		layer_selection->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		layer_selection->symbol = layer_selection_source->symbol; // MAP_OBJECT_SYMBOL_PLUS;
	}
	else
		layer_selection->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;

	selection_layer_attributes();

	map->add(layer_selection);
}

void panel_shapefile_gl::render(interface_window* intf_window)
{
	gl_canvas->clear_buffers();

	super::render(window_status_display->gldc);

	gl_canvas->swap_buffers();

	//Displays report of map scale
	//
	//#ifdef _DEBUG
	//	dynamic_string scale_report;
	//	map->scale->write (scale_report);
	//
	//	wxClientDC dc (this);
	//	wxFont report_font (10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	//	dc.SetFont (report_font);
	//	dc.DrawText (scale_report.get_text_ascii (), 100, 100);
	//#endif

}

void panel_shapefile_gl::redraw(const dynamic_string& source)
{
	++redraw_count;
	redraw_source = source;
	render(this->intf_window);
}

bool panel_shapefile_gl::replace_selection(std::set <long>* upstream_ids, dynamic_string &log)
{
	bool error = super::replace_selection(upstream_ids, log);

	redraw("Replace selection");

	return error;
}

void panel_shapefile_gl::OnPaint(wxPaintEvent & event)
{
	if (event.GetEventObject()->IsSameAs(*gl_canvas))
	{
		if (!(gl_canvas->initialized))
		{
			gl_canvas->init_canvas();

			((interface_window_gl *)this->intf_window)->gldc->init_fonts(12);

			gl_canvas->initialized = true;
		}
		else
		{
			redraw("Paint Event");
		}
	}
}

void panel_shapefile_gl::OnLeftUp(wxMouseEvent &event)
{
	super::OnLeftUp(event);

	redraw("Left Click");
}

void panel_shapefile_gl::SetCurrent(wxKeyEvent& event)
{
	gl_canvas->SetCurrent(*((interface_window_gl *)this->intf_window)->gldc);
}