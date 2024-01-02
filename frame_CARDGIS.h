
class frame_CARDGIS: public wxFrame {
	friend class panel_shapefile;
	public:
		double polygon_x_meters, polygon_y_meters, x_over_y;
		dynamic_string filename_nhd_area;

		frame_CARDGIS
			(const wxString& title,
			const wxPoint& pos,
			const wxSize& size,
			class app_CARDGIS *);
		virtual ~frame_CARDGIS ();

		wxComboBox *combo_select_layer;

		void OnClose (wxCloseEvent&);
		void OnExit(wxCommandEvent&);
		void OnAbout(wxCommandEvent&);
		void OnFileNew(wxCommandEvent&);
		void OnFileOpen(wxCommandEvent&);
		void OnFileOpenTIFF(wxCommandEvent&);
		void OnOpenArea(wxCommandEvent&);
		void OnOpenArea_HR(wxCommandEvent&);
		void OnReadTIGER (wxCommandEvent&);
		void OnOpenRiver (wxCommandEvent&);
		void OnRouteColorado (wxCommandEvent&);
		void OnRouteMississippi (wxCommandEvent&);
		void OnWriteStrahler(wxCommandEvent&);
		void OnWritePointCOMID(wxCommandEvent&);
		void OnWriteCollada (wxCommandEvent &);
		void OnWrite2dAnimation (wxCommandEvent &);
		// void OnBuildBuffer (wxCommandEvent &);
		void OnBuildClipperBuffer (wxCommandEvent &);
		void OnMergePolygons (wxCommandEvent &);
		void OnConfigure (wxCommandEvent &);
		void OnOpenShapefile (wxCommandEvent &);
		void OnReport (wxCommandEvent &);
		void OnReadFlowNHD (wxCommandEvent &);
		void OnReadFlowChina (wxCommandEvent &);
		void OnRouteChinaRiver (wxCommandEvent &);
		void OnRouteAstar (wxCommandEvent &);
		void OnRouteAllChinaRivers (wxCommandEvent &);
		void OnReadFlowlineVAA (wxCommandEvent &);
		void OnReadElevation (wxCommandEvent &);
		void OnDescribeFlow (wxCommandEvent &);
		void OnSelectMapObject (wxCommandEvent &);
		void OnPanelMeasureDistance (wxCommandEvent &);
		void OnReadIowaLakes (wxCommandEvent &);
		void OnReadGridFloat (wxCommandEvent &);
		void OnPointsUpstreamDownstream (wxCommandEvent &);
		void OnReadE85Stations (wxCommandEvent &);

		// Lake Erie
		void OnReadLakeErie (wxCommandEvent &);
		void OnAssignLakeErieGrids (wxCommandEvent &);
		void OnLakeErieMarinaDistance (wxCommandEvent &);
		void OnLakeErieReport (wxCommandEvent &);
		void OnLakeErieReport2 (wxCommandEvent &);

		void OnGrainPrices (wxCommandEvent &);
		void OnLakeCenters (wxCommandEvent &);
		void OnUpstreamCenters (wxCommandEvent &);
		void OnExportPNG (wxCommandEvent &);
		void OnZillowNHD (wxCommandEvent &);
		void OnZillowEcoli (wxCommandEvent &);
		void OnZillowEcoli5Mile (wxCommandEvent &);
		void OnZillow1000m (wxCommandEvent &);
		void OnEColiNHD (wxCommandEvent &);
		void OnSWATAnimation (wxCommandEvent &);
		void OnSWATImage (wxCommandEvent &);

		void OnReadPointLayer (wxCommandEvent &);
		// void OnReadFacilitiesOther (wxCommandEvent &);
		void OnLinkPointLayer (wxCommandEvent &);
		void OnLinkPointLayerThreaded (wxCommandEvent &);
		void OnWriteLinkedPointLayer (wxCommandEvent &);
		void OnReadLinkedPointLayer (wxCommandEvent &);

		/*
		void OnReadLinkedFacilities (wxCommandEvent &);
		void OnReadSamplingStations (wxCommandEvent &);
		void OnReadLinkedUSGS (wxCommandEvent &);
		void OnWriteLinkedUSGS (wxCommandEvent &);
		void OnReadSTORETLegacy(wxCommandEvent &);
		void OnLinkSTORETLegacy(wxCommandEvent &);
		void OnWriteSTORETLegacyBinary(wxCommandEvent &);
		void OnReadSTORETLegacyBinary(wxCommandEvent &);
		void OnReadSTORETModern(wxCommandEvent &);
		void OnLinkSTORETModern(wxCommandEvent &);
		void OnWriteSTORETModernBinary(wxCommandEvent &);
		void OnReadSTORETModernBinary(wxCommandEvent &);
		*/

		void OnSelectID (wxCommandEvent &);
		void OnSelectName (wxCommandEvent&);
		void OnMatchAttribute (wxCommandEvent &);
		void OnMatchName (wxCommandEvent &);
		void OnZoomIn (wxCommandEvent &);
		void OnZoomOut (wxCommandEvent &);
		void OnZoomSelection (wxCommandEvent &);
		void OnShowLegend (wxCommandEvent &);
		void OnRouteFacilities (wxCommandEvent &);
		void OnRouteFacilitiesThreaded (wxCommandEvent &);
		void OnRouteFacilitiesRiver (wxCommandEvent &);
		void OnLinkSamplingStations (wxCommandEvent &);
		void OnSelectLayer (wxCommandEvent &);
		void OnReadFlowNetwork (wxCommandEvent &);
		void OnWriteFlowNetwork (wxCommandEvent &);
		// void OnShowRiver (wxCommandEvent &);
		void OnShowSelection (wxCommandEvent &);
		void OnSelectionClear (wxCommandEvent &);
		void OnShowRiverSegment (wxCommandEvent &);
		void OnListUpstream (wxMenuEvent &);
		void OnListDownstream(wxMenuEvent&);
		void OnListDownstreamFromCOMIDs(wxCommandEvent &); // 2023-04-27 Yongjie
		void OnImport_NHDPlus_HR_geometry (wxCommandEvent&); // 2023-05-04
		void OnImport_NHDPlus_HR_network (wxCommandEvent&); // 2023-05-04
		void OnSelectUpstream (wxMenuEvent &);
		void OnSelectDownstream (wxMenuEvent &);
		void OnClosestSegment (wxMenuEvent &);
		void OnCOMIDNetwork (wxMenuEvent &);
		void OnCOMIDStreamLevel (wxMenuEvent &);
		void OnWriteShapefileLayer (wxCommandEvent &);
		void OnWriteShapefileSelection (wxCommandEvent &);
		void OnBufferFacility (wxCommandEvent &);
		void OnIntersectBuffer (wxCommandEvent &);
		// void OnIntersectBufferUpstream (wxCommandEvent &);
		// void OnIntersectBufferDownstream (wxCommandEvent &);
		// void OnIntersectBufferCensus (wxCommandEvent &);
		void OnIntersectBufferCensusAll (wxCommandEvent &);
		void OnIntersectCircleCensus (wxCommandEvent &);
		void OnIntersectCircleCensusAll (wxCommandEvent &);
		void OnBisectSegment (wxCommandEvent &);
		void OnClickDistance(wxCommandEvent&);
		void OnCheckDistance(wxCommandEvent&);
		void OnIntersectBufferGrid(wxCommandEvent&);
		void OnIntersectTwoLayers(wxCommandEvent&);
		void OnAddSelectionToLayer (wxCommandEvent &);
		void OnAddNewLayer (wxCommandEvent &);
		void OnEditLayerProperties (wxCommandEvent &);
		void OnReadLayerData (wxCommandEvent &);
		void OnExportLayerData (wxCommandEvent &);
		void OnReadZipCodes (wxCommandEvent &);
		void OnSSURGOZipCodes (wxCommandEvent &);
		void OnHUC12Upstream (wxCommandEvent &);
		void OnHUC8Upstream (wxCommandEvent &);
		void RoutingHUC12to8 (wxCommandEvent &);
		void OnRouteCounties (wxCommandEvent &);
		void OnRouteCountiesRiver (wxCommandEvent &);
		void OnCombineRouteCounties (wxCommandEvent &);
		void OnListCountiesUpstream (wxCommandEvent &);
		void OnCombineCountiesUpstream (wxCommandEvent &);
		void OnCountiesUpstreamStrahler (wxCommandEvent &);
		void OnMapFishAdviseries (wxCommandEvent &);
		void OnConvertMultipoint (wxCommandEvent &);

		void OnImportBHO (wxCommandEvent &);
		void OnOpenBHOArea (wxCommandEvent &);
		void OnBHOReservoirs (wxCommandEvent &);
		void OnBHOReservoirMunicipality (wxCommandEvent &);
		void OnFarmlandFinderFieldCircles(wxCommandEvent &);
		void OnFarmlandFinderSSURGO(wxCommandEvent &);
		void OnFarmlandFinderProximity(wxCommandEvent &);
		void OnFarmlandFinderPopulation(wxCommandEvent &);
		void OnFarmlandFinderPowerlines(wxCommandEvent&);
		void OnFarmlandFinderSSURGOWeighted(wxCommandEvent&);
		void OnZillowPowerlineTable(wxCommandEvent&);
		void On_LAGOS_NHD(wxCommandEvent &);
		void On_UMRB_Animation(wxCommandEvent &);
		void OnShowSelectedItems(wxCommandEvent &);
		void On_LAGOS_Zillow(wxCommandEvent &);
		void On_Pipeline_Zillow(wxCommandEvent &);
		void On_Pipeline_Zillow_Within_Radius (wxCommandEvent &);
		void On_Zillow_Schools_Hospitals(wxCommandEvent&);
		void On_ListNearbyLakes (wxCommandEvent &);
		void On_OutlineMatchingWind (wxCommandEvent &);
		void On_BufferPolygonLayers (wxCommandEvent &);
		void On_WindDistances (wxCommandEvent &);
		void On_IntersectWindLayers (wxCommandEvent &);
		void On_MatchUMRBHUCs (wxCommandEvent &);
		void OnLayerObjectTable(wxCommandEvent&);
		void OnMatchAFIDA(wxCommandEvent&);
		void OnMatch_AFIDA_Kansas(wxCommandEvent&);
		void OnScanSWNET (wxCommandEvent&);
		void OnDeleteSubPolygon(wxCommandEvent&);
		void OnWriteCOMIDFromTo(wxCommandEvent&);
		void OnGSSURGO_SSURGO(wxCommandEvent&);
		void OnPointPolygonProximity(wxCommandEvent&);
		DECLARE_EVENT_TABLE()
	protected:
		class app_CARDGIS *app;
		class odbc_database *db_odbc;
		class RecentlyOpenedDatabases *recent_dbs;
		class RecentlyOpenedShapefiles *recent_shapefiles;
		class router_NHD *router_v2, *router_hr;

		class panel_shapefile *panel_watershed;
		class wxButton *button_goto_id;
		// class wxStaticText *static_id, *static_name;
		class long_long_integer_edit *edit_select_id;
		class wxButton *button_select_id, *button_select_name;

		class dynamic_map *map_watershed;
		// class map_layer *layer_rivers; // , *layer_facilities, *layer_sampling_stations, *layer_storet_modern, *layer_storet_legacy;
		double area_elevation_minimum_m, area_elevation_maximum_m;

		// class color_set *attribute_color_sets_river;

		// class interface_window_wx *status_window;  // structure to so panel_shapefile can send status messages to this without knowing specifics of class frame_nhd_view

		int area_utm_zone;
		char area_utm_letter;

		void DoSize();

		bool connect_to_odbc
			(class odbc_database_credentials *creds,
			class interface_window *view,
			dynamic_string &log);

		void create_menus ();

		// void fill_subbasin_list ();
		bool create_tables
			(dynamic_string &log);

		bool distance_in_divergence_km
			(const flow_network_divergence *div,
			const int comid_index,
			const bool up,
			double *distance_km,
			dynamic_string &log);

		void enable_map_controls ();

		void intersect_buffer_upstream
			(const class map_object *facility,
			const map_layer *layer_census_tracts,
			std::vector <double> *buffer_widths,
			double start_upstream_miles,
			double length_upstream_miles,
			const double cut_line_length_miles,
			const short tract_name_source,
			const int tract_numeric_attribute_id,
			const int tract_text_attribute_id,
			class interface_window *,
			dynamic_string &log);
		void intersect_buffer_upstream_difference
			(const class map_object *facility,
			const map_layer *layer_census_tracts,
			std::vector <double> *buffer_widths,
			double start_upstream_miles,
			double length_upstream_miles,
			const double cut_line_length_miles,
			const short tract_name_source,
			const int tract_numeric_attribute_id,
			const int tract_text_attribute_id,
			class interface_window *,
			dynamic_string &log);
		void intersect_buffer_downstream
			(const class map_object *facility,
			const map_layer *layer_census_tracts,
			std::vector <double> *buffer_widths,
			double start_upstream_miles,
			double length_upstream_miles,
			const double cut_line_length_miles,
			const short tract_name_source,
			const int tract_numeric_attribute_id,
			const int tract_text_attribute_id,
			class interface_window *,
			dynamic_string &log);
		void intersect_buffer_downstream_difference
			(const class map_object *facility,
			const map_layer *layer_census_tracts,
			std::vector <double> *buffer_widths,
			double start_upstream_miles,
			double length_upstream_miles,
			const double cut_line_length_miles,
			const short tract_name_source,
			const int tract_numeric_attribute_id,
			const int tract_text_attribute_id,
			class interface_window *,
			dynamic_string &log);
		bool intersect_buffer_up_and_down
			(const class map_object *facility,
			const map_layer *layer_census_tracts,
			std::vector <double> *buffer_widths,
			double start_upstream_miles,
			double length_upstream_miles,
			double start_downstream_miles,
			double length_downstream_miles,
			const double cut_line_length_miles,
			const short tract_name_source,
			const int tract_numeric_attribute_id,
			const int tract_text_attribute_id,
			class interface_window *,
			dynamic_string &log);

		map_layer *load_river
			(dynamic_string &area_name,
			std::set <int> *state_set,
			class interface_window *view,
			dynamic_string &log);
		class map_layer *read_nhd_area_rivers
			(std::vector <dynamic_string> *area_names,
			const bool read_network_bin,
			class interface_window *view,
			dynamic_string &log);
		bool open_file
			(const dynamic_string &filename,
			class interface_window *view,
			dynamic_string &log);

		bool read_db_start
			(class interface_window *view,
			dynamic_string &log);

		void read_field_settings ();
		bool read_schools_hospitals_universities_state
			(const int state_fips,
			const int number_of_threads,
			const dynamic_string& path_output,
			const dynamic_string& prefix_output,
			class map_layer* layer_zillow,
			class interface_window *view,
			dynamic_string& log);
		bool read_schools_hospitals_universities
			(class map_layer* layer_zillow,
			class MapLayerProximity* proximator,
			class interface_window* view,
			dynamic_string& log);
		bool read_zipcode_shapefile
			(const dynamic_string &path_shapefile,
			class shapefile_filter *,
			class map_layer *layer_zipcodes,
			interface_window *update_display,
			dynamic_string &log);
		bool read_water_bodies
			(const dynamic_string &path_shapefile,
			class dynamic_map *map_watershed,
			class interface_window *update_display,
			dynamic_string &log);
		
		map_layer *read_point_layer
			(dynamic_string &filename,
			const int record_limit,
			const bool normalize_longitude,
			const bool numeric_id,
			const BYTE format,
			std::vector <dynamic_string> *column_names,
			std::set <int> *text_column_indexes,
			std::set <int> *skip_column_indexes,
			const int id_column_index,
			const int lat_column_index,
			const int lon_column_index,
			dynamic_string &log);
		bool accumulate_rivers
			(std::set <dynamic_string> *area_names,
			class flow_network *rivers,
			class interface_window *update_display,
			dynamic_string &log);

		class router_NHD* relevent_router();

		void select_entire_river
			(const long long first_id,
			const class map_layer *source,
			class map_layer *destination,
			dynamic_string &log);

		void select_overlapping
			(class bounding_cube *extent,
			const class map_layer *source,
			class map_layer *destination,
			class map_layer *not_including);

		bool write_upstream_downstream
			(const class map_layer *point_layer,
			const bool write_to_file,
			dynamic_string &filename_output,
			const bool add_to_layer,
			class map_layer *destination_layer,
			dynamic_string &log);

		bool SaveRecord
			(dynamic_string &log);
};

enum
{
    ID_Quit = 1,
    ID_About,
	WXID_REPORT_POLLUTANTS = 110,
	wxID_CONFIGURE,
	wxID_IMPORT,
	wxID_GOTO_ID,
	wxID_READ_FLOWLINEVAA,
	wxID_READ_ELEVATION,
	wxID_READ_FLOW_NETWORK_NHD,
	wxID_READ_FLOW_NETWORK_CHINA,
	wxID_ROUTE_CHINA_RIVER,
	wxID_ROUTE_ASTAR,
	wxID_ROUTE_ALL_CHINA_RIVERS,
	wxID_REPORT,
	wxID_SET_SHAPEFILE,

	MENU_READ_POINT_LAYER,
	MENU_LINK_POINT_LAYER,
	MENU_LINK_POINT_LAYER_THREADED,
	MENU_WRITE_LINKED_POINT_LAYER,
	MENU_READ_LINKED_POINT_LAYER,

	MENU_READ_STORET_LEGACY,
	MENU_LINK_STORET_LEGACY,
	MENU_WRITE_STORET_LEGACY_BINARY,
	MENU_READ_STORET_LEGACY_BINARY,

	MENU_READ_STORET_MODERN,
	MENU_LINK_STORET_MODERN,
	MENU_WRITE_STORET_MODERN_BINARY,
	MENU_READ_STORET_MODERN_BINARY,

	MENU_READ_SAMPLING_STATIONS,
	MENU_LINK_SAMPLING_STATIONS,
	MENU_WRITE_LINKED_USGS,
	MENU_READ_LINKED_USGS,

	BUTTON_ZOOM_IN,
	BUTTON_ZOOM_OUT,
	BUTTON_ZOOM_SELECTION,
	BUTTON_SHOW_LEGEND,
	MENU_DESCRIBE_FLOW,
	MENU_ROUTE_FACILITIES,
	MENU_ROUTE_FACILITIES_THREADED,
	MENU_ROUTE_FACILITIES_RIVER,
	MENU_ROUTE_COLORADO,
	MENU_ROUTE_MISSISSIPPI,
	MENU_WRITE_STRAHLER,
	MENU_WRITE_POINT_COMID,
	MENU_WRITE_COLLADA,
	MENU_WRITE_2DANIMATION,
	MENU_BUILD_BUFFER,
	MENU_BUILD_BUFFER_CLIPPER,
	MENU_MERGE_POLYGONS,
	MENU_CLICK_DISTANCE,
	MENU_CHECK_DISTANCE,
	MENU_COMID_NETWORK,
	COMBO_SELECT_LAYER,
	MENU_READ_FLOW_NETWORK,
	MENU_WRITE_FLOW_NETWORK,
	wxID_SHOW_SELECTION,
	wxID_SELECTION_CLEAR,
	wxID_SHOW_RIVER_SEGMENT,
	wxID_READ_BINARIES,
    MENU_AREA_1=601,
    MENU_AREA_2,
    MENU_AREA_3N,
    MENU_AREA_3S,
    MENU_AREA_3W,
	MENU_AREA_4,
	MENU_AREA_5,
	MENU_AREA_6,
	MENU_AREA_7,
	MENU_AREA_8,
	MENU_AREA_9,
	MENU_AREA_10L,
	MENU_AREA_10U,
	MENU_AREA_11,
	MENU_AREA_12,
	MENU_AREA_13,
	MENU_AREA_14,
	MENU_AREA_15,
	MENU_AREA_16,
	MENU_AREA_17,
	MENU_AREA_18,
	MENU_MISSISSIPPI=701,
	MENU_COLORADO,
	MENU_AREA_HR_1,
	MENU_AREA_HR_7,
	MENU_WRITE_SHAPEFILE_LAYER,
	MENU_WRITE_SHAPEFILE_SELECTION,
	MENU_BUFFER_FACILITY,
	MENU_INTERSECT,
	MENU_INTERSECT_UPSTREAM,
	MENU_INTERSECT_DOWNSTREAM,
	MENU_INTERSECT_CIRCLE,
	MENU_INTERSECT_ALL,
	MENU_INTERSECT_CIRCLE_ALL,
	MENU_BISECT,
	BUTTON_SELECT_ID,
	MENU_SELECT_TEXT_ATTRIBUTE,
	BUTTON_SELECT_NAME,
	MENU_MATCH_NAME,
	MENU_ADD_NEW_LAYER,
	MENU_EDIT_LAYER_PROPERTIES,
	MENU_INTERSECT_BUFFER_GRID,
	MENU_IOWA_LAKES,
	MENU_E85,
	MENU_ERIE,
	MENU_ASSIGN_ERIE_GRID,
	MENU_ERIE_MARINA_DISTANCE,
	MENU_ERIE_REPORT,
	MENU_ERIE_REPORT_2,
	MENU_GRIDFLOAT,
	MENU_WRITE_UPSTREAM_DOWNSTREAM,
	MENU_READ_LAYER_DATA,
	MENU_EXPORT_LAYER_DATA,
	MENU_READ_TIGER,
	MENU_READ_ZIPCODES,
	MENU_GRAINPRICE_MATCH,
	MENU_SAVE_PNG,
	MENU_SWAT_ANIMATION,
	MENU_SWAT_IMAGE,
	MENU_LAKE_CENTERS,
	MENU_UPSTREAM_CENTERS,
	MENU_SSURGO_ZIPCODES,
	MENU_HUC_12_UPSTREAM,
	MENU_HUC_8_UPSTREAM,
	MENU_ROUTING_12_8,
	MENU_ROUTE_COUNTIES,
	MENU_ROUTE_COUNTIES_RIVER,
	MENU_COMBINE_ROUTED_COUNTIES,
	MENU_COUNTIES_UPSTREAM,
	MENU_COMBINE_COUNTIES_UPSTREAM,
	MENU_COUNTIES_UPSTREAM_STRAHLER,
	MENU_FISH_ADVISERIES,
	MENU_EXPORT_PNG,
	MENU_IMPORT_BHO,
	MENU_BHO_1,
	MENU_BHO_2,
	MENU_BHO_3,
	MENU_BHO_4,
	MENU_BHO_5,
	MENU_BHO_6,
	MENU_BHO_7,
	MENU_BHO_8,
	MENU_BHO_9,
	MENU_CONVERT_MULTIPOINT,
	EDIT_DELETE_SUBPOLYGON,
	MENU_ZILLOW_NHD,
	MENU_ZILLOW_ECOLI,
	MENU_ZILLOW_ECOLI_5MILE,
	MENU_ZILLOW_1000M,
	MENU_ECOLI_NHD,
	MENU_FARMLANDFINDER_CIRCLES,
	MENU_FARMLANDFINDER_SSURGO,
	MENU_FARMLANDFINDER_SSURGO_WEIGHTED,
	MENU_FARMLANDFINDER_PROXIMITY,
	MENU_FARMLANDFINDER_POPULATION,
	MENU_FARMLANDFINDER_POWERLINES,
	MENU_GSSURGO_SSURGO,
	MENU_ZILLOW_POWERLINE_TABLE,
	MENU_LAGOS_NHD,
	MENU_LAGOS_ZILLOW,
	MENU_PIPELINE_ZILLOW,
	MENU_PIPELINE_ZILLOW_10MI,
	MENU_ZILLOW_SCHOOLS_HOSPITALS,
	MENU_LISTNEARBYLAKES,
	MENU_OUTLINEMATCHINGWIND,
	MENU_BUFFERPOLYGONLAYERS,
	MENU_WINDDISTANCES,
	MENU_INTERSECTWINDLAYERS,
	BUTTON_CURRENT_PROJECT,
	MENU_CROSSLAYER_SELECT,
	MENU_BHO_RESERVOIRS,
	MENU_BHO_RESERVOIR_MUNICIPALITY,
	MENU_UMRB_HUCS,
	BUTTON_LAYER_TABLE,
	MENU_MATCH_AFIDA,
	MENU_MATCH_AFIDA_KANSAS,
	MENU_SCAN_SWNET,
	wxID_OPEN_TIFF,
	MENU_LIST_DOWN_FROM_COMIDS,
	MENU_WRITE_FROMTO,
	IMPORT_NHDPLUS_HR_GEOMETRY,
	IMPORT_NHDPLUS_HR_NETWORK,
	MENU_PROCESS_INTERSECT_TWO,
	MENU_POINT_POLYGON
};
