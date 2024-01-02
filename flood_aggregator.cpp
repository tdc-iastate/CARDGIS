#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include "../util/dynamic_string.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "flood_aggregator.h"

const long ADJACENT_X1 = 23500;
const long ADJACENT_X2 = 29000;
const long ADJACENT_Y1 = 18000;
const long ADJACENT_Y2 = 20000;
const long SEEK_SAME_X = 5000;
const long SEEK_SAME_Y = 3500; // ID 317837 is 1132 above 317838

flood_boundary::flood_boundary ()
{
	left = right = NULL;
	available = false;
}

flood_boundary::flood_boundary (const flood_boundary &other)
{
	copy (other);
}

flood_boundary flood_boundary::operator =
	(const flood_boundary &other)
{
	if (this != &other)
		copy(other);

	return (*this);
}
void flood_boundary::copy
	(const flood_boundary &other)
{
	left = other.left;
	right = other.right;
	available = other.available;
}

flood_grid_cell::flood_grid_cell()

{
	source_point = NULL;
	x_index = y_index = 0;
	wind_speed = -1;
	latitude = longitude = 0;
	memset (neighbor, 0, sizeof (flood_grid_cell *) * 8);
	memset (outsider, 0, sizeof (flood_grid_cell *) * 8);
}

flood_grid_cell::flood_grid_cell
	(class map_object *new_point,
	const int new_x,
	const int new_y)

{
	source_point = new_point;
	x_index = new_x;
	y_index = new_y;
	wind_speed = -1;
	latitude = longitude = 0;
	memset (neighbor, 0, sizeof (flood_grid_cell *) * 8);
	memset (outsider, 0, sizeof (flood_grid_cell *) * 8);
}

flood_grid_cell::flood_grid_cell
	(const flood_grid_cell &other)

{
	copy(other);
}

flood_grid_cell::~flood_grid_cell()

{
}

flood_grid_cell flood_grid_cell::operator =
	(const flood_grid_cell &other)
{
	if (this != &other)
		copy(other);

	return (*this);
}

void flood_grid_cell::clear()
{
	source_point = NULL;
	x_index = y_index = 0;
	wind_speed = -1;
	latitude = longitude = 0;
	memset (neighbor, 0, sizeof (flood_grid_cell *) * 8);
	memset (outsider, 0, sizeof (flood_grid_cell *) * 8);
}

void flood_grid_cell::copy
	(const flood_grid_cell &other)

{
	source_point = other.source_point;
	x_index = other.x_index;
	y_index = other.y_index;
	wind_speed = other.wind_speed;
	latitude = other.latitude;
	longitude = other.longitude;
	memcpy (neighbor, other.neighbor, sizeof (flood_grid_cell *) * 8);
	memcpy (outsider, other.outsider, sizeof (flood_grid_cell *) * 8);
	boundaries[0] = other.boundaries[0];
	boundaries[1] = other.boundaries[1];
	boundaries[2] = other.boundaries[2];
	boundaries[3] = other.boundaries[3];
}

flood_grid_matrix::flood_grid_matrix ()

{
	cells = NULL;
}

flood_grid_matrix::~flood_grid_matrix ()

{
	if (cells)
		delete[] cells;
}

void flood_grid_matrix::clear_use ()

{
	int c, limit;

	limit = count_columns * count_rows;

	for (c = 0; c < limit; ++c)
		cells[c].used = false;
}

void flood_grid_matrix::clear_neighbors ()

{
	int c, limit, direction;

	limit = count_columns * count_rows;

	for (c = 0; c < limit; ++c) {
		for (direction = 0; direction < 8; ++direction) {
			cells[c].neighbor[direction] = NULL;
			cells[c].outsider[direction] = NULL;
		}
		cells[c].boundaries[0].available = cells[c].boundaries[1].available = cells[c].boundaries[2].available = cells[c].boundaries[3].available = false;
	}
}

flood_grid_cell *flood_grid_matrix::get
	(const int x_index,
	const int y_index)

{
	if (cells
	&& (x_index >= 0)
	&& (x_index < count_columns)
	&& (y_index >= 0)
	&& (y_index < count_rows))
		return &cells[y_index * count_columns + x_index];
	else
		return NULL;
}

void flood_grid_matrix::set_size
	(const int max_x,
	const int max_y)

{
	count_columns = max_x;
	count_rows = max_y;

	cells = new flood_grid_cell[count_columns * count_rows];

	for (int y = 0; y < count_rows; ++y)
		for (int x = 0; x < count_columns; ++x) {
			cells[y * count_columns + x].x_index = x;
			cells[y * count_columns + x].y_index = y;
		}

}

void flood_grid_matrix::fill_map_layer
	(map_layer *grid_points,
	const int wind_speed_attribute_index)

{
	map_object *point;
	flood_grid_cell *cell;

	for (int y = 0; y < count_rows; ++y)
		for (int x = 0; x < count_columns; ++x) {
			cell = get (x, y);
			if (cell->source_point) {
				point = grid_points->create_new (MAP_OBJECT_POINT);
				point->id = cell->source_point->id;
				point->latitude = cell->source_point->latitude;
				point->longitude = cell->source_point->longitude;
				point->attributes_numeric[0] = (double) cell->y_index;
				point->attributes_numeric[1] = (double) cell->x_index;
				point->attributes_numeric[2] = cell->source_point->attributes_numeric[wind_speed_attribute_index];
				grid_points->objects.push_back (point);
			}
		}

}

void flood_grid_matrix::set_from_vector
	(std::vector <flood_grid_cell> *cell_vector)

{
	std::vector <flood_grid_cell>::const_iterator cell;

	for (cell = cell_vector->begin ();
	cell != cell_vector->end ();
	++cell) {
		ASSERT (cell->y_index * count_columns + cell->x_index < (count_rows * count_columns));
		cells[cell->y_index * count_columns + cell->x_index].source_point = cell->source_point;
	}
}

void flood_grid_matrix::set_from_map_layer
	(map_layer *grid_points)

{
	std::vector <map_object *>::const_iterator point;
	int x, y;

	for (point = grid_points->objects.begin ();
	point != grid_points->objects.end ();
	++point) {
		y = (*point)->attributes_numeric[0];
		x = (*point)->attributes_numeric[1];
		cells[y * count_columns + x].wind_speed = (int) (*point)->attributes_numeric[2];
		cells[y * count_columns + x].latitude = (*point)->latitude;
		cells[y * count_columns + x].longitude = (*point)->longitude;
		// cells[y * count_columns + x].point = points->match_id ((*point)->id);
	}
}

void flood_grid_matrix::set_vector_x
	(const int x,
	std::vector <flood_grid_cell> *vector_points)
{
	vector_points->clear ();

	for (int y = 0;
	y < count_rows;
	++y)
		vector_points->push_back(cells [y * count_columns + x]);
}

void flood_grid_matrix::set_vector_y
	(const int y,
	std::vector <flood_grid_cell> *vector_points)
{
	vector_points->clear ();
	for (int x = 0;
	x < count_columns;
	++x)
		vector_points->push_back(cells [y * count_columns + x]);
}

void flood_fill
	(const int x,
	const int y,
	const int wind_speed_ge,
	const int wind_speed_lt,
	const int data_attribute_index,
	flood_grid_matrix *grid,
	std::vector <flood_grid_cell> *in_polygon)

{
	std::vector <map_object *>::const_iterator point;
	flood_grid_cell *cell;

	if (((cell = grid->get (x, y)) != NULL)
	&& !cell->used) {
		// if ((cell->point != NULL)
		// && (cell->point->attributes_numeric[data_attribute_index] == wind_speed)) {
		// if (cell->wind_speed == wind_speed)
		if ((cell->wind_speed >= wind_speed_ge)
		&& (cell->wind_speed < wind_speed_lt)) {
			cell->used = true;
			in_polygon->push_back (*cell);

			// left
			flood_fill (x - 1, y, wind_speed_ge, wind_speed_lt, data_attribute_index, grid, in_polygon);
			// right
			flood_fill (x + 1, y, wind_speed_ge, wind_speed_lt, data_attribute_index, grid, in_polygon);
			// up
			flood_fill (x, y + 1, wind_speed_ge, wind_speed_lt, data_attribute_index, grid, in_polygon);
			// down
			flood_fill (x, y - 1, wind_speed_ge, wind_speed_lt, data_attribute_index, grid, in_polygon);

			// top left
			flood_fill (x - 1, y + 1, wind_speed_ge, wind_speed_lt, data_attribute_index, grid, in_polygon);
			// top right
			flood_fill (x + 1, y + 1, wind_speed_ge, wind_speed_lt, data_attribute_index, grid, in_polygon);
			// bottom right
			flood_fill (x + 1, y - 1, wind_speed_ge, wind_speed_lt, data_attribute_index, grid, in_polygon);
			// bottom left
			flood_fill (x - 1, y - 1, wind_speed_ge, wind_speed_lt, data_attribute_index, grid, in_polygon);
		}
	}
}

void seek_adjacent
	(const flood_grid_cell &cell,
	std::vector <map_object *> *points_matching,
	std::vector <flood_grid_cell> *in_polygon,
	bool *used)

// Rows are about 18,880 apart.  Each point in a row may be 1743 further north

// Columns are 23,919 .. 28,700 apart

{
	std::vector <map_object *>::const_iterator point;
	long offset_x, offset_y;

	in_polygon->push_back (cell);
	used[cell.source_point->id - 1] = true;

	// left
	for (point = points_matching->begin ();
	point != points_matching->end ();
	++point) {

		if (!used[(*point)->id - 1]) {
			offset_x = (*point)->longitude - cell.source_point->longitude;

			if ((offset_x > -ADJACENT_X2)
			&& (offset_x < ADJACENT_X2)) {
				offset_y = (*point)->latitude - cell.source_point->latitude;
				if ((offset_y > -ADJACENT_Y2)
				&& (offset_y < ADJACENT_Y2)) {

					// West of 96 W, vertical lines are oriented NW-SE and horizontal lines are oriented SW-NE
					// At 96 W, north-south and east-west lines are square
					// East of 96 W, vertical lines are oriented NE-SW and horizontal lines are oriented SE-NW

					if (cell.source_point->longitude < -97000000) {
						// West of 96 W, vertical lines are oriented NW-SE and horizontal lines are oriented SW-NE=

						if (offset_x > ADJACENT_X1) {
							if (offset_y > ADJACENT_Y1) {
								// North East
								seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index + 1), points_matching, in_polygon, used);
							}
							else
								if (offset_y < -ADJACENT_Y1) {
									// South East
									seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index - 1), points_matching, in_polygon, used);
								}
								else
									if ((offset_y > 0)
									&& (offset_y < SEEK_SAME_Y))
										// directly east
										seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index), points_matching, in_polygon, used);
						}
						else
							if (offset_x < -ADJACENT_X1) {
								if (offset_y > ADJACENT_Y1) {
									// North West
									seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index + 1), points_matching, in_polygon, used);
								}
								else
									if (offset_y < -ADJACENT_Y1) {
										// South West
										seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index - 1), points_matching, in_polygon, used);
									}
									else
										if ((offset_y > -SEEK_SAME_Y)
										&& (offset_y < 0))
											// West
											seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index), points_matching, in_polygon, used);
							}
							else
								// vertical lines are oriented NW-SE
								if ((offset_x < 0)
								&& (offset_x > -SEEK_SAME_X)) {
									if (offset_y > ADJACENT_Y1)
										// up
										seek_adjacent (flood_grid_cell (*point, cell.x_index, cell.y_index + 1), points_matching, in_polygon, used);
								}
								else
									if ((offset_x > 0)
									&& (offset_x < SEEK_SAME_X)) {
										if (offset_y < -ADJACENT_Y1)
											// down
											seek_adjacent (flood_grid_cell (*point, cell.x_index, cell.y_index - 1), points_matching, in_polygon, used);
									}
					}
					else
						if (cell.source_point->longitude < -95000000) {
							// Center of map, lines are pretty square

							if (offset_x > ADJACENT_X1) {
								if (offset_y > ADJACENT_Y1) {
									// North East
									seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index + 1), points_matching, in_polygon, used);
								}
								else
									if (offset_y < -ADJACENT_Y1) {
										// South East
										seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index - 1), points_matching, in_polygon, used);
									}
									else
										if ((offset_y > -1000)
										&& (offset_y < 1000))
											// directly east
											seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index), points_matching, in_polygon, used);
							}
							else
								if (offset_x < -ADJACENT_X1) {
									if (offset_y > ADJACENT_Y1) {
										// North West
										seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index + 1), points_matching, in_polygon, used);
									}
									else
										if (offset_y < -ADJACENT_Y1) {
											// South West
											seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index - 1), points_matching, in_polygon, used);
										}
										else
											if ((offset_y > -1000)
											&& (offset_y < 1000))
												// West
												seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index), points_matching, in_polygon, used);
								}
								else
									if ((offset_x > 0)
									&& (offset_x < SEEK_SAME_X)) {
										if (offset_y > ADJACENT_Y1)
											// up
											seek_adjacent (flood_grid_cell (*point, cell.x_index, cell.y_index + 1), points_matching, in_polygon, used);
									}
									else
										if ((offset_x > -SEEK_SAME_X)
										&& (offset_x < 0)) {
											if (offset_y < -ADJACENT_Y1)
												// down
												seek_adjacent (flood_grid_cell (*point, cell.x_index, cell.y_index - 1), points_matching, in_polygon, used);
										}
						}
						else {
							// Eastern section
							// East of 96 W, vertical lines are oriented NE-SW and horizontal lines are oriented SE-NW
							if (offset_x > ADJACENT_X1) {
								if (offset_y > ADJACENT_Y1) {
									// North East
									seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index + 1), points_matching, in_polygon, used);
								}
								else
									if (offset_y < -ADJACENT_Y1) {
										// South East
										seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index - 1), points_matching, in_polygon, used);
									}
									else
										if ((offset_y > -SEEK_SAME_Y)
										&& (offset_y < 0)) {
											// directly east
											seek_adjacent (flood_grid_cell (*point, cell.x_index + 1, cell.y_index), points_matching, in_polygon, used);
										}
							}
							else
								if (offset_x < -ADJACENT_X1) {
									if (offset_y > ADJACENT_Y1) {
										// North West
										seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index + 1), points_matching, in_polygon, used);
									}
									else
										if (offset_y < -ADJACENT_Y1) {
											// South West
											seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index - 1), points_matching, in_polygon, used);
										}
										else
											if ((offset_y > 0)
											&& (offset_y < SEEK_SAME_Y)) {
												// West
												seek_adjacent (flood_grid_cell (*point, cell.x_index - 1, cell.y_index), points_matching, in_polygon, used);
											}
								}
								else
									// vertical lines are oriented NE-SW
									if ((offset_x > 0)
									&& (offset_x < SEEK_SAME_X)) {
										if (offset_y > ADJACENT_Y1) {
											// up
											seek_adjacent (flood_grid_cell (*point, cell.x_index, cell.y_index + 1), points_matching, in_polygon, used);
										}
									}
									else
										if ((offset_x > -SEEK_SAME_X)
										&& (offset_x < 0)) {
											if (offset_y < -ADJACENT_Y1) {
												// down
												seek_adjacent (flood_grid_cell (*point, cell.x_index, cell.y_index - 1), points_matching, in_polygon, used);
											}
										}

						}

				}

			}
		}
	}
}

flood_grid_cell *leftmost_cell_on_row
	(const int y,
	const std::vector <flood_grid_cell> *in_polygon)

{
	std::vector <flood_grid_cell>::const_iterator cell;
	flood_grid_cell *mostest = NULL;

	for (cell = in_polygon->begin ();
	cell != in_polygon->end ();
	++cell) {
		if (cell->y_index == y) {
			if ((mostest == NULL)
			|| (mostest->x_index > cell->x_index))
				mostest = (flood_grid_cell *) &*cell;
		}
	}
	return mostest;
}

flood_grid_cell *rightmost_cell_on_row
	(const int y,
	const std::vector <flood_grid_cell> *in_polygon)

{
	std::vector <flood_grid_cell>::const_iterator cell;
	flood_grid_cell *mostest = NULL;

	for (cell = in_polygon->begin ();
	cell != in_polygon->end ();
	++cell) {
		if (cell->y_index == y) {
			if ((mostest == NULL)
			|| (mostest->x_index < cell->x_index))
				mostest = (flood_grid_cell *) &*cell;
		}
	}
	return mostest;
}

void find_ranges
	(std::vector <flood_grid_cell> *in_polygon,
	long *min_x,
	long *min_y,
	long *max_x,
	long *max_y,
	int *cell_min_x,
	int *cell_min_y,
	int *cell_max_x,
	int *cell_max_y)

{
	std::vector <flood_grid_cell>::const_iterator cell;
	*min_x = *min_y = MAXLONG;
	*max_x = *max_y = -MAXLONG;
	*cell_min_x = *cell_min_y = MAXINT;
	*cell_max_x = *cell_max_y = -MAXINT;

	for (cell = in_polygon->begin ();
	cell != in_polygon->end ();
	++cell) {
		if (cell->latitude < *min_y)
			*min_y = cell->latitude;
		if (cell->latitude > *max_y)
			*max_y = cell->latitude;

		if (cell->longitude < *min_x)
			*min_x = cell->longitude;
		if (cell->longitude > *max_x)
			*max_x = cell->longitude;

		if (cell->x_index < *cell_min_x)
			*cell_min_x = cell->x_index;
		if (cell->x_index > *cell_max_x)
			*cell_max_x = cell->x_index;

		if (cell->y_index < *cell_min_y)
			*cell_min_y = cell->y_index;
		if (cell->y_index > *cell_max_y)
			*cell_max_y = cell->y_index;
	}

}

void offset
	(std::vector <flood_grid_cell> *in_polygon,
	const int cell_min_x,
	const int cell_min_y,
	int *cell_max_x,
	int *cell_max_y)

{
	std::vector <flood_grid_cell>::iterator cell;
	int offset_x, offset_y;

	offset_x = cell_min_x * -1;
	offset_y = cell_min_y * -1;

	for (cell = in_polygon->begin ();
	cell != in_polygon->end ();
	++cell) {
		cell->x_index += offset_x;
		cell->y_index += offset_y;
	}
	*cell_max_x += offset_x;
	*cell_max_y += offset_y;
}

void outline_clockwise
	(flood_grid_cell *cursor,
	const int sweep_direction,
	map_polygon *outline,
	const device_coordinate &border_size)

// Starts at leftmost cell of bottom row
// Looks clockwise

// No neighbors exist in sweep_direction

{
	flood_grid_cell *neighbor;

	if (!cursor->used) {
		cursor->used = true;
		switch (sweep_direction) {
		case 0:
			// Cursor has nothing above it
			ASSERT (cursor->neighbor[FLOOD_CELL_GRID_TOP] == NULL);

			// top left corner of row
			outline->add_point (cursor->latitude + border_size.y, cursor->longitude - border_size.x);
			// top right corner of row
			outline->add_point (cursor->latitude + border_size.y, cursor->longitude + border_size.x);

			if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOPRIGHT]) != NULL)
				// 1
				outline_clockwise (neighbor, 270, outline, border_size);
			else
				if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHT]) != NULL)
					// 2
					outline_clockwise (neighbor, 0, outline, border_size);
				else
					if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM]) != NULL)
						// 3
						outline_clockwise (neighbor, 0, outline, border_size);
					else
						if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOM]) != NULL)
							// 4
							outline_clockwise (neighbor, 90, outline, border_size);
						else
							if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT]) != NULL)
								// 5
								outline_clockwise (neighbor, 90, outline, border_size);
							else
								if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFT]) != NULL)
									// 6
									outline_clockwise (neighbor, 180, outline, border_size);

			break;
		case 90:
			// Cursor has nothing to the right
			ASSERT (cursor->neighbor[FLOOD_CELL_GRID_RIGHT] == NULL);

			// top right corner of row
			outline->add_point (cursor->latitude + border_size.y, cursor->longitude + border_size.x);
			// bottom right corner of row
			outline->add_point (cursor->latitude - border_size.y, cursor->longitude + border_size.x);

			if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM]) != NULL)
				// 1
				outline_clockwise (neighbor, 0, outline, border_size);
			else
				if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOM]) != NULL)
					// 2
					outline_clockwise (neighbor, 90, outline, border_size);
				else
					if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT]) != NULL)
						// 3
						outline_clockwise (neighbor, 90, outline, border_size);
					else
						if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFT]) != NULL)
							// 4
							outline_clockwise (neighbor, 180, outline, border_size);
						else
							if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFTTOP]) != NULL)
								// 5
								outline_clockwise (neighbor, 180, outline, border_size);
							else
								if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOP]) != NULL)
									// 6
									outline_clockwise (neighbor, 270, outline, border_size);
			break;
		case 180:
			// Cursor has nothing below
			ASSERT (cursor->neighbor[FLOOD_CELL_GRID_BOTTOM] == NULL);

			// bottom right corner of row
			outline->add_point (cursor->latitude - border_size.y, cursor->longitude + border_size.x);
			// bottom left corner of row
			outline->add_point (cursor->latitude - border_size.y, cursor->longitude - border_size.x);

			if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT]) != NULL)
				// 1
				outline_clockwise (neighbor, 90, outline, border_size);
			else
				if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFT]) != NULL)
					// 2
					outline_clockwise (neighbor, 180, outline, border_size);
				else
					if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFTTOP]) != NULL)
						// 3
						outline_clockwise (neighbor, 180, outline, border_size);
					else
						if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOP]) != NULL)
							// 4
							outline_clockwise (neighbor, 270, outline, border_size);
						else
							if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOPRIGHT]) != NULL)
								// 5
								outline_clockwise (neighbor, 270, outline, border_size);
							else
								if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHT]) != NULL)
									// 6
									outline_clockwise (neighbor, 0, outline, border_size);
			break;
		case 270:
			// Cursor has nothing to the left
			ASSERT (cursor->neighbor[FLOOD_CELL_GRID_LEFT] == NULL);

			// bottom left corner of row
			outline->add_point (cursor->latitude - border_size.y, cursor->longitude - border_size.x);

			// top left corner of row
			outline->add_point (cursor->latitude + border_size.y, cursor->longitude - border_size.x);

			// above and to left?
			if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFTTOP]) != NULL)
				// 1
				outline_clockwise (neighbor, 180, outline, border_size);
			else
				if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOP]) != NULL)
					// 2
					outline_clockwise (neighbor, 270, outline, border_size);
				else
					if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOPRIGHT]) != NULL)
						// 3
						outline_clockwise (neighbor, 270, outline, border_size);
					else
						if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHT]) != NULL)
							// 4
							outline_clockwise (neighbor, 0, outline, border_size);
						else
							if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM]) != NULL)
								// 5
								outline_clockwise (neighbor, 0, outline, border_size);
							else
								if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOM]) != NULL)
									// 6
									outline_clockwise (neighbor, 90, outline, border_size);
		}
	}
}

bool flood_grid_cell::add_to_polygon
	(map_polygon *outline,
	const int direction,
	const device_coordinate &border_size)

// Grid Cell in direction is empty or another speed
// Only adds starting point of path

{
	if (boundaries[direction].available == true) {
		boundaries[direction].available = false;
		switch (direction) {
			case FLOOD_CELL_GRID_LEFT:
				outline->add_point (latitude - border_size.y, longitude - border_size.x);
				break;
			case FLOOD_CELL_GRID_TOP:
				outline->add_point (latitude + border_size.y, longitude - border_size.x);
				break;
			case FLOOD_CELL_GRID_RIGHT:
				outline->add_point (latitude + border_size.y, longitude + border_size.x);
				break;
			case FLOOD_CELL_GRID_BOTTOM:
				outline->add_point (latitude - border_size.y, longitude + border_size.x);
		}

		return true;
	}
	else {
		// path has been added to polygon already.
		// End sweep (or error)
		return false;
	}
}

void flood_grid_matrix::corner_bottom_left
	(flood_grid_cell *cell,
	long *lon,
	long *lat)

// find midpoint to cell up and left

{
	flood_grid_cell *nearby;

	if (((nearby = get (cell->x_index - 1, cell->y_index - 1)) != NULL)
	&& (nearby->latitude != 0)) {
		*lon = cell->longitude - ((cell->longitude - nearby->longitude) / 2);
		*lat = cell->latitude - ((cell->latitude - nearby->latitude) / 2);
	}
	else {
		*lon = cell->longitude - border_size.x;
		*lat = cell->latitude - border_size.y;
	}

}

void flood_grid_matrix::corner_bottom_right
	(flood_grid_cell *cell,
	long *lon,
	long *lat)

// find midpoint to cell up and left

{
	flood_grid_cell *nearby;

	if (((nearby = get (cell->x_index + 1, cell->y_index - 1)) != NULL)
	&& (nearby->latitude != 0)) {
		*lon = cell->longitude + ((nearby->longitude - cell->longitude) / 2);
		*lat = cell->latitude - ((cell->latitude - nearby->latitude) / 2);
	}
	else {
		*lon = cell->longitude + border_size.x;
		*lat = cell->latitude - border_size.y;
	}

}

void flood_grid_matrix::corner_top_left
	(flood_grid_cell *cell,
	long *lon,
	long *lat)

// find midpoint to cell up and left

{
	flood_grid_cell *nearby;

	if (((nearby = get (cell->x_index - 1, cell->y_index + 1)) != NULL)
	&& (nearby->latitude != 0)) {
		*lon = cell->longitude - ((cell->longitude - nearby->longitude) / 2);
		*lat = cell->latitude + ((nearby->latitude - cell->latitude) / 2);
	}
	else {
		*lon = cell->longitude - border_size.x;
		*lat = cell->latitude + border_size.y;
	}

}

void flood_grid_matrix::corner_top_right
	(flood_grid_cell *cell,
	long *lon,
	long *lat)

// find midpoint to cell up and left

{
	flood_grid_cell *nearby;

	if (((nearby = get (cell->x_index + 1, cell->y_index + 1)) != NULL)
	&& (nearby->latitude != 0)) {
		*lon = cell->longitude + ((nearby->longitude - cell->longitude) / 2);
		*lat = cell->latitude + ((nearby->latitude - cell->latitude) / 2);
	}
	else {
		*lon = cell->longitude + border_size.x;
		*lat = cell->latitude + border_size.y;
	}

}

bool flood_grid_cell::add_to_polygon
	(map_polygon *outline,
	const int direction,
	class flood_grid_matrix *grid)

// Grid Cell in direction is empty or another speed
// Only adds starting point of path

// 2021-12-20 average border_size is not accurate enough.
// Use midpoint bewtween cells to set position

{
	if (boundaries[direction].available == true) {
		long x, y;

		boundaries[direction].available = false;
		switch (direction) {
			case FLOOD_CELL_GRID_LEFT: // add point at bottom, left
				grid->corner_bottom_left (this, &x, &y);
				outline->add_point (y, x);
				break;

			case FLOOD_CELL_GRID_TOP: // add point at top,left
				grid->corner_top_left (this, &x, &y);
				outline->add_point (y, x);
				break;
			case FLOOD_CELL_GRID_RIGHT: // add point at top,right
				grid->corner_top_right (this, &x, &y);
				outline->add_point (y, x);
				break;
			case FLOOD_CELL_GRID_BOTTOM:  // add point at bottom, right
				grid->corner_bottom_right (this, &x, &y);
				outline->add_point (y, x);
		}

		return true;
	}
	else {
		// path has been added to polygon already.
		// End sweep (or error)
		return false;
	}
}

void sweep_clockwise
	(flood_grid_cell *cursor,
	const int sweep_degrees,
	map_polygon *outline,
	class flood_grid_matrix *grid)

// Starts at leftmost cell of bottom row
// Looks clockwise

// No neighbors exist in sweep_direction

// Add until a neighbor is encountered, then pass off to it

{
	flood_grid_cell *neighbor;
	std::vector <flood_boundary>::iterator path;

	// sweep clockwise across empty cells, adding paths between this and each empty space
	// remove each path we use
	// If we hit a non-empty cell, call sweep_clockwise with it
	// If we hit a non-empty cell with no paths left, stop


	switch (sweep_degrees) {
	case 0:
		// Cursor has nothing above it
		ASSERT (cursor->neighbor[FLOOD_CELL_GRID_TOP] == NULL);

		// add path in top direction
		if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_TOP, grid)) {

			if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOPRIGHT]) != NULL)
				// 1
				sweep_clockwise (neighbor, 270, outline, grid);
			else
				if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHT]) != NULL)
					// 2
					sweep_clockwise (neighbor, 0, outline, grid);
				else {
					if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_RIGHT, grid)) {
						if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM]) != NULL)
							// 3
							sweep_clockwise (neighbor, 0, outline, grid);
						else
							if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOM]) != NULL)
								// 4
								sweep_clockwise (neighbor, 90, outline, grid);
							else {
								if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_BOTTOM, grid)) {
									if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT]) != NULL)
										// 5
										sweep_clockwise (neighbor, 90, outline, grid);
									else
										if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFT]) != NULL)
											// 6
											sweep_clockwise (neighbor, 180, outline, grid);
										else {
											cursor->add_to_polygon (outline, FLOOD_CELL_GRID_LEFT, grid);
											if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFTTOP]) != NULL)
												// 7
												sweep_clockwise (neighbor, 180, outline, grid);
										}
								}
							}
					}
				}
		}

		break;
	case 90:
		// Cursor has nothing to the right
		ASSERT (cursor->neighbor[FLOOD_CELL_GRID_RIGHT] == NULL);

		if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_RIGHT, grid)) {

			if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM]) != NULL)
				// 1
				sweep_clockwise (neighbor, 0, outline, grid);
			else
				if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOM]) != NULL)
					// 2
					sweep_clockwise (neighbor, 90, outline, grid);
				else {
					if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_BOTTOM, grid)) {
						if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT]) != NULL)
							// 3
							sweep_clockwise (neighbor, 90, outline, grid);
						else
							if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFT]) != NULL)
								// 4
								sweep_clockwise (neighbor, 180, outline, grid);
							else {
								if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_LEFT, grid)) {
									if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFTTOP]) != NULL)
										// 5
										sweep_clockwise (neighbor, 180, outline, grid);
									else
										if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOP]) != NULL)
											// 6
											sweep_clockwise (neighbor, 270, outline, grid);
										else {
											cursor->add_to_polygon (outline, FLOOD_CELL_GRID_TOP, grid);
											if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOPRIGHT]) != NULL)
												// 7
												sweep_clockwise (neighbor, 270, outline, grid);
										}
								}
							}
					}
				}
		}
		break;
	case 180:
		// Cursor has nothing below
		ASSERT (cursor->neighbor[FLOOD_CELL_GRID_BOTTOM] == NULL);

		if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_BOTTOM, grid)) {

			if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT]) != NULL)
				// 1
				sweep_clockwise (neighbor, 90, outline, grid);
			else
				if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFT]) != NULL)
					// 2
					sweep_clockwise (neighbor, 180, outline, grid);
				else {
					if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_LEFT, grid)) {
						if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFTTOP]) != NULL)
							// 3
							sweep_clockwise (neighbor, 180, outline, grid);
						else
							if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOP]) != NULL)
								// 4
								sweep_clockwise (neighbor, 270, outline, grid);
							else {
								if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_TOP, grid)) {
									if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOPRIGHT]) != NULL)
										// 5
										sweep_clockwise (neighbor, 270, outline, grid);
									else
										if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHT]) != NULL)
											// 6
											sweep_clockwise (neighbor, 0, outline, grid);
										else {
											cursor->add_to_polygon (outline, FLOOD_CELL_GRID_RIGHT, grid);
											if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM]) != NULL)
												// 7
												sweep_clockwise (neighbor, 0, outline, grid);
										}
								}
							}
					}
				}
		}
		break;
	case 270:
		// Cursor has nothing to the left
		ASSERT (cursor->neighbor[FLOOD_CELL_GRID_LEFT] == NULL);

		// add path on empty side
		if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_LEFT, grid)) {

			// above and to left?
			if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_LEFTTOP]) != NULL)
				// 1
				sweep_clockwise (neighbor, 180, outline, grid);
			else
				if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOP]) != NULL)
					// 2
					sweep_clockwise (neighbor, 270, outline, grid);
				else {
					if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_TOP, grid)) {

						if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_TOPRIGHT]) != NULL)
							// 3
							sweep_clockwise (neighbor, 270, outline, grid);
						else
							if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHT]) != NULL)
								// 4
								sweep_clockwise (neighbor, 0, outline, grid);
							else {
								if (cursor->add_to_polygon (outline, FLOOD_CELL_GRID_RIGHT, grid)) {
									if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM]) != NULL)
										// 5
										sweep_clockwise (neighbor, 0, outline, grid);
									else
										if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOM]) != NULL)
											// 6
											sweep_clockwise (neighbor, 90, outline, grid);
										else {
											cursor->add_to_polygon (outline, FLOOD_CELL_GRID_BOTTOM, grid);
											if ((neighbor = cursor->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT]) != NULL)
												// 7
												sweep_clockwise (neighbor, 90, outline, grid);
										}
								}
							}
					}
				}
		}
	}
}

void flood_grid_cell::add_path
	(flood_grid_cell *on_left,
	flood_grid_cell *on_right,
	const int direction)

{
	ASSERT (boundaries[direction].available == false);

	boundaries [direction].left = on_left;
	boundaries [direction].right = on_right;
	boundaries[direction].available = true;
}

/*
void flood_aggregator::build_paths
	(const int speed,
	std::vector <flood_grid_cell> *in_polygon,
	map_layer *points)

// Using neighbor/outsider arrays, build set of outside line segments for the cells.  

// Paths surrround cells in a clockwise manner

{
	std::vector <flood_grid_cell>::iterator cell;
	flood_grid_cell *grid_cell, *stranger;

	for (cell = in_polygon->begin ();
	cell != in_polygon->end ();
	++cell) {

		grid_cell = grid.get (cell->x_index, cell->y_index);
		// left
		if (((stranger = grid.get (grid_cell->x_index - 1, grid_cell->y_index)) != NULL)
		&& (stranger->wind_speed != -1)) {
			if (stranger->wind_speed != speed)
				// Need path on this side
				grid_cell->add_path (stranger, grid_cell, FLOOD_CELL_GRID_LEFT);
		}
		else
			// no stranger or stranger without pointer
			grid_cell->add_path (NULL, grid_cell, FLOOD_CELL_GRID_LEFT);

		// top
		if (((stranger = grid.get (grid_cell->x_index, grid_cell->y_index + 1)) != NULL)
		&& (stranger->wind_speed != -1)) {
			if (stranger->wind_speed != speed)
				grid_cell->add_path (stranger, grid_cell, FLOOD_CELL_GRID_TOP);
		}
		else
			grid_cell->add_path (NULL, grid_cell, FLOOD_CELL_GRID_TOP);

		// right
		if (((stranger = grid.get (grid_cell->x_index + 1, grid_cell->y_index)) != NULL)
		&& (stranger->wind_speed != -1)) {
			if (stranger->wind_speed != speed)
				grid_cell->add_path (stranger, grid_cell, FLOOD_CELL_GRID_RIGHT);
		}
		else
			grid_cell->add_path (NULL, grid_cell, FLOOD_CELL_GRID_RIGHT);

		// bottom
		if (((stranger = grid.get (grid_cell->x_index, grid_cell->y_index - 1)) != NULL)
		&& (stranger->wind_speed != -1)) {
			if (stranger->wind_speed != speed)
				grid_cell->add_path (stranger, grid_cell, FLOOD_CELL_GRID_BOTTOM);
		}
		else
			grid_cell->add_path (NULL, grid_cell, FLOOD_CELL_GRID_BOTTOM);
	}
}
*/

void flood_aggregator::build_paths
(const int speed_ge,
const int speed_lt ,
	std::vector <flood_grid_cell> *in_polygon,
	map_layer *points)

// Using neighbor/outsider arrays, build set of outside line segments for the cells.  

// Paths surrround cells in a clockwise manner

{
	std::vector <flood_grid_cell>::iterator cell;
	flood_grid_cell *grid_cell, *stranger;

	for (cell = in_polygon->begin ();
	cell != in_polygon->end ();
	++cell) {

		grid_cell = grid.get (cell->x_index, cell->y_index);
		// left
		if (((stranger = grid.get (grid_cell->x_index - 1, grid_cell->y_index)) != NULL)
		&& (stranger->wind_speed != -1)) {
			if ((stranger->wind_speed < speed_ge)
			|| (stranger->wind_speed >= speed_lt))
				// Need path on this side
				grid_cell->add_path (stranger, grid_cell, FLOOD_CELL_GRID_LEFT);
		}
		else
			// no stranger or stranger without pointer
			grid_cell->add_path (NULL, grid_cell, FLOOD_CELL_GRID_LEFT);

		// top
		if (((stranger = grid.get (grid_cell->x_index, grid_cell->y_index + 1)) != NULL)
		&& (stranger->wind_speed != -1)) {
			if ((stranger->wind_speed < speed_ge)
			|| (stranger->wind_speed >= speed_lt))
				grid_cell->add_path (stranger, grid_cell, FLOOD_CELL_GRID_TOP);
		}
		else
			grid_cell->add_path (NULL, grid_cell, FLOOD_CELL_GRID_TOP);

		// right
		if (((stranger = grid.get (grid_cell->x_index + 1, grid_cell->y_index)) != NULL)
		&& (stranger->wind_speed != -1)) {
			if ((stranger->wind_speed < speed_ge)
			|| (stranger->wind_speed >= speed_lt))
				grid_cell->add_path (stranger, grid_cell, FLOOD_CELL_GRID_RIGHT);
		}
		else
			grid_cell->add_path (NULL, grid_cell, FLOOD_CELL_GRID_RIGHT);

		// bottom
		if (((stranger = grid.get (grid_cell->x_index, grid_cell->y_index - 1)) != NULL)
		&& (stranger->wind_speed != -1)) {
			if ((stranger->wind_speed < speed_ge)
			|| (stranger->wind_speed >= speed_lt))
				grid_cell->add_path (stranger, grid_cell, FLOOD_CELL_GRID_BOTTOM);
		}
		else
			grid_cell->add_path (NULL, grid_cell, FLOOD_CELL_GRID_BOTTOM);
	}

}

/*
void flood_aggregator::make_polygon
	(const long id,
	const int speed,
	std::vector <flood_grid_cell> *in_polygon,
	map_layer *points,
	map_layer *polygons)

// Create polygon enclosing all cells in_polygon

{
	flood_grid_cell *boundary_cell, *grid_cell;
	long min_x, max_x, min_y, max_y, average_space_x, average_space_y;
	int cell_min_x, cell_max_x, cell_min_y, cell_max_y;
	map_polygon *outline;
	device_coordinate start_point;

	find_ranges (in_polygon, &min_x, &min_y, &max_x, &max_y, &cell_min_x, &cell_min_y, &cell_max_x, &cell_max_y);

	outline = (map_polygon *) polygons->create_new (MAP_OBJECT_POLYGON);
	outline->id = id;
	outline->attributes_numeric[0] = (double) speed;
	outline->attributes_numeric[1] = (double) in_polygon->size ();

	// average interval between cells
	average_space_x = (long) overall_average_x;
	grid.border_size.x = average_space_x / 2;
	average_space_y = (long) overall_average_y;
	grid.border_size.y = average_space_y / 2;

	// Create sets of neighbors and outsiders
	{
		std::vector <flood_grid_cell>::iterator cell;
		flood_grid_cell *stranger;

		for (cell = in_polygon->begin ();
		cell != in_polygon->end ();
		++cell) {
			grid_cell = grid.get (cell->x_index, cell->y_index);
			// left
			if (((stranger = grid.get (grid_cell->x_index - 1, grid_cell->y_index)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if (stranger->wind_speed == (double) speed)
					grid_cell->neighbor[FLOOD_CELL_GRID_LEFT] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_LEFT] = stranger;
			}

			// left-top
			if (((stranger = grid.get (grid_cell->x_index - 1, grid_cell->y_index + 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if (stranger->wind_speed == (double) speed)
					grid_cell->neighbor[FLOOD_CELL_GRID_LEFTTOP] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_LEFTTOP] = stranger;
			}

			// top
			if (((stranger = grid.get (grid_cell->x_index, grid_cell->y_index + 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if (stranger->wind_speed == (double) speed)
					grid_cell->neighbor[FLOOD_CELL_GRID_TOP] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_TOP] = stranger;
			}

			// top-right
			if (((stranger = grid.get (grid_cell->x_index + 1, grid_cell->y_index + 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if (stranger->wind_speed == (double) speed)
					grid_cell->neighbor[FLOOD_CELL_GRID_TOPRIGHT] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_TOPRIGHT] = stranger;
			}

			// right
			if (((stranger = grid.get (grid_cell->x_index + 1, grid_cell->y_index)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if (stranger->wind_speed == (double) speed)
					grid_cell->neighbor[FLOOD_CELL_GRID_RIGHT] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_RIGHT] = stranger;
			}

			// right-bottom
			if (((stranger = grid.get (grid_cell->x_index + 1, grid_cell->y_index - 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if (stranger->wind_speed == (double) speed)
					grid_cell->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_RIGHTBOTTOM] = stranger;
			}

			// bottom
			if (((stranger = grid.get (grid_cell->x_index, grid_cell->y_index - 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if (stranger->wind_speed == (double) speed)
					grid_cell->neighbor[FLOOD_CELL_GRID_BOTTOM] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_BOTTOM] = stranger;
			}

			// bottom-left
			if (((stranger = grid.get (grid_cell->x_index - 1, grid_cell->y_index - 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if (stranger->wind_speed == (double) speed)
					grid_cell->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_BOTTOMLEFT] = stranger;
			}
		}
	}

	// Using neighbor/outsider arrays, build set of outside line segments for the cells.  
	build_paths (speed, in_polygon, points);

	// Attach paths to new polygon by traversing the associated cells clockwise

	boundary_cell = leftmost_cell_on_row (cell_min_y, in_polygon);

	// Find first outsider of any in_polygon cell
	grid_cell = grid.get (boundary_cell->x_index, boundary_cell->y_index);
	grid.clear_use ();

	// outline_clockwise fails because of single-width spurs
	// outline_clockwise (grid_cell, 270, outline, border_size);

	sweep_clockwise (grid_cell, 270, outline, &grid);

	// repeat start point
	start_point = outline->point_at_index (0);
	outline->add_point (start_point.y, start_point.x);

	outline->set_extent ();

	polygons->objects.push_back (outline);

	polygons_by_speed[speed].push_back (outline);
}
*/

void flood_aggregator::make_polygon
	(const long id,
	const int speed_ge,
	const int speed_lt,
	const int speed_group_index,
	std::vector <flood_grid_cell> *in_polygon,
	map_layer *points,
	map_layer *polygons)

// Create polygon enclosing all cells in_polygon

{
	flood_grid_cell *boundary_cell, *grid_cell;
	long min_x, max_x, min_y, max_y, average_space_x, average_space_y;
	int cell_min_x, cell_max_x, cell_min_y, cell_max_y;
	map_polygon *outline;
	device_coordinate start_point;

	find_ranges (in_polygon, &min_x, &min_y, &max_x, &max_y, &cell_min_x, &cell_min_y, &cell_max_x, &cell_max_y);

	outline = (map_polygon *) polygons->create_new (MAP_OBJECT_POLYGON);
	outline->id = id;
	outline->attributes_numeric[0] = (double) speed_ge;
	outline->attributes_numeric[1] = (double) speed_lt;
	outline->attributes_numeric[2] = (double) in_polygon->size ();

	// average interval between cells
	average_space_x = (long) overall_average_x;
	grid.border_size.x = average_space_x / 2;
	average_space_y = (long) overall_average_y;
	grid.border_size.y = average_space_y / 2;

	// Create sets of neighbors and outsiders
	{
		std::vector <flood_grid_cell>::iterator cell;
		flood_grid_cell *stranger;

		for (cell = in_polygon->begin ();
		cell != in_polygon->end ();
		++cell) {
			grid_cell = grid.get (cell->x_index, cell->y_index);
			// left
			if (((stranger = grid.get (grid_cell->x_index - 1, grid_cell->y_index)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if ((stranger->wind_speed >= (double) speed_ge)
				&& (stranger->wind_speed < (double) speed_lt))
					grid_cell->neighbor[FLOOD_CELL_GRID_LEFT] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_LEFT] = stranger;
			}

			// left-top
			if (((stranger = grid.get (grid_cell->x_index - 1, grid_cell->y_index + 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if ((stranger->wind_speed >= (double) speed_ge)
				&& (stranger->wind_speed < (double) speed_lt))
					grid_cell->neighbor[FLOOD_CELL_GRID_LEFTTOP] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_LEFTTOP] = stranger;
			}

			// top
			if (((stranger = grid.get (grid_cell->x_index, grid_cell->y_index + 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if ((stranger->wind_speed >= (double) speed_ge)
				&& (stranger->wind_speed < (double) speed_lt))
					grid_cell->neighbor[FLOOD_CELL_GRID_TOP] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_TOP] = stranger;
			}

			// top-right
			if (((stranger = grid.get (grid_cell->x_index + 1, grid_cell->y_index + 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if ((stranger->wind_speed >= (double) speed_ge)
				&& (stranger->wind_speed < (double) speed_lt))
					grid_cell->neighbor[FLOOD_CELL_GRID_TOPRIGHT] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_TOPRIGHT] = stranger;
			}

			// right
			if (((stranger = grid.get (grid_cell->x_index + 1, grid_cell->y_index)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if ((stranger->wind_speed >= (double) speed_ge)
				&& (stranger->wind_speed < (double) speed_lt))
					grid_cell->neighbor[FLOOD_CELL_GRID_RIGHT] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_RIGHT] = stranger;
			}

			// right-bottom
			if (((stranger = grid.get (grid_cell->x_index + 1, grid_cell->y_index - 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if ((stranger->wind_speed >= (double) speed_ge)
				&& (stranger->wind_speed < (double) speed_lt))
					grid_cell->neighbor[FLOOD_CELL_GRID_RIGHTBOTTOM] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_RIGHTBOTTOM] = stranger;
			}

			// bottom
			if (((stranger = grid.get (grid_cell->x_index, grid_cell->y_index - 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if ((stranger->wind_speed >= (double) speed_ge)
				&& (stranger->wind_speed < (double) speed_lt))
					grid_cell->neighbor[FLOOD_CELL_GRID_BOTTOM] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_BOTTOM] = stranger;
			}

			// bottom-left
			if (((stranger = grid.get (grid_cell->x_index - 1, grid_cell->y_index - 1)) != NULL)
			&& (stranger->wind_speed != -1)) {
				if ((stranger->wind_speed >= (double) speed_ge)
				&& (stranger->wind_speed < (double) speed_lt))
					grid_cell->neighbor[FLOOD_CELL_GRID_BOTTOMLEFT] = stranger;
				else
					grid_cell->outsider[FLOOD_CELL_GRID_BOTTOMLEFT] = stranger;
			}
		}
	}

	// Using neighbor/outsider arrays, build set of outside line segments for the cells.  
	build_paths (speed_ge, speed_lt, in_polygon, points);

	// Attach paths to new polygon by traversing the associated cells clockwise

	boundary_cell = leftmost_cell_on_row (cell_min_y, in_polygon);

	// Find first outsider of any in_polygon cell
	grid_cell = grid.get (boundary_cell->x_index, boundary_cell->y_index);
	grid.clear_use ();

	// outline_clockwise fails because of single-width spurs
	// outline_clockwise (grid_cell, 270, outline, border_size);

	sweep_clockwise (grid_cell, 270, outline, &grid);

	// repeat start point
	start_point = outline->point_at_index (0);
	outline->add_point (start_point.y, start_point.x);

	outline->set_extent ();

	polygons->objects.push_back (outline);

	polygons_by_speed[speed_group_index].push_back (outline);
}

/*
void flood_aggregator::outline_speed
	(long *next_id,
	const int match_speed,
	map_layer *points,
	map_layer *polygons,
	interface_window *view,
	dynamic_string &log)

// Create polygons for all cells with match_speed

{
	std::vector <flood_grid_cell> in_polygon;
	std::vector <std::vector <flood_grid_cell>> speed_polygons;
	std::vector <std::vector <flood_grid_cell>>::iterator shape;
	int x, y;

	view->update_progress_formatted (0, "Wind Speed=%d", match_speed);
	view->update_progress ("Finding cells", 1);

	grid.clear_neighbors ();

	for (y = 0; y < grid.count_rows; ++y)
		for (x = 0; x < grid.count_columns; ++x) {
			in_polygon.clear ();
			flood_fill (x, y, match_speed, match_speed, points->data_attribute_index, &grid, &in_polygon);
			if (in_polygon.size () > 0)
				// make_polygon calls flood_grid_matrix.clear_use, so either a copy of grid must be made, or call make_polygon after all flood_fill calls
				speed_polygons.push_back (in_polygon);
		}


	view->update_progress ("Making polygons", 1);

	for (shape = speed_polygons.begin ();
	shape != speed_polygons.end ();
	++shape) {
		make_polygon (*next_id, match_speed, &*shape, points, polygons);
		*next_id += 1;
	}
}
*/

void flood_aggregator::outline_speed
	(long *next_id,
	const int speed_ge,
	const int speed_lt,
	const int speed_group_index,
	map_layer *points,
	map_layer *polygons,
	interface_window *view,
	dynamic_string &log)

// Create polygons for all cells with match_speed

{
	std::vector <flood_grid_cell> in_polygon;
	std::vector <std::vector <flood_grid_cell>> speed_polygons;
	std::vector <std::vector <flood_grid_cell>>::iterator shape;
	int x, y;

	view->update_progress_formatted (0, "%d <= Wind Speed < %d", speed_ge, speed_lt);
	view->update_progress ("Finding cells", 1);

	grid.clear_neighbors ();

	for (y = 0; y < grid.count_rows; ++y)
		for (x = 0; x < grid.count_columns; ++x) {
			in_polygon.clear ();
			flood_fill (x, y, speed_ge, speed_lt, points->data_attribute_index, &grid, &in_polygon);
			if (in_polygon.size () > 0)
				// make_polygon calls flood_grid_matrix.clear_use, so either a copy of grid must be made, or call make_polygon after all flood_fill calls
				speed_polygons.push_back (in_polygon);
		}


	view->update_progress ("Making polygons", 1);

	for (shape = speed_polygons.begin ();
	shape != speed_polygons.end ();
	++shape) {
		make_polygon (*next_id, speed_ge, speed_lt, speed_group_index, &*shape, points, polygons);
		*next_id += 1;
	}

}

void flood_aggregator::find_average_sizes
	(dynamic_string &log)

{
	// std::vector <flood_grid_cell> vector_points;
	// std::vector <flood_grid_cell>::iterator vector_point;
	flood_grid_cell *cell, *last_cell;
	int count_gaps, x, y;

	overall_average_x = overall_average_y = 0;
	count_gaps = 0;

	for (y = 0; y < grid.count_rows; ++y) {
		last_cell = NULL;
		for (x = 0; x < grid.count_columns; ++x) {
			cell = grid.get (x, y);

			if (last_cell
			&& (last_cell->wind_speed != -1)
			&& (cell->wind_speed != -1)) {
				ASSERT (cell->x_index == last_cell->x_index + 1);
				ASSERT (cell->y_index == last_cell->y_index);
				overall_average_x += double (cell->longitude - last_cell->longitude);
				++count_gaps;
			}
			last_cell = cell;
		}
	}

	overall_average_x /= (double) count_gaps;

	count_gaps = 0;
	for (x = 0; x <= grid.count_columns; ++x) {
		last_cell = NULL;
		for (y = 0; y < grid.count_rows; ++y) {
			cell = grid.get (x, y);

			if (last_cell
			&& (last_cell->wind_speed != -1)
			&& (cell->wind_speed != -1)) {
				ASSERT (cell->x_index == last_cell->x_index);
				ASSERT (cell->y_index == last_cell->y_index + 1);
				overall_average_y += double (cell->latitude - last_cell->latitude);
				++count_gaps;
				// log.add_formatted ("%12.2lf\t%d\t%ld\t%ld\n", overall_average_y, count_gaps, vector_point->second, last_latitude);
			}
			last_cell = cell;
		}

	}
	overall_average_y /= (double) count_gaps;
	log.add_formatted ("Average Width\t%.1lf\n", overall_average_x);
	log.add_formatted ("Average Height\t%.1lf\n", overall_average_y);
}

void flood_aggregator::build_grid
	(map_layer *points,
	map_layer *grid_points,
	interface_window *view,
	dynamic_string &log)

// Traverse entire grid to set overall_average_x, overall_average_y

{
	std::vector <map_object *>::const_iterator point;
	std::vector <flood_grid_cell> in_polygon;
	long min_id, max_id, min_x, min_y, max_x, max_y;
	int point_count, cell_min_x, cell_min_y, cell_max_x, cell_max_y;
	bool *used;

	// Use the middle third of points for overall_averages
	min_id = MAXLONG;
	max_id = -MAXLONG;

	for (point = points->objects.begin (), point_count= 0;
	point != points->objects.end ();
	++point, ++point_count) {
		if ((*point)->id < min_id)
			min_id = (*point)->id;
		if ((*point)->id > max_id)
			max_id = (*point)->id;
	}

	used = new bool[max_id];
	memset (used, 0, sizeof (bool) * max_id);
	point = points->objects.begin ();

	view->update_progress ("seek_adjacent", 1);
	seek_adjacent (flood_grid_cell (*point, 0, 0), &points->objects, &in_polygon, used);

	// Isle Royal National Park 
	map_object *island_point = points->match_id (432190);
	seek_adjacent (flood_grid_cell (island_point, 563, 628), &points->objects, &in_polygon, used);

	delete[] used;

	view->update_progress ("find_ranges", 1);
	find_ranges (&in_polygon, &min_x, &min_y, &max_x, &max_y, &cell_min_x, &cell_min_y, &cell_max_x, &cell_max_y);

	// -1..37 becomes 0..38
	offset (&in_polygon, cell_min_x, cell_min_y, &cell_max_x, &cell_max_y);

	grid.set_size (cell_max_x + 1, cell_max_y + 1);

	// Set pointers to map_objects in grid
	grid.set_from_vector (&in_polygon);

	// fill map_layer grid_points
	grid.fill_map_layer (grid_points, points->data_attribute_index);
}

void flood_aggregator::divide_by_speed
	(map_layer *points,
	map_layer *grid_points,
	interface_window *view,
	dynamic_string &log)

{
	std::vector <map_object *>::const_iterator point;
	std::map <long, int> latitudes, longitudes;
	std::map <long, int>::iterator scaler;
	int speed;

	// divide points by windspeed value (attribute_numeric [54])

	std::set <int> above;

	for (speed = 0; speed < 10; ++speed) {
		lowest_id[speed] = MAXLONG;
		highest_id[speed] = -MAXLONG;
	}

	for (point = points->objects.begin ();
	point != points->objects.end ();
	++point) {
		speed = (int) (*point)->attributes_numeric[points->data_attribute_index];

		ASSERT ((*point)->attributes_numeric[points->data_attribute_index] - (double) speed < 0.01);

		if (speed < 10) {
			divided_by_speed[speed].push_back (*point);
			if ((*point)->id < lowest_id[speed])
				lowest_id[speed] = (*point)->id;
			if ((*point)->id > highest_id[speed])
				highest_id[speed] = (*point)->id;
		}
		else
			above.insert (speed);
	}

	for (speed = 0; speed < 10; ++speed) {
		log.add_formatted ("Speed %d\tCount %d\n", speed, (int) divided_by_speed[speed].size ());
	}

}

void flood_aggregator::build_grid_from_layer
	(map_layer *grid_points,
	interface_window *view,
	dynamic_string &log)

// Traverse entire grid to set overall_average_x, overall_average_y

{
	std::vector <map_object *>::const_iterator point;
	long min_id, max_id, min_x, min_y, max_x, max_y;
	int point_count, cell_min_x, cell_min_y, cell_max_x, cell_max_y;

	// Use the middle third of points for overall_averages
	min_id = MAXLONG;
	max_id = -MAXLONG;

	for (point = grid_points->objects.begin (), point_count= 0;
	point != grid_points->objects.end ();
	++point, ++point_count) {
		if ((*point)->id < min_id)
			min_id = (*point)->id;
		if ((*point)->id > max_id)
			max_id = (*point)->id;
	}

	view->update_progress ("find_ranges", 1);

	min_x = min_y = MAXLONG;
	max_x = max_y = -MAXLONG;
	cell_min_x = cell_min_y = MAXINT;
	cell_max_x = cell_max_y = -MAXINT;
	for (point = grid_points->objects.begin (), point_count = 0;
	point != grid_points->objects.end ();
	++point, ++point_count) {
		if ((*point)->latitude < min_y)
			min_y = (*point)->latitude;
		if ((*point)->latitude > max_y)
			max_y = (*point)->latitude;

		if ((*point)->longitude < min_x)
			min_x = (*point)->longitude;
		if ((*point)->longitude > max_x)
			max_x = (*point)->longitude;

		// [0] row_index
		if ((*point)->attributes_numeric [0] < cell_min_y)
			cell_min_y = (*point)->attributes_numeric [0];
		if ((*point)->attributes_numeric [0] > cell_max_y)
			cell_max_y = (*point)->attributes_numeric [0];

		// [1] column_index
		if ((*point)->attributes_numeric [1] < cell_min_x)
			cell_min_x = (*point)->attributes_numeric [1];
		if ((*point)->attributes_numeric [1] > cell_max_x)
			cell_max_x = (*point)->attributes_numeric [1];
	}

	grid.set_size (cell_max_x + 1, cell_max_y + 1);

	// Set pointers to map_objects in grid
	grid.set_from_map_layer (grid_points);

	find_average_sizes (log);
}

bool check_surround
	(map_polygon *outside,
	map_polygon *inside,
	dynamic_string *log)

// True if every point of inside is surrounded by outside

{
	int point_index, polygon_index;
	bool surrounded = true;
	device_coordinate node;

	if (inside->type == MAP_OBJECT_POLYGON_COMPLEX) {
		for (polygon_index = 0;
		surrounded
		&& (polygon_index < ((map_polygon_complex *) inside)->polygon_count);
		++polygon_index) {
			for (point_index = 0;
			surrounded
			&& (point_index < ((map_polygon_complex *) inside)->polygons [polygon_index].node_count);
			++point_index) {
				node.x = ((map_polygon_complex *) inside)->polygons [polygon_index].nodes[point_index * 2];
				node.y = ((map_polygon_complex *) inside)->polygons [polygon_index].nodes[point_index * 2 + 1];
				if (!outside->point_inside (&node, log))
					surrounded = false;
			}
		}
	}
	else {
		for (point_index = 0;
		surrounded 
		&& (point_index < inside->node_count);
		++point_index) {
			node.x = inside->nodes[point_index * 2];
			node.y = inside->nodes[point_index * 2 + 1];
			if (!outside->point_inside (&node, log))
				surrounded = false;
		}
	}

	return surrounded;
}

void flood_aggregator::add_enclosure
	(const long outside,
	const long inside,
	std::map <long, std::set <long>> *enclosed)

{
	std::map <long, std::set <long>>::iterator enclosing;

	if ((enclosing = enclosed->find (outside)) != enclosed->end ())
		enclosing->second.insert (inside);
	else {
		std::pair <long, std::set <long>> new_entry;
		new_entry.first = outside;
		new_entry.second.insert (inside);
		enclosed->insert (new_entry);
	}
}

void flood_aggregator::find_enclosed_polygons
	(map_layer *layer_polygons,
	std::map <long, std::set <long>> *enclosed,
	interface_window *view,
	dynamic_string &log)

// compare every polygon from speed 1 with each from 2.
// If overlap, see if completely surrounded using clockwise sweep
// if surrounded, add as a sub-polygon

{
	int speed_index, other_speed_index;
	std::vector <map_object *>::iterator polygon, other_polygon;
	bounding_cube polygon_area;
	int count_surrounded = 0;
	dynamic_string *dump_log = NULL;

	// 814 inside of 231
	for (speed_index = 0;
	// (count_surrounded < 12)
	speed_index < 7;
	++speed_index) {
		for (polygon = polygons_by_speed[speed_index].begin ();
		// (count_surrounded < 12)
		polygon != polygons_by_speed[speed_index].end ();
		++polygon) {
			(*polygon)->check_extent (&polygon_area);
			for (other_speed_index = 0;
			// (count_surrounded < 12)
			other_speed_index < 7;
			++other_speed_index) {
				if (other_speed_index != speed_index) {
					for (other_polygon = polygons_by_speed[other_speed_index].begin ();
					// (count_surrounded < 12)
					other_polygon != polygons_by_speed[other_speed_index].end ();
					++other_polygon) {
						if ((*other_polygon)->overlaps_logical (&polygon_area)) {
							/*
							if (((*polygon)->id == 231)
							&& ((*other_polygon)->id == 814)) {
								log += "Overlap True\n";
								dump_log = &log;
							}
							else
								dump_log = NULL;
							*/
							// sweep around polygon to see if other_polygon surrounds it
							// Since points are on every corner, whichever polygon has more points is bigger
							if ((*polygon)->total_number_of_points () > (*other_polygon)->total_number_of_points ()) {
								if (check_surround ((map_polygon *) *polygon, (map_polygon *) *other_polygon, dump_log)) {
									// log.add_formatted ("%ld inside %ld\n", (*other_polygon)->id, (*polygon)->id);
									add_enclosure ((*polygon)->id, (*other_polygon)->id, enclosed);
									++count_surrounded;
								}
							}
							else {
								if (check_surround ((map_polygon *) *other_polygon, (map_polygon *) *polygon, dump_log)) {
									// log.add_formatted ("%ld inside %ld\n", (*other_polygon)->id, (*polygon)->id);
									add_enclosure ((*other_polygon)->id, (*polygon)->id, enclosed);
									++count_surrounded;
								}
							}
							if (dump_log)
								log += *dump_log;
						}
					}
				}
			}
		}
	}
	log.add_formatted ("Count inside %d.\n", count_surrounded);
}

void find_contained
	(const long outermost_id,
	std::set <long> *contained,
	std::map <long, std::set <long>> *enclosed)

// Recursively add all polygons contained in outermost

{
	std::set <long>::iterator within;
	std::map <long, std::set <long>>::iterator outermost, outer;

	if ((outermost = enclosed->find (outermost_id)) != enclosed->end ()) {
		for (within = outermost->second.begin (); within != outermost->second.end (); ++within) {
			contained->insert (*within);
			find_contained (*within, contained, enclosed);
		}
	}
}

void flood_aggregator::separate_layer
	(map_layer *layer_polygons,
	map_layer **layer_high,
	map_layer **layer_low,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

// Split layer_polygons by wind speed range (attributes_numeric [0] and [1]) into high and low layers

{
	std::vector <map_object *>::iterator object;
	map_object *new_polygon;

	*layer_high = map->create_new (layer_polygons->type);
	(*layer_high)->copy_attributes_only (layer_polygons);
	(*layer_high)->name = "wind_speed_3_5";
	(*layer_high)->draw_as =  MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	(*layer_high)->color =  RGB (127, 127, 127);

	*layer_low = map->create_new (layer_polygons->type);
	(*layer_low)->copy_attributes_only (layer_polygons);
	(*layer_low)->name += " wind_speed_0_2";
	(*layer_high)->draw_as =  MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	(*layer_high)->color = RGB (0, 255, 0);

	for (object = layer_polygons->objects.begin ();
	object != layer_polygons->objects.end ();
	++object) {
		// objects might be MAP_POLYGON or MAP_POLYGON_COMPLEX
		if ((*object)->attributes_numeric[0] == 0.0) {
			new_polygon = (*layer_low)->create_new ((*object)->type);
			new_polygon->copy (*object, *layer_low);
			(*layer_low)->objects.push_back (new_polygon);
		}
		else {
			new_polygon = (*layer_high)->create_new ((*object)->type);
			new_polygon->copy (*object, *layer_high);
			(*layer_high)->objects.push_back (new_polygon);
		}
	}

	map->layers.push_back (*layer_low);
	map->layers.push_back (*layer_high);
}


void flood_aggregator::set_interior_polygons
	(map_layer *layer_polygons,
	interface_window *view,
	dynamic_string &log)

// find_enclosed_polygons builds a set of all enclosed for each polygon
// For each polygon, for each within, recursivly find all enclosed.
// Create map of doubly or triply or more enclosed, remove them from polygon's within set

{
	std::map <long, std::set <long>> enclosed;
	std::set <long> redundant_ids;
	std::map <long, std::set <long>>::iterator outermost, removal_target;
	std::set <long>::iterator level_2, within, redundant;

	std::set <long> contained;

	// Build map of all container/contained pairs
	find_enclosed_polygons (layer_polygons, &enclosed, view, log);

	// Find & remove those that are polygons-within-polygons-within-polygons
	for (outermost = enclosed.begin (); outermost != enclosed.end (); ++outermost) {
		redundant_ids.clear ();

		for (level_2 = outermost->second.begin (); level_2 != outermost->second.end (); ++level_2) {
			contained.clear ();

			// find all polygons contained by level_2
			find_contained (*level_2, &contained, &enclosed);

			// Any that are contained by outermost and also by level_2 must be removed from outermost's contained set
			for (within = contained.begin (); within != contained.end (); ++within) {
				if ((redundant = outermost->second.find (*within)) != outermost->second.end ()) {
					// redundent is inside level_2 and also in outermost's set
					redundant_ids.insert (*redundant);
				}
			}
		}

		// Remove redundant from outermost's set
		for (redundant = redundant_ids.begin (); redundant != redundant_ids.end (); ++redundant)
			outermost->second.erase (*redundant);
	}

	// Build complex polygons with remaining enclosed polygons
	std::map <long, map_object *> complexities;
	map_object *poly;
	map_polygon_complex *new_polygon;

	for (outermost = enclosed.begin (); outermost != enclosed.end (); ++outermost) {
		poly = layer_polygons->match_id (outermost->first);
		new_polygon = new map_polygon_complex;
		new_polygon->id = outermost->first;
		new_polygon->copy_attributes_only (poly, layer_polygons);
		new_polygon->add_polygon ((map_polygon *) poly, layer_polygons);
		for (level_2 = outermost->second.begin (); level_2 != outermost->second.end (); ++level_2) {
			poly = layer_polygons->match_id (*level_2);
			new_polygon->add_polygon ((map_polygon *) poly, layer_polygons);
		}

		complexities.insert (std::pair <long, map_object *> (new_polygon->id, new_polygon));
	}

	// Replace polygons with complex_polygons
	std::vector <map_object *> keepers;
	map_layer erasables;
	std::vector <map_object *>::iterator object;
	std::map <long, map_object *>::iterator replacement;

	for (object = layer_polygons->objects.begin ();
	object != layer_polygons->objects.end ();
	++object) {
		if ((replacement = complexities.find ((*object)->id)) != complexities.end ()) {
			keepers.push_back (replacement->second);
			erasables.objects.push_back (*object);
		}
		else
			keepers.push_back (*object);
	}

	layer_polygons->objects = keepers;
	layer_polygons->set_extent (); // 2022-02-25 Set min_x, min_y, max_x, max_y for new complex polygons
	erasables.clear_objects ();
}
