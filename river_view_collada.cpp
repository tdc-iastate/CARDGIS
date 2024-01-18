
#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include <wchar.h>
#include <chrono>
#include <stack>
#include <ctime>
#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/device_coordinate.h"
#include "../util/message_slot.h"
#include "../util/xml_parser.h"
#include "../util/card_bitmap.h"
#include "../util/card_bitmap_png.h"
#include "../util/matrix.h"
#include "../map/color_set.h"
#include "../util/bounding_cube.h"
#include "../map/dynamic_map.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "../cardgis_console/router_NHD.h"
#include "../Collada/camera_time.h"
#include "../Collada/collada_animation.h"
#include "../Collada/collada_object.h"
#include "../Collada/collada_effect.h"
#include "../Collada/collada_material.h"
#include "../Collada/collada_camera.h"
#include "../Collada/collada_light.h"
#include "../Collada/xml_command_collada.h"
#include "../Collada/xml_configuration_collada.h"
#include "../Collada/collada_scene.h"
#include "../Collada/blender_python.h"

#include "timed_measurements.h"
#include "river_view_map.h"
#include "river_view_collada.h"

#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_field_set.h"

#include "../SamplingStations/WaterSample.h"
#include "elevation_matrix.h"
#include "triangulate.h"

/*
long timestamp_to_frame_number
	(const Timestamp &when,
	const Timestamp &since)

{
	long interval;

	when.get_interval_days (since, &interval);

	return interval * days_per_frame;
}
*/

long timestamp_to_frame_number
	(const std::chrono::system_clock::time_point &when,
	const std::chrono::system_clock::time_point &since,
	double days_per_frame)

{
	long interval_hours;

	interval_hours = std::chrono::duration_cast <std::chrono::hours> (when - since).count();

	return (long) (((double) interval_hours * days_per_frame) / 24.0);
}

void set_angles
	(const logical_coordinate &startpoint,
	const logical_coordinate &endpoint,
	double angle_radians [])

// Default Collada objects extend along x-axis towards +x

// Find angle to reach endpoint from startpoint

// see "R:\NHDPlusv2\drawings\NHD Collada 6712419.odg"

{
	logical_coordinate offset;

	// 2016-04-14 Neither sine nor cosine can get the actual 0-360 rotation angle because the sign of either the opposite or adjacent may be necessary
	offset.x = endpoint.x - startpoint.x;
	offset.y = endpoint.y - startpoint.y;
	offset.z = endpoint.z - startpoint.z;

	angle_radians [0] = 0.0;

	/*
	// Step 1. Rotation around Y to get angle in x-z plane
	angle_radians [1] = atan (offset.x / offset.z);
	find_quadrant (offset.x, offset.z, &angle_radians [1]);
	// Adjust theta to x axis
	angle_radians [1] -= PI / 2.0;

	// Step 2. Rotation around Z to get angle in x-y plane
	// Now computes theta at origin as y/x
	angle_radians [2] = atan (offset.y / offset.x);
	*/

	double xy_base;

	// Base of y-rotation triangle is length to target projected on xy plane
	xy_base = sqrt ((offset.x * offset.x) + (offset.y * offset.y));
	// sign of xy_base matches x sign it's substituted for
	if (offset.x < 0)
		xy_base *= -1;

	// Step 1. Rotation around Y to get angle in x-z plane
	// For purposes of quadrant, xy_base is y and -z is x
	if (xy_base != 0.0) {
		angle_radians [1] = atan (-offset.z / xy_base);
		find_quadrant (-offset.z, xy_base, &angle_radians [1]);
	}
	else
		angle_radians [1] = 0.0;

	// Step 2. Rotation around Z to get angle in x-y plane
	// Thumb rule means we're looking at x-y from below
	// For purposes of quadrant, x is y and y is x
	// If -x, then -y
	if (offset.x > 0.0) {
		angle_radians [2] = atan (offset.y / abs(offset.x));
		find_quadrant (offset.y, abs(offset.x), &angle_radians [2]);
	}
	else
		if (offset.x < 0.0) {
			angle_radians [2] = atan (-offset.y / -offset.x);
			find_quadrant (-offset.y, -offset.x, &angle_radians [2]);
		}
		else {
			// offset.x == 0
			if (offset.y > 0)
				angle_radians [2] = PI / 2.0;
			else
				angle_radians [2] = -PI / 2.0;
		}
}

void set_angle_from_matrix
	(const logical_coordinate &ViewRight,
	const logical_coordinate &ViewUp,
	const logical_coordinate &ViewOut,
	double *angle_radians)

{
	// Extract x-y-z rotations from the resulting matrix
	// http://gamedev.stackexchange.com/questions/50963/how-to-extract-euler-angles-from-transformation-matrix

	angle_radians [0] = atan2 (-ViewUp.z, ViewOut.z);
	double cosYangle = sqrt (ViewRight.x * ViewRight.x + ViewRight.y * ViewRight.y);
	angle_radians [1] = atan2 (ViewRight.z, cosYangle);

	double cosXangle = cos (angle_radians [0]);
	double sinXangle = sin (angle_radians [0]);
	angle_radians [2] = atan2 (cosXangle * ViewUp.x + sinXangle * ViewOut.x, cosXangle * ViewUp.y + sinXangle * ViewOut.y);
}

bool set_angles_camera
	(const logical_coordinate &startpoint,
	const logical_coordinate &endpoint,
	const logical_coordinate up_vector,
	double angle_radians [],
	const bool verbose,
	dynamic_string &log)

// Default camera looks along Z-axis towards -Z
// Top of camera is +Y

// Matrix multiplication necessary to transform view while retaining camera Up

// https://www.fastgraph.com/makegames/3drotation/
// https://www.fastgraph.com/makegames/3drotation/3dsrce.html

{
	logical_coordinate offset;
	matrix view (4, 4);

	// 2016-05-25 Matrix math works, but camera points the wrong way.  Switch start & endpoint
	offset.x = startpoint.x - endpoint.x;
	offset.y = startpoint.y - endpoint.y;
	offset.z = startpoint.z - endpoint.z;

	/*
	angle_radians [2] = 0.0;

	double xz_base;

	// Base of y-rotation triangle is length to target projected on xy plane
	xz_base = sqrt ((offset.x * offset.x) + (offset.z * offset.z));
	// sign of xz_base matches z sign it's substituted for
	if (offset.z < 0)
		xz_base *= -1;

	// Step 1. Rotation around X to get angle in z-y plane
	// For purposes of quadrant, y is x and -xz_base is y
	angle_radians [0] = atan (offset.y / -xz_base);

	find_quadrant (offset.y, -xz_base, &angle_radians [0]);

	// If rotation around x > 180, rotate around Z first
	// if (angle_radians [0] > PI)
	//	angle_radians [2] = PI;

	// Step 2. Rotation around Y to get angle in x-z plane
	// For purposes of quadrant, x is y and z is x
	// If -z, then -y
	if (offset.z > 0.0) {
		angle_radians [1] = atan (offset.x / offset.z);
		find_quadrant (offset.x, offset.z, &angle_radians [1]);
	}
	else {
		angle_radians [1] = atan (-offset.x / -offset.z);
		find_quadrant (-offset.x, -offset.z, &angle_radians [1]);
	}
	*/

	double ViewMagnitude = sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z); // V-hat
	logical_coordinate ViewOut = offset, ViewUp, ViewRight;

	ViewOut.x /= ViewMagnitude;
	ViewOut.y /= ViewMagnitude;
	ViewOut.z /= ViewMagnitude;

	// Row 2 is the projection of Up onto the X, Y, and Z coordinate axes
	// dot product of ViewOut vector and World Up vector gives projection of ViewOut on WorldUp
	double UpProjection = ViewOut.x * up_vector.x + ViewOut.y * up_vector.y + ViewOut.z * up_vector.z; // d

	ViewUp.x = up_vector.x - UpProjection * ViewOut.x;
	ViewUp.y = up_vector.y - UpProjection * ViewOut.y;
	ViewUp.z = up_vector.z - UpProjection * ViewOut.z;

	double UpMagnitude = ViewUp.x * ViewUp.x + ViewUp.y * ViewUp.y + ViewUp.z * ViewUp.z;
	// check for validity
	if (UpMagnitude < 0.0000001) {
		// Second try at making a View Up vector: Use Y axis default  (0,1,0)
		ViewUp.x = -ViewOut.y * ViewOut.x;
		ViewUp.y = 1.0 - ViewOut.x * ViewOut.x;
		ViewUp.z = -ViewOut.y * ViewOut.z;

		// Check for validity:
		UpMagnitude = ViewUp.x * ViewUp.x + ViewUp.y * ViewUp.y + ViewUp.z * ViewUp.z;

		if (UpMagnitude < 0.0000001) {
			 //Final try at making a View Up vector: Use Z axis default  (0,0,1)
			ViewUp.x = -ViewOut.z * ViewOut.x;
			ViewUp.y = -ViewOut.z * ViewOut.y;
			ViewUp.z = 1.0 - ViewOut.z * ViewOut.z;

			// Check for validity:
			UpMagnitude = ViewUp.x * ViewUp.x + ViewUp.y * ViewUp.y + ViewUp.z * ViewUp.z;

			if (UpMagnitude < .0000001) {
				log += "ERROR, all ViewUp vector attempts fail in set_angles_camera.\n";
				return false;
			}
		}
	}

	// Normalize Up vector
	UpMagnitude = sqrt (UpMagnitude); // Up-hat
	ViewUp.x /= UpMagnitude;
	ViewUp.y /= UpMagnitude;
	ViewUp.z /= UpMagnitude;

	// Row 1 cross product of up_unit_vector and view_unit_vector
	ViewRight.x = -ViewOut.y * ViewUp.z + ViewOut.z * ViewUp.y;
	ViewRight.y = -ViewOut.z * ViewUp.x + ViewOut.x * ViewUp.z;
	ViewRight.z = -ViewOut.x * ViewUp.y + ViewOut.y * ViewUp.x;
	
	view.clear ();
	view.set_by_number (1, 3, ViewOut.x);
	view.set_by_number (2, 3, ViewOut.y);
	view.set_by_number (3, 3, ViewOut.z);

	view.set_by_number (1, 2, ViewUp.x);
	view.set_by_number (2, 2, ViewUp.y);
	view.set_by_number (3, 2, ViewUp.z);

	view.set_by_number (1, 1, ViewRight.x);
	view.set_by_number (2, 1, ViewRight.y);
	view.set_by_number (3, 1, ViewRight.z);

	if (verbose)
		view.dump (log);

	view.eular_angle (angle_radians, verbose, log);
	// set_angle_from_matrix (ViewRight, ViewUp, ViewOut, angle_radians);

	/*
	{
		double radians [3];
		ViewRight.x = 0.92;
		ViewRight.y = 0.07;
		ViewRight.z = -0.37;

		ViewUp.x = -0.02;
		ViewUp.y = -0.98;
		ViewUp.z = -0.21;

		ViewOut.x = -0.39;
		ViewOut.y = 0.21;
		ViewOut.z = -0.89;

		view.clear ();
		view.set (1, 3, ViewOut.x);
		view.set (2, 3, ViewOut.y);
		view.set (3, 3, ViewOut.z);

		view.set (1, 2, ViewUp.x);
		view.set (2, 2, ViewUp.y);
		view.set (3, 2, ViewUp.z);

		view.set (1, 1, ViewRight.x);
		view.set (2, 1, ViewRight.y);
		view.set (3, 1, ViewRight.z);

		view.eular_angle (radians);
		set_angle_from_matrix (ViewRight, ViewUp, ViewOut, radians);
	}
	*/

	return true;
}

logical_coordinate spherical_up_vector
	(const double latitude,
	const double longitude)

// up_vector is straight out from earth's core 

// Same as geographic_to_spherical with radius=1

{
	logical_coordinate vector;
	double x1;

	x1 = cos (latitude * DYNAMIC_MAP::DEG2RAD);
	vector.y = sin (latitude * DYNAMIC_MAP::DEG2RAD);

	// 2016-05-23 Invert x else map is reversed when viewed from outside globe
	// 2016-06-26 added "* x1" to these two statements
	vector.z = sin (-longitude * DYNAMIC_MAP::DEG2RAD) * x1;
	vector.x = cos (-longitude * DYNAMIC_MAP::DEG2RAD) * x1;

	return vector;
}

logical_coordinate geographic_to_spherical
	(const logical_coordinate &position)

// Geographic_coordimates
// Convert lat, lon, elevation to x,y,z

// See "R:\NHDPlusv2\drawings\NHD Collada Geopositions.pdf"

{
	logical_coordinate collada_position;
	double x1;
	/*
	collada_position.x = cos (position.x * DYNAMIC_MAP::DEG2RAD) * position.z;
	collada_position.y = -1.0 * sin (position.x * DYNAMIC_MAP::DEG2RAD) * position.z;
	collada_position.z = sin (position.y * DYNAMIC_MAP::DEG2RAD) * position.z;
	*/
	// x_cosine = cos (position.x * DYNAMIC_MAP::DEG2RAD);
	x1 = cos (position.y * DYNAMIC_MAP::DEG2RAD) * position.z;
	collada_position.y = sin (position.y * DYNAMIC_MAP::DEG2RAD) * position.z;

	// 2016-05-23 Invert x else map is reversed when viewed from outside globe
	collada_position.z = sin (-position.x * DYNAMIC_MAP::DEG2RAD) * x1;
	collada_position.x = cos (-position.x * DYNAMIC_MAP::DEG2RAD) * x1;

	return collada_position;
}


int river_view_collada::stream_size
	(const class map_object *segment)

// Higher streamlevel is a *smaller* stream

{
	int stream_width = 10;
	switch ((int) segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL]) {
		case 1:
			stream_width *= 7;
			break;
		case 2:
			stream_width *= 6;
			break;
		case 3:
			stream_width *= 5;
			break;
		case 4:
			stream_width *= 4;
			break;
		case 5:
			stream_width *= 3;
			break;
		case 6:
			stream_width *= 2;
	}

	return stream_width;
}

bool river_view_collada::write_collada_xml
	(const dynamic_string &path_xml,
	interface_window *view,
	dynamic_string &log)

// pan around room, wind up looking at tree

{
	dynamic_string label;
	long last_frame_number;
	bool error = false;

	last_frame_number = 800;

//	create_static_objects ();

	xml_configuration_collada file_xml;
	dynamic_string text, filename_xml = path_xml;
	FILE *xml_file;

	// filename_xml.add_formatted ("Collada.dae");

	log += filename_xml;
	log += "\n";

	// 2014-01-07 Amesbury.DAE
	// Can remove all references to camera
	// cannot remove library_visual_scenes
	// Can remove all references to light
	// Can remove extra technique profile Google Earth, MAX3d
	// Can remove "<library_controllers/>"
	// Can remove "<scene>"
	view->update_progress ("Building XML");

	/*
	// from collada_export.dae
	scene.camera.place.x = 2182602.25;
	scene.camera.place.y = 5985473.5;
	scene.camera.place.z = 4478362.0;

	scene.camera.rotation.x = 0;
	scene.camera.rotation.y = -144.364;
	scene.camera.rotation.z = -20.0;
	*/

	if (file_xml.build ("wxMidi", &scene, view, log)) {
		if (file_xml.declaration.get_length() > 0) {
			text = file_xml.declaration;
			text += "\n";
		}
		view->update_progress ("Writing XML");
		// file_xml.root->write_xml (text, &file_xml, 0, true);
		if ((xml_file = fopen (filename_xml.get_text_ascii (), "w")) != NULL) {
			// fprintf (xml_file, "%s", text.get_text_ascii ());
			file_xml.root->write_xml (xml_file, &file_xml, 0, true);
			fclose (xml_file);
		}
		else {
			error = true;
			log += "ERROR, can't open DAE file \"";
			log += filename_xml;
			log += "\"\n";
		}

	}
	else
		error = true;


	return !error;
}

river_view_collada::river_view_collada ()
	: river_view_map ()

{
	camera_type = COLLADA_CAMERA_FIXED;

	min_elevation_meters = DBL_MAX;
	max_elevation_meters = -DBL_MAX;
}

void river_view_collada::build_stream_materials ()

// Gridfloat after streams so that stream materials can have ids from 1..9

{
	int ftype_index;
	collada_material *material;

	COLORREF nhd_colors [8] = {
		RGB (255, 0, 255), // NHD_FTYPE_ARTIFICIALPATH
		RGB (0, 0, 255), // NHD_FTYPE_STREAMRIVER
		RGB (0, 255, 0), // NHD_FTYPE_COASTLINE
		RGB (112, 146, 190), // NHD_FTYPE_CONNECTOR
		RGB (127, 127, 127), // NHD_FTYPE_CANALDITCH
		RGB (0, 255, 255), // NHD_FTYPE_PIPELINE
		RGB (128, 64, 0), // NHD_FTYPE_7
		RGB (255, 128, 0) // NHD_FTYPE_8
	};
	for (ftype_index = 0; ftype_index < NHD_FTYPE_COUNT; ++ftype_index)
		material = scene.create_material (nhd_colors [ftype_index], 1.0, NHD_FTYPE_name (ftype_index + 1).get_text_ascii ());

}

bool river_view_collada::build_stream
	(map_layer *layer,
	std::set <long> *trail,
	std::map <long, stream_measurements> *time_changes,
	// class bounding_cube *stream_area,
	class elevation_matrix *elevations,
	interface_window *view,
	dynamic_string &log)

// Create collada objects for each stream in trail

{
	// std::vector <map_object *>::const_iterator segment;
	bool error = false;
	map_object *segment;
	int node_index, last_node_index;
	collada_object *stream_segment;
	collada_geometry *stream_shape;
	logical_coordinate segment_start, segment_end, endpoint, offset;
	long object_count = 0, count_segment_nodes = 0, count_missing_elevation = 0, count_segments = 0;
	collada_animation animation;
	collada_animation_segment change;
	std::set <long>::iterator trail_stop;
	std::map <long, stream_measurements>::const_iterator stream_data = time_changes->end ();
	std::map <long, timed_measurements>::const_iterator node_data;
	std::map <std::chrono::system_clock::time_point, double>::const_iterator stream_reading;
	double stream_width;
	// double hypotenuse_xy, hypotenuse_xz;
	double elevation_min, elevation_max, elevation_step;

	stream_shape = new collada_geometry;
	stream_shape->name = "streamshape";
	// stream_shape->build_normals = false;
	stream_shape->id = scene.unique_id_geometry();

	// 1m x 1m x 1m, scaled later
	// Base point should be the center of the 'x=0' face of the
	// stream_shape->make_box (0.0, -0.5, -0.5, 1.0, 0.5, 0.5);
	stream_shape->make_unit_cylinder ();

	scene.geometries.insert(std::pair <long, collada_geometry *>(stream_shape->id, stream_shape));

	// scene.create_lights (0, 0);

	view->update_progress ("Path Layer", 0);

	for (trail_stop = trail->begin ();
	trail_stop != trail->end ();
	++trail_stop) {
		segment = layer->match_id (*trail_stop);
		if (segment->type == MAP_OBJECT_POLYGON) {
			// segment->check_extent (stream_area);

			if (object_count % 100 == 0) {
				view->update_progress_formatted (1, "Count %ld / %ld", ++count_segments, layer->objects.size ());
				view->update_progress_formatted (2, "Path %ld", segment->id);
			}

			if ((elevation_min = segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_MINELEVM]) != -99.98) {
				elevation_max = segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_MAXELEVM];
				elevation_step = (elevation_max - elevation_min) / (double) ((map_polygon *) segment)->node_count;

				// Set min & max elevation so that camera knows range
				if ((elevation_min + EARTH_RADIUS_KM * 1000.0) < min_elevation_meters)
					min_elevation_meters = elevation_min + EARTH_RADIUS_KM * 1000.0;

				if ((elevation_max + EARTH_RADIUS_KM * 1000.0) > max_elevation_meters)
					max_elevation_meters = elevation_max + EARTH_RADIUS_KM * 1000.0;

				if ((stream_data == time_changes->end ())
				|| (stream_data->first != segment->id))
					stream_data = time_changes->find (segment->id);

				if (simplify) {
					// First and Last nodes only
					// Node index not added to object name
					last_node_index = ((map_polygon *) segment)->node_count - 1;

					segment_start.x = (double) ((map_polygon *) segment)->nodes [0] / 1.0e6;
					segment_start.y = (double) ((map_polygon *) segment)->nodes [1] / 1.0e6;
					segment_start.z = elevation_max + EARTH_RADIUS_KM * 1000.0;

					segment_end.x = (double) ((map_polygon *) segment)->nodes [last_node_index * 2] / 1.0e6;
					segment_end.y = (double) ((map_polygon *) segment)->nodes [last_node_index * 2 + 1] / 1.0e6;
					segment_end.z = elevation_min + EARTH_RADIUS_KM * 1000.0;

					if (elevations)
						elevations->add (segment_end); // use lowest point arbitrarily

					stream_segment = new collada_object;
					stream_segment->id = ++count_segment_nodes; // scene.unique_id();
					stream_segment->name.format ("S%ld", segment->id);
					stream_segment->geometry_id = stream_shape->id;
					stream_segment->material_id = (long) segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE];

					stream_segment->base_point = geographic_to_spherical (segment_start);
					endpoint = geographic_to_spherical (segment_end);

					stream_width = (double) stream_size (segment);

					stream_segment->scale [0] = distance_3d (stream_segment->base_point, endpoint);
					stream_segment->scale [1] = stream_width;
					stream_segment->scale [2] = stream_width;

					set_angles (stream_segment->base_point, endpoint, stream_segment->angle_radians);

					// Animation
					if (stream_data != time_changes->end ()) {

						animation.clear ();
						animation.id = stream_segment->unique_id_animation ();
						animation.animation_type = COLLADA_ANIMATE_SCALE;
						animation.dimension_index = 1;

						if ((node_data = stream_data->second.node_readings.begin ()) != stream_data->second.node_readings.end ()) {
							for (stream_reading = node_data->second.amounts_ppm.begin ();
							stream_reading != node_data->second.amounts_ppm.end ();
							++stream_reading) {
								change.clear();
								change.frame_number = timestamp_to_frame_number (stream_reading->first, start_date, days_per_frame);
								change.scale.y = stream_reading->second;
								animation.segments.push_back(change);
							}
							stream_segment->animation.push_back(animation);

							animation.clear ();
							animation.id = stream_segment->unique_id_animation ();
							animation.animation_type = COLLADA_ANIMATE_SCALE;
							animation.dimension_index = 2;
							for (stream_reading = node_data->second.amounts_ppm.begin ();
							stream_reading != node_data->second.amounts_ppm.end ();
							++stream_reading) {
								change.clear();
								change.frame_number = timestamp_to_frame_number (stream_reading->first, start_date, days_per_frame);
								change.scale.z = stream_reading->second;
								animation.segments.push_back(change);
							}
							stream_segment->animation.push_back(animation);
						}
					}

					scene.objects.push_back (stream_segment);
				}
				else
					// Not simplified
					for (node_index = 0;
					node_index < ((map_polygon *) segment)->node_count - 1;
					++node_index) {
						segment_start.x = (double) ((map_polygon *) segment)->nodes [node_index * 2] / 1.0e6;
						segment_start.y = (double) ((map_polygon *) segment)->nodes [node_index * 2 + 1] / 1.0e6;
						segment_start.z = elevation_max - (elevation_step * (double) node_index) + EARTH_RADIUS_KM * 1000.0;

						segment_end.x = (double) ((map_polygon *) segment)->nodes [(node_index + 1) * 2] / 1.0e6;
						segment_end.y = (double) ((map_polygon *) segment)->nodes [(node_index + 1) * 2 + 1] / 1.0e6;
						segment_end.z = elevation_max - (elevation_step * (double) (node_index + 1)) + EARTH_RADIUS_KM * 1000.0;

						if (elevations)
							elevations->add (segment_end); // use lowest point arbitrarily

						stream_segment = new collada_object;
						stream_segment->id = ++count_segment_nodes; // scene.unique_id();
						stream_segment->name.format ("S%ld%03d", segment->id, node_index + 1);
						stream_segment->geometry_id = stream_shape->id;
						stream_segment->material_id = (long) segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE];

						// Start point in meters
						stream_segment->base_point = geographic_to_spherical (segment_start);

						// End point in meters
						endpoint = geographic_to_spherical (segment_end);

						// scale in meters using lat/long
						// Hypotenuse distance should be pythagorean because distance_meters doesn't take into account z and messes up trigenometry below
						// stream_segment->scale [0] = distance_meters (segment_start.x, segment_start.y, segment_end.x, segment_end.y);
						// hypotenuse_xy = distance_2d_xy (stream_segment->base_point, endpoint);
						// hypotenuse_xz = distance_2d_xz (stream_segment->base_point, endpoint);

						stream_width = (double) stream_size (segment);

						stream_segment->scale [0] = distance_3d (stream_segment->base_point, endpoint);
						stream_segment->scale [1] = stream_width;
						stream_segment->scale [2] = stream_width;

						set_angles (stream_segment->base_point, endpoint, stream_segment->angle_radians);

						// Animation
						if (stream_data != time_changes->end ()) {
							animation.clear ();
							animation.id = stream_segment->unique_id_animation();

							animation.clear ();
							animation.id = stream_segment->unique_id_animation ();
							animation.animation_type = COLLADA_ANIMATE_SCALE;
							animation.dimension_index = 1;

							if ((node_data = stream_data->second.node_readings.find ((long) node_index)) != stream_data->second.node_readings.end ()) {
								for (stream_reading = node_data->second.amounts_ppm.begin ();
								stream_reading != node_data->second.amounts_ppm.end ();
								++stream_reading) {
									change.clear();
									change.frame_number = timestamp_to_frame_number (stream_reading->first, start_date, days_per_frame);
									change.scale.y = stream_reading->second;
									animation.segments.push_back(change);
								}
								stream_segment->animation.push_back(animation);

								animation.clear ();
								animation.id = stream_segment->unique_id_animation ();
								animation.animation_type = COLLADA_ANIMATE_SCALE;
								animation.dimension_index = 2;
								for (stream_reading = node_data->second.amounts_ppm.begin ();
								stream_reading != node_data->second.amounts_ppm.end ();
								++stream_reading) {
									change.clear();
									change.frame_number = timestamp_to_frame_number (stream_reading->first, start_date, days_per_frame);
									change.scale.z = stream_reading->second;
									animation.segments.push_back(change);
								}
								stream_segment->animation.push_back(animation);
							}
						}

						scene.objects.push_back (stream_segment);
					}
				
				++object_count;
			}
			else
				++count_missing_elevation;
		}
	}

	log.add_formatted ("River nodes written\t%ld\n", count_segment_nodes);
	log.add_formatted ("River segments missing elevation\t%ld\n", count_missing_elevation);

	return !error;
}

/*
bool create_python_text
	(const long object_id,
	const long station_id,
	const dynamic_string &text,
	logical_coordinate &position,
	logical_coordinate &rotation,
	dynamic_string &python_code)

{
	python_code.add_formatted ("# Station %ld\n", station_id);
    python_code += "\tbpy.ops.object.text_add(\n";
    // python_code += "location=origin,\n";
	python_code.add_formatted ("\tlocation=(%.6lf,%.6lf,%.6lf),\n", position.x, position.y, position.z);
    // python_code += "\trotation=(pi/2,0,0))\n";
	python_code.add_formatted ("\trotation=(%.3lf,%.3lf,%.3lf))\n", rotation.x, rotation.y, rotation.z);
    python_code += "\tob = bpy.context.object\n";
    python_code.add_formatted ("\tob.name = 'Text%ld'\n", object_id);
    python_code += "\ttcu = ob.data\n";
    python_code.add_formatted ("\ttcu.name = 'Text%ldData'\n", object_id);
 
    // # TextCurve attributes
    python_code += "\ttcu.body = \"";
	python_code += text;
	python_code += "\"\n";
    python_code += "\ttcu.font = bpy.data.fonts[0]\n";
    python_code += "\ttcu.offset_x = -9\n";
    python_code += "\ttcu.offset_y = -0.25\n";
    python_code += "\ttcu.shear = 0.0\n"; // 0.5
    python_code += "\ttcu.size = 200\n"; // 3
    python_code += "\ttcu.space_character = 1\n"; // 2
    python_code += "\ttcu.space_word = 4\n";
 
    // # Inherited Curve attributes
    python_code += "\ttcu.extrude = 0.2\n";
    python_code += "\ttcu.fill_mode=\"FRONT\"\n";
    python_code += "\ttcu.use_fill_deform = True\n";
    python_code += "\ttcu.fill_mode=\"FRONT\"\n";

	return true;
}
*/

bool river_view_collada::build_points
	(std::set <map_object *> *stations,
	const char *node_name_prefix,
	const double base_size,
	class elevation_matrix *elevations,
	const bool use_names,
	const bool text_labels,
	const BYTE animation_type,
	interface_window *view,
	dynamic_string &log)

{
	// std::vector <map_object *>::const_iterator segment;
	bool error = false;
	std::set <map_object *>::const_iterator station;
	collada_object *beacon;
	collada_geometry *facility_shape;
	collada_material *material = NULL;
	logical_coordinate segment_start, segment_end, endpoint, offset, sky_point, text_position, text_rotation;
	long object_count = 0, unique_id, animation_id;
	collada_animation animation;
	collada_animation_segment change;
	std::vector <long>::reverse_iterator trail_stop;
	timed_measurements *time_set;
	std::map <std::chrono::system_clock::time_point, double>::const_iterator time;
	double lowest_value, highest_value, range;

	facility_shape = new collada_geometry;
	facility_shape->name = "Shape-";
	facility_shape->name += node_name_prefix;
	facility_shape->id = scene.unique_id_geometry();
	// facility_shape->make_box (0, -0.5, -0.5, 1.0, 0.5, 0.5); // 1m x 1m x 1m, scaled later
	facility_shape->make_unit_cylinder (); // length 1m, radius 0.5m
	scene.geometries.insert(std::pair <long, collada_geometry *>(facility_shape->id, facility_shape));

	if ((animation_type & COLLADA_ANIMATE_COLOR) == 0)
		material = scene.create_material (RGB (0, 255, 0));

	scene.create_lights (0, 0);

	unique_id = scene.unique_id ();

	view->update_progress ("Point Layer", 0);
	for (station = stations->begin ();
	station != stations->end ();
	++station) {
			if (object_count % 100 == 0) {
				view->update_progress_formatted (1, "Count %ld / %ld", object_count, stations->size ());
				view->update_progress_formatted (2, "Point %ld", (*station)->id);
			}

			if (animation_type & COLLADA_ANIMATE_COLOR)
				// Each station must have its own effect/material
				material = scene.create_material (RGB (0, 255, 0));

			beacon = new collada_object;
			beacon->id = unique_id++;
			if (use_names)
				beacon->name = (*station)->name;
			else {
				beacon->name = node_name_prefix;
				beacon->name.add_formatted ("%ld", (*station)->id);
			}

			segment_start.x = (double) (*station)->longitude / 1.0e6;
			segment_start.y = (double) (*station)->latitude / 1.0e6;
			if (elevations
			&& !elevations->empty ())
				segment_start.z = elevations->nearest_elevation (segment_start) + 1.0;
			else
				segment_start.z = EARTH_RADIUS_KM * 1000.0;

			// stream_segment->base_point.x = cos (segment_start.x * DYNAMIC_MAP::DEG2RAD) * EARTH_RADIUS_KM * 1000.0;
			// stream_segment->base_point.y = -1.0 * sin (segment_start.x * DYNAMIC_MAP::DEG2RAD) * EARTH_RADIUS_KM * 1000.0;
			// stream_segment->base_point.z = sin (segment_start.y * DYNAMIC_MAP::DEG2RAD) * EARTH_RADIUS_KM * 1000.0;
			beacon->base_point = geographic_to_spherical (segment_start);

			// orient towards the sky
			sky_point = segment_start;
			sky_point.z += 10000.0;
			sky_point = geographic_to_spherical (sky_point);
			set_angles (beacon->base_point, sky_point, beacon->angle_radians);

			beacon->scale [0] = beacon->scale [1] = beacon->scale [2] = base_size;

			beacon->geometry_id = facility_shape->id;
			beacon->material_id = material->id;

			scene.objects.push_back (beacon);
			++object_count;

			if (text_labels) {
				text_position = segment_start;
				text_position.z += base_size * 2.0;
				text_position = geographic_to_spherical (text_position);
				text_rotation.x = beacon->angle_radians [0];
				text_rotation.y = beacon->angle_radians [1];
				text_rotation.z = beacon->angle_radians [2];
				create_python_text (beacon->id, // (*station)->id,
				(*station)->name, text_position, text_rotation, 200.0, false, false, python_code);
			}

			// if ((time_set = time_changes->find ((*station)->id)) != time_changes->end ()) {
			if ((time_set = (timed_measurements *) (*station)->attributes_void) != NULL) {

				animation_id = beacon->unique_id_animation ();
				// Range is unique for each station
				time_set->statistics (&lowest_value, &highest_value);
				range = highest_value - lowest_value;

				if (animation_type & COLLADA_ANIMATE_SCALE) {
					animation.clear ();
					animation.id = animation_id++;
					animation.animation_type = COLLADA_ANIMATE_SCALE;

					animation.dimension_index = 0;
					for (time = time_set->amounts_ppm.begin ();
					time != time_set->amounts_ppm.end ();
					++time) {
						change.clear();
						change.frame_number = timestamp_to_frame_number (time->first, start_date, days_per_frame);
						change.scale.x = base_size + ((time->second - lowest_value) / range) * amplification_station;
						animation.segments.push_back(change);
					}
					beacon->animation.push_back(animation);
				}
				if (animation_type & COLLADA_ANIMATE_COLOR) {
					// Red
					animation.clear ();
					animation.id = animation_id++;
					animation.animation_type = COLLADA_ANIMATE_COLOR;

					animation.dimension_index = 0;
					for (time = time_set->amounts_ppm.begin ();
					time != time_set->amounts_ppm.end ();
					++time) {
						change.clear();
						change.frame_number = timestamp_to_frame_number (time->first, start_date, days_per_frame);
						change.hue_value = (time->second - lowest_value) / range;
						animation.segments.push_back(change);
					}
					beacon->animation.push_back(animation);

					// Green
					animation.clear ();
					animation.id = animation_id++;
					animation.animation_type = COLLADA_ANIMATE_COLOR;

					animation.dimension_index = 1;
					for (time = time_set->amounts_ppm.begin ();
					time != time_set->amounts_ppm.end ();
					++time) {
						change.clear();
						change.frame_number = timestamp_to_frame_number (time->first, start_date, days_per_frame);
						// change.hue_value = (time->second - lowest_value) / range;
						change.hue_value = RGB (0, 0, 0);
						animation.segments.push_back(change);
					}
					beacon->animation.push_back(animation);

					// Blue
					animation.clear ();
					animation.id = animation_id++;
					animation.animation_type = COLLADA_ANIMATE_COLOR;

					animation.dimension_index = 2;
					for (time = time_set->amounts_ppm.begin ();
					time != time_set->amounts_ppm.end ();
					++time) {
						change.clear();
						change.frame_number = timestamp_to_frame_number (time->first, start_date, days_per_frame);
						change.hue_value = 1.0 - ((time->second - lowest_value) / range);
						animation.segments.push_back(change);
					}
					beacon->animation.push_back(animation);
				}

				/*
				animation.clear ();
				animation.id = beacon->unique_id_animation ();
				animation.animation_type = COLLADA_ANIMATE_SCALE;
				animation.dimension_index = 2;
				for (time = time_set->second.amounts_ppm.begin ();
				time != time_set->second.amounts_ppm.end ();
				++time) {
					change.clear();
					change.time = time->first;
					change.scale.z = time->second;
					animation.segments.push_back(change);
				}
				beacon->animation.push_back(animation);
				*/
			}
		}

	return !error;
}

void river_view_collada::simulate_flow
	(std::map <long, stream_measurements> *stream_time_changes,
	std::set <long> *trail_set,
	const long start_segment_id,
	const long destination_segment_id,
	const router_NHD *router,
	const map_layer *layer_rivers,
	dynamic_string &log)

{
	std::set <long> flow_selections;
	std::set <long>::iterator trail_stop;

	if (start_segment_id != 0)
		flow_selections.insert (start_segment_id);
	else
		// pick a few segments at random and send pulses downstream
		for (trail_stop = trail_set->begin ();
		trail_stop != trail_set->end ();
		++trail_stop) {
			if (rand () % 100 == 0)
				flow_selections.insert (*trail_stop);
		}

	for (trail_stop = flow_selections.begin ();
	trail_stop != flow_selections.end ();
	++trail_stop)
		simulate_flow_stream (stream_time_changes, router, layer_rivers, *trail_stop, destination_segment_id, log);
}

void river_view_collada::simulate_flow_stream
	(std::map <long, stream_measurements> *time_changes,
	const router_NHD *router,
	const map_layer *layer_rivers,
	const long start_comid,
	const long end_comid,
	dynamic_string &log)

{
	std::vector <long long> downstream_ids;
	std::vector <long long>::const_reverse_iterator trail_stop;
	std::vector <double> pulse_wave;
	std::vector <double>::const_iterator crest;
	stream_measurements stream_reading;
	timed_measurements node_reading;
	int node_index;
	std::chrono::system_clock::time_point time, base_time, pulse_time;
	double stream_width;
	map_object *segment;
	bool done;

	pulse_wave.push_back (1.0);
	pulse_wave.push_back (5.5);
	pulse_wave.push_back (12.0);
	pulse_wave.push_back (18.0);
	pulse_wave.push_back (20.0);
	pulse_wave.push_back (21.0);
	pulse_wave.push_back (22.0);
	pulse_wave.push_back (22.0);
	pulse_wave.push_back (20.75);
	pulse_wave.push_back (20.0);
	pulse_wave.push_back (19.0);
	pulse_wave.push_back (16.0);
	pulse_wave.push_back (14.0);
	pulse_wave.push_back (12.0);
	pulse_wave.push_back (8.0);
	pulse_wave.push_back (4.0);
	pulse_wave.push_back (1.0);

	router->rivers.accumulate_downstream_ordered (start_comid, &downstream_ids);
	base_time = start_date + std::chrono::seconds (seconds_per_frame * 12);
	pulse_time = base_time + std::chrono::seconds (seconds_per_frame);
	for (trail_stop = downstream_ids.rbegin (), done = false;
	!done
	&& (trail_stop != downstream_ids.rend ());
	++trail_stop) {
		// Traverse from start_comid downstream
		// initial value
		segment = layer_rivers->match_id (*trail_stop);
		stream_width = (double) stream_size (segment);

		ASSERT (segment->type == MAP_OBJECT_POLYGON);

		stream_reading.clear ();
		stream_reading.comid = *trail_stop;
		for (node_index = 0; node_index < ((map_polygon *) segment)->node_count; ++node_index) {

			node_reading.id = node_index;
			node_reading.amounts_ppm.clear ();
			node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (pulse_time - std::chrono::seconds (seconds_per_frame), stream_width));

			for (time = pulse_time, crest = pulse_wave.begin ();
			crest != pulse_wave.end ();
			time += std::chrono::seconds (seconds_per_frame), ++crest)
				// 10 +/- 6.0
				node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (time, (double) stream_width + *crest * amplification_stream));

			// end value
			node_reading.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (time, stream_width));

			stream_reading.node_readings.insert (std::pair <long, timed_measurements> (node_reading.id, node_reading));

			// advance 1 frame per node
			pulse_time += std::chrono::seconds (seconds_per_frame);
		}

		time_changes->insert (std::pair <long, stream_measurements> (stream_reading.comid, stream_reading));

		if (*trail_stop == end_comid)
			// Don't go all the way to mouth of river
			done = true;
	}

}

bool river_view_collada::read_sampling_data
	(const char *filename_db_readings,
	std::set <map_object *> *plotting_stations,
	const std::chrono::system_clock::time_point &first_date,
	const std::chrono::system_clock::time_point &last_date,
	long *count_samples,
	// std::map <long, timed_measurements> *time_changes,
	interface_window *view,
	dynamic_string &log)

// Read dissolved oxygen database created by samplingstations.exe

// This version reads into time_changes rather than map_objects

{
	bool error = false;
	odbc_database_access *db_odbc;
	odbc_database_credentials creds;
	std::set <map_object *>::iterator station;
	std::map <long, timed_measurements>::iterator station_readings;
	WaterSampleVector samples;
	WaterStationID rowset_ids;
	WaterSample rowset_water;
	dynamic_string filter;
	timed_measurements *stream_readings;
	// std::map <long, long> id_by_comid;
	// std::map <long, long>::iterator comid;
	std::map <long, map_object *> station_by_water_id;
	std::map <long, map_object *>::iterator station_finder;
	long record_count, count_unmatched = 0;
	std::chrono::system_clock::time_point when;

	db_odbc = new odbc_database_access;
	creds.set_database_filename (filename_db_readings);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (db_odbc->open (&creds, view, log)) {

		rowset_ids.initialize_field_set ();
		rowset_water.initialize_field_set ();

		view->update_progress ("Reading WaterSample IDs");
		record_count = 0;

		// Database has station & agency
		for (station = plotting_stations->begin ();
		station != plotting_stations->end ();
		++station) {
			// Get ID from [Station ID]
			filter.clear ();
			filter += db_odbc->identifier_quote_character_open ();
			filter += "Station";
			filter += db_odbc->identifier_quote_character_close ();
			filter += "='";
			filter += (*station)->name;
			filter += "' AND ";
			filter += db_odbc->identifier_quote_character_open ();
			filter += "Agency";
			filter += db_odbc->identifier_quote_character_close ();
			filter += "='";
			filter += (*station)->attributes_text [0];
			filter += "'";

			if (rowset_ids.field_set.open_read (db_odbc, log, filter)) {
				if (rowset_ids.field_set.move_first (log))
					// id_by_comid.insert (std::pair <long, long> (rowset_ids.ID, (*station)->id));
					station_by_water_id.insert (std::pair <long, map_object *> (rowset_ids.ID, *station));
				else
					++count_unmatched;
				rowset_ids.field_set.close ();
			}
			if ((++record_count % 1000) == 0) 
				view->update_progress_formatted  (1, "Station Count %ld unmatched %ld", record_count, count_unmatched);
		}

		view->update_progress ("WaterSample IDs read");
		log.add_formatted ("Plotting stations not found in WaterSamples ID table\t%ld\n", count_unmatched);

		record_count = 0;

		if (rowset_water.field_set.open_read (db_odbc, log)) {
			if (rowset_water.field_set.move_first (log)) {
				do {
					rowset_water.Time.set_time_point (&when);
					if ((when < first_date)
					|| (when > last_date)) {
					}
					else {
						// log.add_formatted ("%ld\t", water_station_id);
						// log += rowset_water.Time.write (5);
						// log.add_formatted ("\t%d\t%d\t%.6lf\n", (int) rowset_water.Depth, (int) rowset_water.Min, rowset_water.Amount);

						/*
						if ((comid = id_by_comid.find (rowset_water.ID)) != id_by_comid.end ()) {

							// frame_number = timestamp_to_frame_number (rowset_water.Time, *first_date);

							/*
							if (rowset_water.ID == stream_readings.id)
								stream_readings.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
							else {
								if ((station_readings = time_changes->find (comid->second)) != time_changes->end ())
									stream_readings.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
								else {
									stream_readings.clear ();
									stream_readings.id = comid->second;
									stream_readings.amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
									time_changes->insert (std::pair <long, timed_measurements> (stream_readings.id, stream_readings));
								}
							}
						}
							*/
						if ((station_finder = station_by_water_id.find (rowset_water.ID)) != station_by_water_id.end ()) {
							if ((stream_readings = (timed_measurements *) station_finder->second->attributes_void) != NULL)
								stream_readings->amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
							else {
								stream_readings = new timed_measurements;
								station_finder->second->attributes_void = stream_readings;
								stream_readings->id = station_finder->second->id;
								stream_readings->amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_water.Amount));
							}
							++count_samples;
						}
					}
					if ((++record_count % 1000) == 0) 
						view->update_progress_formatted  (1, "WaterSamples %ld", record_count);
				} while (rowset_water.field_set.move_next (log));
			}
			rowset_water.field_set.close ();
		}
		else
			error = true;

			/*
					water_station_id = rowset_ids.ID;

					stream_readings.clear ();
					stream_readings.id = (*station)->id;

					filter.format ("%cID%c=%ld", db_odbc->identifier_quote_character_open (), db_odbc->identifier_quote_character_close (), water_station_id);
					if (rowset_water.field_set.open (db_odbc, log, filter)) {
						if (rowset_water.field_set.move_first (log)) {
							do {
								if ((rowset_water.Time < *first_date)
								|| (rowset_water.Time > *last_date)) {
								}
								else {
									// log.add_formatted ("%ld\t", water_station_id);
									// log += rowset_water.Time.write (5);
									// log.add_formatted ("\t%d\t%d\t%.6lf\n", (int) rowset_water.Depth, (int) rowset_water.Min, rowset_water.Amount);


									frame_number = timestamp_to_frame_number (rowset_water.Time, *start_date);
									stream_readings.amounts_ppm.insert (std::pair <long, double> (frame_number, rowset_water.Amount));
								}
							} while (rowset_water.field_set.move_next (log));
						}
					
						rowset_water.field_set.close ();
					}

					time_changes->insert (std::pair <long, timed_measurements> (stream_readings.id, stream_readings));
				}

				rowset_ids.field_set.close ();
			}
			else
				error = true;

			// Find IDs in table [Station ID] from station/agency ?

			// read samples by ID from [Dissolved Oxygen]
		}
		*/
		log.add_formatted ("DB open.\n");
	}
	else {
		delete db_odbc;
		db_odbc = NULL;
		error = true;
	}

	return !error;
}

void river_view_collada::build_lat_lon_grid
	(dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// Lat & Long divisions

{			
	map_object *coordinate;
	long longitude, latitude;
	int object_id;
	std::set <map_object *> map_grid;
	long start_x, start_y;
	// std::map <long, timed_measurements> time_changes;

	// logical_extent is a double range of long values
	start_x = (long) map_watershed->logical_extent.x [0];
	start_x -= (start_x % 100000);

	start_y = (long) map_watershed->logical_extent.y [0];
	start_y -= (start_y % 100000);

	object_id = scene.unique_id ();

	// Latitude around earth at top of map area
	for (longitude = 0;
	longitude <= 359;
	longitude += 1) {
		coordinate = new map_object;
		coordinate->id = object_id++;
		coordinate->name.format ("N%03ldW%03ld", start_y / 100000, longitude);
		coordinate->latitude = start_y;
		coordinate->longitude = longitude * 1000000;
		map_grid.insert (coordinate);
	}
	build_points (&map_grid, "LAT", 30000.0, NULL, true, false, 0, view, log);

	map_grid.clear ();

	// meridian from pole to pole at west edge of map area
	for (latitude = 0;
	latitude <= 89;
	++latitude) {
		coordinate = new map_object;
		coordinate->id = object_id++;
		coordinate->name.format ("W%03ldN%03ld", abs (start_x / 1000000), latitude * 10);
		coordinate->latitude = latitude * 1000000;
		coordinate->longitude = start_x;
		map_grid.insert (coordinate);
	}
	build_points (&map_grid, "LON", 30000.0, NULL, true, false, 0, view, log);

	map_grid.clear ();

	for (longitude = 0;
	longitude <= 359;
	longitude += 1) {
		coordinate = new map_object;
		coordinate->id = object_id++;
		coordinate->name.format ("EQ%ld", longitude);
		coordinate->latitude = 0;
		coordinate->longitude = longitude * 1000000;
		map_grid.insert (coordinate);
	}
	build_points (&map_grid, "EQ", 30000.0, NULL, true, false, 0, view, log);

	map_grid.clear ();

	for (latitude = -89;
	latitude <= 89;
	++latitude) {
		coordinate = new map_object;
		coordinate->id = object_id++;
		coordinate->name.format ("MerP%ld", latitude);
		coordinate->latitude = latitude * 1000000;
		coordinate->longitude = 0;
		map_grid.insert (coordinate);

		coordinate = new map_object;
		coordinate->id = object_id++;
		coordinate->name.format ("MerA%ld", latitude);
		coordinate->latitude = latitude * 1000000;
		coordinate->longitude = 180000000;
		map_grid.insert (coordinate);
	}
	build_points (&map_grid, "Mer", 30000.0, NULL, true, false, 0, view, log);
}

bool river_view_collada::build_camera_animation
	(const std::chrono::system_clock::time_point &start_time,
	const logical_coordinate &position1,
	const logical_coordinate &target1,
	const logical_coordinate &up_vector,
	const std::chrono::system_clock::time_point &end_time,
	const logical_coordinate &position2,
	const logical_coordinate &target2,
	class collada_animation *animation_rotation,
	class collada_animation *animation_translation,
	const bool verbose,
	dynamic_string &log)

{
	double duration_frames;
	std::chrono::system_clock::time_point time;
	// std::chrono::system_clock::duration duration;

	long frame_number;
	logical_coordinate position, target, end_base, end_target, translate_position, translate_target, camera_target, camera_position;
	double camera_rotation [3];
	collada_animation_segment change, change_x, change_y, change_z;

	
	// duration = end_time - start_time;
	// duration_frames = std::chrono::duration_cast <double, std::seconds>.count ();
	scene.camera->base_point = geographic_to_spherical (position1);
	target = geographic_to_spherical (target1);
	end_base = geographic_to_spherical (position2);
	end_target = geographic_to_spherical (target2);

	duration_frames = (double) std::chrono::duration_cast <std::chrono::seconds> (end_time - start_time).count ();

	set_angles_camera (scene.camera->base_point, target, up_vector, scene.camera->angle_radians, verbose, log);

	translate_position.x = (end_base.x - scene.camera->base_point.x) / duration_frames;
	translate_position.y = (end_base.y - scene.camera->base_point.y) / duration_frames;
	translate_position.z = (end_base.z - scene.camera->base_point.z) / duration_frames;

	translate_target.x = (end_target.x - target.x) / duration_frames;
	translate_target.y = (end_target.y - target.y) / duration_frames;
	translate_target.z = (end_target.z - target.z) / duration_frames;

	/*
	rotation_step.x = (end_target.x - target.x) / duration;
	rotation_step.y = (end_target.y - target.y) / duration;
	rotation_step.z = (end_target.z - target.z) / duration;
	*/

	if (verbose) {
		log.add_formatted ("Time\t%ld\t%ld\n", start_time, end_time);
		// log.add_formatted ("Camera start\t%.4lf\t%.4lf\t%.4lf\n", scene.camera->base_point.x, scene.camera->base_point.y, scene.camera->base_point.z);
		// log.add_formatted ("Camera end\t%.4lf\t%.4lf\t%.4lf\n", end_base.x, end_base.y, end_base.z);
		// log.add_formatted ("Start target\t%.4lf\t%.4lf\t%.4lf\n", target.x, target.y, target.z);
		// log.add_formatted ("End target\t%.4lf\t%.4lf\t%.4lf\n", end_target.x, end_target.y, end_target.z);
		// log.add_formatted ("Rotation interval\t%.4lf\t%.4lf\t%.4lf\n", rotation_step.x, rotation_step.y, rotation_step.z);
		log.add_formatted ("Shift\t%.4lf\t%.4lf\t%.4lf", translate_position.x, translate_position.y, translate_position.z);
		log.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\n", translate_target.x, translate_target.y, translate_target.z);
	}

	camera_position = scene.camera->base_point;
	camera_target = target;
	for (time = start_time;
	time <= end_time;
	time += std::chrono::seconds (seconds_per_frame)) {
		set_angles_camera (camera_position, camera_target, up_vector, camera_rotation, verbose, log);

		if (verbose) {
			log.add_formatted ("%ld\t%.4lf\t%.4lf\t%.4lf", time, camera_position.x, camera_position.y, camera_position.z);
			log.add_formatted ("\t%.4lf\t%.4lf\t%.4lf", camera_target.x, camera_target.y, camera_target.z);
			log.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\n", camera_rotation [0] * RADIANS_TO_DEGREES, camera_rotation [1] * RADIANS_TO_DEGREES, camera_rotation [2] * RADIANS_TO_DEGREES);
		}

		frame_number = timestamp_to_frame_number (time, start_date, days_per_frame);

		change.clear();
		change.frame_number = frame_number;
		change.rotation_degrees.x = camera_rotation [0] * RADIANS_TO_DEGREES;
		animation_rotation [0].segments.push_back(change);

		change.clear();
		change.frame_number = frame_number;
		change.rotation_degrees.y = camera_rotation [1] * RADIANS_TO_DEGREES;
		animation_rotation [1].segments.push_back(change);

		change.clear();
		change.frame_number = frame_number;
		change.rotation_degrees.z = camera_rotation [2] * RADIANS_TO_DEGREES;
		animation_rotation [2].segments.push_back(change);

		change_x.clear();
		change_x.frame_number = frame_number;
		change_x.translation.x = camera_position.x;
		animation_translation [0].segments.push_back(change_x);

		change_y.clear();
		change_y.frame_number = frame_number;
		change_y.translation.y = camera_position.y;
		animation_translation [1].segments.push_back(change_y);

		change_z.clear();
		change_z.frame_number = frame_number;
		change_z.translation.z = camera_position.z;
		animation_translation [2].segments.push_back(change_z);

		camera_position += translate_position;
		camera_target += translate_target;
	}

	// log.add_formatted ("Camera final translation\t%.4lf\t%.4lf\t%.4lf\n", change_x.translation.x, change_y.translation.y, change_z.translation.z);

	return true;
}

bool river_view_collada::build_camera_source_outlet
	(const map_object *source,
	const map_object *outlet,
	bounding_cube *stream_area,
	const std::chrono::system_clock::time_point &end_time,
	const class router_NHD *router,
	const class map_layer *river_layer,
	interface_window *view,
	dynamic_string &log)

// start close to source
// end high above outlet looking at center

{
	bool error = false;
	logical_coordinate position, camera_target, start_base, start_target, end_position, end_target, up_vector;
	// long half_time;
	std::chrono::system_clock::time_point start_time;
	double elevation_meters, elevation_min, elevation_max;
	int node_index, polygon_index;
	device_coordinate object_position;
	collada_animation animation_rotation [3], animation_translation [3];
	std::vector <long long> downstream_ids;
	std::vector <long long>::iterator current_comid;
	int polygon_index_ahead, node_index_ahead;
	map_object *segment, *segment_ahead;

	{
		// Create list of ordered comids from source down to outlet

		std::vector <long long>::reverse_iterator comid_downstream;
		std::vector <long long> upstream_ids;
		bool outlet_found = false;

		// upstream_ids are in order upstream starting from outlet
		// Traverse reverse order, discard them once outlet is found
		router->rivers.accumulate_downstream_ordered (source->id, &upstream_ids);

		for (comid_downstream = upstream_ids.rbegin ();
		(comid_downstream != upstream_ids.rend ())
		&& !outlet_found;
		++comid_downstream) {
			downstream_ids.push_back (*comid_downstream);
			if (*comid_downstream == outlet->id)
				outlet_found = true;
		}
	}

	scene.camera = new collada_camera;
	view->update_progress ("Camera", 0);
	scene.camera->name = "Camera_01";

	// move towards center of stream_area, higher up, angled towards center
	animation_rotation [0].id = scene.camera->unique_id_animation();
	animation_rotation [1].id = animation_rotation [0].id + 1; // unique_id_animation won't work a second time until animation_x is added to scene.camera
	animation_rotation [2].id = animation_rotation [1].id + 1; // unique_id_animation won't work a second time until animation_x is added to scene.camera
	animation_translation [0].id = animation_rotation [2].id + 1;
	animation_translation [1].id = animation_translation [0].id + 1;
	animation_translation [2].id = animation_translation [1].id + 1;

	animation_rotation [0].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_rotation [1].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_rotation [2].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_translation [0].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_translation [1].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_translation [2].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_rotation [0].dimension_index = 0;
	animation_rotation [1].dimension_index = 1;
	animation_rotation [2].dimension_index = 2;
	animation_translation [0].dimension_index = 0;
	animation_translation [1].dimension_index = 1;
	animation_translation [2].dimension_index = 2;

	// half_time = end_time / 2;
	start_time = start_date;

	// Start close to source
	if ((elevation_min = source->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_MINELEVM]) != -99.98) {
		elevation_max = source->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_MAXELEVM];
		elevation_meters = (elevation_max - elevation_min) + elevation_min + EARTH_RADIUS_KM * 1000.0;
	}
	else
		elevation_meters = EARTH_RADIUS_KM * 1000.0;

	// Part 1: move from above source node 0 looking at last node
	// ((map_polygon *) source)->centroid (&object_position.y, &object_position.x);
	object_position.x = ((map_polygon *) source)->nodes [0];
	object_position.y = ((map_polygon *) source)->nodes [1];

	log.add_formatted ("Source\t%ld\t%.4lf\t%.4lf\n", source->id, (double) object_position.x / 1.0e6, (double) object_position.y / 1.0e6);

	// up_vector is straight out from earth's core 
	up_vector = spherical_up_vector (object_position.y, object_position.x);

	current_comid = downstream_ids.begin ();
	node_index = 0;
	polygon_index = 0;

	segment = river_layer->match_id (*current_comid);
	position.x = (double) ((map_polygon *) segment)->nodes [0] / 1.0e6;
	position.y = (double) ((map_polygon *) segment)->nodes [1] / 1.0e6;
	position.z = elevation_meters + 3000.0;

	if (nodes_ahead (segment, 0, 0, &segment_ahead, &polygon_index_ahead, &node_index_ahead, &current_comid, &downstream_ids, river_layer)) {
		start_target.x = (double) ((map_polygon *) segment_ahead)->nodes [node_index * 2] / 1.0e6;
		start_target.y = (double) ((map_polygon *) segment_ahead)->nodes [node_index * 2 + 1] / 1.0e6;
		start_target.z = elevation_meters;

		end_position.x = (double) ((map_polygon *) segment_ahead)->nodes [node_index * 2] / 1.0e6;
		end_position.y = (double) ((map_polygon *) segment_ahead)->nodes [node_index * 2 + 1] / 1.0e6;
		end_position.z = elevation_meters + 3000.0;

		segment = segment_ahead;
		polygon_index = polygon_index_ahead;
		node_index = node_index_ahead;

		while (nodes_ahead (segment, polygon_index, node_index, &segment_ahead, &polygon_index_ahead, &node_index_ahead, &current_comid, &downstream_ids, river_layer)) {

			end_target.x = (double) ((map_polygon *) segment_ahead)->nodes [node_index * 2] / 1.0e6;
			end_target.y = (double) ((map_polygon *) segment_ahead)->nodes [node_index * 2 + 1] / 1.0e6;
			end_target.z = elevation_meters;

			build_camera_animation (start_time, position, start_target, up_vector, end_time, end_position, end_target, animation_rotation, animation_translation, false, log);

			position = end_position;
			start_target = end_target;

			end_position.x = (double) ((map_polygon *) segment_ahead)->nodes [node_index * 2] / 1.0e6;
			end_position.y = (double) ((map_polygon *) segment_ahead)->nodes [node_index * 2 + 1] / 1.0e6;
			end_position.z = elevation_meters + 3000.0;

			segment = segment_ahead;
			polygon_index = polygon_index_ahead;
			node_index = node_index_ahead;
		}
	}

	/*
	start_target.x = position.x;
	start_target.y = position.y;
	start_target.z = elevation_meters;

	// end high above center of area
	end_position.x = stream_area->mid_x () / 1.0e6;
	end_position.y = stream_area->mid_y () / 1.0e6;
	end_position.z = stream_area->mid_z () + EARTH_RADIUS_KM * 1000.0 + 50000;

	// End target is center of stream_area
	end_target.x = stream_area->mid_x () / 1.0e6;
	end_target.y = stream_area->mid_y () / 1.0e6;
	end_target.z = stream_area->mid_z () + EARTH_RADIUS_KM * 1000.0;

	// build_camera_animation (12, position, start_target, half_time, end_position, end_target, animation_rotation, animation_translation, log);
	build_camera_animation (12, position, start_target, up_vector, end_time, end_position, end_target, animation_rotation, animation_translation, false, log);
	*/

	/*

	// Part 2: move from high above center to above outlet looking back towards center
	position = end_position;
	start_target = end_target;

	// end position above outlet
	outlet->centroid (&object_position.y, &object_position.x);
	log.add_formatted ("Outlet\t%ld\t%.4lf\t%.4lf\n", outlet->id, (double) object_position.x / 1.0e6, (double) object_position.y / 1.0e6);
	end_position.x = (double) object_position.x / 1.0e6;
	end_position.y = (double) object_position.y / 1.0e6;
	end_position.z = elevation_meters; //  + 20000.0;

	// End target is still center of area
	end_target = start_target;

	build_camera_animation (half_time + 1, position, start_target, end_time, end_position, end_target, animation_rotation, animation_translation, log);
	*/

	scene.camera->animation.push_back(animation_rotation [2]);
	scene.camera->animation.push_back(animation_rotation [1]);
	scene.camera->animation.push_back(animation_rotation [0]);
	scene.camera->animation.push_back(animation_translation [0]);
	scene.camera->animation.push_back(animation_translation [1]);
	scene.camera->animation.push_back(animation_translation [2]);

	return !error;
}

bool river_view_collada::build_camera_fixed
	(const map_object *outlet,
	logical_coordinate *target,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	logical_coordinate target_position, up_vector;

	scene.camera = new collada_camera;
	view->update_progress ("Camera", 0);
	scene.camera->name = "Camera_01";

	// start above outlet, looking at target

	if (outlet) {
		scene.camera->base_point.x = ((map_polygon *) outlet)->nodes [0] / 1.0e6;
		scene.camera->base_point.y = ((map_polygon *) outlet)->nodes [1] / 1.0e6;
		scene.camera->base_point.z = target->z + 25000.0;
		log.add_formatted ("Camera position\t%.6lf\t%.6lf\t%.6lf\n", scene.camera->base_point.x, scene.camera->base_point.y, scene.camera->base_point.z);
		scene.camera->base_point = geographic_to_spherical (scene.camera->base_point);

		log.add_formatted ("Camera target\t%.6lf\t%.6lf\t%.6lf\n", target->x, target->y, target->z);
		target_position = geographic_to_spherical (*target);

		// up_vector is straight out from earth's core 
		up_vector = spherical_up_vector (scene.camera->base_point.y, scene.camera->base_point.x);
		log.add_formatted ("Camera up-vector\t%.6lf\t%.6lf\t%.6lf\n", up_vector.x, up_vector.y, up_vector.z);

		// camera with no rotations looks along the Z axis towards -Z
		set_angles_camera (scene.camera->base_point, target_position, up_vector, scene.camera->angle_radians, false, log);
		log.add_formatted ("Camera rotation\t%.6lf\t%.6lf\t%.6lf\n", scene.camera->angle_radians [0], scene.camera->angle_radians [1], scene.camera->angle_radians [2]);
	}
	else
		error = true;

	return !error;
}

void river_view_collada::make_edge
	(const logical_coordinate &corner1,
	const logical_coordinate &corner2,
	const long id,
	const double width,
	const int shape_id,
	const int color_id)

{
	collada_object *box_edge;
	logical_coordinate endpoint;

	box_edge = new collada_object;
	box_edge->id = id;
	box_edge->name.format ("Bound%03ld", box_edge->id);
	box_edge->material_id = color_id;
	box_edge->geometry_id = shape_id;

	box_edge->base_point = geographic_to_spherical (corner1);

	endpoint = geographic_to_spherical (corner2);

	box_edge->scale [0] = distance_3d (box_edge->base_point, endpoint);
	box_edge->scale [1] = width;
	box_edge->scale [2] = width;
	set_angles (box_edge->base_point, endpoint, box_edge->angle_radians);

	scene.objects.push_back (box_edge);
}

bool river_view_collada::build_boundary_box
	(long * id,
	bounding_cube *area,
	collada_geometry *edge_shape,
	collada_material *color_box,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	double edge_width = 250.0;
	logical_coordinate corner1, corner2;

	corner1.x = area->x [0] / 1.0e6;
	corner1.y = area->y [0] / 1.0e6;
	corner1.z = area->z [0] + EARTH_RADIUS_KM * 1000.0;

	corner2 = corner1;
	corner2.x = area->x [1] / 1.0e6;
	make_edge (corner1, corner2, *id, edge_width, edge_shape->id, color_box->id);
	*id += 1;

	corner1 = corner2;
	corner2.y = area->y [1] / 1.0e6;
	make_edge (corner1, corner2, *id, edge_width, edge_shape->id, color_box->id);
	*id += 1;

	corner1 = corner2;
	corner2.x = area->x [0] / 1.0e6;
	make_edge (corner1, corner2, *id, edge_width, edge_shape->id, color_box->id);
	*id += 1;

	corner1 = corner2;
	corner2.y = area->y [0] / 1.0e6;
	make_edge (corner1, corner2, *id, edge_width, edge_shape->id, color_box->id);
	*id += 1;

	return !error;
}

bool river_view_collada::build_boundary_box
	(bounding_cube *area,
	interface_window *view,
	dynamic_string &log)

{
	collada_geometry *edge_shape = new collada_geometry;

	edge_shape->name = "boxshape";
	edge_shape->id = scene.unique_id_geometry();

	// 1m x 1m x 1m, scaled later
	// Base point should be the center of the 'x=0' face of the
	edge_shape->make_box (0.0, -0.5, -0.5, 1.0, 0.5, 0.5);
	scene.geometries.insert(std::pair <long, collada_geometry *>(edge_shape->id, edge_shape));

	collada_material *material = scene.create_material (RGB (255, 255, 255));

	long object_id = scene.unique_id();

	return build_boundary_box (&object_id, area, edge_shape, material, view, log);
}

bool river_view_collada::add_polygons
	(map_layer *layer,
	std::set <map_object *> *trail,
	const bool use_layer_color,
	const class elevation_matrix *elevations,
	interface_window *view,
	dynamic_string &log)

{
	// std::vector <map_object *>::const_iterator segment;
	bool error = false;
	std::set <map_object *>::const_iterator selected_polygon;
	std::vector <long> color_ids;
	int node_index, color_index, polygon_index;
	collada_object *stream_segment;
	collada_geometry *shape;
	collada_geometry_face face;
	collada_material *area_color;
	long object_count = 0, count_segment_nodes = 0, count_segments = 0, unique_id;
	// collada_animation animation;
	// collada_animation_segment change;
	std::map <long, double>::const_iterator time;
	logical_coordinate coordinate, xbase, ybase, node_zero;
	bounding_cube simple_area;
	double elevation_meters;

	// 1m x 10m x 10m, x is scaled later
	// Base point should be the center of the 'x=0' face of the
	// wall_shape->make_box (0.0, -5.0, -5.0, 1.0, 5.0, 5.0);

	unique_id = scene.unique_id();

	if (use_layer_color)
		area_color = scene.create_material (layer->color);
	else {
		for (color_index = 0; color_index < 5; ++color_index) {
			area_color = scene.create_material (RGB (128 + color_index * 24, 128 + color_index * 24, 128 + color_index * 24));
			color_ids.push_back (area_color->id);
		}
	}

	elevation_meters = EARTH_RADIUS_KM * 1000.0;

	if (simplify) {
		shape = new collada_geometry;
		shape->id = scene.unique_id_geometry();
		shape->name.format ("Shape%ld", shape->id);
		shape->type = COLLADA_OBJECT_3D;
		// shape->make_box (0.0, -0.5, -0.5, 1.0, 0.5, 0.5);
		shape->make_unit_cylinder ();
		scene.geometries.insert(std::pair <long, collada_geometry *>(shape->id, shape));
	}
	else
		shape = NULL;

	// scene.create_lights (0, 0);

	long object_id = scene.unique_id_geometry ();

	if (layer->type == MAP_OBJECT_POLYGON) {
		view->update_progress ("Polygon Layer", 0);

		for (selected_polygon = trail->begin ();
		selected_polygon != trail->end ();
		++selected_polygon) {

			if (object_count % 100 == 0) {
				view->update_progress_formatted (1, "Count %ld / %ld", ++count_segments, layer->objects.size ());
				view->update_progress_formatted (2, "Path %ld", (*selected_polygon)->id);
			}

			// find corners of shape
			// segment_extent.clear ();
			// ((map_polygon *) segment)->check_extent (&segment_extent);
			// segment_corner.x = segment_extent.x [0];
			// segment_corner.y = segment_extent.y [0];

			if (elevations
			&& (elevations->data.size () > 0)) {
				// One altitude value per polygon
				logical_coordinate center;
				long center_x, center_y;
				(*selected_polygon)->centroid (&center_y, &center_x);
				center.x = center_x / 1.0e6;
				center.y = center_y / 1.0e6;
				elevation_meters = elevations->nearest_elevation (center);
			}

			if (simplify) {
				simple_area.clear ();
				(*selected_polygon)->check_extent (&simple_area);
				simple_area.z [0] = simple_area.z [1] = elevation_meters;

				/*
				stream_segment = new collada_object;
				stream_segment->id = unique_id++;
				stream_segment->name.format ("P%ld", segment->id);
				stream_segment->material_id = area_color->id;
				stream_segment->geometry_id = shape->id;

				coordinate.x = simple_area.x [0] / 1.0e6;
				coordinate.y = simple_area.y [0] / 1.0e6;
				coordinate.z = simple_area.z [0];
				stream_segment->base_point = geographic_to_spherical (coordinate);

				coordinate.x = simple_area.x [1] / 1.0e6;
				coordinate.y = simple_area.y [0] / 1.0e6;
				coordinate.z = simple_area.z [0];
				coordinate = geographic_to_spherical (coordinate);
				stream_segment->scale [0] = distance_3d (stream_segment->base_point, coordinate);

				// Set angle along x axis from y=0&z=0
				set_angles (stream_segment->base_point, coordinate, stream_segment->angle_radians);

				coordinate.x = simple_area.x [0] / 1.0e6;
				coordinate.y = simple_area.y [1] / 1.0e6;
				coordinate.z = simple_area.z [0];
				coordinate = geographic_to_spherical (coordinate);
				stream_segment->scale [1] = distance_3d (stream_segment->base_point, coordinate);

				stream_segment->scale [2] = 10.0;
				scene.objects.push_back (stream_segment);
				*/
				build_boundary_box (&unique_id, &simple_area, shape, area_color, view, log);
			}
			else

				if ((*selected_polygon)->type == MAP_OBJECT_POLYGON) {
					shape = new collada_geometry;
					shape->name.format ("Shape%ld", (*selected_polygon)->id);
					shape->type = COLLADA_OBJECT_3D_FLAT; // COLLADA_OBJECT_2D;
					// shape->build_normals = false;
					shape->id = object_id++;
					face.clear ();

					Vector2dVector contour, result;
					Vector2d point;
					std::vector <Vector2d>::iterator new_point;
					for (node_index = 0;
					node_index < ((map_polygon *) (*selected_polygon))->node_count;
					++node_index) {
						point.Set_x ((double) ((map_polygon *) (*selected_polygon))->nodes [node_index * 2] / 1.0e6);
						point.Set_y ((double) ((map_polygon *) (*selected_polygon))->nodes [node_index * 2 + 1] / 1.0e6);
						contour.push_back (point);
					}

					// http://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml
					Triangulate::Process (contour, result);

					for (new_point = result.begin (), node_index = 0;
					new_point != result.end ();
					++new_point, ++node_index) {
						// consecutive sets of 3 Vector2ds constitute the triangles
						coordinate.x = (*new_point).GetX ();
						coordinate.y = (*new_point).GetY ();
						coordinate.z = elevation_meters;
						coordinate = geographic_to_spherical (coordinate);
						shape->positions.push_back (coordinate);
						face.position_indices.push_back ((long) node_index);
					}

					/*
					for (node_index = 0;
					node_index < ((map_polygon *) (*selected_polygon))->node_count;
					++node_index) {

						// compute each node's coordinates in meters from bottom-left corner of bounding box
						coordinate.x = (double) ((map_polygon *) (*selected_polygon))->nodes [node_index * 2] / 1.0e6;
						coordinate.y = (double) ((map_polygon *) (*selected_polygon))->nodes [node_index * 2 + 1] / 1.0e6;

						// Compute actual xyz coordinate of point, no translation
						coordinate.z = elevation_meters;
						coordinate = geographic_to_spherical (coordinate);
						shape->positions.push_back (coordinate);
						// 1 face matching positions
						face.position_indices.push_back ((long) node_index);
					}
					*/

					shape->faces.push_back (face);

					// shape->count_face_vertices = ((map_polygon *) segment)->node_count;
					scene.geometries.insert(std::pair <long, collada_geometry *>(shape->id, shape));

					stream_segment = new collada_object;
					stream_segment->id = unique_id++;
					stream_segment->name.format ("P%ld", (*selected_polygon)->id);
					stream_segment->geometry_id = shape->id;

					if (use_layer_color)
						stream_segment->material_id = area_color->id;
					else
						stream_segment->material_id = color_ids [(*selected_polygon)->id % color_ids.size ()];

					// compute world position of node [0]
					node_zero.x = node_zero.y = node_zero.z = 0.0;
					stream_segment->base_point = node_zero;

					stream_segment->scale [0] = 1.0;
					stream_segment->scale [1] = 1.0;
					stream_segment->scale [2] = 1.0;

					scene.objects.push_back (stream_segment);
					++object_count;
				}
				else
					if ((*selected_polygon)->type == MAP_OBJECT_POLYGON_COMPLEX) {
						for (polygon_index = 0; polygon_index < ((map_polygon_complex *) (*selected_polygon))->polygon_count; ++polygon_index) {

							shape = new collada_geometry;
							shape->name.format ("Shape%ld%03d", (*selected_polygon)->id, polygon_index + 1);
							// shape->type = COLLADA_OBJECT_2D;
							shape->type = COLLADA_OBJECT_3D_FLAT; // COLLADA_OBJECT_2D;
							// shape->build_normals = false;
							shape->id = object_id++;
							face.clear ();

							for (node_index = 0;
							node_index < ((map_polygon_complex *) (*selected_polygon))->polygons [polygon_index].node_count;
							++node_index) {

								// compute each node's coordinates in meters from bottom-left corner of bounding box
								coordinate.x = (double) ((map_polygon_complex *) (*selected_polygon))->polygons [polygon_index].nodes [node_index * 2] / 1.0e6;
								coordinate.y = (double) ((map_polygon_complex *) (*selected_polygon))->polygons [polygon_index].nodes [node_index * 2 + 1] / 1.0e6;

								// Compute actual xyz coordinate of point, no translation
								coordinate.z = elevation_meters;
								coordinate = geographic_to_spherical (coordinate);
								shape->positions.push_back (coordinate);
								// 1 face matching positions
								face.position_indices.push_back ((long) node_index);
							}
							shape->faces.push_back (face);
							scene.geometries.insert(std::pair <long, collada_geometry *>(shape->id, shape));

							stream_segment = new collada_object;
							stream_segment->id = unique_id++;
							stream_segment->name.format ("P%ld%03d", (*selected_polygon)->id, polygon_index + 1);
							stream_segment->geometry_id = shape->id;

							if (use_layer_color)
								stream_segment->material_id = area_color->id;
							else
								stream_segment->material_id = color_ids [(*selected_polygon)->id % color_ids.size ()];

							// compute world position of node [0]
							node_zero.x = node_zero.y = node_zero.z = 0.0;
							stream_segment->base_point = node_zero;

							stream_segment->scale [0] = 1.0;
							stream_segment->scale [1] = 1.0;
							stream_segment->scale [2] = 1.0;

							scene.objects.push_back (stream_segment);
							++object_count;
						}
					}
		}

		log += layer->name;
		log.add_formatted (" nodes written\t%ld\n", count_segment_nodes);
	}

	return !error;
}

bool extract_gridflow_nw_corner
	(const dynamic_string &prefix,
	logical_coordinate *nw_corner,
	dynamic_string &log)


// Get position of NW corner of gridflow from its filename

// floatn39w106_1.flt
// floatn42w094_1.flt
// floatn43w094_1.flt
// usgs_ned_1_n40w105_gridfloat.flt

{
	int index = 0;

	if ((index = prefix.match ("ned", index)) >= 0)
		// usgs_ned_1_n46w097_gridfloat
		// skip ned if it is in filename
		++index;
	else
		// floatn43w094_1
		index = 0;

	if ((index = prefix.match ("n", index)) >= 0) {
		if ((prefix.get_length () >= index + 6)
		&& isdigit (prefix.get_at_ascii (index + 1))
		&& isdigit (prefix.get_at_ascii (index + 2))
		&& (prefix.get_at_ascii (index + 3) == 'w')) {
			nw_corner->y = atof (prefix.mid (index + 1).get_text_ascii ());
			nw_corner->x = atof (prefix.mid (index + 4).get_text_ascii ()) * -1.0;
			return true;
		}
		else {
			log += "ERROR, can't parse gridfloat prefix \"";
			log += prefix.mid (index);
			log += "\".\n";
		}
	}
	else {
		log += "ERROR, can't derive NW corner from FLT filename.\n";
		log += prefix;
		log += "\n";
	}
	return false;
}

bool river_view_collada::read_gridfloat_heatmap
	(const dynamic_string &filename_flt,
	const dynamic_string &filename_image,
	const dynamic_string &filename_dae,
	const bounding_cube &boundary,
	elevation_matrix *elevations, // keep elevations to compute elevation of streets
	const bool create_image,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	collada_object *heatmap;
	collada_geometry *shape;
	collada_material *area_color;
	logical_coordinate coordinate, node_zero, nw_corner, normal, coordinate_2, coordinate_3, last_coordinate;
	card_bitmap_png png_image;
	std::vector <std::tuple <long, long, long>> bitmap_values;
	std::tuple <long, long, long> bitmap_value;

	// 1m x 10m x 10m, x is scaled later
	// Base point should be the center of the 'x=0' face of the
	// wall_shape->make_box (0.0, -5.0, -5.0, 1.0, 5.0, 5.0);

	filename_struct f;
	FILE *f_flt;
	dynamic_string filename;
	int count_columns, count_rows, count_read, index_column;
	double ll_latitude, ll_longitude, cellsize;
	float missing_value, *float_data;
	int combine_factor = 1;

	f.parse (&filename_flt);
	f.set_suffix (L"hdr");
	f.write_file_specification (&filename);

	// n43w094
	// nw_corner.x = -106.0; // -94.0;
	// nw_corner.y = 39.0; // 43.0;
	if (extract_gridflow_nw_corner (f.prefix, &nw_corner, log)) {

		if (read_gridfloat_header (filename, &count_columns, &count_rows, &ll_latitude, &ll_longitude, &cellsize, &missing_value, log)) {

			filename = filename_flt;

			if (create_image)
				area_color = scene.create_material_image ("Heatmap-image", 1.0, "3DEP");
			else
				area_color = scene.create_material (RGB (164, 82, 0), 1.0, "3DEP");

			// scene.create_lights (0, 0);

			if (fopen_s (&f_flt, filename.get_text_ascii (), "rb") == 0) {
				int index_row = 0, count_output_columns, count_output_rows, start_row, start_column, end_row, end_column;
				std::vector <logical_coordinate> uncombined_positions;

				float_data = new float [count_columns];

				start_row = (int) floor ((nw_corner.y - boundary.y [1]) * count_rows);
				end_row = (int) ceil ((nw_corner.y - boundary.y [0]) * count_rows);
				if (end_row >= count_rows)
					end_row = count_rows - 1;

				start_column = (int) floor ((boundary.x [0] - nw_corner.x) * count_columns);
				end_column = (int) ceil ((boundary.x [1] - nw_corner.x) * count_columns);
				if (end_column >= count_columns)
					end_column = count_columns - 1;

				count_output_columns = (end_column - start_column) + 1; // count_columns
				count_output_rows = (end_row - start_row) + 1; // count_rows

				shape = new collada_geometry;
				shape->id = scene.unique_id_geometry();
				shape->name.format ("3DEP%ld", shape->id);
				shape->type = COLLADA_OBJECT_3D_TRIANGLE_SURFACE;
				// shape->build_normals = false;

				while ((count_read = fread (float_data, sizeof (float), count_columns, f_flt)) == count_columns) {

					// make triangle strips instead ?
					if ((index_row >= start_row)
					&& (index_row <= end_row)) {
						for (index_column = start_column; index_column <= end_column; ++index_column) {

							coordinate.x = ll_longitude + (double) index_column * cellsize;
							coordinate.y = ll_latitude + (double) (count_rows - index_row) * cellsize;
							if (float_data [index_column] != missing_value)
								coordinate.z = EARTH_RADIUS_KM * 1000.0 + float_data [index_column];
							else
								coordinate.z = EARTH_RADIUS_KM * 1000.0;

							// keep unconverted coordinates to set elevations in other layers
							elevations->add (coordinate);

							if (coordinate.z < min_elevation_meters)
								min_elevation_meters = coordinate.z;
							if (coordinate.z > max_elevation_meters)
								max_elevation_meters = coordinate.z;

							if (create_image)
								bitmap_values.push_back (std::make_tuple (index_column - start_column, index_row - start_row, (long) coordinate.z));

							coordinate = geographic_to_spherical (coordinate);
							uncombined_positions.push_back (coordinate);
						}
					}
					++index_row;
				}

				if (create_image) {
					std::vector <std::tuple <long, long, long>>::const_iterator point;
					long x, y, gray;
					double range = max_elevation_meters - min_elevation_meters;
					dynamic_string filename_png;

					if (range != 0.0) {
						png_image.Create (count_output_columns, count_output_rows, 24);

						for (point = bitmap_values.begin ();
						point != bitmap_values.end ();
						++point) {
							std::tie (x, y, gray) = *point;
							gray = (int) ((((double) gray - min_elevation_meters) / range) * 255);
							png_image.SetPixel (x, y, RGB (255 - gray, gray, gray));
						}

						filename_png = "Heatmap-image.png";
						scene.image_filenames.push_back (filename_png);

						f.parse (&filename_dae);
						f.write_path (&filename_png);
						filename_png += "Heatmap-image.png";
						png_image.write_png (filename_png.get_text (), log);
					}
				}

				if (combine_factor > 1) {
					/*
					int combined_count;
					double *combined_cells;
					combined_count = 0;
					combined_cells = new double [count_columns / combine_factor];
					shape->positions.push_back (uncombined_positions);

					for (index_column = 0; index_column < count_columns; ++index_column) {

						if (float_data [index_column] != missing_value)
							// compute each node's coordinates in meters from bottom-left corner of bounding box
							combined_cells [index_column / 12] += float_data [index_column];
					}
					++combined_count;

						memset (combined_cells, 0, sizeof (double) * (count_columns / combine_factor));
						combined_count = 0;

					delete [] combined_cells;
					*/
				}
				else {
					std::vector <logical_coordinate>::const_iterator p;

					for (p = uncombined_positions.begin ();
					p != uncombined_positions.end ();
					++p)
						shape->positions.push_back (*p);
				}

				ASSERT (shape->positions.size () == count_output_rows * count_output_columns);

				// every 4 positions forms a square with 2 triangles, added to vertices
				for (index_row = 1; index_row < count_output_rows; ++index_row) {
					for (index_column = 1; index_column < count_output_columns; ++index_column) {
						// "The winding order of vertices produced is counter-clockwise and describes the front side of each polygon."
						// top-left corner vertex
						shape->vertices.push_back ((index_row - 1) * count_output_columns + (index_column - 1));
						// bottom-left corner vertex
						shape->vertices.push_back (index_row * count_output_columns + (index_column - 1));
						// top-right corner vertex
						shape->vertices.push_back ((index_row - 1) * count_output_columns + index_column);

						// top-right corner vertex
						shape->vertices.push_back ((index_row - 1) * count_output_columns + index_column);
						// bottom-left corner vertex
						shape->vertices.push_back (index_row * count_output_columns + (index_column - 1));
						// bottom-right corner vertex
						shape->vertices.push_back (index_row * count_output_columns + index_column);
					}
				}

				scene.geometries.insert(std::pair <long, collada_geometry *> (shape->id, shape));

				heatmap = new collada_object;
				heatmap->id = scene.unique_id ();
				heatmap->name.format ("P%ld", heatmap->id);
				heatmap->geometry_id = shape->id;

				heatmap->material_id = area_color->id;

				// compute world position of node [0]
				node_zero.x = node_zero.y = node_zero.z = 0.0;
				heatmap->base_point = node_zero;

				heatmap->scale [0] = 1.0;
				heatmap->scale [1] = 1.0;
				heatmap->scale [2] = 1.0;

				scene.objects.push_back (heatmap);

				log.add_formatted ("Rows\t%d\t%d.\n", start_row, end_row);
				log.add_formatted ("Output Rows\t%d.\n", count_output_rows);
				log.add_formatted ("Columns\t%d\t%d.\n", start_column, end_column);
				log.add_formatted ("Output Columns\t%d.\n", count_output_columns);
				log.add_formatted ("Position\t%.6lf\t%.6lf\n", nw_corner.y + ((double) start_row / (double) count_rows), nw_corner.x + ((double) start_column / (double) count_columns));
				delete [] float_data;
	
				fclose (f_flt);
			}
			else {
				error = true;
				log += "ERROR, can't open FLT file \"";
				log += filename_flt;
				log += "\n";
			}
		}
		else
			error = true;
	}
	else
		error = true;

	return !error;
}

bool river_view_collada::add_vectors
	(map_layer *layer,
	class bounding_cube *stream_area,
	const class elevation_matrix *elevations,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	std::vector <map_object *>::const_iterator segment;
	int node_index, last_node_index;
	collada_object *stream_segment;
	collada_geometry *pavement_shape;
	collada_material *color_pavement;
	logical_coordinate segment_start, segment_end, endpoint, offset;
	long object_count = 0, count_segment_nodes = 0, count_segments = 0;
	// collada_animation animation;
	// collada_animation_segment change;
	double street_width;

	pavement_shape = new collada_geometry;
	pavement_shape->name = "roadshape";
	// pavement_shape->build_normals = false;
	pavement_shape->id = scene.unique_id_geometry();

	// 1m x 1m x 1m, scaled later
	// Base point should be the center of the 'x=0' face of the
	pavement_shape->make_box (0.0, -0.5, -0.5, 1.0, 0.5, 0.5);

	scene.geometries.insert(std::pair <long, collada_geometry *> (pavement_shape->id, pavement_shape));

	color_pavement = scene.create_material (RGB (112, 146, 190));

	street_width = 10.0;

	view->update_progress ("Path Layer", 0);

	for (segment = layer->objects.begin ();
	segment != layer->objects.end ();
	++segment) {
		if ((*segment)->overlaps_logical (stream_area)) {
			if ((*segment)->type == MAP_OBJECT_POLYGON) {

				if (object_count % 100 == 0) {
					view->update_progress_formatted (1, "Count %ld / %ld", ++count_segments, layer->objects.size ());
					view->update_progress_formatted (2, "Path %ld", (*segment)->id);
				}

				if (simplify) {
					// One map_object for the entire road
					last_node_index = ((map_polygon *) *segment)->node_count - 1;

					segment_start.x = (double) ((map_polygon *) *segment)->nodes [0] / 1.0e6;
					segment_start.y = (double) ((map_polygon *) *segment)->nodes [1] / 1.0e6;
					if (elevations)
						segment_start.z = elevations->nearest_elevation (segment_start) + 1.0;
					else
						segment_start.z = EARTH_RADIUS_KM * 1000.0;

					segment_end.x = (double) ((map_polygon *) *segment)->nodes [last_node_index * 2] / 1.0e6;
					segment_end.y = (double) ((map_polygon *) *segment)->nodes [last_node_index * 2 + 1] / 1.0e6;
					if (elevations)
						segment_end.z = elevations->nearest_elevation (segment_end) + 1.0;
					else
						segment_end.z = EARTH_RADIUS_KM * 1000.0;

					stream_segment = new collada_object;
					stream_segment->id = ++count_segment_nodes; // scene.unique_id();
					stream_segment->name.format ("S%ld", (*segment)->id);
					stream_segment->geometry_id = pavement_shape->id;
					stream_segment->material_id = color_pavement->id;

					// Start/End points in meters
					stream_segment->base_point = geographic_to_spherical (segment_start);
					endpoint = geographic_to_spherical (segment_end);

					stream_segment->scale [0] = distance_3d (stream_segment->base_point, endpoint);
					stream_segment->scale [1] = street_width;
					stream_segment->scale [2] = street_width;

					set_angles (stream_segment->base_point, endpoint, stream_segment->angle_radians);

					scene.objects.push_back (stream_segment);
				}
				else {

					for (node_index = 0;
					node_index < ((map_polygon *) *segment)->node_count - 1;
					++node_index) {
							segment_start.x = (double) ((map_polygon *) *segment)->nodes [node_index * 2] / 1.0e6;
							segment_start.y = (double) ((map_polygon *) *segment)->nodes [node_index * 2 + 1] / 1.0e6;
							if (elevations)
								segment_start.z = elevations->nearest_elevation (segment_start) + 1.0;
							else
								segment_start.z = EARTH_RADIUS_KM * 1000.0;

							segment_end.x = (double) ((map_polygon *) *segment)->nodes [(node_index + 1) * 2] / 1.0e6;
							segment_end.y = (double) ((map_polygon *) *segment)->nodes [(node_index + 1) * 2 + 1] / 1.0e6;
							if (elevations)
								segment_end.z = elevations->nearest_elevation (segment_end) + 1.0;
							else
								segment_end.z = EARTH_RADIUS_KM * 1000.0;

							stream_segment = new collada_object;
							stream_segment->id = ++count_segment_nodes; // scene.unique_id();
							stream_segment->name.format ("S%ld%03d", (*segment)->id, node_index + 1);
							stream_segment->geometry_id = pavement_shape->id;
							stream_segment->material_id = color_pavement->id;

							// Start/End points in meters
							stream_segment->base_point = geographic_to_spherical (segment_start);
							endpoint = geographic_to_spherical (segment_end);

							stream_segment->scale [0] = distance_3d (stream_segment->base_point, endpoint);
							stream_segment->scale [1] = street_width;
							stream_segment->scale [2] = street_width;

							set_angles (stream_segment->base_point, endpoint, stream_segment->angle_radians);

							scene.objects.push_back (stream_segment);
						}
				
					++object_count;
				}
			}
		}
	}

	log.add_formatted ("Vector nodes written\t%ld\n", count_segment_nodes);

	return !error;
}

bool river_view_collada::add_obelisk
	(const logical_coordinate &center,
	const double height_meters,
	class interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	collada_object *obelisk;
	collada_geometry *shape;
	collada_material *area_color;
	logical_coordinate coordinate, top;

	area_color = scene.create_material (RGB (0, 0, 0), 1.0, "Obelisk");

	shape = new collada_geometry;
	shape->id = scene.unique_id_geometry();
	shape->name.format ("OBLSK%ld", shape->id);
	shape->type = COLLADA_OBJECT_3D;
	shape->make_box (0, -0.5, -0.5, 1.0, 0.5, 0.5); // 1m x 1m x 1m, scaled later
	// shape->build_normals = false;
	scene.geometries.insert(std::pair <long, collada_geometry *>(shape->id, shape));

	coordinate = center;
	top = coordinate;
	coordinate = geographic_to_spherical (coordinate);

	top.z += height_meters;
	top = geographic_to_spherical (top);

	obelisk = new collada_object;
	obelisk->id = scene.unique_id ();
	obelisk->name.format ("O%ld", obelisk->id);
	obelisk->geometry_id = shape->id;
	obelisk->material_id = area_color->id;
	obelisk->base_point = coordinate;
	set_angles (obelisk->base_point, top, obelisk->angle_radians);

	obelisk->scale [0] = height_meters;
	obelisk->scale [1] = 25.0;
	obelisk->scale [2] = 25.0;

	scene.objects.push_back (obelisk);

	return !error;
}

void river_view_collada::create_lights
	(const bounding_cube &map_area)

{
	collada_light spot;
	logical_coordinate center;

	scene.lights.clear();

	center = map_area.center ();

	// Above center pointing down
	spot.id = 1;
	spot.name.format ("Light%d", spot.id);
	spot.base_point.x = center.x / 1.0e6;
	spot.base_point.y = center.y / 1.0e6;
	spot.base_point.z = EARTH_RADIUS_KM * 1000.0 + 5000.0;
	spot.target = spot.base_point;
	spot.base_point = geographic_to_spherical (spot.base_point);

	spot.base_point.z = EARTH_RADIUS_KM * 1000.0;
	spot.target = geographic_to_spherical (spot.target);
	set_angles (spot.base_point, spot.target, spot.angle_radians);

	scene.lights.push_back(spot);

	// above east pointing at surface west
	spot.id = 2;
	spot.name.format ("Light%d", spot.id);
	spot.base_point.x = map_area.x [1] / 1.0e6;
	spot.base_point.y = center.y / 1.0e6;
	spot.base_point.z = max_elevation_meters + 4000.0;
	spot.base_point = geographic_to_spherical (spot.base_point);

	spot.target.x = map_area.x [1] / 1.0e6;
	spot.target.y = center.y / 1.0e6;
	spot.target.z = min_elevation_meters;
	spot.target = geographic_to_spherical (spot.target);
	set_angles (spot.base_point, spot.target, spot.angle_radians);

	// above SE pointing at NW
	spot.id = 3;
	spot.name.format ("Light%d", spot.id);
	spot.base_point.x = map_area.x [1] / 1.0e6;
	spot.base_point.y = map_area.y [0] / 1.0e6;
	spot.base_point.z = max_elevation_meters + 4000.0;
	spot.base_point = geographic_to_spherical (spot.base_point);

	spot.target.x = map_area.x [0] / 1.0e6;
	spot.target.y = map_area.y [1] / 1.0e6;
	spot.target.z = min_elevation_meters;
	spot.target = geographic_to_spherical (spot.target);
	set_angles (spot.base_point, spot.target, spot.angle_radians);

	scene.lights.push_back(spot);

}

bool river_view_collada::build_camera_area
	(const std::chrono::system_clock::time_point &end_time,
	interface_window *view,
	dynamic_string &log)

// start above North center
// circle CW

{
	bool error = false, verbose = false;
	logical_coordinate position, camera_target, start_base, end_position, up_vector;
	std::chrono::system_clock::time_point time; // , interval;
	double radius_meters, radius_degrees_longitude, radius_degrees_latitude;
	collada_animation animation_rotation [3], animation_translation [3];

	int degree;

	log += "build_camera_area\n";
	log.add_formatted ("Min elevation meters\t%.6lf\n", min_elevation_meters);
	log.add_formatted ("Max elevation meters\t%.6lf\n", max_elevation_meters);

	scene.camera = new collada_camera;
	view->update_progress ("Camera", 0);
	scene.camera->name = "Camera_01";

	camera_target = map_boundary.center ();
	camera_target.x /= 1.0e6;
	camera_target.y /= 1.0e6;
	camera_target.z = min_elevation_meters;

	// Back camera out to 4x distance from center to edge
	position.x = camera_target.x;
	position.y = camera_target.y - ((map_boundary.y [0] / 1.0e6) - camera_target.y) * 4.0;
	position.z = min_elevation_meters;  // set at same z as camera_target for computation of radius

	radius_meters = distance_meters (position, camera_target);
	radius_degrees_latitude = offset_latitude_meters (camera_target, radius_meters);
	radius_degrees_longitude = offset_latitude_meters (camera_target, radius_meters);

	log.add_formatted ("Radius meters\t%.6lf\tDegrees lat\t%.6lf\tDegrees lon\t%.6lf\n", radius_meters, radius_degrees_latitude, radius_degrees_longitude);

	position.x = camera_target.x;
	position.y = camera_target.y + radius_degrees_latitude;
	position.z = max_elevation_meters + 5000.0;

	animation_rotation [0].id = scene.camera->unique_id_animation();
	animation_rotation [1].id = animation_rotation [0].id + 1; // unique_id_animation won't work a second time until animation_x is added to scene.camera
	animation_rotation [2].id = animation_rotation [1].id + 1; // unique_id_animation won't work a second time until animation_x is added to scene.camera
	animation_translation [0].id = animation_rotation [2].id + 1;
	animation_translation [1].id = animation_translation [0].id + 1;
	animation_translation [2].id = animation_translation [1].id + 1;

	animation_rotation [0].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_rotation [1].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_rotation [2].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_translation [0].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_translation [1].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_translation [2].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_rotation [0].dimension_index = 0;
	animation_rotation [1].dimension_index = 1;
	animation_rotation [2].dimension_index = 2;
	animation_translation [0].dimension_index = 0;
	animation_translation [1].dimension_index = 1;
	animation_translation [2].dimension_index = 2;

	time = start_date + std::chrono::seconds (seconds_per_frame * 12);

	for (degree = 0; degree < 360; ++degree) {
		// Start position at degree
		if (verbose)
			log.add_formatted ("%d\t%.4lf\t%.4lf\n", degree, (double) position.x / 1.0e6, (double) position.y / 1.0e6);

		// end one degree further around
		end_position.x = camera_target.x + sin (degree * DYNAMIC_MAP::DEG2RAD) * radius_degrees_longitude;
		end_position.y = camera_target.y + cos (degree * DYNAMIC_MAP::DEG2RAD) * radius_degrees_latitude;
		end_position.z = position.z;

		// up_vector is straight out from earth's core 
		up_vector = spherical_up_vector (position.y, position.x);

		// several frames per degree
		// build_camera_animation (12, position, start_target, half_time, end_position, end_target, animation_rotation, animation_translation, log);
		build_camera_animation (time, position, camera_target, up_vector, time + std::chrono::seconds (seconds_per_frame), end_position, camera_target, animation_rotation, animation_translation, verbose, log);
		position = end_position;
		time += std::chrono::seconds (seconds_per_frame) * 2;
	}

	scene.camera->animation.push_back(animation_rotation [0]);
	scene.camera->animation.push_back(animation_rotation [1]);
	scene.camera->animation.push_back(animation_rotation [2]);
	scene.camera->animation.push_back(animation_translation [0]);
	scene.camera->animation.push_back(animation_translation [1]);
	scene.camera->animation.push_back(animation_translation [2]);

	return !error;
}

bool river_view_collada::build_camera_spiral_in
	(bounding_cube *stream_area,
	const std::chrono::system_clock::time_point &end_time,
	interface_window *view,
	dynamic_string &log)

// start 20 diameters away
// circle while descending to altitude above area

{
	bool error = false, verbose = false;
	logical_coordinate position, camera_target, start_base, end_position, up_vector, final_camera_target;
	collada_animation animation_rotation [3], animation_translation [3];
	std::chrono::system_clock::time_point time;
	double descent_rate, latitude_increment, longitude_increment;

	int time_degree;

	scene.camera = new collada_camera;
	view->update_progress ("Camera", 0);
	scene.camera->name = "Camera_01";

	camera_target.x = 0;
	camera_target.y = 0;
	camera_target.z = 0;

	// Start at 20 earth radius above area
	position.z = EARTH_RADIUS_KM * 20000.0;
	position.x = stream_area->x [0] / 1.0e6;
	position.y = 0.0;
	end_position = position;

	// Approach from distance
	descent_rate = (position.z - (max_elevation_meters + 2000.0)) / 360.0;
	// Move from equator upwards to area
	latitude_increment = (stream_area->center ().y / 1.0e6) / 360.0;

	animation_rotation [0].id = scene.camera->unique_id_animation();
	animation_rotation [1].id = animation_rotation [0].id + 1; // unique_id_animation won't work a second time until animation_x is added to scene.camera
	animation_rotation [2].id = animation_rotation [1].id + 1; // unique_id_animation won't work a second time until animation_x is added to scene.camera
	animation_translation [0].id = animation_rotation [2].id + 1;
	animation_translation [1].id = animation_translation [0].id + 1;
	animation_translation [2].id = animation_translation [1].id + 1;

	animation_rotation [0].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_rotation [1].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_rotation [2].animation_type = COLLADA_ANIMATE_ROTATE;
	animation_translation [0].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_translation [1].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_translation [2].animation_type = COLLADA_ANIMATE_TRANSLATE;
	animation_rotation [0].dimension_index = 0;
	animation_rotation [1].dimension_index = 1;
	animation_rotation [2].dimension_index = 2;
	animation_translation [0].dimension_index = 0;
	animation_translation [1].dimension_index = 1;
	animation_translation [2].dimension_index = 2;

	time = start_date + std::chrono::seconds (seconds_per_frame * 12);

	// Camera up: Polaris
	up_vector.x = 0;
	up_vector.y = 1.0;
	up_vector.z = 0;

	for (time_degree = 0; time_degree < 360; ++time_degree) {
		// Start position at degree

		// end one degree further around
		end_position.x += (double) time_degree;
		if (end_position.x > 360.0)
			end_position.x -= 360.0;
		end_position.y = position.y + latitude_increment;
		end_position.z = position.z - descent_rate;

		log += "\n";
		log.add_formatted ("%d\t%.4lf\t%.4lf\t%.4lf\n", time_degree, position.x, position.y, position.z);

		// build_camera_animation (12, position, start_target, half_time, end_position, end_target, animation_rotation, animation_translation, log);
		/*
		if (time == 200)
			verbose = true;
		else
			verbose = false;
		*/

		build_camera_animation (time, position, camera_target, up_vector, time + std::chrono::seconds (seconds_per_frame), end_position, camera_target, animation_rotation, animation_translation, verbose, log);
		position = end_position;
		time += std::chrono::seconds (seconds_per_frame) * 2;
	}

	// transition camera_target, not camera_position
	// target moves from 0,0,0 to center of screen_area
	latitude_increment = ((stream_area->center ().y / 1.0e6) - camera_target.y) / 100.0;
	longitude_increment = ((stream_area->center ().x / 1.0e6) - camera_target.x) / 100.0;
	descent_rate = max_elevation_meters / 100.0;
	end_position = position;

	for (time_degree = 0; time_degree < 100; ++time_degree) {
		// Start position at degree
		log += "\n";
		log.add_formatted ("%d\t%.4lf\t%.4lf\t%.4lf\n", time_degree, position.x, position.y, position.z);

		final_camera_target.x = camera_target.x + longitude_increment;
		final_camera_target.y = camera_target.y + latitude_increment;
		final_camera_target.z = camera_target.z + descent_rate;

		build_camera_animation (time, position, camera_target, up_vector, time + std::chrono::seconds (seconds_per_frame), end_position, final_camera_target, animation_rotation, animation_translation, verbose, log);
		position = end_position;
		camera_target = final_camera_target;
		time += std::chrono::seconds (seconds_per_frame) * 2;
	}

	// Camera animation export from blender has location x, y, z followed by rotation z, y, x (followed by scale x, y, z)
	scene.camera->animation.push_back(animation_translation [0]);
	scene.camera->animation.push_back(animation_translation [1]);
	scene.camera->animation.push_back(animation_translation [2]);
	scene.camera->animation.push_back(animation_rotation [2]);
	scene.camera->animation.push_back(animation_rotation [1]);
	scene.camera->animation.push_back(animation_rotation [0]);

	return !error;
}

void river_view_collada::describe_run
	(dynamic_string &log)

{
	river_view_map::describe_run (log);

	switch (camera_type) {
		case COLLADA_CAMERA_NONE:
			log += "No camera.\n";
			break;
		case COLLADA_CAMERA_ORBIT:
			log += "Camera area orbit.\n";
			break;
		case COLLADA_CAMERA_SPACE_APPROACH:
			log += "Camera space approach.\n";
			break;
		case COLLADA_CAMERA_FOLLOW_STREAM:
			log += "Camera follow stream.\n";
			break;
		case COLLADA_CAMERA_FIXED:
			log += "Camera fixed.\n";
	}
}


void river_view_collada::insert_relevant_objects
	(std::set <map_object *> *plotting_stations,
	// const std::set <long> *plotting_streams,
	const map_layer *layer,
	interface_window *view,
	dynamic_string &log)

{
	std::vector <map_object *>::const_iterator station;

	if ((area_definition == RIVER_VIEW_AREA_DEFINED_BOX)
	|| (area_definition == RIVER_VIEW_AREA_SOURCE_UPSTREAM)) {
		for (station = layer->objects.begin ();
		station != layer->objects.end ();
		++station) {
			if ((*station)->overlaps_logical (&map_boundary))
				plotting_stations->insert (*station);
		}
	}
	/*
	else
		if (area_definition == RIVER_VIEW_AREA_SOURCE_UPSTREAM) {
			// find stations near plotting_streams using fast set lookup
			for (station = layer->objects.begin ();
			station != layer->objects.end ();
			++station) {
				if (plotting_streams->find ((*station)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID]) != plotting_streams->end ())
					plotting_stations->insert (*station);
			}
		}
		*/
		else
			if (area_definition == RIVER_VIEW_ALL_AREA) {
				for (station = layer->objects.begin ();
				station != layer->objects.end ();
				++station)
					plotting_stations->insert (*station);
			}
}