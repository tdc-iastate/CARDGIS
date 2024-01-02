
// MapLayerProximity callback functions
void pipeline_zillow_point_callback
	(const class map_object *point,
	const char delimiter,
	dynamic_string &log);
void pipeline_callback
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log);
void gas_distribution_column_header_callback
	(const char delimiter,
	dynamic_string &log);
void gas_gathering_column_header_callback
	(const char delimiter,
	dynamic_string &log);
void hazardous_liquid_column_header_callback
	(const char delimiter,
	dynamic_string &log);
void liquified_natural_gas_column_header_callback
	(const char delimiter,
	dynamic_string &log);

void pipeline_zillow_point_callback_10mi
	(const class map_object *point,
	const char delimiter,
	dynamic_string &log);
void pipeline_callback_10mi
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log);
void pipeline_column_header_callback_10mi
	(const char delimiter,
	dynamic_string &log);


/*
class PipelineIncident {
	public:
		Timestamp Local_DateTime;
		long Report_Number, Operator_ID;
		double Location_Latitude, Location_Longitude;
		double Unintentional_Release;

		PipelineIncident ();
		PipelineIncident
			(const PipelineIncident &);
		PipelineIncident operator =
			(const PipelineIncident &other);
		void copy
			(const PipelineIncident &);
		void clear ();
};
*/

class PipelineAccidents {
	public:
		// ID is REPORT_NUMBER
		// attributes_numeric [0] is OPERATOR_ID
		// attributes_numeric [1] is UNINTENTIONAL_RELEASE
		// attributes_text [0] is LOCAL_DATETIME
		class map_layer *layer_gd;

		PipelineAccidents ();
		virtual ~PipelineAccidents ();

		map_layer *gas_distribution, *gas_gathering, *hazardous_liquid, *liquified_natural_gas;

		void count_within_radius
			(class map_layer *zillow_sales,
			const double radius_miles,
			const int incident_type, // -1 for all
			const int inspect_id,
			class interface_window *view,
			dynamic_string &log);

		bool read_csv
			(const dynamic_string &path_pipelines,
			const int incident_type, // -1 for all
			class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);
		void dump_incidents
			(dynamic_string &log);

	protected:
		bool read_gas_distribution
			(dynamic_string &filename,
			interface_window *view,
			dynamic_string &log);
		bool read_gas_gathering
			(dynamic_string &filename,
			interface_window *view,
			dynamic_string &log);
		bool read_hazardous_liquid
			(dynamic_string &filename,
			interface_window *view,
			dynamic_string &log);
		bool read_liquified_natural_gas
			(dynamic_string &filename,
			interface_window *view,
			dynamic_string &log);
};

