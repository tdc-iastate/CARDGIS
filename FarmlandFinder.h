#pragma once

const int FARMLANDFINDER_FORMAT_AUCTION_1 = 1;
const int FARMLANDFINDER_FORMAT_AUCTION_2 = 2;
const int FARMLANDFINDER_FORMAT_LISTING_1 = 3;
const int FARMLANDFINDER_FORMAT_LISTING_2 = 4;
const int FARMLANDFINDER_FORMAT_NIEYAN = 5;
const int FARMLANDFINDER_FORMAT_REDUCED = 6; // 2023-03-15 "I:\TDC\FarmlandFinder\2022-08-15\Range_Ag_Farmland_Sales_Since_Nov2020.csv"
// const int FARMLANDFINDER_FORMAT_PREVIOUS_OUTPUT = 7; // 2023-03-23 "F:\FarmlandFinder\2022-08-15\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Auction-IA.csv"
const int FARMLANDFINDER_FORMAT_2024_01_19 = 7;
const int FARMLANDFINDER_FORMAT_NYS = 8;

const int FARMLANDFINDER_SALE_ATTRIBUTE_INDEX_STATE_FIPS = 2;
const int FARMLANDFINDER_SALE_ATTRIBUTE_INDEX_COUNTY_FIPS = 3;

void grain_location_callback
	(const class map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log);

class thread_work_recompute_area : public thread_work {
	public:
		class map_object *polygon;
		int area_attribute_index;
};

class thread_weighted_area : public thread_work {
	public:
		class FarmlandFinder *farms;
		int state_fips;
		dynamic_string filename_previous_output, path_ssurgo;
		dynamic_string report;
		std::map <dynamic_string, std::tuple <double, double, double, double>>* nccp_by_mukey;
		bool write_soils;
};

class combined_soil_attributes {
	// attached to ssurgo polygons as map_object.attributes_void
	public:
		class SSURGO_mapunit *ssurgo;
		dynamic_string farmlndcl;
		double nccpi3corn, nccpi3soy, nccpi3sg, nccpi3all;
		class IllinoisProductivityIndex *IllinoisPI;

		combined_soil_attributes ()
		{
			ssurgo = NULL;
			IllinoisPI = NULL;
			nccpi3corn = nccpi3soy = nccpi3sg = nccpi3all = 0.0;
		};
};


class FarmlandFinderSale {
	public:
		long id;
		dynamic_string id_text, Summary;
		double GrossPrice, area_acres, latitude, longitude;
		int StateFIPS, CountyFIPS;
		device_coordinate centroid;

		FarmlandFinderSale ();
		FarmlandFinderSale (const FarmlandFinderSale &);
		void clear ();
		void copy
			(const FarmlandFinderSale &);
		FarmlandFinderSale operator =
			(const FarmlandFinderSale &other);

};

class FarmlandFinderRepresentativeSoil {
	public:
		long id;
		int StateFIPS, CountyFIPS;
		double area_m2;
		// std::vector <std::pair <dynamic_string, double>> ssurgo_mukey_and_areas;
		std::vector <std::pair <long, double>> ssurgo_mukey_and_area_m2;

		FarmlandFinderRepresentativeSoil();
		FarmlandFinderRepresentativeSoil(const FarmlandFinderRepresentativeSoil&);
		void clear();
		void copy
			(const FarmlandFinderRepresentativeSoil&);
		FarmlandFinderRepresentativeSoil operator =
			(const FarmlandFinderRepresentativeSoil& other);

};

class FarmlandFinder {
	public:
		std::vector <FarmlandFinderSale> sales;
		std::set <int> sale_state_fips;
		int multithread_count;

		std::vector <map_layer *> proximity_layers; // layers that will be searched for closest objects in write_proximity_table

		FarmlandFinder ();
		virtual ~FarmlandFinder ();

		bool append_county_files
			(dynamic_string& filename_output,
			std::vector <dynamic_string>* thread_filenames,
			const bool remove_county_files,
			dynamic_string& log);


		bool read_positions
			(const dynamic_string &filename,
			const int limit,
			class CountyMaster *counties,
			dynamic_string &log);

		class map_layer *clip_buffers
			(class map_layer *layer_sale_circles,
			class map_layer *layer_buffer,
			class dynamic_map *map_watershed,
			class interface_window *view,
			dynamic_string &log);

		class map_layer *create_circles_area
			(class dynamic_map *map,
			const bool measure_and_adjust,
			class interface_window* view,
			dynamic_string &log);

		class map_layer *create_circles_radius
			(const double radius_miles,
			class dynamic_map *map,
			const bool measure_and_adjust,
			dynamic_string &log);

		class map_layer *create_circles_radius_beyond
			(const double add_radius_miles,
			class dynamic_map *map,
			const bool measure_and_adjust,
			dynamic_string &log);

		bool count_states
			(class map_layer *layer_sales,
			dynamic_string &log);
		bool create_point_layer
			(class dynamic_map *,
			dynamic_string &log);
		bool divide_urban_areas
			(const dynamic_string &layer_name,
			class dynamic_map *map,
			dynamic_string &error_message);
		void dump
			(dynamic_string &log);
		bool intersect_layers
			(class map_layer *layer_sales,
			const int layer_sales_source,
			class map_layer *layer_ssurgo,
			const int layer_ssurgo_source,
			class map_layer *layer_clipped,
			class interface_window *view,
			dynamic_string &log);
		bool overlay_ssurgo
			(const dynamic_string *path_ssurgo,
			class map_layer *,
			class dynamic_map *map,
			const char delimiter,
			dynamic_string &area_table,
			std::vector <dynamic_string> *clip_table,
			std::map <dynamic_string, dynamic_string> *farmlndcl_by_mukey,
			std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
			class interface_window *,
			dynamic_string &log);
		bool overlay_population
			(const dynamic_string &path_zip,
			const dynamic_string &path_ua,
			const double radius_miles,
			class dynamic_map *map,
			class interface_window *,
			dynamic_string &log);
		bool overlay_ssurgo_state
			(const dynamic_string *path_ssurgo,
			const dynamic_string &filename_output,
			const int state_fips,
			const int first_county_fips,
			class map_layer *,
			class dynamic_map *map,
			const int retain_county_polygons,
			const char delimiter,
			dynamic_string &area_table,
			std::vector <dynamic_string>* county_filenames,
			std::map <dynamic_string, dynamic_string> *farmlndcl_by_mukey,
			std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
			class IllinoisProductivityIndex_container *Illinois_PI,
			class interface_window *,
			dynamic_string &log);
		void intersection_table
			(const int state_fips,
			const int county_fips,
			class map_layer *layer_clip,
			class map_layer *layer_ssurgo,
			const char delimiter,
			std::map <long, double> *area_by_sale_id,
			dynamic_string &table);

		bool read_proximity_shapefile
			(const dynamic_string &path_shapefile,
			const dynamic_string &layer_name,
			const dynamic_string &id_field_name,
			const dynamic_string &name_field_name,
			class dynamic_map *map,
			class interface_window *view,
			dynamic_string &error_message);

		class map_layer* read_sales
			(const dynamic_string& filename_sales,
			class dynamic_map*,
			class CountyMaster* counties,
			class interface_window*,
			dynamic_string& log);
		bool read_sales
			(std::map <dynamic_string, int>* filename_and_formats,
			class CountyMaster*,
			const bool write_rows, // to match original spreadsheet rows
			const int limit,
			class interface_window *,
			dynamic_string& log);
		bool read_ssurgo_datamart
			(class odbc_database* db_ssurgo,
			std::map <dynamic_string, SSURGO_mapunit>* county_mapunits,
			class interface_window* update_display,
			dynamic_string& error_message);
		bool read_previous_output
			(const int state_fips,
			const dynamic_string& filename_sales,
			std::map <long, FarmlandFinderRepresentativeSoil>* soil_sales_by_id,
			const int limit,
			class interface_window *,
			dynamic_string& log) const;

		bool weighted_area_ssurgo
			(dynamic_string &report,
			std::map <long, FarmlandFinderRepresentativeSoil>* soil_sales_by_id,
			const int state_fips,
			const dynamic_string *path_ssurgo,
			std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
			const int limit,
			const bool write_soils,
			class interface_window *,
			dynamic_string& log) const;

		bool read_gSSURGO_mapunit
			(const dynamic_string &path_ssurgo,
			const int state_fips,
			std::map <dynamic_string, dynamic_string>* farmlndcl_by_mukey,
			std::map <dynamic_string, dynamic_string>* muhelcl_by_mukey,
			class interface_window *view,
			dynamic_string &error_message) const;
		bool read_gSSURGO_Valu1
			(const dynamic_string &path_ssurgo,
			const int state_fips,
			std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey, // nccpi3corn, nccpi3soy, nccpi3sg, nccpi3all
			class interface_window *view,
			dynamic_string &error_message) const;
		void write_proximity_table
			(dynamic_string &output_filename,
			class interface_window *view,
			dynamic_string &log);
		bool write_ssurgo_county
			(dynamic_string& filename_output,
			dynamic_string& clip_table,
			const int state_fips,
			const char delimiter,
			const bool write_header,
			dynamic_string& log);
		bool weighted_area_ssurgo_threaded
			(std::set <int> *run_states,
			const dynamic_string &path_ssurgo,
			const bool auction_results,
			const bool write_soils,
			class interface_window* view,
			dynamic_string& log);

	protected:
		COLORREF *proximity_pallette;

		double compute_area_m2
			(class map_object *circle,
			dynamic_string &log);
		class map_object *create_measured_circle
			(const double target_area_m2,
			double *final_area_m2,
			const FarmlandFinderSale *sale,
			class map_layer *layer_circles,
			dynamic_string &log);
		map_object *create_field_circle_area
			(class map_layer *layer_fields,
			const double target_area_acres,
			const class FarmlandFinderSale *sale);
		map_object *create_field_circle_radius
			(const double radius_meters,
			class map_layer *layer_fields,
			class FarmlandFinderSale *sale);
		class map_layer *import_ssurgo_county_shapefile
			(const dynamic_string &filename_ssurgo_shapefile,
			const dynamic_string &ssurgo_county_name,
			class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);

		bool intersect_county
			(const int state_fips,
			const int county_fips,
			const dynamic_string &path_ssurgo,
			std::map <long, SSURGO_mapunit> *mapunits_by_mukey,
			class map_layer *layer_sales,
			class map_layer *layer_clipped,
			class map_layer **layer_ssurgo,
			class dynamic_map *map,
			std::map <dynamic_string, dynamic_string> *farmlndcl_by_mukey,
			std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
			class IllinoisProductivityIndex_container *Illinois_PI,
			class interface_window *view,
			dynamic_string &log);
		bool parse_NYS
			(std::vector <dynamic_string>* tokens,
			class CountyMaster* counties,
			class FarmlandFinderSale* sale,
			dynamic_string& log);
		bool parse_reduced_format
		(std::vector <dynamic_string>* tokens,
			class CountyMaster* counties,
			class FarmlandFinderSale* sale,
			dynamic_string& log);
		bool parse_2024_01_19
			(std::vector <dynamic_string>* tokens,
			class CountyMaster* counties,
			class FarmlandFinderSale* sale,
			dynamic_string& log);

		bool read_ssurgo_components
			(class odbc_database*,
			std::map <long, SSURGO_mapunit>* mapunits_by_mukey,
			std::map <long, SSURGO_component>* components_by_cokey,
			class interface_window* view,
			dynamic_string& log) const;
		bool read_ssurgo_horizons
			(class odbc_database*,
			std::map <long, SSURGO_mapunit>* mapunits_by_mukey,
			std::map <long, SSURGO_component>* components_by_cokey,
			class interface_window* view,
			dynamic_string& log) const;
		bool read_ssurgo_database
			(const dynamic_string& filename_ssurgo_mdb,
			std::map <long, SSURGO_mapunit>* mapunits_by_mukey,
			class interface_window* view,
			dynamic_string& log) const;
		bool read_ssurgo_database
			(const dynamic_string& filename_ssurgo_mdb,
			std::map <long, SSURGO_mapunit>* mapunits_by_mukey,
			std::map <long, SSURGO_component>* components_by_cokey,
			class interface_window* view,
			dynamic_string& log) const;
		bool read_ssurgo_overlay
			(const dynamic_string* path_input,
			class interface_window* view,
			dynamic_string& log);
		bool read_ssurgo_state_database
			(const int state_fips,
			const dynamic_string *path_ssurgo,
			std::vector <dynamic_string> *county_folders,
			std::map <long, SSURGO_mapunit> *mapunits_by_mukey,
			const bool read_horizons,
			class interface_window *view,
			dynamic_string &log) const;
		bool read_ssurgo_state_threaded
			(const int state_fips,
			const dynamic_string* path_ssurgo,
			std::vector <dynamic_string>* county_folders,
			std::map <int, std::map <long, SSURGO_mapunit>>* mapunits_by_mukey_by_county,
			class interface_window* view,
			dynamic_string& log);
		bool ssurgo_datamart_county_folders
			(const dynamic_string* path_ssurgo,
			const int state_fips,
			std::vector <dynamic_string>* county_folders,
			dynamic_string& log) const;
		bool link_mapunits
			(class map_layer *layer,
			std::map <long, SSURGO_mapunit> *mapunits_by_mukey,
			std::map <dynamic_string, dynamic_string> *farmlndcl_by_mukey,
			std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
			class IllinoisProductivityIndex_container *Illinois_PI,
			dynamic_string &log);
		bool recompute_areas
			(class map_layer *layer,
			const int area_attribute_index,
			const byte geographiclib_linetype,
			class interface_window *view,
			dynamic_string &log);
		void write_population
			(class map_layer *layer_zip,
			class map_layer *layer_clipped_zip,
			class map_layer *layer_ua,
			class map_layer *layer_clipped_ua,
			dynamic_string &log);
};

class thread_read_mapunit : public thread_work {
	public:
		class dynamic_string filename_ssurgo_mdb;
		std::map <long, SSURGO_mapunit> mapunits_by_mukey;
};

class thread_write_proximity : public thread_work {
	public:
		std::vector <FarmlandFinderSale> sales;
		std::vector <map_layer *> *proximity_layers;
};

