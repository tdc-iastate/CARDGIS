
class thread_work_intersect : public thread_work {
	public:
		class map_object *polygon;
		const class map_layer *layer_2;
		class map_layer *layer_results;
		int tract_id_source_1, tract_id_source_2;
		int tract_index_source_1, tract_index_source_2;
};

bool work_intersect
	(class thread_work *job);

class thread_work_create_buffer : public thread_work {
	public:
		class map_object *polygon;
		class map_layer layer_buffer;
		double offset_radius_average;
		bool endtype_closed;
};

bool work_create_buffer
	(class thread_work *job);
