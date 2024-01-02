#include "../util/utility_afx.h"
#include <vector>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/Timestamp.h"
#include "odbcinst.h" // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_field_set.h"
#include "E85_Stations.h"

const char *TABLE_NAME_E85_STATIONS = "Stations";
const char *TABLE_NAME_E85_PRICES = "Prices";

E85_Stations::E85_Stations ()

{
	clear ();
}

void E85_Stations::clear ()

// Code Written by DB Utility 2004-10-18 16:13:04

{
	ID = 0L;
	lat = 0.0f;
	lon = 0.0f;
	address [0] = '\0';
	city [0] = '\0';
	state [0] = '\0';
	zip [0] = '\0';
}


void E85_Stations::copy
	(E85_Stations *other)

// Code Written by DB Utility 2004-10-18 16:13:04

{
	ID = other->ID;
	lat = other->lat;
	lon = other->lon;
	strcpy_s (address, 256, other->address);
	strcpy_s (city, 51, other->city);
	strcpy_s (state, 3, other->state);
	strcpy_s (zip, 6, other->zip);
}

void E85_Stations::initialize_field_set ()

// Code Written by DBUtilitySharp 2009-10-19

{
	int count = 0;
	field_set.table_name = TABLE_NAME_E85_STATIONS;

	field_set.add_field (++count, &ID, "ID");
	field_set.add_field (++count, &lat, "lat");
	field_set.add_field (++count, &lon, "lon");
	field_set.add_field (++count, address, 256, "address");
	field_set.add_field (++count, city, 51, "city");
	field_set.add_field (++count, state, 3, "state");
	field_set.add_field (++count, zip, 6, "zip");

	field_set.add_index ("Primary", true, true, "ID");
}

E85_Prices::E85_Prices ()

{
	clear ();
}

void E85_Prices::clear ()

// Code Written by DB Utility 2004-10-18 16:13:04

{
	ID = 0L;
	yw.clear ();
	date.clear ();
	pe85_ret = 0.0f;
	sub_e85 = 0.0f;
}


void E85_Prices::copy
	(E85_Prices *other)

// Code Written by DB Utility 2004-10-18 16:13:04

{
	ID = other->ID;
	yw = other->yw;
	date = other->date;
	pe85_ret = other->pe85_ret;
	sub_e85 = other->sub_e85;
}

void E85_Prices::initialize_field_set ()

// Code Written by DBUtilitySharp 2009-10-19

{
	int count = 0;
	field_set.table_name = TABLE_NAME_E85_PRICES;

	field_set.add_field (++count, &ID, "ID");
	field_set.add_field (++count, &yw, "yw");
	field_set.add_field (++count, &date, "date");
	field_set.add_field (++count, &pe85_ret, "pe85_ret");
	field_set.add_field (++count, &sub_e85, "sub_e85");

	field_set.add_index ("Primary", true, true, "ID");
}
