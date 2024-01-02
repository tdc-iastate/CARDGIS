#include "../util/utility_afx.h"
#include <vector>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/custom.h"
#include "../util/filename_struct.h"
#include "../util/RecentlyOpened.h"
#include "../util/dialog_error_list.h"
#include "../util/device_coordinate.h"
#include <deque>
#include <map>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../util/RecentlyOpenedShapefiles.h"
#include "dialog_upstream_downstream.h"

dialog_upstream_downstream::dialog_upstream_downstream
	(wxWindow *pParent,
	dynamic_map *p_map,
	bool *p_write_output,
	dynamic_string *p_filename_out,
	bool *p_save_to_layer,
	map_layer **p_map_layer)
	: wxDialog (pParent, -1, "Layer")
{
	filename_output = p_filename_out;
	write_output = p_write_output;
	save_to_layer = p_save_to_layer;
	save_map_layer = p_map_layer;
	map = p_map;

	wxBoxSizer *row, *stack = new wxBoxSizer (wxVERTICAL);

	stack->AddSpacer (10);

	check_write_output = new wxCheckBox (this, CHECK_HAS_NAME, wxT("Write to Output File"));
	stack->Add (check_write_output, 0, wxALL, 5);

	row = new wxBoxSizer (wxHORIZONTAL);
	button_browse_output_file = new wxButton (this, BUTTON_BROWSE_SHAPEFILE, wxT("Set Output Filename"));
	row->Add (button_browse_output_file, 0, wxALL, 5);
	row->AddSpacer (10);
	static_output_file = new wxStaticText (this, STATIC_OUTPUT_FILENAME, wxT(""), wxDefaultPosition, wxSize (500, 24));
	row->Add (static_output_file, 0, wxALL, 5);
	stack->Add (row);
	stack->AddSpacer (10);

	check_add_to_layer = new wxCheckBox (this, CHECK_HAS_ID, wxT("Add to Layer"));
	stack->Add (check_add_to_layer, 0, wxALL, 5);

	row = new wxBoxSizer (wxHORIZONTAL);
	combo_layer = new wxComboBox (this, COMBO_SELECT_LAYER, wxEmptyString, wxDefaultPosition, wxSize (250, 24), 0, 0, wxCB_READONLY);
	row->Add (new wxStaticText (this, wxID_ANY, wxT("Layer:")), 0, wxALL, 5);
	row->AddSpacer (10);
	fill_layer_combo (combo_layer, *save_map_layer);
	combo_layer->SetSelection (0);
	row->Add (combo_layer, 0, wxALL, 5);
	stack->Add (row, 1, wxEXPAND | wxALIGN_LEFT | wxALIGN_RIGHT);

	stack->AddSpacer (20);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));
	SetSizerAndFit(stack);

	check_write_output->SetValue (*write_output);
	check_add_to_layer->SetValue (*save_to_layer);
	static_output_file->SetLabel (filename_output->get_text ());
}

BEGIN_EVENT_TABLE(dialog_upstream_downstream, wxDialog)
	EVT_BUTTON (BUTTON_BROWSE_SHAPEFILE, dialog_upstream_downstream::OnBrowseOutputFile)
	EVT_BUTTON (IDCANCEL, dialog_upstream_downstream::OnIdcancel)
END_EVENT_TABLE()

void dialog_upstream_downstream::fill_layer_combo
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

void dialog_upstream_downstream::OnSelChangePreset
	(wxCommandEvent &)

{
}

void dialog_upstream_downstream::enable_windows ()
{
	if (check_write_output->GetValue () != false)
		button_browse_output_file->Enable (true);
	else
		button_browse_output_file->Enable (false);

	if (check_add_to_layer->GetValue () != false)
		combo_layer->Enable (true);
	else
		combo_layer->Enable (false);
}

void dialog_upstream_downstream::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		*write_output = check_write_output->GetValue ();
		*filename_output = static_output_file->GetLabel ().ToAscii ();

		*save_to_layer = check_add_to_layer->GetValue ();
		if (*save_to_layer) {
			int index;

			if ((index = combo_layer->GetSelection ()) != -1)
				*save_map_layer = map->layers [index];
		}
	}
	wxDialog::EndModal (retCode);
}

void dialog_upstream_downstream::OnBrowseOutputFile
	(wxCommandEvent &)
{
	wxFileDialog* openFileDialog = new wxFileDialog( this, _("Open file"), "", "", "Text File (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
 
	openFileDialog->SetDirectory ("r:\\dkeiser\\updown\\iowa\\");
	openFileDialog->SetFilename ("IowaLakesUpstreamDownstream.txt");
	if ( openFileDialog->ShowModal() == wxID_OK ) {

		*filename_output = openFileDialog->GetDirectory().ToAscii ();
		*filename_output += PATH_SEPARATOR;
		*filename_output += openFileDialog->GetFilename().ToAscii ();
		static_output_file->SetLabel (filename_output->get_text ());
	}
}

void dialog_upstream_downstream::OnIdcancel
	(wxCommandEvent &)
{
}
