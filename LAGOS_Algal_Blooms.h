
// MapLayerProximity callback functions
void zillow_point_callback
	(const map_object *point,
	const char delimiter,
	dynamic_string &log);
void nhd_river_callback
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log);
void nhd_river_column_headers_callback
	(const char delimiter,
	dynamic_string &log);
void nhd_lake_callback
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log);
void nhd_lake_column_headers_callback
	(const char delimiter,
	dynamic_string &log);


class LAGOS_chla_reading {
	public:
		Timestamp when;
		long lake_id;
		double value;

		LAGOS_chla_reading ();
		LAGOS_chla_reading
			(const LAGOS_chla_reading &);
		LAGOS_chla_reading operator =
			(const LAGOS_chla_reading &other);
		void copy
			(const LAGOS_chla_reading &);
};

class LAGOS_Algal_Blooms {
	public:
		class map_layer *layer_lagos, *layer_output;
		LAGOS_Algal_Blooms ();
		virtual ~LAGOS_Algal_Blooms ();

		std::map <long, std::vector <LAGOS_chla_reading>> chla_by_lake;

		double average_across_months
			(std::vector <LAGOS_chla_reading> *lake_vector,
			const Timestamp *sale_date,
			const int number_of_months);
		void compute_chla_averages
			(class map_layer *zillow_sales,
			class interface_window *view,
			dynamic_string &log);

		bool read_shapefile
			(const dynamic_string &filename_lagos_shapefile,
			class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);
		bool read_chla
			(const dynamic_string &filename_lagos_shapefile,
			class interface_window *view,
			dynamic_string &log);
		void dump_chla
			(dynamic_string &log);

		bool match_lakes
			(class map_layer *layer_lakes,
			class interface_window *view,
			dynamic_string &log);

		void make_output_layer
			(class dynamic_map *,
			class map_layer *layer_lakes);
		bool write_output_layer
			(const dynamic_string &,
			class interface_window *view,
			dynamic_string &);
};

