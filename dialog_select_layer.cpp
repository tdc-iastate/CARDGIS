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
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "dialog_select_layer.h"
#include "../i_swat_database/SWATDeclarations.h"

dialog_select_layer::dialog_select_layer
	(wxWindow *pParent,
	const wxString &prompt,
	dynamic_map *p_map,
	map_layer **p_selection)
	: wxDialog (pParent, -1, prompt)
{
	map = p_map;
	selection = p_selection;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer * row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Layer:")), 0, wxALL, 5);
	row->AddSpacer (10);
	combo_layer = new wxComboBox(this, COMBO_SELECT_LAYER, wxEmptyString, wxDefaultPosition, wxSize(450, 24), 0, 0, wxCB_DROPDOWN);
	fill_layer_combo (combo_layer, *selection);
	combo_layer->SetSelection (0);
	row->Add (combo_layer, 0, wxALL, 5);
	stack->Add (row);

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));

	SetSizerAndFit(stack);

}

BEGIN_EVENT_TABLE(dialog_select_layer, wxDialog)
END_EVENT_TABLE()

void dialog_select_layer::fill_layer_combo
	(wxComboBox *combo,
	const map_layer *selected)

{
	int index;
	std::vector <map_layer*>::const_iterator layer;

	for (layer = map->layers.begin();
	layer != map->layers.end();
	++layer) {
		index = combo->Append ((*layer)->name.get_text_ascii (), (void *) *layer);
		if (*layer == selected)
			combo->SetSelection (index);
	}
}

void dialog_select_layer::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		int index;

		if ((index = combo_layer->GetSelection ()) != -1)
			*selection = map->layers [index];
	}
	wxDialog::EndModal (retCode);
}


