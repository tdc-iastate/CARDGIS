bool read_sampling_data
	(const char *filename_db_readings,
	class map_layer *,
	class interface_window *view,
	dynamic_string &log);
	

class timed_measurements {
	public:
		// Point ID or node index
		long id;

		// array of readings by node index
		// Measurements for 1 node in a segment or 1 point
		// frame number and amount
		// std::map <long, double> amounts_ppm;
		std::map <std::chrono::system_clock::time_point, double> amounts_ppm;
		// std::vector <timed_node_measurements> node_readings;

		timed_measurements ();
		timed_measurements
			(const timed_measurements &);
		timed_measurements operator =
			(const timed_measurements &);
		virtual void copy
			(const timed_measurements &);
		void clear ();

		virtual void describe
			(dynamic_string &log);

		std::chrono::system_clock::time_point latest_time () const;
		void statistics (double *low, double *high) const;
};

class stream_measurements {
	// encapsulates readings for a stream comid
	public:
		long comid;
		std::map <long, timed_measurements> node_readings;
		stream_measurements ();
		stream_measurements
			(const stream_measurements &);
		stream_measurements operator =
			(const stream_measurements &);
		virtual void copy
			(const stream_measurements &);
		void clear ();
		long latest_time () const;
};

