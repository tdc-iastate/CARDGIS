	
class dialog_match_attribute : public wxDialog {
	public:
		enum {
			ID_FILENAME_OUTPUT_VARIABLES = 103,
			RADIO_TEXT,
			RADIO_NUMERIC
		};
		dialog_match_attribute
			(class wxWindow*,
			class map_layer* p_selected_layer,
			std::set <long long>* p_new_selection);

		class wxTextCtrl *edit_value;
		wxStaticText *static_layer_name;
		class wxComboBox *combo_attributes_text, *combo_attributes_numeric;
		class wxRadioButton *radio_text, *radio_numeric;

		void OnText (wxCommandEvent &);
		void OnNumeric (wxCommandEvent &);

	protected:
		// int *numeric_index, *text_index;
		class map_layer *selected_layer;
		// dynamic_string *match_value;
		std::set <long long>* new_selection;

		void fill_attribute_combo
			(class wxComboBox *combo,
			const std::vector <dynamic_string> *column_names);
		bool find_target
			(const int numeric_index,
			const int text_index,
			dynamic_string& match_value,
			dynamic_string &log);

		virtual void EndModal
			(int retCode);
	DECLARE_EVENT_TABLE()
};
