#include "../util/utility_afx.h"
#include <vector>
#include <mutex>
#include <set>
#include <deque>
#include <map>
#include <list>
#include <thread>
#include <future> // Requires "-std=c++11" option for g++
#include <sys/timeb.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../util/dynamic_string.h"
#include "../util/Timestamp.h"
#include "../util/message_slot.h"
#include "../util/interface_window.h"
#include "../util/interface_window_wx.h"
#include "../util/dialog_error_list.h"
#include "../util/device_coordinate.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_field_set.h"
#include <wx/wx.h>
#include "wx/app.h"
#include "wx/frame.h"
#include <wx/panel.h>
#include <wx/stattext.h>
#include "wx/menu.h"
#include "wx/layout.h"
#include "wx/msgdlg.h"
#include "wx/icon.h"
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/textctrl.h"
#include "wx/settings.h"
#include <wx/grid.h>
#include <wx/listbox.h>
#include <wx/glcanvas.h>
#include "../util/interface_window_gl.h"
// #include "../clipper_ver6.4.2/cpp/clipper.hpp"
//#include "ClipperBuffer.h"
#include "timed_measurements.h"

#ifdef wxHAS_NATIVE_CALENDARCTRL
    #include "wx/generic/calctrlg.h" // wxmsw29ud_adv.lib
#endif

#include "panel_shapefile.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

wxDEFINE_EVENT (MESSAGE_PANEL_SHAPEFILE_SELECT, wxCommandEvent); // Selected id passed via wxCommandEvent.SetString
wxDEFINE_EVENT (MESSAGE_PANEL_MEASURE_DISTANCE, wxCommandEvent);

BEGIN_EVENT_TABLE(panel_shapefile, wxPanel)
	EVT_LEFT_UP(panel_shapefile::OnLeftUp)
	EVT_PAINT(panel_shapefile::OnPaint)
	EVT_CONTEXT_MENU (panel_shapefile::OnContext)
END_EVENT_TABLE()

panel_shapefile::panel_shapefile
	(wxWindow *p_parent,
	dynamic_map *p_map,
	wxStatusBar *p_status_display,
	const wxSize &size)
	: wxPanel (p_parent, wxID_ANY, wxDefaultPosition, size)
{

	map = p_map;
	layer_selection_source = NULL;
	layer_selection = NULL;
	window_status_display = p_status_display;
	use_gl = false;

	refresh_count = update_count = redraw_count = paint_count = render_count = 0;

	pause_rendering = false;
	pending_mode = PANEL_SHAPEFILE_MODE_RESTING;
	draw_point_indexes = false;

	Connect (wxID_DESCRIBE_SELECTION, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) &panel_shapefile::OnDescribeSelection);


	// Static box around entire panel
	/*
	wxBoxSizer *row;
	wxStaticBoxSizer *stack = new wxStaticBoxSizer (wxVERTICAL, this, "Threads");

	// wxStaticBoxSizer *stack = new wxStaticBoxSizer (wxVERTICAL, this, server->url.get_text ());

	// wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );
	row = new wxBoxSizer( wxHORIZONTAL );
	row->AddStretchSpacer (1);
	stack->Add (row, 1, wxEXPAND | wxALIGN_LEFT | wxALIGN_RIGHT);

    stack->SetSizeHints( this );
    SetSizer(stack);
	*/
	gl_interface_window = NULL;
}

panel_shapefile::~panel_shapefile ()

{
	if (gl_interface_window)
		delete gl_interface_window;
}

void panel_shapefile::OnPaint
	(wxPaintEvent &pe)

{
	++paint_count;
	if (!pause_rendering) {
		if (use_gl) {

			if (gl_interface_window == NULL) {
				gl_interface_window = create_interface (true, true);
			}
			if (((interface_window_gl *) gl_interface_window)->glcanvas) {
				//The color buffer is probably the only one that needs cleared, but 
				glClear (GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
				render (gl_interface_window);

				//Switches the active buffer with the back buffer
				((interface_window_gl *) gl_interface_window)->glcanvas->SwapBuffers ();
			}
			cleanup_dc (gl_interface_window);


			// 2019-08-28 Calling cleanup_gl here stops the render before it appears on screen.  Must be a background process
			// delete gl_interface_window;
			// gl_interface_window = NULL;
			// ((interface_window_gl *) gl_interface_window)->cleanup_gl ();
		}
		else {
			interface_window *view = create_interface (true, true);
			render (view);
			cleanup_dc (view);
			delete view;
		}
	}
}

bool panel_shapefile::add_to_selection
	(const long long id,
	dynamic_string &log)

{
	map_object *polygon, *selection_copy;

	if ((polygon = layer_selection_source->match_id (id)) != NULL) {
		if (layer_selection == NULL)
			create_selection_layer (layer_selection_source);

		selection_copy = layer_selection_source->create_new (polygon->type);
		selection_copy->copy (polygon, layer_selection);

		layer_selection->objects.push_back (selection_copy);

		return true;
	}
	else {
		log.add_formatted ("ERROR, id %lld not found in selection layer.\n", id);
		return false;
	}
}

interface_window *panel_shapefile::create_interface
	(const bool with_dc,
	const bool paint_context)

{
	interface_window *window;
	if (use_gl) {
		window = new interface_window_gl;
		if (with_dc) {
			((interface_window_gl *) window)->setup_gl (this);
			if (((interface_window_gl *) window)->glcanvas) {
				if (!((interface_window_gl *) window)->glcanvas->initialized) {
					((interface_window_gl *) window)->init_canvas (this);
					((interface_window_gl *) window)->init_fonts (12);
				}
			}
			// else
			// UDP/TCP remote desktop issue?
		}
	}
	else {
		window = new interface_window_wx;
		if (with_dc) {
			if (paint_context) {
				wxPaintDC* dc = new wxPaintDC(this);
				((interface_window_wx*)window)->dc = dc;
			}
			else {
				wxClientDC *dc = new wxClientDC (this);
				((interface_window_wx *) window)->dc = dc;
			}
		}
	}

	return window;
}

void panel_shapefile::cleanup_dc
	(interface_window *window)

{
	if (use_gl) {
		// ~interface_window_gl takes care of this
		// ((interface_window_gl *) window)->cleanup_gl ();
	}
	else
		delete ((interface_window_wx *) window)->dc;
}

bool panel_shapefile::select_id
	(const long long id)

{
	map_object *polygon, *selection_copy;
	dynamic_string status;

	if ((polygon = layer_selection_source->match_id (id)) != NULL) {

		if (layer_selection == NULL)
			create_selection_layer (layer_selection_source);

		status.format ("Selected %lld ", polygon->id);
		status += polygon->name;
		if (window_status_display)
			window_status_display->SetStatusText (status.get_text ());

		if (use_gl) {
			if (gl_interface_window == NULL) {
				gl_interface_window = create_interface (true);
				//The color buffer is probably the only one that needs cleared, but 
				glClear (GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
			}

			//Switches the active buffer with the back buffer
			((interface_window_gl *) gl_interface_window)->glcanvas->SwapBuffers ();

			erase_selection (gl_interface_window);

			layer_selection->clear_objects ();
			switch (polygon->type) {
				case MAP_OBJECT_POINT:
					layer_selection->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
					layer_selection->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
					break;
				case MAP_OBJECT_POLYGON:
					// case LAYER_TYPE_POLYLINE:
					layer_selection->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
					break;
				case MAP_OBJECT_POLYGON_COMPLEX:
					layer_selection->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			}

			selection_copy = layer_selection_source->create_new (polygon->type);
			selection_copy->copy (polygon, layer_selection);
			layer_selection->objects.push_back (selection_copy);
			layer_selection->draw (gl_interface_window, &scale_screen);

			//Switches the active buffer with the back buffer
			((interface_window_gl *) gl_interface_window)->glcanvas->SwapBuffers ();

			cleanup_dc (gl_interface_window);
		}
		else {
			interface_window *view = create_interface (true);
			erase_selection (view);
			layer_selection->clear_objects ();
			switch (polygon->type) {
				case MAP_OBJECT_POINT:
					layer_selection->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
					layer_selection->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
					break;
				case MAP_OBJECT_POLYGON:
					layer_selection->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
					break;
				case MAP_OBJECT_POLYGON_COMPLEX:
					layer_selection->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			}

			selection_copy = layer_selection_source->create_new (polygon->type);
			selection_copy->copy (polygon, layer_selection);
			layer_selection->objects.push_back (selection_copy);
			layer_selection->draw (view, &scale_screen);
			cleanup_dc (view);
			delete view;
		}
		return true;
	}
	else
		return false;
}

void panel_shapefile::erase_selection
	(interface_window *window)

{
	// Find & draw each polyline in layer_selection_source corresponding to those in layer_selection
	std::vector<map_object *>::const_iterator highway;
	bounding_cube patch;

	if (layer_selection) {

		// Redraw selection area, all layers
		for (highway = layer_selection->objects.begin ();
		highway != layer_selection->objects.end ();
		++highway)
			(*highway)->check_extent (&patch);

		// selection layer should not be drawn
		layer_selection->visible = false;
		map->render_patch (window, &patch, NULL); // layer_selection_source->colors);
		layer_selection->visible = true;
	}
}

void panel_shapefile::create_selection_layer
	(const map_layer *source_layer)

// When user clicks, a copy of selected map_object is added to layer_selection

{
	// Add Selection layer last to draw on top
	layer_selection_source = (map_layer *) source_layer;

	layer_selection = map->create_new (layer_selection_source->type);

	layer_selection->name = "Selection";
	layer_selection->color = RGB (255, 0, 0);
	layer_selection->include_in_legend = false;

	if (layer_selection->type == MAP_OBJECT_POINT) {
		layer_selection->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		layer_selection->symbol = layer_selection_source->symbol; // MAP_OBJECT_SYMBOL_PLUS;
	}
	else {
		layer_selection->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	}


	selection_layer_attributes ();

	map->layers.push_back (layer_selection);
}

void panel_shapefile::OnLeftUp
	(wxMouseEvent &mouse_event)

{
	// wxPoint device_point = ScreenToClient (mouse_event.GetPosition ());
	wxPoint device_point = mouse_event.GetPosition ();
	device_coordinate logical_point;
	map_object *polygon, *selection_copy;
	long distance;
	int keys = mouse_event.GetModifiers ();
	dynamic_string position_text;

	position_text.format ("%d,%d", device_point.x, device_point.y);

	logical_point.x = (long) map->scale->logical_x (device_point.x);
	logical_point.y = (long) map->scale->logical_y (device_point.y);
	position_text.add_formatted (" -> %d,%d", logical_point.x, logical_point.y);

	if (pending_mode == PANEL_SHAPEFILE_MODE_ZOOM)
		zoom_in (device_coordinate (device_point.x, device_point.y));
	else
		if (pending_mode == PANEL_SHAPEFILE_MODE_DISTANCE)
			measure_distance (logical_point);
		else {
			bounding_cube current_view = zoomed_logical_extent;
			current_view.deflate (0.25); // in case we're zoomed out

			if (layer_selection_source
			&& (layer_selection_source != layer_selection)) {
				if (keys && wxMOD_SHIFT)
					// slower but more accurate
					polygon = layer_selection_source->find_layer_object_surrounding (logical_point, &current_view);
				else
					polygon = layer_selection_source->find_nearest_object_fast (logical_point, &distance);
				if (polygon != NULL) {

					if (keys && wxMOD_SHIFT)
						position_text.add_formatted (" Selected (%d) %lld ", keys, polygon->id);
					else
						position_text.add_formatted (" Selected %lld ", polygon->id);
					position_text += polygon->name;
					if (window_status_display)
						window_status_display->SetStatusText (position_text.get_text ());

					// redraw river_layer's copy of selection
					if (use_gl) {
						if (gl_interface_window == NULL) {
							gl_interface_window = create_interface (true);
							//The color buffer is probably the only one that needs cleared, but 
							glClear (GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
						}

						//Switches the active buffer with the back buffer
						((interface_window_gl *) gl_interface_window)->glcanvas->SwapBuffers ();

						if (layer_selection == NULL)
							create_selection_layer (layer_selection_source);
						else {
							erase_selection (gl_interface_window);
							layer_selection->clear_objects ();
							selection_layer_attributes ();
						}

						selection_copy = layer_selection_source->create_new (polygon->type);
						selection_copy->copy (polygon, layer_selection);

						layer_selection->objects.push_back (selection_copy);

						layer_selection->draw (gl_interface_window, &scale_screen);

						//Switches the active buffer with the back buffer
						((interface_window_gl *) gl_interface_window)->glcanvas->SwapBuffers ();

						cleanup_dc (gl_interface_window);
					}
					else {
						interface_window *view = create_interface (true);
						if (layer_selection == NULL)
							create_selection_layer (layer_selection_source);
						else {
							erase_selection (view);
							layer_selection->clear_objects ();
							selection_layer_attributes ();
						}

						selection_copy = layer_selection_source->create_new (polygon->type);
						selection_copy->copy (polygon, layer_selection);

						layer_selection->objects.push_back (selection_copy);

						layer_selection->draw (view, &scale_screen);
						cleanup_dc (view);
						delete view;
					}

					// Send a message to frame so it can display information in toolbar about selection
					wxCommandEvent ev_update (MESSAGE_PANEL_SHAPEFILE_SELECT);
					// ev_update.SetClientObject ((wxClientData *) &string);
					// ev_update.SetExtraLong (selection_copy->id);
					position_text.format ("%lld", selection_copy->id);
					ev_update.SetString (position_text.get_text ());
					GetParent ()->ProcessWindowEvent (ev_update);
					// wxTheApp->QueueEvent (ev_update);
				}
				else {
					if (layer_selection_source) {
						position_text += " no object match in layer \"";
						position_text += layer_selection_source->name;
						position_text += "\"";
					}
					else
						position_text += " no selection layer.";

					if (window_status_display)
						window_status_display->SetStatusText (position_text.get_text ());
				}
			}
			else {
				if (layer_selection_source) {
					position_text += " Selection layer invalid \"";
					position_text += layer_selection_source->name;
					position_text += "\"";
				}
				else
					position_text += " no selection layer.";
			}
		}
	// 2023-01-27 Don't overwrite status written during zoom_in, etc
	// if (window_status_display)
	// window_status_display->SetStatusText (position_text.get_text ());
}

void panel_shapefile::OnContext
	(wxContextMenuEvent &mouse_event)

{
	wxPoint device_point = mouse_event.GetPosition ();
	wxPoint p = ScreenToClient (device_point);
	dynamic_string position;
	
	// position.format ("Lat=%.8lf Lon=%.8lf", scale_screen.logical_y (device_point.y), scale_screen.logical_x (device_point.x));
	position.format ("Lat=%.8lf Lon=%.8lf", scale_screen.logical_y (p.y), scale_screen.logical_x (p.x));

	if (window_status_display)
		window_status_display->SetStatusText (position.get_text ());

	if (layer_selection_source == NULL)
		layer_selection_source = map->layers [0];

	if (layer_selection == NULL)
		create_selection_layer (layer_selection_source);

	if (layer_selection->objects.size () > 0) {
		wxMenu clickMenu;

		clickMenu.Append(wxID_DESCRIBE_SELECTION, wxT("&Describe"));
		clickMenu.Append(wxID_LIST_UP , wxT("&List Upstream"));
		clickMenu.Append(wxID_LIST_DOWN, wxT("&List Downstream"));
		clickMenu.Append(wxID_SELECT_UP, wxT("&Select Upstream"));
		clickMenu.Append(wxID_SELECT_DOWN, wxT("&Select Downstream"));
		clickMenu.Append(MENU_ADD_SELECTION_TO_LAYER, wxT("Add Selection &To Layer"));
		// clickMenu.Append(wxID_LIST_UP , wxT("&List Upstream"));

		PopupMenu (&clickMenu, p);
	}
}

void panel_shapefile::redraw
	(const dynamic_string &source)

{
	++redraw_count;
	redraw_source = source;
	// Refresh ();
	// UpdateWindow (GetHWND ());

	// GetWindowRect returns screen coordinates, i.e. top=87 (height of menu & tool bars)
	// GetClientRect returns client coordinates, i.e. top = 0
	// 
	// wxRect rect;
	// GetWindowRect(GetHWND(), &rect);
	// 
	// RECT rect;
	// ::GetClientRect (GetHWND(), &rect);
	InvalidateRect(GetHWND(), NULL, true);

	// 2023-01-23 InvalidRect erases the screen but doesn't cause any dynamic_map rendering
	// Update does cause Paint to be called

	Update ();
}

void panel_shapefile::Refresh()

// Note that repainting doesn't happen immediately but only during the next event loop iteration, if you need to update the window immediately you should use Update() 

{
	++refresh_count;
	wxPanel::Refresh();
}

void panel_shapefile::Update()

// Note that repainting doesn't happen immediately but only during the next event loop iteration, if you need to update the window immediately you should use Update() 

{
	++update_count;
	wxPanel::Update ();
}

void panel_shapefile::change_layers ()

// Call this instead of Refresh if map layers change

{
	zoomed_logical_extent = map->logical_extent;
	layer_selection_source = NULL; // (map_layer *) map->first_member ();
	if ((layer_selection = map->match ("Selection")) != NULL)
		layer_selection->clear_objects ();
	if (window_status_display)
		window_status_display->SetStatusText ("change_layers");
}

void panel_shapefile::render
	(interface_window *window)

// events are shown in rows, 1 row = 1 line of text

{
	wxPoint pane_size;
	wxRect panel;
	BYTE old_draw_as = 0;
	// double logical_border = zoomed_logical_extent.extent_y () / 20.0;

	if (window_status_display) {
		dynamic_string entry;

		++render_count;
		entry.format ("Refresh %d update %d redraw %d \"", refresh_count, update_count, redraw_count);
		entry += redraw_source;
		entry.add_formatted ("\" paint %d render %d", paint_count, render_count);
		redraw_source = "Direct";
		window_status_display->SetStatusText (entry.get_text ());
	}

	GetSize (&pane_size.x, &pane_size.y);
	panel.SetBottomRight (pane_size);

	scale_screen.set (window, panel, &zoomed_logical_extent, 0.0);

	// reconciled_logical_extent may differ some from zoomed_logical_extent
	// 2023-07-05 Setting zoomed_logical_extent = scale_screen.reconciled_logical_extent
	// causes small but troublesome position shift in RLE on CLRLUM's world map
	// zoomed_logical_extent = scale_screen.reconciled_logical_extent;

	map->scale = &scale_screen;
	// map->draw (&window, error_message);
	if (draw_point_indexes && layer_selection) {
		old_draw_as = layer_selection->draw_as;
		layer_selection->draw_as = MAP_OBJECT_DRAW_NODE_LABEL;
	}

	map->render_patch (window, &zoomed_logical_extent);

	if (draw_point_indexes && layer_selection)
		layer_selection->draw_as = old_draw_as;
}

void panel_shapefile::start_zoom ()

{
	pending_mode = PANEL_SHAPEFILE_MODE_ZOOM;
	SetCursor (*wxCROSS_CURSOR);
}

void panel_shapefile::start_measure ()

{
	pending_mode = PANEL_SHAPEFILE_MODE_DISTANCE;
	SetCursor (*wxCROSS_CURSOR);
}

void panel_shapefile::measure_distance
	(const device_coordinate &logical_point)

// 2021-05-04 Compute distance from (nearest point of) selected object to this click point

{
	device_coordinate *message_point;
	wxCommandEvent ev_update (MESSAGE_PANEL_MEASURE_DISTANCE);
	pending_mode = PANEL_SHAPEFILE_MODE_RESTING;
	SetCursor (*wxSTANDARD_CURSOR);

	// Send a message to frame so it can compute distance
	// Don't compute here because geolib, etc would have to be included here

	message_point = new device_coordinate;
	message_point->copy (logical_point);

	// ev_update.SetExtraLong (selection_copy->id);
	// ev_update.SetClientObject ((wxClientData *) &string);
	ev_update.SetClientData (message_point);
	GetParent ()->ProcessWindowEvent (ev_update);
	// wxTheApp->QueueEvent (ev_update);
}

void panel_shapefile::zoom_in
	(const device_coordinate &device_coordinate)

// 2023-01-27 Perform the zoom in device coordinates, then convert to logical

{
	bounding_cube original_zle = zoomed_logical_extent;
	device_rectangle new_device_frame;
	long width, height;

	pending_mode = PANEL_SHAPEFILE_MODE_RESTING;

	SetCursor(*wxSTANDARD_CURSOR);

	width = map->scale->device_extent.width () / 4;
	height = map->scale->device_extent.height () / 4;

	new_device_frame.left = device_coordinate.x - width / 2;
	new_device_frame.right = new_device_frame.left + width;
	new_device_frame.top = device_coordinate.y - height / 2;
	new_device_frame.bottom = new_device_frame.top + height;

	zoomed_logical_extent.x[0] = map->scale->logical_x(new_device_frame.left);
	zoomed_logical_extent.x[1] = map->scale->logical_x(new_device_frame.right);
	zoomed_logical_extent.y[0] = map->scale->logical_y(new_device_frame.top);
	zoomed_logical_extent.y[1] = map->scale->logical_y(new_device_frame.bottom);

	dynamic_string entry;
	entry.format("zoom in %.3lf..%.3lf %.3lf..%.3lf -> %.3lf..%.3lf %.3lf..%.3lf",
	original_zle.x[0] / 1.0e6, original_zle.x[1] / 1.0e6,
	original_zle.y[0] / 1.0e6, original_zle.y[1] / 1.0e6,
	zoomed_logical_extent.x[0] / 1.0e6, zoomed_logical_extent.x[1] / 1.0e6,
	zoomed_logical_extent.y[0] / 1.0e6, zoomed_logical_extent.y[1] / 1.0e6);
	redraw(entry);
}

void panel_shapefile::zoom_out ()

{
	wxPoint pane_size;
	wxPoint scaled_point;
	double width, height, logical_aspect_ratio;
	bounding_cube original_zle = zoomed_logical_extent;
	bool limit_to_map_extent = false;

	pending_mode = PANEL_SHAPEFILE_MODE_RESTING;
	draw_point_indexes = false;

	SetCursor (*wxSTANDARD_CURSOR);

	device_coordinate logical_center;
	logical_center.x = (zoomed_logical_extent.extent_x () / 2.0) + zoomed_logical_extent.x [0];
	logical_center.y = (zoomed_logical_extent.extent_y () / 2.0) + zoomed_logical_extent.y [0];
	logical_aspect_ratio = zoomed_logical_extent.extent_x() / zoomed_logical_extent.extent_y();

	// proportional expansion of width/height (of window, not current zoom-in)
	// GetSize (&pane_size.x, &pane_size.y);
	// aspect_ratio = (double) pane_size.x / (double) pane_size.y;

	// Double zoomed logical extent width
	width = zoomed_logical_extent.extent_x () * 2.0;
	// expand height in proportion so that we wind up matching aspect ratio of window
	// But logical height is distorted by lat/long
	// height = zoomed_logical_extent.extent_y () * 2.0 * (logical_aspect_ratio / aspect_ratio);
	// height = width / aspect_ratio;
	height = width / logical_aspect_ratio;

	if (limit_to_map_extent) {
		zoomed_logical_extent.x [0] = (double) logical_center.x - (width / 2.0);
		if (zoomed_logical_extent.x [0] < map->logical_extent.x [0]) {
			// hitting left boundary
			zoomed_logical_extent.x [0] = map->logical_extent.x [0];
			zoomed_logical_extent.x [1] = (double) logical_center.x + width;
			if (zoomed_logical_extent.x [1] > map->logical_extent.x [1])
				zoomed_logical_extent.x [1] = map->logical_extent.x [1];
		}
		else {
			zoomed_logical_extent.x [1] = zoomed_logical_extent.x [0] + width;
			if (zoomed_logical_extent.x [1] > map->logical_extent.x [1]) {
				// hitting right boundary
				zoomed_logical_extent.x [1] = map->logical_extent.x [1];
				zoomed_logical_extent.x [0] = zoomed_logical_extent.x [1] - height;
				if (zoomed_logical_extent.x [0] < map->logical_extent.x [0])
					zoomed_logical_extent.x [0] = map->logical_extent.x [0];
			}
		}

		zoomed_logical_extent.y [0] = (double) logical_center.y - (height / 2.0);
		if (zoomed_logical_extent.y [0] < map->logical_extent.y [0]) {
			// hitting top boundary
			zoomed_logical_extent.y [0] = map->logical_extent.y [0];
			zoomed_logical_extent.y [1] = zoomed_logical_extent.y [0] + height;
			if (zoomed_logical_extent.y [1] > map->logical_extent.y [1])
				zoomed_logical_extent.y [1] = map->logical_extent.y [1];
		}
		else {
			zoomed_logical_extent.y [1] = zoomed_logical_extent.y [0] + height;
			if (zoomed_logical_extent.y [1] > map->logical_extent.y [1]) {
				zoomed_logical_extent.y [1] = map->logical_extent.y [1];
				zoomed_logical_extent.y [0] = zoomed_logical_extent.y [1] - height;
				if (zoomed_logical_extent.y [0] < map->logical_extent.y [0])
					zoomed_logical_extent.y [0] = map->logical_extent.y [0];
			}
		}
	}
	else {
		zoomed_logical_extent.x[0] = (double)logical_center.x - (width / 2.0);
		zoomed_logical_extent.x[1] = zoomed_logical_extent.x[0] + width;
		zoomed_logical_extent.y[0] = (double)logical_center.y - (height / 2.0);
		zoomed_logical_extent.y[1] = zoomed_logical_extent.y[0] + height;
	}
	
	dynamic_string entry;
	entry.format ("zoom out ratio %.3lf %.3lfx%.3lf -> %.3lfx%.3lf", logical_aspect_ratio,
	(original_zle.x[1] - original_zle.x[0]) / 1.0e6,
	(original_zle.y[1] - original_zle.y[0]) / 1.0e6,
	(zoomed_logical_extent.x [1] - zoomed_logical_extent.x [0]) / 1.0e6,
	(zoomed_logical_extent.y [1] - zoomed_logical_extent.y [0]) / 1.0e6);
	redraw (entry);
}

void panel_shapefile::zoom_selection ()

{
	wxPoint scaled_point;
	bounding_cube selection_extent;
	std::vector <map_object *>::const_iterator segment;

	if (layer_selection) {
		pending_mode = PANEL_SHAPEFILE_MODE_RESTING;

		SetCursor (*wxSTANDARD_CURSOR);

		for (segment = layer_selection->objects.begin ();
		segment != layer_selection->objects.end ();
		++segment)
			(*segment)->check_extent (&selection_extent);

		if (selection_extent.x [1] == selection_extent.x [0]) {
			// selected single facility or station
			selection_extent.x [0] -= 4000.0;
			selection_extent.x [1] += 4000.0;
			selection_extent.y [0] -= 4000.0;
			selection_extent.y [1] += 4000.0;
		}
		else
			// expand selection slightly
			selection_extent.inflate (1.05);

		if (layer_selection->objects.size () == 1)
			// single object may be polygon
			draw_point_indexes = true;
		else
			draw_point_indexes = false;

		zoomed_logical_extent = selection_extent;
	
		redraw ("zoom_selection");
	}
}

void panel_shapefile::OnDescribeSelection
	(wxMenuEvent &)

{
	dynamic_string log;
	std::vector <map_object *>::const_iterator segment;
	std::vector <dynamic_string>::const_iterator column_label;
	bounding_cube polygon_area;
	int index;

	log += layer_selection_source->name;
	log += "\n";

	for (segment = layer_selection->objects.begin ();
	segment != layer_selection->objects.end ();
	++segment) {

		for (index = 0, column_label = layer_selection->column_names_numeric.begin ();
		column_label != layer_selection->column_names_numeric.end ();
		++column_label, ++index) {
			log += *column_label;
			log += "=";
			log.add_formatted ("%.8lf", (*segment)->attributes_numeric [index]);

			if (index == layer_selection->data_attribute_index)
				log += " *";

			log += "\n";
		}
		
		for (index = 0, column_label = layer_selection->column_names_text.begin ();
		column_label != layer_selection->column_names_text.end ();
		++column_label, ++index) {
			log += *column_label;
			log += "=\"";
			log += (*segment)->attributes_text [index];
			log += "\"\n";
		}
		
		/*}
		if (layer_selection->type != MAP_OBJECT_POINT) {
			// River
			if (layer_selection->attribute_count_numeric == NHD_SEGMENT_ATTRIBUTE_COUNT) {
				log.add_formatted ("FType=%d\n", (int) (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE]);
				log.add_formatted ("Length km=%.3lf\n", (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM]);
				log.add_formatted ("Divergence=%d\n", (int) (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE]);
				log.add_formatted ("Level Path=%ld\n", (long) (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH]);
				log.add_formatted ("Stream Level=%d\n", (int) (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL]);
				log.add_formatted ("Strahler Order=%d\n", (int) (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER]);
				log.add_formatted ("UpHydroseq=%ld\n", (long) (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_UPHYDROSEQ]);
				log.add_formatted ("DnHydroseq=%ld\n", (long) (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ]);
			}
		}
		else {
			// Points
			if (layer_selection->attribute_count_numeric == LINKED_POINT_ATTRIBUTE_COUNT) {
				log.add_formatted ("COMID=%ld\n", (long) (*segment)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID]);
				log.add_formatted ("Polygon index=%d\n", (int) (*segment)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX]);
				log.add_formatted ("Point index=%d\n", (int) (*segment)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX]);
				log.add_formatted ("Divergence=%d\n", (int) (*segment)->attributes_numeric [USGS_ATTRIBUTES_INDEX_DIVERGENCE]);
				log.add_formatted ("Distance, m=%.3lf\n", (*segment)->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M]);
			}
		}
		*/


		log += "\n";

		(*segment)->dump (layer_selection, log);

		if ((layer_selection_source->attributes_void_usage == MAP_LAYER_USAGE_VOID_TIMED_MEASUREMENTS)
		&& (*segment)->attributes_void)
			((timed_measurements *) ((*segment)->attributes_void))->describe (log);

		{
			color_set_definition *c;
			if (layer_selection_source->data_attribute_index >= 0)
				if ((c = layer_selection_source->colors.match_range ((*segment)->attributes_numeric [layer_selection_source->data_attribute_index])) != NULL) {
					log += "\nColor\t";
					log += c->label;
					log += "\n\n";
				}

		}

		// compute length for comparison

		if (((*segment)->type == MAP_OBJECT_VECTOR)
		|| ((*segment)->type == MAP_OBJECT_POLYGON)
		|| ((*segment)->type == MAP_OBJECT_POLYGON_COMPLEX)) {
			(*segment)->write_length_km (log, true, 0, -1, -1);

			// 2022-03-04 Don't use ClipperLib here; wxMap doesn't have it
			// To add area to description, call out to local replacement for this function

			/*
				// compute area
				ClipperLib::Paths trail;
				std::vector < ClipperLib::Path >::const_iterator paths_path;
				ClipperBuffer clipper;
				double object_area_m2 = 0.0;

				clipper.map_object_to_Path (*segment, &trail, log);

				for (paths_path = trail.begin ();
				paths_path != trail.end ();
				++paths_path)
					object_area_m2 += area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_GEODESIC);

				log.add_formatted ("Area\t%.8lf\tm2\n", object_area_m2);
			*/

		}
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"River", L"", log);
	d.ShowWindowModal ();
}

void panel_shapefile::change_selection_layer
	(map_layer *new_selection_target)

{
	layer_selection_source = new_selection_target;

	if (layer_selection) {
		layer_selection->clear_objects ();
		selection_layer_attributes ();
	}
	//Refresh ();
}

void panel_shapefile::selection_layer_attributes ()

// Change layer_selection to mirror attributes of layer_selection_source

// Name remains "Selection" and color remains red

{
	layer_selection->type = layer_selection_source->type;
	layer_selection->enclosure = layer_selection_source->enclosure;
	layer_selection->line_width = layer_selection_source->line_width;
	layer_selection->symbol_size = layer_selection_source->symbol_size;
	layer_selection->attribute_count_numeric = layer_selection_source->attribute_count_numeric;
	layer_selection->attribute_count_text = layer_selection_source->attribute_count_text;
	layer_selection->attributes_void_usage = layer_selection_source->attributes_void_usage;
	layer_selection->column_names_numeric = layer_selection_source->column_names_numeric;
	layer_selection->column_names_text = layer_selection_source->column_names_text;
	layer_selection->data_attribute_index = layer_selection_source->data_attribute_index;
}

bool panel_shapefile::replace_selection
	(std::set <long long> *upstream_ids,
	dynamic_string &log)

{
	std::set <long long>::const_iterator id;
	bool error = false;

	if (layer_selection_source) {
		dynamic_string status;

		if (layer_selection == NULL)
			create_selection_layer (layer_selection_source);

		if (use_gl) {
			if (gl_interface_window == NULL) {
				gl_interface_window = create_interface (true);
				//The color buffer is probably the only one that needs cleared, but 
				glClear (GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
			}

			//Switches the active buffer with the back buffer
			((interface_window_gl *) gl_interface_window)->glcanvas->SwapBuffers ();

			// Can't add to existing selection, otherwise we have to figure out which item in selection we're going dowstream from
			erase_selection (gl_interface_window);

			layer_selection->clear_objects ();

			status = "Selected";
			for (id = upstream_ids->begin ();
			id != upstream_ids->end ();
			++id)
				if (!add_to_selection (*id, log))
					error = true;
				else
					status.add_formatted (" %lld", *id);

			if (window_status_display) {
				status += " in layer \"";
				status += layer_selection_source->name;
				status += "\".";
				if (error)
					status += " (ERROR)";
				window_status_display->SetStatusText (status.get_text ());
			}

			layer_selection->draw (gl_interface_window, &scale_screen);
			//Switches the active buffer with the back buffer
			((interface_window_gl *) gl_interface_window)->glcanvas->SwapBuffers ();
			cleanup_dc (gl_interface_window);
		}
		else {
			interface_window *view = create_interface (true);
			erase_selection (view);
			layer_selection->clear_objects ();
			status = "Selected";
			for (id = upstream_ids->begin ();
			id != upstream_ids->end ();
			++id)
				if (!add_to_selection (*id, log))
					error = true;
				else
					status.add_formatted (" %lld", *id);

			if (window_status_display) {
				status += " in layer \"";
				status += layer_selection_source->name;
				status += "\".";
				if (error)
					status += " (ERROR)";
				window_status_display->SetStatusText (status.get_text ());
			}

			layer_selection->draw (view, &scale_screen);
			cleanup_dc (view);
			delete view;

		}
	}
	else {
		error = true;
		log += "ERROR, no designated selection layer.\n";
	}

	return !error;
}

bool panel_shapefile::make_dib_image
	(wxBitmap &bitmap,
	wxRect &box,
	dynamic_string &error_message)

{
	wxMemoryDC memory_dc;
	bool error = false;

	// memory device context
	memory_dc.SelectObject (bitmap);

	// Fill with white background
	memory_dc.SetBrush (*wxWHITE_BRUSH);
	memory_dc.DrawRectangle (box);

	interface_window_wx view;
	map_scale scale_box;

	view.dc = &memory_dc;

	scale_box.set (NULL, box, &map->logical_extent, 0, SHAPEFILE_PROJECTED_LAT_LONG);

	map->scale = &scale_box;

	map->draw (&view, error_message);
	map->draw_legend (&view, error_message);

	// add_label (&memory_dc, &draw_box);
	memory_dc.SelectObject (wxNullBitmap);

	map->scale = &scale_screen;

	return !error;
}

bool panel_shapefile::draw_bitmap
	(wxImage *png_image,
	const wxSize &image_size,
	dynamic_string &error_message)

// Called from page_prevalence for saving to file

{
	wxBitmap bitmap;
	wxRect box (0, 0, image_size.GetWidth () - 1, image_size.GetHeight () - 1);
	bool error = false;

	if (bitmap.Create (box.GetWidth (), box.GetHeight ())) {
		if (make_dib_image (bitmap, box, error_message))
			*png_image = bitmap.ConvertToImage();
		else
			error = true;

	}
	else {
		error = true;
		error_message += "ERROR, bitmap create failed.\n";
	}

	return !error;
}