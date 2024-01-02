#pragma once

void find_adjacent_counties
	(class map_layer* layer_counties,
	std::map <int, std::set <int>>* adjacent_counties,
	const bool match_across_state_lines,
	class interface_window*,
	dynamic_string& log);
int *read_CropReportingDistricts
	(const dynamic_string& filename_CRD,
	int *max_county_fips,
	class interface_window*,
	dynamic_string& log);

class thread_work_match_county: public thread_work {
	public:
		class AFIDA *controller;
		class AFIDA_County *county;
		std::map <int, std::set <int>> *adjacent_counties;
		const int *CRD_by_county;
		char delimiter;
		// dynamic_string report;
		dynamic_string filename_output;

		int state_fips, limit;
};


class AFIDA_Sale {
	public:
		dynamic_string id, buyer;
		int fips, state_fips, county_fips, CRD;
		double AverageWindSpeed;
		
		int high_wind; // 1 for top 1/3 of average wind speed among all the counties
		
		int wind_name; // AFIDA: buyer name includes "wind"

		// Iowa: 1 for buyer type "corporation" or "government"
		// AFIDA: 1 for buyer name not including "wind"
		int investor;

		int tillable_ground;
		double acres, crop_acres, percent_tillable;
		double price, estimated_value, current_value, csr;
		bool csr_missing;
		int year, month;

		// 2023-02-28 Keep all attributes of new table
		char activitytype [12];
		char country [30];
		long parcelid;
		BYTE activitynumber;
		long ownerid;
		BYTE uscode;
		short countrycode;
		short ppb;
		char ownertype [25];
		BYTE relationshiptorepresentative;
		BYTE typeofinterest;
		short percentofownership;
		BYTE acquisitionmethod;
		long debt;
		double crop;
		double pasture;
		double forest;
		double otheragriculture;
		short Rank;
		char CountyPopulation [75];
		char _merge [30];

		AFIDA_Sale();
		AFIDA_Sale(const AFIDA_Sale&);
		void clear();
		void copy (const AFIDA_Sale&);
		AFIDA_Sale operator =
			(const AFIDA_Sale& other);

		void describe_foreign
			(const char delimiter,
			dynamic_string&,
			const bool clear_first = false) const;
		void describe_domestic
			(const char delimiter,
			dynamic_string&,
			const bool clear_first = false) const;
};

class AFIDA_County {
	public:
		int fips, state_fips, county_fips;
		std::vector <AFIDA_Sale> foreign, domestic;

		std::vector <std::pair <AFIDA_Sale, AFIDA_Sale>> matched;

		AFIDA_County();
		AFIDA_County(const AFIDA_County&);
		void clear();
		void copy(const AFIDA_County&);
		AFIDA_County operator =
			(const AFIDA_County& other);

		bool match_sales_pythagorean
			(const int limit,
			const class AFIDA* reference,
			const int dimension_count,
			std::map <int, std::set <int>>* adjacent_counties,
			const char delimiter,
			dynamic_string& report,
			dynamic_string& log);
		bool match_sales_weighted
			(const dynamic_string &filename_output,
			const int limit,
			const class AFIDA* reference,
			std::map <int, std::set <int>>* adjacent_counties,
			const int* CRD_by_county,
			const char delimiter,
			class interface_window *view,
			dynamic_string& log);

		bool add_foreign_unique(const AFIDA_Sale&);
		bool add_domestic_unique(const AFIDA_Sale&);

		AFIDA_Sale *match_foreign
			(const dynamic_string &id) const;
		AFIDA_Sale *match_domestic
			(const dynamic_string &id) const;
};

class AFIDA {
	public:
		std::vector <AFIDA_Sale> foreign, domestic_all_states;
		std::map <int, AFIDA_County> potential_matches;
		double min_acres, max_acres, range_acres;
		double min_percent_tillable, max_percent_tillable, range_percent_tillable;
		std::map <int, double> high_wind_by_fips;
		int max_county_fips;

		int foreign_id_index, domestic_id_index; // column indices of foreign and domestic IDs used by append_and_filter_output_files

		std::vector <dynamic_string> thread_filenames;

		AFIDA();

		bool append_and_filter_output_files
			(const dynamic_string& filename_output,
			const char delimiter,
			const bool cleanup_files,
			class interface_window* view,
			dynamic_string& log);
		bool read
			(const dynamic_string& filename,
			const int state_fips,
			class CountyMaster*,
			const int* CRD_by_county,
			dynamic_string& log);
		bool read_2023_02_24
			(const dynamic_string& filename,
			const int state_fips,
			class CountyMaster*,
			const int* CRD_by_county,
			dynamic_string& log);
		bool read_county_wind
			(const dynamic_string& filename,
			class CountyMaster* counties,
			dynamic_string& log);
		bool read_Kansas
			(const dynamic_string& filename,
			class CountyMaster*,
			const int* CRD_by_county,
			dynamic_string& log);
		bool read_Iowa
			(const dynamic_string& filename,
			class CountyMaster*,
			dynamic_string& log);
		bool read_FarmlandFinder_2022_09_22
			(const dynamic_string& filename,
			class CountyMaster*,
			const int* CRD_by_county,
			dynamic_string& log);
		void report
			(class CountyMaster*,
			std::map <int, std::set <int>>* adjacent_counties,
			dynamic_string& log);
		void county_report
			(const dynamic_string &filename_output,
			const int state_fips,
			class CountyMaster*,
			std::map <int, std::set <int>>* adjacent_counties,
			const int* CRD_by_county,
			const int dimension_count,
			class interface_window *,
			dynamic_string& log);
protected:
	bool append_output_files
		(const dynamic_string& filename_output,
		const int state_fips,
		const char delimiter,
		std::vector <dynamic_string>* thread_filenames,
		dynamic_string& log);
	void check_sale_ranges
			(const class AFIDA_Sale* sale);
		void check_ranges();
		void county_report_matches
			(const dynamic_string &filename_output,
			const char delimiter,
			const int state_fips,
			class CountyMaster* counties,
			std::map <int, std::set <int>>* adjacent_counties,
			const int* CRD_by_county,
			dynamic_string& report,
			class interface_window* view,
			dynamic_string& log);
		void fill_potential_matches
			(std::vector <AFIDA_Sale>* local_domestic,
			const int state_fips,
			class CountyMaster* counties,
			std::map <int, std::set <int>>* adjacent_counties,
			const int* CRD_by_county,
			dynamic_string &log);
		void filter_to_state_or_adjacent_counties
			(std::vector <AFIDA_Sale>* sales,
			const int state_fips,
			std::map <int, std::set <int>>* adjacent_counties,
			dynamic_string &log);

		bool set_Iowa_tillable_ground
			(dynamic_string& log);
		void write_column_headers
			(const int state_fips,
			FILE* f,
			const char delimiter);
};

