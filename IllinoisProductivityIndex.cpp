#include "..\util\utility_afx.h"
#include <map>
#include "..\util\dynamic_string.h"
#include "..\util\interface_window.h"
#ifdef _ODBC_TDC
#include <odbcinst.h>
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_field_set.h"
#else
#include "..\util\db.h"
#endif
#include "IllinoisProductivityIndex.h"

const char *TABLE_NAME_ILLINOIS_PRODUCTIVITY_INDEX = "New_Yields_PI_Export";

IllinoisProductivityIndex::IllinoisProductivityIndex ()
{
	field_set = NULL;
	clear ();
}

IllinoisProductivityIndex::IllinoisProductivityIndex (const IllinoisProductivityIndex &other)
{
	field_set = NULL;
	copy (&other);
}

IllinoisProductivityIndex::~IllinoisProductivityIndex ()

{
	if (field_set)
		delete field_set;
}

void IllinoisProductivityIndex::clear ()

// Code Written by DBUtilitySharp 2006-12-08

{
	Area_Name [0] = '\0';
	AreaSymbol [0] = '\0';
	MapunitKey [0] = '\0';
	ComponentName[0] = '\0';
	MapunitSymbol [0] = '\0';
	COMPKind [0] = '\0';
	MUKind [0] = '\0';
	Texture_Factor = 0.0;
	_811_Slope_ERO_Fact = 0.0;
	_810_Slope_ERO_Fact = 0.0;
	MapunitName [0] = '\0';
	NirrLCCNirrSubcl [0] = '\0';
	PI = 0.0;
}

void IllinoisProductivityIndex::copy
	(const IllinoisProductivityIndex *other)

{
	strcpy_s (Area_Name, 81, other->Area_Name);
	strcpy_s (AreaSymbol, 6, other->AreaSymbol);
	strcpy_s (MapunitKey, 31, other->MapunitKey);
	strcpy_s (ComponentName, 61, other->ComponentName);
	strcpy_s (MapunitSymbol, 7, other->MapunitSymbol);
	strcpy_s (COMPKind, 31, other->COMPKind);
	strcpy_s (MUKind, 31, other->MUKind);
	Texture_Factor = other->Texture_Factor;
	_811_Slope_ERO_Fact = other->_811_Slope_ERO_Fact;
	_810_Slope_ERO_Fact = other->_810_Slope_ERO_Fact;
	strcpy_s (MapunitName, 176, other->MapunitName);
	strcpy_s (NirrLCCNirrSubcl, 7, other->NirrLCCNirrSubcl);
	PI = other->PI;
}

IllinoisProductivityIndex IllinoisProductivityIndex::operator =
	(const IllinoisProductivityIndex &other)
{
	if (this != &other) {
		copy (&other);
	}
	return (*this);
}

void IllinoisProductivityIndex::initialize_field_set ()

// Code Written by DBUtilitySharp 2014-12-17

{
	int count = 0;
	field_set = new odbc_field_set;
	field_set->table_name = TABLE_NAME_ILLINOIS_PRODUCTIVITY_INDEX;

	field_set->add_field (++count, Area_Name, 81, "Area_Name");
	field_set->add_field (++count, AreaSymbol, 6, "Area Symbol");
	field_set->add_field (++count, MapunitKey, 31, "Mapunit Key");
	field_set->add_field (++count, ComponentName, 61, "Component Name");
	field_set->add_field (++count, MapunitSymbol, 7, "Mapunit Symbol");
	field_set->add_field (++count, COMPKind, 31, "COMPKind");
	field_set->add_field (++count, MUKind, 31, "MUKind");
	field_set->add_field (++count, &Texture_Factor, "Texture_Factor");
	field_set->add_field (++count, &_811_Slope_ERO_Fact, "811_Slope_ERO_Fact");
	field_set->add_field (++count, &_810_Slope_ERO_Fact, "810_Slope_ERO_Fact");
	field_set->add_field (++count, MapunitName, 176, "Mapunit Name");
	field_set->add_field (++count, NirrLCCNirrSubcl, 7, "Nirr LCCNirr Subcl");
	field_set->add_field (++count, &PI, "PI");

	field_set->add_index ("Mapunit and Component", false, false, "Mapunit Key", "Component Name");
}

dynamic_string strip_component
	(const dynamic_string &ssurgo_component)

	// Mapunit 179078 "St. Charles" has to match "StCharles"
{
	int index, len = ssurgo_component.get_length ();
	char c;
	dynamic_string shortname;

	for (index = 0; index < len; ++index) {
		c = ssurgo_component.get_at_ascii (index);
		if ((c != ' ')
		&& (c != '.'))
			shortname += c;
	}
	return shortname;
}

IllinoisProductivityIndex *IllinoisProductivityIndex_container::get
	(const dynamic_string &mukey,
	const dynamic_string &component)

{
	std::pair <dynamic_string, dynamic_string> key;
	std::map <std::pair <dynamic_string, dynamic_string>, IllinoisProductivityIndex>::const_iterator index;

	key.first = mukey;
	key.second = strip_component (component);

	if ((index = pi_by_soil.find (key)) != pi_by_soil.end ())
		return (IllinoisProductivityIndex *) &index->second;
	else
		return NULL;
}

bool IllinoisProductivityIndex_container::read
	(const dynamic_string &filename_mdb,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	odbc_database_access *db_odbc = new odbc_database_access;
	odbc_database_credentials creds;
	IllinoisProductivityIndex set_pi;
	std::pair <dynamic_string, dynamic_string> key;

	creds.set_database_filename (filename_mdb);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (((odbc_database_access *) db_odbc)->open (&creds, view, log)) {
		view->update_status_bar ("DB open");

		set_pi.initialize_field_set ();
		if (set_pi.field_set->open_read (db_odbc, log)) {
			if (set_pi.field_set->move_first (log)) {
				do {
					key.first = set_pi.MapunitKey;
					key.second = set_pi.ComponentName;
					pi_by_soil.insert (std::pair <std::pair <dynamic_string, dynamic_string>, IllinoisProductivityIndex> (key, set_pi));
				} while (set_pi.field_set->move_next (log));
			}
			set_pi.field_set->close ();
		}

		db_odbc->close ();
	}
	else
		error = true;

	delete db_odbc;
	db_odbc = NULL;

	return !error;
}
