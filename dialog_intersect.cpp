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
#include "../util/filename_struct.h"
#include "../util/listbox_files.h"
#include "../util/device_coordinate.h"
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
#include "dialog_intersect.h"
#include "../i_swat_database/SWATDeclarations.h"


BEGIN_EVENT_TABLE(dialog_intersect, wxDialog)
	EVT_BUTTON(BUTTON_NYS, dialog_intersect::NewYork)
END_EVENT_TABLE()

dialog_intersect::dialog_intersect
	(wxWindow *pParent,
	dynamic_map *p_map,
	map_layer **p_selection_1,
	map_layer **p_selection_2)
	: wxDialog(pParent, -1, "Select intersecting layers")
{
	map = p_map;
	selection_1 = p_selection_1;
	selection_2 = p_selection_2;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* preset_box = new wxStaticBoxSizer(wxHORIZONTAL, this, "Project Presets");
	preset_box->Add(new wxButton(this, BUTTON_NYS, _("NYS Ag sales vs Urban Areas"), wxDefaultPosition, wxDefaultSize), 0, wxALL, 5);

	stack->Add(preset_box);
	stack->AddSpacer(25);

	wxBoxSizer * row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Layer 1:")), 0, wxALL, 5);
	row->AddSpacer (10);
	combo_layer_1 = new wxComboBox(this, COMBO_SELECT_LAYER_1, wxEmptyString, wxDefaultPosition, wxSize(500, 24), 0, 0, wxCB_DROPDOWN);
	fill_layer_combo (combo_layer_1, *selection_1);
	combo_layer_1->SetSelection (0);
	row->Add (combo_layer_1, 0, wxALL, 5);
	stack->Add (row);

	row = new wxBoxSizer(wxHORIZONTAL);
	row->Add(new wxStaticText(this, wxID_ANY, wxT("Layer 2:")), 0, wxALL, 5);
	row->AddSpacer(10);
	combo_layer_2 = new wxComboBox(this, COMBO_SELECT_LAYER_2, wxEmptyString, wxDefaultPosition, wxSize(500, 24), 0, 0, wxCB_DROPDOWN);
	fill_layer_combo(combo_layer_2, *selection_2);
	combo_layer_2->SetSelection(1);
	row->Add(combo_layer_2, 0, wxALL, 5);
	stack->Add(row);


	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

}


void dialog_intersect::fill_layer_combo
	(wxComboBox *combo,
	const map_layer *selected)

{
	int index;
	std::vector <map_layer*>::const_iterator layer;

	combo->Clear ();
	for (layer = map->layers.begin();
	layer != map->layers.end();
	++layer) {
		index = combo->Append ((*layer)->name.get_text_ascii (), (void *) *layer);
		if (*layer == selected)
			combo->SetSelection (index);
	}
}

void dialog_intersect::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		int index;

		if ((index = combo_layer_1->GetSelection()) != -1)
			*selection_1 = map->layers[index];
		if ((index = combo_layer_2->GetSelection()) != -1)
			*selection_2 = map->layers[index];
	}
	wxDialog::EndModal (retCode);
}

map_layer* dialog_intersect::ag_sale_circles(dynamic_string& log)

{
	map_layer* layer_circles;
	importer_shapefile shapefile;

	layer_circles = map->create_new(MAP_OBJECT_POLYGON);
	layer_circles->name = "NYS Ag Sales";
	layer_circles->color = RGB(0, 255, 0);
	layer_circles->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;

	shapefile.filename_source = "H:\\NewYorkAgSales\\NYS_Ag_Sales_08102023 Circles\\FarmlandFinder plus-3-mile radius.shp";

	shapefile.id_field_name = "ID";
	shapefile.name_field_name = "Name";

	shapefile.projection_from_prj_file = true;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	if (shapefile.import(layer_circles, NULL, map, NULL, log)) {
		map->layers.push_back(layer_circles);
		return layer_circles;
	}
	else {
		delete layer_circles;
		return NULL;
	}
}

map_layer* dialog_intersect::urban_areas(dynamic_string& log)

{
	map_layer* layer_cities;
	importer_shapefile shapefile;

	layer_cities = map->create_new(MAP_OBJECT_POLYGON);
	layer_cities->name = "Urban Areas";
	layer_cities->color = RGB(0, 255, 255);
	layer_cities->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;

	shapefile.filename_source = "H:\\NewYorkAgSales\\tl_rd22_us_uac20_WGS1984\\tl_rd22_us_uac20_WGS1984.shp";

	shapefile.id_field_name = "UACE20";
	shapefile.name_field_name = "NAME20";

	shapefile.projection_from_prj_file = true;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	if (shapefile.import(layer_cities, NULL, map, NULL, log)) {
		map->layers.push_back(layer_cities);
		return layer_cities;
	}
	else {
		delete layer_cities;
		return NULL;
	}
}

void dialog_intersect::NewYork (wxCommandEvent&)


// 2023-08-17 NYS ag sales vs urban areas

{
	map_layer *layer_circles, *layer_cities;
	dynamic_string log;

	SetCursor(*wxHOURGLASS_CURSOR);

	if ((layer_circles = ag_sale_circles (log)) != NULL) {
		fill_layer_combo(combo_layer_1, layer_circles);

		if ((layer_cities = urban_areas(log)) != NULL) {
			fill_layer_combo(combo_layer_2, layer_cities);
		}
	}
	SetCursor(wxNullCursor);

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"NYS", L"", log);
	d.ShowWindowModal();
}


