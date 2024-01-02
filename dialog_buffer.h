
class dialog_buffer : public wxDialog {
	public:
		enum {
			ID_BUTTON_FILENAME_POINTS,
			COMBO_FORMAT
		};
		dialog_buffer
			(class wxWindow *,
			class dynamic_map *p_map,
			class map_layer **p_facility_layer,
			class map_object **p_facility,
			class map_layer **p_census_layer,
			std::vector <double> *p_buffer_widths,
			double *p_start_upstream_miles,
			double *p_length_upstream_miles,
			double *p_start_downstream_miles,
			double *p_length_downstream_miles,
			double *p_cut_line_length_miles);

		double_edit *edit_start_upstream, *edit_length_upstream, *edit_start_downstream, *edit_length_downstream, *edit_cut_line_length;
		wxComboBox *combo_facility_layer, *combo_polygon_layer;
		long_integer_edit *edit_facility;
		wxTextCtrl *edit_buffer_width;

	protected:
		class dynamic_map *map;
		map_layer **facility_layer, **census_layer;
		class map_object **facility;
		std::vector <double> *buffer_widths;
		double *start_upstream_miles, *length_upstream_miles, *start_downstream_miles, *length_downstream_miles, *cut_line_length_miles;

		virtual void EndModal
			(int retCode);

		void fill_layer_combo
			(class wxComboBox *combo,
			const class map_layer *selected);

	DECLARE_EVENT_TABLE()
};
