const BYTE GEOGRAPHICLIB_LINETYPE_GEODESIC = 1;
const BYTE GEOGRAPHICLIB_LINETYPE_EXACT = 2;
const BYTE GEOGRAPHICLIB_LINETYPE_AUTHALIC = 3;
const BYTE GEOGRAPHICLIB_LINETYPE_RHUMB = 4;

// const int BUFFER_CUT_LINE_LENGTH = 25;

// Where does clipped polygon get ID?
const short CLIPPED_ID_SOURCE_NAME = 1; // (TRACT_ID_NAME_SOURCE_NAME) map object.name
const short CLIPPED_ID_SOURCE_ID = 2; // (TRACT_ID_NAME_SOURCE_ID) map_object.id
const short CLIPPED_ID_SOURCE_NUMERIC_ATTRIBUTE = 3; // tract_index_source_1 or tract_index_source_2
const short CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE = 4; // tract_index_source_1 or tract_index_source_2
const short CLIPPED_ID_SOURCE_THREE_NAMES = 5; // PlaceTract (ID), Census AREAKEY (Name), and PLCIDFP00 (attributes_text [2])
const short CLIPPED_ID_SOURCE_ID_AND_NAME = 6; // map_object.id to attributes_numeric, map_object.name to attributes_text

void area_state_overlaps
	(const int area_number,
	std::set <int> *state_set);

double area_m2
	(const ClipperLib::Path *path,
	const BYTE linetype);

void nearest_up
	(const class flow_network_link *nearest_link,
	const long long level_path_id,
	std::vector <long long> *upstream_ids,
	long long *next_comid_up,
	const class router_NHD *router);

void nearest_down
	(const class flow_network_link *nearest_link,
	std::vector <long long> *downstream_ids,
	long long *next_comid_down,
	const class router_NHD *router);

bool read_census_tracts
	(const dynamic_string &path_shapefile,
	class dynamic_map *map_watershed,
	class map_layer *,
	std::set <int> *state_fips,
	class interface_window *,
	dynamic_string &log);
bool read_PlaceTract
	(const dynamic_string &path_shapefile,
	class dynamic_map *map_watershed,
	class map_layer *,
	std::set <int> *state_fips,
	class interface_window *,
	dynamic_string &log);
bool read_PlaceTract_clipint
	(const dynamic_string &path_shapefile,
	class dynamic_map *map_watershed,
	class map_layer *,
	std::set <int> *state_fips,
	std::chrono::system_clock::time_point *time_after_shp,
	class interface_window *,
	dynamic_string &log);

bool read_county_boundaries
	(const dynamic_string& path_shapefile,
	class dynamic_map* map_watershed,
	class map_layer*,
	std::set <int>* state_fips, // Filter list, can be NULL
	class interface_window*,
	dynamic_string& log);

bool read_tiger_streets
	(const dynamic_string &path_shapefile,
	class dynamic_map *map_watershed,
	class map_layer *,
	std::set <int> *state_fips,
	const bool major_roads_only,
	class interface_window *,
	dynamic_string &log);

bool subtract_inside_areas
	(std::map <dynamic_string, double *> *outside_area,
	ClipperLib::Paths *paths_clipped_inside,
	std::vector <dynamic_string> *tract_id_names_inside,
	ClipperLib::Paths *paths_clipped_outside,
	std::vector <dynamic_string> *tract_id_names_outside,
	dynamic_string &log,
	const bool verbose = false);

class bisection_definition {
	// line crossing a river 'perpendicular' to a point
	public:
		// ClipperLib::Paths vector;
		logical_coordinate end_0, end_1, center_point;

		void create_bisection_2pt
			(const logical_coordinate upstream_first [2],
			const double radius_average,
			double *theta,
			const bool verbose,
			dynamic_string &log);

		void create_bisection_3pt
			(const logical_coordinate upstream_first [3],
			const double radius_average,
			double *theta,
			const bool verbose,
			dynamic_string &log);

		void create_map_object
			(class map_layer *,
			dynamic_string &log) const;

		// Buffer polygons must always be in counter-clockwise point order
		bool set_upstream_downstream
			(const logical_coordinate &intersection) const;
};

class ClipperBuffer {
	public:
		short tract_id_source_1, tract_id_source_2;
		short tract_index_source_1, tract_index_source_2; // If id_source is CLIPPED_ID_SOURCE_NUMERIC_ATTRIBUTE or CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE
		short buffer_width_output_index;
		int thread_wait_interval_ms;
		bool thread_run_tiny;
		// int use_attribute_numeric_index, use_attribute_text_index;
		dynamic_string layer_1_id_column_name, layer_2_id_column_name, layer_1_name_column_name, layer_2_name_column_name;

		ClipperBuffer ()
		{
			tract_id_source_1 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
			tract_id_source_2 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
			tract_index_source_1 = -1;
			tract_index_source_2 = -1;
			buffer_width_output_index = -1;
			thread_wait_interval_ms = 5;
			thread_run_tiny = false;
			layer_1_id_column_name = "Facility";
			layer_2_id_column_name = "Census Tract";
		};
		void add_area_attribute
			(class map_layer *layer,
			dynamic_string &log);

		bool add_downstream_within_segment
			(class map_object *river,
			const int nearest_polygon_index,
			const int nearest_point_index,
			double *upstream_within_segment_m,
			const double limit_meters,
			ClipperLib::Path *p,
			class map_layer *,
			dynamic_string &log) const;
		bool add_upstream_within_segment
			(class map_object *river,
			const int nearest_polygon_index,
			const int nearest_point_index,
			double *downstream_within_segment_m,
			const double limit_meters,
			ClipperLib::Path *p,
			class map_layer *layer_river_points,
			dynamic_string &log) const;

		void add_to_path
			(ClipperLib::Path *,
			const long x,
			const long y) const;

		bool look_upstream_within_segment
			(const class map_object *river,
			const int nearest_polygon_index,
			const int nearest_point_index,
			const double limit_meters,
			double *distance_within_segment_m,
			int *polygon_index,
			int *point_index,
			dynamic_string &log) const;
		bool look_downstream_within_segment
			(const class map_object *river,
			const int nearest_polygon_index,
			const int nearest_point_index,
			const double limit_meters,
			double *distance_within_segment_m,
			int *polygon_index,
			int *point_index,
			dynamic_string &log) const;

		// Clip one polygon against another, accumulating to final_cuts
		bool cut_and_accumulate
			(class map_object *input_polygon_1,
			ClipperLib::Paths *paths_polygon_2,
			ClipperLib::Paths *final_cuts,
			std::vector <long long> *polygon_1_ids,
			int *count_contributing,
			const long polygon_2_id,
			const bool verbose,
			dynamic_string &log);

		// Subtract input_polygon_1 from paths_polygon_2
		bool subtract
			(ClipperLib::Paths *paths_subject,
			ClipperLib::Paths *paths_clip,
			ClipperLib::Paths *differences,
			const bool verbose,
			dynamic_string &log);

		bool create
			(const class map_layer *river,
			class map_layer *buffer,
			class interface_window *view,
			dynamic_string &log);
		void dump_paths
			(ClipperLib::Paths *path_up_and_down,
			dynamic_string &log) const;
		bool Paths_to_map_layer
			(const ClipperLib::Paths *,
			long facility_id,
			double buffer_id,
			const std::vector <dynamic_string>* tract_id_names, // can be NULL
			const std::vector <dynamic_string>* tract_names, // can be NULL
			class map_layer *buffer,
			const bool compute_area,
			class interface_window *view,
			dynamic_string &log);
		bool Paths_to_map_layer
			(const ClipperLib::Paths *,
			long *starting_id,
			long facility_id,
			double numeric_attribute,
			const dynamic_string *text_attribute,
			class map_layer *buffer,
			std::vector <map_object *> *clips_added,
			const bool compute_area,
			class interface_window *view,
			dynamic_string &log);
		bool map_layer_to_Paths
			(const class map_layer *buffer,
			ClipperLib::Paths *,
			class interface_window *view,
			dynamic_string &log);
		bool map_object_to_Path
			(const class map_object *,
			ClipperLib::Paths *,
			dynamic_string &log);
		bool compute_polygon_area
			(const ClipperLib::Paths *,
			class map_layer *buffer,
			const int attribute_index,
			const byte geographiclib_linetype,
			class interface_window *view,
			dynamic_string &log);
		bool create_buffer
			(const ClipperLib::Paths *,
			ClipperLib::Paths *solution,
			const double offset_radius_average,
			dynamic_string &log) const;
		bool create_grid
			(class map_layer *layer_grid,
			const double grid_size_miles,
			const double length_stream_miles,
			class map_object *river,
			const int nearest_polygon_index,
			const int nearest_point_index,
			dynamic_string &log) const;
		bool create_paths
			(const class map_layer *layer_rivers,
			class map_layer *layer_river_points,
			std::vector <long long> *upstream_ids,
			const double length_km,
			ClipperLib::Path *,
			dynamic_string &log) const;
		bool create_paths_reverse
			(const class map_layer *layer_rivers,
			class map_layer *layer_river_objects,
			const std::vector <long long> *downstream_ids,
			//const double buffer_radius_m,
			const double length_km,
			ClipperLib::Path *,
			dynamic_string &log) const;
		bool create_threaded
			(const class map_layer *river,
			class map_layer *buffer,
			const int thread_count,
			const double buffer_radius_m,
			const bool endtype_closed_polygon, // 2022-01-25 needed for inner polygons (if river endpoints coincide with start points?)
			class interface_window *view,
			dynamic_string &log);
		/*
		bool designate_grid_cells
			(class map_layer *layer_grid,
			class map_layer *layer_grid_vectors,
			const class map_layer *layer_river_points,
			const class map_object *river,
			const int nearest_polygon_index,
			const int nearest_point_index,
			const class router_NHD *router,
			dynamic_string &log) const;
		*/
		bool intersect_overlapping
			(const ClipperLib::Paths *solution,
			const class map_layer *layer_census_tracts,
			ClipperLib::Paths *clipped_tracts,
			std::vector <dynamic_string>* tract_id_names,
			std::vector <dynamic_string>* tract_names,
			class interface_window *view,
			dynamic_string &log) const;
		bool intersect_threaded
			(const class map_layer *layer_1,
			const class map_layer *layer_2,
			class map_layer *layer_intersected,
			const int thread_count,
			class interface_window *view,
			dynamic_string &log) const;

		bool find_bisection_line
			(class map_object *segment,
			const int nearest_polygon_index,
			const int nearest_point_index,
			const long next_comid_up,
			const long next_comid_down,
			// const map_layer *layer_rivers,
			const class router_NHD *router,
			const double radius_average,
			class bisection_definition *result,
			double *theta,
			std::set <long long> *two_point_instances,
			const bool verbose,
			dynamic_string &log) const;

		bool intersect_circles_threaded
			(const class map_layer *facilities,
			const class map_layer *layer_rivers,
			const class map_layer *layer_census_tracts,
			const class router_NHD *router,
			const int thread_count,
			FILE *output_table,
			class interface_window *view,
			dynamic_string &log);
		bool buffer_stream_threaded
			(const class map_layer *facilities,
			const class map_layer *layer_rivers,
			const class map_layer *layer_census_tracts,
			const class router_NHD *router,
			const double length_stream_miles,
			const double cut_line_length_miles,
			const double minimum_buffer_radius_miles,
			const double maximum_buffer_radius_miles,
			const int thread_count,
			FILE *output_table,
			class interface_window *view,
			dynamic_string &log);
		bool buffer_upstream_threaded
			(const class map_layer *facilities,
			const class map_layer *layer_rivers,
			const class map_layer *layer_census_tracts,
			const class router_NHD *router,
			const double buffer_width_miles,
			const double start_length_miles,
			const double length_stream_miles,
			const double cut_line_length_miles,
			const int thread_count,
			const bool write_column_headers,
			FILE *output_table,
			class interface_window *view,
			dynamic_string &log);
		bool buffer_upstream_threaded_difference
			(const class map_layer *facilities,
			const class map_layer *layer_rivers,
			const class map_layer *layer_census_tracts,
			const class router_NHD *router,
			const double buffer_width_miles,
			const double length_stream_miles,
			const double start_length_miles,
			const double cut_line_length_miles,
			const int thread_count,
			FILE *output_table,
			class interface_window *view,
			dynamic_string &log);
		bool buffer_downstream_threaded
			(const class map_layer *facilities,
			const class map_layer *layer_rivers,
			const class map_layer *layer_census_tracts,
			const class router_NHD *router,
			const double buffer_width_miles,
			const double start_stream_miles,
			const double length_stream_miles,
			const double cut_line_length_miles,
			const int thread_count,
			const bool write_column_headers,
			FILE *output_table,
			class interface_window *view,
			dynamic_string &log);
		bool buffer_downstream_threaded_difference
			(const class map_layer *facilities,
			const class map_layer *layer_rivers,
			const class map_layer *layer_census_tracts,
			const class router_NHD *router,
			const double buffer_width_miles,
			const double length_stream_miles,
			const double start_length_miles,
			const double cut_line_length_miles,
			const int thread_count,
			FILE *output_table,
			class interface_window *view,
			dynamic_string &log);

		bool make_circle
			(class map_object *segment,
			const int nearest_polygon_index,
			const int nearest_point_index,
			const long next_comid_up,
			const long next_comid_down,
			const class router_NHD *router,
			const double radius_average,
			ClipperLib::Paths *buffers_up,
			ClipperLib::Paths *buffers_down,
			std::set <long long> *two_point_instances,
			dynamic_string &log) const;

		bool merge
			(const class map_layer *buffer,
			class map_layer *combined_buffer,
			class interface_window *view,
			dynamic_string &log);
		bool merge_threaded
			(const class map_layer *buffer,
			class map_layer *combined_buffer,
			const int thread_count,
			class interface_window *view,
			dynamic_string &log);

		double average_offset
			(const class map_layer *layer_rivers,
			const long comid,
			const double buffer_radius_m,
			dynamic_string &log) const;

		bool find_start_point_upstream
			(const class map_object *nearest_segment,
			const int nearest_polygon_index,
			const int nearest_point_index,
			const class map_layer *layer_rivers,
			std::vector <long long> *upstream_ids,
			const double start_upstream_km,
			class map_object **new_segment,
			int *new_polygon_index,
			int *new_point_index,
			const bool verbose,
			dynamic_string &log) const;
		bool find_start_point_downstream
			(const class map_object *nearest_segment,
			const int nearest_polygon_index,
			const int nearest_point_index,
			const class map_layer *layer_rivers,
			std::vector <long long> *upstream_ids,
			const double start_upstream_km,
			class map_object **new_segment,
			int *new_polygon_index,
			int *new_point_index,
			const bool verbose,
			dynamic_string &log) const;

		bool cut_buffer
			(const long facility_id,
			const double buffer_size_miles,
			std::map <double, long> *truncation_error_histogram,
			const bisection_definition *bisection,
			const ClipperLib::Paths *buffers_up_and_down,
			ClipperLib::Paths *path_buffers_upstream,
			ClipperLib::Paths *path_buffers_downstream,
			dynamic_string &log) const;

		void write_area_table
			(class map_layer* layer_clipped,
			const char* direction, // "up" or "down"
			dynamic_string& log);

		// Area table with all polygons in layer_1 summed to single rows
		void write_area_table_group_by_1
			(const class map_layer* layer_1,
			const class map_layer* layer_clipped,
			dynamic_string& log);

	protected:
		void add_to_path
			(ClipperLib::Path *,
			const device_coordinate &) const;

		bool merge_segment
			(class map_polygon *segment,
			ClipperLib::Paths *collective);
		void set_column_headers
			(FILE *output_table) const;
		void set_tract_id
			(std::vector <dynamic_string>* tract_ids,
			std::vector <dynamic_string>* tract_names,
			const class map_object *tract) const;
};