#include "../util/utility_afx.h"
#include <vector>
#include <stack>
#include <map>
#include "../util/list.h"
#include "../util/dynamic_string.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/RecentlyOpened.h"
#include "../util/xml_parser.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/map_scale.h"
#include "../map/shapefile.h"

#include <CGAL/basic.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Point_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_traits_2.h>
#include <CGAL/General_polygon_set_2.h>
#include <CGAL/approximated_offset_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Gps_circle_segment_traits_2.h>

#include "MinkowskiBuffer.h"

// From Minkowski2d sample bops_circular.h
// typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
//  typedef CGAL::Gps_circle_segment_traits_2<Kernel>         Traits;
//  typedef CGAL::General_polygon_set_2<Traits>               Polygon_set_2;
// typedef Traits::Polygon_2                                 Polygon_2;
// typedef Traits::Polygon_with_holes_2                      Polygon_with_holes_2;


typedef CGAL::Cartesian <double> R;
typedef CGAL::Polygon_traits_2 <R> Traits;
typedef Traits::Point_2 Point;
typedef std::vector <Point> Container;
// typedef CGAL::Polygon_2<Traits,Container> Polygon;

bool MinkowskiBuffer::create
	(map_layer *river,
	dynamic_string &log)

// http://doc.cgal.org/latest/Minkowski_sum_2/Minkowski_sum_2_2approx_offset_8cpp-example.html

{
	bool error = false;
	std::vector <map_object *> ::const_iterator segment;
	int index;

	CGAL::Polygon_2 <Traits, std::vector <Traits::Point_2>> P;
	Traits::Point_2 point;

	// in_file >> P;
	for (segment = river->objects.begin ();
	segment != river->objects.end ();
	++segment) {
		for (index = 0;
		index <= ((map_polygon *) (* segment))->node_count;
		++index) {
			P.push_back (Point ( (double) ((map_polygon *) (* segment))->nodes [index * 2] / 1.0e6, (double) ((map_polygon *) (* segment))->nodes [index * 2 + 1] / 1.0e6));
		}
	}

	/*
	std::cout << "Read an input polygon with " << P.size() << " vertices."            << std::endl;
	// Approximate the offset polygon with radius 5 and error bound 0.00001.
	// boost::timer timer;
	// Traits::Polygon_with_holes_2 offset = CGAL::approximated_offset_2  (P, 5, 0.00001);
	CGAL::ArrangementDirectionalXMonotoneTraits_2 <Trails::Point_2> offset = CGAL::approximated_offset_2  (P, 5, 0.00001);
	// double secs = timer.elapsed();
	std::cout << "The offset polygon has " << offset.outer_boundary().size()            << " vertices, " << offset.number_of_holes() << " holes."            << std::endl;
	// std::cout << "Offset computation took " << secs << " seconds." << std::endl;
	*/

	return !error;
}
