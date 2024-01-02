#include "../util/utility_afx.h"
#include <vector>
#include <set>
#include <map>
#include <chrono>
#include <wx/stattext.h>
#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/filedlg.h>
#include <wx/clipbrd.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/listbox.h>
#include <wx/app.h>
#include <wx/button.h>
#include "../util/dynamic_string.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/filename_struct.h"
#include "../util/listbox_files.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "dialog_legend.h"


BEGIN_EVENT_TABLE( panel_color_legend, wxPanel )
	EVT_PAINT( panel_color_legend::OnPaint )
END_EVENT_TABLE()

panel_color_legend::panel_color_legend ()
	: wxPanel ()

{
	map = NULL;
	colors = NULL;
}

panel_color_legend::panel_color_legend
	(wxWindow *parent,
	const wxWindowID id,
	// class color_set *p_colors,
	const wxPoint &position,
	const wxSize &size)
	: wxPanel (parent, id, position, size)

{
	map = NULL;
	colors = NULL;
}


void panel_color_legend::draw_map_colors ()

{
	std::vector <color_set_definition>::const_iterator color;
	int x = 5, y = 0, index = 0;
	wxPaintDC dc (this);
	dynamic_string entry;
	wxPen pen;
	wxBrush brush;
	wxFont label_font (9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	dc.SetFont (label_font);
	dc.SetTextForeground (RGB (0, 0, 0));
	// dc.SetTextBackground (RGB (255, 255, 255));
	dc.SetBackgroundMode (wxBG_STYLE_TRANSPARENT);

	std::vector <map_layer*>::const_iterator layer;

	for (layer = map->layers.begin();
	layer != map->layers.end();
	++layer) {
		dc.DrawText ((*layer)->name.get_text_ascii (), x, y + 3);
		y += 25;

		switch ((*layer)->draw_as) {
			case MAP_OBJECT_DRAW_FILL_LAYER_COLOR:
			case MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR:
			case MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR:
				pen.SetColour ((*layer)->color);
				dc.SetPen (pen);
				brush.SetColour ((*layer)->color);
				dc.SetBrush (brush);
				dc.DrawRectangle (x + 25, y, 75, 24);

				entry = (*layer)->name;
				dc.DrawText (entry.get_text_ascii (), x + 105, y + 3);
				y += 30;
				break;

			case MAP_OBJECT_DRAW_FILL_DATA_COLOR:
			case MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR:
			case MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR:
			case MAP_OBJECT_DRAW_SYMBOL_FILL_AND_OUTLINE:
				for (color = (*layer)->colors.colors.begin ();
				color != (*layer)->colors.colors.end ();
				++color) {
					pen.SetColour (color->color);
					dc.SetPen (pen);
					brush.SetColour (color->color);
					dc.SetBrush (brush);
					dc.DrawRectangle (x + 25, y, 75, 24);

					entry.format ("%.3lf <= x < %.3lf", color->ge, color->lt);
					if (color->label.get_length () > 0) {
						entry += " : ";
						entry += color->label;
					}
					dc.DrawText (entry.get_text_ascii (), x + 105, y + 3);

					++index;
					y += 30;
				}
				/*
				break;
			default:
				MAP_OBJECT_DRAW_FILL_AND_OUTLINE = 3; // outline is layer color, fill colors from attribute_numeric [attribute_index]
				MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR_MODULO = 9; // Attribute is an integer, select color using modulo function, with map_layer.draw_as_modulo for divisor
				MAP_OBJECT_DRAW_PIE_CHARTS = 10; // pie chart from attributes_numeric
				MAP_OBJECT_DRAW_NODE_LABEL = 11; // uses MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR for color
				MAP_OBJECT_DRAW_SYMBOL_SIZE_FUNCTION = 12; // Symbol size determined by function passed to map_layer.size_function
				*/
		}
	}
}

void panel_color_legend::OnPaint
	(wxPaintEvent &)
{
	if (map)
		draw_map_colors ();
	else {
		// Colors for a single layer
		std::vector <color_set_definition>::const_iterator color;
		int y = 0, index = 0;
		wxPaintDC dc (this);
		dynamic_string entry;
		wxPen pen;
		wxBrush brush;
		wxFont label_font (9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		dc.SetFont (label_font);
		dc.SetTextForeground (RGB (0, 0, 0));
		// dc.SetTextBackground (RGB (255, 255, 255));
		dc.SetBackgroundMode (wxBG_STYLE_TRANSPARENT);

		for (color = colors->colors.begin ();
		color != colors->colors.end ();
		++color) {
			pen.SetColour (color->color);
			dc.SetPen (pen);
			brush.SetColour (color->color);
			dc.SetBrush (brush);
			dc.DrawRectangle (5, y, 75, 24);

			entry.format ("%.3lf <= x < %.3lf", color->ge, color->lt);
			if (color->label.get_length () > 0) {
				entry += " : ";
				entry += color->label;
			}
			dc.DrawText (entry.get_text_ascii (), 85, y + 3);

			++index;
			y += 30;
		}
	}
}

dialog_legend::dialog_legend
	(wxWindow *pParent,
	dynamic_map *p_map,
	map_layer *p_selection)
	: wxDialog (pParent, -1, "Color Legend")
{
	std::vector <map_layer*>::const_iterator layer;
	map = p_map;
	selection_layer = p_selection;

	color_height = 0;

	for (layer = map->layers.begin();
	layer != map->layers.end();
	++layer) {
		color_height += 25; // One line for name of layer
		switch ((*layer)->draw_as) {
			case MAP_OBJECT_DRAW_FILL_LAYER_COLOR:
			case MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR:
			case MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR:
				color_height += 30;
				break;

			case MAP_OBJECT_DRAW_FILL_DATA_COLOR:
			case MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR:
			case MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR:
			case MAP_OBJECT_DRAW_SYMBOL_FILL_AND_OUTLINE:
				color_height += (*layer)->colors.colors.size () * 30;
				/*
				break;
			default:
				MAP_OBJECT_DRAW_FILL_AND_OUTLINE = 3; // outline is layer color, fill colors from attribute_numeric [attribute_index]
				MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR_MODULO = 9; // Attribute is an integer, select color using modulo function, with map_layer.draw_as_modulo for divisor
				MAP_OBJECT_DRAW_PIE_CHARTS = 10; // pie chart from attributes_numeric
				MAP_OBJECT_DRAW_NODE_LABEL = 11; // uses MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR for color
				MAP_OBJECT_DRAW_SYMBOL_SIZE_FUNCTION = 12; // Symbol size determined by function passed to map_layer.size_function
				*/
		}
	}
	if (color_height < 300)
		color_height = 600;

	/*
	if (((colors = layer->colors) != NULL)
	&& (colors->colors.size () > 0)) {
		color_height = colors->colors.size () * 30;
		if (color_height < 300)
			color_height = 600;
	}
	else
		color_height = 600;
	*/

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	static_layer_name = new wxStaticText (this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize (500, 24));
	stack->Add (static_layer_name);

	stack->AddSpacer (15);
	// color_panel = new wxScrolled <wxPanel> (this, wxID_ANY, wxDefaultPosition, wxSize (500, 300));
	color_panel = new wxScrolled <panel_color_legend> (this, wxID_ANY, wxDefaultPosition, wxSize (500, 600));
	color_panel->map = map;
	color_panel->SetVirtualSize (wxSize (500, color_height));
	// color_panel->SetScrollRate (0, (color_height - 300) / 10);
	stack->Add (color_panel);
	stack->AddSpacer (15);
	stack->Add (CreateButtonSizer (wxOK));

	SetSizerAndFit(stack);

	if (selection_layer)
		static_layer_name->SetLabel (selection_layer->name.get_text ());

	// show_colors ();
}

BEGIN_EVENT_TABLE(dialog_legend, wxDialog)
END_EVENT_TABLE()

void dialog_legend::show_colors ()

{
	color_panel->Refresh ();
}

void dialog_legend::EndModal
	(int retCode)

{
	wxDialog::EndModal (retCode);
}


