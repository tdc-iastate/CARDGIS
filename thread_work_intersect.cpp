#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include <stack>
#include <chrono>
#include "../util/dynamic_string.h"
#include "../util/device_coordinate.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../subbasin_effect/thread_manager.h"
#include "thread_work_intersect.h"
// #include "../clipper_ver6.2.1/cpp/clipper.hpp"
#include "../clipper_ver6.4.2/cpp/clipper.hpp"
#include "ClipperBuffer.h"


bool work_intersect
	(thread_work *job)

{
	ClipperBuffer clipper;
	ClipperLib::Path p;
	ClipperLib::Paths ssurgo_paths, clipped_bits;
	std::vector <dynamic_string> clipped_bit_ids, clipped_bit_names;
	std::vector <map_object *>::iterator clipped_polygon;

	clipper.tract_id_source_1 = ((thread_work_intersect *) job)->tract_id_source_1; // TRACT_ID_SOURCE_TEXT_ATTRIBUTE;
	clipper.tract_index_source_1 = ((thread_work_intersect *) job)->tract_index_source_1;
	clipper.tract_id_source_2 = ((thread_work_intersect *) job)->tract_id_source_2;
	clipper.tract_index_source_2 = ((thread_work_intersect *) job)->tract_index_source_2;

    job->start = std::chrono::system_clock::now();

	map_object *zipcode = ((thread_work_intersect *) job)->polygon;
	map_layer *layer_clipped = ((thread_work_intersect *) job)->layer_results;

	ssurgo_paths.clear ();
	clipper.map_object_to_Path (zipcode, &ssurgo_paths, job->log);
	clipper.intersect_overlapping (&ssurgo_paths, ((thread_work_intersect *) job)->layer_2,
	&clipped_bits, &clipped_bit_ids, &clipped_bit_names, NULL, job->log);
	clipper.Paths_to_map_layer (&clipped_bits, 1, 0, &clipped_bit_ids, &clipped_bit_names, layer_clipped, true, NULL, job->log);

	switch (((thread_work_intersect *) job)->tract_id_source_1) {
		case CLIPPED_ID_SOURCE_ID:
			// Paths_to_map_layer sets attributes_text [0] to clipped_bit_ids
			// which are set from soil's attribute_text [soil_attribute_index_MUKEY]
			for (clipped_polygon = layer_clipped->objects.begin ();
			clipped_polygon != layer_clipped->objects.end ();
			++clipped_polygon)
				(*clipped_polygon)->attributes_numeric[0] = (double) zipcode->id;
			break;
		case CLIPPED_ID_SOURCE_NAME:
			// Paths_to_map_layer sets attributes_text [0] to clipped_bit_ids
			// which are set from soil's attribute_text [soil_attribute_index_MUKEY]
			// set attributes_text [1] to zipcode id
			for (clipped_polygon = layer_clipped->objects.begin ();
			clipped_polygon != layer_clipped->objects.end ();
			++clipped_polygon)
				(*clipped_polygon)->attributes_text[1] = zipcode->name;
			break;
		case CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE:
			// Paths_to_map_layer sets attributes_text [0] to clipped_bit_ids
			// which are set from soil's attribute_text [soil_attribute_index_MUKEY]
			for (clipped_polygon = layer_clipped->objects.begin ();
			clipped_polygon != layer_clipped->objects.end ();
			++clipped_polygon)
				(*clipped_polygon)->attributes_text[1] = zipcode->attributes_text [((thread_work_intersect *) job)->tract_index_source_1];
			break;
		case CLIPPED_ID_SOURCE_ID_AND_NAME:
			// Paths_to_map_layer sets attributes_text [0] to clipped_bit_ids
			// which are set from soil's attribute_text [soil_attribute_index_MUKEY]
			for (clipped_polygon = layer_clipped->objects.begin();
			clipped_polygon != layer_clipped->objects.end();
			++clipped_polygon) {
				(*clipped_polygon)->attributes_numeric[0] = (double)zipcode->id;
				(*clipped_polygon)->attributes_text[0] = zipcode->name;
			}
			break;
	}


	job->status = THREAD_WORK_STATUS_COMPLETE;
    job->end = std::chrono::system_clock::now();

	return true;
}

void paths_to_complex_polygon
	(map_polygon_complex *outbuffer,
	const ClipperLib::Paths *solution,
	map_layer *layer_buffer,
	const bool endtype_closed)

// Add 1 or more paths from solution to existing map_polygon_complex outbuffer
// Called by work_create_buffer

{
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	map_polygon *outline;
	int node_index;

	for (paths_path = solution->begin ();
	paths_path != solution->end ();
	++paths_path) {
		outline = (map_polygon *) layer_buffer->create_new (MAP_OBJECT_POLYGON);
		outline->id = outbuffer->id;
		outline->node_count = (long) paths_path->size ();
		outline->nodes = new long[outline->node_count * 2];
		for (path_point = paths_path->begin (), node_index = 0;
		path_point != paths_path->end ();
		++path_point, ++node_index) {
			outline->nodes[node_index * 2] = (long) path_point->X;
			outline->nodes[node_index * 2 + 1] = (long) path_point->Y;
		}
		if (endtype_closed) {
			// 2022-01-25 For polygons created with etClosedPolygon, add first point again as last to close resulting polygons
			path_point = paths_path->begin ();
			outline->add_point ((long) path_point->Y, (long) path_point->X);
		}
		outbuffer->add_polygon (outline, layer_buffer);
		delete outline;
	}
}

void buffer_one_polygon
	(ClipperLib::Paths *solution,
	map_polygon *polygon,
	const double offset_radius_average,
	const bool endtype_closed)

{
	ClipperLib::ClipperOffset co = ClipperLib::ClipperOffset();
	ClipperLib::Paths all_p;
	ClipperLib::Path p;
	int node_index;

	p.clear ();
	p.reserve (polygon->node_count);
	all_p.clear ();
	for (node_index = 0;
	node_index < polygon->node_count;
	++node_index)
		p.push_back (ClipperLib::IntPoint ((ClipperLib::cInt) polygon->nodes[node_index * 2],
		(ClipperLib::cInt) polygon->nodes[node_index * 2 + 1]));

	all_p.push_back (p);

	co.Clear ();
	// 2022-01-25 etClosedPolygon works for inner polygons when etOpenRound fails
	if (endtype_closed)
		co.AddPaths (all_p, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etClosedPolygon);
	else
		co.AddPaths (all_p, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etOpenRound);

	solution->clear ();
	co.Execute (*solution, offset_radius_average);
}

bool work_create_buffer
	(thread_work *job)

// Create a buffer around polygon, a distance of offset_radius_average

// Multiple polygons will be created if polygon is complex

{
	ClipperLib::Paths solution;
	std::vector < ClipperLib::Path >::const_iterator paths_path;
	std::vector < ClipperLib::IntPoint >::const_iterator path_point;
	map_object *polygon = ((thread_work_create_buffer *) job)->polygon;
	map_polygon *outline;
	map_polygon_complex *outbuffer;
	int polygon_index, node_index;
	bool error = false, skip;

	if (polygon->type == MAP_OBJECT_POLYGON) {
		// all_p.clear ();
		// p.clear ();
		// p.reserve (((map_polygon *) polygon)->node_count);

		buffer_one_polygon (&solution, (map_polygon *) polygon, ((thread_work_create_buffer *) job)->offset_radius_average, ((thread_work_create_buffer *) job)->endtype_closed);
		/*
		for (node_index = 0;
		node_index < ((map_polygon *) polygon)->node_count;
		++node_index)
			p.push_back (ClipperLib::IntPoint ((ClipperLib::cInt) ((map_polygon *) polygon)->nodes[node_index * 2], (ClipperLib::cInt) ((map_polygon *) polygon)->nodes[node_index * 2 + 1]));

		all_p.push_back (p);

		co.Clear ();
		// 2022-01-25 etClosedPolygon works for inner polygons when etOpenRound fails
		if ( ((thread_work_create_buffer *) job)->endtype_closed)
			co.AddPaths (all_p, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etClosedPolygon);
		else
			co.AddPaths (all_p, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etOpenRound);
		solution.clear();
		co.Execute (solution,  ((thread_work_create_buffer *) job)->offset_radius_average);
		*/

		if (solution.size () == 1) {
			for (paths_path = solution.begin ();
			paths_path != solution.end ();
			++paths_path) {
				outline = (map_polygon *) ((thread_work_create_buffer *) job)->layer_buffer.create_new (MAP_OBJECT_POLYGON);
				outline->id = polygon->id;
				outline->node_count = (long) paths_path->size ();
				outline->nodes = new long[outline->node_count * 2];
				for (path_point = paths_path->begin (), node_index = 0;
				path_point != paths_path->end ();
				++path_point, ++node_index) {
					outline->nodes[node_index * 2] = (long) path_point->X;
					outline->nodes[node_index * 2 + 1] = (long) path_point->Y;
				}
				if (((thread_work_create_buffer *) job)->endtype_closed) {
					// 2022-01-25 For polygons created with etClosedPolygon, add first point again as last to close resulting polygons
					path_point = paths_path->begin ();
					outline->add_point ((long) path_point->Y, (long) path_point->X);
				}
				((thread_work_create_buffer *) job)->layer_buffer.objects.push_back (outline);
			}
		}
		else {
			// 2022-02-28 Buffer polygon will be complex, but source polygon is not
			outbuffer = (map_polygon_complex *)  ((thread_work_create_buffer *) job)->layer_buffer.create_new (MAP_OBJECT_POLYGON_COMPLEX);
			outbuffer->id = polygon->id;

			paths_to_complex_polygon (outbuffer, &solution, &((thread_work_create_buffer *) job)->layer_buffer, ((thread_work_create_buffer *) job)->endtype_closed);

			((thread_work_create_buffer *) job)->layer_buffer.objects.push_back (outbuffer);
		}

	}
	else {
		// Complex polygon

		// 2022-03-02 Outer polygon must be the first one so we know which ones to shrink instead of expand.
		// is polygon [0] always the outside one?
#ifdef _DEBUG
		{
			int node_count = ((map_polygon_complex *) polygon)->polygons[0].node_count;

			for (polygon_index = 1; polygon_index < ((map_polygon_complex *) polygon)->polygon_count; ++polygon_index) {
				if (((map_polygon_complex *) polygon)->polygons[polygon_index].node_count >= node_count) {
					job->log.add_formatted ("ERROR, complex polygon %ld, first polygon has fewer nodes than #%d (%d < %d).\n", polygon->id, polygon_index + 1,
					node_count, ((map_polygon_complex *) polygon)->polygons[polygon_index].node_count);
					error = true;
				}
			}
		}
#endif

		if (!error) {
			skip = false;
			outbuffer = (map_polygon_complex *)  ((thread_work_create_buffer *) job)->layer_buffer.create_new (MAP_OBJECT_POLYGON_COMPLEX);
			outbuffer->id = polygon->id;

			buffer_one_polygon (&solution, &((map_polygon_complex *) polygon)->polygons[0], ((thread_work_create_buffer *) job)->offset_radius_average, ((thread_work_create_buffer *) job)->endtype_closed);

			if (solution.size () > 0)
				paths_to_complex_polygon (outbuffer, &solution, &((thread_work_create_buffer *) job)->layer_buffer, ((thread_work_create_buffer *) job)->endtype_closed);
			else 
				if (((thread_work_create_buffer *) job)->offset_radius_average < 0.0)
					// Don't buffer islands within complex polygons if the buffer size is negative, and outermost buffer disappears.
					skip = true;

			if (!skip) {
				// Remaining (internal) polygons buffered with negative radius
				for (polygon_index = 1; polygon_index < ((map_polygon_complex *) polygon)->polygon_count; ++polygon_index) {
					buffer_one_polygon (&solution, &((map_polygon_complex *) polygon)->polygons[polygon_index], -((thread_work_create_buffer *) job)->offset_radius_average, ((thread_work_create_buffer *) job)->endtype_closed);
					paths_to_complex_polygon (outbuffer, &solution, &((thread_work_create_buffer *) job)->layer_buffer, ((thread_work_create_buffer *) job)->endtype_closed);
				}
			}

			if (outbuffer->polygon_count == 0)
				// reducing buffer radius large relative to polygon
				delete outbuffer;
			else
				if (outbuffer->polygon_count > 1)
					((thread_work_create_buffer *) job)->layer_buffer.objects.push_back (outbuffer);
				else {
					// source polygon is complex, but solution was simple (internal islands too small for buffer radius)
					// Convert to non-complex polygon

					outline = (map_polygon *)  ((thread_work_create_buffer *) job)->layer_buffer.create_new (MAP_OBJECT_POLYGON);

					outline->copy_without_attributes (&outbuffer->polygons[0]);
					outline->copy_attributes_only (outbuffer, &((thread_work_create_buffer *) job)->layer_buffer);

					((thread_work_create_buffer *) job)->layer_buffer.objects.push_back (outline);
					delete outbuffer;
				}
			outbuffer = NULL;
		}

	}	 

	if (error) {
		job->status = THREAD_WORK_STATUS_ERROR;
		job->end = std::chrono::system_clock::now ();

		return false;
	}
	else {
		job->status = THREAD_WORK_STATUS_COMPLETE;
		job->end = std::chrono::system_clock::now ();

		return true;
	}
}

