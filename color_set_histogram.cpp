
#include "../util/utility_afx.h"
#include <vector>
#include <map>
#include "../util/dynamic_string.h"
#include "../util/interface_window.h"
#include "../util/interface_window_mfc.h"
#include "../util/device_coordinate.h"
#include "../util/bounding_cube.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "color_set_histogram.h"
// #include <math.h>

/*
void color_set_definition_histogram::copy
	(const color_set_definition *other)

{
	color_set_definition::copy (other);
	count = ((color_set_definition_histogram *) other)->count;
}

*/

void color_set_histogram::clear_counts ()

{
	std::vector <color_set_definition>::iterator range;

	for (range = colors.begin ();
	range != colors.end ();
	++range)
		range->count = 0;
}

long color_set_histogram::total_count
	(long *max_count)

// Count total for histogram denominator
{
	std::vector <color_set_definition>::iterator range;
	long denominator = 0;
	if ((range = colors.begin ()) != colors.end ()) {
		*max_count = range->count;
		while (range != colors.end ()) {
			denominator += range->count;
			if (range->count > *max_count)
				*max_count = range->count;
			++range;
		}
	}
	return denominator;
}

color_set_definition *color_set_histogram::highest_count () const

{
	std::vector <color_set_definition>::const_iterator range;
	color_set_definition *top_color;

	top_color = NULL;
	for (range = colors.begin ();
	range != colors.end ();
	++range) {
		if ((top_color == NULL)
		|| (range->count > top_color->count))
			top_color = (color_set_definition *) &*range;
	}
	return top_color;
}

color_set_definition *color_set_histogram::add_color
	(COLORREF color,
	const double ge,
	const double lt,
	const char *description)

// ge <= x < lt

{
	color_set_definition color_range;

	color_range.ge = ge;
	color_range.lt = lt;
	color_range.color = color;
	if (description != NULL)
		color_range.label = description;

	colors.push_back (color_range);
	return &colors [colors.size () - 1];
}

COLORREF color_set_histogram::get_color
	(const double x) const

// bottom of range overlaps
// color is used if low <= x < high

{
	color_set_definition *found;

	if ((found = match_range (x)) != NULL) {
		found->count += 1;
		return found->color;
	}
	else
		return 0;
}

void color_set_histogram::dump
	(dynamic_string &text) const

{
	std::vector <color_set_definition>::const_iterator range;

	for (range = colors.begin ();
	range != colors.end ();
	++range)
		text.add_formatted ("%lg <= x < %lg\t%d\t%d\t%d\t%ld\n", range->ge, range->lt,
		(int) GetRValue(range->color), (int) GetGValue (range->color), (int) GetBValue (range->color),
		range->count);
}

