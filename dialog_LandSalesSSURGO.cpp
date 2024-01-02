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
#include "../util/custom.h"
#include "../util/device_coordinate.h"
#include "../util/bounding_cube.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
// #include "FarmlandFinder.h"
#include "dialog_LandSalesSSURGO.h"

dialog_LandSalesSSURGO::dialog_LandSalesSSURGO
	(wxWindow *pParent,
	dynamic_string *sales_shapefile,
	dynamic_string *p_filename_output,
	int *p_run_state,
	int *p_first_county_fips,
	bool *auction,
	bool *cleanup)
	: wxDialog (pParent, -1, "Land Sales to SSURGO")
{
	path_sales_shapefile = sales_shapefile;
	filename_output = p_filename_output;
	run_state = p_run_state;
	first_county_fips = p_first_county_fips;
	auction_results = auction;
	remove_county_files = cleanup;

	wxBoxSizer *stack = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
	stack->AddSpacer(10);
	row->Add(new wxStaticText(this, wxID_ANY, wxT("State FIPS:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer(10);
	edit_state_fips = new integer_edit(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	row->Add(edit_state_fips);
	stack->Add(row);

	row = new wxBoxSizer(wxHORIZONTAL);
	stack->AddSpacer(10);
	row->Add(new wxStaticText(this, wxID_ANY, wxT("First County FIPS:")), 0, wxALIGN_BOTTOM);
	row->AddSpacer(10);
	edit_first_county_fips = new integer_edit(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	row->Add(edit_first_county_fips);
	stack->Add(row);

	stack->AddSpacer (10);
	row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxButton (this, BUTTON_BROWSE_SHAPEFILE, wxT("Set")), 0, wxALIGN_BOTTOM);
	row->AddSpacer (10);
	static_filename = new wxStaticText (this, wxID_ANY, path_sales_shapefile->get_text (), wxDefaultPosition, wxSize (500, 24));
	row->Add (static_filename);
	stack->Add (row);

	stack->AddSpacer(10);
	row = new wxBoxSizer(wxHORIZONTAL);
	row->Add(new wxButton(this, BUTTON_BROWSE_OUTPUT, wxT("Set")), 0, wxALIGN_BOTTOM);
	row->AddSpacer(10);
	static_filename_output = new wxStaticText(this, wxID_ANY, filename_output->get_text(), wxDefaultPosition, wxSize(500, 24));
	row->Add(static_filename_output);
	stack->Add(row);

	stack->AddSpacer(10);
	row = new wxBoxSizer(wxHORIZONTAL);
	check_auction_results = new wxCheckBox(this, wxID_ANY, L"Auction Results");
	row->Add(check_auction_results);
	stack->Add(row, 0, wxALL, 5);

	stack->AddSpacer(10);
	row = new wxBoxSizer(wxHORIZONTAL);
	check_remove_county_files = new wxCheckBox(this, wxID_ANY, L"Remove intermediate county files");
	row->Add(check_remove_county_files);
	stack->Add(row, 0, wxALL, 5);

	stack->AddSpacer (25);
	stack->Add (CreateButtonSizer (wxOK | wxCANCEL));
	SetSizerAndFit(stack);

	edit_state_fips->Set (*run_state);
	edit_first_county_fips->Set (*first_county_fips);
	check_auction_results->SetValue(*auction_results);
	check_remove_county_files->SetValue (*remove_county_files);

	// static_filename->SetLabel (path_sales_shapefile->get_text ());
}

BEGIN_EVENT_TABLE(dialog_LandSalesSSURGO, wxDialog)
	EVT_BUTTON (BUTTON_BROWSE_SHAPEFILE, dialog_LandSalesSSURGO::OnBrowseSalesShapefile)
	EVT_BUTTON (BUTTON_BROWSE_OUTPUT, dialog_LandSalesSSURGO::OnBrowseOutputFile)
END_EVENT_TABLE()

void dialog_LandSalesSSURGO::EndModal
	(int retCode)

{
	if (retCode == wxID_OK) {
		*run_state = edit_state_fips->Get ();
		*first_county_fips = edit_first_county_fips->Get();
		*auction_results = check_auction_results->GetValue();
		*remove_county_files = check_remove_county_files->GetValue();
	}
	wxDialog::EndModal (retCode);
}

void dialog_LandSalesSSURGO::OnBrowseSalesShapefile
	(wxCommandEvent&)

{
	if (get_read_path(path_sales_shapefile, L"ShapeFiles (*.shp)|*.shp", this))
		static_filename->SetLabel(path_sales_shapefile->get_text());
}

void dialog_LandSalesSSURGO::OnBrowseOutputFile
	(wxCommandEvent &)

{
	if (get_write_path (filename_output, L"Output File (*.csv)|*.csv", L"csv", this))
		static_filename_output->SetLabel (filename_output->get_text ());
}

