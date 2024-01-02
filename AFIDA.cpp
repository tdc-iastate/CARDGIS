
#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include <stack>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/listbox_files.h"
#include "../util/interface_window_wx.h"
#include "../util/device_coordinate.h"
#include "../util/dialog_error_list.h"
#include "../util/custom.h"
#include "../util/RecentlyOpened.h"
#include "../util/message_slot.h"
#include "../util/xml_parser.h"
#include "../util/arbitrary_counter.h"
#include "../util/Ledger.h"
#include "../util/state_names.h"
#include "CountyMaster.h"
#include "../subbasin_effect/thread_manager.h"
#include "AFIDA.h"

#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"

void correct_iowa_county_name
	(dynamic_string& name)

// 2022-04-15 I:\TDC\AFIDA\Iowa_Ag_Sales_append_all.csv

{
	if (name == "Pottawattomie")
		name = "Pottawattamie";
}

void correct_county_name
(dynamic_string& name,
	const int state_fips)

// 2022-10-04 "I:\TDC\FarmlandFinder\2022-10-04\State_average_wind_speed_all.csv"

{
	switch (state_fips) {
		case 17:
			if (name == "Dewitt")
				name = "De Witt";
			else
				if (name == "Dekalb")
					name = "De Kalb";
				else
					if (name == "Dupage")
						name = "Du Page";
					else
						if (name == "Saint Clair")
							name = "St. Clair";
			break;
		case 18:
			if (name == "St Joseph")
				name = "St. Joseph";
			break;
		case 26:
			if (name == "Saint Joseph")
				name = "St. Joseph";
			else
				if (name == "Saint Clair")
					name = "St. Clair";
			break;
		case 27:
			if (name == "Saint Louis")
				name = "St. Louis";
			break;
		case 29:
			if (name == "Dekalb")
				name = "De Kalb";
			else
				if (name == "Saint Charles")
					name = "St. Charles";
				else
					if (name == "Saint Clair")
						name = "St. Clair";
					else
						if (name == "Saint Francois")
							name = "St. Francois";
						else
							if (name == "Sainte Genevieve")
								name = "Ste. Genevieve";
							else
								if (name == "Saint Louis")
									name = "St. Louis";
								else
									if (name == "Saint Louis City")
										name = "St. Louis City";
			break;
		case 38:
			if (name == "Mountrial")
				name = "Mountrail";
			break;
		case 55:
			if (name == "Saint Croix")
				name = "St. Croix";
	//		default:
	}
}

void set_wind_counties_iowa_kansas
	(std::map <int, double> *high_wind_by_fips)

// "I:\TDC\AFIDA\sheets\State Average Wind Speed.xlsx"

// 2022-04-21 Now uses the top 1/3 counties in each state (33 for Iowa, 37 for Kansas)

{
	high_wind_by_fips->insert(std::pair <int, double>(19001, 22.45)); //Hamilton, IA
	high_wind_by_fips->insert(std::pair <int, double>(19003, 22.1)); //Greene, IA
	high_wind_by_fips->insert(std::pair <int, double>(19005, 21.86)); //Boone, IA
	high_wind_by_fips->insert(std::pair <int, double>(19007, 21.54)); //Webster, IA
	high_wind_by_fips->insert(std::pair <int, double>(19009, 21.21)); //Story, IA
	high_wind_by_fips->insert(std::pair <int, double>(19011, 21.19)); //Carroll, IA
	high_wind_by_fips->insert(std::pair <int, double>(19013, 21.17)); //Humboldt, IA
	high_wind_by_fips->insert(std::pair <int, double>(19015, 20.95)); //Guthrie, IA
	high_wind_by_fips->insert(std::pair <int, double>(19017, 20.92)); //Palo Alto, IA
	high_wind_by_fips->insert(std::pair <int, double>(19019, 20.82)); //Wright, IA
	high_wind_by_fips->insert(std::pair <int, double>(19021, 20.5)); //Kossuth, IA
	high_wind_by_fips->insert(std::pair <int, double>(19023, 20.22)); //Hancock, IA
	high_wind_by_fips->insert(std::pair <int, double>(19025, 20.04)); //Dallas, IA
	high_wind_by_fips->insert(std::pair <int, double>(19027, 19.99)); //Audubon, IA
	high_wind_by_fips->insert(std::pair <int, double>(19029, 19.94)); //Calhoun, IA
	high_wind_by_fips->insert(std::pair <int, double>(19031, 19.89)); //Davis, IA
	high_wind_by_fips->insert(std::pair <int, double>(19033, 19.86)); //Monona, IA
	high_wind_by_fips->insert(std::pair <int, double>(19035, 19.78)); //Hardin, IA
	high_wind_by_fips->insert(std::pair <int, double>(19037, 19.71)); //Woodbury, IA
	high_wind_by_fips->insert(std::pair <int, double>(19039, 19.4)); //Clay, IA
	high_wind_by_fips->insert(std::pair <int, double>(19041, 19.39)); //Madison, IA
	high_wind_by_fips->insert(std::pair <int, double>(19043, 19.31)); //Polk, IA
	high_wind_by_fips->insert(std::pair <int, double>(19045, 19.3)); //Appanoose, IA
	high_wind_by_fips->insert(std::pair <int, double>(19047, 19.23)); //Monroe, IA
	high_wind_by_fips->insert(std::pair <int, double>(19049, 19.23)); //Clarke, IA
	high_wind_by_fips->insert(std::pair <int, double>(19051, 19.18)); //Allamakee, IA
	high_wind_by_fips->insert(std::pair <int, double>(19053, 19.02)); //Warren, IA
	high_wind_by_fips->insert(std::pair <int, double>(19055, 18.95)); //Wapello, IA
	high_wind_by_fips->insert(std::pair <int, double>(19057, 18.92)); //Franklin, IA
	high_wind_by_fips->insert(std::pair <int, double>(19059, 18.91)); //Van Buren, IA
	high_wind_by_fips->insert(std::pair <int, double>(19061, 18.85)); //Sac, IA
	high_wind_by_fips->insert(std::pair <int, double>(19063, 18.84)); //Pocahontas, IA
	high_wind_by_fips->insert(std::pair <int, double>(19065, 18.72)); //Plymouth, IA

	high_wind_by_fips->insert(std::pair <int, double>(20001, 27.55)); //Rush, KS
	high_wind_by_fips->insert(std::pair <int, double>(20003, 26.08)); //Ellis, KS
	high_wind_by_fips->insert(std::pair <int, double>(20005, 25.98)); //Pratt, KS
	high_wind_by_fips->insert(std::pair <int, double>(20007, 25.13)); //Barton, KS
	high_wind_by_fips->insert(std::pair <int, double>(20009, 25.1)); //Pawnee, KS
	high_wind_by_fips->insert(std::pair <int, double>(20011, 25.04)); //Edwards, KS
	high_wind_by_fips->insert(std::pair <int, double>(20013, 24.51)); //Harper, KS
	high_wind_by_fips->insert(std::pair <int, double>(20015, 24.38)); //Kingman, KS
	high_wind_by_fips->insert(std::pair <int, double>(20017, 24.35)); //Stafford, KS
	high_wind_by_fips->insert(std::pair <int, double>(20019, 24.28)); //Trego, KS
	high_wind_by_fips->insert(std::pair <int, double>(20021, 23.75)); //Hodgeman, KS
	high_wind_by_fips->insert(std::pair <int, double>(20023, 23.46)); //Russell, KS
	high_wind_by_fips->insert(std::pair <int, double>(20025, 23.35)); //Reno, KS
	high_wind_by_fips->insert(std::pair <int, double>(20027, 23.14)); //Barber, KS
	high_wind_by_fips->insert(std::pair <int, double>(20029, 22.79)); //Cheyenne, KS
	high_wind_by_fips->insert(std::pair <int, double>(20031, 22.58)); //Rice, KS
	high_wind_by_fips->insert(std::pair <int, double>(20033, 22.46)); //Logan, KS
	high_wind_by_fips->insert(std::pair <int, double>(20035, 22.44)); //Kiowa, KS
	high_wind_by_fips->insert(std::pair <int, double>(20037, 22.37)); //Thomas, KS
	high_wind_by_fips->insert(std::pair <int, double>(20039, 22.04)); //Graham, KS
	high_wind_by_fips->insert(std::pair <int, double>(20041, 21.85)); //Scott, KS
	high_wind_by_fips->insert(std::pair <int, double>(20043, 21.78)); //Ness, KS
	high_wind_by_fips->insert(std::pair <int, double>(20045, 21.74)); //Marshall, KS
	high_wind_by_fips->insert(std::pair <int, double>(20047, 21.7)); //Lane, KS
	high_wind_by_fips->insert(std::pair <int, double>(20049, 21.58)); //Sheridan, KS
	high_wind_by_fips->insert(std::pair <int, double>(20051, 21.5)); //Osborne, KS
	high_wind_by_fips->insert(std::pair <int, double>(20053, 21.36)); //Decatur, KS
	high_wind_by_fips->insert(std::pair <int, double>(20055, 21.31)); //Wallace, KS
	high_wind_by_fips->insert(std::pair <int, double>(20057, 21.23)); //Greeley, KS
	high_wind_by_fips->insert(std::pair <int, double>(20059, 21.21)); //Sherman, KS
	high_wind_by_fips->insert(std::pair <int, double>(20061, 21.12)); //Sumner, KS
	high_wind_by_fips->insert(std::pair <int, double>(20063, 21.11)); //Rawlins, KS
	high_wind_by_fips->insert(std::pair <int, double>(20065, 21.02)); //Wichita, KS
	high_wind_by_fips->insert(std::pair <int, double>(20067, 20.92)); //Comanche, KS
	high_wind_by_fips->insert(std::pair <int, double>(20069, 20.68)); //Mcpherson, KS
	high_wind_by_fips->insert(std::pair <int, double>(20071, 20.64)); //Gove, KS
	high_wind_by_fips->insert(std::pair <int, double>(20073, 20.62)); //Lincoln, KS
}

bool AFIDA::read_county_wind
	(const dynamic_string& filename_wind,
	CountyMaster* counties,
	dynamic_string& log)

// "I:\TDC\FarmlandFinder\2022-10-04\State_average_wind_speed_all.csv"

// 2022-10-04 new version with 13 states

{
	FILE* file_csv;
	char data[2048];
	std::vector <dynamic_string> tokens;
	dynamic_string inconstant_filename, parse_data, county_name;
	bool error = false;
	int count_read = 0, state_fips, county_fips, average_wind, high_wind;
	std::map <int, int> count_by_state;
	std::map <int, int>::iterator state_count_read;

	// 2022-10-05 Iowa, Kansas, Ohio missing from this file. Nieyan: skip Ohio, use older Iowa & Kansas from I:\TDC\AFIDA\sheets\State Average Wind Speed.xlsx
	set_wind_counties_iowa_kansas (&high_wind_by_fips);

	inconstant_filename = filename_wind;
	if (fopen_s(&file_csv, inconstant_filename.get_text_ascii(), "r") == 0) {

		// Rank, Average Wind Speed, Unit, County, State, high wind
		// 1, 26, mph, Stephenson, IL, 1
		// 2, 22.68, mph, Stark, IL, 1

		if (fgets(data, 2048, file_csv)) {

			while (!error
			&& fgets(data, 1024, file_csv)) {
				++count_read;
				parse_data = data;
				parse_data.tokenize(",\r\n", &tokens, true, false);

				if (tokens.size() == 6) {

					average_wind = atof (tokens [1].get_text_ascii ());
					county_name = tokens[3];
					state_fips = state_FIPS_from_abbreviation(tokens[4].get_text_ascii());
					high_wind = atoi (tokens [5].get_text_ascii ());
					// county_name.trim_right();
					correct_county_name(county_name, state_fips);

					if ((county_fips = counties->get_FIPS (state_fips, county_name.get_text_ascii())) != -1) {

						if (high_wind == 1) {
							high_wind_by_fips.insert(std::pair <int, double>(state_fips * 1000 + county_fips, average_wind));

							if ((state_count_read = count_by_state.find (state_fips)) != count_by_state.end ()) 
								state_count_read->second += 1;
							else
								count_by_state.insert (std::pair <int, int> (state_fips, 1));
						}
					}
					else {
						log += "ERROR, FIPS invalid \"";
						log += tokens[3];
						log += "\", \"";
						log += tokens[4];
						log += "\"\n";
						error = true;
					}

				}
				else {
					log.add_formatted("ERROR, can't parse county wind file (%d tokens) \"", tokens.size());
					log += filename_wind;
					log.add_formatted("\" row %d.\n", count_read);

					log += "\"";
					log += data;
					log += "\"\n";
					error = true;
				}
			}
		}

		fclose(file_csv);

		log.add_formatted("County Wind Records read\t%d\n", count_read);
		log.add_formatted("Counties with high wind\t%d\n", (int) high_wind_by_fips.size ());

		log += "\tState\tCount\n";
		for (state_count_read = count_by_state.begin ();
		state_count_read != count_by_state.end ();
		++state_count_read)
			log.add_formatted ("\t%d\t%d\n", state_count_read->first, state_count_read->second);

	}
	else {
		log += "ERROR, can't open county wind file \"";
		log += filename_wind;
		log += "\"\n";
		error = true;
	}
	return !error;
}

int *read_CropReportingDistricts
	(const dynamic_string &filename_CRD,
	int *max_county_fips,
	class interface_window*,
	dynamic_string& log)

// 2022-04-28 emailed from Wendong.  Iowa and Kansas only

// 2022-10-12 Nieyan: CRD is not unique within states.  Use with state FIPS.

{
	FILE* file_csv;
	char data[2048];
	std::vector <dynamic_string> tokens;
	dynamic_string inconstant_filename, parse_data;
	int district, state_fips, county_fips;
	bool error = false;
	int count_read = 0, fips;
	std::vector <std::pair <int, int>> first_pass;
	std::vector <std::pair <int, int>>::iterator first_pass_pair;

	*max_county_fips = 0;

	inconstant_filename = filename_CRD;
	if (fopen_s(&file_csv, inconstant_filename.get_text_ascii(), "r") == 0) {

		// State	Ag District	Ag District Code	County	County ANSI
		//	IOWA	CENTRAL	50	BOONE	15
		//	IOWA	CENTRAL	50	DALLAS	49

		// All-state version 2022-10-04, format has changed:

		// State FIPS, Ag district, County FIPS, County name
		// 17, 10, 11, Bureau
		// 17, 10, 15, Carroll
		// 17, 10, 73, Henry
		// 17, 10, 85, Jo Daviess

		if (fgets(data, 2048, file_csv)) {

			while (!error
			&& fgets(data, 1024, file_csv)) {
				++count_read;
				parse_data = data;
				parse_data.tokenize(",\r\n", &tokens, true, false);

				if (tokens.size() == 4) {
					state_fips = atoi (tokens [0].get_text_ascii ());
					district = atoi (tokens [1].get_text_ascii ());
					county_fips = atoi (tokens [2].get_text_ascii ());
					fips = state_fips * 1000 + county_fips;
					first_pass.push_back (std::pair <int, int> (fips, district));
					if (fips > *max_county_fips)
						*max_county_fips = fips;
				}
				else {
					log.add_formatted("ERROR, can't parse CRD file (%d tokens) \"", tokens.size());
					log += filename_CRD;
					log.add_formatted("\" row %d.\n", count_read);

					log += "\"";
					log += data;
					log += "\"\n";
					error = true;
				}
			}
		}

		fclose(file_csv);

		log.add_formatted("CRD Records read\t%d\n", count_read);

		if (!error) {
			int * CRD_by_county = new int [*max_county_fips];
			memset (CRD_by_county, 0, sizeof (int) * *max_county_fips);
			for (first_pass_pair = first_pass.begin ();
			first_pass_pair != first_pass.end ();
			++first_pass_pair)
				CRD_by_county [first_pass_pair->first - 1] = first_pass_pair->second;

			return CRD_by_county;
		}
	}
	else {
		log += "ERROR, can't open Iowa file \"";
		log += filename_CRD;
		log += "\"\n";
	}
	return NULL;
}

AFIDA_Sale::AFIDA_Sale()

{
	clear ();
}

AFIDA_Sale::AFIDA_Sale(const AFIDA_Sale& other)

{
	copy(other);
}

void AFIDA_Sale::clear()

{
	id.clear ();
	buyer.clear ();
	fips = state_fips = county_fips = -1;
	CRD = -1;
	acres = crop_acres = percent_tillable = 0.0;
	price = 0.0;
	estimated_value = 0.0;
	current_value = 0.0;
	year = month = 0;
	AverageWindSpeed = 0.0;
	high_wind = investor = tillable_ground = 0;
	wind_name = 0;
	csr = 0.0;
	csr_missing = false;

	// 2023-02-28 Keep all attributes of new table
	activitytype [0] = '\0';
	country[0] = '\0';
	parcelid = 0;
	activitynumber = 0;
	ownerid = 0;
	uscode = 0;
	countrycode = 0;
	ppb = 0;
	ownertype [0] = '\0';
	relationshiptorepresentative = 0;
	typeofinterest = 0;
	percentofownership = 0;
	acquisitionmethod = 0;
	debt = 0;
	crop = 0.0;
	pasture = 0.0;
	forest = 0.0;
	otheragriculture = 0.0;
	Rank = 0;
	CountyPopulation[0] = '\0';
	_merge[0] = '\0';

}

void AFIDA_Sale::copy
	(const AFIDA_Sale& other)

{
	id = other.id;
	buyer = other.buyer;
	fips = other.fips;
	CRD = other.CRD;
	state_fips = other.state_fips;
	county_fips = other.county_fips;
	acres = other.acres;
	crop_acres = other.crop_acres;
	percent_tillable = other.percent_tillable;
	price = other.price;
	estimated_value = other.estimated_value;
	current_value = other.current_value;
	year = other.year;
	month = other.month;
	AverageWindSpeed = other.AverageWindSpeed;
	high_wind = other.high_wind;
	wind_name = other.wind_name;
	investor = other.investor;
	tillable_ground = other.tillable_ground;
	csr = other.csr;
	csr_missing = other.csr_missing;

	// 2023-02-28 Keep all attributes of new table
	strcpy_s (activitytype, 12, other.activitytype);
	strcpy_s (country, 30, other.country);
	parcelid = other.parcelid;
	activitynumber = other.activitynumber;
	ownerid = other.ownerid;
	uscode = other.uscode;
	countrycode = other.countrycode;
	ppb = other.ppb;
	strcpy_s (ownertype, 25, other.ownertype);
	relationshiptorepresentative = other.relationshiptorepresentative;
	typeofinterest = other.typeofinterest;
	percentofownership = other.percentofownership;
	acquisitionmethod = other.acquisitionmethod;
	debt = other.debt;
	crop = other.crop;
	pasture = other.pasture;
	forest = other.forest;
	otheragriculture = other.otheragriculture;
	Rank = other.Rank;
	strcpy_s (CountyPopulation, 75, other.CountyPopulation);
	strcpy_s (_merge, 30, other._merge);
}

AFIDA_Sale AFIDA_Sale::operator =
	(const AFIDA_Sale& other)

{
	if (this != &other)
		copy(other);

	return (*this);
}

void AFIDA_Sale::describe_foreign
	(const char delimiter,
	dynamic_string& report,
	const bool clear_first) const

{
	// report.add_formatted("\t%d\t%d\t%.6lf\t%.6lf\t%.6lf\t%ld", month, year, acres, crop_acres, percent_tillable, price);
	if (clear_first)
		report.clear ();
	report += delimiter;
	report += id;
	report.add_formatted("%c%05d%c%d", delimiter, fips, delimiter, CRD);
	report.add_formatted("%c%d%c$%.2lf%c$%.2lf%c$%.2lf%c%.6lf%c%.6lf", delimiter, year, delimiter, price, delimiter, estimated_value, delimiter, current_value, delimiter, acres, delimiter, crop_acres);
	report += delimiter;
	report += "\"";
	report += buyer;
	report += "\"";
	report.add_formatted("%c%d%c%d%c%d%c%d", delimiter, tillable_ground, delimiter, high_wind, delimiter, wind_name, delimiter, investor);

	// 2023-03-01 Keep all attributes of new table
	report += delimiter;
	report += "\"";
	report += activitytype;
	report += "\"";
	report += delimiter;
	report += "\"";
	report += country;
	report += "\"";
	report.add_formatted ("%c%ld%c%d%c%ld%c%d%c%d%c%d", delimiter, parcelid, delimiter, (int) activitynumber,
	delimiter, ownerid, delimiter, (int) uscode, delimiter, (int) countrycode, delimiter, (int) ppb);
	report += delimiter;
	report += "\"";
	report += ownertype;
	report += "\"";
	report.add_formatted("%c%d%c%d%c%d%c%ld%c%ld%c%.2lf%c%.2lf%c%.2lf%c%.2lf%c%d", delimiter, (int) relationshiptorepresentative,
	delimiter, (int) typeofinterest, delimiter, (int) percentofownership, delimiter, (int) acquisitionmethod,
	delimiter, debt, delimiter, crop, delimiter, pasture, delimiter, forest, delimiter, otheragriculture, delimiter, (int) Rank);
	report += delimiter;
	report += "\"";
	report += CountyPopulation;
	report += "\"";
	report += delimiter;
	report += "\"";
	report += _merge;
	report += "\"";
}

/*
void AFIDA_Sale::describe_iowa
(dynamic_string& report) const

{
	// report.add_formatted("\t%d\t%d\t%.6lf\t%.6lf\t%.6lf\t%ld", month, year, acres, crop_acres, percent_tillable, price);
	report += "\t";
	report += id;
	report.add_formatted("\t%05d", fips);
	report.add_formatted("\t%d\t$%.2lf\t%.6lf\t%.6lf", year, price, acres, crop_acres);
	report += "\t";
	report += buyer;

	if (!csr_missing)
		report.add_formatted("\t%.4lf", csr);
	else
		report += "\t";

	report.add_formatted("\t%d\t%d", tillable_ground, high_wind);

	report.add_formatted("\t%d", investor);
}
*/

void AFIDA_Sale::describe_domestic
	(const char delimiter,
	dynamic_string& report,
	const bool clear_first) const

{
	// report.add_formatted("\t%d\t%d\t%.6lf\t%.6lf\t%.6lf\t%ld", month, year, acres, crop_acres, percent_tillable, price);
	if (clear_first)
		report.clear();
	report += delimiter;
	report += id;
	report.add_formatted("%c%05d%c%d", delimiter, fips, delimiter, CRD);
	report.add_formatted("%c%d%c$%.2lf%c%.6lf%c%.6lf", delimiter, year, delimiter, price, delimiter, acres, delimiter, crop_acres);

	if (!csr_missing)
		report.add_formatted("%c%.4lf", delimiter, csr);
	else
		report += delimiter;

	report.add_formatted("%c%d%c%d", delimiter, tillable_ground, delimiter, high_wind);

	report.add_formatted("%c%d", delimiter, investor);
}

AFIDA_County::AFIDA_County()

{
	clear ();
}

AFIDA_County::AFIDA_County(const AFIDA_County& other)

{
	copy(other);
}

void AFIDA_County::clear()

{
	fips = state_fips = county_fips = -1;
	foreign.clear();
	domestic.clear();
}

void AFIDA_County::copy
	(const AFIDA_County& other)

{
	fips = other.fips;
	state_fips = other.state_fips;
	county_fips = other.county_fips;
	foreign = other.foreign;
	domestic = other.domestic;
}

AFIDA_County AFIDA_County::operator =
	(const AFIDA_County& other)

{
	if (this != &other)
		copy(other);

	return (*this);
}

bool AFIDA_County::add_foreign_unique
(const AFIDA_Sale& buyer)

// add only if not already there
{
	if (match_foreign(buyer.id) == NULL) {
		foreign.push_back(buyer);
		return true;
	}
	else
		return false;
}

bool AFIDA_County::add_domestic_unique
	(const AFIDA_Sale& buyer)

// add only if not already there
{
	if (match_domestic(buyer.id) == NULL) {
		domestic.push_back(buyer);
		return true;
	}
	else
		return false;
}

AFIDA_Sale* AFIDA_County::match_domestic
	(const dynamic_string& id) const

{
	std::vector <AFIDA_Sale>::const_iterator sale;
	AFIDA_Sale* found = NULL;

	for (sale = domestic.begin();
	!found
	&& (sale != domestic.end());
	++sale) {
		if (sale->id == id)
			found = (AFIDA_Sale*)&*sale;
	}

	return found;
}

AFIDA_Sale* AFIDA_County::match_foreign
	(const dynamic_string& id) const

{
	std::vector <AFIDA_Sale>::const_iterator sale;
	AFIDA_Sale* found = NULL;

	for (sale = foreign.begin();
	!found
	&& (sale != foreign.end());
	++sale) {
		if (sale->id == id)
			found = (AFIDA_Sale*)&*sale;
	}

	return found;
}


bool really_adjacent
	(const int fips_1,
	const int fips_2,
	std::map <int, std::set <int>>* adjacent_counties,
	dynamic_string &log)

// AFIDA_County.foreign and domestic each contain adjacent county matches.
// Even though county A is adjacent to B, and B is adjacent to C, A may not be adjacent to C

{
	std::map <int, std::set <int>>::const_iterator adjacent_county_list;
	std::set <int>::const_iterator adjacent_county;
	bool match = false;

	if (fips_1 == fips_2)
		match = true;
	else
		if ((adjacent_county_list = adjacent_counties->find(fips_1)) != adjacent_counties->end()) {
			for (adjacent_county = adjacent_county_list->second.begin();
			!match
			&& (adjacent_county != adjacent_county_list->second.end());
			++adjacent_county)
				if (*adjacent_county == fips_2)
					match = true;
		}
	// if (!match)
	//	log.add_formatted("%d not really adjacent to %d\n", fips_1, fips_2);
	return match;
}

bool matching_CRD
	(const int fips_1,
	const int fips_2,
	const int CRD_by_county [],
	dynamic_string& log)

// True if counties are in the same Crop Reporting District

// False if either county not found

{
	int district_1, district_2;
	std::map <int, int>::const_iterator district_record;
	bool match = false;

	if (fips_1 == fips_2)
		match = true;
	else
		// 2022-10-12 CRD are only unique within state
		if ((fips_1 % 1000) == (fips_2 % 1000)) {
			if ((district_1 = CRD_by_county [fips_1 - 1]) > 0) {
				if ((district_2 = CRD_by_county [fips_2 - 1]) >= 0) {
					if (district_1 == district_2)
						match = true;
				}
				else
					log.add_formatted("Error, no CRD found for FIPS %d\n", fips_2);
			}
			else
				log.add_formatted("Error, no CRD found for FIPS %d\n", fips_1);
		}

	return match;
}

bool AFIDA_County::match_sales_pythagorean
	(const int limit,
	const class AFIDA* reference,
	const int dimension_count,
	std::map <int, std::set <int>>* adjacent_counties,
	const char delimiter,
	dynamic_string &report,
	dynamic_string &log)

// finding the most similar sales for each foreigner purchasing sale. The rules for each foreigner purchase sale are:
// 1. in the previous year in the same or the neighboring counties;
// 2. most similar at percent tillable(crop acres / total acres) and total acres;
// 3. keep the most similar 4 sales from Iowa or Kansas ag land sales data.

// Returns true if any matching the correct year (or previous year) are found

{
	std::tuple <double, double, double, AFIDA_Sale, AFIDA_Sale> quad;
	std::pair <AFIDA_Sale, AFIDA_Sale> pair;
	std::vector <AFIDA_Sale>::const_iterator buy_1, buy_2;
	std::vector <std::pair <AFIDA_Sale, AFIDA_Sale>> exact_all;
	std::vector <std::pair <AFIDA_Sale, AFIDA_Sale>>::const_iterator exact;
	std::map <double, std::tuple <double, double, double, AFIDA_Sale, AFIDA_Sale>> matches_by_difference;
	std::map <double, std::tuple <double, double, double, AFIDA_Sale, AFIDA_Sale>>::const_iterator match;
	double diff_acres, diff_tillable, diff_wind, pythagorean, normalized_1, normalized_2;
	int row_count;
	bool exact_match;

	for (buy_1 = foreign.begin();
	buy_1 != foreign.end();
	++buy_1)
		for (buy_2 = domestic.begin();
		buy_2 != domestic.end();
		++buy_2) {
			// 2022-04-21 Wendong: foreign 2009 should match domestic 2009 or 2008
			if ((buy_1->year == (buy_2->year + 1))
			|| (buy_1->year == buy_2->year)) {

				if (really_adjacent (buy_1->fips, buy_2->fips, adjacent_counties, log)) {

					/*
					if (buy_2->acres > 0.0)
						diff_acres = std::abs((buy_1->acres - buy_2->acres) / buy_2->acres);
					else
						diff_acres = 0.0;
					*/
					// Since the 2 dimensions are in differing units, use ((value - min) / range) instead of value
					normalized_1 = (buy_1->acres - reference->min_acres) / reference->range_acres;
					normalized_2 = (buy_2->acres - reference->min_acres) / reference->range_acres;
					diff_acres = normalized_1 - normalized_2;

					/*
					if (buy_2->percent_tillable > 0.0)
						diff_tillable = std::abs((buy_1->percent_tillable - buy_2->percent_tillable) / buy_2->percent_tillable);
					else
						diff_tillable = 0.0;
					*/
					normalized_1 = (buy_1->percent_tillable - reference->min_percent_tillable) / reference->range_percent_tillable;
					normalized_2 = (buy_2->percent_tillable - reference->min_percent_tillable) / reference->range_percent_tillable;
					diff_tillable = normalized_1 - normalized_2;

					// wind_value is just 1 or 0
					diff_wind = buy_1->high_wind - buy_2->high_wind;

					if (dimension_count == 2)
						exact_match = (diff_acres == 0) && (diff_tillable == 0);
					else
						exact_match = (diff_acres == 0) && (diff_tillable == 0) && (diff_wind == 0);

					if (exact_match) {
						pair.first = *buy_1;
						pair.second = *buy_2;
						exact_all.push_back(pair);
					}
					else {
						/*
						// For pythagorean distance, if denominator (buy_2) was 0, use buy_1 value
						distance_acres = buy_2->acres - buy_1->acres;
						distance_tillable = buy_2->percent_tillable - buy_1->percent_tillable;
						pythagorean = sqrt(distance_acres * distance_acres + distance_tillable * distance_tillable);
						*/
						if (dimension_count == 3)
							pythagorean = sqrt(diff_acres * diff_acres + diff_tillable * diff_tillable + diff_wind * diff_wind);
						else
							pythagorean = sqrt(diff_acres * diff_acres + diff_tillable * diff_tillable);

						quad = std::make_tuple(diff_acres, diff_tillable, diff_wind, *buy_1, *buy_2);
						matches_by_difference.insert(std::pair <double, std::tuple <double, double, double, AFIDA_Sale, AFIDA_Sale>>(pythagorean, quad));
					}
				}
			}
		}

	report.clear();
	for (exact = exact_all.begin();
	exact != exact_all.end();
	++exact) {
		report.add_formatted("0.0%c%c", delimiter, delimiter, delimiter);
		if (dimension_count == 3)
			report += delimiter;
		exact->first.describe_foreign (delimiter, report);
//		exact->second.describe_domestic (report);
		report += "\n";
	}
	for (match = matches_by_difference.begin(), row_count = 0;
	(match != matches_by_difference.end())
	&& (row_count < limit);
	++match, ++row_count) {

		/*
		if (std::get <2>(match->second).county_fips != std::get <3>(match->second).county_fips)
			log.add_formatted ("Adjacent County\t%d\t%d.\n", std::get <2>(match->second).state_fips * 1000 + std::get <2>(match->second).county_fips,
			std::get <3>(match->second).state_fips * 1000 + std::get <3>(match->second).county_fips);
		*/

		report.add_formatted("%.6lf", match->first);

		// diff acres
		report.add_formatted("%c%.6lf", delimiter, std::get <0>(match->second));

		// diff percent tillable
		report.add_formatted("%c%.6lf", delimiter, std::get <1>(match->second));

		if (dimension_count == 3)
			// diff wind
			report.add_formatted("%c%.6lf", delimiter, std::get <2>(match->second));

		std::get <3>(match->second).describe_foreign (delimiter, report);
//		std::get <4>(match->second).describe_domestic (report);
		report += "\n";
	}

	if ((exact_all.size() > 0)
	|| (matches_by_difference.size() > 0))
		return true;
	else
		return false;
}

bool AFIDA_County::match_sales_weighted
	(const dynamic_string &filename_output,
	const int limit,
	const class AFIDA* reference,
	std::map <int, std::set <int>>* adjacent_counties,
	const int* CRD_by_county,
	const char delimiter,
	interface_window* view,
	dynamic_string& log)

// (1).select neighboring county and current and prior year in state ag sales
// (2).metric = 35 % *acre_diff + 20 % wind + 35 % tillable_dummy + 10 % investor_dummy for foreign owner without wind in name

{
	std::tuple <double, double, double, double, AFIDA_Sale, AFIDA_Sale> quad;
	std::pair <AFIDA_Sale, AFIDA_Sale> pair;
	std::vector <AFIDA_Sale>::const_iterator buy_1, buy_2;
	std::vector <std::pair <AFIDA_Sale, AFIDA_Sale>> exact_all;
	std::vector <std::pair <AFIDA_Sale, AFIDA_Sale>>::const_iterator exact;
	std::map <double, std::tuple <double, double, double, double, AFIDA_Sale, AFIDA_Sale>> matches_by_difference;
	std::map <double, std::tuple <double, double, double, double, AFIDA_Sale, AFIDA_Sale>>::const_iterator match;
	double diff_acres, diff_tillable, diff_high_wind, diff_investor, score;
	int row_count;
	bool exact_match, error = false;
	AFIDA_Sale *foreign_sale, *domestic_sale;

	for (buy_1 = foreign.begin();
	buy_1 != foreign.end();
	++buy_1) {
		for (buy_2 = domestic.begin();
		buy_2 != domestic.end();
		++buy_2) {
			// 2022-04-21 Wendong: foreign 2009 should match domestic 2009 or 2008
			if ((buy_1->year == (buy_2->year + 1))
			|| (buy_1->year == buy_2->year)) {

				if (matching_CRD(buy_1->fips, buy_2->fips, CRD_by_county, log)
				|| really_adjacent(buy_1->fips, buy_2->fips, adjacent_counties, log)) {

					diff_acres = std::abs(buy_1->acres - buy_2->acres);
					diff_high_wind = std::abs(buy_1->high_wind - buy_2->high_wind);
					diff_tillable = std::abs(buy_1->tillable_ground - buy_2->tillable_ground);
					diff_investor = std::abs (buy_1->investor - buy_2->investor);

					exact_match = (diff_acres == 0.0) && (diff_tillable == 0.0) && (diff_high_wind == 0.0) && (diff_investor == 0.0);

					if (exact_match) {
						pair.first = *buy_1;
						pair.second = *buy_2;
						exact_all.push_back(pair);
					}
					else {

						if (buy_2->wind_name)
							// AFIDA record has "wind" in name
							score = 0.35 * diff_acres + 0.20 * diff_high_wind + 0.35 * diff_tillable + 0.10 * diff_investor;
						else
							score = 0.30 * diff_acres + 0.30 * diff_high_wind + 0.30 * diff_tillable + 0.10 * diff_investor;

						quad = std::make_tuple (diff_acres, diff_tillable, diff_high_wind, diff_investor, *buy_1, *buy_2);
						matches_by_difference.insert(std::pair <double, std::tuple <double, double, double, double, AFIDA_Sale, AFIDA_Sale>>(score, quad));
					}
				}
				/*
				else {
					if (buy_1->id == "AFIDA_IA004") {
						log += "Domestic match for ID=";
						log += buy_1->id;
						log.add_formatted(" year=%d FIPS=%02d%03d, ID=", buy_1->year, buy_1->state_fips, buy_1->county_fips);
						log += buy_2->id;
						log.add_formatted(" year=%d FIPS=%02d%03d, not really adjacent.\n", buy_2->year, buy_2->state_fips, buy_2->county_fips);
					}
				}
				*/
			}
			/*
			else {
				if (buy_1->id == "AFIDA_IA004") {
					log += "Domestic match for ID=";
					log += buy_1->id;
					log.add_formatted(" year=%d FIPS=%02d%03d, ID=", buy_1->year, buy_1->state_fips, buy_1->county_fips);
					log += buy_2->id;
					log.add_formatted(" year=%d FIPS=%02d%03d, year does not match.\n", buy_2->year, buy_2->state_fips, buy_2->county_fips);
				}
			}
			*/
		}
	}

	if ((exact_all.size () > 0)
	|| (matches_by_difference.size () > 0)) {
		FILE *f;
		dynamic_string inconstant_filename = filename_output, report_text;

		if (fopen_s (&f, inconstant_filename.get_text_ascii (), "w") == 0) {
			for (exact = exact_all.begin();
			exact != exact_all.end();
			++exact) {
				fprintf (f, "%d", state_fips);
				fprintf (f, "%c0.0%c%c%c%c", delimiter, delimiter, delimiter, delimiter, delimiter);
				exact->first.describe_foreign (delimiter, report_text, true);
				fwrite (report_text.get_text_ascii (), report_text.get_length (), 1, f);
				exact->second.describe_domestic (delimiter, report_text, true);
				fwrite (report_text.get_text_ascii(), report_text.get_length(), 1, f);
				// fprintf (f, "%c%d", delimiter, fips);
				fprintf (f, "\n");
			}
			for (match = matches_by_difference.begin(), row_count = 0;
			match != matches_by_difference.end();
			// && (row_count < limit);
			++match, ++row_count) {

				/*
				if (std::get <2>(match->second).county_fips != std::get <3>(match->second).county_fips)
					log.add_formatted ("Adjacent County\t%d\t%d.\n", std::get <2>(match->second).state_fips * 1000 + std::get <2>(match->second).county_fips,
					std::get <3>(match->second).state_fips * 1000 + std::get <3>(match->second).county_fips);
				*/

				fprintf(f, "%d", state_fips);

				fprintf (f, "%c%.6lf", delimiter, match->first);

				// diff acres
				fprintf(f, "%c%.6lf", delimiter, std::get <0>(match->second));

				// diff percent tillable
				fprintf(f, "%c%.6lf", delimiter, std::get <1>(match->second));

				// diff wind
				fprintf(f, "%c%.6lf", delimiter, std::get <2>(match->second));

				// diff investor
				fprintf(f, "%c%.6lf", delimiter, std::get <3>(match->second));

				foreign_sale = (AFIDA_Sale *) & std::get <4>(match->second);
				domestic_sale = (AFIDA_Sale *) &std::get <5> (match->second);

				foreign_sale->describe_foreign(delimiter, report_text, true);
				fwrite(report_text.get_text_ascii(), report_text.get_length(), 1, f);
				domestic_sale->describe_domestic(delimiter, report_text, true);
				fwrite(report_text.get_text_ascii(), report_text.get_length(), 1, f);

				if (foreign_sale->county_fips != domestic_sale->county_fips) {
					if (really_adjacent(foreign_sale->fips, domestic_sale->fips, adjacent_counties, log))
						fprintf (f, "%cAdjacent", delimiter);
					else
						if (matching_CRD(foreign_sale->fips, domestic_sale->fips, CRD_by_county, log))
							fprintf (f, "%cCRD", delimiter);

				}
				// fprintf(f, "%c%d", delimiter, fips);
				fprintf (f, "\n");

				if (view
				&& (row_count % 500 == 0))
					view->update_progress_formatted (2, "Row %d", row_count);
			}
			fclose (f);
		}
		else {
			log += "ERROR, can't open output filename \"";
			log += filename_output;
			log += "\".\n";
			error = true;
		}
	}
	else
		error = true;

	return !error;
}

/*
bool AFIDA_County::match_sales_weighted_kansas
	(const int limit,
	const class AFIDA* reference,
	std::map <int, std::set <int>>* adjacent_counties,
	std::map <int, int>* CRD_by_county,
	dynamic_string& report,
	interface_window* view,
	dynamic_string& log)

	// (1).select neighboring countyand currentand prior year in Kansas ag sales
	// (2).metric = 35 % *acre_diff + 20 % wind + 35 % tillable_dummy + 10 % investor_dummy for foreign owner without wind in name

{
	std::tuple <double, double, double, double, AFIDA_Sale, AFIDA_Sale> quad;
	std::pair <AFIDA_Sale, AFIDA_Sale> pair;
	std::vector <AFIDA_Sale>::const_iterator buy_1, buy_2;
	std::vector <std::pair <AFIDA_Sale, AFIDA_Sale>> exact_all;
	std::vector <std::pair <AFIDA_Sale, AFIDA_Sale>>::const_iterator exact;
	std::map <double, std::tuple <double, double, double, double, AFIDA_Sale, AFIDA_Sale>> matches_by_difference;
	std::map <double, std::tuple <double, double, double, double, AFIDA_Sale, AFIDA_Sale>>::const_iterator match;
	double diff_acres, diff_tillable, diff_high_wind, diff_investor, score;
	int row_count;
	bool exact_match;
	AFIDA_Sale *foreign_sale, *domestic_sale;

	for (buy_1 = foreign.begin();
	buy_1 != foreign.end();
	++buy_1) {
		for (buy_2 = domestic.begin();
		buy_2 != domestic.end();
		++buy_2) {
			// 2022-04-21 Wendong: foreign 2009 should match domestic 2009 or 2008
			if ((buy_1->year == (buy_2->year + 1))
			|| (buy_1->year == buy_2->year)) {

				if (matching_CRD(buy_1->state_fips, buy_1->county_fips, buy_2->state_fips, buy_2->county_fips, CRD_by_county, log)
				|| really_adjacent(buy_1->state_fips, buy_1->county_fips, buy_2->state_fips, buy_2->county_fips, adjacent_counties, log)) {

					diff_acres = std::abs(buy_1->acres - buy_2->acres);
					diff_high_wind = std::abs(buy_1->high_wind - buy_2->high_wind);
					diff_tillable = std::abs(buy_1->tillable_ground - buy_2->tillable_ground);
					diff_investor = std::abs(buy_1->investor - buy_2->investor);

					exact_match = (diff_acres == 0.0) && (diff_tillable == 0.0) && (diff_high_wind == 0.0) && (diff_investor == 0.0);

					if (exact_match) {
						pair.first = *buy_1;
						pair.second = *buy_2;
						exact_all.push_back(pair);
					}
					else {

						if (buy_2->wind_name)
							// AFIDA record has "wind" in name
							score = 0.35 * diff_acres + 0.20 * diff_high_wind + 0.35 * diff_tillable + 0.10 * diff_investor;
						else
							score = 0.30 * diff_acres + 0.30 * diff_high_wind + 0.30 * diff_tillable + 0.10 * diff_investor;

						quad = std::make_tuple(diff_acres, diff_tillable, diff_high_wind, diff_investor, *buy_1, *buy_2);
						matches_by_difference.insert(std::pair <double, std::tuple <double, double, double, double, AFIDA_Sale, AFIDA_Sale>>(score, quad));
					}
				}
			}
		}
	}

	report.clear();
	for (exact = exact_all.begin();
	exact != exact_all.end();
	++exact) {
		report += "0.0\t\t\t\t";
		exact->first.describe_foreign(report);
		exact->second.describe_kansas(report);
		report += "\n";
	}
	for (match = matches_by_difference.begin(), row_count = 0;
	match != matches_by_difference.end();
	// && (row_count < limit);
	++match, ++row_count) {

		report.add_formatted("%.6lf", match->first);

		// diff acres
		report.add_formatted("\t%.6lf", std::get <0>(match->second));

		// diff percent tillable
		report.add_formatted("\t%.6lf", std::get <1>(match->second));

		// diff wind
		report.add_formatted("\t%.6lf", std::get <2>(match->second));

		// diff investor
		report.add_formatted("\t%.6lf", std::get <3>(match->second));

		foreign_sale = (AFIDA_Sale*)&std::get <4>(match->second);
		domestic_sale = (AFIDA_Sale*)&std::get <5>(match->second);

		foreign_sale->describe_foreign(report);
		domestic_sale->describe_kansas(report);

		if (foreign_sale->county_fips != domestic_sale->county_fips) {
			if (really_adjacent(foreign_sale->state_fips, foreign_sale->county_fips, domestic_sale->state_fips, domestic_sale->county_fips, adjacent_counties, log))
				report += "\tAdjacent";
			else
				if (matching_CRD(foreign_sale->state_fips, foreign_sale->county_fips, domestic_sale->state_fips, domestic_sale->county_fips, CRD_by_county, log))
					report += "\tCRD";

		}
		report += "\n";
		if (view
		&& (row_count % 500 == 0))
			view->update_progress_formatted(2, "Row %d", row_count);
	}

	if ((exact_all.size() > 0)
	|| (matches_by_difference.size() > 0))
		return true;
	else
		return false;
}
*/

AFIDA::AFIDA()

{
	min_acres = DBL_MAX;
	max_acres = -DBL_MAX;
	range_acres = 0.0;
	min_percent_tillable = DBL_MAX;
	max_percent_tillable = -DBL_MAX;
	range_percent_tillable = 0.0;

	foreign_id_index = domestic_id_index = -1;
}

bool AFIDA::read
	(const dynamic_string& filename_afida,
	const int state_fips,
	CountyMaster *counties,
	const int* CRD_by_county,
	dynamic_string& log)

{
	FILE* file_csv;
	char data[1024];
	std::vector <dynamic_string> tokens;
	dynamic_string inconstant_filename, parse_data;
	bool error = false;
	int count_read = 0, count_valid = 0; //, count_ignored = 0;
	AFIDA_Sale sale;

	// set_wind_counties(&high_wind_by_fips);

	inconstant_filename = filename_afida;
	if (fopen_s(&file_csv, inconstant_filename.get_text_ascii(), "r") == 0) {

		// ID_AFIDA	state	county	owner	country	background	primary_place_of_business	acres	type	ownership_percentage	purchase_price	estimated_value	current_value	purchase_month	purchase_year	crop_acres	pasture_acres	forest_acres	other_ag_acres	other_non_ag_acres	use	sale_year	sale_month	tillable_pct	tillable	wind_dummy
		//	AFIDA_IA001	Iowa	Hancock	Willow Creek Wind, LLC	CANADA	not US or not corp	UNITED STATES	6110.59	LLCS and other	100	0	22657759.72	22657759.72	12	2014	5450.65	439.96	0	0	219.98	no change	2014	12	0.89200062	1	1
		//	AFIDA_IA002	Iowa	Cerro Gordo	Willow Creek Wind, LLC	CANADA	not US or not corp	UNITED STATES	1396.99	LLCS and other	100	0	4849004.14	4849004.14	12	2014	1246.12	100.58	0	0	50.29	no change	2014	12	0.89200354	1	1

		if (fgets(data, 1024, file_csv)) {

			while (!error
			&& fgets(data, 1024, file_csv)) {
				++count_read;
				parse_data = data;
				parse_data.tokenize(",\r\n", &tokens, true, false);

				if (tokens.size() >= 26) {
					sale.clear();
					sale.id = tokens [0];

					if ((sale.state_fips = state_FIPS_from_name(tokens[1].get_text_ascii())) > 0) {
						correct_county_name (tokens [2], sale.state_fips);
						if ((sale.county_fips = counties->get_FIPS(sale.state_fips, tokens[2].get_text_ascii())) >= 1) {

							sale.fips = sale.state_fips * 1000 + sale.county_fips;

							if (sale.fips > 0)
								sale.CRD = CRD_by_county[sale.fips - 1];

							sale.acres = atof(tokens[7].get_text_ascii());

							if (tokens [8].equals_insensitive ("individual") == false)
								// For AFIDA, there is a variable called type, and investor = 1 if type is not individual (type != indiivdual)
								// - this means that they are corps, LLCs, trust etc.
								sale.investor = 1;

							if (tokens[10].get_length() > 0)
								// purchase_price
								sale.price = atof(tokens[10].get_text_ascii());

							if (tokens[11].get_length() > 0)
								// estimated_value
								sale.estimated_value = atof(tokens[11].get_text_ascii());

							if (tokens[12].get_length() > 0)
								// current_value
								sale.current_value = atof(tokens[12].get_text_ascii());

							sale.month = atol(tokens[13].get_text_ascii());
							sale.year = atol(tokens[14].get_text_ascii());
							sale.crop_acres = atof(tokens[15].get_text_ascii());

							// (1). tillable_pct: between 0 and 1. This equals to cropland_acres/acres and measures the percent of tillable land.
							sale.percent_tillable = atof (tokens [23].get_text_ascii ());
							// (2).tillable: dummy variable. 1 for variable tillable_pct is above 0.6 and 0 for the rest.
							sale.tillable_ground = (tokens[24] == "1") ? 1 : 0;
							// (3). wind_dummy: dummy variable. 1 for the name of foreign owner with “wind”. 0 for the rest.
							sale.wind_name = (tokens[25] == "1") ? 1 : 0;

							if (high_wind_by_fips.find (sale.state_fips * 1000 + sale.county_fips) != high_wind_by_fips.end ())
								sale.high_wind = 1;

							// 10% investor_dummy for foreign owner without wind in name
							sale.buyer = tokens [3];
							if (tokens [3].match_insensitive ("wind", 0) != -1)
								sale.investor = 1;

							foreign.push_back(sale);
							++count_valid;
							// else
							//	++count_ignored;
						}
						else {
							log += "ERROR, can't match county name \"";
							log += tokens[2];
							log += "\" state \"";
							log += tokens [0];
							log += "\" in AFIDA file \"";
							log += filename_afida;
							log += "\"\n";
							error = true;
						}
					}
					else {
						log += "ERROR, can't match state name \"";
						log += tokens[0];
						log += "\" in AFIDA file \"";
						log += filename_afida;
						log += "\"\n";
						error = true;
					}
				}
				else {
					log.add_formatted("ERROR, can't parse AFIDA file (%d tokens) \"", tokens.size());
					log += filename_afida;
					log += "\"\n";

					log += "\"";
					log += data;
					log += "\"\n";
					error = true;
				}
			}
		}

		fclose(file_csv);

		log.add_formatted("Records read\t%d\n", count_read);
		log.add_formatted("Valid records read\t%d\n", count_valid);
		// log.add_formatted("Records from ignored state\t%d\n", count_ignored);
	}
	else {
		log += "ERROR, can't open AFIDA file \"";
		log += filename_afida;
		log += "\"\n";
		error = true;
	}
	return !error;
}

bool AFIDA::read_2023_02_24
	(const dynamic_string& filename_afida,
	const int state_fips,
	CountyMaster* counties,
	const int* CRD_by_county,
	dynamic_string& log)

// 2023-02-24 New columns from Wendong "AFIDA_Database_update_Kansas_2023.csv"

{
	FILE* file_csv;
	char data[1024];
	std::vector <dynamic_string> tokens;
	dynamic_string inconstant_filename, parse_data;
	bool error = false;
	int count_read = 0, count_valid = 0; //, count_ignored = 0;
	AFIDA_Sale sale;

	// set_wind_counties(&high_wind_by_fips);

	inconstant_filename = filename_afida;
	if (fopen_s(&file_csv, inconstant_filename.get_text_ascii(), "r") == 0) {

		// ID_AFIDA	state	county	owner	country	background	primary_place_of_business	acres	type	ownership_percentage	purchase_price	estimated_value	current_value	purchase_month	purchase_year	crop_acres	pasture_acres	forest_acres	other_ag_acres	other_non_ag_acres	use	sale_year	sale_month	tillable_pct	tillable	wind_dummy
		//	AFIDA_IA001	Iowa	Hancock	Willow Creek Wind, LLC	CANADA	not US or not corp	UNITED STATES	6110.59	LLCS and other	100	0	22657759.72	22657759.72	12	2014	5450.65	439.96	0	0	219.98	no change	2014	12	0.89200062	1	1
		//	AFIDA_IA002	Iowa	Cerro Gordo	Willow Creek Wind, LLC	CANADA	not US or not corp	UNITED STATES	1396.99	LLCS and other	100	0	4849004.14	4849004.14	12	2014	1246.12	100.58	0	0	50.29	no change	2014	12	0.89200354	1	1

		// AFIDA_ID	state	county	ownername	activitytype	fips	country	parcelid	activitynumber	ownerid	uscode	countrycode	ppb	acres	ownertype	relationshiptorepresentative	typeofinterest	percentofownership	acquisitionmethod	purchaseprice	estimatedvalue	currentvalue	debt	acquisitionmonth	acquisitionyear	crop	pasture	forest	otheragriculture	Rank	AverageWindSpeed	CountyPopulation	_merge	high_wind	tillable_pct	tillable
		// AFIDA_001	Kansas	Anderson	DEER CREEK WIND PROJECT, L.L.C.Holding	20003	ITALY	34964	0	26925	0	380	840	160	Other	2	6	100	6	0	107000	107000	0	9	2008	0	71	0	89	93	16.47	Anderson, KS / 7, 962	Matched(3)	0	0	0
		// AFIDA_002	Kansas	Anderson	DEER CREEK WIND PROJECT, L.L.C.Holding	20003	ITALY	35028	0	26925	0	380	840	660	Other	2	6	100	6	0	701000	701000	0	10	2008	651	0	0	9	93	16.47	Anderson, KS / 7, 962	Matched(3)	0	0.98636365	1

		if (fgets(data, 1024, file_csv)) {

			while (!error
				&& fgets(data, 1024, file_csv)) {
				++count_read;
				parse_data = data;
				parse_data.tokenize(",\r\n", &tokens, true, false);

				if (tokens.size() >= 26) {
					sale.clear();
					sale.id = tokens[0];

					if ((sale.state_fips = state_FIPS_from_name(tokens[1].get_text_ascii())) > 0) {
						correct_county_name(tokens[2], sale.state_fips);
						if ((sale.county_fips = counties->get_FIPS(sale.state_fips, tokens[2].get_text_ascii())) >= 1) {

							sale.fips = sale.state_fips * 1000 + sale.county_fips;

							if (sale.fips > 0)
								sale.CRD = CRD_by_county[sale.fips - 1];

							sale.buyer = tokens[3];
							sale.acres = atof(tokens[13].get_text_ascii());

							if (tokens[14].equals_insensitive("individual") == false)
								// For AFIDA, there is a variable called type, and investor = 1 if type is not individual (type != indiivdual)
								// - this means that they are corps, LLCs, trust etc.
								sale.investor = 1;

							if (tokens[19].get_length() > 0)
								// purchase_price
								sale.price = atof(tokens[19].get_text_ascii());

							if (tokens[20].get_length() > 0)
								// estimated_value
								sale.estimated_value = atof(tokens[20].get_text_ascii());

							if (tokens[21].get_length() > 0)
								// current_value
								sale.current_value = atof(tokens[21].get_text_ascii());

							sale.month = atol(tokens[23].get_text_ascii());
							sale.year = atol(tokens[24].get_text_ascii());
							sale.crop_acres = atof(tokens[25].get_text_ascii());

							// (1). tillable_pct: between 0 and 1. This equals to cropland_acres/acres and measures the percent of tillable land.
							sale.percent_tillable = atof(tokens[34].get_text_ascii());
							// (2).tillable: dummy variable. 1 for variable tillable_pct is above 0.6 and 0 for the rest.
							sale.tillable_ground = (tokens[35] == "1") ? 1 : 0;
							// (3). wind_dummy: dummy variable. 1 for the name of foreign owner with “wind”. 0 for the rest.
							if (tokens [3].match_insensitive ("wind") != -1)
								sale.wind_name = (tokens[25] == "1") ? 1 : 0;

							if (high_wind_by_fips.find(sale.state_fips * 1000 + sale.county_fips) != high_wind_by_fips.end())
								sale.high_wind = 1;

							// 10% investor_dummy for foreign owner without wind in name
							// 2023-02-27: not present in Kansas AFIDA file anyway
							if (tokens[3].match_insensitive("wind") == -1)
								sale.investor = 1;

							sale.AverageWindSpeed = atof(tokens [30].get_text_ascii ());

							// 2023-02-28 Keep all attributes of new table
							strcpy_s(sale.activitytype, 12, tokens [4].get_text_ascii ());
							if (tokens[6].get_length() > 0)
								strcpy_s(sale.country, 30, tokens[6].get_text_ascii());
							sale.parcelid = atol (tokens [7].get_text_ascii ());
							sale.activitynumber = (BYTE) atoi (tokens [8].get_text_ascii ());
							sale.ownerid = atol(tokens[9].get_text_ascii());
							sale.uscode = (BYTE) atoi(tokens[10].get_text_ascii());
							sale.countrycode = (short) atoi (tokens [11].get_text_ascii ());
							sale.ppb = (short)atoi(tokens[12].get_text_ascii());
							strcpy_s(sale.ownertype, 25, tokens [14].get_text_ascii ());
							sale.relationshiptorepresentative = (BYTE)atoi(tokens[15].get_text_ascii());;
							sale.typeofinterest = (BYTE)atoi(tokens[16].get_text_ascii());;
							sale.percentofownership = (short) atoi(tokens[17].get_text_ascii());;
							if (tokens[18].get_length() > 0)
								sale.acquisitionmethod = (BYTE) atoi(tokens[18].get_text_ascii());;
							if (tokens[22].get_length() > 0)
								sale.debt = atol (tokens [22].get_text_ascii ());
							if (tokens[25].get_length() > 0)
								sale.crop = atof(tokens[25].get_text_ascii());
							if (tokens[26].get_length() > 0)
								sale.pasture = atof(tokens[26].get_text_ascii());
							if (tokens[27].get_length() > 0)
								sale.forest = atof(tokens[27].get_text_ascii());
							if (tokens[28].get_length() > 0)
								sale.otheragriculture = atof(tokens[28].get_text_ascii());
							sale.Rank = (short) atoi(tokens[29].get_text_ascii());
							strcpy_s(sale.CountyPopulation, 75, tokens [31].get_text_ascii ());
							strcpy_s(sale._merge, 30, tokens[32].get_text_ascii());

							foreign.push_back(sale);
							++count_valid;
							// else
							//	++count_ignored;
						}
						else {
							log += "ERROR, can't match county name \"";
							log += tokens[2];
							log += "\" state \"";
							log += tokens[0];
							log += "\" in AFIDA file \"";
							log += filename_afida;
							log += "\"\n";
							error = true;
						}
					}
					else {
						log += "ERROR, can't match state name \"";
						log += tokens[0];
						log += "\" in AFIDA file \"";
						log += filename_afida;
						log += "\"\n";
						error = true;
					}
				}
				else {
					log.add_formatted("ERROR, can't parse AFIDA file (%d tokens) \"", tokens.size());
					log += filename_afida;
					log += "\"\n";

					log += "\"";
					log += data;
					log += "\"\n";
					error = true;
				}
			}
		}

		fclose(file_csv);

		log.add_formatted("Records read\t%d\n", count_read);
		log.add_formatted("Valid records read\t%d\n", count_valid);
		// log.add_formatted("Records from ignored state\t%d\n", count_ignored);
	}
	else {
		log += "ERROR, can't open AFIDA file \"";
		log += filename_afida;
		log += "\"\n";
		error = true;
	}
	return !error;
}

void correct_kansas_county_name
	(dynamic_string& name)

// 2022-04-15 I:\TDC\AFIDA\Kansas_Analysis_Data.csv

{
	if (name == "Atchinson")
		name = "Atchison";
	else
		if (name == "Chautuaqua")
			name = "Chautauqua";
		else
			if (name == "Neosha")
				name = "Neosho";
			else
				if (name == "Greenword")
					name = "Greenwood";
}

bool AFIDA::read_Kansas
	(const dynamic_string& filename_afida,
	CountyMaster* counties,
	const int* CRD_by_county,
	dynamic_string& log)

// 2022-04-14 requested original version of Iowa/Kansas sales data.
// New Kansas file with 15 columns provided.

{
	FILE* file_csv;
	char data[1024];
	std::vector <dynamic_string> tokens;
	dynamic_string inconstant_filename, parse_data;
	bool error = false;
	int count_read = 0, count_valid = 0;
	AFIDA_Sale sale;

	inconstant_filename = filename_afida;
	if (fopen_s(&file_csv, inconstant_filename.get_text_ascii(), "r") == 0) {

		// id	state	county	acres	parcel_price	cropland_acres	pasture_acres	year	priceperacre	afida	crop_percent	pasture_percent	parcel_sq	log_priceper_acre	log_acres	afida_size	Rank	AverageWindSpeed	CountyPopulation	_merge	high_wind	tillable_pct	tillable
		//	13572	Kansas	Allen	151	65000	71	80	1984	432	0	0.47019866	0.52980131	22801	6.0684257	5.0172796	0	83	17.22	Allen, KS / 13, 212	Matched(3)	0	0.47019866	0
		//	2859	Kansas	Allen	160	64000	70	90	1973	400	0	0.4375	0.5625	25600	5.9914646	5.0751739	0	83	17.22	Allen, KS / 13, 212	Matched(3)	0	0.4375	0

		// id	state	county	acres	parcel_price	cropland_acres	pasture_acres	year	priceperacre	afida	crop_percent	pasture_percent	parcel_sq	log_priceper_acre	log_acres	afida_size	Rank	AverageWindSpeed	CountyPopulation	_merge	high_wind	tillable_pct	tillable


		if (fgets(data, 1024, file_csv)) {

			while (!error
				&& fgets(data, 1024, file_csv)) {
				++count_read;
				parse_data = data;
				parse_data.tokenize(",\r\n", &tokens, true, false);

				if (tokens.size() == 23) {
					sale.clear();

					sale.id = tokens [0];

					if ((sale.state_fips = state_FIPS_from_name(tokens[1].get_text_ascii())) > 0) {
						correct_kansas_county_name(tokens[2]);

						sale.county_fips = counties->get_FIPS(sale.state_fips, tokens[2].get_text_ascii());

						sale.fips = sale.state_fips * 1000 + sale.county_fips;

						if (sale.fips > 0)
							sale.CRD = CRD_by_county[sale.fips - 1];

						if (sale.county_fips != -1) {
							sale.acres = atof(tokens[3].get_text_ascii());
							sale.price = atof(tokens[4].get_text_ascii());
							sale.crop_acres = atof(tokens[5].get_text_ascii());

							sale.year = atoi(tokens[7].get_text_ascii());
							sale.AverageWindSpeed = atof(tokens[17].get_text_ascii()); // AverageWindSpeed
							sale.high_wind = (tokens[20] == "1") ? 1 : 0; // high_wind
							sale.percent_tillable = atof (tokens [21].get_text_ascii ()); // tillable_pct
							sale.tillable_ground = (tokens[22] == "1") ? 1 : 0; // tillable

							domestic_all_states.push_back(sale);
							++count_valid;
						}
						else {
							log += "Can't match county name \"";
							log += tokens[2];
							log += "\", State \"";
							log += tokens[1];
							log += "\" in Kansas file \"";
							log += filename_afida;
							log.add_formatted ("\" row %d.\n", count_read);
						}
					}
					else {
						log += "ERROR, can't match state name \"";
						log += tokens[1];
						log += "\" in Kansas file \"";
						log += filename_afida;
						log.add_formatted("\" row %d.\n", count_read);
						error = true;
					}
				}
				else {
					log.add_formatted("ERROR, can't parse Kansas file (%d tokens) \"", tokens.size());
					log += filename_afida;
					log += "\"\n";

					log += "\"";
					log += data;
					log.add_formatted("\" row %d.\n", count_read);
					error = true;
				}
			}
		}

		fclose(file_csv);

		log.add_formatted("Kansas Records read\t%d\n", count_read);
		log.add_formatted("Kansas Valid records read\t%d\n", count_valid);
	}
	else {
		log += "ERROR, can't open Kansas file \"";
		log += filename_afida;
		log += "\"\n";
		error = true;
	}
	return !error;
}

bool AFIDA::read_FarmlandFinder_2022_09_22
	(const dynamic_string& filename_afida,
	CountyMaster* counties,
	const int* CRD_by_county,
	dynamic_string& log)

// 2022-09-22 

{
	FILE* file_csv;
	char data[2048];
	std::vector <dynamic_string> tokens;
	dynamic_string inconstant_filename, parse_data;
	bool error = false;
	int count_read = 0, count_valid = 0;
	AFIDA_Sale sale;

	inconstant_filename = filename_afida;
	if (fopen_s(&file_csv, inconstant_filename.get_text_ascii(), "r") == 0) {

		// id_text	farmlandfi	auction	district	state	state_fips	county	county_fips	latitude_x	longitude_x	sale_date	sale_year	sale_mon	sale_day	gross_price	price	gross_acres	tillable_acres	crp_acres	cash_rent	crp_rent	rent_source	gross_taxes	soil_rating	max_soil_id	max_mukey	max_musym	max_name	max_name_1	max_texture	max_slope_description	max_csr	max_component_name	max_taxonomic_class	max_slope	max_frmlndcl	max_nccpi3corn	max_nccpi3soy	max_nccpi3sg	max_nccpi3all	max_illinois_pi	max_ww_soil_id	max_ww_mukey	max_ww_musym	max_ww_name	max_ww_name_1	max_ww_texture	max_ww_slope_description	max_ww_csr	max_ww_component_name	max_ww_taxonomic_class	max_ww_slope	max_ww_frmlndcl	max_ww_nccpi3corn	max_ww_nccpi3soy	max_ww_nccpi3sg	max_ww_nccpi3all	max_ww_illinois_pi	avg_csr	avg_slope	avg_nccpi3corn	avg_nccpi3soy	avg_nccpi3sg	avg_nccpi3all	avg_illinois_pi	frmlndcl_por_farmland_of_statewi	frmlndcl_por_all_areas_are_prime	frmlndcl_por_not_prime_farmland	frmlndcl_por_prime_farmland_if_d	frmlndcl_por_prime_ifdrained	frmlndcl_por_prime_farmland_if_p	frmlndcl_por_na	frmlndcl_por_prime_farmland_if_i	frmlndcl_por_farmland_of_local_i	frmlndcl_por_farmland_of_unique_	frmlndcl_por_prime_farmland_if_s	soiltexture_por_silty_loam	soiltexture_por_silty_clay_loam	soiltexture_por_loam	soiltexture_por_water	soiltexture_por_etc	soiltexture_por_clay_loam	soiltexture_por_sandy_loam	soiltexture_por_sand	soiltexture_por_silty_clay	soiltexture_por_loamy_sand	soiltexture_por_clay	soiltexture_por_sandy_clay_loam	soiltexture_por_silt	wind_speed	wind_class_x	wind_dummy
		// 1859_AUC	5986	1	Southwest	Iowa	19	Adair	1	41.454559 - 94.269157	8 / 26 / 2020	2020	8	26			160	140.5					4264	81.5	2932	402143	11B	Colo - Ely silty clay loam, 2 to 5 percent slopes	Colo - Ely	silty clay loams	2 to 5 percent slopes	72	Colo	Fine - silty, mixed, superactive, mesic Cumulic Endoaquolls	4	Prime farmland if drained	0.84399998	0.81199998	0.49200001	0.866		2932	402143	11B	Colo - Ely silty clay loam, 2 to 5 percent slopes	Colo - Ely	silty clay loams	2 to 5 percent slopes	72	Colo	Fine - silty, mixed, superactive, mesic Cumulic Endoaquolls	4	Prime farmland if drained	0.84399998	0.81199998	0.49200001	0.866		72.825676	5.7843943	0.85840762	0.75072414	0.65213227	0.86936253		0.55257469	0.15598594	0	0	0.29143938	0	0	0	0	0	0	0	0.92094582	0	0	0	0.079054199	0	0	0	0	0	0	0	7.4177747	3	1
		// 1860_AUC	5993	1	Southwest	Iowa	19	Adair	1	41.41925 - 94.569389	4 / 9 / 2020	2020	4	9	647800	8200	79	75.910004	5.2800002				2000	77.900002	2229	402176	370B	Sharpsburg silty clay loam, 2 to 5 percent slopes	Sharpsburg	silty clay loam	2 to 5 percent slopes	83	Sharpsburg	TYPIC ARGIUDOLLS, FINE, MONTMORILLONITIC, MESIC	4	All areas are prime farmland	0.926	0.79299998	0.77499998	0.926		2229	402176	370B	Sharpsburg silty clay loam, 2 to 5 percent slopes	Sharpsburg	silty clay loam	2 to 5 percent slopes	83	Sharpsburg	TYPIC ARGIUDOLLS, FINE, MONTMORILLONITIC, MESIC	4	All areas are prime farmland	0.926	0.79299998	0.77499998	0.926		71.53273	5.297956	0.86132175	0.73874944	0.72537607	0.86176217		0.41337037	0.56661034	0	0	0.020019267	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	7.7351432	4	1

		if (fgets(data, 2048, file_csv)) {

			while (!error
			&& fgets(data, 2048, file_csv)) {
				++count_read;
				parse_data = data;
				parse_data.tokenize(",\r\n", &tokens, true, false);

				if (tokens.size() >= 23) {
					sale.clear();

					sale.id = tokens[0];
					if (tokens [5].get_length () > 0)
						sale.state_fips = atoi(tokens[5].get_text_ascii());
					else
						sale.state_fips = state_FIPS_from_name (tokens [4].get_text_ascii ());

					if (tokens [7].get_length () > 0)
						sale.county_fips = atoi(tokens[7].get_text_ascii());
					else
						sale.county_fips = counties->get_FIPS (sale.state_fips, tokens [6].get_text_ascii ());

					if (sale.county_fips != -1) {
						sale.fips = sale.state_fips * 1000 + sale.county_fips;

						if (sale.fips > 0)
							sale.CRD = CRD_by_county[sale.fips - 1];

						sale.acres = atof(tokens[16].get_text_ascii());
						if (tokens [15].get_length () > 0)
							sale.price = atof(tokens[15].get_text_ascii());
						sale.crop_acres = atof(tokens[17].get_text_ascii());

						// avg_csr
						if (tokens[58].get_length() > 0)
							sale.csr = atof(tokens[58].get_text_ascii());
						else
							sale.csr_missing = true;

						sale.year = atoi(tokens[11].get_text_ascii());
						sale.month = atoi (tokens [12].get_text_ascii ());

						// wind_speed
						// previous versions: "AverageWindSpeed"
						sale.AverageWindSpeed = atof(tokens[89].get_text_ascii());

						sale.high_wind = (tokens[20] == "1") ? 1 : 0; // high_wind
						if (tokens [21].get_length () > 0)
							sale.percent_tillable = atof(tokens[21].get_text_ascii()); // tillable_pct
						sale.tillable_ground = (tokens[22] == "1") ? 1 : 0; // tillable

						// tillable_pct
//						sale.percent_tillable = atof(tokens[21].get_text_ascii());
//						sale.tillable_ground = (tokens[22] == "1") ? 1 : 0;

		sale.buyer = tokens[32]; // buyer

						// wind_dummy ?
						// previous versions: "high_wind"
						// sale.high_wind = ?
						// sale.investor = ?


						domestic_all_states.push_back(sale);
						++count_valid;
					}
					else {
						log += "Can't read FIPS \"";
						log += tokens[7];
						log += "\", State \"";
						log += tokens[5];
						log += "\" in FarmlandFinder file \"";
						log += filename_afida;
						log.add_formatted("\" row %d.\n", count_read);
						error = true;
					}
				}
				else {
					log.add_formatted("ERROR, can't parse FarmlandFinder file (%d tokens) \"", tokens.size());
					log += filename_afida;
					log += "\"\n";

					log += "\"";
					log += data;
					log.add_formatted("\" row %d.\n", count_read);
					error = true;
				}
			}
		}

		fclose(file_csv);

		log.add_formatted("FarmlandFinder Records read\t%d\n", count_read);
		log.add_formatted("FarmlandFinder Valid records read\t%d\n", count_valid);
	}
	else {
		log += "ERROR, can't open Kansas file \"";
		log += filename_afida;
		log += "\"\n";
		error = true;
	}
	return !error;
}

bool AFIDA::set_Iowa_tillable_ground
	(dynamic_string& log)

// (4). tillable_ground: dummy variable. 1 for price_csr>0.25*diff_max_min & csr>50. 0 for the rest.
// Variable diff_max_min is to measure the difference between the maximum and the minimum value of variable price_csr within the same county and year.
// Because Iowa ag sales dataset doesn’t have cropland acres variable. The method could help to measure if it’s tillable ground.

{
	std::vector <AFIDA_Sale>::iterator iowa_sale;
	std::map <int, double> min_csr, max_csr, diff_max_min;
	std::map <int, double>::iterator csr_year_min, csr_year_max, diff_year;
	bool error = false;

	for (iowa_sale = domestic_all_states.begin();
	iowa_sale != domestic_all_states.end();
	++iowa_sale) {
		// Don't count blank field as 0
		if (!iowa_sale->csr_missing) {

			if ((csr_year_min = min_csr.find(iowa_sale->year)) != min_csr.end()) {
				if (iowa_sale->csr < csr_year_min->second)
					csr_year_min->second = iowa_sale->csr;
			}
			else
				min_csr.insert(std::pair <int, double>(iowa_sale->year, iowa_sale->csr));

			if ((csr_year_max = max_csr.find(iowa_sale->year)) != max_csr.end()) {
				if (iowa_sale->csr > csr_year_max->second)
					csr_year_max->second = iowa_sale->csr;
			}
			else
				max_csr.insert(std::pair <int, double>(iowa_sale->year, iowa_sale->csr));
		}
	}

	for (csr_year_min = min_csr.begin(); csr_year_min != min_csr.end(); ++csr_year_min) {
		if ((csr_year_max = max_csr.find(csr_year_min->first)) != max_csr.end()) {
			if ((diff_year = diff_max_min.find(csr_year_min->first)) != diff_max_min.end())
				diff_year->second = csr_year_max->second - csr_year_min->second;
			else
				diff_max_min.insert(std::pair <int, double>(csr_year_min->first, csr_year_max->second - csr_year_min->second));
		}
		else {
			log.add_formatted("ERROR, CSR min max year %d mismatch.\n", csr_year_min->first);
			error = true;
		}
	}

	for (iowa_sale = domestic_all_states.begin();
	iowa_sale != domestic_all_states.end();
	++iowa_sale) {
		if ((diff_year = diff_max_min.find(iowa_sale->year)) != diff_max_min.end()) {
			// 1 for price_csr>0.25*diff_max_min & csr>50.
			if ((iowa_sale->csr > 50.0)
			&& (iowa_sale->csr > (0.25 * diff_year->second)))
				iowa_sale->tillable_ground = 1;
		}
		else {
			log.add_formatted("ERROR, no diff_max_min for year %d.\n", iowa_sale->year);
			error = true;
		}
	}
	log += "\nCSR range\n";
	log += "Year\tMin\tMax\tDiff\tDiff*0.25\n";
	for (diff_year = diff_max_min.begin (), csr_year_min = min_csr.begin(), csr_year_max = max_csr.begin ();
	diff_year != diff_max_min.end();
	++diff_year, ++csr_year_min, ++csr_year_max)
		log.add_formatted("%d\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n", diff_year->first, csr_year_min->second, csr_year_max->second, diff_year->second, diff_year->second * 0.25);
	log += "\n";

	return !error;
}


bool AFIDA::read_Iowa
	(const dynamic_string& filename_afida,
	CountyMaster* counties,
	dynamic_string& log)

// "E:\GIS\AFIDA\Iowa_Ag_Sales_append_all.csv";

// 2022-04-26 new version with 77 columns

{
	FILE* file_csv;
	char data[2048];
	std::vector <dynamic_string> tokens;
	dynamic_string inconstant_filename, parse_data, county_name;
	bool error = false;
	int count_valid = 0, count_read = 0;
	AFIDA_Sale sale;
	Timestamp when;

	// set_wind_counties(&high_wind_by_fips);

	inconstant_filename = filename_afida;
	if (fopen_s(&file_csv, inconstant_filename.get_text_ascii(), "r") == 0) {

		// id	legal_desc	sale_num	st	nd	year	county	code	township	sale_date	acre_tot	acre_tax	pct_ac_tax	csr	price_tot	price_acre	assess_lnd	ass_lnd_pct	price_ac_adj	price_adj_pct	seller	sell_instate	seller_type	sell_family	sell_couple	sell_single	sell_estate	sell_trust	sell_corp	sell_govt	sell_npo	sell_check	buyer	buy_instate	buyer_type	buy_family	buy_couple	buy_single	buy_estate	buy_trust	buy_corp	buy_govt	buy_npo	buy_check	ia_2_ia	ia_2_e	e_2_ia	e_2_e	sale_check	sale_terms	note_addl	sale_deed	sale_auction	sale_family	sale_adjacent	sale_highbldg	sale_belowmkt	sale_forest	sale_split	sale_exchange	sale_wetland	sale_develop	sale_month	sale_day	sale_year	sale_quarter	price_csr	Rank	AverageWindSpeed	CountyPopulation	_merge	high_wind	investor	min_pricecsr	max_pricecsr	diff_max_min	tillable_ground
		// 	20					1990	Chickasaw	1	New Hampton	8 / 24 / 1990	100			60	90000	900	38782	90			Francis & Beverly Drewelow(IA)	1	Family	0	1	0	0	0	0	0	0	1	Fred & JoAnne Stange(IA)	1	Family	0	1	0	0	0	0	0	0	1	1	0	0	0	1	Deed		1	0	0	0	0	0	0	0	0	0	0	8	24	1990	3	15	90	15.51	Chickasaw, IA / 12, 329	Matched(3)	0	0	5.5277805	27.083334	21.555553	1
		// 	13047					1990	Chickasaw	1	Dresden	 11 / 4 / 1990	112	110.5	0.98660713	60	132000	1178.5714	69060	1	1194.5702	0.98660713	Katz Est., Melvin - Fredericksburg	1	Estate	0	0	0	1	0	0	0	0	1	David Good(IA)	1	Single	0	0	1	0	0	0	0	0	1	1	0	0	0	1	Deed * Near Assessed value	1	0	0	0	0	0	0	0	0	0	0	7	5	1990	3	19.642857	90	15.51	Chickasaw, IA / 12, 329	Matched(3)	0	0	5.5277805	27.083334	21.555553	1

		if (fgets(data, 2048, file_csv)) {

			while (!error
				&& fgets(data, 1024, file_csv)) {
				++count_read;
				parse_data = data;
				parse_data.tokenize(",\r\n", &tokens, true, false);

				if (tokens.size() == 77) {
					sale.clear();

					sale.id = tokens [0];
					sale.state_fips = 19;

					county_name = tokens[6];
					county_name.trim_right();
					correct_iowa_county_name(county_name);
					sale.county_fips = counties->get_FIPS(sale.state_fips, county_name.get_text_ascii());

					if (sale.county_fips != -1) {

						if (tokens [10].get_length () > 0)
							sale.acres = atof(tokens[10].get_text_ascii());

						if (tokens [13].get_length () > 0)
							sale.csr = atof(tokens[13].get_text_ascii());
						else
							sale.csr_missing = true;

						if (tokens [14].get_length () > 0)
							sale.price = atof(tokens[14].get_text_ascii());

						when.parse (tokens[9].get_text_ascii (), 0);
						if (when.is_defined()) {
							sale.year = when.get_year();
							sale.month = when.get_month();

							sale.buyer = tokens [32]; // buyer
							sale.AverageWindSpeed = atof(tokens[68].get_text_ascii()); // AverageWindSpeed
							sale.high_wind = (tokens[71] == "1") ? 1 : 0; // high_wind
							sale.investor = (tokens [72] == "1") ? 1 : 0; // investor

							// log.add_formatted("%02d-%03d\t%.6lf\t%ld\n", state_fips, county_fips, acres, price);
							domestic_all_states.push_back(sale);
							++count_valid;
						}
						else {
							log += "ERROR, can't parse Iowa Date \"";
							log += tokens [8];
							log += "\" in file\"";
							log += filename_afida;
							log.add_formatted("\" row %d.\n", count_read);
							error = true;
						}
					}
					else {
						log += "Can't match county name \"";
						log += tokens[5];
						log += "\" State \"Iowa\" in Iowa file \"";
						log += filename_afida;
						log.add_formatted("\" row %d.\n", count_read);
					}
				}
				else {
					log.add_formatted("ERROR, can't parse Iowa file (%d tokens) \"", tokens.size());
					log += filename_afida;
					log.add_formatted("\" row %d.\n", count_read);

					log += "\"";
					log += data;
					log += "\"\n";
					error = true;
				}
			}
		}

		fclose(file_csv);

		log.add_formatted("Iowa Records read\t%d\n", count_read);
		log.add_formatted("Iowa Valid records read\t%d\n", count_valid);

		if (!set_Iowa_tillable_ground (log))
			error = true;
	}
	else {
		log += "ERROR, can't open Iowa file \"";
		log += filename_afida;
		log += "\"\n";
		error = true;
	}
	return !error;
}

bool sort_sales
	(const AFIDA_Sale& buyer_1,
	const AFIDA_Sale& buyer_2)
{
	if (buyer_1.state_fips < buyer_2.state_fips)
		return true;
	else
		if (buyer_1.state_fips > buyer_2.state_fips)
			return false;
		else
			if (buyer_1.county_fips < buyer_2.county_fips)
				return true;
			else
				return false;
}

void AFIDA::fill_potential_matches
	(std::vector <AFIDA_Sale> *local_domestic,
	const int state_fips,
	CountyMaster* counties,
	std::map <int, std::set <int>>* adjacent_counties,
	const int* CRD_by_county,
	dynamic_string &log)

{
	std::vector <AFIDA_Sale>::const_iterator buyer;
	int count_included = 0, count_skipped = 0, count_adjacent = 0, count_crd = 0, district_index;
	std::map <int, AFIDA_County>::iterator county;
	std::map <int, std::set <int>>::const_iterator nearby_county_list;
	std::set <int>::const_iterator nearby_county;
	AFIDA_County new_county;

	for (buyer = foreign.begin();
	buyer != foreign.end();
	++buyer) {
		if ((counties->match(buyer->state_fips, buyer->county_fips)) != NULL) {
			if ((county = potential_matches.find(buyer->fips)) != potential_matches.end())
				county->second.add_foreign_unique (*buyer);
			else {
				new_county.clear();
				new_county.fips = buyer->fips;
				new_county.state_fips = buyer->state_fips;
				new_county.county_fips = buyer->county_fips;
				new_county.foreign.push_back(*buyer);
				potential_matches.insert(std::pair <int, AFIDA_County>(buyer->fips, new_county));
			}
			// and to all adjacent counties
			if ((nearby_county_list = adjacent_counties->find(buyer->fips)) != adjacent_counties->end()) {
				for (nearby_county = nearby_county_list->second.begin();
				nearby_county != nearby_county_list->second.end();
				++nearby_county) {
					if ((county = potential_matches.find(*nearby_county)) != potential_matches.end()) {
						if (county->second.add_foreign_unique(*buyer))
							++count_adjacent;
					}
					else {
						new_county.clear();
						new_county.fips = *nearby_county;
						new_county.state_fips = *nearby_county / 1000;
						new_county.county_fips = *nearby_county % 1000;
						new_county.foreign.push_back(*buyer);
						potential_matches.insert(std::pair <int, AFIDA_County>(*nearby_county, new_county));
						++count_adjacent;
					}
				}
			}
			// Add all counties from the CRD that aren't already in
			if (buyer->CRD > 0) {
				for (district_index = 0;
				district_index != max_county_fips;
				++district_index) {
					// 2022-10-12 CRDs are only unique within state
					if (((district_index + 1) / 1000) == buyer->state_fips) {
						if ((CRD_by_county [district_index] == buyer->CRD)
						&& ((district_index + 1) != buyer->fips)) {
							if ((county = potential_matches.find(district_index + 1)) != potential_matches.end()) {
								if (county->second.add_foreign_unique(*buyer))
									++count_crd;
							}
							else {
								new_county.clear();
								new_county.fips = district_index + 1;
								new_county.state_fips = (district_index + 1) / 1000;
								new_county.county_fips = (district_index + 1)  % 1000;
								new_county.foreign.push_back(*buyer);
								potential_matches.insert(std::pair <int, AFIDA_County> (new_county.fips, new_county));
								++count_crd;
							}
						}
					}
				}
			}

			++count_included;
		}
		else {
			++count_skipped;
			log.add_formatted("Invalid foreign FIPS \t%02d%03d\n", buyer->state_fips, buyer->county_fips);
		}
	}

	log.add_formatted("Foreign potential matches\t%d\n", count_included);
	log.add_formatted("Foreign potential matches (adjacent)\t%d\n", count_adjacent);
	log.add_formatted("Foreign potential matches (CRD)\t%d\n", count_crd);
	log.add_formatted("Foreign sales skipped\t%d\n", count_skipped);

	count_included = count_skipped = count_adjacent = count_crd = 0;
	for (buyer = local_domestic->begin();
	buyer != local_domestic->end();
	++buyer) {
		if ((counties->match(buyer->state_fips, buyer->county_fips)) != NULL) {

			// Add this sale to its county
			if ((county = potential_matches.find(buyer->fips)) != potential_matches.end())
				county->second.add_domestic_unique(*buyer);
			else {
				new_county.clear();
				new_county.fips = buyer->fips;
				new_county.county_fips = buyer->fips;
				new_county.state_fips = buyer->state_fips;
				new_county.county_fips = buyer->county_fips;
				new_county.domestic.push_back(*buyer);
				potential_matches.insert(std::pair <int, AFIDA_County>(buyer->fips, new_county));
			}
			// and to all adjacent counties
			if ((nearby_county_list = adjacent_counties->find(buyer->fips)) != adjacent_counties->end()) {
				for (nearby_county = nearby_county_list->second.begin();
				nearby_county != nearby_county_list->second.end();
				++nearby_county) {
					if ((county = potential_matches.find(*nearby_county)) != potential_matches.end()) {
						if (county->second.add_domestic_unique(*buyer))
							++count_adjacent;
					}
					else {
						new_county.clear();
						new_county.fips = *nearby_county;
						new_county.state_fips = *nearby_county / 1000;
						new_county.county_fips = *nearby_county % 1000;
						new_county.domestic.push_back(*buyer);
						potential_matches.insert(std::pair <int, AFIDA_County>(*nearby_county, new_county));
						++count_adjacent;
					}
				}
			}
			// Add all counties from the CRD that aren't already in
			if (buyer->CRD > 0) {
				for (district_index = 0;
				district_index != max_county_fips;
				++district_index) {
					// 2022-10-12 CRDs are only unique within state
					if (((district_index + 1) / 1000) == buyer->state_fips) {
						if ((CRD_by_county [district_index] == buyer->CRD)
						&& ((district_index + 1)  != buyer->fips)) {
							if ((county = potential_matches.find(district_index + 1)) != potential_matches.end()) {
								if (county->second.add_domestic_unique(*buyer))
									++count_crd;
							}
							else {
								new_county.clear();
								new_county.fips = district_index + 1;
								new_county.state_fips = new_county.fips / 1000;
								new_county.county_fips = new_county.fips % 1000;
								new_county.foreign.push_back(*buyer);
								potential_matches.insert(std::pair <int, AFIDA_County>(new_county.fips, new_county));
								++count_crd;
							}
						}
					}
				}
			}

			++count_included;

		}
		else {
			++count_skipped;
			log.add_formatted("Invalid domestic FIPS \t%02d-%03d\n", buyer->state_fips, buyer->county_fips);
		}
	}
	log.add_formatted("State potential matches\t%d\n", count_included);
	log.add_formatted("State potential matches (adjacent)\t%d\n", count_adjacent);
	log.add_formatted("State potential matches (CRD)\t%d\n", count_crd);
	log.add_formatted("State sales skipped\t%d\n", count_skipped);

}

void AFIDA::filter_to_state_or_adjacent_counties
	(std::vector <AFIDA_Sale> *sales,
	const int state_fips,
	std::map <int, std::set <int>>* adjacent_counties,
	dynamic_string &log)

// Filter foreign to just Kansas or counties adjacent to Kansas

{
	std::vector <AFIDA_Sale> temp;
	std::vector <AFIDA_Sale>::const_iterator buyer_1;
	int fips;
	std::map <int, std::set <int>>::const_iterator county_set;
	std::set <int>::const_iterator county_adjacent;
	bool nearby;

	for (buyer_1 = sales->begin();
	buyer_1 != sales->end();
	++buyer_1)
		if (buyer_1->state_fips == state_fips)
			temp.push_back(*buyer_1);
		else {
			// Is this sale in a county adjacent to any counties in target state?
			fips = buyer_1->state_fips * 1000 + buyer_1->county_fips;
			nearby = false;
			if ((county_set = adjacent_counties->find(fips)) != adjacent_counties->end()) {
				for (county_adjacent = county_set->second.begin();
				!nearby
				&& (county_adjacent != county_set->second.end());
				++county_adjacent)
					if (*county_adjacent / 1000 == state_fips)
						nearby = true;
			}
			if (nearby)
				temp.push_back(*buyer_1);
		}

	log.add_formatted("Size before State %d filter\t%d\n", state_fips, (int)foreign.size());
	*sales = temp;
	log.add_formatted("Size after filter\t%d\n", (int)foreign.size());
}

void AFIDA::report
	(CountyMaster* counties,
	std::map <int, std::set <int>>* adjacent_counties,
	dynamic_string& log)

{
	std::map <short, std::vector <CountyMasterRecord>>::const_iterator kansas_county_list;
	std::vector <CountyMasterRecord>::const_iterator county;
	std::vector <AFIDA_Sale>::const_iterator buyer_1, buyer_2;

	filter_to_state_or_adjacent_counties(&foreign, 20, adjacent_counties, log);

	// std::sort(foreign.begin(), foreign.end(), sort_sales);
	// std::sort(kansas.begin(), kansas.end(), sort_sales);

	if ((kansas_county_list = counties->counties_by_state.find(20)) != counties->counties_by_state.end()) {
		std::vector <dynamic_string> sales_1, sales_2;
		std::vector <dynamic_string>::iterator s1, s2;
		dynamic_string sale_text;

		log += "FIPS\tCounty Name";
		log += "\tForeign Month\tForeign Year\tForeign Acres\tForeign Price";
		log += "\tKansas Month\tKansas Year\tKansas Acres\tKansas Price";
		log += "\n";

		buyer_1 = foreign.begin();
		buyer_2 = domestic_all_states.begin();

		for (county = kansas_county_list->second.begin();
		county != kansas_county_list->second.end();
		++county) {
			log.add_formatted("%02d-%03d\t", 20, county->CountyFIPS);
			log += county->CountyName;

			while ((buyer_1 != foreign.end ())
			&& (buyer_1->county_fips < county->CountyFIPS))
				++buyer_1;

			while ((buyer_2 != domestic_all_states.end())
			&& (buyer_2->county_fips < county->CountyFIPS))
				++buyer_2;

			sales_1.clear();
			sales_2.clear();

			while ((buyer_1 != foreign.end())
			&& (buyer_1->county_fips == county->CountyFIPS)) {
				sale_text.format ("\t%d\t%d\t%.6lf\t%.2lf", buyer_1->month, buyer_1->year, buyer_1->acres, buyer_1->price);
				sales_1.push_back(sale_text);
				++buyer_1;
			}

			while ((buyer_2 != domestic_all_states.end())
			&& (buyer_2->county_fips == county->CountyFIPS)) {
				sale_text.format("\t%d\t%d\t%.6lf\t%.2lf", buyer_2->month, buyer_2->year, buyer_2->acres, buyer_2->price);
				sales_2.push_back(sale_text);
				++buyer_2;
			}

			for (s1 = sales_1.begin(), s2 = sales_2.begin();
			(s1 != sales_1.end()) && (s2 != sales_2.end());
			++s1, ++s2) {
				log += "\t\t";
				log += *s1;
				log += *s2;
				log += "\n";
			}
			while (s1 != sales_1.end()) {
				log += "\t\t";
				log += *s1;
				log += "\t\t\t\t\n";
				++s1;
			}
			while (s2 != sales_2.end()) {
				log += "\t\t";
				log += "\t\t\t\t";
				log += *s2;
				log += "\n";
				++s2;
			}

			log += "\n";
		}
	}

}

bool coincident_point
	(map_object* p1,
	map_object* p2)

// True if any point in p1 coincides with any point in p2

{
	int index_1, index_2, polygon_1, polygon_2;
	bool found = false;

	if (p1->type == MAP_OBJECT_POLYGON) {
		for (index_1 = 0;
		!found && (index_1 < ((map_polygon*)p1)->node_count);
		++index_1) {
			if (p2->type == MAP_OBJECT_POLYGON) {
				for (index_2 = 0;
				!found && (index_2 < ((map_polygon*)p2)->node_count);
				++index_2) {
					if ((((map_polygon*)p1)->nodes[index_1 * 2] == ((map_polygon*)p2)->nodes[index_2 * 2])
					&& (((map_polygon*)p1)->nodes[index_1 * 2 + 1] == ((map_polygon*)p2)->nodes[index_2 * 2 + 1]))
						found = true;
				}
			}
			else {
				ASSERT(p2->type == MAP_OBJECT_POLYGON_COMPLEX);
				for (polygon_2 = 0;
				!found && (polygon_2 < ((map_polygon_complex*)p2)->polygon_count);
				++polygon_2) {
					for (index_2 = 0;
					!found && (index_2 < ((map_polygon_complex*)p2)->polygons[polygon_2].node_count);
					++index_2) {
						if ((((map_polygon*)p1)->nodes[index_1 * 2] == ((map_polygon_complex*)p2)->polygons[polygon_2].nodes[index_2 * 2])
							&& (((map_polygon*)p1)->nodes[index_1 * 2 + 1] == ((map_polygon_complex*)p2)->polygons[polygon_2].nodes[index_2 * 2 + 1]))
							found = true;
					}
				}
			}
		}
	}
	else {
		ASSERT(p1->type == MAP_OBJECT_POLYGON_COMPLEX);
		for (polygon_1 = 0;
		!found && (polygon_1 < ((map_polygon_complex*)p1)->polygon_count);
		++polygon_1) {
			for (index_1 = 0;
			!found && (index_1 < ((map_polygon_complex*)p1)->polygons[polygon_1].node_count);
			++index_1) {
				if (p2->type == MAP_OBJECT_POLYGON) {
					for (index_2 = 0;
						!found && (index_2 < ((map_polygon*)p2)->node_count);
						++index_2) {
						if ((((map_polygon_complex*)p1)->polygons[polygon_1].nodes[index_1 * 2] == ((map_polygon*)p2)->nodes[index_2 * 2])
						&& (((map_polygon_complex*)p1)->polygons[polygon_1].nodes[index_1 * 2 + 1] == ((map_polygon*)p2)->nodes[index_2 * 2 + 1]))
							found = true;
					}
				}
				else {
					ASSERT(p2->type == MAP_OBJECT_POLYGON_COMPLEX);
					for (polygon_2 = 0;
					!found && (polygon_2 < ((map_polygon_complex*)p2)->polygon_count);
					++polygon_2) {
						for (index_2 = 0;
						!found && (index_2 < ((map_polygon_complex*)p2)->polygons[polygon_2].node_count);
						++index_2) {
							if ((((map_polygon_complex*)p1)->polygons[polygon_1].nodes[index_1 * 2] == ((map_polygon_complex*)p2)->polygons[polygon_2].nodes[index_2 * 2])
							&& (((map_polygon_complex*)p1)->polygons[polygon_1].nodes[index_1 * 2 + 1] == ((map_polygon_complex*)p2)->polygons[polygon_2].nodes[index_2 * 2 + 1]))
								found = true;
						}
					}
				}
			}

		}

	}
	return found;
}

void find_adjacent_counties
	(map_layer* layer_counties,
	std::map <int, std::set <int>>* adjacent_counties,
	const bool match_across_state_lines,
	interface_window*view,
	dynamic_string& log)

// 2022-04-18 Produce list of FIPS codes of adjacent counties for each county in map_layer

// County shapefile from "r:\Census\TIGER 2012\County\tl_2012_us_county\tl_2012_us_county.shp"
// has exactly coincident adjacent borders

// 2022-04-21 Wendong: Don't match adjacent counties across state lines

{
	std::vector <map_object*>::iterator county, other_county;
	std::map <int, std::set <int>>::iterator match_list, other_match_list;
	std::set <int> new_set;
	std::pair <std::map <int, std::set <int>>::iterator, bool> insertion_result;
	int fips_index, fips, other_fips;
	bounding_cube county_extent;
	bool skip;

	// Add a numeric field for FIPS to save time
	fips_index = layer_counties->attribute_count_numeric;
	layer_counties->resize(fips_index + 1, layer_counties->attribute_count_text);
	layer_counties->column_names_numeric.push_back ("FIPS");

	for (county = layer_counties->objects.begin();
	county != layer_counties->objects.end();
	++county) {
		// attributes_numeric[0] is State, [1] is county
		fips = atoi((*county)->attributes_text[0].get_text_ascii()) * 1000 + atoi((*county)->attributes_text[1].get_text_ascii());
		(*county)->attributes_numeric[fips_index] = fips;
	}

	for (county = layer_counties->objects.begin();
	county != layer_counties->objects.end();
	++county) {

		fips = (int)(*county)->attributes_numeric[fips_index];
		view->update_progress_formatted (1, "FIPS %d", fips);

		match_list = adjacent_counties->find(fips);

		county_extent.clear();
		(*county)->check_extent(&county_extent);

		for (other_county = layer_counties->objects.begin();
		other_county != layer_counties->objects.end();
		++other_county) {
			if (county != other_county) {
				other_fips = (int)(*other_county)->attributes_numeric[fips_index];
				// are these counties already paired up?
				if ((match_list == adjacent_counties->end ())
				|| (match_list->second.find(other_fips) == match_list->second.end())) {
					// Are they close to each other?
					if ((*other_county)->overlaps_logical(&county_extent)) {
						skip = false;

						if ((match_across_state_lines == false)
						&& ((fips / 1000) != (other_fips / 1000)))
							skip = true;
						else
							skip = false;

						if (!skip
						&& coincident_point(*county, *other_county)) {

							if (match_list != adjacent_counties->end())
								match_list->second.insert(other_fips);
							else {
								new_set.clear();
								new_set.insert(other_fips);
								insertion_result = adjacent_counties->insert(std::pair <int, std::set <int>>(fips, new_set));
								match_list = insertion_result.first;
							}

							if ((other_match_list = adjacent_counties->find(other_fips)) != adjacent_counties->end())
								other_match_list->second.insert(fips);
							else {
								new_set.clear();
								new_set.insert(fips);
								adjacent_counties->insert(std::pair <int, std::set <int>>(other_fips, new_set));
							}

						}
					}
				}
			}
		}
	}

	if (false) {
		std::set <int>::const_iterator pair;

		for (match_list = adjacent_counties->begin();
		match_list != adjacent_counties->end();
		++match_list) {
			log.add_formatted("%d", match_list->first);
			for (pair = match_list->second.begin();
			pair != match_list->second.end();
			++pair) {
				log.add_formatted("\t%d", *pair);
			}
			log += "\n";
		}
	}
}

void AFIDA::check_sale_ranges
	(const AFIDA_Sale* sale)

{
	if (sale->acres < min_acres)
		min_acres = sale->acres;
	if (sale->acres > max_acres)
		max_acres = sale->acres;
	if (sale->percent_tillable < min_percent_tillable)
		min_percent_tillable = sale->percent_tillable;
	if (sale->percent_tillable > max_percent_tillable)
		max_percent_tillable = sale->percent_tillable;

}

void AFIDA::check_ranges()

{
	std::vector <AFIDA_Sale>::const_iterator sale;

	for (sale = foreign.begin();
	sale != foreign.end();
	++sale)
		check_sale_ranges(&*sale);

	for (sale = domestic_all_states.begin();
	sale != domestic_all_states.end();
	++sale)
		check_sale_ranges(&*sale);

	range_acres = max_acres - min_acres;
	range_percent_tillable = max_percent_tillable - min_percent_tillable;
}

void AFIDA::write_column_headers
	(const int state_fips,
	FILE* f,
	const char delimiter)

{
	char state_name_text[25];
	// 	report.add_formatted("\t%d\t$%.2lf\t%.6lf\t%.6lf\t%.6lf\t%.1lf\t%d", year, price, acres, crop_acres, percent_tillable, high_wind, investor);

	fprintf(f, ",,,,,,Foreign,,,,,,,,,,,,,");

	fprintf (f, ",,,,,,,,,,,,,,,,,,,,,"); // 2023-03-01 21 new fields

	if (state_fips == -1)
		fprintf(f, "%cDomestic\n", delimiter);
	else
		fprintf(f, "%c%s\n", delimiter, state_name(state_fips, state_name_text, 25));

	fprintf(f, "State%cScore", delimiter);
	fprintf(f, "%cDiff Acres%cDiff Tillable", delimiter, delimiter);
	fprintf(f, "%cDiff High Wind", delimiter);
	fprintf(f, "%cDiff Investor", delimiter);

	foreign_id_index = 6;
	domestic_id_index = 41;

	fprintf(f, "%cID%cFIPS%cCRD", delimiter, delimiter, delimiter);
	// log += "\tYear\tPrice\tAcres\tCrop Acres\tBuyer\tTillable\tHigh Wind\tWind Name\tInvestor";
	fprintf(f, "%cYear%cPurchase Price%cEstimated Value%cCurrent Value%cAcres%cCrop Acres%cBuyer%cTillable%cHigh Wind%cWind Name%cInvestor",
	delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);

	// 2023-03-01 Keep all attributes of new table
	fprintf (f, "%cactivitytype%ccountry%cparcelid%cactivitynumber%cownerid%cuscode%ccountrycode%cppb%cownertype%crelationshiptorepresentative",
	delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
	fprintf (f, "%ctypeofinterest%cpercentofownership%cacquisitionmethod%cdebt%ccrop%cpasture%cforest%cotheragriculture%cRank%cCountyPopulation%c_merge",
	delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);	

	/*
	log.add_formatted("%02d%03d\t", match_county->first);
	county = counties->match(match_county->first / 1000, match_county->first % 1000);
	log += county->StateName;
	log += "\t";
	log += county->CountyName;
	log += "\n";
	*/

	// Domestic (state name)
	fprintf(f, "%cID%cFIPS%cCRD%cYear%cPrice%cAcres%cCrop Acres", delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
	/*if (state_fips == 19)
		log += "\tBuyer\tCSR\tTillable\tHigh Wind\tInvestor";
	else
		log += "\tTillable\tHigh Wind\tInvestor";
	*/
	fprintf(f, "%cCSR%cTillable%cHigh Wind%cInvestor", delimiter, delimiter, delimiter, delimiter);

	fprintf(f, "%cAdjacent or CRD", delimiter);
	fprintf(f, "\n");
}


void AFIDA::county_report
	(const dynamic_string &filename_output,
	const int state_fips,
	CountyMaster* counties,
	std::map <int, std::set <int>>* adjacent_counties,
	const int* CRD_by_county,
	const int dimension_count,
	interface_window *view,
	dynamic_string& log)

// To match adjacent counties, filter_to_state_or_adjacent_counties reduces foreign and domestic lists to adjacent_counties.

{
	// std::map <short, std::vector <CountyMasterRecord>>::const_iterator kansas_county_list;
	// std::vector <CountyMasterRecord>::const_iterator county;
	// CountyMasterRecord* county;
	std::map <int, AFIDA_County>::iterator match_county;
	dynamic_string report;
	char delimiter = ',';
	std::vector <AFIDA_Sale> local_domestic;

	// state_name(state_fips, state, 25);

	potential_matches.clear();

	// cut vector foreign down to counties in state or adjacent counties
	// 2022-10-12 AFIDA.read reads all states, and fill_potential_matches will load target state and adjacent counties,
	filter_to_state_or_adjacent_counties(&foreign, state_fips, adjacent_counties, log);
	local_domestic = domestic_all_states;
	filter_to_state_or_adjacent_counties(&local_domestic, state_fips, adjacent_counties, log);

	check_ranges();

	/*
	log += "\tMin\tMax\tRange\n";
	log.add_formatted("Acres\t%.4lf\t%.4lf\t%.4lf\n", min_acres, max_acres, range_acres);
	log.add_formatted("Percent Tillable\t%.4lf\t%.4lf\t%.4lf\n", min_percent_tillable, max_percent_tillable, range_percent_tillable);
	if (dimension_count == 3)
		log.add_formatted("Wind\t%.4lf\t%.4lf\t%.4lf\n", 0.0, 1.0, 1.0);
	*/

	log += "\n";

	fill_potential_matches (&local_domestic, state_fips, counties, adjacent_counties, CRD_by_county, log);

		/*
		for (match_county = potential_matches.begin();
		match_county != potential_matches.end();
		++match_county) {
			view->update_progress_formatted (1, "County Report %d", match_county->first);
			if (state_fips == 19) {
				if (match_county->second.match_sales_weighted_iowa (limit, this, adjacent_counties, CRD_by_county, report, view, log)) {
					// log += "\n";
					log += report;
				}
			}
			else {
				if (match_county->second.match_sales_weighted_kansas(limit, this, adjacent_counties, CRD_by_county, report, view, log)) {
					// log += "\n";
					log += report;
				}
			}
		}
		*/
	county_report_matches (filename_output, delimiter, state_fips, counties, adjacent_counties, CRD_by_county, report, view, log);
}

bool work_county_matches
	(thread_work* job)

{
	thread_work_match_county *cjob = (thread_work_match_county *) job;

	if (cjob->county->match_sales_weighted (cjob->filename_output, cjob->limit, cjob->controller, cjob->adjacent_counties,
	cjob->CRD_by_county, cjob->delimiter, NULL, job->log)) {
	}

	job->status = THREAD_WORK_STATUS_COMPLETE;
	job->end = std::chrono::system_clock::now();

	return true;
}

void split_output_path
	(const dynamic_string& filename_output,
	dynamic_string& output_path,
	dynamic_string& output_prefix)

	// "F:\FarmlandFinder\2022-08-15\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Listings.csv"
	// to "F:\FarmlandFinder\2022-08-15\" and "Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Listings"

{
	filename_struct parser;
	parser.set_filename(filename_output);
	parser.write_path(&output_path);
	output_prefix = parser.prefix;
}

void AFIDA::county_report_matches
	(const dynamic_string &filename_output,
	const char delimiter,
	const int state_fips,
	CountyMaster* counties,
	std::map <int, std::set <int>>* adjacent_counties,
	const int* CRD_by_county,
	dynamic_string &report,
	interface_window* view,
	dynamic_string& log)

{
	int limit = -1;
	std::map <int, AFIDA_County>::iterator match_county;
	thread_manager threader;
	std::map <long, thread_work*> jobs;
	std::map <long, thread_work*>::iterator completed_job;
	thread_work_match_county* job;
	thread_manager_statistics stats;
	dynamic_string output_path, output_prefix;

	view->update_progress ("County Report Matches", 0);

	split_output_path(filename_output, output_path, output_prefix);

	for (match_county = potential_matches.begin();
	match_county != potential_matches.end();
	++match_county) {
		job = new thread_work_match_county;
		job->id = match_county->second.state_fips * 1000 + match_county->second.county_fips;
		job->work_function = &work_county_matches;
		job->controller = this;
		job->county = &match_county->second;
		job->adjacent_counties = adjacent_counties;
		job->CRD_by_county = CRD_by_county;
		job->limit = limit;
		job->description.format("Run %ld", job->id);
		job->delimiter = delimiter;
		job->view = view;

		job->filename_output = output_path;
		job->filename_output += output_prefix;
		job->filename_output.add_formatted("-%ld.csv", job->id);
		thread_filenames.push_back(job->filename_output);

		jobs.insert(std::pair <long, thread_work*>(job->id, job));
	}

	threader.thread_count = 12;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 5;
	stats.reset(threader.thread_count);
	threader.statistics = &stats;

	threader.run_tiny(&jobs, view, log);

	for (completed_job = jobs.begin();
	completed_job != jobs.end();
	++completed_job) {
		view->update_progress_formatted(1, "Job %ld", completed_job->first);


		// fwrite (((thread_work_match_county*)completed_job->second)->report.get_text_ascii (), 1, ((thread_work_match_county*)completed_job->second)->report.get_length (), file_output);
		log += completed_job->second->log;
		// ((thread_work_match_county*)completed_job->second)->report.clear();
		completed_job->second->log.clear();
	}

	// view->update_progress("Append and Filter", 1);
	// append_and_filter_output_files (filename_output, state_fips, delimiter, &thread_filenames, log);
}

bool AFIDA::append_output_files
	(const dynamic_string& filename_output,
	const int state_fips,
	const char delimiter,
	std::vector <dynamic_string>* all_thread_filenames,
	dynamic_string& log)

	// from MapLayerProximity.append_thread_files

{
	FILE* f, * thread_read;
	bool error = false;
	dynamic_string inconstant_filename = filename_output, column_names;
	std::vector <dynamic_string>::iterator thread_filename;

	if (fopen_s(&f, inconstant_filename.get_text_ascii(), "w") == 0) {

		BYTE buffer[2048];
		int buffer_size;
		filename_struct fs;
		dynamic_string target_folder, target_filename;

		write_column_headers (state_fips, f, delimiter);

		for (thread_filename = all_thread_filenames->begin();
			thread_filename != all_thread_filenames->end();
			++thread_filename) {

			if (fopen_s(&thread_read, thread_filename->get_text_ascii(), "r") == 0) {
				while ((buffer_size = fread(buffer, 1, 2048, thread_read)) > 0)
					fwrite(buffer, buffer_size, 1, f);
				fclose(thread_read);
			}

			fs.parse(&*thread_filename);
			fs.write_path(&target_folder);
			fs.write_filename(&target_filename);
			remove_file(target_folder, target_filename, &error, log);
		}

		fclose(f);
	}
	else {
		error = true;
		log += "ERROR, can't open output file \"";
		log += filename_output;
		log += "\".\n";
	}
	return !error;
}

bool AFIDA::append_and_filter_output_files
	(const dynamic_string& filename_output,
	const char delimiter,
	const bool cleanup_files,
	class interface_window *view,
	dynamic_string& log)

// from MapLayerProximity.append_thread_files

// Reads line-by-line and discards duplicate records (where the same AFIDA-FarmlandFinder pair matched via different AFIDA_County threads).

{
	FILE* f, * thread_read;
	bool error = false;
	dynamic_string inconstant_filename = filename_output, column_names;
	std::vector <dynamic_string>::iterator thread_filename;
	std::set <std::pair <dynamic_string, dynamic_string>> unique_outputs;

	view->update_progress("Append and Filter", 1);
	if (fopen_s(&f, inconstant_filename.get_text_ascii(), "w") == 0) {

		char buffer[2048];
		int buffer_size;
		filename_struct fs;
		dynamic_string target_folder, target_filename, match_record;
		std::vector <dynamic_string> tokens;
		std::pair <dynamic_string, dynamic_string> match_record_ids;

		write_column_headers(-1, f, delimiter);

		for (thread_filename = thread_filenames.begin();
		thread_filename != thread_filenames.end();
		++thread_filename) {

			if (fopen_s(&thread_read, thread_filename->get_text_ascii(), "r") == 0) {

				view->update_progress(*thread_filename, 2);

				while (fgets(buffer, 2048, thread_read) != NULL) {
					match_record = buffer;
					match_record.tokenize (",\n", &tokens, true, false);

					match_record_ids.first = tokens [foreign_id_index]; // 6
					match_record_ids.second = tokens [domestic_id_index]; // 20

					if (unique_outputs.find (match_record_ids) == unique_outputs.end ()) {
						unique_outputs.insert (match_record_ids);
						buffer_size = strlen (buffer);
						fwrite(buffer, buffer_size, 1, f);
					}
				}

				fclose(thread_read);
			}

			fs.parse(&*thread_filename);
			fs.write_path(&target_folder);
			fs.write_filename(&target_filename);
			if (cleanup_files)
				remove_file(target_folder, target_filename, &error, log);
		}

		fclose(f);
	}
	else {
		error = true;
		log += "ERROR, can't open output file \"";
		log += filename_output;
		log += "\".\n";
	}
	return !error;
}

