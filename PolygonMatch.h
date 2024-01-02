
class PolygonMatch {
	public:
		class map_layer *layer_1, *layer_2;

		char output_delimiter;
		int thread_count;
		int huc12_attribute_index;

		PolygonMatch ();

		bool run
			(dynamic_string &filename_output,
			dynamic_string &log);
		bool run_threaded
			(dynamic_string &output_file,
			class interface_window *view,
			dynamic_string &log);
};

