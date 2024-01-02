
#include "../util/utility_afx.h"
#include <vector>
#include <map>
#include "../util/dynamic_string.h"
#include "../util/bounding_cube.h"
#include "../Collada/camera_time.h"
#include "../Collada/collada_animation.h"
#include "../Collada/collada_object.h"
#include "elevation_matrix.h"

elevation_matrix_cell::elevation_matrix_cell ()

{
}

elevation_matrix_cell::elevation_matrix_cell
	(const elevation_matrix_cell &other)

{
	copy (other);
}

void elevation_matrix_cell::copy
	(const elevation_matrix_cell &other)

{
	points = other.points;
}

elevation_matrix_cell elevation_matrix_cell::operator =
	(const elevation_matrix_cell& other)

{
	copy (other);

	return (*this);
}

void elevation_matrix::add
	(const logical_coordinate &point)

{
	/*
	std::map <double, elevation_matrix_column>::iterator column;

	if ((column = longitude_columns.find (point.x)) != longitude_columns.end ()) {
		std::map <double, logical_coordinate>::iterator row;
		if ((row = column->second.points_by_latitude.find (point.y)) != column->second.points_by_latitude.end ())
			// new value at this location
			row->second.z = point.z;
		else
			column->second.points_by_latitude.insert (std::pair <double, logical_coordinate> (point.y, point));
	}
	else {
		elevation_matrix_column new_column;
		new_column.points_by_latitude.insert (std::pair <long, logical_coordinate> (point.y, point));
		longitude_columns.insert (std::pair <double, elevation_matrix_column> (point.x, new_column));
	}
	*/
	data.push_back (point);
	border.check_extent (point.x, point.y, point.z);
}

double fast_distance_2d_squared
	(const logical_coordinate &p1,
	const logical_coordinate &p2)
	
// no need for sqrt if just comparing for nearest

{
	return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

bool elevation_matrix_cell::nearest
	(const logical_coordinate &target,
	logical_coordinate *best,
	double *distance_squared) const

// Find value nearest y

{
	/*
	std::map <double, logical_coordinate>::const_iterator close, just_below, just_above;

	if ((close = points_by_latitude.lower_bound (y)) != points_by_latitude.end ()) {

		double distance1, distance2, distance3;
		just_below = just_above = close;
		--just_below;
		++just_above;

		distance2 = fast_distance_2d_squared (point, close->second);

		if (just_below != points_by_latitude.end ()) {
			distance1 = fast_distance_2d_squared (point, just_below->second);
			if (distance2 <= distance1) {
				*distance_squared = distance2;
				point = close->second;
			}
			else {
				*distance_squared = distance1;
				point = just_below->second;
			}
		}
		else {
			*distance_squared = distance2;
			point = close->second;
		}

		if (just_above != points_by_latitude.end ()) {
			distance3 = fast_distance_2d_squared (point, just_above->second);
			if (*distance_squared > distance3) {
				*distance_squared = distance3;
				point = just_above->second;
			}
		}
	}
	else
		// nothing at or before y
		if ((close = points_by_latitude.begin ()) != points_by_latitude.end ()) {
			*distance_squared = fast_distance_2d_squared (point, close->second);
			point = close->second;
		}
		else
			return false;
	*/
	std::vector <logical_coordinate>::const_iterator p;
	double distance;
	bool candidate_found = false;

	for (p = points.begin ();
	p != points.end ();
	++p) {
		if ((distance = fast_distance_2d_squared (*p, target)) < *distance_squared) {
			*distance_squared = distance;
			*best = *p;
			candidate_found = true;
		}
	}

	return candidate_found;
}

elevation_matrix::elevation_matrix ()

{
}

elevation_matrix::~elevation_matrix ()

{
}

void elevation_matrix::set_nearest ()

// move points from vector into grid

{
	std::vector <logical_coordinate>::const_iterator point;
	std::vector <logical_coordinate>::iterator other, near_1, near_2;
	double near_1_squared = DBL_MAX, near_2_squared = DBL_MAX, other_squared;

	for (point = data.begin ();
	point != data.end ();
	++point) {
		near_1 = near_2 = data.end ();
		for (other = data.begin ();
		other != data.end ();
		++other) {
			if (point != other) {
				other_squared = fast_distance_2d_squared (*point, *other);
				if (near_1 != data.end ()) {
					if (near_2 != data.end ()) {
						if (other_squared < near_2_squared) {
							if (other_squared < near_1_squared) {
								// new #1 closest
								near_2 = near_1;
								near_2_squared = near_1_squared;
								near_1 = other;
								near_1_squared = other_squared;
							}
							else {
								// new #2 closest
								near_2 = near_1;
								near_2_squared = other_squared;
							}
						}
					}
					else {
						other_squared = fast_distance_2d_squared (*point, *other);
						if (other_squared < near_1_squared) {
							near_2 = near_1;
							near_2_squared = near_1_squared;
							near_1 = other;
							near_1_squared = other_squared;
						}
						else {
							near_2 = other;
							near_2_squared = other_squared;
						}
					}
				}
				else {
					near_1 = other;
					near_1_squared = other_squared;
				}
			}
		}
		nearest_1.push_back (*near_1);
		nearest_2.push_back (*near_2);
	}
}

double elevation_matrix::nearest_elevation
	(const logical_coordinate &position) const

{
	return nearest_point (position).z;
}

logical_coordinate elevation_matrix::nearest_point
	(const logical_coordinate &position) const

// elevations is heatmap from 3DEP gridfloat

// Find nearest position

{
	/*
	int x, y;
	double cell_size_x, cell_size_y, distance_squared = DBL_MAX;
	logical_coordinate best;

	cell_size_x = (border.extent_x () * 1.01) / (double) ELEVATION_MATRIX_WIDTH;
	cell_size_y = (border.extent_y () * 1.01) / (double) ELEVATION_MATRIX_HEIGHT;

	x = (position.x - border.x [0]) / cell_size_x;
	y = (position.y - border.y [0]) / cell_size_y;

	grid [x + y * 10].nearest (position, &best, &distance_squared);

	// check 8 nearby squares
	if (x > 0) {
		if (y > 0)
			grid [(x - 1) + (y - 1) * ELEVATION_MATRIX_WIDTH].nearest (position, &best, &distance_squared);
		grid [(x - 1) + y * ELEVATION_MATRIX_WIDTH].nearest (position, &best, &distance_squared);
		if (y < ELEVATION_MATRIX_HEIGHT)
			grid [(x - 1) + (y + 1) * ELEVATION_MATRIX_WIDTH].nearest (position, &best, &distance_squared);
	}

	if (y > 0)
		grid [x + (y - 1) * ELEVATION_MATRIX_WIDTH].nearest (position, &best, &distance_squared);
	grid [x + (y + 1) * ELEVATION_MATRIX_WIDTH].nearest (position, &best, &distance_squared);

	if (x < ELEVATION_MATRIX_HEIGHT) {
		if (y > 0)
			grid [(x + 1) + (y - 1) * ELEVATION_MATRIX_WIDTH].nearest (position, &best, &distance_squared);
		grid [(x + 1) + y * ELEVATION_MATRIX_WIDTH].nearest (position, &best, &distance_squared);
		if (y < ELEVATION_MATRIX_HEIGHT)
			grid [(x + 1) + (y + 1) *ELEVATION_MATRIX_WIDTH].nearest (position, &best, &distance_squared);
	}

	if (distance_squared != DBL_MAX)
		return best;
	else {
	*/
		// check all
		std::vector <logical_coordinate>::const_iterator p;
		logical_coordinate best;
		double distance, distance_squared = DBL_MAX;

		for (p = data.begin ();
		p != data.end ();
		++p) {
			if ((distance = fast_distance_2d_squared (*p, position)) < distance_squared) {
				distance_squared = distance;
				best = *p;
			}
		}

		return best;

	/*
	}
	std::map <double, elevation_matrix_column>::const_iterator left, center, right;
	logical_coordinate center_nearest, left_nearest, right_nearest;
	double distance_left, distance, distance_right;

	if ((center = longitude_columns.lower_bound (position.x)) != longitude_columns.end ()) {
		double distance1, distance3;

		left = right = center;
		--left;
		right = center;
		++right;

		center->second.nearest (position.y, center_nearest, &distance);

		if (left != longitude_columns.end ()) {
			left->second.nearest (position.y, left_nearest, &distance_left);
			if (distance > distance_left) {
				distance = distance_left;
			}
		}

		if (right != longitude_columns.end ()) {
			right->second.nearest (position.y, right_nearest, &distance_right);
			if (distance > distance_right) {
				distance = distance_right;
			}
		}
	}
	else
		// nothing at or before x
		if ((center = longitude_columns.begin ()) != longitude_columns.end ()) {
			right = center;
			++right;

			center->second.nearest (position.y, center_nearest, &distance);

			if (right != longitude_columns.end ()) {
				right->second.nearest (position.y, right_nearest, &distance_right);
				if (distance > distance_right) {
					distance = distance_right;
				}
			}
		}
		else
			return false;

	std::vector <logical_coordinate>::const_iterator p;
	double distance, best = DBL_MAX, best_z = 0.0;

	for (p = data.begin ();
	p != data.end ();
	++p) {
		if ((distance = fast_distance_2d_squared (*p, position)) < best) {
			best = distance;
			best_z = p->z;
		}
	}

	return best_z;
	*/
}

bool elevation_matrix::empty () const

{
	return data.size () == 0;
}

void elevation_matrix::compute_normals
	(elevation_matrix *normals) const

{
	std::vector <logical_coordinate>::const_iterator point, near_1, near_2;
	logical_coordinate c2, c3, normal_point;

	for (point = data.begin (), near_1 = nearest_1.begin (), near_2 = nearest_2.begin ();
	point != data.end ();
	++point, ++near_1, ++near_2) {
		compute_normal_face (*point, *near_1, *near_2, &normal_point);
		normals->data.push_back (normal_point);
	}

}