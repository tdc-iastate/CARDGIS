#include "../util/utility_afx.h"
#include <vector>
#include <map>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_field_set.h"
#include "CountyMaster.h"

CountyMasterRecord::CountyMasterRecord ()
{
	field_set = NULL;
	clear ();
}

CountyMasterRecord::CountyMasterRecord
	(const CountyMasterRecord &other)
{
	field_set = NULL;
	copy (other);
}

void CountyMasterRecord::clear ()
{
	ID = 0L;
	USCN = 0L;
	CountyName[0] = '\0';
	StateName[0] = '\0';
	StateFIPS = 0;
	CountyFIPS = 0;
}

void CountyMasterRecord::copy
	(const CountyMasterRecord &other)

{
	ID = other.ID;
	USCN = other.USCN;
	strcpy_s (CountyName, 51, other.CountyName);
	strcpy_s (StateName, 26, other.StateName);
	StateFIPS = other.StateFIPS;
	CountyFIPS = other.CountyFIPS;
}

CountyMasterRecord CountyMasterRecord::operator =
	(const CountyMasterRecord &other)
{
	if (this != &other) {
		copy (other);
	}
	return (*this);
}

void CountyMasterRecord::initialize_field_set ()

{
	int count = 0;
	field_set = new odbc_field_set;
	field_set->table_name = "CTM5";

	field_set->add_field (++count, &ID, "County");
	field_set->add_field (++count, &USCN, "Order");
	field_set->add_field (++count, CountyName, 51, "County Name");
	field_set->add_field (++count, StateName, 26, "State Name");
	field_set->add_field (++count, &StateFIPS, "State FIPS");
	field_set->add_field (++count, &CountyFIPS, "County FIPS");

	field_set->add_index ("CountyID", true, true, "County");
}


CountyMaster::CountyMaster ()
{
}


CountyMaster::~CountyMaster ()
{
}

bool CountyMaster::read
	(const char *filename_county_master,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	odbc_database_access *db_odbc = new odbc_database_access;
	odbc_database_credentials creds;
	CountyMasterRecord set_counties;
	std::map <short, std::vector <CountyMasterRecord>>::iterator state_entry;

	creds.set_database_filename (filename_county_master);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (((odbc_database_access *) db_odbc)->open (&creds, view, log)) {
		view->update_status_bar ("DB open");

		set_counties.initialize_field_set ();
		if (set_counties.field_set->open_read (db_odbc, log)) {
			if (set_counties.field_set->move_first (log)) {
				do {
					if ((state_entry = counties_by_state.find (set_counties.StateFIPS)) == counties_by_state.end ()) {
						std::vector <CountyMasterRecord> new_state;
						new_state.push_back (set_counties);
						counties_by_state.insert (std::pair <short, std::vector <CountyMasterRecord>> (set_counties.StateFIPS, new_state));
					}
					else
						state_entry->second.push_back (set_counties);
				} while (set_counties.field_set->move_next (log));
			}
			set_counties.field_set->close ();
		}
	}
	else {
		delete db_odbc;
		db_odbc = NULL;
		error = true;
	}
	return !error;
}

int CountyMaster::get_FIPS
	(const int state_fips,
	const char *name) const

{
	int found = -1;
	std::map <short, std::vector <CountyMasterRecord>>::const_iterator state_entry;
	std::vector <CountyMasterRecord>::const_iterator county;
	dynamic_string county_name = name;

	if (((state_fips == 17)
	|| (state_fips == 18)
	|| (state_fips == 29))
	&& county_name.equals_insensitive ("DeKalb"))
		county_name = "De Kalb";

	if ((state_fips == 17)
	&& county_name.equals_insensitive ("LaSalle"))
		county_name = "La Salle";

	if ((state_fips == 18)
	&& county_name.equals_insensitive ("LaPorte"))
		county_name = "La Porte";

	if ((state_fips == 27)
	&& county_name.equals_insensitive ("Lake of the Woods"))
		county_name = "Lake of the Wood";

	if ((state_fips == 36)
	&& county_name.equals_insensitive ("St Lawrence"))
		county_name = "St. Lawrence";

	if ((state_fips == 38)
	&& county_name.equals_insensitive ("LaMoure"))
		county_name = "La Moure";

	if ((state_entry = counties_by_state.find (state_fips)) != counties_by_state.end ()) {
		for (county = state_entry->second.begin ();
		(found == -1)
		&& (county != state_entry->second.end ());
		++county) {
			if (county_name.equals_insensitive (county->CountyName))
				found = county->CountyFIPS;
		}
	}

	return found;
}

CountyMasterRecord *CountyMaster::match
	(const int state_fips,
	const int county_fips) const

{
	CountyMasterRecord *found = NULL;
	std::map <short, std::vector <CountyMasterRecord>>::const_iterator state_entry;
	std::vector <CountyMasterRecord>::const_iterator county;

	if ((state_entry = counties_by_state.find(state_fips)) != counties_by_state.end()) {
		for (county = state_entry->second.begin();
		(found == NULL)
		&& (county != state_entry->second.end());
		++county) {
			if (county->CountyFIPS == county_fips)
				found = (CountyMasterRecord *) & *county;
		}
	}

	return found;
}