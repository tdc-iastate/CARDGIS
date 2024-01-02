#include "../util/utility_afx.h"
#include <vector>
#include <set>
#include <map>
#include <mutex>
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
#include "../util/interface_window_wx.h"
#include "../util/filename_struct.h"
#include "../util/listbox_files.h"
#include "../util/device_coordinate.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "dialog_selection.h"
#include "panel_shapefile.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

dialog_selection::dialog_selection
	(wxWindow *pParent,
	flow_network *p_river_network)
	: wxDialog (pParent, -1, "River")
{
	river_network = p_river_network;
	layer_selection = NULL;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer * row = new wxBoxSizer (wxHORIZONTAL);

	static_map_status = new wxStaticText (this, ID_STATIC_MAP_STATUS, L"Status:", wxDefaultPosition, wxSize (300, 24));
	row->Add (static_map_status);
	row->AddSpacer (20);

	wxString items [3] = {"NHD Flowlines", "Treatment Facilities", "USGS Sample Stations"};
	combo_select_layer = new wxComboBox (this, COMBO_SELECT_LAYER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, items, wxCB_DROPDOWN);
	combo_select_layer->SetSelection (0);
	row->Add (combo_select_layer);

	row->AddSpacer (20);
	row->Add (new wxButton (this, wxOK, "Close", wxDefaultPosition, wxDefaultSize));

	stack->Add (row);
	stack->AddSpacer (10);

	// status_window = new interface_window_wx;
	// status_window->setup_wx_status_control (static_map_status);

	panel_river = new panel_shapefile (this, &map, NULL, wxSize (1000, 1000));
	stack->Add (panel_river);

	// stack->AddSpacer (25);
	// stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);
}

BEGIN_EVENT_TABLE(dialog_selection, wxDialog)
	EVT_COMBOBOX(COMBO_SELECT_LAYER, dialog_selection::OnSelectLayer)
END_EVENT_TABLE()

void dialog_selection::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
	}
	wxDialog::EndModal (retCode);
}


void dialog_selection::set_map ()

// Calling function has created map layers

{
	map.set_extent ();

	// map.logical_extent.inflate (1.01);
	panel_river->change_layers ();
	panel_river->Refresh ();
}

void dialog_selection::OnSelectLayer
	(wxCommandEvent &)

{
	// int index = combo_select_layer->GetSelection ();
	// 2022-05-05 combo_select_layer was not filled in anyway
	// panel_river->change_selection_layer ((map_layer *) map.find (index));
}

void dialog_selection::create_selection_layer
	(const map_layer *layer_from)

// called from frame_nhd_view

// Selection goes into layer "Selection"

{
	layer_selection = new map_layer_wx;
	layer_selection->copy (layer_from);
	layer_selection->name = "Selection"; // Was importer->filename_source.  Needed for OnShowRiver
	layer_selection->color = RGB (0, 255, 0);
	map.layers.push_back (layer_selection);
}

map_layer *dialog_selection::create_map_layer
	(const map_layer *layer_from)

// called from frame_nhd_view

{
	map_layer *layer_points;

	layer_points = new map_layer_wx;
	layer_points->name = layer_from->name;
	layer_points->attribute_count_numeric = layer_from->attribute_count_numeric;
	layer_points->attribute_count_text = layer_from->attribute_count_text;
	layer_points->color = layer_from->color;
	layer_points->symbol = layer_from->symbol;
	layer_points->draw_as = layer_from->draw_as;
	map.layers.push_back (layer_points);
	return layer_points;
}

void dialog_selection::describe_segment
	(map_polygon *line,
	double *stream_length_km,
	dynamic_string &log)

{
	int node_index, next_index;
	double length_km = 0.0;

	log.add_formatted ("%ld", line->id);

	*stream_length_km = 0.0;
	for (node_index = 0; node_index < (line->node_count - 1); ++node_index) {
		next_index = node_index + 1;

		length_km = distance_meters ((double) line->nodes [node_index * 2 + 1] / 1.0e6, (double) line->nodes [node_index * 2] / 1.0e6,
		(double) line->nodes [next_index * 2 + 1] / 1.0e6, (double) line->nodes [next_index * 2] / 1.0e6) / 1000.0;

		log.add_formatted ("\t%.2lf\t%.2lf\t%.4lf\n",
		(double) line->nodes [node_index * 2] / 1.0e6,
		(double) line->nodes [node_index * 2 + 1] / 1.0e6,
		length_km);
		*stream_length_km += length_km;
	}

	// show the last point and total distance
	node_index = line->node_count - 1;

	log.add_formatted ("\t%.2lf\t%.2lf\n", (double) line->nodes [node_index * 2] / 1.0e6, (double) line->nodes [node_index * 2 + 1] / 1.0e6);

	if (line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] == 2.0)
		log.add_formatted ("%ld\tDIV.\t\t%.4lf\t%.4lf\n", line->id, *stream_length_km, line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM]);
	else
		log.add_formatted ("%ld\t\t\t%.4lf\t%.4lf\n", line->id, *stream_length_km, line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM]);
}

