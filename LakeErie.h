
class matrix_averager {

	// Accumulate & report average boating distance by zipcode and ODNR_Region

	public:
		void add_to
			(const long row,
			const short column,
			const double value);

		void write
			(dynamic_string &report) const;
	protected:
		std::map <long, std::map <short, std::pair <long, double>>> data;
		std::set <short> all_columns;
};

class LakeErie {
	public:
		dynamic_string filename_marina_db;
		void read
			(class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);
		void assign_grids
			(class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);
		void marina_distance
			(class dynamic_map *map,
			const bool all_combinations,
			class interface_window *view,
			dynamic_string &log);

		class map_layer *read_grid_shapefile
			(class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);

		class map_layer *read_contour_shapefile
			(class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);

		bool report_angler_trips_by_gridcode
			(class interface_window *view,
			dynamic_string &log);
		bool report_grid_counts
			(const class map_layer *layer_grid,
			const class card_bitmap_tiff *tiff_image,
			dynamic_string *filename_grid_report,
			dynamic_string *filename_point_report,
			dynamic_string &log);
		void report_relevent_travel
			(class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);
	protected:
		bool read_marina_grid_pairs
			(class odbc_database *db,
			std::vector <std::pair <long, long>> *marina_grid_pairs,
			dynamic_string &log);
		bool read_grid_regions
			(class odbc_database *db,
			std::map <long, short> *grid_regions,
			dynamic_string &log);
		bool read_zip_to_marina
			(class odbc_database *db,
			std::map <long, std::map <long, double>> *zip_marina_miles,
			dynamic_string &log);

		bool read_marina_layer
			(class odbc_database *db,
			class dynamic_map *map,
			dynamic_string &log);
};

