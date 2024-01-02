#pragma once

const int ZILLOW_FIRST_YEAR = 2009;
const int ZILLOW_LAST_YEAR = 2018;
const int ZILLOW_YEAR_COUNT = 10;

void zillow_point_callback_3
	(const map_object *point,
	const char delimiter,
	dynamic_string &log);

class ZillowSales {
	public:
		int thread_count;
		class map_layer *layer_zillow[ZILLOW_YEAR_COUNT];
		class map_layer *layer_ecoli;

		ZillowSales ();
		virtual ~ZillowSales ();
		bool load_zillow_sales
			(const dynamic_string &path,
			const int first_year,  // -1 for all
			const int last_year,
			const bool add_timestamp,
			dynamic_map *map_watershed,
			class interface_window *view,
			dynamic_string &error_message);
		bool read_csv
			(const dynamic_string &,
			const int first_record,
			const int last_record,
			class CountyMaster *,
			dynamic_map *map_watershed,
			class interface_window *view,
			dynamic_string &error_message);
		bool read_csv_state
			(const dynamic_string &,
			class CountyMaster *,
			dynamic_map *map_watershed,
			class interface_window *view,
			dynamic_string &error_message);
		bool read_csv_format_3
			(const dynamic_string &,
			const int state_fips,
			const int first_record,
			const int last_record,
			class CountyMaster *,
			dynamic_map *map_watershed,
			class interface_window *view,
			dynamic_string &error_message);
		bool load_EColi_outbreaks
			(const dynamic_string &filename_ecoli,
			dynamic_map *map_watershed,
			const int record_limit,
			class interface_window *view,
			dynamic_string &error_message);

		bool homes_in_5_miles
			(dynamic_map *map_watershed,
			dynamic_string &filename_output,
			const char &delimiter,
			class interface_window *view,
			dynamic_string &error_message);

		bool outbreaks_in_1000m
			(class map_layer *layer_rivers,
			class map_layer *layer_nhd_lakes,
			dynamic_map *map_watershed,
			dynamic_string &filename_output,
			class interface_window *view,
			dynamic_string &error_message);

		bool nearest_outbreak
			(class interface_window *view,
			dynamic_string &log);
		bool nearest_water_to_sale
			(class map_layer *layer_rivers,
			class map_layer *layer_nhd_lakes,
			class interface_window *view,
			dynamic_string &log);
		bool nearest_water_to_outbreak
			(class map_layer *layer_rivers,
			class map_layer *layer_nhd_lakes,
			class interface_window *view,
			dynamic_string &log);

	protected:
		void make_colors (class color_set *);

};
