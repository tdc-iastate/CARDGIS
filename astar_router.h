#pragma once

typedef double WeightCostType;
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, WeightCostType> > mygraph_t;
typedef boost::property_map<mygraph_t, boost::edge_weight_t>::type WeightMap;
typedef mygraph_t::vertex_descriptor AstarVertex;
typedef mygraph_t::edge_descriptor edge_descriptor;
typedef mygraph_t::vertex_iterator vertex_iterator;
typedef std::pair<int, int> EdgePair;

class astar_graph_edge {
	public:
		long segment_id;
		int index;

		long fnode, tnode;
		std::pair <int, int> point_indexes;

		double weight;

		astar_graph_edge ();
		astar_graph_edge (const astar_graph_edge &);
		void copy
			(const astar_graph_edge &);
		astar_graph_edge operator =
			(const astar_graph_edge &other);
};

class astar_router {
	public:
		class map_object *start_object, *end_object, *start_segment, *end_segment;
		double distance_start_m, distance_between_m, distance_end_m, elevation_start, elevation_end;
		std::vector <long long> result_points;
		std::vector <long long> result_edges;

		astar_router ();
		virtual ~astar_router ();

		bool run
			(const long start_point_id,
			const long goal_point_id,
			const class astar_graph *graph,
			const bool verbose,
			dynamic_string &log);
		bool route
			(class map_layer *layer_link_points,
			class map_layer *layer_rivers,
			class astar_graph *,
			dynamic_string &log);
		bool route_quiet
			(class map_layer *layer_link_points,
			class map_layer *layer_rivers,
			class astar_graph *,
			dynamic_string &log);
		void write_route_line
			(dynamic_string &log);
	protected:
		class map_object *closest_non_island
			(device_coordinate &position,
			int *point_index,
			class map_layer *layer_rivers);
};

class astar_graph {
	public:
		std::map <int, astar_graph_edge> graph_edges;
		mygraph_t *g;
		WeightCostType *weights;
		device_coordinate *locations;
		unsigned int num_edges, N;
		std::map <long, int> point_id_by_index;
		std::map <long, int> point_index_by_id;

		// Quick access to set of edges for each point
		std::map <int, std::set <int>> edge_id_by_point_id;

		astar_graph ();
		virtual ~astar_graph ();

		bool setup_graph
			(class map_layer* layer_link_points,
			class map_layer* layer_rivers,
			const bool verbose,
			dynamic_string& log);
		bool setup_graph_china_router
			(class map_layer* layer_link_points,
			class map_layer* layer_rivers,
			const bool verbose,
			dynamic_string& log);
		long find_edge_between
			(const int point_index,
			const int previous_point_index) const;
	protected:
		void setup_edge_point_index
			(const int point_index,
			const int edge_index);

};

class thread_work_AstarRouting : public thread_work {
	public:
		class astar_graph *graph;
		class astar_router router;
		class map_layer *layer_rivers, *layer_link_points, *layer_counties;
		std::map <long, map_object *> *river_layer_index;
};

bool astar_route_all
	(class map_layer *layer_stations,
	class map_layer *layer_counties,
	class map_layer *layer_rivers,
	class map_layer *layer_link_points,
	class interface_window *view,
	dynamic_string &log);

bool astar_route_all_multithread
	(class map_layer *layer_stations,
	class map_layer *layer_counties,
	class map_layer *layer_rivers,
	class map_layer *layer_link_points,
	class interface_window *view,
	dynamic_string &log);
