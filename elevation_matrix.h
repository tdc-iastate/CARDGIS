
const int ELEVATION_MATRIX_WIDTH = 100;
const int ELEVATION_MATRIX_HEIGHT = 100;

class elevation_matrix_cell {
	public:
		std::vector <logical_coordinate> points;

		elevation_matrix_cell ();
		elevation_matrix_cell (const elevation_matrix_cell &);
		void copy
			(const elevation_matrix_cell &);

		elevation_matrix_cell operator =
			(const elevation_matrix_cell & other);

		bool nearest
			(const logical_coordinate &target,
			logical_coordinate *best,
			double *distance_squared) const;
};

class elevation_matrix {
	public:
		// std::map <double, elevation_matrix_cell> longitude_columns;
		std::vector <logical_coordinate> data;

		// Track 2 nearest neighbors of each point
		std::vector <logical_coordinate> nearest_1, nearest_2;

		// elevation_matrix_cell *grid;
		// int number_of_columns, number_of_rows;

		elevation_matrix ();
		virtual ~elevation_matrix ();

		void add
			(const logical_coordinate &);

		void set_nearest ();

		void compute_normals
			(class elevation_matrix *normals) const;

		double nearest_elevation
			(const logical_coordinate &position) const;
		logical_coordinate nearest_point
			(const logical_coordinate &position) const;

		bool empty () const;

	protected:
		bounding_cube border;

};