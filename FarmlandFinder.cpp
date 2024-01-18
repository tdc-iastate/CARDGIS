#include "../util/utility_afx.h"
#include <vector>
#include <chrono>
#include <map>
#include <set>
#include <deque>
#include <thread>
#include <future>
#include <algorithm>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/state_names.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/shapefile_filter.h"
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_field_set.h"
#include "../Soils/rowset_ssurgo_horizon.h"
#include "../Soils/rowset_ssurgo_component.h"
#include "../Soils/rowset_ssurgo_mapunit.h"
#include "../subbasin_effect/thread_manager.h"
// #include "../clipper_ver6.2.1/cpp/clipper.hpp"
#include "../clipper_ver6.4.2/cpp/clipper.hpp"
#include "ClipperBuffer.h"
#include "FarmlandFinder.h"
#include "CountyMaster.h"
#include "IllinoisProductivityIndex.h"

dynamic_string packed_state_name
	(const int state_fips)

{
	int index, destination, len;
	char state_folder_name[40];
	dynamic_string packed;

	state_name (state_fips, state_folder_name, 40);

	len = strlen (state_folder_name);

	for (index = 0, destination = 0;
	index < len;
	++index)
		if (state_folder_name[index] != ' ')
			packed += state_folder_name [index];

	return packed;
}

void grain_location_callback
	(const map_object *point,
	const double distance_m,
	const char delimiter,
	dynamic_string &log)

// Called by work_MapLayerProximity

{
	// log += delimiter;
	// log.add_formatted ("%ld", point->id);

	log += delimiter;
	log += "\"";
	log += point->name; // ID
	log += "\"";

	log += delimiter;
	log += "\"";
	log += point->attributes_text [0]; // Name
	log += "\"";

	log += delimiter;
	log.add_formatted ("%.6lf", distance_m);
}

FarmlandFinderSale::FarmlandFinderSale ()
{
	clear ();
}

FarmlandFinderSale::FarmlandFinderSale
	(const FarmlandFinderSale &other)
{
	copy (other);
}

void FarmlandFinderSale::clear ()
{
	id = 0;
	id_text.clear ();
	Summary.clear ();
	GrossPrice = area_acres = latitude = longitude = 0.0;
	StateFIPS = CountyFIPS = -1;
	centroid.clear ();
}

void FarmlandFinderSale::copy
	(const FarmlandFinderSale &other)

{
	id = other.id;
	id_text = other.id_text;
	Summary = other.Summary;
	GrossPrice = other.GrossPrice;
	StateFIPS = other.StateFIPS;
	CountyFIPS = other.CountyFIPS;
	area_acres = other.area_acres;
	latitude = other.latitude;
	longitude = other.longitude;
	centroid = other.centroid;
}

FarmlandFinderSale FarmlandFinderSale::operator =
	(const FarmlandFinderSale &other)
{
	if (this != &other) {
		copy (other);
	}
	return (*this);
}

FarmlandFinderRepresentativeSoil::FarmlandFinderRepresentativeSoil()

{
}

FarmlandFinderRepresentativeSoil::FarmlandFinderRepresentativeSoil
	(const FarmlandFinderRepresentativeSoil&other)
{
	copy(other);
}
void FarmlandFinderRepresentativeSoil::clear()

{
	id = 0L;
	StateFIPS = 0;
	CountyFIPS = 0;
	area_m2 = 0.0f;
	ssurgo_mukey_and_area_m2.clear ();
}

void FarmlandFinderRepresentativeSoil::copy
	(const FarmlandFinderRepresentativeSoil&other)
{
	std::vector <std::pair <long, double>>::const_iterator other_ssurgo;

	id = other.id;
	StateFIPS = other.StateFIPS;
	CountyFIPS = other.CountyFIPS;
	area_m2 = other.area_m2;
	
	for (other_ssurgo = other.ssurgo_mukey_and_area_m2.begin();
	other_ssurgo != other.ssurgo_mukey_and_area_m2.end();
	++other_ssurgo)
		ssurgo_mukey_and_area_m2.push_back (*other_ssurgo);
}

FarmlandFinderRepresentativeSoil FarmlandFinderRepresentativeSoil::operator =
	(const FarmlandFinderRepresentativeSoil& other)

{
	if (this != &other)
		copy(other);

	return (*this);
}

FarmlandFinder::FarmlandFinder ()
{
	multithread_count = 12;
	proximity_pallette = NULL;
}


FarmlandFinder::~FarmlandFinder ()
{
	if (proximity_pallette)
		delete[] proximity_pallette;

}

bool FarmlandFinder::read_sales
	(std::map <dynamic_string, int> *filename_and_formats,
	CountyMaster *counties,
	const bool write_rows, // to match original spreadsheet rows
	const int limit,
	dynamic_string &log)

{
	FILE *f;
	char buffer[8192];
	std::map <dynamic_string, int>::iterator filename_sales;
	dynamic_string data, row_table, inconstant_filename;
	std::vector <dynamic_string> tokens;
	long id = 1, count_read, count_valid;
	FarmlandFinderSale sale;
	bool error = false, valid_record, skip_fragment, done = false;

	for (filename_sales = filename_and_formats->begin ();
	!done
	&& (filename_sales != filename_and_formats->end ());
	++filename_sales) {
		count_read = count_valid = 0;
		inconstant_filename = filename_sales->first;
		if (fopen_s (&f, inconstant_filename.get_text_ascii (), "r") == 0) {
			if (fgets (buffer, 8192, f)) {
				while (fgets (buffer, 8192, f)
				&& !done) {
					data = buffer;
					data.tokenize (",\n", &tokens, true, false);
					skip_fragment = false;
					++count_read;

					sale.clear ();
					valid_record = true;
					sale.id = id++;

					if (write_rows) {
						// 2021-02-18 Previous run used line length 1024 which caused some extra IDs for leftover line fragments
						// Increment ID here to match
						if (strlen (buffer) > 1024) {
							// ++id;
							// skip_fragment = true;
						}

						if (tokens.size () < 5) {
							// || (tokens [0].left(1) == "\n")
							// || (tokens [0].left(1) == ",")
							// || (tokens [0].get_length () == 0))
								// 2021-02-18 some Notes fields in "All State Auction Results.CSV" have linefeeds in them,
								// resulting in fragments appearing as subsequent lines:
								// "\n"
								// ",25-147N-44W,Polk,Northwest,Minnesota,47.52652534,-96.21387362"
							++id;
							skip_fragment = true;
						}
					}

					switch (filename_sales->second) {
					case FARMLANDFINDER_FORMAT_AUCTION_1:
						// 2021-02-25 re-exported with two new columns ID, ID_TEXT, State, County
						// ID, ID_TEXT, STATEIPS, COUNTYFIPS, Summary, Sale Date, Gross Price, $/Acre, Gross Acres,
						// Tillable Acres, CRP Acres, Soil Rating, Cash Rent - $/Acre, CRP Rent - $/Acre, Gross Taxes,
						// FarmlandFinder URL, Broker URL, Rent Source, Notes, S-T-R, County, District, State, Latitude, Longitude

						// Sale_ID,Summary,Sale Date,Gross Price,$/Acre,Gross Acres,
						// Tillable Acres,CRP Acres,Soil Rating,Cash Rent - $/Acre,CRP Rent - $/Acre,Gross Taxes,
						// FarmlandFinder URL,Broker URL,Rent Source,Notes,S-T-R,County,District,State,Latitude,Longitude
						// Auc_21_01,"1979 acres - Thomas County, Nebraska",2/17/2021,,,1979,0,,10.29,,,"$11,640.00",https://app.farmlandfinder.com/map?id=60014fd8526a876e9e97c601&area=1979&county=Thomas&state=Nebraska&str=17-23N-29W,https://agriaffiliates.com/properties/reynolds-ranch-absolute-auction/,,,17-23N-29W,Thomas,North,Nebraska,41.96313965,-100.7042471


						if (tokens.size () > 3) {
							sale.id = atoi (tokens[0].get_text_ascii ());
							sale.id_text = tokens [1];
							sale.Summary = tokens[4];

							// $740,316.00 
							if (tokens[6].get_length () > 0) {
								if (tokens[6].left (1) == "$")
									tokens[6] = tokens[6].mid (1);
								tokens[6].replace (",", "");
								sale.GrossPrice = atof (tokens[6].get_text_ascii ());
							}

							if (tokens.size () > 8) {
								if (tokens[8].get_length () > 0)
									sale.area_acres = atof (tokens[8].get_text_ascii ());

								if ((tokens.size () > 22)
								&& (tokens[22].get_length () > 0))
									sale.StateFIPS = state_FIPS_from_name (tokens[22].get_text_ascii ());
								else
									sale.StateFIPS = -1;

								if ((tokens.size () > 20)
								&& (tokens[20].get_length () > 0)) {
									tokens[20].trim_right (); // "Madison "
									sale.CountyFIPS = counties->get_FIPS (sale.StateFIPS, tokens[20].get_text_ascii ());
									if (sale.CountyFIPS == -1) {
										log += "Unknown county \"";
										log += tokens[20];
										log.add_formatted ("\" State %d\n", sale.StateFIPS);
									}
								}

								if ((tokens.size () > 23)
								&& (tokens[23].get_length () > 0)) {
									sale.latitude = atof (tokens[23].get_text_ascii ());

									if ((tokens.size () > 24)
									&& (tokens[24].get_length () > 0))
										sale.longitude = atof (tokens[24].get_text_ascii ());
									else
										valid_record = false;
								}
								else
									valid_record = false;
							}
							else
								valid_record = false;
						}
						else
							valid_record = false;
						break;
					case FARMLANDFINDER_FORMAT_AUCTION_2:
						// Sale_ID,Summary,Sale Date,Gross Price,$/Acre,Gross Acres,
						// Tillable Acres,CRP Acres,Soil Rating,Cash Rent - $/Acre,CRP Rent - $/Acre,Gross Taxes,
						// FarmlandFinder URL,Broker URL,Rent Source,Notes,S-T-R,County,District,State,Latitude,Longitude
						// Auc_21_01,"1979 acres - Thomas County, Nebraska",2/17/2021,,,1979,0,,10.29,,,"$11,640.00",https://app.farmlandfinder.com/map?id=60014fd8526a876e9e97c601&area=1979&county=Thomas&state=Nebraska&str=17-23N-29W,https://agriaffiliates.com/properties/reynolds-ranch-absolute-auction/,,,17-23N-29W,Thomas,North,Nebraska,41.96313965,-100.7042471


						if (tokens.size () > 2) {

							sale.id_text = tokens[0];
							sale.Summary = tokens[1];

							// $740,316.00 
							if (tokens[3].get_length () > 0) {
								if (tokens[3].left (1) == "$")
									tokens[3] = tokens[3].mid (1);
								tokens[3].replace (",", "");
								sale.GrossPrice = atof (tokens[3].get_text_ascii ());
							}

							if (tokens.size () > 5) {
								if (tokens[5].get_length () > 0)
									sale.area_acres = atof (tokens[5].get_text_ascii ());

								if ((tokens.size () > 19)
								&& (tokens[19].get_length () > 0))
									sale.StateFIPS = state_FIPS_from_name (tokens[19].get_text_ascii ());
								else
									sale.StateFIPS = -1;

								if ((tokens.size () > 17)
								&& (tokens[17].get_length () > 0)) {
									tokens[17].trim_right (); // "Madison "
									sale.CountyFIPS = counties->get_FIPS (sale.StateFIPS, tokens[17].get_text_ascii ());
									if (sale.CountyFIPS == -1) {
										log += "Unknown county \"";
										log += tokens[17];
										log.add_formatted ("\" State %d\n", sale.StateFIPS);
									}
								}

								if ((tokens.size () > 20)
								&& (tokens[20].get_length () > 0)) {
									sale.latitude = atof (tokens[20].get_text_ascii ());

									if ((tokens.size () > 21)
									&& (tokens[21].get_length () > 0))
										sale.longitude = atof (tokens[21].get_text_ascii ());
									else
										valid_record = false;
								}
								else
									valid_record = false;
							}
							else
								valid_record = false;
						}
						else
							valid_record = false;
						break;
					case FARMLANDFINDER_FORMAT_LISTING_1:
						// 2021-01-26 "land sales through real estate broker listings"
						// Summary,Status,Gross Acres,Tillable Acres,CRP Acres,Soil Rating,Gross Price,$/Acre,Cash Rent - $/Acre,CRP Rent - $/Acre,Gross Taxes,FarmlandFinder URL,Broker URL,S-T-R,County,District,State,Soils Rating Source,Rent Source,Sale Type,Sale Date,Last Updated,Latitude,Longitude
						// "10 acres - Adams County, Illinois",Listing Expired,10,,,,,,,,,,,,Adams,West,Illinois,,,Listing,,7/1/2020,,

						// Sale_ID,Summary,Status,Gross Acres,Tillable Acres,CRP Acres,Soil Rating,Gross Price,$/Acre,Cash Rent - $/Acre,CRP Rent - $/Acre,Gross Taxes,FarmlandFinder URL,Broker URL,S-T-R,County,District,State,Soils Rating Source,Rent Source,Sale Type,Sale Date,Last Updated,Latitude,Longitude
						// List_21_01,"158.96 acres - Kearney County, Nebraska",Listing Expired,158.96,144.28,,30.56,"$840,000.00","$5,284",,,"$5,453.00",https://app.farmlandfinder.com/map?id=6024e8c4876486eb6c6bcd27&area=158.96&county=Kearney&state=Nebraska&str=1-6N-14W,https://marshallauction.com/158-96-acres-pivot-irrigated-kearney-county-nebraska/,1-6N-14W,Kearney,South,Nebraska,,,Listing,,12/9/2020,40.51342,-98.84252

						if (tokens.size () == 24) {
							sale.id_text.format ("%ld_LIST", sale.id);
							sale.Summary = tokens[0];

							if (tokens[2].get_length () > 0)
								sale.area_acres = atof (tokens[2].get_text_ascii ());

							if (tokens[6].get_length () > 0) {
								if (tokens[6].left (1) == "$")
									tokens[6] = tokens[6].mid (1);
								tokens[6].replace (",", "");
								sale.GrossPrice = atof (tokens[6].get_text_ascii ());
							}

							if (tokens[16].get_length () > 0) {
								sale.StateFIPS = state_FIPS_from_name (tokens[16].get_text_ascii ());

								if (tokens[14].get_length () > 0) {
									tokens[14].trim_right (); // "Madison "
									sale.CountyFIPS = counties->get_FIPS (sale.StateFIPS, tokens[14].get_text_ascii ());
									if (sale.CountyFIPS == -1) {
										log += "Unknown county \"";
										log += tokens[14];
										log.add_formatted ("\" State %d\n", sale.StateFIPS);
									}
								}
							}
							else
								sale.StateFIPS = -1;

							if ((tokens[22].get_length () > 0)
							&& (tokens[23].get_length () > 0)) {
								sale.latitude = atof (tokens[22].get_text_ascii ());
								sale.longitude = atof (tokens[23].get_text_ascii ());
							}
							else
								valid_record = false;
						}
						else
							valid_record = false;
						break;
					case FARMLANDFINDER_FORMAT_LISTING_2:
						// 2021-02-19 "Listings_022021_New_SinceDec2020.csv"

						// Sale_ID,Summary,Status,Gross Acres,Tillable Acres,CRP Acres,Soil Rating,Gross Price,$/Acre,Cash Rent - $/Acre,CRP Rent - $/Acre,Gross Taxes,FarmlandFinder URL,Broker URL,S-T-R,County,District,State,Soils Rating Source,Rent Source,Sale Type,Sale Date,Last Updated,Latitude,Longitude
						// List_21_01,"158.96 acres - Kearney County, Nebraska",Listing Expired,158.96,144.28,,30.56,"$840,000.00","$5,284",,,"$5,453.00",https://app.farmlandfinder.com/map?id=6024e8c4876486eb6c6bcd27&area=158.96&county=Kearney&state=Nebraska&str=1-6N-14W,https://marshallauction.com/158-96-acres-pivot-irrigated-kearney-county-nebraska/,1-6N-14W,Kearney,South,Nebraska,,,Listing,,12/9/2020,40.51342,-98.84252

						if (tokens.size () == 25) {
							sale.id_text = tokens[0];
							sale.Summary = tokens[1];

							if (tokens[3].get_length () > 0)
								sale.area_acres = atof (tokens[3].get_text_ascii ());

							if (tokens[7].get_length () > 0) {
								if (tokens[7].left (1) == "$")
									tokens[7] = tokens[7].mid (1);
								tokens[7].replace (",", "");
								sale.GrossPrice = atof (tokens[7].get_text_ascii ());
							}

							if (tokens[17].get_length () > 0) {
								sale.StateFIPS = state_FIPS_from_name (tokens[17].get_text_ascii ());

								if (tokens[15].get_length () > 0) {
									tokens[15].trim_right (); // "Madison "
									sale.CountyFIPS = counties->get_FIPS (sale.StateFIPS, tokens[15].get_text_ascii ());
									if (sale.CountyFIPS == -1) {
										log += "Unknown county \"";
										log += tokens[15];
										log.add_formatted ("\" State %d\n", sale.StateFIPS);
									}
								}
							}
							else
								sale.StateFIPS = -1;

							if ((tokens[23].get_length () > 0)
							&& (tokens[24].get_length () > 0)) {
								sale.latitude = atof (tokens[23].get_text_ascii ());
								sale.longitude = atof (tokens[24].get_text_ascii ());
							}
							else
								valid_record = false;
						}
						else
							valid_record = false;
						break;
					case FARMLANDFINDER_FORMAT_NIEYAN:
						// 2021-08-27 CSV file exported from "I:\TDC\FarmlandFinder\transmission lines\farm_variable_distance\farm_variable_distance.dbf"
						// ORIG_FID, ID, ID_TEXT, Auction, Latitude, Longitude, gross_acres, buffdis, BUFF_DIST

						if (tokens.size () > 8) {
							sale.id = atoi (tokens[0].get_text_ascii ());
							sale.id_text = tokens [2];

							if (tokens[6].get_length () > 0)
								sale.area_acres = atof (tokens[6].get_text_ascii ());
							sale.latitude = atof (tokens[4].get_text_ascii ());
							sale.longitude = atof (tokens[5].get_text_ascii ());
						}
						else
							valid_record = false;
						break;
					case FARMLANDFINDER_FORMAT_REDUCED:
						if (!parse_reduced_format (&tokens, counties, &sale, log))
							valid_record = false;
						break;
					case FARMLANDFINDER_FORMAT_NYS:
						if (!parse_NYS(&tokens, counties, &sale, log))
							valid_record = false;
					}

					if (valid_record) {
						sale.centroid.x = (long) (sale.longitude * 1.0e6);
						sale.centroid.y = (long) (sale.latitude * 1.0e6);
						sales.push_back (sale);
					}

					if (valid_record)
						++count_valid;

					if (write_rows) {
						if (!skip_fragment) {
							row_table.add_formatted ("%ld", sale.id);
							row_table += "\t";
							row_table += sale.id_text;
							if (tokens.size () > 0) {
								row_table += "\t";
								if ((filename_sales->second == FARMLANDFINDER_FORMAT_AUCTION_1)
								|| (filename_sales->second == FARMLANDFINDER_FORMAT_LISTING_1)) {
									tokens[0].trim_right ();
									row_table += tokens[0];
								}
								else {
									tokens[1].trim_right ();
									row_table += tokens[1];
								}
								if (valid_record) {
									row_table += "\t";
									row_table.add_formatted ("%d", sale.StateFIPS);
									row_table += "\t";
									row_table.add_formatted ("%d", sale.CountyFIPS);
								}
							}
							row_table += "\n";
						}
						else
							log.add_formatted ("Skipped\t%ld\n", id);
					}
					if ((limit != -1)
					&& (id > limit))
						done = true;
				}
			}
			fclose (f);

			log += row_table;
			log += filename_sales->first;
			log.add_formatted ("\tCount read\t%ld\tCount valid\t%ld\n", count_read, count_valid);
		}
		else {
			log += "ERROR, can't read Farmland Sales file \"";
			log += filename_sales->first;
			log += "\".\n";
			error = true;
		}
	}
	return !error;
}

bool FarmlandFinder::parse_reduced_format
	(std::vector <dynamic_string>* tokens,
	CountyMaster* counties,
	FarmlandFinderSale *sale,
	dynamic_string& log)

// auction,listing,id,id_text,latitude,longitude,s_t_r
// 0,1,999,"999_LIST",41.9218,-88.4728,"30-40N-7E"
// 0,1,9999,"9999_LIST",41.7816,-84.6091,"13-8S-3W"

// 2022-08-16 State & County FIPS needed for subsequent run of frame_CARDGIS.OnFarmlandFinderSSURGO
// auction,listing,id,id_text,latitude,longitude,s_t_r,GrossPrice,AreaAcres,StateFIPS,CountyFIPS
// 0,1,999,"999_LIST",41.9218,-88.4728,"30-40N-7E",XX,XXX,xx,xx

{
	bool error = false;
	if (tokens->size() >= 7) {
		sale->id = atoi((*tokens)[2].get_text_ascii());
		sale->id_text = (*tokens)[3];

		sale->latitude = atof((*tokens)[4].get_text_ascii());
		sale->longitude = atof((*tokens)[5].get_text_ascii());

		if (tokens->size() >= 9) {
			sale->GrossPrice = atof((*tokens)[7].get_text_ascii());
			sale->area_acres = atof((*tokens)[8].get_text_ascii());
			if (tokens->size() >= 11) {
				sale->StateFIPS = state_FIPS_from_name((*tokens)[9].get_text_ascii());
				sale->CountyFIPS = counties->get_FIPS(sale->StateFIPS, (*tokens)[10].get_text_ascii());
			}
		}
	}
	else
		error = true;

	return !error;
}

bool FarmlandFinder::parse_NYS
	(std::vector <dynamic_string>* tokens,
	CountyMaster* counties,
	FarmlandFinderSale* sale,
	dynamic_string& log)

// 2023-08-14

//	ID_New	ID_New2	fid_1	geoid	county_name	print_key	total_sale_acres	prop_class_desc_last_roll	latitude	longitude
//	1	1_Dutchess	333178	6359 - 04 - 695420 - 0000636950594200	Dutchess	6359 - 04 - 695420 - 0000	27.2	Horse Farms	41.62443 - 73.88259
//	2	2_Dutchess	333553	6360 - 03 - 099220 - 0000630990602200	Dutchess	6360 - 03 - 099220 - 0000	82.24	Field Crops	41.62443 - 73.88259
//	3	3_Dutchess	325579	6260 - 02 - 658517 - 0000626580605170	Dutchess	6260 - 02 - 658517 - 0000	40.5	Truck Crops - Not Mucklands	41.62443 - 73.88259

{
	bool error = false;

	if (tokens->size() >= 10) {
		sale->id = atoi((*tokens)[0].get_text_ascii());
		sale->id_text = (*tokens)[1];

		if (is_number((*tokens)[8])
		&& is_number((*tokens)[9])) {
			sale->latitude = atof((*tokens)[8].get_text_ascii());
			sale->longitude = atof((*tokens)[9].get_text_ascii());

			if (tokens->size() >= 9) {
				sale->GrossPrice = 0.0;
				sale->area_acres = atof((*tokens)[6].get_text_ascii());
				if (tokens->size() >= 11) {
					sale->StateFIPS = 36; // New York
					sale->CountyFIPS = counties->get_FIPS(sale->StateFIPS, (*tokens)[4].get_text_ascii());
				}
			}
		}
		else {
			log += "Invalid lat/long text: \"";
			log += (*tokens) [8];
			log += ",";
			log += (*tokens)[9];
			log += "\", id=";
			log += (*tokens)[0];
			log += ".\n";
			error = true;
		}

	}
	else
		error = true;

	return !error;
}

bool FarmlandFinder::read_positions
	(const dynamic_string &filename_sales,
	const int limit,
	CountyMaster *counties,
	dynamic_string &log)

// 2021-07-01 reduced format with lat & long

// 2023-3-15 FARMLANDFINDER_FORMAT_REDUCED

{
	FILE *f;
	char buffer[8192];
	dynamic_string data, row_table, inconstant_filename;
	std::vector <dynamic_string> tokens;
	long id = 1, count_read, count_valid;
	FarmlandFinderSale sale;
	bool error = false, skip_fragment, done = false;

	count_read = count_valid = 0;
	inconstant_filename = filename_sales;
	if (fopen_s (&f, inconstant_filename.get_text_ascii (), "r") == 0) {
		if (fgets (buffer, 8192, f)) {
			while (fgets (buffer, 8192, f)
			&& !done) {
				data = buffer;
				data.tokenize (",\n", &tokens, true, false);
				skip_fragment = false;
				++count_read;

				sale.clear ();
				sale.id = id++;

				if (parse_reduced_format (&tokens, counties, &sale, log)) {
					sale.centroid.x = (long) (sale.longitude * 1.0e6);
					sale.centroid.y = (long) (sale.latitude * 1.0e6);
					sales.push_back (sale);
					++count_valid;
				}

				if ((limit != -1)
				&& (id > limit))
					done = true;
			}
		}
		fclose (f);

		log += row_table;
		log += filename_sales;
		log.add_formatted ("\tCount read\t%ld\tCount valid\t%ld\n", count_read, count_valid);
	}
	else {
		log += "ERROR, can't read Farmland Sales file \"";
		log += filename_sales;
		log += "\".\n";
		error = true;
	}
	return !error;
}

bool FarmlandFinder::create_point_layer
	(dynamic_map *map,
	dynamic_string &log)

{
	map_layer *layer;
	map_object *point;
	std::vector <FarmlandFinderSale>::iterator sale;

	layer = map->create_new (MAP_OBJECT_POINT);
	layer->name = "Sales";
	layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer->color = RGB (127, 127, 127);

	for (sale = sales.begin ();
	sale != sales.end ();
	++sale) {
		point = layer->create_new (MAP_OBJECT_POINT);
		point->id = sale->id;
		point->name = sale->id_text;
		point->latitude = (long) (sale->latitude * 1.0e6);
		point->longitude = (long) (sale->longitude * 1.0e6);
		layer->objects.push_back (point);
	}

	map->layers.push_back (layer);
	return true;
}


void FarmlandFinder::dump
	(dynamic_string &log)

{
	std::vector <FarmlandFinderSale>::iterator sale;

	log += "ID\tText ID\tSummary\tStateFIPS\tCountyFIPS\tGrossPrice\tArea Acres\tlatitude\tlongitude\n";

	for (sale = sales.begin ();
	sale != sales.end ();
	++sale) {
		log.add_formatted ("%ld", sale->id);
		log += "\t";
		log += sale->id_text;
		log += "\t";
		log += sale->Summary;
		log.add_formatted ("\t%d\t%d\t%.6lf\t%.6lf\t%.8lf\t%.8lf", sale->StateFIPS, sale->CountyFIPS, sale->GrossPrice, sale->area_acres, sale->latitude, sale->longitude);
		log += "\n";
	}

}

bool work_write_proximity
	(thread_work *job)

// [Microsoft][ODBC Microsoft Access Driver] Too many client tasks.

{
	bool error = false;
	std::vector <FarmlandFinderSale>::const_iterator sale;
	std::vector <map_layer *>::const_iterator layer;
	map_object *feature;
	double distance_meters;

	for (sale = ((thread_write_proximity *) job)->sales.begin ();
	sale != ((thread_write_proximity *) job)->sales.end ();
	++sale) {
		job->log.add_formatted ("%ld", sale->id);
		job->log += "\t";
		job->log += sale->id_text;
		job->log += "\t";
		job->log += sale->Summary;
		job->log.add_formatted ("\t%d\t%d\t%.6lf\t%.6lf\t%.8lf\t%.8lf", sale->StateFIPS, sale->CountyFIPS, sale->GrossPrice, sale->area_acres, sale->latitude, sale->longitude);

		for (layer = ((thread_write_proximity *) job)->proximity_layers->begin ();
		layer != ((thread_write_proximity *) job)->proximity_layers->end ();
		++layer) {
			if ((feature = (*layer)->find_nearest_object (sale->centroid, &distance_meters)) != NULL)
				job->log.add_formatted ("\t%ld\t%.6lf", feature->id, distance_meters);
			else
				job->log += "\t\t";
		}

		job->log += "\n";
	}

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

void FarmlandFinder::write_proximity_table
	(dynamic_string &output_filename,
	interface_window *view,
	dynamic_string &log)

{
	std::vector <FarmlandFinderSale>::iterator sale;
	thread_manager threader;
	std::vector <map_layer *>::iterator layer;
	std::map <long, thread_work *> jobs;
	std::map <long, thread_work *>::iterator job_iterator;
	thread_write_proximity *job;
	thread_manager_statistics stats;
	int thread_index, job_count;
	long sales_index;

	job_count = 100;
	threader.thread_count = 22;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1; // 5;
	stats.reset (threader.thread_count);
	threader.statistics = &stats;

	for (thread_index = 0; thread_index < job_count; ++thread_index) {
		job = new thread_write_proximity;
		job->id = thread_index + 1;
		job->work_function = &work_write_proximity;
		job->proximity_layers = &proximity_layers;
		job->view = view;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	for (sale = sales.begin (), sales_index = 0;
	sale != sales.end ();
	++sale, ++sales_index) {
		job_iterator = jobs.find ((sales_index % job_count) + 1);
		((thread_write_proximity *) job_iterator->second)->sales.push_back (*sale);
	}

	
	view->set_data (&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
	threader.run (&jobs, view, log);
	view->set_data (NULL);

	view->update_status_bar ("Threader run complete");

	FILE *f;
	dynamic_string progress;

	if (fopen_s (&f, output_filename.get_text_ascii (), "w") == 0) {

		fprintf (f, "ID\tText ID\tSummary\tStateFIPS\tCountyFIPS\tGrossPrice\tArea Acres\tlatitude\tlongitude");
		for (layer = proximity_layers.begin ();
		layer != proximity_layers.end ();
		++layer) {
			fprintf (f, "\tNearest ");
			fprintf (f, "%s", (*layer)->name.get_text_ascii ());
			fprintf (f, "\tdistance, m");
		}
		fprintf (f, "\n");

		for (job_iterator = jobs.begin ();
		job_iterator != jobs.end ();
		++job_iterator) {
			progress.format ("Writing output job %ld.\n", job_iterator->first);
			view->update_scroll (progress);
			fwrite (job_iterator->second->log.get_text_ascii (), job_iterator->second->log.get_length (), 1, f);
			job_iterator->second->log.clear ();
		}
		fclose (f);
	}
	else
		log += "ERROR, can't open output file.\n";

	// stats.write_time_summary (log);
}

bool FarmlandFinder::write_ssurgo_county
	(dynamic_string &filename_output,
	dynamic_string &table,
	const int state_fips,
	const char delimiter,
	const bool write_header,
	dynamic_string &log)

{
	FILE* file_output;
	bool error = false;

	if (fopen_s(&file_output, filename_output.get_text_ascii(), "w") == 0) {
		if (write_header) {
			fprintf(file_output, "State FIPS%cCounty FIPS%cSale%cSoil ID%cMukey%cMusym%cName%cName 1%cTexture%cSlope Description%cCSR%cComponent Name%cTaxonomic Class%cRepresentative Slope",
			delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
			fprintf(file_output, "%cgSSURGO frmlndcl", delimiter);
			fprintf(file_output, "%cnccpi3corn%cnccpi3soy%cnccpi3sg%cnccpi3all", delimiter, delimiter, delimiter, delimiter);
			if (state_fips == 17)
				fprintf(file_output, "%cIllinois PI", delimiter);
			fprintf(file_output, "%c\"Exact Area, m^2\"\n", delimiter);
		}
		fwrite(table.get_text_ascii(), table.get_length(), 1, file_output);

		fclose(file_output);
	}
	else {
		log += "ERROR, can't open output filename \"";
		log += filename_output;
		log += "\".\n";
		error = true;
	}
	return !error;
}

map_object *FarmlandFinder::create_field_circle_area
	(map_layer *layer_fields,
	const double target_area_acres,
	const FarmlandFinderSale *sale)

// create 72 points, every 5 degrees

{
	logical_coordinate centerpoint (sale->longitude, sale->latitude, 0.0);
	double area_meters_2, radius_meters;
	map_polygon *circle;
	long lat, lon;
	int degrees;
	double radians, lat_offset, lon_offset;

	area_meters_2 = target_area_acres * M2_PER_ACRE;

	// a = pi*r^2 -> a/pi = r^2
	radius_meters = sqrt (area_meters_2 / PI);

	circle = (map_polygon *) layer_fields->create_new (MAP_OBJECT_POLYGON);
	circle->id = sale->id;

	lat_offset = offset_latitude_meters (centerpoint, radius_meters);
	lon_offset = offset_longitude_meters (centerpoint, radius_meters);

	for (degrees = 0; degrees < 360; degrees += 5) {
		radians = (double) degrees * DEGREES_TO_RADIANS;
		lon = (long) ((centerpoint.x + lon_offset * sin (radians)) * 1.0e6);
		lat = (long) ((centerpoint.y + lat_offset * cos (radians)) * 1.0e6);
		circle->add_point (lat, lon);
	}

	/*
	// N
	lon = (long) (centerpoint.x * 1.0e6);
	lat = (long) ((centerpoint.y + lat_offset) * 1.0e6);
	circle->add_point (lat, lon);

	// E
	lon = (long) ((centerpoint.x + lon_offset) * 1.0e6);
	lat = (long) (centerpoint.y * 1.0e6);
	circle->add_point (lat, lon);

	// S
	lon = (long) (centerpoint.x * 1.0e6);
	lat = (long) ((centerpoint.y - lat_offset) * 1.0e6);
	circle->add_point (lat, lon);

	// W
	lon = (long) ((centerpoint.x - lon_offset) * 1.0e6);
	lat = (long) (centerpoint.y * 1.0e6);
	circle->add_point (lat, lon);
	*/

	return circle;
}

map_object *FarmlandFinder::create_measured_circle
	(const double target_area_m2,
	double *final_area_m2,
	const FarmlandFinderSale *sale,
	map_layer *layer_circles,
	dynamic_string &log)

// Repeatedly guess and measure using ClipperLib's area function

{
	bool close_enough;
	double circle_area_m2 = 0, max_difference = 1.0, difference_m2, use_area_acres, difference_fraction, last_area_increasing, last_area_decreasing;
	map_object *circle = NULL;
	int try_count = 0;

	close_enough = false;
	last_area_increasing = -9999;
	last_area_decreasing = 9999;
	use_area_acres = target_area_m2 / M2_PER_ACRE;
	while (!close_enough) {
		circle = create_field_circle_area (layer_circles, use_area_acres, sale);
		circle_area_m2 = compute_area_m2 (circle, log);
		difference_m2 = target_area_m2 - circle_area_m2;

		if (++try_count > 100)
			close_enough = true;
		else
			// as difference_m2 alternates between decreasing positive values and increasing negative values, track the approach
			if ((abs (last_area_increasing - last_area_decreasing) / target_area_m2) < 0.025)
				close_enough = true;
			else {
				difference_fraction = difference_m2 / target_area_m2;

				if (difference_m2 > max_difference) {
					use_area_acres += difference_fraction * use_area_acres;
					log.add_formatted ("%.1lf\t%.4lf\t%.4lf\t%.4lf\t-\n", sale->area_acres, difference_m2, difference_fraction, use_area_acres);
					last_area_increasing = use_area_acres;
					delete circle;
				}
				else
					if (difference_m2 < -max_difference) {
						use_area_acres += difference_fraction * use_area_acres;
						log.add_formatted ("%.1lf\t%.4lf\t%.4lf\t%.4lf\t+\n", sale->area_acres, difference_m2, difference_fraction, use_area_acres);
						last_area_decreasing = use_area_acres;

						delete circle;
					}
					else
						close_enough = true;
		}
	}
	*final_area_m2 = circle_area_m2;
	return circle;
}

map_layer *FarmlandFinder::create_circles_area
	(dynamic_map *map,
	const bool measure_and_adjust,
	dynamic_string &log)

{
	map_layer *layer_fields = NULL;
	std::vector <FarmlandFinderSale>::iterator sale;
	map_object *circle = NULL;
	double target_area_m2, circle_area_m2;
	dynamic_string area_log;

	// Create map layers for dialog_river
	layer_fields = map->create_new (MAP_OBJECT_POLYGON);
	layer_fields->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT; // Draw line from first node to last
	layer_fields->initialize_attributes = true; // clear all attributes as polygons are created
	layer_fields->attribute_count_numeric = 8;
	layer_fields->column_names_numeric.push_back ("Price");
	layer_fields->column_names_numeric.push_back ("Area Acres");
	layer_fields->column_names_numeric.push_back ("State FIPS");
	layer_fields->column_names_numeric.push_back ("County FIPS");
	layer_fields->column_names_numeric.push_back ("CenterX");
	layer_fields->column_names_numeric.push_back ("CenterY");
	layer_fields->column_names_numeric.push_back ("Radius, m");
	layer_fields->column_names_numeric.push_back ("Area Check, acres");

	layer_fields->attribute_count_text = 1;
	layer_fields->column_names_text.push_back ("Summary");

	layer_fields->name = "FarmlandFinder Sales"; // Was importer->filename_source.  Needed for OnShowRiver
	layer_fields->color = RGB (128, 128, 128);
	layer_fields->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	map->layers.push_back (layer_fields);

	area_log += "Area Acres\tDifference m2\tDifference Fraction\tuse_area_acres\t+/-\n";

	log += "ID\tAreaAcres\tAreaMeters\n";

	for (sale = sales.begin ();
	sale != sales.end ();
	++sale) {
		if (measure_and_adjust) {
			target_area_m2 = sale->area_acres * M2_PER_ACRE;
			circle = create_measured_circle (target_area_m2, &circle_area_m2, &*sale, layer_fields, area_log);
		}
		else {
			circle = create_field_circle_area (layer_fields, sale->area_acres, &*sale);
			circle_area_m2 = compute_area_m2 (circle, log);
		}

		circle->name = sale->id_text.left (DBASE_TEXT_FIELD_LENGTH - 1);
		circle->attributes_text [0] = sale->Summary.left (DBASE_TEXT_FIELD_LENGTH - 1);
		circle->attributes_numeric[0] = sale->GrossPrice;
		circle->attributes_numeric[1] = sale->area_acres;
		circle->attributes_numeric[FARMLANDFINDER_SALE_ATTRIBUTE_INDEX_STATE_FIPS] = sale->StateFIPS;
		circle->attributes_numeric[FARMLANDFINDER_SALE_ATTRIBUTE_INDEX_COUNTY_FIPS] = sale->CountyFIPS;
		circle->attributes_numeric[4] = sale->longitude;
		circle->attributes_numeric[5] = sale->latitude;
		circle->attributes_numeric[6] = sqrt(circle_area_m2 / PI);
		circle->attributes_numeric[7] = circle_area_m2 / M2_PER_ACRE;
		log.add_formatted ("%ld\t%.4lf\t%.4lf\n", circle->id, circle->attributes_numeric[1], circle->attributes_numeric[6]);
		circle->set_extent ();
		layer_fields->objects.push_back (circle);
	}

	log += area_log;

	return layer_fields;
}

double FarmlandFinder::compute_area_m2
	(map_object *circle,
	dynamic_string &log)

// 2021-08-27 Use ClipperLib's area to compare to sale area to see if create_field_circle_area correctly sized the circle

{
	ClipperBuffer clipper;
	ClipperLib::Paths trail;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	double circle_area_m2 = 0.0;

	clipper.map_object_to_Path (circle, &trail, log);

	for (paths_path = trail.begin ();
	paths_path != trail.end ();
	++paths_path)
		// clockwise circle points cause area negation
		circle_area_m2 += area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);

	return -circle_area_m2;
}

map_object *FarmlandFinder::create_field_circle_radius
	(const double radius_meters,
	map_layer *layer_fields,
	FarmlandFinderSale *sale)

// create 72 points, every 5 degrees

{
	logical_coordinate centerpoint (sale->longitude, sale->latitude, 0.0);
	map_polygon *circle;
	long lat, lon;
	int degrees;
	double radians, lat_offset, lon_offset;

	circle = (map_polygon *) layer_fields->create_new (MAP_OBJECT_POLYGON);
	circle->id = sale->id;

	lat_offset = offset_latitude_meters (centerpoint, radius_meters);
	lon_offset = offset_longitude_meters (centerpoint, radius_meters);

	for (degrees = 0; degrees < 360; degrees += 5) {
		radians = (double) degrees * DEGREES_TO_RADIANS;
		lon = (long) ((centerpoint.x + lon_offset * sin (radians)) * 1.0e6);
		lat = (long) ((centerpoint.y + lat_offset * cos (radians)) * 1.0e6);
		circle->add_point (lat, lon);
	}

	/*
	// N
	lon = (long) (centerpoint.x * 1.0e6);
	lat = (long) ((centerpoint.y + lat_offset) * 1.0e6);
	circle->add_point (lat, lon);

	// E
	lon = (long) ((centerpoint.x + lon_offset) * 1.0e6);
	lat = (long) (centerpoint.y * 1.0e6);
	circle->add_point (lat, lon);

	// S
	lon = (long) (centerpoint.x * 1.0e6);
	lat = (long) ((centerpoint.y - lat_offset) * 1.0e6);
	circle->add_point (lat, lon);

	// W
	lon = (long) ((centerpoint.x - lon_offset) * 1.0e6);
	lat = (long) (centerpoint.y * 1.0e6);
	circle->add_point (lat, lon);
	*/

	return circle;
}


map_layer *FarmlandFinder::create_circles_radius
	(const double radius_miles,
	dynamic_map *map,
	const bool measure_and_adjust,
	dynamic_string &log)

// 2021-07-01 Create 5 or 10 mile circles around each sale

{
	map_layer *layer_fields = NULL;
	std::vector <FarmlandFinderSale>::iterator sale;
	map_object *circle = NULL;
	bool close_enough;
	double circle_area_m2 = 0.0, use_radius_meters = 0.0, intended_area_m2, difference_m2, difference_fraction, max_difference = 0.1;

	// Create map layers for dialog_river
	layer_fields = map->create_new (MAP_OBJECT_POLYGON);
	layer_fields->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT; // Draw line from first node to last
	layer_fields->initialize_attributes = true; // clear all attributes as polygons are created
	/*
	layer_fields->attribute_count_numeric = 3;
	layer_fields->column_names_numeric.push_back ("CenterX");
	layer_fields->column_names_numeric.push_back ("CenterY");
	layer_fields->column_names_numeric.push_back ("Area Check, acres");

	layer_fields->attribute_count_text = 1;
	layer_fields->column_names_text.push_back ("Summary");
	*/

	layer_fields->attribute_count_numeric = 4;
	layer_fields->column_names_numeric.push_back ("CenterX");
	layer_fields->column_names_numeric.push_back ("CenterY");
	layer_fields->column_names_numeric.push_back ("Radius, m");
	layer_fields->column_names_numeric.push_back ("Area Check, acres");

	layer_fields->attribute_count_text = 1;
	layer_fields->column_names_text.push_back ("Summary");

	layer_fields->name.format ("FarmlandFinder %.1lf mile radius", radius_miles); // Was importer->filename_source.  Needed for OnShowRiver
	layer_fields->color = RGB (128, 128, 128);
	layer_fields->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	map->layers.push_back (layer_fields);

	intended_area_m2 = (PI * radius_miles * radius_miles) * 640.0;

	for (sale = sales.begin ();
	sale != sales.end ();
	++sale) {
		use_radius_meters = radius_miles * METERS_PER_MILE;
		if (measure_and_adjust) {
			close_enough = false;
			while (!close_enough) {
				circle = create_field_circle_radius (use_radius_meters, layer_fields, &*sale);
				circle_area_m2 = compute_area_m2 (circle, log);
				difference_m2 = intended_area_m2 - circle_area_m2;

				// Use smaller difference fraction because of relationship between radius and acres
				difference_fraction = (difference_m2 / PI) / intended_area_m2;
				if (difference_m2 > max_difference) {
					use_radius_meters += (difference_fraction * use_radius_meters);
					log.add_formatted ("%ld\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t-\n", sale->id, intended_area_m2, difference_m2, difference_fraction, use_radius_meters);
					delete circle;
				}
				else
					if (difference_m2 < -max_difference) {
						use_radius_meters += (difference_fraction * use_radius_meters);
						log.add_formatted ("%ld\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t+\n", sale->id, intended_area_m2, difference_m2, difference_fraction, use_radius_meters);
						delete circle;
					}
					else
						close_enough = true;
			}
		}
		else {
			circle = create_field_circle_radius (use_radius_meters, layer_fields, &*sale);
			circle_area_m2 = compute_area_m2 (circle, log);
		}

		circle->name = sale->id_text.left (DBASE_TEXT_FIELD_LENGTH - 1);
		circle->attributes_numeric[0] = sale->longitude;
		circle->attributes_numeric[1] = sale->latitude;
		circle->attributes_numeric[2] = sqrt(circle_area_m2 / PI);
		circle->attributes_numeric[3] = circle_area_m2 / M2_PER_ACRE;
		circle->set_extent ();
		layer_fields->objects.push_back (circle);
	}

	return layer_fields;
}

map_layer *FarmlandFinder::create_circles_radius_beyond
	(const double add_radius_miles,
	dynamic_map *map,
	const bool measure_and_adjust,
	dynamic_string &log)

// 2021-09-09 Create circles around each sale 1/2 or 1 mile beyond normal area radius

{
	map_layer *layer_fields = NULL;
	std::vector <FarmlandFinderSale>::iterator sale;
	map_object *circle = NULL;
	double area_meters_2, circle_area_m2 = 0.0;
	double use_radius_meters = 0.0, intended_area_m2;

	// Create map layers for dialog_river
	layer_fields = map->create_new (MAP_OBJECT_POLYGON);
	layer_fields->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT; // Draw line from first node to last
	layer_fields->initialize_attributes = true; // clear all attributes as polygons are created
	/*
	layer_fields->attribute_count_numeric = 4;
	layer_fields->column_names_numeric.push_back ("CenterX");
	layer_fields->column_names_numeric.push_back ("CenterY");
	layer_fields->column_names_numeric.push_back ("Radius, m");
	layer_fields->column_names_numeric.push_back ("Area Check, acres");

	layer_fields->attribute_count_text = 1;
	layer_fields->column_names_text.push_back ("Summary");
	*/
	layer_fields->attribute_count_numeric = 4;
	layer_fields->column_names_numeric.push_back ("CenterX");
	layer_fields->column_names_numeric.push_back ("CenterY");
	layer_fields->column_names_numeric.push_back ("Radius, m");
	layer_fields->column_names_numeric.push_back ("Area Check, acres");

	layer_fields->attribute_count_text = 1;
	layer_fields->column_names_text.push_back ("Summary");
	layer_fields->name.format ("FarmlandFinder +%.1lf mile radius", add_radius_miles); // Was importer->filename_source.  Needed for OnShowRiver
	layer_fields->color = RGB (128, 128, 128);
	layer_fields->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	map->layers.push_back (layer_fields);

	for (sale = sales.begin ();
	sale != sales.end ();
	++sale) {
		area_meters_2 = sale->area_acres * M2_PER_ACRE;
		use_radius_meters = sqrt (area_meters_2 / PI) + (add_radius_miles * METERS_PER_MILE);
		if (measure_and_adjust) {
			intended_area_m2 = (PI * use_radius_meters * use_radius_meters);

			circle = create_measured_circle (intended_area_m2, &circle_area_m2, &*sale, layer_fields, log);
		}
		else {
			circle = create_field_circle_radius (use_radius_meters, layer_fields, &*sale);
			circle_area_m2 = compute_area_m2 (circle, log);
		}

		circle->name = sale->id_text.left (DBASE_TEXT_FIELD_LENGTH - 1);
		circle->attributes_numeric[0] = sale->longitude;
		circle->attributes_numeric[1] = sale->latitude;
		circle->attributes_numeric[2] = sqrt(circle_area_m2 / PI);
		circle->attributes_numeric[3] = circle_area_m2 / M2_PER_ACRE;
		circle->set_extent ();
		layer_fields->objects.push_back (circle);
	}

	return layer_fields;
}

bool work_read_mapunit
	(thread_work *job)

// [Microsoft][ODBC Microsoft Access Driver] Too many client tasks.

{
	bool error = false;
	odbc_database_access *db_odbc = new odbc_database_access;
	odbc_database_credentials creds;
	SSURGO_mapunit set_mu;
	std::map <short, std::vector <CountyMasterRecord>>::iterator state_entry;
	long mukey;

	creds.set_database_filename (((thread_read_mapunit *) job)->filename_ssurgo_mdb);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (((odbc_database_access *) db_odbc)->open (&creds, NULL, job->log)) {

		set_mu.initialize_field_set ();
		if (set_mu.field_set->open_read (db_odbc, job->log)) {
			if (set_mu.field_set->move_first (job->log)) {
				do {
					mukey = atol (set_mu.mukey);
					((thread_read_mapunit *) job)->mapunits_by_mukey.insert (std::pair <long, SSURGO_mapunit> (mukey, set_mu));
				} while (set_mu.field_set->move_next (job->log));
			}
			set_mu.field_set->close ();
		}

		db_odbc->close ();
	}
	else
		error = true;

	delete db_odbc;
	db_odbc = NULL;

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		job->end = std::chrono::system_clock::now ();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool FarmlandFinder::read_ssurgo_components
	(odbc_database *db_odbc,
	std::map <long, SSURGO_mapunit>* mapunits_by_mukey,
	std::map <long, SSURGO_component>* components_by_cokey, // May be NULL if not reading horizons
	interface_window *view,
	dynamic_string &log) const

// mapunit and component
	 
{
	bool error = false;
	SSURGO_component set_comp;
	std::map <long, SSURGO_mapunit>::iterator mapunit;
	long mukey, cokey;

	set_comp.initialize_field_set ();
	if (set_comp.field_set->open_read (db_odbc, log)) {
		if (set_comp.field_set->move_first (log)) {
			do {
				mukey = atol (set_comp.mukey);
				if ((mapunit = mapunits_by_mukey->find (mukey)) != mapunits_by_mukey->end ()) {
					mapunit->second.components.push_back (set_comp);

					if (components_by_cokey) {
						cokey = atol (set_comp.cokey);
						ASSERT(cokey < 999999999);
						components_by_cokey->insert(std::pair <long, SSURGO_component>(cokey, set_comp));
					}
				}
				else
					error = true;

			} while (!error
			&& set_comp.field_set->move_next (log));
		}
		set_comp.field_set->close ();

		for (mapunit = mapunits_by_mukey->begin ();
		mapunit != mapunits_by_mukey->end ();
		++mapunit)
			std::sort (mapunit->second.components.begin (), mapunit->second.components.end ());
	}
	else
		error = true;

	return !error;
}

bool FarmlandFinder::read_ssurgo_horizons
	(odbc_database* db_odbc,
	std::map <long, SSURGO_mapunit>* mapunits_by_mukey,
	std::map <long, SSURGO_component>* components_by_cokey,
	interface_window* view,
	dynamic_string& log) const

{
	bool error = false;
	SSURGO_horizon set_layer;
	std::map <long, SSURGO_component>::iterator component;
	std::map <long, SSURGO_mapunit>::iterator mapunit;
	SSURGO_component *insertion_component;
	long mukey, cokey;

	set_layer.initialize_field_set();
	if (set_layer.field_set->open_read(db_odbc, log)) {
		if (set_layer.field_set->move_first(log)) {
			do {
				cokey = atol(set_layer.cokey);
				if ((component = components_by_cokey->find(cokey)) != components_by_cokey->end()) {
					// must add this horizon to the soil in mapunits_by_mukey
					mukey = atol(component->second.mukey);
					if ((mapunit = mapunits_by_mukey->find (mukey)) != mapunits_by_mukey->end()) {
						if ((insertion_component = mapunit->second.match_cokey(set_layer.cokey)) != NULL)
							insertion_component->horizons.push_back (set_layer);
						else {
							log += "ERROR, component ";
							log += set_layer.cokey;
							log += " not found in mapunit ";
							log += component->second.mukey;
							log += "\n";
							error = true;
						}
					}
					else {
						log += "ERROR, mapunit ";
						log += component->second.mukey;
						log += " not found for horizon.\n";
						error = true;
					}
				}
				else {
					log += "ERROR, component ";
					log += set_layer.cokey;
					log += " not found for horizon.\n";
					error = true;
				}

			} while (!error
			&& set_layer.field_set->move_next(log));
		}
		set_layer.field_set->close();

		for (component = components_by_cokey->begin();
		component != components_by_cokey->end();
		++component)
			std::sort(component->second.horizons.begin(), component->second.horizons.end());
	}
	else
		error = true;

	return !error;
}

bool FarmlandFinder::read_ssurgo_database
	(const dynamic_string &filename_ssurgo_mdb,
	std::map <long, SSURGO_mapunit> *mapunits_by_mukey,
	interface_window *view,
	dynamic_string &log) const

// mapunit and component
	 
{
	bool error = false;
	odbc_database_access *db_odbc = new odbc_database_access;
	odbc_database_credentials creds;
	SSURGO_mapunit set_mu;
	long mukey;

	creds.set_database_filename (filename_ssurgo_mdb);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (((odbc_database_access *) db_odbc)->open (&creds, view, log)) {
		view->update_status_bar ("DB open");

		set_mu.initialize_field_set ();
		if (set_mu.field_set->open_read (db_odbc, log)) {
			if (set_mu.field_set->move_first (log)) {
				do {
					mukey = atol (set_mu.mukey);
					// mukey is char [31], but MAXLONG is 0x7fffffff, 2,147,483,647
					ASSERT (mukey < 999999999);
					mapunits_by_mukey->insert (std::pair <long, SSURGO_mapunit> (mukey, set_mu));
				} while (set_mu.field_set->move_next (log));
			}
			set_mu.field_set->close ();
		}

		read_ssurgo_components(db_odbc, mapunits_by_mukey, NULL, view, log);

		db_odbc->close ();
	}
	else
		error = true;

	delete db_odbc;
	db_odbc = NULL;

	return !error;
}

bool FarmlandFinder::read_ssurgo_database
	(const dynamic_string& filename_ssurgo_mdb,
	std::map <long, SSURGO_mapunit>* mapunits_by_mukey,
	std::map <long, SSURGO_component>* components_by_mukey,
	interface_window* view,
	dynamic_string& log) const

// mapunit and component and horizon

{
	bool error = false;
	odbc_database_access* db_odbc = new odbc_database_access;
	odbc_database_credentials creds;
	SSURGO_mapunit set_mu;
	std::map <long, SSURGO_component> components_by_cokey;
	long mukey;

	creds.set_database_filename(filename_ssurgo_mdb);
	creds.set_driver_name(ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.type = ODBC_ACCESS;
	creds.set_hostname(".");

	if (((odbc_database_access*)db_odbc)->open(&creds, view, log)) {
		if (view)
			view->update_progress ("Reading mapunits", 2);

		set_mu.initialize_field_set();
		if (set_mu.field_set->open_read(db_odbc, log)) {
			if (set_mu.field_set->move_first(log)) {
				do {
					mukey = atol(set_mu.mukey);
					// mukey is char [31], but MAXLONG is 0x7fffffff, 2,147,483,647
					ASSERT(mukey < 999999999);
					mapunits_by_mukey->insert(std::pair <long, SSURGO_mapunit>(mukey, set_mu));
				} while (set_mu.field_set->move_next(log));
			}
			set_mu.field_set->close();
		}

		if (view)
			view->update_progress("Reading components", 2);
		read_ssurgo_components(db_odbc, mapunits_by_mukey, &components_by_cokey, view, log);
		if (view)
			view->update_progress("Reading horizons", 2);
		read_ssurgo_horizons(db_odbc, mapunits_by_mukey, &components_by_cokey, view, log);

		db_odbc->close();
	}
	else
		error = true;

	delete db_odbc;
	db_odbc = NULL;

	return !error;
}

bool FarmlandFinder::read_proximity_shapefile
	(const dynamic_string &path_proximity,
	const dynamic_string &layer_name,
	const dynamic_string &id_field_name,
	const dynamic_string &name_field_name,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &error_message)

// Read shapefile & add to map

{
	map_layer *layer_roads;
	importer_shapefile shapefile;
	int color_index = map->layers.size ();

	if (proximity_pallette == NULL)
		proximity_pallette = make_palette_10 ();

	layer_roads = map->create_new (MAP_OBJECT_POLYGON);
	layer_roads->name = layer_name;
	layer_roads->color = proximity_pallette [color_index];
	map->layers.push_back(layer_roads);

	shapefile.filename_source = path_proximity;

	if (id_field_name.get_length () > 0)
		shapefile.id_field_name = id_field_name;

	if (name_field_name.get_length () > 0)
		shapefile.name_field_name = name_field_name;

	shapefile.projection_from_prj_file = true;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	view->update_progress (layer_name, 1);
	if (shapefile.import (layer_roads, NULL, map, view, error_message)) {

		if (layer_roads->type == MAP_OBJECT_POINT) {
			layer_roads->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			layer_roads->symbol = MAP_OBJECT_SYMBOL_PLUS;
		}
		else
			layer_roads->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;

		proximity_layers.push_back (layer_roads);
		return true;
	}
	else
		return false;
}

map_layer *FarmlandFinder::import_ssurgo_county_shapefile
	(const dynamic_string &filename_ssurgo_shapefile,
	const dynamic_string &ssurgo_county_name,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

//  AREASYMBOL,C,20
//  SPATIALVER,N,10,0
//  MUSYM,C,6
//  MUKEY,C,30
{
	map_layer *layer_ssurgo_county;
	importer_shapefile shapefile;

	layer_ssurgo_county = map->create_new (MAP_OBJECT_POLYGON);
	layer_ssurgo_county->name = ssurgo_county_name;
	layer_ssurgo_county->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_ssurgo_county->color = RGB (0, 255, 0);
	// 2021-01-13 layer_ssurgo_county will be deleted after each county, so don't add to map
	// map->add (layer_ssurgo_county);

	shapefile.filename_source = filename_ssurgo_shapefile;
	// MUKEY is defined as C,30, which trips importer_shapefile.id_field_overflow
	// shapefile.id_field_name = "MUKEY";
	shapefile.projection_from_prj_file = true;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	 // MUKEY winds up at attributes_text [2]

	view->update_progress ("Importing SSURGO Shapefile", 1);
	if (shapefile.import (layer_ssurgo_county, NULL, map, NULL, log)) {
		return layer_ssurgo_county;
	}
	else
		return NULL;
}

dynamic_string parse_mapunit_name
	(const char *muname,
	const char delimiter)

// Wendong: convert the mapunit name into columns using “,” as delimiter, so that the first line of your example
// would become two columns Mandeville silt loam, and another field 2 to 5 percent slopes. Then you could use the
// regular expression and pick the words after the first space or remove the first word, something like that?      

// "Name 1", "Texture", "Slope Description"
// Chelsea loamy fine sand, 9 to 18 percent slopes
// Kenyon loam, 5 to 9 percent slopes, moderately eroded

{
	dynamic_string name = muname, component_name, texture, slope, output_text;
	std::vector <dynamic_string> tokens;
	std::vector <dynamic_string>::iterator word;

	name.tokenize (",", &tokens);

	word = tokens.begin ();
	if (++word != tokens.end ()) {
		do {
			slope += " ";
			slope += *word;
		} while (++word != tokens.end ());

		slope.trim_left (); // remove space that followed comma, plus leading space added during loop

		// Assume component name is only 1 word followed by texture
		name = tokens[0];
		name.tokenize (" ", &tokens);

		if ((word = tokens.begin ()) != tokens.end ()) {
			component_name = *word;
			if (++word != tokens.end ()) {
				do {
					texture += " ";
					texture += *word;
				} while (++word != tokens.end ());
				texture.trim_left (); // remove leading space added during loop
			}
		}
		else
			// Only 1 word before comma
			component_name = name;
	}
	else {
		// No commas, so no slope
		// Assume component name is only 1 word followed by texture
		name.tokenize (" ", &tokens);

		if ((word = tokens.begin ()) != tokens.end ()) {
			component_name = *word;
			if (++word != tokens.end ()) {
				do {
					texture += " ";
					texture += *word;
				} while (++word != tokens.end ());
				texture.trim_left (); // remove leading space added during loop
			}
		}
		else
			// Only 1 word
			component_name = name;
	}
	

	output_text += delimiter;
	output_text += "\"";
	output_text += component_name;
	output_text += "\"";
	output_text += delimiter;
	output_text += "\"";
	output_text += texture;
	output_text += "\"";
	output_text += delimiter;
	output_text += "\"";
	output_text += slope;
	output_text += "\"";

	return output_text;
}

dynamic_string parse_mapunit_name_fast
	(const char* muname,
	const char delimiter)

	// Wendong: convert the mapunit name into columns using “,” as delimiter, so that the first line of your example
	// would become two columns Mandeville silt loam, and another field 2 to 5 percent slopes. Then you could use the
	// regular expression and pick the words after the first space or remove the first word, something like that?      

	// "Name 1", "Texture", "Slope Description"
	// Chelsea loamy fine sand, 9 to 18 percent slopes
	// Kenyon loam, 5 to 9 percent slopes, moderately eroded

{
	dynamic_string name = muname, component_name, texture, slope, output_text;
	int slope_index, texture_index;

	if ((slope_index = name.match ((BYTE) ',', 0)) != -1) {
		slope = name.mid (slope_index + 1, -1);
		slope.trim_left(); // remove space that followed comma, plus leading space added during loop
		name = name.left (slope_index);
		// Assume component name is only 1 word followed by texture
		if ((texture_index = name.match((BYTE) ' ', -1)) != -1) {
			texture = name.mid (texture_index + 1, -1);
			component_name = name.left (texture_index);

			texture.trim_left(); // remove leading space added during loop
		}
		else
			// Only 1 word before comma
			component_name = name;
	}
	else {
		// No commas, so no slope
		// Assume component name is only 1 word followed by texture
		if ((texture_index = name.match((BYTE) ' ', -1)) != -1) {
			texture = name.mid(texture_index + 1, -1);
			component_name = name.left(texture_index);

			texture.trim_left(); // remove leading space added during loop
		}
		else
			// Only 1 word
			component_name = name;
	}


	output_text += delimiter;
	output_text += "\"";
	output_text += component_name;
	output_text += "\"";
	output_text += delimiter;
	output_text += "\"";
	output_text += texture;
	output_text += "\"";
	output_text += delimiter;
	output_text += "\"";
	output_text += slope;
	output_text += "\"";

	return output_text;
}

void FarmlandFinder::intersection_table
	(const int state_fips,
	const int county_fips,
	map_layer *layer_clips,
	map_layer *layer_ssurgo,
	const char delimiter,
	std::map <long, double> *area_by_sale_id,
	dynamic_string &log)

{
	std::vector <map_object *>::const_iterator clip;
	map_object *soil;
	dynamic_string soil_text, last_soil_text;
	SSURGO_mapunit *mapunit;
	SSURGO_component *component;
	long sale_id, soil_id, last_soil_id = -1;
	std::map <long, double>::iterator sale_area;
	combined_soil_attributes *attributes;

	for (clip = layer_clips->objects.begin ();
	clip != layer_clips->objects.end ();
	++clip) {

		log.add_formatted ("%d%c%d", state_fips, delimiter, county_fips);
		// Clip attributes_numeric [0] is Sale ID
		sale_id = (long) (*clip)->attributes_numeric[0];
		log.add_formatted ("%c%ld", delimiter, sale_id);

		// Clip attributes_numeric [1] is Soil ID
		soil_id = (long) (*clip)->attributes_numeric[1];
		log.add_formatted ("%c%ld", delimiter, soil_id);

		if (soil_id == last_soil_id)
			soil_text = last_soil_text;
		else {
			soil_text.clear();
			if ((soil = layer_ssurgo->match_id (soil_id)) != NULL) {
				soil_text += delimiter;
				soil_text += soil->attributes_text[2]; // MUKEY
				soil_text += delimiter;
				soil_text += soil->attributes_text[1]; // MUSYM

				attributes = (combined_soil_attributes *) soil->attributes_void;
				ASSERT (attributes != NULL);
				if ((mapunit = attributes->ssurgo) != NULL) {

					soil_text += delimiter;
					if (delimiter == '\t')
						soil_text += mapunit->muname;
					else {
						// Commas in muname
						soil_text += "\"";
						soil_text += mapunit->muname;
						soil_text += "\"";
					}

					// 2021-02-02 Wendong: Break map unit name into 3 parts
					soil_text += parse_mapunit_name_fast (mapunit->muname, delimiter);

					soil_text.add_formatted ("%c%d", delimiter, (int) mapunit->iacornsr);

					// compname, slope_r
					if (mapunit->components.size () > 0) {
						component = &mapunit->components[0];
						soil_text += delimiter;
						soil_text += component->compname;
						soil_text += delimiter;
						if (delimiter == '\t')
							soil_text += component->taxclname;
						else {
							// Commas in taxonomic class
							soil_text += "\"";
							soil_text += component->taxclname;
							soil_text += "\"";
						}
						soil_text.add_formatted ("%c%.4lf", delimiter, component->slope_r);
					}
					else {
						soil_text += delimiter;
						soil_text += delimiter;
					}
				}
				else {
					soil_text += delimiter;
					soil_text += delimiter;
					soil_text += delimiter;
				}
				soil_text += delimiter;
				soil_text += attributes->farmlndcl;

				soil_text.add_formatted ("%c%.8lf%c%.8lf%c%.8lf%c%.8lf", delimiter, attributes->nccpi3corn,
				delimiter, attributes->nccpi3soy, delimiter, attributes->nccpi3sg, delimiter, attributes->nccpi3all);

				if (state_fips == 17) {
					soil_text += delimiter;
					if (attributes->IllinoisPI)
						soil_text.add_formatted ("%.6lf", attributes->IllinoisPI->PI);
				}
			}
			else {
				soil_text.add_formatted ("%c%c%c%c%c%c%c%c%c", delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
				if (state_fips == 17)
					soil_text += delimiter;
			}
			last_soil_id = soil_id;
			last_soil_text = soil_text;
		}

		log += soil_text;

		// log.add_formatted ("\t%.6lf", (*clip)->attributes_numeric[2]); // Geodesic
		log.add_formatted ("%c%.6lf", delimiter, (*clip)->attributes_numeric[3]); // Exact
		// log.add_formatted ("\t%.6lf", (*clip)->attributes_numeric[4]); // AUTHALIC
		// log.add_formatted ("\t%.6lf", (*clip)->attributes_numeric[5]); // Rhumbic
		log += "\n";

		if ((sale_area = area_by_sale_id->find (sale_id)) != area_by_sale_id->end ())
			sale_area->second += (*clip)->attributes_numeric[3];
		else
			area_by_sale_id->insert (std::pair <long, double> (sale_id, (*clip)->attributes_numeric[3]));
	}
}

bool FarmlandFinder::link_mapunits
	(map_layer *layer_ssurgo,
	std::map <long, SSURGO_mapunit> *mapunits_by_mukey,
	std::map <dynamic_string, dynamic_string> *farmlndcl_by_mukey,
	std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
	IllinoisProductivityIndex_container *Illinois_PI,
	dynamic_string &log)

// MUKEY is the link between shapefile polygons and SSURGO_mapunit

{
	std::vector <map_object *>::const_iterator soil;
	std::map <long, SSURGO_mapunit>::const_iterator mu;
	long mukey;
	bool error = false;
	combined_soil_attributes *mu_pointer;
	std::map <dynamic_string, dynamic_string>::const_iterator farmlndcl;
	std::map <dynamic_string, std::tuple <double, double, double, double>>::const_iterator nccp;
	std::vector <SSURGO_component>::const_iterator component;

	layer_ssurgo->attributes_void_usage = MAP_LAYER_USAGE_VOID_POINTER;

	for (soil = layer_ssurgo->objects.begin ();
	soil != layer_ssurgo->objects.end ();
	++soil) {
		mukey = atol ((*soil)->attributes_text[2].get_text_ascii ());
		if ((mu = mapunits_by_mukey->find (mukey)) != mapunits_by_mukey->end ()) {
			mu_pointer = new combined_soil_attributes;
			mu_pointer->ssurgo = (SSURGO_mapunit *) &mu->second;

			if (Illinois_PI)
				if ((component = mu_pointer->ssurgo->components.begin ()) != mu_pointer->ssurgo->components.end ())
					mu_pointer->IllinoisPI = Illinois_PI->get ((*soil)->attributes_text [2], component->compname);

			if ((farmlndcl = farmlndcl_by_mukey->find ((*soil)->attributes_text[2])) != farmlndcl_by_mukey->end ())
				mu_pointer->farmlndcl = farmlndcl->second;

			if ((nccp = nccp_by_mukey->find ((*soil)->attributes_text[2])) != nccp_by_mukey->end ()) {
				mu_pointer->nccpi3corn = std::get <0> (nccp->second);
				mu_pointer->nccpi3soy = std::get <1> (nccp->second);
				mu_pointer->nccpi3sg = std::get <2> (nccp->second);
				mu_pointer->nccpi3all = std::get <3> (nccp->second);
			}

			(*soil)->attributes_void = mu_pointer;
		}
		else {
			error = true;
			log.add_formatted ("ERROR, SSURGO polygon %ld mapunit not found for mukey %ld.\n", (*soil)->id, mukey);
		}
	}

	return !error;
}

bool FarmlandFinder::intersect_layers
	(map_layer *layer_sales,
	const int layer_sales_source,
	map_layer *layer_ssurgo,
	const int layer_ssurgo_source,
	map_layer *layer_clipped,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	dynamic_string thread_log;
	// Create a ClipperLib.Paths from ssurgo layer 
	ClipperBuffer clipper;
	// clipper.intersect_threaded (layer_ssurgo, layer_zipcodes, layer_clipped, -1, app->multithread_count, &view, log);
	clipper.tract_id_source_1 = layer_sales_source; // CLIPPED_ID_SOURCE_ID;
	clipper.tract_id_source_2 = layer_ssurgo_source; // CLIPPED_ID_SOURCE_ID; // Number 1..n, not MUKEY
	clipper.tract_index_source_2 = -1;
	clipper.thread_wait_interval_ms = 1;
	clipper.thread_run_tiny = true;

	if (!clipper.intersect_threaded (layer_sales, layer_ssurgo, layer_clipped, multithread_count, view, thread_log)) {
		log += thread_log;
		error = true;
	}

	return !error;
}

bool FarmlandFinder::intersect_county
	(const int state_fips,
	const int county_fips,
	const dynamic_string &path_ssurgo,
	std::map <long, SSURGO_mapunit> *mapunits_by_mukey,
	map_layer *layer_sales,
	map_layer *layer_clipped,
	map_layer **layer_ssurgo,
	dynamic_map *map,
	std::map <dynamic_string, dynamic_string> *farmlndcl_by_mukey,
	std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
	IllinoisProductivityIndex_container *Illinois_PI,
	interface_window *view,
	dynamic_string &log)

{
	dynamic_string filename_ssurgo_shapefile, ssurgo_county_name, ssurgo_mu_name, subdirectory, thread_log;
	char abbreviation[6];
	dynamic_string state_folder_name;
	bool error = false;

	state_folder_name = packed_state_name (state_fips);
	state_abbreviation (state_fips, abbreviation, 6);

	// soil_ia169
	ssurgo_county_name = "soil_";
	ssurgo_county_name += abbreviation;
	ssurgo_county_name.add_formatted ("%03d", county_fips);

	// soilmu_a_ia169
	ssurgo_mu_name = "soilmu_a_";
	ssurgo_mu_name += abbreviation;
	ssurgo_mu_name.add_formatted ("%03d", county_fips);

	// "E:\ssurgo\DATAMART\Iowa\soil_ia169\spatial\soilmu_a_ia169.shp"
	filename_ssurgo_shapefile = path_ssurgo;
	filename_ssurgo_shapefile += "DATAMART\\";
	filename_ssurgo_shapefile += state_folder_name;
	filename_ssurgo_shapefile += PATH_SEPARATOR;
	filename_ssurgo_shapefile += ssurgo_county_name;
	filename_ssurgo_shapefile += PATH_SEPARATOR;
	filename_ssurgo_shapefile += "spatial\\"; 
	filename_ssurgo_shapefile += ssurgo_mu_name;
	filename_ssurgo_shapefile += ".shp"; 

	if ((*layer_ssurgo = import_ssurgo_county_shapefile (filename_ssurgo_shapefile, ssurgo_county_name, map, view, log)) != NULL) {

		view->update_progress_formatted (0, "State %d County %d", state_fips, county_fips);
		// Attach mukey to objects in layer_ssurgo?
		if (link_mapunits (*layer_ssurgo, mapunits_by_mukey, farmlndcl_by_mukey, nccp_by_mukey, Illinois_PI, log)) {
			view->update_progress_formatted (0, "State %d County %d polygons linked", state_fips, county_fips);

			if (!intersect_layers (layer_sales, CLIPPED_ID_SOURCE_ID,
			*layer_ssurgo, CLIPPED_ID_SOURCE_ID,
			layer_clipped, view, log)) {
				log.add_formatted ("ERROR occurred in county %02d%03d.\n", state_fips, county_fips);
				error = true;
			}
			view->update_progress_formatted (0, "State %d County %d after intersect", state_fips, county_fips);
		}
		else
			error = true;

	}
	else
		error = true;

	return !error;
}

bool FarmlandFinder::read_ssurgo_state_threaded
	(const int state_fips,
	const dynamic_string *path_ssurgo,
	std::vector <dynamic_string> *county_folders,
	std::map <int, std::map <long, SSURGO_mapunit>> *mapunits_by_mukey_by_county,
	interface_window *view,
	dynamic_string &log)

// For each census tract that overlaps the area of the solution
// create an intersection

{
	bool error = false;
	thread_manager threader;
	std::map <long, thread_work *> jobs;
	thread_read_mapunit *job;
	thread_manager_statistics stats;
	std::vector <dynamic_string>::iterator folder;
	dynamic_string filename_ssurgo_mdb, ssurgo_county_name, state_folder_name;
	char abbreviation [6];
	int county_fips;

	state_folder_name = packed_state_name (state_fips);
	state_abbreviation (state_fips, abbreviation, 6);

	threader.thread_count = 3;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1; // 5;
	stats.reset (threader.thread_count);
	threader.statistics = &stats;

	for (folder = county_folders->begin ();
	folder != county_folders->end ();
	++folder) {
		county_fips = atoi (folder->mid (7).get_text_ascii ());
		// soil_ia169
		ssurgo_county_name = "soil_";
		ssurgo_county_name += abbreviation;
		ssurgo_county_name.add_formatted ("%03d", county_fips);

		// "E:\SSURGO\DATAMART\Iowa\soil_ia013\soildb_IA_2002.mdb"
		filename_ssurgo_mdb = *path_ssurgo;
		filename_ssurgo_mdb += state_folder_name;
		filename_ssurgo_mdb += PATH_SEPARATOR;
		filename_ssurgo_mdb += ssurgo_county_name;
		filename_ssurgo_mdb += PATH_SEPARATOR;
		filename_ssurgo_mdb += "soildb_";
		filename_ssurgo_mdb += abbreviation;
		filename_ssurgo_mdb += "_2002";
		filename_ssurgo_mdb += ".mdb";

		job = new thread_read_mapunit;
		job->id = county_fips;
		job->work_function = &work_read_mapunit;
		job->filename_ssurgo_mdb = filename_ssurgo_mdb;

		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	threader.run (&jobs, view, log);

	std::map <long, thread_work *>::iterator completed_job;

	for (completed_job = jobs.begin ();
	completed_job != jobs.end ();
	++completed_job) {

		county_fips = ((thread_read_mapunit *) completed_job->second)->id;
		mapunits_by_mukey_by_county->insert (std::pair <int, std::map <long, SSURGO_mapunit>> (county_fips, ((thread_read_mapunit *) completed_job->second)->mapunits_by_mukey));
	}

	stats.write_time_summary (log);

	return !error;
}

bool FarmlandFinder::read_ssurgo_state_database
	(const int state_fips,
	const dynamic_string *path_ssurgo,
	std::vector <dynamic_string> *county_folders,
	std::map <long, SSURGO_mapunit> *mapunits_by_mukey,
	const bool read_horizon,
	interface_window *view,
	dynamic_string &log) const

// 2021-01-19 Read a single statewide SSURGO MDB file.  All county Mapunit tables have been combined into this

{
	std::vector <dynamic_string>::iterator folder;
	dynamic_string filename_ssurgo_mdb, ssurgo_county_name, state_folder_name;
	bool error = false;

	state_folder_name = packed_state_name (state_fips);

	// "E:\SSURGO\DATAMART\Iowa All Mapunit.mdb"
	filename_ssurgo_mdb = *path_ssurgo;
	filename_ssurgo_mdb += "Database\\States\\";
	filename_ssurgo_mdb += state_folder_name;
	filename_ssurgo_mdb += " All Mapunit.accdb";
	if (read_horizon) {
		std::map <long, SSURGO_component> components_by_cokey;

		if (read_ssurgo_database(filename_ssurgo_mdb, mapunits_by_mukey, &components_by_cokey, view, log)) {
		}
		else
			error = true;
	}
	else {
		if (read_ssurgo_database (filename_ssurgo_mdb, mapunits_by_mukey, view, log)) {
		}
		else
			error = true;
	}

	/*
	state_abbreviation (state_fips, abbreviation, 6);

	for (folder = county_folders->begin ();
	folder != county_folders->end ();
	++folder) {
		county_fips = atoi (folder->mid (7).get_text_ascii ());
		// soil_ia169
		ssurgo_county_name = "soil_";
		ssurgo_county_name += abbreviation;
		ssurgo_county_name.add_formatted ("%03d", county_fips);


		// "E:\SSURGO\DATAMART\Iowa\soil_ia013\soildb_IA_2002.mdb"
		filename_ssurgo_mdb = *path_ssurgo;
		filename_ssurgo_mdb += state_folder_name;
		filename_ssurgo_mdb += PATH_SEPARATOR;
		filename_ssurgo_mdb += ssurgo_county_name;
		filename_ssurgo_mdb += PATH_SEPARATOR;
		filename_ssurgo_mdb += "soildb_";
		filename_ssurgo_mdb += abbreviation;
		filename_ssurgo_mdb += "_2002";
		filename_ssurgo_mdb += ".mdb";

		if (read_ssurgo_mapunit (filename_ssurgo_mdb, &mapunits_by_mukey, view, log)) {
			mapunits_by_mukey_by_county->insert (std::pair <int, std::map <long, SSURGO_mapunit>> (county_fips, mapunits_by_mukey));
		}
		else
			error = true;
	}
	*/
	return !error;
}

void get_output_path
	(const dynamic_string& filename_output,
	dynamic_string& output_path,
	dynamic_string& output_prefix)

// "F:\FarmlandFinder\2022-08-15\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Listings.csv"
// to "F:\FarmlandFinder\2022-08-15\" and "Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Listings"

{
	filename_struct parser;
	parser.set_filename (filename_output);
	parser.write_path (&output_path);
	output_prefix = parser.prefix;
}


bool FarmlandFinder::ssurgo_datamart_county_folders
	(const dynamic_string* path_ssurgo,
	const int state_fips,
	std::vector <dynamic_string> *county_folders,
	dynamic_string &log) const

{
	dynamic_string filename_ssurgo_state, state_folder_name;
	std::vector <dynamic_string> temp_folders;
	std::vector <dynamic_string>::iterator folder;
	int county_fips;
	bool error = false;

	// get all county folder names in a state
	filename_ssurgo_state = *path_ssurgo;
	filename_ssurgo_state += "DATAMART\\";
	state_folder_name = packed_state_name(state_fips);
	filename_ssurgo_state += state_folder_name;
	filename_ssurgo_state += PATH_SEPARATOR;
	county_folders->clear();

	if (list_of_matching_filenames("soil_*", filename_ssurgo_state, false, county_folders, log)) {

		if (false) {
			for (folder = county_folders->begin(), county_fips = 0;
			folder != county_folders->end();
			++folder, ++county_fips) {
				if ((county_fips >= 20)
				&& (county_fips <= 25))
					temp_folders.push_back(*folder);

				/*
				if ((county_fips >= 50)
				&& (county_fips <= 80))
					temp_folders.push_back (*folder);
				*/
			}
			county_folders->clear();
			for (folder = temp_folders.begin();
			folder != temp_folders.end();
			++folder)
				county_folders->push_back(*folder);
		}
	}
	else {
		error = true;
		log.add_formatted("ERROR, No matching folders for state %d.\n", state_fips);
	}
	return !error;
}

bool FarmlandFinder::overlay_ssurgo_state
	(const dynamic_string *path_ssurgo,
	const dynamic_string &filename_output,
	const int state_fips,
	const int first_county_fips,
	map_layer *layer_sales,
	dynamic_map *map,
	const int retain_county_polygons,
	const char delimiter,
	dynamic_string &area_table,
	std::vector <dynamic_string> *county_filenames,
	std::map <dynamic_string, dynamic_string> *farmlndcl_by_mukey,
	std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
	IllinoisProductivityIndex_container *Illinois_PI,
	interface_window *view,
	dynamic_string &log)

{
	dynamic_string timespan, state_folder_name, results, filename_county_output, output_path, output_prefix;
	std::vector <dynamic_string> county_folders;
	std::vector <dynamic_string>::iterator folder;
	std::vector <map_object *>::const_iterator sale;
	map_layer *layer_ssurgo = NULL, *layer_clipped = NULL;
	bool error = false;
	double running_average_time_ms;
	int county_fips = 0;
	int county_count = 0;
	std::chrono::system_clock::time_point start_time, end_time;
	std::chrono::duration <double, std::milli> elapsed_milliseconds, total;
	std::map <long, SSURGO_mapunit> mapunits_by_mukey;
	std::map <long, SSURGO_mapunit>::iterator mapunits;
	std::map <long, double> area_by_sale_id;
	std::map <long, double>::iterator sale_area;

	view->update_progress_formatted (0, "overlay state %d", state_fips);

	layer_clipped = map->create_new (MAP_OBJECT_POLYGON);
	layer_clipped->name = "Intersection";
	layer_clipped->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_clipped->color = RGB (255, 0, 0);

	layer_clipped->attribute_count_numeric = 6; // Space for area fields in ClipperBuffer.Paths_to_map_layer
	layer_clipped->column_names_numeric.push_back ("FarmlandFinder ID");
	layer_clipped->column_names_numeric.push_back ("SSURGO ID");
	layer_clipped->column_names_numeric.push_back ("Geodesic m^2");
	layer_clipped->column_names_numeric.push_back ("Exact m^2");
	layer_clipped->column_names_numeric.push_back ("Authalic m^2");
	layer_clipped->column_names_numeric.push_back ("Rhumb m^2");
	layer_clipped->initialize_attributes = true;
	// map_watershed->add (layer_clipped);

	total = std::chrono::duration <double, std::milli>::zero ();

	// get all county folder names in a state
	if (ssurgo_datamart_county_folders (path_ssurgo, state_fips, &county_folders, log)) {

		get_output_path (filename_output, output_path, output_prefix);
		// county_folders.clear ();
		// county_folders.push_back ("soil_ia159");
		// county_folders.push_back ("soil_ia161");

		mapunits_by_mukey.clear ();
		read_ssurgo_state_database (state_fips, path_ssurgo, &county_folders, &mapunits_by_mukey, false, view, log);

		log.add_formatted ("State\t%d\tCounties\t%d\n", state_fips, (int) county_folders.size ());

		for (folder = county_folders.begin ();
		!error
		// && (county_count < 10)
		&& (folder != county_folders.end ());
		++folder) {
			++county_count;

			// soil_IA123
			start_time = std::chrono::system_clock::now ();
			county_fips = atoi (folder->mid (7).get_text_ascii ());

			// keep all county filenames, even if we're restarting with a first_county_fips in the middle somewhere
			filename_county_output = output_path;
			filename_county_output += output_prefix;
			filename_county_output.add_formatted("-%d.csv", county_fips);
			county_filenames->push_back(filename_county_output);

			if ((first_county_fips == -1)
			|| (county_fips >= first_county_fips)) {

				// mapunits_by_mukey = mapunits_by_mukey_by_county.find (county_fips);

				if (!intersect_county (state_fips, county_fips, *path_ssurgo, &mapunits_by_mukey, layer_sales, layer_clipped, &layer_ssurgo,
				map, farmlndcl_by_mukey, nccp_by_mukey, Illinois_PI, view, log))
					error = true;
				intersection_table (state_fips, county_fips, layer_clipped, layer_ssurgo, delimiter, &area_by_sale_id, results);
				if (county_fips == retain_county_polygons) {
					// Bartholomew County, Indiana
					map->layers.push_back (layer_ssurgo);
				}
				else {
					delete layer_ssurgo;
					layer_ssurgo = NULL;
				}
				layer_clipped->clear_objects ();

				write_ssurgo_county (filename_county_output, results, state_fips, delimiter, (folder == county_folders.begin ()), log);

				results.clear();

				end_time = std::chrono::system_clock::now ();

				elapsed_milliseconds = end_time - start_time;
				total += elapsed_milliseconds;
				running_average_time_ms = total.count () / (double) county_count;
				timespan.format ("%02d%03d %.2lf sec, Ave %.2lf sec", state_fips, county_fips, elapsed_milliseconds.count () / 1000.0, running_average_time_ms / 1000.0);
				view->update_progress (timespan, 2);
			}

		}
		if (error)
			log.add_formatted ("ERROR occurred in county %02d%03d.\n", state_fips, county_fips);

		// Show total clip area for each sale ID
		log.add_formatted ("area size\t%d\n", area_by_sale_id.size ());
		for (sale_area = area_by_sale_id.begin ();
		sale_area != area_by_sale_id.end ();
		++sale_area)
			area_table.add_formatted ("%ld%c%.6lf%c%.6lf\n", sale_area->first, delimiter, sale_area->second, delimiter, sale_area->second * ACRES_PER_M2);
	}
	else
		error = true;

	return !error;
}

bool FarmlandFinder::overlay_ssurgo
	(const dynamic_string *path_ssurgo,
	map_layer *layer_sales,
	dynamic_map *map,
	const char delimiter,
	dynamic_string &area_table,
	std::vector <dynamic_string> *clip_table,
	std::map <dynamic_string, dynamic_string> *farmlndcl_by_mukey,
	std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
	interface_window *view,
	dynamic_string &log)

{
	std::set <int>::iterator sale_state;
	std::vector <map_object *>::const_iterator sale;
	bool error = false;

	log.add_formatted ("State FIPS%cCounty FIPS%cSale%cSoil ID%cMukey%cMusym%cName%cCSR%cExact Area, m^2\n",
	delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);

	for (sale_state = sale_state_fips.begin ();
	!error
	&& (sale_state != sale_state_fips.end ());
	++sale_state) {
		/* 2022-10-03 overlay_ssurgo_state parameters have changed, and this function is no longer used
		if (!overlay_ssurgo_state (path_ssurgo, *sale_state, layer_sales, map, -1, delimiter, area_table, clip_table, farmlndcl_by_mukey, nccp_by_mukey, NULL, view, log))
			error = true;
		*/
	}

	// layer_clipped = intersect_county (19, 13, path_ssurgo, layer_sales, &layer_ssurgo, map, view, log);

	return !error;
}

bool FarmlandFinder::count_states
	(map_layer *layer_sales,
	dynamic_string &log)

{
	std::vector <int>::iterator sale_state;
	std::vector <map_object *>::const_iterator sale;
	int state_fips;
	bool error = false;

	// Get vector of relevent states (10 of them)
	for (sale = layer_sales->objects.begin ();
	sale != layer_sales->objects.end ();
	++sale) {
		state_fips = (int) (*sale)->attributes_numeric[FARMLANDFINDER_SALE_ATTRIBUTE_INDEX_STATE_FIPS];
		sale_state_fips.insert (state_fips);
	}


	return !error;
}

bool FarmlandFinder::read_gSSURGO_mapunit
	(const dynamic_string &path_ssurgo,
	const int state_fips,
	std::map <dynamic_string, dynamic_string>* farmlndcl_by_mukey,
	std::map <dynamic_string, dynamic_string>* muhelcl_by_mukey,
	interface_window *view,
	dynamic_string &error_message) const

// 2021-01-29 Gridded SSURGO gSSURGO

// In Arcmap "mapunit" table opened from GDB folder, saved as DBF using "Table to dBASE" tool

{
	dynamic_string filename_dbf, farmclass_text, helclass_text, key_text;
	FILE *infile;
	CDBFile dbase_file;
	dbase_record *rec;
	std::map <int, dbase_field>::const_iterator dbf_field;
	std::map <int, shapefile_dbase_field>::iterator new_field;
	void *farmlndcl, *mukey, *muhelcl;
	int column_offset = 0;
	long record_number;
	bool error = false;

	// "E:\SSURGO\gSSURGO\Illinois\mapunit.dbf"
	filename_dbf = path_ssurgo;
	filename_dbf += "gSSURGO\\";
	filename_dbf += packed_state_name (state_fips);  
	filename_dbf += PATH_SEPARATOR;
	filename_dbf += "mapunit.dbf";

	if (view)
		view->update_progress ("Reading DBF", 0);

	farmlndcl_by_mukey->clear ();

	if (fopen_s (&infile, filename_dbf.get_text_ascii (), "rb") == 0) {
		if (view)
			view->update_progress ("Reading Header", 1);

		if (dbase_file.read_header (infile, error_message)) {

			// 1  OBJECTID,N,10,0  (doesn't appear in some exports)
			// 2  musym,C,6
			// 3  muname,C,240
			// 4  mukind,C,254
			// 5  mustatus,C,254
			// 6  muacres,N,10,0
			// 7  mapunitlfw,N,5,0
			// 8  mapunitl_1,N,5,0
			// 9  mapunitl_2,N,5,0
			// 10 mapunitpfa,N,13,11
			// 11 mapunitp_1,N,13,11
			// 12 mapunitp_2,N,13,11
			// 13 farmlndcl,C,254
			// 14 muhelcl,C,254
			// 15 muwathelcl,C,254
			// 16 muwndhelcl,C,254
			// 17 interpfocu,C,30
			// 18 invesinten,C,254
			// 19 iacornsr,N,5,0
			// 20 nhiforsoig,C,254
			// 21 nhspiagr,N,19,11
			// 22 vtsepticsy,C,254
			// 23 mucertstat,C,254
			// 24 lkey,C,30
			// 25 mukey,C,30
	
			dbf_field = dbase_file.fields.find (1);
			if (strcmp (dbf_field->second.Name, "OBJECTID") == 0)
				// 'direct' export omits this
				column_offset = 1;

			dbf_field = dbase_file.fields.find (24 + column_offset);
			if (strcmp (dbf_field->second.Name, "mukey") != 0) {
				error = true;
				error_message += "ERROR, MUKEY field not found in gSSURGO DBF \"";
				error_message += filename_dbf;
				error_message += "\".\n";
			}

			dbf_field = dbase_file.fields.find (12 + column_offset);
			if (strcmp (dbf_field->second.Name, "farmlndcl") != 0) {
				error = true;
				error_message += "ERROR, farmlndcl field not found in gSSURGO DBF \"";
				error_message += filename_dbf;
				error_message += "\".\n";
			}

			if (view) {
				view->update_progress ("Reading rows", 1);
				view->update_progress_formatted (2, "Record Count %ld.\n", (long) dbase_file.RecordCount);
			}

			for (record_number = 1;
			!error
			&& (record_number <= (long) dbase_file.RecordCount);
			++record_number) {
				if ((record_number % 1000 == 0)
				&& view)
					view->update_progress_formatted (2, "Rec %ld", record_number);
				rec = dbase_file.read_record (record_number, infile);

				farmlndcl = dbase_file.GetFieldValue (rec, 12 + column_offset);
				muhelcl = dbase_file.GetFieldValue(rec, 13 + column_offset);
				mukey = dbase_file.GetFieldValue (rec, 24 + column_offset);

				key_text = (char*)mukey;
				key_text.trim_left();
				key_text.trim_right();

				if (farmlndcl_by_mukey) {
					farmclass_text = (char *) farmlndcl;
					farmclass_text.trim_left ();
					farmclass_text.trim_right ();
					farmlndcl_by_mukey->insert(std::pair <dynamic_string, dynamic_string>(key_text, farmclass_text));
				}

				if (muhelcl_by_mukey) {
					helclass_text = (char*)muhelcl;
					helclass_text.trim_left();
					helclass_text.trim_right();
					if (helclass_text.get_length () > 0)
						muhelcl_by_mukey->insert(std::pair <dynamic_string, dynamic_string>(key_text, helclass_text));
				}
			}
		}

		fclose (infile);
		if (view)
			view->update_progress ("File Closed", 1);

	}
	else {
		error = true;
		error_message += "Unable to open file \"";
		error_message += filename_dbf;
		error_message += "\".\n";
	}

	return (!error);
}


bool FarmlandFinder::read_gSSURGO_Valu1
	(const dynamic_string &path_ssurgo,
	const int state_fips,
	std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
	interface_window *view,
	dynamic_string &error_message) const

// 2021-01-29 Gridded SSURGO gSSURGO
// attach nccpi3corn, nccpi3soy, nccpi3sg, and nccpi3all to Sales/SSURGO data

// In Arcmap "valu1" table opened from GDB folder, saved as DBF using "Table to dBASE" tool

{
	dynamic_string filename_dbf, key_text;
	FILE *infile;
	CDBFile dbase_file;
	dbase_record *rec;
	std::map <int, dbase_field>::const_iterator dbf_field;
	std::map <int, shapefile_dbase_field>::iterator new_field;
	void *mukey;
	double nccpi3corn, nccpi3soy, nccpi3sg, nccpi3all;
	long record_number;
	std::tuple <double, double, double, double> nccp;
	int column_offset = 0;
	bool error = false;

	if (view)
		view->update_progress ("Reading DBF", 0);

	// "E:\SSURGO\gSSURGO\Illinois\Valu1.dbf"
	filename_dbf = path_ssurgo;
	filename_dbf += "gSSURGO\\";
	filename_dbf += packed_state_name (state_fips);  
	filename_dbf += PATH_SEPARATOR;
	filename_dbf += "Valu1.dbf";

	nccp_by_mukey->clear ();

	if (fopen_s (&infile, filename_dbf.get_text_ascii (), "rb") == 0) {
		if (view)
			view->update_progress ("Reading Header", 1);

		if (dbase_file.read_header (infile, error_message)) {

			// 	1	OBJECTID,N,10,0
			// 	2	aws0_5,N,13,11
			// 	3	aws5_20,N,13,11
			// 	4	aws20_50,N,13,11
			// 	5	aws50_100,N,13,11
			// 	6	aws100_150,N,13,11
			// 	7	aws150_999,N,13,11
			// 	8	aws0_20,N,13,11
			// 	9	aws0_30,N,13,11
			// 	10	aws0_100,N,13,11
			// 	11	aws0_150,N,13,11
			// 	12	aws0_999,N,13,11
			// 	13	tk0_5a,N,13,11
			// 	14	tk5_20a,N,13,11
			// 	15	tk20_50a,N,13,11
			// 	16	tk50_100a,N,13,11
			// 	17	tk100_150a,N,13,11
			// 	18	tk150_999a,N,13,11
			// 	19	tk0_20a,N,13,11
			// 	20	tk0_30a,N,13,11
			// 	21	tk0_100a,N,13,11
			// 	22	tk0_150a,N,13,11
			// 	23	tk0_999a,N,13,11
			// 	24	musumcpcta,N,5,0
			// 	25	soc0_5,N,13,11
			// 	26	soc5_20,N,13,11
			// 	27	soc20_50,N,13,11
			// 	28	soc50_100,N,13,11
			// 	29	soc100_150,N,13,11
			// 	30	soc150_999,N,13,11
			// 	31	soc0_20,N,13,11
			// 	32	soc0_30,N,13,11
			// 	33	soc0_100,N,13,11
			// 	34	soc0_150,N,13,11
			// 	35	soc0_999,N,13,11
			// 	36	tk0_5s,N,13,11
			// 	37	tk5_20s,N,13,11
			// 	38	tk20_50s,N,13,11
			// 	39	tk50_100s,N,13,11
			// 	40	tk100_150s,N,13,11
			// 	41	tk150_999s,N,13,11
			// 	42	tk0_20s,N,13,11
			// 	43	tk0_30s,N,13,11
			// 	44	tk0_100s,N,13,11
			// 	45	tk0_150s,N,13,11
			// 	46	tk0_999s,N,13,11
			// 	47	musumcpcts,N,5,0
			// 	48	nccpi3corn,N,13,11
			// 	49	nccpi3soy,N,13,11
			// 	50	nccpi3cot,N,13,11
			// 	51	nccpi3sg,N,13,11
			// 	52	nccpi3all,N,13,11
			// 	53	pctearthmc,N,5,0
			// 	54	rootznemc,N,5,0
			// 	55	rootznaws,N,5,0
			// 	56	droughty,N,5,0
			// 	57	pwsl1pomu,N,5,0
			// 	58	musumcpct,N,5,0
			// 	59	mukey,C,30
			dbf_field = dbase_file.fields.find (1);
			if (strcmp (dbf_field->second.Name, "OBJECTID") == 0)
				// 'direct' export omits this
				column_offset = 1;
	
			dbf_field = dbase_file.fields.find (58 + column_offset);
			if (strcmp (dbf_field->second.Name, "mukey") != 0) {
				error = true;
				error_message += "ERROR, MUKEY field not found in gSSURGO DBF \"";
				error_message += filename_dbf;
				error_message += "\".\n";
			}

			dbf_field = dbase_file.fields.find (47 + column_offset);
			if (strcmp (dbf_field->second.Name, "nccpi3corn") != 0) {
				error = true;
				error_message += "ERROR, nccpi3corn field not found in gSSURGO DBF \"";
				error_message += filename_dbf;
				error_message += "\".\n";
			}

			dbf_field = dbase_file.fields.find (48 + column_offset);
			if (strcmp (dbf_field->second.Name, "nccpi3soy") != 0) {
				error = true;
				error_message += "ERROR, nccpi3soy field not found in gSSURGO DBF \"";
				error_message += filename_dbf;
				error_message += "\".\n";
			}

			dbf_field = dbase_file.fields.find (50 + column_offset);
			if (strcmp (dbf_field->second.Name, "nccpi3sg") != 0) {
				error = true;
				error_message += "ERROR, nccpi3sg field not found in gSSURGO DBF \"";
				error_message += filename_dbf;
				error_message += "\".\n";
			}

			dbf_field = dbase_file.fields.find (51 + column_offset);
			if (strcmp (dbf_field->second.Name, "nccpi3all") != 0) {
				error = true;
				error_message += "ERROR, nccpi3all field not found in gSSURGO DBF \"";
				error_message += filename_dbf;
				error_message += "\".\n";
			}

			if (view) {
				view->update_progress ("Reading rows", 1);
				view->update_progress_formatted (2, "Record Count %ld.\n", (long) dbase_file.RecordCount);
			}

			for (record_number = 1;
			!error
			&& (record_number <= (long) dbase_file.RecordCount);
			++record_number) {
				if ((record_number % 1000 == 0)
				&& view)
					view->update_progress_formatted (2, "Rec %ld", record_number);
				rec = dbase_file.read_record (record_number, infile);

				nccpi3corn = *((double *) dbase_file.GetFieldValue (rec, 47 + column_offset));
				nccpi3soy =  *((double *) dbase_file.GetFieldValue (rec, 48 + column_offset));
				nccpi3sg =  *((double *) dbase_file.GetFieldValue (rec, 50 + column_offset));
				nccpi3all =  *((double *) dbase_file.GetFieldValue (rec, 51 + column_offset));
				nccp = std::make_tuple (nccpi3corn, nccpi3soy, nccpi3sg, nccpi3all);

				mukey = dbase_file.GetFieldValue (rec, 58 + column_offset);

				key_text = (char *) mukey;
				key_text.trim_left ();
				key_text.trim_right ();
				nccp_by_mukey->insert (std::pair <dynamic_string, std::tuple <double, double, double, double>> (key_text, nccp));
			}
		}

		fclose (infile);
		if (view)
			view->update_progress ("File Closed", 1);

	}
	else {
		error = true;
		error_message += "Unable to open file \"";
		error_message += filename_dbf;
		error_message += "\".\n";
	}

	return (!error);
}

/*
bool FarmlandFinder::read_nhd_water_bodies
	(const dynamic_string &filename_nhd_root,
	std::vector <dynamic_string> *area_names,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &log)
{
	bool error = false;
	dynamic_string filename_nhd_lake;
	std::vector <dynamic_string>::iterator area_name;

	map_layer *layer_lakes = map->create_new (MAP_OBJECT_POLYGON);
	layer_lakes->name = "NHD Water Bodies";
	layer_lakes->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
	layer_lakes->color = RGB (0, 0, 255);
	map->add (layer_lakes);

	for (area_name = area_names->begin (); area_name != area_names->end (); ++area_name) {

		filename_nhd_lake = filename_nhd_root;
		filename_nhd_lake += "NHDPlus";
		filename_nhd_lake += *area_name;
		filename_nhd_lake += "\\";
		filename_nhd_lake += "NHDSnapshot\\Hydrography\\NHDWaterbody.shp";

		importer_shapefile nhd_importer;

		view->update_progress (filename_nhd_lake, 0);

		nhd_importer.filename_source = filename_nhd_lake;
		nhd_importer.has_id = true;
		nhd_importer.id_field_name = "COMID";
		nhd_importer.has_name = true;
		nhd_importer.name_field_name = "GNIS_NAME";
		nhd_importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		nhd_importer.normalize_longitude = false;
		nhd_importer.take_dbf_columns = true;

		if (!nhd_importer.import (layer_lakes, map, view, log))
			error = true;
	}

	if (!error)
		proximity_layers.push_back (layer_lakes);

	return !error;
}
*/

bool FarmlandFinder::divide_urban_areas
	(const dynamic_string &layer_name,
	dynamic_map *map,
	dynamic_string &log)

// Wendong: use field from DBF to divide urban areas into UA and UC

// Field "UATYP10", value U is associated with Urbanized Area, C is Urban Cluster

{
	importer_shapefile shapefile;
	std::vector <map_layer *>::iterator layer, ua_layer;
	std::vector <map_object *> ua, uc;
	std::vector <map_object *>::iterator town;
	bool error = false;

	ua_layer = proximity_layers.end ();

	for (layer = proximity_layers.begin ();
	(ua_layer == proximity_layers.end ())
	&& (layer != proximity_layers.end ());
	++layer)
		if ((*layer)->name == layer_name)
			ua_layer = layer;

	if (ua_layer != proximity_layers.end ()) {
		map_layer *uc_layer = map->create_new (MAP_OBJECT_POLYGON);
		uc_layer->name = "Urban Clusters";
		uc_layer->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
		uc_layer->color = RGB (0, 127, 255);
		map->layers.push_back (uc_layer);
		proximity_layers.push_back (uc_layer);

		for (town = (*ua_layer)->objects.begin ();
		town != (*ua_layer)->objects.end ();
		++town)
			if ((*town)->attributes_text[4] == "U")
				ua.push_back (*town);
			else
				if ((*town)->attributes_text[4] == "C")
					uc.push_back (*town);
				else {
					log.add_formatted ("ERROR, urban id %ld has invalid type\n", (*town)->id);
					error = true;
				}

		(*ua_layer)->objects.clear ();

		// ua back into ua_layer
		for (town = ua.begin ();
		town != ua.end ();
		++town)
			(*ua_layer)->objects.push_back (*town);

		// uc into uc_layer
		for (town = uc.begin ();
		town != uc.end ();
		++town)
			uc_layer->objects.push_back (*town);

	}
	else
		error = true;
	return !error;
}

void FarmlandFinder::write_population
	(map_layer *layer_zip,
	map_layer *layer_clipped_zip,
	map_layer *layer_ua,
	map_layer *layer_clipped_ua,
	dynamic_string &log)

{
	std::vector <FarmlandFinderSale>::const_iterator sale;
	map_object *zip, *ua;
	std::vector <map_object *>::const_iterator clip;
	std::map <dynamic_string, double> zip_areas, ua_areas;
	std::map <dynamic_string, double>::iterator clipped_area;
	double population, area_fraction, allocation, population_ua_10;
	int count_ua, count_ua_10;
	dynamic_string inspection_log, ua_area_name;
	bool inspect_id = true;

	log += "Sale ID\tName";
	log += "\tZip Count\tPopulation";
	log += "\t#UA\tPopulation UA\t#UA > 10%\tPopulation UA > 10%";
	log += "\n";

	for (sale = sales.begin ();
	sale != sales.end ();
	++sale) {
		inspection_log.clear ();
		log.add_formatted ("%ld", sale->id);
		log += "\t";
		log += sale->id_text;

		// Add area of all zip codes that overlap this sale circle
		zip_areas.clear ();
		population = 0.0;
		for (clip = layer_clipped_zip->objects.begin ();
		clip != layer_clipped_zip->objects.end ();
		++clip)
			if ((*clip)->attributes_numeric[0] == sale->id) {
				if ((clipped_area = zip_areas.find ((*clip)->attributes_text[0])) != zip_areas.end ())
					clipped_area->second += (*clip)->attributes_numeric[GEOGRAPHICLIB_LINETYPE_EXACT];
				else
					zip_areas.insert (std::pair <dynamic_string, double> ((*clip)->attributes_text[0], (*clip)->attributes_numeric[GEOGRAPHICLIB_LINETYPE_EXACT]));
			}

		if (inspect_id)
			inspection_log += "\t";

		if ((layer_zip->column_names_numeric[4] != "Shape_Area")
		|| (layer_zip->column_names_numeric[5] != "POPU2019")) {
			log += "ERROR, zip layer attributes_numeric invalid.\n";
		}
		else {

			for (clipped_area = zip_areas.begin ();
			clipped_area != zip_areas.end ();
			++clipped_area) {
				if ((zip = layer_zip->match_name (clipped_area->first)) != NULL) {
					//  [4] Shape Area
					//  [5] POPU2019
					allocation = (clipped_area->second / zip->attributes_numeric[4]) * zip->attributes_numeric[5];
					population += allocation;
					if (inspect_id) {
						inspection_log += "\tZip:";
						inspection_log += clipped_area->first;
						inspection_log.add_formatted ("|%.4lf|%.4lf|%.2lf|%.2lf", clipped_area->second / zip->attributes_numeric[4], zip->attributes_numeric[4], allocation, zip->attributes_numeric[5]);
					}
				}

			}
			log.add_formatted ("\t%d", zip_areas.size ());
			log.add_formatted ("\t%.4lf", population);
		}

		// Add area of all urban areas that overlap this sale circle
		ua_areas.clear ();
		count_ua = count_ua_10 = 0;
		population = population_ua_10 = 0;
		for (clip = layer_clipped_ua->objects.begin ();
		clip != layer_clipped_ua->objects.end ();
		++clip)
			if ((*clip)->attributes_numeric[0] == sale->id) {
				if ((clipped_area = ua_areas.find ((*clip)->attributes_text[0])) != ua_areas.end ())
					clipped_area->second += (*clip)->attributes_numeric[GEOGRAPHICLIB_LINETYPE_EXACT];
				else {
					ua_area_name = (*clip)->attributes_text[0];
					ua_area_name.trim_right ();
					ua_areas.insert (std::pair <dynamic_string, double> (ua_area_name, (*clip)->attributes_numeric[GEOGRAPHICLIB_LINETYPE_EXACT]));
				}
			}

		for (clipped_area = ua_areas.begin ();
		clipped_area != ua_areas.end ();
		++clipped_area) {
			if ((ua = layer_ua->match_name (clipped_area->first)) != NULL) {
				count_ua += 1;
				//  [4] Shape Area
				//  [5] POPU2019
				area_fraction = clipped_area->second / ua->attributes_numeric[4];
				allocation = area_fraction * ua->attributes_numeric[5];
				population += allocation;
				if (area_fraction > .1) {
					count_ua_10 += 1;
					population_ua_10 += allocation;
				}
				if (inspect_id) {
					inspection_log += "\tUA:";
					inspection_log += clipped_area->first;
					inspection_log.add_formatted ("|%.4lf|%.4lf|%.2lf|%.2lf", clipped_area->second / ua->attributes_numeric[4], ua->attributes_numeric[4], allocation, ua->attributes_numeric[5]);
				}
			}
		}
		log.add_formatted ("\t%d\t%.4lf\t%d\t%.4lf", count_ua, population, count_ua_10, population_ua_10);

		log += inspection_log;
		log += "\n";
	}
}

bool work_recompute_area
	(class thread_work *job)

{
	ClipperBuffer clippy;
	ClipperLib::Paths trail;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	double polygon_area_m2;
	map_object *polygon = ((thread_work_recompute_area *) job)->polygon;
	trail.clear ();

	clippy.map_object_to_Path (polygon, &trail, job->log);
	polygon_area_m2 = 0.0;
	for (paths_path = trail.begin ();
	paths_path != trail.end ();
	++paths_path)
		polygon_area_m2 += area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);

	// 2021-07-06 Changing map_object_to_Path to negate negative longitudes produces the same absolute value, so just negate resulting area here
	// Clockwise/counter-clockwise issue?
	polygon->attributes_numeric [((thread_work_recompute_area *) job)->area_attribute_index] = -polygon_area_m2;

	job->status = THREAD_WORK_STATUS_COMPLETE;
    job->end = std::chrono::system_clock::now();

	return true;
}

bool FarmlandFinder::recompute_areas
	(map_layer *layer,
	const int area_attribute_index,
	const byte geographiclib_linetype,
	interface_window *view,
	dynamic_string &log)

// Zipcode and UA area values should be computed by the same code that computes fragment areas
// so that fractional computation of population & area are more correct

{
	bool error = false;
	std::vector <map_object *>::const_iterator polygon;
	thread_manager threader;
	std::map <long, thread_work *> jobs;
	thread_work_recompute_area *job;
	thread_manager_statistics stats;
	int job_index = 0;

	threader.thread_count = multithread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 5;
	stats.reset (threader.thread_count);
	threader.statistics = &stats;

	view->update_progress ("Recomputing Area", 1);
	for (polygon = layer->objects.begin ();
	polygon != layer->objects.end ();
	++polygon) {
		job = new thread_work_recompute_area;
		job->id = ++job_index;
		job->work_function = &work_recompute_area;
		job->polygon = *polygon;
		job->area_attribute_index = 4;
		job->description.format ("Run %ld", job->id);
		jobs.insert (std::pair <long, thread_work *> (job->id, job));
	}

	threader.run_tiny (&jobs, view, log);

	return !error;
}

map_layer *FarmlandFinder::read_sales
	(const dynamic_string &filename_sales,
	dynamic_map *map,
	CountyMaster *counties,
	interface_window *view,
	dynamic_string &log)

// Fills vector sales and creates map_layer

{
	map_layer *layer_sales = NULL;
	std::vector <FarmlandFinderSale>::const_iterator sale;
	map_object *sale_location;

	view->update_progress ("Reading CSV", 0);
	if (read_positions (filename_sales, -1, counties, log)) {
		layer_sales = map->create_new (MAP_OBJECT_POINT);
		layer_sales->name = "Sales Location";
		layer_sales->color = RGB (255, 127, 39);
		layer_sales->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		layer_sales->initialize_attributes = true; // clear all attributes as polygons are created
		layer_sales->attribute_count_numeric = 4; // FARMLANDFINDER_SALE_ATTRIBUTE_INDEX_COUNTY_FIPS = 3
		layer_sales->column_names_numeric.push_back("Gross Price");
		layer_sales->column_names_numeric.push_back("Area Acres");
		layer_sales->column_names_numeric.push_back("State FIPS");
		layer_sales->column_names_numeric.push_back("County FIPS");
		map->layers.push_back (layer_sales);

		for (sale = sales.begin (); sale != sales.end (); ++sale) {
			sale_location = layer_sales->create_new (MAP_OBJECT_POINT);
			sale_location->id = sale->id;
			sale_location->name = sale->id_text;
			sale_location->latitude = (long) (sale->latitude * 1.0e6);
			sale_location->longitude = (long) (sale->longitude * 1.0e6);
			sale_location->attributes_numeric[0] = sale->GrossPrice;
			sale_location->attributes_numeric[1] = sale->area_acres;
			sale_location->attributes_numeric[FARMLANDFINDER_SALE_ATTRIBUTE_INDEX_STATE_FIPS] = (double)sale->StateFIPS;
			sale_location->attributes_numeric[FARMLANDFINDER_SALE_ATTRIBUTE_INDEX_COUNTY_FIPS] = (double)sale->CountyFIPS;
			layer_sales->objects.push_back (sale_location);
		}

	}

	return layer_sales;
}

bool FarmlandFinder::overlay_population
	(const dynamic_string &filename_zip,
	const dynamic_string &filename_ua,
	const double radius_miles,
	dynamic_map *map,
	interface_window *view,
	dynamic_string &log)

{
	map_layer *layer_circles = NULL, *layer_zip, *layer_ua, *layer_clipped_zip, *layer_clipped_ua;
	importer_shapefile shapefile;
	bool error = false;

	{
		layer_zip = map->create_new (MAP_OBJECT_POLYGON);
		layer_zip->name = "Zipcodes";
		layer_zip->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
		layer_zip->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_zip->color = RGB (128, 128, 128);
		layer_zip->initialize_attributes = true;
		/*
		take_dbf_columns will fill these
		layer_zip->column_names_text.push_back("ZCTA5CE10");
		layer_zip->column_names_text.push_back("GEOID10");
		layer_zip->column_names_text.push_back("CLASSFP10");
		layer_zip->column_names_text.push_back("MTFCC10");
		layer_zip->column_names_text.push_back("FUNCSTAT10");
		layer_zip->column_names_text.push_back("INTPTLAT10");
		layer_zip->column_names_text.push_back ("INTPTLON10");
		layer_zip->column_names_text.push_back("INTPTLAT10");
		layer_zip->column_names_text.push_back("INTPTLON10");
		layer_zip->column_names_text.push_back("GEOID_Data");
		*/

		map->layers.push_back (layer_zip);

		importer_shapefile importer;
		importer.filename_source = filename_zip;
		// Both ID fields are text!
		// ZCTA5CE10,C,5
		// GEOID10,C,5
		importer.name_field_name = "ZCTA5CE10";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;
		// importer.data_field_names_text = layer_zipcodes->column_names_text;

		if (!importer.import (layer_zip, NULL, map, view, log))
			error = true;
		else
			// attributes_numeric [4] is "Shape Area"
			recompute_areas (layer_zip, 4, GEOGRAPHICLIB_LINETYPE_EXACT, view, log);
	}

	{
		layer_ua = map->create_new (MAP_OBJECT_POLYGON);
		layer_ua->name = "Urban Areas";
		layer_ua->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
		layer_ua->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_ua->color = RGB (128, 128, 128);
		layer_ua->initialize_attributes = true;
		layer_ua->attribute_count_text = 2;
		// layer_zip->column_names_text.push_back ("INTPTLAT10");
		// layer_zip->column_names_text.push_back ("INTPTLON10");
		map->layers.push_back (layer_ua);

		importer_shapefile importer;
		importer.filename_source = filename_ua;
		importer.name_field_name = "NAME10";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;
		// importer.data_field_names_text = layer_zipcodes->column_names_text;

		if (!importer.import (layer_ua, NULL, map, view, log))
			error = true;
		else
			// attributes_numeric [4] is "Shape Area"
			recompute_areas (layer_ua, 4, GEOGRAPHICLIB_LINETYPE_EXACT, view, log);
	}

	view->update_progress ("Intersecting Zip", 0);
	layer_circles = create_circles_radius (radius_miles, map, false, log);

	layer_clipped_zip = map->create_new (MAP_OBJECT_POLYGON);
	layer_clipped_zip->name = "Zip Intersection";
	layer_clipped_zip->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_clipped_zip->color = RGB (255, 0, 0);

	layer_clipped_zip->attribute_count_numeric = 5; // Space for area fields in ClipperBuffer.Paths_to_map_layer
	layer_clipped_zip->column_names_numeric.push_back ("FarmlandFinder ID");
	// layer_clipped_zip->column_names_numeric.push_back ("Zip Code");
	layer_clipped_zip->column_names_numeric.push_back ("Geodesic m^2");
	layer_clipped_zip->column_names_numeric.push_back ("Exact m^2");
	layer_clipped_zip->column_names_numeric.push_back ("Authalic m^2");
	layer_clipped_zip->column_names_numeric.push_back ("Rhumb m^2");
	layer_clipped_zip->attribute_count_text = 1;
	layer_clipped_zip->column_names_text.push_back ("Zip Code");
	layer_clipped_zip->initialize_attributes = true;
	map->layers.push_back (layer_clipped_zip);

	intersect_layers (layer_circles, CLIPPED_ID_SOURCE_ID,
	layer_zip, CLIPPED_ID_SOURCE_NAME,
	layer_clipped_zip, view, log);
	// dump (log);

	{
		view->update_progress ("Intersecting UA", 0);

		layer_clipped_ua = map->create_new (MAP_OBJECT_POLYGON);
		layer_clipped_ua->name = "UA Intersection";
		layer_clipped_ua->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_clipped_ua->color = RGB (255, 0, 0);

		layer_clipped_ua->attribute_count_numeric = 5; // Space for area fields in ClipperBuffer.Paths_to_map_layer
		layer_clipped_ua->column_names_numeric.push_back ("FarmlandFinder ID");
		layer_clipped_ua->column_names_numeric.push_back ("Geodesic m^2");
		layer_clipped_ua->column_names_numeric.push_back ("Exact m^2");
		layer_clipped_ua->column_names_numeric.push_back ("Authalic m^2");
		layer_clipped_ua->column_names_numeric.push_back ("Rhumb m^2");
		layer_clipped_ua->attribute_count_text = 1;
		layer_clipped_ua->column_names_text.push_back ("UA Name");
		layer_clipped_ua->initialize_attributes = true;
		map->layers.push_back (layer_clipped_ua);

		intersect_layers (layer_circles, CLIPPED_ID_SOURCE_ID,
		layer_ua, CLIPPED_ID_SOURCE_NAME,
		layer_clipped_ua, view, log);
	}

	log.add_formatted ("Radius %.1lf miles\n", radius_miles);
	write_population (layer_zip, layer_clipped_zip, layer_ua, layer_clipped_ua, log);

	return !error;
}

class map_layer *FarmlandFinder::clip_buffers
	(map_layer *layer_sales,
	map_layer *layer_buffer,
	dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// 2021-08-24
// Clip each farm sale circle against layer of powerline buffers
// Farm locations overlap

{
	ClipperBuffer clipper;
	std::vector <map_object *>::const_iterator line_buffer;
	ClipperLib::Paths paths_sale, clipped_region, clipped_tracts, final_cuts, negative_cuts;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	map_layer *layer_clipped, *layer_negative_space;
	std::vector <map_object *>::const_iterator sale;
	bounding_cube sale_bounds;
	int count_contributing;
	dynamic_string powerline_id_string;
	double sale_area_m2;
	std::map <long long, double> area_m2_by_sale_id;
	std::vector <long long> buffer_ids;
	std::vector <map_object *> clip_objects;
	std::vector <long long>::iterator buffer_id;
	std::map <long long, std::vector <map_object *>> clips_by_sale_id, negative_space_by_sale_id;
	long last_cut_id = 0, last_negative_id = 0;

	layer_clipped = map_watershed->create_new (MAP_OBJECT_POLYGON);
	layer_clipped->name = layer_sales->name;
	layer_clipped->name += " Powerline Overlap";
	layer_clipped->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR; // MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_clipped->color = RGB (0, 255, 0);
	layer_clipped->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
	layer_clipped->attribute_count_numeric = 6; // Space for area fields in ClipperBuffer.Paths_to_map_layer
	layer_clipped->column_names_numeric.push_back ("FarmlandFinder");
	layer_clipped->column_names_numeric.push_back ("Line_Count");
	layer_clipped->column_names_numeric.push_back ("Geodesic m^2");
	layer_clipped->column_names_numeric.push_back ("Exact m^2");
	layer_clipped->column_names_numeric.push_back ("Authalic m^2");
	layer_clipped->column_names_numeric.push_back ("Rhumb m^2");
	layer_clipped->attribute_count_text = 1;
	layer_clipped->column_names_text.push_back ("Line_IDs");
	layer_clipped->initialize_attributes = true;
	map_watershed->layers.push_back (layer_clipped);

	layer_negative_space = map_watershed->create_new (MAP_OBJECT_POLYGON);
	layer_negative_space->name = layer_sales->name;
	layer_negative_space->name += " Negative Space";
	layer_negative_space->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR; // MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_negative_space->color = RGB (63, 127, 63);
	layer_negative_space->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
	layer_negative_space->attribute_count_numeric = 5; // Space for area fields in ClipperBuffer.Paths_to_map_layer
	layer_negative_space->column_names_numeric.push_back ("FarmlandFinder");
	layer_negative_space->column_names_numeric.push_back ("Geodesic m^2");
	layer_negative_space->column_names_numeric.push_back ("Exact m^2");
	layer_negative_space->column_names_numeric.push_back ("Authalic m^2");
	layer_negative_space->column_names_numeric.push_back ("Rhumb m^2");
	layer_negative_space->attribute_count_text = 0;
	// layer_clipped->column_names_text.push_back ("Line_IDs");
	layer_negative_space->initialize_attributes = true;
	map_watershed->layers.push_back (layer_negative_space);

	// Convert map_layer to ClipperLib Paths
	// clipper.map_layer_to_Paths (layer_buffer, &paths_powerline_buffer, view, log);

	view->update_progress ("Clipping Buffers", 1);

	for (sale = layer_sales->objects.begin ();
	sale != layer_sales->objects.end ();
	++sale) {

		// Set overlapped area of sale
		sale_bounds.clear ();
		(*sale)->check_extent (&sale_bounds);

		// Create a ClipperLib paths of the sale circle
		paths_sale.clear ();
		clipper.map_object_to_Path (*sale, &paths_sale, log);

		sale_area_m2 = 0.0;
		for (paths_path = paths_sale.begin ();
		paths_path != paths_sale.end ();
		++paths_path)
			sale_area_m2 += area_m2 (&*paths_path, GEOGRAPHICLIB_LINETYPE_EXACT);
		// Negative areas of farm circles are due to clockwise point numbering.
		area_m2_by_sale_id.insert (std::pair <long, double> ((*sale)->id, -1.0 * sale_area_m2));

		// Create a trail of any layer_buffer_objects that overlap the sale
		count_contributing = 0;
		powerline_id_string.clear ();
		final_cuts.clear ();
		negative_cuts.clear ();
		buffer_ids.clear ();
		for (line_buffer = layer_buffer->objects.begin ();
		line_buffer != layer_buffer->objects.end ();
		++line_buffer) {
			if ((*line_buffer)->overlaps_logical (&sale_bounds)) {
				clipper.cut_and_accumulate (*line_buffer, &paths_sale, &final_cuts, &buffer_ids, &count_contributing,
				(*sale)->id,
				false, // ((*sale)->id == 11359),
				log);

				clipper.subtract (&paths_sale, &final_cuts, &negative_cuts, false, log);
			}
		}

		if (final_cuts.size () > 0) {
			// log.add_formatted ("%ld\t%d\t%ld\n", (*sale)->id, count_contributing, last_cut_id);

			// Create string of ids to become attributes_text[0] of each clip map_object
			for (buffer_id = buffer_ids.begin (); buffer_id != buffer_ids.end (); ++buffer_id) {
				if (powerline_id_string.get_length () > 0)
					powerline_id_string += " ";
				powerline_id_string.add_formatted ("%lld", *buffer_id);
			}

			view->update_progress_formatted (1, "Sale %lld", (*sale)->id);

			clip_objects.clear ();
			clipper.tract_id_source_1 = CLIPPED_ID_SOURCE_ID;
			clipper.tract_id_source_2 = 0; // Not using attributes_numeric [1] for powerline id (uses text_attributes [0] instead)
			clipper.buffer_width_output_index = 1; // actually using this slot for line_count
			clipper.Paths_to_map_layer (&final_cuts, &last_cut_id, (*sale)->id, (double) count_contributing, &powerline_id_string, layer_clipped, &clip_objects, true, view, log);

			// Track final_cut IDs by sale
			clips_by_sale_id.insert (std::pair <long long, std::vector <map_object *>> ((*sale)->id, clip_objects));

			clip_objects.clear ();
			clipper.tract_id_source_1 = CLIPPED_ID_SOURCE_ID;
			clipper.tract_id_source_2 = 0; // Not using attributes_numeric [1] for any id
			clipper.buffer_width_output_index = -1;
			clipper.Paths_to_map_layer (&negative_cuts, &last_negative_id, (*sale)->id, 0, NULL, layer_negative_space, &clip_objects, true, view, log);

			// Track final_cut IDs by sale
			negative_space_by_sale_id.insert (std::pair <long, std::vector <map_object *>> ((*sale)->id, clip_objects));
		}
	}

	{
		dynamic_string report;
		std::map <long long, double>::iterator sale_area;
		std::map <long long, std::vector <map_object *>>::iterator sale_clips;
		std::vector <map_object *>::iterator farm, clip;
		double total_clip_acres, total_clip_fraction, total_negative_acres;
		int exact_area_attribute_index = 3;

		report += "Farm ID\tName\tFarm Gross Acres\tFarm Area Exact, acres\tOverlap ID\tGeodesic, m^2\tExact, m^2\tAuthalic, m^2\tRhumb, m^2";
		report += "\tExact Acres\tFraction Overlapped\tNegative Acres";
		report += "\n";
		for (farm = layer_sales->objects.begin ();
		farm != layer_sales->objects.end ();
		++farm) {
			total_clip_acres = total_clip_fraction = total_negative_acres = 0.0;
			sale_area = area_m2_by_sale_id.find ((*farm)->id);
			if ((sale_clips = clips_by_sale_id.find ((*farm)->id)) != clips_by_sale_id.end ()) {

				for (clip = sale_clips->second.begin (); clip != sale_clips->second.end (); ++clip) {
					report.add_formatted ("\t\t\t\t%lld", (*clip)->id);
					report.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf",
					(*clip)->attributes_numeric[2], (*clip)->attributes_numeric[exact_area_attribute_index], (*clip)->attributes_numeric[4], (*clip)->attributes_numeric[5]);

					report.add_formatted ("\t%.4lf", (*clip)->attributes_numeric[exact_area_attribute_index] / M2_PER_ACRE);
					report.add_formatted ("\t%.4lf", (*clip)->attributes_numeric[exact_area_attribute_index] / sale_area->second);
					report += "\n";
					total_clip_acres += (*clip)->attributes_numeric[exact_area_attribute_index] / M2_PER_ACRE;
					total_clip_fraction += (*clip)->attributes_numeric[exact_area_attribute_index] / sale_area->second;
				}
			}

			if ((sale_clips = negative_space_by_sale_id.find ((*farm)->id)) != negative_space_by_sale_id.end ()) {

				for (clip = sale_clips->second.begin (); clip != sale_clips->second.end (); ++clip) {
					report.add_formatted ("\t\t\t\t%lld", (*clip)->id);
					report.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf",
					(*clip)->attributes_numeric[2], (*clip)->attributes_numeric[exact_area_attribute_index], (*clip)->attributes_numeric[4], (*clip)->attributes_numeric[5]);

					report += "\t\t";
					report.add_formatted ("\t%.4lf", (*clip)->attributes_numeric[exact_area_attribute_index] / M2_PER_ACRE);
					report += "\n";
					total_negative_acres += (*clip)->attributes_numeric[exact_area_attribute_index] / M2_PER_ACRE;
				}
			}
			report.add_formatted ("%lld", (*farm)->id);
			report += "\t";
			report += (*farm)->name;
			report.add_formatted ("\t%.3lf", (*farm)->attributes_numeric[3]);
			report.add_formatted ("\t%.3lf", sale_area->second / M2_PER_ACRE);
			report += "\t\t\t\t\t";
			report.add_formatted ("\t%.3lf", total_clip_acres);
			report.add_formatted ("\t%.3lf", total_clip_fraction);
			report.add_formatted ("\t%.3lf", total_negative_acres);
			report += "\n";

		}
		log += report;
	}
	layer_clipped->set_extent ();
	layer_negative_space->set_extent ();

	return layer_clipped;
}

bool FarmlandFinder::append_county_files
	(dynamic_string &filename_output,
	std::vector <dynamic_string>* thread_filenames,
	const bool remove_county_files,
	dynamic_string& log)

// from MapLayerProximity.append_thread_files

{
	FILE* f, * thread_read;
	bool error = false;
	dynamic_string column_names;
	std::vector <dynamic_string>::iterator thread_filename;

	if (fopen_s(&f, filename_output.get_text_ascii(), "w") == 0) {


		BYTE buffer[2048];
		int buffer_size;
		filename_struct fs;
		dynamic_string target_folder, target_filename;

		for (thread_filename = thread_filenames->begin();
		thread_filename != thread_filenames->end();
		++thread_filename) {

			if (fopen_s(&thread_read, thread_filename->get_text_ascii(), "r") == 0) {
				while ((buffer_size = fread(buffer, 1, 2048, thread_read)) > 0)
					fwrite(buffer, 1, buffer_size, f);
				fclose(thread_read);
			}

			fs.parse(&*thread_filename);
			fs.write_path(&target_folder);
			fs.write_filename(&target_filename);
			if (remove_county_files)
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

bool FarmlandFinder::read_previous_output
	(const int state_fips,
	const dynamic_string &filename_sales,
	std::map <long, FarmlandFinderRepresentativeSoil> *soil_sales_by_id,
	const int limit,
	interface_window*view,
	dynamic_string& log) const

// 2023-03-24 read output of previous run with sales and SSURGO IDs already in place

{
	FILE* f;
	char buffer[8192];
	dynamic_string data, inconstant_filename, progress;
	std::vector <dynamic_string> tokens;
	long id, count_read, count_valid;
	bool error = false, done = false;
	FarmlandFinderRepresentativeSoil sale;
	std::map <long, FarmlandFinderRepresentativeSoil>::iterator soil_sale;
	std::pair <long, double> soil_area;
	int token_count;

	// if (state_fips == 17)
		// "Illinois PI" is token [19]
	//	file_token_count = 21;
	//else
	//	file_token_count = 20;

	count_read = count_valid = 0;
	inconstant_filename = filename_sales;
	if (fopen_s(&f, inconstant_filename.get_text_ascii(), "r") == 0) {
		view->update_scroll (inconstant_filename);

		if (fgets(buffer, 8192, f)) {
			while (fgets(buffer, 8192, f)
			&& !done) {
				data = buffer;
				data.tokenize(",\n", &tokens, true, false);
				++count_read;

				sale.clear();

				// State FIPS	County FIPS	Sale	Soil ID	Mukey	Musym	Name	Name 1	Texture	Slope Description	CSR	Component Name	Taxonomic Class	Representative Slope	gSSURGO frmlndcl	nccpi3corn	nccpi3soy	nccpi3sg	nccpi3all	Exact Area, m ^ 2
				// 20	9	1983	16	2668912	3800	Crete silt loam, 0 to 1 percent slopes, loess plainsand breaks	Crete	silt loam	0 to 1 percent slopes, loess plainsand breaks	0	Crete	Fine, smectitic, mesic Pachic Udertic Argiustolls	1	All areas are prime farmland	0.579	0.658	0.524	0.658	198354.0018
				// 20	9	1983	18	1440481	5632	Platte soils, occasionally flooded	Platte	soils	occasionally flooded	0	Platte	Sandy, mixed, mesic Aeric Fluvaquents	1	Farmland of statewide importance	0.214	0.152	0.204	0.214	5578896.937

				if ((token_count = tokens.size ()) >= 20) {
					// if (tokens [4] == "2796147")
						// MUKEY has component name with comma: "Orthents, very hilly"
						// but quotes weren't written around component names.
						// token_count = 22;

					id = atoi(tokens[2].get_text_ascii());

					// First Kansas sales record has 189 soil records with some duplicates
					// Create one record for the sale with area-weighted % sand/silt/clay across all soils

					// ASSERT in read_ssurgo_database checks for long overflow of mukey [31];
					soil_area.first = atol(tokens[4].get_text_ascii());

					soil_area.second = atof(tokens[token_count - 1].get_text_ascii());

					if ((soil_sale = soil_sales_by_id->find(id)) != soil_sales_by_id->end()) {
						soil_sale->second.ssurgo_mukey_and_area_m2.push_back(soil_area);
						soil_sale->second.area_m2 += soil_area.second;
					}
					else {

						sale.id = id;
						sale.StateFIPS = atoi(tokens[0].get_text_ascii());
						sale.CountyFIPS = atoi(tokens[1].get_text_ascii());

						sale.ssurgo_mukey_and_area_m2.push_back(soil_area);
						sale.area_m2 += soil_area.second;

						soil_sales_by_id->insert(std::pair <long, FarmlandFinderRepresentativeSoil>(sale.id, sale));
					}

					++count_valid;

					if ((limit != -1)
					&& (id > limit))
						done = true;

					/*
					if (count_valid % 1000 == 0) {
						progress.format ("%d records", count_valid);
						view->update_scroll(progress);
					}
					*/
				}
				else
					done = true;
			}
		}
		fclose(f);

		log += filename_sales;
		log.add_formatted("\tCount read\t%ld\tCount valid\t%ld\n", count_read, count_valid);
	}
	else {
		log += "ERROR, can't read Farmland Sales file \"";
		log += filename_sales;
		log += "\".\n";
		error = true;
	}
	return !error;
}

bool FarmlandFinder::weighted_area_ssurgo
	(dynamic_string &report,
	std::map <long, FarmlandFinderRepresentativeSoil>* soil_sales_by_id,
	const int state_fips,
	const dynamic_string* path_ssurgo,
	std::map <dynamic_string, std::tuple <double, double, double, double>> *nccp_by_mukey,
	const int limit,
	const bool write_soils,
	interface_window* view,
	dynamic_string& log) const

{
	std::map <long, FarmlandFinderRepresentativeSoil>::const_iterator report_sale;
	std::vector <std::pair <long, double>>::const_iterator ssurgo_id;
	std::map <long, SSURGO_mapunit> mapunits_by_mukey;
	std::map <long, SSURGO_mapunit>::const_iterator mapunit;
	std::vector <SSURGO_component>::const_iterator component;
	std::vector <SSURGO_horizon>::const_iterator horizon;
	std::vector <dynamic_string> county_folders;
	dynamic_string soil_report, weighted_line;
	bool error = false;
	std::map <dynamic_string, std::tuple <double, double, double, double>>::const_iterator nccp;
	double sand, silt, clay, slope, csr, nccpi [4];

	if (view)
		view->update_progress("Reading SSURGO", 1);

	// get all county folder names in a state
	if (ssurgo_datamart_county_folders(path_ssurgo, state_fips, &county_folders, log)
	&& read_ssurgo_state_database(state_fips, path_ssurgo, &county_folders, &mapunits_by_mukey, true, view, log)) {

		if (view)
			view->update_progress("Writing Table", 1);

		for (report_sale = soil_sales_by_id->begin();
		!error
		&& (report_sale != soil_sales_by_id->end());
		++report_sale) {

			if (write_soils)
				weighted_line.format("%d\t%ld\t\t%.6lf\t1.0", state_fips, report_sale->first, report_sale->second.area_m2);
			else
				weighted_line.format("%d\t%ld\t%.6lf", state_fips, report_sale->first, report_sale->second.area_m2);
			soil_report.clear ();
			sand = silt = clay = slope = csr = 0.0;
			memset (nccpi, 0, sizeof (double) * 4);

			for (ssurgo_id = report_sale->second.ssurgo_mukey_and_area_m2.begin();
			ssurgo_id != report_sale->second.ssurgo_mukey_and_area_m2.end();
			++ssurgo_id) {
				soil_report.add_formatted("\t%ld", ssurgo_id->first);
				soil_report.add_formatted("\t%.6lf", ssurgo_id->second);
				soil_report.add_formatted("\t%.6lf", ssurgo_id->second / report_sale->second.area_m2);

				if ((mapunit = mapunits_by_mukey.find(ssurgo_id->first)) != mapunits_by_mukey.end()) {
					if ((component = mapunit->second.components.begin()) != mapunit->second.components.end()) {
						if ((horizon = component->horizons.begin()) != component->horizons.end()) {
							// Based on textureand exact area, calculate for each sale% loam, % silt loam, % clay, % silt loam for each sale,
							// which is the total area for each soil texture type divided by the total exact area for each sale.

							// sandtotal_r : Mineral particles 0.05mm to 2.0mm in equivalent diameter as a weight percentage of the less than 2 mm fraction
							// silttotal_r : Mineral particles 0.002 to 0.05mm in equivalent diameter as a weight percentage of the less than 2.0mm fraction.
							// claytotal_r : Mineral particles less than 0.002mm in equivalent diameter as a weight percentage of the less than 2.0mm fraction.
							soil_report.add_formatted ("\t%.6f\t%.6f\t%.6f", horizon->sandtotal_r, horizon->silttotal_r, horizon->claytotal_r);
							sand += horizon->sandtotal_r * ssurgo_id->second;
							silt += horizon->silttotal_r * ssurgo_id->second;
							clay += horizon->claytotal_r * ssurgo_id->second;
						}
						else {
							/*
							log.add_formatted("Mapunit %ld \"", mapunit->first);
							log += mapunit->second.muname;
							log += "\", Component ";
							log += component->cokey;
							log += "\" has no horizons.\n";
							*/
							soil_report += "\t\t\t";
						}

						// Acre-weighted slope using representative slopeand exact area
						soil_report.add_formatted("\t%.6f", component->slope_r);
						slope += component->slope_r * ssurgo_id->second;

						// Acre-weighted CSR and all NCCPI measures
						soil_report.add_formatted("\t%d", (int) mapunit->second.iacornsr);
						csr += (double) mapunit->second.iacornsr * ssurgo_id->second;

						if ((nccp = nccp_by_mukey->find(mapunit->second.mukey)) != nccp_by_mukey->end()) {
							soil_report.add_formatted("\t%.6f", std::get <0>(nccp->second)); // nccpi3corn
							soil_report.add_formatted("\t%.6f", std::get <1>(nccp->second)); // nccpi3soy
							soil_report.add_formatted("\t%.6f", std::get <2>(nccp->second)); // nccpi3sg
							soil_report.add_formatted("\t%.6f", std::get <3>(nccp->second)); // nccpi3all

							nccpi[0] += std::get <0>(nccp->second) * ssurgo_id->second;
							nccpi[1] += std::get <1>(nccp->second) * ssurgo_id->second;
							nccpi[2] += std::get <2>(nccp->second) * ssurgo_id->second;
							nccpi[3] += std::get <3>(nccp->second) * ssurgo_id->second;
						}
					}
					else {
						error = true;
						log.add_formatted ("ERROR, Mapunit %ld has no components.\n", mapunit->first);
					}
				}

				soil_report += "\n";
			}
			if (!write_soils)
				weighted_line.add_formatted ("\t%d", (int) report_sale->second.ssurgo_mukey_and_area_m2.size ());
			weighted_line.add_formatted("\t%.6f\t%.6f\t%.6f", sand / report_sale->second.area_m2, silt / report_sale->second.area_m2, clay / report_sale->second.area_m2);
			weighted_line.add_formatted("\t%.6f\t%.6f", slope / report_sale->second.area_m2, csr / report_sale->second.area_m2);
			weighted_line.add_formatted("\t%.6f\t%.6f\t%.6f\t%.6f", nccpi [0] / report_sale->second.area_m2, nccpi[1] / report_sale->second.area_m2,
			nccpi[2] / report_sale->second.area_m2, nccpi[3] / report_sale->second.area_m2);

			weighted_line += "\n";

			report += weighted_line;
			if (write_soils)
				report += soil_report;
		}
	}

	return !error;
}

bool FarmlandFinder::read_ssurgo_datamart
	(odbc_database* db_ssurgo,
	std::map <dynamic_string, SSURGO_mapunit>* county_mapunits,
	interface_window* update_display,
	dynamic_string& error_message)

// read full SSURGO from one county-specific access database
// Uses string for mapunit

{
	bool error = false;
	SSURGO_mapunit set_mapunits;
	std::vector <SSURGO_component>::iterator component;
	long count_table = 0L, count_fail = 0L, count_read = 0L;

	set_mapunits.initialize_field_set ();

	if (set_mapunits.field_set->open_read (db_ssurgo, error_message)) {

		if (set_mapunits.field_set->move_first (error_message)) {
			do {
				++count_table;
				set_mapunits.components.clear();

				if (set_mapunits.read_components(db_ssurgo, error_message)) {
					// components are sorted by comppct_r, percent of mapunit composition
					if (count_table % 100 == 0)
						update_display->update_progress (set_mapunits.muname, 1);

					county_mapunits->insert (std::pair <dynamic_string, SSURGO_mapunit> (set_mapunits.mukey, set_mapunits));
				}
				else
					error = true;
			} while (!error
			&& (set_mapunits.field_set->move_next (error_message)));
		}

		set_mapunits.field_set->close();
	}
	else
		error = true;

	error_message.add_formatted("soils read\t%ld\n", count_table);
	error_message.add_formatted("Soils Failed\t%ld\n", count_fail);
	count_read += count_table;

	return !error;
}

bool work_weighted_area
	(thread_work* job)

// [Microsoft][ODBC Microsoft Access Driver] Too many client tasks.

{
	bool error = false;
	std::map <long, FarmlandFinderRepresentativeSoil> *soil_sales_by_id;

	soil_sales_by_id = new std::map <long, FarmlandFinderRepresentativeSoil>;


	job->view->update_scroll("gSSURGO read");
	if (((thread_weighted_area*)job)->farms->read_previous_output(((thread_weighted_area *) job)->state_fips,
	((thread_weighted_area*)job)->filename_previous_output,
	soil_sales_by_id, -1, job->view, job->log)) {
		((thread_weighted_area*)job)->farms->weighted_area_ssurgo(((thread_weighted_area*)job)->report,
		soil_sales_by_id,
		((thread_weighted_area*)job)->state_fips, &((thread_weighted_area*)job)->path_ssurgo,
		((thread_weighted_area*)job)->nccp_by_mukey, -1, ((thread_weighted_area*)job)->write_soils, NULL, job->log);
	}

	delete soil_sales_by_id;

	if (!error) {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		job->end = std::chrono::system_clock::now();
		return true;
	}
	else {
		job->status = THREAD_WORK_STATUS_ERROR;
		return false;
	}
}

bool FarmlandFinder::weighted_area_ssurgo_threaded
	(std::set <int> *run_states,
	const dynamic_string& path_ssurgo,
	const bool auction_results,
	const bool write_soils,
	interface_window* view,
	dynamic_string& log)

{
	thread_manager threader;
	std::map <long, thread_work*> jobs;
	std::map <long, thread_work*>::iterator job_iterator;
	thread_weighted_area* job;
	thread_manager_statistics stats;
	int thread_index;
	std::set <int>::iterator run_state;
	bool error = false;
	char abbreviation [3];

	threader.thread_count = multithread_count;
	threader.maximum_restart_count = 0;
	threader.wait_interval_ms = 1; // 5;
	stats.reset(threader.thread_count);
	threader.statistics = &stats;

	thread_index = 0;
	for (run_state = run_states->begin();
	run_state != run_states->end();
	++run_state) {

		job = new thread_weighted_area;
		job->id = thread_index + 1;
		job->farms = this;
		job->state_fips = *run_state;

		// read_gSSURGO_Valu1 doesn't appear to be threadsafe
		job->nccp_by_mukey = new std::map <dynamic_string, std::tuple <double, double, double, double>>;
		read_gSSURGO_Valu1(path_ssurgo, *run_state, job->nccp_by_mukey, view, log);

		state_abbreviation(*run_state, abbreviation, 3);
		if (auction_results)
			job->filename_previous_output = "F:\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Auction-";
		else
			job->filename_previous_output = "F:\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Listings-";
		job->filename_previous_output += abbreviation;
		job->filename_previous_output += ".csv";

		job->work_function = &work_weighted_area;

		job->path_ssurgo = path_ssurgo;
		job->write_soils = write_soils;

		job->view = view;

		job->description.format("Run %ld", job->id);
		jobs.insert(std::pair <long, thread_work*>(job->id, job));
		++thread_index;
	}


	view->set_data(&threader); // sends DIALOG_DATA_POINTER to message_slot in dialog_run_threads
	threader.run_tiny(&jobs, view, log);
	view->set_data(NULL);

	view->update_status_bar("Threader run complete");

	for (job_iterator = jobs.begin();
		job_iterator != jobs.end();
		++job_iterator) {
		delete ((thread_weighted_area*)job_iterator->second)->nccp_by_mukey;

		log += job_iterator->second->log;
		job_iterator->second->log.clear();
	}

	if (write_soils)
		log += "State\tSale\tMukey\tArea m^2\tFraction of Sale Area\tsandtotal_r\tsilttotal_r\tclaytotal_r\tslope_r\tIACSR\tnccpi3corn\tnccpi3soy\tnccpi3sg\tnccpi3all\n";
	else
		log += "State\tSale\tArea m^2\tSoil Count\tsandtotal_r\tsilttotal_r\tclaytotal_r\tslope_r\tIACSR\tnccpi3corn\tnccpi3soy\tnccpi3sg\tnccpi3all\n";

	for (job_iterator = jobs.begin();
	job_iterator != jobs.end();
	++job_iterator) {

		log += ((thread_weighted_area*)job_iterator->second)->report;
		((thread_weighted_area*)job_iterator->second)->report.clear();
	}

	// stats.write_time_summary (log);

	return !error;
}
