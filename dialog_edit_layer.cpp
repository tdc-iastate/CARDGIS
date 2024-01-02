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
#include <wx/clrpicker.h>
#include "../util/dynamic_string.h"
#include "../util/interface_window.h"
#include "../util/custom.h"
#include "../util/filename_struct.h"
#include "../util/listbox_files.h"
#include "../util/device_coordinate.h"
#include "../util/arbitrary_counter.h"
#include "../util/dialog_error_list.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"
#include "dialog_legend.h"
#include "dialog_edit_layer.h"
#include "timed_measurements.h"


BEGIN_EVENT_TABLE(dialog_edit_layer, wxDialog)
	EVT_BUTTON (BUTTON_RIVER_FTYPE, dialog_edit_layer::OnRiverFTYPE)
	EVT_BUTTON (BUTTON_RIVER_LENGTH, dialog_edit_layer::OnRiverLength)
	EVT_BUTTON (BUTTON_RIVER_STREAMLEVEL, dialog_edit_layer::OnRiverStreamLevel)
	EVT_BUTTON (BUTTON_RIVER_MAJOR_ONLY, dialog_edit_layer::OnRiverMajorOnly)
	EVT_BUTTON (BUTTON_RIVER_DIVERGENCE, dialog_edit_layer::OnRiverDivergence)
	EVT_BUTTON (BUTTON_RIVER_LEVEL_PATH, dialog_edit_layer::OnRiverLevelPath)
	EVT_BUTTON (BUTTON_RIVER_STRAHLER_ORDER, dialog_edit_layer::OnRiverStrahlerOrder)
	EVT_BUTTON (BUTTON_RIVER_UPHYDROSEQ, dialog_edit_layer::OnRiverUpHydroSeq)
	EVT_BUTTON (BUTTON_RIVER_DNHYDROSEQ, dialog_edit_layer::OnRiverDnHydroSeq)
	EVT_BUTTON (BUTTON_RIVER_ELEVATIONMIN, dialog_edit_layer::OnRiverElevationMin)
	EVT_BUTTON (BUTTON_RIVER_ELEVATIONMAX, dialog_edit_layer::OnRiverElevationMax)
	EVT_BUTTON (BUTTON_SUBBASIN_BACKGROUND, dialog_edit_layer::OnSubbasinBackground)
	EVT_BUTTON (BUTTON_AUTORANGE, dialog_edit_layer::OnAutoRange)
	EVT_BUTTON (BUTTON_INTEGER_SCALE, dialog_edit_layer::OnIntegerScale)
	EVT_BUTTON (BUTTON_SECCHI, dialog_edit_layer::OnSecchiDepth)
END_EVENT_TABLE()

dialog_edit_layer::dialog_edit_layer
	(wxWindow *pParent,
	dynamic_map *p_map,
	map_layer *p_selection)
	: wxDialog (pParent, -1, "Edit Layer")
{
	map = p_map;
	layer = p_selection;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer * row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Layer Name:")), 0, wxALL, 5);
	row->AddSpacer (10);
	edit_layer_name = new wxTextCtrl (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	row->Add (edit_layer_name, 0, wxALL, 5);
	row->AddSpacer (25);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Title:")), 0, wxALL, 5);
	row->AddSpacer (10);
	edit_title = new wxTextCtrl (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	row->Add (edit_title, 0, wxALL, 5);
	stack->Add (row);

	wxBoxSizer *top_row = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer *column = new wxBoxSizer (wxVERTICAL);

	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Layer Type:")), 0, wxALL, 5);
	row->AddSpacer (10);

	{
		wxArrayString choices;
		choices.Add ("Point");
		choices.Add ("Path");
		choices.Add ("Polygon");

		combo_layer_type = new wxComboBox (this, COMBO_SELECT_TYPE, wxEmptyString, wxDefaultPosition, wxSize (250, 24), choices,  wxCB_READONLY);
		row->Add (combo_layer_type, 0, wxALL, 5);
	}
	column->Add (row);

	{
		wxStaticBoxSizer *autorange_box = new wxStaticBoxSizer (wxVERTICAL, this, wxT ("Attributes:"));
		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, wxID_ANY, wxT ("Numeric:")), 0, wxALL, 5);
		row->AddSpacer (10);
		list_column_names_numeric = new wxListBox (this, wxID_ANY, wxDefaultPosition, wxSize (300, 100), 0, 0);
		row->Add (list_column_names_numeric, 0, wxALL, 5);
		autorange_box->Add (row);

		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, wxID_ANY, wxT ("Text:")), 0, wxALL, 5);
		row->AddSpacer (10);
		list_column_names_text = new wxListBox (this, wxID_ANY, wxDefaultPosition, wxSize (300, 100), 0, 0);
		row->Add (list_column_names_text, 0, wxALL, 5);
		autorange_box->Add (row);
		column->Add (autorange_box, 0, wxALL, 5);
	}

	row = new wxBoxSizer (wxHORIZONTAL);
	check_show_labels_id = new wxCheckBox (this, wxID_ANY, L"Show ID Labels");
	row->Add (check_show_labels_id, 0, wxALL, 5);
	row->AddSpacer (25);
	check_show_labels_name = new wxCheckBox (this, wxID_ANY, L"Show Name Labels");
	row->Add (check_show_labels_name, 0, wxALL, 5);
	column->Add (row);

	{
		wxArrayString choices;

		choices.Add ("None");
		choices.Add ("Fill Layer color");
		choices.Add ("Fill Data color");
		choices.Add ("Fill and Outline");
		choices.Add ("Outline Layer Color");
		choices.Add ("Outline Data Color");
		choices.Add ("Symbol Layer Color");
		choices.Add ("Symbol Data Color");
		choices.Add ("Symbol Fill and Outline");
		choices.Add ("Outline Data Color Modulo");
		choices.Add ("Pie Charts");
		choices.Add ("Node Label");
		choices.Add ("sized by timed data count");

		draw_as_combo_indexes.insert (std::pair <int, int> (0, MAP_OBJECT_DRAW_NONE));
		draw_as_combo_indexes.insert (std::pair <int, int> (1, MAP_OBJECT_DRAW_FILL_LAYER_COLOR));
		draw_as_combo_indexes.insert (std::pair <int, int> (2, MAP_OBJECT_DRAW_FILL_DATA_COLOR));
		draw_as_combo_indexes.insert (std::pair <int, int> (3, MAP_OBJECT_DRAW_FILL_AND_OUTLINE));
		draw_as_combo_indexes.insert (std::pair <int, int> (4, MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR));
		draw_as_combo_indexes.insert (std::pair <int, int> (5, MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR));
		draw_as_combo_indexes.insert (std::pair <int, int> (6, MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR));
		draw_as_combo_indexes.insert (std::pair <int, int> (7, MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR));
		draw_as_combo_indexes.insert (std::pair <int, int> (8, MAP_OBJECT_DRAW_SYMBOL_FILL_AND_OUTLINE));
		draw_as_combo_indexes.insert (std::pair <int, int> (9, MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR_MODULO));
		draw_as_combo_indexes.insert (std::pair <int, int> (10, MAP_OBJECT_DRAW_PIE_CHARTS));
		draw_as_combo_indexes.insert (std::pair <int, int> (11, MAP_OBJECT_DRAW_NODE_LABEL));
		draw_as_combo_indexes.insert (std::pair <int, int> (12, MAP_OBJECT_DRAW_SYMBOL_SIZE_FUNCTION));

		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, wxID_ANY, wxT("Draw As:")), 0, wxALL, 5);
		combo_draw_as = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize (250, 24), choices,  wxCB_READONLY);
		row->Add (combo_draw_as, 0, wxALL, 5);
		column->Add (row);

		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, wxID_ANY, wxT("Layer color:")), 0, wxALL, 5);
		control_color = new wxColourPickerCtrl (this, wxID_ANY, layer->color, wxDefaultPosition, wxDefaultSize);
		row->Add (control_color);
		column->Add (row);

		row = new wxBoxSizer (wxHORIZONTAL);
		row->Add (new wxStaticText (this, wxID_ANY, wxT("Symbol:")), 0, wxALL, 5);
		row->AddSpacer (10);
		choices.clear ();
		choices.Add ("POINT");
		choices.Add ("CIRCLE");
		choices.Add ("PLUS");
		choices.Add ("X");
		choices.Add ("SQUARE");
		choices.Add ("TRIANGLE");
		choices.Add ("DIAMOND");

		combo_draw_symbol = new wxComboBox (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize (250, 24), choices,  wxCB_READONLY);
		row->Add (combo_draw_symbol, 0, wxALL, 5);
		column->Add (row);
	}
	top_row->Add (column);

	column = new wxBoxSizer (wxVERTICAL);

	{
		wxStaticBoxSizer *autorange_box = new wxStaticBoxSizer (wxVERTICAL, this, wxT ("Data Ranges:"));

		autorange_box->Add (new wxButton (this, BUTTON_AUTORANGE, L"Set"));
		autorange_box->AddSpacer (5);
		check_separate_zero = new wxCheckBox (this, wxID_ANY, L"Separate Zero");
		autorange_box->Add (check_separate_zero, 0, wxALL, 5);

		color_set colors = layer->colors;
		int color_height;
		if (colors.colors.size () > 0) {
			color_height = colors.colors.size () * 30;
			if (color_height < 300)
				color_height = 300;
		}
		else
			color_height = 300;

		color_panel = new wxScrolled <panel_color_legend> (this, wxID_ANY, wxDefaultPosition, wxSize (400, 300));
		color_panel->colors = &layer->colors;
		color_panel->SetVirtualSize (wxSize (500, color_height));
		autorange_box->Add (color_panel);

		column->Add (autorange_box, 0, wxALL, 5);
		top_row->Add (column);
	}
	stack->Add (top_row);

	wxStaticBoxSizer *preset_box = new wxStaticBoxSizer (wxHORIZONTAL, this, wxT ("Custom Settings:"));

	wxBoxSizer *button_column = new wxBoxSizer( wxVERTICAL );
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_FTYPE, L"River - FTYPE"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_LENGTH, L"River - Length"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_STREAMLEVEL, L"River - Stream Level"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_MAJOR_ONLY, L"River - major channels"), 0, wxALL, 5);
	preset_box->Add (button_column);

	button_column = new wxBoxSizer( wxVERTICAL );
	button_column->Add (new wxButton (this, BUTTON_RIVER_DIVERGENCE, L"River - Divergence"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_LEVEL_PATH, L"River - Level Path"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_STRAHLER_ORDER, "River - Strahler Order"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_UPHYDROSEQ, "River - UpHydroSeq"), 0, wxALL, 5);
	preset_box->Add (button_column);

	button_column = new wxBoxSizer( wxVERTICAL );
	button_column->Add (new wxButton (this, BUTTON_RIVER_DNHYDROSEQ, "River - DnHydroSeq"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_ELEVATIONMIN, "River - Elevation Min"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_RIVER_ELEVATIONMAX, "River - Elevation Max"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_SUBBASIN_BACKGROUND, L"Subbasin - Background"), 0, wxALL, 5);
	preset_box->Add (button_column);

	button_column = new wxBoxSizer( wxVERTICAL );
	button_column->Add (new wxButton (this, BUTTON_INTEGER_SCALE, L"Subbasin - Integers"), 0, wxALL, 5);
	button_column->AddSpacer (5);
	button_column->Add (new wxButton (this, BUTTON_SECCHI, L"Secchi Depth"), 0, wxALL, 5);
	preset_box->Add (button_column);

	stack->Add (preset_box, 0, wxALL, 5);

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

	edit_layer_name->SetValue (layer->name.get_text ());
	edit_title->SetValue (map->title.get_text ());
	show_column_names ();
	combo_layer_type->SetSelection (layer->type - 1);
	combo_draw_symbol->SetSelection (layer->symbol - 1);
	check_show_labels_id->SetValue (layer->draw_labels_id);
	check_show_labels_name->SetValue (layer->draw_labels_name);
	check_separate_zero->SetValue (false);

	{
		std::map <int, int>::iterator draw_as;
		for (draw_as = draw_as_combo_indexes.begin ();
		draw_as != draw_as_combo_indexes.end ();
		++draw_as)
			if (draw_as->second == layer->draw_as)
				combo_draw_as->SetSelection (draw_as->first);
	}
}



int symbol_size_function
	(const map_object *point,
	const map_scale *scale)

{
	if (point->attributes_void) {
		return ((int) (scale->device_x ((double) ((timed_measurements *) point->attributes_void)->amounts_ppm.size ()) / 5000.0));
	}
	else
		return 5;
}

void dialog_edit_layer::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		int index;

		layer->name = edit_layer_name->GetValue ().ToAscii ();
		map->title = edit_title->GetValue ().ToAscii ();
		layer->draw_labels_id = check_show_labels_id->GetValue ();
		layer->draw_labels_name = check_show_labels_name->GetValue ();
		layer->color = control_color->GetColour ().GetPixel ();

		if ((index = list_column_names_numeric->GetSelection ()) != -1)
			layer->data_attribute_index = index;
		
		if ((index = combo_layer_type->GetSelection ()) != -1)
			layer->type = index + 1;

		if ((index = combo_draw_as->GetSelection ()) != -1) {
			std::map <int, int>::iterator draw_as;
			draw_as = draw_as_combo_indexes.find (index);
			layer->draw_as = draw_as->second;

			if (layer->draw_as == MAP_OBJECT_DRAW_SYMBOL_SIZE_FUNCTION)
				layer->size_function_parameter = symbol_size_function;

		}
		layer->symbol = combo_draw_symbol->GetSelection () + 1;
	}
	wxDialog::EndModal (retCode);
}

void dialog_edit_layer::show_column_names ()

{
	std::vector <dynamic_string>::const_iterator column_name;

	list_column_names_numeric->Clear ();

	for (column_name = layer->column_names_numeric.begin ();
	column_name != layer->column_names_numeric.end ();
	++column_name)
		list_column_names_numeric->Append (column_name->get_text ());

	if (layer->column_names_numeric.size () > layer->data_attribute_index)
		list_column_names_numeric->SetSelection (layer->data_attribute_index);

	list_column_names_text->Clear ();
	for (column_name = layer->column_names_text.begin ();
	column_name != layer->column_names_text.end ();
	++column_name)
		list_column_names_text->Append (column_name->get_text ());
}


void dialog_edit_layer::OnRiverFTYPE
	(wxCommandEvent &)


{
	layer->colors.colors.clear ();

	color_panel->colors = &layer->colors;

	layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE;
	layer->colors.add_color (RGB (255, 0, 255), (double) NHD_FTYPE_ARTIFICIALPATH, 2.0, NHD_FTYPE_name (NHD_FTYPE_ARTIFICIALPATH).get_text_ascii ()); // Magenta
	layer->colors.add_color (RGB (0, 0, 255), (double) NHD_FTYPE_STREAMRIVER, 3.0, NHD_FTYPE_name (NHD_FTYPE_STREAMRIVER).get_text_ascii ()); // Blue
	layer->colors.add_color (RGB (0, 255, 0), (double) NHD_FTYPE_COASTLINE, 4.0, NHD_FTYPE_name (NHD_FTYPE_COASTLINE).get_text_ascii ()); // Green
	layer->colors.add_color (RGB (112, 146, 190), (double) NHD_FTYPE_CONNECTOR, 5.0, NHD_FTYPE_name (NHD_FTYPE_CONNECTOR).get_text_ascii ()); // Navy blue
	layer->colors.add_color (RGB (127, 127, 127), (double) NHD_FTYPE_CANALDITCH, 6.0, NHD_FTYPE_name (NHD_FTYPE_CANALDITCH).get_text_ascii ()); // gray
	layer->colors.add_color (RGB (0, 255, 255), (double) NHD_FTYPE_PIPELINE, 7.0, NHD_FTYPE_name (NHD_FTYPE_PIPELINE).get_text_ascii ()); // cyan
	color_panel->Refresh ();
	combo_draw_as->SetSelection (layer->draw_as);
	if (layer->column_names_numeric.size () > layer->data_attribute_index)
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
}

void dialog_edit_layer::OnRiverLength 
	(wxCommandEvent &)

{
	layer->colors.colors.clear ();

	color_panel->colors = &layer->colors;
	// Grayscale for length
	layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM;

	layer->colors.add_color (RGB (224, 224, 224), 0.0, 0.5);
	layer->colors.add_color (RGB (192, 192, 192), 0.5, 1.0);
	layer->colors.add_color (RGB (160, 160, 160), 1.0, 2.0);
	layer->colors.add_color (RGB (128, 128, 128), 2.0, 3.0);
	layer->colors.add_color (RGB (96, 96, 96), 3.0, 4.0);
	layer->colors.add_color (RGB (64, 64, 64), 4.0, 5.0);
	layer->colors.add_color (RGB (32, 32, 32), 5.0, 1.0e6);
	color_panel->Refresh ();
	combo_draw_as->SetSelection (layer->draw_as);
	if (layer->column_names_numeric.size () > layer->data_attribute_index)
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
}

void dialog_edit_layer::OnRiverStreamLevel
	(wxCommandEvent &)

{
	layer->colors.colors.clear ();

	color_panel->colors = &layer->colors;

	layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL;

	layer->colors.add_color (RGB (53, 94, 0), 0.0, 1.0); // Green darkest
	layer->colors.add_color (RGB (0, 255, 0), 1.0, 2.0); // Green 2
	layer->colors.add_color (RGB (148, 189, 94), 2.0, 3.0); // Green 3
	layer->colors.add_color (RGB (51, 204, 102), 3.0, 4.0); // Green 4
	layer->colors.add_color (RGB (0, 255, 0), 4.0, 5.0); // Green lightest

	layer->colors.add_color (RGB (76, 25, 0), 5.0, 6.0); // Yellow scale
	layer->colors.add_color (RGB (153, 102, 51), 6.0, 7.0);
	layer->colors.add_color (RGB (179, 179, 0), 7.0, 8.0);
	layer->colors.add_color (RGB (230, 230, 76), 8.0, 9.0);
	layer->colors.add_color (RGB (255, 255, 0), 9.0, 10.0);

	layer->colors.add_color (RGB (40, 0, 153), 10.0, 11.0); // Blue to cyan
	layer->colors.add_color (RGB (35, 0, 220), 11.0, 12.0);
	layer->colors.add_color (RGB (0, 71, 255), 12.0, 13.0);
	layer->colors.add_color (RGB (0, 0, 255), 13.0, 14.0);
	layer->colors.add_color (RGB (0, 184, 255), 14.0, 15.0);
	layer->colors.add_color (RGB (0, 255, 255), 15.0, 16.0);

	color_panel->Refresh ();
	combo_draw_as->SetSelection (layer->draw_as);
	if (layer->column_names_numeric.size () > layer->data_attribute_index)
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
}

void dialog_edit_layer::OnRiverMajorOnly
	(wxCommandEvent &)

{
	layer->colors.colors.clear ();

	color_panel->colors = &layer->colors;

	layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL;

	// two shades of light gray to stay inconspicuous above subbasin colors
	// layer->colors.add_color (RGB (0, 255, 255), 0.0, 1.0);
	layer->colors.add_color (RGB (0, 0, 255), 1.0, 2.0);
	layer->colors.add_color (RGB (0, 255, 255), 2.0, 3.0);

	// others not visible
	color_panel->Refresh ();
	combo_draw_as->SetSelection (layer->draw_as);
	if (layer->column_names_numeric.size () > layer->data_attribute_index)
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
}


void dialog_edit_layer::OnAutoRange
	(wxCommandEvent &)

{
	dynamic_string log;
	std::vector <map_object *>::const_iterator p;
	arbitrary_counter ranger;
	int index;

	if ((index = list_column_names_numeric->GetSelection ()) != -1)
		layer->data_attribute_index = index;

	map->autorange (layer, 10, check_separate_zero->GetValue (), false, log);

	color_panel->colors = &layer->colors;

	layer->colors.make_labels ();


	// specific to subbasin_effect map
	// Red for negative, Green for positive
	// lowest value is -1,298,840
	// highest value is 461,970
	/*
	layer->colors.add_color (RGB (128, 0, 0), -1300000.0, -800000.0, "-1.3e6..-8.0e5");
	layer->colors.add_color (RGB (191, 0, 0), -800000.0, -500000.0, "-8.0e5..-5.0e5");
	layer->colors.add_color (RGB (255, 0, 0), -500000.0, -100000.0, "-5.0e5..-1.0e5");
	// zero values are marked -1 to distinguish from Not-Run
	layer->colors.add_color (RGB (255, 128, 128), -100000.0, -1.0, "-1.0e5..0");
	layer->colors.add_color (RGB (192, 192, 192), -1.0, 0.0, "0..1");
	// Not run values (0.0) are omitted
	layer->colors.add_color (RGB (0, 255, 0), 1.0, 100000.0, "1..1.0e5");
	layer->colors.add_color (RGB (0, 128, 0), 100000.0, 500000.0, ">=1");
	*/

	/*
	// 2016-09-23 Single comparison run showing PS effect from target to outlet
	layer->colors.add_color (RGB (192, 192, 192), 0.0, 0.01, "0");
	layer->colors.add_color (RGB (255, 255, 0), 0.01, 5000.0, "0..5.0e5"); // yellow
	layer->colors.add_color (RGB (255, 128, 0), 5000.0, 100000.0, "5.0e3..1.0e5"); // orange
	layer->colors.add_color (RGB (255, 0, 0), 500000.0, 100000.0, "1.0e5..5.0e5");
	layer->colors.add_color (RGB (191, 0, 0), 500000.0, 800000.0, "5.0e5..8.0e5");
	layer->colors.add_color (RGB (128, 0, 0), 800000.0, 1300000.0, "8.0e5..1.3e6");
	*/
	/*
	// 2016-09-23 Single comparison run showing NPS effect from target 4579 to outlet
	layer->colors.add_color (RGB (192, 192, 192), 0.0, 0.01, "0");
	layer->colors.add_color (RGB (255, 255, 0), 0.01, 10000.0, "0..1.0e4"); // yellow
	layer->colors.add_color (RGB (255, 128, 0), 10000.0, 100000.0, "1.0e4..1.0e5"); // orange
	layer->colors.add_color (RGB (255, 0, 0), 100000.0, 200000.0, "1.0e5..2.0e5");
	layer->colors.add_color (RGB (191, 0, 0), 200000.0, 400000.0, "2.0e5..4.0e5");
	layer->colors.add_color (RGB (128, 0, 0), 400000.0, 1100000.0, "4.0e5..1.1e6");
	*/

	/*
	// 2016-09-23 573 random targets, showing PS&NPS effect on Outlet Reach from each
	// 2017-01-17 Target 2873 NO3
	layer->colors.add_color (RGB (192, 192, 192), 0.0, 0.01, "0");
	layer->colors.add_color (RGB (255, 255, 0), 0.01, 100000.0, "0..1.0e5"); // yellow
	layer->colors.add_color (RGB (255, 128, 0), 100000.0, 200000.0, "1.0e5..2.0e5"); // orange
	layer->colors.add_color (RGB (255, 0, 0), 200000.0, 500000.0, "2.0e5..5.0e5"); // Red
	layer->colors.add_color (RGB (255, 0, 128), 500000.0, 1000000.0, "5.05..1.0e6"); // Bright magenta
	*/

	/*
	// 2017-01-17 Target 2873 Mineral P
	layer->colors.add_color (RGB (192, 192, 192), 0.0, 0.01, "0");
	layer->colors.add_color (RGB (255, 255, 0), 0.01, 700.0, "0..700"); // yellow
	layer->colors.add_color (RGB (255, 128, 0), 700.0, 800.0, "700..800"); // orange
	layer->colors.add_color (RGB (255, 0, 0), 800.0, 1000.0, "800..1000"); // Red
	layer->colors.add_color (RGB (255, 0, 128), 1000.0, 2001.0, "1000..2001"); // Bright magenta
	*/

	// others not visible
	color_panel->Refresh ();

	// 2022-03-10 Don't change draw_as
	/*
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		// layer->data_attribute_index = 4; // NO3, kg
		// list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	}
	else {
		// layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
	*/

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Point File Preview", L"", log);
	d.ShowWindowModal ();
}

void dialog_edit_layer::OnSubbasinBackground
	(wxCommandEvent &)

{
	layer->colors.clear ();
	color_panel->colors = NULL;

	color_panel->colors = &layer->colors;
	layer->color = RGB (192, 192, 192);

	// others not visible
	color_panel->Refresh ();
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	}
	else {
		layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnRiverDivergence
	(wxCommandEvent &)

{
	layer->colors.clear ();

	color_panel->colors = &layer->colors;

	// Divergence
	layer->colors.add_color (RGB (0, 255, 0), 0.0, 1.0, "None"); // Green
	layer->colors.add_color (RGB (0, 0, 255), 1.0, 2.0, "Main"); // Blue
	layer->colors.add_color (RGB (255, 0, 0), 2.0, 3.0, "Not-Main"); // red

	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE;
	color_panel->Refresh ();
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	}
	else {
		layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnRiverLevelPath
	(wxCommandEvent &)
{
	layer->colors.clear ();

	color_panel->colors = &layer->colors;
	// Level Path ID
	layer->colors.add_color (RGB (53, 94, 0), 0.0, 1.0); // Green darkest
	layer->colors.add_color (RGB (0, 255, 0), 1.0, 2.0); // Green 2
	layer->colors.add_color (RGB (148, 189, 94), 2.0, 3.0); // Green 3
	layer->colors.add_color (RGB (51, 204, 102), 3.0, 4.0); // Green 4
	layer->colors.add_color (RGB (0, 255, 0), 4.0, 5.0); // Green lightest

	layer->colors.add_color (RGB (76, 25, 0), 5.0, 6.0); // Yellow scale
	layer->colors.add_color (RGB (153, 102, 51), 6.0, 7.0);
	layer->colors.add_color (RGB (179, 179, 0), 7.0, 8.0);
	layer->colors.add_color (RGB (230, 230, 76), 8.0, 9.0);
	layer->colors.add_color (RGB (255, 255, 0), 9.0, 10.0);

	layer->colors.add_color (RGB (40, 0, 153), 10.0, 11.0); // Blue to cyan
	layer->colors.add_color (RGB (35, 0, 220), 11.0, 12.0);
	layer->colors.add_color (RGB (0, 71, 255), 12.0, 13.0);
	layer->colors.add_color (RGB (0, 0, 255), 13.0, 14.0);
	layer->colors.add_color (RGB (0, 184, 255), 14.0, 15.0);
	layer->colors.add_color (RGB (0, 255, 255), 15.0, 16.0);

	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH;
	color_panel->Refresh ();
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	}
	else {
		layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnRiverStrahlerOrder
	(wxCommandEvent &)

{
	layer->colors.clear ();

	color_panel->colors = &layer->colors;
	// Strahler order
	layer->colors.add_color (RGB (192, 192, 192), 0.0, 1.0); // 0 value gray
	layer->colors.add_color (RGB (76, 25, 0), 1.0, 2.0); // Yellow scale
	layer->colors.add_color (RGB (153, 102, 51), 2.0, 3.0);
	layer->colors.add_color (RGB (179, 179, 0), 3.0, 4.0);
	layer->colors.add_color (RGB (230, 230, 76), 4.0, 5.0);
	layer->colors.add_color (RGB (255, 255, 0), 5.0, 6.0);

	layer->colors.add_color (RGB (40, 0, 153), 6.0, 7.0); // Blue to cyan
	layer->colors.add_color (RGB (0, 71, 255), 7.0, 8.0);
	layer->colors.add_color (RGB (0, 0, 255), 8.0, 9.0);
	layer->colors.add_color (RGB (0, 184, 255), 9.0, 10.0);
	layer->colors.add_color (RGB (0, 255, 255), 10.0, 11.0);

	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER;
	color_panel->Refresh ();
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	}
	else {
		layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnRiverUpHydroSeq
	(wxCommandEvent &)
{
	layer->colors.clear ();

	color_panel->colors = &layer->colors;
	// UpHydroSeq
	layer->colors.add_color (RGB (255, 0, 255), 0.0, 1.0); // Magenta
	layer->colors.add_color (RGB (0, 0, 255), 1.0, 2.0); // Blue
	layer->colors.add_color (RGB (0, 255, 0), 2.0, 3.0); // Green
	layer->colors.add_color (RGB (112, 146, 190), 3.0, 4.0); // Navy blue
	layer->colors.add_color (RGB (127, 127, 127), 4.0, 5.0); // gray
	layer->colors.add_color (RGB (0, 255, 255), 5.0, 6.0); // cyan
	color_panel->Refresh ();
	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_UPHYDROSEQ;
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	}
	else {
		layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnRiverDnHydroSeq
	(wxCommandEvent &)
{
	layer->colors.clear ();
	color_panel->colors = &layer->colors;

	// DnHydroSeq
	layer->colors.add_color (RGB (255, 0, 255), 0.0, 1.0); // Magenta
	layer->colors.add_color (RGB (0, 0, 255), 1.0, 2.0); // Blue
	layer->colors.add_color (RGB (0, 255, 0), 2.0, 3.0); // Green
	layer->colors.add_color (RGB (112, 146, 190), 3.0, 4.0); // Navy blue
	layer->colors.add_color (RGB (127, 127, 127), 4.0, 5.0); // gray
	layer->colors.add_color (RGB (0, 255, 255), 5.0, 6.0); // cyan

	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ;
	color_panel->Refresh ();
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	}
	else {
		layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnRiverElevationMin
	(wxCommandEvent &)
{
	layer->colors.clear ();
	color_panel->colors = &layer->colors;

	// Elevation Min, 16 grayscale divisions
	for (int index = 0; index < 15; ++index)
		layer->colors.add_color (RGB (index * 16, index * 16, index * 16), index);

	layer->colors.add_color (RGB (255, 255, 255), 15);

	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_MINELEVM;

	color_panel->Refresh ();
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	}
	else {
		layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnRiverElevationMax
	(wxCommandEvent &)
{
	layer->colors.clear ();
	color_panel->colors = &layer->colors;

	// Elevation Min, 16 grayscale divisions
	for (int index = 0; index < 15; ++index)
		layer->colors.add_color (RGB (index * 16, index * 16, index * 16), index);

	layer->colors.add_color (RGB (255, 255, 255), 15);

	layer->data_attribute_index = NHD_SEGMENT_ATTRIBUTE_INDEX_MAXELEVM;

	color_panel->Refresh ();
	if (layer->column_names_numeric.size () > layer->data_attribute_index) {
		list_column_names_numeric->SetSelection (layer->data_attribute_index);
		layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	}
	else {
		layer->data_attribute_index = 0;
		layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	}
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnIntegerScale
	(wxCommandEvent &)

// Map object attribute field has integer values for a few distinct colors

{
	layer->colors.clear ();
	color_panel->colors = &layer->colors;


	layer->colors.add_color (RGB (192, 192, 192), 0.0, 1.0, "0");
	layer->colors.add_color (RGB (255, 0, 0), 1.0, 2.0, "1");
	layer->colors.add_color (RGB (0, 255, 0), 2.0, 3.0, "2");
	layer->colors.add_color (RGB (0, 0, 255), 3.0, 4.0, "3");
	layer->colors.add_color (RGB (255, 255, 0), 4.0, 5.0, "4");
	layer->colors.add_color (RGB (192, 192, 192), 5.0, 6.0, "5");

	// others not visible
	color_panel->Refresh ();
	layer->data_attribute_index = 0;
	layer->draw_as = MAP_OBJECT_DRAW_FILL_DATA_COLOR;
	combo_draw_as->SetSelection (layer->draw_as);
}

void dialog_edit_layer::OnSecchiDepth
	(wxCommandEvent &)

// 2016-11-03 points with Lake Erie Secchi Depth

// ave 3.82, std = ~2, max=13

{
	layer->colors.clear ();
	color_panel->colors = &layer->colors;

	layer->colors.add_color (RGB (0, 0, 0), 0.0, 0.01, "0<=x<0.01");
	layer->colors.add_color (RGB (128, 128, 128), 0.0, 0.01, "0.0<=x<1"); // gray
	layer->colors.add_color (RGB (128, 64, 0), 1.0, 1.5, "1<=x<1.5"); // brown
	layer->colors.add_color (RGB (255, 128, 0), 1.5, 2.0, "1.5<=x<2"); // orange
	layer->colors.add_color (RGB (128, 0, 64), 2.0, 3.0, "2<=x<3"); // dark magenta
	layer->colors.add_color (RGB (128, 0, 128), 3.0, 4.0, "3<=x<4"); // medium magenta
	layer->colors.add_color (RGB (0, 128, 128), 4.0, 5.0, "4<=x<5");
	layer->colors.add_color (RGB (255, 128, 64), 5.0, 6.0, "5<=x<6");
	layer->colors.add_color (RGB (0, 0, 255), 6.0, 8.0, "6<=x<8");
	layer->colors.add_color (RGB (0, 255, 255), 8.0, 10.0, "8<=x<10");
	layer->colors.add_color (RGB (0, 255, 0), 10.0, 15.0, "10<=x<15");

	// others not visible
	color_panel->Refresh ();
	layer->data_attribute_index = 4;
	layer->draw_as = MAP_OBJECT_DRAW_SYMBOL_FILL_AND_OUTLINE;
	combo_draw_as->SetSelection (layer->draw_as);
}

