extern const char *TABLE_NAME_E85_STATIONS;
extern const char *TABLE_NAME_E85_PRICES;

class E85_Stations  {
	public:
		long ID;
		float lat, lon;
		char address [256], city [51], state [3], zip [6];

		E85_Stations ();
		void clear ();
		void copy (E85_Stations *other);

		odbc_field_set field_set;
		virtual void initialize_field_set ();
};

class E85_Prices  {
	public:
		long ID;
		Timestamp yw, date;
		float pe85_ret, sub_e85;

		E85_Prices ();
		void clear ();
		void copy (E85_Prices *other);

		odbc_field_set field_set;
		virtual void initialize_field_set ();
};

