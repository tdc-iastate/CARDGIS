
extern const char *TABLE_NAME_ILLINOIS_PRODUCTIVITY_INDEX;

// 2021-02-03 from "I:\TDC\FarmlandFinder\2020_12Dec_04_New_Yields_PI_Exports.xlsx"

class IllinoisProductivityIndex {
	public:
		char Area_Name [81];
		char AreaSymbol [6];
		char MapunitKey [31];
		char ComponentName[61];
		char MapunitSymbol [7];
		char COMPKind [31];
		char MUKind [31];
		double Texture_Factor;
		double _811_Slope_ERO_Fact;
		double _810_Slope_ERO_Fact;
		char MapunitName [176];
		char NirrLCCNirrSubcl [7];
		double PI;

		odbc_field_set *field_set;
		virtual void initialize_field_set ();

		IllinoisProductivityIndex ();
		IllinoisProductivityIndex
			(const IllinoisProductivityIndex &);
		virtual ~IllinoisProductivityIndex ();
		void clear ();
		void copy (const IllinoisProductivityIndex *other);
		IllinoisProductivityIndex operator =
			(const IllinoisProductivityIndex &other);
};


class IllinoisProductivityIndex_container {
	public:
		IllinoisProductivityIndex *get
			(const dynamic_string &mukey,
			const dynamic_string &component);

		bool read
			(const dynamic_string &filename_mdb,
			class interface_window *,
			dynamic_string &log);

	protected:
		std::map <std::pair <dynamic_string, dynamic_string>, IllinoisProductivityIndex> pi_by_soil;
};
