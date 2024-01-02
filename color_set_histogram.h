/*
class color_set_definition_histogram : public color_set_definition {
	public:
		long count;

		color_set_definition_histogram ()
		: color_set_definition ()
		{
			count = 0;
		};

		virtual void copy
			(const color_set_definition *);
};
*/

class color_set_histogram : public color_set {
	public:
		virtual color_set_definition *add_color
			(COLORREF color,
			const double ge,
			const double lt,
			const char *description = NULL);

		void clear_counts ();

		color_set_definition *highest_count () const;
		virtual void dump
			(dynamic_string &text) const;
		virtual COLORREF get_color
			(const double x) const;
		long total_count
			(long *max_count);
};
