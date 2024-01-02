
const int SWAT_ANIMATOR_INPUT_CSV = 1;
const int SWAT_ANIMATOR_INPUT_HSL = 2;
const int SWAT_ANIMATOR_INPUT_ODBC = 3;
const int SWAT_ANIMATOR_INPUT_REACH = 4; // OUTPUT.RCH file from run

class swat_animator {
	public:
		int target_subbasin;
		dynamic_string target_column_name_1, target_column_name_2;
		dynamic_string title_text_1, title_text_2;
		dynamic_string output_filename_prefix;
		bounding_cube override_logical_window;
		bool show_diffs;
		int input_source;

		swat_animator ();
		virtual ~swat_animator ();
		bool process
			(dynamic_string &filename_data,
			const dynamic_string &output_path,
			const BYTE y_m_d,
			class dynamic_map *map_watershed,
			class map_layer *layer,
			class panel_shapefile *,
			class interface_window *view,
			dynamic_string &log);
		bool process_side_by_side
			(dynamic_string &filename_data_1,
			dynamic_string &filename_data_2,
			const BYTE y_m_d,
			const bool unified_scale,
			const dynamic_string &output_path,
			class dynamic_map *local_map_1,
			class dynamic_map *local_map_2,
			class map_layer *layer_1,
			class map_layer *layer_2,
			class panel_shapefile *,
			class interface_window *view,
			dynamic_string &log);
		bool process_image
			(dynamic_string &filename_data,
			const BYTE y_m_d,
			const bool monthly_average,
			const dynamic_string &output_path,
			class dynamic_map *map_watershed,
			class map_layer *layer,
			class panel_shapefile *,
			class interface_window *view,
			dynamic_string &log);
		bool process_image_side_by_side
			(dynamic_string &filename_data_1,
			dynamic_string &filename_data_2,
			const BYTE y_m_d,
			const bool unified_scale,
			const bool monthly_average,
			const dynamic_string &output_path,
			class dynamic_map *local_map_1,
			class dynamic_map *local_map_2,
			class map_layer *layer_1,
			class map_layer *layer_2,
			class panel_shapefile *,
			class interface_window *view,
			dynamic_string &log);
	protected:
		arbitrary_count ranger_1, ranger_2;
		Timestamp earliest_date, latest_date;

		void add_target_highlight
			(dynamic_map *map,
			const long target_subbasin,
			map_layer *data_layer);
		bool animate_polygons_by_level
			(std::map <int, std::map <long, double>> *data_by_input_level,
			const dynamic_string &output_path,
			class dynamic_map *map,
			class map_layer *layer,
			class interface_window *view,
			class dynamic_string &log);

		bool compute_average
			(std::map <Timestamp, std::map <long, double>> *map_data_by_time,
			const int denominator,
			const bool monthly_average,
			class arbitrary_count *ranger,
			class autorange_histogram *,
			std::map <long, double> *average_by_subbasin,
			dynamic_string &log);

		bool draw_map_on_bitmap
			(wxBitmap &bitmap,
			const wxRect &box,
			class dynamic_map *map,
			dynamic_string &error_message);
		void make_bins
			(const int bin_count,
			const std::map <Timestamp, std::map <long, double>> *map_data);
		bool read_data
			(dynamic_string &filename_data,
			const dynamic_string &match_column_name,
			const BYTE y_m_d,
			std::map <Timestamp, std::map <long, double>> *map_data,
			class arbitrary_count *ranger,
			class autorange_histogram *,
			class map_layer *layer,
			dynamic_string &log);
		bool read_data_odbc
			(dynamic_string &filename_data,
			const dynamic_string &match_column_name,
			std::map <int, std::map <long, double>> *map_data,
			class arbitrary_count *ranger,
			class autorange_histogram *,
			class map_layer *layer,
			dynamic_string &log);
		bool read_daily_hsl
			(dynamic_string &filename_data,
			const dynamic_string &match_column_name,
			const BYTE y_m_d,
			std::map <Timestamp, std::map <long, double>> *map_data,
			class arbitrary_count *ranger,
			class autorange_histogram *,
			class map_layer *layer,
			dynamic_string &log);

		bool render_frame
			(const Timestamp &when,
			const BYTE y_m_d,
			std::map <Timestamp, std::map <long, double>> *map_data,
			class dynamic_map *map_watershed,
			class map_layer *layer,
			dynamic_string &log);

		bool render_frame_polygon_data
			(const std::map <long, double> *,
			class dynamic_map *map_watershed,
			class map_layer *layer,
			dynamic_string &log);

		void set_colors_no3
			(class map_layer *layer);
		void set_layer_pallette
			(class map_layer *layer,
			class arbitrary_count *ranger,
			class autorange_histogram *,
			dynamic_string &log);
};
