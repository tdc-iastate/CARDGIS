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
#include "../util/device_coordinate.h"
#include "../util/filename_struct.h"
#include "../util/listbox_files.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/dynamic_map_gl2.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "dialog_river.h"
#include "panel_shapefile.h"
// #include "../i_swat_database/SWATDeclarations.h"
// #include "app_nhd_view.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"

dialog_river::dialog_river
	(wxWindow *pParent,
	class dynamic_map *p_map,
	const bool opengl,
	flow_network *p_river_network,
	class color_set *p_colors)
	: wxDialog (pParent, -1, "River")
{
	river_network = p_river_network;
	streams = other_streams = layer_point_from = layer_point_to = NULL;
	colors = p_colors;
	frame_map = p_map;
	use_gl = opengl;

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

	if (use_gl)
		map = new dynamic_map_gl2;
	else
		map = new dynamic_map_wx;

	panel_river = new panel_shapefile (this, map, NULL, wxSize (650, 600));
	panel_river->use_gl = false;
	stack->Add (panel_river);

	// stack->AddSpacer (25);
	// stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

}

BEGIN_EVENT_TABLE(dialog_river, wxDialog)
	EVT_COMBOBOX(COMBO_SELECT_LAYER, dialog_river::OnSelectLayer)
END_EVENT_TABLE()

dialog_river::~dialog_river ()

{
	if (map)
		delete map;
}

void dialog_river::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
	}
	wxDialog::EndModal (retCode);
}


void dialog_river::set_map ()

// Calling function has created map layers

{
	map->set_extent ();

	// map.logical_extent.inflate (1.01);

	panel_river->change_layers ();
	panel_river->Refresh ();
}

void dialog_river::OnSelectLayer
	(wxCommandEvent &)

{
	int index = combo_select_layer->GetSelection ();
	panel_river->change_selection_layer (map->layers [index]);
}

void dialog_river::create_map_layers
	(const map_layer *layer_from,
	const map_layer *layer_to)

// called from frame_nhd_view

{
	// Create map layers for dialog_river
	other_streams = map->create_new (MAP_OBJECT_POLYGON);
	other_streams->enclosure = MAP_POLYGON_NOT_ENCLOSED; // Don't draw line from first node to last
	other_streams->initialize_attributes = true; // clear all attributes as polygons are created
	other_streams->attribute_count_numeric = 4;
	other_streams->name = "Other Rivers"; // Was importer->filename_source.  Needed for OnShowRiver
	other_streams->color = RGB (128, 128, 128);
	other_streams->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	map->layers.push_back (other_streams);

	// Draw routing path layer above other streams
	streams = map->create_new (MAP_OBJECT_POLYGON);
	streams->enclosure = MAP_POLYGON_NOT_ENCLOSED; // Don't draw line from first node to last
	streams->initialize_attributes = true; // clear all attributes as polygons are created
	streams->attribute_count_numeric = 4;
	streams->name = "River Segment"; // Was importer->filename_source.  Needed for OnShowRiver
	streams->color = RGB (0, 255, 0);
	streams->colors = *colors;
	streams->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	map->layers.push_back (streams);

	layer_point_from = map->create_new (MAP_OBJECT_POINT);
	layer_point_from->name = layer_from->name;
	layer_point_from->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC; // Will be filled with COMID of nearest NHD segment, index of nearest point, DIVERGENCE flag, distance_m
	layer_point_from->color = layer_from->color;
	layer_point_from->symbol = layer_from->symbol;
	layer_point_from->draw_as = layer_from->draw_as;
	layer_point_from->draw_labels_id = true;
	map->layers.push_back (layer_point_from);

	layer_point_to = map->create_new (MAP_OBJECT_POINT);
	layer_point_to->name = layer_to->name;
	layer_point_to->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC; // Will be filled with COMID of nearest NHD segment, index of nearest point, DIVERGENCE flag, distance_m
	layer_point_to->color = layer_to->color;
	layer_point_to->symbol = layer_to->symbol;
	layer_point_to->draw_as = layer_to->draw_as;
	layer_point_to->draw_labels_id = true;
	map->layers.push_back (layer_point_to);
}

void dialog_river::describe_segment
	(map_object *line,
	double *stream_length_km,
	dynamic_string &log)

// One line

{
	int node_index, polygon_index, next_index;
	map_polygon *vector;

	*stream_length_km = 0.0;

	if (line->type == MAP_OBJECT_POLYGON) {
		vector = (map_polygon *) line;
		for (node_index = 0; node_index < (vector->node_count - 1); ++node_index) {
			next_index = node_index + 1;

			*stream_length_km += distance_meters ((double) vector->nodes [node_index * 2 + 1] / 1.0e6, (double) vector->nodes [node_index * 2] / 1.0e6,
			(double) vector->nodes [next_index * 2 + 1] / 1.0e6, (double) vector->nodes [next_index * 2] / 1.0e6) / 1000.0;
		}
	}
	else {
		for (polygon_index = 0;
		polygon_index < ((map_polygon_complex *) line)->polygon_count;
		++polygon_index) {
			vector = (map_polygon *) &((map_polygon_complex *) line)->polygons [polygon_index];
			for (node_index = 0; node_index < (vector->node_count - 1); ++node_index) {
				next_index = node_index + 1;

				*stream_length_km += distance_meters ((double) vector->nodes [node_index * 2 + 1] / 1.0e6, (double) vector->nodes [node_index * 2] / 1.0e6,
				(double) vector->nodes [next_index * 2 + 1] / 1.0e6, (double) vector->nodes [next_index * 2] / 1.0e6) / 1000.0;
			}
		}
	}

	if (line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] == 2.0)
		log.add_formatted ("%ld\tDIV.\t\t%.4lf\t%.4lf\n", line->id, *stream_length_km, line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM]);
	else
		log.add_formatted ("%ld\t\t\t%.4lf\t%.4lf\n", line->id, *stream_length_km, line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM]);
}

void dialog_river::describe_segment_points
	(map_object *line,
	const int nearest_point_index,
	double *stream_length_km,
	dynamic_string &log)

// write the individual node coordinates of either the start or end segment

{
	int node_index, next_index;
	double length_km = 0.0;
	map_polygon *vector;
	device_coordinate point1, point2;

	log.add_formatted ("%ld", line->id);

	*stream_length_km = 0.0;

	if (line->type == MAP_OBJECT_POLYGON) {
		vector = (map_polygon *) line;

		for (node_index = 0; node_index < (vector->node_count - 1); ++node_index) {
			next_index = node_index + 1;

			point1 = vector->point_at_index (node_index);
			point2 = vector->point_at_index (next_index);
			length_km = distance_meters (point1, point2) / 1000.0;

			log.add_formatted ("\t%.2lf\t%.2lf\t%.4lf",
			(double) point1.x / 1.0e6,
			(double) point1.y / 1.0e6,
			length_km);
			if (node_index == nearest_point_index)
				log += "\t*";
			log += "\n";
			*stream_length_km += length_km;
		}

		// show the last point and total distance
		node_index = vector->node_count - 1;
		point1 = vector->point_at_index (vector->node_count - 1);

		log.add_formatted ("\t%.2lf\t%.2lf\n", (double) point1.x / 1.0e6, (double) point1.y / 1.0e6);
	}
	else {
		int polygon_index;

		for (polygon_index = 0;
		polygon_index != ((map_polygon_complex *) line)->polygon_count;
		++polygon_index) {
			vector = (map_polygon *) &((map_polygon_complex *) line)->polygons [polygon_index];

			for (node_index = 0; node_index < (vector->node_count - 1); ++node_index) {
				next_index = node_index + 1;

				point1 = vector->point_at_index (node_index);
				point2 = vector->point_at_index (next_index);
				length_km = distance_meters (point1, point2) / 1000.0;

				log.add_formatted ("\t%.2lf\t%.2lf\t%.4lf",
				(double) point1.x / 1.0e6,
				(double) point1.y / 1.0e6,
				length_km);
				if (node_index == nearest_point_index)
					log += "\t*";
				log += "\n";
				*stream_length_km += length_km;
			}

			if (polygon_index ==  ((map_polygon_complex *) line)->polygon_count - 1) {
				// show the last point and total distance
				node_index = vector->node_count - 1;
				point1 = vector->point_at_index (vector->node_count - 1);

				log.add_formatted ("\t%.2lf\t%.2lf\n", (double) point1.x / 1.0e6, (double) point1.y / 1.0e6);
			}
		}

	}

	if (line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] == 2.0)
		log.add_formatted ("%ld\tDIV.\t\t%.4lf\t%.4lf\n", line->id, *stream_length_km, line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM]);
	else
		log.add_formatted ("%ld\t\t\t%.4lf\t%.4lf\n", line->id, *stream_length_km, line->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM]);
}

void dialog_river::describe_all
	(const bool upstream,
	dynamic_string &log)

// lengths of all segments, plus attributes of facility & station

{
	std::vector <map_object *>::const_iterator point;
	double total_length_preset, total_length_km, stream_length_km, distance_between_km, distance_start_km, distance_end_km;
	dynamic_string log_start_segment, log_end_segment, log_river_segments;
	long start_segment_id = 0, end_segment_id = 0;
	int count_between = 0, start_segment_node_id = 0, end_segment_node_id = 0;
	total_length_preset = total_length_km = distance_between_km = distance_start_km = distance_end_km = 0.0;
	map_object *nearest_segment;

	// log_start_segment: Description of start point(s)
	for (point = layer_point_from->objects.begin ();
	point != layer_point_from->objects.end ();
	++point) {
		// (*point)->write_data (facilities, log);
		start_segment_node_id = (int) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
		log_start_segment += layer_point_from->name;
		log_start_segment.add_formatted ("\t%ld\t", (*point)->id);
		log_start_segment += (*point)->name;
		log_start_segment.add_formatted ("\t%ld\t%d\t%d\n",
		(long) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID],
		(int) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX],
		start_segment_node_id);
		start_segment_id = (long) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];

		if ((nearest_segment = streams->match_id ((long) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID])) != NULL)
			if (nearest_segment->type == MAP_OBJECT_POLYGON) {
				if (upstream) {
					((map_polygon *) nearest_segment)->perimeter_between_meters (0, (int) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX], &distance_start_km, log);
					distance_start_km /= 1.0e3;
				}
				else {
					((map_polygon *) nearest_segment)->perimeter_between_meters ((int) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX], ((map_polygon *) nearest_segment)->node_count - 1, &distance_start_km, log);
					distance_start_km /= 1.0e3;
				}
			}
	}

	if ((point = layer_point_to->objects.begin ()) != layer_point_to->objects.end ()) {
		end_segment_id = (long) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
		end_segment_node_id = (int) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
	}

	// log_river_segments: Description of nodes of start point, segments between, and nodes of end point
	for (point = streams->objects.begin ();
	point != streams->objects.end ();
	++point) {
		// (*point)->dump (streams, log);
		if ((*point)->id == start_segment_id)
			describe_segment_points (*point, start_segment_node_id, &stream_length_km, log_river_segments);
		else
			if ((*point)->id == end_segment_id)
				describe_segment_points (*point, end_segment_node_id, &stream_length_km, log_river_segments);
			else {
				describe_segment (*point, &stream_length_km, log_river_segments);
				++count_between;
				distance_between_km += stream_length_km;
				total_length_km += stream_length_km;
				total_length_preset += (*point)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM];
			}
	}

	// log_end_segment: Description of end point(s)
	for (point = layer_point_to->objects.begin ();
	point != layer_point_to->objects.end ();
	++point) {
		log_end_segment += layer_point_to->name;
		log_end_segment.add_formatted ("\t%ld\t", (*point)->id);
		log_end_segment += (*point)->name;
		// (*point)->write_data (stations, log);
		log_end_segment.add_formatted ("\t%ld\t%d\t%d\n",
		(long) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID],
		(int) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX],
		end_segment_node_id);

		if ((nearest_segment = streams->match_id ((long) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID])) != NULL)
			if (nearest_segment->type == MAP_OBJECT_POLYGON) {
				if (upstream) {
					((map_polygon *) nearest_segment)->perimeter_between_meters ((int) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX], ((map_polygon *) nearest_segment)->node_count - 1, &distance_end_km, log);
					distance_end_km /= 1.0e3;
				}
				else {
					((map_polygon *) nearest_segment)->perimeter_between_meters (0, (int) (*point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX], &distance_end_km, log);
					distance_end_km /= 1.0e3;
				}
			}
	}
	total_length_km += distance_start_km;
	total_length_km += distance_end_km;

	log += "\t\t\tCalculated Length km\tPreset Length km\n";
	log.add_formatted ("Total length\t\t\t%.3lf\t%.3lf\n", total_length_km, total_length_preset + distance_start_km + distance_end_km);
	log.add_formatted ("Start Segment\t\t\t%.3lf\n", distance_start_km);
	log.add_formatted ("Segments Between\t\t%d\t%.3lf\t%.3lf\n", count_between, distance_between_km, total_length_preset);
	log.add_formatted ("End Segment\t\t\t%.3lf\n", distance_end_km);

	{
		// 2017-07-05 diagnosing 150mile limit
		std::vector <map_object *>::const_iterator end_point;
		long start_x, start_y, end_x, end_y;

		point = layer_point_from->objects.begin ();
		end_point = layer_point_to->objects.begin ();
		if ((point != layer_point_from->objects.end ())
		&& (end_point != layer_point_to->objects.end ())) {
			(*point)->centroid (&start_y, &start_x);
			(*end_point)->centroid (&end_y, &end_x);
			log.add_formatted ("\t%.3lf\t%.3lf\n", (double) start_y / 1.0e6, (double) start_x / 1.0e6);
			log.add_formatted ("\t%.3lf\t%.3lf\n", (double) end_y / 1.0e6, (double) end_x / 1.0e6);
			log.add_formatted ("Crow flight\t\t\t%.3lf\n", distance_meters (start_y, start_x, end_y, end_x) / 1000.0);
		}
	}

	log += "\nPoint\tID\tName\tCOMID\tPolygon\tNode\n";
	log += log_start_segment;
	log += log_end_segment;
	log += "\n";

	log += "ID\tLong\tLat\tCalculated Length km\tPreset Length km\n";
	log += log_river_segments;

	log += "\n";

}
