#pragma once

class CountyMasterRecord {
	public:
		long ID;
		long USCN;
		char CountyName[51];
		char StateName[26];
		short StateFIPS, CountyFIPS;

		
		CountyMasterRecord ();
		CountyMasterRecord (const CountyMasterRecord &);
		void clear ();
		void copy
			(const CountyMasterRecord &);
		CountyMasterRecord operator =
			(const CountyMasterRecord &other);

		class odbc_field_set *field_set;
		virtual void initialize_field_set ();
};


class CountyMaster {
	public:
		std::map <short, std::vector <CountyMasterRecord>> counties_by_state;

		CountyMaster ();
		virtual ~CountyMaster ();

		bool read
			(const char *filename_county_master,
			class interface_window *view,
			dynamic_string &log);

		int get_FIPS
			(const int state_fips,
			const char* county_name) const;
		class CountyMasterRecord *match
			(const int state_fips,
			const int county_fips) const;
};

