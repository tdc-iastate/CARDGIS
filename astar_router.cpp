// https://www.boost.org/doc/libs/1_38_0/libs/graph/example/astar-cities.cpp
// Boost C++ Libraries
// ...one of the most highly regarded and expertly designed C++ library projects in the world.
// — Herb Sutter and Andrei Alexandrescu, C++ Coding Standards

// This is the documentation for an old version of Boost. Click here to view this page for the latest version.
// libs/graph/example/astar-cities.cpp

// Copyright Beman Dawes, David Abrahams, 1998-2005.
// Copyright Rene Rivera 2004-2008.
// Distributed under the Boost Software License, Version 1.0.


//
//=======================================================================
// Copyright (c) 2004 Kristopher Beevers
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//

#include "../util/utility_afx.h"
#include <vector>
#include <list>
// #include <iostream>
// #include <fstream>
#include <map>
#include <thread>
#include <future>
#include "../util/dynamic_string.h"
#include "../util/device_coordinate.h"
#include "../util/interface_window.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
// #include <boost/random.hpp>
#include <boost/graph/graphviz.hpp>
// #include <sys/time.h>
#include <math.h>    // for sqrt
#include "../subbasin_effect/thread_manager.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"
#include "ChinaRivers.h"
#include "astar_router.h"

using namespace boost;
using namespace std;

// auxiliary types
/*
struct location
{
  float y, x; // lat, long
};
*/

bool work_Astar_Routing
	(thread_work *job_ptr)

{
	std::vector <map_object *>::const_iterator county;
	thread_work_AstarRouting *job = (thread_work_AstarRouting *) job_ptr;
	int county_index;

	/*
	county = job->layer_counties->objects.begin ();
	// job->router.end_object = job->layer_counties->match_id (1390);
	job->router.end_object = *county;
	if (job->router.route_quiet (job->layer_link_points, job->layer_rivers, job->log))
		job->router.write_route_line (job->log);
	*/
	for (county = job->layer_counties->objects.begin (), county_index = 0;
	county != job->layer_counties->objects.end ();
	++county, ++county_index) {
		if ((county_index >= 0)
		&& (county_index < 300)) {
			job->router.end_object = *county;
			if (job->router.route_quiet (job->layer_link_points, job->layer_rivers, job->graph, job->log))
				job->router.write_route_line (job->log);
			job->view->update_scroll_formatted (1, "%ld->%ld", job->router.start_object->id, (*county)->id);
		}
	}

	job->status = THREAD_WORK_STATUS_COMPLETE;
	return true;
}

astar_graph_edge::astar_graph_edge ()

{
	segment_id = 0L;
	index = 0;
	fnode = tnode = 0L;
	weight = 0.0;
}

astar_graph_edge::astar_graph_edge
	(const astar_graph_edge &other)

{
	copy (other);
}

astar_graph_edge astar_graph_edge::operator =
	(const astar_graph_edge &other)
{
	if (this != &other)
		copy(other);

	return (*this);
}

void astar_graph_edge::copy
	(const astar_graph_edge &other)

{
	segment_id = other.segment_id;
	index = other.index;
	fnode = other.fnode;
	tnode = other.tnode;
	point_indexes = other.point_indexes;
	weight = other.weight;
}


/*
template <class Name, class LocMap>
class city_writer {
	public:
		city_writer(Name n, LocMap l, float _minx, float _maxx,
		float _miny, float _maxy,
		unsigned int _ptx, unsigned int _pty)
		: name(n), loc(l), minx(_minx), maxx(_maxx), miny(_miny),
		maxy(_maxy), ptx(_ptx), pty(_pty) {}

		template <class Vertex>  void operator()(ostream& out, const Vertex& v) const {
			float px = 1 - (loc[v].x - minx) / (maxx - minx);
			float py = (loc[v].y - miny) / (maxy - miny);
			out << "[label=\"" << name[v] << "\", pos=\""
			<< static_cast<unsigned int>(ptx * px) << ","
			<< static_cast<unsigned int>(pty * py)
			<< "\", fontsize=\"11\"]";
		}
	private:
		Name name;
		LocMap loc;
		float minx, maxx, miny, maxy;
		unsigned int ptx, pty;
};

template <class WeightMap> class time_writer {
	public:
		time_writer(WeightMap w) : wm(w) {}
		template <class Edge>
		void operator()(ostream &out, const Edge& e) const {
			out << "[label=\"" << wm[e] << "\", fontsize=\"11\"]";
		}
	private:
		WeightMap wm;
};
*/


// euclidean distance heuristic
template <class Graph, class CostType, class LocMap> class distance_heuristic : public astar_heuristic<Graph, CostType> {
	public:
		typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
		distance_heuristic(LocMap l, Vertex goal)
		: m_location(l), m_goal(goal) {}

		CostType operator()(Vertex u)
		{
			CostType dx = m_location[m_goal].x - m_location[u].x;
			CostType dy = m_location[m_goal].y - m_location[u].y;
			return ::sqrt(dx * dx + dy * dy);
		}
	private:
		LocMap m_location;
		Vertex m_goal;
};


struct found_goal {}; // exception for termination

// visitor that terminates when we find the goal
template <class Vertex> class astar_goal_visitor : public boost::default_astar_visitor
{
	public:
		astar_goal_visitor(Vertex goal) : m_goal(goal) {}
		template <class Graph> void examine_vertex(Vertex u, Graph& g)
		{
			if(u == m_goal)
				throw found_goal();
		}
	private:
		Vertex m_goal;
};


astar_graph::astar_graph ()

{
	g = NULL;
	weights = NULL;
	locations = NULL;
}

astar_graph::~astar_graph ()

{
	if (g)
		delete g;
	if (weights)
		delete weights;
	if (locations)
		delete locations;
}


astar_router::astar_router ()

{
	start_object = end_object = start_segment = end_segment = NULL;
	// start_point_index = end_point_index = -1;
	distance_start_m = distance_between_m = distance_end_m = elevation_start = elevation_end = 0.0;
}

astar_router::~astar_router ()

{

}

void astar_graph::setup_edge_point_index
	(const int point_index,
	const int edge_index)

// For finding the paths traversed, keep map of sets of edges touching each point
{
	std::map <int, std::set <int>>::iterator point_edges;

	if ((point_edges = edge_id_by_point_id.find (point_index)) == edge_id_by_point_id.end ()) {
		std::set <int> edge_set;
		edge_set.insert (edge_index);
		edge_id_by_point_id.insert (std::pair <int, std::set <int>> (point_index, edge_set));
	}
	else
		point_edges->second.insert (edge_index);
}

long astar_graph::find_edge_between
	(const int point_index,
	const int previous_point_index) const

{
	std::map <int, std::set <int>>::const_iterator point_edges;
	std::set <int>::iterator edge_index;
	std::map <int, astar_graph_edge>::const_iterator graph_edge;
	long found = -1;

	point_edges = edge_id_by_point_id.find (point_index);

	// Find one of the edges that also touches previous_point_index
	for (edge_index = point_edges->second.begin ();
	(found == -1)
	&& (edge_index != point_edges->second.end ());
	++edge_index) {
		graph_edge = graph_edges.find (*edge_index);
		if ((graph_edge->second.point_indexes.first == previous_point_index)
		|| (graph_edge->second.point_indexes.second == previous_point_index))
			found = graph_edge->second.segment_id;
	}

	return found;
}

bool astar_router::route
	(map_layer *layer_link_points,
	map_layer *layer_rivers,
	astar_graph *graph,
	dynamic_string &log)

// astar_router.result_points never includes first point, but always include last
// astar_router.result_edges may or may not include first_segment and last_segment

{
	int start_point_index, end_point_index;
	std::vector <long long>::iterator road_id, next_road_id;
	map_object *road;
	device_coordinate start_position, end_position;
	double segment_length_m;

	start_object->centroid (&start_position.y, &start_position.x);
	start_segment = closest_non_island (start_position, &start_point_index, layer_rivers);
	elevation_start = start_segment->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_START];
	
	end_object->centroid (&end_position.y, &end_position.x);
	end_segment = closest_non_island (end_position, &end_point_index, layer_rivers);
	elevation_end = end_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_END];

	// A* always starts with FNODEs of both start_segment and end_segment
	if (run ((long) start_segment->attributes_numeric[CHINA_ROUTER_INDEX_FNODE],
	(long) end_segment->attributes_numeric[CHINA_ROUTER_INDEX_FNODE],
	graph,
	false, // set to true if a* crashes to see graph points & edges
	log)) {

		log.add_formatted ("%lld node %d to %ld node %d\n", start_segment->id, start_point_index, end_segment->id, end_point_index);

		log += "Segment\tIndex\tLat\tLon\tLength,km\tTotal,km\n";

		distance_start_m = distance_between_m = distance_end_m = 0.0;

		for (road_id = result_edges.begin (); road_id != result_edges.end (); ++road_id) {
			next_road_id = road_id;
			++next_road_id;

			if (road_id == result_edges.begin ()) {
				// First entry in result_edges

				log.add_formatted ("%ld", start_segment->id);
				if (*road_id == start_segment->id) {
					// result_edges does include start_segment
					// Therefore we traversed start_segment to get to its TNODE
					// Therefore we moved through it downstream
					((map_polygon *) start_segment)->write_length_km (log, false, 1, start_point_index, -1);
					((map_polygon *) start_segment)->perimeter_between_meters (start_point_index, -1, &distance_start_m, log);
				}
				else {
					// First result_edge was not start_segment
					// Therefore we didn't traverse start_segment to get to its TNODE
					// Therefore we're moving through it upstream
					((map_polygon *) start_segment)->write_length_km (log, false, 1, 0, start_point_index);
					((map_polygon *) start_segment)->perimeter_between_meters (0, start_point_index, &distance_start_m, log);

					// first entry is not start_segment, so add it all to distance_between_m
					road = layer_rivers->match_id (*road_id);
					((map_polygon *) road)->perimeter_between_meters (0, -1, &segment_length_m, log);
					distance_between_m += segment_length_m;
					log.add_formatted ("%ld\t\t\t\t\t%.4lf", road->id, segment_length_m / 1000.0);
				}
			}
			else
				if (next_road_id == result_edges.end ()) {
					// Last segment of results
					if (*road_id != end_segment->id) {
						// Last entry is not end_segment, so add it all to distance_between_m
						road = layer_rivers->match_id (*road_id);
						((map_polygon *) road)->perimeter_between_meters (0, -1, &segment_length_m, log);
						distance_between_m += segment_length_m;
						log.add_formatted ("%ld\t\t\t\t\t%.4lf\n", road->id, segment_length_m / 1000.0);
					}

					// End segment
					log.add_formatted ("%ld", end_segment->id);
					if (*road_id == end_segment->id) {
						// result_edges does include end_segment
						// Therefore we traversed end_segment to get to its FNODE
						// Therefore we moved through it upstream
						((map_polygon *) end_segment)->write_length_km (log, false, 1, end_point_index, -1);
						((map_polygon *) end_segment)->perimeter_between_meters (end_point_index, -1, &distance_end_m, log);
					}
					else {
						// Last result_edge was not end_segment
						// Therefore we didn't traverse end_segment to get to its FNODE
						// Therefore we're moving through it downstream
						((map_polygon *) end_segment)->write_length_km (log, false, 1, 0, end_point_index);
						((map_polygon *) end_segment)->perimeter_between_meters (0, end_point_index, &distance_end_m, log);
					}
				}
				else {
					// In-between
					road = layer_rivers->match_id (*road_id);
					((map_polygon *) road)->perimeter_between_meters (0, -1, &segment_length_m, log);
					distance_between_m += segment_length_m;
					log.add_formatted ("%ld\t\t\t\t\t%.4lf", road->id, segment_length_m / 1000.0);
				}
			log += "\n";
		}

		log.add_formatted ("\t\t\t\t\t%.4lf\tkm\n", (distance_start_m + distance_between_m + distance_end_m) / 1000.0);
		// log.add_formatted ("Router distance\t\t\t\t\t%.4lf\tkm\n", distance_between_m);

		return true;
	}
	else {
		log.add_formatted ("%ld\t%ld", start_object->id, end_object->id);
		log += "\tNo Path\n";
		return false;
	}
}

map_object *astar_router::closest_non_island
	(device_coordinate &position,
	int *point_index,
	map_layer *layer_rivers)

// If nearest segment is a polygon that starts and ends at the same point, keep looking
// Uses FNODE and TNODE

{
	long distance, nearest_distance;
	map_object *found = NULL;
	std::vector<map_object *>::const_iterator highway;

	nearest_distance = LONG_MAX;

	for(highway = layer_rivers->objects.begin();
	highway != layer_rivers->objects.end();
	++highway) {

		distance = (*highway)->closest_distance (position);
		if (distance < nearest_distance) {
			if ((*highway)->attributes_numeric[CHINA_ROUTER_INDEX_FNODE] != (*highway)->attributes_numeric[CHINA_ROUTER_INDEX_TNODE]) {
				nearest_distance = distance;
				found = *highway;
			}
		}
	}

	found->closest_point_meters (position, point_index);

	return found;
}

bool astar_router::route_quiet
	(map_layer *layer_link_points,
	map_layer *layer_rivers,
	astar_graph *graph,
	dynamic_string &log)

// multi-threaded version

{
	int start_point_index, end_point_index;
	std::vector <long long>::iterator road_id, next_road_id;
	map_object *road;
	device_coordinate start_position, end_position;
	double segment_length_m;

	start_object->centroid (&start_position.y, &start_position.x);
	start_segment = closest_non_island (start_position, &start_point_index, layer_rivers);
	// start_segment = layer_rivers->find_layer_object (start_position, &gap);
	// start_segment->closest_point_meters (start_position, &start_point_index);
	elevation_start = start_segment->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_START];
	
	end_object->centroid (&end_position.y, &end_position.x);
	end_segment = closest_non_island (end_position, &end_point_index, layer_rivers);
	// end_segment = layer_rivers->find_layer_object (end_position, &gap);
	// send_segment->closest_point_meters (end_position, &end_point_index);
	elevation_end = end_segment->attributes_numeric [CHINA_ROUTER_INDEX_ELEVATION_END];

	if (run ((long long) start_segment->attributes_numeric[CHINA_ROUTER_INDEX_FNODE],
	(long long) end_segment->attributes_numeric[CHINA_ROUTER_INDEX_FNODE],
	graph,
	false, log)) {

		distance_start_m = distance_between_m = distance_end_m = 0.0;

		for (road_id = result_edges.begin (); road_id != result_edges.end (); ++road_id) {
			next_road_id = road_id;
			++next_road_id;
			if (road_id == result_edges.begin ()) {
				if (*road_id == start_segment->id) {
					// first segment was traversed and included in distance_between_m

					// result_edges does include start_segment
					// Therefore we traversed start_segment to get to its TNODE
					// Therefore we moved through it downstream
					((map_polygon *) start_segment)->perimeter_between_meters (start_point_index, -1, &distance_start_m, log);
				}
				else {
					// First result_edge was not start_segment
					// Therefore we didn't traverse start_segment to get to its TNODE
					// Therefore we're moving through it upstream
					((map_polygon *) start_segment)->perimeter_between_meters (0, start_point_index, &distance_start_m, log);

					// first entry is not start_segment, so add it all to distance_between_m
					road = layer_rivers->match_id (*road_id);
					((map_polygon *) road)->perimeter_between_meters (0, -1, &segment_length_m, log);
					distance_between_m += segment_length_m;
				}
			}
			else
				if (next_road_id == result_edges.end ()) {
					if (*road_id == end_segment->id) {
						// last segment was traversed and included in distance_between_m

						// result_edges does include end_segment
						// Therefore we traversed end_segment to get to its FNODE
						// Therefore we moved through it upstream
						((map_polygon *) end_segment)->perimeter_between_meters (end_point_index, -1, &distance_end_m, log);
					}
					else {
						// Last result_edge was not end_segment
						// Therefore we didn't traverse end_segment to get to its FNODE
						// Therefore we're moving through it downstream
						((map_polygon *) end_segment)->perimeter_between_meters (0, end_point_index, &distance_end_m, log);

						// Last entry is not end_segment, so add it all to distance_between_m
						road = layer_rivers->match_id (*road_id);
						((map_polygon *) road)->perimeter_between_meters (0, -1, &segment_length_m, log);
						distance_between_m += segment_length_m;
					}
				}
				else {
					road = layer_rivers->match_id (*road_id);
					((map_polygon *) road)->perimeter_between_meters (0, -1, &segment_length_m, log);
					distance_between_m += segment_length_m;
				}
		}

		return true;
	}
	else {
		log.add_formatted ("%lld\t%lld", start_object->id, end_object->id);
		log += "\tNo Path\n";
		return false;
	}
}

bool astar_router::run
	(const long start_point_id,
	const long goal_point_id,
	const astar_graph *graph,
	const bool verbose,
	dynamic_string &log)

// Construct a graph and use A* to route from start_point_id to goal_point_id

{
	AstarVertex start, goal;
	std::map <long, int>::const_iterator index_by_id;
	std::map <long, int>::const_iterator id_by_index;

	if ((index_by_id = graph->point_index_by_id.find (start_point_id)) != graph->point_index_by_id.end ()) {
		start = index_by_id->second;

		if ((index_by_id = graph->point_index_by_id.find (goal_point_id)) != graph->point_index_by_id.end ()) {
			goal = index_by_id->second;

			std::vector<mygraph_t::vertex_descriptor> p (boost::num_vertices (*(graph->g)));
			std::vector<WeightCostType> d (boost::num_vertices (*(graph->g)));
			try {
				// call astar named parameter interface
				astar_search
				(*(graph->g),
				start,
				distance_heuristic<mygraph_t, WeightCostType, device_coordinate *> (graph->locations, goal),
				predecessor_map (&p[0]).distance_map (&d[0]).visitor (astar_goal_visitor<AstarVertex> (goal)));


			}
			catch (found_goal fg) { // found a path to the goal
				std::list<AstarVertex> shortest_path;
				long edge_id;
				int last_point_index = (int) start;

				for (AstarVertex v = goal;; v = p[v]) {
					shortest_path.push_front (v);
					if (p[v] == v)
						break;
				}

				list<AstarVertex>::iterator spi = shortest_path.begin ();
				// cout << name[start];
				for (++spi; spi != shortest_path.end (); ++spi) {
					id_by_index = graph->point_id_by_index.find (*spi);
					result_points.push_back (id_by_index->second);

					// Find edge between last point and spi
					edge_id = graph->find_edge_between (*spi, last_point_index);

					result_edges.push_back (edge_id);

					last_point_index = *spi;

					if (verbose)
						log.add_formatted (" -> %d/%ld\n", *spi, id_by_index->second);
				}
				// distance_between_m = d[goal];
				if (verbose)
					log.add_formatted ("Total cost (km): %.4lf\n", d[goal]);
				return true;
			}
		}
	}
	return false; 
}

bool astar_graph::setup_graph_china_router
	(map_layer *layer_link_points,
	map_layer *layer_rivers,
	const bool verbose,
	dynamic_string &log)

// Construct a graph g and (point) locations and use A* to route from start_point_id to goal_point_id

{
	std::map <int, astar_graph_edge>::iterator graph_edge;
	astar_graph_edge new_graph_edge;
	bool error = false;

	std::vector <map_object *>::const_iterator point, segment;
	int point_index, edge_index;
	std::map <long, int>::iterator index_by_id;
	std::map <long, int>::iterator id_by_index;
	// std::set <long> preroute = { 301416, 301382, 500622, 500620, 500609, 500607 };

	for (segment = layer_rivers->objects.begin (), edge_index = 0, point_index = 0;
	segment != layer_rivers->objects.end ();
	++segment) {
		if ((*segment)->attributes_numeric[CHINA_ROUTER_INDEX_FNODE] != (*segment)->attributes_numeric[CHINA_ROUTER_INDEX_TNODE]) {
			// A few polygons have endpoint = startpoint, they outline islands.
			// if (preroute.find ((*segment)->id) != preroute.end ()) {
				new_graph_edge.segment_id = (*segment)->id;
				new_graph_edge.index = edge_index;
				new_graph_edge.weight = (*segment)->attributes_numeric[CHINA_ROUTER_INDEX_LENGTH_KM];

				new_graph_edge.fnode = (long) (*segment)->attributes_numeric[CHINA_ROUTER_INDEX_FNODE];
				// If id fnode has been used before, find the mapped index
				if ((index_by_id = point_index_by_id.find (new_graph_edge.fnode)) == point_index_by_id.end ()) {
					new_graph_edge.point_indexes.first = point_index;
					setup_edge_point_index (point_index, edge_index);
					point_index_by_id.insert (std::pair <long, int> (new_graph_edge.fnode, point_index++));
				}
				else {
					new_graph_edge.point_indexes.first = index_by_id->second;
					setup_edge_point_index (index_by_id->second, edge_index);
				}

				new_graph_edge.tnode = (long) (*segment)->attributes_numeric[CHINA_ROUTER_INDEX_TNODE];
				// If id tnode has been used before, find the mapped index
				if ((index_by_id = point_index_by_id.find (new_graph_edge.tnode)) == point_index_by_id.end ()) {
					new_graph_edge.point_indexes.second = point_index;
					setup_edge_point_index (point_index, edge_index);
					point_index_by_id.insert (std::pair <long, int> (new_graph_edge.tnode, point_index++));
				}
				else {
					new_graph_edge.point_indexes.second = index_by_id->second;
					setup_edge_point_index (index_by_id->second, edge_index);
				}

				graph_edges.insert (std::pair <int, astar_graph_edge> (edge_index, new_graph_edge));

				++edge_index;
			// }
		}
	}

	if (!error) {
		num_edges = graph_edges.size ();
		EdgePair *edge_array = new EdgePair[num_edges];
		weights = new WeightCostType[num_edges];

		// Fill edge_array and weights
		if (verbose) {
			log += "Edges\n";
			log += "Index\tSegment ID\tPoint Index 1\tPoint Id 1\tPoint Index 2\tPoint iD 2\tWeight\n";
		}
		for (graph_edge = graph_edges.begin (), edge_index = 0;
		graph_edge != graph_edges.end ();
		++graph_edge, ++edge_index) {

			if (verbose) {
				log.add_formatted ("%d\t%ld", graph_edge->first, graph_edge->second.segment_id);
				log.add_formatted ("\t%d\t%ld", graph_edge->second.point_indexes.first, graph_edge->second.fnode);
				log.add_formatted ("\t%d\t%ld", graph_edge->second.point_indexes.second, graph_edge->second.tnode);
				log.add_formatted ("\t%.4lf\n", graph_edge->second.weight);
			}

			edge_array[edge_index] = graph_edge->second.point_indexes;
			weights[edge_index] = graph_edge->second.weight;
		}

		// Doing point locations after edges to insure that every point is associated with at least 1 edge
		N = point_index_by_id.size ();
		locations = new device_coordinate[N];

		if (verbose) {
			log += "Locations\n";
			log += "Index\tPoint ID\tx\ty\n";
		}
		for (point = layer_link_points->objects.begin ();
		point != layer_link_points->objects.end ();
		++point) {
			if ((index_by_id = point_index_by_id.find ((*point)->id)) != point_index_by_id.end ()) {
				point_id_by_index.insert (std::pair <int, long> (index_by_id->second, index_by_id->first));
				locations[index_by_id->second].x = (*point)->longitude;
				locations[index_by_id->second].y = (*point)->latitude;
				// ids.push_back ((*point)->id);
				if (verbose)
					log.add_formatted ("%d\t%ld\t%ld\t%ld\n", index_by_id->second, index_by_id->first, locations[index_by_id->second].x, locations[index_by_id->second].y);
			}
		}

		// create graph
		g = new mygraph_t (N);
		WeightMap weightmap = get (boost::edge_weight, *g);
		for (std::size_t j = 0; j < num_edges; ++j) {
			edge_descriptor e;
			bool inserted;

			tie (e, inserted) = add_edge (edge_array[j].first, edge_array[j].second, *g);
			weightmap[e] = weights[j];
		}

		if (verbose) {
			log += "A* search\n";
			log += "Edge Array\n";
			log += "Index\tPoint Index 1\tPoint Index 2\tWeight\n";
			for (edge_index = 0; edge_index < num_edges; ++edge_index)
				log.add_formatted ("%d\t%d\t%d\t%.4lf\n", edge_index, edge_array[edge_index].first, edge_array[edge_index].second, weights[edge_index]);
			log += "\n";

			log += "Location Array\n";
			log += "Index\tx\ty\n";
			for (point_index = 0; point_index < N; ++point_index)
				log.add_formatted ("%d\t%ld\t%ld\n", point_index, locations[point_index].x, locations[point_index].y);
			log += "\n";
		}
	}
	return !error; 
}

bool astar_graph::setup_graph
	(map_layer* layer_link_points,
	map_layer* layer_rivers,
	const bool verbose,
	dynamic_string& log)

// Construct a graph g and (point) locations and use A* to route from start_point_id to goal_point_id

{
	/*
	typedef adjacency_list<listS, vecS, undirectedS, no_property, property<edge_weight_t, WeightCostType> > mygraph_t;
	typedef property_map<mygraph_t, edge_weight_t>::type WeightMap;
	typedef mygraph_t::vertex_descriptor vertex;
	typedef mygraph_t::edge_descriptor edge_descriptor;
	typedef mygraph_t::vertex_iterator vertex_iterator;
	typedef std::pair<int, int> edge;
	*/
	std::map <int, astar_graph_edge>::iterator graph_edge;
	astar_graph_edge new_graph_edge;
	bool error = false;

	std::vector <map_object*>::const_iterator point, segment;
	int point_index, edge_index;
	std::map <long, int>::iterator index_by_id;
	std::map <long, int>::iterator id_by_index;
	// std::set <long> preroute = { 301416, 301382, 500622, 500620, 500609, 500607 };

	for (segment = layer_rivers->objects.begin(), edge_index = 0, point_index = 0;
	segment != layer_rivers->objects.end();
	++segment) {
		if ((*segment)->attributes_numeric[CHINA_ROUTER_INDEX_FNODE] != (*segment)->attributes_numeric[CHINA_ROUTER_INDEX_TNODE]) {
			// A few polygons have endpoint = startpoint, they outline islands.
			// if (preroute.find ((*segment)->id) != preroute.end ()) {
			new_graph_edge.segment_id = (*segment)->id;
			new_graph_edge.index = edge_index;
			new_graph_edge.weight = (*segment)->attributes_numeric[CHINA_ROUTER_INDEX_LENGTH_KM];

			new_graph_edge.fnode = (long)(*segment)->attributes_numeric[CHINA_ROUTER_INDEX_FNODE];
			// If id fnode has been used before, find the mapped index
			if ((index_by_id = point_index_by_id.find(new_graph_edge.fnode)) == point_index_by_id.end()) {
				new_graph_edge.point_indexes.first = point_index;
				setup_edge_point_index(point_index, edge_index);
				point_index_by_id.insert(std::pair <long, int>(new_graph_edge.fnode, point_index++));
			}
			else {
				new_graph_edge.point_indexes.first = index_by_id->second;
				setup_edge_point_index(index_by_id->second, edge_index);
			}

			new_graph_edge.tnode = (long)(*segment)->attributes_numeric[CHINA_ROUTER_INDEX_TNODE];
			// If id tnode has been used before, find the mapped index
			if ((index_by_id = point_index_by_id.find(new_graph_edge.tnode)) == point_index_by_id.end()) {
				new_graph_edge.point_indexes.second = point_index;
				setup_edge_point_index(point_index, edge_index);
				point_index_by_id.insert(std::pair <long, int>(new_graph_edge.tnode, point_index++));
			}
			else {
				new_graph_edge.point_indexes.second = index_by_id->second;
				setup_edge_point_index(index_by_id->second, edge_index);
			}

			graph_edges.insert(std::pair <int, astar_graph_edge>(edge_index, new_graph_edge));

			++edge_index;
			// }
		}
	}

	if (!error) {
		num_edges = graph_edges.size();
		EdgePair* edge_array = new EdgePair[num_edges];
		weights = new WeightCostType[num_edges];

		// Fill edge_array and weights
		if (verbose) {
			log += "Edges\n";
			log += "Index\tSegment ID\tPoint Index 1\tPoint Id 1\tPoint Index 2\tPoint iD 2\tWeight\n";
		}
		for (graph_edge = graph_edges.begin(), edge_index = 0;
			graph_edge != graph_edges.end();
			++graph_edge, ++edge_index) {

			if (verbose) {
				log.add_formatted("%d\t%ld", graph_edge->first, graph_edge->second.segment_id);
				log.add_formatted("\t%d\t%ld", graph_edge->second.point_indexes.first, graph_edge->second.fnode);
				log.add_formatted("\t%d\t%ld", graph_edge->second.point_indexes.second, graph_edge->second.tnode);
				log.add_formatted("\t%.4lf\n", graph_edge->second.weight);
			}

			edge_array[edge_index] = graph_edge->second.point_indexes;
			weights[edge_index] = graph_edge->second.weight;
		}

		// Doing point locations after edges to insure that every point is associated with at least 1 edge
		N = point_index_by_id.size();
		locations = new device_coordinate[N];

		if (verbose) {
			log += "Locations\n";
			log += "Index\tPoint ID\tx\ty\n";
		}
		for (point = layer_link_points->objects.begin();
			point != layer_link_points->objects.end();
			++point) {
			if ((index_by_id = point_index_by_id.find((*point)->id)) != point_index_by_id.end()) {
				point_id_by_index.insert(std::pair <int, long>(index_by_id->second, index_by_id->first));
				locations[index_by_id->second].x = (*point)->longitude;
				locations[index_by_id->second].y = (*point)->latitude;
				// ids.push_back ((*point)->id);
				if (verbose)
					log.add_formatted("%d\t%ld\t%ld\t%ld\n", index_by_id->second, index_by_id->first, locations[index_by_id->second].x, locations[index_by_id->second].y);
			}
		}

		// create graph
		g = new mygraph_t(N);
		WeightMap weightmap = get(boost::edge_weight, *g);
		for (std::size_t j = 0; j < num_edges; ++j) {
			edge_descriptor e;
			bool inserted;

			tie(e, inserted) = add_edge(edge_array[j].first, edge_array[j].second, *g);
			weightmap[e] = weights[j];
		}

		if (verbose) {
			log += "A* search\n";
			log += "Edge Array\n";
			log += "Index\tPoint Index 1\tPoint Index 2\tWeight\n";
			for (edge_index = 0; edge_index < num_edges; ++edge_index)
				log.add_formatted("%d\t%d\t%d\t%.4lf\n", edge_index, edge_array[edge_index].first, edge_array[edge_index].second, weights[edge_index]);
			log += "\n";

			log += "Location Array\n";
			log += "Index\tx\ty\n";
			for (point_index = 0; point_index < N; ++point_index)
				log.add_formatted("%d\t%ld\t%ld\n", point_index, locations[point_index].x, locations[point_index].y);
			log += "\n";
		}
	}
	return !error;
}

void astar_router::write_route_line
	(dynamic_string &log)

{
	std::vector <long long>::iterator road_id;

	log.add_formatted ("%lld\t%lld", start_object->id, end_object->id);
	log.add_formatted ("\t%lld\t%lld", start_segment->id, end_segment->id);
	log.add_formatted ("\t%.4lf", distance_start_m / 1000.0);
	log.add_formatted ("\t%.4lf", distance_between_m / 1000.0);
	log.add_formatted ("\t%.4lf", distance_end_m / 1000.0);
	log.add_formatted ("\t%.4lf", (distance_start_m + distance_between_m + distance_end_m) / 1000.0);
	log.add_formatted ("\t%.4lf", elevation_start);
	log.add_formatted ("\t%.4lf", elevation_end);

	for (road_id = result_edges.begin ();
	road_id != result_edges.end ();
	++road_id) {
		if ((road_id == result_edges.begin ())
		&& (*road_id != start_segment->id))
			log.add_formatted ("\t%lld", start_segment->id);

		log.add_formatted ("\t%lld", *road_id);
	}
	log += "\n";
}


bool astar_route_all_multithread
	(map_layer *layer_stations,
	map_layer *layer_counties,
	map_layer *layer_rivers,
	map_layer *layer_link_points,
	interface_window *view,
	dynamic_string &log)

{
	std::vector <map_object *>::const_iterator station, county;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator i_job;
	thread_work_AstarRouting *job;
	thread_manager threader;
	thread_manager_statistics stats;
	int thread_count = 24, station_index;
	long job_count;
	bool error = false;
	astar_graph graph;

	if (graph.setup_graph (layer_link_points, layer_rivers, false, log)) {


		job_count = 0;

		for (station = layer_stations->objects.begin (), station_index = 0;
		station != layer_stations->objects.end ();
		++station, ++station_index) {

			if ((station_index >= 0)
			&& (station_index < 10)) {
				job = new thread_work_AstarRouting;
				job->work_function = &work_Astar_Routing;
				job->graph = &graph;
				job->router.start_object = (*station);
				job->layer_counties = layer_counties;
				job->layer_rivers = layer_rivers;
				job->layer_link_points = layer_link_points;
				job->view = view;

				job->id = ++job_count;
				job->description.format ("%ld Station %ld", job->id, job->router.start_object->id);
				jobs.insert (std::pair <long, thread_work *> (job->id, job));
			}
		}

		threader.thread_count = thread_count;
		threader.maximum_restart_count = 0;
		threader.wait_interval_ms = 5;
		threader.pause_time_minimum_seconds = 0;
		threader.pause_time_maximum_seconds = 1;
		stats.reset (thread_count);
		threader.statistics = &stats;

		view->set_data (&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
		threader.run (&jobs, view, log);
		// threader.run_tiny (&jobs, view, log);
		view->set_data (NULL);

		view->update_status_bar ("Threader run complete");

		log += "Start\tEnd\tStart Segment\tEnd Segment\tStart Distance km\tDistance Between km\tEnd Distance km\tTotal Distance km\tStart Elevation\tEnd Elevation\tSegment";
		log += "\n";

		for (i_job = jobs.begin ();
		i_job != jobs.end ();
		++i_job) {
			log += i_job->second->log;
			delete i_job->second;
		}
	}

	return !error;
}

bool write_batch_log
	(dynamic_string &filename,
	dynamic_string &log)

{
	FILE *f;
	
	if (fopen_s (&f, filename.get_text_ascii (), "w") == 0) {
		fwrite (log.get_text_ascii (), log.get_length (), 1, f);
		fclose (f);
		return true;
	}
	else
		return false;
}



bool astar_route_all
	(map_layer *layer_stations,
	map_layer *layer_counties,
	map_layer *layer_rivers,
	map_layer *layer_link_points,
	interface_window *view,
	dynamic_string &log)

// Some boost internals not threadsafe?

{
	std::vector <map_object *>::const_iterator station, county;
	int station_index, county_index;
	bool error = false;
	dynamic_string filename_batch_log, batch_log;
	astar_graph graph;

	if (graph.setup_graph (layer_link_points, layer_rivers, false, log)) {


		for (station = layer_stations->objects.begin (), station_index = 0;
		station != layer_stations->objects.end ();
		++station, ++station_index) {

			// if ((station_index >= 0)
			// && (station_index < 10)) {
				filename_batch_log.format ("I:\\TDC\\China rivers\\5 level river network\\Routed Stations\\WaterStation%04ld.tsv", (*station)->id);
				batch_log.clear ();
				batch_log += "Start\tEnd\tStart Segment\tEnd Segment\tStart Distance km\tDistance Between km\tEnd Distance km\tTotal Distance km\tStart Elevation\tEnd Elevation\tSegment";
				batch_log += "\n";

				for (county = layer_counties->objects.begin (), county_index = 0;
				county != layer_counties->objects.end ();
				++county, ++county_index) {
					// if ((county_index >= 0)
					// && (county_index < 300)) {
						astar_router router;

						router.start_object = *station;
						router.end_object = *county;
						view->update_formatted ("%ld->%ld", router.start_object->id, router.end_object->id);

						if (router.route_quiet (layer_link_points, layer_rivers, &graph, batch_log))
							router.write_route_line (batch_log);
					// }
				}
				write_batch_log (filename_batch_log, batch_log);
			// }
		}
	}

	return !error;
}
