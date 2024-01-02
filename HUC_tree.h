
// Near duplicate of flow_network_link, but with long long ids

class HUC_network_link {
	public:
		long long id;
		std::set <HUC_network_link *> from; // 2017-08-17 duplicates in here cause stack overflow

		HUC_network_link ();
		HUC_network_link
			(const HUC_network_link &);
		virtual ~HUC_network_link ();

		void clear ();
		void copy
			(const HUC_network_link &other);

		bool accumulate_upstream
			(std::vector <long long> *comids,
			std::set <long long> *duplicate_check,
			dynamic_string &log) const;
		void build_map_layer
			(class map_layer *,
			const class map_layer *layer_hucs,
			dynamic_string &log) const;
		void write_upstream
			(FILE *f) const;
		int depth () const;

		void add_source
			(const class HUC_network_link *other);
		void copy_leaves
			(const HUC_network_link *other);

		HUC_network_link *match_id_slow
			(const long long target_id) const;
		HUC_network_link *match_just_below
			(const long long target_id) const;
};

class HUC_tree {
	public:
		// std::map <long long, std::set <long long>> tree;
		std::map <long long, HUC_network_link *> root_fragments; // doesn't include divergences
		std::map <long long, HUC_network_link *> all_ids; // Fast lookup of every COMID.  Doesn't include divergences
		std::set <long long> unique_hucs;
		std::map <long long, HUC_network_link *> broken_links; // Fast lookup of every COMID.  Doesn't include divergences

		bool read
			(dynamic_string &filename,
			dynamic_string &log);
		bool read_source_8
			(dynamic_string &filename,
			dynamic_string &log);
		bool read_source_12
			(dynamic_string &filename,
			const long huc_2_ge,
			const long huc_2_lt,
			dynamic_string &log);
		bool read_source_combine
			(dynamic_string &filename,
			const long huc_3_ge,
			const long huc_3_lt,
			dynamic_string &log);
		bool convert_12_8
			(dynamic_string &filename,
			std::map <long, long> *from_to_8,
			dynamic_string &log);

		void build_tree_recursive
			(class interface_window *view,
			dynamic_string &log);
		void build_tree
			(class interface_window *view,
			dynamic_string &log);
		void build_tree_vectors
			(class map_layer *vector_layer,
			const class map_layer *huc_layer,
			class interface_window *view,
			dynamic_string &log);
		bool build_vector_shapefile
			(dynamic_string &filename_vector_shp,
			class map_layer *layer_hucs,
			class map_layer *layer_fragments,
			class dynamic_map *map,
			const bool huc_id_is_name,
			class interface_window *update_display,
			dynamic_string &log);
		bool follow_raw_downstream
			(const long long node_id,
			dynamic_string &log);

		void highlight_loop_8
			(class map_layer *layer_trail,
			class map_layer *layer_hucs,
			class interface_window *view,
			dynamic_string &log);
		void highlight_loop_12
			(class map_layer *layer_trail,
			class map_layer *layer_hucs,
			class interface_window *view,
			dynamic_string &log);

		class HUC_network_link *match_id
			(const long long comid) const;
		class HUC_network_link *match_id_slow
			(const long long comid) const;
		class HUC_network_link *match_just_below
			(const long long comid) const;
		class HUC_network_link *match_all_just_below
			(const long long comid) const;

		void write
			(dynamic_string &filename,
			class interface_window *view,
			dynamic_string &log);
	protected:
		std::map <long long, long long> from_to;
		// std::set <long long> lost_rivers; // Mouth of any river that ended in missing segment (i.e. 03N's 10466473->9889032.  9889032 is actually in 02)

		bool accumulate_upstream
			(const long long comid,
			std::vector <long long> *comids,
			dynamic_string &log) const;
		void add_link
			(const HUC_network_link *link);
		void add_river
			(const class HUC_network_link *outlet);

		HUC_network_link *cleanup_loop
			(HUC_network_link *loopy,
			dynamic_string &log);
		bool convert_tokens
			(dynamic_string &from,
			dynamic_string &to,
			long *from_huc_8,
			long *to_huc_8,
			dynamic_string &log);
		bool convert_tokens
			(dynamic_string &from,
			dynamic_string &to,
			long long *from_huc,
			long long *to_huc,
			dynamic_string &log);

		bool follow_downstream
			(const class HUC_network_link *tributary,
			dynamic_string &log);
};
