#pragma once

const int PROXIMITY_WITHIN_POLYGON_DONT_DETECT = 0; // Doesn't check if base layer point is within proximity layer polygons
const int PROXIMITY_WITHIN_POLYGON_NEGATIVE = 1; // If proximity layer is polygon or complex polygon and base layer point is within, negative distance
const int PROXIMITY_WITHIN_POLYGON_ZERO = 2; // If proximity layer is polygon or complex polygon and base layer point is within, zero distance


// 2022-01-03 Address points to lake centers
void address_point_callback
	(const map_object *point,
	const char delimiter,
	dynamic_string &log);
void lake_centroid_callback
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log);
void lake_centroid_callback_headers
	(const char delimiter,
	dynamic_string &log);
void proximity_id_name_callback
	(const class map_object* point,
	const char delimiter,
	dynamic_string& log);
void proximity_id_name_distance_callback
	(const class map_object* point,
	const double distance_m,
	const char delimiter,
	dynamic_string& log);
void proximity_id_name_distance_column_headers_callback
	(const char delimiter,
	dynamic_string& column_names);

class ProximityResult {
	public:
		class map_object *object;
		double distance_m;

		ProximityResult ();
		ProximityResult (const ProximityResult &other);
		ProximityResult operator =
			(const ProximityResult &other);
		virtual void copy
			(const ProximityResult &other);
		void clear ()
		{
			object = NULL;
			distance_m = 0.0;
		};
};

class ProximityLayerResult {
	public:
		// 0 or 1 result per layer
		std::vector <std::pair <class map_layer *, ProximityResult>> layers;

		ProximityLayerResult ();
		ProximityLayerResult (const ProximityLayerResult &other);
		ProximityLayerResult operator =
			(const ProximityLayerResult &other);
		virtual void copy
			(const ProximityLayerResult &other);
};

class job_MapLayerProximity: public thread_work {
	public:
		class MapLayerProximity *proximator;
		map_object *base_layer_point;
		ProximityLayerResult result;
		int within_check;
		int add_to_base_shapefile_index; // If >= 0, Numeric distances go in the these columns of attributes_numeric

		// 2022-07-15 Each thread gets unique file, but not each job
		std::vector <FILE *> *output_files;
		int list_size, match_count, thread_count;
};

class ProximityLayer {
	public:
		map_layer *layer;

		// optional callback function to write point ID & distance
		void (*write_layer_point) (const class map_object *, const double distance_m, const char delimiter, dynamic_string &);
		// optional callback function to write column headers used by write_layer_point
		void (*write_column_headers) (const char delimiter, dynamic_string &);

		ProximityLayer ();
		ProximityLayer (const ProximityLayer &other);
		ProximityLayer operator =
			(const ProximityLayer &other);
		virtual void copy
			(const ProximityLayer &other);
};

class MapLayerProximity {

	// Given a map_layer and a vector of other map_layers,
	// find nearest pairing from first to each of the others

	public:
		map_layer *base_layer;
		std::vector <ProximityLayer> proximity_layers; // layers that will be searched for closest objects in write_proximity_table

		// Callback function to write first columns of base_layer object
		void (*write_point_function) (const class map_object *, const char delimiter, dynamic_string &);

		dynamic_string output_filename, base_layer_column_headers;
		int thread_count;
		char delimiter;
		double cutoff_radius_miles, cutoff_radius_m, cutoff_radius_m_plus;

		MapLayerProximity ();

		void compute_proximity
			(std::map <long, ProximityLayerResult> *nearest,
			class interface_window *view,
			dynamic_string &log);
		void compute_enclosure
			(std::map <long, ProximityLayerResult> *inside,
			class interface_window *view,
			dynamic_string &log);

		bool divide_urban_areas
			(const dynamic_string &layer_name,
			dynamic_map *map,
			dynamic_string &log);

		class ProximityLayer *read_proximity_csv
			(const dynamic_string &path_csv,
			const dynamic_string &layer_name,
			class dynamic_map *map,
			class interface_window *view,
			dynamic_string &error_message);
		class ProximityLayer* read_proximity_csv_2
			(const dynamic_string& path_csv,
			const dynamic_string& layer_name,
			const long max_lake_id, // 2022-01-11 Yongjie: omit lakes 140..142 
			class dynamic_map* map,
			class interface_window* view,
			dynamic_string& error_message);
		/*
		bool read_proximity_shapefile
			(const dynamic_string& path_shapefiles,
			const dynamic_string& filename_shapefile,
			const dynamic_string& id_field_name,
			const dynamic_string& name_field_name,
			void (*p_write_layer_point) (const class map_object*, const double distance_m, const char delimiter, dynamic_string&),
			void (*p_write_column_headers) (const char delimiter, dynamic_string&),
			class map_layer* layer,
			class dynamic_map* map,
			class interface_window* view,
			dynamic_string& error_message);
		*/
		class ProximityLayer* read_proximity_shapefile
			(const dynamic_string& path_shapefiles,
			const dynamic_string& filename_shapefile,
			const dynamic_string& layer_name,
			const dynamic_string& id_field_name,
			const dynamic_string& name_field_name,
			class dynamic_map *map,
			class interface_window* view,
			dynamic_string& error_message);
		bool read_state_shapefiles
			(const dynamic_string &path_shapefile,
			const dynamic_string &layer_name,
			const dynamic_string &id_field_name,
			const dynamic_string &name_field_name,
			class dynamic_map *map,
			class interface_window *view,
			dynamic_string &error_message);
		bool read_shapefile
			(const dynamic_string& path_shapefiles,
			const dynamic_string& filename_shapefile,
			const dynamic_string& id_field_name,
			const dynamic_string& name_field_name,
			class map_layer* layer,
			class dynamic_map* map,
			class interface_window* view,
			dynamic_string& error_message);
		void write_proximity_table
			(const int within_check,
			const int add_to_base_shapefile_index, // If >= 0, column_numeric index for distance result
			class interface_window *view,
			dynamic_string &log);
		bool write_proximity_table_within_radius
			(class interface_window *view,
			dynamic_string &log);
		void write_proximity_table_list_closest
			(const int list_size,
			class interface_window *view,
			dynamic_string &log);
	protected:
		bool open_thread_files
			(std::vector <FILE*>* thread_files,
			std::vector <dynamic_string>* thread_filenames,
			dynamic_string& log);
		bool append_thread_files
			(std::vector <FILE*>* thread_files,
			std::vector <dynamic_string>* thread_filenames,
			dynamic_string& log);
};

