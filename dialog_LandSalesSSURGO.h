
class dialog_LandSalesSSURGO : public wxDialog {
	public:
		enum {
			ID_MAP = 103,
			COMBO_SELECT_LAYER_START,
			COMBO_SELECT_LAYER_FINISH,
			ID_STATIC_MAP_STATUS,
			BUTTON_BROWSE_SHAPEFILE,
			BUTTON_BROWSE_OUTPUT
		};
		dialog_LandSalesSSURGO
			(class wxWindow *,
			dynamic_string* sales_shapefile,
			dynamic_string* p_filename_output,
			int *statefips,
			int *p_first_county_fips,
			bool *auction,
			bool *cleanup);

		class integer_edit *edit_state_fips, *edit_first_county_fips;
		class wxStaticText *static_filename, * static_filename_output;
		wxCheckBox* check_auction_results, *check_remove_county_files;
		void OnBrowseSalesShapefile(wxCommandEvent&);
		void OnBrowseOutputFile(wxCommandEvent&);

	protected:
		class dynamic_string *path_sales_shapefile, *filename_output;
		int *run_state;
		int *first_county_fips;
		bool* auction_results, * remove_county_files;

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
