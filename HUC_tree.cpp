#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/utility.h"
#include "HUC_tree.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/dynamic_map.h"
#include "../map/map_scale.h"

HUC_network_link::HUC_network_link  ()

{
	clear ();
}

HUC_network_link::HUC_network_link 
	(const HUC_network_link  &other)

{
	copy (other);
}

void HUC_network_link::clear ()

// Code Written by DB Utility 2002-06-19 10:52:50

{
	from.clear ();
	id = 0;
}

HUC_network_link::~HUC_network_link ()
{

	std::set <HUC_network_link *>::iterator link;

	for (link = from.begin ();
	link != from.end ();
	++link)
		delete (*link);
}

void HUC_network_link::copy
	(const HUC_network_link &other)

// Code Written by DB Utility 2002-06-19 10:52:50

{
	from = other.from;
	id = other.id;
}

void HUC_network_link::write_upstream
	(FILE *f) const

{
	std::set <HUC_network_link *>::const_iterator link;

	fprintf (f, "\t%lld", id);

	for (link = from.begin ();
	link != from.end ();
	++link)
		(*link)->write_upstream (f);
}

int HUC_network_link::depth () const

{
	int d, max_d;
	std::set <HUC_network_link *>::const_iterator link;

	max_d = 0;
	for (link = from.begin ();
	link != from.end ();
	++link) {
		d = (*link)->depth ();
		if (d > max_d)
			max_d = d;
	}
	return max_d + 1;
}

void HUC_network_link::build_map_layer
	(map_layer *layer_trail,
	const map_layer *layer_hucs,
	dynamic_string &log) const

{
	std::set <HUC_network_link *>::const_iterator link;
	map_object *source_polygon, *destination_polygon;
	map_object_vector *vector;

	if ((destination_polygon = layer_hucs->match_id ((long) id)) != NULL) {

		for (link = from.begin ();
		link != from.end ();
		++link) {

			if ((source_polygon = layer_hucs->match_id ((long) (*link)->id)) != NULL) {
				// New vector from source to link

				vector = (map_object_vector *) layer_trail->create_new (MAP_OBJECT_VECTOR);
				vector->id = (long) id;
				vector->name = source_polygon->name;
				vector->attributes_text[0].format ("to %ld", destination_polygon->id);
				source_polygon->centroid (&vector->latitude, &vector->longitude);
				destination_polygon->centroid (&vector->destination_latitude, &vector->destination_longitude);
				layer_trail->objects.push_back (vector);

			}
			else
				log.add_formatted ("ERROR, huc %lld not found in layer_hucs.\n", (*link)->id);

			(*link)->build_map_layer (layer_trail, layer_hucs, log);
		}
	}
	else
		log.add_formatted ("ERROR, huc %lld not found in layer_hucs.\n", id);
}

bool HUC_network_link::accumulate_upstream
	(std::vector <long long> *comids,
	std::set <long long> *duplicate_check,
	dynamic_string &log) const
{
	std::set <HUC_network_link *>::const_iterator link;
	bool error = false;

	if (duplicate_check->find (id) == duplicate_check->end ()) {
		comids->push_back (id);
		duplicate_check->insert (id);

		for (link = from.begin ();
		!error
		&& (link != from.end ());
		++link)
			if (!(*link)->accumulate_upstream (comids, duplicate_check, log))
				error = true;
	}
	else {
		std::vector <long long>::const_iterator node;
		log.add_formatted ("ERROR, tree loop.\n");
		for (node = comids->begin ();
		node != comids->end ();
		++node)
			log.add_formatted ("\t%lld", *node);
		log += "\n";
		error = true;
	}
	return !error;
}

void HUC_network_link::add_source
	(const HUC_network_link *other)

// Root of other is a duplicate for this, so take its leaves

{
	from.insert ((HUC_network_link *) other);
}

void HUC_network_link::copy_leaves
	(const HUC_network_link *other)

// Root of other is a duplicate for this, so take its leaves

{
	std::set <HUC_network_link *>::const_iterator link;

	for (link = other->from.begin ();
	link != other->from.end ();
	++link) {
		from.insert (*link);
	}
}

HUC_network_link *HUC_network_link::match_id_slow
	(const long long target_id) const

{

	if (id == target_id)
		return (HUC_network_link *) this;
	else {
		HUC_network_link *found = NULL;
		std::set <HUC_network_link *>::const_iterator leaf;

		// 2015-03-11 Compiler error? Tries to increment leaf even though from is empty
		// if (from.size () > 0)
		for (leaf = from.begin ();
		(found == NULL)
		&& (leaf != from.end ());
		leaf++)
			found = (*leaf)->match_id_slow (target_id);

		return found;
	}
}

HUC_network_link *HUC_network_link::match_just_below
	(const long long target_id) const

// Find a node who's immediate from list includes target_id

{

	HUC_network_link *found = NULL;
	std::set <HUC_network_link *>::const_iterator leaf;

	// 2015-03-11 Compiler error? Tries to increment leaf even though from is empty
	// if (from.size () > 0)
	for (leaf = from.begin ();
	(found == NULL)
	&& (leaf != from.end ());
	leaf++)
		if ((*leaf)->id == target_id)
			found = (HUC_network_link *) this;
		else
			found = (*leaf)->match_just_below (target_id);

	return (found);
}

bool HUC_tree::accumulate_upstream
	(const long long comid,
	std::vector <long long> *comids,
	dynamic_string &log) const

{
	std::map <long long, HUC_network_link *>::const_iterator branch;
	std::set <long long> duplicate_check;
	HUC_network_link *leaf;
	bool found = false, error = false;

	for (branch = root_fragments.begin ();
	!found
	&& !error
	&& (branch != root_fragments.end ());
	++branch) {
		if ((leaf = branch->second->match_id_slow (comid)) != NULL) {
			if (!leaf->accumulate_upstream (comids, &duplicate_check, log))
				error = true;
			found = true;
		}
	}
	return !error;
}

bool HUC_tree::read
	(dynamic_string &path_huc_fromto,
	dynamic_string &log)

{
	char *position, *token_huc, *token_tohuc;
	FILE *file_csv;
	char data [256];
	long count_hucs = 0;
	long long from_huc, to_huc;
	bool error = false;

	if (fopen_s (&file_csv, path_huc_fromto.get_text_ascii (), "r") == 0) {

		// huc12,tohuc
		// 31401030101,31401030102
		// 31401030102,31401030103

		if (fgets (data, 256, file_csv)) {
			while (fgets (data, 256, file_csv)) {
				position = NULL;
				if (((token_huc = strtok_s (data, ",\r\n", &position)) != NULL)
				&& ((token_tohuc = strtok_s (NULL, ",\r\n", &position)) != NULL)) {
					from_huc = atoll (token_huc);
					to_huc = atoll (token_tohuc);
					if (from_huc != 0) {
						unique_hucs.insert (from_huc);

						if (to_huc != 0)
							unique_hucs.insert (to_huc);

						if (from_huc == to_huc) {
							log.add_formatted ("WARNING, HUC loops to itself at %lld.\n", from_huc);
							from_to.insert (std::pair <long long, long long> (from_huc, 0));
						}
						else
							// follow_downstream expects from_to to have 0 for river outlet
							from_to.insert (std::pair <long long, long long> (from_huc, to_huc));

						++count_hucs;
					}
				}

			}
		}

		fclose (file_csv);
		log.add_formatted ("HUC count: %ld\n", count_hucs);
	}
	else {
		error = true;
		log += "ERROR, can't open HUC file \"";
		log += path_huc_fromto;
		log += "\"\n";
	}
	return !error;
}

bool HUC_tree::read_source_12
	(dynamic_string &path_huc_fromto,
	const long huc_2_ge,
	const long huc_2_lt,
	dynamic_string &log)

{
	FILE *file_csv;
	char data [2048];
	dynamic_string parse_data;
	std::vector <dynamic_string> tokens;
	long count_hucs, count_invalid;
	int from_column_index, to_column_index;
	long long from_huc, to_huc, huc_lt, huc_ge;
	bool error = false;

	if (huc_2_ge != -1) {
		huc_ge = huc_2_ge * 1.0e10;
		huc_lt = huc_2_lt * 1.0e10;
	}
	else {
		huc_ge = -1;
		huc_lt = -1;
	}

	if (fopen_s (&file_csv, path_huc_fromto.get_text_ascii (), "r") == 0) {

		// OBJECTID,TNMID,MetaSourceID,SourceDataDesc,SourceOriginator,SourceFeatureID,LoadDate,GNIS_ID,AreaAcres,AreaSqKm,States,HUC12,Name,HUType,HUMod,ToHUC,NonContributingAcres,NonContributingSqKm,Shape_Length,Shape_Area
		// 1,{B1EF0C55-72ED-4FF6-A3BA-97A87C6A6C47},,,,,1/18/2013 7:07:56,0,"12,663.000000000000000",51.245388179999999,AL,031401030101,Pond Creek,S,NM,031401030102,0.000000000000000,0.000000000000000,0.396873074861680,0.004858583040857
		// 2,{F0D9874D-52BA-4FDC-A5E6-E259B627764D},,,,,1/18/2013 7:07:56,0,"37,030.000000000000000",149.855225800000000,AL,031401030102,Lightwood Knot Creek,S,NM,031401030103,0.000000000000000,0.000000000000000,0.845521565832901,0.014214008021309
		// 3,{2E0CB201-5672-45B5-8CA7-A60070122697},,,,,1/18/2013 7:07:56,0,"26,011.000000000000000",105.262875460000000,AL,031401030103,Poley Creek-Lightwood Knot Creek,S,NM,031401030302,0.000000000000000,0.000000000000000,0.663940716431087,0.009979460735394

		count_hucs = count_invalid = 0;

		if (fgets (data, 256, file_csv)) {
			while (!error
			&& fgets (data, 2048, file_csv)) {
				parse_data = data;
				parse_data.tokenize (",\r\n", &tokens, true, false);

				from_column_index = 11;
				to_column_index = 15;

				if (tokens.size () != 20) {
					if (tokens[1].left (38) == "{E76E59D4-FC03-4C34-A858-F3E60C7D6165}") {
						// linefeed after token [1]
						fgets (data, 2048, file_csv);
						// Subsequent line starts with a single quote, so skip it
						parse_data = &data[1];
						parse_data.tokenize (",\r\n", &tokens, true, false);
						from_column_index = 10;
						to_column_index = 14;
					}
					else {
						log += "ERROR, invalid # of tokens in source file.\n\"";
						log += data;
						log += "\"\n";
						error = true;
					}
				}

				if (!error) {
					if (convert_tokens (tokens[from_column_index], tokens[to_column_index], &from_huc, &to_huc, log)) {
						if (huc_ge != -1) {
							// Filter to huc

							if ((from_huc >= huc_ge)
							&& (from_huc < huc_lt)) {
								++count_hucs;
								unique_hucs.insert (from_huc);

								if (from_huc != to_huc)
									from_to.insert (std::pair <long long, long long> (from_huc, to_huc));
							}
						}
						else {
							if (from_huc != 0) {
								++count_hucs;
								unique_hucs.insert (from_huc);

								if (to_huc != 0)
									unique_hucs.insert (to_huc);

								if (from_huc == to_huc) {
									log.add_formatted ("WARNING, HUC loops to itself at %lld.\n", from_huc);
									from_to.insert (std::pair <long long, long long> (from_huc, 0));
								}
								else
									from_to.insert (std::pair <long long, long long> (from_huc, to_huc));
							}
						}
					}
					else
						++count_invalid;

				}
			}
		}

		fclose (file_csv);
		log.add_formatted ("HUC count: %ld\n", count_hucs);
		log.add_formatted ("Invalid HUCs: %ld\n", count_invalid);
	}
	else {
		error = true;
		log += "ERROR, can't open HUC file \"";
		log += path_huc_fromto;
		log += "\"\n";
	}
	return !error;
}

void recurse_tree
	(const long long from_huc,
	std::set <long long> *all_upstream,
	std::map <long long, std::set <long long>> *tree)

// Add this to all_upstream
// Find any branches in tree keyed by from_huc and add all second members

{
	std::map <long long, std::set <long long>>::iterator found_branch;
	std::set <long long>::iterator upstream_huc;

	all_upstream->insert (from_huc);

	if ((found_branch = tree->find (from_huc)) != tree->end ()) {
		// everthing in found_branch is upstream from branch
		for (upstream_huc = found_branch->second.begin ();
		upstream_huc != found_branch->second.end ();
		++upstream_huc)
			recurse_tree (*upstream_huc, all_upstream, tree);
	}
}

void HUC_tree::write
	(dynamic_string &filename,
	interface_window *view,
	dynamic_string &log)

{
	std::set <long long>::iterator huc;
	std::map <long long, HUC_network_link *>::iterator branch;
	FILE *f;
	bool direct_to_file = false;
	std::vector <long long> upstream;
	std::vector <long long>::iterator source;
	std::set <long long> duplicate_check;

	if (fopen_s (&f, filename.get_text_ascii (), "w") == 0) {

		for (huc = unique_hucs.begin ();
		huc != unique_hucs.end ();
		++huc) {
			// fprintf (f, "%lld", *huc);
			if ((branch = all_ids.find (*huc)) != all_ids.end ()) {

				// log.add_formatted ("%lld\t%d\n", branch->first, branch->second->depth ());

				if (direct_to_file) {
					branch->second->write_upstream (f);
					fprintf (f, "\n");
				}
				else {
					view->update_progress_formatted (1, "%lld", branch->first);
					if (branch->second->accumulate_upstream (&upstream, &duplicate_check, log)) {
						fprintf (f, "%lld", *huc);
						for (source = upstream.begin (); source != upstream.end (); ++source) {
							fprintf (f, "\t%lld", *source);
						}
						fprintf (f, "\n");
					}

					duplicate_check.clear ();
					upstream.clear ();
				}

				fflush (f);
			}
			else
				log.add_formatted ("ERROR, branch not found in all_ids for %lld\n", *huc);
		}
		fclose (f);
	}
	else {
		log += "ERROR, can't open output file \"";
		log += filename;
		log += "\".\n";
	}
}

void HUC_tree::add_river
	(const HUC_network_link *outlet)

// HUC_network_links are found corrupted.  Test as rivers are inserted

{
	root_fragments.insert (std::pair <long long, HUC_network_link *> (outlet->id, (HUC_network_link *) outlet));
}

void HUC_tree::add_link
	(const HUC_network_link *node)

// HUC_network_links are found corrupted.  Test as each is inserted

{
	all_ids.insert (std::pair <long long, HUC_network_link *> (node->id, (HUC_network_link *) node));
}

HUC_network_link *HUC_tree::cleanup_loop
	(HUC_network_link *loopy,
	dynamic_string &log)

// Break the link just below duplicate. Make duplicate an outlet

{
	HUC_network_link *found = NULL, *child = NULL, *duplicate = NULL;
	std::set <HUC_network_link *>::const_iterator leaf;

	// Find the one below duplicate
	if ((child = loopy->match_just_below (loopy->id)) != NULL) {
		log.add_formatted ("cleanup_loop: split at %lld.\n", child->id);

		// Can't call match_id_upstream because it will stop immediately for id match
		for (leaf = child->from.begin ();
		leaf != child->from.end ();
		leaf++)
			if ((*leaf)->id == loopy->id) {
				duplicate = *leaf;
				child->from.erase (leaf);
				add_river (duplicate);
			}

		follow_downstream (loopy, log);
	}
	else {
		log += "cleanup_loop: duplicate not found.\n";
		broken_links.insert (std::pair <long long, HUC_network_link *> (loopy->id, loopy));
	}

	return found;
}

void HUC_tree::build_tree
	(interface_window *view,
	dynamic_string &log)

{
	std::map <long long, long long>::iterator link;
	// std::map <long long, HUC_network_link *>::iterator test_id;
	HUC_network_link *branch_from, *branch_to;
	int link_count = 0, unprocessed_count = 1;

	while (unprocessed_count > 0) {
		unprocessed_count = 0;
		for (link = from_to.begin ();
		link != from_to.end ();
		++link) {
			if (link_count % 10 == 0)
				view->update_progress_formatted (1, "%lld -> %lld", link->first, link->second);

			// ignore when 'from' is 0 
			// ignore when 'to' is -1
			// when 'to' is -1 we're at end of a line that's already been processed
			if ((link->first != 0)
			&& (link->second > 0)) {
				HUC_network_link *old_branch_from, *old_branch_to;

				if ((old_branch_from = match_id (link->first)) != NULL) {

					if ((old_branch_to = match_id (link->second)) != NULL)
						// Both ends of link exist already
						old_branch_to->add_source (old_branch_from);
					else {
						// From was previously added as a 'To'
						//root_fragments.erase (link->first);
						branch_to = new HUC_network_link;
						branch_to->id = link->second;
						branch_to->add_source (old_branch_from);
						add_link (branch_to);
					}
				}
				else
					if ((old_branch_to = match_id (link->second)) != NULL) {
						// to was previously added 
						branch_from = new HUC_network_link;
						branch_from->id = link->first;
						add_link (branch_from);
						// branch_from drains to old_branch
						old_branch_to->add_source (branch_from);
					}
					else {
						// From & to both new
						branch_from = new HUC_network_link;
						branch_from->id = link->first;
						add_link (branch_from);

						branch_to = new HUC_network_link;
						branch_to->id = link->second;
						branch_to->add_source (branch_from);
						add_link (branch_to);

						// add_river (branch_to);
					}
					// Mark link as processed
					link->second = -1;
					++unprocessed_count;
			}
			else {
				if ((link->first != 0)
				&& (link->second == 0)) {
					// Outlet
					HUC_network_link *old_branch;
					if ((old_branch = match_id (link->first)) != NULL) {
						add_river (old_branch);
					}
					else {
						branch_to = new HUC_network_link;
						branch_to->id = link->first;
						add_link (branch_to);
						add_river (branch_to);
					}
					// Mark link as processed
					link->second = -1;
					++unprocessed_count;
				}
				else
					if ((link->first != 0)
					&& (link->second == -1)) {
						// processed already
					}
					else {
						log.add_formatted ("???\t%lld\t%lld\n", link->first, link->second);
					}
			}
			++link_count;
		}
	}
	log.add_formatted ("Leaf count %d\n", link_count);
	log.add_formatted ("Branch count %d\n", root_fragments.size ());

	if (false) {
		HUC_network_link *old_branch;
		long long follow_id = 70101080107;
		log.add_formatted ("\t%lld\n", follow_id);
		while ((old_branch = match_just_below (follow_id)) != NULL) {
			log.add_formatted ("->\t%lld\n", follow_id);
			follow_id = old_branch->id;
		}

		log += "\n";

		log.add_formatted ("all_ids\t%lld\n", follow_id);
		while ((old_branch = match_all_just_below (follow_id)) != NULL) {
			log.add_formatted ("->\t%lld\n", old_branch->id);
			follow_id = old_branch->id;
		}

	}

}

bool HUC_tree::follow_downstream
	(const HUC_network_link *tributary,
	dynamic_string &log)

// Recursive
// The map 'from_to' may describe flow from tributary to a new node
// If so, create a new HUC_network_link that has tributary in its 'from' vector

{
	std::map <long long, long long>::iterator link;
	HUC_network_link *branch, *old_branch;

	if ((link = from_to.find (tributary->id)) != from_to.end ()) {
		// next link in this chain
		if (link->second == -1) {
			// traversed already
			// Find the existing branch with a from matching comid
			// current_root will lead into this old branch
			if ((old_branch = match_id_slow (link->first)) != NULL) {
				ASSERT (old_branch->id == tributary->id);
				// attach current_root to old_branch
				old_branch->copy_leaves (tributary);
			}
			else
				// This link is marked as having been visited, but what river did it become part of?
				// Loop detection; it is part of this tributary
				return false;
		}
		else
			if (link->second == 0) {
				// mouth of river
				add_river (tributary);
				link->second = -1;
			}
			else {
				// Not 0 or -1
				// Continue downstream
				branch = new HUC_network_link;
				branch->id = link->second;
				branch->add_source ((HUC_network_link*) tributary);
				// 2017-07-18 Original code put link->second into all_ids here (just after setting that variable to -1).
				// Instead add link->first
				add_link (branch);
				link->second = -1;
				return follow_downstream (branch, log);
			}
			return true;
	}
	else {
		// destination not found, error
		// 03N (10466473->9889032).  9889032 is actually in 02
		// Act as if it were a 0, treat FROM as a separate river when encountered
		// Must still add to root_fragments so it can be found by other leaves
		// 2015-03-23 Must index using the segment above missing segment,
		// in case two lost rivers drain to one missing comid
		HUC_network_link *segment_above;
		std::vector <long long> trails;
		std::vector <long long>::const_iterator trailstop;
		std::set <long long> duplicate_check;

		ASSERT (tributary->from.size () == 1);
		segment_above = *(tributary->from.begin ());

		add_river (segment_above);
		// lost_rivers.insert (segment_above->id);
		// log.add_formatted ("Outlet %lld -> %lld.\n", segment_above->id, tributary->id);
		tributary->accumulate_upstream (&trails, &duplicate_check, log);
		for (trailstop = trails.begin ();
		trailstop != trails.end ();
		++trailstop)
			log.add_formatted ("\t%lld", *trailstop);
		log += "\n";
		// Add a link "-> 0" for this?  Not necessary

		return true;
	}
}

void HUC_tree::build_tree_recursive
	(interface_window *view,
	dynamic_string &log)

{
	std::map <long long, long long>::iterator link;
	HUC_network_link *branch, *branch_from;
	int link_count = 0;

	// link = from_to->find (18264243);
	for (link = from_to.begin ();
	link != from_to.end ();
	++link) {

		if (link_count % 10 == 0)
			view->update_progress_formatted (1, "%lld -> %lld", link->first, link->second);

		// ignore when 'from' is 0 
		// ignore when 'to' is -1
		// when 'to' is -1 we're at end of a line that's already been processed
		if ((link->first != 0)
		&& (link->second > 0)) {

			branch_from = new HUC_network_link;
			branch_from->id = link->first;
			add_link (branch_from);

			branch = new HUC_network_link;
			branch->id = link->second;
			branch->add_source (branch_from);
			link->second = -1;

			if (follow_downstream (branch, log)) {
				add_link (branch);
			}
			else {
				log += "ERROR, previously traversed branch not found (loop).  ";
				log.add_formatted ("%lld -> %lld\n", link->first, link->second);
				// find matching id above in branch
				// clip from there to branch outlet
				cleanup_loop (branch, log);
			}
		}
		else {
			if ((link->first != 0)
			&& (link->second == 0)) {
				// 2015-03-06 67121595->0 is a segment (no leaves) that drains to coastline.  Still needs to be processed here.
				branch = new HUC_network_link;
				branch->id = link->first;
				add_link (branch);
				add_river (branch);
				link->second = -1;
			}
		}
		++link_count;
	}
	log.add_formatted ("Leaf count %d\n", link_count);
	log.add_formatted ("Branch count %d\n", root_fragments.size ());
	/*
	log.add_formatted ("Lost River count %d\n", lost_rivers.size ());

	log += "Lost Branches\n";
	std::set <long long>::const_iterator lost;
	for (lost = lost_rivers.begin ();
	lost != lost_rivers.end ();
	++lost) {
		log.add_formatted ("\t%lld\n", *lost);
	}
	*/
}

HUC_network_link *HUC_tree::match_id_slow
	(const long long comid) const

// 2015-03-13 As tree is built, all_comids is incomplete.  Don't rely on it

{
	std::map <long long, HUC_network_link *>::const_iterator branch;
	HUC_network_link *found = NULL;

	for (branch = root_fragments.begin ();
	!found
	&& (branch != root_fragments.end ());
	++branch)
		found = branch->second->match_id_slow (comid);

	return found;
}

HUC_network_link *HUC_tree::match_id
	(const long long comid) const

// 2015-03-13 As tree is built, all_comids is incomplete.  Don't rely on it

{
	std::map <long long, HUC_network_link *>::const_iterator branch;
	HUC_network_link *found = NULL;

	if ((branch = all_ids.find (comid)) != all_ids.end ())
		found = branch->second;

	return found;
}

HUC_network_link *HUC_tree::match_just_below
	(const long long comid) const

{
	std::map <long long, HUC_network_link *>::const_iterator branch;
	HUC_network_link *found = NULL;

	for (branch = root_fragments.begin ();
	!found
	&& (branch != root_fragments.end ());
	++branch)
		found = branch->second->match_just_below (comid);

	return found;
}

HUC_network_link *HUC_tree::match_all_just_below
	(const long long comid) const

// Look for id that comid flows to, non-recursive, using all_ids rather than root_fragments

{
	std::map <long long, HUC_network_link *>::const_iterator branch;
	std::set <HUC_network_link *>::const_iterator leaf;
	HUC_network_link *found = NULL;

	for (branch = all_ids.begin ();
	!found
	&& (branch != all_ids.end ());
	++branch) {
		for (leaf = branch->second->from.begin ();
		!found
		&& (leaf != branch->second->from.end ());
		leaf++)
			if ((*leaf)->id == comid)
				found = branch->second;
	}

	return found;
}

bool HUC_tree::follow_raw_downstream
	(const long long id,
	dynamic_string &log)

// non-recursive
// The map 'from_to' may describe flow from tributary to a new node
// If so, create a new HUC_network_link that has tributary in its 'from' vector

{
	std::map <long long, long long>::iterator link;
	long long follow_id = id;

	while ((link = from_to.find (follow_id)) != from_to.end ()) {
		log.add_formatted ("%lld\t%lld\n", link->first, link->second);
		follow_id = link->second;
	}
	return true;
}


bool HUC_tree::build_vector_shapefile
	(dynamic_string &filename_vector_shp,
	map_layer *layer_hucs,
	map_layer *layer_fragments,
	dynamic_map *map,
	const bool huc_id_is_name,
	interface_window *update_display,
	dynamic_string &log)

// Use tree network to produce a new shapefile with vectors from each huc to the one it drains to

{
	bool error = false;
	std::vector <map_object *>::iterator polygon_from;
	std::map <long long, HUC_network_link *>::iterator fragment;
	std::set <HUC_network_link *>::iterator fragment_from;
	logical_coordinate fragment_spot;
	map_object *polygon_to;
	map_object_vector *vector;
	HUC_network_link *destination;
	dynamic_string huc_name;
	long long huc_id;

	// save time by creating this layer as wx so map_objects don't have to be converted
	map_layer *layer_vectors = map->create_new (MAP_OBJECT_VECTOR);
	layer_vectors->enclosure = MAP_POLYGON_NOT_ENCLOSED; // Don't draw line from first node to last
	layer_vectors->name = "Vectors"; // Was importer->filename_source.  Needed for OnShowRiver
	layer_vectors->color = RGB (0, 0, 255);
	layer_vectors->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_vectors->attribute_count_text = 1;
	layer_vectors->initialize_attributes = true; // clear all attributes as polygons are created

	map->layers.push_back (layer_vectors);

	for (polygon_from = layer_hucs->objects.begin ();
	polygon_from != layer_hucs->objects.end ();
	++polygon_from) {

		// Create a vector from this polygon to its destination

		vector = (map_object_vector *) layer_vectors->create_new (MAP_OBJECT_VECTOR);
		vector->id = (*polygon_from)->id;
		vector->name = (*polygon_from)->name;
		(*polygon_from)->centroid (&vector->latitude, &vector->longitude);

		if (huc_id_is_name)
			huc_id = atoll ((*polygon_from)->name.get_text_ascii ());
		else
			huc_id = (long) (long) (*polygon_from)->id;

		// Search tree for node that polygon_from drains into
		if ((destination = match_all_just_below (huc_id)) != NULL) {
			// Find the HUC polygon with the same id

			if (huc_id_is_name) {
				huc_name.format ("%012lld", destination->id);
				polygon_to = layer_hucs->match_name (huc_name);
			}
			else
				polygon_to = layer_hucs->match_id ((long) destination->id);

			if (polygon_to != NULL) {
				// Vector from polygon_from to polygon_to
				polygon_to->centroid (&vector->destination_latitude, &vector->destination_longitude);
				vector->attributes_text[0] = huc_name;
				layer_vectors->objects.push_back (vector);
			}
			else
				delete vector;
		}
		else {
			// river outlet
			delete vector;
		}

		if ((fragment = broken_links.find (huc_id)) != broken_links.end ()) {
			vector = (map_object_vector *) layer_fragments->create_new (MAP_OBJECT_VECTOR);
			vector->id = (*polygon_from)->id;
			vector->name = (*polygon_from)->name;
			(*polygon_from)->centroid (&vector->latitude, &vector->longitude);

			huc_name.format ("%012lld", fragment->first);
			if ((fragment_from = fragment->second->from.begin ()) != fragment->second->from.end ()) {
				while (fragment_from != fragment->second->from.end ()) {
					huc_name.format ("%012lld", (*fragment_from)->id);
					if ((polygon_to = layer_hucs->match_name (huc_name)) != NULL) {
						// Vector from polygon_from to polygon_to
						polygon_to->centroid (&vector->destination_latitude, &vector->destination_longitude);
						layer_fragments->objects.push_back (vector);
					}
					else {
						fragment_spot.set ((*polygon_from)->longitude, (*polygon_from)->latitude);
						vector->destination_longitude = vector->longitude;
						vector->destination_latitude = vector->latitude + offset_latitude_meters (fragment_spot, 1000.0);
						vector->attributes_text[0] = huc_name;
						layer_fragments->objects.push_back (vector);
					}
					++fragment_from;
				}
			}
			else {
				fragment_spot.set ((*polygon_from)->longitude, (*polygon_from)->latitude);
				vector->destination_longitude = vector->longitude;
				vector->destination_latitude = vector->latitude + offset_latitude_meters (fragment_spot, -1000.0);
				layer_fragments->objects.push_back (vector);
			}
		}
	}

	log.add_formatted ("Vector layer size %d\n", layer_vectors->objects.size ());

	layer_vectors->set_extent ();
	return !error;
}

long long combine_8
	(long long huc12)

{
	long long huc8 = huc12 - (huc12 % 10000);
	huc8 /= 10000;

	return huc8;
}

bool HUC_tree::read_source_combine
	(dynamic_string &path_huc_fromto,
	const long huc_3_ge,
	const long huc_3_lt,
	dynamic_string &log)

{
	FILE *file_csv;
	char data [2048];
	dynamic_string parse_data;
	std::vector <dynamic_string> tokens;
	long count_hucs;
	int from_column_index, to_column_index;
	long from_huc_8, to_huc_8, huc_8_lt, huc_8_ge;
	bool error = false;

	huc_8_ge = huc_3_ge * 1.0e5;
	huc_8_lt = huc_3_lt * 1.0e5;

	if (fopen_s (&file_csv, path_huc_fromto.get_text_ascii (), "r") == 0) {

		// OBJECTID,TNMID,MetaSourceID,SourceDataDesc,SourceOriginator,SourceFeatureID,LoadDate,GNIS_ID,AreaAcres,AreaSqKm,States,HUC12,Name,HUType,HUMod,ToHUC,NonContributingAcres,NonContributingSqKm,Shape_Length,Shape_Area
		// 1,{B1EF0C55-72ED-4FF6-A3BA-97A87C6A6C47},,,,,1/18/2013 7:07:56,0,"12,663.000000000000000",51.245388179999999,AL,031401030101,Pond Creek,S,NM,031401030102,0.000000000000000,0.000000000000000,0.396873074861680,0.004858583040857
		// 2,{F0D9874D-52BA-4FDC-A5E6-E259B627764D},,,,,1/18/2013 7:07:56,0,"37,030.000000000000000",149.855225800000000,AL,031401030102,Lightwood Knot Creek,S,NM,031401030103,0.000000000000000,0.000000000000000,0.845521565832901,0.014214008021309
		// 3,{2E0CB201-5672-45B5-8CA7-A60070122697},,,,,1/18/2013 7:07:56,0,"26,011.000000000000000",105.262875460000000,AL,031401030103,Poley Creek-Lightwood Knot Creek,S,NM,031401030302,0.000000000000000,0.000000000000000,0.663940716431087,0.009979460735394

		count_hucs = 0;

		if (fgets (data, 256, file_csv)) {
			while (!error
			&& fgets (data, 2048, file_csv)) {
				parse_data = data;
				parse_data.tokenize (",\r\n", &tokens, true, false);

				from_column_index = 11;
				to_column_index = 15;

				if (tokens.size () != 20) {
					if (tokens[1].left (38) == "{E76E59D4-FC03-4C34-A858-F3E60C7D6165}") {
						// linefeed after token [1]
						fgets (data, 2048, file_csv);
						// Subsequent line starts with a single quote, so skip it
						parse_data = &data[1];
						parse_data.tokenize (",\r\n", &tokens, true, false);
						from_column_index = 10;
						to_column_index = 14;
					}
					else {
						log += "ERROR, invalid # of tokens in source file.\n\"";
						log += data;
						log += "\"\n";
						error = true;
					}
				}

				if (!error) {
					if (convert_tokens (tokens[from_column_index], tokens[to_column_index], &from_huc_8, &to_huc_8, log)) {

						if (huc_3_ge != -1) {
							// Filter to huc

							if ((from_huc_8 != 0)
							&& (from_huc_8 != to_huc_8)) {
								if ((from_huc_8 >= huc_8_ge)
								&& (from_huc_8 < huc_8_lt)) {
									++count_hucs;
									unique_hucs.insert ((long long) from_huc_8);
									if (to_huc_8 != 0)
										unique_hucs.insert ((long long) to_huc_8);
									from_to.insert (std::pair <long long, long long> ((long long) from_huc_8, (long long) to_huc_8));
								}
							}
						}
						else {
							if ((from_huc_8 != 0)
							&& (from_huc_8 != to_huc_8)) {
								++count_hucs;
								unique_hucs.insert ((long long) from_huc_8);

								if (to_huc_8 != 0)
									unique_hucs.insert ((long long) to_huc_8);

								from_to.insert (std::pair <long long, long long> ((long long) from_huc_8, (long long) to_huc_8));
							}
						}
					}

				}
			}
		}

		fclose (file_csv);
		log.add_formatted ("HUC count: %ld\n", count_hucs);
	}
	else {
		error = true;
		log += "ERROR, can't open HUC file \"";
		log += path_huc_fromto;
		log += "\"\n";
	}
	return !error;
}

bool HUC_tree::convert_tokens
	(dynamic_string &from,
	dynamic_string &to,
	long *from_huc_8,
	long *to_huc_8,
	dynamic_string &log)

{
	long long from_huc, to_huc, old_huc;

	if (convert_tokens (from, to, &from_huc, &to_huc, log)) {
		*from_huc_8 = combine_8 (from_huc);
		*to_huc_8 = combine_8 (to_huc);

		if ((*from_huc_8 == 2040303)
		&& (*to_huc_8 == 2040202)) {
			log.add_formatted ("Correction, removing %ld -> %ld.\n", *from_huc_8, *to_huc_8);
			return false;
		}
		else
			if ((*from_huc_8 == 4060101)
			&& (*to_huc_8 == 4060102)) {
				log.add_formatted ("Correction, removing %ld -> %ld.\n", *from_huc_8, *to_huc_8);
				return false;
			}
			else
				if ((*from_huc_8 == 4060102)
				&& (*to_huc_8 == 4060101)) {
					log.add_formatted ("Correction, removing %ld -> %ld.\n", *from_huc_8, *to_huc_8);
					return false;
				}
				else
					if ((*from_huc_8 == 10240005)
					&& (*to_huc_8 == 10240001)) {
						log.add_formatted ("Correction, removing %ld -> %ld.\n", *from_huc_8, *to_huc_8);
						return false;
					}
					else
						if ((*from_huc_8 == 3100102)
						&& (*to_huc_8 == 3140202)) {
							log.add_formatted ("Correction, removing %ld -> %ld.\n", *from_huc_8, *to_huc_8);
							return false;
						}
						else
							if ((*from_huc_8 == 3110205)
							&& (*to_huc_8 == 3110201)) {
								// 2017-08-22 central Florida
								log.add_formatted ("Correction, removing %ld -> %ld.\n", *from_huc_8, *to_huc_8);
								return false;
							}
							else
								if (((*from_huc_8 == 3120001) && (*to_huc_8 == 3120003))
								|| ((*from_huc_8 == 3120003) && (*to_huc_8 == 3120001))) {
									// 2017-08-22 west Florida, both drain to Gulf
									log.add_formatted ("Correction, removing %ld -> %ld.\n", *from_huc_8, *to_huc_8);
									return false;
								}
								else
									if ((*from_huc_8 == 16030005) && (*to_huc_8 == 16030008)) {
										// 2017-08-22 ought to be 16030008 -> 16030005 -> 16030009
										old_huc = *to_huc_8;
										*to_huc_8 = 16030009;
										log.add_formatted ("Correction, changing %ld -> %ld to %ld -> %ld.\n", *from_huc_8, old_huc, *from_huc_8, *to_huc_8);
										return false;
									}
						/*
						else
							if ((*from_huc_8 == 9020203)
							&& (*to_huc_8 == 0)) {
								// 9020203 -> 9020204 is dropped if this is allowed through
								log.add_formatted ("Correction, removing %ld -> %ld.\n", *from_huc_8, *to_huc_8);
								return false;
							}
							*/
		return true;
	}
	else
		return false;
}

bool drop_huc_12
	(const long long target_from,
	const long long target_to,
	long long *from_huc,
	long long *to_huc,
	dynamic_string &log)

{
	if ((*from_huc == target_from)
	&& (*to_huc == target_to)) {
		log.add_formatted ("Correction: %lld -> %lld dropped.\n", *from_huc, *to_huc);
		return true;
	}
	else
		return false;
}

void correct_huc_12
	(const long long target_from,
	const long long target_to,
	const long long new_target_from,
	const long long new_target_to,
	long long *from_huc,
	long long *to_huc,
	dynamic_string &log)

{
	if ((*from_huc == target_from)
	&& (*to_huc == target_to)) {
		log.add_formatted ("Correction: %lld -> %lld changed to %lld -> %lld.\n", *from_huc, *to_huc, new_target_from, new_target_to);
		*from_huc = new_target_from;
		*to_huc = new_target_to;
	}
}

bool HUC_tree::convert_tokens
	(dynamic_string &from,
	dynamic_string &to,
	long long *from_huc,
	long long *to_huc,
	dynamic_string &log)

{
	bool valid = true;
	long long old_huc;

	if (is_number (from)) {
		*from_huc = atoll (from.get_text_ascii ());
		to.trim_left ();
		to.trim_right ();

		if ((to == "OCEAN")
		|| (to == "CANADA")
		|| (to == "MEXICO")
		|| (to == "CLOSED BASIN")
		|| (to == "Closed Basin"))
			*to_huc = 0;
		else
			if (is_number (to)) {
				*to_huc = atoll (to.get_text_ascii ());

				if (*to_huc == 17070702020703) {
					// DKeiser 2017-08-21
					old_huc = *to_huc;
					*to_huc = 170702020703;
					log.add_formatted ("Correction: %lld changed to %lld.\n", old_huc, *to_huc);
					valid = false;
				}
				else
					if (*to_huc == 100800080104201) {
						// DKeiser 2017-08-21
						old_huc = *to_huc;
						*to_huc = 100800080104;
						log.add_formatted ("Correction: %lld changed to %lld.\n", old_huc, *to_huc);
						valid = false;
					}
					else
						if (*to_huc == 1304102120802) {
							// DKeiser 2017-08-21
							old_huc = *to_huc;
							*to_huc = 130402120802;
							log.add_formatted ("Correction: %lld changed to %lld.\n", old_huc, *to_huc);
							valid = false;
						}
						else
							if (*to_huc == 1304102120803) {
								// DKeiser 2017-08-21
								old_huc = *to_huc;
								*to_huc = 130402120803;
								log.add_formatted ("Correction: %lld changed to %lld.\n", old_huc, *to_huc);
								valid = false;
							}
							else
								if (*to_huc == 1304102120903) {
									// DKeiser 2017-08-21
									old_huc = *to_huc;
									*to_huc = 130402120903;
									log.add_formatted ("Correction: %lld changed to %lld.\n", old_huc, *to_huc);
									valid = false;
								}
								else
									if (*to_huc == 315020301) {
										log.add_formatted ("Correction: %lld -> %lld dropped.\n", *from_huc, *to_huc);
										valid = false;
									}
									else {
										// Area 01
										correct_huc_12 (10400020207, 10200020406, 10400020207, 10400020406, from_huc, to_huc, log); // 2017-08-24
										correct_huc_12 (10400020405, 10200040406, 10400020405, 10400020406, from_huc, to_huc, log); // 2017-08-24

										// Area 03
										correct_huc_12 (31001020502, 31402020102, 31001020502, 31001020503, from_huc, to_huc, log); // 2017-08-24
										correct_huc_12 (30801011606, 30801011506, 30801011606, 30801012001, from_huc, to_huc, log); // 2017-08-25

										// Area 04
										correct_huc_12 (40601020201, 40601010202, 40601020201, 40601020202, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020101, 40601010103, 40601020101, 40601020103, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020103, 40601010104, 40601020103, 40601020104, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020102, 40601010104, 40601020102, 40601020104, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020209, 40601010402, 40601020209, 40601020402, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020402, 40601010406, 40601020402, 40601020406, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020404, 40601010406, 40601020404, 40601020406, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020403, 40601010404, 40601020403, 40601020404, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020405, 40601010406, 40601020405, 40601020406, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020501, 40601010504, 40601020501, 40601020504, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020505, 40601010506, 40601020505, 40601020506, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020601, 40601010605, 40601020601, 40601020605, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020607, 40601010701, 40601020607, 40601020701, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010103, 40601020104, 40601010103, 40601010104, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010102, 40601020103, 40601010102, 40601010103, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010201, 40601020202, 40601010201, 40601010203, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010506, 40601020507, 40601010506, 40601010507, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010406, 40601020507, 40601010406, 40601010507, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010601, 40601020604, 40601010601, 40601010604, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010802, 40601020803, 40601010802, 40601010803, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010803, 40601020804, 40601010803, 40601010804, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010804, 40601020901, 40601010804, 40601010901, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010902, 40601020904, 40601010902, 40601010904, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010901, 40601020904, 40601010901, 40601010904, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601021001, 40601011002, 40601021001, 40601021002, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020810, 40601010901, 40601020810, 40601020901, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010202, 40601020203, 40601010202, 40601010203, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010101, 40601020102, 40601010101, 40601010102, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010505, 40601020506, 40601010505, 40601010506, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010405, 40601020406, 40601010405, 40601010406, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010402, 40601020404, 40601010402, 40601010404, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010801, 40601020803, 40601010801, 40601010803, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010705, 40601020706, 40601010705, 40601010706, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010707, 40601020901, 40601010707, 40601010901, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020902, 40601010903, 40601020902, 40601020903, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020706, 40601010901, 40601020706, 40601020901, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020901, 40601010903, 40601020901, 40601020903, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020903, 40601010904, 40601020903, 40601020904, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010903, 40601020904, 40601010903, 40601010904, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601021003, 40601011004, 40601021003, 40601021004, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601021002, 40601011004, 40601021002, 40601021004, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020906, 40601011002, 40601020906, 40601021002, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020802, 40601010803, 40601020802, 40601020803, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020801, 40601010803, 40601020801, 40601020803, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020701, 40601010702, 40601020701, 40601020702, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010302, 40601020304, 40601010302, 40601010304, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010301, 40601020302, 40601010301, 40601010304, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010501, 40601020503, 40601010501, 40601010503, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020503, 40601010506, 40601020503, 40601020506, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020301, 40601010302, 40601020301, 40601020302, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020303, 40601010304, 40601020303, 40601020304, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020302, 40601010303, 40601020302, 40601020303, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010303, 40601020304, 40601010303, 40601010304, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020502, 40601010503, 40601020502, 40601020503, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020506, 40601010507, 40601020506, 40601020507, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010504, 40601020505, 40601010504, 40601010505, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020603, 40601010604, 40601020603, 40601020604, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020602, 40601010604, 40601020602, 40601020604, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020702, 40601010704, 40601020702, 40601020704, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010602, 40601020603, 40601010602, 40601010603, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020704, 40601010705, 40601020704, 40601020705, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020504, 40601010506, 40601020504, 40601020506, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010503, 40601020504, 40601010503, 40601010504, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020310, 40601010504, 40601020310, 40601020504, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010401, 40601020402, 40601010401, 40601010404, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010502, 40601020503, 40601010502, 40601010503, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010403, 40601020404, 40601010403, 40601010404, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010304, 40601020504, 40601010304, 40601010504, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020507, 40601010605, 40601020507, 40601020605, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010701, 40601020703, 40601010701, 40601010703, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020703, 40601010704, 40601020703, 40601020704, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020406, 40601010504, 40601020406, 40601020504, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020401, 40601010402, 40601020401, 40601020406, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010404, 40601020405, 40601010404, 40601010405, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010702, 40601020703, 40601010702, 40601010703, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010703, 40601020704, 40601010703, 40601010704, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010603, 40601020605, 40601010603, 40601010605, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601020705, 40601010706, 40601020705, 40601020706, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (40601010704, 40601020705, 40601010704, 40601010705, from_huc, to_huc, log); // 2017-08-25

										// Area 06
										correct_huc_12 (60101080903, 60101080906, 60101080903, 60101080905, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (60101080707, 60101080905, 60101080707, 60101080906, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (60101080902, 60101080906, 60101080902, 60101080905, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (60101080901, 60101080905, 60101080901, 60101080906, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (60101040204, 60101040208, 60101040204, 60101040205, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (60101080607, 60101080703, 60101080607, 60101080608, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (60101080403, 60101080603, 60101080403, 60101080606, from_huc, to_huc, log); // 2017-08-25

										// Area 08
										if (drop_huc_12 (80302071601, 80302071602, from_huc, to_huc, log)) // 2017-08-25
											valid = false;
										if (drop_huc_12 (80302071602, 80302071601, from_huc, to_huc, log)) // 2017-08-25
											valid = false;
										if (drop_huc_12 (80302071604, 80302071605, from_huc, to_huc, log)) // 2017-08-25
											valid = false;
										if (drop_huc_12 (80302071605, 80302071606, from_huc, to_huc, log)) // 2017-08-25
											valid = false;
										if (drop_huc_12 (80302071606, 80302071605, from_huc, to_huc, log)) // 2017-08-25
											valid = false;
										correct_huc_12 (80302020306, 80302020600, 80302020306, 80302020600, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (80302020205, 80302020600, 80302020205, 80302020600, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (80302020305, 80302020600, 80302020305, 80302020600, from_huc, to_huc, log); // 2017-08-25

										// Area 9
										correct_huc_12 (90201010205, 90201010203, 90201010205, 90201010502, from_huc, to_huc, log); // 2017-08-23

										// Area 10
										if (drop_huc_12 (102400051303, 102400010101, from_huc, to_huc, log)) // 2017-08-23
											valid = false;
										correct_huc_12 (101800100401, 101800100402, 101800100401, 101800100501, from_huc, to_huc, log); // 2017-08-23
										correct_huc_12 (101800100406, 101800100603, 101800100406, 101800100401, from_huc, to_huc, log); // 2017-08-23
										correct_huc_12 (101800100407, 101800100603, 101800100407, 101800100501, from_huc, to_huc, log); // 2017-08-23
										if (drop_huc_12 (101800100403, 101800100404, from_huc, to_huc, log)) // 2017-08-23
											valid = false;
										correct_huc_12 (101800100106, 101800100304, 101800100106, 101800100201, from_huc, to_huc, log); // 2017-08-23
										if (drop_huc_12 (101800100605, 101800100805, from_huc, to_huc, log)) // 2017-08-23
											valid = false;
										if (drop_huc_12 (101800100803, 101800100805, from_huc, to_huc, log)) // 2017-08-23
											valid = false;
										if (drop_huc_12 (101800100603, 101800100605, from_huc, to_huc, log)) // 2017-08-23
											valid = false;
										correct_huc_12 (101800100703, 101800100704, 101800100703, 101800100702, from_huc, to_huc, log); // 2017-08-23
										if (drop_huc_12 (101800100101, 101800100105, from_huc, to_huc, log)) // 2017-08-23
											valid = false;

										if (drop_huc_12 (101800100503, 101800100504, from_huc, to_huc, log)) // 2017-08-23
											valid = false;
										correct_huc_12 (101800100704, 101800100705, 101800100704, 101800100701, from_huc, to_huc, log); // 2017-08-23
										correct_huc_12 (101800100502, 101800100504, 101800100502, 101800100501, from_huc, to_huc, log); // 2017-08-23
										correct_huc_12 (101800100702, 101800100705, 101800100702, 101800100701, from_huc, to_huc, log); // 2017-08-23

										correct_huc_12 (101800100805, 101800100811, 101800100805, 101800100707, from_huc, to_huc, log); // 2017-08-24
										if (drop_huc_12 (101800100806, 101800100811, from_huc, to_huc, log)) // 2017-08-24
											valid = false;
										correct_huc_12 (101800100804, 101800100811, 101800100804, 101800100805, from_huc, to_huc, log); // 2017-08-24
										if (drop_huc_12 (101800100811, 101800100813, from_huc, to_huc, log)) // 2017-08-24
											valid = false;
										if (drop_huc_12 (101800100810, 101800100811, from_huc, to_huc, log)) // 2017-08-24
											valid = false;
										if (drop_huc_12 (101800100813, 101800100811, from_huc, to_huc, log)) // 2017-08-24
											valid = false;
										correct_huc_12 (101800100705, 101800100811, 101800100705, 101800100707, from_huc, to_huc, log); // 2017-08-24

										correct_huc_12 (100800010502, 100800010503, 100800010502, 100800010601, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010503, 100800010505, 100800010503, 100800010504, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010505, 100800010703, 100800010505, 100800010601, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010804, 100800011002, 100800010804, 100800010805, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010603, 100800010604, 100800010603, 100800010602, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010601, 100800010602, 100800010601, 100800010606, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010405, 100800010407, 100800010405, 100800010409, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010407, 100800010408, 100800010407, 100800010409, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800020110, 100800020305, 100800020110, 100800020203, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800020107, 100800020108, 100800020107, 100800020203, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800030108, 100800030110, 100800030108, 100800030212, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800030110, 100800030211, 100800030110, 100800030108, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800030109, 100800030110, 100800030109, 100800030108, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800030204, 100800030205, 100800030204, 100800030211, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800050101, 100800050102, 100800050101, 100800050501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150202, 101900150206, 101900150202, 101900150201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150206, 101900150405, 101900150206, 101900150208, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150402, 101900150403, 101900150402, 101900150206, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800050507, 100800050607, 100800050507, 100800050501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060105, 101800060306, 101800060105, 101800060202, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800060406, 100800050607, 100800060406, 100800060405, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060603, 101800060606, 101800060603, 101800060604, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800070203, 100800070204, 100800070203, 100800070301, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800070301, 100800070303, 100800070301, 100800070305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800070108, 100800070204, 100800070108, 100800070401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800070704, 100800070804, 100800070704, 100800070707, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800070609, 100800071105, 100800070609, 100800070404, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800071101, 100800071102, 100800071101, 100800071104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800071102, 100800071105, 100800071102, 100800071104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800090401, 100800090403, 100800090401, 100800090402, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800100701, 101800100702, 101800100701, 101800100707, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800110101, 100800110106, 100800110101, 100800110102, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800110105, 101800110107, 101800110105, 101800110103, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800110107, 101800110303, 101800110107, 101800110108, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800110604, 101800110902, 101800110604, 101800110606, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800110806, 101800110904, 101800110806, 101800110807, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800120108, 101800120502, 101800120108, 101800120201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010602, 100800010603, 100800010602, 100800010601, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800050502, 100800050505, 100800050502, 100800050501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800010504, 100800010505, 100800010504, 100800010502, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800050501, 100800050505, 100800050501, 100800050508, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800050601, 100800050607, 100800050601, 100800050501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800060401, 100800060402, 100800060401, 100800060403, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800060405, 100800060406, 100800060405, 100800050501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800060404, 100800060406, 100800060404, 100800060403, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800060403, 100800060404, 100800060403, 100800060405, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800070101, 100800070102, 100800070101, 100800070104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800070102, 100800070108, 100800070102, 100800070101, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800070103, 100800070108, 100800070103, 100800070104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902030103, 100902030105, 100902030103, 100902030102, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902040101, 100902040103, 100902040101, 100902040102, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902040102, 100902040103, 100902040102, 100902040106, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201010104, 101201010105, 101201010104, 101201010301, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201010106, 101201010208, 101201010106, 101201010104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201010201, 101201010202, 101201010201, 101201010204, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201010204, 101201010208, 101201010204, 101201010301, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201010301, 101201010304, 101201010301, 101201010305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201050101, 101201050203, 101201050101, 101201050103, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201050103, 101201050104, 101201050103, 101201050108, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201050201, 101201050102, 101201050201, 101201050202, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201050302, 101201050303, 101201050302, 101201050304, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101201050303, 101201050304, 101201050303, 101201050302, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800030304, 101800030403, 101800030304, 101800030306, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800050501, 101800050504, 101800050501, 101800050502, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070205, 101800070802, 101800070205, 101800070301, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070301, 101800070302, 101800070301, 101800070303, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070302, 101800070303, 101800070302, 101800070301, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070401, 101800070404, 101800070401, 101800070403, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070402, 101800070404, 101800070402, 101800070403, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070404, 101800070405, 101800070404, 101800070406, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070405, 101800070603, 101800070405, 101800070406, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070503, 101800070605, 101800070503, 101800070504, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070602, 101800070603, 101800070602, 101800070604, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800070603, 101800070605, 101800070603, 101800070602, from_huc, to_huc, log); // 2017-08-29
										if (drop_huc_12 (101800070605, 101800070805, from_huc, to_huc, log)) // 2017-08-29
											valid = false;
										correct_huc_12 (101800071203, 101800080303, 101800071203, 101800070910, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800080103, 101800080305, 101800080103, 101800080104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800080303, 101800080305, 101800080303, 101800080304, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800020302, 100800020305, 100800020302, 100800020303, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800020306, 100800020307, 100800020306, 100800020305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800030103, 100800030107, 100800030103, 100800030104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800030104, 100800030105, 100800030104, 100800030108, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800030107, 100800030110, 100800030107, 100800030108, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800030207, 100800030208, 100800030207, 100800030211, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800040205, 100800040302, 100800040205, 100800040203, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800050302, 100800050303, 100800050302, 100800050305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800050601, 100800050501, 100800050601, 100800050602, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800050602, 100800050604, 100800050602, 100800050606, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080101, 100800080104, 100800080101, 100800080102, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080104, 100800080404, 100800080104, 100800080105, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080201, 100800080104, 100800080201, 100800080301, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080202, 100800080104, 100800080202, 100800080201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080204, 100800080106, 100800080204, 100800080201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080301, 100800080302, 100800080301, 100800080305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080305, 100800080306, 100800080305, 100800080501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080406, 100800080606, 100800080406, 100800080403, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080403, 100800080404, 100800080403, 100800080501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080501, 100800080503, 100800080501, 100800080701, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080601, 100800080606, 100800080601, 100800080603, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080602, 100800080606, 100800080602, 100800080603, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080603, 100800080606, 100800080603, 100800080604, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080604, 100800080605, 100800080604, 100800080607, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080606, 100800080704, 100800080606, 100800080607, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080701, 100800080702, 100800080701, 100800080705, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800080705, 100800080706, 100800080705, 100800071205, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800071101, 100800071104, 100800071101, 100800071104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800071104, 100800071105, 100800071104, 100800071201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800071201, 100800071202, 100800071201, 100800071203, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800071203, 100800071204, 100800071203, 100800071205, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800071205, 100800071206, 100800071205, 100800071208, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800071208, 100800100309, 100800071208, 100800100301, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100301, 100800100302, 100800100301, 100800100310, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100302, 100800100303, 100800100302, 100800100304, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100303, 100800100309, 100800100303, 100800100304, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100310, 100800100602, 100800100310, 100800100402, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100505, 100800100603, 100800100505, 100800100403, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100603, 100800100802, 100800100603, 100800100602, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100602, 100800100603, 100800100602, 100800100605, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100701, 100800100702, 100800100701, 100800100703, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100703, 100800100803, 100800100703, 100800100901, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800100901, 100800100904, 100800100901, 100800150302, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800140404, 100800140407, 100800140404, 100800140406, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800140405, 100800140407, 100800140405, 100800140406, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800150101, 100800150102, 100800150101, 100800150104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902010203, 100902010205, 100902010203, 100902010208, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902010208, 100902020201, 100902010208, 100902020101, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902010304, 100902010306, 100902010304, 100902010305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902010305, 100902010306, 100902010305, 100902020101, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902010306, 100902020201, 100902010306, 100902010305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020101, 100902020104, 100902020101, 100902020102, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020103, 100902020104, 100902020103, 100902020106, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020106, 100902020205, 100902020106, 100902020303, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020303, 100902020408, 100902020303, 100902020401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902030404, 100902030405, 100902030404, 100902030406, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902030406, 100902030407, 100902030406, 100902020101, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902030407, 100902020201, 100902030407, 100902030406, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902050305, 100902020804, 100902050305, 100902020703, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020401, 100902020403, 100902020401, 100902020501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020405, 100902020406, 100902020405, 100902020401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020501, 100902020502, 100902020501, 100902020601, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020502, 100902020507, 100902020502, 100902020503, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020503, 100902020507, 100902020503, 100902020501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020504, 100902020505, 100902020504, 100902020501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020601, 100902020604, 100902020601, 100902020606, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020606, 100902020804, 100902020606, 100902020703, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020805, 100902020901, 100902020805, 100902020806, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020902, 100902020903, 100902020902, 100902070101, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902020904, 100902060507, 100902020904, 100902020902, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902050104, 100902050111, 100902050104, 100902050104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902050105, 100902050106, 100902050105, 100902050104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902050201, 100902050203, 100902050201, 100902050204, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902050111, 100902050203, 100902050111, 100902050201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902050202, 100902050203, 100902050202, 100902050201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902050301, 100902050302, 100902050301, 100902050304, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902050305, 100902020804, 100902050305, 100902020703, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902060504, 100902060505, 100902060504, 100902060507, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902060507, 100902070103, 100902060507, 100902070101, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902070101, 100902070103, 100902070101, 100902070104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902070103, 100902070107, 100902070103, 100902070104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902070104, 100902070107, 100902070104, 100902070201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902070201, 100902070203, 100902070201, 100902070204, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902070204, 100902070206, 100902070204, 100902070207, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902070207, 100902070209, 100902070207, 100902070302, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100902070302, 100902070304, 100902070302, 100902070305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020106, 101800020503, 101800020106, 101800020107, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020107, 101800020503, 101800020107, 101800020201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020201, 101800020205, 101800020201, 101800020204, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020204, 101800020205, 101800020204, 101800020601, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020303, 101800020505, 101800020303, 101800020201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020505, 101800020903, 101800020505, 101800020504, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020601, 101800020603, 101800020601, 101800020604, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020602, 101800020603, 101800020602, 101800020601, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020603, 101800020906, 101800020603, 101800020604, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040203, 101800040206, 101800040203, 101800040204, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040204, 101800040206, 101800040204, 101800040205, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040206, 101800040407, 101800040206, 101800040301, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040301, 101800040303, 101800040301, 101800040305, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040401, 101800040402, 101800040401, 101800040501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040402, 101800040407, 101800040402, 101800040403, from_huc, to_huc, log); // 2017-08-29
										if (drop_huc_12 (101800040404, 101800040407, from_huc, to_huc, log)) // 2017-08-29
											valid = false;
										correct_huc_12 (101800040405, 101800040407, 101800040405, 101800040401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040406, 101800040501, 101800040406, 101800040401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800050401, 101800050402, 101800050401, 101800040401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040501, 101800040503, 101800040501, 101800040502, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040502, 101800040503, 101800040502, 101800040504, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800050505, 101800040501, 101800050505, 101800050504, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800050504, 101800050505, 101800050504, 101800050401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060202, 101800060203, 101800060202, 101800060207, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060205, 101800060206, 101800060205, 101800060202, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060206, 101800060402, 101800060206, 101800060207, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060401, 101800060402, 101800060401, 101800060403, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060402, 101800060406, 101800060402, 101800060401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060405, 101800060406, 101800060405, 101800060501, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060406, 101800060502, 101800060406, 101800060405, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060501, 101800060502, 101800060501, 101800060503, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060503, 101800060505, 101800060503, 101800060504, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060504, 101800060505, 101800060504, 101800060601, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060601, 101800060602, 101800060601, 101800060801, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060801, 101800060802, 101800060801, 101800060803, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800060803, 101800060804, 101800060803, 101800060805, from_huc, to_huc, log); // 2017-08-29
										if (drop_huc_12 (101800090501, 101800090504, from_huc, to_huc, log)) // 2017-08-29
											valid = false;
										if (drop_huc_12 (101800090504, 101800090704, from_huc, to_huc, log)) // 2017-08-29
											valid = false;
										correct_huc_12 (101800100204, 101800100407, 101800100204, 101800100205, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800100404, 101800100407, 101800100404, 101800100401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800100501, 101800100503, 101800100501, 101800100504, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800120103, 101800120105, 101800120103, 101800120106, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800120105, 101800120106, 101800120105, 101800120103, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150103, 101900150107, 101900150103, 101900150104, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150104, 101900150107, 101900150104, 101900150201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150106, 101900150107, 101900150106, 101900150201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150107, 101900150403, 101900150107, 101900150106, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150201, 101900150202, 101900150201, 101900150204, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101900150204, 101900150205, 101900150204, 101900150206, from_huc, to_huc, log); // 2017-08-29
										if (drop_huc_12 (101900150205, 101900150206, from_huc, to_huc, log)) // 2017-08-29
											valid = false;
										correct_huc_12 (100800020305, 100800020306, 100800020305, 100800020309, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800040203, 100800040204, 100800040203, 100800040302, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800040302, 100800040305, 100800040302, 100800040303, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (100800040303, 100800040304, 100800040303, 100800050101, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020103, 101800020104, 101800020103, 101800020101, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020101, 101800020107, 101800020101, 101800020201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020203, 101800020205, 101800020203, 101800020204, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020202, 101800020205, 101800020202, 101800020201, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020205, 101800020503, 101800020205, 101800020204, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800020505, 101800020504, 101800020505, 101800020508, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040101, 101800040104, 101800040101, 101800040103, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040103, 101800040106, 101800040103, 101800040105, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040104, 101800040106, 101800040104, 101800040103, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800040105, 101800040204, 101800040105, 101800040108, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800050106, 101800050304, 101800050106, 101800050202, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800050202, 101800050203, 101800050202, 101800050401, from_huc, to_huc, log); // 2017-08-29
										if (drop_huc_12 (101800050405, 101800050505, from_huc, to_huc, log)) // 2017-08-29
											valid = false;
										correct_huc_12 (101800050304, 101800050505, 101800050304, 101800050401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800080304, 101800080305, 101800080304, 101800080401, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800080305, 101800080702, 101800080305, 101800080304, from_huc, to_huc, log); // 2017-08-29
										correct_huc_12 (101800080904, 101800081103, 101800080904, 101800080905, from_huc, to_huc, log); // 2017-08-29

										correct_huc_12 (101201010203, 101201010208, 101201010203, 101201010204, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201010205, 101201010207, 101201010205, 101201010204, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201010207, 101201010208, 101201010207, 101201010204, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201010208, 101201010304, 101201010208, 101201010207, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201020202, 101201020205, 101201020202, 101201020203, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201020203, 101201030205, 101201020203, 101201030101, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201020205, 101201030301, 101201020205, 101201020203, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201030102, 101201030103, 101201030102, 101201030104, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201030104, 101201030205, 101201030104, 101201030105, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201030401, 101201030402, 101201030401, 101201030404, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201030404, 101201030405, 101201030404, 101201060101, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201030505, 101201060105, 101201030505, 101201030507, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201040202, 101201040206, 101201040202, 101201040203, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201040203, 101201040204, 101201040203, 101201040206, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201040305, 101201040307, 101201040305, 101201040306, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201040306, 101201040307, 101201040306, 101201040303, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201040307, 101201030505, 101201040307, 101201040206, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201040105, 101201040305, 101201040105, 101201040203, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201060101, 101201060105, 101201060101, 101201060106, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201060102, 101201060103, 101201060102, 101201060101, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201060106, 101201060502, 101201060106, 101201060201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201060201, 101201060202, 101201060201, 101201060203, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201060202, 101201060204, 101201060202, 101201060201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201060204, 101201060503, 101201060204, 101201060207, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201060202, 101201060204, 101201060202, 101201060201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201060204, 101201060503, 101201060204, 101201060207, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201070307, 101201070406, 101201070307, 101201070504, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201070403, 101201070406, 101201070403, 101201070404, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201070404, 101201070406, 101201070404, 101201070407, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201070407, 101201070408, 101201070407, 101201070301, from_huc, to_huc, log); // 2017-08-30
										// 101201070408 is missing from 
										// "H:\scratch\HUC12_USA\NHDPlusV21_NationalData_WBDSnapshot_Shapefile_08\NHDPlusNationalData\WBDSnapshot_National.dbf"
										// apparently a typo, possibly ID 34459 is transposed to 101201070308.
										correct_huc_12 (101201070504, 101201070505, 101201070504, 101201060201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201070505, 101201060502, 101201070505, 101201070504, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201080103, 101201080105, 101201080103, 101201080302, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201080104, 101201080105, 101201080104, 101201080103, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201080105, 101201080106, 101201080105, 101201080103, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201080302, 101201080307, 101201080302, 101201080306, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201080305, 101201080307, 101201080305, 101201080306, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201080306, 101201080307, 101201080306, 101201080309, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202010706, 101202010805, 101202010706, 101202010901, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202010901, 101202010903, 101202010901, 101202010904, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202010904, 101202010907, 101202010904, 101202020102, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202020103, 101202020105, 101202020103, 101202020104, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202020104, 101202020105, 101202020104, 101202020106, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202020108, 101202020110, 101202020108, 101202020109, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202020109, 101202020110, 101202020109, 101202020201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202020201, 101202020202, 101202020201, 101202020204, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202020203, 101202020204, 101202020203, 101202020201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202030401, 101202030402, 101202030401, 101202020201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202030402, 101202030403, 101202030402, 101202030404, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202030403, 101202030407, 101202030403, 101202030404, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202030404, 101202030405, 101202030404, 101202030401, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101202030407, 101202020503, 101202030407, 101202020201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101500020301, 101500020302, 101500020301, 101500020304, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101500020303, 101500020307, 101500020303, 101500020304, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101500020307, 101500020404, 101500020307, 101500020401, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201080309, 101201060506, 101201080309, 101201060204, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101500020401, 101500020402, 101500020401, 101500020507, from_huc, to_huc, log); // 2017-08-30
										if (drop_huc_12 (101500020402, 101500020404, from_huc, to_huc, log)) // 2017-08-30
											valid = false;
										correct_huc_12 (101800020701, 101800020703, 101800020701, 101800020702, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800020702, 101800020703, 101800020702, 101800020604, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800020903, 101800020906, 101800020903, 101800021001, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800020904, 101800020905, 101800020904, 101800020903, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800021002, 101800021003, 101800021002, 101800030101, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800021003, 101800021004, 101800021003, 101800021002, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800021004, 101800021005, 101800021004, 101800021002, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060301, 101800060306, 101800060301, 101800060302, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060302, 101800060306, 101800060302, 101800060304, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060303, 101800060306, 101800060303, 101800060304, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060604, 101800060605, 101800060604, 101800060601, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060505, 101800060606, 101800060505, 101800060504, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060606, 101800060802, 101800060606, 101800060801, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060704, 101800060802, 101800060704, 101800060703, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060802, 101800060804, 101800060802, 101800060801, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800061103, 101800061106, 101800061103, 101800061104, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800061104, 101800061105, 101800061104, 101800030306, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800070308, 101800070802, 101800070308, 101800070309, from_huc, to_huc, log); // 2017-08-30
										if (drop_huc_12 (101800070801, 101800070802, from_huc, to_huc, log)) // 2017-08-30
											valid = false;
										correct_huc_12 (101800070802, 101800070803, 101800070802, 101800070801, from_huc, to_huc, log); // 2017-08-30
										if (drop_huc_12 (101800070803, 101800070805, from_huc, to_huc, log)) // 2017-08-30
											valid = false;
										if (drop_huc_12 (101800070804, 101800070805, from_huc, to_huc, log)) // 2017-08-30
											valid = false;
										correct_huc_12 (101800070901, 101800070903, 101800070901, 101800070905, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800070903, 101800070905, 101800070903, 101800070904, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800070904, 101800070905, 101800070904, 101800070901, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800071004, 101800071005, 101800071004, 101800070905, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090201, 101800090206, 101800090201, 101800090202, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090206, 101800090501, 101800090206, 101800090202, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090305, 101800090501, 101800090305, 101800090303, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090603, 101800090605, 101800090603, 101800090606, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090701, 101800090702, 101800090701, 101800090705, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090702, 101800090706, 101800090702, 101800090701, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090703, 101800090706, 101800090703, 101800090701, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090706, 101800090810, 101800090706, 101800090605, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800100405, 101800100407, 101800100405, 101800100404, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800100402, 101800100407, 101800100402, 101800100403, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800100201, 101800100204, 101800100201, 101800100205, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800100203, 101800100204, 101800100203, 101800100201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800100202, 101800100204, 101800100202, 101800100201, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800110906, 101800090304, 101800110906, 101800110503, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101201040303, 101201040305, 101201040303, 101201040307, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060702, 101800060704, 101800060702, 101800060703, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800080403, 101800080704, 101800080403, 101800080405, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800080805, 101800081102, 101800080805, 101800090101, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800081101, 101800081103, 101800081101, 101800081102, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090301, 101800090302, 101800090301, 101800090303, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090302, 101800090304, 101800090302, 101800090303, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090303, 101800090304, 101800090303, 101800090401, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090304, 101800090305, 101800090304, 101800090303, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800060703, 101800060704, 101800060703, 101800060801, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090202, 101800090206, 101800090202, 101800090301, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090204, 101800090206, 101800090204, 101800090202, from_huc, to_huc, log); // 2017-08-30
										correct_huc_12 (101800090205, 101800090206, 101800090205, 101800090204, from_huc, to_huc, log); // 2017-08-30
										if (drop_huc_12 (101800090203, 101800090206, from_huc, to_huc, log)) // 2017-08-30
											valid = false;
										correct_huc_12 (101800090402, 101800090403, 101800090402, 101800090605, from_huc, to_huc, log); // 2017-08-30

										// Area 14
										correct_huc_12 (140401070208, 140401070702, 140401070208, 140401070401, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401070704, 140401071001, 140401070704, 140401070705, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401070403, 140401070704, 140401070403, 140401070801, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401070802, 140401071002, 140401070802, 140401070803, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401080104, 140401080208, 140401080104, 140401080106, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401030501, 140401030504, 140401030501, 140401030502, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401080301, 140401080303, 140401080301, 140401080302, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401060304, 140401060801, 140401060304, 140401060307, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401060103, 140401050104, 140401060103, 140401060102, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401090201, 140401090303, 140401090201, 140401090203, from_huc, to_huc, log); // 2017-08-25
										if (drop_huc_12 (140401040607, 1404010306021, from_huc, to_huc, log)) // 2017-08-24
											valid = false;
										if (drop_huc_12 (140401040605, 1404010406071, from_huc, to_huc, log)) // 2017-08-24
											valid = false;
										correct_huc_12 (140401040112, 140401040601, 140401040112, 140401040111, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401040110, 140401040111, 140401040110, 140401040401, from_huc, to_huc, log); // 2017-08-25
										correct_huc_12 (140401030309, 140401030602, 140401030309, 140401030308, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030203, 140401030305, 140401030203, 140401030101, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020606, 140401010705, 140401020606, 140401020603, from_huc, to_huc, log); // 2017-08-28
										if (drop_huc_12 (140401040607, 140401030602, from_huc, to_huc, log)) // 2017-08-28
											valid = false;
										correct_huc_12 (140401010602, 140401010703, 140401010602, 140401010605, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010801, 140401010802, 140401010801, 140401011001, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401040301, 140401040306, 140401040301, 140401040303, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070605, 140401071001, 140401070605, 140401070606, from_huc, to_huc, log); // 2017-08-28
										if (drop_huc_12 (140402000803, 140402001201, from_huc, to_huc, log)) // 2017-08-28
											valid = false;

										correct_huc_12 (140401030305, 140401030309, 140401030305, 140401030307, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030301, 140401030305, 140401030301, 140401030304, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030303, 140401030305, 140401030303, 140401030304, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030304, 140401030309, 140401030304, 140401030307, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030305, 140401030309, 140401030305, 140401030307, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011305, 140401030602, 140401011305, 140401011306, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011303, 140401011305, 140401011303, 140401011302, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080208, 140401080211, 140401080208, 140401080206, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080205, 140401080208, 140401080205, 140401080203, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080203, 140401080205, 140401080203, 140401080206, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060604, 140401060802, 140401060604, 140401060501, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060501, 140401060503, 140401060501, 140401060704, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060704, 140401060901, 140401060704, 140401060502, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060501, 140401060503, 140401060501, 140401060502, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060702, 140401060704, 140401060702, 140401060703, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070103, 140401070208, 140401070103, 140401070104, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080102, 140401080104, 140401080102, 140401080103, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070205, 140401070206, 140401070205, 140401070204, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070204, 140401070207, 140401070204, 140401070208, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070402, 140401070403, 140401070402, 140401070401, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070401, 140401070402, 140401070401, 140401070403, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401071002, 140401071005, 140401071002, 140401071004, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401071005, 140401071007, 140401071005, 140401071008, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401071009, 140401060403, 140401071009, 140401071010, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060105, 140401060403, 140401060105, 140401060102, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060101, 140401060105, 140401060101, 140401060102, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401050702, 140401050704, 140401050702, 140401050701, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401050701, 140401050704, 140401050701, 140401050703, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401050108, 140401050205, 140401050108, 140401050203, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020603, 140401020604, 140401020603, 140401020302, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020404, 140401020601, 140401020404, 140401020403, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020403, 140401020404, 140401020403, 140401020301, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020301, 140401020302, 140401020301, 140401020303, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020304, 140401020601, 140401020304, 140401020303, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010203, 140401010702, 140401010203, 140401010204, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010105, 140401010302, 140401010105, 140401010102, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010102, 140401010105, 140401010102, 140401010202, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010202, 140401010203, 140401010202, 140401010204, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020103, 140401020601, 140401020103, 140401020104, from_huc, to_huc, log); // 2017-08-28 
										correct_huc_12 (140401020102, 140401020103, 140401020102, 140401020301, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020602, 140401020604, 140401020602, 140401020603, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010601, 140401010702, 140401010601, 140401010602, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010603, 140401010703, 140401010603, 140401010604, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010604, 140401010703, 140401010604, 140401010605, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010402, 140401010403, 140401010402, 140401010401, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010401, 140401010402, 140401010401, 140401010403, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010403, 140401010703, 140401010403, 140401010404, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010703, 140401010705, 140401010703, 140401010801, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060209, 140401060302, 140401060209, 140401060208, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060208, 140401060209, 140401060208, 140401060211, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060504, 140401060801, 140401060504, 140401060505, from_huc, to_huc, log); // 2017-08-28

										correct_huc_12 (140401020303, 140401020304, 140401020303, 140401020305, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401020302, 140401020304, 140401020302, 140401020303, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010101, 140401010105, 140401010101, 140401010102, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010103, 140401010105, 140401010103, 140401010102, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401010104, 140401010105, 140401010104, 140401010102, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140402001304, 140402001005, 140402001304, 140402001305, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140402001303, 140402001304, 140402001303, 140402001302, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030110, 140500030701, 140500030110, 140500030201, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030107, 140500030109, 140500030107, 140500030106, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030303, 140500030702, 140500030303, 140500030202, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401090202, 140401090303, 140401090202, 140401090203, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140402000802, 140402001201, 140402000802, 140402000804, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140402000903, 140402001201, 140402000903, 140402000902, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140402000901, 140402000903, 140402000901, 140402000902, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140402000902, 140402000903, 140402000902, 140402000905, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030107, 140401030602, 140401030107, 140401030108, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140402000104, 140402000106, 140402000104, 140402000103, from_huc, to_huc, log); // 2017-08-28
										if (drop_huc_12 (140402000105, 140402000106, from_huc, to_huc, log)) // 2017-08-28
											valid = false;
										correct_huc_12 (140500040204, 140500040305, 140500040204, 140500040202, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040202, 140500040203, 140500040202, 140500040301, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040309, 140500030704, 140500040309, 140500040308, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030702, 140500030704, 140500030702, 140500030703, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030705, 140500030809, 140500030705, 140500030704, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030805, 140500030806, 140500030805, 140500030801, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030803, 140500030804, 140500030803, 140500030801, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030308, 140401030309, 140401030308, 140401030307, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030307, 140401030308, 140401030307, 140401030101, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030101, 140401030107, 140401030101, 140401030103, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030104, 140401030107, 140401030104, 140401030105, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401030102, 140401030107, 140401030102, 140401030101, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401071008, 140401071009, 140401071008, 140401070808, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060202, 140401060203, 140401060202, 140401060201, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060201, 140401060202, 140401060201, 140401060204, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060204, 140401060205, 140401060204, 140401060207, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060207, 140401060209, 140401060207, 140401060208, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060903, 140401060908, 140401060903, 140401060904, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060902, 140401060903, 140401060902, 140401060906, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060901, 140401060903, 140401060901, 140401060906, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070203, 140401070207, 140401070203, 140401070204, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401040109, 140401040112, 140401040109, 140401040110, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401040112, 140401040111, 140401040112, 140401040110, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011202, 140401011203, 140401011202, 140401011201, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011201, 140401011203, 140401011201, 140401011204, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011004, 140401011302, 140401011004, 140401011003, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011102, 140401011003, 140401011102, 140401011103, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011101, 140401011104, 140401011101, 140401011102, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070602, 140401070604, 140401070602, 140401070603, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070701, 140401070702, 140401070701, 140401070704, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080206, 140401080207, 140401080206, 140401080209, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080201, 140401080205, 140401080201, 140401080203, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080202, 140401080205, 140401080202, 140401080203, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080204, 140401080205, 140401080204, 140401080203, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401080210, 140401080211, 140401080210, 140401080209, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401070702, 140401070704, 140401070702, 140401070701, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401071008, 140401071009, 140401071008, 140401070808, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040308, 140500040309, 140500040308, 140500030203, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030802, 140500030809, 140500030802, 140500030801, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030801, 140500030809, 140500030801, 140500030901, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011003, 140401011004, 140401011003, 140401011002, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011001, 140401011002, 140401011001, 140401011005, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401011002, 140401011003, 140401011002, 140401011001, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140402000103, 140402000104, 140402000103, 140402000106, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030606, 140500030705, 140500030606, 140500030704, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030604, 140500030606, 140500030604, 140500030605, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030601, 140500030602, 140500030601, 140500030606, from_huc, to_huc, log); // 2017-08-28
										if (drop_huc_12 (140401030608, 140401030610, from_huc, to_huc, log)) // 2017-08-28
											valid = false;
										if (drop_huc_12 (140401030609, 140401030610, from_huc, to_huc, log)) // 2017-08-28
											valid = false;
										if (drop_huc_12 (140401030607, 140401030608, from_huc, to_huc, log)) // 2017-08-28
											valid = false;
										if (drop_huc_12 (140401030606, 140401030607, from_huc, to_huc, log)) // 2017-08-28
											valid = false;
										correct_huc_12 (140401030604, 140401030606, 140401030604, 140401030603, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060102, 140401060105, 140401060102, 140401060201, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060203, 140401060205, 140401060203, 140401060201, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401060205, 140401060209, 140401060205, 140401060204, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401050104, 140401050105, 140401050104, 140401050103, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401050105, 140401050107, 140401050105, 140401050106, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030804, 140500030809, 140500030804, 140500030805, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030404, 140500030705, 140500030404, 140500030403, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030705, 140500030809, 140500030705, 140500030704, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030704, 140500030705, 140500030704, 140500030801, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040102, 140500040104, 140500040102, 140500040103, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040303, 140500040305, 140500040303, 140500040302, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030403, 140500030404, 140500030403, 140500030408, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500030402, 140500030404, 140500030402, 140500030403, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040305, 140500040307, 140500040305, 140500040306, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040302, 140500040305, 140500040302, 140500040301, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040306, 140500040309, 140500040306, 140500040308, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140500040301, 140500040302, 140500040301, 140500040306, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401090103, 140401090109, 140401090103, 140401090104, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140401090101, 140401090104, 140401090101, 140401090102, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140200040505, 140100040506, 140200040505, 140200040506, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140200050102, 140200050103, 140200050102, 140200050105, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140801030611, 140801011802, 140801030611, 140801011904, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (140801020501, 140801010502, 140801020501, 140801020502, from_huc, to_huc, log); // 2017-08-28

										// Area 17

										if (drop_huc_12 (170200021610, 170200021708, from_huc, to_huc, log)) // (Canada) 2017-08-28
											valid = false;
										if (drop_huc_12 (170200021707, 170200021708, from_huc, to_huc, log)) // (Canada) 2017-08-28
											valid = false;
										if (drop_huc_12 (170200021703, 170200021708, from_huc, to_huc, log)) // (Canada) 2017-08-28
											valid = false;
										if (drop_huc_12 (170200021704, 170200021708, from_huc, to_huc, log)) // (Canada) 2017-08-28
											valid = false;
										if (drop_huc_12 (170200021705, 170200021708, from_huc, to_huc, log)) // (Canada) 2017-08-28
											valid = false;
										if (drop_huc_12 (170200021706, 170200021708, from_huc, to_huc, log)) // (Canada) 2017-08-28
											valid = false;
										if (drop_huc_12 (170200021708, 170200020601, from_huc, to_huc, log)) // (Canada) 2017-08-28
											valid = false;
										correct_huc_12 (170200010202, 170200010501, 170200010202, 170200010203, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200010103, 170200010202, 170200010103, 170200010105, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170102160301, 170102160305, 170102160301, 170102160302, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200060204, 170200060206, 170200060204, 170200060205, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200060205, 170200060206, 170200060205, 170200060211, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200060206, 170200060207, 170200060206, 170200060211, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200060102, 170200060103, 170200060102, 170200060108, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200020603, 170200020604, 170200020603, 170200020606, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200060202, 170200060207, 170200060202, 170200060211, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170401050301, 170401050302, 170401050301, 170401050303, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170401050307, 170401050308, 170401050307, 170401050309, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170401050308, 170401050306, 170401050308, 170401050307, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170401050303, 170401050306, 170401050303, 170401050307, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200010502, 170200010503, 170200010502, 170200010508, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200010302, 170200010304, 170200010302, 170200010303, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200010307, 170200010505, 170200010307, 170200010309, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170102160104, 170102160203, 170102160104, 170102160105, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170102040612, 170102040801, 170102040612, 170102040701, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170102040807, 170102130503, 170102040807, 170102040812, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170102040804, 170102040806, 170102040804, 170102040807, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200060103, 170200060207, 170200060103, 170200060104, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170200070302, 170200070304, 170200070302, 170200070303, from_huc, to_huc, log); // 2017-08-28
										if (drop_huc_12 (170200020104, 170200020303, from_huc, to_huc, log)) // (Canada) 2017-08-28
											valid = false;
										correct_huc_12 (171100050203, 171100050303, 171100050203, 171100050204, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (171100050202, 171100050203, 171100050202, 171100050205, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (171100050102, 171100050203, 171100050102, 171100050103, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (171100050405, 171100050603, 171100050405, 171100050408, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170102040704, 170102040707, 170102040704, 170102040706, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170102040705, 170102040707, 170102040705, 170102040706, from_huc, to_huc, log); // 2017-08-28
										correct_huc_12 (170102040805, 170102040806, 170102040805, 170102040807, from_huc, to_huc, log); // 2017-08-28
									}
			}
			else {
				valid = false;
				log += "Non-numeric, to : \"";
				log += to;
				log += "\".\n";
			}
	}
	else {
		log += "Non-numeric, from : \"";
		log += from;
		log += "\".\n";
		log += "\tTo: \"";
		log += to;
		log += "\".\n";
		valid = false;
	}

	return valid;
}

bool HUC_tree::convert_12_8
(dynamic_string &path_huc_fromto,
std::map <long, long> *from_to_8,
dynamic_string &log)

// 2017-08-17 Read HUC21 from-to file and write HUC8 from-to file

{
	FILE *file_csv;
	char data[2048];
	dynamic_string parse_data;
	std::vector <dynamic_string> tokens;
	long count_hucs;
	int from_column_index, to_column_index;
	long from_huc_8, to_huc_8;
	bool error = false;

	if (fopen_s (&file_csv, path_huc_fromto.get_text_ascii (), "r") == 0) {

		// OBJECTID,TNMID,MetaSourceID,SourceDataDesc,SourceOriginator,SourceFeatureID,LoadDate,GNIS_ID,AreaAcres,AreaSqKm,States,HUC12,Name,HUType,HUMod,ToHUC,NonContributingAcres,NonContributingSqKm,Shape_Length,Shape_Area
		// 1,{B1EF0C55-72ED-4FF6-A3BA-97A87C6A6C47},,,,,1/18/2013 7:07:56,0,"12,663.000000000000000",51.245388179999999,AL,031401030101,Pond Creek,S,NM,031401030102,0.000000000000000,0.000000000000000,0.396873074861680,0.004858583040857
		// 2,{F0D9874D-52BA-4FDC-A5E6-E259B627764D},,,,,1/18/2013 7:07:56,0,"37,030.000000000000000",149.855225800000000,AL,031401030102,Lightwood Knot Creek,S,NM,031401030103,0.000000000000000,0.000000000000000,0.845521565832901,0.014214008021309
		// 3,{2E0CB201-5672-45B5-8CA7-A60070122697},,,,,1/18/2013 7:07:56,0,"26,011.000000000000000",105.262875460000000,AL,031401030103,Poley Creek-Lightwood Knot Creek,S,NM,031401030302,0.000000000000000,0.000000000000000,0.663940716431087,0.009979460735394


		count_hucs = 0;
		if (fgets (data, 256, file_csv)) {
			while (!error
			&& fgets (data, 2048, file_csv)) {
				parse_data = data;
				parse_data.tokenize (",\r\n", &tokens, true, false);

				from_column_index = 11;
				to_column_index = 15;

				if (tokens.size () != 20) {
					if (tokens[1].left (38) == "{E76E59D4-FC03-4C34-A858-F3E60C7D6165}") {
						// linefeed after token [1]
						fgets (data, 2048, file_csv);
						// Subsequent line starts with a single quote, so skip it
						parse_data = &data[1];
						parse_data.tokenize (",\r\n", &tokens, true, false);
						from_column_index = 10;
						to_column_index = 14;
					}
					else {
						log += "ERROR, invalid # of tokens in source file.\n\"";
						log += data;
						log += "\"\n";
						error = true;
					}
				}

				if (!error) {
					if (convert_tokens (tokens[from_column_index], tokens[to_column_index], &from_huc_8, &to_huc_8, log)) {
						if ((from_huc_8 != 0)
						&& (from_huc_8 != to_huc_8)) {
							std::map <long, long>::iterator existing_pair;
							if ((existing_pair = from_to_8->find (from_huc_8)) != from_to_8->end ()) {
								if ((existing_pair->second == 0)
								&& (to_huc_8 != 0)) {
									// 9020203->0 skip in favor of 9020203->9020204
									existing_pair->second = to_huc_8;
									log.add_formatted ("Correction: %ld->0 changed to %ld->%ld.\n", from_huc_8, from_huc_8, to_huc_8);
								}
							}
							else
								from_to_8->insert (std::pair <long, long> (from_huc_8, to_huc_8));
						}
					}
				}
			}
		}

		fclose (file_csv);
		log.add_formatted ("HUC count: %ld\n", count_hucs);
	}
	else {
		error = true;
		log += "ERROR, can't open HUC file \"";
		log += path_huc_fromto;
		log += "\"\n";
	}
	return !error;
}

bool HUC_tree::read_source_8
	(dynamic_string &path_huc_fromto,
	dynamic_string &log)

{
	FILE *file_csv;
	char data [2048];
	dynamic_string parse_data;
	std::vector <dynamic_string> tokens;
	long count_hucs, from_huc, to_huc;
	bool error = false;

	if (fopen_s (&file_csv, path_huc_fromto.get_text_ascii (), "r") == 0) {

		// 1010002,1010008
		// 1010003,1010008

		count_hucs = 0;
		while (!error
		&& fgets (data, 2048, file_csv)) {
			parse_data = data;
			parse_data.tokenize (",\r\n", &tokens, true, false);

			if (tokens.size () == 2) {
				from_huc = atol (tokens[0].get_text_ascii ());
				to_huc = atol (tokens[1].get_text_ascii ());

				++count_hucs;
				unique_hucs.insert ((long long) from_huc);

				if (to_huc != 0)
					unique_hucs.insert ((long long) to_huc);

				if (from_huc == to_huc)
					log.add_formatted ("WARNING, HUC loops to itself at %lld.\n", from_huc);
				else
					from_to.insert (std::pair <long long, long long> ((long long) from_huc, (long long) to_huc));
			}
		}

		fclose (file_csv);
		log.add_formatted ("HUC count: %ld\n", count_hucs);
	}
	else {
		error = true;
		log += "ERROR, can't open HUC file \"";
		log += path_huc_fromto;
		log += "\"\n";
	}
	return !error;
}

void HUC_tree::highlight_loop_8
	(map_layer *layer_trail,
	map_layer *layer_hucs,
	interface_window *view,
	dynamic_string &log)

// Find loop in tree by duplication check, add it to view

{
	std::set <long long>::iterator huc;
	std::map <long long, HUC_network_link *>::iterator branch;
	std::vector <long long> upstream;
	std::vector <long long>::iterator source;
	std::set <long long> duplicate_check;
	bool loop_found = false;
	map_object_vector *vector;
	map_object *huc_polygon, *destination_polygon;
	HUC_network_link *link;
	long huc_8_id;

	for (huc = unique_hucs.begin ();
	!loop_found
	&& (huc != unique_hucs.end ());
	++huc) {
		if ((branch = all_ids.find (*huc)) != all_ids.end ()) {

			if (!branch->second->accumulate_upstream (&upstream, &duplicate_check, log)) {
				// A loop was found.  Add everything in upstream to layer_trail for inspection
				loop_found = true;
				view->update_progress_formatted (1, "Loop found at %lld", branch->first);
				for (source = upstream.begin (); source != upstream.end (); ++source) {

					if ((link = match_all_just_below (*source)) != NULL) {

						huc_8_id = (long) *source;
						if ((huc_polygon = layer_hucs->match_id (huc_8_id)) != NULL) {
							huc_8_id = (long) link->id;
							if ((destination_polygon = layer_hucs->match_id (huc_8_id)) != NULL) {
								// New vector from source to link

								vector = (map_object_vector *) layer_trail->create_new (MAP_OBJECT_VECTOR);
								vector->id = (long) *source;
								vector->name = huc_polygon->name;
								vector->attributes_text[0].format ("to %ld", destination_polygon->id);
								huc_polygon->centroid (&vector->latitude, &vector->longitude);
								destination_polygon->centroid (&vector->destination_latitude, &vector->destination_longitude);
								layer_trail->objects.push_back (vector);

							}
							else
								log.add_formatted ("ERROR, upstream huc %lld not found in layer_hucs.\n", link->id);
						}
						else
							log.add_formatted ("ERROR, upstream huc %lld not found in layer_hucs.\n", *source);
					}
					else
						log.add_formatted ("ERROR, HUC link not found for %lld.\n", *source);

				}
			}

			duplicate_check.clear ();
			upstream.clear ();

		}
		else
			log.add_formatted ("ERROR, branch not found in all_ids for %lld\n", *huc);
	}
}

void HUC_tree::build_tree_vectors
	(map_layer *vector_layer,
	const map_layer *huc_layer,
	interface_window *view,
	dynamic_string &log)

{
	std::set <long long>::iterator huc;
	std::map <long long, HUC_network_link *>::iterator branch;

	for (huc = unique_hucs.begin ();
	huc != unique_hucs.end ();
	++huc) {
		if ((branch = all_ids.find (*huc)) != all_ids.end ()) {

			branch->second->build_map_layer (vector_layer, huc_layer, log);

		}
		else
			log.add_formatted ("ERROR, branch not found in all_ids for %lld\n", *huc);
	}
}

void HUC_tree::highlight_loop_12
	(map_layer *layer_trail,
	map_layer *layer_hucs,
	interface_window *view,
	dynamic_string &log)

// Find loop in tree by duplication check, add it to view

{
	std::set <long long>::iterator huc;
	std::map <long long, HUC_network_link *>::iterator branch;
	std::vector <long long> upstream;
	std::vector <long long>::iterator source;
	std::set <long long> duplicate_check;
	bool loop_found = false;
	map_object_vector *vector;
	map_object *huc_polygon, *destination_polygon;
	HUC_network_link *link;
	dynamic_string huc_name;

	for (huc = unique_hucs.begin ();
	!loop_found
	&& (huc != unique_hucs.end ());
	++huc) {
		if ((branch = all_ids.find (*huc)) != all_ids.end ()) {

			if (!branch->second->accumulate_upstream (&upstream, &duplicate_check, log)) {
				// A loop was found.  Add everything in upstream to layer_trail for inspection
				loop_found = true;
				view->update_progress_formatted (1, "Loop found at %lld", branch->first);
				for (source = upstream.begin (); source != upstream.end (); ++source) {

					if ((link = match_all_just_below (*source)) != NULL) {

						huc_name.format ("%012lld", (*source));
						if ((huc_polygon = layer_hucs->match_name (huc_name)) != NULL) {
							huc_name.format ("%012lld", link->id);
							if ((destination_polygon = layer_hucs->match_name (huc_name)) != NULL) {
								// New vector from source to link

								vector = (map_object_vector *) layer_trail->create_new (MAP_OBJECT_VECTOR);
								vector->id = huc_polygon->id;
								vector->name = huc_polygon->name;
								vector->attributes_text[0] = "to ";
								vector->attributes_text [0] += destination_polygon->name;
								huc_polygon->centroid (&vector->latitude, &vector->longitude);
								destination_polygon->centroid (&vector->destination_latitude, &vector->destination_longitude);
								layer_trail->objects.push_back (vector);

							}
							else
								log.add_formatted ("ERROR, upstream huc %lld not found in layer_hucs.\n", link->id);
						}
						else
							log.add_formatted ("ERROR, upstream huc %lld not found in layer_hucs.\n", *source);
					}
					else
						log.add_formatted ("ERROR, HUC link not found for %lld.\n", *source);

				}
			}

			duplicate_check.clear ();
			upstream.clear ();

		}
		else
			log.add_formatted ("ERROR, branch not found in all_ids for %lld\n", *huc);
	}
}

