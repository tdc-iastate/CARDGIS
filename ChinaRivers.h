
const int CHINA_ROUTER_INDEX_FNODE = 0;
const int CHINA_ROUTER_INDEX_TNODE = 1;
const int CHINA_ROUTER_INDEX_SOURCES_COUNT = 2;
const int CHINA_ROUTER_INDEX_SINKS_COUNT = 3;
const int CHINA_ROUTER_INDEX_ELEVATION_START = 4;
const int CHINA_ROUTER_INDEX_ELEVATION_END = 5;
const int CHINA_ROUTER_INDEX_LENGTH_KM = 6;

const int CHINA_LINK_POINT_ELEVATION = 0;
const int CHINA_LINK_POINT_SOURCES_COUNT = 2;
const int CHINA_LINK_POINT_SINKS_COUNT = 3;


class ChinaRouter {
	public:
		class map_object *start_object, *end_object;
		int start_point_index, end_point_index;
		std::vector <map_object *> path;
		double distance_start_m, distance_m, distance_end_m, elevation_start, elevation_end;
		bool upstream;

		ChinaRouter ();
		bool route
			(class map_layer *layer_rivers,
			const std::map <long, class map_object *> *river_layer_index,
			dynamic_string &log);
		bool route_directional
			(class map_layer *layer_rivers,
			const std::map <long, class map_object *> *river_layer_index,
			dynamic_string &log);
		void write_route_line
			(dynamic_string &log);
};

class thread_work_ChinaRouting : public thread_work {
	public:
		class ChinaRouter router;
		class map_layer *layer_rivers, *layer_counties;
		std::map <long, map_object *> *river_layer_index;
};

class ChinaLinkPoint {
	public:
		long id;
		std::vector <long> sources, sinks;

		ChinaLinkPoint ();
		ChinaLinkPoint
			(const ChinaLinkPoint &);
		ChinaLinkPoint operator =
			(const ChinaLinkPoint &other);
		void copy
			(const ChinaLinkPoint &);

		void add_sink
			(const long segment);
		void add_source
			(const long segment);
		void remove_sink
			(const long segment);
		void remove_source
			(const long segment);
};

class ChinaRivers {
	public:
		dynamic_string filename_china_root;
		std::map <long, ChinaLinkPoint> point_links;
		std::map <long, class map_object *> river_layer_index;
		// Maps indexed by point ID 
		// std::map <long, std::vector <std::pair <long, long>>> point_sources; // Segments that drain to point
		// std::map <long, std::vector <std::pair <long, long>>> point_sinks; // Segments that drain away from point

		// std::map <long, std::vector <long>> connections;

		void add_link_points
			(class map_object *segment,
			class map_layer *layer_points,
			const int level_number,
			long *new_point_id,
			dynamic_string &log);
		void add_segment_sink
			(class map_object *segment,
			const long sink_segment_id);
		void add_segment_source
			(class map_object *segment,
			const long source_segment_id);
		void attach_segment_to_points
			(class map_object *segment,
			class map_layer *layer_points,
			dynamic_string &log);
		void dump_point_maps
			(dynamic_string &log) const;

		map_layer *build_point_layer
			(dynamic_map *,
			interface_window *,
			dynamic_string &log);
		map_layer *import_point_layer
			(dynamic_map *,
			interface_window *,
			dynamic_string &log);
		bool build_connection_map
			(class dynamic_map *map_watershed,
			class map_layer *layer_rivers,
			class map_layer *layer_points,
			dynamic_string &log);
		bool consolidate_points
			(class dynamic_map *map_watershed,
			class map_layer *layer_points,
			dynamic_string &log);
		bool copy_to_rivers
			(dynamic_map *,
			class map_layer *,
			class map_layer *layer_points,
			interface_window *,
			dynamic_string &log);

		class map_layer *read_counties
			(const dynamic_string &filename,
			dynamic_map *,
			interface_window *,
			dynamic_string &log);

		bool read_5_levels
			(dynamic_map *,
			interface_window *,
			dynamic_string &log);

		bool route_all
			(class map_layer *,
			class map_layer *,
			class map_layer *layer_rivers,
			class interface_window *,
			dynamic_string &log);

		class map_layer *create_point_layer
			(dynamic_map *map_watershed);
			class map_layer *import_point_layer
			(const dynamic_string &filename,
			dynamic_map *map_watershed,
			class interface_window *view,
			dynamic_string &log);

		void check_node_order
			(class map_object *copy,
			class map_layer *layer_points,
			const int level_number,
			long *new_point_id,
			dynamic_string &log);
		void set_point_links
			(class map_object *segment,
			class map_object *copy,
			class map_layer *layer_points,
			const int level_number,
			long *new_point_id,
			dynamic_string &log);
		bool find_duplicates
			(dynamic_map *,
			class map_layer *layer_rivers,
			interface_window *,
			dynamic_string &log);

		void set_segment_to
			(class map_layer *layer_rivers,
			router_NHD *,
			dynamic_string &log);

		void write_segment_elevations
			(class map_layer *layer_rivers,
			class map_layer *layer_points,
			dynamic_string &log) const;

		protected:

		void purge_targets
			(class map_layer *layer,
			std::map <long, long> *targets);
};
