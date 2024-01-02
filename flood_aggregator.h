#pragma once

const int FLOOD_CELL_GRID_LEFT = 0;
const int FLOOD_CELL_GRID_TOP = 1;
const int FLOOD_CELL_GRID_RIGHT = 2;
const int FLOOD_CELL_GRID_BOTTOM = 3;
const int FLOOD_CELL_GRID_LEFTTOP = 4;
const int FLOOD_CELL_GRID_TOPRIGHT = 5;
const int FLOOD_CELL_GRID_RIGHTBOTTOM = 6;
const int FLOOD_CELL_GRID_BOTTOMLEFT = 7;

class flood_boundary {
	public:
		class flood_grid_cell *left, *right;
		bool available;

		flood_boundary ();
		flood_boundary (const flood_boundary &other);
		flood_boundary operator =
			(const flood_boundary &);
		void copy
			(const flood_boundary &);
};

class flood_grid_cell {
	public:
		class map_object *source_point;
		int x_index, y_index;
		int wind_speed;
		long latitude, longitude;
		bool used;

		// segments
		flood_grid_cell *neighbor [8]; // left, left-top, top, top-right, ...
		flood_grid_cell *outsider [8]; // left, left-top, top, top-right, ...

		flood_boundary boundaries [4];

		flood_grid_cell ();
		flood_grid_cell
			(class map_object *,
			const int new_x,
			const int new_y);
		flood_grid_cell (const flood_grid_cell &other);
		virtual ~flood_grid_cell ();
		flood_grid_cell operator =
			(const flood_grid_cell &);
		void copy
			(const flood_grid_cell &);
		void clear();

		void add_path
			(class flood_grid_cell *on_left,
			class flood_grid_cell *on_right,
			const int direction);
		bool add_to_polygon
			(class map_polygon *outline,
			const int direction,
			const device_coordinate &border_size);
		bool add_to_polygon
			(class map_polygon *outline,
			const int direction,
			class flood_grid_matrix *grid);
};

class flood_grid_matrix {
	public:
		int count_rows, count_columns;
		device_coordinate border_size;

		flood_grid_matrix ();
		virtual ~flood_grid_matrix ();
		void clear_use ();
		void clear_neighbors ();

		void corner_bottom_left
			(class flood_grid_cell *cell,
			long *lon,
			long *lat);
		void corner_bottom_right
			(class flood_grid_cell *cell,
			long *lon,
			long *lat);
		void corner_top_left
			(class flood_grid_cell *cell,
			long *lon,
			long *lat);
		void corner_top_right
			(class flood_grid_cell *cell,
			long *lon,
			long *lat);

		void fill_map_layer
			(class map_layer *,
			const int wind_speed_attribute_index);
		flood_grid_cell *get
			(const int x_index,
			const int y_index);

		void set_from_vector
			(std::vector <flood_grid_cell> *);
		void set_from_map_layer
			(class map_layer *grid_points);

		void set_size
			(const int max_x,
			const int max_y);

		void set_vector_x
			(const int y,
			std::vector <flood_grid_cell> *);
		void set_vector_y
			(const int y,
			std::vector <flood_grid_cell> *);

	protected:
		class flood_grid_cell *cells;
		
};

class flood_aggregator {
	public:
		std::vector <map_object *> divided_by_speed[10];
		std::vector <map_object *> polygons_by_speed[10];
		long lowest_id [10], highest_id[10];

		double overall_average_x, overall_average_y;

		flood_grid_matrix grid;

		void build_grid
			(class map_layer *points,
			class map_layer *grid_points,
			class interface_window *view,
			dynamic_string &log);
		void build_grid_from_layer
			(class map_layer *grid_points,
			class interface_window *view,
			dynamic_string &log);

		void divide_by_speed
			(class map_layer *points,
			class map_layer *grid_points,
			class interface_window *view,
			dynamic_string &log);

		void find_average_sizes
			(dynamic_string &log);

		void outline_speed
			(long *next_id,
			const int speed_ge,
			const int speed_lt,
			const int speed_group_index,
			class map_layer *points,
			class map_layer *polygons,
			class interface_window *view,
			dynamic_string &log);

		void separate_layer
			(class map_layer *layer_polygons,
			class map_layer **layer_high,
			class map_layer **layer_low,
			class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);
		void set_interior_polygons
			(class map_layer *layer_polygons,
			class interface_window *view,
			dynamic_string &log);

	protected:

		void add_enclosure
			(const long outside,
			const long inside,
			std::map <long, std::set <long>> *enclosed);
		void build_paths
			(const int speed_ge,
			const int speed_lt,
			std::vector <flood_grid_cell> *in_polygon,
			class map_layer *points);
		void find_enclosed_polygons
			(class map_layer *layer_polygons,
			std::map <long, std::set <long>> *enclosed,
			class interface_window *view,
			dynamic_string &log);

		void make_polygon
			(const long id,
			const int speed_ge,
			const int speed_lt,
			const int speed_group_index,
			std::vector <flood_grid_cell> *in_polygon,
			class map_layer *points,
			class map_layer *polygons);

};

