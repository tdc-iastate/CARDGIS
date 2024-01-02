
class prices_by_week {
	public:
		std::map <long, double> prices;
		// long time (year * 100 + week)
		// double price;

		prices_by_week ();
		prices_by_week
			(const prices_by_week &);
		prices_by_week operator =
			(const prices_by_week &other);
		virtual void copy
			(const prices_by_week &other);

		void add_price
			(const short year,
			const short week,
			const double price,
			const long id,
			dynamic_string &log);
};

class GrainPrice {
	public:
		dynamic_string filename_db;
		void compare_prices	
			(class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);

		void distance_warehouse_township
			(class dynamic_map *map,
			class interface_window *view,
			dynamic_string &log);

		bool setup
			(class dynamic_map *map,
			class interface_window *,
			dynamic_string &log);

	protected:
		std::map <long, prices_by_week> warehouse_prices, township_prices;

		bool read_warehouses
			(class odbc_database *db,
			class dynamic_map *map,
			dynamic_string &log);
		bool read_warehouse_prices
			(class odbc_database *db,
			class dynamic_map *map,
			dynamic_string &log);
		bool read_townships
			(class odbc_database *db,
			class dynamic_map *map,
			dynamic_string &log);
		bool read_township_prices
			(class odbc_database *db,
			class dynamic_map *map,
			dynamic_string &log);
};

