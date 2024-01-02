const int NUMBER_OF_RIVER_VIEW_AREA_TYPES = 4;
const int RIVER_VIEW_AREA_DEFINED_BOX = 1;
const int RIVER_VIEW_AREA_SOURCE_OUTLET = 2;
const int RIVER_VIEW_AREA_SOURCE_UPSTREAM = 3;
const int RIVER_VIEW_ALL_AREA = 4;

// const int FRAMES_PER_DAY = 4;
// const int SECONDS_PER_FRAME = 21600;
// const int HOURS_PER_FRAME = 6;

void set_e85_colors
	(color_set *colors,
	const double min,
	const double max,
	const double mean,
	const double std);

class river_view_map {
	public:
		double days_per_frame, hours_per_frame;
		int seconds_per_frame, frames_per_second;
		int area_definition;
		long start_id, destination_id;
		bounding_cube map_boundary;
		std::chrono::system_clock::time_point start_date, end_date;
		double amplification_stream, amplification_station;
		bool simplify;
		std::vector <bool> render_layers;
		std::vector <double> layer_symbol_size;
		std::set <long> plotting_streams;
		std::map <dynamic_string, std::set <map_object *>> plotting_objects;

		river_view_map ();

		void add_flow_stream
			(std::map <long, stream_measurements> *stream_time_changes,
			const class router_NHD *router,
			const map_layer *layer_rivers,
			const long comid,
			const int polygon_index,
			const int point_index,
			const class timed_measurements *station_reading,
			const class bounding_cube *map_boundary,
			dynamic_string &log);
		void add_streamflow_at_node
			(std::map <long, stream_measurements> *stream_time_changes,
			const class map_object *segment,
			const int polygon_index,
			const int point_index,
			const std::chrono::hours &time_offset,
			const class timed_measurements *station_reading,
			dynamic_string &log);
		bool add_polygons
			(class dynamic_map *map,
			std::chrono::system_clock::time_point &when,
			class map_layer *,
			std::vector <long> *trail,
			const bool use_layer_color,
			const class elevation_matrix *elevations,
			class interface_window *view,
			dynamic_string &log);
		bool build_stream
			(class dynamic_map *map,
			std::chrono::system_clock::time_point &when,
			class map_layer *,
			std::set <long> *trail,
			std::map <long, stream_measurements> *time_changes,
			class elevation_matrix *elevations,
			class interface_window *view,
			dynamic_string &log);
		bool add_vectors
			(class dynamic_map *map,
			std::chrono::system_clock::time_point &when,
			class map_layer *,
			class bounding_cube *stream_area,
			const class elevation_matrix *elevations,
			class interface_window *view,
			dynamic_string &log);
		virtual bool build_points
			(class dynamic_map *map,
			std::chrono::system_clock::time_point &when,
			std::set <map_object *> *stations,
			const char *node_name_prefix,
			const double base_size,
			// std::map <long, timed_measurements> *time_changes,
			class elevation_matrix *elevations,
			const bool use_names,
			const bool text_labels,
			class interface_window *view,
			dynamic_string &log);

		/*
		void create_map
			(class dynamic_map *,
			std::chrono::system_clock::time_point &when,
			dynamic_map *map_watershed,
			std::map <long, stream_measurements> *stream_time_changes,
			class elevation_matrix *elevations,
			class interface_window *view,
			dynamic_string &log);
			*/

		virtual bool set_point_data
			(std::chrono::system_clock::time_point &when,
			std::chrono::hours &interval,
			class map_layer *points,
			std::vector <map_object *> *zero_holder,
			const double base_size,
			const bool use_names,
			const bool text_labels,
			class interface_window *view,
			dynamic_string &log);

		virtual void describe_run
			(dynamic_string &);

		void feed_streams
			(std::map <long, stream_measurements> *stream_time_changes,
			// const std::map <long, timed_measurements> *point_sampling_data,
			const class map_layer *layer_storet_legacy,
			const class router_NHD *router,
			const class map_layer *layer_rivers,
			const class bounding_cube *map_boundary,
			dynamic_string &log);

		void insert_relevant_points
			(std::set <map_object *> *plotting_stations,
			const std::set <long> *plotting_streams,
			const class map_layer *layer,
			class interface_window *view,
			dynamic_string &log);

		void insert_relevant_streams
			(const class map_layer *layer,
			class router_NHD *router,
			class map_object **outlet,
			class interface_window *view,
			dynamic_string &log);

		virtual bool read_gridfloat_heatmap
			(const dynamic_string &filename_flt,
			const dynamic_string &filename_image,
			const dynamic_string &filename_dae,
			const bounding_cube &boundary,
			class elevation_matrix *elevations,
			const bool create_image,
			class interface_window *view,
			dynamic_string &log);

		bool read_sampling_data
			(const char *filename_db_readings,
			std::set <map_object *> *plotting_stations,
			const std::chrono::system_clock::time_point &start_date,
			const std::chrono::system_clock::time_point &end_date,
			// std::map <long, timed_measurements> *time_changes,
			long *count_samples,
			class interface_window *view,
			dynamic_string &log);

		void simulate_flow
			(std::map <long, stream_measurements> *time_changes,
			std::set <long> *trail_set,
			const long start_id,
			const long destination_id,
			const class router_NHD *router,
			const map_layer *layer_rivers,
			dynamic_string &log);
		
		int stream_size
			(const class map_object *);

	protected:

		double distance_2d_xy
			(const logical_coordinate &start,
			const logical_coordinate &end);
		double distance_2d_xz
			(const logical_coordinate &start,
			const logical_coordinate &end);
		double distance_3d
			(const logical_coordinate &start,
			const logical_coordinate &end);

		bool nodes_ahead
			(const class map_object *segment,
			const int polygon_index,
			const int node_index,
			class map_object **segment_ahead,
			int *polygon_index_ahead,
			int *node_index_ahead,
			std::vector <long long>::iterator *current_comid,
			std::vector <long long> *downstream_ids,
			const class map_layer *river_layer);
		bool read_gridfloat_header
			(dynamic_string &filename_hdr,
			int *count_columns,
			int *count_rows,
			double *ll_latitude,
			double *ll_longitude,
			double *cellsize,
			float *missing_value,
			dynamic_string &log);
		void set_colors
			(class color_set *colors,
			const double min,
			const double max,
			const double mean,
			const double std);
		void simulate_flow_stream
			(std::map <long, stream_measurements> *time_changes,
			const class router_NHD *router,
			const map_layer *layer_rivers,
			const long comid,
			const long end_comid,
			dynamic_string &log);
};

