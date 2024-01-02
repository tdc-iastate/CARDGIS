const double STATION_BASE_SIZE = 20.0;

const int NUMBER_OF_COLLADA_CAMERA_TYPES = 5;
const int COLLADA_CAMERA_NONE = 0;
const int COLLADA_CAMERA_FIXED = 1;
const int COLLADA_CAMERA_ORBIT = 2;
const int COLLADA_CAMERA_SPACE_APPROACH = 3;
const int COLLADA_CAMERA_FOLLOW_STREAM = 4;

bool set_angles_camera
	(const logical_coordinate &startpoint,
	const logical_coordinate &endpoint,
	const logical_coordinate up_vector,
	double angle_radians [],
	const bool verbose,
	dynamic_string &log);

class river_view_collada : public river_view_map {
	public:
		collada_scene scene;
		int camera_type;
		double min_elevation_meters, max_elevation_meters; // set by both build_stream and read_gridflow_heatmap
		dynamic_string python_code;

		river_view_collada ();

		bool add_obelisk
			(const logical_coordinate &nw_corner,
			const double height_meters,
			class interface_window *view,
			dynamic_string &log);

		void build_lat_lon_grid
			(class dynamic_map *map_watershed,
			class interface_window *view,
			dynamic_string &log);
		virtual bool add_polygons
			(class map_layer *,
			std::set <map_object *> *objects_to_plot,
			const bool use_layer_color,
			const class elevation_matrix *elevations,
			class interface_window *view,
			dynamic_string &log);
		bool build_boundary_box
			(class bounding_cube *area,
			class interface_window *view,
			dynamic_string &log);
		bool build_camera_source_outlet
			(const map_object *source,
			const map_object *,
			class bounding_cube *stream_area,
			const std::chrono::system_clock::time_point &end_time,
			const class router_NHD *router,
			const class map_layer *river_layer,
			class interface_window *view,
			dynamic_string &log);
		bool build_camera_area
			(const std::chrono::system_clock::time_point &end_time,
			class interface_window *view,
			dynamic_string &log);
		bool build_camera_spiral_in
			(class bounding_cube *stream_area,
			const std::chrono::system_clock::time_point &end_time,
			class interface_window *view,
			dynamic_string &log);
		bool build_camera_fixed
			(const map_object *outlet,
			class logical_coordinate *target,
			class interface_window *view,
			dynamic_string &log);
		void build_stream_materials ();
		bool build_stream
			(class map_layer *,
			std::set <long> *trail,
			std::map <long, stream_measurements> *time_changes,
			// class bounding_cube *stream_area,
			class elevation_matrix *elevations,
			class interface_window *view,
			dynamic_string &log);
		virtual bool add_vectors
			(class map_layer *,
			class bounding_cube *stream_area,
			const class elevation_matrix *elevations,
			class interface_window *view,
			dynamic_string &log);
		bool build_points
			(std::set <map_object *> *stations,
			const char *node_name_prefix,
			const double base_size,
			// std::map <long, timed_measurements> *time_changes,
			class elevation_matrix *elevations,
			const bool use_names,
			const bool text_labels,
			const BYTE animation_type,
			class interface_window *view,
			dynamic_string &log);
		void create_lights
			(const bounding_cube &map_area);

		virtual void describe_run
			(dynamic_string &);

		void insert_relevant_objects
			(std::set <map_object *> *plotting_stations,
			// const std::set <long> *plotting_streams,
			const class map_layer *layer,
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
			const long start_segment_id,
			const long destination_segment_id,
			const class router_NHD *router,
			const map_layer *layer_rivers,
			dynamic_string &log);
		
		bool write_collada_xml
			(const dynamic_string &path_xml,
			class interface_window *,
			dynamic_string &log);

		int stream_size
			(const class map_object *);

	protected:
		bool build_boundary_box
			(long *id,
			class bounding_cube *area,
			class collada_geometry *edge_shape,
			class collada_material *color_box,
			class interface_window *view,
			dynamic_string &log);
		bool build_camera_animation
			(const std::chrono::system_clock::time_point &start_time,
			const logical_coordinate &position1,
			const logical_coordinate &target1,
			const logical_coordinate &up_vector,
			const std::chrono::system_clock::time_point &end_time,
			const logical_coordinate &position2,
			const logical_coordinate &target2,
			class collada_animation *animation_rotation,
			class collada_animation *animation_translation,
			const bool verbose,
			dynamic_string &log);
		void make_edge
			(const logical_coordinate &corner1,
			const logical_coordinate &corner2,
			const long id,
			const double width,
			const int shape_id,
			const int color_id);

		void simulate_flow_stream
			(std::map <long, stream_measurements> *time_changes,
			const class router_NHD *router,
			const map_layer *layer_rivers,
			const long comid,
			const long end_comid,
			dynamic_string &log);
};

