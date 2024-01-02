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
#include "../util/filename_struct.h"
#include "../util/custom.h"
#include "../util/edit_time_point.h"
#include "../util/list.h"
#include "../util/Timestamp.h"
#include "../util/device_coordinate.h"
#include "dialog_map_layers.h"
// #include "../map/map_scale.h"
#include "../map/bounding_cube.h"
#include "../map/dynamic_map.h"

#include "../Collada/camera_time.h"
#include "../Collada/collada_animation.h"
#include "../Collada/collada_object.h"
#include "../Collada/collada_material.h"
#include "../Collada/collada_camera.h"
#include "../Collada/collada_light.h"
#include "../Collada/collada_scene.h"
#include "timed_measurements.h"
#include "collada_river_view.h"

dialog_map_layers::dialog_map_layers
	(wxWindow *pParent,
	dynamic_map *p_map)
	: wxDialog (pParent, -1, "Map Layers")
{
	map = p_map;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

 	wxBoxSizer *row;
	wxCheckBox *checkbox;
	wxComboBox *combo;
	wxStaticText *static_text;
	map_layer *layer;
	int layer_index, layer_count = map->size ();

	stack->AddSpacer (10);

	wxString point_combo_types [2];
	point_combo_types [0] = "Fixed Color";
	point_combo_types [1] = "Data Color";
	point_combo_types [2] = "Timed Data Size";

	layer = (map_layer *) map->first_member ();
	while (layer) {
		row = new wxBoxSizer (wxHORIZONTAL);
		checkbox = new wxCheckBox (this, wxID_ANY, layer->name.get_text_ascii (), wxDefaultPosition, wxDefaultSize);
		checkbox_layer_visible.push_back (checkbox);
		row->Add (checkbox, 0);
		row->AddSpacer (15);

		static_text = new wxStaticText (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize (350, 25));
		static_layer.push_back (static_text);
		row->Add (static_text);
		stack->Add (row);
		stack->AddSpacer (10);

		if (layer->type == MAP_OBJECT_POINT) {
			combo = new wxComboBox (this, wxID_ANY, L"", wxDefaultPosition, wxDefaultSize, 3, point_combo_types, wxCB_READONLY);
			combo_render.push_back (combo);
			row->Add (combo, 0, wxALL, 5);
		}
		else {
		}
		layer = (map_layer *) layer->next;
	}

	// button_royal_gorge = new wxButton (this, BUTTON_ROYAL_GORGE, wxT("Royal Gorge"));
	// row->Add (button_royal_gorge, 0, wxALL, 5);
	// stack->Add (row);

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

	layer = (map_layer *) map->first_member ();
	layer_index = 0;
	while (layer) {
		checkbox_layer_visible [layer_index]->SetValue (layer->visible);
		if (layer->type == MAP_OBJECT_POINT) {
			if (layer->draw_as == MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR)
				combo_render [layer_index]->SetSelection (0);
			else
				if (layer->draw_as == MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR)
					combo_render [layer_index]->SetSelection (1);
				else
					combo_render [layer_index]->SetSelection (2);
		}
		++layer_index;
		layer = (map_layer *) layer->next;
	}
}

BEGIN_EVENT_TABLE(dialog_map_layers, wxDialog)
	EVT_BUTTON (BUTTON_ROYAL_GORGE, dialog_map_layers::OnSetRoyalGorge)
	EVT_BUTTON (BUTTON_AMES, dialog_map_layers::OnSetAmes)
	EVT_BUTTON (BUTTON_RACCOON, dialog_map_layers::OnSetRaccoon)
END_EVENT_TABLE()

void dialog_map_layers::OnSetAmes
	(wxCommandEvent &)

{
}

void dialog_map_layers::OnSetRaccoon
	(wxCommandEvent &)

{
}

void dialog_map_layers::OnSetRoyalGorge
	(wxCommandEvent &)

{
}

void dialog_map_layers::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		map_layer *layer = (map_layer *) map->first_member ();
		int layer_index = 0;
		while (layer) {
			layer->visible = checkbox_layer_visible [layer_index]->GetValue ();
			if (layer->type == MAP_OBJECT_POINT) {
				switch (combo_render [layer_index]->GetSelection ()) {
					case 0:
						layer->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
						break;
					case 1:
						layer->draw_as = MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;
						break;
					// case 2:
						// Symbol size from Timed measurements
						// layer->draw_as = ;
				}
			}
			++layer_index;
			layer = (map_layer *) layer->next;
		}

	}
	wxDialog::EndModal (retCode);
}

