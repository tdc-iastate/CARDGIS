
#include "../util/utility_afx.h"
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <future>
#include <stack>
#include <chrono>
#include <wx/frame.h>
#include <wx/notebook.h>
#include <wx/filename.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/dialog.h>
#include <wx/numdlg.h>
// #include <wx/log.h>
#include <wx/app.h>
#include <wx/combobox.h>
#include <wx/listbox.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/stopwatch.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <sys/timeb.h>

// #include <iostream>
// #include <string>
// #include <sstream>

#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/srs/transformation.hpp>
#include <boost/geometry/srs/projection.hpp>
#include <boost/geometry/srs/projections/proj/aea.hpp>
#include <boost/geometry/srs/epsg.hpp>

#include "../util/dynamic_string.h"
#include "../util/utility.h"
#include "../util/filename_struct.h"
#include "../util/Timestamp.h"
#include "../util/interface_window.h"
#include "../util/listbox_files.h"
#include "../util/interface_window_wx.h"
#include "../util/device_coordinate.h"
#include "../util/bounding_cube.h"
#include "../util/dialog_error_list.h"
#include "../util/dialog_prompt.h"
#include "../util/card_bitmap.h"
#include "../util/card_bitmap_tiff.h"
#include "../util/custom.h"
#include "../util/RecentlyOpened.h"
#include "../util/message_slot.h"
#include "../util/xml_parser.h"
#include "../util/arbitrary_counter.h"
#include "../util/Ledger.h"
#include "../util/state_names.h"
#include "../map/color_set.h"
#include "../map/dynamic_map.h"
#include "../map/dynamic_map_wx.h"
#include "../map/dynamic_map_gl2.h"
#include "../map/map_scale.h"
#include "../map/dbase.h"
#include "../map/shapefile.h"
#include "../map/shapefile_filter.h"
#include "../map/map_object_tiff.h"
#include "../map/map_object_tiff_wx.h"
#include "../map/map_object_tiff_gl.h"
#include "../map/BoostProjections.h"
#include "../map/projection_albers.h"
// wx setup.h must appear before odbc_inst.h or error with HAVE_UNISTD_H
#include <odbcinst.h> // use the Odbcinst.lib import library. Also, Odbccp32.dll must be in the path at run time (or Odbcinst.dll for 16 bit).
#include "../odbc/odbc_database.h"
#include "../odbc/odbc_database_access.h"
#include "../odbc/odbc_database_mysql.h"
#include "../odbc/odbc_database_sql_server.h"
#include "../odbc/odbc_field_set.h"
#include "../util/RecentlyOpenedDatabases.h"
#include "../util/RecentlyOpenedShapefiles.h"
#include "panel_shapefile.h"
#include "app_CARDGIS.h"
#include "../cardgis_console/flow_network_divergence.h"
#include "../cardgis_console/flow_network_link.h"
#include "frame_CARDGIS.h"
#include "dialog_configure.h"
#include "dialog_open_database.h"
#include "color_set_histogram.h"
#include "../map/dialog_import_shapefile.h"
#include "../cardgis_console/string_grid.h"
#include "dialog_river_segment.h"
#include "dialog_river.h"
#include "dialog_selection.h"
#include "../cardgis_console/router_NHD.h"
#include "dialog_read_area.h"
#include "dialog_route_points.h"
#include "dialog_import_points.h"
#include "dialog_select_layer.h"
#include "dialog_legend.h"
#include "../Collada/camera_time.h"
#include "../Collada/collada_animation.h"
#include "../Collada/collada_object.h"
#include "../Collada/collada_effect.h"
#include "../Collada/collada_material.h"
#include "../Collada/collada_light.h"
#include "../Collada/xml_command_collada.h"
#include "../Collada/xml_configuration_collada.h"
#include "../Collada/collada_scene.h"
#include "../Collada/blender_python.h"
// #include "MinkowskiBuffer.h"
// #include "../clipper_ver6.2.1/cpp/clipper.hpp"
#include "../clipper_ver6.4.2/cpp/clipper.hpp"
#include "ClipperBuffer.h"

#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Constants.hpp>
#include "timed_measurements.h"
#include "river_view_map.h"
#include "river_view_collada.h"
#include "dialog_edit_layer.h"
#include "dialog_upstream_downstream.h"
#include "dialog_river_view.h"
#include "dialog_collada.h"
#include "dialog_buffer.h"
#include "elevation_matrix.h"
#include "E85_Stations.h"
#include "dialog_read_TIGER.h"
#include "LakeErie.h"
#include "GrainPrice.h"
#include "swat_animator.h"
#include "../subbasin_effect/dialog_run_threads.h"
#include "dialog_match_attribute.h"
#include "HUC_tree.h"
#include "../cardgis_console/rowset_DownstreamByCOMID.h"
#include "../cardgis_console/rowset_DownstreamByCounty.h"
#include "../cardgis_console/rowset_UpstreamCounties.h"
#include "../subbasin_effect/thread_manager.h"
#include "../cardgis_console/UpstreamDownstream.h"
#include "dialog_read_layer_data.h"
// #include "mapview_gl.h"
// #include "panel_shapefile_gl.h"
#include "ChinaRivers.h"

#include "astar_router.h"
#include "../cardgis_console/router_BHO.h"
#include "ZillowSales.h"
#include "../Soils/rowset_ssurgo_horizon.h"
#include "../Soils/rowset_ssurgo_component.h"
#include "../Soils/rowset_ssurgo_mapunit.h"
#include "FarmlandFinder.h"
#include "CountyMaster.h"
#include "LAGOS_Algal_Blooms.h"
#include "dialog_LandSalesSSURGO.h"
#include "IllinoisProductivityIndex.h"
#include "MapLayerProximity.h"
#include "dialog_project.h"
#include "PipelineAccidents.h"
#include "flood_aggregator.h"
#include "PolygonMatch.h"
#include "../util/dialog_ledger.h"
#include "dialog_distance.h"
#include "AFIDA.h"
#include "../cardgis_console/flow_network_divergence_HR.h"
#include "../cardgis_console/flow_network_link_HR.h"
#include "dialog_intersect.h"

// const wxString NHD_LAYER_NAMES [6] = {"NHD Flowlines", "Treatment Facilities", "USGS Sample Stations", "STORET Legacy", "STORET Modern"};

void extract_area_name
	(dynamic_string &filename_nhd_root,
	dynamic_string *area_name)

// "r:\nhdplusv2\nhdplus01\" -> "01"

{
	int index;

	if ((index = filename_nhd_root.match_insensitive ("NHDPLUSV2\\NHDPLUS")) != -1) {
		*area_name = filename_nhd_root.mid (index + 17);
		if (area_name->right (1) == "\\")
			*area_name = area_name->left (area_name->get_length () - 1);
	}
	else
		*area_name = "AREA";
}

BEGIN_EVENT_TABLE (frame_CARDGIS, wxFrame)
	EVT_MENU (wxID_EXIT, frame_CARDGIS::OnExit)
	EVT_MENU (ID_About, frame_CARDGIS::OnAbout)
	EVT_MENU (wxID_OPEN, frame_CARDGIS::OnOpenShapefile)
	EVT_MENU (wxID_NEW, frame_CARDGIS::OnFileNew)
	EVT_MENU (wxID_OPEN_TIFF, frame_CARDGIS::OnFileOpenTIFF)
	// EVT_MENU(wxID_SET_SHAPEFILE, frame_CARDGIS::OnSetShapefile)
	EVT_MENU (wxID_REPORT, frame_CARDGIS::OnReport)
	EVT_MENU (wxID_READ_FLOW_NETWORK_NHD, frame_CARDGIS::OnReadFlowNHD)
	EVT_MENU (wxID_READ_FLOW_NETWORK_CHINA, frame_CARDGIS::OnReadFlowChina)
	EVT_MENU (wxID_ROUTE_CHINA_RIVER, frame_CARDGIS::OnRouteChinaRiver)
	EVT_MENU (wxID_ROUTE_ASTAR, frame_CARDGIS::OnRouteAstar)
	EVT_MENU (wxID_ROUTE_ALL_CHINA_RIVERS, frame_CARDGIS::OnRouteAllChinaRivers)
	EVT_MENU (wxID_READ_FLOWLINEVAA, frame_CARDGIS::OnReadFlowlineVAA)
	EVT_MENU (wxID_READ_ELEVATION, frame_CARDGIS::OnReadElevation)
	// EVT_MENU (MENU_SELECT_COMID, frame_CARDGIS::OnSelectID)
	// EVT_MENU_RANGE( ID_FILE_RECENT_SHAPEFILE, ID_FILE_RECENT_SHAPEFILE + 25, frame_CARDGIS::OnOpenRecent )
	EVT_MENU_RANGE(MENU_AREA_1, MENU_AREA_1 + 21, frame_CARDGIS::OnOpenArea)
	EVT_MENU_RANGE(MENU_AREA_HR_1, MENU_AREA_HR_1 + 1, frame_CARDGIS::OnOpenArea_HR)
	EVT_MENU_RANGE (MENU_MISSISSIPPI, MENU_MISSISSIPPI + 1, frame_CARDGIS::OnOpenRiver)
	EVT_MENU_RANGE (MENU_BHO_1, MENU_BHO_9, frame_CARDGIS::OnOpenBHOArea)
	EVT_MENU (MENU_IOWA_LAKES, frame_CARDGIS::OnReadIowaLakes)
	EVT_MENU (MENU_E85, frame_CARDGIS::OnReadE85Stations)

	// Lake Erie
	EVT_MENU (MENU_ERIE, frame_CARDGIS::OnReadLakeErie)
	EVT_MENU (MENU_ASSIGN_ERIE_GRID, frame_CARDGIS::OnAssignLakeErieGrids)
	EVT_MENU (MENU_ERIE_MARINA_DISTANCE, frame_CARDGIS::OnLakeErieMarinaDistance)
	EVT_MENU (MENU_ERIE_REPORT, frame_CARDGIS::OnLakeErieReport)
	EVT_MENU (MENU_ERIE_REPORT_2, frame_CARDGIS::OnLakeErieReport2)

	// Grain Prices
	EVT_MENU (MENU_GRAINPRICE_MATCH, frame_CARDGIS::OnGrainPrices)

	// 2017-06-16 Dave Keiser national lakes
	EVT_MENU (MENU_LAKE_CENTERS, frame_CARDGIS::OnLakeCenters)
	EVT_MENU (MENU_UPSTREAM_CENTERS, frame_CARDGIS::OnUpstreamCenters)

	// EVT_MENU (MENU_SAVE_PNG, frame_CARDGIS::OnSavePNG)
	EVT_MENU (MENU_SWAT_ANIMATION, frame_CARDGIS::OnSWATAnimation)
	EVT_MENU (MENU_SWAT_IMAGE, frame_CARDGIS::OnSWATImage)

	EVT_MENU (MENU_GRIDFLOAT, frame_CARDGIS::OnReadGridFloat)
	EVT_MENU (MENU_WRITE_UPSTREAM_DOWNSTREAM, frame_CARDGIS::OnPointsUpstreamDownstream)
	EVT_MENU (MENU_READ_TIGER, frame_CARDGIS::OnReadTIGER)

	// Treatment Facilities
	EVT_MENU (MENU_READ_POINT_LAYER, frame_CARDGIS::OnReadPointLayer)
	// EVT_MENU (MENU_READ_FACILITIES_OTHER, frame_CARDGIS::OnReadFacilitiesOther)
	EVT_MENU (MENU_LINK_POINT_LAYER, frame_CARDGIS::OnLinkPointLayer)
	EVT_MENU (MENU_LINK_POINT_LAYER_THREADED, frame_CARDGIS::OnLinkPointLayerThreaded)
	EVT_MENU (MENU_WRITE_LINKED_POINT_LAYER, frame_CARDGIS::OnWriteLinkedPointLayer)
	EVT_MENU (MENU_READ_LINKED_POINT_LAYER, frame_CARDGIS::OnReadLinkedPointLayer)

	EVT_BUTTON (BUTTON_ZOOM_IN, frame_CARDGIS::OnZoomIn)
	EVT_BUTTON (BUTTON_ZOOM_OUT, frame_CARDGIS::OnZoomOut)
	EVT_BUTTON (BUTTON_ZOOM_SELECTION, frame_CARDGIS::OnZoomSelection)
	EVT_BUTTON (BUTTON_SHOW_LEGEND, frame_CARDGIS::OnShowLegend)
	EVT_BUTTON (BUTTON_SELECT_ID, frame_CARDGIS::OnSelectID)
	EVT_MENU (MENU_SELECT_TEXT_ATTRIBUTE, frame_CARDGIS::OnMatchAttribute)
	EVT_MENU (MENU_MATCH_NAME, frame_CARDGIS::OnMatchName)
	EVT_BUTTON (BUTTON_SELECT_NAME, frame_CARDGIS::OnSelectName)
	EVT_MENU (MENU_DESCRIBE_FLOW, frame_CARDGIS::OnDescribeFlow)
	EVT_MENU (MENU_ROUTE_FACILITIES, frame_CARDGIS::OnRouteFacilities)
	EVT_MENU (MENU_ROUTE_FACILITIES_THREADED, frame_CARDGIS::OnRouteFacilitiesThreaded)
	EVT_MENU (MENU_ROUTE_FACILITIES_RIVER, frame_CARDGIS::OnRouteFacilitiesRiver)
	EVT_MENU (MENU_ROUTE_COLORADO, frame_CARDGIS::OnRouteColorado)
	EVT_MENU (MENU_ROUTE_MISSISSIPPI, frame_CARDGIS::OnRouteMississippi)
	EVT_MENU (MENU_WRITE_STRAHLER, frame_CARDGIS::OnWriteStrahler)
	EVT_MENU (MENU_WRITE_POINT_COMID, frame_CARDGIS::OnWritePointCOMID)
	EVT_MENU (MENU_WRITE_COLLADA, frame_CARDGIS::OnWriteCollada)
	EVT_MENU (MENU_WRITE_2DANIMATION, frame_CARDGIS::OnWrite2dAnimation)
	EVT_MENU (MENU_MERGE_POLYGONS, frame_CARDGIS::OnMergePolygons)
	EVT_MENU (MENU_READ_FLOW_NETWORK, frame_CARDGIS::OnReadFlowNetwork)
	EVT_MENU (MENU_WRITE_FLOW_NETWORK, frame_CARDGIS::OnWriteFlowNetwork)
	EVT_MENU (wxID_SHOW_SELECTION, frame_CARDGIS::OnShowSelection)
	EVT_MENU (wxID_SELECTION_CLEAR, frame_CARDGIS::OnSelectionClear)
	EVT_MENU (wxID_SHOW_RIVER_SEGMENT, frame_CARDGIS::OnShowRiverSegment)
	EVT_MENU (MENU_WRITE_SHAPEFILE_LAYER, frame_CARDGIS::OnWriteShapefileLayer)
	EVT_MENU (MENU_WRITE_SHAPEFILE_SELECTION, frame_CARDGIS::OnWriteShapefileSelection)
	EVT_MENU (MENU_BUFFER_FACILITY, frame_CARDGIS::OnBufferFacility)
	EVT_MENU(MENU_PROCESS_INTERSECT_TWO, frame_CARDGIS::OnIntersectTwoLayers)
	// EVT_MENU (MENU_INTERSECT, frame_CARDGIS::OnIntersectBufferCensus)

	// EVT_MENU (MENU_BUILD_BUFFER, frame_CARDGIS::OnBuildBuffer)
	EVT_MENU (MENU_BUILD_BUFFER_CLIPPER, frame_CARDGIS::OnBuildClipperBuffer)
	EVT_MENU (MENU_INTERSECT, frame_CARDGIS::OnIntersectBuffer)
	// EVT_MENU (MENU_INTERSECT_UPSTREAM, frame_CARDGIS::OnIntersectBufferUpstream)
	EVT_MENU (MENU_INTERSECT_CIRCLE, frame_CARDGIS::OnIntersectCircleCensus)
	EVT_MENU (MENU_INTERSECT_CIRCLE_ALL, frame_CARDGIS::OnIntersectCircleCensusAll)
	EVT_MENU (MENU_INTERSECT_ALL, frame_CARDGIS::OnIntersectBufferCensusAll)
	EVT_MENU (MENU_INTERSECT_BUFFER_GRID, frame_CARDGIS::OnIntersectBufferGrid)
	EVT_MENU (MENU_BISECT, frame_CARDGIS::OnBisectSegment)

	EVT_MENU(MENU_CLICK_DISTANCE, frame_CARDGIS::OnClickDistance)
	EVT_MENU(MENU_CHECK_DISTANCE, frame_CARDGIS::OnCheckDistance)
	EVT_COMBOBOX(COMBO_SELECT_LAYER, frame_CARDGIS::OnSelectLayer)
	EVT_CLOSE (frame_CARDGIS::OnClose)
	EVT_COMMAND (wxID_ANY, MESSAGE_PANEL_SHAPEFILE_SELECT, OnSelectMapObject)
	EVT_COMMAND (wxID_ANY, MESSAGE_PANEL_MEASURE_DISTANCE, OnPanelMeasureDistance)
	EVT_MENU (MENU_ADD_SELECTION_TO_LAYER, frame_CARDGIS::OnAddSelectionToLayer)
	EVT_MENU (MENU_ADD_NEW_LAYER, frame_CARDGIS::OnAddNewLayer)
	EVT_MENU (MENU_EDIT_LAYER_PROPERTIES, frame_CARDGIS::OnEditLayerProperties)
	EVT_BUTTON (MENU_EDIT_LAYER_PROPERTIES, frame_CARDGIS::OnEditLayerProperties)
	EVT_MENU (MENU_READ_LAYER_DATA, frame_CARDGIS::OnReadLayerData)
	EVT_MENU (MENU_EXPORT_LAYER_DATA, frame_CARDGIS::OnExportLayerData)
	EVT_MENU (MENU_READ_ZIPCODES, frame_CARDGIS::OnReadZipCodes)
	EVT_MENU (MENU_SSURGO_ZIPCODES, frame_CARDGIS::OnSSURGOZipCodes)
	EVT_MENU (MENU_HUC_12_UPSTREAM, frame_CARDGIS::OnHUC12Upstream)
	EVT_MENU (MENU_HUC_8_UPSTREAM, frame_CARDGIS::OnHUC8Upstream)
	EVT_MENU (MENU_ROUTING_12_8, frame_CARDGIS::RoutingHUC12to8)
	EVT_MENU (MENU_ROUTE_COUNTIES, frame_CARDGIS::OnRouteCounties)
	EVT_MENU (MENU_ROUTE_COUNTIES_RIVER, frame_CARDGIS::OnRouteCountiesRiver)
	EVT_MENU (MENU_COMBINE_ROUTED_COUNTIES, frame_CARDGIS::OnCombineRouteCounties)
	EVT_MENU (MENU_COUNTIES_UPSTREAM, frame_CARDGIS::OnListCountiesUpstream)
	EVT_MENU (MENU_COMBINE_COUNTIES_UPSTREAM, frame_CARDGIS::OnCombineCountiesUpstream)
	EVT_MENU (MENU_COUNTIES_UPSTREAM_STRAHLER, frame_CARDGIS::OnCountiesUpstreamStrahler)
	EVT_MENU (MENU_FISH_ADVISERIES, frame_CARDGIS::OnMapFishAdviseries)
	EVT_MENU (MENU_EXPORT_PNG, frame_CARDGIS::OnExportPNG)
	EVT_MENU (MENU_ZILLOW_NHD, frame_CARDGIS::OnZillowNHD)
	EVT_MENU (MENU_ZILLOW_ECOLI, frame_CARDGIS::OnZillowEcoli)
	EVT_MENU (MENU_ZILLOW_ECOLI_5MILE, frame_CARDGIS::OnZillowEcoli5Mile)
	EVT_MENU (MENU_ZILLOW_1000M, frame_CARDGIS::OnZillow1000m)
	EVT_MENU (MENU_ECOLI_NHD, frame_CARDGIS::OnEColiNHD)

	EVT_MENU (MENU_IMPORT_BHO, frame_CARDGIS::OnImportBHO)
	EVT_MENU (MENU_CONVERT_MULTIPOINT, frame_CARDGIS::OnConvertMultipoint)
	EVT_MENU (MENU_FARMLANDFINDER_CIRCLES, frame_CARDGIS::OnFarmlandFinderFieldCircles)
	EVT_MENU(MENU_FARMLANDFINDER_SSURGO, frame_CARDGIS::OnFarmlandFinderSSURGO)
	EVT_MENU(MENU_FARMLANDFINDER_SSURGO_WEIGHTED, frame_CARDGIS::OnFarmlandFinderSSURGOWeighted)
	EVT_MENU (MENU_FARMLANDFINDER_PROXIMITY, frame_CARDGIS::OnFarmlandFinderProximity)
	EVT_MENU (MENU_FARMLANDFINDER_POPULATION, frame_CARDGIS::OnFarmlandFinderPopulation)
	EVT_MENU(MENU_FARMLANDFINDER_POWERLINES, frame_CARDGIS::OnFarmlandFinderPowerlines)
	EVT_MENU(MENU_GSSURGO_SSURGO, frame_CARDGIS::OnGSSURGO_SSURGO)
	EVT_MENU(MENU_POINT_POLYGON, frame_CARDGIS::OnPointPolygonProximity)
	EVT_MENU(MENU_ZILLOW_POWERLINE_TABLE, frame_CARDGIS::OnZillowPowerlineTable)
	EVT_MENU(MENU_MATCH_AFIDA, frame_CARDGIS::OnMatchAFIDA)
	EVT_MENU(MENU_MATCH_AFIDA_KANSAS, frame_CARDGIS::OnMatch_AFIDA_Kansas)
	EVT_MENU (MENU_CROSSLAYER_SELECT, frame_CARDGIS::OnShowSelectedItems)
	EVT_MENU (MENU_LAGOS_NHD, frame_CARDGIS::On_LAGOS_NHD)
	EVT_MENU (MENU_LAGOS_ZILLOW, frame_CARDGIS::On_LAGOS_Zillow)
	EVT_MENU (MENU_PIPELINE_ZILLOW, frame_CARDGIS::On_Pipeline_Zillow)
	EVT_MENU (MENU_PIPELINE_ZILLOW_10MI, frame_CARDGIS::On_Pipeline_Zillow_Within_Radius)
	EVT_MENU (MENU_ZILLOW_SCHOOLS_HOSPITALS, frame_CARDGIS::On_Zillow_Schools_Hospitals)
	EVT_MENU (MENU_LISTNEARBYLAKES, frame_CARDGIS::On_ListNearbyLakes)
	EVT_MENU (MENU_OUTLINEMATCHINGWIND, frame_CARDGIS::On_OutlineMatchingWind)
	EVT_MENU (MENU_BUFFERPOLYGONLAYERS, frame_CARDGIS::On_BufferPolygonLayers)
	EVT_MENU (MENU_WINDDISTANCES, frame_CARDGIS::On_WindDistances)
	EVT_MENU (MENU_INTERSECTWINDLAYERS, frame_CARDGIS::On_IntersectWindLayers)
	EVT_MENU (MENU_BHO_RESERVOIRS, frame_CARDGIS::OnBHOReservoirs)
	EVT_MENU (MENU_BHO_RESERVOIR_MUNICIPALITY, frame_CARDGIS::OnBHOReservoirMunicipality)
	EVT_MENU (MENU_UMRB_HUCS, frame_CARDGIS::On_MatchUMRBHUCs)
	EVT_BUTTON (BUTTON_LAYER_TABLE, frame_CARDGIS::OnLayerObjectTable)
	EVT_MENU(MENU_SCAN_SWNET, frame_CARDGIS::OnScanSWNET)
	EVT_MENU(MENU_LIST_DOWN_FROM_COMIDS, frame_CARDGIS::OnListDownstreamFromCOMIDs)
	EVT_MENU(MENU_WRITE_FROMTO, frame_CARDGIS::OnWriteCOMIDFromTo)
	EVT_MENU(IMPORT_NHDPLUS_HR_GEOMETRY, frame_CARDGIS::OnImport_NHDPlus_HR_geometry)
	EVT_MENU(IMPORT_NHDPLUS_HR_NETWORK, frame_CARDGIS::OnImport_NHDPlus_HR_network)
	EVT_MENU(EDIT_DELETE_SUBPOLYGON, frame_CARDGIS::OnDeleteSubPolygon)
	EVT_MENU(MENU_IOWA_CATCHMENTS, frame_CARDGIS::OnReadIowaCatchments)
	EVT_BUTTON(BUTTON_CURRENT_PROJECT, frame_CARDGIS::OnPointPolygonProximity)
END_EVENT_TABLE()

frame_CARDGIS::frame_CARDGIS
	(const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	app_CARDGIS *p_app)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	app = p_app;
	if (app->use_gl)
		map_watershed = new dynamic_map_gl2;
	else
		map_watershed = new dynamic_map_wx;
	recent_dbs = new RecentlyOpenedDatabases;
	recent_shapefiles = new RecentlyOpenedShapefiles;
	recent_shapefiles->load_configuration (app->AppName);
	filename_nhd_area = app->filename_nhd_root;
	filename_nhd_area += "NHDPlus01\\";
	area_utm_zone = 0;
	area_utm_letter = ' ';

	// dynamic_string t;
	// t.test ();
	// dialog_error_list de (this, L"dynamic_string test", L"", t);
	// de.ShowWindowModal ();

	// no distinction between facility/station point layers; just river layer and multiple point layers
	// layer_facilities = layer_sampling_stations = layer_storet_modern = layer_storet_legacy = NULL;
	// layer_rivers = NULL;
	router_v2 = new router_NHD;
	router_hr = NULL;
	// attribute_color_sets_river = NULL;

	create_menus ();

	wxBoxSizer *column;
	wxSize frame_size;

	frame_size = GetSize ();

	// 1st Column: Legend & controls & shapefile display
	column = new wxBoxSizer (wxVERTICAL);

	column->AddSpacer (10);
	// Legend
	// wxStaticBoxSizer *stack_legend = new wxStaticBoxSizer (wxVERTICAL, this, "NHD Shapefile");

	wxBoxSizer * row = new wxBoxSizer (wxHORIZONTAL);
	row->Add (new wxButton (this, BUTTON_ZOOM_IN, "Zoom In", wxDefaultPosition, wxDefaultSize));
	row->AddSpacer (25);
	row->Add (new wxButton (this, BUTTON_ZOOM_OUT, "Zoom Out", wxDefaultPosition, wxDefaultSize));
	row->AddSpacer (25);
	row->Add (new wxButton (this, BUTTON_ZOOM_SELECTION, "Zoom Selection", wxDefaultPosition, wxDefaultSize));
	/*
	row->AddSpacer (25);
	row->Add (new wxStaticText (this, wxID_ANY, L"ID:", wxDefaultPosition, wxDefaultSize));
	row->AddSpacer (5);
	static_id = new wxStaticText (this, wxID_ANY, "000000", wxDefaultPosition, wxDefaultSize);
	row->Add (static_id);
	row->AddSpacer (10);
	row->Add (new wxStaticText (this, wxID_ANY, L"Name:", wxDefaultPosition, wxDefaultSize));
	row->AddSpacer (5);
	static_name = new wxStaticText (this, wxID_ANY, "          ", wxDefaultPosition, wxDefaultSize);
	row->Add (static_name);
	*/

	row->AddSpacer (25);

	edit_select_id = new long_long_integer_edit (this, wxID_ANY, L"");
	row->Add (edit_select_id);
	row->AddSpacer (5);
	button_select_id = new wxButton (this, BUTTON_SELECT_ID, "Select ID");
	row->Add (button_select_id);
	row->AddSpacer (5);
	button_select_name = new wxButton (this, BUTTON_SELECT_NAME, "Select Name");
	row->Add (button_select_name);

	row->AddSpacer (5);
	row->Add (new wxStaticText (this, -1, L"Select Layer:"));
	row->AddSpacer (10);
	combo_select_layer = new wxComboBox (this, COMBO_SELECT_LAYER, wxEmptyString, wxDefaultPosition, wxSize (350, 24), 0, 0, wxCB_READONLY);
	row->Add (combo_select_layer);
	row->AddSpacer (5);
	row->Add (new wxButton (this, MENU_EDIT_LAYER_PROPERTIES, "Edit"));

	row->AddSpacer (25);

	row->Add (new wxButton (this, BUTTON_SHOW_LEGEND, "Legend", wxDefaultPosition, wxDefaultSize));

	row->AddSpacer (10);
	row->Add (new wxButton (this, BUTTON_CURRENT_PROJECT, "Project", wxDefaultPosition, wxDefaultSize)); // whatever's current

	row->AddSpacer (10);
	row->Add (new wxButton (this, BUTTON_LAYER_TABLE, "Table", wxDefaultPosition, wxDefaultSize)); // view ledger of layer objects

	column->Add (row);
	column->AddSpacer (10);

    CreateStatusBar();

	// status_window = new interface_window_wx;
	// status_window->setup_wx (this);

	panel_watershed = new panel_shapefile (this, map_watershed, GetStatusBar (), wxSize (frame_size.x, frame_size.y));
	if (app->use_gl)
		panel_watershed->use_gl = true;
	column->Add (panel_watershed, 0, wxEXPAND);

    SetSizerAndFit (column);
//    row->SetSizeHints( this );

	// create_color_sets ();

	// create_map_layers (map_watershed);

	// Initialize elevation range here so that if more than one area is loaded, these values will be adjusted correctly by router_NHD.read_NED_elevation
	area_elevation_minimum_m = DBL_MAX;
	area_elevation_maximum_m = DBL_MIN;

	enable_map_controls ();

	db_odbc = NULL;

	/*
	dynamic_string log;
	// river.test_divergence (log);
	logical_coordinate base, target, up_vector;
	double angle_radians [3];

	base.x = 94061700.0951;
	base.y = 18235895.1605;
	base.z = 0.0;

	target.x = target.y = target.z = 0.0;

	up_vector.x = 0;
	up_vector.y = 1.0;
	up_vector.z = 0;

	set_angles_camera (base, target, up_vector, angle_radians, true, log);
	log.add_formatted ("Angle\t%.4lf\t%.4lf\t%.4lf\n", angle_radians [0] * RADIANS_TO_DEGREES, angle_radians [1] * RADIANS_TO_DEGREES, angle_radians [2] * RADIANS_TO_DEGREES);

	base.x = 93680118.6495;
	base.y = 18362810.0697;
	base.z = -1635192.5535;

	log += "\n";

	set_angles_camera (base, target, up_vector, angle_radians, true, log);
	log.add_formatted ("Angle\t%.4lf\t%.4lf\t%.4lf\n", angle_radians [0] * RADIANS_TO_DEGREES, angle_radians [1] * RADIANS_TO_DEGREES, angle_radians [2] * RADIANS_TO_DEGREES);

	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
	*/
}

frame_CARDGIS::~frame_CARDGIS ()

{
    // delete wxLog::SetActiveTarget(m_logOld);

	if (router_v2)
		delete router_v2;
	if (router_hr)
		delete router_hr;

	if (map_watershed)
		delete map_watershed;

	if (db_odbc) {
		db_odbc->close ();
		delete db_odbc;
	}
	// control.clear ();

	if (recent_shapefiles) {
		recent_shapefiles->save_configuration (app->AppName);
		delete recent_shapefiles;
	}
	if (recent_dbs) {
		recent_dbs->save_configuration (app->AppName);
		delete recent_dbs;
	}
	/*
	if (attribute_color_sets_river)
		delete [] attribute_color_sets_river;
		*/

}

/*
void frame_CARDGIS::create_color_sets ()

{
	attribute_color_sets_river = new color_set [NHD_SEGMENT_ATTRIBUTE_COUNT];

	// Ftype
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE].add_color (RGB (255, 0, 255), (double) NHD_FTYPE_ARTIFICIALPATH, 2.0, NHD_FTYPE_name (NHD_FTYPE_ARTIFICIALPATH).get_text_ascii ()); // Magenta
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE].add_color (RGB (0, 0, 255), (double) NHD_FTYPE_STREAMRIVER, 3.0, NHD_FTYPE_name (NHD_FTYPE_STREAMRIVER).get_text_ascii ()); // Blue
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE].add_color (RGB (0, 255, 0), (double) NHD_FTYPE_COASTLINE, 4.0, NHD_FTYPE_name (NHD_FTYPE_COASTLINE).get_text_ascii ()); // Green
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE].add_color (RGB (112, 146, 190), (double) NHD_FTYPE_CONNECTOR, 5.0, NHD_FTYPE_name (NHD_FTYPE_CONNECTOR).get_text_ascii ()); // Navy blue
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE].add_color (RGB (127, 127, 127), (double) NHD_FTYPE_CANALDITCH, 6.0, NHD_FTYPE_name (NHD_FTYPE_CANALDITCH).get_text_ascii ()); // gray
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_FTYPE].add_color (RGB (0, 255, 255), (double) NHD_FTYPE_PIPELINE, 7.0, NHD_FTYPE_name (NHD_FTYPE_PIPELINE).get_text_ascii ()); // cyan

	// Grayscale for length
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM].add_color (RGB (224, 224, 224), 0.0, 0.5);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM].add_color (RGB (192, 192, 192), 0.5, 1.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM].add_color (RGB (160, 160, 160), 1.0, 2.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM].add_color (RGB (128, 128, 128), 2.0, 3.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM].add_color (RGB (96, 96, 96), 3.0, 4.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM].add_color (RGB (64, 64, 64), 4.0, 5.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LENGTHKM].add_color (RGB (32, 32, 32), 5.0, 1.0e6);

	// Divergence
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE].add_color (RGB (0, 255, 0), 0.0, 1.0, "None"); // Green
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE].add_color (RGB (0, 0, 255), 1.0, 2.0, "Main"); // Blue
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE].add_color (RGB (255, 0, 0), 2.0, 3.0, "Not-Main"); // red

	// Level Path ID
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (53, 94, 0), 0.0, 1.0); // Green darkest
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (0, 255, 0), 1.0, 2.0); // Green 2
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (148, 189, 94), 2.0, 3.0); // Green 3
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (51, 204, 102), 3.0, 4.0); // Green 4
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (0, 255, 0), 4.0, 5.0); // Green lightest

	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (76, 25, 0), 5.0, 6.0); // Yellow scale
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (153, 102, 51), 6.0, 7.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (179, 179, 0), 7.0, 8.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (230, 230, 76), 8.0, 9.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (255, 255, 0), 9.0, 10.0);

	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (40, 0, 153), 10.0, 11.0); // Blue to cyan
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (35, 0, 220), 11.0, 12.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (0, 71, 255), 12.0, 13.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (0, 0, 255), 13.0, 14.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (0, 184, 255), 14.0, 15.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH].add_color (RGB (0, 255, 255), 15.0, 16.0);

	// Stream Level
	// attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (192, 192, 192), 0.0, 1.0); // Gray
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (53, 94, 0), 0.0, 1.0); // Green darkest
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (0, 255, 0), 1.0, 2.0); // Green 2
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (148, 189, 94), 2.0, 3.0); // Green 3
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (51, 204, 102), 3.0, 4.0); // Green 4
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (0, 255, 0), 4.0, 5.0); // Green lightest

	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (76, 25, 0), 5.0, 6.0); // Yellow scale
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (153, 102, 51), 6.0, 7.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (179, 179, 0), 7.0, 8.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (230, 230, 76), 8.0, 9.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (255, 255, 0), 9.0, 10.0);

	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (40, 0, 153), 10.0, 11.0); // Blue to cyan
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (35, 0, 220), 11.0, 12.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (0, 71, 255), 12.0, 13.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (0, 0, 255), 13.0, 14.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (0, 184, 255), 14.0, 15.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL].add_color (RGB (0, 255, 255), 15.0, 16.0);

	// Strahler order
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (192, 192, 192), 0.0, 1.0); // 0 value gray
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (76, 25, 0), 1.0, 2.0); // Yellow scale
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (153, 102, 51), 2.0, 3.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (179, 179, 0), 3.0, 4.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (230, 230, 76), 4.0, 5.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (255, 255, 0), 5.0, 6.0);

	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (40, 0, 153), 6.0, 7.0); // Blue to cyan
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (0, 71, 255), 7.0, 8.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (0, 0, 255), 8.0, 9.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (0, 184, 255), 9.0, 10.0);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER].add_color (RGB (0, 255, 255), 10.0, 11.0);

	// UpHydroSeq
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_UPHYDROSEQ].add_color (RGB (255, 0, 255), 0.0, 1.0); // Magenta
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_UPHYDROSEQ].add_color (RGB (0, 0, 255), 1.0, 2.0); // Blue
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_UPHYDROSEQ].add_color (RGB (0, 255, 0), 2.0, 3.0); // Green
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_UPHYDROSEQ].add_color (RGB (112, 146, 190), 3.0, 4.0); // Navy blue
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_UPHYDROSEQ].add_color (RGB (127, 127, 127), 4.0, 5.0); // gray
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_UPHYDROSEQ].add_color (RGB (0, 255, 255), 5.0, 6.0); // cyan

	// DnHydroSeq
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ].add_color (RGB (255, 0, 255), 0.0, 1.0); // Magenta
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ].add_color (RGB (0, 0, 255), 1.0, 2.0); // Blue
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ].add_color (RGB (0, 255, 0), 2.0, 3.0); // Green
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ].add_color (RGB (112, 146, 190), 3.0, 4.0); // Navy blue
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ].add_color (RGB (127, 127, 127), 4.0, 5.0); // gray
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_DNHYDROSEQ].add_color (RGB (0, 255, 255), 5.0, 6.0); // cyan

	// Elevation Min, 16 grayscale divisions
	for (int index = 0; index < 15; ++index)
		attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_MINELEVM].add_color (RGB (index * 16, index * 16, index * 16), index);
	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_MINELEVM].add_color (RGB (255, 255, 255), 15);

	attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_MAXELEVM].copy (&attribute_color_sets_river [NHD_SEGMENT_ATTRIBUTE_INDEX_MINELEVM]);
}
*/

void frame_CARDGIS::enable_map_controls ()

{
	if (map_watershed->layers.size () > 0)
		combo_select_layer->Enable (true);
	else
		combo_select_layer->Enable (false);

	// fill combo's layers
	int index;

	std::vector <map_layer*>::const_iterator layer;

	combo_select_layer->Clear();
	for (layer = map_watershed->layers.begin();
	layer != map_watershed->layers.end();
	++layer) {
		if ((*layer)->name != "Selection") {
			index = combo_select_layer->Append ((*layer)->name.get_text_ascii (), (void *) *layer);
			combo_select_layer->SetClientData (index, *layer);
			if ((*layer == panel_watershed->layer_selection_source))
				combo_select_layer->SetSelection (index);
		}
	}
}

void frame_CARDGIS::create_menus ()

{
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	fileMenu->Append(wxID_OPEN_TIFF, "Open &TIFF");

    wxMenu * const menuAreaPopup = new wxMenu;

    menuAreaPopup->Append (MENU_AREA_1, "0&1 New England");
    menuAreaPopup->Append (MENU_AREA_2, "0&2 Chesapeake");
    menuAreaPopup->Append (MENU_AREA_3N, "03N Carolinas");
    menuAreaPopup->Append (MENU_AREA_3S, "03S Florida");
    menuAreaPopup->Append (MENU_AREA_3W, "03W Alabama");
	menuAreaPopup->Append (MENU_AREA_4, "0&4 Michigan");
	menuAreaPopup->Append (MENU_AREA_5, "0&5 Ohio");
	menuAreaPopup->Append (MENU_AREA_6, "0&6 Tennessee");
	menuAreaPopup->Append (MENU_AREA_7, "0&7 UMRB");
	menuAreaPopup->Append (MENU_AREA_8, "0&8 Lower Mississippi");
	menuAreaPopup->Append (MENU_AREA_9, "0&9 North Dakota");
	menuAreaPopup->Append (MENU_AREA_10L, "10&L Missouri Lower");
	menuAreaPopup->Append (MENU_AREA_10U, "10&U Missouri Upper");
	menuAreaPopup->Append (MENU_AREA_11, "11 Arkansas River");
	menuAreaPopup->Append (MENU_AREA_12, "12 Texas Gulf Coast");
	menuAreaPopup->Append (MENU_AREA_13, "13 Texas SW");
	menuAreaPopup->Append (MENU_AREA_14, "14 Colorado Upper");
	menuAreaPopup->Append (MENU_AREA_15, "15 Colorado Lower");
	menuAreaPopup->Append (MENU_AREA_16, "16 Nevada-Utah");
	menuAreaPopup->Append (MENU_AREA_17, "17 Washington");
	menuAreaPopup->Append (MENU_AREA_18, "18W California");

    fileMenu->AppendSubMenu(menuAreaPopup, "Read NHDPlusV2 &Area Files");

	// if (recent_shapefiles)
	//	recent_shapefiles->setup_menu (fileMenu, ID_FILE_RECENT_SHAPEFILE);

    wxMenu * const menuRiverPopup = new wxMenu;
    menuRiverPopup->Append (MENU_COLORADO, "0&1 Colorado");
    menuRiverPopup->Append (MENU_MISSISSIPPI, "0&2 Mississippi");
    fileMenu->AppendSubMenu(menuRiverPopup, "Read NHDPlusV2 &River Files");

	fileMenu->AppendSeparator();

	wxMenu* const menuAreaHRPopup = new wxMenu;

	menuAreaHRPopup->Append(MENU_AREA_HR_1, "0&1 New England");
	menuAreaHRPopup->Append(MENU_AREA_HR_7, "0&7 UMRB");

	fileMenu->AppendSubMenu(menuAreaHRPopup, "Read NHDPlus &HR Area Files");


	fileMenu->AppendSeparator();
	fileMenu->Append(MENU_READ_POINT_LAYER, wxT("Read Points"), wxT("Read facility or sampling station location list"));
	fileMenu->Append(MENU_READ_TIGER, wxT("Read TIGER streets"), wxT("Read TIGER streets for specified area"));
    fileMenu->Append(MENU_READ_ZIPCODES, wxT("Read Zipcodes"), wxT("Read Zipcode shapefile"));
	fileMenu->AppendSeparator();
    fileMenu->Append(wxID_CONFIGURE, wxT("&Configure"), wxT("Set paths"));
    fileMenu->AppendSeparator();

	{
		// Popup menu for projects
		wxMenu * const menuPopupProject = new wxMenu;

		menuPopupProject->Append (MENU_GRAINPRICE_MATCH, wxT ("G&rain Prices"), wxT ("Match warehouses to townships"));
		menuPopupProject->Append (MENU_SWAT_ANIMATION, wxT ("S&WAT Animation"), wxT ("Animation SWAT output data"));
		menuPopupProject->Append (MENU_SWAT_IMAGE, wxT ("SWAT &Image"), wxT ("Annual average summary image of SWAT output data"));
		menuPopupProject->Append (MENU_IOWA_LAKES, wxT ("&Iowa Lakes"), wxT ("Read Iowa Lake Positions"));
		menuPopupProject->Append (MENU_E85, wxT ("E&85"), wxT ("Read E85 stations & prices"));
		menuPopupProject->Append (MENU_LISTNEARBYLAKES, wxT ("List Lakes"), wxT ("List 10 lakes nearest each address"));
		menuPopupProject->Append (MENU_OUTLINEMATCHINGWIND, wxT ("Wind Class Matching"), wxT ("Create polygons of consistent wind classes"));
		menuPopupProject->Append (MENU_BUFFERPOLYGONLAYERS, wxT ("Buffer Polygon Layers"), wxT ("Create polygons 5 and 10 miles in and out from layers"));
		menuPopupProject->Append (MENU_WINDDISTANCES, wxT ("Wind Distances"), wxT ("Compute distances from point to wind polygons"));
		menuPopupProject->Append (MENU_INTERSECTWINDLAYERS, wxT ("Intersect Wind Layers"), wxT ("Intersect wind layers"));
		menuPopupProject->Append(MENU_UMRB_HUCS, wxT("Match UMRB HUCs"), wxT("Match SWAT UMRB polygons with HUC12 polygons"));


		// Popup for Erie-related work
		wxMenu * const eriePopup = new wxMenu;
		eriePopup->Append (MENU_ERIE, wxT ("&Read shapefiles"), wxT ("Read Lake Erie contour & grid"));
		eriePopup->Append (MENU_ASSIGN_ERIE_GRID, wxT ("Assign Erie Grid"), wxT ("Assign Grid IDs to points in Lake Erie"));
		eriePopup->Append (MENU_ERIE_MARINA_DISTANCE, wxT ("Lake Erie &Marina Distance"), wxT ("Find distance from points to associated grid centers"));
		eriePopup->Append (MENU_ERIE_REPORT, wxT ("Report &1"), wxT ("Table of survey counts by grid code"));
		eriePopup->Append (MENU_ERIE_REPORT_2, wxT ("Report &2"), wxT ("Table of nearest relevent travel distances"));

		menuPopupProject->AppendSubMenu (eriePopup, "&Erie");

		menuPopupProject->Append (MENU_LAKE_CENTERS, wxT ("&Lake Centers"), wxT ("Create new layer with centerpoints of polygons"));
		menuPopupProject->Append (MENU_UPSTREAM_CENTERS, wxT ("&Upstream Centers"), wxT ("Create table of distances from points to upstream COMIDs"));

	    menuPopupProject->Append(MENU_SSURGO_ZIPCODES, wxT("SSURGO Zipcode overlap"), wxT("Read SSURGO and Zipcode shapefiles, overlap"));
		menuPopupProject->Append(MENU_ROUTING_12_8, wxT("HUC 12 to 8"), wxT("Read HUC12 from/to file and write HUC8 from/to file"));
	    menuPopupProject->Append(MENU_HUC_12_UPSTREAM, wxT("HUC 12 Routing"), wxT("Read HUC12 from/to file and write upstream table"));
	    menuPopupProject->Append(MENU_HUC_8_UPSTREAM, wxT("HUC 8 Routing"), wxT("Read HUC8 from/to file and write upstream table"));
		menuPopupProject->Append(MENU_ROUTE_COUNTIES, wxT("Route Counties"), wxT("Route from county centers to outlet within area"));
		menuPopupProject->Append(MENU_ROUTE_COUNTIES_RIVER, wxT("Route Counties for River"), wxT("Route from county centers to river outlet"));
		menuPopupProject->Append(MENU_COMBINE_ROUTED_COUNTIES, wxT("Combine Routed Counties"), wxT("Combine tables of routed counties from different areas"));
		menuPopupProject->Append(MENU_COUNTIES_UPSTREAM, wxT("List Counties Upstream"), wxT("Find counties upstream via river nearest county center."));
		menuPopupProject->Append(MENU_COUNTIES_UPSTREAM_STRAHLER, wxT("List Counties Upstream by Strahler"), wxT("Find counties upstream with Strahler Order threshold."));
		menuPopupProject->Append(MENU_COMBINE_COUNTIES_UPSTREAM, wxT("Combine Counties Upstream"), wxT("Combine tables of upstream counties from different areas"));
		menuPopupProject->Append(MENU_FISH_ADVISERIES, wxT("Map fish adviseries"), wxT("Map fish adviseries"));
		menuPopupProject->Append(MENU_IOWA_CATCHMENTS, wxT("Map Iowa Catchments"), wxT("List COMIDS upstream from Iowa Catchments"));

	
	    wxMenu * const menuPopup_BHO = new wxMenu;
		menuPopupProject->Append (MENU_IMPORT_BHO, wxT ("Import BHO"), wxT ("Read BHO Area and build network"));
		// menuPopupProject->Append (MENU_OPEN_BHO, wxT ("BHO"), wxT ("Open BHO Area"));
		menuPopup_BHO->Append (MENU_BHO_1, "0&1 Costa Atlantica Norte");
		menuPopup_BHO->Append (MENU_BHO_2, "0&2 Bacia Amazonica");
		menuPopup_BHO->Append (MENU_BHO_3, "0&3 Costa Atlantica da Ilha do Marajo");
		menuPopup_BHO->Append (MENU_BHO_4, "0&4 Bacia do Tocantins – Araguaia");
		menuPopup_BHO->Append (MENU_BHO_5, "0&5 Costa Atlantica Nordeste");
		menuPopup_BHO->Append (MENU_BHO_6, "0&6 Bacia do Sao Francisco");
		menuPopup_BHO->Append (MENU_BHO_7, "0&7 Costa Atlantica Leste");
		menuPopup_BHO->Append (MENU_BHO_8, "0&8 Bacia do Prata");
		menuPopup_BHO->Append (MENU_BHO_9, "0&9 Costa Atlantica Sul e Costa do Pacífico");
		menuPopup_BHO->Append (MENU_BHO_RESERVOIRS, "Link Reservoirs to sampling stations");
		menuPopup_BHO->Append (MENU_BHO_RESERVOIR_MUNICIPALITY, "Link Municipalities to Reservoirs or other");	
		menuPopupProject->AppendSubMenu (menuPopup_BHO, "&BHO");

	    wxMenu * const menuPopup_Zillow = new wxMenu;
		menuPopup_Zillow->Append(MENU_ZILLOW_NHD, wxT("&Sales to NHD"), wxT("Map time-series of Zillow Sales Data to NHD Water Bodies"));
		menuPopup_Zillow->Append(MENU_ZILLOW_ECOLI, wxT("Sales to &EColi"), wxT("Link home sales to e-Coli outbreaks"));
		menuPopup_Zillow->Append(MENU_ZILLOW_ECOLI_5MILE, wxT("EColi &5-Mile"), wxT("List home sales within 5 miles of e-Coli outbreaks"));
		menuPopup_Zillow->Append(MENU_ECOLI_NHD, wxT("EColi to NHD"), wxT("List nearest water bodies to each outbreak"));
		menuPopup_Zillow->Append(MENU_ZILLOW_1000M, wxT("Sales 1000m"), wxT("List outbreak within 1000m of each sale"));
		menuPopupProject->AppendSubMenu (menuPopup_Zillow, "&Zillow");

	    wxMenu * const menuPopup_FarmlandFinder = new wxMenu;
		menuPopup_FarmlandFinder->Append (MENU_FARMLANDFINDER_CIRCLES, wxT ("FarmlandFinder Field Circles"), wxT ("Create a shapefile of circles for sales data from FarmlandFinder"));
		menuPopup_FarmlandFinder->Append (MENU_FARMLANDFINDER_SSURGO, wxT ("FarmlandFinder SSURGO"), wxT ("Find dominant SSURGO soils for each farm sale polygon"));
		menuPopup_FarmlandFinder->Append(MENU_FARMLANDFINDER_SSURGO_WEIGHTED, wxT("FarmlandFinder SSURGO Weighted"), wxT("Area-weighted SSURGO characterstics for farm sale polygons"));
		menuPopup_FarmlandFinder->Append (MENU_FARMLANDFINDER_PROXIMITY, wxT ("FarmlandFinder Proximity"), wxT ("Produce table of closest roads, water, features"));
		menuPopup_FarmlandFinder->Append (MENU_FARMLANDFINDER_POPULATION, wxT ("FarmlandFinder Population"), wxT ("Produce table of populations within 5 and 10 miles"));
		menuPopup_FarmlandFinder->Append (MENU_FARMLANDFINDER_POWERLINES, wxT ("FarmlandFinder Powerlines"), wxT ("Produce table of powerlines overlapping farmland"));
		menuPopup_FarmlandFinder->Append (MENU_ZILLOW_POWERLINE_TABLE, wxT("Zillow Powerline Table"), wxT("Produce table of powerlines within 10 miles of Zillow sales"));
		menuPopup_FarmlandFinder->Append(MENU_GSSURGO_SSURGO, wxT("GSSURGO-SSURGO overlay"), wxT("Produce table of GSSURGO HEL values by area"));
		menuPopupProject->AppendSubMenu (menuPopup_FarmlandFinder, "&FarmlandFinder");

		menuPopupProject->Append (MENU_LAGOS_NHD, wxT ("LAGOS NHD Water Bodies"), wxT ("Attach LAGOS identifiers to NHD Water Bodies"));
		menuPopupProject->Append (MENU_LAGOS_ZILLOW, wxT ("LAGOS Zillow"), wxT ("Attach LAGOS chla data to Zillow sales"));
		menuPopupProject->Append (MENU_PIPELINE_ZILLOW, wxT ("Pipeline Accidents to Zillow"), wxT ("Attach pipeline accident data to Zillow sales"));
		menuPopupProject->Append (MENU_PIPELINE_ZILLOW_10MI, wxT ("Pipeline Accidents within (8mi) of Zillow"), wxT ("Attach pipeline accident data to Zillow sales within radius (8mi)."));
		menuPopupProject->Append (MENU_ZILLOW_SCHOOLS_HOSPITALS, wxT ("Proximity of Zillow Sales to Schools & Hospitals"), wxT ("Proximity tables for Zillow sales to public schools, universities, and hospitals"));
		menuPopupProject->Append(MENU_MATCH_AFIDA, wxT("Match AFIDA"), wxT("Match AFIDA records to more detailed land sales records"));
		menuPopupProject->Append(MENU_MATCH_AFIDA_KANSAS, wxT("Match AFIDA Kansas"), wxT("Match AFIDA records to Kansas sales records"));
		menuPopupProject->Append(MENU_SCAN_SWNET, wxT("Scan SWNET GeoTIFFs"), wxT("Create a list of GeoTIFF locations from SWNET"));

		menuPopupProject->Append(MENU_LIST_DOWN_FROM_COMIDS, wxT("Downstream COMID List"), wxT("Create a list of all COMIDs downstream from a list of COMIDs"));
		menuPopupProject->Append(MENU_WRITE_FROMTO, wxT("COMID From-To List"), wxT("Create a file of all COMIDs and the next downstream COMID"));
		menuPopupProject->Append(IMPORT_NHDPLUS_HR_GEOMETRY, wxT("Import router_NHDPlus_HR Geometry"), wxT("Import data exported via Python from router_NHDPlus_HR"));
		menuPopupProject->Append(IMPORT_NHDPLUS_HR_NETWORK, wxT("Import router_NHDPlus_HR Network"), wxT("Import data exported via Python from router_NHDPlus_HR"));
		menuPopupProject->Append(MENU_POINT_POLYGON, wxT("Proximity Points to Polygons"), wxT("Find Proximity of Points to Polygon layer"));

		fileMenu->AppendSubMenu (menuPopupProject, "&Projects");
	}

	fileMenu->Append(MENU_GRIDFLOAT, wxT("&Grid Float"), wxT("Read 3DEM elevation file"));
	fileMenu->Append(MENU_EXPORT_PNG, wxT("&Export to PNG"), wxT("Save current map as a PNG file"));
	fileMenu->Append (MENU_WRITE_SHAPEFILE_LAYER, wxT("&Write Shapefile of Layer"), wxT("Save current layer to shapefile"));
	fileMenu->Append (MENU_WRITE_SHAPEFILE_SELECTION, wxT("&Write Shapefile of Selection"), wxT("Save current selection to shapefile"));
	fileMenu->Append(MENU_EXPORT_LAYER_DATA, wxT("Export Layer Data"), wxT("Write layer data as a table"));
	// fileMenu->Append(MENU_EXPORT_PNG, wxT("Export PNG"), wxT("Save current map as a PNG file"));
    fileMenu->Append(wxID_EXIT, wxT("E&xit"), wxT("Exit program"));

    wxMenu *editMenu = new wxMenu;
    editMenu->Append(wxID_COPY);
    editMenu->Append(wxID_PASTE);
    editMenu->Append(WXID_REPORT_POLLUTANTS, wxT("&Pollutant Report"), wxT("Summarize N && P outputs"));
	editMenu->Append(wxID_SHOW_SELECTION, wxT("Show &Selection"), wxT("Detailed view of selected sections"));
	editMenu->Append(wxID_SELECTION_CLEAR, wxT("&Clear Selection"), wxT("Unselect all"));
	editMenu->Append(MENU_CLICK_DISTANCE, wxT("Click Distance"), wxT("Compute distance from current selection to click."));
	editMenu->Append(MENU_CHECK_DISTANCE, wxT("Compute Distance"), wxT("Compute distance between map objects."));
	editMenu->Append(wxID_SHOW_RIVER_SEGMENT, wxT("Show River &Segment"), wxT("View of segments from facility to USGS station"));
	editMenu->Append(MENU_SELECT_TEXT_ATTRIBUTE, wxT("Select by Attribute &Value"), wxT("Select map object by value of text or numeric attribute"));
	editMenu->Append(MENU_MATCH_NAME, wxT("Select by &name"), wxT("Select map object by name"));
	editMenu->Append(MENU_ADD_NEW_LAYER, wxT("&Add New Layer"), wxT("Add a new empty layer"));
	editMenu->Append(MENU_EDIT_LAYER_PROPERTIES, wxT("&Edit Layer Properties"), wxT("View & edit layer properties"));
	editMenu->Append(MENU_ADD_SELECTION_TO_LAYER, wxT("Add Selection &To Layer"), wxT("Add a copy of selected map objects to another layer"));
	editMenu->Append(MENU_READ_LAYER_DATA, wxT("Read Layer Data"), wxT("Add data to layer from a CSV file"));
	editMenu->Append(MENU_CONVERT_MULTIPOINT, wxT("Convert Multipoint Layer"), wxT("Create a point layer from a multipoint layer"));
	editMenu->Append(MENU_CROSSLAYER_SELECT, wxT("Select from Multiple Layers"), wxT("select items from multiple layers"));
	editMenu->Append(EDIT_DELETE_SUBPOLYGON, wxT("Delete sub-polygon"), wxT("Delete an inner polygon from selected complex polygon"));

	wxMenu *processMenu = new wxMenu;
	processMenu->Append(wxID_READ_FLOWLINEVAA, wxT("Read Flowline&VAA.dbf"), wxT("Get divergence codes for flow segments"));
	processMenu->Append(wxID_READ_ELEVATION, wxT("Read ElevSlope.dbf"), wxT("Get min and max elevation for flow segments"));
	processMenu->AppendSeparator ();
	processMenu->Append(wxID_READ_FLOW_NETWORK_NHD, wxT("Read NHD Network"), wxT("Read PlusFlow.dbf file describing flow network"));
	processMenu->Append(wxID_READ_FLOW_NETWORK_CHINA, wxT("Read China Rivers Network"), wxT("Read files describing flow network"));
	processMenu->Append(wxID_ROUTE_CHINA_RIVER, wxT("Route China River"), wxT("Route one station to one county on China River network"));
	processMenu->Append(wxID_ROUTE_ASTAR, wxT("Route A*"), wxT("Route one station to one county on River network with A* algorithm"));
	processMenu->Append(wxID_ROUTE_ALL_CHINA_RIVERS, wxT("Route China Rivers"), wxT("Route all stations to all counties on China River network"));
	processMenu->Append(MENU_WRITE_FLOW_NETWORK, wxT("&Write Flow Network"), wxT("Save completed flow network"));
	processMenu->Append(MENU_READ_FLOW_NETWORK, wxT("Read &Flow Network"), wxT("Read binary file version of flow network"));
	// processMenu->Append(MENU_READ_FACILITIES_OTHER, wxT("Read Facilities - Other"), wxT("Read treatment locations as a COMID list"));
	processMenu->AppendSeparator ();
	processMenu->Append(MENU_LINK_POINT_LAYER, wxT("Link Points"), wxT("Find COMID for each facility or sampling station"));
	processMenu->Append(MENU_LINK_POINT_LAYER_THREADED, wxT("Link Points (threaded)"), wxT("Find COMID for each facility or sampling station"));
	processMenu->Append(MENU_WRITE_LINKED_POINT_LAYER, wxT("&Write Linked Points"), wxT("Save linked facility or sampling station locations"));
	processMenu->Append(MENU_READ_LINKED_POINT_LAYER, wxT("Read Linked &Points"), wxT("Read linked facility or sampling stations"));
	processMenu->Append(MENU_PROCESS_INTERSECT_TWO, wxT("&Intersect Two Layers"), wxT("Create a layer which is the intersection of two polygon layers"));
	processMenu->AppendSeparator ();

    wxMenu * const menuPopupBuffer = new wxMenu;

	// menuPopupBuffer->Append (MENU_BUFFER_FACILITY, wxT("Buffer Facility"), wxT("Create buffer zones for one facility"));
	menuPopupBuffer->Append (MENU_INTERSECT, wxT("Buffer and &Intersect"), wxT("Create buffer zones for one facility and overlap with Census Tracts"));
	// menuPopupBuffer->Append (MENU_INTERSECT_UPSTREAM, wxT("Buffer Facility Upstream"), wxT("Create upstream buffer zones for one facility and overlap with Census Tracts"));
	//menuPopupBuffer->Append (MENU_INTERSECT_DOWNSTREAM, wxT("Buffer Facility Downstream"), wxT("Create downstream buffer zones for one facility and overlap with Census Tracts"));
	menuPopupBuffer->Append (MENU_INTERSECT_CIRCLE, wxT("Intersect &Circle"), wxT("Create circular zones for one facility and overlap with Census Tracts"));
	menuPopupBuffer->Append (MENU_INTERSECT_ALL, wxT("Buffer and Intersect &All"), wxT("Create buffer zones for all facilities and overlap with Census Tracts"));
	menuPopupBuffer->Append (MENU_INTERSECT_CIRCLE_ALL, wxT("Create Circles and Intersect All"), wxT("Create concentric circles around all facilities and overlap with Census Tracts"));
	menuPopupBuffer->Append (MENU_BISECT, wxT("&Bisect Segment"), wxT("Add a vector perpendicular to midpoint of river segment"));
	menuPopupBuffer->Append (MENU_INTERSECT_BUFFER_GRID, wxT("Buffer and &Grid"), wxT("Create buffer zones for facility and overlay with grid"));

    processMenu->AppendSubMenu (menuPopupBuffer, "&Buffer");

	processMenu->Append(MENU_WRITE_UPSTREAM_DOWNSTREAM, wxT("Write Upstream Downstream"), wxT("Write COMIDs upstream and downstream from points."));
	/*
	processMenu->AppendSeparator ();
	processMenu->Append(MENU_READ_SAMPLING_STATIONS, wxT("Read USGS Stations"), wxT("Read water quality sampling stations list"));
	processMenu->Append(MENU_LINK_SAMPLING_STATIONS, wxT("Link &USGS Stations"), wxT("Find COMID for each USGS station"));
	processMenu->Append(MENU_WRITE_LINKED_USGS, wxT("&Write Linked USGS"), wxT("Save linked USGS stations"));
	processMenu->Append(MENU_READ_LINKED_USGS, wxT("Read Linked &USGS"), wxT("Read USGS stations with link attributes"));
	processMenu->AppendSeparator ();
	processMenu->Append(MENU_READ_STORET_LEGACY, wxT("Read STORET Legacy Stations"), wxT("Read STORET legacy sampling stations list"));
	processMenu->Append(MENU_LINK_STORET_LEGACY, wxT("Link STORET Legacy Stations"), wxT("Find COMID for each STORET Legacy station"));
	processMenu->Append(MENU_WRITE_STORET_LEGACY_BINARY, wxT("&Write Linked STORET Legacy"), wxT("Save linked STORET Legacy stations"));
	processMenu->Append(MENU_READ_STORET_LEGACY_BINARY, wxT("&Read Linked STORET Legacy"), wxT("Read linked STORET Legacy stations"));

	processMenu->AppendSeparator ();
	processMenu->Append(MENU_READ_STORET_MODERN, wxT("Read STORET Modern Stations"), wxT("Read STORET modern sampling stations list"));
	processMenu->Append(MENU_LINK_STORET_MODERN, wxT("Link STORET Modern Stations"), wxT("Find COMID for each STORET Modern station"));
	processMenu->Append(MENU_WRITE_STORET_MODERN_BINARY, wxT("&Write Linked STORET Modern"), wxT("Save linked STORET Modern stations"));
	processMenu->Append(MENU_READ_STORET_MODERN_BINARY, wxT("&Read Linked STORET Modern"), wxT("Read linked STORET Modern stations"));
	*/
	wxMenu *execMenu = new wxMenu;
	execMenu->Append(MENU_DESCRIBE_FLOW, wxT("&Describe Flow"), wxT("List COMID tree structure"));
	execMenu->Append(MENU_ROUTE_FACILITIES, wxT("Route Treatment Facilities"), wxT("Find COMID for each USGS station"));
	execMenu->Append(MENU_ROUTE_FACILITIES_THREADED, wxT("Route Treatment Facilities, &Threaded"), wxT("Find COMID for each USGS station, multithreaded version"));
	execMenu->Append(MENU_ROUTE_FACILITIES_RIVER, wxT("Route Treatment Facilities &River"), wxT("Find COMID for each USGS station along a single river"));
	execMenu->Append(MENU_ROUTE_COLORADO, wxT("Route Facilities &Colorado"), wxT("Build bin files for the complete Colorado River"));
	execMenu->Append(MENU_ROUTE_MISSISSIPPI, wxT("Route Facilities &Mississippi"), wxT("Build bin files for the complete Mississippi River"));
	execMenu->Append(MENU_WRITE_STRAHLER, wxT("Write Strahler Values"), wxT("Write a CSV of Strahler values by COMID."));
	execMenu->Append(MENU_WRITE_POINT_COMID, wxT("Write Point Layer COMIDs"), wxT("Write a CSV of Point identifiers by COMID."));
	execMenu->Append(MENU_WRITE_2DANIMATION, wxT("Write 2d Animation"), wxT("Write 2d Animation."));
	execMenu->Append(MENU_WRITE_COLLADA, wxT("Write Collada Animation"), wxT("Write Collada Animation."));
	// execMenu->Append(MENU_BUILD_BUFFER, wxT("Create Buffer"), wxT("Create a shapefile with a defined buffer around streams."));
	execMenu->Append(MENU_BUILD_BUFFER_CLIPPER, wxT("Create Buffer"), wxT("Create a map layer with a defined buffer around streams."));
	execMenu->Append(MENU_MERGE_POLYGONS, wxT("Merge Polygons"), wxT("Combine polygons in buffer layer."));
	execMenu->Append(MENU_COMID_NETWORK, wxT("COMID Network"), wxT("COMID From/To Report (minus divergences)."));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(editMenu, wxT("&Edit"));
    menuBar->Append(processMenu, wxT("&Process"));
    menuBar->Append(execMenu, wxT("E&xecute"));

	SetMenuBar(menuBar);

	Connect (wxID_CONFIGURE, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &frame_CARDGIS::OnConfigure);
	Connect (wxID_SELECT_UP, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) &frame_CARDGIS::OnSelectUpstream);
	Connect (wxID_SELECT_DOWN, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) &frame_CARDGIS::OnSelectDownstream);
	Connect (wxID_LIST_UP, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) &frame_CARDGIS::OnListUpstream);
	Connect (wxID_LIST_DOWN, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) &frame_CARDGIS::OnListDownstream);
	Connect (wxID_CLOSEST_SEGMENT, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) &frame_CARDGIS::OnClosestSegment);
	Connect (MENU_COMID_NETWORK, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) &frame_CARDGIS::OnCOMIDNetwork);
}

void frame_CARDGIS::OnClose
	(wxCloseEvent& e)

{
	// if (current_shapefile)
	// app->current_shapefile_index = recent_shapefiles->match (current_shapefile);

	if ( e.CanVeto()
	&& db_odbc) { // && m_bFileNotSaved)
		if ( wxMessageBox("Save current record?", "", wxICON_QUESTION | wxYES_NO) == wxYES) {
			dynamic_string log;

			read_field_settings ();
			SaveRecord (log);

			if (log.get_length () > 0) {
				e.Veto (true);
				log.convert_linefeeds_for_CEdit ();
				dialog_error_list d (this, L"File Open", L"", log);
				d.ShowWindowModal ();
			}
		}
	}
	Destroy();  // you may also do:  event.Skip();
	// since the default event handler does call Destroy(), too
}


void frame_CARDGIS::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close (true /*force closing*/);
}

void frame_CARDGIS::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox( _("i_SWAT"), _("About i_SWAT"), wxOK | wxICON_INFORMATION, this );
}

/*
void frame_CARDGIS::OnOpenRecent
	(wxCommandEvent &ev)

{
	dynamic_string log;
	importer_shapefile importer;

	if (recent_shapefiles->get (ev.GetId () - ID_FILE_RECENT_SHAPEFILE, &importer)) {
		panel_watershed->pause_rendering = true;
		open_shapefile (&importer, log);
		panel_watershed->pause_rendering = false;

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"File Open", L"", log);
			d.ShowWindowModal ();
		}
		enable_map_controls ();
		panel_watershed->change_layers ();
		panel_watershed->Refresh ();
	}
}
*/

bool frame_CARDGIS::connect_to_odbc
	(odbc_database_credentials *creds,
	interface_window *view,
	dynamic_string &log)

// DB can't be reached solely through it's filename

{
	dynamic_string connection_string;
	bool cancel = false, error = false;

	// if (!((app_i_epic *) AfxGetApp ())->get_credentials (database_filename.get_text_ascii (), type, &creds))

	dialog_open_database ddb (this, &app->db_credentials, creds);
	if (ddb.ShowModal () == wxID_OK) {
		app->add_credentials (*creds);
	}
	else
		cancel = true;

	if (!cancel) {
		switch (creds->type) {
			case ODBC_MYSQL:
				db_odbc = new odbc_database_mysql;
				break;
			case ODBC_SQLSERVER:
				db_odbc = new odbc_database_sql_server;
				break;
			case ODBC_ACCESS:
				db_odbc = new odbc_database_access;
				break;
		}
		if (db_odbc) {
			if (db_odbc->open (creds, view, log)) {
				recent_dbs->retain (creds);
			}
			else {
				error = true;
				delete db_odbc;
				db_odbc = NULL;
			}
		}
	}

	if (error)
		return false;
	else
		return !cancel;
}

void frame_CARDGIS::OnFileNew
	(wxCommandEvent&)

{
    filename_struct f;
	dynamic_string log;
	BYTE database_type= ODBC_MYSQL;
	bool cancel = false;

	if (database_type == ODBC_MYSQL) {
		interface_window_wx view;
		view.setup_wx (this);
		view.update_status_bar ("Creating database");
		// new_mysql_database (log);
	}
	else {
		dynamic_string path;
		// ODBC_ACCESS:
		wxFileDialog openFileDialog ( this, _("Open file"), "", "", "Access files (*.mdb)|*.mdb|Firebird files (*.fdb)|*.fdb", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
 
		if ( openFileDialog.ShowModal() == wxID_OK ) {
			SetCursor (*wxHOURGLASS_CURSOR);
			interface_window_wx view;
			view.setup_wx (this);
			view.update_status_bar ("Creating database");

			path += openFileDialog.GetDirectory().ToAscii ();
			path += PATH_SEPARATOR;
			path += openFileDialog.GetFilename().ToAscii ();

			/*
			switch (database_type) {
				case ODBC_SQLSERVER:
					if (!new_sqlserver_database (error_message))
						cancel = true;
					break;
				case ODBC_ACCESS:
					if (prompt_remove_database (database_filename)) {
						database_maker maker;
						db_odbc = new odbc_database_access;

						if (db_odbc
						&& maker.create_database (database_filename.get_text_ascii (), error_message)) {

							if (((odbc_database_access *) db_odbc)->open_by_name (database_filename, ODBC_ACCESS_DEFAULT_DRIVER, "", "", "", error_message))
								boss.create_tables (db_odbc, error_message);

							maker.close ();
						}
					}
					else
						cancel = true;
					break;
				case ODBC_FIREBIRD:
					if (prompt_remove_database (database_filename)) {
						if (is_firebird (database_filename.get_text ())) {
							odbc_database_credentials creds;

							creds.set_database_filename (database_filename);
							creds.type = ODBC_FIREBIRD;
							creds.set_hostname ("localhost");
							creds.set_driver_name (ODBC_FIREBIRD_DEFAULT_DRIVER);
							((app_i_swat *) AfxGetApp ())->db_credentials.push_back (creds);
							dialog_open_database ddb (NULL, &((app_i_swat *) AfxGetApp ())->db_credentials, &creds, ((app_i_swat *) AfxGetApp ())->db_encryption_key);
							if (ddb.DoModal () == IDOK) {
								((app_i_swat *) AfxGetApp ())->add_credentials (creds);

								db_odbc = new odbc_database_firebird;
								if (((odbc_database_firebird *) db_odbc)->open (&creds, ((app_i_swat *) AfxGetApp ())->db_encryption_key, error_message)) {

									((app_i_swat *) AfxGetApp ())->add_credentials (creds);

									boss.create_tables (db_odbc, error_message);
								}
							}
						}
						else {
							error_message += "ERROR, filename suffix incorrect.\n";
							cancel = true;
						}
					}
					else
						cancel = true;
			}
			*/
		}
		else
			cancel = true;
	}

	if (!cancel) {

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();

		// wxString wxlog = log.get_text_wide ();
		// m_logWindow->WriteText (wxlog);

		SetCursor (wxNullCursor);
	}
}

void frame_CARDGIS::OnFileOpen
	(wxCommandEvent&)

{
    filename_struct f;
	dynamic_string path;
	dynamic_string log;
	bool cancel = false;

	// ODBC_ACCESS:
	wxFileDialog openFileDialog ( this, _("Open file"), "", "", "Shape files (*.shp)|*.shp", wxFD_OPEN, wxDefaultPosition);
 
	if ( openFileDialog.ShowModal() == wxID_OK ) {
		SetStatusText ("OnFileOpen");
		SetCursor (*wxHOURGLASS_CURSOR);
		interface_window_wx view;
		view.start_progress (this);
		view.update_progress ("Reading Shapefile", 1);

		path += openFileDialog.GetDirectory().ToAscii ();
		path += PATH_SEPARATOR;
		path += openFileDialog.GetFilename().ToAscii ();

		/*
		if (read_shapefile (path, &view, log)) {
			view.update_status_bar ("Updating recent credentials");
			odbc_database_credentials creds;
			creds.type = ODBC_ACCESS;
			creds.set_database_filename (path);
			recent_dbs->retain (&creds);
		}
		else
			cancel = true;
		*/
	}
	else
		cancel = true;

	if (!cancel) {

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();

		// wxString wxlog = log.get_text_wide ();
		// m_logWindow->WriteText (wxlog);

		SetCursor (wxNullCursor);
	}
}

void frame_CARDGIS::OnFileOpenTIFF
	(wxCommandEvent&)

{
	filename_struct f;
	dynamic_string path_tiff, filename_tiff;
	dynamic_string log;
	bool cancel = false, error = false;
	map_object_tiff* tiff;
	map_layer* layer_tiff;

	// path_tiff = "R:\\NHDPlusv2\\SWNET\\NHDPLUS_H_0101_HU4_RASTER\\";
	// path_tiff = "R:\\NHDPlusv2\\SWNET\\NHDPLUS_H_0102_HU4_RASTER\\";
	// path_tiff = "R:\\NHDPlusv2\\SWNET\\NHDPLUS_H_0103_HU4_RASTER\\";
	// path_tiff = "R:\\NHDPlusv2\\SWNET\\NHDPLUS_H_0204_HU4_RASTER\\";
	path_tiff = "R:\\NHDPlusv2\\SWNET\\NHDPLUS_H_0309_HU4_RASTER\\";
	// path_tiff = "R:\\NHDPlusv2\\SWNET\\NHDPLUS_H_0101_HU4_RASTER\\";
	// path_tiff = "R:\\NHDPlusv2\\SWNET\\NHDPLUS_H_19020401_HU8_RASTER\\";
	// path_tiff = "R:\\NHDPlusv2\\SWNET\\NHDPLUS_H_19020101_HU8_RASTER\\";

	// filename_tiff = "swnet.tif";

	// ODBC_ACCESS:
	wxFileDialog openFileDialog (this, _("Open file"), "", "", "TIFF files (*.tif)|*.tif", wxFD_OPEN, wxDefaultPosition);

	openFileDialog.SetDirectory (path_tiff.get_text ());

	if (openFileDialog.ShowModal() == wxID_OK) {
		SetStatusText("OnFileOpenTIFF");

		interface_window_wx view;
		view.start_progress(this);
		view.update_progress("Reading TIFF", 1);

		filename_tiff += openFileDialog.GetDirectory().ToAscii();
		filename_tiff += PATH_SEPARATOR;
		filename_tiff += openFileDialog.GetFilename().ToAscii();

		if ((layer_tiff = map_watershed->match("TIFF")) == NULL) {
			layer_tiff = map_watershed->create_new(MAP_OBJECT_TIFF);
			layer_tiff->name = "TIFF";
			layer_tiff->include_in_legend = false;
			layer_tiff->color = RGB(255, 0, 0);
			layer_tiff->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR; // MAP_OBJECT_DRAW_POINT_LAYER_COLOR;
			map_watershed->layers.push_back(layer_tiff);
		}

		/*
		if ((layer_tiff_outline = map_watershed->match ("TIFF Rect")) == NULL) {
			layer_tiff_outline = map_watershed->create_new(MAP_OBJECT_POLYGON);
			layer_tiff_outline->name = "TIFF Rect";
			layer_tiff_outline->include_in_legend = false;
			layer_tiff_outline->color = RGB(0, 0, 255);
			layer_tiff_outline->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_tiff_outline->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
			map_watershed->layers.push_back(layer_tiff_outline);
		}
		*/

		if (app->use_gl)
			tiff = new map_object_tiff_gl;
		else
			tiff = new map_object_tiff_wx;

		wxInitAllImageHandlers();

		// if (tiff->read_wxbitmap(filename_tiff.get_text_ascii(), NULL, log)) {
		if (tiff->read_tiff(filename_tiff.get_text_ascii(), NULL, log)) {

			log += filename_tiff;
			log += "\n";

			if (tiff->tiff->bits_per_pixel >= 24) {
				tiff->tiff->make_8_bit ();
				// tiff->tiff->recolor ();
			}
//			tiff->tiff->reduce(20);
			tiff->tiff->bit_value_histogram(log);

			if (false) {
				// Test forcing of tiff coordinates to center of NHD area 01
				tiff->tiff->logical_extent.y[0] = 44.743261;
				tiff->tiff->logical_extent.x[0] = -70.827633;
				tiff->tiff->logical_extent.y[1] = 43.287278;
				tiff->tiff->logical_extent.x[1] = -68.722338;
			}
			else {
				double llx, lly;
				/*
				boost_ll_albers_3174 (tiff->tiff->logical_extent.x[0], tiff->tiff->logical_extent.y[0], llx, lly, log);
				tiff->tiff->logical_extent.x[0] = llx;
				tiff->tiff->logical_extent.y[0] = lly;
				boost_ll_albers_3174 (tiff->tiff->logical_extent.x[1], tiff->tiff->logical_extent.y[1], llx, lly, log);
				tiff->tiff->logical_extent.x[1] = llx;
				tiff->tiff->logical_extent.y[1] = lly;
				*/

				Albers2LL(&lly, &llx, tiff->tiff->logical_extent.y[0], tiff->tiff->logical_extent.x[0], tiff->tiff->longitude_central_meridian,
				tiff->tiff->LatitudeofOrigin, tiff->tiff->StandardParallel1, tiff->tiff->StandardParallel2);
				tiff->tiff->logical_extent.x[0] = llx;
				tiff->tiff->logical_extent.y[0] = lly;

				Albers2LL(&lly, &llx, tiff->tiff->logical_extent.y[1], tiff->tiff->logical_extent.x[1], tiff->tiff->longitude_central_meridian,
				tiff->tiff->LatitudeofOrigin, tiff->tiff->StandardParallel1, tiff->tiff->StandardParallel2);
				tiff->tiff->logical_extent.x[1] = llx;
				tiff->tiff->logical_extent.y[1] = lly;

				// reset lat & long after conversion
				tiff->latitude = (long)(tiff->tiff->logical_extent.y[0] * 1.0e6);
				tiff->longitude = (long)(tiff->tiff->logical_extent.x[0] * 1.0e6);
			}

			layer_tiff->objects.push_back(tiff);

			/*
			tiff_rect = new map_polygon;

			tiff_rect->add_point((long) (tiff->tiff->latitude_1*1.0e6), (long)(tiff->tiff->longitude_1 * 1.0e6));
			tiff_rect->add_point((long) (tiff->tiff->latitude_1 * 1.0e6), (long)(tiff->tiff->longitude_2 * 1.0e6));
			tiff_rect->add_point((long) (tiff->tiff->latitude_2 * 1.0e6), (long)(tiff->tiff->longitude_2 * 1.0e6));
			tiff_rect->add_point((long) (tiff->tiff->latitude_2 * 1.0e6), (long)(tiff->tiff->longitude_1 * 1.0e6));
			tiff_rect->set_extent ();

			layer_tiff_outline->objects.push_back (tiff_rect);
			*/


			map_watershed->set_extent();  // set dynamic_map.logical_extent
			panel_watershed->change_layers();
			panel_watershed->change_selection_layer (layer_tiff);
			enable_map_controls();
			panel_watershed->Refresh();
		}
		else
			error = true;
	}
	else
		cancel = true;

	if (!cancel) {

		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"File Open TIFF", L"", log);
		d.ShowWindowModal();
	}
}

/*
void frame_CARDGIS::open_shapefile
	(importer_shapefile *importer,
	dynamic_string &log)

{
	filename_struct f;

	{
		interface_window_wx view;
		view.start_progress (this);

		SetStatusText ("Importing shapefile");

		router->setup_shapefile (filename_nhd_area, importer);

		router->read_shapefile (importer, map_watershed, &view, log);

		view.destroy_dialog_progress ();
	}

	f.parse (&importer->filename_source);
	
	recent_shapefiles->retain (importer);

	SetStatusText ("End of open_shapefile");
}
*/

void frame_CARDGIS::OnOpenShapefile
	(wxCommandEvent&)

{

	importer_shapefile importer;
	map_layer *new_layer;
	dynamic_string log;
	bool clip_to_map = false; // map_watershed->layers.size() > 0;

	/*
	double albers_n, albers_C, albers_rho0, R;
	double latitude, longitude, northing, easting;
	long longitude_central_meridian, latitude_origin;
	double standard_parallel_radians [2];

	// example p. 291 of "Map Projections: a Working Manual"
	R = 1.0;
	longitude_central_meridian = -96;
	latitude_origin = 23;
	northing = 35.0;
	easting = -75.0;
	standard_parallel_radians [0] = 29.5 * DEGREES_TO_RADIANS;
	standard_parallel_radians [1] = 45.5 * DEGREES_TO_RADIANS;

	albers_n = (sin (standard_parallel_radians [0]) + sin (standard_parallel_radians [1])) / 2.0;
	albers_C = pow (cos (standard_parallel_radians [0]), 2.0) + 2.0 * albers_n * sin (standard_parallel_radians [0]);
	albers_rho0 = R * pow (albers_C - 2.0 * albers_n * sin ((double) latitude_origin * DEGREES_TO_RADIANS), 0.5) / albers_n;

	Albers2LL (&latitude, &longitude, northing, easting, R, longitude_central_meridian, albers_n, albers_C, albers_rho0);
	*/

	filename_nhd_area = app->filename_nhd_root;
	filename_nhd_area += "NHDPlus01\\";
	// router->setup_shapefile (filename_nhd_area, &importer);
	dialog_import_shapefile dis (this, recent_shapefiles, &importer, &clip_to_map);
 
	if (dis.ShowModal() == wxID_OK ) {
		interface_window_wx view;
		view.start_progress (this);

		new_layer = map_watershed->create_new (MAP_OBJECT_POLYGON);
		new_layer->name = importer.filename_source;
		new_layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		new_layer->include_in_legend = true;
		map_watershed->layers.push_back (new_layer);

		importer.take_dbf_columns = true;

		if (clip_to_map) {
			shapefile_filter *region_filter;
			map_watershed->check_extent ();
			region_filter = new shapefile_filter;
			region_filter->type = SHAPEFILE_FILTER_AREA;
			region_filter->location = map_watershed->logical_extent;

			// Use normalize to convert units from millionths of degrees to degrees,
			// since shapefile_features are in degrees (or meters, etc).

			// 2020-03-25 BHO import of geoft_bho_pontodrenagem over geoft_bho_trechodrenagem
			// Both shapefiles have the same projection, changing sign of longitude is incorrect
			region_filter->location.normalize (true);

			importer.filters.push_back (region_filter);
		}

		panel_watershed->pause_rendering = true;
		importer.import (new_layer, NULL, map_watershed, &view, log);

		if (new_layer->type == MAP_OBJECT_POINT) {
			// importer_shapefile.read_shapefile may change map_layer.type
			new_layer->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			new_layer->symbol = MAP_OBJECT_SYMBOL_PLUS;
		}
		map_watershed->check_extent ();

		panel_watershed->pause_rendering = false;

		panel_watershed->change_layers ();
		panel_watershed->change_selection_layer (new_layer);

		enable_map_controls ();
		panel_watershed->redraw ("OnOpenShapefile");

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"File Open", L"", log);
			d.ShowWindowModal ();
		}

	}
}

bool frame_CARDGIS::read_db_start
	(interface_window *,
	dynamic_string &)

{
	bool error = false;

	return !error;
}

bool frame_CARDGIS::open_file
	(const dynamic_string &filename,
	interface_window *view,
	dynamic_string &log)

{
	bool error = false;
	db_odbc = new odbc_database_access;
	odbc_database_credentials creds;
	creds.set_database_filename (filename);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	// if (((odbc_database_access *) db_odbc)->open (filename, ODBC_ACCESS_DEFAULT_DRIVER, "", "", L"", log))
	if (((odbc_database_access *) db_odbc)->open (&creds, view, log)) {
		view->update_status_bar ("DB open");
		if (read_db_start (view, log)) {
			// set_position (view, log);
		}
		else
			error = true;
	}
	else {
		delete db_odbc;
		db_odbc = NULL;
		error = true;
	}

	return !error;
}

bool frame_CARDGIS::SaveRecord
	(dynamic_string &)

{
	bool error = false;

	return !error;
}

void frame_CARDGIS::DoSize()
{
    // if ( !m_logWindow )
    //  return;


	/*
    wxSize size = GetClientSize();
    wxCoord segment_height = size.y / 5;
    // list_control->SetSize(0, 0, size.x, y);
    if (tree) {
	    tree->SetSize (0, 0, size.x, segment_height * 2);
		// list_subbasins->SetSize (0, segment_height * 2 + 1, size.x, segment_height * 4);
		m_logWindow->SetSize(0, segment_height * 4 + 1, size.x, size.y);
	}
	else
		m_logWindow->SetSize(0, 0, size.x, size.y);
	*/
}

void frame_CARDGIS::OnConfigure
	(wxCommandEvent&)

{
	dialog_configure *d = new dialog_configure (this, app);
	if (d->ShowModal () == wxID_OK)
		panel_watershed->redraw ("OnConfigure");
}

void frame_CARDGIS::read_field_settings ()

{
}

/*
void frame_CARDGIS::OnListConnected
	(wxCommandEvent&)

{
	dynamic_string log;
	long id;

	id = 1;
	dialog_prompt dp (this, "Enter ID", &id);
	if (dp.ShowModal () == wxID_OK) {
		SetCursor (*wxHOURGLASS_CURSOR);

		interface_window_wx update_display;
		update_display.start_progress (this);

		log += "Upstream\n";
		router->rivers.write_upstream (id, log);

		log += "Downstream\n";
		router->rivers.write_downstream (id, log);

		SetCursor (*wxSTANDARD_CURSOR);
		if (log.get_length () > 0) {

			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"Next Field", L"", log);
			d.ShowWindowModal ();
		}
	}
}
*/

void frame_CARDGIS::OnReport
	(wxCommandEvent&)

// Summarize area, CP, Landuse

{

	dynamic_string log;

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Set CDL Landuse", L"", log);
	d.ShowWindowModal ();
}

/*
bool work_condense
	(flow_network *stream_1,
	flow_network *stream_2)

// Called by thread_manager.start_job via std::async

{
	stream_1->append (stream_2);
	stream_1->condense ();
	return true;
}


void frame_CARDGIS::condense_pairs
	(flow_network *streams,
	const int thread_count,
	interface_window *view)
{
	std::future <bool> *threads;
	int thread_index, complete_count;
	bool finished, *done;
	done = new bool [thread_count];
	dynamic_string update;

	view->update_progress ("Starting condense threads", 0);
	threads = new std::future <bool> [thread_count];
	for (thread_index = 0; thread_index < thread_count; ++thread_index) {
		threads [thread_index] = std::async (std::launch::async, work_condense, &streams [thread_index * 2], &streams [thread_index * 2 + 1]);
		done [thread_index] = false;
	}

	view->update_progress ("Waiting for condense threads", 0);
	finished = false;
	complete_count = 0;
	while (!finished) {
		finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			if (!done [thread_index]) {
				finished = false;
				if (threads [thread_index].wait_for (std::chrono::milliseconds (100)) == std::future_status::ready) {
					done [thread_index] = true;
					++complete_count;
					update.format ("%d of %d complete", complete_count, thread_count);
					view->update_progress (update, 1);
				}
			}
		}
	}

	delete [] threads;
}

bool work_build_tree
	(std::map <long, long> *segment,
	flow_network *stream)

// Called by thread_manager.start_job via std::async

{
	// stream->build_tree_2 (segment, NULL);
	return true;
}

void frame_CARDGIS::build_tree_threaded
	(const int thread_count,
	interface_window *view)

{
	int segment_size;
	int thread_index, segment_index, complete_count;
	std::map <long, long>::const_iterator link;
	std::future <bool> *threads;
	std::map <long, long> *segments;
	bool finished, *done;
	flow_network *streams;
	dynamic_string update;

	segment_size = from_to.size () / thread_count;
	if (segment_size * thread_count != (int) from_to.size ())
		segment_size += 1;

	link = from_to.begin ();

	streams = new flow_network [thread_count];
	segments = new std::map <long, long> [thread_count];
	threads = new std::future <bool> [thread_count];
	done = new bool [thread_count];

	view->update_progress ("Starting threads", 0);
	for (thread_index = 0; thread_index < thread_count; ++thread_index) {
		done [thread_index] = false;

		// Put a section of from_to into segments [index];
		for (segment_index = 0;
		(link != from_to.end ())
		&& (segment_index < segment_size);
		++segment_index) {
			segments [thread_index].insert (*link);
			++link;
		}

		threads [thread_index] = std::async (std::launch::async, work_build_tree, &segments [thread_index], &streams [thread_index]);
	}

	view->update_progress ("Waiting for threads", 0);
	finished = false;
	complete_count = 0;
	while (!finished) {
		finished = true;
		for (thread_index = 0;
		thread_index < thread_count;
		++thread_index) {
			if (!done [thread_index]) {
				finished = false;
				if (threads [thread_index].wait_for (std::chrono::milliseconds (100)) == std::future_status::ready) {
					done [thread_index] = true;
					++complete_count;
					update.format ("%d of %d complete", complete_count, thread_count);
					view->update_progress (update, 1);
				}
			}
		}
	}

	delete [] threads;
	delete [] segments;

	view->update_progress ("Reconciling stream groups", 0);

	int pair_count = thread_count / 2, pair_index;
	while (pair_count >= 1) {
		condense_pairs (streams, pair_count, view);
		for (pair_index = 0; pair_index < pair_count; ++pair_index)
			streams [pair_index + 1] = streams [pair_index * 2];
		pair_count /= 2;
	}

	// River's fragment map is empty.  Set it to first one
	river.root_fragments = streams [0].root_fragments;

	delete [] streams;
}
*/

/*
int find_index
	(std::vector <long> *comids,
	const long target)

{
	std::vector <long>::iterator id;
	int found = -1;

	for (id = comids->begin ();
	(found == -1) && (id != comids->end ());
	++id) {
		if (*id == target)
			found = id - comids->begin ();
	}
	return found;
}
*/

void frame_CARDGIS::OnReadFlowNHD
	(wxCommandEvent&)

// Read PlusFlow.dbf

{

	dynamic_string log;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	router_v2->read_flow_and_build_network (filename_nhd_area, &view, log);

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Build NHD network", L"", log);
	d.ShowWindowModal ();
}


void frame_CARDGIS::OnReadFlowChina
	(wxCommandEvent&)

// Read 5 shapefiles for china rivers
// Build network from them

{
	dynamic_string log;
	ChinaRivers crivers;
	map_layer *layer_rivers, *layer_stations, *layer_points, *layer_counties;
	map_object *station;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);
	panel_watershed->pause_rendering = true;

	layer_rivers = create_river_layer (map_watershed, false, false);
	router_v2->river_map_layer = layer_rivers;
	layer_rivers->attributes_void_usage = MAP_LAYER_USAGE_VOID_SOURCES_SINKS;

	// Layer of Water Stations
	layer_stations = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_stations->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_stations->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
	layer_stations->symbol_size *= 2;
	layer_stations->initialize_attributes = true; // clear all attributes as polygons are created
	layer_stations->name = "Water Stations";
	layer_stations->color = RGB (0, 0, 255);
	map_watershed->layers.push_back (layer_stations);
	router_v2->read_points_csv ("I:\\TDC\\China rivers\\sheets\\water station address_all Converted.csv", true, true, false, layer_stations, map_watershed, log);

	// Layer of counties
	layer_counties = crivers.read_counties ("I:\\TDC\\China rivers\\County_pl\\BOUNT_poly.shp", map_watershed, &view, log);

	// On edge of Taihu Lake: 120°16'03''E, 31°26'03''N
	station = layer_stations->create_new (MAP_OBJECT_POINT);
	station->id = 1;
	station->latitude = (long) ((31.0 + (26.0 / 60.0) + (3.0 / 3600.0)) * 1.0e6);
	station->longitude = (long) ((120.0 + (16.0 / 60.0) + (3.0 / 3600.0)) * 1.0e6);
	layer_stations->objects.push_back (station);
    // Near a river: 119°39'29''E, 32°21'04''N
	station = layer_stations->create_new (MAP_OBJECT_POINT);
	station->id = 2;
	station->latitude = (long) ((32.0 + (21.0 / 60.0) + (4.0 / 3600.0)) * 1.0e6);
	station->longitude = (long) ((119.0 + (39.0 / 60.0) + (29.0 / 3600.0)) * 1.0e6);
	layer_stations->objects.push_back (station);

	crivers.filename_china_root = "H:\\scratch\\China Rivers\\5 level river network\\";

	// layer_points = crivers.build_point_layer (map_watershed, layer_rivers, &view, log);
	layer_points = crivers.import_point_layer (map_watershed, &view, log);
	crivers.read_5_levels (map_watershed, &view, log);
	crivers.consolidate_points (map_watershed, layer_points, log);
	crivers.copy_to_rivers (map_watershed, layer_rivers, layer_points, &view, log);
	crivers.build_connection_map (map_watershed, layer_rivers, layer_points, log);
	crivers.dump_point_maps (log);
	crivers.write_segment_elevations (layer_rivers, layer_points, log);

	map_watershed->set_extent ();
	view.update_progress ("Building map layer");

	view.update_progress ("Building river layer index");
	{
		// Build an index of map objects for quick acess later
		std::vector <map_object *>::const_iterator polyline;

		for (polyline = layer_rivers->objects.begin ();
		polyline != layer_rivers->objects.end ();
		++polyline) {
			crivers.river_layer_index.insert (std::pair <long, map_object *> ((*polyline)->id, *polyline));
			// router->river_layer_index.insert (std::pair <long, map_object *> ((*polyline)->id, *polyline));
		}
	}

	// router->build_China_network (map_watershed, &view, log);

	panel_watershed->pause_rendering = false;
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnReadFlowChina");

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Build China River network", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnRouteChinaRiver
	(wxCommandEvent&)

// Connect stations to sampling points

{
	dynamic_string log;
	ChinaRivers crivers;
	ChinaRouter c_router;
	map_layer *layer_rivers, *layer_link_points, *layer_stations, *layer_counties, *layer_route;
	map_object *segment_copy;
	device_coordinate station_position, county_center;
	std::vector <map_object *>::iterator road;
	long long start_point_id, destination_id;

	layer_rivers = map_watershed->match ("Rivers");
	layer_link_points = map_watershed->match ("Link Points");
	layer_stations = map_watershed->match ("Water Stations");
	layer_counties = map_watershed->match ("Counties");

	// start_point_id = 71;
	// destination_id = 3228;
	start_point_id = 35;
	destination_id = 1390;

	dialog_river_segment drs (this, map_watershed, &layer_stations, &layer_counties, &start_point_id, &destination_id);
	if (drs.ShowModal () == wxID_OK) {

		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);
		panel_watershed->pause_rendering = true;
		SetCursor (*wxHOURGLASS_CURSOR);

		if ((layer_route = map_watershed->match ("Route")) == NULL) {
			layer_route = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_route->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			layer_route->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
			layer_route->enclosure = MAP_POLYGON_NOT_ENCLOSED;
			layer_route->line_width = 2;
			layer_route->initialize_attributes = true; // clear all attributes as polygons are created
			layer_route->name = "Route";
			layer_route->color = RGB (255, 255, 0);
			layer_route->attribute_count_numeric = layer_rivers->attribute_count_numeric;
			layer_route->attribute_count_text = layer_rivers->attribute_count_text;
			layer_route->attributes_void_usage = layer_rivers->attributes_void_usage;
			layer_route->column_names_numeric = layer_rivers->column_names_numeric;
			layer_route->column_names_text = layer_rivers->column_names_text;
			map_watershed->layers.push_back (layer_route);
		}
		else
			layer_route->clear_objects ();

		c_router.start_object = layer_stations->match_id (start_point_id);
		c_router.end_object = layer_counties->match_id (destination_id);

		// Build an index of map objects for quick acess
		view.update_progress ("Building river layer index");
		for (road = layer_rivers->objects.begin ();
		road != layer_rivers->objects.end ();
		++road)
			crivers.river_layer_index.insert (std::pair <long, map_object *> ((*road)->id, *road));

		view.update_progress ("Routing");

		c_router.route_directional (layer_rivers, &crivers.river_layer_index, log);

		for (road = c_router.path.begin ();
		road != c_router.path.end ();
		++road) {
			segment_copy = layer_rivers->create_new ((*road)->type);
			segment_copy->copy (*road, layer_route);
			layer_route->objects.push_back (segment_copy);
		}

		{
			std::vector <map_object *>::iterator next_road;
			double perimeter, total_length_km = 0.0;

			log.add_formatted ("%lld to %lld", c_router.start_object->id, c_router.end_object->id);
			if (c_router.upstream)
				log += "\tUpstream\n";
			else
				log += "\tDownstream\n";

			log += "Segment\tIndex\tLat\tLon\tLength,km\tTotal,km\n";

			for (road = c_router.path.begin ();
			road != c_router.path.end ();
			++road) {
				log.add_formatted ("%lld", (*road)->id);

				next_road = road;
				next_road++;

				if (road == c_router.path.begin ()) {
					// First segment
					// log.add_formatted ("\t%d", c_router.start_point_index);
					if (c_router.upstream) {
						((map_polygon *) *road)->write_length_km (log, false, 1, 0, c_router.start_point_index);
						((map_polygon *) *road)->perimeter_between_meters (0, c_router.start_point_index, &perimeter, log);
					}
					else {
						((map_polygon *) *road)->write_length_km (log, false, 1, c_router.start_point_index, -1);
						((map_polygon *) *road)->perimeter_between_meters (c_router.start_point_index, ((map_polygon *) *road)->total_number_of_points () - 1, &perimeter, log);
					}
				}
				else
					if (next_road == c_router.path.end ()) {
						// Last segment
						// log.add_formatted ("\t%d", c_router.end_point_index);
						if (c_router.upstream) {
							((map_polygon *) *road)->write_length_km (log, false, 1, c_router.end_point_index, ((map_polygon *) *road)->total_number_of_points () - 1);
							((map_polygon *) *road)->perimeter_between_meters (c_router.end_point_index, ((map_polygon *) *road)->total_number_of_points () - 1, &perimeter, log);
						}
						else {
							((map_polygon *) *road)->write_length_km (log, false, 1, 0, c_router.end_point_index);
							((map_polygon *) *road)->perimeter_between_meters (0, c_router.end_point_index, &perimeter, log);
						}
					}
					else {
						// In-between
						((map_polygon *) *road)->perimeter_between_meters (0, ((map_polygon *) *road)->total_number_of_points () - 1, &perimeter, log);
						log.add_formatted ("\t\t\t\t\t%.4lf", perimeter / 1000.0);
					}


				total_length_km += perimeter / 1000.0;
				log += "\n";
			}
			log.add_formatted ("\t\t\t\t\t%.4lf\tkm\n", total_length_km);
			log.add_formatted ("Router distance\t\t\t\t\t%.4lf\tkm\n", c_router.distance_m / 1000.0);


			log += "\n";
			log += "Segment\tElevation Start\tElevation End\n";

			for (road = c_router.path.begin ();
			road != c_router.path.end ();
			++road) {
				log.add_formatted ("%ld", (*road)->id);
				log.add_formatted ("\t%.4lf\t%.4lf", (*road)->attributes_numeric[2], (*road)->attributes_numeric[3]);
				log += "\n";
			}
			log.add_formatted ("Elevation change\t%.4lf\t%.4lf\n", c_router.elevation_start, c_router.elevation_end);
		}

		view.destroy_dialog_progress ();
		SetCursor (*wxSTANDARD_CURSOR);
		panel_watershed->pause_rendering = false;
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnRouteChinaRiver");

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Route China River points", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnRouteAstar
	(wxCommandEvent&)

// Connect stations to sampling points

{
	dynamic_string log;
	astar_router a_router;
	astar_graph graph;
	map_layer *layer_rivers, *layer_link_points, *layer_stations, *layer_counties;
	device_coordinate station_position, county_center;
	long long start_point_id, destination_id;
	std::set <long long> result_edge_set;

	layer_rivers = map_watershed->match ("Rivers");
	layer_link_points = map_watershed->match ("Link Points");
	layer_stations = map_watershed->match ("Water Stations");
	layer_counties = map_watershed->match ("Counties");

	// start_point_id = 71;
	// destination_id = 3228;
	start_point_id = 35;
	destination_id = 1390;

	dialog_river_segment drs (this, map_watershed, &layer_stations, &layer_counties, &start_point_id, &destination_id);
	if (drs.ShowModal () == wxID_OK) {
		map_object *road;
		std::vector <long long>::iterator road_id, next_road_id;

		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);
		panel_watershed->pause_rendering = true;
		SetCursor (*wxHOURGLASS_CURSOR);

		a_router.start_object = layer_stations->match_id (start_point_id);
		a_router.end_object = layer_counties->match_id (destination_id);

		log.add_formatted ("Station %ld to County %ld\n", start_point_id, destination_id);

		graph.setup_graph_china_router (layer_link_points, layer_rivers, false, log);
		a_router.route (layer_link_points, layer_rivers, &graph, log);
		// a_router.route_quiet (layer_link_points, layer_rivers, &graph, log);
		// a_router.write_route_line (log);

		log += "\n";
		log += "Segment\tElevation Start\tElevation End\n";

		for (road_id = a_router.result_edges.begin ();
		road_id != a_router.result_edges.end ();
		++road_id) {
			// Need to add start_segment?
			if ((road_id == a_router.result_edges.begin ())
			&& (*road_id != a_router.start_segment->id)) {
				// result_edges did not include start_segment
				log.add_formatted ("%lld", a_router.start_segment->id);
				log.add_formatted ("\t%.4lf\t%.4lf", a_router.start_segment->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_START],
				a_router.start_segment->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_END]);
				log += "\n";
			}
			road = layer_rivers->match_id (*road_id);
			log.add_formatted ("%lld", road->id);
			log.add_formatted ("\t%.4lf\t%.4lf", road->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_START],
			road->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_END]);
			log += "\n";

			// Need to add end_segment?
			next_road_id = road_id;
			++next_road_id;
			if ((next_road_id == a_router.result_edges.end ())
			&& (*road_id != a_router.end_segment->id)) {
				log.add_formatted ("%lld", a_router.end_segment->id);
				log.add_formatted ("\t%.4lf\t%.4lf", a_router.end_segment->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_START],
				a_router.end_segment->attributes_numeric[CHINA_ROUTER_INDEX_ELEVATION_END]);
				log += "\n";
			}
		}
		log.add_formatted ("Elevation change\t%.4lf\t%.4lf\n", a_router.elevation_start, a_router.elevation_end);

		for (road_id = a_router.result_edges.begin ();
		road_id != a_router.result_edges.end ();
		++road_id)
			result_edge_set.insert (*road_id);

		panel_watershed->change_selection_layer (layer_rivers);
		panel_watershed->replace_selection (&result_edge_set, log);

		view.destroy_dialog_progress ();
		SetCursor (*wxSTANDARD_CURSOR);
		panel_watershed->pause_rendering = false;
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnRouteA*");

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"A* River points", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnRouteAllChinaRivers
	(wxCommandEvent&)

// Connect stations to sampling points

{
	dynamic_string log;
	map_layer *layer_rivers, *layer_stations, *layer_counties, *layer_link_points;
	std::vector <map_object *>::iterator road;
	bool multithread = false;
	interface_window_wx view;
	dialog_run_threads *update_display = NULL;

	if (multithread) {
		update_display = new dialog_run_threads (this, true, false);
		update_display->Show ();
		update_display->setup_interface_window (app, &view);
	}
	else {
		view.setup_wx (this);
		view.start_progress (this);
	}

	panel_watershed->pause_rendering = true;

	layer_rivers = map_watershed->match ("Rivers");
	layer_stations = map_watershed->match ("Water Stations");
	layer_counties = map_watershed->match ("Counties");
	layer_link_points = map_watershed->match ("Link Points");

	view.update_progress ("Routing");
	if (multithread)
		astar_route_all_multithread (layer_stations, layer_counties, layer_rivers, layer_link_points, &view, log);
	else
		astar_route_all (layer_stations, layer_counties, layer_rivers, layer_link_points, &view, log);

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route All China Rivers", L"", log);
	d.ShowWindowModal ();

	if (multithread) {
		update_display->Destroy ();
		delete update_display;
	}

	panel_watershed->pause_rendering = false;
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnRouteAllChinaRivers");

}

void frame_CARDGIS::OnDescribeFlow
	(wxCommandEvent&)

// Summarize area, CP, Landuse

{
	string_grid grid;
	dynamic_string log;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	view.update_status_bar ("Writing to Grid");

	router_v2->rivers.dump (&grid);
	view.update_status_bar ("Writing Grid to log");
	grid.write (log);

	log += "Divergences\n";
	router_v2->rivers.divergences.write (log);

	// log += "Divergences Down\n";
	// router->rivers.divergences_down.write (log);

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"NHD network", L"", log);
	d.ShowWindowModal ();
}

map_layer *frame_CARDGIS::read_point_layer
	(dynamic_string &filename_points,
	const int record_limit,
	const bool normalize_longitude,
	const bool numeric_id,
	const BYTE file_format,
	std::vector <dynamic_string> *column_names,
	std::set <int> *text_column_indexes,
	std::set <int> *skip_column_indexes,
	const int id_column_index,
	const int lat_column_index,
	const int lon_column_index,
	dynamic_string &log)

// Create a new point layer and read points from the named file using given format

{
	bool error = false;
	map_layer *layer_points = NULL;
	layer_points = map_watershed->create_new (MAP_OBJECT_POINT);

	layer_points->name = filename_points;
	if (file_format == CSV_POINT_FORMAT_SPECIFIED) {
		layer_points->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
		layer_points->attribute_count_text = text_column_indexes->size ();
	}
	else {
		layer_points->attribute_count_text = text_column_indexes->size ();

		if (id_column_index >= 0)
			// lat, long and index columns are not attributes
			layer_points->attribute_count_numeric = column_names->size () - layer_points->attribute_count_text - 3;
		else
			// lat, long columns are not attributes
			layer_points->attribute_count_numeric = column_names->size() - layer_points->attribute_count_text - 2;
		layer_points->initialize_attributes = true;
	}

	{
		// Set map_layer's column_names_*** fields
		std::vector <dynamic_string>::iterator column_name;
		int column_index;
		for (column_name = column_names->begin (), column_index = 0;
		column_name != column_names->end ();
		++column_name, ++column_index) {
			if (skip_column_indexes->find (column_index) == skip_column_indexes->end ()) {
				if ((column_index != id_column_index)
				&& (column_index != lat_column_index)
				&& (column_index != lon_column_index)) {
					// Not ID, not lat or lon
					if (text_column_indexes->find (column_index) != text_column_indexes->end ())
						layer_points->column_names_text.push_back (*column_name);
					else
						layer_points->column_names_numeric.push_back (*column_name);
				}
			}
		}
	}
	layer_points->color = COLOR_FACILITIES;
	layer_points->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
	layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;

	switch (file_format) {
		case CSV_POINT_FORMAT_FACILITIES:
			if (!router_v2->read_facilities (filename_points, normalize_longitude, layer_points, map_watershed, log))
				error = true;
			break;
		case CSV_POINT_FORMAT_COMID:
			if (!router_v2->read_points_by_comid (filename_points, layer_points, map_watershed, log))
				error = true;
			break;
		case CSV_POINT_FORMAT_USGS:
			if (!router_v2->read_sampling_stations (filename_points, normalize_longitude, layer_points, map_watershed, log))
				error = true;
			break;
		case CSV_POINT_FORMAT_STORET_LEGACY_OLD:
			layer_points->attribute_count_text = 1;
			if (!router_v2->read_storet_legacy_old (filename_points, layer_points, map_watershed, log))
				error = true;
			break;
		case CSV_POINT_FORMAT_STORET_LEGACY_NEW:
			layer_points->attribute_count_text = 1;
			if (!router_v2->read_storet_legacy_new (filename_points, layer_points, map_watershed, log))
				error = true;
			break;
		case CSV_POINT_FORMAT_SPECIFIED:
		case CSV_POINT_FORMAT_AUTOMATIC:
			if (!router_v2->read_point_layer (filename_points, record_limit, normalize_longitude, text_column_indexes, skip_column_indexes, id_column_index, lat_column_index, lon_column_index, layer_points, map_watershed, log))
				error = true;
			break;
		case CSV_POINT_FORMAT_ID_LAT_LONG:
			if (!router_v2->read_points_csv (filename_points, normalize_longitude, numeric_id, true, layer_points, map_watershed, log))
				error = true;
	}

	if (!error) {
		// 2020-10-19 return point layer if successful
		map_watershed->layers.push_back(layer_points);
		return layer_points;
	}
	else {
		delete layer_points;
		return NULL;
	}
}

void frame_CARDGIS::OnReadPointLayer
	(wxCommandEvent&)

{
	dynamic_string log, filename_points;
	BYTE file_format;
	map_layer *new_layer;
	bool normalize_longitude, numeric_id;
	Ledger preview_rows;
	std::set <int> text_column_indexes, skip_column_indexes;
	int id_column_index = -1, lat_column_index = -1, lon_column_index = -1;

	// filename_points = "R:\\DKEISER\\wastewater\\cwns_latlon.csv";

	// 2016-11-03 Lake Erie Secchi data
	// ReadingNo,Date,Date2,Site,Secchi (m),Station,Site_code,Lat,Lon
	// 1,2011-05-05,40668,14-972,1.2,,,41.571,-82.77083333
	// 2,2011-05-19,40682,14-972,0.3,,,41.571,-82.77083333
	/*
	file_format = CSV_POINT_FORMAT_SPECIFIED;
	filename_points = "I:\\TDC\\LakeErie\\Secchi Depths\\Ohio_Lake_Erie_Water_Clarity_2011_2013.csv";
	lat_column_index = 2;
	lon_column_index = 3;
	text_column_indexes.insert (1);
	text_column_indexes.insert (3);
	normalize_longitude = false;
	*/

	file_format = CSV_POINT_FORMAT_AUTOMATIC;
	// filename_points = "I:\\TDC\\BHO\\Received 2020-10-14\\Monitoring_satations_Lat_Long.csv";
	filename_points = "I:\\TDC\\BHO\\Received 2021-07-26\\Water_Reservoirs.csv";
	// lat_column_index = 1;
	// lon_column_index = 2;
	normalize_longitude = false;
	numeric_id = false; // 2020-10-19 BHO measuring stations have to retain numeric ID

	dialog_import_points dip (this, &filename_points, "", map_watershed, &file_format, &normalize_longitude, &numeric_id, &preview_rows, &text_column_indexes, &skip_column_indexes,
	&id_column_index, &lat_column_index, &lon_column_index);
	if (dip.ShowModal () == wxID_OK) {

		SetCursor (*wxHOURGLASS_CURSOR);
		new_layer = read_point_layer (filename_points, -1, normalize_longitude, numeric_id, file_format, &preview_rows.column_names, &text_column_indexes, &skip_column_indexes,
		id_column_index, lat_column_index, lon_column_index, log);
		map_watershed->set_extent ();

		panel_watershed->pause_rendering = false;
		if (new_layer) {
			panel_watershed->change_layers ();
			panel_watershed->change_selection_layer (new_layer);
		}
		enable_map_controls ();
		panel_watershed->redraw ("OnReadPointLayer");
		SetCursor (*wxSTANDARD_CURSOR);

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Point Layer", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnLinkPointLayerThreaded(wxCommandEvent &)

{
	dynamic_string log, report;
	bool write_agency_column = false; // true for STORET legacy file with Agency in attributes_text [0]
	bool ponto = false; // true for BHO Pontodrenagem

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)) {
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		map_layer *layer_points = panel_watershed->layer_selection_source;
		router_NHD * router = relevent_router();
		map_layer* layer_rivers = router->river_map_layer; // map_watershed->match("Rivers");

		if (layer_points->name.match_insensitive ("Pontodrenagem") != -1)
			ponto = true;

		if (layer_points->attribute_count_numeric < LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC) {
			int new_size_numeric;

			new_size_numeric = layer_points->attribute_count_numeric > LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC ? layer_points->attribute_count_numeric : LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;

			log += "Resizing attributes of layer \"";
			log += layer_points->name;
			log.add_formatted ("\" from %d,%d to %d,%d.\n",
			layer_points->attribute_count_numeric, layer_points->attribute_count_text, new_size_numeric, layer_points->attribute_count_text);

			layer_points->resize (new_size_numeric, layer_points->attribute_count_text);
		}

		log += layer_points->name;
		log += "\n";
		router->link_points_threaded (layer_points, app->multithread_count, layer_rivers, write_agency_column, ponto, &view, report, log);
		log += report;
	}
	else
		log += "ERROR, no selected layer, or layer is not POINT type.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Linked Points", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnLinkPointLayer (wxCommandEvent &)

{
	dynamic_string log, report;

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)) {
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		router_NHD* router = relevent_router();
		map_layer* layer_rivers = router->river_map_layer; // map_watershed->match("Rivers");

		log += panel_watershed->layer_selection_source->name;
		log += "\n";
		router->link_points (panel_watershed->layer_selection_source, layer_rivers, &view, report, log);
		log += report;
	}
	else
		log += "ERROR, no selected layer, or layer is not POINT type.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Linked Points", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnWriteLinkedPointLayer (wxCommandEvent &)

{
	dynamic_string log, filename_network;

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)) {
		filename_network = filename_nhd_area;
		filename_network += "Points.bin";

		if (get_write_path (&filename_network, L"Binary Point Files (*.bin)|*.bin", L"*.bin", NULL)) {

			relevent_router ()->write_linked_points_binary_storet(filename_network, panel_watershed->layer_selection_source, log);

			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"Write Linked Sample Stations", L"", log);
			d.ShowWindowModal ();
		}
	}
	else
		log += "ERROR, no selected layer, or layer is not POINT type.\n";
}

void frame_CARDGIS::OnReadLinkedPointLayer (wxCommandEvent &)

{
	dynamic_string log, filename_points;
	map_layer *layer_points;

	if (get_read_path (&filename_points, L"Linked Point Files (*.bin)|*.bin", NULL)) {

		layer_points = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_points->name = filename_points;
		layer_points->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
		layer_points->initialize_attributes = true;
		layer_points->color = COLOR_FACILITIES_OTHER;
		layer_points->symbol = MAP_OBJECT_SYMBOL_X; // MAP_OBJECT_SYMBOL_CIRCLE;
		layer_points->symbol_size = 6;
		layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		map_watershed->layers.push_back (layer_points);

		// router->read_linked_points_binary (filename_points, layer_points, log);
		relevent_router () ->read_linked_points_binary_storet(filename_points, layer_points, log);

		enable_map_controls ();

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Read Linked Points", L"", log);
		d.ShowWindowModal ();
	}
}

/*
void frame_CARDGIS::OnReadSTORETModern
	(wxCommandEvent&)

{

	dynamic_string log, filename_stations;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	filename_stations = "R:\\DKEISER\\water_quality_stations\\storet_modern_stations 2015-03-24.csv";

	router->read_sampling_stations (filename_stations, layer_storet_modern, map_watershed, log);

	enable_map_controls ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"STORET Modern Stations", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnLinkSTORETModern
	(wxCommandEvent&)

{

	dynamic_string report, log;

	if (layer_storet_modern) {
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		log += layer_storet_modern->name;
		log += "\n";
		router->link_points (layer_storet_modern, app->multithread_count, layer_rivers, &view, report, log);
		log += report;
	}
	else
		log += "ERROR, no STORET modern layer.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"STORET Modern Stations", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnWriteSTORETModernBinary
	(wxCommandEvent&)
{
	dynamic_string log, filename_network;

	filename_network = filename_nhd_area;
	filename_network += "STORETModern.bin";

	if (get_write_path (&filename_network, L"Binary Station Files (*.bin)|*.bin", L"*.bin", NULL)) {

		router->write_linked_points_binary_storet (filename_network, layer_storet_modern, log);

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Write Linked STORET Legacy Stations", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnReadSTORETModernBinary
	(wxCommandEvent&)
{
	dynamic_string log, filename_network;

	filename_network = filename_nhd_area;
	filename_network += "STORETModern.bin";

	router->read_linked_points_binary_storet (filename_network, layer_storet_modern, log);

	map_watershed->set_extent ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Read Linked Sample Stations", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnReadSTORETLegacy
	(wxCommandEvent&)

{

	dynamic_string log, filename_stations;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	filename_stations = "R:\\DKEISER\\water_quality_stations\\storet_legacy_stations 2015-04-03.csv";

	router->read_storet_legacy (filename_stations, layer_storet_legacy, map_watershed, log);

	enable_map_controls ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"STORET Legacy Stations", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnLinkSTORETLegacy
	(wxCommandEvent&)

{

	dynamic_string report, log;

	if (layer_storet_legacy) {
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		log += layer_storet_legacy->name;
		log += "\n";
		router->link_points (layer_storet_legacy, app->multithread_count, layer_rivers, &view, report, log);
		log += report;
	}
	else
		log += "ERROR, no STORET legacy layer.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"STORET Legacy Stations", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnWriteSTORETLegacyBinary
	(wxCommandEvent&)
{
	dynamic_string log, filename_network;

	filename_network = filename_nhd_area;
	filename_network += "STORETLegacy.bin";

	if (get_write_path (&filename_network, L"Binary Station Files (*.bin)|*.bin", L"*.bin", NULL)) {

		router->write_linked_points_binary_storet (filename_network, layer_storet_legacy, log);

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Write Linked STORET Legacy Stations", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnReadSTORETLegacyBinary
	(wxCommandEvent&)
{
	dynamic_string log, filename_network;

	filename_network = filename_nhd_area;
	filename_network += "STORETLegacy.bin";

	router->read_linked_points_binary_storet (filename_network, layer_storet_legacy, log);

	map_watershed->set_extent ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Read Linked Sample Stations", L"", log);
	d.ShowWindowModal ();
}
void frame_CARDGIS::OnLinkSamplingStations
	(wxCommandEvent&)

{

	dynamic_string log, report;

	if (layer_sampling_stations) {
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		log += layer_sampling_stations->name;
		log += "\n";
		router->link_points (layer_sampling_stations, app->multithread_count, layer_rivers, &view, report, log);
		log += report;
	}
	else
		log += "ERROR, no sampling stations layer.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Water Quality Sampling Stations", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnReadLinkedUSGS
	(wxCommandEvent&)
{
	dynamic_string log, filename_network;

	filename_network = filename_nhd_area;
	filename_network += "USGS.bin";

	router->read_linked_points_binary (filename_network, layer_sampling_stations, log);

	map_watershed->set_extent ();


	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Read Linked Sample Stations", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnWriteLinkedUSGS
	(wxCommandEvent&)
{
	dynamic_string log, filename_network;

	filename_network = filename_nhd_area;
	filename_network += "USGS.bin";

	if (get_write_path (&filename_network, L"Binary Station Files (*.bin)|*.bin", L"*.bin", NULL)) {

		router->write_linked_points_binary (filename_network, layer_sampling_stations, log);

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Write Linked Sample Stations", L"", log);
		d.ShowWindowModal ();
	}
}
*/

void frame_CARDGIS::OnSelectID
	(wxCommandEvent&)

// Find & select segment matching id

{
	dynamic_string log;
	long long id;
	std::set <long long> new_selection;

	SetCursor (*wxHOURGLASS_CURSOR);

	id = edit_select_id->Get ();
	new_selection.insert (id);

	if (!panel_watershed->replace_selection (&new_selection, log)) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Select by id", L"", log);
		d.ShowWindowModal ();
	}

	SetCursor (*wxSTANDARD_CURSOR);
}

void frame_CARDGIS::OnSelectName
	(wxCommandEvent&)

// Find & select segment matching id

{
	dynamic_string log, match_text;
	std::set <long long> new_selection;
	std::vector <map_object *> ::iterator polygon;

	SetCursor (*wxHOURGLASS_CURSOR);

	match_text = edit_select_id->GetValue ().ToAscii ();

	if (panel_watershed->layer_selection_source) {
		for (polygon = panel_watershed->layer_selection_source->objects.begin ();
		polygon != panel_watershed->layer_selection_source->objects.end ();
		++polygon) {
			if ((*polygon)->name == match_text)
				new_selection.insert ((*polygon)->id);
		}

		if (new_selection.size () > 0) {
			std::set <long long>::iterator p;
			if (new_selection.size () > 1) {
				log.add_formatted ("%d map objects matched.\n", (int) new_selection.size ());
				for (p = new_selection.begin ();
				p != new_selection.end ();
				++p)
					log.add_formatted ("\t%lld\n", *p);
			}

			if (!panel_watershed->replace_selection (&new_selection, log)) {
			}
		}
		else {
			log += "ERROR, can't find value \"";
			log += match_text;
			log += "\" in object names.\n";
		}
		SetCursor (*wxSTANDARD_CURSOR);

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"Select by id", L"", log);
			d.ShowWindowModal ();
		}
	}
}

void frame_CARDGIS::OnMatchAttribute
	(wxCommandEvent&)

// Find & select segment matching text or numeric attribute

{
	dynamic_string log, attribute;
	std::set <long long> new_selection;

	if (panel_watershed->layer_selection_source) {
		dialog_match_attribute dp (this, panel_watershed->layer_selection_source, &new_selection);
		if (dp.ShowModal () == wxID_OK) {
			if (!panel_watershed->replace_selection (&new_selection, log)) {
				log.convert_linefeeds_for_CEdit ();
				dialog_error_list d (this, L"Select by id", L"", log);
				d.ShowWindowModal ();
			}
		}
	}
}

void frame_CARDGIS::OnMatchName
	(wxCommandEvent&)

// Find & select segment matching name

{
	dynamic_string log, match_text;
	std::set <long long> new_selection;
	std::vector <map_object *>::const_iterator polygon;

	if (panel_watershed->layer_selection_source) {
		dialog_prompt dp (this, "Enter text", &match_text);
		if (dp.ShowModal () == wxID_OK) {
			SetCursor (*wxHOURGLASS_CURSOR);

			for (polygon = panel_watershed->layer_selection_source->objects.begin ();
			polygon != panel_watershed->layer_selection_source->objects.end ();
			++polygon) {
				if ((*polygon)->name == match_text)
					new_selection.insert ((*polygon)->id);
			}

			if (new_selection.size () > 0) {
				std::set <long long>::iterator p;
				log.add_formatted ("%d map objects matched.\n", (int) new_selection.size ());
				for (p = new_selection.begin ();
				p != new_selection.end ();
				++p)
					log.add_formatted ("\t%lld\n", *p);

				if (!panel_watershed->replace_selection (&new_selection, log)) {
				}
			}
			else {
				log += "ERROR, can't find value \"";
				log += match_text;
				log += "\" in object names.\n";
			}
			SetCursor (*wxSTANDARD_CURSOR);

			if (log.get_length () > 0) {
				log.convert_linefeeds_for_CEdit ();
				dialog_error_list d (this, L"Select by id", L"", log);
				d.ShowWindowModal ();
			}
		}
	}
}

void frame_CARDGIS::OnZoomSelection
	(wxCommandEvent&)

{
	panel_watershed->zoom_selection ();
}

void frame_CARDGIS::OnZoomIn
	(wxCommandEvent&)

{
	panel_watershed->start_zoom ();
}

void frame_CARDGIS::OnZoomOut
	(wxCommandEvent&)

{
	panel_watershed->zoom_out ();
}

void frame_CARDGIS::OnReadFlowlineVAA
	(wxCommandEvent&)

// Second file with Divergence code

{

	dynamic_string log;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	router_v2->read_flow_VAA (filename_nhd_area, &view, log);

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Build NHD network", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnReadElevation
	(wxCommandEvent&)

// Min & max elevation by COMID

{

	dynamic_string log;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	router_v2->read_NED_elevation (filename_nhd_area, &area_elevation_minimum_m, &area_elevation_maximum_m, &view, log);

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Build NHD network", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnSelectLayer
	(wxCommandEvent &)

{
	// int index = combo_select_layer->GetSelection ();
	// wxString items [3] = {"NHD Flowlines", "Treatment Facilities", "USGS Sample Stations"};
	int index;
	
	if ((index = combo_select_layer->GetSelection ()) != -1) {
		map_layer *new_selection;

		new_selection = (map_layer *) combo_select_layer->GetClientData (index);
		// Have to ignore selection layer since it's not displayed in combo
		panel_watershed->change_selection_layer (new_selection);
		// enable_map_controls (); // causes loop back via EVT_COMBOBOX COMBO_SELECT_LAYER

		SetStatusText (panel_watershed->layer_selection_source->name.get_text ());
	}
}

/*
void frame_CARDGIS::OnEditMapLayers
	(wxCommandEvent &)

{
		layer_rivers->data_attribute_index = index;
		layer_rivers->colors = &attribute_color_sets_river [index];
		panel_watershed->redraw ("OnSelectColorAttribute");

		if (index == NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH) {
			layer_rivers->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR_MODULO;
			layer_rivers->draw_as_modulo = 16;
		}
		else
			if ((index == NHD_SEGMENT_ATTRIBUTE_INDEX_MINELEVM)
			|| (index == NHD_SEGMENT_ATTRIBUTE_INDEX_MAXELEVM)) {
				// recompute min & max elevation ranges for attribute_color_sets_river
				color_set_definition *range;
				double elevation_step = (area_elevation_maximum_m - area_elevation_minimum_m) / (double) attribute_color_sets_river [index].size ();
				int division;

				range = (color_set_definition *) attribute_color_sets_river [index].first_member ();
				division = 0;
				while (range) {
					range->ge = area_elevation_minimum_m + elevation_step * (double) division;
					range->lt = area_elevation_minimum_m + elevation_step * (double) (division + 1);
					range = (color_set_definition *) range->next;
					++division;
				}
				layer_rivers->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
			}
			else
				layer_rivers->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
}
*/

void frame_CARDGIS::OnRouteFacilities
	(wxCommandEvent&)

{
	map_layer *layer_from, *layer_to;
	dynamic_string log, filename_output;
	bool clip1000m = true, output_file = false, write_station_names = false;
	char output_delimiter = '\t';

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	dialog_route_points drp (this, map_watershed, &layer_from, &layer_to, &output_file, &filename_output, &clip1000m, &output_delimiter, &write_station_names);
	if (drp.ShowModal () == wxID_OK) {

		router_NHD* router = relevent_router();

		if ((layer_from->type == MAP_OBJECT_POINT)
		&& (layer_to->type == MAP_OBJECT_POINT)) {
			log += layer_from->name;
			log += "\n";
			log += layer_to->name;
			log += "\n";
			if (output_file)
				router->route_facilities (layer_from, layer_to, clip1000m, &view, &filename_output, output_delimiter, write_station_names, log);
			else
				router->route_facilities (layer_from, layer_to, clip1000m, &view, NULL, output_delimiter, write_station_names, log);
		}
		else
			log += "ERROR, layer type is not POINT.\n";

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Route Table", L"", log);
		d.ShowWindowModal ();
	}
}

bool write_log
	(dynamic_string &filename,
	dynamic_string &log)

{
	FILE *f;
	
	if (fopen_s (&f, filename.get_text_ascii (), "w") == 0) {
		fwrite (log.get_text_ascii (), log.get_length (), 1, f);
		fclose (f);
		return true;
	}
	else
		return false;
}

void frame_CARDGIS::OnRouteFacilitiesThreaded
	(wxCommandEvent&)

{

	dynamic_string log, report, *filename_output;
	map_layer *layer_from, *layer_to;
	int index;
	bool clip1000m = true, output_file = false, write_station_name = false;
	char output_delimiter = '\t';

	filename_output = new dynamic_string;
	*filename_output = "f:\\scratch\\bho\\Ponto Rede Routed.csv";

	dialog_route_points drp (this, map_watershed, &layer_from, &layer_to, &output_file, filename_output, &clip1000m, &output_delimiter, &write_station_name);
	if (drp.ShowModal () == wxID_OK) {
		router_NHD* router = relevent_router();

		if (!output_file) {
			delete filename_output;
			filename_output = NULL;
		}

		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		if ((layer_from->type == MAP_OBJECT_POINT)
		&& (layer_to->type == MAP_OBJECT_POINT)) {
			log += layer_from->name;
			log += "\n";
			log += layer_to->name;
			log += "\n";
			router->route_facilities_threaded (app->multithread_count, layer_from, layer_to, clip1000m, &view, filename_output, output_delimiter, write_station_name, report, log);
		}
		else
			log += "ERROR, layer type is not POINT.\n";

		{
			filename_struct fname_struct;
			dynamic_string filename = filename_nhd_area, prefix;

			fname_struct.parse (&layer_to->name);
			prefix = fname_struct.prefix;
			if ((index = prefix.match (" ")) != -1)
				prefix = prefix.left (index);

			filename += "Facilites";
			filename += prefix;
			filename += "-";
			// filename += area_name;
			filename += ".txt";
			write_log (filename, report);
		}

		view.destroy_dialog_progress ();
		if (filename_output)
			delete filename_output;

		log += report;
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Route Table", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnRouteFacilitiesRiver
	(wxCommandEvent&)

// Route a single point in layer_from to all points in layer_to

{
	dynamic_string log, filename_output;
	map_layer *layer_from, *layer_to;
	long facility_id = 230001001; // 330091001;
	bool clip1000m = true, output_file = false, write_station_names = false;
	char output_delimiter = '\t';

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	router_NHD* router = relevent_router();
	map_layer *layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	if (layer_rivers) {
		dialog_route_points drp (this, map_watershed, &layer_from, &layer_to, &output_file, &filename_output, &clip1000m, &output_delimiter, &write_station_names);
		if (drp.ShowModal () == wxID_OK) {
			if ((layer_from->type == MAP_OBJECT_POINT)
			&& (layer_to->type == MAP_OBJECT_POINT)) {

				dialog_prompt dp (this, "Enter 'From' point ID", &facility_id);
				if (dp.ShowModal () == wxID_OK) {
					log += layer_from->name;
					log += "\n";
					log += layer_to->name;
					log += "\n";

					if (output_file)
						router->route_facilities (facility_id, layer_rivers, layer_from, layer_to, clip1000m, &view, &filename_output, output_delimiter, log);
					else
						router->route_facilities (facility_id, layer_rivers, layer_from, layer_to, clip1000m, &view, NULL, output_delimiter, log);
				}
			}
			else
				log += "ERROR, no selection layer, or layer type is not POINT.\n";
		}
		else
			log += "ERROR, no facilities layer.\n";
	}
	else
		log += "ERROR, no rivers layer.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnReadFlowNetwork
	(wxCommandEvent&)
{
	dynamic_string log, filename_network;

	// filename_network = "R:\\NHDPlusv2\\NHDPlus01\\Network.bin";
	filename_network = "H:\\NHDPLUS_HR\\NHDPlus_HR_Area01.bin";

	if (get_read_path(&filename_network, L"Binary Network Files (*.bin)|*.bin", NULL)) {
		router_NHD* router = relevent_router();
		router->rivers.read_long_long (filename_network, log);

		// NHDPlus_HR
		router->ftype_coastline = 566;

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"Read Flow Network", L"", log);
			d.ShowWindowModal ();
		}
	}
}

void frame_CARDGIS::OnWriteFlowNetwork
	(wxCommandEvent&)
{
	dynamic_string log, filename_network;

	filename_network = filename_nhd_area;
	filename_network += "Network.bin";

	if (get_write_path (&filename_network, L"Binary Network Files (*.bin)|*.bin", L"*.bin", NULL)) {
		router_NHD* router = relevent_router();
		router->rivers.write (filename_network, log);

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Write Flow Network", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnListUpstream
	(wxMenuEvent &)

{
	map_object *branch;

	if (panel_watershed->layer_selection->objects.size () > 0) {
		branch = panel_watershed->layer_selection->objects [0];

		std::vector <long long> comids;
		std::vector <long long>::const_iterator id;
		dynamic_string log;
		interface_window_wx update_display;

		SetCursor (*wxHOURGLASS_CURSOR);

		update_display.start_progress (this);

		log += "Upstream\n";
		relevent_router ()->rivers.accumulate_upstream(branch->id, &comids);

		for (id = comids.begin ();
		id != comids.end ();
		++id)
			log.add_formatted ("%ld\n", *id);

		SetCursor (*wxSTANDARD_CURSOR);
		if (log.get_length () > 0) {

			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"River", L"", log);
			d.ShowWindowModal ();
		}
	}
}

void frame_CARDGIS::OnListDownstream
	(wxMenuEvent &)

{
	map_object *branch;
	
	if (panel_watershed->layer_selection->objects.size () > 0) {
		std::vector <long long> comids;
		std::vector <long long>::iterator comid;
		branch = panel_watershed->layer_selection->objects [0];

		dynamic_string log;
		interface_window_wx update_display;

		SetCursor (*wxHOURGLASS_CURSOR);

		update_display.start_progress (this);
		router_NHD* router = relevent_router();

		{
			flow_network_link* leaf;

			if ((leaf = router->rivers.mouth_of_river(branch->id)) != NULL) {
				log.add_formatted("Mouth for %lld is leaf %lld.\n", branch->id, leaf->id);
			}
		}

		log += "Downstream\n";
		// router->rivers.write_downstream (branch->id, panel_watershed->layer_selection_source, log);
		router->rivers.accumulate_downstream_ordered (branch->id, &comids);
		for (comid = comids.begin (); comid != comids.end (); ++comid)
			log.add_formatted ("%lld\n", *comid);

		SetCursor (*wxSTANDARD_CURSOR);
		if (log.get_length () > 0) {

			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"River", L"", log);
			d.ShowWindowModal ();
		}
	}
}

void frame_CARDGIS::OnListDownstreamFromCOMIDs
	(wxCommandEvent&)

// 2023-04-27 For Yongjie, read a list of COMIDs filtered to be headers (no FROM ID)
// For each produce a list of downstream COMIDs.

{
	dynamic_string filename_source, filename_output, log;
	FILE *file_input, *file_csv;
	bool error = false;
	long record_count = 0;
	filename_struct filenamer;

	filename_source = "F:\\scratch\\Yongjie\\flowline_headcomids.csv";

	if (get_read_path(&filename_source, L"Text File (*.csv)|*.csv", NULL)) {
		char data [256];
		long long headwater;
		std::vector <long long> comids;
		std::vector <long long>::iterator comid;

		interface_window_wx update_display;

		SetCursor(*wxHOURGLASS_CURSOR);

		update_display.start_progress(this);

		router_NHD* router = relevent_router();

		if (fopen_s(&file_input, filename_source.get_text_ascii(), "r") == 0) {

			filenamer.parse (&filename_source);
			filename_output = filenamer.prefix;
			filename_output += "_Downstream";
			filenamer.set_prefix (filename_output.get_text ());
			filenamer.write_file_specification (&filename_output);

			if (fopen_s(&file_csv, filename_output.get_text_ascii(), "w") == 0) {

				fprintf(file_csv, "COMID,Downstream COMID\n");

				if (fgets(data, 256, file_input)) {
					while (fgets(data, 256, file_input)) {

						headwater = atoll (data);
						comids.clear ();
						router->rivers.accumulate_downstream_ordered (headwater, &comids);

						if (comids.size () > 0) {
							fprintf(file_csv, "%lld", headwater);
							for (comid = comids.begin(); comid != comids.end(); ++comid)
								fprintf(file_csv, ",%lld\n", *comid);
						}
						/*
						else
							fprintf(file_csv, "%ld\n", headwater);
						*/
						if ((++record_count % 1000) == 0)
							update_display.update_progress_formatted(1, "COMIDs %ld", record_count);
					}
				}

				fclose(file_csv);
			}
			else {
				error = true;
				log += "ERROR, can't open output CSV file \"";
				log += filename_output;
				log += "\"\n";
			}
			fclose (file_input);
		}
		else {
			error = true;
			log += "ERROR, can't open output COMID input file \"";
			log += filename_source;
			log += "\"\n";
		}

		SetCursor(*wxSTANDARD_CURSOR);

		if (log.get_length() > 0) {

			log.convert_linefeeds_for_CEdit();
			dialog_error_list d(this, L"River", L"", log);
			d.ShowWindowModal();
		}
	}
}

void frame_CARDGIS::OnImport_NHDPlus_HR_geometry
	(wxCommandEvent&)

// 2023-05-04 Python script "R:\router_NHDPlus_HR\Python\router_NHDPlus_HR_export07Geometry.py" writes two files, one with network flow and one with point geometry.
// Import as a new map layer

{
	dynamic_string filename_geometry, log;
	map_layer *new_layer;
	bool error = false;

	filename_geometry = "H:\\router_NHDPlus_HR\\output01Geometry.txt";

	if (get_read_path(&filename_geometry, L"Text File (*.txt)|*.txt", NULL)) {
		interface_window_wx update_display;
		update_display.start_progress(this);

		new_layer = create_river_layer (map_watershed, true, false);

		if (router_hr == NULL)
			router_hr = new router_NHD;

		router_hr->river_map_layer = new_layer;

		if (!router_hr->read_NHDPlus_HR_geometry(filename_geometry, new_layer, &update_display, log))
			error = true;

		map_watershed->set_extent();

		if (new_layer) {
			panel_watershed->change_layers();
			panel_watershed->change_selection_layer(new_layer);
		}
		enable_map_controls();
		panel_watershed->redraw("OnImport_NHDPlus_HR_geometry");

		update_display.destroy_dialog_progress ();

		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"New Layer", L"", log);
		d.ShowWindowModal();
	}
}

void frame_CARDGIS::OnImport_NHDPlus_HR_network
(wxCommandEvent&)

// 2023-05-04 Python script "R:\router_NHDPlus_HR\Python\router_NHDPlus_HR_export07Geometry.py" writes two files, one with network flow and one with point geometry.
// Import as a new map layer

{
	dynamic_string filename_network, log;
	map_layer* nhd_layer;
	bool error = false;
	const int record_limit = -1;

	filename_network = "H:\\router_NHDPlus_HR\\output01Network.txt";

	if (get_read_path(&filename_network, L"Text File (*.txt)|*.txt", NULL)) {
		interface_window_wx update_display;
		update_display.start_progress(this);

		if ((nhd_layer = map_watershed->match ("Rivers")) == NULL)
			nhd_layer = *(map_watershed->layers.begin ());

		// https://lab.data.ca.gov/dataset/nhd-major-rivers-major-rivers-and-creeks-major-lakes-and-reservoirs/6dab5b08-6fff-4532-87fd-f8b3d4336acb
		router_hr->ftype_coastline = 566;

		router_hr->read_flow_and_build_network_NHDPlus_HR(filename_network, nhd_layer, record_limit, &update_display, log);

		panel_watershed->redraw("OnImport_NHDPlus_HR_network");

		update_display.destroy_dialog_progress();

		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"New Layer", L"", log);
		d.ShowWindowModal();
	}
	else
		error = true;
}

void frame_CARDGIS::select_overlapping
	(class bounding_cube *extent,
	const class map_layer *source,
	class map_layer *destination,
	class map_layer *not_including)

// select objects in source that overlap destination, but which aren't in layer "not_including"

{
	std::vector <map_object *>::const_iterator segment;
	map_object *copy;

	for (segment = source->objects.begin ();
	segment != source->objects.end ();
	++segment) {
		if ((*segment)->overlaps_logical (extent)) {
			if (not_including->match_id ((*segment)->id) == NULL) {
				copy = destination->create_new ((*segment)->type);
				copy->copy (*segment, destination);
				destination->objects.push_back (copy);
			}
		}
	}
}

void frame_CARDGIS::select_entire_river
	(const long long first_id,
	const map_layer *source,
	map_layer *destination,
	dynamic_string &log)

{
	std::set <long long> upstream_ids;
	std::set <long long>::const_iterator id;
	map_object *segment, *copy;
	flow_network_link *mouth;
	router_NHD *router = relevent_router ();

	// Add tree to dialog_river.map

	if ((mouth = router->rivers.mouth_of_river (first_id)) != NULL) {
		router->rivers.accumulate_upstream (mouth->id, &upstream_ids);

		// Both sets include first_id, so remove it from one
		upstream_ids.erase (first_id);

		for (id = upstream_ids.begin ();
		id != upstream_ids.end ();
		++id) {
			segment = source->match_id (*id);
			copy = destination->create_new (segment->type);
			copy->copy (segment, destination);
			destination->objects.push_back (copy);
		}
	}
	else
		log.add_formatted ("ERROR, mouth of river not found for segment COMID %lld.\n", first_id);
}

/*
void frame_CARDGIS::OnShowRiver
	(wxCommandEvent&)
{
	
	if (panel_watershed->layer_selection
	&& (panel_watershed->layer_selection->objects.size () > 0)) {

		SetCursor (*wxHOURGLASS_CURSOR);
		dynamic_string log;
		bounding_cube extent;
		std::vector <map_object *>::const_iterator point;
		map_object *copy;

		dialog_river d (this, &river, attribute_color_sets_river);

		d.create_map_layers ();

		if (panel_watershed->layer_selection_source->name == "Rivers") {
			map_object *branch = panel_watershed->layer_selection->objects [0];
			select_entire_river (branch->id, layer_rivers, d.streams, log);
		}

		// select facilities in this region
		for (point = layer_facilities->objects.begin ();
		point != layer_facilities->objects.end ();
		++point) {
			if (extent.contains ((*point)->latitude, (*point)->longitude)) {
				copy = d.facilities->create_new ((*point)->type);
				copy->copy (*point, d.facilities);
				copy->name.format ("%ld", copy->id);
				d.facilities->objects.push_back (copy);
			}
		}

		// select facilities in this region
		for (point = layer_sampling_stations->objects.begin ();
		point != layer_sampling_stations->objects.end ();
		++point) {
			if (extent.contains ((*point)->latitude, (*point)->longitude)) {
				copy = d.stations->create_new ((*point)->type);
				copy->copy (*point, d.stations);
				copy->name.format ("%ld", copy->id);
				d.stations->objects.push_back (copy);
			}
		}

		d.set_map ();

		SetCursor (*wxSTANDARD_CURSOR);

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list de (this, L"River", L"", log);
			de.ShowWindowModal ();
		}

		d.ShowWindowModal ();
	}
}
*/

void frame_CARDGIS::OnSelectUpstream
	(wxMenuEvent &)

{
	map_object *branch;
	dynamic_string log;
	
	if (panel_watershed->layer_selection->objects.size () > 0) {
		branch = panel_watershed->layer_selection->objects [0];

		SetCursor (*wxHOURGLASS_CURSOR);

		std::set <long long> upstream_ids;
		std::set <long long>::const_iterator id;

		relevent_router()->rivers.accumulate_upstream (branch->id, &upstream_ids);

		if (!panel_watershed->replace_selection (&upstream_ids, log)) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"River", L"", log);
			d.ShowWindowModal ();
		}

		SetCursor (*wxSTANDARD_CURSOR);
	}
}

router_NHD* frame_CARDGIS::relevent_router()

{

	if (router_hr
	&& (router_hr->river_map_layer == panel_watershed->layer_selection_source))
		return router_hr;
	else
		return router_v2;
}

void frame_CARDGIS::OnSelectDownstream
	(wxMenuEvent &)

{
	map_object *branch;
	dynamic_string log;
	
	if (panel_watershed->layer_selection->objects.size () > 0) {
		branch = panel_watershed->layer_selection->objects [0];

		SetCursor (*wxHOURGLASS_CURSOR);

		std::set <long long> downstream_ids;

		// determine which router based on layer_selection
		relevent_router ()->rivers.accumulate_downstream(branch->id, &downstream_ids);

		if (!panel_watershed->replace_selection (&downstream_ids, log)) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"River", L"", log);
			d.ShowWindowModal ();
		}

		SetCursor (*wxSTANDARD_CURSOR);
	}
}

void frame_CARDGIS::OnClosestSegment
	(wxMenuEvent &)

// If Selected layer is a point layer, find nearest NHD segement for each point

{
	dynamic_string log;
	std::vector <map_object *>::const_iterator segment;
	std::vector <map_object *>::const_iterator polyline;
	int point_index, best_point_index = 0;
	double distance_m, best_distance;
	map_object *closest_segment = NULL;
	device_coordinate p;

	best_distance = DBL_MAX;

	map_layer *layer_rivers = map_watershed->match ("Rivers");

	if (layer_rivers) {
		for (segment = panel_watershed->layer_selection->objects.begin ();
		segment != panel_watershed->layer_selection->objects.end ();
		++segment) {
			(*segment)->dump (panel_watershed->layer_selection, log);

			if ((*segment)->type == MAP_OBJECT_POINT) {

				for (polyline = layer_rivers->objects.begin ();
				polyline != layer_rivers->objects.end ();
				++polyline) {
					p.x = (*segment)->longitude;
					p.y = (*segment)->latitude;

					distance_m = ((map_polygon *) (*polyline))->closest_point_meters (p, &point_index);
					if (distance_m < best_distance) {
						best_distance = distance_m;
						best_point_index = point_index;
						closest_segment = *polyline;
					}
				}
			}
		}

		if (closest_segment) {
			closest_segment->dump (panel_watershed->layer_selection, log);
			log.add_formatted ("Best Point Index\t%d", best_point_index);
			if (closest_segment->type == MAP_OBJECT_POLYGON)
				log.add_formatted ("\t%ld\t%ld", ((map_polygon *) closest_segment)->nodes [best_point_index * 2 + 1], ((map_polygon *) closest_segment)->nodes [best_point_index * 2]);
			log += "\n";
		}
	}
	else
		log += "ERROR, no rivers layer.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"River", L"", log);
	d.ShowWindowModal ();
}

void append_vector
	(std::vector <long long> *destination,
	std::vector <std::pair <long long, double>> *source,
	const bool reverse_source)

// last member of source is also last of destination

{
	if (reverse_source) {
		std::vector <std::pair <long long, double>>::const_reverse_iterator div_id;

		for (div_id = source->rbegin ();
		div_id != source->rend ();
		++div_id)
			destination->push_back (div_id->first);
	}
	else {
		std::vector <std::pair <long long, double>>::const_iterator div_id;

		for (div_id = source->begin ();
		div_id != source->end ();
		++div_id)
			destination->push_back (div_id->first);
	}
}

void prepend_vector
	(std::vector <long long> *destination,
	std::vector <std::pair <long long, double>> *source,
	const bool reverse_source)

// last member of source is also last of destination

{
	if (reverse_source) {
		std::vector <std::pair <long long, double>>::const_reverse_iterator div_id;

		for (div_id = source->rbegin ();
		div_id != source->rend ();
		++div_id)
			destination->insert (destination->begin (), div_id->first);
	}
	else {
		std::vector <std::pair <long long, double>>::const_iterator div_id;

		for (div_id = source->begin ();
		div_id != source->end ();
		++div_id)
			destination->insert (destination->begin (), div_id->first);
	}
}

void frame_CARDGIS::OnShowRiverSegment
	(wxCommandEvent&)

// Show a facility, a USGS station, and the river between

{
	bounding_cube extent;
	map_object *start_point, *start_point_copy, *start_point_river_segment, *destination_map_object, *destination_river_segment, *destination_copy, *river_segment, *river_segment_copy;
	device_coordinate p;
	double best_distance_m;
	int closest_polygon_index, closest_index;
	flow_network_link *facility_link, *destination_link;
	std::vector <long long> traversed_comids;
	std::vector <long long>::const_iterator river_segment_id;
	flow_network_divergence *divergence_start, *divergence_destination;
	// bool up;
	long long start_point_id, destination_id;
	map_layer *layer_from, *layer_to;
	divergence_exit_option *start_exit_down, *start_exit_up, *destination_exit_down, *destination_exit_up;
	dynamic_string log;
	bool error = false, upstream = false, ponto_start = false, ponto_end = false;
	char output_delimiter = '\t';

	// facility_id = 230060001;
	// station_id = 7485;

	// layer_from = (map_layer *) map_watershed->find (1);
	// layer_to = (map_layer *) map_watershed->find (2);

	// start_point_id = 450168001;
	// destination_id = 25725;

	// 2015-09-09 Dave Keiser: make image of one facility to station with large symbols
	// start_point_id = 230001001;
	// destination_id = 7699;

	// start_point_id = 330100003;
	// layer_from = (map_layer *) map_watershed->match ("R:\\NHDPlusv2\\NHDPlus01\\cwns_latlon.bin");
	// destination_id = 6731609;

	map_layer *layer_rivers = map_watershed->match ("Rivers");

	// set Start point to currently selected point if any
	if (panel_watershed->layer_selection) {
		std::vector <map_object *>::iterator selected_point;
		if ((selected_point = panel_watershed->layer_selection->objects.begin ()) != panel_watershed->layer_selection->objects.end ())
			start_point_id = (*selected_point)->id;
		else
			start_point_id = 0;
		layer_from = panel_watershed->layer_selection_source;
	}
	else {
		// Set layer_from and layer_to to the first two point layers found
		std::vector <map_layer*>::const_iterator layer;

		layer_from = NULL;
		layer_to = NULL;

		for (layer = map_watershed->layers.begin();
		(layer != map_watershed->layers.end())
		&& ((layer_from == NULL) || (layer_to == NULL));
		++layer) {
			if ((*layer)->type == MAP_OBJECT_POINT) {
				if (!layer_from)
					layer_from = *layer;
				else
					layer_to = *layer;
			}
		}

		start_point_id = 0;
		destination_id = 0;
	}

	// destination_id = 61373;
	// layer_to = (map_layer *) map_watershed->match ("USGS Stations");
	router_NHD *router = relevent_router ();

	dialog_river_segment drs (this, map_watershed, &layer_from, &layer_to, &start_point_id, &destination_id);
	if (drs.ShowModal () == wxID_OK) {

		if (layer_from->name.match_insensitive ("Pontodrenagem") != -1)
			ponto_start = true;
		if (layer_to->name.match_insensitive ("Pontodrenagem") != -1)
			ponto_end = true;

		SetCursor (*wxHOURGLASS_CURSOR);

		dialog_river d (this, map_watershed, false, &router->rivers, &layer_rivers->colors);

		d.create_map_layers (layer_from, layer_to);

		start_point = layer_from->match_id (start_point_id);
		if ((destination_map_object = layer_to->match_id (destination_id)) != NULL) {

			start_point_copy = d.layer_point_from->create_new (start_point->type);
			// start_point and layer_from may not have the same # of attributes expected in d.layer_point_from
			start_point_copy->copy_without_attributes (start_point);

			// 2016-09-15 Don't set attributes_numeric [3] or route_facilities_threaded_queue will believe it's a divergence
			// start_point_copy->attributes_numeric [3] = 1.0; // Matching facility color red below
			d.layer_point_from->objects.push_back (start_point_copy);

			// put sample station in d's map
			destination_copy = d.layer_point_to->create_new (destination_map_object->type);
			// destination_map_object and layer_to may not have the same # of attributes expected in d.layer_point_to
			destination_copy->copy_without_attributes (destination_map_object);

			// 2016-09-15 Don't set attributes_numeric [3] or route_facilities_threaded_queue will believe it's a divergence
			// destination_copy->attributes_numeric [3] = 2.0; // Matching station color magenta below
			d.layer_point_to->objects.push_back (destination_copy);

			// Find the nearest flow_network_link to start_point.  Use centroid so it doesn't matter what type the original layer was
			start_point_copy->centroid (&p.y, &p.x);
			if (ponto_start)
				// Pontodrenagem points are (by definition) at the intersections of river segments
				start_point_river_segment = nearest_polygon_downstream (p, layer_rivers, &router->rivers, &best_distance_m, &closest_polygon_index, &closest_index,
				router->ftype_coastline);
			else
				start_point_river_segment = nearest_polygon (p, layer_rivers, &router->rivers, &best_distance_m, &closest_polygon_index, &closest_index);
			start_point_copy->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID] = (double) start_point_river_segment->id;
			start_point_copy->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX] = (double) closest_polygon_index;
			start_point_copy->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX] = (double) closest_index;
			
			if ((facility_link = router->rivers.match_comid (start_point_river_segment->id)) == NULL) {
				// start_point is nearest a divergence.  Find it
				if ((divergence_start = router->rivers.match_divergence (start_point_river_segment->id)) != NULL)
					start_point_copy->attributes_numeric [USGS_ATTRIBUTES_INDEX_DIVERGENCE] = 2.0;

				// Find the flow_network_links just upstream & downstream from start divergence
				start_exit_up = divergence_start->nearest_upstream_link (&router->rivers.all_comids, &router->rivers.divergences, &router->river_layer_index, log);
				start_exit_down = divergence_start->nearest_downstream_link (&router->rivers.all_comids, &router->rivers.divergences, &router->river_layer_index, log);
			}
			else {
				divergence_start = NULL;
				start_exit_up = start_exit_down = NULL;
			}

			// Find the nearest flow_network_link to destination.  Use centroid so it doesn't matter what type the original layer was
			destination_copy->centroid (&p.y, &p.x);
			if (ponto_end)
				// Pontodrenagem points are (by definition) at the intersections of river segments
				destination_river_segment = nearest_polygon_downstream (p, layer_rivers, &router->rivers, &best_distance_m, &closest_polygon_index, &closest_index,
				router->ftype_coastline);
			else
				destination_river_segment = nearest_polygon (p, layer_rivers, &router->rivers, &best_distance_m, &closest_polygon_index, &closest_index);
			destination_copy->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID] = (double) destination_river_segment->id;
			destination_copy->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX] = (double) closest_polygon_index;
			destination_copy->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX] = (double) closest_index;

			if ((destination_link = router->rivers.match_comid (destination_river_segment->id)) == NULL) {
				// end_point is nearest a divergence.  Find it
				divergence_destination = router->rivers.match_divergence (destination_river_segment->id);

				// Find the flow_network_links just upstream & downstream from destination divergence
				destination_exit_up = divergence_destination->nearest_upstream_link (&router->rivers.all_comids, &router->rivers.divergences, &router->river_layer_index, log);
				destination_exit_down = divergence_destination->nearest_downstream_link (&router->rivers.all_comids, &router->rivers.divergences, &router->river_layer_index, log);
			}
			else {
				divergence_destination = NULL;
				destination_exit_up = destination_exit_down = NULL;
			}

			// Destination is upstream from start?
			if (facility_link) {
				if (divergence_destination)
					facility_link->accumulate_upstream_single_trail_ordered (destination_exit_down->link->id, &traversed_comids);
				else
					facility_link->accumulate_upstream_single_trail_ordered (destination_link->id, &traversed_comids);
			}
			else {
				if (divergence_start != NULL) {
					// Facility on divergence
					// USGS above facility ?
					if (divergence_destination)
						start_exit_up->link->accumulate_upstream_single_trail_ordered (destination_exit_down->link->id, &traversed_comids);
					else
						start_exit_up->link->accumulate_upstream_single_trail_ordered (destination_link->id, &traversed_comids);
				}
				else
					error = true;
			}

			if (traversed_comids.size () != 0) {
				// Upstream is correct
				upstream = true;
				if (divergence_start) {
					start_exit_up->describe (log);
					// start_exit_up->path is in order from (divergence) to (non-divergence link), upstream
					// It has 1 entry which is redundant with end of traversed_ids
					start_exit_up->path.pop_back ();
					// append reversed to traverse_comids
					append_vector (&traversed_comids, &start_exit_up->path, true);
				}
				if (divergence_destination) {
					destination_exit_down->describe (log);
					// destination_exit_down->path is in order from (divergence) to (non-divergence link), downstream
					// It has 1 entry which is redundant with end of traversed_ids
					destination_exit_down->path.pop_back ();
					// prepend to traversed_comids
					prepend_vector (&traversed_comids, &destination_exit_down->path, false);
				}
			}
			else {
				// Destination downstream from start?

				if (destination_link) {
					if (divergence_start) {
						start_exit_down->describe (log);
						destination_link->accumulate_upstream_single_trail_ordered (start_exit_down->link->id, &traversed_comids);
					}
					else
						destination_link->accumulate_upstream_single_trail_ordered (facility_link->id, &traversed_comids);
				}
				else {
					// if (usgs_station->attributes_numeric [USGS_ATTRIBUTES_INDEX_DIVERGENCE] == 2.0)
					// destination is on divergence
					if (divergence_start) {
						start_exit_down->describe (log);
						destination_exit_up->link->accumulate_upstream_single_trail_ordered (start_exit_down->link->id, &traversed_comids);
					}
					else
						destination_exit_up->link->accumulate_upstream_single_trail_ordered (facility_link->id, &traversed_comids);
				}

				if (traversed_comids.size () != 0) {
					// Downstream is correct
					if (divergence_start) {
						// start_exit_down->path is in order from (divergence) to (non-divergence link), downstream
						// It has 1 note which is redundant with end of traversed_ids
						start_exit_down->path.pop_back ();
						// append start_exit_down->path in reverse order to end of traversed_comids
						append_vector (&traversed_comids, &start_exit_down->path, true);
					}
					if (divergence_destination) {
						// destination_exit_up->path is in order from (divergence) to (non-divergence link), upstream
						// It has 1 note which is redundant with end of traversed_ids
						destination_exit_up->path.pop_back ();
						// append to traversed_comids in reverse order
						prepend_vector (&traversed_comids, &destination_exit_up->path, false);
					}
				}
			}

			// Copy all traversed segments to dialog_river's river layer
			for (river_segment_id = traversed_comids.begin ();
			river_segment_id != traversed_comids.end ();
			++river_segment_id) {
				river_segment = layer_rivers->match_id (*river_segment_id);
				river_segment_copy = d.streams->create_new (river_segment->type);
				river_segment_copy->copy (river_segment, d.streams);
				river_segment_copy->name.format ("%ld", river_segment_copy->id);
				d.streams->objects.push_back (river_segment_copy);
			}

			d.streams->check_extent (&extent);
			// select other streams in extent
			// 2015-09-09 Boost extent east & west for shape of window
			extent.inflate (0.3, 0.769);
			select_overlapping (&extent, layer_rivers, d.other_streams, d.streams);

			d.layer_point_from->draw_labels_name = true;
			d.layer_point_to->draw_labels_name = true;

			// 2015-09-09 Color & thickness settings for Dave (Joe's presentation)
			if (layer_from->type == MAP_OBJECT_POINT) {
				d.layer_point_from->draw_as = MAP_OBJECT_DRAW_SYMBOL_FILL_AND_OUTLINE;
				d.layer_point_from->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
			}
			else
				d.layer_point_from->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;

			d.layer_point_from->data_attribute_index = 3; // NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE unused for points
			d.layer_point_from->symbol_size = 16;
			d.layer_point_from->line_width = 1;
			d.layer_point_from->color = RGB (0, 0, 0);
			if (layer_to->type == MAP_OBJECT_POINT) {
				d.layer_point_to->draw_as = MAP_OBJECT_DRAW_SYMBOL_FILL_AND_OUTLINE;
				d.layer_point_to->symbol = MAP_OBJECT_SYMBOL_DIAMOND;
			}
			else
				d.layer_point_to->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;

			d.layer_point_to->data_attribute_index = 3; // NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE unused for points
			d.layer_point_to->symbol_size = 16;
			d.layer_point_to->line_width = 1;
			d.layer_point_to->color = RGB (0, 0, 0);
			d.streams->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			d.streams->color = RGB (0, 255, 255);
			d.streams->line_width = 3;
			d.other_streams->color = RGB (0, 0, 255);

			// colors used just to fill circle of facility
			d.layer_point_to->colors = d.layer_point_from->colors;
			d.layer_point_from->colors.add_color (RGB (255, 0, 0), 1.0, 2.0, "Facility Red");
			d.layer_point_from->colors.add_color (RGB (163, 73, 164), 2.0, 3.0, "Station Magenta");

			d.set_map ();

			SetCursor (*wxSTANDARD_CURSOR);

			d.describe_all (upstream, log);

			if ((layer_from->type == MAP_OBJECT_POINT)
			&& (layer_to->type == MAP_OBJECT_POINT)) {

				interface_window_wx view;
				view.setup_wx (this);
				view.start_progress (this);

				// router->route_facilities_threaded_queue (4, d.layer_point_from, d.layer_point_to, 2, &view, NULL, output_delimiter, true, log);
				router->route_facilities (d.layer_point_from, d.layer_point_to, true, &view, NULL, output_delimiter, true, log);
			}

			log.convert_linefeeds_for_CEdit ();
			dialog_error_list de (this, L"River", L"", log);
			de.ShowWindowModal ();

			d.ShowWindowModal ();
		}
		else {
			SetCursor (*wxSTANDARD_CURSOR);
			log.add_formatted ("ERROR, station ID %ld not found in layer \"", destination_id);
			log += layer_to->name;
			log += "\".\n";
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list de (this, L"River", L"", log);
			de.ShowWindowModal ();
		}
	}
}

void frame_CARDGIS::OnShowSelection
	(wxCommandEvent&)
{
	std::vector <map_layer*>::const_iterator layer;
	map_layer *layer_copy;

	if (panel_watershed->layer_selection
	&& (panel_watershed->layer_selection->objects.size () > 0)) {

		SetCursor (*wxHOURGLASS_CURSOR);
		dynamic_string log;
		bounding_cube extent;
		std::vector <map_object *>::const_iterator point;
		map_object *copy;
		router_NHD *router = relevent_router ();

		dialog_selection d (this, &router->rivers);

		d.create_selection_layer (panel_watershed->layer_selection);
		d.layer_selection->check_extent (&extent);

		for (layer = map_watershed->layers.begin();
		layer != map_watershed->layers.end();
		++layer) {
			// select facilities in this region
			if ((*layer) != panel_watershed->layer_selection) {
				layer_copy = d.create_map_layer (*layer);
				for (point = (*layer)->objects.begin ();
				point != (*layer)->objects.end ();
				++point) {
					if (extent.contains ((*point)->latitude, (*point)->longitude)) {
						copy = layer_copy->create_new ((*point)->type);
						copy->copy (*point, layer_copy);
						copy->name.format ("%ld", copy->id);
						layer_copy->objects.push_back (copy);
					}
				}
			}
		}

		// select other streams in extent
		// select_overlapping (&extent, layer_rivers, d.other_streams, d.layer_selection);

		d.set_map ();

		SetCursor (*wxSTANDARD_CURSOR);

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list de (this, L"River", L"", log);
			de.ShowWindowModal ();
		}

		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnSelectionClear
	(wxCommandEvent&)
{
	if (panel_watershed->layer_selection) {
		panel_watershed->layer_selection->objects.clear ();
		delete panel_watershed->layer_selection;
		panel_watershed->layer_selection = NULL;
		panel_watershed->redraw (L"Clear Selection");
	}
}

bool frame_CARDGIS::read_water_bodies
	(const dynamic_string &path_shapefile,
	dynamic_map *map,
	interface_window *update_display,
	dynamic_string &log)

{
	bool error = false;
	importer_shapefile importer;
	update_display->update_progress ("Reading Shapefile");

	map_layer *layer_lakes = map->match ("Water Bodies");
	map_layer *layer_rivers = map->match ("Rivers");

	if (layer_lakes == NULL) {
		layer_lakes = map->create_new (MAP_OBJECT_POLYGON);
		layer_lakes->name = "Water Bodies";
		layer_lakes->draw_as = MAP_OBJECT_DRAW_FILL_LAYER_COLOR;
		// layer_lakes->color = RGB (112, 146, 190);
		layer_lakes->color = RGB (192, 192, 192);
		map->layers.push_back (layer_lakes);

		// put lakes first so river network is drawn on top
		if (layer_rivers) {
			std::vector <map_layer *> temp_layers;
			std::vector <map_layer *>::iterator other_layer;

			for (other_layer = map->layers.begin ();
			other_layer != map->layers.end ();
			++other_layer)
				if ((*other_layer != layer_rivers)
				&& (*other_layer != layer_lakes))
					temp_layers.push_back (*other_layer);

			temp_layers.push_back(layer_lakes);
			temp_layers.push_back(layer_rivers);

			map->layers.clear ();
			map->layers = temp_layers;
		}

		importer.filename_source = path_shapefile;
		importer.id_field_name = "COMID";
		importer.name_field_name = "GNIS_NAME";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		if (!importer.import (layer_lakes, NULL, map, update_display, log))
			error = true;

		map->set_extent ();

		log += "NHD layer source : ";
		log += importer.filename_source;
		log += "\n";
		log.add_formatted ("NHD layer size : %d\n", layer_lakes->objects.size ());
	}

	return !error;
}

map_layer *read_linked_points
	(dynamic_string &filename,
	const char *name,
	const COLORREF color,
	const int symbol,
	const int attribute_count_text,
	dynamic_map *map_watershed,
	router_NHD *router,
	dynamic_string &log,
	bool *error)

{
	map_layer *layer_usgs;
	layer_usgs = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_usgs->name = name;
	layer_usgs->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC; // Will be filled with COMID of nearest NHD segment, index of nearest point, DIVERGENCE flag, distance_m
	layer_usgs->column_names_numeric.push_back ("COMID");
	layer_usgs->column_names_numeric.push_back ("Polygon Index");
	layer_usgs->column_names_numeric.push_back ("Point Index");
	layer_usgs->column_names_numeric.push_back ("Divergence");
	layer_usgs->column_names_numeric.push_back ("Distance m");
	layer_usgs->attribute_count_text = attribute_count_text;
	layer_usgs->initialize_attributes = true;
	layer_usgs->color = color;
	layer_usgs->symbol = symbol;
	layer_usgs->symbol_size = 6;
	layer_usgs->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	map_watershed->layers.push_back (layer_usgs);

	if (!router->read_linked_points_binary_storet (filename, layer_usgs, log))
		*error = true;

	return layer_usgs;
}

void frame_CARDGIS::OnOpenArea
	(wxCommandEvent &ev)

{
	dynamic_string log, area_name, filename_all_monitors;
	importer_shapefile importer;
	bool error = false;
	const int READ_AREA_OPTIONS = 14;
	bool read_layers [READ_AREA_OPTIONS], read_timed_data [READ_AREA_OPTIONS];
	dynamic_string layer_names [READ_AREA_OPTIONS] = {"VAA", "Flow Network", "Elevation", "Facilities", "USGS (nitrate)", "USGS (all)", "STORET Legacy", "STORET Modern", "Other", "Water Bodies", "Census Tracts", "PlaceTract", "County Boundaries", "Industrial Sites"};
	dynamic_string layer_paths [READ_AREA_OPTIONS];
	std::chrono::system_clock::time_point start_time, end_time_base, end_times_layer [READ_AREA_OPTIONS], previous, time_after_shp_base, time_after_shp_placetract;

	memset (read_layers, 0, sizeof (bool) * READ_AREA_OPTIONS);
	memset (read_timed_data, 0, sizeof (bool) * READ_AREA_OPTIONS);
	read_layers[0] = read_layers[1] = read_layers[2] = true; //  read_layers[3] = read_layers[9] = true;

	interface_window_wx *update_display;

	filename_nhd_area = app->filename_nhd_root;
	switch (ev.GetId ()) {
		case MENU_AREA_1:
			area_name = "01";
			break;
		case MENU_AREA_2:
			area_name = "02";
			break;
		case MENU_AREA_3N:
			area_name = "03N";
			break;
		case MENU_AREA_3S:
			area_name = "03S";
			break;
		case MENU_AREA_3W:
			area_name = "03W";
			break;
		case MENU_AREA_4:
			area_name = "04";
			break;
		case MENU_AREA_5:
			area_name = "05";
			break;
		case MENU_AREA_6:
			area_name = "06";
			break;
		case MENU_AREA_7:
			area_name = "07";
			break;
		case MENU_AREA_8:
			area_name = "08";
			break;
		case MENU_AREA_9:
			area_name = "09";
			break;
		case MENU_AREA_10U:
			area_name = "10U";
			break;
		case MENU_AREA_10L:
			area_name = "10L";
			break;
		case MENU_AREA_11:
			area_name = "11";
			break;
		case MENU_AREA_12:
			area_name = "12";
			break;
		case MENU_AREA_13:
			area_name = "13";
			break;
		case MENU_AREA_14:
			area_name = "14";
			break;
		case MENU_AREA_15:
			area_name = "15";
			break;
		case MENU_AREA_16:
			area_name = "16";
			break;
		case MENU_AREA_17:
			area_name = "17";
			break;
		case MENU_AREA_18:
			area_name = "18";
	}
	filename_nhd_area += "NHDPlus";
	filename_nhd_area += area_name;
	filename_nhd_area += "\\";

	filename_all_monitors = app->filename_other_root;
	filename_all_monitors += "updown\\all_monitors\\nwis\\comid\\";

	layer_paths [0] = filename_nhd_area + "NHDPlusAttributes\\PlusFlowlineVAA.dbf";
	layer_paths [1] = filename_nhd_area + "Network.bin";

	layer_paths [2] = filename_nhd_area + "NHDPlusAttributes";
	layer_paths [2] += PATH_SEPARATOR;
	layer_paths [2] += "elevslope.dbf";

	layer_paths [3] = app->filename_nhd_root + "WWTF Binaries\\cwns_latlon-Links-";
	layer_paths [3] += area_name;
	layer_paths [3] += ".bin";

	layer_paths [4] = filename_nhd_area + "usgs_stations.bin";

	layer_paths [5] = app->filename_nhd_root + "USGS Binaries\\usgs_stations-Links-";
	layer_paths [5] += area_name;
	layer_paths [5] += ".bin";

	// layer_paths [6] = filename_nhd_area + "storet_legacy_stations.bin";
	// layer_paths [6] = app->filename_other_root;
	// layer_paths [6] += "updown\\all_monitors\\storet_legacy\\comid\\storet_legacy_stations-Links-";
	// layer_paths [6] += area_name;
	// layer_paths [6] += ".bin";

	// layer_paths [6] = filename_nhd_area;
	// layer_paths [6] += "storet_legacy_stations";
	// layer_paths [6] += ".bin";

	layer_paths [6] = app->filename_nhd_root + "STORET Legacy Binaries\\";
	layer_paths[6] += "storet_legacy_stations-Links-";
	layer_paths [6] += area_name;
	layer_paths [6] += ".bin";
	read_timed_data [6] = true;

	// layer_paths [7] = filename_nhd_area + "storet_modern_stations.bin";
	// layer_paths [7] = app->filename_other_root;
	// layer_paths [7] += "updown\\all_monitors\\storet_modern\\comid\\storet_modern_stations-Links-";
	// layer_paths [7] += area_name;
	// layer_paths [7] += ".bin";

	layer_paths [7] = app->filename_nhd_root + "STORET Modern Binaries\\";
	layer_paths [7] += "storet_modern_stations-Links-";
	layer_paths [7] += area_name;
	layer_paths [7] += ".bin";

	// "R:\DKEISER\other\comid_2016_04_07-09.bin"
	layer_paths [8] = app->filename_other_root;
	layer_paths [8] += "comid_2016_04_07-";
	layer_paths [8] += area_name;
	layer_paths [8] += ".bin";

	layer_paths [9] = filename_nhd_area + "\\NHDSnapshot\\Hydrography\\NHDWaterbody.shp";

	layer_paths [10] = app->filename_census_root;
	// layer_paths [10] += "\\geolytics_ncdb\\tracts.shp";
	layer_paths[10] += "tracts_proj_WGS1984.shp";

	layer_paths [11] = app->filename_census_root;
	// layer_paths [11] += "place_wwtf_tract_intersect\\";
	// layer_paths[11] += "PlaceTract_WGS1984_Intersect.shp";
	layer_paths [11] += "place_wwtf_tract_clip_intersect\\";
	layer_paths [11] += "PlaceTract_clipint_WGS1984.shp";
	/*
// if (app->filename_nhd_root.get_at_ascii (0) == 'M')
//	layer_paths [11] += "place_wwtf_tract_clip_intersect\\";
//else
//	layer_paths [11] = "n:\\mlong\\nhdplus01\\test\\";
	layer_paths [11] = "g:\\scratch\\nhdplus01\\test\\";
	layer_paths [11] += "PlaceTract_clipint_WGS1984.shp";
	*/

	// 2017-09-07 TIGER ROOT is TIGER2015, but county shapefile is under TIGER2012
	layer_paths [12] += "r:\\Census\\TIGER 2012\\County\\tl_2012_us_county\\tl_2012_us_county.shp";

	layer_paths [13] = app->filename_other_root;
	layer_paths [13] += "\\all_facilities\\frs-Links-";
	layer_paths [13] += area_name;
	layer_paths [13] += ".bin";

	dialog_read_area dra (this, READ_AREA_OPTIONS, read_layers, read_timed_data, layer_names, layer_paths);
	if (dra.ShowModal () == wxID_OK) {

		start_time = std::chrono::system_clock::now ();

		update_display = new interface_window_wx;
		update_display->start_progress (this);
		update_display->update_progress ("NHDView");

		panel_watershed->pause_rendering = true;
		// save time by creating this layer as wx so map_objects don't have to be converted


		if (read_layers[0] || read_layers[1]) {
			// Area column names will be set as shapefiles are read
			map_layer *layer_rivers = create_river_layer (map_watershed, false, false);

			router_v2->river_map_layer = layer_rivers;

			// Two columns will be read from this shapefile
			// layer_rivers->column_names_numeric.push_back ("FTYPE");
			// layer_rivers->column_names_numeric.push_back ("FTYPE");
			// layer_rivers->column_names_numeric.push_back ("Lengthkm");
			router_v2->setup_shapefile (filename_nhd_area, layer_rivers, &importer);
			router_v2->read_shapefile (&importer, layer_rivers, map_watershed, update_display, log, &time_after_shp_base);

			end_time_base = std::chrono::system_clock::now ();

			if (read_layers[0]) {
				// read divergence, other codes from PlusFlowlineVAA
				layer_rivers->column_names_numeric.push_back ("Divergence");
				layer_rivers->column_names_numeric.push_back ("LevelPath");
				layer_rivers->column_names_numeric.push_back ("StreamLevel");
				layer_rivers->column_names_numeric.push_back ("StrahlerOrder");
				layer_rivers->column_names_numeric.push_back ("UpHydro");
				layer_rivers->column_names_numeric.push_back ("DownHydro");
				layer_rivers->column_names_numeric.push_back ("ElevMinm");
				layer_rivers->column_names_numeric.push_back ("ElevMaxm");
				if (!router_v2->read_flow_VAA (filename_nhd_area, update_display, log))
					error = true;

				end_times_layer [0] = std::chrono::system_clock::now ();
			}

			if (read_layers[1]) {
				// network.bin
				if (!router_v2->rivers.read (layer_paths[1], log))
					error = true;

				router_v2->report_counts (log);

				end_times_layer [1] = std::chrono::system_clock::now ();
			}
		}

		if (read_layers[2]) {
			if (!router_v2->read_NED_elevation (layer_paths[2], &area_elevation_minimum_m, &area_elevation_maximum_m, update_display, log))
				error = true;
			end_times_layer [2] = std::chrono::system_clock::now ();
		}

		if (read_layers[3]) {
			// Treatment facilities
			read_linked_points (layer_paths[3], layer_paths[3].get_text_ascii (), COLOR_FACILITIES, MAP_OBJECT_SYMBOL_CIRCLE, 0, map_watershed, router_v2, log, &error);
			end_times_layer [3] = std::chrono::system_clock::now ();
		}

		if (read_layers[4]) {
			// 'nitrate' USGS stations and nearest COMID list
			read_linked_points (layer_paths[4], "USGS Stations (nitrate)", COLOR_USGS_STATIONS, MAP_OBJECT_SYMBOL_PLUS, 0, map_watershed, router_v2, log, &error);
			end_times_layer [4] = std::chrono::system_clock::now ();
		}

		if (read_layers[5]) {
			// 'all' USGS stations and nearest COMID list
			read_linked_points (layer_paths[5], "USGS Stations", COLOR_USGS_STATIONS, MAP_OBJECT_SYMBOL_PLUS, 0, map_watershed, router_v2, log, &error);
			end_times_layer [5] = std::chrono::system_clock::now ();
		}

		if (read_layers[6]) {

			map_layer *layer_storet_legacy;
			if (((layer_storet_legacy = read_linked_points (layer_paths[6], "STORET Legacy", COLOR_STORET_LEGACY, MAP_OBJECT_SYMBOL_PLUS, 1, map_watershed, router_v2, log, &error)) != NULL)
			&& !error) {

				if (read_timed_data[6]) {
					/*
					Timestamp t;
					std::chrono::system_clock::time_point start_date, end_date;
					t.set (1971, 1, 1, 0, 0, 0);
					t.set_time_point (&start_date);
					t.set (1975, 12, 31, 23, 59, 59);
					t.set_time_point (&end_date);
					*/
					dynamic_string filename_timed_data;

					filename_timed_data = "H:\\scratch\\DissolvedOxygen\\All Dissolved Oxygen.accdb";

					read_sampling_data (filename_timed_data.get_text_ascii (), layer_storet_legacy, update_display, log);
				}
			}
			end_times_layer [6] = std::chrono::system_clock::now ();
		}

		if (read_layers[7]) {
			// STORET modern
			read_linked_points (layer_paths[7], "STORET Modern", COLOR_STORET_MODERN, MAP_OBJECT_SYMBOL_PLUS, 0, map_watershed, router_v2, log, &error);
			end_times_layer [7] = std::chrono::system_clock::now ();
		}

		if (read_layers[8]) {
			// Other Treatment facilities
			read_linked_points (layer_paths[8], layer_paths[8].get_text_ascii (), COLOR_FACILITIES_OTHER, MAP_OBJECT_SYMBOL_CIRCLE, 0, map_watershed, router_v2, log, &error);
			end_times_layer [8] = std::chrono::system_clock::now ();
		}

		if (read_layers[9]) {
			// Water Bodies
			read_water_bodies (layer_paths[9], map_watershed, update_display, log);
			end_times_layer [9] = std::chrono::system_clock::now ();
		}

		if (read_layers[10]) {
			// Census Tracts
			std::set <int> state_set;
			map_layer *layer_census;

			area_state_overlaps (ev.GetId () - MENU_AREA_1 + 1, &state_set);
			if ((layer_census = map_watershed->match ("Census Tracts")) == NULL) {
				layer_census = map_watershed->create_new (MAP_OBJECT_POLYGON);
				layer_census->name = "Census Tracts";
				layer_census->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
				layer_census->color = RGB (255, 0, 255);
				// Census ID is an 11-digit number
				// layer_census->attribute_count_text = 1;
				// layer_census->column_names_text.push_back ("Census Tract ID");
				layer_census->initialize_attributes = true;
				map_watershed->layers.push_back (layer_census);
			}
			read_census_tracts (layer_paths[10], map_watershed, layer_census, &state_set, update_display, log);
			end_times_layer [10] = std::chrono::system_clock::now ();
		}

		if (read_layers[11]) {
			// Census Tracts
			std::set <int> state_set;
			map_layer *layer_placetract;

			area_state_overlaps (ev.GetId () - MENU_AREA_1 + 1, &state_set);
			if ((layer_placetract = map_watershed->match ("PlaceTract")) == NULL) {
				layer_placetract = map_watershed->create_new (MAP_OBJECT_POLYGON);
				layer_placetract->name = "PlaceTract";
				layer_placetract->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
				layer_placetract->color = RGB (255, 0, 255);
				layer_placetract->initialize_attributes = true;
				map_watershed->layers.push_back (layer_placetract);
			}
			// read_PlaceTract (layer_paths [11], map_watershed, layer_placetract, &state_set, update_display, log);
			read_PlaceTract_clipint (layer_paths[11], map_watershed, layer_placetract, &state_set, &time_after_shp_placetract, update_display, log);
			end_times_layer [11] = std::chrono::system_clock::now ();
		}

		if (read_layers[12]) {
			// County Boundaries
			std::set <int> state_set;
			map_layer *layer_county;

			area_state_overlaps (ev.GetId () - MENU_AREA_1 + 1, &state_set);
			if ((layer_county = map_watershed->match ("County Boundaries")) == NULL) {
				layer_county = map_watershed->create_new (MAP_OBJECT_POLYGON);
				layer_county->name = "County Boundaries";
				layer_county->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
				layer_county->color = RGB (128, 0, 128);
				layer_county->initialize_attributes = true;
				map_watershed->layers.push_back (layer_county);
			}
			read_county_boundaries (layer_paths[12], map_watershed, layer_county, &state_set, update_display, log);
			end_times_layer [12] = std::chrono::system_clock::now ();
		}

		if (read_layers[13]) {
			read_linked_points (layer_paths[13], "Industrial Sites", COLOR_INDUSTRIAL_SITES, MAP_OBJECT_SYMBOL_CIRCLE, 0, map_watershed, router_v2, log, &error);
			end_times_layer [13] = std::chrono::system_clock::now ();
		}

		map_watershed->set_extent ();

		delete update_display;


		std::chrono::duration <double, std::milli> elapsed_milliseconds, total;
		previous = start_time;

		// Time to read base shapefile

		elapsed_milliseconds = time_after_shp_base - previous;
		previous = time_after_shp_base;
		log.add_formatted ("Time Elapsed\tBase SHP\t%.1lf", elapsed_milliseconds.count ());
		log += "\t";
		log += importer.filename_source;
		log += "\n";
		total += elapsed_milliseconds;

		elapsed_milliseconds = end_time_base - previous;
		previous = end_time_base;
		log.add_formatted ("Time Elapsed\tBase DBF\t%.1lf", elapsed_milliseconds.count ());
		log += "\t";
		log += importer.filename_source;
		log += "\n";
		total += elapsed_milliseconds;

		// time for each layer
		for (int layer_index = 0; layer_index < READ_AREA_OPTIONS; ++layer_index) {
			if (read_layers[layer_index]) {

				if (layer_index == 11) {
					elapsed_milliseconds = time_after_shp_placetract - previous;
					previous = time_after_shp_placetract;
					log.add_formatted ("Time Elapsed\t%d SHP\t%.1lf", layer_index + 1, elapsed_milliseconds.count ());
					log += "\t";
					log += layer_paths[layer_index];
					log += "\n";
					total += elapsed_milliseconds;

				}
				elapsed_milliseconds = end_times_layer [layer_index] - previous;
				previous = end_times_layer [layer_index];
				log.add_formatted ("Time Elapsed\t%d\t%.1lf", layer_index + 1, elapsed_milliseconds.count ());
				log += "\t";
				log += layer_paths[layer_index];
				log += "\n";
				total += elapsed_milliseconds;
			}
		}
		log.add_formatted ("Time Elapsed\tALL\t%.1lf\n", total.count ());

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"File Open", L"", log);
			d.ShowWindowModal ();
		}
		panel_watershed->pause_rendering = false;
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenArea");
	}
}

void frame_CARDGIS::OnOpenArea_HR
(wxCommandEvent& ev)

// 2023-06-08 Read NHDPlus_HD area

{
	dynamic_string log, area_name, filename_geometry, filename_attributes, filename_network;
	bool error = false;
	const int READ_AREA_OPTIONS = 14;
	bool read_layers[READ_AREA_OPTIONS];
	/*
	bool read_timed_data[READ_AREA_OPTIONS];
	dynamic_string layer_names[READ_AREA_OPTIONS] = { "VAA", "Flow Network", "Elevation", "Facilities", "USGS (nitrate)", "USGS (all)", "STORET Legacy", "STORET Modern", "Other", "Water Bodies", "Census Tracts", "PlaceTract", "County Boundaries", "Industrial Sites" };
	dynamic_string layer_paths[READ_AREA_OPTIONS];
	*/
	std::chrono::system_clock::time_point start_time, end_time_base, end_times_layer[READ_AREA_OPTIONS], previous, time_after_geometry, time_after_attributes;

	// memset(read_timed_data, 0, sizeof(bool) * READ_AREA_OPTIONS);
	memset(read_layers, 0, sizeof(bool) * READ_AREA_OPTIONS);
	read_layers[0] = read_layers[1] = read_layers[2] = true; //  read_layers[3] = read_layers[9] = true;

	interface_window_wx* update_display;

	switch (ev.GetId()) {
	case MENU_AREA_HR_1:
		area_name = "01";
		break;
	case MENU_AREA_HR_7:
		area_name = "07";
		break;
	}

	// "R:\NHDPlus_HR\Python\NHDPLUS_HR_exportGeometry.py"
	filename_geometry = app->filename_nhd_hr_root;
	filename_geometry += "output";
	filename_geometry += area_name;
	filename_geometry += "Geometry.txt";

	// "R:\NHDPlus_HR\Python\NHDPLUS_HR_exportNetwork.py"
	// Reading only for map_object attributes
	filename_attributes = app->filename_nhd_hr_root;
	filename_attributes += "output";
	filename_attributes += area_name;
	filename_attributes += "Network.txt";

	// "R:\NHDPlus_HR\NHDPlus_HR_Area01.bin"
	// OutputxxNetwork.txt after it's read and saved back to binary flow network
	filename_network = app->filename_nhd_hr_root;
	filename_network += "NHDPlus_HR_Area";
	filename_network += area_name;
	filename_network += ".bin";

	/*
	dialog_read_area dra(this, READ_AREA_OPTIONS, read_layers, read_timed_data, layer_names, layer_paths);
	if (dra.ShowModal() == wxID_OK) {
	*/

		start_time = std::chrono::system_clock::now();

		update_display = new interface_window_wx;
		update_display->start_progress(this);
		update_display->update_progress("NHDView_HR");

		panel_watershed->pause_rendering = true;
		// save time by creating this layer as wx so map_objects don't have to be converted


		if (read_layers[0]) {
			end_time_base = std::chrono::system_clock::now();
			map_layer* layer_rivers = create_river_layer(map_watershed, true, false);
			if (router_hr == NULL)
				router_hr = new router_NHD;

			router_hr->river_map_layer = layer_rivers;
			layer_rivers->color = RGB(128, 0, 128);

			if (router_hr->read_NHDPlus_HR_geometry(filename_geometry, layer_rivers, update_display, log)) {
				time_after_geometry = std::chrono::system_clock::now();
				if (router_hr->read_flow_network_NHDPlus_HR (filename_attributes, layer_rivers, false, update_display, log)) {
					time_after_attributes = std::chrono::system_clock::now();
					// network.bin
					if (!router_hr->rivers.read_long_long (filename_network, log))
						error = true;

					// 2023-06-08 NHDPlus_HR Ftype values too high for report_counts
					// router->report_counts(log);
					end_times_layer[0] = std::chrono::system_clock::now();
				}
				else
					error = true;
			}
			else
				error = true;
		}

		map_watershed->set_extent();

		delete update_display;

		std::chrono::duration <double, std::milli> elapsed_milliseconds, total;
		previous = start_time;

		// Time to read base shapefile

		elapsed_milliseconds = time_after_geometry - previous;
		previous = time_after_geometry;
		log.add_formatted("Time Elapsed\tGeometry\t%.1lf", elapsed_milliseconds.count());
		log += "\t";
		log += filename_geometry;
		log += "\n";
		total += elapsed_milliseconds;

		elapsed_milliseconds = time_after_attributes - previous;
		previous = time_after_attributes;
		log.add_formatted("Time Elapsed\tAttributes\t%.1lf", elapsed_milliseconds.count());
		log += "\t";
		log += filename_attributes;
		log += "\n";
		total += elapsed_milliseconds;

		elapsed_milliseconds = end_times_layer[0] - previous;
		previous = end_times_layer[0];
		log.add_formatted("Time Elapsed\tNetwork\t%.1lf", elapsed_milliseconds.count());
		log += "\t";
		log += filename_network;
		log += "\n";
		total += elapsed_milliseconds;

		/*
		// time for each layer
		for (int layer_index = 0; layer_index < READ_AREA_OPTIONS; ++layer_index) {
			if (read_layers[layer_index]) {

				if (layer_index == 11) {
					elapsed_milliseconds = time_after_shp_placetract - previous;
					previous = time_after_shp_placetract;
					log.add_formatted("Time Elapsed\t%d SHP\t%.1lf", layer_index + 1, elapsed_milliseconds.count());
					log += "\t";
					log += layer_paths[layer_index];
					log += "\n";
					total += elapsed_milliseconds;

				}
				elapsed_milliseconds = end_times_layer[layer_index] - previous;
				previous = end_times_layer[layer_index];
				log.add_formatted("Time Elapsed\t%d\t%.1lf", layer_index + 1, elapsed_milliseconds.count());
				log += "\t";
				log += layer_paths[layer_index];
				log += "\n";
				total += elapsed_milliseconds;
			}
		}
		*/
		log.add_formatted("Time Elapsed\tALL\t%.1lf\n", total.count());

		if (log.get_length() > 0) {
			log.convert_linefeeds_for_CEdit();
			dialog_error_list d(this, L"File Open", L"", log);
			d.ShowWindowModal();
		}
		panel_watershed->pause_rendering = false;
		panel_watershed->change_layers();
		enable_map_controls();
		panel_watershed->redraw("OnOpenArea");
}

void frame_CARDGIS::OnReadTIGER
	(wxCommandEvent &ev)

{
	dynamic_string log, area_name, path_tiger;
	importer_shapefile importer;
	std::vector <dynamic_string> street_types;
	bool *read_street_type;

	interface_window_wx *update_display;

	path_tiger = "R:\\Census\\TIGER 2015\\Roads\\";

	street_types.push_back ("I Interstate");
	street_types.push_back ("U U.S. Highway");
	street_types.push_back ("S State Highway");
	street_types.push_back ("C County Road");
	street_types.push_back ("M Metro Street");
	street_types.push_back ("Other");
	read_street_type = new bool [street_types.size ()];
	memset (read_street_type, 0, sizeof (bool) * street_types.size ());

	dialog_read_TIGER drt (this, &path_tiger, &map_watershed->logical_extent, &street_types, read_street_type);
	if (drt.ShowModal () == wxID_OK) {

		update_display = new interface_window_wx;
		update_display->start_progress (this);
		update_display->update_progress ("NHDView");

		panel_watershed->pause_rendering = true;

		// TIGER Streets
		std::set <int> state_set;
		map_layer *layer_tiger;

		area_state_overlaps (ev.GetId () - MENU_AREA_1 + 1, &state_set);

		if ((layer_tiger = map_watershed->match ("TIGER Streets")) == NULL) {
			layer_tiger = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_tiger->name = "TIGER Streets";
			layer_tiger->enclosure = MAP_POLYGON_NOT_ENCLOSED;
			layer_tiger->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_tiger->color = RGB (128, 128, 128);
			layer_tiger->initialize_attributes = true;
			layer_tiger->attribute_count_text = 2;
			layer_tiger->column_names_text.push_back ("RTTYP");
			layer_tiger->column_names_text.push_back ("MTFCC");
			map_watershed->layers.push_back (layer_tiger);
		}
		read_tiger_streets (path_tiger, map_watershed, layer_tiger, &state_set, true, update_display, log);

		map_watershed->set_extent ();

		delete update_display;

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"File Open", L"", log);
			d.ShowWindowModal ();
		}
		panel_watershed->pause_rendering = false;
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenTIGER");
	}

	delete [] read_street_type;
}

void frame_CARDGIS::OnOpenRiver
	(wxCommandEvent &ev)

{
	dynamic_string log, river_name, filename_single_area;
	importer_shapefile importer;
	bool error = false;
	bool read_layers [8], read_timed_data [8];
	int area_index;
	dynamic_string layer_names [8] = {"VAA", "Flow Network", "Facilities", "USGS Binary", "STORET Legacy", "STORET Modern", "Other", "Water Bodies"};
	dynamic_string layer_paths [8];
	std::set <dynamic_string> area_names;
	std::set <dynamic_string>::const_iterator single_area_name;
	map_layer *layer_rivers;

	memset (read_layers, 0, sizeof (bool) * 8);
	memset (read_timed_data, 0, sizeof (bool) * 8);
	read_layers [0] = read_layers [1] = read_layers [2] = true;

	interface_window_wx *update_display;

	filename_nhd_area = app->filename_nhd_root;
	switch (ev.GetId ()) {
		case MENU_COLORADO:
			river_name = "Colorado";
			area_names.insert ("14");
			area_names.insert ("15");
			break;
		case MENU_MISSISSIPPI:
			river_name = "Mississippi";
			area_names.insert ("05"); // ohio
			area_names.insert ("06"); // Tennessee
			area_names.insert ("07"); // UMRB
			area_names.insert ("08"); // Lower Mississippi
			area_names.insert ("10U"); // Missouri Upper
			area_names.insert ("10L"); // Missouri Lower
			area_names.insert ("11"); // Arkansas
	}
	filename_nhd_area += river_name;
	filename_nhd_area += "\\";

	layer_paths [0] = filename_nhd_area + "NHDPlusAttributes\\PlusFlowlineVAA.dbf";
	layer_paths [1] = filename_nhd_area + "Network.bin";
	layer_paths [2] = filename_nhd_area + "cwns_latlon.bin";

	layer_paths [3] = filename_nhd_area + "usgs_stations.bin";
	layer_paths [4] = filename_nhd_area + "storet_legacy_stations.bin";
	layer_paths [5] = filename_nhd_area + "storet_modern_stations.bin";

	layer_paths [6] = app->filename_other_root;
	layer_paths [6] += river_name;
	layer_paths [6] += "-Facilities.bin";

	layer_paths [7] = filename_nhd_area + "\\NHDSnapshot\\Hydrography\\NHDWaterbody.shp";

	dialog_read_area dra (this, 8, read_layers, read_timed_data, layer_names, layer_paths);
	if (dra.ShowModal () == wxID_OK) {

		update_display = new interface_window_wx;
		update_display->start_progress (this);
		update_display->update_progress ("NHDView");

		panel_watershed->pause_rendering = true;
		// router->setup_shapefile (filename_nhd_area, &importer);
		// save time by creating this layer as wx so map_objects don't have to be converted

		layer_rivers = create_river_layer (map_watershed, false, false);
		router_v2->river_map_layer = layer_rivers;

		for (single_area_name = area_names.begin (), area_index = 0;
		single_area_name != area_names.end ();
		++single_area_name, ++area_index) {
			// Read each shapefile
			filename_single_area = app->filename_nhd_root;
			filename_single_area += "NHDPlus";
			filename_single_area += *single_area_name;
			filename_single_area += "\\";

			// read shapefile
			importer.clear ();
			router_v2->setup_shapefile (filename_single_area, layer_rivers, &importer);
			router_v2->clear_intermediates ();
			router_v2->read_shapefile (&importer, layer_rivers, map_watershed, update_display, log);

			if (read_layers [0]) {
				// read divergence codes from PlusFlowlineVAA
				if (!router_v2->read_flow_VAA (filename_single_area, update_display, log))
					error = true;
			}
		}

		if (read_layers [1]) {
			if (!router_v2->rivers.read (layer_paths [1], log))
				error = true;

			router_v2->report_counts (log);
		}

		if (read_layers [2]) {
			map_layer *layer_facilities;

			// Treatment facilities cwns_latlon
			layer_facilities = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_facilities->name = layer_paths [2];
			layer_facilities->type = MAP_OBJECT_POINT;
			layer_facilities->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
			layer_facilities->initialize_attributes = true;
			layer_facilities->color = COLOR_FACILITIES;
			layer_facilities->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
			layer_facilities->symbol_size = 6;
			layer_facilities->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			map_watershed->layers.push_back (layer_facilities);

			if (!router_v2->read_linked_points_binary_storet (layer_paths [2], layer_facilities, log))
				error = true;
		}

		if (read_layers [6]) {
			map_layer *layer_other;

			// Other Treatment facilities

			layer_other = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_other->name = layer_paths [6];
			layer_other->type = MAP_OBJECT_POINT;
			layer_other->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
			layer_other->initialize_attributes = true;
			layer_other->color = COLOR_FACILITIES_OTHER;
			layer_other->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
			layer_other->symbol_size = 6;
			layer_other->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			map_watershed->layers.push_back (layer_other);

			if (!router_v2->read_linked_points_binary_storet (layer_paths [6], layer_other, log))
				error = true;
		}
		if (read_layers [3]) {
			// USGS stations and nearest COMID list

			map_layer *layer_usgs;
			layer_usgs = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_usgs->name = "USGS Stations";
			layer_usgs->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC; // Will be filled with COMID of nearest NHD segment, index of nearest point, DIVERGENCE flag, distance_m
			layer_usgs->initialize_attributes = true;
			layer_usgs->color = COLOR_USGS_STATIONS;
			layer_usgs->symbol = MAP_OBJECT_SYMBOL_PLUS;
			layer_usgs->symbol_size = 6;
			layer_usgs->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			map_watershed->layers.push_back (layer_usgs);

			if (!router_v2->read_linked_points_binary_storet (layer_paths [3], layer_usgs, log))
				error = true;
		}

		if (read_layers [4]) {
			map_layer *layer_storet_legacy;

			layer_storet_legacy = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_storet_legacy->name = "STORET Legacy";
			layer_storet_legacy->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC; // Will be filled with COMID of nearest NHD segment, index of nearest point, DIVERGENCE flag, distance_m
			layer_storet_legacy->attribute_count_text = 1;
			layer_storet_legacy->initialize_attributes = true;
			layer_storet_legacy->color = COLOR_STORET_LEGACY;
			layer_storet_legacy->symbol = MAP_OBJECT_SYMBOL_SQUARE;
			layer_storet_legacy->symbol_size = 6;
			layer_storet_legacy->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			map_watershed->layers.push_back (layer_storet_legacy);

			if (!router_v2->read_linked_points_binary_storet (layer_paths [4], layer_storet_legacy, log))
				error = true;
		}

		if (read_layers [5]) {
			map_layer *layer_storet_modern;
			layer_storet_modern = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_storet_modern->name = "STORET Modern";
			layer_storet_modern->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC; // Will be filled with COMID of nearest NHD segment, index of nearest point, DIVERGENCE flag, distance_m
			layer_storet_modern->initialize_attributes = true;
			layer_storet_modern->color = COLOR_STORET_MODERN;
			layer_storet_modern->symbol = MAP_OBJECT_SYMBOL_X;
			layer_storet_modern->symbol_size = 6;
			layer_storet_modern->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			map_watershed->layers.push_back (layer_storet_modern);

			if (!router_v2->read_linked_points_binary_storet (layer_paths [5], layer_storet_modern, log))
				error = true;
		}

		if (read_layers [7]) {
			// Water Bodies
			read_water_bodies (layer_paths [7], map_watershed, update_display, log);
		}

		map_watershed->set_extent ();

		delete update_display;

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"File Open", L"", log);
			d.ShowWindowModal ();
		}
		panel_watershed->pause_rendering = false;
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenRiver");
	}
}

bool frame_CARDGIS::accumulate_rivers
	(std::set <dynamic_string> *area_names,
	flow_network *rivers,
	interface_window *update_display,
	dynamic_string &log)

// Read rivers & points for all areas in set area_names

{
	std::set <dynamic_string>::const_iterator area_name;
	int area_index;
	dynamic_string filename, filename_area;
	importer_shapefile importer;
	bool error = false;

	// Read the other areas into array rivers []
	for (area_name = area_names->begin (), area_index = 0;
	area_name != area_names->end ();
	++area_name, ++area_index) {
		filename_area = app->filename_nhd_root;
		filename_area += "NHDPlus";
		filename_area += *area_name;
		filename_area += "\\";

		importer.clear ();
		map_layer *layer_rivers = create_river_layer (map_watershed, false, false);
		if (router_v2->river_map_layer == NULL)
			router_v2->river_map_layer = layer_rivers;
		router_v2->setup_shapefile (filename_area, layer_rivers, &importer);
		router_v2->setup_shapefile (filename_area, layer_rivers, &importer);
		router_v2->clear_intermediates ();
		router_v2->read_shapefile (&importer, layer_rivers, map_watershed, update_display, log);

		// read divergence codes from PlusFlowlineVAA
		if (!router_v2->read_flow_VAA (filename_area, update_display, log))
			error = true;

		filename = filename_area;
		filename += "Network.bin";

		if (!rivers [area_index].read (filename, log))
			error = true;
	}

	return !error;
}

void frame_CARDGIS::OnRouteMississippi
	(wxCommandEvent &)

{
	dynamic_string log, filename, filename_network;
	bool error = false;
	std::set <dynamic_string> area_names = {
		"05", // ohio
		"06", // Tennessee
		"07", // UMRB
		"10U", // Missouri Upper
		"10L", // Missouri Lower
		"11", // Arkansas
	};

	std::set <dynamic_string>::const_iterator area_name;
	importer_shapefile importer;
	flow_network *rivers;
	interface_window_wx *update_display;
	int area_index;

	update_display = new interface_window_wx;
	update_display->start_progress (this);
	update_display->update_progress ("NHDView");

	panel_watershed->pause_rendering = true;

	// Don't need shapefiles
	// Read binary networks, binary stations, binary facilities
	// USGS stations and nearest COMID list

	rivers = new flow_network [area_names.size ()];

	{
		// "08" Lower Mississippi
		filename_nhd_area = app->filename_nhd_root;
		filename_nhd_area += "NHDPLUS08\\";

		importer.clear ();
		map_layer *layer_rivers = create_river_layer (map_watershed, false, false);
		router_v2->river_map_layer = layer_rivers;
		router_v2->setup_shapefile (filename_nhd_area, layer_rivers, &importer);
		router_v2->clear_intermediates ();
		router_v2->read_shapefile (&importer, layer_rivers, map_watershed, update_display, log);

		// read divergence codes from PlusFlowlineVAA
		if (!router_v2->read_flow_VAA (filename_nhd_area, update_display, log))
			error = true;

		filename = filename_nhd_area;
		filename += "Network.bin";

		if (!router_v2->rivers.read (filename, log))
			error = true;
	}

	// Read all other areas into rivers & point layers
	accumulate_rivers (&area_names, rivers, update_display, log);

	// recombine rivers to router->rivers
	// Put all the separate systems together, no attempt to match branches
	router_v2->rivers.append (&rivers [0]); // 05 Ohio
	router_v2->rivers.append (&rivers [1]); // 06 Tennessee
	router_v2->rivers.append (&rivers [2]);
	router_v2->rivers.append (&rivers [3]);
	router_v2->rivers.append (&rivers [4]);
	router_v2->rivers.append (&rivers [5]);

	for (area_name = area_names.begin (), area_index = 0;
	area_name != area_names.end ();
	++area_name, ++area_index) {
		rivers [area_index].all_comids.clear ();
		rivers [area_index].all_mouths.clear ();
		rivers [area_index].divergences.divergences.clear ();
	}

	// 1861888 is in 05 and 06

	// 05 Ohio to 08
	router_v2->rivers.connect (1844789, 7469418, log);
	// 06 Tennessee to 05 Ohio
	router_v2->rivers.connect (1861888, 1840095, log);
	router_v2->rivers.connect (1862004, 1840191, log);

	// 07 UMRB to 08
	router_v2->rivers.connect (5093446, 7469418, log);
	// 10U Missouri to 10L
	router_v2->rivers.connect (11764402, 17244390, log);
	// 10L Missouri to 07
	router_v2->rivers.connect (6018266, 3624763, log);
	// 11 Arkansas to 07
	router_v2->rivers.connect (22850075, 15334434, log);

	// 11 Red to 08
	router_v2->rivers.connect (941140164, 19408128, log);
	// 11 White to 08
	router_v2->rivers.connect (14320629, 14320631, log);
	// 11 White to 08
	router_v2->rivers.connect (11800811, 9277950, log);
	router_v2->rivers.connect (11800815, 9277950, log);
	// 11 Cache (ditch) to 08
	router_v2->rivers.connect (11795911, 9272688, log);
	// 11 Arkansas to 08
	router_v2->rivers.connect (22850051, 15334434, log);

	delete [] rivers;

	// log.add_formatted ("Map layer size\t%d\n", layer_rivers->objects.size ());
	log.add_formatted ("Network all_comids size\t%d\n", router_v2->rivers.all_comids.size ());
	log.add_formatted ("Network all_mouths size\t%d\n", router_v2->rivers.all_mouths.size ());

	delete update_display;

	filename_network = app->filename_nhd_root;
	filename_network += "Mississippi\\network.bin";
	router_v2->rivers.write (filename_network, log);

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}

	panel_watershed->pause_rendering = false;
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnRouteMississippi");
}

void frame_CARDGIS::OnRouteColorado
	(wxCommandEvent &)

{
	dynamic_string log, filename, filename_network;
	bool error = false;
	std::set <dynamic_string> area_names = {
		"14" // Upper Colorado
	};

	std::set <dynamic_string>::const_iterator area_name;
	importer_shapefile importer;
	flow_network *rivers;
	interface_window_wx *update_display;
	int area_index;

	update_display = new interface_window_wx;
	update_display->start_progress (this);
	update_display->update_progress ("NHDView");

	panel_watershed->pause_rendering = true;

	// Don't need shapefiles
	// Read binary networks, binary stations, binary facilities
	// USGS stations and nearest COMID list

	rivers = new flow_network [area_names.size ()];

	{
		// "15" Lower Colorado
		filename_nhd_area = app->filename_nhd_root;
		filename_nhd_area += "NHDPLUS15\\";

		importer.clear ();
		map_layer *layer_rivers = create_river_layer (map_watershed, false, false);
		router_v2->river_map_layer = layer_rivers;
		router_v2->setup_shapefile (filename_nhd_area, layer_rivers, &importer);
		router_v2->clear_intermediates ();
		router_v2->read_shapefile (&importer, layer_rivers, map_watershed, update_display, log);

		// read divergence codes from PlusFlowlineVAA
		if (!router_v2->read_flow_VAA (filename_nhd_area, update_display, log))
			error = true;

		filename = filename_nhd_area;
		filename += "Network.bin";

		if (!router_v2->rivers.read (filename, log))
			error = true;
	}

	// Read area 14 into rivers, accumulate area 14 points
	accumulate_rivers (&area_names, rivers, update_display, log);

	// recombine rivers to router->rivers
	// Put all the separate systems together, no attempt to match branches
	router_v2->rivers.append (&rivers [0]); // 14 Upper Colorado

	for (area_name = area_names.begin (), area_index = 0;
	area_name != area_names.end ();
	++area_name, ++area_index) {
		rivers [area_index].all_comids.clear ();
		rivers [area_index].all_mouths.clear ();
		rivers [area_index].divergences.divergences.clear ();
	}

	// 14 to 15
	router_v2->rivers.connect (20734037, 20734041, log);
	router_v2->rivers.connect (18267749, 20734041, log);

	delete [] rivers;

	// log.add_formatted ("Map layer size\t%d\n", layer_rivers->objects.size ());
	log.add_formatted ("Network all_comids size\t%d\n", router_v2->rivers.all_comids.size ());
	log.add_formatted ("Network all_mouths size\t%d\n", router_v2->rivers.all_mouths.size ());
	delete update_display;

	filename_network = app->filename_nhd_root;
	filename_network += "Colorado\\network.bin";

	/*
	{
		// Mouth of Colorado is 21412883.  Remove any other rivers
		std::set <long> irrelevent_mouths;
		std::set <long>::iterator target;
		std::map <long, flow_network_link *>::iterator mouth;

		for (mouth = router->rivers.root_fragments.begin ();
		mouth != router->rivers.root_fragments.end ();
		++mouth) {
			if (mouth->first != 21412883)
				irrelevent_mouths.insert (mouth->first);
		}

		for (target = irrelevent_mouths.begin ();
		target != irrelevent_mouths.end ();
		++target)
			router->rivers.prune (*target);
	}
	*/

	router_v2->rivers.write (filename_network, log);

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}

	panel_watershed->pause_rendering = false;
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnRouteColorado");
}

void frame_CARDGIS::OnWriteStrahler
	(wxCommandEvent &)

{
	dynamic_string log, filename;
	bool error = false;
	dynamic_string filename_strahler;
	FILE *file_csv;
	std::vector <map_object *>::const_iterator segment;

	map_layer *layer_rivers = map_watershed->match ("Rivers");

	if (layer_rivers) {
		filename_strahler = filename_nhd_area;
		filename_strahler += "Strahler.csv";
		log += filename_strahler;
		log += "\n";

		if (fopen_s (&file_csv, filename_strahler.get_text_ascii (), "w") == 0) {

			fwrite ("COMID,Strahler\n", 15, 1, file_csv);

			for (segment = layer_rivers->objects.begin ();
			segment != layer_rivers->objects.end ();
			++segment) {
				fprintf (file_csv, "%lld,%lld\n", (*segment)->id, (long long) (*segment)->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STRAHLER_ORDER]);

			}

			fclose (file_csv);
		}
		else {
			error = true;
			log += "ERROR, can't open Strahler CSV file \"";
			log += filename_strahler;
			log += "\"\n";
		}
	}
	else
		log += "ERROR, no rivers layer.\n";

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnWritePointCOMID
	(wxCommandEvent &)

// 2015-04-22 Write any point layer's identifiers with closest COMID

{
	dynamic_string log, filename;
	bool error = false;
	dynamic_string filename_comid;
	map_layer *point_layer;
	dialog_select_layer dsl (this, L"Select Point Layer", map_watershed, &point_layer);

	if (dsl.ShowModal () == wxID_OK) {

		if (point_layer->type == MAP_OBJECT_POINT) {
			if (point_layer->name.match (PATH_SEPARATOR) != -1) {
				// facility layers use full path as layer name
				filename_struct f;
				f.parse (&point_layer->name);
				f.write_path (&filename_comid);
				filename_comid += f.prefix;
			}
			else {
				filename_comid = filename_nhd_area;
				filename_comid += point_layer->name;
			}
			filename_comid += " COMID.csv";
			log += filename_comid;
			log += "\n";

			if (!relevent_router ()->write_point_layer_comid(point_layer, filename_comid, log))
				error = true;
		}
		else {
			error = true;
			log += "ERROR, map layer is not POINT type.\n";
		}

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"File Open", L"", log);
			d.ShowWindowModal ();
		}
	}
}

void frame_CARDGIS::OnShowLegend
	(wxCommandEvent&)

{
	dialog_legend d (this, map_watershed, panel_watershed->layer_selection_source);
	d.ShowModal ();
}

void frame_CARDGIS::OnWriteCollada
	(wxCommandEvent &)

// Convert layer to xyz coordinates

{
	dynamic_string log, filename_dae, filename_python, filename_blender_output;
	dynamic_string filename_comid, filename_stream_samples;
	river_view_collada river_viewer;
	bool build_latlon = false, boundary_box = false, gridfloat_3dep = false, add_light = false;
	std::chrono::system_clock::time_point end_time;
	elevation_matrix elevations;
	std::vector <map_layer*>::const_iterator layer;
	map_layer *layer_storet_legacy = NULL;
	std::map <long, stream_measurements> stream_time_changes;
	// std::map <long, timed_measurements> point_sampling_data;
	// std::map <dynamic_string, std::set <long>>::iterator objects_to_plot;
	std::set <map_object *> objects_in_area;
	std::map <dynamic_string, std::set <map_object *>>::iterator objects_to_plot;
	filename_struct output_path;
	long count_samples = 0;

	router_NHD *router = relevent_router ();
	map_layer *layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");


	for (layer = map_watershed->layers.begin();
	layer != map_watershed->layers.end();
	++layer) {
		if (*layer == layer_rivers)
			river_viewer.render_layers.push_back (true);
		else
			river_viewer.render_layers.push_back (false);
		river_viewer.layer_symbol_size.push_back ((*layer)->symbol_size);
	}

	river_viewer.start_id = 6979016; // 6978552; // 6579808; // 6594992; // 19333373;
	river_viewer.destination_id = 6978998; // 6978642; // 6597740; // 6597494; // 19333191
	filename_dae = "h:\\scratch\\collada\\Collada.dae";

	filename_stream_samples = "H:\\scratch\\DissolvedOxygen\\All Dissolved Oxygen.accdb";
	filename_python = "NHDview.py";
	filename_blender_output = "h:\\scratch\\collada\\NHDview.avi";

	river_viewer.map_boundary.x [0] = -93712500.0;
	river_viewer.map_boundary.x [1] = -93574000.0;
	river_viewer.map_boundary.y [0] = 42000100.0;
	river_viewer.map_boundary.y [1] = 42076110.0;
	river_viewer.area_definition = RIVER_VIEW_AREA_DEFINED_BOX;

	Timestamp t;
	t.set (1971, 1, 1, 0, 0, 0);
	t.set_time_point (&river_viewer.start_date);
	t.set (1975, 12, 31, 23, 59, 59);
	t.set_time_point (&river_viewer.end_date);

	dialog_collada dc (this, &filename_dae, map_watershed, &gridfloat_3dep, &boundary_box, &build_latlon, &add_light, &river_viewer);
	if (dc.ShowModal () == wxID_OK) {

		std::vector <long> counties;
		map_object *outlet = NULL;
		srand (9551);
		int layer_index;

		interface_window_wx view;
		view.start_progress (this);

		end_time = river_viewer.start_date;

		for (layer = map_watershed->layers.begin(), layer_index = 0;
		layer != map_watershed->layers.end();
		++layer, ++layer_index) {
			if (((*layer)->name == "E85 Prices")
			&& (river_viewer.render_layers [layer_index])) {
				river_viewer.days_per_frame = 1.0;
				river_viewer.seconds_per_frame = 5400;
				river_viewer.hours_per_frame = 0.25;
				river_viewer.frames_per_second = 18;
			}
		}

		river_viewer.describe_run (log);

		output_path.parse (&filename_dae);
		output_path.set_suffix (L"py");
		output_path.write_file_specification (&filename_python);
		write_python_header (river_viewer.python_code, filename_dae, filename_blender_output, &river_viewer.scene);

		// First pass to set plotting_streams

		if (layer_rivers) {
			for (layer = map_watershed->layers.begin(), layer_index = 0;
			layer != map_watershed->layers.end();
			++layer, ++layer_index) {
				if (*layer == layer_rivers) {

					if (river_viewer.render_layers [layer_index]) {
						river_viewer.insert_relevant_streams (layer_rivers, router, &outlet, &view, log);		
						log.add_formatted ("Plotting stream size\t%d\n", river_viewer.plotting_streams.size ());
					}
					else {
						if (!gridfloat_3dep)
							river_viewer.min_elevation_meters = river_viewer.max_elevation_meters = EARTH_RADIUS_KM * 1000.0;

						if (river_viewer.area_definition == RIVER_VIEW_ALL_AREA) {
							// Set overall boundary by river area even if we're not using them
							std::vector <map_object *>::const_iterator object;
							for (object = layer_rivers->objects.begin ();
							object != layer_rivers->objects.end ();
							++object)
								(*object)->check_extent (&river_viewer.map_boundary);
							log.add_formatted ("Area boundary\t%.4lf\t%.4lf\t%.4lf\t%.4lf\n", river_viewer.map_boundary.x [0], river_viewer.map_boundary.x [1], river_viewer.map_boundary.y [0], river_viewer.map_boundary.y [1]);
						}
					}
				}
			}
		}

		// Read legacy points and sampling data, set plotting_stations (selection of points that will be exported)
		// Must be done before feed_streams
		for (layer = map_watershed->layers.begin(), layer_index = 0;
		layer != map_watershed->layers.end();
		++layer, ++layer_index) {
			if ((river_viewer.render_layers [layer_index])
			&& (*layer != layer_rivers)) {
				objects_in_area.clear ();
				river_viewer.insert_relevant_objects (&objects_in_area, *layer, &view, log);

				if ((*layer)->name == "STORET Legacy") {
					layer_storet_legacy = *layer;
					river_viewer.read_sampling_data (filename_stream_samples.get_text_ascii (), &objects_in_area, river_viewer.start_date, river_viewer.end_date, &count_samples, &view, log);
				}

				river_viewer.plotting_objects.insert (std::pair <dynamic_string, std::set <map_object *>> ((*layer)->name, objects_in_area));
			}
			++layer_index;
		}

		if ((river_viewer.plotting_streams.size () > 0)
		// && (point_sampling_data.size () > 0))
		&& (count_samples > 0)) {
			// Apply sampling data to streams
			river_viewer.feed_streams (&stream_time_changes, layer_storet_legacy, router, *layer, &river_viewer.map_boundary, log);

			// find latest time
			std::map <long, timed_measurements>::const_iterator node_reading;
			std::map <long, stream_measurements>::const_iterator stream_reading;
			std::chrono::system_clock::time_point latest;
			for (stream_reading = stream_time_changes.begin ();
			stream_reading != stream_time_changes.end ();
			++stream_reading) {
				for (node_reading = stream_reading->second.node_readings.begin (); node_reading != stream_reading->second.node_readings.end (); ++node_reading)
					if ((latest = node_reading->second.latest_time ()) > end_time)
						end_time = latest;
			}
		}
	
		river_viewer.build_stream_materials (); // build stream materials now so they can can have ids from 1..9

		if (gridfloat_3dep) {
			// R:\3DEP\1arc-second
			// Folder name is NW corner of covered area: 45w096 goes from 44 to 45 north and 95 to 94 w
			bounding_cube boundary;
			dynamic_string filename_flt, filename_image;
			logical_coordinate nw_corner;
			std::map <long, collada_geometry *>::iterator g;

			if (river_viewer.map_boundary.x [0] > -99000000.0) {
				if (river_viewer.map_boundary.y [0] > 44000000.0) {
					// Western area of Minnesota river
					// filename_flt = "H:\\scratch\\USGS_NED_1_n44w095_GridFloat\\usgs_ned_1_n44w095_gridfloat.flt";
					// filename_image = "H:\\scratch\\USGS_NED_1_n44w095_GridFloat\\USGS_NED_1_n44w095_GridFloat_thumb.jpg";
					filename_flt = "H:\\scratch\\USGS_NED_1_n46w097_GridFloat\\usgs_ned_1_n46w097_gridfloat.flt";
					filename_image = "H:\\scratch\\USGS_NED_1_n46w097_GridFloat\\USGS_NED_1_n46w097_GridFloat_thumb.jpg";
				}
				else {
					filename_flt = "H:\\scratch\\n43w094\\floatn43w094_1.flt";
					filename_image = "H:\\scratch\\n43w94\\floatn43w094_1_thumb.jpg";
				}
			}
			else {
				// Royal Gorge Area
				filename_flt = "H:\\scratch\\n39w106\\floatn39w106_1.flt";
				filename_image = "H:\\scratch\\n39w106\\floatn39w106_1_thumb.jpg";

				// Bridge over Royal Gorge
				nw_corner.x = -105.3237;
				nw_corner.y = 38.46358;
				nw_corner.z = EARTH_RADIUS_KM * 1000.0 + river_viewer.min_elevation_meters;
				river_viewer.add_obelisk (nw_corner, 100.0, &view, log);
			}

			boundary = river_viewer.map_boundary;
			boundary.normalize ();

			river_viewer.read_gridfloat_heatmap (filename_flt, filename_image, filename_dae, boundary, &elevations, true, &view, log);

			// nw_corner.z += 3000.0;
			// river_viewer.add_obelisk (nw_corner, &view, log);
		}

		for (layer = map_watershed->layers.begin(), layer_index = 0;
		layer != map_watershed->layers.end();
		++layer, ++layer_index) {
			if (river_viewer.render_layers [layer_index]) {
				objects_to_plot = river_viewer.plotting_objects.find ((*layer)->name);
				if (*layer == layer_rivers) {
					// bounding_cube stream_area;
					if (elevations.empty ())
						// Set elevations from stream
						river_viewer.build_stream (*layer, &river_viewer.plotting_streams, &stream_time_changes, &elevations, &view, log);
					else
						// elevations were set by gridfloat heatmap
						river_viewer.build_stream (*layer, &river_viewer.plotting_streams, &stream_time_changes, NULL, &view, log);
				}
				else {
					if ((*layer)->type == MAP_OBJECT_POINT) {
						// >1 point layer means prefixes need to be specified somehow
						dynamic_string prefix;
						prefix = (*layer)->name.left (4);
						prefix.trim_right ();
						river_viewer.build_points (&objects_to_plot->second, prefix.get_text_ascii (), river_viewer.layer_symbol_size [layer_index], &elevations, true, true, COLLADA_ANIMATE_SCALE | COLLADA_ANIMATE_COLOR, &view, log);
					}
					else
						if ((*layer)->type == MAP_OBJECT_POLYGON) {
							if ((*layer)->enclosure != MAP_POLYGON_NOT_ENCLOSED) {
								if ((*layer)->name == "County Boundaries")
									river_viewer.add_polygons (*layer, &objects_to_plot->second, false, NULL, &view, log);
								else
									river_viewer.add_polygons (*layer, &objects_to_plot->second, true, &elevations, &view, log);
							}
							else
								// TIGER streets
								river_viewer.add_vectors (*layer, &river_viewer.map_boundary, &elevations, &view, log);
						}
					}
			}
			++layer_index;
		}

		if (boundary_box)
			river_viewer.build_boundary_box (&river_viewer.map_boundary, &view, log);

		if (build_latlon)
			river_viewer.build_lat_lon_grid (map_watershed, &view, log);

		switch (river_viewer.camera_type) {
			case COLLADA_CAMERA_ORBIT:
				river_viewer.build_camera_area (end_time, &view, log);
				break;
			case COLLADA_CAMERA_SPACE_APPROACH:
				river_viewer.build_camera_spiral_in (&river_viewer.map_boundary, end_time, &view, log);
				break;
			case COLLADA_CAMERA_FOLLOW_STREAM:
				{
					// animated camera moving from start above map near outlet
					map_object *source = layer_rivers->match_id (river_viewer.start_id);
					river_viewer.build_camera_source_outlet (source, outlet, &river_viewer.map_boundary, end_time, router, layer_rivers, &view, log);
				}
				break;
			case COLLADA_CAMERA_FIXED:
				{
					// Build camera above outlet pointing at center
					logical_coordinate target;
					target = river_viewer.map_boundary.center ();
					target.x /= 1.0e6;
					target.y /= 1.0e6;
					target.z = river_viewer.min_elevation_meters;
					river_viewer.build_camera_fixed (outlet, &target, &view, log);
				}
		}

		if (add_light)
		river_viewer.create_lights (river_viewer.map_boundary);

		river_viewer.write_collada_xml (filename_dae, &view, log);

		write_python_tail (river_viewer.python_code);

		write_string_to_file (filename_python, river_viewer.python_code, log);
	}

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

bool make_dib_image_map
	(wxImage *png_image,
	dynamic_map *map,
	const bool include_legend,
	dynamic_string &error_message)

{
	wxMemoryDC memory_dc;
	bool error = false;
	wxBitmap bitmap;
	wxRect box (0, 0, 1920, 1080);
	map_scale scale_bitmap;

	int border = (int) (map->logical_extent.extent_x () / 40.0);

	if (bitmap.Create (box.GetWidth (), box.GetHeight ())) {
		// memory device context
		memory_dc.SelectObject (bitmap);

		scale_bitmap.minimum_scale = 0.01;
		scale_bitmap.device_extent = box;
		scale_bitmap.set (&map->logical_extent, border, SHAPEFILE_PROJECTED_LAT_LONG);
		map->scale = &scale_bitmap;

		// Fill with white background
		memory_dc.SetBrush (*wxWHITE_BRUSH);
		memory_dc.DrawRectangle (box);

		interface_window_wx view;

		view.dc = &memory_dc;
		map->draw (&view, error_message);
		if (include_legend)
			map->draw_legend (&view, error_message);

		memory_dc.SelectObject (wxNullBitmap);

		*png_image = bitmap.ConvertToImage();
	}

	return !error;
}

bool save_image_map
	(dynamic_string &path,
	dynamic_map *map,
	const bool draw_legend,
	dynamic_string &log)

{
	wxImage image_png;
	bool error = false;

	map->check_extent ();

	if (make_dib_image_map (&image_png, map, draw_legend, log)) {
		if (!image_png.SaveFile (path.get_text (), wxBITMAP_TYPE_PNG )) {
			log += "ERROR, can't save PNG image to \"";
			log += path;
			log += "\".\n";
			error = true;
		}
	}
	else
		log + "ERROR, cant' draw bitmap.\n";
	return !error;
}

void frame_CARDGIS::OnWrite2dAnimation
	(wxCommandEvent &)

{
	dynamic_string log, filename_dae, filename_png, path_png;
	dynamic_string filename_comid, filename_stream_samples;
	river_view_map river_viewer;
	bool boundary_box = false, gridfloat_3dep = false;
	std::chrono::system_clock::time_point end_time;
	std::vector <map_layer *>::iterator layer;
	map_layer *layer_storet_legacy = NULL, *layer_e85 = NULL;
	std::map <long, stream_measurements> stream_time_changes;
	std::set <map_object *> selected_objects;
	elevation_matrix elevations;
	long count_samples = 0;
	router_NHD *router = relevent_router ();

	// map_layer *layer_rivers = map_watershed->match ("Rivers");

	for (layer = map_watershed->layers.begin();
	layer != map_watershed->layers.end();
	++layer)
		river_viewer.render_layers.push_back (false);

	river_viewer.start_id = 6979016; // 6978552; // 6579808; // 6594992; // 19333373;
	river_viewer.destination_id = 6978998; // 6978642; // 6597740; // 6597494; // 19333191
	filename_dae = "h:\\scratch\\collada\\Collada.dae";

	filename_stream_samples = "H:\\scratch\\DissolvedOxygen\\All Dissolved Oxygen.accdb";
	path_png = "i:\\tdc\\ethanol\\prices\\";

	river_viewer.map_boundary.x [0] = -93712500.0;
	river_viewer.map_boundary.x [1] = -93574000.0;
	river_viewer.map_boundary.y [0] = 42000100.0;
	river_viewer.map_boundary.y [1] = 42076110.0;
	river_viewer.area_definition = RIVER_VIEW_AREA_DEFINED_BOX;

	Timestamp t;
	t.set (1971, 1, 1, 0, 0, 0);
	t.set_time_point (&river_viewer.start_date);
	t.set (1975, 12, 31, 23, 59, 59);
	t.set_time_point (&river_viewer.end_date);

	dialog_river_view dc (this, &filename_dae, map_watershed, &gridfloat_3dep, &boundary_box, &river_viewer);
	if (dc.ShowModal () == wxID_OK) {

		srand (9551);
		int layer_index;
		map_object *outlet = NULL;

		interface_window_wx view;
		view.start_progress (this);

		end_time = river_viewer.start_date;

		river_viewer.describe_run (log);

		// First pass to set plotting_streams
		for (layer = map_watershed->layers.begin(), layer_index = 0;
		layer != map_watershed->layers.end();
		++layer, ++layer_index) {
			if (((*layer)->name == "Rivers")
			&& (river_viewer.render_layers [layer_index])) {
				river_viewer.insert_relevant_streams (*layer, router, &outlet, &view, log);
				log.add_formatted ("Plotting stream size\t%d\n", river_viewer.plotting_streams.size ());
			}
		}

		// Read legacy points and sampling data, set plotting_stations
		// Must be done before feed_streams
		layer_index = 0;
		for (layer = map_watershed->layers.begin();
		layer != map_watershed->layers.end();
		++layer) {
			if (river_viewer.render_layers [layer_index]) {
				if ((*layer)->type == MAP_OBJECT_POINT) {
					selected_objects.clear ();
					river_viewer.insert_relevant_points (&selected_objects, *layer, &view, log);
					river_viewer.plotting_objects.insert (std::pair <dynamic_string, std::set <map_object *>> ((*layer)->name, selected_objects));

					if ((*layer)->name == "STORET Legacy") {
						layer_storet_legacy = *layer;
						river_viewer.read_sampling_data (filename_stream_samples.get_text_ascii (), &selected_objects, river_viewer.start_date, river_viewer.end_date, &count_samples, &view, log);
					}
					else
						if ((*layer)->name == "E85 Prices") {
							layer_e85 = *layer;
							layer_e85->include_in_legend = true;
						}
				}
			}
			else
				(*layer)->visible = false;
			++layer_index;
		}

		if ((river_viewer.plotting_streams.size () > 0)
		// && (point_sampling_data.size () > 0))
		&& (count_samples > 0)) {
			// Apply sampling data to streams
			river_viewer.feed_streams (&stream_time_changes, layer_storet_legacy, router, *layer, &river_viewer.map_boundary, log);

			// find latest time
			std::map <long, timed_measurements>::const_iterator node_reading;
			std::map <long, stream_measurements>::const_iterator stream_reading;
			std::chrono::system_clock::time_point latest;
			for (stream_reading = stream_time_changes.begin ();
			stream_reading != stream_time_changes.end ();
			++stream_reading) {
				for (node_reading = stream_reading->second.node_readings.begin (); node_reading != stream_reading->second.node_readings.end (); ++node_reading)
					if ((latest = node_reading->second.latest_time ()) > end_time)
						end_time = latest;
			}
		}
	
		if (gridfloat_3dep) {
			bounding_cube boundary;
			dynamic_string filename_flt, filename_image;
			logical_coordinate nw_corner;
			std::map <long, collada_geometry *>::iterator g;

			if (river_viewer.map_boundary.x [0] > -99000000.0) {
				filename_flt = "H:\\scratch\\n43w094\\floatn43w094_1.flt";
				filename_image = "H:\\scratch\\n43w94\\floatn43w094_1_thumb.jpg";
			}
			else {
				// Royal Gorge Area
				filename_flt = "H:\\scratch\\n39w106\\floatn39w106_1.flt";
				filename_image = "H:\\scratch\\n39w106\\floatn39w106_1_thumb.jpg";

				// Bridge over Royal Gorge
				nw_corner.x = -105.3237;
				nw_corner.y = 38.46358;
				nw_corner.z = EARTH_RADIUS_KM * 1000.0;
				// river_viewer.add_obelisk (nw_corner, 100.0, &view, log);
			}

			boundary = river_viewer.map_boundary;
			boundary.normalize ();

			river_viewer.read_gridfloat_heatmap (filename_flt, filename_image, filename_dae, boundary, &elevations, true, &view, log);

			// nw_corner.z += 3000.0;
			// river_viewer.add_obelisk (nw_corner, &view, log);
		}

		wxImage::AddHandler(new wxPNGHandler);

		std::chrono::hours one_week = std::chrono::hours (168);
		std::chrono::system_clock::time_point time;
		std::vector <map_object *> zero_holder;
		std::vector <map_object *>::iterator zero;
		long frame_count = 0;

		for (time = river_viewer.start_date;
		time <= river_viewer.end_date;
		time += one_week) {
			t.set_from_time_point (time);
			map_watershed->title = t.write (TIMESTAMP_YYYY_MM_DD);

			// Instead of creating a near-copy of map used by frame_CARDGIS, just change data attributes for E85 points to match time
			river_viewer.set_point_data (time, one_week, layer_e85, &zero_holder, (double) layer_e85->symbol_size, true, true, &view, log);

			filename_png = path_png;
			filename_png.add_formatted ("F%03ld.png", ++frame_count);
			view.update_progress (filename_png, 0);

			save_image_map (filename_png, map_watershed, false, log);

			// Put non-reporting stations back in so they can appear at later times
			for (zero = zero_holder.begin ();
			zero != zero_holder.end ();
			++zero)
				layer_e85->objects.push_back (*zero);
			zero_holder.clear ();
		}
	}

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnMergePolygons
	(wxCommandEvent &)

{
	dynamic_string log, buffer_layer_name;
	map_layer *layer_buffer, *layer_combined_buffer;
	double buffer_radius_m = 2000.0;
	ClipperBuffer clipper;

	interface_window_wx view;
	view.start_progress (this);

	buffer_layer_name.format ("Buffer-%.1lfkm", (long) buffer_radius_m / 1000.0);
	layer_buffer = map_watershed->match (buffer_layer_name.get_text_ascii ());

	layer_combined_buffer = map_watershed->create_new (MAP_OBJECT_POLYGON);
	layer_combined_buffer->name.format ("Combined Buffer", (long) buffer_radius_m / 1000.0);
	layer_combined_buffer->color = RGB (255, 0, 255);
	layer_combined_buffer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_combined_buffer->initialize_attributes = true; // clear all attributes as polygons are created
	map_watershed->layers.push_back (layer_combined_buffer);

	view.update_progress ("Polygon Layer", 0);
	// clipper.merge_threaded (layer_buffer, layer_combined_buffer, 30, &view, log);
	clipper.merge (layer_buffer, layer_combined_buffer, &view, log);

	layer_combined_buffer->set_extent ();

	log.add_formatted ("Polygon count\t%d\n", (int) layer_combined_buffer->objects.size ());

	layer_buffer->visible = false;
	panel_watershed->change_layers ();
	panel_watershed->change_selection_layer (layer_combined_buffer);
	enable_map_controls ();
	panel_watershed->redraw ("OnMergePolygons");

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}	

/*
void add_points_buffer
	(const map_object *segment,
	const int point_set,
	const double buffer_radius_m,
	std::vector <map_object> *points_left,
	std::vector <map_object> *points_right)

{
	double x_offset, y_offset;
	logical_coordinate point_1, point_2;
	double theta, xdiff, ydiff;
	map_object buffer_point;

	point_1.set (((map_polygon *) segment)->nodes [point_set * 2], ((map_polygon *) segment)->nodes [point_set * 2 + 1]);
	point_2.set (((map_polygon *) segment)->nodes [(point_set + 1) * 2], ((map_polygon *) segment)->nodes [(point_set + 1) * 2 + 1]);

	xdiff = point_2.x - point_1.x;
	ydiff = point_2.y - point_1.y;
			
	if (xdiff != 0.0) {
		theta = atan (xdiff / ydiff);

		x_offset = cos (theta) * offset_longitude_meters (point_1, buffer_radius_m);
		y_offset = sin (theta) * offset_latitude_meters (point_1, buffer_radius_m);
	}
	else {
		// No xdiff; vertical segment
		x_offset = offset_longitude_meters (point_1, buffer_radius_m);
		y_offset = offset_latitude_meters (point_1, buffer_radius_m);
	}

	// left side
	buffer_point.longitude = long ((point_1.x - x_offset) * 1.0e6);
	buffer_point.latitude = long ((point_1.y + y_offset) * 1.0e6);
	points_left->push_back (buffer_point);

	// right side
	buffer_point.longitude = long ((point_1.x + x_offset) * 1.0e6);
	buffer_point.latitude = long ((point_1.y - y_offset ) * 1.0e6);
	points_right->push_back (buffer_point);
}

void frame_CARDGIS::OnBuildBuffer
	(wxCommandEvent &)

// Create a new layer with buffer outline, using trig to compute points

{
	dynamic_string log;
	map_layer *layer_rivers = map_watershed->match ("Rivers");
	map_layer *layer_buffer;
	std::vector <map_object *>::const_iterator segment;
	double buffer_radius_m = 1000.0;
	int point_set, index_arrow;
	long polygon_count = 0;
	map_polygon_wx *buffer, *arrow;
	std::vector <map_object> points_left, points_right;
	std::vector <map_object>::const_iterator buffer_point;
	std::vector <map_object>::const_reverse_iterator counterpoint;

	interface_window_wx view;
	view.start_progress (this);

	layer_buffer = new map_layer_wx;
	layer_buffer->type = MAP_OBJECT_POLYGON;
	layer_buffer->name.format ("Buffer-%.1lfkm", (long) buffer_radius_m / 1000.0);
	layer_buffer->color = RGB (255, 127, 39);
	layer_buffer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_buffer->initialize_attributes = true; // clear all attributes as polygons are created
	map_watershed->layers.push_back (layer_buffer);

	color_set *arrow_colors = new color_set;
	arrow_colors->add_color (RGB (195, 195, 195), 1, "Left");
	arrow_colors->add_color (RGB (195, 195, 195), 2, "Right");
	map_layer *layer_arrow = new map_layer_wx;
	layer_arrow->type = MAP_OBJECT_POLYGON;
	layer_arrow->name = "Arrow";
	// layer_arrow->color = RGB (127, 127, 127);
	layer_arrow->colors = arrow_colors;
	layer_arrow->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
	layer_arrow->attribute_count_numeric = 1;
	layer_arrow->initialize_attributes = true; // clear all attributes as polygons are created
	map_watershed->layers.push_back (layer_arrow);

	view.update_progress ("Polygon Layer", 0);
	for (segment = layer_rivers->objects.begin ();
	segment != layer_rivers->objects.end ();
	++segment) {
		if ((*segment)->id == 20800677) {
			points_left.clear ();
			points_right.clear ();

			(*segment)->dump (layer_rivers, log);

			if ((*segment)->type != MAP_OBJECT_POLYGON_COMPLEX) {
				for (point_set = 0; point_set < ((map_polygon *) (*segment))->node_count - 1; ++point_set) {

					add_points_buffer (*segment, point_set, buffer_radius_m, &points_left, &points_right);

				}
			}
			else {
				int polygon_index;
				for (polygon_index = 0; polygon_index < ((map_polygon_complex *) (*segment))->polygon_count; ++polygon_index) {
					for (point_set = 0; point_set < ((map_polygon_complex *) (*segment))->polygons [polygon_index].node_count - 1; ++point_set)
						add_points_buffer (&((map_polygon_complex *) (*segment))->polygons [polygon_index], point_set, buffer_radius_m, &points_left, &points_right);
				}
			}

			// Move points into new polygon
			buffer = (map_polygon_wx *) layer_buffer->create_new (MAP_OBJECT_POLYGON);
			buffer->id = ++polygon_count;
			buffer->node_count = points_left.size () + points_right.size ();
			buffer->nodes = new long [buffer->node_count * 2];
			point_set = 0;

			for (buffer_point = points_left.begin ();
			buffer_point != points_left.end ();
			++point_set, ++buffer_point) {
				buffer->nodes [point_set * 2] = buffer_point->longitude;
				buffer->nodes [point_set * 2 + 1] = buffer_point->latitude;
			}

			// buffer->dump (layer_buffer, log);
			layer_buffer->objects.push_back (buffer);

			// Show vectors from each point to buffer point
			index_arrow = 0;
			for (point_set = 0, buffer_point = points_left.begin ();
			(point_set < ((map_polygon *) (*segment))->node_count - 1)
			&& (buffer_point != points_left.end ());
			++buffer_point, ++point_set) {
				arrow = (map_polygon_wx *) layer_arrow->create_new (MAP_OBJECT_POLYGON);
				arrow->id = ++index_arrow;
				arrow->attributes_numeric [0] = 1.0;
				arrow->node_count = 2;
				arrow->nodes = new long [2];
				arrow->nodes [0] = buffer_point->longitude;
				arrow->nodes [1] = buffer_point->latitude;
				arrow->nodes [2] = ((map_polygon *) *segment)->nodes [point_set * 2];
				arrow->nodes [3] = ((map_polygon *) *segment)->nodes [point_set * 2 + 1];

				layer_arrow->objects.push_back (arrow);
			}

			// Show vectors from each point to buffer point
			index_arrow = 0;
			for (point_set = 0, buffer_point = points_right.begin ();
			(point_set < ((map_polygon *) (*segment))->node_count - 1)
			&& (buffer_point != points_right.end ());
			++buffer_point, ++point_set) {
				arrow = (map_polygon_wx *) layer_arrow->create_new (MAP_OBJECT_POLYGON);
				arrow->id = ++index_arrow;
				arrow->attributes_numeric [0] = 2.0;
				arrow->node_count = 2;
				arrow->nodes = new long [2];
				arrow->nodes [0] = buffer_point->longitude;
				arrow->nodes [1] = buffer_point->latitude;
				arrow->nodes [2] = ((map_polygon *) *segment)->nodes [point_set * 2];
				arrow->nodes [3] = ((map_polygon *) *segment)->nodes [point_set * 2 + 1];

				layer_arrow->objects.push_back (arrow);
			}
		}
	}

	layer_buffer->set_extent ();
	layer_arrow->set_extent ();

	log.add_formatted ("Polygon count\t%ld\n", polygon_count);

	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnSetShapefile");

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}
*/

void frame_CARDGIS::OnBuildClipperBuffer
	(wxCommandEvent &)

// Create a new layer with buffer outline

{
	dynamic_string log;
	map_layer *layer_buffer;
	double buffer_radius_m = 10000.0;
	ClipperBuffer clipper;

	interface_window_wx view;
	view.start_progress (this);

	layer_buffer = map_watershed->create_new (MAP_OBJECT_POLYGON);
	layer_buffer->name.format ("Buffer-%.1lfkm", (long) buffer_radius_m / 1000.0);
	layer_buffer->color = RGB (255, 127, 39);
	layer_buffer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_buffer->initialize_attributes = true; // clear all attributes as polygons are created
	map_watershed->layers.push_back (layer_buffer);

	view.update_progress ("Polygon Layer", 0);
	// clipper.create (layer_rivers, layer_buffer, &view, log);
	// clipper.create_threaded (layer_rivers, layer_buffer, 30, buffer_radius_m, &view, log);
	clipper.create_threaded (panel_watershed->layer_selection, layer_buffer, 4, buffer_radius_m, false, &view, log);

	layer_buffer->set_extent ();

	log.add_formatted ("Polygon count\t%d\n", (int) layer_buffer->objects.size ());

	panel_watershed->change_layers ();
	panel_watershed->change_selection_layer (layer_buffer);
	enable_map_controls ();
	panel_watershed->redraw ("OnBuildClipperBuffer");

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnWriteShapefileLayer
	(wxCommandEvent&)

// Save polygons to shapefile

{
	dynamic_string log, filename_shapefile;
	importer_shapefile s;
	filename_struct shapefile_struct;

	if (panel_watershed->layer_selection_source != NULL) {

		shapefile_struct.parse (&panel_watershed->layer_selection_source->name);

		wxFileDialog openFileDialog ( this, _("Open file"), "", "", "Shape files (*.shp)|*.shp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

		shapefile_struct.set_suffix (L"shp");
		shapefile_struct.write_path (&filename_shapefile);
		openFileDialog.SetPath (filename_shapefile.get_text ());

		shapefile_struct.write_filename (&filename_shapefile);
		openFileDialog.SetFilename (filename_shapefile.get_text ());
 
		if ( openFileDialog.ShowModal() == wxID_OK ) {
			SetStatusText ("OnWriteShapefileLayer");
			SetCursor (*wxHOURGLASS_CURSOR);
			interface_window_wx view;
			view.start_progress (this);
			view.update_progress ("Reading Shapefile", 1);

			filename_shapefile = openFileDialog.GetDirectory().ToAscii ();
			filename_shapefile += PATH_SEPARATOR;
			filename_shapefile += openFileDialog.GetFilename().ToAscii ();
	
			s.filename_source = filename_shapefile;
			s.write_from_map_layer (panel_watershed->layer_selection_source, NULL, panel_watershed->layer_selection_source->enclosure == MAP_POLYGON_ADD_LAST_SEGMENT, &view, log);

			{
				map_layer *layer_echo = map_watershed->create_new (MAP_OBJECT_POLYGON);
				layer_echo->name = "ReadBack";
				layer_echo->color = RGB (255, 127, 39);
				layer_echo->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
				layer_echo->initialize_attributes = true; // clear all attributes as polygons are created
				map_watershed->layers.push_back (layer_echo);
			}

			SetCursor (wxNullCursor);
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list de (this, L"Shapefile Write", L"", log);
			de.ShowWindowModal ();
		}
	}
}

void frame_CARDGIS::OnWriteShapefileSelection
	(wxCommandEvent&)

// Save selection to shapefile

{
	dynamic_string log, filename_shapefile;
	importer_shapefile s;
	std::map <long long, shapefile_feature> map_features;

	if (panel_watershed->layer_selection != NULL) {

		wxFileDialog openFileDialog ( this, _("Open file"), "", "", "Shape files (*.shp)|*.shp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
		filename_shapefile = panel_watershed->layer_selection_source->name;
		filename_shapefile += "Selection.shp";
		openFileDialog.SetFilename (filename_shapefile.get_text ());
 
		if ( openFileDialog.ShowModal() == wxID_OK ) {
			SetStatusText ("OnWriteShapefileSelection");
			SetCursor (*wxHOURGLASS_CURSOR);
			interface_window_wx view;
			view.start_progress (this);
			view.update_progress ("Reading Shapefile", 1);

			filename_shapefile = openFileDialog.GetDirectory().ToAscii ();
			filename_shapefile += PATH_SEPARATOR;
			filename_shapefile += openFileDialog.GetFilename().ToAscii ();

			s.create_features (panel_watershed->layer_selection, &map_features, panel_watershed->layer_selection->enclosure == MAP_POLYGON_ADD_LAST_SEGMENT, log);
			panel_watershed->layer_selection->check_extent (&s.bounding_box);
			s.bounding_box.x [0] /= 1.0e6;
			s.bounding_box.x [1] /= 1.0e6;
			s.bounding_box.y [0] /= 1.0e6;
			s.bounding_box.y [1] /= 1.0e6;

			s.filename_source = filename_shapefile;
			s.write_from_map_layer (panel_watershed->layer_selection, NULL, panel_watershed->layer_selection->enclosure == MAP_POLYGON_ADD_LAST_SEGMENT, &view, log);

			{
				map_layer *layer_echo = map_watershed->create_new (MAP_OBJECT_POLYGON);
				layer_echo->name = "ReadBack";
				layer_echo->color = RGB (255, 127, 39);
				layer_echo->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
				layer_echo->initialize_attributes = true; // clear all attributes as polygons are created
				map_watershed->layers.push_back (layer_echo);
			}

			SetCursor (wxNullCursor);
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list de (this, L"Shapefile Write", L"", log);
			de.ShowWindowModal ();
		}
	}
}

map_layer *create_buffer_layer
	(const dynamic_string &layer_name,
	const dynamic_string &id_source_name_1, // Facility
	const dynamic_string &id_source_name_2, // CensusTract
	const COLORREF &color,
	dynamic_map *map)

{
	map_layer *layer_buffer = map->create_new (MAP_OBJECT_POLYGON);
	layer_buffer->name = layer_name;
	layer_buffer->color = color;
	layer_buffer->line_width = 2;
	layer_buffer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_buffer->attribute_count_numeric = 6;
	layer_buffer->column_names_numeric.push_back (id_source_name_1);
	layer_buffer->column_names_numeric.push_back ("BufferWidth");
	layer_buffer->column_names_numeric.push_back ("Geodesic");
	layer_buffer->column_names_numeric.push_back ("Exact");
	layer_buffer->column_names_numeric.push_back ("Authalic");
	layer_buffer->column_names_numeric.push_back ("Rhumb");
	layer_buffer->attribute_count_text = 1;
	layer_buffer->column_names_text.push_back (id_source_name_2);
	layer_buffer->initialize_attributes = true; // clear all attributes as polygons are created

	return layer_buffer;
}

void frame_CARDGIS::OnBufferFacility
	(wxCommandEvent&)

// For one facility, find nearest segment
// For all segments up & downstream, create buffer
// merge buffers

{
	dynamic_string log;
	map_layer *layer_buffer;
	map_object *facility;
	std::map <long long, map_object *>::const_iterator nearest_segment;
	flow_network_link *nearest_link;
	long long nearest_comid, level_path_id;
	std::vector <long long> downstream_ids, upstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Path p;
//	double buffer_radius_m = 1.0 * METERS_PER_MILE;
	double buffer_radius_m = 100.0;
	int index_miles, nearest_point_index, nearest_polygon_index;
	double length_stream_km = (25.0 * METERS_PER_MILE) / 1000.0;
	double offset_radius_average_1_mile, offset_radius_average;
	double upstream_within_segment_m, downstream_within_segment_m;
	_timeb start, end;
	float elapsed_milliseconds;
	ClipperLib::Paths path_river, paths_buffers;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	map_layer *layer_rivers = map_watershed->match ("Rivers");

	if (layer_rivers) {
		if (panel_watershed->layer_selection_source
		&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)) {

			if (panel_watershed->layer_selection->objects.size () == 1) {
				_ftime_s (&start);

				router_NHD *router = relevent_router ();

				// layer for resulting buffers
				layer_buffer = create_buffer_layer ("Buffer", "Facility", "Census Tract", RGB (255, 127, 39), map_watershed);
				layer_buffer->name.format ("Buffer-%.1lfkm", (long) buffer_radius_m / 1000.0);
				map_watershed->layers.push_back (layer_buffer);

				facility = panel_watershed->layer_selection->objects [0];

				// facility_id = 350004001;
				// nearest_comid = 17826502;
				// level_path_id = 680003345;
				nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
				nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
				nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];

				nearest_segment = router->river_layer_index.find (nearest_comid);
				level_path_id = (long long) nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

				// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (0, nearest_point_index, &upstream_within_segment_m, log);
				// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (nearest_point_index, ((map_polygon *) nearest_segment->second)->total_number_of_points () - 1, &downstream_within_segment_m, log);

				log.add_formatted ("Facility\t%lld\n", facility->id);
				log.add_formatted ("Nearest COMID\t%lld\n", nearest_comid);
				log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

				view.update_progress ("Accumulate downstream", 0);
				// router->rivers.accumulate_downstream (nearest_comid, &downstream_ids);
				router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

				view.update_progress ("Accumulate upstream", 0);
				if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
					nearest_link->accumulate_upstream_LevelPath_ordered (level_path_id, &router->river_layer_index, &upstream_ids);
				else
					log.add_formatted ("ERROR, can't find flow_network_link for COMID %lld.\n", nearest_comid);

				// computed distance from nearest point in nearest segment up & down

				// discard ids beyond length_downstream_km

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper.average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, log);

				view.update_progress ("Create Paths Upstream", 0);
				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				p.clear ();
				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				clipper.add_upstream_within_segment ((map_polygon *) nearest_segment->second, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, NULL, log);
				log.add_formatted ("Distance Up\t%.2lf\n", upstream_within_segment_m);
				clipper.create_paths (layer_rivers, NULL, &upstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, log);
				view.update_progress ("Create Paths Downstream", 0);
				path_river.push_back (p);

				p.clear ();
				clipper.add_downstream_within_segment ((map_polygon *) nearest_segment->second, nearest_polygon_index, nearest_point_index, &downstream_within_segment_m, length_stream_km * 1000.0, &p, NULL, log);
				log.add_formatted ("Distance Down\t%.2lf\n", downstream_within_segment_m);
				clipper.create_paths_reverse (layer_rivers, NULL, &downstream_ids, length_stream_km - (downstream_within_segment_m / 1000.0), &p, log);
				path_river.push_back (p);

				view.update_progress ("Create buffers", 0);
				for (index_miles = 1;
				index_miles <= 10;
				++index_miles) {
					offset_radius_average = offset_radius_average_1_mile * (double) index_miles;

					view.update_progress_formatted (1, "buffer %d miles", index_miles);

					// clipper.create_threaded (panel_watershed->layer_selection, layer_buffer, 30, buffer_radius_m, &view, log);
					// clipper.create_buffer_map_layer (&path_downstream, layer_buffer, offset_radius_average, facility->id, &view, log);
					clipper.create_buffer (&path_river, &paths_buffers, offset_radius_average, log);
					clipper.Paths_to_map_layer (&paths_buffers, facility->id, 0, NULL, NULL, layer_buffer, true, &view, log);
				}

				layer_buffer->set_extent ();

				panel_watershed->change_layers ();
				panel_watershed->change_selection_layer (layer_buffer);
				enable_map_controls ();

				_ftime_s (&end);

				elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
				log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);
			}
			else
				log += "ERROR, must have one point selected.\n";
		}
		else
			log += "ERROR, must have point layer selected.\n";
	}
	else
		log += "Error, no river layer.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnBisectSegment
	(wxCommandEvent&)

// For each facility in point layer, compute intersection with Census layer

{
	dynamic_string log;
	map_layer *layer_bisection_line;
	std::vector <map_object *>::const_iterator segment;
	bisection_definition bisection;
	logical_coordinate river_points [3];
	int polygon_index, point_index;
	double cross_radius_m, theta;
	device_coordinate center;
	logical_coordinate center_logical;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POLYGON)
	&& panel_watershed->layer_selection) {

		if (panel_watershed->layer_selection->objects.size () == 1) {

			if ((layer_bisection_line = map_watershed->match ("Bisection")) == NULL) {
				layer_bisection_line = map_watershed->create_new (MAP_OBJECT_POLYGON);
				layer_bisection_line->name = "Bisection";
				layer_bisection_line->color = RGB (0, 0, 0);
				layer_bisection_line->line_width = 2;
				layer_bisection_line->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
				layer_bisection_line->initialize_attributes = true; // clear all attributes as polygons are created
				map_watershed->layers.push_back (layer_bisection_line);
			}

			segment = panel_watershed->layer_selection->objects.begin ();
			polygon_index = 0;

			(*segment)->centroid (&center.y, &center.x);
			center_logical.set (center.x, center.y);
			cross_radius_m = offset_latitude_meters (center_logical, 1000.0);
			log.add_formatted ("\nCross Radius\t%.6lf\n", cross_radius_m);

			for (point_index = 0;
			point_index < ((map_polygon *) *segment)->node_count;
			++point_index) {

				log.add_formatted ("\nSegment\t%ld\n", (*segment)->id);
				log.add_formatted ("Point index\t%d\t%d\n", polygon_index, point_index);

				if (point_index == 0) {
					river_points [0].set (((map_polygon *) *segment)->nodes [point_index * 2], ((map_polygon *) *segment)->nodes [point_index * 2 + 1]);
					river_points [1].set (((map_polygon *) *segment)->nodes [(point_index + 1) * 2], ((map_polygon *) *segment)->nodes [(point_index + 1) * 2 + 1]);

					bisection.create_bisection_2pt (river_points, cross_radius_m, &theta, true, log);
				}
				else
					if (point_index < (((map_polygon *) *segment)->node_count - 1)) {
						river_points [0].set (((map_polygon *) *segment)->nodes [(point_index - 1) * 2], ((map_polygon *) *segment)->nodes [(point_index - 1) * 2 + 1]);
						river_points [1].set (((map_polygon *) *segment)->nodes [point_index * 2], ((map_polygon *) *segment)->nodes [point_index * 2 + 1]);
						river_points [2].set (((map_polygon *) *segment)->nodes [(point_index + 1) * 2], ((map_polygon *) *segment)->nodes [(point_index + 1) * 2 + 1]);

						bisection.create_bisection_3pt (river_points, cross_radius_m, &theta, true, log);
					}
					else {
						river_points [0].set (((map_polygon *) *segment)->nodes [point_index * 2], ((map_polygon *) *segment)->nodes [point_index * 2 + 1]);
						river_points [1].set (((map_polygon *) *segment)->nodes [(point_index - 1) * 2], ((map_polygon *) *segment)->nodes [(point_index - 1) * 2 + 1]);

						bisection.create_bisection_2pt (river_points, cross_radius_m, &theta, true, log);
					}

				bisection.create_map_object (layer_bisection_line, log);
			}

			layer_bisection_line->set_extent ();

			panel_watershed->change_layers ();
			// panel_watershed->change_selection_layer (layer_bisection_line);
			enable_map_controls ();
		}
		else
			log += "ERROR, must have one COMID selected.\n";
	}
	else
		log += "ERROR, must have river layer selected.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnIntersectBufferCensusAll
	(wxCommandEvent&)

// For each facility in point layer, compute intersection with Census layer

{
	dynamic_string log, filename_table, filename_log, layer_name_facilities, area_name, output_folder;
	map_layer *layer_facilities, *layer_census_tracts;
	ClipperBuffer clipper;
	FILE *output_table_file;
	double length_stream_miles = 25.0; // 1.0;
	double cut_line_length_miles = 25.0;
	int minimum_buffer_radius_miles = 1, maximum_buffer_radius_miles = 10;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);
	// message_slot threaded_messages;
	// view.attach_mutex (&threaded_messages);
	// view.destination = INTERFACE_SEND_MUTEX_SCROLL | INTERFACE_SEND_UPDATE_DIALOG;

	extract_area_name (filename_nhd_area, &area_name);

	output_folder = filename_nhd_area;
	output_folder = "h:\\scratch\\nhd\\";

	filename_table = output_folder;
	filename_table.add_formatted ("BufferClip-%dmile", (int) length_stream_miles);
	filename_table += area_name;
	filename_table += ".txt";

	filename_log = output_folder;
	filename_log.add_formatted ("BufferClip-%dmile", (int) length_stream_miles);
	filename_log += area_name;
	filename_log += ".log";

	// layer_facilities name is specific to area
	layer_name_facilities = app->filename_nhd_root + "WWTF Binaries\\cwns_latlon-Links-";
	layer_name_facilities += area_name;
	layer_name_facilities += ".bin";

	layer_facilities = map_watershed->match (layer_name_facilities.get_text_ascii ());
	layer_census_tracts = map_watershed->match ("Census Tracts");
	router_NHD* router = relevent_router();
	map_layer *layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	if (fopen_s (&output_table_file, filename_table.get_text_ascii (), "w") == 0) {
		clipper.buffer_stream_threaded (layer_facilities, layer_rivers, layer_census_tracts, router,
		length_stream_miles, cut_line_length_miles, minimum_buffer_radius_miles, maximum_buffer_radius_miles, 30, output_table_file, &view, log);
		fclose (output_table_file);
	}
	else {
		log += "ERROR, can't open output log file ";
		log += filename_table;
		log += "\n";
	}

	write_log (filename_log, log);

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnIntersectCircleCensus
	(wxCommandEvent&)

// Create buffers 25 miles up and down from a single facility
// For each buffer zone, compute intersection with Census layer

{
	dynamic_string log;
	map_layer *layer_clipped_up, *layer_clipped_down, *layer_census_tracts;
	map_object *facility;
	std::map <long long, map_object *>::const_iterator nearest_segment;
	flow_network_link *nearest_link;
	long long nearest_comid;
	std::vector <long long> downstream_ids, upstream_ids;
	ClipperBuffer clipper;
	int nearest_point_index, nearest_polygon_index;
	double offset_radius_average_1_mile;
	_timeb start, end;
	float elapsed_milliseconds;
	ClipperLib::Paths buffers_up, buffers_down, paths_clipped;
	std::vector <dynamic_string> tract_id_names;
	std::vector <double> tract_areas;
	long long buffer_id, level_path_id, next_comid_up, next_comid_down;
	bisection_definition bisection;
	std::set <long long> index_miles_run, two_point_instances;
	std::set <long long>::iterator index_miles;
	router_NHD* router = relevent_router();

	index_miles_run.insert (1);
	index_miles_run.insert (2);
	index_miles_run.insert (3);
	index_miles_run.insert (4);
	index_miles_run.insert (5);
	index_miles_run.insert (6);
	index_miles_run.insert (7);
	index_miles_run.insert (8);
	index_miles_run.insert (9);
	index_miles_run.insert (10);
	index_miles_run.insert (15);
	index_miles_run.insert (20);
	index_miles_run.insert (25);

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)
	&& panel_watershed->layer_selection) {

		if (panel_watershed->layer_selection->objects.size () == 1) {
			_ftime_s (&start);

			clipper.tract_id_source_1 = CLIPPED_ID_SOURCE_NAME;
			// clipper.use_attribute_text_index = 0;
			clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
			clipper.tract_index_source_2 = 0;
			clipper.buffer_width_output_index = 1;
			facility = panel_watershed->layer_selection->objects [0];

			nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
			nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
			nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
			nearest_segment = router->river_layer_index.find (nearest_comid);
			level_path_id = (long) nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

			log.add_formatted ("Facility\t%lld\n", facility->id);
			log.add_formatted ("Nearest COMID\t%lld\n", nearest_comid);
			log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

			if (nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

				layer_census_tracts = map_watershed->match ("Census Tracts");

				// layer for intersecting clips
				if ((layer_clipped_up = map_watershed->match ("Clipped-Up")) == NULL) {
					layer_clipped_up = create_buffer_layer ("Clipped-Up", "Facility", "Census Tract", RGB (255, 255, 0), map_watershed);
					map_watershed->layers.push_back (layer_clipped_up);
				}

				if ((layer_clipped_down = map_watershed->match ("Clipped-Down")) == NULL) {
					layer_clipped_down = create_buffer_layer ("Clipped-Down", "Facility", "Census Tract", RGB (255, 127, 39), map_watershed);
					map_watershed->layers.push_back (layer_clipped_down);
				}

				// next points up & downstream needed for find_bisection_line
				view.update_progress ("Next downstream", 0);
				// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

				view.update_progress ("Next upstream", 0);
				if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
					// nearest_link->accumulate_upstream_LevelPath_ordered (level_path_id, &router->river_layer_index, &upstream_ids);
					nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
					nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);

					{
						// List upstream and downstream ids
						std::vector <long long>::const_iterator stream;
						log += "\nDownstream\t";
						for (stream = downstream_ids.begin ();
						stream != downstream_ids.end ();
						++stream)
							log.add_formatted (" %lld", *stream);
						log += "\nUpstream\t";
						for (stream = upstream_ids.begin ();
						stream != upstream_ids.end ();
						++stream)
							log.add_formatted (" %lld", *stream);
						log += "\n";
					}

					{
						// Compute radius for 1 mile
						logical_coordinate center_point;
						double offset_radius_x, offset_radius_y;
						double buffer_radius_m = 1.0 * METERS_PER_MILE;

						center_point.x = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
						center_point.y = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
						offset_radius_x = offset_longitude_meters (center_point, buffer_radius_m);
						offset_radius_y = offset_latitude_meters (center_point, buffer_radius_m);
						offset_radius_average_1_mile = (offset_radius_x + offset_radius_y) / 2.0;
					}

					view.update_progress ("Create buffers", 0);
					for (index_miles = index_miles_run.begin ();
					index_miles != index_miles_run.end ();
					++index_miles) {
						buffer_id = *index_miles;

						// offset_radius_average = offset_radius_average_1_mile * (double) index_miles;

						view.update_progress_formatted (1, "buffer %ld miles", *index_miles);

						buffers_up.clear ();
						buffers_down.clear ();

						// Create two semi-circles bisected at line to nearest river point
						clipper.make_circle (nearest_segment->second, nearest_polygon_index, nearest_point_index, next_comid_up, next_comid_down, router,
						offset_radius_average_1_mile * (double) *index_miles, &buffers_up, &buffers_down, &two_point_instances, log);

						if (layer_census_tracts) {
							paths_clipped.clear ();
							tract_id_names.clear ();
							clipper.intersect_overlapping (&buffers_up, layer_census_tracts, &paths_clipped, &tract_id_names, NULL, &view, log);
							clipper.Paths_to_map_layer (&paths_clipped, facility->id, buffer_id, &tract_id_names, NULL, layer_clipped_up, true, &view, log);

							paths_clipped.clear ();
							tract_id_names.clear ();
							clipper.intersect_overlapping (&buffers_down, layer_census_tracts, &paths_clipped, &tract_id_names, NULL, &view, log);
							clipper.Paths_to_map_layer (&paths_clipped, facility->id, buffer_id, &tract_id_names, NULL, layer_clipped_down, true, &view, log);
						}
						else {
							clipper.Paths_to_map_layer (&buffers_up, facility->id, buffer_id, NULL, NULL, layer_clipped_up, true, &view, log);
							clipper.Paths_to_map_layer (&buffers_down, facility->id, buffer_id, NULL, NULL, layer_clipped_down, true, &view, log);
						}
					}
				}
				else
					log.add_formatted ("ERROR, can't find flow_network_link for COMID %lld.\n", nearest_comid);

				layer_clipped_up->set_extent ();
				layer_clipped_down->set_extent ();

				panel_watershed->change_layers ();
				panel_watershed->change_selection_layer (layer_clipped_up);
				enable_map_controls ();

				_ftime_s (&end);

				clipper.write_area_table (layer_clipped_up, "Up", log);
				clipper.write_area_table (layer_clipped_down, "Down", log);

				elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
				log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);
			}
			else
				log.add_formatted ("ERROR, closest COMID %lld is a divergence.\n", nearest_comid);
		}
		else
			log += "ERROR, must have one point selected.\n";
	}
	else
		log += "ERROR, must have one object in a point layer selected.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnClickDistance
	(wxCommandEvent&)

// 2016-02-22 Jennifer West compare distance to ArcGIS computation

{
	// if (panel_watershed->layer_selection_source
	// && (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)
	if (panel_watershed->layer_selection)
		panel_watershed->start_measure ();
}

void frame_CARDGIS::OnCheckDistance
(wxCommandEvent&)

// 2016-02-22 Jennifer West compare distance to ArcGIS computation

{
	dialog_distance dd(this, panel_watershed->layer_selection, panel_watershed->layer_selection_source, map_watershed);
	if (dd.ShowModal() == wxID_OK) {

	}
}

void frame_CARDGIS::OnPanelMeasureDistance
	(wxCommandEvent &ev)

// panel_shapefile.OnLeftUp has sent MESSAGE_PANEL_MEASURE_DISTANCE
// user has clicked on a location to measure (from selection to)

// Message Client Data is a pointer to a device_coordinate that must be deleted

{
	dynamic_string log;
	double start_latitude [3], start_longitude [3], end_latitude [3], end_longitude [3], distance_m;
	std::vector <map_object *> ::const_iterator point;
	device_coordinate *message_point, selected_point;
    GeographicLib::Geodesic geod (GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());

	// id = ev.GetExtraLong ();
	// edit_select_id->Set (id);
	message_point = (device_coordinate *) ev.GetClientData ();

	if (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT) {

		// (-48,-115)  to (-24.67257,-124.7795)
		// (36,-4)	to (36.58216,-4.536889)
		// (79,101)    to (78.99503,101.0734)
		end_latitude[0] = (double) message_point->y / 1.0e6;
		end_longitude[0] = (double) message_point->x / 1.0e6;

		for (point = panel_watershed->layer_selection->objects.begin ();
		point != panel_watershed->layer_selection->objects.end ();
		++point) {

			(*point)->centroid (&selected_point.y, &selected_point.x);
			start_latitude[0] = (double) selected_point.y / 1.0e6;
			start_longitude[0] = (double) selected_point.x / 1.0e6;

			log += "lat1\tlon1\tlat2\tlon2\tHaversine\tGeographicLib\n";

			log.add_formatted ("%.8lf\t%.8lf\t%.8lf\t%.8lf", start_latitude[0], start_longitude[0], end_latitude[0], end_longitude[0]);
			log.add_formatted ("\t%.8lf", distance_meters (start_latitude[0], start_longitude[0], end_latitude[0], end_longitude[0]));

			geod.Inverse (start_latitude[0], start_longitude[0], end_latitude[0], end_longitude[0], distance_m);

			log.add_formatted ("\t%.8lf\n", distance_m);
		}
	}
	delete message_point;

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnIntersectCircleCensusAll
	(wxCommandEvent&)

// For each facility in point layer, compute intersection with Census layer

{

	dynamic_string log, filename_table, filename_log, layer_name_facilities, area_name;
	map_layer *layer_facilities, *layer_census_tracts;
	ClipperBuffer clipper;
	FILE *output_table_file;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);
	// message_slot threaded_messages;
	// view.attach_mutex (&threaded_messages);
	// view.destination = INTERFACE_SEND_MUTEX_SCROLL | INTERFACE_SEND_UPDATE_DIALOG;

	// get area # from filename_nhd_area
	// R:\NHDPlusv2\NHDPlus04\

	extract_area_name (filename_nhd_area, &area_name);
	filename_table = filename_nhd_area;
	filename_table += "CircleClip";
	filename_table += area_name;
	filename_table += ".txt";
	filename_log = filename_nhd_area;
	filename_log += "CircleClip";
	filename_log += area_name;
	filename_log += ".log";

	// layer_facilities name is specific to area
	layer_name_facilities = filename_nhd_area + "cwns_latlon.bin";
	layer_facilities = map_watershed->match (layer_name_facilities.get_text_ascii ());
	layer_census_tracts = map_watershed->match ("Census Tracts");
	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	if (fopen_s (&output_table_file, filename_table.get_text_ascii (), "w") == 0) {
		clipper.intersect_circles_threaded (layer_facilities, layer_rivers, layer_census_tracts, router, 30, output_table_file, &view, log);
		fclose (output_table_file);
	}
	else {
		log += "ERROR, can't open output log file ";
		log += filename_table;
		log += "\n";
	}

	write_log (filename_log, log);

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnSelectMapObject
	(wxCommandEvent &ev)

// panel_shapefile.OnLeftUp has sent MESSAGE_PANEL_SHAPEFILE_SELECT
// user has clicked on a map object, changing selection

{
	// long id;
	dynamic_string id_text;

	id_text = ev.GetString ().ToAscii ();
	edit_select_id->ChangeValue (id_text.get_text ());
}

void frame_CARDGIS::OnIntersectBufferGrid
	(wxCommandEvent&)

// Create buffers (length_stream_miles) miles up and down from a single facility
// Overlay with grid, determining upstream/downstream of each grid cell
// For each buffer zone, compute intersection with Census layer

{
	dynamic_string log;
	map_layer *layer_clipped_up, *layer_clipped_down, *layer_census_tracts, *layer_grid = NULL, *layer_river_points = NULL, *layer_grid_vectors = NULL;
	map_object *facility;
	std::map <long long, map_object *>::const_iterator nearest_segment;
	flow_network_link *nearest_link;
	long long nearest_comid, level_path_id, next_comid_up, next_comid_down;
	std::vector <long long> downstream_ids, upstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Path p;
	int index_miles, nearest_point_index, nearest_polygon_index;
	// double length_stream_km = (25.0 * METERS_PER_MILE) / 1000.0;
	// double length_stream_km = (100.0 * METERS_PER_MILE) / 1000.0;
	double radius_average_25km, offset_radius_average_1_mile, buffer_radius_average;
	double upstream_within_segment_m, downstream_within_segment_m;
	int length_stream_miles = 25, grid_size_miles = 2.0;
	double length_stream_km, cut_line_length_miles = 25.0;
	_timeb start, end;
	float elapsed_milliseconds;
	ClipperLib::Paths path_river, buffer_up_and_down, buffers_up, buffers_down, paths_clipped;
	std::vector <dynamic_string> tract_id_names;
	std::vector <double> tract_areas;
	long buffer_id;
	std::map <int, long> all_truncation_errors;
	std::map <int, long>::iterator histogram_entry;
	std::set <long long> two_point_instances;

	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	if (layer_rivers) {
		if (panel_watershed->layer_selection_source
		&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)
		&& panel_watershed->layer_selection) {

			if (panel_watershed->layer_selection->objects.size () == 1) {

				dialog_prompt dp (this, "Buffer length", &length_stream_miles);
				if (dp.ShowModal () == wxID_OK) {

					length_stream_km = ((double) length_stream_miles * METERS_PER_MILE) / 1000.0;

					interface_window_wx view;
					view.setup_wx (this);
					view.start_progress (this);

					_ftime_s (&start);

					facility = panel_watershed->layer_selection->objects [0];

					nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
					nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
					nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
					nearest_segment = router->river_layer_index.find (nearest_comid);
					level_path_id = (long) nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

					// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (0, nearest_point_index, &upstream_within_segment_m, log);
					// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (nearest_point_index, ((map_polygon *) nearest_segment->second)->total_number_of_points () - 1, &downstream_within_segment_m, log);

					log.add_formatted ("Facility\t%lld\n", facility->id);
					log.add_formatted ("Buffer length\t%d\tmiles\n", length_stream_miles);
					log.add_formatted ("Grid Size\t%d\tmiles\n", grid_size_miles);
					log.add_formatted ("Nearest COMID\t%lld\n", nearest_comid);
					log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

					if (nearest_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

						layer_census_tracts = map_watershed->match ("Census Tracts");

						if ((layer_grid = map_watershed->match ("Grid")) == NULL) {
							layer_grid = map_watershed->create_new (MAP_OBJECT_POLYGON);
							layer_grid->name = "Grid";
							layer_grid->color = RGB (0, 0, 0);
							layer_grid->line_width = 3;
							// layer_grid->draw_as = MAP_OBJECT_DRAW_FILL_AND_OUTLINE;
							layer_grid->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
							layer_grid->attribute_count_numeric = 4; // x, y, upstream, nearest COMID
							layer_grid->data_attribute_index = 2; // upstream determines color
							layer_grid->initialize_attributes = true; // clear all attributes as polygons are created

							layer_grid->colors.add_color (RGB (0, 255, 0), 1, "Upstream");
							layer_grid->colors.add_color (RGB (255, 0, 255), 2, "Downstream");
							map_watershed->layers.push_back (layer_grid);
						}

						// layer for intersecting clips
						if ((layer_clipped_up = map_watershed->match ("Clipped-Up")) == NULL) {
							layer_clipped_up = create_buffer_layer ("Clipped-Up", "Facility", "Census Tract", RGB (255, 255, 0), map_watershed);
							map_watershed->layers.push_back (layer_clipped_up);
						}

						if ((layer_clipped_down = map_watershed->match ("Clipped-Down")) == NULL) {
							layer_clipped_down = create_buffer_layer ("Clipped-Down", "Facility", "Census Tract", RGB (255, 127, 39), map_watershed);
							map_watershed->layers.push_back (layer_clipped_down);
						}

						if ((layer_river_points = map_watershed->match ("River Points")) == NULL) {
							layer_river_points = create_buffer_layer ("River Points", "Facility", "Census Tract", RGB (0, 255, 255), map_watershed);
							layer_river_points->type = MAP_OBJECT_VECTOR;
							layer_river_points->enclosure = MAP_POLYGON_NOT_ENCLOSED;
							layer_river_points->attribute_count_numeric = 1; // upstream or downstream
							map_watershed->layers.push_back (layer_river_points);
						}

						if ((layer_grid_vectors = map_watershed->match ("GridVectors")) == NULL) {
							layer_grid_vectors = map_watershed->create_new (MAP_OBJECT_VECTOR);
							layer_grid_vectors->name = "GridVectors";
							layer_grid_vectors->color = RGB (192, 192, 192);
							layer_grid_vectors->line_width = 1;
							map_watershed->layers.push_back (layer_grid_vectors);
						}


						// view.update_progress ("Accumulate downstream", 0);
						// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

						// view.update_progress ("Accumulate upstream", 0);
						if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
							nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
							nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);

							{
								// List upstream and downstream ids
								std::vector <long long>::const_iterator stream;
								log += "\nDownstream\t";
								for (stream = downstream_ids.begin ();
								stream != downstream_ids.end ();
								++stream)
									log.add_formatted (" %lld", *stream);
								log += "\nUpstream\t";
								for (stream = upstream_ids.begin ();
								stream != upstream_ids.end ();
								++stream)
									log.add_formatted (" %lld", *stream);
								log += "\n";
							}

							{
								logical_coordinate center_point;
								double offset_radius_x, offset_radius_y;
								double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE; // 25-mile buffer bisection

								center_point.x = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
								center_point.y = (double) ((map_polygon *) nearest_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
								offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
								offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
								radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
							}

							offset_radius_average_1_mile = clipper.average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, log);

							view.update_progress ("Create Paths Upstream", 0);
							p.clear ();
							// upstream_ids are in order upstream
							// so don't use reverse_iterator
							clipper.add_upstream_within_segment ((map_polygon *) nearest_segment->second, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, layer_river_points, log);
							log.add_formatted ("Distance up nearest segment\t%.2lf\tm\n", upstream_within_segment_m);
							clipper.create_paths (layer_rivers, layer_river_points, &upstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, log);
							view.update_progress ("Create Paths Downstream", 0);

							// first node of this p is closest to facility
							path_river.push_back (p);

							p.clear ();
							clipper.add_downstream_within_segment ((map_polygon *) nearest_segment->second, nearest_polygon_index, nearest_point_index, &downstream_within_segment_m, length_stream_km * 1000.0, &p, layer_river_points, log);
							log.add_formatted ("Distance down nearest segment\t%.2lf\tm\n", downstream_within_segment_m);
							clipper.create_paths_reverse (layer_rivers, layer_river_points, &downstream_ids, length_stream_km - (downstream_within_segment_m / 1000.0), &p, log);

							path_river.push_back (p);

							view.update_progress ("Create buffers", 0);
							for (index_miles = 10;
							index_miles >= 1;
							--index_miles) {
								buffer_id = (long) index_miles;
								buffer_radius_average = offset_radius_average_1_mile * (double) index_miles;

								view.update_progress_formatted (1, "buffer %d miles", index_miles);

								// clipper.Paths_to_map_layer (&path_river_upstream, facility->id, buffer_id, NULL, layer_river_points, true, &view, log);
								// clipper.Paths_to_map_layer (&path_river_downstream, facility->id, buffer_id, NULL, layer_river_points, true, &view, log);
								clipper.create_buffer (&path_river, &buffer_up_and_down, buffer_radius_average, log);

								if (index_miles == 10) {
									// center grid at nearest point, overlapping extent of buffers
									clipper.create_grid (layer_grid, grid_size_miles, length_stream_miles, nearest_segment->second, nearest_polygon_index, nearest_point_index, log);
									// clipper.designate_grid_cells (layer_grid, layer_river_points, nearest_segment->second, nearest_polygon_index, nearest_point_index, &upstream_ids, log);
									router->designate_grid_cells (layer_grid, layer_grid_vectors, layer_river_points, nearest_segment->second, nearest_polygon_index, nearest_point_index, log);
								}

								/*
								buffers_up.clear ();
								buffers_down.clear ();

								if (layer_census_tracts) {
									paths_clipped.clear ();
									tract_id_names.clear ();
									clipper.intersect_overlapping (&buffers_up, layer_census_tracts, &paths_clipped, &tract_id_names, use_utm_zone, &view, log);
									clipper.Paths_to_map_layer (&paths_clipped, facility->id, buffer_id, &tract_id_names, layer_clipped_up, true, &view, log);

									paths_clipped.clear ();
									tract_id_names.clear ();
									clipper.intersect_overlapping (&buffers_down, layer_census_tracts, &paths_clipped, &tract_id_names, use_utm_zone, &view, log);
									clipper.Paths_to_map_layer (&paths_clipped, facility->id, buffer_id, &tract_id_names, layer_clipped_down, true, &view, log);
								}
								*/
							}

							layer_clipped_up->set_extent ();
							layer_clipped_down->set_extent ();
							layer_grid->set_extent ();
							layer_grid_vectors->set_extent ();

							if (layer_river_points)
								layer_river_points->set_extent ();

							panel_watershed->change_layers ();
							panel_watershed->change_selection_layer (layer_clipped_up);
							enable_map_controls ();

							_ftime_s (&end);

							clipper.write_area_table (layer_clipped_up, "Up", log);
							clipper.write_area_table (layer_clipped_down, "Down", log);

							for (histogram_entry = all_truncation_errors.begin ();
							histogram_entry != all_truncation_errors.end ();
							++histogram_entry)
								log.add_formatted ("%d miles\t%ld\n", histogram_entry->first, histogram_entry->second);

							elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
							log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);
						}
						else
							log.add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);

					}
					else
						log.add_formatted ("ERROR, closest COMID %ld is a divergence.\n", nearest_comid);
				}
			}
			else
				log += "ERROR, must have one point selected.\n";
		}
		else
			log += "ERROR, must have point layer selected.\n";
	}
	else
		log += "ERROR no river layer.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

/*
void frame_CARDGIS::OnIntersectBufferUpstream
	(wxCommandEvent&)

// Create buffers (length_stream_miles) miles up from a single facility starting at length_offset_miles
// For each buffer zone, compute intersection with Census layer

{
	dynamic_string log;
	map_layer *layer_clipped_up, *layer_census_tracts, *layer_river_points = NULL;
	map_object *facility, *starting_segment;
	std::map <long, map_object *>::const_iterator facility_segment;
	flow_network_link *nearest_link;
	long nearest_comid, level_path_id, next_comid_up;
	std::vector <long> upstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Path p;
	int index_miles, nearest_point_index, nearest_polygon_index;
	double radius_average_25km, offset_radius_average_1_mile, buffer_radius_average;
	double upstream_within_segment_m;
	double length_stream_km, start_upstream_km;
	_timeb start, end;
	float elapsed_milliseconds;
	ClipperLib::Paths path_river, buffers_up, paths_clipped;
	std::vector <dynamic_string> tract_id_names;
	std::vector <double> tract_areas;
	long buffer_id;
	int use_utm_zone = 0; // area_utm_zone
	bool skip;

	double length_stream_miles = 25.0;
	// double start_upstream_miles = 12.5;
	double start_upstream_miles = 0.0;

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)
	&& panel_watershed->layer_selection) {

		if (panel_watershed->layer_selection->objects.size () == 1) {

			dialog_prompt dp (this, "Buffer length", &length_stream_miles);
			if (dp.ShowModal () == wxID_OK) {

				length_stream_km = (length_stream_miles * METERS_PER_MILE) / 1000.0;
				start_upstream_km = (start_upstream_miles * METERS_PER_MILE) / 1000.0;

				interface_window_wx view;
				view.setup_wx (this);
				view.start_progress (this);

				_ftime_s (&start);

				facility = panel_watershed->layer_selection->objects [0];

				nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
				nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
				nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
				facility_segment = router->river_layer_index.find (nearest_comid);
				level_path_id = (long) facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

				log.add_formatted ("Facility\t%ld\n", facility->id);
				log.add_formatted ("Buffer length\t%.2lf\tmiles\t%.2lf\tkm\n", length_stream_miles, length_stream_km);
				log.add_formatted ("Buffer start upstream\t%.2lf\tmiles\t%.2lf\tkm\n", start_upstream_miles, start_upstream_km);
				log.add_formatted ("Nearest COMID\t%ld\n", nearest_comid);
				log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

					if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

						layer_census_tracts = map_watershed->match ("Census Tracts");

						// layer for intersecting clips
						if ((layer_clipped_up = map_watershed->match ("Clipped-Up")) == NULL) {
							layer_clipped_up = create_buffer_layer ("Clipped-Up", RGB (255, 255, 0));
							map_watershed->layers.push_back (layer_clipped_up);
						}

						if ((layer_river_points = map_watershed->match ("River Points")) == NULL) {
							layer_river_points = create_buffer_layer ("River Points", RGB (0, 255, 255));
							layer_river_points->type = MAP_OBJECT_VECTOR;
							layer_river_points->attribute_count_numeric = 1; // upstream or downstream
							map_watershed->layers.push_back (layer_river_points);
						}

						// view.update_progress ("Accumulate downstream", 0);
						// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

						// view.update_progress ("Accumulate upstream", 0);
						if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
							nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);

							skip = false;
							if (start_upstream_miles > 0.0) {
								int starting_polygon_index, starting_point_index;

								if (clipper.find_start_point (facility_segment->second, nearest_polygon_index, nearest_point_index, layer_rivers, &upstream_ids, start_upstream_km,
								&starting_segment, &starting_polygon_index, &starting_point_index, true, log)) {

									nearest_comid = starting_segment->id;
									nearest_polygon_index = starting_polygon_index;
									nearest_point_index = starting_point_index;

									// rebuild upstream_ids starting at starting_segment
									upstream_ids.clear ();
									if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
										nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
									else {
										log.add_formatted ("ERROR, can't find flow_network_link for new starting comid %ld.\n", nearest_comid);
										skip = true;
									}
								}
								else
									skip = true;
							}
							else
								starting_segment = facility_segment->second;

							if (!skip) {
								{
									// List upstream ids
									std::vector <long>::const_iterator stream;
									log += "\nUpstream\t";
									for (stream = upstream_ids.begin ();
									stream != upstream_ids.end ();
									++stream)
										log.add_formatted (" %ld", *stream);
									log += "\n";
								}

								{
									logical_coordinate center_point;
									double offset_radius_x, offset_radius_y;
									double bisect_radius_m = BUFFER_CUT_LINE_LENGTH * METERS_PER_MILE; // 25-mile buffer bisection

									center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
									center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
									offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
									offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
									radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
								}

								// computed distance from nearest point in nearest segment up & down

								// discard ids beyond length_downstream_km

								// Compute offset_radius_average for 1 mile, then multiply it for each of the others
								offset_radius_average_1_mile = clipper.average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, log);

								view.update_progress ("Create Paths Upstream", 0);

								// upstream_ids are in order upstream
								// so don't use reverse_iterator
								p.clear ();
								clipper.add_upstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, layer_river_points, log);
								clipper.create_paths (layer_rivers, layer_river_points, &upstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, log);
								view.update_progress ("Create Paths Downstream", 0);
								path_river.push_back (p);

								// clipper.dump_paths (&path_river, log);

								view.update_progress ("Create buffers", 0);
								for (index_miles = 1;
								index_miles <= 10;
								++index_miles) {
									buffer_id = (long) index_miles;
									if (use_utm_zone != 0)
										// Just meters
										buffer_radius_average = (double) index_miles * METERS_PER_MILE;
									else
										buffer_radius_average = offset_radius_average_1_mile * (double) index_miles;

									view.update_progress_formatted (1, "buffer %d miles", index_miles);

									clipper.create_buffer (&path_river, &buffers_up, buffer_radius_average, log);

									if (layer_census_tracts) {
										paths_clipped.clear ();
										tract_id_names.clear ();
										clipper.intersect_overlapping (&buffers_up, layer_census_tracts, &paths_clipped, &tract_id_names, use_utm_zone, &view, log);
										clipper.Paths_to_map_layer (&paths_clipped, facility->id, buffer_id, &tract_id_names, layer_clipped_up, true, &view, log);
									}
									else
										// show buffers
										clipper.Paths_to_map_layer (&buffers_up, facility->id, buffer_id, NULL, layer_clipped_up, true, &view, log);
								}

								layer_clipped_up->set_extent ();

								if (layer_river_points)
									layer_river_points->set_extent ();

								panel_watershed->change_layers ();
								panel_watershed->change_selection_layer (layer_clipped_up);
								enable_map_controls ();

								_ftime_s (&end);

								clipper.write_area_table (layer_clipped_up, "Up", log);

								elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
								log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);
							}
						}
						else
							log.add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);

					}
					else
						log.add_formatted ("ERROR, closest COMID %ld is a divergence.\n", nearest_comid);
			}
		}
		else
			log += "ERROR, must have one point selected.\n";
	}
	else
		log += "ERROR, must have point layer selected.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}
*/

void frame_CARDGIS::OnIntersectBuffer
	(wxCommandEvent&)

// Create buffers (length_stream_miles) miles up from a single facility starting at length_offset_miles
// For each buffer zone, compute intersection with Census layer

{
	dynamic_string log;
	map_object *facility = NULL;
	map_layer *facility_layer, *layer_polygons;
	_timeb start, end;
	std::vector <double> buffer_widths;
	short tract_name_source;
	int tract_source_numeric, tract_source_text;

	/*
	buffer_widths.push_back (1);
	buffer_widths.push_back (2);
	buffer_widths.push_back (3);
	buffer_widths.push_back (4);
	buffer_widths.push_back (5);
	buffer_widths.push_back (6);
	buffer_widths.push_back (7);
	buffer_widths.push_back (8);
	buffer_widths.push_back (9);
	buffer_widths.push_back (10);
	*/
	buffer_widths.push_back (0.25);
	buffer_widths.push_back (50);
	buffer_widths.push_back (100);

	double length_upstream_miles = 25.0, start_upstream_miles = 0.0, length_downstream_miles = 0.0, start_downstream_miles = 0.0;
	double cut_line_length_miles = 25.0;

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)
	&& panel_watershed->layer_selection) {
		facility_layer = panel_watershed->layer_selection_source;
		if (panel_watershed->layer_selection->objects.size () == 1)
			facility = panel_watershed->layer_selection->objects [0];
	}

	layer_polygons = map_watershed->match ("Census Tracts");

	dialog_buffer dp (this, map_watershed, &facility_layer, &facility, &layer_polygons, &buffer_widths, &start_upstream_miles, &length_upstream_miles, &start_downstream_miles, &length_downstream_miles, &cut_line_length_miles);
	if (dp.ShowModal () == wxID_OK) {
		float elapsed_milliseconds;

		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		_ftime_s (&start);

		if (layer_polygons->name.match_insensitive ("PlaceTract") != -1) {
			// FID, AREAKEY, PLCIDFP00
			tract_name_source = CLIPPED_ID_SOURCE_THREE_NAMES;
			tract_source_numeric = -1;
			tract_source_text = 2;

		}
		else {
			// census layer
			tract_name_source = CLIPPED_ID_SOURCE_NAME;
			tract_source_numeric = -1;
			tract_source_text = 0;
		}

		if ((length_upstream_miles > 0.0)
		&& (length_downstream_miles > 0.0))
			intersect_buffer_up_and_down (facility, layer_polygons, &buffer_widths, start_upstream_miles, length_upstream_miles, start_downstream_miles, length_downstream_miles, cut_line_length_miles,
			tract_name_source, tract_source_numeric, tract_source_text, &view, log);
		else
			if (length_upstream_miles > 0.0) {
				if (start_downstream_miles > 0.0)
					intersect_buffer_upstream_difference (facility, layer_polygons, &buffer_widths, start_upstream_miles, length_upstream_miles, cut_line_length_miles,
					tract_name_source, tract_source_numeric, tract_source_text, &view, log);
				else
					intersect_buffer_upstream (facility, layer_polygons, &buffer_widths, start_upstream_miles, length_upstream_miles, cut_line_length_miles,
					tract_name_source, tract_source_numeric, tract_source_text, &view, log);
			}
			else
				if (length_downstream_miles > 0.0) {
					if (start_downstream_miles > 0.0)
						intersect_buffer_downstream_difference (facility, layer_polygons, &buffer_widths, start_downstream_miles, length_downstream_miles, cut_line_length_miles,
						tract_name_source, tract_source_numeric, tract_source_text, &view, log);
					else
						intersect_buffer_downstream (facility, layer_polygons, &buffer_widths, start_downstream_miles, length_downstream_miles, cut_line_length_miles,
						tract_name_source, tract_source_numeric, tract_source_text, &view, log);
				}

		_ftime_s (&end);

		elapsed_milliseconds = ((float) end.time * 1000.0f + (float) end.millitm) - ((float) start.time * 1000.0f + (float) start.millitm);
		log.add_formatted ("Elapsed: %.3f seconds\n", elapsed_milliseconds / 1000.0f);

		panel_watershed->change_layers ();
		enable_map_controls ();
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Route Table", L"", log);
	d.ShowWindowModal ();
}

bool frame_CARDGIS::intersect_buffer_up_and_down
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
	class interface_window *view,
	dynamic_string &log)

// Create buffers (length_stream_miles) miles up and down from a single facility
// For each buffer zone, compute intersection with Census layer

{
	map_layer *layer_clipped_up, *layer_clipped_down, *layer_bisection_line = NULL, *layer_river_points = NULL;
	std::map <long long, map_object *>::const_iterator facility_segment;
	long long nearest_comid, level_path_id, next_comid_up = 0, next_comid_down = 0;
	std::vector <long long> downstream_ids, upstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Path p;
	flow_network_link *nearest_link;
	int nearest_point_index, nearest_polygon_index;
	std::vector <double>::const_iterator index_miles;
	// double length_stream_km = (25.0 * METERS_PER_MILE) / 1000.0;
	// double length_stream_km = (100.0 * METERS_PER_MILE) / 1000.0;
	double radius_average_25km, offset_radius_average_1_mile, theta, buffer_radius_average;
	double upstream_within_segment_m, downstream_within_segment_m;
	double length_upstream_km, length_downstream_km, start_upstream_km, start_downstream_km;
	ClipperLib::Paths path_river, buffer_up_and_down, buffers_up, buffers_down, paths_clipped;
	std::vector <dynamic_string> tract_id_names;
	std::vector <double> tract_areas;
	bisection_definition bisection;
	bool show_bisection_line = true;
	std::map <double, long> all_truncation_errors;
	std::map <double, long>::iterator histogram_entry;
	std::set <long long> two_point_instances;
	bool verbose = true, error = false;
	map_object *start_segment_upstream, *start_segment_downstream;

	length_upstream_km = ((double) length_upstream_miles * METERS_PER_MILE) / 1000.0;
	start_upstream_km = ((double) start_upstream_miles * METERS_PER_MILE) / 1000.0;
	length_downstream_km = ((double) length_downstream_miles * METERS_PER_MILE) / 1000.0;
	start_downstream_km = ((double) start_downstream_miles * METERS_PER_MILE) / 1000.0;

	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	//2018-04-12 Clipper's tracts will use Census AREAKEY name field
	clipper.tract_id_source_1 = tract_name_source;
	clipper.tract_index_source_1 = tract_numeric_attribute_id;
	clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
	clipper.tract_index_source_2 = tract_text_attribute_id;
	clipper.buffer_width_output_index = 1;

	if (layer_rivers) {
		nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
		nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
		nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
		facility_segment = router->river_layer_index.find (nearest_comid);
		level_path_id = (long) facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

		// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (0, nearest_point_index, &upstream_within_segment_m, log);
		// ((map_polygon *) nearest_segment->second)->perimeter_between_meters (nearest_point_index, ((map_polygon *) nearest_segment->second)->total_number_of_points () - 1, &downstream_within_segment_m, log);

		log.add_formatted ("Facility\t%lld\n", facility->id);
		log.add_formatted ("Buffer Length Up\t%.1lf\tmiles\tDown\t%.1lf\n", length_upstream_miles, length_downstream_miles);
		log.add_formatted ("Nearest COMID\t%lld\n", nearest_comid);
		log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

		if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// layer for intersecting clips
			if ((layer_clipped_up = map_watershed->match ("Clipped-Up")) == NULL) {
				layer_clipped_up = create_buffer_layer ("Clipped-Up", "Facility", "Census Tract", RGB (255, 255, 0), map_watershed);
				map_watershed->layers.push_back (layer_clipped_up);
			}

			if ((layer_clipped_down = map_watershed->match ("Clipped-Down")) == NULL) {
				layer_clipped_down = create_buffer_layer ("Clipped-Down", "Facility", "Census Tract", RGB (255, 127, 39), map_watershed);
				map_watershed->layers.push_back (layer_clipped_down);
			}

			if (show_bisection_line) {
				if ((layer_bisection_line = map_watershed->match ("Bisection")) == NULL) {
					layer_bisection_line = map_watershed->create_new (MAP_OBJECT_VECTOR);
					layer_bisection_line->name = "Bisection";
					layer_bisection_line->color = RGB (0, 0, 0);
					layer_bisection_line->line_width = 2;
					layer_bisection_line->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
					layer_bisection_line->initialize_attributes = true; // clear all attributes as polygons are created
					map_watershed->layers.push_back (layer_bisection_line);
				}
			}

			if ((layer_river_points = map_watershed->match ("River Points")) == NULL) {
				layer_river_points = create_buffer_layer ("River Points", "Facility", "Census Tract", RGB (0, 255, 255), map_watershed);
				layer_river_points->type = MAP_OBJECT_VECTOR;
				layer_river_points->enclosure = MAP_POLYGON_NOT_ENCLOSED;
				layer_river_points->attribute_count_numeric = 1; // upstream or downstream
				map_watershed->layers.push_back (layer_river_points);
			}

			if (start_upstream_miles > 0.0) {
				int starting_polygon_index, starting_point_index;

				if (clipper.find_start_point_upstream (facility_segment->second, nearest_polygon_index, nearest_point_index, layer_rivers, &upstream_ids, start_upstream_km,
				&start_segment_upstream, &starting_polygon_index, &starting_point_index, true, log)) {

					nearest_comid = start_segment_upstream->id;
					nearest_polygon_index = starting_polygon_index;
					nearest_point_index = starting_point_index;

					// rebuild upstream_ids starting at starting_segment
					upstream_ids.clear ();
					if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
						nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
					else {
						log.add_formatted ("ERROR, can't find flow_network_link for new starting comid %ld.\n", nearest_comid);
						error = true;
					}
				}
				else
					error = true;
			}
			else {
				start_segment_upstream = facility_segment->second;
				if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
					nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
				else {
					log.add_formatted ("ERROR, can't find flow_network_link for new starting comid %ld.\n", nearest_comid);
					error = true;
				}
			}

			if (start_downstream_miles > 0.0) {
				int starting_polygon_index, starting_point_index;

				if (clipper.find_start_point_downstream (facility_segment->second, nearest_polygon_index, nearest_point_index, layer_rivers, &downstream_ids, start_downstream_km,
				&start_segment_downstream, &starting_polygon_index, &starting_point_index, true, log)) {

					nearest_comid = start_segment_downstream->id;
					nearest_polygon_index = starting_polygon_index;
					nearest_point_index = starting_point_index;

					// rebuild upstream_ids starting at starting_segment
					downstream_ids.clear ();
					if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
						nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);
					else {
						log.add_formatted ("ERROR, can't find flow_network_link for new starting comid %lld.\n", nearest_comid);
						error = true;
					}
				}
				else
					error = true;
			}
			else {
				start_segment_downstream = facility_segment->second;
				if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
					nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);
				else {
					log.add_formatted ("ERROR, can't find flow_network_link for new starting comid %lld.\n", nearest_comid);
					error = true;
				}
			}

			if (!error) {
				{
					// List upstream and downstream ids
					std::vector <long long>::const_iterator stream;
					log += "\nDownstream\t";
					for (stream = downstream_ids.begin ();
					stream != downstream_ids.end ();
					++stream)
						log.add_formatted (" %lld", *stream);
					log += "\nUpstream\t";
					for (stream = upstream_ids.begin ();
					stream != upstream_ids.end ();
					++stream)
						log.add_formatted (" %lld", *stream);
					log += "\n";
				}

				{
					logical_coordinate center_point;
					double offset_radius_x, offset_radius_y;
					double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE; // 25-mile buffer bisection

					center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
					center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
					offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
					offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
					radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
				}

				clipper.find_bisection_line (facility_segment->second, nearest_polygon_index, nearest_point_index, next_comid_up, next_comid_down, router, radius_average_25km, &bisection, &theta, &two_point_instances, verbose, log);
				if (show_bisection_line)
					bisection.create_map_object (layer_bisection_line, log);

				// computed distance from nearest point in nearest segment up & down

				// discard ids beyond length_downstream_km

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper.average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, log);

				view->update_progress ("Create Paths Upstream", 0);

				p.clear ();
				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				clipper.add_upstream_within_segment ((map_polygon *) start_segment_upstream, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_upstream_km * 1000.0, &p, layer_river_points, log);
				log.add_formatted ("Distance up nearest segment\t%.2lf\tm\n", upstream_within_segment_m);
				clipper.create_paths (layer_rivers, layer_river_points, &upstream_ids, length_upstream_km - (upstream_within_segment_m / 1000.0), &p, log);
				view->update_progress ("Create Paths Downstream", 0);
				path_river.push_back (p);

				p.clear ();
				clipper.add_downstream_within_segment ((map_polygon *) start_segment_downstream, nearest_polygon_index, nearest_point_index, &downstream_within_segment_m, length_downstream_km * 1000.0, &p, layer_river_points, log);
				log.add_formatted ("Distance down nearest segment\t%.2lf\tm\n", downstream_within_segment_m);
				clipper.create_paths_reverse (layer_rivers, layer_river_points, &downstream_ids, length_downstream_km - (downstream_within_segment_m / 1000.0), &p, log);

				path_river.push_back (p);

				view->update_progress ("Create buffers", 0);
				for (index_miles = buffer_widths->begin ();
				index_miles != buffer_widths->end ();
				++index_miles) {
					buffer_radius_average = offset_radius_average_1_mile * *index_miles;

					view->update_progress_formatted (1, "buffer %.1lf miles", *index_miles);

					// clipper.Paths_to_map_layer (&path_river, facility->id, buffer_id, NULL, layer_river_points, true, &view, log);
					clipper.create_buffer (&path_river, &buffer_up_and_down, buffer_radius_average, log);
					// clipper.Paths_to_map_layer (&path_buffers, layer_buffer, &view, log);
					buffers_up.clear ();
					buffers_down.clear ();

					if (clipper.cut_buffer (facility->id, *index_miles, &all_truncation_errors, &bisection, &buffer_up_and_down, &buffers_up, &buffers_down, log)) {

						if (layer_census_tracts) {
							paths_clipped.clear ();
							tract_id_names.clear ();
							clipper.intersect_overlapping (&buffers_up, layer_census_tracts, &paths_clipped, &tract_id_names, NULL, view, log);
							clipper.Paths_to_map_layer (&paths_clipped, facility->id, *index_miles, &tract_id_names, NULL, layer_clipped_up, true, view, log);

							paths_clipped.clear ();
							tract_id_names.clear ();
							clipper.intersect_overlapping (&buffers_down, layer_census_tracts, &paths_clipped, &tract_id_names, NULL, view, log);
							clipper.Paths_to_map_layer (&paths_clipped, facility->id, *index_miles, &tract_id_names, NULL, layer_clipped_down, true, view, log);
						}
					}
					else
						// show buffers & bisection line
						clipper.Paths_to_map_layer (&buffer_up_and_down, facility->id, *index_miles, NULL, NULL, layer_clipped_up, true, view, log);
				}

				layer_clipped_up->set_extent ();
				layer_clipped_down->set_extent ();
				if (show_bisection_line)
					layer_bisection_line->set_extent ();

				if (layer_river_points)
					layer_river_points->set_extent ();

				clipper.write_area_table (layer_clipped_up, "Up", log);
				clipper.write_area_table (layer_clipped_down, "Down", log);

				for (histogram_entry = all_truncation_errors.begin ();
				histogram_entry != all_truncation_errors.end ();
				++histogram_entry)
					log.add_formatted ("%d miles\t%ld\n", histogram_entry->first, histogram_entry->second);
			}
		}
	}
	else
		log += "ERROR, no river layer.\n";

	return !error;
}

void frame_CARDGIS::intersect_buffer_upstream
	(const map_object *facility,
	const map_layer *layer_census_tracts,
	std::vector <double> *buffer_widths,
	double start_upstream_miles,
	double length_upstream_miles,
	const double cut_line_length_miles,
	const short tract_name_source,
	const int tract_numeric_attribute_id,
	const int tract_text_attribute_id,
	interface_window *view,
	dynamic_string &log)

// Create buffers (length_stream_miles) miles up from a single facility starting at length_offset_miles
// For each buffer zone, compute intersection with Census layer

{
	map_layer *layer_clipped_up, *layer_river_points = NULL;
	map_object *starting_segment = NULL;
	std::map <long long, map_object *>::const_iterator facility_segment;
	flow_network_link *nearest_link;
	long long nearest_comid, level_path_id, next_comid_up;
	std::vector <long long> upstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Path p;
	int nearest_point_index, nearest_polygon_index;
	std::vector <double>::const_iterator index_miles;
	double radius_average_25km, offset_radius_average_1_mile, buffer_radius_average;
	double upstream_within_segment_m;
	double length_stream_km, start_upstream_km;
	ClipperLib::Paths path_river, buffers_up, paths_clipped;
	std::vector <dynamic_string> tract_id_names;
	std::vector <double> tract_areas;
	bool skip;

	length_stream_km = (length_upstream_miles * METERS_PER_MILE) / 1000.0;
	start_upstream_km = (start_upstream_miles * METERS_PER_MILE) / 1000.0;

	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	//2018-04-12 Clipper's tracts will use Census AREAKEY name field
	clipper.tract_id_source_1 = tract_name_source;
	clipper.tract_index_source_1 = tract_numeric_attribute_id;
	clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
	clipper.tract_index_source_2 = tract_text_attribute_id;
	clipper.buffer_width_output_index = 1;

	if (layer_rivers) {
		nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
		nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
		nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
		facility_segment = router->river_layer_index.find (nearest_comid);
		level_path_id = (long) facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

		log.add_formatted ("Facility\t%lld\n", facility->id);
		log.add_formatted ("Buffer length\t%.2lf\tmiles\t%.2lf\tkm\n", length_upstream_miles, length_stream_km);
		log.add_formatted ("Buffer start upstream\t%.2lf\tmiles\t%.2lf\tkm\n", start_upstream_miles, start_upstream_km);
		log.add_formatted ("Nearest COMID\t%lld\n", nearest_comid);
		log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

		if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// layer for intersecting clips
			if ((layer_clipped_up = map_watershed->match ("Clipped-Up")) == NULL) {
				layer_clipped_up = create_buffer_layer ("Clipped-Up", "Facility", "Census Tract", RGB (255, 255, 0), map_watershed);
				map_watershed->layers.push_back (layer_clipped_up);
			}

			if ((layer_river_points = map_watershed->match ("River Points")) == NULL) {
				layer_river_points = create_buffer_layer ("River Points", "Facility", "Census Tract", RGB (0, 255, 255), map_watershed);
				layer_river_points->type = MAP_OBJECT_VECTOR;
				layer_river_points->enclosure = MAP_POLYGON_NOT_ENCLOSED;
				layer_river_points->attribute_count_numeric = 1; // upstream or downstream
				map_watershed->layers.push_back (layer_river_points);
			}

			// view.update_progress ("Accumulate downstream", 0);
			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			// view.update_progress ("Accumulate upstream", 0);
			skip = false;
			if (start_upstream_miles > 0.0) {
				int starting_polygon_index, starting_point_index;

				if (clipper.find_start_point_upstream (facility_segment->second, nearest_polygon_index, nearest_point_index, layer_rivers, &upstream_ids, start_upstream_km,
				&starting_segment, &starting_polygon_index, &starting_point_index, true, log)) {

					nearest_comid = starting_segment->id;
					nearest_polygon_index = starting_polygon_index;
					nearest_point_index = starting_point_index;

					// build upstream_ids starting at starting_segment
					upstream_ids.clear ();
					if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
						nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
					else {
						log.add_formatted ("ERROR, can't find flow_network_link for new starting comid %ld.\n", nearest_comid);
						skip = true;
					}
				}
				else
					skip = true;
			}
			else {
				if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
					nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
					starting_segment = facility_segment->second;
				}
				else {
					log.add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
					skip = true;
				}
			}

			if (!skip) {
				{
					// List upstream ids
					std::vector <long long>::const_iterator stream;
					log += "\nUpstream\t";
					for (stream = upstream_ids.begin ();
					stream != upstream_ids.end ();
					++stream)
						log.add_formatted (" %lld", *stream);
					log += "\n";
				}

				{
					logical_coordinate center_point;
					double offset_radius_x, offset_radius_y;
					double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE; // 25-mile buffer bisection

					center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
					center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
					offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
					offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
					radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
				}

				// computed distance from nearest point in nearest segment up & down

				// discard ids beyond length_downstream_km

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper.average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, log);

				view->update_progress ("Create Paths Upstream", 0);

				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				p.clear ();
				clipper.add_upstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, layer_river_points, log);
				clipper.create_paths (layer_rivers, layer_river_points, &upstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, log);
				view->update_progress ("Create Paths Downstream", 0);
				path_river.push_back (p);

				// clipper.dump_paths (&path_river, log);

				view->update_progress ("Create buffers", 0);
				for (index_miles = buffer_widths->begin ();
				index_miles != buffer_widths->end ();
				++index_miles) {
					buffer_radius_average = offset_radius_average_1_mile * *index_miles;

					view->update_progress_formatted (1, "buffer %d miles", *index_miles);

					clipper.create_buffer (&path_river, &buffers_up, buffer_radius_average, log);

					if (layer_census_tracts) {
						paths_clipped.clear ();
						tract_id_names.clear ();
						clipper.intersect_overlapping (&buffers_up, layer_census_tracts, &paths_clipped, &tract_id_names, NULL, view, log);
						clipper.Paths_to_map_layer (&paths_clipped, facility->id, *index_miles, &tract_id_names, NULL, layer_clipped_up, true, view, log);
					}
					else
						// show buffers
						clipper.Paths_to_map_layer (&buffers_up, facility->id, *index_miles, NULL, NULL, layer_clipped_up, true, view, log);
				}

				layer_clipped_up->set_extent ();

				if (layer_river_points)
					layer_river_points->set_extent ();

				clipper.write_area_table (layer_clipped_up, "Up", log);
			}
		}
		else
			log.add_formatted ("ERROR, closest COMID %ld is a divergence.\n", nearest_comid);
	}
	else
		log += "ERROR, no river layer.\n";
}

void frame_CARDGIS::intersect_buffer_upstream_difference
	(const map_object *facility,
	const map_layer *layer_polygons,
	std::vector <double> *buffer_widths,
	double start_upstream_miles,
	double length_upstream_miles,
	const double cut_line_length_miles,
	const short tract_name_source,
	const int tract_numeric_attribute_id,
	const int tract_text_attribute_id,
	interface_window *view,
	dynamic_string &log)

// Compute buffer area between start & length

{
	map_layer *layer_clipped_outside, *layer_clipped_inside, *layer_census_tracts, *layer_river_points_in = NULL, *layer_river_points_out = NULL;
	map_object *starting_segment = NULL;
	std::map <long long, map_object *>::const_iterator facility_segment;
	flow_network_link *nearest_link;
	long long nearest_comid, level_path_id, next_comid_up;
	std::vector <long long> upstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Path p;
	int nearest_point_index, nearest_polygon_index;
	std::vector <double>::const_iterator index_miles;
	double radius_average_25km, offset_radius_average_1_mile, buffer_radius_average;
	double upstream_within_segment_m;
	double length_stream_km, start_stream_km;
	ClipperLib::Paths path_river_inside, path_river_outside, buffers_inside, buffers_outside, paths_clipped_inside, paths_clipped_outside;
	std::vector <dynamic_string> tract_id_names_inside, tract_id_names_outside;
	std::vector <dynamic_string>::const_iterator tract_name_outside;
	std::vector <double> tract_areas;
	std::vector < ClipperLib::Path >::const_iterator clipped_path;
	std::map <dynamic_string, double *> outside_areas;
	std::map <dynamic_string, double *>::iterator outside_area;
	bool skip;

	length_stream_km = (length_upstream_miles * METERS_PER_MILE) / 1000.0;
	start_stream_km = (start_upstream_miles * METERS_PER_MILE) / 1000.0;

	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	//2018-04-12 Clipper's tracts will use Census AREAKEY name field
	clipper.tract_id_source_1 = tract_name_source;
	clipper.tract_index_source_1 = tract_numeric_attribute_id;
	clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
	clipper.tract_index_source_2 = tract_text_attribute_id;
	clipper.buffer_width_output_index = 1;

	if (layer_rivers) {
		nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
		nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
		nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
		facility_segment = router->river_layer_index.find (nearest_comid);
		level_path_id = (long) facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

		log.add_formatted ("Facility\t%lld\n", facility->id);
		log.add_formatted ("Buffer length\t%.2lf\tmiles\t%.2lf\tkm\n", length_upstream_miles, length_stream_km);
		log.add_formatted ("Buffer start upstream\t%.2lf\tmiles\t%.2lf\tkm\n", start_upstream_miles, start_stream_km);
		log.add_formatted ("Nearest COMID\t%lld\n", nearest_comid);
		log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

		if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			layer_census_tracts = map_watershed->match ("Census Tracts");

			// layer for intersecting clips
			if ((layer_clipped_outside = map_watershed->match ("Clipped-Outside")) == NULL) {
				layer_clipped_outside = create_buffer_layer ("Clipped-Outside", "Facility", "Census Tract", RGB (255, 201, 14), map_watershed);
				map_watershed->layers.push_back (layer_clipped_outside);
			}
			if ((layer_clipped_inside = map_watershed->match ("Clipped-Inside")) == NULL) {
				layer_clipped_inside = create_buffer_layer ("Clipped-Inside", "Facility", "Census Tract", RGB (255, 255, 0), map_watershed);
				map_watershed->layers.push_back (layer_clipped_inside);
			}

			if ((layer_river_points_out = map_watershed->match ("River Points Outer")) == NULL) {
				layer_river_points_out = create_buffer_layer ("River Points Outer", "Facility", "Census Tract", RGB (128, 128, 192), map_watershed);
				layer_river_points_out->type = MAP_OBJECT_VECTOR;
				layer_river_points_out->enclosure = MAP_POLYGON_NOT_ENCLOSED;
				layer_river_points_out->attribute_count_numeric = 1; // upstream or downstream
				map_watershed->layers.push_back (layer_river_points_out);
			}
			if ((layer_river_points_in = map_watershed->match ("River Points Inner")) == NULL) {
				layer_river_points_in = create_buffer_layer ("River Points Inner", "Facility", "Census Tract", RGB (0, 255, 255), map_watershed);
				layer_river_points_in->type = MAP_OBJECT_VECTOR;
				layer_river_points_in->enclosure = MAP_POLYGON_NOT_ENCLOSED;
				layer_river_points_in->attribute_count_numeric = 1; // upstream or downstream
				map_watershed->layers.push_back (layer_river_points_in);
			}

			// view.update_progress ("Accumulate downstream", 0);
			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			// view.update_progress ("Accumulate upstream", 0);
			skip = false;
			if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
				nearest_up (nearest_link, level_path_id, &upstream_ids, &next_comid_up, router);
				starting_segment = facility_segment->second;
			}
			else {
				log.add_formatted ("ERROR, can't find flow_network_link for COMID %lld.\n", nearest_comid);
				skip = true;
			}

			if (!skip) {
				{
					// List upstream ids
					std::vector <long long>::const_iterator stream;
					log += "\nUpstream\t";
					for (stream = upstream_ids.begin ();
					stream != upstream_ids.end ();
					++stream)
						log.add_formatted (" %lld", *stream);
					log += "\n";
				}

				{
					logical_coordinate center_point;
					double offset_radius_x, offset_radius_y;
					double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE; // 25-mile buffer bisection

					center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
					center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
					offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
					offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
					radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
				}

				// computed distance from nearest point in nearest segment up & down

				// discard ids beyond length_downstream_km

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper.average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, log);

				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				view->update_progress ("Create Paths Inside", 0);
				p.clear ();
				clipper.add_upstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, start_stream_km * 1000.0, &p, layer_river_points_in, log);
				clipper.create_paths (layer_rivers, layer_river_points_in, &upstream_ids, start_stream_km - (upstream_within_segment_m / 1000.0), &p, log);
				path_river_inside.push_back (p);

				view->update_progress ("Create Paths OutSide", 0);
				p.clear ();
				clipper.add_upstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &upstream_within_segment_m, length_stream_km * 1000.0, &p, layer_river_points_out, log);
				clipper.create_paths (layer_rivers, layer_river_points_out, &upstream_ids, length_stream_km - (upstream_within_segment_m / 1000.0), &p, log);
				path_river_outside.push_back (p);

				view->update_progress ("Create buffers", 0);
				for (index_miles = buffer_widths->begin ();
				index_miles != buffer_widths->end ();
				++index_miles) {
					buffer_radius_average = offset_radius_average_1_mile * *index_miles;

					view->update_progress_formatted (1, "buffer %.2lf miles", *index_miles);
				
					/*
					{
						// List upstream ids
						std::vector <long>::const_iterator stream;
						log += "\nUpstream\t";
						for (stream = upstream_ids.begin ();
						stream != upstream_ids.end ();
						++stream)
							log.add_formatted (" %ld", *stream);
						log += "\n";

						log += "Inside\n";
						clipper.dump_paths (&path_river_inside, log);
						log += "Outside\n";
						clipper.dump_paths (&path_river_outside, log);
					}
					*/
					clipper.create_buffer (&path_river_inside, &buffers_inside, buffer_radius_average, log);
					clipper.create_buffer (&path_river_outside, &buffers_outside, buffer_radius_average, log);

					if (layer_census_tracts) {
						paths_clipped_inside.clear ();
						paths_clipped_outside.clear ();
						tract_id_names_inside.clear ();
						tract_id_names_outside.clear ();
						clipper.intersect_overlapping (&buffers_inside, layer_census_tracts, &paths_clipped_inside, &tract_id_names_inside, NULL, view, log);
						clipper.intersect_overlapping (&buffers_outside, layer_census_tracts, &paths_clipped_outside, &tract_id_names_outside, NULL, view, log);

						clipper.Paths_to_map_layer (&paths_clipped_inside, facility->id, *index_miles, &tract_id_names_inside, NULL, layer_clipped_inside, true, view, log);
						clipper.Paths_to_map_layer (&paths_clipped_outside, facility->id, *index_miles, &tract_id_names_outside, NULL, layer_clipped_outside, true, view, log);
					}
					else {
						// show buffers
						clipper.Paths_to_map_layer (&buffers_inside, facility->id, *index_miles, NULL, NULL, layer_clipped_inside, true, view, log);
						clipper.Paths_to_map_layer (&buffers_inside, facility->id, *index_miles, NULL, NULL, layer_clipped_outside, true, view, log);
					}

					// Recomputes the polygon areas that have already been put in layer_clipped_outside by paths_clipped_outside 
					// But console version never creates layer_clipped_outside, so this computation should use the same function
					subtract_inside_areas (&outside_areas, &paths_clipped_inside, &tract_id_names_inside, &paths_clipped_outside, &tract_id_names_outside, log);

					for (outside_area = outside_areas.begin ();
					outside_area != outside_areas.end ();
					++outside_area) {
						log.add_formatted ("%lld", facility->id);
						log.add_formatted ("\t%.2lf", *index_miles);
						log += "\t";
						log += outside_area->first;
						log.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tUp\n",
						outside_area->second [0], outside_area->second [1], outside_area->second [2], outside_area->second [3]);
						delete [] outside_area->second;
					}
					outside_areas.clear ();
				}

				layer_clipped_inside->set_extent ();
				layer_clipped_outside->set_extent ();

				if (layer_river_points_in)
					layer_river_points_in->set_extent ();

				if (layer_river_points_out)
					layer_river_points_out->set_extent ();

				clipper.write_area_table (layer_clipped_inside, "Up-In", log);
				clipper.write_area_table (layer_clipped_outside, "Up-Out", log);
			}
		}
		else
			log.add_formatted ("ERROR, closest COMID %ld is a divergence.\n", nearest_comid);
	}
	else
		log += "ERROR, no river layer.\n";
}

void frame_CARDGIS::intersect_buffer_downstream_difference
	(const map_object *facility,
	const map_layer *layer_census_tracts,
	std::vector <double> *buffer_widths,
	double start_upstream_miles,
	double length_upstream_miles,
	double cut_line_length_miles,
	const short tract_name_source,
	const int tract_numeric_attribute_id,
	const int tract_text_attribute_id,
	interface_window *view,
	dynamic_string &log)

// Compute buffer area between start & length

{
	map_layer *layer_clipped_outside, *layer_clipped_inside, *layer_river_points_in = NULL, *layer_river_points_out = NULL;
	map_object *starting_segment = NULL;
	std::map <long long, map_object *>::const_iterator facility_segment;
	flow_network_link *nearest_link;
	long long nearest_comid, next_comid_up;
	std::vector <long long> downstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Path p;
	int nearest_point_index, nearest_polygon_index;
	std::vector <double>::const_iterator index_miles;
	double radius_average_25km, offset_radius_average_1_mile, buffer_radius_average;
	double downstream_within_segment_m;
	double length_stream_km, start_stream_km;
	ClipperLib::Paths path_river_inside, path_river_outside, buffers_inside, buffers_outside, paths_clipped_inside, paths_clipped_outside;
	std::vector <dynamic_string> tract_id_names_inside, tract_id_names_outside;
	std::vector <dynamic_string>::const_iterator tract_name_outside;
	std::vector <double> tract_areas;
	std::vector < ClipperLib::Path >::const_iterator clipped_path;
	std::map <dynamic_string, double *> outside_areas;
	std::map <dynamic_string, double *>::iterator outside_area;
	bool skip;

	length_stream_km = (length_upstream_miles * METERS_PER_MILE) / 1000.0;
	start_stream_km = (start_upstream_miles * METERS_PER_MILE) / 1000.0;

	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	//2018-04-12 Clipper's tracts will use Census AREAKEY name field
	// clipper.tract_id_name_source = tract_name_source;
	// clipper.use_attribute_numeric_index = tract_numeric_attribute_id;
	// clipper.use_attribute_text_index = tract_text_attribute_id;
	clipper.tract_id_source_1 = tract_name_source;
	clipper.tract_index_source_1 = tract_numeric_attribute_id;
	clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
	clipper.tract_index_source_2 = tract_text_attribute_id;
	clipper.buffer_width_output_index = 1;

	if (layer_rivers) {

		nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
		nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
		nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
		facility_segment = router->river_layer_index.find (nearest_comid);

		log.add_formatted ("Facility\t%lld\n", facility->id);
		log.add_formatted ("Buffer length\t%.2lf\tmiles\t%.2lf\tkm\n", length_upstream_miles, length_stream_km);
		log.add_formatted ("Buffer start upstream\t%.2lf\tmiles\t%.2lf\tkm\n", start_upstream_miles, start_stream_km);
		log.add_formatted ("Nearest COMID\t%lld\n", nearest_comid);
		log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

		if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// layer for intersecting clips
			if ((layer_clipped_outside = map_watershed->match ("Clipped-Outside")) == NULL) {
				layer_clipped_outside = create_buffer_layer ("Clipped-Outside", "Facility", "Census Tract", RGB (255, 201, 14), map_watershed);
				map_watershed->layers.push_back (layer_clipped_outside);
			}
			if ((layer_clipped_inside = map_watershed->match ("Clipped-Inside")) == NULL) {
				layer_clipped_inside = create_buffer_layer ("Clipped-Inside", "Facility", "Census Tract", RGB (255, 255, 0), map_watershed);
				map_watershed->layers.push_back (layer_clipped_inside);
			}

			if ((layer_river_points_out = map_watershed->match ("River Points Outer")) == NULL) {
				layer_river_points_out = create_buffer_layer ("River Points Outer", "Facility", "Census Tract", RGB (128, 128, 192), map_watershed);
				layer_river_points_out->type = MAP_OBJECT_VECTOR;
				layer_river_points_out->enclosure = MAP_POLYGON_NOT_ENCLOSED;
				layer_river_points_out->attribute_count_numeric = 1; // upstream or downstream
				map_watershed->layers.push_back (layer_river_points_out);
			}
			if ((layer_river_points_in = map_watershed->match ("River Points Inner")) == NULL) {
				layer_river_points_in = create_buffer_layer ("River Points Inner", "Facility", "Census Tract", RGB (0, 255, 255), map_watershed);
				layer_river_points_in->type = MAP_OBJECT_VECTOR;
				layer_river_points_in->enclosure = MAP_POLYGON_NOT_ENCLOSED;
				layer_river_points_in->attribute_count_numeric = 1; // upstream or downstream
				map_watershed->layers.push_back (layer_river_points_in);
			}

			// view.update_progress ("Accumulate downstream", 0);
			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			// view.update_progress ("Accumulate upstream", 0);
			skip = false;
			if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
				nearest_down (nearest_link, &downstream_ids, &next_comid_up, router);
				starting_segment = facility_segment->second;
			}
			else {
				log.add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
				skip = true;
			}

			if (!skip) {
				{
					// List upstream ids
					std::vector <long long>::const_iterator stream;
					log += "\nDownstream\t";
					for (stream = downstream_ids.begin ();
					stream != downstream_ids.end ();
					++stream)
						log.add_formatted (" %ld", *stream);
					log += "\n";
				}

				{
					logical_coordinate center_point;
					double offset_radius_x, offset_radius_y;
					double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE; // 25-mile buffer bisection

					center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
					center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
					offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
					offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
					radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
				}

				// computed distance from nearest point in nearest segment up & down

				// discard ids beyond length_downstream_km

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper.average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, log);

				// upstream_ids are in order upstream
				// so don't use reverse_iterator
				view->update_progress ("Create Paths Inside", 0);
				p.clear ();
				clipper.add_downstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &downstream_within_segment_m, start_stream_km * 1000.0, &p, layer_river_points_in, log);
				clipper.create_paths_reverse (layer_rivers, layer_river_points_in, &downstream_ids, start_stream_km - (downstream_within_segment_m / 1000.0), &p, log);
				path_river_inside.push_back (p);

				view->update_progress ("Create Paths OutSide", 0);
				p.clear ();
				clipper.add_downstream_within_segment ((map_polygon *) starting_segment, nearest_polygon_index, nearest_point_index, &downstream_within_segment_m, length_stream_km * 1000.0, &p, layer_river_points_out, log);
				clipper.create_paths_reverse (layer_rivers, layer_river_points_out, &downstream_ids, length_stream_km - (downstream_within_segment_m / 1000.0), &p, log);
				path_river_outside.push_back (p);

				view->update_progress ("Create buffers", 0);
				for (index_miles = buffer_widths->begin ();
				index_miles != buffer_widths->end ();
				++index_miles) {
					buffer_radius_average = offset_radius_average_1_mile * *index_miles;

					view->update_progress_formatted (1, "buffer %.1lf miles", *index_miles);
				
					/*
					{
						// List upstream ids
						std::vector <long>::const_iterator stream;
						log += "\nUpstream\t";
						for (stream = upstream_ids.begin ();
						stream != upstream_ids.end ();
						++stream)
							log.add_formatted (" %ld", *stream);
						log += "\n";

						log += "Inside\n";
						clipper.dump_paths (&path_river_inside, log);
						log += "Outside\n";
						clipper.dump_paths (&path_river_outside, log);
					}
					*/
					clipper.create_buffer (&path_river_inside, &buffers_inside, buffer_radius_average, log);
					clipper.create_buffer (&path_river_outside, &buffers_outside, buffer_radius_average, log);

					if (layer_census_tracts) {
						paths_clipped_inside.clear ();
						paths_clipped_outside.clear ();
						tract_id_names_inside.clear ();
						tract_id_names_outside.clear ();
						clipper.intersect_overlapping (&buffers_inside, layer_census_tracts, &paths_clipped_inside, &tract_id_names_inside, NULL, view, log);
						clipper.intersect_overlapping (&buffers_outside, layer_census_tracts, &paths_clipped_outside, &tract_id_names_outside, NULL, view, log);

						clipper.Paths_to_map_layer (&paths_clipped_inside, facility->id, *index_miles, &tract_id_names_inside, NULL, layer_clipped_inside, true, view, log);
						clipper.Paths_to_map_layer (&paths_clipped_outside, facility->id, *index_miles, &tract_id_names_outside, NULL, layer_clipped_outside, true, view, log);
					}
					else {
						// show buffers
						clipper.Paths_to_map_layer (&buffers_inside, facility->id, *index_miles, NULL, NULL, layer_clipped_inside, true, view, log);
						clipper.Paths_to_map_layer (&buffers_inside, facility->id, *index_miles, NULL, NULL, layer_clipped_outside, true, view, log);
					}

					// Recomputes the polygon areas that have already been put in layer_clipped_outside by paths_clipped_outside 
					// But console version never creates layer_clipped_outside, so this computation should use the same function
					subtract_inside_areas (&outside_areas, &paths_clipped_inside, &tract_id_names_inside, &paths_clipped_outside, &tract_id_names_outside, log);

					for (outside_area = outside_areas.begin ();
					outside_area != outside_areas.end ();
					++outside_area) {
						log.add_formatted ("%ld", facility->id);
						log.add_formatted ("\t%.2lf", *index_miles);
						log += "\t";
						log += outside_area->first;
						log.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\tDown\n",
						outside_area->second [0], outside_area->second [1], outside_area->second [2], outside_area->second [3]);
						delete [] outside_area->second;
					}
					outside_areas.clear ();
				}

				layer_clipped_inside->set_extent ();
				layer_clipped_outside->set_extent ();

				if (layer_river_points_in)
					layer_river_points_in->set_extent ();
				if (layer_river_points_out)
					layer_river_points_out->set_extent ();

				clipper.write_area_table (layer_clipped_inside, "Down-In", log);
				clipper.write_area_table (layer_clipped_outside, "Down-Out", log);
			}
		}
		else
			log.add_formatted ("ERROR, closest COMID %ld is a divergence.\n", nearest_comid);
	}
	else
		log += "ERROR, no river layer.\n";
}

void frame_CARDGIS::intersect_buffer_downstream
	(const class map_object *facility,
	const map_layer *layer_census_tracts,
	std::vector <double> *buffer_widths,
	double start_downstream_miles,
	double length_downstream_miles,
	const double cut_line_length_miles,
	const short tract_name_source,
	const int tract_numeric_attribute_id,
	const int tract_text_attribute_id,
	class interface_window *view,
	dynamic_string &log)

// Create buffers (length_stream_miles) miles up and down from a single facility
// For each buffer zone, compute intersection with Census layer

{
	map_layer *layer_clipped_down, *layer_river_points = NULL;
	std::map <long long, map_object *>::const_iterator facility_segment;
	map_object *starting_segment = NULL;
	flow_network_link *nearest_link;
	long long nearest_comid, next_comid_down;
	std::vector <long long> downstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Path p;
	int nearest_point_index, nearest_polygon_index;
	std::vector <double>::const_iterator index_miles;
	double radius_average_25km, offset_radius_average_1_mile, buffer_radius_average;
	double downstream_within_segment_m;
	double length_stream_km, start_downstream_km;
	ClipperLib::Paths path_river, buffers_down, paths_clipped;
	std::vector <dynamic_string> tract_id_names;
	std::vector <double> tract_areas;
	bool error = false;

	length_stream_km = ((double) length_downstream_miles * METERS_PER_MILE) / 1000.0;
	start_downstream_km = (start_downstream_miles * METERS_PER_MILE) / 1000.0;

	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	//2018-04-12 Clipper's tracts will use Census AREAKEY name field
	// clipper.tract_id_name_source = tract_name_source;
	// clipper.use_attribute_numeric_index = tract_numeric_attribute_id;
	// clipper.use_attribute_text_index = tract_text_attribute_id;
	clipper.tract_id_source_1 = tract_name_source;
	clipper.tract_index_source_1 = tract_numeric_attribute_id;
	clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
	clipper.tract_index_source_2 = tract_text_attribute_id;
	clipper.buffer_width_output_index = 1;

	if (layer_rivers) {
		nearest_comid = facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID];
		nearest_polygon_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POLYGON_INDEX];
		nearest_point_index = (int) facility->attributes_numeric [USGS_ATTRIBUTES_INDEX_POINT_INDEX];
		facility_segment = router->river_layer_index.find (nearest_comid);

		log.add_formatted ("Facility\t%lld\n", facility->id);
		log.add_formatted ("Buffer start\t%.2lf\tmiles\n", start_downstream_miles);
		log.add_formatted ("Buffer length\t%.2lf\tmiles\n", length_downstream_miles);
		log.add_formatted ("Nearest COMID\t%lld\n", nearest_comid);
		log.add_formatted ("Nearest Point\t%d\t%d\n", nearest_polygon_index, nearest_point_index);

		if (facility_segment->second->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] != 2) {

			// layer for intersecting clips
			if ((layer_clipped_down = map_watershed->match ("Clipped-Down")) == NULL) {
				layer_clipped_down = create_buffer_layer ("Clipped-Down", "Facility", "Census Tract", RGB (255, 127, 39), map_watershed);
				map_watershed->layers.push_back (layer_clipped_down);
			}

			if ((layer_river_points = map_watershed->match ("River Points")) == NULL) {
				layer_river_points = create_buffer_layer ("River Points", "Facility", "Census Tract", RGB (0, 255, 255), map_watershed);
				layer_river_points->type = MAP_OBJECT_VECTOR;
				layer_river_points->enclosure = MAP_POLYGON_NOT_ENCLOSED;
				layer_river_points->attribute_count_numeric = 1; // upstream or downstream
				map_watershed->layers.push_back (layer_river_points);
			}

			// view.update_progress ("Accumulate downstream", 0);
			// router->rivers.accumulate_downstream_ordered (nearest_comid, &downstream_ids);

			// view.update_progress ("Accumulate upstream", 0);

			if (start_downstream_miles > 0.0) {
				int starting_polygon_index, starting_point_index;

				if (clipper.find_start_point_upstream (facility_segment->second, nearest_polygon_index, nearest_point_index, layer_rivers, &downstream_ids, start_downstream_km,
				&starting_segment, &starting_polygon_index, &starting_point_index, true, log)) {

					nearest_comid = starting_segment->id;
					nearest_polygon_index = starting_polygon_index;
					nearest_point_index = starting_point_index;

					// build upstream_ids starting at starting_segment
					downstream_ids.clear ();
					if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL)
						nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);
					else {
						log.add_formatted ("ERROR, can't find flow_network_link for new starting comid %ld.\n", nearest_comid);
						error = true;
					}
				}
				else
					error = true;
			}
			else {
				if ((nearest_link = router->rivers.match_comid (nearest_comid)) != NULL) {
					nearest_down (nearest_link, &downstream_ids, &next_comid_down, router);
					starting_segment = facility_segment->second;
				}
				else {
					log.add_formatted ("ERROR, can't find flow_network_link for COMID %ld.\n", nearest_comid);
					error = true;
				}
			}

			if (!error) {
				{
					// List downstream ids
					std::vector <long long>::const_iterator stream;
					log += "\nDownstream\t";
					for (stream = downstream_ids.begin ();
					stream != downstream_ids.end ();
					++stream)
						log.add_formatted (" %lld", *stream);
					log += "\n";
				}

				{
					logical_coordinate center_point;
					double offset_radius_x, offset_radius_y;
					double bisect_radius_m = cut_line_length_miles * METERS_PER_MILE; // 25-mile buffer bisection

					center_point.x = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2] / 1.0e6;
					center_point.y = (double) ((map_polygon *) facility_segment->second)->nodes [nearest_point_index * 2 + 1] / 1.0e6;
					offset_radius_x = offset_longitude_meters (center_point, bisect_radius_m);
					offset_radius_y = offset_latitude_meters (center_point, bisect_radius_m);
					radius_average_25km = (offset_radius_x + offset_radius_y) / 2.0;
				}

				// computed distance from nearest point in nearest segment up & down

				// discard ids beyond length_downstream_km

				// Compute offset_radius_average for 1 mile, then multiply it for each of the others
				offset_radius_average_1_mile = clipper.average_offset (layer_rivers, nearest_comid, METERS_PER_MILE, log);

				view->update_progress ("Create Paths Downstream", 0);

				p.clear ();
				clipper.add_downstream_within_segment (starting_segment, nearest_polygon_index, nearest_point_index, &downstream_within_segment_m, length_stream_km * 1000.0, &p, layer_river_points, log);
				log.add_formatted ("Distance down nearest segment\t%.2lf\tm\n", downstream_within_segment_m);
				clipper.create_paths_reverse (layer_rivers, layer_river_points, &downstream_ids, length_stream_km - (downstream_within_segment_m / 1000.0), &p, log);

				path_river.push_back (p);

				view->update_progress ("Create buffers", 0);
				for (index_miles = buffer_widths->begin ();
				index_miles != buffer_widths->end ();
				++index_miles) {
					buffer_radius_average = offset_radius_average_1_mile * *index_miles;

					view->update_progress_formatted (1, "buffer %.2lf miles", *index_miles);
					log.add_formatted ("Buffer width\t%.2lf\tmiles\n", *index_miles);

					// clipper.Paths_to_map_layer (&path_river, facility->id, buffer_id, NULL, layer_river_points, true, &view, log);
					clipper.create_buffer (&path_river, &buffers_down, buffer_radius_average, log);
					// clipper.Paths_to_map_layer (&path_buffers, layer_buffer, &view, log);
					if (layer_census_tracts) {
						paths_clipped.clear ();
						tract_id_names.clear ();
						clipper.intersect_overlapping (&buffers_down, layer_census_tracts, &paths_clipped, &tract_id_names, NULL, view, log);
						clipper.Paths_to_map_layer (&paths_clipped, facility->id, *index_miles, &tract_id_names, NULL, layer_clipped_down, true, view, log);
					}
					// else
					// 2018-08-27 Always create buffer map layer in case intersect didn't actually find anything
					// show buffers & bisection line
					clipper.Paths_to_map_layer (&buffers_down, facility->id, *index_miles, NULL, NULL, layer_clipped_down, true, view, log);
				}

				layer_clipped_down->set_extent ();

				if (layer_river_points)
					layer_river_points->set_extent ();

				clipper.write_area_table (layer_clipped_down, "Down", log);
			}
			else
				log.add_formatted ("ERROR, can't find flow_network_link for COMID %lld.\n", nearest_comid);

		}
		else
			log.add_formatted ("ERROR, closest COMID %lld is a divergence.\n", nearest_comid);
	}
	else
		log += "ERROR, layer rivers.\n";
}

bool frame_CARDGIS::write_upstream_downstream
	(const map_layer *point_layer,
	const bool write_to_file,
	dynamic_string &filename_output,
	const bool add_to_layer,
	map_layer *destination_layer,
	dynamic_string &log)

{
	std::vector <long long> trail;
	std::vector <long long>::reverse_iterator trail_stop;
	std::vector <long long>::iterator trail_stop_downstream;
	std::vector <map_object *>::const_iterator center;
	std::map <long long, flow_network_link *>::const_iterator mouth;
	std::map <long long, flow_network_link *>::const_iterator link;
	map_object *branch, *segment, *new_segment;
	long long level_path_id;
	FILE *file_output;
	bool error = false;

	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	if (layer_rivers) {

		if (write_to_file) {
			if (fopen_s (&file_output, filename_output.get_text_ascii (), "w") == 0)
				fprintf (file_output, "Lake\tName\tCOMID\nDirection\n");
			else {
				log += "ERROR, can't open output file \"";
				log += filename_output;
				log += "\"\n";
				error = true;
			}
		}
		else
			file_output = NULL;

		for (center = point_layer->objects.begin ();
		center != point_layer->objects.end ();
		++center) {

			if ((branch = layer_rivers->match_id ((long) (*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID])) != NULL) {
				level_path_id = (long) branch->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_LEVEL_PATH];

				// list downstream COMIDs
				trail.clear ();
				if ((mouth = router->rivers.all_mouths.find (branch->id)) != router->rivers.all_mouths.end ()) {
					mouth->second->accumulate_upstream_single_trail_ordered (branch->id, &trail);

					for (trail_stop = trail.rbegin ();
					trail_stop != trail.rend ();
					++trail_stop) {
						if (write_to_file) {
							fprintf (file_output, "%lld", (*center)->id);
							fprintf (file_output, "\t%s", (*center)->name.get_text_ascii ());
							fprintf (file_output, "\t%lld\tDown\n", *trail_stop);
						}

						if (add_to_layer) {
							if ((segment = layer_rivers->match_id (*trail_stop)) != NULL) {
								new_segment = destination_layer->create_new (segment->type);
								new_segment->copy (segment, destination_layer);
								destination_layer->objects.push_back (new_segment);
							}
							else {
								log.add_formatted ("ERROR, downstream COMID %ld not found in layer_rivers.\n", *trail_stop);
								error = true;
							}
						}
					}
				}
				else {
					log.add_formatted ("ERROR, can't find river mouth for COMID %ld, \"", branch->id);
					log += (*center)->name;
					log += "\"\n";
					error = true;
					if (write_to_file) {
						fprintf (file_output, "%lld", (*center)->id);
						fprintf (file_output, "\t%s", (*center)->name.get_text_ascii ());
						fprintf (file_output, "\t%lld\tDOWN\n", (long long) (*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID]);
					}
				}

				if ((link = router->rivers.all_comids.find ((long) (*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID])) != router->rivers.all_comids.end ()) {

					// List upstream COMIDs
					trail.clear ();
					link->second->accumulate_upstream (&trail);
					for (trail_stop_downstream = trail.begin ();
					trail_stop_downstream != trail.end ();
					++trail_stop_downstream) {
						if (write_to_file) {
							fprintf (file_output, "%lld", (*center)->id);
							fprintf (file_output, "\t%s", (*center)->name.get_text_ascii ());
							fprintf (file_output, "\t%lld\tUP\n", *trail_stop_downstream);
						}
						if (add_to_layer) {
							if (*trail_stop_downstream != branch->id) {
								if ((segment = layer_rivers->match_id (*trail_stop_downstream)) != NULL) {
									new_segment = destination_layer->create_new (segment->type);
									new_segment->copy (segment, destination_layer);
									destination_layer->objects.push_back (new_segment);
								}
								else {
									log.add_formatted ("ERROR, upstream COMID %lld not found in layer_rivers.\n", *trail_stop);
									error = true;
								}
							}
						}
					}

					if (write_to_file) {
						// List upstream COMIDs, on LevelPath
						trail.clear ();
						link->second->accumulate_upstream_LevelPath_ordered (level_path_id, &router->river_layer_index, &trail);

						for (trail_stop_downstream = trail.begin ();
						trail_stop_downstream != trail.end ();
						++trail_stop_downstream) {
							fprintf (file_output, "%lld", (*center)->id);
							fprintf (file_output, "\t%s", (*center)->name.get_text_ascii ());
							fprintf (file_output, "\t%lld\tUPLevelPath\n", *trail_stop_downstream);
						}
					}
				}
				else {
					log.add_formatted ("ERROR, can't find flow_network_link for COMID %ld, \"", (long) (*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID]);
					log += (*center)->name;
					log += "\"\n";
					error = true;

					if (write_to_file) {
						fprintf (file_output, "%lld", (*center)->id);
						fprintf (file_output, "\t%s", (*center)->name.get_text_ascii ());
						fprintf (file_output, "\t%lld\tUP\n", (long long) (*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID]);
					}
				}
			}
			else {
				log.add_formatted ("ERROR, can't find map object with ID %ld, \"", (long) (*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID]);
				log += (*center)->name;
				log += "\"\n";
				fprintf (file_output, "%lld", (*center)->id);
				fprintf (file_output, "\t%s", (*center)->name.get_text_ascii ());
				fprintf (file_output, "\t%lld\tDOWN\n", (long long) (*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID]);
				error = true;
			}
		}

		if (write_to_file)
			fclose (file_output);
	}
	else
		log += "ERROR, no river layer.\n";

	return !error;
}

void frame_CARDGIS::OnReadIowaLakes
	(wxCommandEvent&)

// 2016-04-06 Read layer of Iowa Lakes
// Find centerpoints and nearest COMID to centerpoints
// List upstream & downstream

{
	dynamic_string log;
	map_layer *layer_iowa_lakes, *layer_iowa_lake_centers;
	importer_shapefile importer;
	interface_window_wx view;
	view.start_progress (this);

	map_layer *layer_rivers = map_watershed->match ("Rivers");

	if (layer_rivers) {
		importer.filename_source = "R:\\DKEISER\\updown\\iowa\\132iowalakes\\132Lakes.shp";
		importer.id_field_name = "ID";
		importer.name_field_name = "LAKE_NAME";
		importer.projection = SHAPEFILE_PROJECTED_UTM;
		importer.take_dbf_columns = false;

		// 2020-03-24 must define importer_shapefile.dbf_column_destinations_numeric, dbf_column_destinations_text
		ASSERT (false);

		importer.longitude_central_meridian = -93;
		importer.normalize_longitude = false;

		layer_iowa_lakes = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_iowa_lakes->name = importer.filename_source;
		layer_iowa_lakes->attribute_count_numeric = 1;
		layer_iowa_lakes->attribute_count_text = 2;
		layer_iowa_lakes->initialize_attributes = true;
		layer_iowa_lakes->color = RGB (255, 127, 39);
		layer_iowa_lakes->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_iowa_lakes->column_names_text.push_back ("WBID");
		layer_iowa_lakes->column_names_text.push_back ("COUNTY");
		map_watershed->layers.push_back (layer_iowa_lakes);

		panel_watershed->pause_rendering = true;
		importer.import (layer_iowa_lakes, NULL, map_watershed, &view, log);
		panel_watershed->pause_rendering = false;

		// create a layer for center of each lake
		layer_iowa_lake_centers = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_iowa_lake_centers->name = "Iowa Lake Centers";
		layer_iowa_lake_centers->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
		layer_iowa_lake_centers->attribute_count_text = 2;
		layer_iowa_lake_centers->initialize_attributes = true;
		layer_iowa_lake_centers->color = RGB (255, 127, 39);
		layer_iowa_lake_centers->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		layer_iowa_lake_centers->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
		layer_iowa_lake_centers->symbol_size = 6;
		map_watershed->layers.push_back (layer_iowa_lake_centers);

		view.update_progress ("Finding centroids");
		{
			std::vector <map_object *>::const_iterator lake;
			map_object *center;

			for (lake = layer_iowa_lakes->objects.begin ();
			lake != layer_iowa_lakes->objects.end ();
			++lake) {
				view.update_progress_formatted (1, "Lake %ld", (*lake)->id);
				center = layer_iowa_lake_centers->create_new (MAP_OBJECT_POINT);
				center->id = (*lake)->id;
				center->name = (*lake)->name;
				center->attributes_text [0] = (*lake)->attributes_text [0];
				center->attributes_text [1] = (*lake)->attributes_text [1];
				(*lake)->centroid (&center->latitude, &center->longitude);
				layer_iowa_lake_centers->objects.push_back (center);
			}
		}

		view.update_progress ("Matching centroids to COMIDs");
		// find nearest comid to each centerpoint
		{
			std::vector <map_object *>::const_iterator center;
			device_coordinate centerpoint;
			map_object *segment;
			long distance;

			for (center = layer_iowa_lake_centers->objects.begin ();
			center != layer_iowa_lake_centers->objects.end ();
			++center) {
				view.update_progress_formatted (1, "Centroid %lld", (*center)->id);
				centerpoint.x = (*center)->longitude;
				centerpoint.y = (*center)->latitude;
				if ((segment = layer_rivers->find_nearest_object_fast (centerpoint, &distance)) != NULL)
					(*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID] = (double) segment->id;

				// Distance is NOT meters
				(*center)->attributes_numeric [USGS_ATTRIBUTES_INDEX_DISTANCE_M] = (double) distance;
			}
		}

		layer_iowa_lakes->set_extent ();
		map_watershed->set_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
	}
	else
		log += "ERROR, no river layer.\n";

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Iowa Lakes", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnReadE85Stations
	(wxCommandEvent&)

// 2016-08-30 Database of E85 stations & prices
// Fill map layer with points defined in [Stations]
// Set attributes_void of points to timed_measurements from table [Prices]

{
	dynamic_string log;
	map_layer *layer_e85, *layer_nonreporting;

	interface_window_wx view;
	view.start_progress (this);

	layer_e85 = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_e85->name = "E85 Prices";
	layer_e85->attribute_count_numeric = 1; // Price as of a specific date will be set here
	layer_e85->attribute_count_text = 0;
	layer_e85->initialize_attributes = true;
	layer_e85->color = RGB (255, 128, 64);
	layer_e85->draw_as = MAP_OBJECT_DRAW_SYMBOL_FILL_AND_OUTLINE;
	layer_e85->symbol = MAP_OBJECT_SYMBOL_CIRCLE;

	layer_nonreporting = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_nonreporting->name = "Stations Not Reporting";
	layer_nonreporting->attribute_count_numeric = 1; // Price as of a specific date will be set here
	layer_nonreporting->attribute_count_text = 0;
	layer_nonreporting->initialize_attributes = true;
	layer_nonreporting->color = RGB (192, 192, 192);
	layer_nonreporting->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_nonreporting->symbol = MAP_OBJECT_SYMBOL_CIRCLE;

	// Ethanol values from database
	double min = 1.079;
	double max = 4.165666667;
	double mean = 2.208469073;
	double std = 0.562583115;

	set_e85_colors (&layer_e85->colors, min, max, mean, std);

	map_watershed->layers.push_back (layer_nonreporting);
	map_watershed->layers.push_back (layer_e85);

	view.update_progress ("E85");
	{
		odbc_database_access *db = new odbc_database_access;
		odbc_database_credentials creds;
		map_object *station;

		E85_Stations set_stations;

		creds.set_database_filename ("I:\\TDC\\Ethanol\\Prices\\E85 Station Prices.accdb");
		creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);

		if (db->open (&creds, &view, log)) {

			set_stations.initialize_field_set ();
			if (set_stations.field_set.open_read (db, log)) {

				if (set_stations.field_set.move_first (log)) {
					do {
						station = layer_e85->create_new (MAP_OBJECT_POINT);
						station->id = set_stations.ID;
						station->name = set_stations.address;
						station->latitude = (long) (set_stations.lat * 1.0e6);
						station->longitude = (long) (set_stations.lon * 1.0e6);
						// station->longitude_normalize ();
						layer_e85->objects.push_back (station);
					} while (set_stations.field_set.move_next (log));
				}

				set_stations.field_set.close ();
			}

			std::chrono::system_clock::time_point when;
			timed_measurements *sales;
			long record_count = 0;
			E85_Prices rowset_e85_prices;

			rowset_e85_prices.initialize_field_set ();
			if (rowset_e85_prices.field_set.open_read (db, log)) {
				if (rowset_e85_prices.field_set.move_first (log)) {
					do {
						rowset_e85_prices.date.set_time_point (&when);

						if ((station = layer_e85->match_id (rowset_e85_prices.ID)) != NULL) {

							if ((sales = (timed_measurements *) station->attributes_void) != NULL)
								sales->amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_e85_prices.pe85_ret));
							else {
								sales = new timed_measurements;
								station->attributes_void = sales;
								sales->id = rowset_e85_prices.ID;
								sales->amounts_ppm.insert (std::pair <std::chrono::system_clock::time_point, double> (when, rowset_e85_prices.pe85_ret));
							}
						}

						if ((++record_count % 1000) == 0) 
							view.update_progress_formatted  (1, "WaterSamples %ld", record_count);
					} while (rowset_e85_prices.field_set.move_next (log));
				}
				rowset_e85_prices.field_set.close ();
			}

			// move nonreporting stations to their own layer
			std::vector <map_object *>::iterator s, next_s;
			s = layer_e85->objects.begin ();
			while (s != layer_e85->objects.end ()) {
				next_s = s;
				++next_s;

				if ((*s)->attributes_void == NULL) {
					layer_nonreporting->objects.push_back (*s);
					layer_e85->objects.erase (s);
				}

				s = next_s;
			}

			db->close ();
		}
	}

	layer_e85->set_extent ();
	layer_nonreporting->set_extent ();

	panel_watershed->change_layers ();
	enable_map_controls ();

	view.destroy_dialog_progress ();

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"E85", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnAddSelectionToLayer
	(wxCommandEvent&)

{
	dynamic_string log;
	map_layer *destination_layer;
	std::vector <map_object *>::const_iterator object;
	map_object *copy;

	if (panel_watershed->layer_selection_source
	&& panel_watershed->layer_selection) {
		// dialog to select target layer
		dialog_select_layer dsl (this, L"Select Point Layer", map_watershed, &destination_layer);

		if (dsl.ShowModal () == wxID_OK) {
			if (destination_layer != panel_watershed->layer_selection_source) {
				if (destination_layer->type == panel_watershed->layer_selection_source->type) {
					if ((destination_layer->attribute_count_numeric == panel_watershed->layer_selection_source->attribute_count_numeric)
					&& (destination_layer->attribute_count_text == panel_watershed->layer_selection_source->attribute_count_text)) {

						for (object = panel_watershed->layer_selection->objects.begin ();
						object != panel_watershed->layer_selection->objects.end ();
						++object) {
							copy = destination_layer->create_new ((*object)->type);
							copy->copy (*object, destination_layer);
							destination_layer->objects.push_back (copy);
						}

						destination_layer->set_extent ();
						map_watershed->set_extent ();
						panel_watershed->change_layers ();
						enable_map_controls ();
					}
					else
						log += "ERROR, destination layer attribute counts must be the same type as source layer.\n";
				}
				else
					log += "ERROR, destination layer must be the same type as source layer.\n";

			}
			else
				log += "ERROR, destination layer must be different than source layer.\n";
		}
	}
	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Iowa Lakes", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnAddNewLayer
	(wxCommandEvent&)

{
	map_layer *new_layer;

	map_layer *layer_rivers = map_watershed->match ("Rivers");

	if (layer_rivers) {
		new_layer = map_watershed->create_new (layer_rivers->type);
		new_layer->name = "Lake UpDown";
		new_layer->attribute_count_numeric = layer_rivers->attribute_count_numeric;
		new_layer->attribute_count_text = layer_rivers->attribute_count_text;
		new_layer->draw_as = layer_rivers->draw_as;
		new_layer->color = layer_rivers->color;
		new_layer->column_names_numeric = layer_rivers->column_names_numeric;
		new_layer->column_names_text = layer_rivers->column_names_text;
	}
	else {
		new_layer = map_watershed->create_new (MAP_OBJECT_VECTOR);
		new_layer->name = "New layer";
	}

	dialog_edit_layer del (this, map_watershed, new_layer);

	if (del.ShowModal () == wxID_OK) {
		SetCursor (*wxHOURGLASS_CURSOR);
		map_watershed->layers.push_back (new_layer);
		panel_watershed->change_layers ();
		enable_map_controls ();
		SetCursor (wxNullCursor);
	}

}

void frame_CARDGIS::OnEditLayerProperties
	(wxCommandEvent&)

{
	if (panel_watershed->layer_selection_source) {
		dialog_edit_layer del (this, map_watershed, panel_watershed->layer_selection_source);

		if (del.ShowModal () == wxID_OK) {
			enable_map_controls ();
			panel_watershed->Refresh ();
		}
	}

}

void frame_CARDGIS::OnPointsUpstreamDownstream (wxCommandEvent &)

// Find all COMIDS in river that are upstream/downsteram from points in current layer
// Add them to 3rd layer or write to file or both

{
	dynamic_string log, filename_output;
	bool write_to_file = false, add_to_layer = true;
	map_layer *destination_layer = map_watershed->match ("Lake UpDown");

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)) {

		dialog_upstream_downstream dud (this, map_watershed, &write_to_file, &filename_output, &add_to_layer, &destination_layer);

		if (dud.ShowModal () == wxID_OK) {
			SetCursor (*wxHOURGLASS_CURSOR);

			write_upstream_downstream (panel_watershed->layer_selection_source, write_to_file, filename_output, add_to_layer, destination_layer, log);

			if (add_to_layer) {
				destination_layer->set_extent ();
				map_watershed->set_extent ();
				panel_watershed->change_layers ();
				enable_map_controls ();
			}
			SetCursor (wxNullCursor);
		}
	}
	else
		log += "ERROR, one point layer must be selected.\n";

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Points Upstream Downstream", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnReadGridFloat
	(wxCommandEvent&)

// 2016-04-26 3DEM files from http://viewer.nationalmap.gov

{
	dynamic_string log, filename_flt, filename_dae, filename_image;
	bool error = false;

	// filename_flt = "H:\\scratch\\n40w105\\usgs_ned_1_n40w105_gridfloat.flt";
	filename_flt = "H:\\scratch\\n42w094\\floatn42w094_1.flt";
	// filename_image = "H:\\scratch\\n40w105\\USGS_NED_1_n40w105_GridFloat_thumb.jpg";
	filename_image = "H:\\scratch\\n42w94\\floatn42w094_1_thumb.jpg";
	filename_dae = "H:\\scratch\\n42w094\\GridFloat.dae";

	if (get_read_path (&filename_flt, L"GridFloat Files (*.flt)|*.flt", NULL)) {

		interface_window_wx view;
		view.start_progress (this);

		bounding_cube boundary;
		elevation_matrix elevations;

		// Boundaries of Ames
		boundary.x [0] = -93.7;
		// boundary.x [1] = -93.571;
		boundary.x [1] = -93.625;
		boundary.y [0] = 41.986;
		// boundary.y [1] = 42.063;
		boundary.y [1] = 41.999;

		river_view_collada river_viewer;
		river_viewer.read_gridfloat_heatmap (filename_flt, filename_image, "", boundary, &elevations, false, &view, log);

		river_viewer.write_collada_xml (filename_dae, &view, log);

		map_watershed->set_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
	}
	else
		error = true;

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"GridFloat", L"", log);
		d.ShowWindowModal ();
	}
}


void frame_CARDGIS::OnAssignLakeErieGrids
	(wxCommandEvent &)

// 2016-11-03 Assign a grid ID to secchi readings

{
	dynamic_string log;
	LakeErie erie;
	interface_window_wx view;
	view.start_progress (this);

	erie.assign_grids (map_watershed, &view, log);
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnAssignLakeErieGrids");

	view.destroy_dialog_progress ();

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Erie", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnLakeErieMarinaDistance
	(wxCommandEvent &)

// 2016-11-09 Read Marina layer and list of Marina->Grid pairs
// Compute distance from each marina to center of each grid

{
	dynamic_string log, filename_marina_db;
	LakeErie erie;
	interface_window_wx view;
	view.start_progress (this);
	bool all_combinations;

	erie.filename_marina_db = "I:\\TDC\\LakeErie\\Angler Survey\\Lake Erie Anglers.accdb";

	// If all_combinations is false, pairs of marina->grid are read from database table [Marina to Grid]
	// and trip/distance only computed for these 
	all_combinations = false;

	erie.marina_distance (map_watershed, all_combinations, &view, log);

	map_watershed->set_extent ();

	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnLakeErieMarinaDistance");

	view.destroy_dialog_progress ();

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Erie", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnLakeErieReport
	(wxCommandEvent &)

// 2016-11-09 Read Marina layer and list of Marina->Grid pairs
// Compute distance from each marina to center of each grid

{
	dynamic_string log;
	LakeErie erie;

	interface_window_wx view;
	view.start_progress (this);

	erie.filename_marina_db = "I:\\TDC\\LakeErie\\Angler Survey\\Lake Erie Anglers.accdb";

	erie.report_angler_trips_by_gridcode (&view, log);

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Erie", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnLakeErieReport2
	(wxCommandEvent &)

// 2016-12-14 Find closest relevent marina for each grid
// Compute total driving & boating distance from each zipcode to grid through that marina

// Produces 4 reports that are saved to "Travel Distances.xlsx"

{
	dynamic_string log;
	LakeErie erie;

	interface_window_wx view;
	view.start_progress (this);

	erie.filename_marina_db = "I:\\TDC\\LakeErie\\Angler Survey\\Lake Erie Anglers.accdb";

	erie.report_relevent_travel (map_watershed, &view, log);

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Erie", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnReadLakeErie
	(wxCommandEvent&)

// 2016-09-07 Shapefile from Wedong of Lake Erie fishing grid centerpoints

// Create the grid around them.  Show on map with contour & margins

// Grid is more-or-less columns 1 to 28 and rows 1(00)..10(00)

// See illustration in "I:\TDC\LakeErie\Lake_Erie_Fishing_Grid_ODNR.pdf"

{
	dynamic_string log;
	LakeErie erie;

	interface_window_wx view;
	view.start_progress (this);

	erie.read (map_watershed, &view, log);

	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnLakeErie");

	view.destroy_dialog_progress ();

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Erie", L"", log);
		d.ShowWindowModal ();
	}
}

bool read_layer_data_csv
	(dynamic_string &filename,
	map_layer *layer,
	long *count_changed,
	long *count_missing,
	dynamic_string &log)

{
	bool error = false;
	std::vector <dynamic_string> columns;
	std::vector <dynamic_string>::iterator column;
	int column_index, column_offset_index = 0;
	long id;
	dynamic_string data_line;
	map_object *polygon;
	FILE *file_csv;

	if (fopen_s (&file_csv, filename.get_text_ascii (), "r") == 0) {
		char buffer[1024];
		if (fgets (buffer, 1024, file_csv)) {
			// read column names
			column_index = 0;
			data_line = buffer;
			data_line.tokenize (",\t\n", &columns, true, false);

			column_offset_index = layer->attribute_count_numeric;
			layer->resize (layer->attribute_count_numeric + (columns.size () - 1), layer->attribute_count_text);

			column = columns.begin (); // first column name is presumably "ID"
			++column;
			while (column != columns.end ()) {
				layer->column_names_numeric.push_back (*column);
				++column;
			}

			while (fgets (buffer, 1024, file_csv)) {
				data_line = buffer;
				data_line.tokenize (",\t\n", &columns, true, false);
				if (columns.size () > 0) {
					id = atol (columns[0].get_text_ascii ());
					if ((polygon = layer->match_id (id)) != NULL) {
						*count_changed += 1;
						for (column_index = 1; column_index < columns.size (); ++column_index) {
							polygon->attributes_numeric[(column_index - 1) + column_offset_index] = atof (columns[column_index].get_text_ascii ());

							/*
							if (polygon->attributes_numeric [column_index - 1] == 0.0)
								// Mark 0 values as -1 to distinguish from Not-Run
								polygon->attributes_numeric [column_index - 1] = -1.0;
							*/
						}
					}
					else
						*count_missing += 1;
				}
			}
		}
		fclose (file_csv);
	}
	else {
		error = true;
		log += "ERROR, unable to open csv file \"";
		log += filename;
		log += "\".\n";
	}
	return !error;
}

bool read_layer_data_odbc
	(const dynamic_string &filename,
	const dynamic_string &table_name,
	const dynamic_string &filter_id_text,
	const char *id_field_name,
	const char *data_field_name,
	map_layer *layer,
	long *count_changed,
	long *count_missing,
	dynamic_string &log)

// see swat_animator.read_data_odbc

{
	bool error = false;
	std::vector <dynamic_string> columns;
	std::vector <dynamic_string>::iterator column;
	long source_subbasin, impacted_subbasin;
	char source_huc[13], impacted_huc[13];
	double Delta, data, flow [2], p [2];
	int column_index_offset;
	map_object *polygon;
	dynamic_string filter;

	odbc_field_set results;

	odbc_database_access *db_layer;
	db_layer = new odbc_database_access;
	odbc_database_credentials creds;
	creds.set_database_filename (filename);
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");

	if (db_layer->open (&creds, NULL, log)) {
		results.table_name = table_name;

		filter += db_layer->identifier_quote_character_open ();
		filter += "Source Subbasin";
		filter += db_layer->identifier_quote_character_close ();
		filter += " = ";
		filter += filter_id_text;
		filter += " AND ";
		filter += db_layer->identifier_quote_character_open ();
		filter += "Phosphorus Change - tons per year";
		filter += db_layer->identifier_quote_character_close ();
		filter += " = ";
		filter += "50.0";

		// 2018-09-25 subbasin_effect ramp run
		results.add_field (1, &source_subbasin, "Source Subbasin");
		results.add_field (2, &impacted_subbasin, id_field_name);
		results.add_field (3, &Delta, "Phosphorus Change - tons per year");
		results.add_field (4, source_huc, 13, "Source HUC12");
		results.add_field (5, impacted_huc, 13, "Impacted HUC12");
		results.add_field (6, &flow [0], "Baseline Flow Out - cms");
		results.add_field (7, &flow [1], "Experiment Flow Out - cms");
		results.add_field (8, &p [0], "Baseline Phosphorus - tons per year");
		results.add_field (9, &p [1], "Experiment Phosphorus - tons per year");

		// add 1 field to map layer numeric attributes
		column_index_offset = layer->attribute_count_numeric;
		layer->resize (layer->attribute_count_numeric + 1, layer->attribute_count_text);
		layer->column_names_numeric.push_back (data_field_name);

		if (results.open_read (db_layer, log, filter)) {
			if (results.move_first (log)) {
				do {
					if ((polygon = layer->match_id (impacted_subbasin)) != NULL) {
						data = p[1] - p[0];
						polygon->attributes_numeric[column_index_offset] = data;
						*count_changed += 1;
						log.add_formatted ("%ld\t%.2lf\n", polygon->id, data);
					}
					else
						++count_missing;
				} while (results.move_next (log));
			}
			results.close ();
		}
		else
			error = true;

		db_layer->close ();
		delete db_layer;
	}
	else {
		error = true;
		log += "ERROR, unable to open database file \"";
		log += filename;
		log += "\".\n";
	}
	return !error;
}

void frame_CARDGIS::OnExportPNG
	(wxCommandEvent&)

// Draw current map, with legend, into a PNG

{
	dynamic_string filename_png, log;
	wxImage::AddHandler(new wxPNGHandler);

	wxFileDialog* openFileDialog = new wxFileDialog( this, _("Open file"), "", "", "Image Files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	openFileDialog->SetPath ("e:\\swat runs\\UMRB\\");
	openFileDialog->SetFilename ("CARDGIS.png");

	openFileDialog->SetFilename (map_watershed->title.get_text ());
 
	if (openFileDialog->ShowModal () == wxID_OK) {	
		filename_png += openFileDialog->GetDirectory().ToAscii ();
		filename_png += PATH_SEPARATOR;
		filename_png += openFileDialog->GetFilename().ToAscii ();
		save_image_map (filename_png, map_watershed, true, log);
	}
	delete openFileDialog;

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Export PNG", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnReadLayerData
	(wxCommandEvent&)

// 2016-09-16 Read a CSV file for 12-digit HUC polygon shapefile.
// Data to attach to "R:\NSF project\GIS_Data\12digits_UMRB.shp"

// First line is column names
// First column is ID

{
	map_layer *layer;
	if ((layer = panel_watershed->layer_selection_source) != NULL) {
		dynamic_string log, filename, table_name, filter_id_text;
		std::vector <dynamic_string> column_names;
		long count_changed = 0, count_missing = 0;
		BYTE format; // 1 = CSV, 2 = Counties Upstream, 3=odbc

		/*
		// filename = "R:\\SWAT\\UMRB\\Database\\NIFA 2016-07-06\\Results UMRB.csv";
		filename = "h:\\scratch\\CountyRoutes\\Strahler All 4\\Counties Upstream Strahler All 4.accdb";
		format = 2;
		*/

		/*
		// filename = "R:\\swat\\UMRB\\Database\\2018-09-11\\SubbasinEffect.accdb";
		filename = "E:\\swat runs\\UMRB\\2018-09-11\\SubbasinEffect.accdb";
		table_name = "Phosphorus PS Ramp";
		filter_id_text = "1010";
		format = 3;
		*/
		filename = "E:\\swat runs\\UMRB\\2018-09-11\\HUC8 Changes.csv";
		format = 1;

		dialog_read_layer_data drd (this, &filename, &table_name, &format, &filter_id_text, &column_names);

		if (drd.ShowModal () == wxID_OK) {

			map_watershed->title = filter_id_text;
			map_watershed->title += " P PS Monthly";

			switch (format) {
				case 1:
					read_layer_data_csv (filename, layer, &count_changed, &count_missing, log);
					break;
				case 2:
					read_upstream_map_layer_data (filename, layer, &count_changed, &count_missing, log);
					break;
				case 3:
					// read_layer_data_odbc (filename, table_name, "Impacted Subbasin", "Phosphorus Change - tons per year", filter_id_text, layer, &count_changed, &count_missing, log);
					read_layer_data_odbc (filename, table_name, filter_id_text, "Impacted Subbasin", "Phosphorus Change - tons per year", layer, &count_changed, &count_missing, log);
			}


			log.add_formatted ("Objects changed\t%ld\n", count_changed);
			log.add_formatted ("Data not matched\t%ld\n", count_missing);

			if (log.get_length () > 0) {
				log.convert_linefeeds_for_CEdit ();
				dialog_error_list d (this, L"Erie", L"", log);
				d.ShowWindowModal ();
			}
		}
	}

}

void frame_CARDGIS::OnCOMIDNetwork
	(wxMenuEvent &)

// 2016-10-14 reports for Yongjie of UMRB network & streamlevel

{
	dynamic_string log;
	std::map <long long, flow_network_link *>::const_iterator outlet;
	std::vector <std::pair <long long, long long>> pairs;
	std::vector <std::pair <long long, long long>>::iterator pair;
	std::map <long long, double> streamlevels;
	std::map <long long, double>::iterator level;
	map_object *segment;
	long count = 0;

	router_NHD* router = relevent_router();
	map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	if (layer_rivers) {
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		// root fragment 5093446 is UMRB outlet
		outlet = router->rivers.root_fragments.find (5093446);
		outlet->second->accumulate_upstream_pairs (&pairs);

		log += "COMID\tDestination\n";
		for (pair = pairs.begin ();
		pair != pairs.end ();
		++pair) {
			if ((segment = layer_rivers->match_id (pair->first)) != NULL) {
				if (segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_DIVERGENCE] == 0) {
					log.add_formatted ("%lld\t%lld\n", pair->first, pair->second);

					if ((level = streamlevels.find (pair->first)) == streamlevels.end ())
						streamlevels.insert (std::pair <long, double> (pair->first, segment->attributes_numeric [NHD_SEGMENT_ATTRIBUTE_INDEX_STREAM_LEVEL]));
				}
				// else
				//	log.add_formatted ("%ld\tDIVERGENCE\n", pair->first);
			}
			else
				log.add_formatted ("ERROR, can't find COMID %lld\n", pair->first);
			if (++count % 1000 == 0)
				view.update_progress_formatted (1, "%ld pairs", count);
		}
	}
	else
		log += "ERROR, no rivers layer.\n";

	log += "\n";
	log += "COMID\tStreamLevel\n";
	for (level = streamlevels.begin ();
	level != streamlevels.end ();
	++level)
		log.add_formatted ("\t%lld\t%.1lf\n", level->first, level->second);

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"River", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnExportLayerData
	(wxCommandEvent&)

// 2016-11-03 Dump all attributes as a table
// Currently no lat & lon included
// 2018-01-25 Added lat & long

{
	dynamic_string output_line, filename_out, log;
	map_layer *layer;
	std::vector <map_object *>::const_iterator point;
	std::vector <dynamic_string>::const_iterator column_name;
	int column_index, node_index, line_index;
	char delimiter = ',';
	FILE *output_file;
	bool show_names = false, write_attributes = false;
	int map_object_type;

	if ((layer = panel_watershed->layer_selection_source) != NULL) {
		filename_out = layer->name;
		filename_out += ".csv";
		if (get_write_path (&filename_out, L"CSV Files|*.csv|All Files|*.*", L"csv", this)) {
			SetCursor(*wxHOURGLASS_CURSOR);
			if (fopen_s (&output_file, filename_out.get_text_ascii (), "w") == 0) {

				if (layer->type == MAP_OBJECT_POLYGON) {
					// NHD river layer is MAP_OBJECT_POLYGON, but does include some complex polygons
					map_object_type = MAP_OBJECT_POLYGON;
					for (point = layer->objects.begin();
					point != layer->objects.end();
					++point)
						if ((*point)->type == MAP_OBJECT_POLYGON_COMPLEX)
							map_object_type = MAP_OBJECT_POLYGON_COMPLEX;
				}
				else
					map_object_type = layer->type;

				// First pass to see if any nonzero-length names exist
				for (point = layer->objects.begin ();
				(show_names == false)
				&& (point != layer->objects.end ());
				++point)
					if ((*point)->name.get_length () > 0)
						show_names = true;

				// Column names
				output_line = "\"ID\"";

				if (map_object_type == MAP_OBJECT_POLYGON) {
					output_line += delimiter;
					output_line += "\"Node\"";
				}
				else
					if (map_object_type == MAP_OBJECT_POLYGON_COMPLEX) {
						output_line += delimiter;
						output_line += "\"Line\"";
						output_line += delimiter;
						output_line += "\"Node\"";
					}


				output_line += delimiter;
				output_line += "\"Latitude\"";
				output_line += delimiter;
				output_line += "\"Longitude\"";

				if (show_names) {
					output_line += delimiter;
					output_line += "\"Name\"";
				}

				if (write_attributes) {
					for (column_name = layer->column_names_numeric.begin ();
					column_name != layer->column_names_numeric.end ();
					++column_name) {

						output_line += delimiter;
						output_line += "\"";
						output_line += *column_name;
						output_line += "\"";
					}

					for (column_name = layer->column_names_text.begin ();
					column_name != layer->column_names_text.end ();
					++column_name) {

						output_line += delimiter;
						output_line += "\"";
						output_line += *column_name;
						output_line += "\"";
					}
				}

				fprintf (output_file, "%s\n", output_line.get_text_ascii ());

				for (point = layer->objects.begin ();
				point != layer->objects.end ();
				++point) {

					switch ((*point)->type) {
						case MAP_OBJECT_POINT:
							output_line.clear();
							output_line.add_formatted("%ld", (*point)->id);
							output_line += delimiter;
							output_line.add_formatted ("%lg", (double) (*point)->latitude / 1.0e6);
							output_line += delimiter;
							output_line.add_formatted ("%lg", (double) (*point)->longitude / 1.0e6);
							if (show_names) {
								output_line += delimiter;
								if ((*point)->name.get_length() > 0)
									output_line += (*point)->name;
							}

							if (write_attributes) {
								for (column_index = 0; column_index < layer->attribute_count_numeric; ++column_index) {
									output_line += delimiter;
									output_line.add_formatted("%.3lf", (*point)->attributes_numeric[column_index]);
								}
								for (column_index = 0; column_index < layer->attribute_count_text; ++column_index) {
									output_line += delimiter;
									output_line += "\"";
									output_line += (*point)->attributes_text[column_index];
									output_line += "\"";
								}
							}
							fprintf(output_file, "%s\n", output_line.get_text_ascii());
							break;
						case MAP_OBJECT_POLYGON:
							for (node_index = 0; node_index < ((map_polygon *)(*point))->node_count; ++node_index) {
								output_line.clear();
								output_line.add_formatted("%ld", (*point)->id);
								output_line.add_formatted("%c%d", delimiter, node_index + 1);
								output_line += delimiter;
								output_line.add_formatted("%lg", (double)((map_polygon*)(*point))->nodes [node_index * 2 + 1] / 1.0e6);
								output_line += delimiter;
								output_line.add_formatted("%lg", (double)((map_polygon*)(*point))->nodes [node_index * 2] / 1.0e6);

								if (show_names) {
									output_line += delimiter;
									if ((*point)->name.get_length() > 0)
										output_line += (*point)->name;
								}

								if (write_attributes) {
									for (column_index = 0; column_index < layer->attribute_count_numeric; ++column_index) {
										output_line += delimiter;
										output_line.add_formatted("%.3lf", (*point)->attributes_numeric[column_index]);
									}
									for (column_index = 0; column_index < layer->attribute_count_text; ++column_index) {
										output_line += delimiter;
										output_line += "\"";
										output_line += (*point)->attributes_text[column_index];
										output_line += "\"";
									}
								}
								fprintf(output_file, "%s\n", output_line.get_text_ascii());
							}
							break;

						case MAP_OBJECT_POLYGON_COMPLEX:
							for (line_index = 0; line_index < ((map_polygon_complex *) (*point))->polygon_count; ++line_index) {
								for (node_index = 0; node_index < ((map_polygon_complex*)(*point))->polygons [line_index].node_count; ++node_index) {
									output_line.clear();
									output_line.add_formatted("%ld", (*point)->id);
									output_line.add_formatted("%c%d", delimiter, line_index + 1);
									output_line.add_formatted("%c%d", delimiter, node_index + 1);
									output_line += delimiter;
									output_line.add_formatted("%lg", (double)((map_polygon_complex*)(*point))->polygons[line_index].nodes[node_index * 2 + 1] / 1.0e6);
									output_line += delimiter;
									output_line.add_formatted("%lg", (double)((map_polygon_complex*)(*point))->polygons[line_index].nodes[node_index * 2] / 1.0e6);
									if (show_names) {
										output_line += delimiter;
										if ((*point)->name.get_length() > 0)
											output_line += (*point)->name;
									}

									if (write_attributes) {
										for (column_index = 0; column_index < layer->attribute_count_numeric; ++column_index) {
											output_line += delimiter;
											output_line.add_formatted("%.3lf", (*point)->attributes_numeric[column_index]);
										}
										for (column_index = 0; column_index < layer->attribute_count_text; ++column_index) {
											output_line += delimiter;
											output_line += "\"";
											output_line += (*point)->attributes_text[column_index];
											output_line += "\"";
										}
									}
									fprintf(output_file, "%s\n", output_line.get_text_ascii());
								}
							}

					}
				}
				fclose (output_file);
			}
			else {
				log += "ERROR, can't open output file \"";
				log += filename_out;
				log += "\".\n";
			}
			SetCursor(wxNullCursor);
		}
		else
			log += "ERROR, no selected layer.\n";
	}
	else
		log += "ERROR, no selected layer.\n";

	if (log.get_length() > 0) {
		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"Export Layer", L"", log);
		d.ShowWindowModal();
	}
}

void frame_CARDGIS::OnReadZipCodes
	(wxCommandEvent &)

// 2016-11-10 Wendong's Lake Erie Angler survey
// Read zipcodes, find centroids

// 2017-07-07 more detailed than Census zipcodes?

{
	dynamic_string log, area_name, path_zipcode_shapefile;


	path_zipcode_shapefile = "H:\\scratch\\LakeErieAnglers\\tl_2016_us_zcta510\\tl_2016_us_zcta510.shp";

	if (get_read_path (&path_zipcode_shapefile, L"Zipcode shapefile (*.shp)|*.shp", NULL)) {

		interface_window_wx update_display;
		update_display.start_progress (this);
		update_display.update_progress ("NHDView");

		panel_watershed->pause_rendering = true;

		// TIGER Streets
		map_layer *layer_zipcodes, *layer_zip_centers;

		if ((layer_zipcodes = map_watershed->match ("Zipcodes")) == NULL) {
			layer_zipcodes = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_zipcodes->name = "Zipcodes";
			layer_zipcodes->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
			layer_zipcodes->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_zipcodes->color = RGB (128, 128, 128);
			layer_zipcodes->initialize_attributes = true;
			layer_zipcodes->attribute_count_text = 2;
			layer_zipcodes->column_names_text.push_back ("INTPTLAT10");
			layer_zipcodes->column_names_text.push_back ("INTPTLON10");
			map_watershed->layers.push_back (layer_zipcodes);
		}

		read_zipcode_shapefile (path_zipcode_shapefile, NULL, layer_zipcodes, &update_display, log);

		{
			// map_object *center;
			std::vector <map_object *>::iterator zip;
			// double coordinate;
			long center_lat, center_lon;

			// Add a 2nd layer of centerpoints using INTPTLAT10,INTPTLON10
			// to check against computed centerpoint values
			layer_zip_centers = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_zip_centers->name = "Zipcode Centers";
			layer_zip_centers->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			layer_zip_centers->symbol = MAP_OBJECT_SYMBOL_PLUS;
			layer_zip_centers->color = RGB (255, 0, 0);
			layer_zip_centers->initialize_attributes = true;
			map_watershed->layers.push_back (layer_zip_centers);

			for (zip = layer_zipcodes->objects.begin ();
			zip != layer_zipcodes->objects.end ();
			++zip) {
				/*
				center = layer_zip_centers->create_new (MAP_OBJECT_POINT);
				center->id = (*zip)->id;
				coordinate = atof ((*zip)->attributes_text [0].get_text_ascii ());
				center->latitude = (long) (coordinate * 1.0e6);
				coordinate = atof ((*zip)->attributes_text [1].get_text_ascii ());
				center->longitude = (long) (coordinate * 1.0e6);
				layer_zip_centers->objects.push_back (center);
				*/
				(*zip)->centroid (&center_lat, &center_lon);
				log += (*zip)->name;
				log.add_formatted ("\t%.8lf\t%.8lf", (double) center_lat / 1.0e6, (double) center_lon / 1.0e6);
				log += "\t";
				log += (*zip)->attributes_text [0];
				log += "\t";
				log += (*zip)->attributes_text [1];
				log += "\n";
			}


		}

		map_watershed->set_extent ();

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"File Open", L"", log);
			d.ShowWindowModal ();
		}
		panel_watershed->pause_rendering = false;
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenZipCodes");
	}

}

bool frame_CARDGIS::read_zipcode_shapefile
	(const dynamic_string &path_shapefile,
	class shapefile_filter *filter,
	map_layer *layer_zipcodes,
	interface_window *update_display,
	dynamic_string &log)

{
	bool error = false;
	importer_shapefile importer;
	update_display->update_progress ("Reading Shapefile");

	importer.filename_source = path_shapefile;
	// Both ID fields are text!
	// ZCTA5CE10,C,5
	// GEOID10,C,5
	// importer.id_field_name = "GEOID10";
	importer.name_field_name = "ZCTA5CE10";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = false;
	// importer.data_field_names_text = layer_zipcodes->column_names_text;

	// 2020-03-24 must define importer_shapefile.dbf_column_destinations_numeric, dbf_column_destinations_text
	ASSERT (false);

	if (filter)
		importer.filters.push_back (filter);

	if (!importer.import (layer_zipcodes, NULL, map_watershed, update_display, log))
		error = true;

	map_watershed->set_extent ();

	log += "Zipcode layer source : ";
	log += importer.filename_source;
	log += "\n";
	log.add_formatted ("Zipcode layer size : %d\n", layer_zipcodes->objects.size ());

	return !error;
}


void frame_CARDGIS::OnGrainPrices
	(wxCommandEvent &)

// 2017-01-03 Read warehoses and townships
// Find nearest township to each warehouse

{
	dynamic_string log, filename_marina_db;
	GrainPrice gp;
	interface_window_wx view;
	view.start_progress (this);

	gp.filename_db = "I:\\TDC\\GrainPrice\\GrainPrice.accdb";

	if (gp.setup (map_watershed, &view, log)) {

		gp.distance_warehouse_township (map_watershed, &view, log);
		gp.compare_prices (map_watershed, &view, log);

		map_watershed->set_extent ();
		
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("GrainPrices");
	}

	view.destroy_dialog_progress ();

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"GrainPrices", L"", log);
		d.ShowWindowModal ();
	}
}

/*
void frame_CARDGIS::OnSavePNG
	(wxCommandEvent &)

// Create a PNG image of current map

{
	dynamic_string log, filename_output;
	interface_window_wx view;
	view.start_progress (this);

	wxImage::AddHandler(new wxPNGHandler);

	filename_output = "NHDView.png";

	if (get_write_path (&filename_output, L"PNG Files|*.png|All Files|*.*", L"png", this)) {
		wxImage image_png;
		// wxSize image_size (1920, 1080);
		wxSize image_size (3840, 2160);

		if (panel_watershed->draw_bitmap (&image_png, image_size, log)) {
			if (!image_png.SaveFile (filename_output.get_text (), wxBITMAP_TYPE_PNG )) {
				log += "ERROR, can't save PNG image to \"";
				log += filename_output;
				log += "\".\n";
			}
		}
		else
			log + "ERROR, can't draw bitmap.\n";

	}

	view.destroy_dialog_progress ();

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"GrainPrices", L"", log);
		d.ShowWindowModal ();
	}
}
*/
void frame_CARDGIS::OnSWATAnimation
	(wxCommandEvent &)

// 2017-01-23 Read csv file of monthly swat output
// Create a PNG image for each month

// Selected layer has to be outlines of the subbasins/reaches

{
	dynamic_string log, output_path, filename_data, filename_data_2;
	swat_animator animator;
	map_layer *layer;
	bool side_by_side, unified_scale = true;
	BYTE y_m_d = 3;

	interface_window_wx view;
	view.start_progress (this);

	// Input file has to be sorted by year,month,day (not ID)
	// filename_data = "E:\\swat runs\\UMRB\\2017-01-10\\Target 421 Sorted.txt";
	// output_path = "E:\\swat runs\\UMRB\\2017-01-10\\images\\SWAT";
	// output_path = "E:\\swat runs\\UMRB\\2017-01-10\\images FlowOut\\";

	// filename_data = "E:\\swat runs\\OTRB\\2017-09-14\\Target 1813 Sorted.txt";
	// filename_data = "E:\\swat runs\\OTRB\\Target 1813 Sorted GF.txt";
	// output_path = "E:\\swat runs\\OTRB\\images\\";

	// filename_data = "E:\\swat runs\\Iowa\\Boone\\PointSourceTest\\Target 4 Sorted.txt";
	// output_path = "E:\\swat runs\\Iowa\\Boone\\PointSourceTest\\Images\\";

	// filename_data = "E:\\swat runs\\Minnesota\\LeSueur 2017-04-11\\Target 77 Buffer Strips 664.txt";
	// output_path = "E:\\swat runs\\Minnesota\\LeSueur 2017-04-11\\images\\";

	/*
	filename_data = "E:\\swat runs\\OTRB\\2017-11-20\\Subbasin 1813 Sorted.txt";
	filename_data_2 = "E:\\swat runs\\OTRB\\2017-11-20\\HRU 1813 Sorted.txt";
	side_by_side = true;
	output_path = "E:\\swat runs\\OTRB\\2017-11-20\\images\\";
	animator.target_subbasin = 1813;
	// animator.target_column_name = "NO3Out";
	// animator.target_column_name = "FlowOut";
	animator.target_column_name_1 = "Soil Water Content";
	animator.target_column_name_2 = "Soil Water Content";

	*/
	// 2017-12-04
	// 2018-01-11
	// filename_data = "E:\\swat runs\\OTRB\\2017-11-20\\Reach 1813 PS Sorted.txt";
	// filename_data = "E:\\swat runs\\OTRB\\2017-11-20\\Reach 1813 PS+NPS ZEROINI.txt";
	/*
	filename_data = "E:\\swat runs\\OTRB\\2017-11-20\\Target 1813 NPS Reach IWQ.txt";
	filename_data_2 = "E:\\swat runs\\OTRB\\2017-11-20\\Target 1813 PS Reach IWQ.txt";
	animator.target_subbasin = 1813;
	side_by_side = true;
	unified_scale = false;
	y_m_d = 2;
	output_path = "E:\\swat runs\\OTRB\\2017-11-20\\images\\";
	animator.target_column_name_1 = "NO3Out";
	// animator.target_column_name_2 = "NO3 Surface Runoff";
	animator.target_column_name_2 = "NO3Out";
	animator.title_text_1 = "Non Point Source NO3Out, kg N";
	// animator.title_text_2 = "Subbasin Precipitation, mm H2O";
	animator.title_text_2 = "Point Source NO3Out, kg N";
	*/
	/*
	// filename_data = "E:\\swat runs\\OTRB\\2017-11-20\\Subbasin 1813 NPS Sorted.txt";
	// filename_data_2 = "E:\\swat runs\\OTRB\\2017-11-20\\HRU 1813 NPS Sorted.txt";
	filename_data = "E:\\swat runs\\OTRB\\2018-04-03\\Target 1813 PS Daily.csv";
	filename_data_2 = "E:\\swat runs\\OTRB\\2018-04-03\\Target 1813 PS Daily.csv";
	side_by_side = true;
	unified_scale = false;
	output_path = "E:\\swat runs\\OTRB\\2018-04-03\\images\\";
	animator.target_subbasin = 1813;
	animator.target_column_name_1 = "NO3Out";
	animator.target_column_name_2 = "MineralPOut";
	animator.title_text_1 = "Point Source NO3Out, kg N";
	animator.title_text_2 = "Point Source MineralPOut, kg N";
	y_m_d = 3;
	*/
	/*
	filename_data = "E:\\swat runs\\Iowa\\Boone\\NIFA 2015-12-21\\Target 2 NPS+PS.txt";
	side_by_side = false;
	output_path = "E:\\swat runs\\Iowa\\Boone\\NIFA 2015-12-21\\images\\";
	animator.target_subbasin = 2;
	animator.target_column_name_1 = "NO3Out";
	*/
	/*
	// 2018-03-30
	// filename_data = "E:\\swat runs\\OTRB\\2017-11-20\\Target 1813 NPS Reach DP.txt";
	filename_data = "E:\\swat runs\\OTRB\\2017-11-20\\ReachData.txt";
	y_m_d = 3;
	side_by_side = false;
	output_path = "E:\\swat runs\\OTRB\\2017-11-20\\images\\";
	animator.target_subbasin = 1813;
	animator.target_column_name_1 = "Baseline SHYD no3";
	// animator.title_text_1 = "Non Point Source NO3Out, kg N";
	animator.title_text_1 = "SHYD NO3Out";
	*/
	/*
	animator.override_logical_window.x[0] = -83.889247;
	animator.override_logical_window.x[1] = -82.792354;
	animator.override_logical_window.y[0] = 38.625791;
	animator.override_logical_window.y[1] = 39.418250;
	*/

	/*
	// 2018-04-16
	filename_data = "E:\\swat runs\\OTRB\\2018-04-03\\VAROUTE-SHYD ROUTE.txt";
	filename_data_2 = "E:\\swat runs\\OTRB\\2018-04-03\\VAROUTE-SHYD ROUTE.txt";
	side_by_side = true;
	unified_scale = false;
	output_path = "E:\\swat runs\\OTRB\\2018-04-03\\images\\";
	animator.target_subbasin = 1813;
	animator.target_column_name_1 = "Baseline Varoute water, m^3";
	animator.target_column_name_2 = "Baseline Varoute no3, kg";
	animator.show_diffs = true;
	animator.title_text_1 = "Water, m^3";
	animator.title_text_2 = "VAROUTE NO3, kg";
	animator.input_source = SWAT_ANIMATOR_INPUT_HSL;
	y_m_d = 3;
	*/

	/*
	// 2018-09-12
	filename_data = "E:\\swat runs\\UMRB\\2018-09-11\\Target 234 PS Daily.csv";
	side_by_side = false;
	unified_scale = false;
	output_path = "E:\\swat runs\\UMRB\\2018-09-11\\images\\";
	animator.target_subbasin = 234;
	animator.target_column_name_1 = "MineralPOut";
	animator.title_text_1 = "Point Source MineralPOut, kg N";
	animator.input_source = SWAT_ANIMATOR_INPUT_CSV;
	y_m_d = 3; // daily
	*/

	// 2018-09-25
	filename_data = "r:\\swat\\UMRB\\Database\\2018-09-11\\SubbasinEffect.accdb";
	animator.target_subbasin = 1010;
	side_by_side = false;
	unified_scale = false;
	output_path = "E:\\swat runs\\UMRB\\2018-09-11\\images\\";
	animator.target_column_name_1 = "MineralPOut";
	animator.title_text_1 = "Non Point Source MineralPOut, kg N";
	y_m_d = 2; // Data is by month
	animator.output_filename_prefix = "Target 1010 MineralP PS";
	animator.input_source = SWAT_ANIMATOR_INPUT_ODBC;

	if ((layer = panel_watershed->layer_selection) != NULL)
		layer->visible = false;

	if ((layer = panel_watershed->layer_selection_source) != NULL) {
		if (side_by_side) {
			// Create local maps here so swat_animator doesn't have to know about wx
			dynamic_map_wx local_map_1, local_map_2;
			map_layer *layer_1, *layer_2;

			local_map_1.logical_extent = local_map_2.logical_extent = map_watershed->logical_extent;
			local_map_1.scale = local_map_2.scale = map_watershed->scale;
			layer_1 = local_map_1.create_new (MAP_OBJECT_POLYGON);
			layer_1->copy (layer);
			layer_1->name = "Water";
			local_map_1.layers.push_back (layer_1);

			layer_2 = local_map_2.create_new (MAP_OBJECT_POLYGON);
			layer_2->copy (layer);
			layer_2->name = "NO3 Diff";
			local_map_2.layers.push_back (layer_2);

			animator.process_side_by_side (filename_data, filename_data_2, y_m_d, unified_scale, output_path, &local_map_1, &local_map_2, layer_1, layer_2, panel_watershed, &view, log);
		}
		else {
			dynamic_map_wx local_map_1;
			map_layer *layer_1;

			local_map_1.logical_extent = map_watershed->logical_extent;
			local_map_1.scale = map_watershed->scale;
			layer_1 = local_map_1.create_new (MAP_OBJECT_POLYGON);
			layer_1->copy (layer);
			layer_1->name = "Reach";
			local_map_1.layers.push_back (layer_1);

			animator.process (filename_data, output_path, y_m_d, &local_map_1, layer_1, panel_watershed, &view, log);
		}
	}
	else
		log += "ERROR, polygon layer not loaded.\n";
	if ((layer = panel_watershed->layer_selection) != NULL)
		layer->visible = true;

	view.destroy_dialog_progress ();

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"GrainPrices", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnSWATImage
	(wxCommandEvent &)

// 2018-02-07 Read csv file of monthly swat output
// Create a PNG image showing totals over time

// Selected layer has to be outlines of the subbasins/reaches

{
	dynamic_string log, output_path, filename_data, filename_data_2;
	swat_animator animator;
	map_layer *layer;
	bool side_by_side, unified_scale, monthly_average;
	BYTE y_m_d = 3;

	interface_window_wx view;
	view.start_progress (this);

	// 2018-02-07
	filename_data = "E:\\swat runs\\OTRB\\2017-11-20\\Target 1813 NPS Reach IWQ.txt";
	filename_data_2 = "E:\\swat runs\\OTRB\\2017-11-20\\Target 1813 PS Reach IWQ.txt";
	animator.target_subbasin = 1813;
	side_by_side = true;
	unified_scale = false;
	output_path = "E:\\swat runs\\OTRB\\2017-11-20\\images\\";
	animator.target_column_name_1 = "MineralPOut";
	animator.target_column_name_2 = "MineralPOut";
	animator.title_text_1 = "Non Point Source MineralPOut, kg N";
	animator.title_text_2 = "Point Source MineralPOut, kg N";
	y_m_d = 2; // Data is by month
	monthly_average = false;  // results will be average of nonzero monthly values?
	animator.output_filename_prefix = "Target 1813 MineralP IWQ Annual Ave";
	animator.input_source = SWAT_ANIMATOR_INPUT_CSV;


	if ((layer = panel_watershed->layer_selection) != NULL)
		layer->visible = false;

	if ((layer = panel_watershed->layer_selection_source) != NULL) {
		if (side_by_side) {
			// Create local maps here so swat_animator doesn't have to know about wx
			dynamic_map_wx local_map_1, local_map_2;
			map_layer *layer_1, *layer_2;

			local_map_1.logical_extent = local_map_2.logical_extent = map_watershed->logical_extent;
			local_map_1.scale = local_map_2.scale = map_watershed->scale;
			layer_1 = local_map_1.create_new (MAP_OBJECT_POLYGON);
			layer_1->copy (layer);
			layer_1->name = "Subbasin";
			local_map_1.layers.push_back (layer_1);

			layer_2 = local_map_2.create_new (MAP_OBJECT_POLYGON);
			layer_2->copy (layer);
			layer_2->name = "Subbasin";
			local_map_2.layers.push_back (layer_2);

			animator.process_image_side_by_side (filename_data, filename_data_2, y_m_d, unified_scale, monthly_average, output_path, &local_map_1, &local_map_2, layer_1, layer_2, panel_watershed, &view, log);
		}
		else {
			dynamic_map_wx local_map_1;
			map_layer *layer_1;

			local_map_1.logical_extent = map_watershed->logical_extent;
			local_map_1.scale = map_watershed->scale;
			layer_1 = local_map_1.create_new (MAP_OBJECT_POLYGON);
			layer_1->copy (layer);
			layer_1->name = "Reach";
			local_map_1.layers.push_back(layer_1);

			animator.process_image (filename_data, y_m_d, monthly_average, output_path, &local_map_1, layer_1, panel_watershed, &view, log);
		}
	}
	else
		log += "ERROR, polygon layer not loaded.\n";
	if ((layer = panel_watershed->layer_selection) != NULL)
		layer->visible = true;

	view.destroy_dialog_progress ();

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"GrainPrices", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnLakeCenters
	(wxCommandEvent &)

// 2017-06-16 Create a new layer with center points of each polygon
// Selected layer should be lake poylgons (or vectors)

{
	dynamic_string log, report;
	map_layer *layer_centers, *layer_rivers;
	map_object *center;
	std::vector <map_object *>::const_iterator lake;
	bounding_cube map_extent;
	int center_index;

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POLYGON)) {
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		layer_rivers = map_watershed->match ("Rivers");
		if (layer_rivers != NULL) {

			// Expand overlap area by 5km
			double buffer_x, buffer_y;
			layer_rivers->check_extent (&map_extent);

			buffer_x = offset_longitude_meters (map_extent.center (), 5000.0);
			buffer_y = offset_latitude_meters (map_extent.center (), 5000.0);

			map_extent.x[0] -= buffer_x;
			map_extent.x[1] += buffer_x;
			map_extent.y[0] -= buffer_y;
			map_extent.y[1] += buffer_y;
		}

		layer_centers = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_centers->name = panel_watershed->layer_selection_source->name;
		layer_centers->name += " Centers";
		layer_centers->attribute_count_numeric = 0;
		layer_centers->attribute_count_text = 0;
		layer_centers->initialize_attributes = true;
		layer_centers->color = COLOR_FACILITIES;
		layer_centers->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
		layer_centers->symbol_size = 6;
		layer_centers->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		map_watershed->layers.push_back (layer_centers);

		for (lake = panel_watershed->layer_selection_source->objects.begin ();
		lake != panel_watershed->layer_selection_source->objects.end ();
		++lake) {
			center = layer_centers->create_new (MAP_OBJECT_POINT);
			center->id = (*lake)->id;
			center->name = (*lake)->name;

			// 2017-06-21 Dave: use numeric index center of lake vector
			// (*lake)->centroid (&center->latitude, &center->longitude);
			center_index = ((map_polygon *) (*lake))->node_count / 2;
			center->longitude = ((map_polygon *) (*lake))->nodes [center_index * 2];
			center->latitude = ((map_polygon *) (*lake))->nodes [center_index * 2 + 1];

			if ((layer_rivers == NULL)
			|| (layer_rivers->overlaps_logical (&map_extent)))
				// If NHD rivers are present
				// Restrict lake centers to within 5km of boundary
				layer_centers->objects.push_back (center);
			else
				delete center;
		}

		panel_watershed->change_layers ();

		if (map_watershed->layers.size () == 1)
			panel_watershed->change_selection_layer (layer_centers);

		enable_map_controls ();
	}
	else
		log += "ERROR, no selected layer, or layer is not POLYGON type.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Linked Points", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnUpstreamCenters
	(wxCommandEvent &)

// 2017-06-16 Compute distance from each point to center of each upstream COMID

{
	dynamic_string log, report;
	map_layer *layer_comid_centers;
	bool write_agency_column = false; // true for STORET Legacy
	bool write_station_names = true;
	bool ponto = false; // BHO Pontodrenagem
	char output_delimiter = '\t';

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POINT)) {
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		router_NHD* router = relevent_router();
		map_layer* layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

		// create a layer for centerpoints for each upstream_comid
		layer_comid_centers = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_comid_centers->name = "COMID Centers";
		layer_comid_centers->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
		layer_comid_centers->initialize_attributes = true;
		layer_comid_centers->color = COLOR_FACILITIES;
		layer_comid_centers->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
		layer_comid_centers->symbol_size = 6;
		layer_comid_centers->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		map_watershed->layers.push_back (layer_comid_centers);

		// step 1 find nearest COMID for each lake center
		log += panel_watershed->layer_selection_source->name;
		log += "\n";
		router->link_points_threaded(panel_watershed->layer_selection_source, app->multithread_count, layer_rivers, write_agency_column, ponto, &view, report, log);
		log += report;

		// Traverse lake center points
		{
			std::vector <map_object *>::const_iterator center_point, point_upstream;
			std::vector <long long> upstream_comids;
			std::vector <long long>::iterator upstream_comid;
			map_object *upstream_center, *upstream_river_segment;		

			for (center_point = panel_watershed->layer_selection_source->objects.begin ();
			center_point != panel_watershed->layer_selection_source->objects.end ();
			++center_point) {
				// make list of upstream COMIDs
				router->rivers.accumulate_upstream ((*center_point)->attributes_numeric [USGS_ATTRIBUTES_INDEX_COMID], &upstream_comids);

				// Fill layer_comid_centers
				layer_comid_centers->objects.clear ();
				for (upstream_comid = upstream_comids.begin ();
				upstream_comid != upstream_comids.end ();
				++upstream_comid) {
					if ((upstream_river_segment = layer_rivers->match_id (*upstream_comid)) != NULL) {

						upstream_center = layer_comid_centers->create_new (MAP_OBJECT_POINT);
						upstream_center->id = *upstream_comid;
						upstream_river_segment->centroid (&upstream_center->latitude, &upstream_center->longitude);
						layer_comid_centers->objects.push_back (upstream_center);
					}
					else
						log.add_formatted ("ERROR, upstream COMID %lld not found in river layer\n", *upstream_comid);
				}

				// computer distance to each member of layer_comid_centers
				router->route_facilities_threaded_queue (app->multithread_count, panel_watershed->layer_selection_source, layer_comid_centers, 8,
				&view, NULL, output_delimiter, write_station_names, log);
			}
		}
	}
	else
		log += "ERROR, no selected layer, or layer is not POINT type.\n";

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"Linked Points", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnSSURGOZipCodes
	(wxCommandEvent &)

// 2017-07-12 Adriana: overlap SSURGO and zipcodes for 9 states.

{
	dynamic_string log, area_name, path_zipcode_shapefile, filename_ssurgo, filename_report;
	map_layer *layer_zipcodes, *layer_ssurgo, *layer_clipped;
	bool error = false;
	bounding_cube ssurgo_extent;
	int state_fips;

	path_zipcode_shapefile = "H:\\scratch\\LakeErieAnglers\\tl_2016_us_zcta510\\tl_2016_us_zcta510.shp";
	filename_ssurgo = "R:\\ssurgo\\DATAMART\\Iowa\\soil_ia001\\spatial\\soilmu_a_ia001.shp";
	filename_report = "h:\\scratch\\SSURGO ZipCodes\\ia001.txt";
	state_fips = 19;

	log += filename_ssurgo;
	log += "\n";

	dialog_run_threads update_display (this, true, false);
	update_display.Show ();

	interface_window_wx view;
	update_display.setup_interface_window (app, &view);

	panel_watershed->pause_rendering = true;

	if ((layer_ssurgo = map_watershed->match ("SSURGO")) == NULL) {
		layer_ssurgo = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_ssurgo->name = "SSURGO";
		layer_ssurgo->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_ssurgo->color = RGB (0, 255, 0);
		layer_ssurgo->initialize_attributes = true;
		map_watershed->layers.push_back(layer_ssurgo);
	}

	if ((layer_zipcodes = map_watershed->match ("Zipcodes")) == NULL) {
		layer_zipcodes = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_zipcodes->name = "Zipcodes";
		layer_zipcodes->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
		layer_zipcodes->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_zipcodes->color = RGB (128, 128, 128);
		layer_zipcodes->initialize_attributes = true;
		layer_zipcodes->attribute_count_text = 2;
		layer_zipcodes->column_names_text.push_back ("INTPTLAT10");
		layer_zipcodes->column_names_text.push_back ("INTPTLON10");
		map_watershed->layers.push_back(layer_zipcodes);
	}
	if ((layer_clipped = map_watershed->match ("Intersection")) == NULL) {
		layer_clipped = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_clipped->name = "Intersection";
		layer_clipped->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_clipped->color = RGB (255, 0, 0);
		layer_clipped->attribute_count_numeric = 6; // Space for area fields in ClipperBuffer.Paths_to_map_layer
		layer_clipped->column_names_numeric.push_back ("Facility ID"); // leftover from other functions
		layer_clipped->column_names_numeric.push_back ("Buffer Width"); // leftover from other functions
		layer_clipped->column_names_numeric.push_back ("Geodesic m^2");
		layer_clipped->column_names_numeric.push_back ("Exact m^2");
		layer_clipped->column_names_numeric.push_back ("Authalic m^2");
		layer_clipped->column_names_numeric.push_back ("Rhumb m^2");
		layer_clipped->attribute_count_text = 2;
		layer_clipped->column_names_text.push_back ("MUKey");
		layer_clipped->column_names_text.push_back ("ZipCode");
		layer_clipped->initialize_attributes = true;
		map_watershed->layers.push_back (layer_clipped);
	}
	{
		importer_shapefile importer_ssurgo;
		importer_ssurgo.filename_source = filename_ssurgo;
		importer_ssurgo.name_field_name = "MUSYM";
		importer_ssurgo.projection = SHAPEFILE_PROJECTED_UTM;
		importer_ssurgo.longitude_central_meridian = -93;
		importer_ssurgo.normalize_longitude = false;
		importer_ssurgo.take_dbf_columns = true;

		if (!importer_ssurgo.import (layer_ssurgo, NULL, map_watershed, &view, log))
			error = true;
	}

	if (!error) {

		layer_ssurgo->set_extent ();
		layer_ssurgo->check_extent (&ssurgo_extent);
		ssurgo_extent.normalize ();
		ssurgo_extent.inflate (1.05);

		{
			shapefile_filter *filter;

			view.update_progress ("Reading Zipcodes");

			filter = new shapefile_filter;
			filter->type = SHAPEFILE_FILTER_AREA;
			filter->location = ssurgo_extent;

			read_zipcode_shapefile (path_zipcode_shapefile, filter, layer_zipcodes, &view, log);
		}

		int soil_attribute_index_MUKEY = 1; // -1 to use name instead of indexing attributes_text


		// Create a ClipperLib.Paths from ssurgo layer 
		{
			ClipperBuffer clipper;
			// clipper.intersect_threaded (layer_ssurgo, layer_zipcodes, layer_clipped, -1, app->multithread_count, &view, log);
			clipper.tract_id_source_1 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
			clipper.tract_index_source_1 = soil_attribute_index_MUKEY;
			clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_TEXT_ATTRIBUTE;
			clipper.tract_index_source_2 = -1;

			clipper.intersect_threaded (layer_zipcodes, layer_ssurgo, layer_clipped, app->multithread_count, &view, log);
		}

		{
			dynamic_string report;
			std::vector <map_object *>::iterator fragment;

			report += "ID\tState FIPS\tZip Code\tMUKey\tGeodesic, m^2\tExact, m^2\tAuthalic, m^2\tRhumb, m^2\n";
			for (fragment = layer_clipped->objects.begin ();
			fragment != layer_clipped->objects.end ();
			++fragment) {
				report.add_formatted ("%ld\t%d\t", (*fragment)->id, state_fips);
				report += (*fragment)->attributes_text[1];
				report += "\t";
				report += (*fragment)->attributes_text[0];
				report.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\n",
					(*fragment)->attributes_numeric[2], (*fragment)->attributes_numeric[3], (*fragment)->attributes_numeric[4], (*fragment)->attributes_numeric[5]);
			}
			write_log (filename_report, report);
		}

		if (log.get_length () > 0) {
			log.convert_linefeeds_for_CEdit ();
			dialog_error_list d (this, L"File Open", L"", log);
			d.ShowWindowModal ();
		}
		update_display.Destroy ();

		panel_watershed->pause_rendering = false;
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenZipCodes");
	}
}

void frame_CARDGIS::OnHUC12Upstream
	(wxCommandEvent &)

// 2017-07-17 Dave Keiser: read from/to list of HUCs and produce list of upstream HUCs for each.

{
	dynamic_string log, path_huc_fromto, path_output, filename_vector_shp, filename_HUC_shp;
	HUC_tree *tree;
	importer_shapefile importer;
	long huc_2_ge, huc_2_lt;
	bool tree_checked_ok;

	// path_huc_fromto = "R:\\DKEISER\\watersheds_updown\\huc12_usa.csv";
	// huc_2_ge = 10;
	// huc_2_lt = 11;
	huc_2_ge = huc_2_lt = -1;

	path_huc_fromto = "R:\\DKEISER\\watersheds_updown\\source_file\\huc12_us.txt";
	if (huc_2_ge != -1)
		path_output.format ("h:\\scratch\\HUC12_USA\\huc12_usa-%02ld.txt", huc_2_ge);
	else
		path_output = "h:\\scratch\\HUC12_USA\\huc12_usa.txt";
	filename_vector_shp = "h:\\scratch\\HUC12_USA\\HUCVectors.shp";
	tree_checked_ok = true; // set false to find loop and create a layer showing it

	log += path_huc_fromto;
	log += "\n";

	interface_window_wx view;
	view.setup_wx (this);

	view.start_progress (this);

	// READ the HUC polygon map layer
	map_layer *layer_hucs = map_watershed->create_new (MAP_OBJECT_POLYGON);
	layer_hucs->enclosure = MAP_POLYGON_NOT_ENCLOSED; // Don't draw line from first node to last
	layer_hucs->name.format ("%02d0 HUCs", huc_2_ge); // Was importer->filename_source.  Needed for OnShowRiver
	layer_hucs->color = RGB (128, 64, 0);
	layer_hucs->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_hucs->attribute_count_text = 1;
	layer_hucs->initialize_attributes = true; // clear all attributes as polygons are created
	map_watershed->layers.push_back (layer_hucs);

	map_layer *layer_fragments = map_watershed->create_new (MAP_OBJECT_VECTOR);
	layer_fragments->enclosure = MAP_POLYGON_NOT_ENCLOSED; // Don't draw line from first node to last
	layer_fragments->name = "UMRB HUC Fragments"; // Was importer->filename_source.  Needed for OnShowRiver
	layer_fragments->color = RGB (255, 0, 0);
	layer_fragments->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_fragments->attribute_count_text = 1;
	layer_fragments->initialize_attributes = true; // clear all attributes as polygons are created
	map_watershed->layers.push_back (layer_fragments);

	filename_HUC_shp = "H:\\scratch\\HUC12_USA\\NHDPlusV21_NationalData_WBDSnapshot_Shapefile_08\\NHDPlusNationalData\\WBDSnapshot_National.shp";
	importer.filename_source = filename_HUC_shp;
	importer.id_field_name = "OBJECTID_1";
	importer.name_field_name = "HUC_12";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.longitude_central_meridian = -93;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = false;

	// 2020-03-24 must define importer_shapefile.dbf_column_destinations_numeric, dbf_column_destinations_text
	ASSERT (false);

	if (huc_2_ge != -1) {
		// FILTER to 07 UMRB
		shapefile_filter *filter = new shapefile_filter;
		filter->type = SHAPEFILE_FILTER_MASK;
		filter->field_name = "HUC_12";
		// filter->pattern.format ("%02ld##########", huc);
		filter->pattern.format ("%02ld#########", huc_2_ge);

		importer.filters.push_back (filter);
	}

	// read huc polygon shapefile
	if (importer.import (layer_hucs, NULL, map_watershed, &view, log)) {
		view.update_progress ("Reading FROM-TO list");

		tree = new HUC_tree;
		if (tree->read_source_12 (path_huc_fromto, huc_2_ge, huc_2_lt, log)) {

			log.add_formatted ("Unique HUC size: %ld\n", (long) tree->unique_hucs.size ());

			// tree.follow_raw_downstream (70200071002, log);

			view.update_progress ("Building Tree");
			tree->build_tree (&view, log);
			log.add_formatted ("Tree size: %ld\n", (long) tree->all_ids.size ());

			view.update_progress ("Writing Table");

			if (tree_checked_ok) {
				// Write output table of upstream HUCs
				tree->write (path_output, &view, log);
				// Produce shapefile of vectors
				tree->build_vector_shapefile (filename_vector_shp, layer_hucs, layer_fragments, map_watershed, true, &view, log);
			}
			else
				// Search for loop and fill layer_fragments with looped ids
				tree->highlight_loop_12 (layer_fragments, layer_hucs, &view, log);

			map_watershed->check_extent ();
			panel_watershed->change_layers ();
			enable_map_controls ();
			panel_watershed->redraw ("OnHUCUpstream");
		}
		delete tree;
	}

	view.destroy_dialog_progress ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnHUC8Upstream
	(wxCommandEvent &)

// 2017-07-17 Dave Keiser: read from/to list of HUCs and produce list of upstream HUCs for each.

{
	dynamic_string log, path_huc_fromto, path_output, filename_vector_shp, filename_HUC_shp;
	HUC_tree *tree;
	importer_shapefile importer;

	path_huc_fromto = "R:\\DKEISER\\watersheds_updown\\output_8\\huc8_usa_fromto.csv";
	// path_huc_fromto = "h:\\scratch\\HUC12_USA\\huc8_usa_fromto.csv";

	path_output = "h:\\scratch\\HUC12_USA\\huc8_usa.txt";

	filename_vector_shp = "h:\\scratch\\HUC12_USA\\HUCVectors.shp";

	log += path_huc_fromto;
	log += "\n";

	interface_window_wx view;
	view.setup_wx (this);

	view.start_progress (this);

	// READ the HUC polygon map layer
	map_layer *layer_hucs = map_watershed->create_new (MAP_OBJECT_VECTOR);
	layer_hucs->enclosure = MAP_POLYGON_NOT_ENCLOSED; // Don't draw line from first node to last
	layer_hucs->name = "HUCs"; // Was importer->filename_source.  Needed for OnShowRiver
	layer_hucs->color = RGB (0, 255, 0);
	layer_hucs->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_hucs->attribute_count_text = 1;
	layer_hucs->initialize_attributes = true; // clear all attributes as polygons are created
	map_watershed->layers.push_back (layer_hucs);

	map_layer *layer_fragments = map_watershed->create_new (MAP_OBJECT_VECTOR);
	layer_fragments->enclosure = MAP_POLYGON_NOT_ENCLOSED; // Don't draw line from first node to last
	layer_fragments->name = "UMRB HUC Fragments"; // Was importer->filename_source.  Needed for OnShowRiver
	layer_fragments->color = RGB (255, 0, 0);
	layer_fragments->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_fragments->attribute_count_text = 1;
	layer_fragments->initialize_attributes = true; // clear all attributes as polygons are created
	map_watershed->layers.push_back (layer_fragments);

	filename_HUC_shp = "D:\\GIS\\HUC8\\WBDHU8.shp";
	importer.filename_source = filename_HUC_shp;
	importer.id_field_name = "HUC8";
	importer.name_field_name = "NAME";
	importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
	importer.normalize_longitude = false;
	importer.take_dbf_columns = true;

	// FILTER to 07 UMRB
	shapefile_filter *filter = new shapefile_filter;
	filter->type = SHAPEFILE_FILTER_RANGE;
	filter->field_name = "HUC8";
	// filter->pattern.format ("%02ld##########", huc);
	// USA HUC8 Alaska, Hawaii and PR are 19010102 and up
	filter->range_ge = 1000000;
	filter->range_lt = 19000000;

	importer.filters.push_back (filter);

	// read huc polygon shapefile
	if (importer.import (layer_hucs, NULL, map_watershed, &view, log)) {
		view.update_progress ("Reading FROM-TO list");

		tree = new HUC_tree;
		if (tree->read_source_8 (path_huc_fromto, log)) {

			log.add_formatted ("Unique HUC size: %ld\n", (long) tree->unique_hucs.size ());

			// tree.follow_raw_downstream (70200071002, log);

			view.update_progress ("Building Tree");
			tree->build_tree (&view, log);
			log.add_formatted ("Tree size: %ld\n", (long) tree->all_ids.size ());

			// HUC_network_link *l = tree->match_all_just_below (4060101);
			// l = tree->match_all_just_below (4060102);

			view.update_progress ("Writing Table");

			// Write output table of upstream HUCs
 			tree->write (path_output, &view, log);
			// tree->highlight_loop_8 (layer_fragments, layer_hucs, &view, log);

			// Produce shapefile of vectors
			// tree->build_vector_shapefile (filename_vector_shp, layer_hucs, layer_fragments, map_watershed, false, &view, log);
			tree->build_tree_vectors (layer_fragments, layer_hucs, &view, log);

			map_watershed->check_extent ();
			panel_watershed->change_layers ();
			enable_map_controls ();
			panel_watershed->redraw ("OnHUCUpstream");
		}
		delete tree;
	}

	view.destroy_dialog_progress ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::RoutingHUC12to8
	(wxCommandEvent &)

// 2017-08-17 Dave Keiser: Try converting huc12 to huc8

{
	dynamic_string log, path_huc_fromto, path_output;
	HUC_tree *tree;
	importer_shapefile importer;

	path_huc_fromto = "R:\\DKEISER\\watersheds_updown\\source_file\\huc12_us.txt";
	path_output = "h:\\scratch\\HUC12_USA\\huc8_usa_fromto.csv";

	log += path_huc_fromto;
	log += "\n";

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	view.update_progress ("Reading FROM-TO list");

	tree = new HUC_tree;
	std::map <long, long> from_to_8;
	if (tree->convert_12_8 (path_huc_fromto, &from_to_8, log)) {
		view.update_progress ("Writing Table");
		FILE *f;
		std::map <long, long>::iterator route;

		if (fopen_s (&f, path_output.get_text_ascii (), "w") == 0) {

			for (route = from_to_8.begin ();
			route != from_to_8.end ();
			++route)
				fprintf (f, "%ld,%ld\n", route->first, route->second);

			fclose (f);
		}
		else {
			log += "ERROR, can't open output file \"";
			log += path_output;
			log += "\"\n";
		}
	}
	delete tree;

	view.destroy_dialog_progress ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnRouteCounties
	(wxCommandEvent &)

// 2017-09-06 Dave Keiser: Find nearest comid to each county center, list all counties downstream with routed distance to each

{
	dynamic_string log, report, path_output, filename_county_shp;
	map_layer *layer_county, *layer_rivers, *layer_centers, *layer_vectors;
	std::vector <map_object *>::const_iterator county;
	map_object *center;
	long count_out_of_county = 0, count_ended_in_county = 0, count_too_distant = 0, count_missing_river = 0, count_ok;
	std::vector <DownstreamByCOMID *> routes_comid;
	std::vector <DownstreamByCounty *> routes_county;
	bool write_agency_column = false; // true for STORET Legacy
	bool ponto = false; // BHO Pontodrenagem

	path_output = "H:\\scratch\\CountyRoutes\\Routed.accdb";

	log += "\n";

	interface_window_wx view;
	view.setup_wx (this);

	view.start_progress (this);

	// Shapefile in "r:\Census\TIGER 2012\County\tl_2012_us_county\tl_2012_us_county.shp"
	// is an exact match by visual inspection with "R:\GIS\County Map\CensusBureau_2016_us_county_5m\cb_2016_us_county_5m.shp"

	router_NHD* router = relevent_router();
	layer_rivers = router->river_map_layer; // map_watershed->match ("Rivers");

	if (((layer_county = map_watershed->match ("County Boundaries")) != NULL)
	&& (layer_rivers != NULL)) {

		layer_centers = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_centers->name = "County Centers";
		layer_centers->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
		layer_centers->initialize_attributes = true;
		layer_centers->color = COLOR_FACILITIES_OTHER;
		layer_centers->symbol = MAP_OBJECT_SYMBOL_X; // MAP_OBJECT_SYMBOL_CIRCLE;
		layer_centers->symbol_size = 6;
		layer_centers->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		map_watershed->layers.push_back (layer_centers);

		for (county = layer_county->objects.begin ();
		county != layer_county->objects.end ();
		++county) {
			center = layer_centers->create_new (MAP_OBJECT_POINT);
			center->id = (*county)->id;
			center->name = (*county)->name;
			(*county)->centroid (&center->latitude, &center->longitude);
			layer_centers->objects.push_back (center);
		}
		view.update_progress ("Linking Centroids");
		router->link_points_threaded (layer_centers, app->multithread_count, layer_rivers, write_agency_column, ponto, &view, report, log);

		{
			// show line from county center to neareset comid point
			map_object_vector *arrow;
			map_object *segment;
			map_polygon *river_segment;

			layer_vectors = map_watershed->create_new (MAP_OBJECT_VECTOR);
			layer_vectors->name = "Vectors";
			layer_vectors->color = RGB (255, 0, 255);
			layer_vectors->line_width = 1;
			map_watershed->layers.push_back (layer_vectors);

			for (county = layer_centers->objects.begin ();
			county != layer_centers->objects.end ();
			++county) {
				arrow = (map_object_vector *) layer_vectors->create_new (MAP_OBJECT_VECTOR);
				arrow->id = (*county)->id;
				arrow->name = (*county)->name;
				arrow->latitude = (*county)->latitude;
				arrow->longitude = (*county)->longitude;

				if ((segment = layer_rivers->match_id ((*county)->attributes_numeric[USGS_ATTRIBUTES_INDEX_COMID])) != NULL) {
					if (segment->type == MAP_OBJECT_POLYGON) {
						river_segment = (map_polygon *) segment;
						arrow->destination_longitude = (long) river_segment->nodes[(int) (*county)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POINT_INDEX] * 2];
						arrow->destination_latitude = (long) river_segment->nodes[(int) (*county)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POINT_INDEX] * 2 + 1];
					}
					else {
						river_segment = &((map_polygon_complex *) segment)->polygons[(int) (*county)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POLYGON_INDEX]];
						arrow->destination_longitude = (long) river_segment->nodes[(int) (*county)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POINT_INDEX] * 2];
						arrow->destination_latitude = (long) river_segment->nodes[(int) (*county)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POINT_INDEX] * 2 + 1];
					}
				}
				else
					log.add_formatted ("ERROR, arrow COMID %ld not found.\n", (*county)->attributes_numeric[USGS_ATTRIBUTES_INDEX_COMID]);

				layer_vectors->objects.push_back (arrow);
			}
		}

		router->route_points_downstream (layer_centers, layer_county, layer_rivers, &map_watershed->logical_extent, &routes_comid, &routes_county,
		&count_out_of_county, &count_ended_in_county, &count_too_distant, &count_missing_river, &count_ok, &view, report);

		log.add_formatted ("Routing started outside of county\t%ld\n", count_out_of_county);
		log.add_formatted ("Routing never exited county\t%ld\n", count_ended_in_county);
		log.add_formatted ("Nearest river >= 10km distant\t%ld\n", count_too_distant);
		log.add_formatted ("Nearest river not found\t%ld\n", count_missing_river);
		log.add_formatted ("County ok\t%ld\n", count_ok);

		// Write results to database path_output
		{
			filename_struct f_area;
			dynamic_string area_name;
			std::set <long> target_counties;
			std::vector <DownstreamByCOMID *>::iterator route_comid;
			std::vector <DownstreamByCounty *>::iterator route_county;

			// get area name
			f_area.parse (&filename_nhd_area);
			area_name = f_area.containing_subdirectory ();
			area_name = area_name.left (area_name.get_length () - 1); // Cut off the trailing '\'
			if (isdigit (area_name.get_at_ascii (area_name.get_length () - 1)))
				// "NHDPlus17"
				area_name = area_name.right (2);
			else
				// "NHDPlus03N"
				area_name = area_name.right (3);

			write_downstream_by_comid (path_output, area_name, &routes_comid, &view, log);
			write_downstream_by_county (path_output, area_name, &routes_county, &view, log);

			for (route_comid = routes_comid.begin ();
			route_comid != routes_comid.end ();
			++route_comid)
				delete *route_comid;

			for (route_county = routes_county.begin ();
			route_county != routes_county.end ();
			++route_county)
				delete *route_county;
		}

		log += report;

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnRouteCounties");
	}
	else
		log += "ERROR, NHD rivers and county boundaries must already be loaded.\n";

	view.destroy_dialog_progress ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void find_related_areas
	(const dynamic_string &area,
	std::set <dynamic_string> *related_areas)

// See http://tdc%202015-03-01.html#2015-03-27 for table of area interlinks

{
	related_areas->insert (area);

	if (area == "02") {
		related_areas->insert ("03S");
		related_areas->insert ("03N");
	}
	else
		if (area == "03W")
			related_areas->insert ("08");
		else
			if (area == "08") {
				related_areas->insert ("06");
				related_areas->insert ("05");
				related_areas->insert ("07");
				related_areas->insert ("10U");
				related_areas->insert ("10L");
				related_areas->insert ("11");
				related_areas->insert ("12");
			}
			else
				if (area == "15")
					related_areas->insert ("14");
				else
					if (area == "18")
						related_areas->insert ("17");
}

map_layer *frame_CARDGIS::load_river
	(dynamic_string &area_name,
	std::set <int> *state_set,
	interface_window *view,
	dynamic_string &log)

// 2017-09-21 Read all NHD segments for the area and all areas upstream for the entire river

{
	std::set <dynamic_string> related_areas;
	std::set <dynamic_string>::iterator run_area;
	dynamic_string filename_nhd, filename_network;
	importer_shapefile importer;
	filename_struct f_input;
	map_layer *layer_rivers;
	bool error = false;
	int area_number;

	layer_rivers = new map_layer;
	layer_rivers->name = "Rivers"; // Was importer->filename_source.  Needed for OnShowRiver
	layer_rivers->initialize_attributes = true; // clear all attributes as polygons are created
	layer_rivers->attribute_count_numeric = NHD_SEGMENT_ATTRIBUTE_COUNT; // FTYPE, LENGTHKM, DIVERGENCE, UPHYDROSEQ, DNHYDROSEQ
	map_watershed->layers.push_back (layer_rivers);
	layer_rivers->column_names_numeric.push_back ("FTYPE");
	layer_rivers->column_names_numeric.push_back ("Lengthkm");

	// other areas may drain into this
	find_related_areas (area_name, &related_areas);

	f_input.parse (&app->filename_nhd_root);

	for (run_area = related_areas.begin ();
	run_area != related_areas.end ();
	++run_area) {
		log += "Related Area\t";
		log += *run_area;
		log += "\n";
		f_input.write_path (&filename_nhd);
		filename_nhd += "NHDplus";
		filename_nhd += *run_area;
		filename_nhd += PATH_SEPARATOR;
		area_number = nhd_area_number (*run_area);
		area_state_overlaps (area_number, state_set);

		view->update_progress_formatted (0, "Related Area %d", area_number);

		// read shapefile
		router_v2->setup_shapefile (filename_nhd, layer_rivers, &importer); 
		if (!router_v2->read_shapefile (&importer, layer_rivers, map_watershed, view, log))
			error = true;

		// read VAA
		router_v2->read_flow_VAA (filename_nhd, view, log);

		// Read binary network
		filename_network = filename_nhd;
		filename_network += "Network.bin";
		if (!router_v2->rivers.read (filename_network, log))
			error = true;
	}

	return layer_rivers;
}

void frame_CARDGIS::OnRouteCountiesRiver
	(wxCommandEvent &)

// 2017-09-11 Route counties for a river that may include several NHD areas

{
	dynamic_string filename_report, filename_county_shp, filename_output, area_name, report, log;
	std::set <int> state_set;
	map_layer *layer_rivers, *layer_county, *layer_centers;
	std::vector <map_object *>::iterator county;
	map_object *center;
	std::vector <DownstreamByCOMID *> routes_comid;
	std::vector <DownstreamByCounty *> routes_county;
	filename_struct f_output;
	long count_out_of_county = 0, count_ended_in_county = 0, count_too_distant = 0, count_missing_river = 0, count_ok = 0;
	bool write_agency_column = false; // true for STORET Legacy
	bool ponto = false; // BHO Pontodrenagem

	// area_name = "02";
	// area_name = "03W";
	// area_name = "04";
	// area_name = "08";
	area_name = "09";
	// filename_nhd_root = "R:\\NHDPlusv2\\NHDPlus13\\";
	// filename_nhd_root = "R:\\NHDPlusv2\\NHDPlus15\\";
	// filename_nhd_root = "R:\\NHDPlusv2\\NHDPlus16\\";
	// filename_nhd_root = "R:\\NHDPlusv2\\NHDPlus18\\";

	dialog_prompt dp (this, "Area Name", &area_name);
	if (dp.ShowModal () == wxID_OK) {

		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		filename_county_shp = "r:\\Census\\TIGER 2012\\County\\tl_2012_us_county\\tl_2012_us_county.shp";
		filename_output = "H:\\scratch\\CountyRoutes\\Routed.accdb";
		f_output.parse (&filename_output);
		f_output.write_path (&filename_report);
		filename_report += "Routed-";
		filename_report += area_name;
		filename_report += ".log";

		layer_rivers = load_river (area_name, &state_set, &view, log);

		layer_county = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_county->name = "County Boundaries";
		layer_county->initialize_attributes = true;
		map_watershed->layers.push_back (layer_county);

		read_county_boundaries (filename_county_shp, map_watershed, layer_county, &state_set, &view, log);

		view.update_progress ("Linking Centroids");
		layer_centers = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_centers->name = "County Centers";
		layer_centers->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
		layer_centers->initialize_attributes = true;
		map_watershed->layers.push_back (layer_centers);

		map_watershed->set_extent ();

		for (county = layer_county->objects.begin ();
		county != layer_county->objects.end ();
		++county) {
			center = layer_centers->create_new (MAP_OBJECT_POINT);
			center->id = (*county)->id;
			center->name = (*county)->name;
			(*county)->centroid (&center->latitude, &center->longitude);
			layer_centers->objects.push_back (center);
		}

		view.update_progress ("Linking Centroids");
		router_v2->link_points_threaded (layer_centers, app->multithread_count, layer_rivers, write_agency_column, ponto, &view, report, log);

		view.update_progress ("Routing Centroids", 0);

		router_v2->route_points_downstream (layer_centers, layer_county, layer_rivers, &map_watershed->logical_extent, &routes_comid, &routes_county,
		&count_out_of_county, &count_ended_in_county, &count_too_distant, &count_missing_river, &count_ok, &view, report);

		log.add_formatted ("Routing started outside of county\t%ld\n", count_out_of_county);
		log.add_formatted ("Routing never exited county\t%ld\n", count_ended_in_county);
		log.add_formatted ("Nearest river >= 10km distant\t%ld\n", count_too_distant);
		log.add_formatted ("Nearest river not found\t%ld\n", count_missing_river);
		log.add_formatted ("County ok\t%ld\n", count_ok);

		// Write results to database path_output
		{
			std::vector <DownstreamByCOMID *>::iterator route_comid;
			std::vector <DownstreamByCounty *>::iterator route_county;
			std::set <long> target_counties;

			log += "Output database\t";
			log += filename_output;
			log += "\n";
			log += "Area name\t";
			log += area_name;
			log += "\n";

			write_downstream_by_county (filename_output, area_name, &routes_county, &view, log);
			write_downstream_by_comid (filename_output, area_name, &routes_comid, &view, log);

			for (route_comid = routes_comid.begin ();
			route_comid != routes_comid.end ();
			++route_comid)
				delete *route_comid;

			for (route_county = routes_county.begin ();
			route_county != routes_county.end ();
			++route_county)
				delete *route_county;
		}

		log += report;
		view.destroy_dialog_progress ();

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnCombineRouteCounties
	(wxCommandEvent &)

// 2017-09-13 Read tables of routed counties
// When duplicate counties are found, retain the closest 

{
	odbc_database_access *db;
	odbc_database_credentials creds;
	bool error = false;
	const int RIVER_COUNT = 10;
	dynamic_string table_name, river_areas [RIVER_COUNT] = {"01", "02", "03W", "04", "08", "09", "13", "15", "16", "18" }, filename_output, log;
	int river_index, river_numbers[RIVER_COUNT] = { NHDAREA_01, NHDAREA_02, NHDAREA_03W, NHDAREA_04, NHDAREA_08, NHDAREA_09, NHDAREA_13, NHDAREA_15, NHDAREA_16, NHDAREA_18 };
	// FIPS, area_constant, distance
	std::map <long, double> area_counties;
	std::map <long, std::pair <int, double>> counties;
	std::map <long, std::pair <int, double>>::iterator final_county;
	long count_written = 0;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	filename_output = "H:\\scratch\\CountyRoutes\\Routed.accdb";

	creds.set_database_filename (filename_output);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);

	db = new odbc_database_access;
	if (db->open (&creds, &view, log)) {

		view.update_progress (L"Reading all entries", 0);
		for (river_index = 0; river_index < RIVER_COUNT; ++river_index) {
			table_name = TABLE_NAME_DOWNSTREAM_BY_COUNTY;
			table_name += " Area";
			table_name += river_areas[river_index];

			log += "[";
			log += table_name;
			log += "]\n";
			// Fill counties from this area, replacing any duplicates found closer than existing entries
			if (!read_downstream_by_county (db, river_numbers[river_index], table_name, &counties, &view, log))
				error = true;
		}

		view.update_progress (L"Writing best routes", 0);
		log.add_formatted ("Unique county map size\t%d\n", counties.size ());

		// copy routes from counties to all-US table

		DownstreamByCounty set_output;
		set_output.initialize_field_set ();
		set_output.field_set->table_name = TABLE_NAME_DOWNSTREAM_BY_COUNTY;
		if (set_output.field_set->open_write (db, log)) {
			log += "Output table open [";
			log += set_output.field_set->table_name;
			log += "]\n";

			// Entries are from all 10 tables
			// Instead of opening all 10 input tables at once, traverse map 10 times and open each table in order

			for (river_index = 0;
			!error
			&& (river_index < RIVER_COUNT);
			++river_index) {
				// Keep set_input local so field_set doesn't have to be deleted for each area
				DownstreamByCounty set_input;

				set_input.initialize_field_set ();
				set_input.field_set->table_name = TABLE_NAME_DOWNSTREAM_BY_COUNTY;
				set_input.field_set->table_name += " Area";
				set_input.field_set->table_name += river_areas[river_index];

				view.update_progress (set_input.field_set->table_name, 1);

				if (set_input.field_set->open_read (db, log)) {
					log += "Input table open [";
					log += set_input.field_set->table_name;
					log += "]\n";

					if (set_input.field_set->move_first (log)) {
						do {

							if ((final_county = counties.find (set_input.County)) != counties.end ()) {
								if (final_county->second.first == river_numbers[river_index]) {
									// This area's version of county was closest
									set_output.County = set_input.County;
									set_output.Order = set_input.Order;
									set_output.CrossedCounty = set_input.CrossedCounty;
									set_output.Distancekm = set_input.Distancekm;
									if (set_output.field_set->write (log))
										++count_written;
									else
										error = true;
								}
							}
							else {
								error = true;
								log.add_formatted ("ERROR, input table county %ld not in final counties list.\n", set_input.County);
							}
						} while (set_input.field_set->move_next (log));
					}
					else
						error = true;

					set_input.field_set->close ();
				}
				else
					error = true;
			}

			set_output.field_set->close ();
		}

		db->close ();
		log.add_formatted ("County records written\t%ld\n", count_written);
	}
	else
		error = true;
		
	view.destroy_dialog_progress ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnListCountiesUpstream
	(wxCommandEvent &)

// 2017-09-20 Dave Keiser: List counties upstream from each 

{
	dynamic_string log, path_output, filename_county_shp, area_name, report;
	map_layer *layer_county, *layer_rivers, *layer_centers, *layer_vectors;
	std::set <int> state_set;
	map_object *center;
	bool error = false;
	long count_out_of_county = 0, count_ended_in_county = 0;
	std::map <long, std::vector <UpstreamCounties *>> counties;
	std::vector <map_object *>::const_iterator county_shape, county_center;
	UpstreamDownstream updown;
	bool write_agency_column = false; // true for STORET Legacy
	bool ponto = false; // BHO Pontodrenagem

	filename_county_shp = "r:\\Census\\TIGER 2012\\County\\tl_2012_us_county\\tl_2012_us_county.shp";
	path_output = "H:\\scratch\\CountyRoutes\\Counties Upstream.accdb";
	// area_name = "01";
	// area_name = "02";
	area_name = "03W";
	// area_name = "04";
	// area_name = "08";
	// area_name = "09";
	// "13";
	// "15";
	// "16";
	// "18";

	dialog_prompt dp (this, "Area Name", &area_name);
	if (dp.ShowModal () == wxID_OK) {

		interface_window_wx view;
		view.setup_wx (this);

		view.start_progress (this);

		layer_rivers = load_river (area_name, &state_set, &view, log);

		layer_county = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_county->name = "County Boundaries";
		layer_county->initialize_attributes = true;
		map_watershed->layers.push_back (layer_county);

		read_county_boundaries (filename_county_shp, map_watershed, layer_county, &state_set, &view, log);

		// Shapefile in "r:\Census\TIGER 2012\County\tl_2012_us_county\tl_2012_us_county.shp"
		// is an exact match by visual inspection with "R:\GIS\County Map\CensusBureau_2016_us_county_5m\cb_2016_us_county_5m.shp"

		layer_centers = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_centers->name = "County Centers";
		layer_centers->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
		layer_centers->initialize_attributes = true;
		map_watershed->layers.push_back (layer_centers);

		for (county_shape = layer_county->objects.begin ();
		county_shape != layer_county->objects.end ();
		++county_shape) {
			center = layer_centers->create_new (MAP_OBJECT_POINT);
			center->id = (*county_shape)->id;
			center->name = (*county_shape)->name;
			(*county_shape)->centroid (&center->latitude, &center->longitude);
			layer_centers->objects.push_back (center);
		}

		view.update_progress ("Linking Centroids");
		router_v2->link_points_threaded (layer_centers, app->multithread_count, layer_rivers, write_agency_column, ponto, &view, report, log);
		log += report;

		{
			// show line from county center to neareset comid point
			map_object_vector *arrow;
			map_object *segment;
			map_polygon *river_segment;

			layer_vectors = map_watershed->create_new (MAP_OBJECT_VECTOR);
			layer_vectors->name = "Vectors";
			layer_vectors->color = RGB (255, 0, 255);
			layer_vectors->line_width = 1;
			map_watershed->layers.push_back (layer_vectors);

			for (county_center = layer_centers->objects.begin ();
			county_center != layer_centers->objects.end ();
			++county_center) {
				arrow = (map_object_vector *) layer_vectors->create_new (MAP_OBJECT_VECTOR);
				arrow->id = (*county_center)->id;
				arrow->name = (*county_center)->name;
				arrow->latitude = (*county_center)->latitude;
				arrow->longitude = (*county_center)->longitude;

				if ((segment = layer_rivers->match_id ((*county_center)->attributes_numeric[USGS_ATTRIBUTES_INDEX_COMID])) != NULL) {
					if (segment->type == MAP_OBJECT_POLYGON) {
						river_segment = (map_polygon *) segment;
						arrow->destination_longitude = (long) river_segment->nodes[(int) (*county_center)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POINT_INDEX] * 2];
						arrow->destination_latitude = (long) river_segment->nodes[(int) (*county_center)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POINT_INDEX] * 2 + 1];
					}
					else {
						river_segment = &((map_polygon_complex *) segment)->polygons[(int) (*county_center)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POLYGON_INDEX]];
						arrow->destination_longitude = (long) river_segment->nodes[(int) (*county_center)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POINT_INDEX] * 2];
						arrow->destination_latitude = (long) river_segment->nodes[(int) (*county_center)->attributes_numeric[USGS_ATTRIBUTES_INDEX_POINT_INDEX] * 2 + 1];
					}
				}
				else
					log.add_formatted ("ERROR, arrow COMID %ld not found.\n", (*county_center)->attributes_numeric[USGS_ATTRIBUTES_INDEX_COMID]);

				layer_vectors->objects.push_back (arrow);
			}
		}

		updown.list_counties_upstream (router_v2, layer_centers, layer_county, layer_rivers, &counties,
		&count_out_of_county, &count_ended_in_county, &view, log);

		log.add_formatted ("Routing started outside of county\t%ld\n", count_out_of_county);
		log.add_formatted ("Routing never exited county\t%ld\n", count_ended_in_county);


		if (!error) {
			// Write results to database path_output
			write_upstream_counties (path_output, area_name, &counties, &view, log);

			// write results to log
			std::map <long, std::vector <UpstreamCounties *>>::iterator county;
			std::vector <UpstreamCounties *>::iterator upstream_county;

			for (county = counties.begin (); county != counties.end (); ++county) {
				log.add_formatted ("%ld", county->first);
				for (upstream_county = county->second.begin ();
				upstream_county != county->second.end ();
				++upstream_county)
					log.add_formatted ("\t%ld@%ld", (*upstream_county)->UpstreamCounty, (*upstream_county)->crossing_COMID);
				log += "\n";

				for (upstream_county = county->second.begin ();
				upstream_county != county->second.end ();
				++upstream_county)
					delete *upstream_county;
			}
		}

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnListCountiesUpstream");

		view.destroy_dialog_progress ();

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnCombineCountiesUpstream
	(wxCommandEvent &)

// 2017-09-28 Read tables of upstream counties
// When duplicate counties are found, retain the closest 

{
	odbc_database_access *db;
	odbc_database_credentials creds;
	bool error = false;
	const int RIVER_COUNT = 10;
	dynamic_string table_name, river_areas [RIVER_COUNT] = {"01", "02", "03W", "04", "08", "09", "13", "15", "16", "18" }, filename_output, log;
	int river_index, river_numbers[RIVER_COUNT] = { NHDAREA_01, NHDAREA_02, NHDAREA_03W, NHDAREA_04, NHDAREA_08, NHDAREA_09, NHDAREA_13, NHDAREA_15, NHDAREA_16, NHDAREA_18 };
	// FIPS, area_constant, distance
	std::map <long, double> area_counties;
	std::map <long, std::pair <int, double>> counties;
	std::map <long, std::pair <int, double>>::iterator final_county;
	long count_written = 0;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	// filename_output = "H:\\scratch\\CountyRoutes\\Counties Upstream.accdb";
	// filename_output = "H:\\scratch\\CountyRoutes\\Strahler All 4\\Counties Upstream Strahler All 4.accdb";
	// filename_output = "H:\\scratch\\CountyRoutes\\Strahler All 3\\Counties Upstream Strahler All 3.accdb";
	filename_output = "H:\\scratch\\CountyRoutes\\Strahler All 3\\Counties Upstream Strahler All 3.accdb";

	creds.set_database_filename (filename_output);
	creds.type = ODBC_ACCESS;
	creds.set_hostname (".");
	creds.set_driver_name (ODBC_ACCESS_DEFAULT_DRIVER_64);

	db = new odbc_database_access;
	if (db->open (&creds, &view, log)) {

		view.update_progress (L"Reading all entries", 0);
		for (river_index = 0; river_index < RIVER_COUNT; ++river_index) {
			table_name = TABLE_NAME_UPSTREAM_COUNTIES;
			table_name += " Area";
			table_name += river_areas[river_index];

			log += "[";
			log += table_name;
			log += "]\n";
			// Fill counties from this area, replacing any duplicates found closer than existing entries
			if (!read_upstream_by_county (db, river_numbers[river_index], table_name, &counties, &view, log))
				error = true;
		}

		view.update_progress (L"Writing best routes", 0);
		log.add_formatted ("Unique county map size\t%d\n", counties.size ());

		// copy routes from counties to all-US table

		UpstreamCounties set_output;
		set_output.initialize_field_set ();
		set_output.field_set->table_name = TABLE_NAME_UPSTREAM_COUNTIES;
		if (set_output.field_set->open_write (db, log)) {
			log += "Output table open [";
			log += set_output.field_set->table_name;
			log += "]\n";

			// Entries are from all 10 tables
			// Instead of opening all 10 input tables at once, traverse map 10 times and open each table in order

			for (river_index = 0;
			!error
			&& (river_index < RIVER_COUNT);
			++river_index) {
				// Keep set_input local so field_set doesn't have to be deleted for each area
				UpstreamCounties set_input;

				set_input.initialize_field_set ();
				set_input.field_set->table_name = TABLE_NAME_UPSTREAM_COUNTIES;
				set_input.field_set->table_name += " Area";
				set_input.field_set->table_name += river_areas[river_index];

				view.update_progress (set_input.field_set->table_name, 1);

				if (set_input.field_set->open_read (db, log)) {
					log += "Input table open [";
					log += set_input.field_set->table_name;
					log += "]\n";

					if (set_input.field_set->move_first (log)) {
						do {

							if ((final_county = counties.find (set_input.County)) != counties.end ()) {
								if (final_county->second.first == river_numbers[river_index]) {
									// This area's version of county was closest
									set_output.County = set_input.County;
									set_output.UpstreamCounty = set_input.UpstreamCounty;
									set_output.Distancekm = set_input.Distancekm;
									if (set_output.field_set->write (log))
										++count_written;
									else
										error = true;
								}
							}
							else {
								error = true;
								log.add_formatted ("ERROR, input table county %ld not in final counties list.\n", set_input.County);
							}
						} while (set_input.field_set->move_next (log));
					}
					else
						error = true;

					set_input.field_set->close ();
				}
				else
					error = true;
			}

			set_output.field_set->close ();
		}

		db->close ();
		log.add_formatted ("County records written\t%ld\n", count_written);
	}
	else
		error = true;
		
	view.destroy_dialog_progress ();

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnCountiesUpstreamStrahler
	(wxCommandEvent &)

// 2017-10-02 Find counties upstream using every COMID with StrahlerOrder >= 4

{
	dynamic_string log, path_output, filename_county_shp, area_name, report;
	map_layer *layer_county, *layer_rivers;
	std::set <int> state_set;
	int minimum_strahler_start, minimum_strahler_all;
	bool error = false;
	long count_out_of_county = 0, count_ended_in_county = 0;
	std::map <long, std::vector <UpstreamCounties *>> counties;
	std::vector <map_object *>::const_iterator county_shape;
	UpstreamDownstream updown;

	filename_county_shp = "r:\\Census\\TIGER 2012\\County\\tl_2012_us_county\\tl_2012_us_county.shp";
	// path_output = "H:\\scratch\\CountyRoutes\\Counties Upstream Strahler All 4.accdb";
	path_output = "R:\\DKEISER\\updown_counties\\Strahler All 3\\Counties Upstream Strahler All 3.accdb";
	minimum_strahler_start = 3;
	minimum_strahler_all = 3;
	area_name = "01";
	// area_name = "02";
	// area_name = "03W";
	// area_name = "04";
	// area_name = "08";
	// area_name = "09";
	// "13";
	// "15";
	// "16";
	// "18";

	dialog_prompt dp (this, "Area Name", &area_name);
	if (dp.ShowModal () == wxID_OK) {

		interface_window_wx view;
		view.setup_wx (this);

		log += "Output Database\t\"";
		log += path_output;
		log += "\"\n";

		layer_rivers = load_river (area_name, &state_set, &view, log);

		layer_county = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_county->name = "County Boundaries";
		layer_county->initialize_attributes = true;
		map_watershed->layers.push_back (layer_county);

		read_county_boundaries (filename_county_shp, map_watershed, layer_county, &state_set, &view, log);

		view.update_status_bar ("Multhread starting");

		{
			dialog_run_threads update_display (this, true, false);
			update_display.Show ();
			update_display.setup_interface_window (app, &view);

			updown.list_counties_upstream_strahler_multithread (minimum_strahler_start, minimum_strahler_all, router_v2, layer_county, layer_rivers, &counties,
			&count_out_of_county, &count_ended_in_county, &view, log);
		}

		view.update_status_bar ("Multhread complete");
		log.add_formatted ("Routing started outside of county\t%ld\n", count_out_of_county);
		log.add_formatted ("Routing never exited county\t%ld\n", count_ended_in_county);

		if (!error) {
			// Write results to database path_output
			write_upstream_counties (path_output, area_name, &counties, &view, log);
			view.update_status_bar ("Database written");

			// write results to log
			std::map <long, std::vector <UpstreamCounties *>>::iterator county;
			std::vector <UpstreamCounties *>::iterator upstream_county;

			for (county = counties.begin (); county != counties.end (); ++county) {

				view.update_status_bar_formatted ("County %ld", county->first);

				log.add_formatted ("%ld", county->first);
				for (upstream_county = county->second.begin ();
				upstream_county != county->second.end ();
				++upstream_county)
					log.add_formatted ("\t%ld@%ld", (*upstream_county)->UpstreamCounty, (*upstream_county)->crossing_COMID);
				log += "\n";
			}

			view.update_status_bar ("Log appended");
			for (county = counties.begin (); county != counties.end (); ++county) {
				for (upstream_county = county->second.begin ();
				upstream_county != county->second.end ();
				++upstream_county)
					delete *upstream_county;
			}
			view.update_status_bar ("Upstream counties deleted");
		}

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnListCountiesUpstream");

		view.update_status_bar ("Map Updated");

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnMapFishAdviseries
	(wxCommandEvent &)

// 2018-02-08 Plot Chuan's list of fish adviseries by 14-digit Reach identifier

// Area 02 should be open

// "H:\scratch\Chuan\fish_advisory_lakes.csv"

{
	dynamic_string log, filename_advisories, area_name, report;
	map_layer *layer_water_bodies, *layer_matches;
	map_object *match_point;
	std::vector <map_object *>::iterator lake;
	std::map <long, dynamic_string> advisories;
	std::map <long, dynamic_string>::iterator warning;
	long id;

	filename_advisories = "I:\\TDC\\FishAdvisories\\fish_advisory_lakes.csv";
	area_name = "02";

	interface_window_wx view;
	view.setup_wx (this);

	// read advisory file
	{
		FILE *f;
		char data[2048];
		dynamic_string token_text;
		std::vector <dynamic_string> tokens;

		// Header line
		// OBJECTID,PERMANENT_IDENTIFIER,EVENTDATE,REACHCODE,REACHSMDATE,REACHRESOLUTION,FEATURE_PERMANENT_IDENTIFIER,FEATURECLASSREF,SOURCE_ORIGINATOR,SOURCE_DATADESC,SOURCE_FEATUREID,FEATUREDETAILURL,EVENTTYPE,EVENT_AREASQKM,GEOGSTATE,CYCLE_YEAR,START_DATE,END_DATE,WBD_HUC12,WBD_HUC12_PERCENT,NHDREACH_CAC,VERTICES,SHAPE_Length,SHAPE_Area

		if ((fopen_s (&f, filename_advisories.get_text_ascii (), "r")) == 0) {

			if (fgets (data, 2048, f)) {
				while (fgets (data, 2048, f)) {
					token_text = data;
					token_text.tokenize (",\n", &tokens, true, false);
					if (tokens.size () == 24) {
						// ID is column 1
						id = atol (tokens[0].get_text_ascii ());
						// Reach is column 4
						advisories.insert (std::pair <long, dynamic_string> (id, tokens[3]));
					}
				}
			}
			fclose (f);
		}
		else
			log += "ERROR, can't read advisory file.\n";
	}

	log.add_formatted ("Advisories\t%d\n", advisories.size ());

	layer_water_bodies = map_watershed->match ("Water Bodies");

	layer_matches = map_watershed->create_new (MAP_OBJECT_POINT);
	layer_matches->name = "Fish Advisories";
	layer_matches->type = MAP_OBJECT_POINT;
	layer_matches->color = RGB (0, 0, 63);
	layer_matches->symbol = MAP_OBJECT_SYMBOL_PLUS;
	layer_matches->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_matches->draw_labels_id = true;
	map_watershed->layers.push_back (layer_matches);

	log += "\n";
	log += "NHD Lake ID\tREACHCODE\tAdvisory ID\n";

	for (warning = advisories.begin ();
	warning != advisories.end ();
	++warning) {
		view.update_progress_formatted (0, "Advisory %ld", warning->first);
		if (warning->second.get_length () > 0) {
			view.update_progress (warning->second, 1);
			for (lake = layer_water_bodies->objects.begin (); lake != layer_water_bodies->objects.end (); ++lake) {

				if ((*lake)->attributes_text[3] == warning->second) {
					log.add_formatted ("%ld", (*lake)->id);
					log += "\t";
					log += warning->second;
					log.add_formatted ("\t%ld", warning->first);
					log += "\n";

					match_point = layer_matches->create_new (MAP_OBJECT_POINT);
					match_point->id = warning->first;
					(*lake)->centroid (&match_point->latitude, &match_point->longitude);

					layer_matches->objects.push_back (match_point);
				}
			}
		}
	}

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("OnMapFishAdviseries");

	view.update_status_bar ("Map Updated");

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnConvertMultipoint
	(wxCommandEvent &)

// 2020-03-24 BHO layer geoft_bho_pontodrenagem.shp uses SHAPEFILE_FORM_MULTIPOINT and becomes Polygon during import
// Convert it to a point layer

{
	dynamic_string log;

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POLYGON)) {

		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		map_layer *layer_points = convert_multipoint (panel_watershed->layer_selection_source, map_watershed, &view, log);

		panel_watershed->change_layers ();

		panel_watershed->change_selection_layer (layer_points);

		enable_map_controls ();
		panel_watershed->redraw ("OnConvertMultipoint");
	}
	else
		log += "ERROR, no selected layer, or layer is not POLYGON type.\n";

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Convert Multipoint", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnImportBHO
	(wxCommandEvent &)

// 2020-02-14 Import an area of BHO river system

{
	dynamic_string log, path_bho_trecho, path_bho_ponto, path_bho_attributes, path_flow_network, bacia_string;
	bool error = false;
	int bacia;

	wxTextEntryDialog d_bacia (this, L"Bacia?", L"Import BHO", L"1", wxICON_QUESTION | wxOK | wxCANCEL);
	if (d_bacia.ShowModal () == wxID_OK) {
		bacia = atoi (d_bacia.GetValue().ToAscii ());
		interface_window_wx view;
		view.setup_wx (this);
		view.start_progress (this);

		delete router_v2;
		router_v2 = new router_BHO;

		path_bho_trecho = "F:\\scratch\\BHO\\geoft_bho_trechodrenagem\\geoft_bho_trechodrenagem.shp";
		path_bho_ponto = "F:\\scratch\\BHO\\geoft_bho_pontodrenagem\\geoft_bho_pontodrenagem\\geoft_bho_pontodrenagem.shp";

		path_bho_attributes.format ("I:\\tdc\\BHO\\Area %d\\Trech Attributes %d.bin", bacia, bacia);
		path_flow_network.format ("I:\\tdc\\BHO\\Area %d\\Trech %d.bin", bacia, bacia);

		if (((router_BHO *) router_v2)->import_trechodrenagem (path_bho_trecho, bacia, map_watershed, &view, log)
		&& ((router_BHO *) router_v2)->import_pontodrenagem (path_bho_ponto, map_watershed, &view, log)) {
			map_layer *layer_rivers = create_river_layer (map_watershed, true, true);
			router_v2->river_map_layer = layer_rivers;
			map_watershed->set_extent ();
			((router_BHO *)router_v2)->fill_rivers (layer_rivers, log);
			router_v2->read_flow_and_build_network (filename_nhd_area, &view, log);

			((router_BHO *)router_v2)->write_attributes (path_bho_attributes, layer_rivers, log);
			router_v2->rivers.write (path_flow_network, log);
		}
		else
			error = true;

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnImportBHO");

		view.update_status_bar ("Map Updated");

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnOpenBHOArea
	(wxCommandEvent &ev)

// 2020-02-19 Import BHO shapefile and saved binary network

{
	dynamic_string log, path_bho_segments, filename_network, path_bho_attributes;
	bool error = false;
	int bacia;

	/*
	1- Costa Atlântica Norte
	2- Bacia Amazônica
	3- Costa Atlântica da Ilha do Marajó
	4- Bacia do Tocantins – Araguaia
	5- Costa Atlântica Nordeste
	6- Bacia do São Francisco
	7- Costa Atlântica Leste
	8- Bacia do Prata
	9- Costa Atlântica Sul e Costa do Pacífico
	*/

	bacia = (ev.GetId () - MENU_BHO_1) + 1;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	delete router_v2;
	router_v2 = new router_BHO;

	path_bho_segments = "F:\\scratch\\BHO\\geoft_bho_trechodrenagem\\geoft_bho_trechodrenagem.shp";
	path_bho_attributes.format ("f:\\scratch\\BHO\\Area %d\\Trech Attributes %d.bin", bacia, bacia);
	filename_network.format ("F:\\scratch\\BHO\\Area %d\\Trech %d.bin", bacia, bacia);

	map_layer *layer_rivers = create_river_layer (map_watershed, true, true);
	router_v2->river_map_layer = layer_rivers;
	layer_rivers->column_names_numeric.push_back ("COBASIO"); // NHD_SEGMENT_ATTRIBUTE_INDEX_COBASIO
	if (((router_BHO *)router_v2)->read_trechodrenagem (path_bho_segments, bacia, map_watershed, layer_rivers, &view, log)) {
		map_watershed->set_extent ();
		((router_BHO *)router_v2)->read_attributes (path_bho_attributes, layer_rivers, log);
		router_v2->rivers.read (filename_network, log);
	}
	else
		error = true;

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	panel_watershed->change_selection_layer (layer_rivers);
	enable_map_controls ();
	panel_watershed->redraw ("OnOpenBHOArea");

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Read Flow Network", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnBHOReservoirs
(wxCommandEvent &ev)

// 2021-07-26 Import BHO reservoir points, link up and downstream to sampling stations

{
	dynamic_string log, filename_points, filename_stations, filename_output, report, path_bho_segments, filename_network, path_bho_attributes;
	bool error = false;
	std::set <int> text_column_indexes, skip_column_indexes;
	int id_column_index, lat_column_index, lon_column_index;
	map_layer *layer_lakes, *layer_stations, *layer_linked_lakes = NULL, *layer_linked_stations = NULL;
	Ledger preview_sheet;
	bool clip1000m;
	// bool write_station_names = false;
	// char output_delimiter = ',';
	bool ponto = false, preview_points_only = false; // true for BHO Pontodrenagem, where points are already on the network
	int bacia;
	char output_delimiter = ',';

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	delete router_v2;
	router_v2 = new router_BHO;

	// filename_points = "I:\\TDC\\BHO\\Received 2021-07-26\\Water_Reservoirs.csv";
	filename_points = "I:\\TDC\\BHO\\Received 2022-10-31\\Dams_NorthEast.csv";
	// filename_stations = "I:\\TDC\\BHO\\Received 2020-10-14\\Monitoring_satations_Lat_Long.csv";
	filename_stations = "I:\\TDC\\BHO\\Received 2022-10-31\\Monitoring_stations.csv";
	// filename_output = "f:\\scratch\\bho\\Reservoir-MonitoringStations.csv";
	filename_output = "f:\\scratch\\bho\\Dams-MonitoringStations.csv";
	clip1000m = false;
	preview_points_only = false;

	map_layer *layer_rivers = create_river_layer (map_watershed, true, true);
	router_v2->river_map_layer = layer_rivers;
	layer_rivers->column_names_numeric.push_back ("COBASIO"); // NHD_SEGMENT_ATTRIBUTE_INDEX_COBASIO
	path_bho_segments = "F:\\scratch\\BHO\\geoft_bho_trechodrenagem\\geoft_bho_trechodrenagem.shp";
	if (!preview_points_only) {
		for (bacia = 1; bacia <= 9; ++bacia) {
			if (((router_BHO *)router_v2)->read_trechodrenagem (path_bho_segments, bacia, map_watershed, layer_rivers, &view, log)) {
				path_bho_attributes.format ("f:\\scratch\\BHO\\Area %d\\Trech Attributes %d.bin", bacia, bacia);
				filename_network.format ("F:\\scratch\\BHO\\Area %d\\Trech %d.bin", bacia, bacia);
				((router_BHO *)router_v2)->read_attributes (path_bho_attributes, layer_rivers, log);
				router_v2->rivers.read (filename_network, log);
			}
			else
				error = true;
		}
		map_watershed->set_extent ();
	}

	// Reservoirs
	if (preview_position_file (filename_points, CSV_POINT_FORMAT_AUTOMATIC, "", 10, &preview_sheet, &id_column_index, &lat_column_index, &lon_column_index, &text_column_indexes, NULL, log)) {

		preview_sheet.dump (log);

		layer_lakes = read_point_layer (filename_points, -1, false, true, CSV_POINT_FORMAT_AUTOMATIC,
		&preview_sheet.column_names, &text_column_indexes, &skip_column_indexes, id_column_index, lat_column_index, lon_column_index, log);

		// process this layer into linked binary point layer 
		layer_linked_lakes = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_linked_lakes->copy (layer_lakes);
		layer_linked_lakes->name += " Linked";
		layer_linked_lakes->resize (LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC, layer_linked_lakes->attribute_count_text);
		map_watershed->layers.push_back (layer_linked_lakes);
		if (!preview_points_only)
			router_v2->link_points_threaded (layer_linked_lakes, app->multithread_count, layer_rivers, false, ponto, &view, report, log);
		else
			// Second point file won't read correctly if a first map_layer exists and extent has not been set.
			map_watershed->set_extent();
	}
	log += report;

	// Monitoring Stations
	if (preview_position_file (filename_stations, CSV_POINT_FORMAT_AUTOMATIC, "", 10, &preview_sheet, &id_column_index, &lat_column_index, &lon_column_index, &text_column_indexes, NULL, log)) {

		preview_sheet.dump (log);

		layer_stations = read_point_layer (filename_stations, -1, false, true, CSV_POINT_FORMAT_AUTOMATIC,
		&preview_sheet.column_names, &text_column_indexes, &skip_column_indexes, id_column_index, lat_column_index, lon_column_index, log);

		// process this layer into linked binary point layer 
		layer_linked_stations = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_linked_stations->copy (layer_stations);
		layer_linked_stations->name += " Linked";
		layer_linked_stations->resize (LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC, layer_linked_stations->attribute_count_text);
		layer_linked_stations->symbol = MAP_OBJECT_SYMBOL_PLUS;
		layer_linked_stations->color = RGB (255, 127, 0);
		map_watershed->layers.push_back (layer_linked_stations);
		if (!preview_points_only)
			router_v2->link_points_threaded (layer_linked_stations, app->multithread_count, layer_rivers, false, ponto, &view, report, log);
	}
	log += report;

	if (!preview_points_only)
		router_v2->route_facilities_threaded (app->multithread_count, layer_linked_lakes, layer_linked_stations, clip1000m, &view, &filename_output,
		output_delimiter, false, report, log);

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	// panel_watershed->change_selection_layer (layer_rivers);
	enable_map_controls ();
	panel_watershed->redraw ("OnOpenBHOArea");

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Read Flow Network", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnBHOReservoirMunicipality
	(wxCommandEvent &ev)

// 2021-07-26 Import BHO reservoir points, link up and downstream to centerpoints of municipalities

// 2022-10-14 Link Municipalites up & downstream to each other

{
	dynamic_string log, filename_reservoir, filename_muni, filename_output, report, path_bho_segments, filename_network, path_bho_attributes;
	bool error = false;
	std::set <int> text_column_indexes, skip_column_indexes;
	int id_column_index, lat_column_index, lon_column_index;
	map_layer *layer_lakes, *layer_muni, *layer_linked_lakes = NULL, *layer_linked_muni = NULL;
	Ledger preview_sheet;
	bool clip1000m, write_station_names = false;
	char output_delimiter = ',';
	bool ponto = false, muni_muni = false; // true for BHO Pontodrenagem, where points are already on the network
	int bacia;

	interface_window_wx view;
	view.setup_wx (this);
	view.start_progress (this);

	delete router_v2;
	router_v2 = new router_BHO;

	filename_reservoir = "I:\\TDC\\BHO\\Received 2021-07-26\\Water_Reservoirs.csv";
	// filename_muni = "F:\\scratch\\BHO\\Munic_shapefile\\BR_Municipios_2019.shp";
	// filename_output = "f:\\scratch\\bho\\Reservoir-Municipality.csv";
	filename_muni = "F:\\scratch\\BHO\\BR_Municipios_2021\\BR_Municipios_2021_WGS1984.shp";
	filename_output = "f:\\scratch\\bho\\Municipality-Municipality.csv";
	clip1000m = false;
	muni_muni = true;

	map_layer *layer_rivers = create_river_layer (map_watershed, true, true);
	router_v2->river_map_layer = layer_rivers;
	layer_rivers->column_names_numeric.push_back ("COBASIO"); // NHD_SEGMENT_ATTRIBUTE_INDEX_COBASIO
	path_bho_segments = "F:\\scratch\\BHO\\geoft_bho_trechodrenagem\\geoft_bho_trechodrenagem.shp";
	for (bacia = 1; bacia <= 9; ++bacia) {
		if (((router_BHO *)router_v2)->read_trechodrenagem (path_bho_segments, bacia, map_watershed, layer_rivers, &view, log)) {
			path_bho_attributes.format ("f:\\scratch\\BHO\\Area %d\\Trech Attributes %d.bin", bacia, bacia);
			filename_network.format ("F:\\scratch\\BHO\\Area %d\\Trech %d.bin", bacia, bacia);
			((router_BHO *)router_v2)->read_attributes (path_bho_attributes, layer_rivers, log);
			router_v2->rivers.read (filename_network, log);
		}
		else
			error = true;
	}
	map_watershed->set_extent ();

	// Reservoirs
	if (!muni_muni) {
		if (preview_position_file (filename_reservoir, CSV_POINT_FORMAT_AUTOMATIC, "", 10, &preview_sheet, &id_column_index, &lat_column_index, &lon_column_index, &text_column_indexes, NULL, log)) {

			preview_sheet.dump (log);

			layer_lakes = read_point_layer (filename_reservoir, -1, false, true, CSV_POINT_FORMAT_AUTOMATIC,
			&preview_sheet.column_names, &text_column_indexes, &skip_column_indexes, id_column_index, lat_column_index, lon_column_index, log);

			// process this layer into linked binary point layer 
			layer_linked_lakes = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_linked_lakes->copy (layer_lakes);
			layer_linked_lakes->name += " Linked";
			layer_linked_lakes->resize (LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC, layer_linked_lakes->attribute_count_text);
			map_watershed->layers.push_back (layer_linked_lakes);
			router_v2->link_points_threaded (layer_linked_lakes, app->multithread_count, layer_rivers, false, ponto, &view, report, log);
		}
		log += report;
	}

	// Municipalities
	{
		importer_shapefile importer;
		std::vector <map_object *>::iterator county;
		map_object *center;

		layer_muni = map_watershed->create_new (MAP_OBJECT_POLYGON);
		importer.filename_source = filename_muni;
		importer.id_field_name = "CD_MUN";
		importer.name_field_name = "NM_MUN";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		if (!importer.import (layer_muni, NULL, map_watershed, &view, log, NULL))
			error = true;
		else {
			map_watershed->layers.push_back (layer_muni);

			// process this layer into linked binary point layer 
			layer_linked_muni = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_linked_muni->name = "Municipalities Linked";
			layer_linked_muni->attribute_count_numeric = LINKED_POINT_ATTRIBUTE_COUNT_NUMERIC;
			layer_linked_muni->symbol = MAP_OBJECT_SYMBOL_PLUS;
			layer_linked_muni->color = RGB (255, 127, 0);

			for (county = layer_muni->objects.begin ();
			county != layer_muni->objects.end ();
			++county) {
				center = layer_linked_muni->create_new (MAP_OBJECT_POINT);
				center->id = (*county)->id;
				center->name = (*county)->name;
				(*county)->centroid (&center->latitude, &center->longitude);
				layer_linked_muni->objects.push_back (center);
			}

			map_watershed->layers.push_back (layer_linked_muni);
			router_v2->link_points_threaded (layer_linked_muni, app->multithread_count, layer_rivers, false, ponto, &view, report, log);
		}
	}
	log += report;

	if (muni_muni) 
		router_v2->route_facilities_threaded (app->multithread_count, layer_linked_muni, layer_linked_muni, clip1000m, &view, &filename_output, output_delimiter, write_station_names,
		report, log);
	else
		router_v2->route_facilities_threaded(app->multithread_count, layer_linked_lakes, layer_linked_muni, clip1000m, &view, &filename_output, output_delimiter, write_station_names,
		report, log);

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	// panel_watershed->change_selection_layer (layer_rivers);
	enable_map_controls ();
	panel_watershed->redraw ("OnOpenBHOArea");

	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"Read Flow Network", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnZillowNHD
	(wxCommandEvent &)

// 2020-11-18 Wendong: 10 shapefiles by year of Michigan Zillow Sales

{
	dynamic_string log, path_zillow;
	ZillowSales zillow;
	map_layer *layer_rivers, *layer_nhd_lakes;

	{
		interface_window_wx view;
		dialog_run_threads update_display (this, true, false);
		update_display.Show ();
		update_display.setup_interface_window (app, &view);

		view.update_progress ("Reading Shapefiles", 0);

		path_zillow = "F:\\scratch\\Michigan_EColi\\Michigan Housing Points by Year\\";
		zillow.load_zillow_sales (path_zillow,
		2015, 2017,
		false, map_watershed, &view, log);

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("Zillow NHD");

		view.update_status_bar ("Map Updated");

		if ((layer_rivers = map_watershed->match ("Rivers")) != NULL) {
			if ((layer_nhd_lakes = map_watershed->match ("Water Bodies")) != NULL) {

				zillow.thread_count = 28;
				// Table listing nearest distance from each sale to NHD river or Water body
				zillow.nearest_water_to_sale (layer_rivers, layer_nhd_lakes, &view, log);
			}
		}
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnZillowEcoli
	(wxCommandEvent &)

// 2020-11-19 Wendong: distance from home sale to outbreak (within 1 year after outbreak)

{
	dynamic_string log, path_zillow, filename_ecoli;
	ZillowSales zillow;

	{
		interface_window_wx view;
		dialog_run_threads update_display (this, true, false);
		update_display.Show ();
		update_display.setup_interface_window (app, &view);

		view.update_progress ("Reading Shapefiles", 0);

		// path_zillow = "F:\\scratch\\Michigan_EColi\\Michigan Housing Points by Year\\";
		// filename_ecoli = "F:\\scratch\\Michigan_EColi\\Michigan_EColi_2009_2018_Point.shp";

		path_zillow = "e:\\tdc\\Michigan_EColi\\Michigan Housing Points by Year\\";
		filename_ecoli = "e:\\tdc\\Michigan_EColi\\Michigan_EColi_2009_2018_Point.shp";

		zillow.load_EColi_outbreaks (filename_ecoli, map_watershed, -1, &view, log);
		zillow.load_zillow_sales (path_zillow, 2018, 2018, true, map_watershed, &view, log);

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("Zillow Sales - EColi");

		view.update_status_bar ("Map Updated");

		zillow.thread_count = 30;
		// Table listing nearest distance from each sale to NHD river or Water body
		zillow.nearest_outbreak (&view, log);
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnZillowEcoli5Mile
	(wxCommandEvent &)

// 2020-11-23 Wendong: List All home sales within 5 miles of each outbreak

{
	dynamic_string log, path_zillow, filename_ecoli, filename_output;
	ZillowSales zillow;
	int first_record = 1, last_record = -1, number_of_threads = 24;
	char delimiter = ',';

	dialog_project dj (this, &first_record, &last_record, &number_of_threads, NULL, NULL);
	if (dj.ShowModal () == wxID_OK) {
		interface_window_wx view;
		dialog_run_threads update_display (this, true, false);
		update_display.Show ();
		update_display.setup_interface_window (app, &view);

		view.update_progress ("Reading Shapefiles", 0);

		path_zillow = "F:\\scratch\\Michigan_EColi\\Michigan Housing Points by Year\\";
		filename_ecoli = "F:\\scratch\\Michigan_EColi\\Michigan_EColi_2009_2018_Point.shp";
		filename_output = "F:\\scratch\\Michigan_EColi\\EColi_5mile.csv";

		// path_zillow = "e:\\tdc\\Michigan_EColi\\Michigan Housing Points by Year\\";
		// filename_ecoli = "e:\\tdc\\Michigan_EColi\\Michigan_EColi_2009_2018_Point.shp";

		zillow.load_EColi_outbreaks (filename_ecoli, map_watershed, last_record, &view, log);
		zillow.load_zillow_sales (path_zillow,
		ZILLOW_FIRST_YEAR, ZILLOW_LAST_YEAR,
		// ZILLOW_FIRST_YEAR, ZILLOW_FIRST_YEAR + 1,
		true, map_watershed, &view, log);

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("Zillow Sales - EColi");

		view.update_status_bar ("Map Updated");

		zillow.thread_count = number_of_threads;
		// Table listing nearest distance from each sale to NHD river or Water body
		zillow.homes_in_5_miles (map_watershed, filename_output, delimiter, &view, log);
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnEColiNHD
	(wxCommandEvent &)

// 2020-11-23 Wendong: List nearest NHD water body to each outbreak

{
	dynamic_string log, filename_ecoli;
	ZillowSales zillow;
	map_layer *layer_rivers, *layer_nhd_lakes;

	{
		interface_window_wx view;
		dialog_run_threads update_display (this, true, false);
		update_display.Show ();
		update_display.setup_interface_window (app, &view);

		view.update_progress ("Reading Shapefiles", 0);

		filename_ecoli = "F:\\scratch\\Michigan_EColi\\Michigan_EColi_2009_2018_Point.shp";
		zillow.load_EColi_outbreaks (filename_ecoli, map_watershed, -1, &view, log);

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("Zillow NHD");

		view.update_status_bar ("Map Updated");

		if ((layer_rivers = map_watershed->match ("Rivers")) != NULL) {
			if ((layer_nhd_lakes = map_watershed->match ("Water Bodies")) != NULL) {

				zillow.thread_count = 8;
				// Table listing nearest distance from each sale to NHD river or Water body
				zillow.nearest_water_to_outbreak (layer_rivers, layer_nhd_lakes, &view, log);
			}
		}
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnZillow1000m
	(wxCommandEvent &)

// 2020-12-04 Wendong: For the 1000m buffer, could you help calculate 
// - the total number of outbreaks prior to sale.
// - distance in meters to the closest outbreak & outbreak ID & time
// - distance meters to most recent outbreak & outbreak ID & time
// - days from closest outbreak compared to house sale.

// 2020-12-07 Wendong: For each NHD water closest to house sale, list distance from that water to previous outbreaks within 1000m

{
	dynamic_string log, path_zillow, filename_ecoli, filename_output;
	ZillowSales zillow;
	map_layer *layer_rivers, *layer_nhd_lakes;
	int first_year = 2010, last_year;

	{
		interface_window_wx view;
		dialog_run_threads update_display (this, true, false);
		update_display.Show ();
		update_display.setup_interface_window (app, &view);

		dialog_prompt dp (this, "Enter Year", &first_year);
		if (dp.ShowModal () == wxID_OK) {
			last_year = first_year;
			if ((layer_rivers = map_watershed->match ("Rivers")) != NULL) {
				if ((layer_nhd_lakes = map_watershed->match ("Water Bodies")) != NULL) {
					view.update_progress ("Reading Shapefiles", 0);

					// ZILLOW_FIRST_YEAR, ZILLOW_LAST_YEAR,
					// ZILLOW_LAST_YEAR, ZILLOW_LAST_YEAR,
					path_zillow = app->project_root;
					path_zillow += "Michigan_EColi\\Michigan Housing Points by Year\\";
					filename_ecoli = app->project_root;
					filename_ecoli += "Michigan_EColi\\Michigan_EColi_2009_2018_Point.shp";
					filename_output = app->project_root;
					filename_output.add_formatted ("Michigan_EColi\\Zillow1000m_%d.txt", first_year);

					// path_zillow = "e:\\tdc\\Michigan_EColi\\Michigan Housing Points by Year\\";
					// filename_ecoli = "e:\\tdc\\Michigan_EColi\\Michigan_EColi_2009_2018_Point.shp";
					// filename_output.format ("e:\\tdc\\Michigan_EColi\\Zillow1000m_%d.txt", first_year);

					zillow.load_EColi_outbreaks (filename_ecoli, map_watershed, -1, &view, log);
					zillow.load_zillow_sales (path_zillow,
					first_year, last_year,
					true, map_watershed, &view, log);

					map_watershed->check_extent ();
					panel_watershed->change_layers ();
					enable_map_controls ();
					panel_watershed->redraw ("Zillow Sales - EColi");

					view.update_status_bar ("Map Updated");

					zillow.thread_count = 24;
					// Table listing nearest distance from each sale to NHD river or Water body
					zillow.outbreaks_in_1000m (layer_rivers, layer_nhd_lakes, map_watershed, filename_output, &view, log);
				}
			}
		}
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnFarmlandFinderFieldCircles
	(wxCommandEvent &)

// Creates a shapefile with circles centered on a FarmlandFinder sale location, with area = listed sale area.
// Can also create layers with circles extending 0.5 mile, 1.0 mile, or 3.0 miles beyond the listed sale area.

// 2021-01-07 Wendong: Create circles for each sale record from "I:\TDC\FarmlandFinder\All State Auction Results.xlsx"
// using assumed centerpoint and an area corresponding to Gross Acres

// 2021-01-26 Create circles for each real estate broker sale record in "I:\TDC\FarmlandFinder\All State Sold Listings.xlsx"

{
	dynamic_string log, path_shapefile, filename_shapefile, filename_county_master;
	std::map <dynamic_string, int> filenames_sales;
	FarmlandFinder farms;
	CountyMaster counties;
	map_layer *layer_circles = NULL, *layer_radius_half_mile = NULL, *layer_radius_mile = NULL, *layer_radius_3mile = NULL;
	bool write_shapefile, measure_and_correct, write_half_mile, write_mile, write_three_mile = false;

	path_shapefile = "f:\\FarmlandFinder\\";
	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb"; 

	/*
	// Farmland Finder data
	filename_shapefile = "FarmlandFinder Auctions.shp";
	filenames_sales.insert (std::pair <dynamic_string, int> ("I:\\TDC\\FarmlandFinder\\All State Auction Results.csv", FARMLANDFINDER_FORMAT_AUCTION_1));
	filenames_sales.insert (std::pair <dynamic_string, int> ("I:\\TDC\\FarmlandFinder\\2021 New Sales_Distances\\Auctions_022021_New_SinceDec2020.csv", FARMLANDFINDER_FORMAT_AUCTION_2));
	write_shapefile = measure_and_correct = false;
	write_half_mile = write_mile = true;
	*/

	/*
	// Real Estate Listings data
	filename_shapefile = "Real Estate Listings.shp";
	filenames_sales.insert (std::pair <dynamic_string, int>  ("I:\\TDC\\FarmlandFinder\\All State Sold Listings.csv", FARMLANDFINDER_FORMAT_LISTING_1));
	filenames_sales.insert (std::pair <dynamic_string, int>  ("I:\\TDC\\FarmlandFinder\\2021 New Sales_Distances\\Listings_022021_New_SinceDec2020.csv", FARMLANDFINDER_FORMAT_LISTING_2));
	write_shapefile = measure_and_correct = false;
	write_half_mile = write_mile = true;
	*/

	/*
	// 2021-08-27 Nieyan's Farmland Finder data
	filename_shapefile = "farm_variable.shp";
	filenames_sales.insert (std::pair <dynamic_string, int> ("I:\\TDC\\FarmlandFinder\\transmission lines\\farm_variable_distance\\farm_variable.csv", FARMLANDFINDER_FORMAT_NIEYAN));
	write_shapefile = true;
	measure_and_correct = true;
	write_half_mile = write_mile = true;
	*/

	/*
	// 2023-08-14
	path_shapefile = "H:\\NewYorkAgSales\\NYS_Ag_Sales_08102023 Circles\\";
	filename_shapefile = "NYS_Ag_Sales_08102023.shp";
	filenames_sales.insert(std::pair <dynamic_string, int>("I:\\TDC\\NewYorkAgSales\\NYS_Ag_Sales_08102023.csv", FARMLANDFINDER_FORMAT_NYS));
	write_shapefile = true;
	write_half_mile = write_mile = false;
	write_three_mile = true;
	measure_and_correct = true;
	*/

	// 2024-01-19
	path_shapefile = "F:\\FarmlandFinder\\2024-01-19\\";
	filename_shapefile = "ATTOMandFarmlandFinder_IAMNNE_V2.shp";
	filenames_sales.insert(std::pair <dynamic_string, int>("I:\\TDC\\FarmlandFinder\\2024-01-19\\ATTOMandFarmlandFinder_IAMNNE_V2.csv", FARMLANDFINDER_FORMAT_2024_01_19));
	write_shapefile = true;
	write_half_mile = write_mile = false;
	write_three_mile = false;
	measure_and_correct = true;

	interface_window_wx view;
	view.start_progress (this);

	counties.read (filename_county_master.get_text_ascii (), &view, log);

	view.update_progress ("Reading CSV", 0);
	if (farms.read_sales (&filenames_sales, &counties, !write_shapefile, -1, &view, log)) {
		layer_circles = farms.create_circles_area (map_watershed, measure_and_correct, &view, log);

		view.update_progress ("Circles Created.", 0);

		{
			// Check how many points have unique locations
			std::set <std::pair <long, long>> unique_locations;
			std::vector <map_object *>::const_iterator circle;
			long lat, lon;

			for (circle = layer_circles->objects.begin();
			circle != layer_circles->objects.end();
			++circle) {
				(*circle)->centroid (&lat, &lon);
				unique_locations.insert (std::pair <long, long> (lat, lon));
			}

			log.add_formatted ("%d unique locations for %d circles.\n", (int) unique_locations.size (), (int) layer_circles->objects.size ());
		}

		if (write_half_mile)
			layer_radius_half_mile = farms.create_circles_radius_beyond (0.5, map_watershed, measure_and_correct, log);
		if (write_mile)
			layer_radius_mile = farms.create_circles_radius_beyond(1.0, map_watershed, measure_and_correct, log);
		if (write_three_mile)
			layer_radius_3mile = farms.create_circles_radius_beyond(3.0, map_watershed, measure_and_correct, log);

		farms.dump (log);

		if (write_shapefile) {
			importer_shapefile shapefile;
			filename_struct pather;

			pather.parse (&filename_shapefile);
			pather.set_path (path_shapefile.get_text ());
			pather.write_file_specification (&shapefile.filename_source);

			shapefile.write_from_map_layer (layer_circles, NULL, true, &view, log);

			if (write_half_mile) {
				pather.set_filename ("FarmlandFinder plus-half-mile radius.shp");
				pather.write_file_specification (&shapefile.filename_source);

				shapefile.write_from_map_layer (layer_radius_half_mile, NULL, true, &view, log);
			}

			if (write_mile) {
				pather.set_filename("FarmlandFinder plus-mile radius.shp");
				pather.write_file_specification(&shapefile.filename_source);

				shapefile.write_from_map_layer(layer_radius_mile, NULL, true, &view, log);
			}

			if (write_three_mile) {
				pather.set_filename("FarmlandFinder plus-3-mile radius.shp");
				pather.write_file_specification(&shapefile.filename_source);

				shapefile.write_from_map_layer(layer_radius_3mile, NULL, true, &view, log);
			}
		}
	}

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("FarmlandFinder");

	view.update_status_bar ("Map Updated");

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnFarmlandFinderSSURGO
	(wxCommandEvent &)

// Read shapefile of circles for each FarmlandFinder sale.  Overlap with SSURGO
// Runs one state at a time, using multi-threading to run counties in parallel
// County results are written to file, one file per county, then re-read and appended to single state output
// 
// (raster) gSSURGO values by mapunit and county are matched to the (polygon) SSURGO polygons
//
// 2021-01-25 Add dominant component's representative slope

{
	dynamic_string log, filename_sales, filename_output, path_ssurgo, area_table;
	dynamic_string results;
	std::vector <dynamic_string> county_filenames;
	std::vector <dynamic_string>::iterator table;
	FarmlandFinder farms;
	map_layer *layer_farm_sales = NULL;
	importer_shapefile shapefile;
	std::map <dynamic_string, dynamic_string> farmlndcl_by_mukey;
	std::map <dynamic_string, std::tuple <double, double, double, double>> nccp_by_mukey;
	int run_state, retain_county_polygons = -1, first_county_fips = -1;
	IllinoisProductivityIndex_container *Illinois_PI = NULL;
	char delimiter = ',';
	bool auction_results, remove_county_files = true, filter_listing_ids = true;

	/*
	// filename_sales = "I:\\TDC\\FarmlandFinder\\Field Circles\\FarmlandFinder Auctions.shp";
	// filename_sales = "I:\\TDC\\FarmlandFinder\\Field Circles\\Real Estate Listings.shp";
	filename_sales = "F:\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020-5mile.shp";
	auction_results = true;
	if (auction_results)
		filename_output = "F:\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Auction.csv";
	else
		filename_output = "F:\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Listings.csv";
	run_state = 38;
	retain_county_polygons = -1;
	*/

	/*
	// 2023-08-14
	filename_sales = "H:\\NewYorkAgSales\\NYS_Ag_Sales_08102023 Circles\\NYS_Ag_Sales_08102023.shp";
	auction_results = false;
	filename_output = "h:\\NewYorkAgSales\\NYS_Ag_Sales_08102023_SSURGO_overlap.csv";
	run_state = 36;
	retain_county_polygons = 19;
	filter_listing_ids = false;
	*/

	// 2024-01-23
	filename_sales = "F:\\FarmlandFinder\\2024-01-19\\ATTOMandFarmlandFinder_IAMNNE_V2.shp";
	auction_results = false;
	filename_output = "F:\\FarmlandFinder\\2024-01-19\\ATTOMandFarmlandFinder_IAMNNE_V2_overlap.csv";
	run_state = 19;
	retain_county_polygons = 27; // 19, 27, 31
	filter_listing_ids = false;

	path_ssurgo = "e:\\SSURGO\\";
	// path_proximity_shapefiles = "I:\\TDC\\FarmlandFinder\\2021 New Sales_Distances\\Proximity_Shapefiles\\";

	// run_state = wxGetNumberFromUser (L"Run which state", L"State FIPS", L"FarmlandFinder", run_state, 1, 78, this);
	dialog_LandSalesSSURGO dl (this, &filename_sales, &filename_output, &run_state, &first_county_fips, &auction_results, &remove_county_files);
	if (dl.ShowModal () == wxID_OK) {

		interface_window_wx view;
		view.start_progress (this);

		layer_farm_sales = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_farm_sales->name = "Land Sales";
		layer_farm_sales->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_farm_sales->color = RGB (255, 0, 0);
		layer_farm_sales->include_in_legend = true;
		map_watershed->layers.push_back (layer_farm_sales);

		shapefile.filename_source = filename_sales;
		shapefile.id_field_name = "ID";
		shapefile.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		shapefile.normalize_longitude = false;
		shapefile.take_dbf_columns = true;

		farms.multithread_count = 26;

		if (filter_listing_ids) {
			// Filter to Auctions only (1-100,000) or Listings only (100,000+)
			shapefile_filter* sales_listings = new shapefile_filter;
			sales_listings->type = SHAPEFILE_FILTER_RANGE;
			sales_listings->field_number = 1; // ID
			if (auction_results) {
				sales_listings->range_ge = 1;
				sales_listings->range_lt = 99999;
			}
			else {
				sales_listings->range_ge = 100000;
				sales_listings->range_lt = 1000000;
			}
			shapefile.filters.push_back(sales_listings);
		}

		if (shapefile.import (layer_farm_sales, NULL, map_watershed, &view, log)) {

			if (run_state == 17) {
				Illinois_PI = new IllinoisProductivityIndex_container;
				Illinois_PI->read ("I:\\TDC\\FarmlandFinder\\Illinois Productivity Index.accdb", &view, log);
			}

			farms.count_states (layer_farm_sales, log);

			farms.read_gSSURGO_mapunit (path_ssurgo, run_state, &farmlndcl_by_mukey, NULL, &view, log);
			farms.read_gSSURGO_Valu1 (path_ssurgo, run_state, &nccp_by_mukey, &view, log);

			// overlay each polygon per soil in circle with highway line

			farms.overlay_ssurgo_state (&path_ssurgo, filename_output, run_state, first_county_fips, layer_farm_sales, map_watershed, retain_county_polygons,
			delimiter, area_table,
			&county_filenames, &farmlndcl_by_mukey, &nccp_by_mukey, Illinois_PI, &view, log);
			// farms.overlay_ssurgo (&path_ssurgo, layer_farm_sales, map_watershed, table, &view, log);


			if (run_state == 17)
				delete Illinois_PI;
		}

		farms.append_county_files (filename_output, &county_filenames, remove_county_files, log);
		/*
		{
			FILE *file_output;

			if (fopen_s (&file_output, filename_output.get_text_ascii (), "w") == 0) {
				fprintf (file_output, "State FIPS%cCounty FIPS%cSale%cSoil ID%cMukey%cMusym%cName%cName 1%cTexture%cSlope Description%cCSR%cComponent Name%cTaxonomic Class%cRepresentative Slope",
				delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
				fprintf(file_output, "%cgSSURGO frmlndcl", delimiter);
				fprintf (file_output, "%cnccpi3corn%cnccpi3soy%cnccpi3sg%cnccpi3all", delimiter, delimiter, delimiter, delimiter);
				if (run_state == 17)
					fprintf(file_output, "%cIllinois PI", delimiter);
				fprintf(file_output, "%c\"Exact Area, m^2\"\n", delimiter);
				for (table = clip_table.begin ();
				table != clip_table.end ();
				++table)
					fwrite (table->get_text_ascii (), table->get_length (), 1, file_output);
				clip_table.clear ();

				fprintf (file_output, "\nSale ID%cArea, m^2%cArea, acres\n", delimiter, delimiter);
				fwrite (area_table.get_text_ascii (), area_table.get_length(), 1, file_output);

				area_table.clear ();
				fclose (file_output);
			}
			else {
				log += "ERROR, can't open output filename \"";
				log += filename_output;
				log += "\".\n";
			}
		}
		*/

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("FarmlandFinder");

		view.destroy_dialog_progress ();

		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"File Open", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::OnFarmlandFinderSSURGOWeighted
	(wxCommandEvent&)

// 2023-03-20 Wendong: add area-weighted ssurgo calculations to output of OnFarmlandFinderSSURGO

{
	dynamic_string log, filename_input, filename_county_master, filename_output, path_ssurgo, area_table, report;
	dynamic_string results;
	std::vector <dynamic_string>::iterator table;
	FarmlandFinder farms;
	std::map <dynamic_string, dynamic_string> farmlndcl_by_mukey;
	char delimiter = ',', abbreviation [3];
	bool auction_results, write_soils, multithreaded;
	filename_struct file_parser;
	std::map <long, FarmlandFinderRepresentativeSoil> soil_sales_by_id;
	std::map <dynamic_string, std::tuple <double, double, double, double>> nccp_by_mukey;
	std::set <int> run_states;
	std::set <int>::iterator run_state;

	interface_window_wx view;
	dialog_run_threads* update_display = NULL;

	/*
	run_states.insert(17);
	run_states.insert(18);
	run_states.insert(19);
	run_states.insert(20);
	run_states.insert(26);
	run_states.insert(27);
	*/
	run_states.insert(29);
	/*
	run_states.insert(31);
	run_states.insert(38);
	// run_states.insert(39); // Ohio
	run_states.insert(46);
	run_states.insert(55);
	*/

	path_ssurgo = "e:\\SSURGO\\";

	write_soils = false;

	auction_results = true;
	multithreaded = false;

	if (multithreaded) {
		update_display = new dialog_run_threads(this, true, false);
		update_display->Show();
		update_display->setup_interface_window(app, &view);

		farms.multithread_count = 4;
		farms.weighted_area_ssurgo_threaded (& run_states, path_ssurgo, auction_results, write_soils, &view, log);

		update_display->Destroy();
		delete update_display;
	}
	else {
		view.setup_wx(this);
		view.start_progress(this);

		if (write_soils)
			report = "State\tSale\tMukey\tArea m^2\tFraction of Sale Area\tsandtotal_r\tsilttotal_r\tclaytotal_r\tslope_r\tIACSR\tnccpi3corn\tnccpi3soy\tnccpi3sg\tnccpi3all\n";
		else
			report = "State\tSale\tArea m^2\tSoil Count\tsandtotal_r\tsilttotal_r\tclaytotal_r\tslope_r\tIACSR\tnccpi3corn\tnccpi3soy\tnccpi3sg\tnccpi3all\n";

		for (run_state = run_states.begin ();
		run_state != run_states.end ();
		++run_state) {
			state_abbreviation(*run_state, abbreviation, 3);

			if (auction_results)
				filename_input = "F:\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Auction-";
			else 
				filename_input = "F:\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020-5mile-SSURGO-Listings-";

			filename_input += abbreviation;
			filename_input += ".csv";

			// Create filename_output from filename_input
			file_parser.parse (&filename_input);
			file_parser.set_path ("F:\\FarmlandFinder\\2023-03-20\\");
			filename_output = file_parser.prefix;
			filename_output += "-Weighted";
			file_parser.set_prefix (filename_output.get_text ());
			file_parser.write_file_specification (&filename_output);

			// farms.read_gSSURGO_mapunit(path_ssurgo, run_state, &farmlndcl_by_mukey, NULL, &view, log);
			farms.read_gSSURGO_Valu1(path_ssurgo, *run_state, &nccp_by_mukey, &view, log);

			view.update_progress_formatted (0, "Reading CSV State %d", *run_state);
			soil_sales_by_id.clear ();
			if (farms.read_previous_output (*run_state, filename_input, &soil_sales_by_id, -1, &view, log))
				farms.weighted_area_ssurgo (report, &soil_sales_by_id, *run_state, &path_ssurgo, &nccp_by_mukey, -1, write_soils, &view, log);
		}
		log += report;
		view.destroy_dialog_progress();
	}

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"File Open", L"", log);
	d.ShowWindowModal();
}

void frame_CARDGIS::On_LAGOS_NHD
	(wxCommandEvent &)

// 2021-01-13 Wendong: Tie LAGOS points to surrounding NHD water bodies for analysis

{
	dynamic_string log, filename_lagos_shapefile, filename_output, area_name;
	int area;
	std::vector <dynamic_string> area_names;
	map_layer *layer_nhd_lakes;
	LAGOS_Algal_Blooms lagos;

	filename_lagos_shapefile = "f:\\scratch\\LAGOS\\LAGOS_Lakes.shp";
	// filename_nhd_lake = "NHDSnapshot\\Hydrography\\NHDWaterbody.shp";
	filename_output = "f:\\scratch\\LAGOS\\LAGOS_NHD.shp";

	interface_window_wx view;
	view.start_progress (this);

	if (lagos.read_shapefile (filename_lagos_shapefile, map_watershed, &view, log)) {

		for (area = MENU_AREA_1; area <= MENU_AREA_18; area++) {
			if (true) {
			// if (area == MENU_AREA_7) {
				area_name = NHD_AREA_NAME [area - MENU_AREA_1];
				area_names.push_back (area_name);
			}
		}

		layer_nhd_lakes = read_nhd_water_bodies (app->filename_nhd_root, &area_names, map_watershed, &view, log);
		lagos.make_output_layer (map_watershed, layer_nhd_lakes);
		lagos.match_lakes (layer_nhd_lakes, &view, log);
		lagos.write_output_layer (filename_output, &view, log);
	}

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("LAGOS");

	view.update_status_bar ("Map Updated");

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnFarmlandFinderProximity
	(wxCommandEvent &)

// 2021-02-22 Wendong: Shapefiles for roads & features
// produce table of distance & id of closest, also NHD streams & lakes

{
	dynamic_string log, path_shapefile, filename_shapefile, filename_county_master, filename_output, lake;
	std::vector <dynamic_string> area_names;
	std::map <dynamic_string, int> filenames_sales;
	FarmlandFinder farms;
	CountyMaster counties;
	map_layer *layer_rivers = NULL, *layer_lakes = NULL;
	importer_shapefile shapefile;
	bool cleanup_after = true;

	path_shapefile = "f:\\FarmlandFinder\\";
	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb"; 

	interface_window_wx view;
	dialog_run_threads *update_display = NULL;

	update_display = new dialog_run_threads (this, true, false);
	update_display->Show ();
	update_display->setup_interface_window (app, &view);

	/*
	// Farmland Finder data
	filename_shapefile = "FarmlandFinder Auctions.shp";
	// filenames_sales.insert (std::pair <dynamic_string, int> ("I:\\TDC\\FarmlandFinder\\All State Auction Results.csv", FARMLANDFINDER_FORMAT_AUCTION_1));
	// filenames_sales.insert(std::pair <dynamic_string, int>("I:\\TDC\\FarmlandFinder\\2021 New Sales_Distances\\Auctions_022021_New_SinceDec2020.csv", FARMLANDFINDER_FORMAT_AUCTION_2));
	filenames_sales.insert(std::pair <dynamic_string, int>("I:\\TDC\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020.csv", FARMLANDFINDER_FORMAT_REDUCED));
	// filename_output = "f:\\FarmlandFinder\\2021-02-19\\FarmlandFinder Proximity.txt";
	filename_output = "f:\\FarmlandFinder\\2023-03-15\\FarmlandFinder Proximity.txt";
	*/

	/*
	// Real Estate Listings data
	filename_shapefile = "Real Estate Listings.shp";
	filenames_sales.insert (std::pair <dynamic_string, int>  ("I:\\TDC\\FarmlandFinder\\All State Sold Listings.csv", FARMLANDFINDER_FORMAT_LISTING_1));
	filenames_sales.insert (std::pair <dynamic_string, int>  ("I:\\TDC\\FarmlandFinder\\2021 New Sales_Distances\\Listings_022021_New_SinceDec2020.csv", FARMLANDFINDER_FORMAT_LISTING_2));
	filename_output = "f:\\FarmlandFinder\\2021-02-19\\Real Estate Proximity.txt";
	*/

	filename_shapefile = "FarmlandFinder Auctions.shp";
	filenames_sales.insert(std::pair <dynamic_string, int>("H:\\NewYorkAgSales\\NYS_Ag_Sales_08102023.csv", FARMLANDFINDER_FORMAT_NYS));
	// filename_output = "f:\\FarmlandFinder\\2021-02-19\\FarmlandFinder Proximity.txt";
	filename_output = "h:\\NewYorkAgSales\\Urban Proximity.txt";

	counties.read (filename_county_master.get_text_ascii (), &view, log);

	// Company,C,254 | State,C,254 | PADD,N,10,0 | Period,C,254	Cap_Mmgal,C,254	Source,C,254 | Longitude,N,19,11 | Latitude,N,19,11
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\Biodiesel_Plants_US_20190101.shp", "Biodiesel", "", "Company", map_watershed, &view, log);

	// Company,C,254 | Site_Name,C,254 | State,C,254 | PADD,N,10,0 | Data_Perio,C,254 | Capacity,N,10,0 | Source,C,254 | Longitude,N,19,11 | Latitude,N,19,11
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\Ethanol_Plants_US_20190101.shp", "Ethanol Plants", "", "Company", map_watershed, &view, log);

	// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\County_Roads.shp", "County Roads", "OBJECTID", "LINEARID", map_watershed, &view, log);
	// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\State_Highways.shp", "State Highways", "OBJECTID", "LINEARID", map_watershed, &view, log);
	// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\Interstate_Highways.shp", "Interstate Highways", "OBJECTID", "LINEARID", map_watershed, &view, log);
	// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\Midwest_All_Roads.shp", "All Roads", "OBJECTID", "LINEARID", map_watershed, &view, log);

	// UACE10,C,5| GEOID10,C,5 | NAME10,C,100 | NAMELSAD10,C,100 | LSAD10,C,2 | MTFCC10,C,5 | UATYP10,C,1 | FUNCSTAT10,C,1 | ALAND10,N,14,0 | AWATER10,N,14,0 | INTPTLAT10,C,11 | INTPTLON10,C,12
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\tl_2019_us_uac10.shp", "Urbanized Areas", "GEOID10", "NAME10", map_watershed, &view, log);
	farms.divide_urban_areas ("Urbanized Areas", map_watershed, log);

	// TYPEPIPE,C,10 | Operator,C,50 | Status,C,50 | Shape_Leng,N,19,11
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\NaturalGas_Pipelines_US_202001.shp", "Natural Gas Pipelines", "", "", map_watershed, &view, log);

	// LINEARID,C,22 | FULLNAME,C,100 | MTFCC,C,5 | OBJECTID,N,10,0
	farms.read_proximity_shapefile ("F:\\FarmlandFinder\\Proximity_Shapefiles\\Railroads.shp", "Railroads", "OBJECTID", "LINEARID", map_watershed, &view, log);

	area_names.push_back ("04"); // Michigan
	area_names.push_back ("05"); // Ohio
	area_names.push_back ("07"); // UMRB
	area_names.push_back ("08"); // LMRB
	area_names.push_back ("09"); // North Dakota
	area_names.push_back ("10U"); // Missouri Upper
	area_names.push_back ("10L"); // Missouri Lower
	area_names.push_back ("11"); // Arkansas

/*
	layer_lakes = read_nhd_water_bodies(app->filename_nhd_root, &area_names, map_watershed, &view, log);
		farms.proximity_layers.push_back (layer_lakes);

	if ((layer_rivers = read_nhd_area_rivers (&area_names, false, &view, log)) != NULL)
		farms.proximity_layers.push_back (layer_rivers);
*/

	view.update_progress ("Reading CSV", 0);
	if (farms.read_sales (&filenames_sales, &counties, false, -1, &view, log)) {
		if (!cleanup_after)
			farms.create_point_layer (map_watershed, log);
		farms.write_proximity_table (filename_output, &view, log);
	}

	if (cleanup_after) {
		std::vector <map_layer *>::iterator layer;

		for (layer = farms.proximity_layers.begin ();
		layer != farms.proximity_layers.end ();
		++layer)
			(*layer)->clear_objects ();
	}

	update_display->Destroy ();

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("FarmlandFinder");

	view.update_status_bar ("Map Updated");

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

map_layer *frame_CARDGIS::read_nhd_area_rivers
	(std::vector <dynamic_string> *area_names,
	const bool read_network_bin,
	interface_window *view,
	dynamic_string &log)

// Load NHD rivers for specified areas

{
	bool error = false;
	std::vector <dynamic_string>::iterator area_name;

	// NHD Rivers
	importer_shapefile importer;
	dynamic_string filename_nhd_rivers, filename_network;

	map_layer *layer_rivers = create_river_layer (map_watershed, false, false);
	router_v2->river_map_layer = layer_rivers;

	for (area_name = area_names->begin (); area_name != area_names->end (); ++area_name) {
		filename_nhd_rivers = app->filename_nhd_root;
		filename_nhd_rivers += "NHDPlus";
		filename_nhd_rivers += *area_name;
		filename_nhd_rivers += "\\";
		filename_network = filename_nhd_rivers + "Network.bin";

		// Two columns will be read from this shapefile
		// layer_rivers->column_names_numeric.push_back ("FTYPE");
		// layer_rivers->column_names_numeric.push_back ("FTYPE");
		// layer_rivers->column_names_numeric.push_back ("Lengthkm");
		router_v2->setup_shapefile (filename_nhd_rivers, layer_rivers, &importer);
		router_v2->read_shapefile (&importer, layer_rivers, map_watershed, view, log, NULL);

		// read divergence, other codes from PlusFlowlineVAA
		layer_rivers->column_names_numeric.push_back ("Divergence");
		layer_rivers->column_names_numeric.push_back ("LevelPath");
		layer_rivers->column_names_numeric.push_back ("StreamLevel");
		layer_rivers->column_names_numeric.push_back ("StrahlerOrder");
		layer_rivers->column_names_numeric.push_back ("UpHydro");
		layer_rivers->column_names_numeric.push_back ("DownHydro");
		layer_rivers->column_names_numeric.push_back ("ElevMinm");
		layer_rivers->column_names_numeric.push_back ("ElevMaxm");
		if (!router_v2->read_flow_VAA (filename_nhd_rivers, view, log))
			error = true;

		if (read_network_bin) {
			// network.bin
			if (!router_v2->rivers.read (filename_network, log))
				error = true;
		}
	}

	if (!error)
		return layer_rivers;
	else
		return NULL;
}

void frame_CARDGIS::OnShowSelectedItems
	(wxCommandEvent &)

// 2021-02-23 Check OnFarmlandFinderProximity by highlighting selected items from multiple layers

{
	dynamic_string log, layer_name;
	map_layer *layer_items = NULL, *layer_source = NULL;
	std::map <dynamic_string, long> ids_by_layer;
	std::map <dynamic_string, long>::iterator target;
	map_object *object, *center_copy;

	ids_by_layer.insert (std::pair <dynamic_string, long> ("Sales", 681));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("Biodiesel", 43));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("Ethanol Plants", 45));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("County Roads", 13382701));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("State Highways", 5506166));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("Interstate Highways", 5939024));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("All Roads", 13382701));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("Natural Gas Pipelines", 29688));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("Railroads", 58707));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("NHD Water Bodies", 13803930));
	ids_by_layer.insert (std::pair <dynamic_string, long> ("Rivers", 13804400));

	// New layer to get copies of selected items
	if ((layer_items = map_watershed->match ("CrossLayer")) == NULL) {
		layer_items = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_items->name = "CrossLayer";
		layer_items->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		layer_items->color = RGB (255, 0, 0);
		layer_items->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
		map_watershed->layers.push_back (layer_items);
	}
	else
		layer_items->clear_objects ();

	panel_watershed->change_layers ();
	panel_watershed->change_selection_layer (layer_items);

	for (target = ids_by_layer.begin ();
	target != ids_by_layer.end ();
	++target) {
		layer_name = target->first;
		if ((layer_source = map_watershed->match (layer_name.get_text_ascii ())) != NULL) {
			if ((object = layer_source->match_id (target->second)) != NULL) {
				center_copy = layer_items->create_new (MAP_OBJECT_POINT);
				center_copy->id = object->id;
				center_copy->name = object->name;
				object->centroid (&center_copy->latitude, &center_copy->longitude);
				layer_items->objects.push_back (center_copy);
				panel_watershed->add_to_selection (center_copy->id, log);
			}
			else {
				log.add_formatted ("ERROR, object %ld not found in layer \"", target->second);
				log += target->first;
				log += "\"\n";
			}
		}
		else {
			log += "ERROR, no layer \"";
			log += target->first;
			log += "\"\n";
		}
	}

	map_watershed->check_extent ();
	enable_map_controls ();
	panel_watershed->redraw ("OnShowSelectedItems");

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::On_LAGOS_Zillow
	(wxCommandEvent &)

// 2021-04-12 Wendong: Tie LAGOS chla data to Zillow housing

{
	dynamic_string log, path_lagos, path_farmlandfinder, filename_lagos_shapefile, area_name, filename_lagos_chla, filename_zillow_csv, filename_county_master;
	int area;
	map_layer *layer_rivers, *layer_nhd_lakes;
	std::vector <dynamic_string> area_names;
	LAGOS_Algal_Blooms lagos;
	ZillowSales zillow;
	MapLayerProximity proximator;
	CountyMaster counties;
	int number_of_threads = 20, first_sales_record, last_sales_record;
	std::chrono::system_clock::time_point start_time, end_time;
	std::chrono::duration <double, std::milli> elapsed_milliseconds;
	int within_check = PROXIMITY_WITHIN_POLYGON_DONT_DETECT;

	first_sales_record = 2900001;
	last_sales_record = 3000000;
	path_lagos = "F:\\scratch\\Lagos\\"; // app->project_root; // "f:\\scratch\\LAGOS\\";

	path_farmlandfinder = "f:\\FarmlandFinder\\Proximity_Shapefiles\\";

	// filename_nhd_lake = "NHDSnapshot\\Hydrography\\NHDWaterbody.shp";
	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb"; 
	filename_lagos_shapefile = path_lagos + "LAGOS_Lakes.shp";
	filename_lagos_chla = path_lagos + "Zillow-LAGOS\\LAGOS.csv";
	filename_zillow_csv = path_lagos + "Zillow-LAGOS\\append_all_10miles.csv";
	proximator.output_filename = path_lagos;
	proximator.delimiter = ',';

	dialog_project dj (this, &first_sales_record, &last_sales_record, &number_of_threads, NULL, NULL, "FarmlandFinder Proximity Root", &path_farmlandfinder);
	if (dj.ShowModal () == wxID_OK) {

		interface_window_wx view;
		view.start_progress (this);

		start_time = std::chrono::system_clock::now ();

		proximator.output_filename.add_formatted ("Zillow-LAGOS\\2022-07-14\\Zillow-Lagos %07d-%07d.csv", first_sales_record, last_sales_record);

		counties.read (filename_county_master.get_text_ascii (), &view, log);

		// create LAGOS_Algal_Blooms.layer_lagos from shapefile
		if (lagos.read_shapefile (filename_lagos_shapefile, map_watershed, &view, log)) {

			// Fill LAGOS_Algal_Blooms.chla_by_lake with reading times & values from CSV file
			lagos.read_chla (filename_lagos_chla, &view, log);

			// lagos.dump_chla (log);
			/*
			{
				std::map <long, std::vector <LAGOS_chla_reading>>::iterator lake_vector;
				Timestamp test_date;
				test_date.set (2000, 6, 28, 0, 0, 0);

				if ((lake_vector = lagos.chla_by_lake.find (96015)) != lagos.chla_by_lake.end ()) {
					lagos.average_across_months (&lake_vector->second, &test_date, 12);
				}
			}
			*/

			// Create a point map layer of sales from Zillow data
			zillow.read_csv (filename_zillow_csv, first_sales_record, last_sales_record, &counties, map_watershed, &view, log);

			// zillow.layer_zillow[0]->objects.resize (500);

			lagos.compute_chla_averages (zillow.layer_zillow[0], &view, log);

			// Company,C,254 | State,C,254 | PADD,N,10,0 | Period,C,254	Cap_Mmgal,C,254	Source,C,254 | Longitude,N,19,11 | Latitude,N,19,11
			proximator.read_proximity_shapefile (path_farmlandfinder, "Biodiesel_Plants_US_20190101.shp", "Biodiesel", "", "Company", map_watershed, &view, log);
			// Company,C,254 | Site_Name,C,254 | State,C,254 | PADD,N,10,0 | Data_Perio,C,254 | Capacity,N,10,0 | Source,C,254 | Longitude,N,19,11 | Latitude,N,19,11
			proximator.read_proximity_shapefile (path_farmlandfinder, "Ethanol_Plants_US_20190101.shp", "Ethanol Plants", "", "Company", map_watershed, &view, log);

			// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "County_Roads.shp", "County Roads", "OBJECTID", "LINEARID", map_watershed, &view, log);

			// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "State_Highways.shp", "State Highways", "OBJECTID", "LINEARID", map_watershed, &view, log);

			// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "Interstate_Highways.shp", "Interstate Highways", "OBJECTID", "LINEARID", map_watershed, &view, log);

			// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "Midwest_All_Roads.shp", "All Roads", "OBJECTID", "LINEARID", map_watershed, &view, log);

			// UACE10,C,5| GEOID10,C,5 | NAME10,C,100 | NAMELSAD10,C,100 | LSAD10,C,2 | MTFCC10,C,5 | UATYP10,C,1 | FUNCSTAT10,C,1 | ALAND10,N,14,0 | AWATER10,N,14,0 | INTPTLAT10,C,11 | INTPTLON10,C,12
			proximator.read_proximity_shapefile (path_farmlandfinder, "tl_2019_us_uac10.shp", "Urbanized Areas", "GEOID10", "NAME10", map_watershed, &view, log);
			proximator.divide_urban_areas ("Urbanized Areas", map_watershed, log);
			// TYPEPIPE,C,10 | Operator,C,50 | Status,C,50 | Shape_Leng,N,19,11
			proximator.read_proximity_shapefile (path_farmlandfinder, "NaturalGas_Pipelines_US_202001.shp", "Natural Gas Pipelines", "", "", map_watershed, &view, log);
			// LINEARID,C,22 | FULLNAME,C,100 | MTFCC,C,5 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "Railroads.shp", "Railroads", "OBJECTID", "LINEARID", map_watershed, &view, log);

			for (area = MENU_AREA_1; area <= MENU_AREA_18; area++) {
				if (true) {
					// if (area == MENU_AREA_7) {
					// path_nhd_lake = app->filename_nhd_root;
					area_name = NHD_AREA_NAME [area - MENU_AREA_1];
					area_names.push_back (area_name);
				}
			}
			if ((layer_rivers = read_nhd_area_rivers (&area_names, false, &view, log)) != NULL) {
				ProximityLayer proxy;
				proxy.layer = layer_rivers;
				proxy.write_layer_point = nhd_river_callback;
				proxy.write_column_headers = nhd_river_column_headers_callback;
				proximator.proximity_layers.push_back (proxy);
			}

			if ((layer_nhd_lakes = read_nhd_water_bodies (app->filename_nhd_root, &area_names, map_watershed, &view, log)) != NULL) {
				ProximityLayer proxy;
				proxy.layer = layer_nhd_lakes;
				proxy.write_layer_point = nhd_lake_callback;
				proxy.write_column_headers = nhd_lake_column_headers_callback;
				proximator.proximity_layers.push_back (proxy);
			}

			// lagos.make_output_layer (map_watershed);

			proximator.base_layer = zillow.layer_zillow[0];
			proximator.write_point_function = &zillow_point_callback;
			if (proximator.delimiter == '\t') {
				proximator.base_layer_column_headers = "ID\tRow ID\tImportParc";
				// proximator.base_layer_column_headers = "\tProperty Full Street Address\tSales Date\tState FIPS\tCounty FIPS\tSales Price Amount\tLot Size ft ^ 2\tlatitude\tlongitude";
				proximator.base_layer_column_headers += "\tNearest LAGOS ID\tdistance, m\t9 month chla average\t12 month chla average";
			}
			else {
				proximator.base_layer_column_headers = "\"ID\",\"Row ID\",\"ImportParc\"";
				// proximator.base_layer_column_headres += ",\"Property Full Street Address\",\"Sales Date\",\"State FIPS\",\"County FIPS\",\"Sales Price Amount\",\"Lot Size ft^2\",\"latitude\",\"longitude\"";
				proximator.base_layer_column_headers += ",\"Nearest LAGOS ID\",\"distance, m\",\"9 month chla average\",\"12 month chla average\"";
			}
			proximator.thread_count = number_of_threads;
			proximator.write_proximity_table (within_check, -1, &view, log);
		}
		end_time = std::chrono::system_clock::now ();
		elapsed_milliseconds = end_time - start_time;
		log.add_formatted ("Time Elapsed\t%.3lf\tseconds\n", (double) elapsed_milliseconds.count () / 1000.0);

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("LAGOS");

		view.update_status_bar ("Map Updated");
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::On_Pipeline_Zillow
	(wxCommandEvent &)

// 2021-04-19 Wendong: Tie Pipeline accident data to Zillow housing, roads, etc

{
	dynamic_string log, area_name, path_pipeline, filename_county_master, path_zillow_csv, path_farmlandfinder;
	int area;
	map_layer *layer_rivers, *layer_nhd_lakes;
	std::vector <dynamic_string> area_names;
	ZillowSales zillow;
	MapLayerProximity proximator;
	CountyMaster counties;
	PipelineAccidents pipes;
	int number_of_threads = 20, first_sales_record, last_sales_record;
	std::chrono::system_clock::time_point start_time, end_time;
	std::chrono::duration <double, std::milli> elapsed_milliseconds;
	int state_fips = 2;
	double cutoff_radius_miles;
	int within_check = PROXIMITY_WITHIN_POLYGON_DONT_DETECT;

	first_sales_record = 1;
	last_sales_record = 1000;

	path_pipeline = app->project_root; // F:\scratch\Pipelines
	
	cutoff_radius_miles = 8.0;
	// filename_nhd_lake = "NHDSnapshot\\Hydrography\\NHDWaterbody.shp";
	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb"; 
	path_zillow_csv = path_pipeline + "housing_transactions\\";
	// filename_zillow_csv = "F:\\scratch\\Pipelines\\ZillowTest.csv";
	proximator.output_filename = path_pipeline;
	proximator.delimiter = ',';
	path_farmlandfinder = "f:\\FarmlandFinder\\Proximity_Shapefiles";

	dialog_project dj (this, &first_sales_record, &last_sales_record, &number_of_threads, NULL, NULL, "FarmlandFinder Proximity Root", &path_farmlandfinder);
	if (dj.ShowModal () == wxID_OK) {

		interface_window_wx view;
		view.start_progress (this);

		start_time = std::chrono::system_clock::now ();

		proximator.output_filename.add_formatted ("Pipeline Proximity State %02d %07d-%07d.csv", state_fips, first_sales_record, last_sales_record);

		counties.read (filename_county_master.get_text_ascii (), &view, log);

		// create LAGOS_Algal_Blooms.layer_lagos from shapefile
		if (pipes.read_csv (path_pipeline, -1, map_watershed, &view, log)) {

			{
				ProximityLayer proxy;
				proxy.layer = pipes.gas_distribution;
				proxy.write_layer_point = pipeline_callback;
				proxy.write_column_headers = gas_distribution_column_header_callback;
				proximator.proximity_layers.push_back (proxy);
			}
			{
				ProximityLayer proxy;
				proxy.layer = pipes.gas_gathering;
				proxy.write_layer_point = pipeline_callback;
				proxy.write_column_headers = gas_gathering_column_header_callback;
				proximator.proximity_layers.push_back (proxy);
			}
			{
				ProximityLayer proxy;
				proxy.layer = pipes.hazardous_liquid;
				proxy.write_layer_point = pipeline_callback;
				proxy.write_column_headers = hazardous_liquid_column_header_callback;
				proximator.proximity_layers.push_back (proxy);
			}
			{
				ProximityLayer proxy;
				proxy.layer = pipes.liquified_natural_gas;
				proxy.write_layer_point = pipeline_callback;
				proxy.write_column_headers = liquified_natural_gas_column_header_callback;
				proximator.proximity_layers.push_back (proxy);
			}

			// Create a point map layer of sales from Zillow data
			zillow.read_csv_format_3 (path_zillow_csv, state_fips, first_sales_record, last_sales_record, &counties, map_watershed, &view, log);
			// Count pipeline incidents within 10 miles
			pipes.count_within_radius (zillow.layer_zillow[0], cutoff_radius_miles, -1, 16, &view, log);
			// Company,C,254 | State,C,254 | PADD,N,10,0 | Period,C,254	Cap_Mmgal,C,254	Source,C,254 | Longitude,N,19,11 | Latitude,N,19,11
			proximator.read_proximity_shapefile (path_farmlandfinder, "Biodiesel_Plants_US_20190101.shp", "Biodiesel", "", "Company", map_watershed, &view, log);
			// Company,C,254 | Site_Name,C,254 | State,C,254 | PADD,N,10,0 | Data_Perio,C,254 | Capacity,N,10,0 | Source,C,254 | Longitude,N,19,11 | Latitude,N,19,11
			proximator.read_proximity_shapefile (path_farmlandfinder, "Ethanol_Plants_US_20190101.shp", "Ethanol Plants", "", "Company", map_watershed, &view, log);
			// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "County_Roads.shp", "County Roads", "OBJECTID", "LINEARID", map_watershed, &view, log);
			// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "State_Highways.shp", "State Highways", "OBJECTID", "LINEARID", map_watershed, &view, log);
			// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "Interstate_Highways.shp", "Interstate Highways", "OBJECTID", "LINEARID", map_watershed, &view, log);
			// LINEARID,C,22 | FULLNAME,C,100 | RTTYP,C,1 | MTFCC,C,5 | PREQUAL,C,3 | PREDIR,C,2 | PRETYP,C,14 | NAME,C,100 | SUFTYP,C,14 | SUFDIR,C,2 | SUFQUAL,C,3 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "Midwest_All_Roads.shp", "All Roads", "OBJECTID", "LINEARID", map_watershed, &view, log);

			// UACE10,C,5| GEOID10,C,5 | NAME10,C,100 | NAMELSAD10,C,100 | LSAD10,C,2 | MTFCC10,C,5 | UATYP10,C,1 | FUNCSTAT10,C,1 | ALAND10,N,14,0 | AWATER10,N,14,0 | INTPTLAT10,C,11 | INTPTLON10,C,12
			proximator.read_proximity_shapefile (path_farmlandfinder, "tl_2019_us_uac10.shp", "Urbanized Areas", "GEOID10", "NAME10", map_watershed, &view, log);
			proximator.divide_urban_areas ("Urbanized Areas", map_watershed, log);

			// TYPEPIPE,C,10 | Operator,C,50 | Status,C,50 | Shape_Leng,N,19,11
			proximator.read_proximity_shapefile (path_farmlandfinder, "NaturalGas_Pipelines_US_202001.shp", "Natural Gas Pipelines", "", "", map_watershed, &view, log);

			// LINEARID,C,22 | FULLNAME,C,100 | MTFCC,C,5 | OBJECTID,N,10,0
			proximator.read_proximity_shapefile (path_farmlandfinder, "Railroads.shp", "Railroads", "OBJECTID", "LINEARID", map_watershed, &view, log);
			for (area = MENU_AREA_1; area <= MENU_AREA_18; area++) {
				if (true) {
					// if (area == MENU_AREA_7) {
					area_name = NHD_AREA_NAME [area - MENU_AREA_1];
					area_names.push_back (area_name);
				}
			}
			if ((layer_rivers = read_nhd_area_rivers (&area_names, false, &view, log)) != NULL) {
				ProximityLayer proxy;
				proxy.layer = layer_rivers;
				proxy.write_layer_point = nhd_river_callback;
				proxy.write_column_headers = nhd_river_column_headers_callback;
				proximator.proximity_layers.push_back (proxy);
			}

			if ((layer_nhd_lakes = read_nhd_water_bodies (app->filename_nhd_root, &area_names, map_watershed, &view, log)) != NULL) {
				ProximityLayer proxy;
				proxy.layer = layer_nhd_lakes;
				proxy.write_layer_point = nhd_lake_callback;
				proxy.write_column_headers = nhd_lake_column_headers_callback;
				proximator.proximity_layers.push_back (proxy);
			}

			proximator.base_layer = zillow.layer_zillow[0];
			proximator.write_point_function = &pipeline_zillow_point_callback;
			if (proximator.delimiter == '\t') {
				proximator.base_layer_column_headers = "ID\tRow ID\tTrans ID\tImportParcelID";
				// proximator.base_layer_column_headers = "\tProperty Full Street Address\tSales Date\tState FIPS\tCounty FIPS\tSales Price Amount\tLot Size ft ^ 2\tlatitude\tlongitude";
				proximator.base_layer_column_headers += "\tGas Distribution 10 mi.\tGas Gathering 10 mi.\tHazardous Liquid 10 mi.\tLiquified Natural Gas 10 mi.";
			}
			else {
				proximator.base_layer_column_headers = "\"ID\",\"Row ID\",\"Trans ID\",\"ImportParcelID\"";
				// proximator.base_layer_column_headres += ",\"Property Full Street Address\",\"Sales Date\",\"State FIPS\",\"County FIPS\",\"Sales Price Amount\",\"Lot Size ft^2\",\"latitude\",\"longitude\"";
				proximator.base_layer_column_headers += ",\"Gas Distribution 10 mi.\",\"Gas Gathering 10 mi.\",\"Hazardous Liquid 10 mi.\",\"Liquified Natural Gas 10 mi.\"";
			}
			proximator.thread_count = number_of_threads;
			proximator.write_proximity_table (within_check, -1, &view, log);
		}
		end_time = std::chrono::system_clock::now ();
		elapsed_milliseconds = end_time - start_time;
		log.add_formatted ("Time Elapsed\t%.3lf\tseconds\n", (double) elapsed_milliseconds.count () / 1000.0);

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("LAGOS");

		view.update_status_bar ("Map Updated");
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::On_Pipeline_Zillow_Within_Radius
	(wxCommandEvent &)

// 2021-05-05 Nieyang Cheng: show distances & ids of all pipeline incidents <= 10km.

{
	dynamic_string log, area_name, path_pipeline, filename_county_master, path_zillow_csv;
	ZillowSales zillow;
	CountyMaster counties;
	PipelineAccidents pipes;
	int number_of_threads = 20, first_sales_record, last_sales_record;
	std::chrono::system_clock::time_point start_time, end_time;
	std::chrono::duration <double, std::milli> elapsed_milliseconds;
	int state_fips, first_state_fips, last_state_fips;
	int incident_type; // 1-4 Gas Distribution, Gas Gathering, Hazardous Liquids, LNG
	double cutoff_radius_miles = 8.0;
	bool error = false;

	char *incident_name[4] = {"GD", "GTGG", "HL", "LNG"};

	first_state_fips = 1;
	last_state_fips = 56;
	first_sales_record = 1;
	last_sales_record = -1;

	path_pipeline = app->project_root; // F:\scratch\Pipelines
	
	// filename_nhd_lake = "NHDSnapshot\\Hydrography\\NHDWaterbody.shp";
	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb"; 
	path_zillow_csv = path_pipeline + "housing_transactions\\";

	dialog_project dj (this, &first_sales_record, &last_sales_record, &number_of_threads, &first_state_fips, &last_state_fips);
	if (dj.ShowModal () == wxID_OK) {

		interface_window_wx view;

		view.start_progress (this);
		/* 
		dialog_run_threads *update_display = NULL;

		update_display = new dialog_run_threads (this, true, false);
		update_display->Show ();
		update_display->setup_interface_window (app, &view);
		*/

		start_time = std::chrono::system_clock::now ();

		counties.read (filename_county_master.get_text_ascii (), &view, log);

		for (state_fips = first_state_fips;
		!error
		&& (state_fips <= last_state_fips);
		++state_fips) {
			// Create a point map layer of sales from Zillow data
			if (zillow.read_csv_format_3 (path_zillow_csv, state_fips, first_sales_record, last_sales_record, &counties, map_watershed, &view, log)) {

				for (incident_type = 4;
				!error
				&& (incident_type <= 4);
				++incident_type) {
					MapLayerProximity proximator;

					proximator.cutoff_radius_miles = cutoff_radius_miles;
					proximator.delimiter = ',';

					proximator.output_filename = path_pipeline;
					proximator.output_filename += incident_name[incident_type - 1];
					if (last_sales_record != -1)
						proximator.output_filename.add_formatted (" Incidents 8mi State %02d %07d-%07d.csv", state_fips, first_sales_record, last_sales_record);
					else
						proximator.output_filename.add_formatted (" Incidents 8mi State %02d.csv", state_fips);

					// create LAGOS_Algal_Blooms.layer_lagos from shapefile
					if (pipes.read_csv (path_pipeline, incident_type, map_watershed, &view, log)) {

						switch (incident_type) {
						case 1:
						{
							ProximityLayer proxy;
							proxy.layer = pipes.gas_distribution;
							proxy.write_layer_point = pipeline_callback_10mi;
							proxy.write_column_headers = pipeline_column_header_callback_10mi;
							proximator.proximity_layers.push_back (proxy);
						}
						break;
						case 2:
						{
							ProximityLayer proxy;
							proxy.layer = pipes.gas_gathering;
							proxy.write_layer_point = pipeline_callback_10mi;
							proxy.write_column_headers = pipeline_column_header_callback_10mi;
							proximator.proximity_layers.push_back (proxy);
						}
						break;
						case 3:
						{
							ProximityLayer proxy;
							proxy.layer = pipes.hazardous_liquid;
							proxy.write_layer_point = pipeline_callback_10mi;
							proxy.write_column_headers = pipeline_column_header_callback_10mi;
							proximator.proximity_layers.push_back (proxy);
						}
						break;
						case 4:
						{
							ProximityLayer proxy;
							proxy.layer = pipes.liquified_natural_gas;
							proxy.write_layer_point = pipeline_callback_10mi;
							proxy.write_column_headers = pipeline_column_header_callback_10mi;
							proximator.proximity_layers.push_back (proxy);
						}
						}

						// Count pipeline incidents within 10 miles
						// pipes.count_within_radius (zillow.layer_zillow[0], cutoff_radius_miles, incident_type, 16, &view, log);

						proximator.base_layer = zillow.layer_zillow[0];
						proximator.write_point_function = &pipeline_zillow_point_callback_10mi;
						if (proximator.delimiter == '\t') {
							proximator.base_layer_column_headers = "ID\tRow ID\tTrans ID\tImportParcelID";
						}
						else {
							proximator.base_layer_column_headers = "\"ID\",\"Row ID\",\"Trans ID\",\"ImportParcelID\"";
						}
						proximator.thread_count = number_of_threads;
						if (!proximator.write_proximity_table_within_radius (&view, log))
							error = true;
					}
					else
						error = true;
				}
			}
		}

		end_time = std::chrono::system_clock::now ();
		elapsed_milliseconds = end_time - start_time;
		log.add_formatted ("Time Elapsed\t%.3lf\tseconds\n", (double) elapsed_milliseconds.count () / 1000.0);

		/*
		update_display->Destroy ();
		delete update_display;
		*/

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("LAGOS");

		view.update_status_bar ("Map Updated");
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnFarmlandFinderPopulation
	(wxCommandEvent &)

// 2021-07-01 Wendong: Find overlapping zip code population at 5 and 10 mile radius
// Also # of urban areas where > 10% overlap occurs

// 2022-08-15 Nieyan: updated FarmlandFinder sales & listings

{
	dynamic_string log, filename_zip, filename_ua, filename_county_master;
	dynamic_string filename_sales;
	FarmlandFinder farms;
	CountyMaster counties;
	map_layer *layer_sales;
	double radius_miles;
	int within_check = PROXIMITY_WITHIN_POLYGON_DONT_DETECT;

	filename_zip = "f:\\FarmlandFinder\\Population Density\\ACS_2019_Zipcode_w_Popu.shp";
	filename_ua = "f:\\FarmlandFinder\\Population Density\\ACS_2019_UA_w_Popu.shp";

	// Farmland Finder data
	// filename_sales = "f:\\FarmlandFinder\\Population Density\\FarmlandFinder_Data_ID_LatLon_Only.csv";
	filename_sales = "F:\\FarmlandFinder\\2022-08-15\\Range_Ag_Farmland_Sales_Since_Nov2020.csv";
	farms.multithread_count = 20;
	radius_miles = 5.0;
	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb";

	interface_window_wx view;
	view.start_progress (this);

	counties.read(filename_county_master.get_text_ascii(), &view, log);

	layer_sales = farms.read_sales (filename_sales, map_watershed, &counties, &view, log);
	farms.overlay_population (filename_zip, filename_ua, radius_miles, map_watershed, &view, log);

	{
		// 2021-07-08 Add proximity to grain locations
		MapLayerProximity proximator;
		ProximityLayer *proxy_layer;

		proximator.delimiter = '\t';

		proxy_layer = proximator.read_proximity_csv ("f:\\FarmlandFinder\\Grain_Location_Lat_Lon_072021.csv", "Grain Locations",
		map_watershed, &view, log);
		proxy_layer->write_column_headers = &proximity_id_name_distance_column_headers_callback;
		proxy_layer->write_layer_point = &grain_location_callback;
		proximator.base_layer = layer_sales;
		proximator.write_point_function = &proximity_id_name_callback;
		if (proximator.delimiter == '\t') {
			proximator.base_layer_column_headers = "ID\tListing ID";
		}
		else {
			proximator.base_layer_column_headers = "\"ID\",\"Listing ID\"";
		}
		proximator.thread_count = farms.multithread_count;
		proximator.write_proximity_table (within_check, -1, &view, log);
	}

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("FarmlandFinder");

	view.update_status_bar ("Map Updated");

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::OnFarmlandFinderPowerlines
	(wxCommandEvent &)

// 2021-08-23 Nieyan Chang: overlap FarmlandFinder circles and 5km radius buffers with powerline shapefile

{
	const int POWERLINE_LAYER_COUNT = 5;
	dynamic_string log, filename_feets[POWERLINE_LAYER_COUNT];
	dynamic_string filename_sales, filename_half_mile, filename_mile;
	FarmlandFinder farms;
	map_layer *layer_feets[POWERLINE_LAYER_COUNT], *layer_sales, *layer_half_mile, *layer_mile, *layer_buffer = NULL;
	int powerline_index;

	filename_feets[0] = "f:\\FarmlandFinder\\transmission lines\\lines_types\\WGS_1984\\feets0_WGS1984.shp";
	filename_feets[1] = "f:\\FarmlandFinder\\transmission lines\\lines_types\\WGS_1984\\feets100_WGS1984.shp";
	filename_feets[2] = "f:\\FarmlandFinder\\transmission lines\\lines_types\\WGS_1984\\feets120_WGS1984.shp";
	filename_feets[3] = "f:\\FarmlandFinder\\transmission lines\\lines_types\\WGS_1984\\feets150_WGS1984.shp";
	filename_feets[4] = "f:\\FarmlandFinder\\transmission lines\\lines_types\\WGS_1984\\feets200_WGS1984.shp";

	int powerline_size[POWERLINE_LAYER_COUNT] = { 100, 100, 120, 150, 200 };
	char *layer_name [POWERLINE_LAYER_COUNT] = { "Feet 0", "Feet 100", "Feet 120", "Feet 150", "Feet 200" };

	color_set line_colors;

	line_colors.add_color (RGB (128, 64, 0), 1); // brown
	line_colors.add_color (RGB (112, 146, 190), 1); // blue-gray
	line_colors.add_color (RGB (255, 128, 0), 1); // orange
	line_colors.add_color (RGB (0, 0, 128), 1);
	line_colors.add_color (RGB (0, 128, 0), 1);

	// Farmland Finder data
	filename_sales = "F:\\FarmlandFinder\\FarmlandFinder Field Circles\\farm_variable.shp";
	filename_half_mile = "F:\\FarmlandFinder\\FarmlandFinder Field Circles\\FarmlandFinder plus-half-mile radius.shp";
	filename_mile = "F:\\FarmlandFinder\\FarmlandFinder Field Circles\\FarmlandFinder plus-mile radius.shp";
	farms.multithread_count = 6;

	interface_window_wx view;
	view.start_progress (this);

	view.update_progress ("Reading Shapefiles");

	// Powerline layers
	for (powerline_index = 0; powerline_index < POWERLINE_LAYER_COUNT; ++powerline_index) {
		importer_shapefile importer;
		layer_feets[powerline_index] = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_feets[powerline_index]->name = layer_name [powerline_index];
		layer_feets[powerline_index]->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_feets[powerline_index]->color = line_colors.colors[powerline_index].color;
		map_watershed->layers.push_back (layer_feets[powerline_index]);

		importer.filename_source = filename_feets[powerline_index];
		importer.id_field_name = "ID";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		if (importer.import (layer_feets[powerline_index], NULL, map_watershed, &view, log)) {
			map_watershed->set_extent ();
			log.add_formatted ("Feets100 layer size : %d\n", layer_feets[powerline_index]->objects.size ());
		}
	}

	{
		// Farmland Finder field circles
		importer_shapefile importer;
		layer_sales = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_sales->name = "Land Sales";
		layer_sales->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_sales->color = RGB (0, 128, 255);
		layer_sales->include_in_legend = true;
		map_watershed->layers.push_back (layer_sales);

		importer.filename_source = filename_sales;
		importer.id_field_name = "ID";
		importer.name_field_name = "NAME";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		/*
		shapefile_filter *filter_1982 = new shapefile_filter;
		filter_1982->type = SHAPEFILE_FILTER_RANGE;
		filter_1982->field_number = 120; // ORIG_FID
		filter_1982->range_ge = 4103;
		filter_1982->range_lt = 4104;

		importer.filters.push_back (filter_1982);
		*/


		if (importer.import (layer_sales, NULL, map_watershed, &view, log)) {
		}
	}

	{
		// Farm-centered 1/2-mile radius 
		importer_shapefile importer;

		layer_half_mile = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_half_mile->name = "Farm plus-half-mile circles";
		layer_half_mile->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_half_mile->color = RGB (64, 0, 128);
		layer_half_mile->include_in_legend = true;
		map_watershed->layers.push_back (layer_half_mile);

		importer.filename_source = filename_half_mile;
		importer.id_field_name = "ID";
		importer.name_field_name = "NAME";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		/*
		shapefile_filter *filter_1982 = new shapefile_filter;
		filter_1982->type = SHAPEFILE_FILTER_RANGE;
		filter_1982->field_number = 120; // ORIG_FID
		filter_1982->range_ge = 4103;
		filter_1982->range_lt = 4104;
		importer.filters.push_back (filter_1982);
		*/

		if (importer.import (layer_half_mile, NULL, map_watershed, &view, log)) {
		}
	}

	{
		// Farm-centered 1-mile radius 
		importer_shapefile importer;

		layer_mile = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_mile->name = "Farm plus-mile circles";
		layer_mile->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_mile->color = RGB (64, 0, 128);
		layer_mile->include_in_legend = true;
		map_watershed->layers.push_back (layer_mile);

		importer.filename_source = filename_mile;
		importer.id_field_name = "ID";
		importer.name_field_name = "NAME";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		if (importer.import (layer_mile, NULL, map_watershed, &view, log)) {
		}
	}

	// Buffers around powerlines
	for (powerline_index = 0; powerline_index < POWERLINE_LAYER_COUNT; ++powerline_index) {
		// Create buffer layer around powerlines
		ClipperBuffer clipper;
		ClipperLib::Path p;
		ClipperLib::Paths path_powerline, path_powerline_buffer;
		std::vector <map_object *>::const_iterator powerline;
		double offset_radius_average_m;

		if (powerline_index == 0) {
			layer_buffer = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_buffer->name = "Powerline Buffers";
			layer_buffer->color = RGB (255, 127, 39);
			layer_buffer->line_width = 2;
			layer_buffer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_buffer->attribute_count_numeric = 7;
			layer_buffer->column_names_numeric.push_back ("FarmlandFinder ID");
			layer_buffer->column_names_numeric.push_back ("Powerline ID");
			layer_buffer->column_names_numeric.push_back ("BufferWidth");
			layer_buffer->column_names_numeric.push_back ("Geodesic");
			layer_buffer->column_names_numeric.push_back ("Exact");
			layer_buffer->column_names_numeric.push_back ("Authalic");
			layer_buffer->column_names_numeric.push_back ("Rhumb");
			layer_buffer->attribute_count_text = 0;
			layer_buffer->initialize_attributes = true; // clear all attributes as polygons are created

			map_watershed->layers.push_back (layer_buffer);
		}

		for (powerline = layer_feets[powerline_index]->objects.begin ();
		powerline != layer_feets[powerline_index]->objects.end ();
		++powerline) {
			path_powerline.clear ();
			clipper.map_object_to_Path (*powerline, &path_powerline, log);

			// Radius in meters for this line
			offset_radius_average_m = clipper.average_offset (layer_feets[powerline_index], (*powerline)->id, (powerline_size[powerline_index] / 2) / FEET_PER_METER, log);

			clipper.create_buffer (&path_powerline, &path_powerline_buffer, offset_radius_average_m, log);

			clipper.tract_id_source_1 = CLIPPED_ID_SOURCE_ID;
			clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_ID;
			clipper.buffer_width_output_index = 2;

			// No need to compute area of powerline buffers
			clipper.Paths_to_map_layer (&path_powerline_buffer, (*powerline)->id, offset_radius_average_m, NULL, NULL, layer_buffer, false, &view, log);
		}
		layer_buffer->set_extent ();
	}

	// Intersect buffers with each farm separately 

	farms.clip_buffers (layer_sales, layer_buffer, map_watershed, &view, log);
	farms.clip_buffers (layer_half_mile, layer_buffer, map_watershed, &view, log);
	farms.clip_buffers (layer_mile, layer_buffer, map_watershed, &view, log);

	layer_buffer->draw_as = MAP_OBJECT_DRAW_NONE;

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	enable_map_controls ();
	panel_watershed->redraw ("FarmlandFinder");

	view.update_status_bar ("Map Updated");

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::On_OutlineMatchingWind
	(wxCommandEvent &)

// 2021-12-10 Nieyan: "...using wind speed at 80-meter height data" match wind class

{
	dynamic_string log;
	map_layer *layer_wind_data, *layer_gridpoints, *layer_polygons, *layer_high, *layer_low;
	bool gridpoints_prebuilt = true;

	// dialog_project dj (this, &first_sales_record, &last_sales_record, &number_of_threads, &first_state_fips, &last_state_fips);
	// if (dj.ShowModal () == wxID_OK) {
	{

		interface_window_wx view;
		view.start_progress (this);

		// Event newest libtiff version 4.3.0 can't read tiff file "wtk_conus_80m_mean_masked.tif"
		// card_bitmap_tiff tiff;
		// tiff.read_tiff ("h:\\scratch\\FarmlandFinder\\wind speed\\wtk_conus_80m_mean_masked.tif", NULL, log);

		// OBJECTID,N,10,0	FID_wind_8,N,10,0	pointid,N,10,0	grid_code,N,19,11	POINT_X,N,19,11	POINT_Y,N,19,11	FID_states,N,10,0	states_STA,C,25	states_S_1,C,2	states_SUB,C,20	states_S_2,C,2	states_POP,N,10,0	states_P_1,N,19,11	states_P_2,N,10,0	states_P_3,N,19,11	states_WHI,N,10,0	states_BLA,N,10,0	states_AME,N,10,0	states_ASI,N,10,0	states_HAW,N,10,0	states_HIS,N,10,0	states_OTH,N,10,0	states_MUL,N,10,0	states_MAL,N,10,0	states_FEM,N,10,0	states_AGE,N,10,0	states_A_1,N,10,0	states_A_2,N,10,0	states_A_3,N,10,0	states_A_4,N,10,0	states_A_5,N,10,0	states_A_6,N,10,0	states_A_7,N,10,0	states_A_8,N,10,0	states_A_9,N,10,0	states__10,N,10,0	states__11,N,10,0	states_MED,N,19,11	states_M_1,N,19,11	states_M_2,N,19,11	states_HOU,N,10,0	states_AVE,N,19,11	states_HSE,N,10,0	states_H_1,N,10,0	states_MAR,N,10,0	states_M_3,N,10,0	states_MHH,N,10,0	states_FHH,N,10,0	states_FAM,N,10,0	states__12,N,19,11	states_H_2,N,10,0	states_VAC,N,10,0	states_OWN,N,10,0	states_REN,N,10,0	states_NO_,N,19,11	states__13,N,19,11	states_CRO,N,19,11	states__14,N,19,11	states_SQM,N,19,11	covered_st,C,254	wind_class,N,10,0
		// 1	1446584	1446584	7.01616460011	-98.33469519310	36.99931297700	17	Kansas	20	West North Central	KS	2960432	36.00000000000	2853118	34.70000000000	2391044	167864	28150	67762	2238	300042	110127	85933	1415408	1437710	205492	202447	198884	203821	204454	377720	346673	406264	331247	190389	126409	59318	36.00000000000	34.70000000000	37.30000000000	1112096	2.49000000000	140529	168809	250118	318191	32147	81709	734103	3.06000000000	1233215	121119	753532	358564	58569.00000000000	781.00000000000	29125505.00000000000	320694.00000000000	82277.97000000000	Kansas	3
		// 2	1446585	1446585	6.97864042893	-98.31083473040	36.99982285240	17	Kansas	20	West North Central	KS	2960432	36.00000000000	2853118	34.70000000000	2391044	167864	28150	67762	2238	300042	110127	85933	1415408	1437710	205492	202447	198884	203821	204454	377720	346673	406264	331247	190389	126409	59318	36.00000000000	34.70000000000	37.30000000000	1112096	2.49000000000	140529	168809	250118	318191	32147	81709	734103	3.06000000000	1233215	121119	753532	358564	58569.00000000000	781.00000000000	29125505.00000000000	320694.00000000000	82277.97000000000	Kansas	2
		// 3	1446586	1446586	6.90169061457	-98.28697392170	37.00032749160	17	Kansas	20	West North Central	KS	2960432	36.00000000000	2853118	34.70000000000	2391044	167864	28150	67762	2238	300042	110127	85933	1415408	1437710	205492	202447	198884	203821	204454	377720	346673	406264	331247	190389	126409	59318	36.00000000000	34.70000000000	37.30000000000	1112096	2.49000000000	140529	168809	250118	318191	32147	81709	734103	3.06000000000	1233215	121119	753532	358564	58569.00000000000	781.00000000000	29125505.00000000000	320694.00000000000	82277.97000000000	Kansas	2


		{
			importer_shapefile importer;
			view.update_progress ("Reading Points");
			// importer.filename_source = "I:\\TDC\\FarmlandFinder\\wind speed\\intersect_states_wgs1984.shp"; 
			importer.filename_source = "h:\\scratch\\FarmlandFinder\\wind speed\\intersect_states_wgs1984.shp";
			importer.id_field_name = "OBJECTID";
			importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			layer_wind_data = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_wind_data->name = importer.filename_source;
			layer_wind_data->draw_as = MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;
			layer_wind_data->symbol = MAP_OBJECT_SYMBOL_PLUS;
			layer_wind_data->data_attribute_index = 54;
			map_watershed->layers.push_back (layer_wind_data);
			/*
			shapefile_filter *filter;
			filter = new shapefile_filter;
			filter->type = SHAPEFILE_FILTER_AREA_NAME;
			filter->field_name = "states_S_1";
			filter->area_name = "20";
			importer.filters.push_back (filter);

			filter = new shapefile_filter;
			filter->type = SHAPEFILE_FILTER_AREA;
			filter->location.x [0] = -99.5;
			filter->location.x [1] = -99.0;
			filter->location.y [0] = 37.5;
			filter->location.y [1] = 38.0;
			importer.filters.push_back (filter);
			*/

			layer_wind_data->colors.clear ();
			// Three shades of green for 0..2
			layer_wind_data->colors.add_color (RGB (191, 255, 191), 0.0, 1.0, "0<=x<1");
			layer_wind_data->colors.add_color (RGB (0, 255, 64), 1.0, 2.0, "1<=x<2");
			layer_wind_data->colors.add_color (RGB (0, 128, 0), 2.0, 3.0, "2<=x<3");
			// three shades of blue for 3..5
			layer_wind_data->colors.add_color (RGB (128, 255, 255), 3.0, 4.0, "3<=x<4");
			layer_wind_data->colors.add_color (RGB (51, 153, 255), 4.0, 5.0, "4<=x<5");
			layer_wind_data->colors.add_color (RGB (0, 0, 255), 5.0, 6.0, "5<=x<6");

			panel_watershed->pause_rendering = true;
			importer.import (layer_wind_data, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}


		layer_gridpoints = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_gridpoints->name = "h:\\scratch\\FarmlandFinder\\wind speed\\Wind Grid.shp";
		layer_gridpoints->draw_as = MAP_OBJECT_DRAW_NONE; //  MAP_OBJECT_DRAW_SYMBOL_DATA_COLOR;
		layer_gridpoints->symbol = MAP_OBJECT_SYMBOL_CIRCLE;
		layer_gridpoints->data_attribute_index = 0;
		map_watershed->layers.push_back (layer_gridpoints);

		if (gridpoints_prebuilt) {
			importer_shapefile importer;

			view.update_progress ("Reading Grid");
			importer.filename_source = layer_gridpoints->name;
			importer.id_field_name = "ID";
			importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			panel_watershed->pause_rendering = true;
			importer.import (layer_gridpoints, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}
		else {
			layer_gridpoints->attribute_count_numeric = 3; // row index, column index
			layer_gridpoints->column_names_numeric.push_back ("Row Index");
			layer_gridpoints->column_names_numeric.push_back ("Column Index");
			layer_gridpoints->column_names_numeric.push_back ("Wind Speed");
			layer_gridpoints->initialize_attributes = true;
		}


		layer_polygons = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_polygons->name = "Polygons";
		layer_polygons->draw_as = MAP_OBJECT_DRAW_NONE; 
		layer_polygons->data_attribute_index = 0;
		// layer_polygons->colors = speed_colors;

		layer_polygons->attribute_count_numeric = 3;
		layer_polygons->column_names_numeric.push_back ("Wind Speed GE");
		layer_polygons->column_names_numeric.push_back ("Wind Speed LT");
		layer_polygons->column_names_numeric.push_back ("Cell Count");
		layer_polygons->initialize_attributes = true;
		layer_polygons->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
		map_watershed->layers.push_back (layer_polygons);

		flood_aggregator flooder;
		long id;

		view.update_progress ("Building Grid");
		if (gridpoints_prebuilt)
			// Build grid from map_layer (from previous run)
			flooder.build_grid_from_layer (layer_gridpoints, &view, log);
		else
			// Build grid from shapefile points.
			flooder.build_grid (layer_wind_data, layer_gridpoints, &view, log);

		flooder.find_average_sizes (log);
		flooder.divide_by_speed (layer_wind_data, layer_gridpoints, &view, log);

		view.update_progress ("Outlining Speed");
		flooder.grid.clear_use ();
		id = 1;

		/*
		flooder.outline_speed (&id, 0, layer_wind_data, layer_polygons, &view, log);
		flooder.outline_speed (&id, 1, layer_wind_data, layer_polygons, &view, log);
		flooder.outline_speed (&id, 2, layer_wind_data, layer_polygons, &view, log);
		flooder.outline_speed (&id, 3, layer_wind_data, layer_polygons, &view, log);
		flooder.outline_speed (&id, 4, layer_wind_data, layer_polygons, &view, log);
		flooder.outline_speed (&id, 5, layer_wind_data, layer_polygons, &view, log);
		*/

		flooder.outline_speed (&id, 0, 3, 0, layer_wind_data, layer_polygons, &view, log);
		flooder.outline_speed (&id, 3, 6, 1, layer_wind_data, layer_polygons, &view, log);

		flooder.set_interior_polygons (layer_polygons, &view, log);

		// Separate layer_polygons into high and low
		flooder.separate_layer (layer_polygons, &layer_high, &layer_low, map_watershed, &view, log);

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenShapefile");

		view.update_status_bar ("Map Updated");
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::On_MatchUMRBHUCs
	(wxCommandEvent &)

// 2021-12-21 Is Yiannis' UMRB HUC shapfile related to 'official' HUC12 map?

{
	dynamic_string log, filename_output, filename_swat, filename_huc12;
	map_layer *layer_swat, *layer_huc12;
	PolygonMatch matcher;
	int huc12_attribute_index;

	interface_window_wx view;
	view.start_progress (this);

	// 2018-09-25
	filename_swat = "R:\\NSF project\\UMRB_SWAT2012_project_New\\Watershed\\Shapes\\subs1_WGS_1984.shp";
	// filename_huc12 = "R:\\SWAT\\NSF Big Data\\SWAT output videos\\UMRB test\\Shapefile for huc12 UMRB\\huc12_UMRB.shp";
	filename_huc12 = "R:\\SWAT\\NSF Big Data\\WBD\\WBDHU12_umrb.shp";
	huc12_attribute_index = 7; // 'huc12'
	filename_output = "H:\\scratch\\NSF InFEWS\\UMRB Polygon IDs.csv";

	{
		importer_shapefile importer;
		view.update_progress ("Reading SWAT Polygons");

		importer.filename_source = filename_swat;
		importer.id_field_name = "OBJECTID";
		importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		layer_swat = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_swat->name = importer.filename_source;
		layer_swat->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_swat->color = RGB (128, 128, 255);
		map_watershed->layers.push_back (layer_swat);

		panel_watershed->pause_rendering = true;
		importer.import (layer_swat, NULL, map_watershed, &view, log);
		panel_watershed->pause_rendering = false;
	}

	{
		importer_shapefile importer;
		view.update_progress ("Reading HUC12 Polygons");

		importer.filename_source = filename_huc12;
		importer.id_field_name = "TNMID";
		importer.name_field_name = "NAME";
		importer.projection_from_prj_file = true;
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		layer_huc12 = map_watershed->create_new (MAP_OBJECT_POLYGON);
		layer_huc12->name = importer.filename_source;
		layer_huc12->draw_as = MAP_OBJECT_DRAW_OUTLINE_DATA_COLOR;
		layer_huc12->color = RGB (128, 255, 128);
		map_watershed->layers.push_back (layer_huc12);

		/*
			shapefile_filter *filter;
			filter = new shapefile_filter;
			filter->type = SHAPEFILE_FILTER_MASK;
			filter->field_name = "tohuc";
			filter->pattern = "07##########";
			importer.filters.push_back (filter);
		*/

		panel_watershed->pause_rendering = true;
		importer.import (layer_huc12, NULL, map_watershed, &view, log);
		panel_watershed->pause_rendering = false;
	}

	matcher.layer_1 = layer_swat;
	matcher.layer_2 = layer_huc12;
	matcher.thread_count = 6;
	matcher.huc12_attribute_index = huc12_attribute_index; 

	// matcher.run (filename_output, log);
	matcher.run_threaded (filename_output, &view, log);

	view.destroy_dialog_progress ();

	map_watershed->check_extent ();
	panel_watershed->change_layers ();
	enable_map_controls ();


	if (log.get_length () > 0) {
		log.convert_linefeeds_for_CEdit ();
		dialog_error_list d (this, L"SWAT UMRB", L"", log);
		d.ShowWindowModal ();
	}
}

void frame_CARDGIS::On_ListNearbyLakes
	(wxCommandEvent &)

// 2021-12-09 List 10 lakes nearest each address coordinate

{
	dynamic_string log, filename_lake, area_name, filename_county_master, filename_user_address;
	ZillowSales zillow;
	int number_of_threads = 20, first_sales_record, last_sales_record, first_state_fips, last_state_fips;
	std::chrono::system_clock::time_point start_time, end_time;
	std::chrono::duration <double, std::milli> elapsed_milliseconds;
	map_layer *layer_address;
	ProximityLayer *lake_proxy;
	importer_shapefile importer;

	first_state_fips = last_state_fips = 1;
	first_sales_record = 1;
	last_sales_record = 5000000;

	// filename_nhd_lake = "NHDSnapshot\\Hydrography\\NHDWaterbody.shp";
	filename_lake = "F:\\Lake Proximity\\Lake139.csv";
	filename_user_address = "F:\\Lake Proximity\\maps of mailing addresses\\cleaned_address_location.shp";

	dialog_project dj (this, &first_sales_record, &last_sales_record, &number_of_threads, &first_state_fips, &last_state_fips);
	if (dj.ShowModal () == wxID_OK) {

		interface_window_wx view;

		view.start_progress (this);
		/* 
		dialog_run_threads *update_display = NULL;

		update_display = new dialog_run_threads (this, true, false);
		update_display->Show ();
		update_display->setup_interface_window (app, &view);
		*/

		start_time = std::chrono::system_clock::now ();

		// Read point map layer of Addresses
		// FIRST_NAME,C,254	MI,C,254	LASTNAME,C,254	TITLE,C,254	NMPRESENT,C,254	CONFIDENCE,N,10,0	GENDER,C,254	FIPS,N,10,0	TRACT,N,10,0	BLOCK,C,254	ADDRESS1,C,254	ADDRESS2,C,254	CITY,C,254	STATE_ABBR,C,254	ZIP,C,254	ZIP4,C,254	DROP_IND,C,254	VACANCY,C,254	REC_TYPE,C,254	DEL_IND,N,10,0	SEASONAL,C,254	CARRIERID,C,254	WALK_SEQ,N,10,0	LOT_CODE,C,254	DEL_BAR,N,10,0	DEL_TYPE,C,254	CEN_DIV_9,N,10,0	KEY_CODE,N,10,0	DMA,N,10,0	MSA,N,10,0	CBSA,N,10,0	CD,N,10,0	SIZE,C,254	CEN_REG_4,N,10,0	POBOX_FLAG,C,254	PCT_HISP,N,10,0	PCT_BLACK,N,10,0	PCT_ASIAN,N,10,0	PCT_WHITE,N,10,0	PCT_AM_IND,N,10,0	HISP_IND,C,254	MEDIAN_INC,N,10,0	SEQUENCE,N,10,0	AGE1,N,10,0	AGE2,N,10,0	NUM_CHILD,N,10,0	PRES_CHILD,C,254	CHILD_AGE,C,254	OWN,C,254	LengthResi,N,10,0	ADULT_AGE,N,19,11	YOUNG_ADLT,C,254	ETECH,C,254	USR,C,254	NAME,C,254	address,C,254	id,C,254	master_id,C,254	age,C,254	year,C,254	source,C,254	panel,C,254	pilot,C,254	region,C,254	City_1,C,254	State,C,254	accesscode,C,254	lat,N,19,11	lon,N,19,11
		// JOHN A DOE1	1	Y	2	M	19153	5100	3049	103 SW 3RD ST	APT 112	DES MOINES	IA	50309	4672	N	N	H	1	N	C101	416	0320A	374	C	4	1	679	19780	19780	3	B	2		4	17	4	74	1	N	53917	1077	48	48	0		0000000000000000000		2	100000000000.00000000000		K	U	CHRISTOPHER W BEELER	103 SW 3RD ST												41.58356700600	-93.61913098600
		// JOHN A DOE2	0	Y	2	F	19153	5100	3049	103 SW 3RD ST	APT 317	DES MOINES	IA	50309	4774	N	N	H	1	N	C101	516	0347A	920	C	4	1	679	19780	19780	3	B	2		4	17	4	74	1	N	53917	1105	24	24	0		0000000000000000000		2	10000000000000000000.00000000000		K	U	CAITLIN GALLAGHER	103 SW 3RD ST												41.58356700600	-93.61913098600

		importer.filename_source = filename_user_address;
		importer.id_field_name = "SEQUENCE";
		importer.name_field_name = "NAME";
		importer.projection_from_prj_file = true; // wgs_1984
		importer.normalize_longitude = false;
		importer.take_dbf_columns = true;

		layer_address = map_watershed->create_new (MAP_OBJECT_POINT);
		layer_address->name = importer.filename_source;
		layer_address->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
		layer_address->symbol = MAP_OBJECT_SYMBOL_POINT;
		layer_address->color = RGB (128, 128, 255);
		map_watershed->layers.push_back (layer_address);

		if (importer.import (layer_address, NULL, map_watershed, &view, log)) {

			MapLayerProximity proximator;
			
			if ((lake_proxy = proximator.read_proximity_csv_2 (filename_lake, "Iowa Lakes", 139, map_watershed, &view, log)) != NULL) {

				proximator.delimiter = ',';

				// proximator.output_filename = app->project_root;
				proximator.output_filename = "F:\\Lake Proximity\\Address to Lake.csv";

				proximator.base_layer = layer_address;
				proximator.write_point_function = &address_point_callback;
				if (proximator.delimiter == '\t')
					proximator.base_layer_column_headers = "ID\tName\tAddress1\tAddress2\tCity\tState\tZip\tZip4";
				else
					proximator.base_layer_column_headers = "\"ID\",\"Name\",\"Address1\",\"Address2\",\"City\",\"State\",\"Zip\",\"Zip4\"";

				proximator.thread_count = number_of_threads;
				proximator.write_proximity_table_list_closest (10, &view, log);
			}
		}

		end_time = std::chrono::system_clock::now ();
		elapsed_milliseconds = end_time - start_time;
		log.add_formatted ("Time Elapsed\t%.3lf\tseconds\n", (double) elapsed_milliseconds.count () / 1000.0);

		/*
		update_display->Destroy ();
		delete update_display;
		*/

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();

		view.update_status_bar ("Map Updated");
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

map_layer *make_buffer
	(const double radius_miles,
	const dynamic_string &layer_name,
	const COLORREF &color,
	map_layer *source_layer,
	dynamic_map *map_watershed,
	interface_window *view,
	dynamic_string &log)

// Create a new polygon layer that is a buffer inside or outside of source_layer 
// with distance radius_miles

{
	map_layer *buffer_layer;
	ClipperBuffer clipper;
	bool error = false;
	int thread_count = 12;
	double radius_meters = radius_miles * METERS_PER_MILE;

	view->update_progress_formatted (0, "Creating %.1lf mile buffer", radius_miles);

	// 5 mile
	buffer_layer = map_watershed->create_new (MAP_OBJECT_POLYGON);
	buffer_layer->name = layer_name;
	buffer_layer->name.add_formatted (" Buffer %dmile", (int) radius_miles);
	buffer_layer->color = color;
	buffer_layer->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	buffer_layer->initialize_attributes = true; // clear all attributes as polygons are created
	// buffer_layer->enclosed = true;
	map_watershed->layers.push_back (buffer_layer);

	view->update_progress (buffer_layer->name, 0);
	if (!clipper.create_threaded (source_layer, buffer_layer, thread_count, radius_meters, true, view, log))
		error = true;

	view->update_progress ("Setting Map Layer Extent", 0);
	buffer_layer->set_extent ();

	log.add_formatted ("Polygon count, %.1lf mile (%.4lf m) \t%d\n", radius_miles, radius_meters, (int) buffer_layer->objects.size ());

	if (!error)
		return buffer_layer;
	else {
		map_watershed->detach (buffer_layer);
		delete buffer_layer;
		return NULL;
	}
}

void frame_CARDGIS::On_BufferPolygonLayers
	(wxCommandEvent &)

// 2022-01-12 Nieyan: Create buffers 5 & 10 miles inside and outside of two wind speed layers
// 2022-01-24 Nieyan: try interior values 1 & 2 miles

{
	dynamic_string log, filename_wind_speed, filename_prefix;
	map_layer *layer_wind_data;
	filename_struct file_parser;

	// filename_wind_speed = "h:\\scratch\\FarmlandFinder\\wind speed\\wind_speed_0_2.shp";
	filename_wind_speed = "h:\\scratch\\FarmlandFinder\\wind speed\\wind_speed_3_5.shp";

	file_parser.parse (&filename_wind_speed);
	filename_prefix = file_parser.prefix;

	// dialog_project dj (this, &first_sales_record, &last_sales_record, &number_of_threads, &first_state_fips, &last_state_fips);
	// if (dj.ShowModal () == wxID_OK) {
	{

		interface_window_wx view;
		view.start_progress (this);

		{
			importer_shapefile importer;
			view.update_progress ("Reading Polygons");
			importer.filename_source = filename_wind_speed;
			importer.id_field_name = "OBJECTID";
			importer.projection_from_prj_file = true;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			layer_wind_data = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_wind_data->name = importer.filename_source;
			layer_wind_data->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_wind_data->color = RGB (255, 128, 0);
			// layer_wind_data->data_attribute_index = 54;
			map_watershed->layers.push_back (layer_wind_data);
			/*
			shapefile_filter *filter;
			filter = new shapefile_filter;
			filter->type = SHAPEFILE_FILTER_AREA_NAME;
			filter->field_name = "states_S_1";
			filter->area_name = "20";
			importer.filters.push_back (filter);

			filter = new shapefile_filter;
			filter->type = SHAPEFILE_FILTER_AREA;
			filter->location.x [0] = -99.5;
			filter->location.x [1] = -99.0;
			filter->location.y [0] = 37.5;
			filter->location.y [1] = 38.0;
			importer.filters.push_back (filter);
			*/

			panel_watershed->pause_rendering = true;
			importer.import (layer_wind_data, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}

		COLORREF *buffer_colors = make_palette_6 ();

		make_buffer (5.0, filename_prefix, buffer_colors [0], layer_wind_data, map_watershed, &view, log);
		make_buffer (10.0, filename_prefix, buffer_colors [1], layer_wind_data, map_watershed, &view, log);
		make_buffer (-1.0, filename_prefix, buffer_colors [2], layer_wind_data, map_watershed, &view, log);
		make_buffer (-2.0, filename_prefix, buffer_colors [3], layer_wind_data, map_watershed, &view, log);
		make_buffer (-5.0, filename_prefix, buffer_colors [4], layer_wind_data, map_watershed, &view, log);
		make_buffer (-10.0, filename_prefix, buffer_colors [5], layer_wind_data, map_watershed, &view, log);

		delete [] buffer_colors;

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenShapefile");

		view.update_status_bar ("Map Updated");
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

void frame_CARDGIS::On_IntersectWindLayers
	(wxCommandEvent &)

// 2022-03-01 Nieyan: Subtract layers

{
	dynamic_string log, filename_wind_speed, filename_buffer_1, filename_buffer_2, filename_states;
	map_layer *layer_wind_data, *layer_buffer_1, *layer_buffer_2, *layer_clipped, *layer_states;

	// filename_wind_speed = "h:\\scratch\\FarmlandFinder\\wind speed\\wind_speed_0_2.shp";
	// filename_wind_speed = "h:\\scratch\\FarmlandFinder\\wind speed\\wind_speed_3_5.shp";
	filename_states = "H:\\scratch\\FarmlandFinder\\wind speed\\USA_States_(Generalized)\\USA_States_Generalized.shp";

	filename_buffer_1 = "h:\\scratch\\FarmlandFinder\\wind speed\\wind_speed_3_5 Buffer 5mile.shp";
	filename_buffer_2 = "h:\\scratch\\FarmlandFinder\\wind speed\\wind_speed_0_2 Buffer -5mile.shp";

	// dialog_project dj (this, &first_sales_record, &last_sales_record, &number_of_threads, &first_state_fips, &last_state_fips);
	// if (dj.ShowModal () == wxID_OK) {
	{

		interface_window_wx view;
		view.start_progress (this);

		if (false) {
			importer_shapefile importer;
			view.update_progress ("Reading Polygons");
			importer.filename_source = filename_wind_speed;
			importer.id_field_name = "ID";
			importer.projection_from_prj_file = true;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true; // ID, Wind Speed, Wind Speed1, Cell Count

			layer_wind_data = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_wind_data->name = importer.filename_source;
			layer_wind_data->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_wind_data->color = RGB (255, 128, 0);
			// layer_wind_data->data_attribute_index = 54;
			map_watershed->layers.push_back (layer_wind_data);

			panel_watershed->pause_rendering = true;
			importer.import (layer_wind_data, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}
		{
			importer_shapefile importer;
			view.update_progress ("Reading States");
			importer.filename_source = filename_states;
			importer.id_field_name = "STATE_FIPS";
			importer.name_field_name = "STATE_NAME";
			importer.projection_from_prj_file = true;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true; // ID, Wind Speed, Wind Speed1, Cell Count

			layer_states = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_states->name = importer.filename_source;
			layer_states->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_states->color = RGB (255, 128, 0);
			map_watershed->layers.push_back (layer_states);

			panel_watershed->pause_rendering = true;
			importer.import (layer_states, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}

		{
			importer_shapefile importer;
			view.update_progress ("Reading Buffer 1");
			importer.filename_source = filename_buffer_1;
			importer.id_field_name = "ID";
			importer.projection_from_prj_file = true;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			layer_buffer_1 = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_buffer_1->name = importer.filename_source;
			layer_buffer_1->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_buffer_1->color = RGB (0, 0, 255);
			map_watershed->layers.push_back (layer_buffer_1);

			panel_watershed->pause_rendering = true;
			importer.import (layer_buffer_1, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}

		if (false) {
			importer_shapefile importer;
			view.update_progress ("Reading Buffer 2");
			importer.filename_source = filename_buffer_2;
			importer.id_field_name = "ID";
			importer.projection_from_prj_file = true;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			layer_buffer_2 = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_buffer_2->name = importer.filename_source;
			layer_buffer_2->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_buffer_2->color = RGB (192, 192, 192);
			map_watershed->layers.push_back (layer_buffer_2);

			panel_watershed->pause_rendering = true;
			importer.import (layer_buffer_2, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}

		if ((layer_clipped = map_watershed->match ("Intersection")) == NULL) {
			layer_clipped = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_clipped->name = "Intersection";
			layer_clipped->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_clipped->color = RGB (128, 0, 255);
			layer_clipped->attribute_count_numeric = 6; // Space for area fields in ClipperBuffer.Paths_to_map_layer
			layer_clipped->column_names_numeric.push_back ("Buffer 1 ID");
			layer_clipped->column_names_numeric.push_back ("STATE_FIPS");
			layer_clipped->column_names_numeric.push_back ("Geodesic m^2");
			layer_clipped->column_names_numeric.push_back ("Exact m^2");
			layer_clipped->column_names_numeric.push_back ("Authalic m^2");
			layer_clipped->column_names_numeric.push_back ("Rhumb m^2");
			layer_clipped->attribute_count_text = 0;
			layer_clipped->initialize_attributes = true;
			map_watershed->layers.push_back (layer_clipped);
		}

		{
			ClipperBuffer clipper;

			clipper.tract_id_source_1 = CLIPPED_ID_SOURCE_ID;
			clipper.tract_index_source_1 = -1;
			clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_ID;
			clipper.tract_index_source_2 = -1;

			clipper.intersect_threaded (layer_buffer_1, layer_states, layer_clipped, app->multithread_count, &view, log);
		}

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenShapefile");

		view.update_status_bar ("Map Updated");

		{
			dynamic_string report;
			std::vector <map_object *>::iterator fragment;

			report += "ID\t";
			report += layer_clipped->column_names_numeric[0];
			report += "\t";
			report += layer_clipped->column_names_numeric[1];
			report += "\tGeodesic, m^2\tExact, m^2\tAuthalic, m^2\tRhumb, m^2\n";

			for (fragment = layer_clipped->objects.begin ();
			fragment != layer_clipped->objects.end ();
			++fragment) {
				report.add_formatted ("%ld", (*fragment)->id);
				report.add_formatted ("\t%.1lf", (*fragment)->attributes_numeric [0]);
				report.add_formatted ("\t%.1lf", (*fragment)->attributes_numeric [1]);
				report.add_formatted ("\t%.4lf\t%.4lf\t%.4lf\t%.4lf\n",
					(*fragment)->attributes_numeric[2], (*fragment)->attributes_numeric[3], (*fragment)->attributes_numeric[4], (*fragment)->attributes_numeric[5]);
			}
			log += report;
			// write_log (filename_report, report);
		}

	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}

int relevent_wind_speed
	(map_object *within_0_2,
	map_object *within_3_5,
	const double surround_0_2_m,
	const double surround_3_5_m,
	const double ignore_cutoff_km2,
	const int area_attribute_index,
	dynamic_string &reason)

// If surrounding polygon < ignore_cutoff_km2, fall back to the other

{
	int choice = -1;
	reason.clear ();

	if ((within_0_2)
	&& (within_3_5)) {
		// This point is enclosed by both wind speeds
		if (surround_0_2_m < surround_3_5_m) {
			if (within_0_2->attributes_numeric [area_attribute_index] >= ignore_cutoff_km2)
				// 0-2 closest surrounding
				choice = 1;
			else {
				// 0-2 closest surrounding, but it's smaller than cutoff
				choice = 2;
				reason = "0-2 too small.";
			}
		}
		else {
			if (within_3_5->attributes_numeric [area_attribute_index] >= ignore_cutoff_km2)
				// 3-5 closest surrounding
				choice = 2;
			else {
				// 3-5 closest surrounding, but it's smaller than cutoff
				choice = 1;
				reason = "3-5 too small.";
			}
		}
	}
	else {
		if (within_0_2) {
			reason = "Enclosed only by 0-2.";
			if (within_0_2->attributes_numeric[area_attribute_index] >= ignore_cutoff_km2)
				choice = 1;
			else
				reason += " 0-2 too small.";
		}
		else
			if (within_3_5) {
				reason = "Enclosed only by 3-5.";
				if (within_3_5->attributes_numeric[area_attribute_index] >= ignore_cutoff_km2)
					choice = 2;
				else
					reason += " 3-5 too small.";
			}
			else
				reason = "Not enclosed.";
	}

	return choice;
}

void frame_CARDGIS::On_WindDistances
	(wxCommandEvent &)

// 2022-03-03 Nieyan: Compute distances from points to wind speed areas

{
	ClipperBuffer clipper;
	dynamic_string log, filename_wind_0_2, filename_wind_3_5, filename_points;
	map_layer *layer_points, *layer_wind_0_2, *layer_wind_3_5;
	double ignore_cutoff_km2;

	filename_points = "I:\\TDC\\FarmlandFinder\\wind speed\\farm_point\\farm_points.shp";

	filename_wind_0_2 = "h:\\scratch\\FarmlandFinder\\wind speed\\wind_speed_0_2.shp";
	filename_wind_3_5 = "h:\\scratch\\FarmlandFinder\\wind speed\\wind_speed_3_5.shp";

	ignore_cutoff_km2 = 300.0; // 300.0; // 190.0

	// dialog_project dj (this, &first_sales_record, &last_sales_record, &number_of_threads, &first_state_fips, &last_state_fips);
	// if (dj.ShowModal () == wxID_OK) {
	{

		interface_window_wx view;
		view.start_progress (this);

		{
			importer_shapefile importer;
			view.update_progress ("Reading Points");
			importer.filename_source = filename_points;
			// importer.id_field_name = "ID";
			importer.name_field_name = "ID_TEXT";
			importer.projection_from_prj_file = true;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			layer_points = map_watershed->create_new (MAP_OBJECT_POINT);
			layer_points->name = importer.filename_source;
			layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
			layer_points->symbol = MAP_OBJECT_SYMBOL_PLUS;
			layer_points->color = RGB (0, 255, 0);
			map_watershed->layers.push_back (layer_points);

			panel_watershed->pause_rendering = true;
			importer.import (layer_points, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;

			// layer_points->objects.resize (1000);
		}

		{
			importer_shapefile importer;
			view.update_progress ("Reading Wind 0-2");
			importer.filename_source = filename_wind_0_2;
			importer.id_field_name = "ID";
			importer.projection_from_prj_file = true;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			layer_wind_0_2 = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_wind_0_2->name = importer.filename_source;
			layer_wind_0_2->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_wind_0_2->color = RGB (0, 0, 255);
			map_watershed->layers.push_back (layer_wind_0_2);

			panel_watershed->pause_rendering = true;
			importer.import (layer_wind_0_2, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}

		{
			importer_shapefile importer;
			view.update_progress ("Reading Wind 3-5");
			importer.filename_source = filename_wind_3_5;
			importer.id_field_name = "ID";
			importer.projection_from_prj_file = true;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			layer_wind_3_5 = map_watershed->create_new (MAP_OBJECT_POLYGON);
			layer_wind_3_5->name = importer.filename_source;
			layer_wind_3_5->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_wind_3_5->color = RGB (128, 0, 0);
			map_watershed->layers.push_back (layer_wind_3_5);

			panel_watershed->pause_rendering = true;
			importer.import (layer_wind_3_5, NULL, map_watershed, &view, log);
			panel_watershed->pause_rendering = false;
		}

		// Add & compute area attribute for all polygons
		clipper.add_area_attribute (layer_wind_0_2, log);
		clipper.add_area_attribute (layer_wind_3_5, log);

		std::vector <map_object *>::const_iterator point;
		// device_coordinate center;
		// bounding_cube coordinate_hint;
		map_object *within_0_2, *within_3_5, *nearby_0_2, *nearby_3_5;
		int area_attribute_index, choice;
		double surround_0_2_m, surround_3_5_m, distance_0_2_m, distance_3_5_m;
		dynamic_string reason;

		MapLayerProximity proximater_0_2, proximater_3_5;
		ProximityLayer proxy;
		std::map <long, ProximityLayerResult> nearest_0_2, nearest_3_5, enclosing_0_2, enclosing_3_5;
		std::map <long, ProximityLayerResult>::iterator point_result;
		std::vector <std::pair <map_layer *, ProximityResult>>::iterator layer_nearness;

		log.add_formatted ("Cutoff size\t%.4lf\tkm^2\n", ignore_cutoff_km2);

		log += "ID\tName\tLatitude\tLongitude";
		log += "\tInside\tDistance, km";
		log += "\tNearest Surrounding 0-2\tBoundary Distance, km\tSize, km ^ 2";
		log += "\tNearest Surrounding 3 - 5\tBoundary Distance, km\tSize, km ^ 2";
		log += "\tNearest 0-2\tDistance, km\tSize, km^2";
		log += "\tNearest 3 - 5\tDistance, km\tSize, km ^ 2";
		log += "\tNote\n";

		area_attribute_index = layer_wind_0_2->attribute_count_numeric - 1;

		proximater_0_2.base_layer = layer_points;
		proxy.layer = layer_wind_0_2;
		proximater_0_2.proximity_layers.push_back (proxy);

		proximater_3_5.base_layer = layer_points;
		proxy.layer = layer_wind_3_5;
		proximater_3_5.proximity_layers.push_back (proxy);

		proximater_0_2.compute_enclosure (&enclosing_0_2, &view, log);
		proximater_0_2.compute_proximity (&nearest_0_2, &view, log);

		proximater_3_5.compute_enclosure (&enclosing_3_5, &view, log);
		proximater_3_5.compute_proximity (&nearest_3_5, &view, log);

		for (point = layer_points->objects.begin ();
		point != layer_points->objects.end ();
		++point) {
			/*
			(*point)->centroid (&center);

			coordinate_hint.x[0] = center.x - 0.0005;
			coordinate_hint.y[0] = center.y - 0.0005;
			coordinate_hint.x[1] = coordinate_hint.x[0] + 0.001;
			coordinate_hint.y[1] = coordinate_hint.y[0] + 0.001;

			within_0_2 = layer_wind_0_2->find_layer_object_surrounding (center, &coordinate_hint);
			if (within_0_2)
				surround_0_2_m = within_0_2->closest_point_meters (center, &point_index);
			else
				surround_0_2_m = 0.0;
			within_3_5 = layer_wind_3_5->find_layer_object_surrounding (center, &coordinate_hint);
			if (within_3_5)
				surround_3_5_m = within_3_5->closest_point_meters (center, &point_index);
			else
				surround_3_5_m = 0.0;

			nearest_0_2 = layer_wind_0_2->find_nearest_object (center, &distance_0_2_m);
			nearest_3_5 = layer_wind_3_5->find_nearest_object (center, &distance_3_5_m);
			*/

			// Nearest Enclosed, if any, layer_wind_0_2
			point_result = enclosing_0_2.find ((*point)->id);
			layer_nearness = point_result->second.layers.begin ();
			within_0_2 = layer_nearness->second.object;
			surround_0_2_m = layer_nearness->second.distance_m;

			// Nearest Enclosed, if any, layer_wind_3_5
			point_result = enclosing_3_5.find ((*point)->id);
			layer_nearness = point_result->second.layers.begin ();
			within_3_5 = layer_nearness->second.object;
			surround_3_5_m = layer_nearness->second.distance_m;

			// Nearest Not Enclosed, layer_wind_0_2
			point_result = nearest_0_2.find ((*point)->id);
			layer_nearness = point_result->second.layers.begin ();
			nearby_0_2 = layer_nearness->second.object;
			distance_0_2_m = layer_nearness->second.distance_m;

			// Nearest Not Enclosed, layer_wind_3_5
			point_result = nearest_3_5.find ((*point)->id);
			layer_nearness = point_result->second.layers.begin ();
			nearby_3_5 = layer_nearness->second.object;
			distance_3_5_m = layer_nearness->second.distance_m;

			log.add_formatted ("%ld\t", (*point)->id);
			log += (*point)->name;
			log.add_formatted ("\t%ld\t%ld", (*point)->latitude, (*point)->longitude);

			choice = relevent_wind_speed (within_0_2, within_3_5,
			surround_0_2_m, surround_3_5_m,
			ignore_cutoff_km2, area_attribute_index, reason);
			
			if (choice == 1) {
				log += "\t'0-2";
				log.add_formatted ("\t%.4lf", surround_0_2_m / 1000.0);
			}
			else 
				if (choice == 2) {
					log += "\t'3-5";
					log.add_formatted ("\t%.4lf", surround_3_5_m / 1000.0);
				}
				else
					log += "\t\t";

			if (within_0_2) {
				log.add_formatted ("\t%ld\t%.4lf\t%.4lf", within_0_2->id,
				surround_0_2_m / 1000.0,
				within_0_2->attributes_numeric[area_attribute_index]);
			}
			else
				log += "\t\t\t";

			if (within_3_5) {
				log.add_formatted ("\t%ld\t%.4lf\t%.4lf", within_3_5->id,
				surround_3_5_m / 1000.0,
				within_3_5->attributes_numeric[area_attribute_index]);
			}
			else
				log += "\t\t\t";

			log.add_formatted ("\t%ld\t%.4lf\t%.4lf", nearby_0_2->id,
			distance_0_2_m / 1000.0,
			nearby_0_2->attributes_numeric [area_attribute_index]);

			log.add_formatted ("\t%ld\t%.4lf\t%.4lf", nearby_3_5->id,
			distance_3_5_m / 1000.0,
			nearby_3_5->attributes_numeric [area_attribute_index]);

			log += "\t";
			if (reason.get_length () > 0)
				log += reason;

			log += "\n";
		}

		map_watershed->check_extent ();
		panel_watershed->change_layers ();
		enable_map_controls ();
		panel_watershed->redraw ("OnOpenShapefile");

		view.update_status_bar ("Map Updated");
	}

	log.convert_linefeeds_for_CEdit ();
	dialog_error_list d (this, L"File Open", L"", log);
	d.ShowWindowModal ();
}


void frame_CARDGIS::OnLayerObjectTable
	(wxCommandEvent &)

{
	dynamic_string log, row_name, number_text;
	Ledger preview_rows;
	int attribute_index, selected_row_index;
	std::vector <map_object *>::const_iterator object;
	std::vector <dynamic_string> row;
	std::vector <dynamic_string>::const_iterator attribute_name;
	long lat, lon;

	map_layer *layer = panel_watershed->layer_selection_source;

	if (layer) {

		SetCursor (*wxHOURGLASS_CURSOR);

		preview_rows.column_names.push_back ("Name");
		preview_rows.column_names.push_back ("Center Latitude");
		preview_rows.column_names.push_back ("Center Longitude");
		preview_rows.column_names.push_back ("Type");
		// preview_rows.column_names.push_back ("Area m2");

		for (attribute_name = layer->column_names_numeric.begin ();
		attribute_name != layer->column_names_numeric.end ();
		++attribute_name)
			preview_rows.column_names.push_back (*attribute_name);

		for (attribute_name = layer->column_names_text.begin ();
		attribute_name != layer->column_names_text.end ();
		++attribute_name)
			preview_rows.column_names.push_back (*attribute_name);

		for (object = layer->objects.begin ();
		object != layer->objects.end ();
		++object) {

			row_name.format ("%lld", (*object)->id);

			row.clear ();
			row.push_back ((*object)->name);

			// lat & long
			(*object)->centroid (&lat, &lon);
			number_text.format ("%.4lf", (double) lat / 1.0e6);
			row.push_back (number_text);
			number_text.format ("%.4lf", (double) lon / 1.0e6);
			row.push_back (number_text);

			// Object Type
			if ((*object)->type == MAP_OBJECT_POINT)
				row.push_back ("Point");
			else
				if ((*object)->type == MAP_OBJECT_POLYGON)
					row.push_back ("Polygon");
				else
					if ((*object)->type == MAP_OBJECT_POLYGON_COMPLEX)
						row.push_back ("Complex Polygon");
					else
						if ((*object)->type == MAP_OBJECT_VECTOR)
							row.push_back ("Vector");
						else
							if ((*object)->type == MAP_OBJECT_TIFF)
								row.push_back ("TIFF");
							else
								row.push_back ("Other");


			// Attributes Numeric
			for (attribute_index = 0; attribute_index < layer->attribute_count_numeric; ++attribute_index) {
				number_text.format ("%.4lf", (*object)->attributes_numeric [attribute_index]);
				row.push_back (number_text);
			}

			// Attributes Text
			for (attribute_index = 0; attribute_index < layer->attribute_count_text; ++attribute_index)
				row.push_back ((*object)->attributes_text [attribute_index]);

			preview_rows.add_row (row_name, row);
		}
		SetCursor (wxNullCursor);

		// see dialog_commodity::show_ledger from CLRLUM
		selected_row_index = -1;
		dialog_ledger dl (this, &preview_rows, &selected_row_index, layer->name);
		if (dl.ShowModal () == wxID_OK) {

			if (selected_row_index != -1) {
				map_object* selected_object;
				std::set <long long> new_selection;
				selected_object = layer->objects [selected_row_index];
				edit_select_id->Set (selected_object->id);
				new_selection.insert (selected_object->id);

				panel_watershed->replace_selection (&new_selection, log);
			}
		}

	}

}

void zillow_powerline_base_point_callback
	(const map_object* point,
	const char delimiter,
	dynamic_string& log)

{
	// ImportParcelID is 2nd numeric, but 1st numeric is used as ID
	log.add_formatted("%.1lf", point->attributes_numeric [0]);
}

void zillow_powerline_proximity_callback
	(const map_object* point,
	const double distance_m,
	const char delimiter,
	dynamic_string& log)

// Called by work_MapLayerProximity
// Converts distance to miles

{
	// ID\tStreamLevel\tFtype\tDistance
	log += delimiter;
	log.add_formatted("%ld", point->id);

	log += delimiter;
	log.add_formatted("%.6lf", distance_m / METERS_PER_MILE);
}

void zillow_powerline_proximity_column_headers_callback
	(const char delimiter,
	dynamic_string& column_names)
{
	column_names.clear();
	column_names += delimiter;
	column_names += "\"ObjectID\"";
	column_names += delimiter;
	column_names += "\"Distance, miles\"";
}

void frame_CARDGIS::OnZillowPowerlineTable
	(wxCommandEvent&)

// 2022-04-04 Nieyan Chang: Find all transmission lines within 10 miles of Zillow Sales

{
	dynamic_string log, filename_powerlines, id_column_name;
	dynamic_string path_sales, filename_sales, filename_output, path_output;
	map_layer* layer_sales = NULL, * layer_powerlines;
	BYTE csv_file_format;
	bool normalize_longitude, numeric_id, error = false;
	std::set <int> text_column_indexes, skip_column_indexes;
	int id_column_index, lat_column_index, lon_column_index, sales_file_index;
	int number_of_threads;

	dynamic_string prefix_sales[6] = {"house_17", "house_19", "house_20", "house_27", "house_29", "house_31"};
	number_of_threads = 24;

	// filename_powerlines = "I:\\TDC\\FarmlandFinder\\transmission lines\\zillow houses\\transmission line shapefiles\\eletricity_transmission.shp";
	filename_powerlines = "f:\\scratch\\Zillow\\transmission line shapefiles\\ElectricityTransmissionWGS1984.shp";
	// Farmland Finder data
	path_sales = "I:\\TDC\\FarmlandFinder\\transmission lines\\zillow houses\\zillow house data\\house_clearing\\";
	path_output = "f:\\scratch\\Zillow\\";
	id_column_name = "TransID"; // "ImportParcelID" is a numeric attribute that will be used by zillow_powerline_base_point_callback

	csv_file_format = CSV_POINT_FORMAT_AUTOMATIC;
	// lat_column_index = 1;
	// lon_column_index = 2;
	normalize_longitude = false;
	numeric_id = false; // First TransID is 432098124
	Ledger preview_rows;

	filename_sales = path_sales;
	filename_sales += prefix_sales[0];
	filename_sales += ".csv";

	dialog_import_points dip(this, &filename_sales, id_column_name, map_watershed, &csv_file_format, &normalize_longitude, &numeric_id, &preview_rows, &text_column_indexes, &skip_column_indexes,
	&id_column_index, &lat_column_index, &lon_column_index);
	if (dip.ShowModal() == wxID_OK) {

		interface_window_wx view;
		view.start_progress(this);

		view.update_progress ("Reading Shapefiles");
		{
			importer_shapefile importer;
			layer_powerlines = map_watershed->create_new(MAP_OBJECT_POLYGON);
			layer_powerlines->name = "Transmission";
			layer_powerlines->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_powerlines->color = RGB(0, 0, 255);
			map_watershed->layers.push_back (layer_powerlines);

			importer.filename_source = filename_powerlines;
			importer.id_field_name = "OBJECTID";
			importer.projection = SHAPEFILE_PROJECTED_LAT_LONG;
			importer.normalize_longitude = false;
			importer.take_dbf_columns = true;

			if (importer.import(layer_powerlines, NULL, map_watershed, &view, log)) {
				map_watershed->set_extent();
				log.add_formatted("Powerline layer size : %d\n", layer_powerlines->objects.size());
			}
		}

		MapLayerProximity proximator_hsu;

		proximator_hsu.delimiter = ',';
		proximator_hsu.write_point_function = &zillow_powerline_base_point_callback;
		proximator_hsu.base_layer_column_headers = "\"ImportParcelID\"";
		proximator_hsu.thread_count = number_of_threads;

		if (!read_schools_hospitals_universities(layer_sales, &proximator_hsu, &view, log))
			error = true;

		// for (sales_file_index = 0; sales_file_index < 6; ++sales_file_index) {
		{	sales_file_index = 4;

			if (layer_sales) {
				map_watershed->detach (layer_sales);
				delete layer_sales;
			}

			filename_sales = path_sales;
			filename_sales += prefix_sales[sales_file_index];
			filename_sales += ".csv";
			view.update_progress_formatted(1, "Reading Points %d", sales_file_index);

			layer_sales = read_point_layer(filename_sales, -1, normalize_longitude, numeric_id, csv_file_format, &preview_rows.column_names, &text_column_indexes, &skip_column_indexes,
			id_column_index, lat_column_index, lon_column_index, log);

			if (false) {
				MapLayerProximity proximator_electric;

				proximator_electric.thread_count = number_of_threads;
				proximator_electric.delimiter = ',';
				filename_output = path_output;
				filename_output += "Zillow_";
				filename_output += prefix_sales[sales_file_index];
				filename_output += "_Electricity_10mile.csv";
				proximator_electric.output_filename = filename_output;

				proximator_electric.base_layer = layer_sales;
				proximator_electric.write_point_function = &zillow_powerline_base_point_callback;
				{
					ProximityLayer proxy;
					proxy.layer = layer_powerlines;
					proxy.write_layer_point = zillow_powerline_proximity_callback;
					proxy.write_column_headers = zillow_powerline_proximity_column_headers_callback;
					proximator_electric.proximity_layers.push_back(proxy);
				}
				proximator_electric.base_layer_column_headers = "\"ImportParcelID\""; // , \"Name\",\"Address1\",\"Address2\",\"City\",\"State\",\"Zip\",\"Zip4\"";

				proximator_electric.cutoff_radius_miles = 10.0;
				proximator_electric.write_proximity_table_within_radius(&view, log);
			}

			proximator_hsu.output_filename = path_output;
			proximator_hsu.output_filename += prefix_sales[sales_file_index];
			proximator_hsu.output_filename += " Hospital School University.csv";

			proximator_hsu.base_layer = layer_sales;
//			proximator_hsu.write_proximity_table(&view, log);
		}

		map_watershed->check_extent();
		panel_watershed->change_layers();
		enable_map_controls();
		panel_watershed->redraw("Zillow");

		view.update_status_bar("Map Updated");
		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"File Open", L"", log);
		d.ShowWindowModal();
	}
}

void hospital_school_university_callback
	(const map_object* point,
	const double distance_m,
	const char delimiter,
	dynamic_string& log)

	// Called by work_MapLayerProximity

{
	log += delimiter;
	log += point->attributes_text[0]; // HOS_ID, SCH_ID, UNIV_ID

	log += delimiter;
	log.add_formatted("%.6lf", distance_m / METERS_PER_MILE);
}

void hospital_column_headers_callback
(const char delimiter,
	dynamic_string& column_names)
{
	column_names.clear();
	column_names += delimiter;
	column_names += "\"Hos_ID\"";
	column_names += delimiter;
	column_names += "\"Distance, miles\"";
}

void school_column_headers_callback
	(const char delimiter,
	dynamic_string& column_names)
{
	column_names.clear();
	column_names += delimiter;
	column_names += "\"Sch_ID\"";
	column_names += delimiter;
	column_names += "\"Distance, miles\"";
}

void university_column_headers_callback
	(const char delimiter,
	dynamic_string& column_names)
{
	column_names.clear();
	column_names += delimiter;
	column_names += "\"Univ_ID\"";
	column_names += delimiter;
	column_names += "\"Distance, miles\"";
}

bool frame_CARDGIS::read_schools_hospitals_universities
	(map_layer* layer_zillow,
	MapLayerProximity *proximator,
	interface_window* view,
	dynamic_string& log)

// Read in hospitals, schools, universities for all states
// called from OnZillowPowerlineTable

{
	map_layer* cleanup_layer, *layer_hospitals, *layer_schools, *layer_universities, *layer_state;
	std::vector<map_object*>::iterator site;
	dynamic_string filename_shu;
	bool error = false;
	int state_fips, layer_count;
	char abbrev[25];
	ProximityLayer proxy;

	// Delete schools_hospitals_universities from previous state
	if ((cleanup_layer = map_watershed->match("Hospitals")) != NULL)
		map_watershed->remove (cleanup_layer);
	if ((cleanup_layer = map_watershed->match("Schools")) != NULL)
		map_watershed->remove(cleanup_layer);
	if ((cleanup_layer = map_watershed->match("Universities")) != NULL)
		map_watershed->remove(cleanup_layer);

	layer_hospitals = map_watershed->create_new(MAP_OBJECT_POINT);
	layer_schools = map_watershed->create_new(MAP_OBJECT_POINT);
	layer_universities = map_watershed->create_new(MAP_OBJECT_POINT);

	for (state_fips = 1, layer_count = 1;
	!error
	&& (state_fips <= 56);
	++state_fips, ++layer_count) {

		state_abbreviation(state_fips, abbrev, 25);

		if (abbrev[0] != '\0') {
			filename_shu.format("State_%02d.shp", state_fips);

			// Hos_ID,C,80	State,C,80	fips,C,80
			layer_state = map_watershed->create_new(MAP_OBJECT_POINT);
			if (proximator->read_shapefile("I:\\TDC\\Pipeline_Accidents\\school_hospital_university_shapefiles\\Hospital\\", filename_shu, "", "",
			layer_state, map_watershed, view, log)) {
				if (layer_count == 1)
					layer_hospitals->copy_attributes_only(layer_state);
				layer_hospitals->copy_objects (layer_state);

				delete layer_state;
			}
			else
				error = true;

			// Sch_ID,C,80	State,C,80	fips,C,80
			layer_state = map_watershed->create_new(MAP_OBJECT_POINT);
			if (proximator->read_shapefile("I:\\TDC\\Pipeline_Accidents\\school_hospital_university_shapefiles\\PubSchool\\", filename_shu, "", "",
			layer_state, map_watershed, view, log)) {
				if (layer_count == 1)
					layer_schools->copy_attributes_only(layer_state);
				layer_schools->copy_objects(layer_state);

				delete layer_state;
			}
			else
				error = true;
			// Univ_ID,C,80	State,C,80	fips,C,80
			layer_state = map_watershed->create_new(MAP_OBJECT_POINT);
			if (proximator->read_shapefile("I:\\TDC\\Pipeline_Accidents\\school_hospital_university_shapefiles\\University\\", filename_shu, "", "",
			layer_state, map_watershed, view, log)) {
				if (layer_count == 1)
					layer_universities->copy_attributes_only(layer_state);
				layer_universities->copy_objects(layer_state);

				delete layer_state;
			}
			else
				error = true;
		}
	}
	if (!error) {
		long count_hospitals, count_schools, count_universities;
		int color_index = map_watershed->layers.size();
		COLORREF* proximity_pallette;
		proximity_pallette = make_palette_10();

		count_hospitals = count_schools = count_universities = 0;

		// Each state gets IDs from 1..n
		// Renumber to prevent ID collisions
		for (site = layer_hospitals->objects.begin();
		site != layer_hospitals->objects.end();
		++site)
			(*site)->id = ++count_hospitals;

		for (site = layer_schools->objects.begin();
		site != layer_schools->objects.end();
		++site)
			(*site)->id = ++count_schools;

		for (site = layer_universities->objects.begin();
		site != layer_universities->objects.end();
		++site)
			(*site)->id = ++count_universities;

		log.add_formatted("Hospital layer size\t%ld\n", count_hospitals);
		log.add_formatted("School layer size\t%ld\n", count_schools);
		log.add_formatted("University layer size\t%ld\n", count_universities);

		layer_hospitals->name = "Hospitals";
		layer_hospitals->color = proximity_pallette[color_index++];
		map_watershed->layers.push_back(layer_hospitals);
		proxy.layer = layer_hospitals;
		proxy.write_layer_point = hospital_school_university_callback;
		proxy.write_column_headers = hospital_column_headers_callback;
		proximator->proximity_layers.push_back(proxy);

		layer_schools->name = "Schools";
		layer_schools->color = proximity_pallette[color_index++];
		map_watershed->layers.push_back(layer_schools);
		proxy.layer = layer_schools;
		proxy.write_layer_point = hospital_school_university_callback;
		proxy.write_column_headers = school_column_headers_callback;
		proximator->proximity_layers.push_back(proxy);

		layer_universities->name = "Universities";
		layer_universities->color = proximity_pallette[color_index++];
		map_watershed->layers.push_back(layer_universities);
		proxy.layer = layer_universities;
		proxy.write_layer_point = hospital_school_university_callback;
		proxy.write_column_headers = university_column_headers_callback;
		proximator->proximity_layers.push_back(proxy);

		delete[] proximity_pallette;
	}

	return !error;

}

bool frame_CARDGIS::read_schools_hospitals_universities_state
	(const int state_fips,
	const int number_of_threads,
	const dynamic_string& path_output,
	const dynamic_string& prefix_output,
	map_layer* layer_zillow,
	interface_window* view,
	dynamic_string& log)

// Creates proximity tables from layer_zillow to each of 3 other layers

// Called from On_Zillow_Schools_Hospitals

{
	map_layer* cleanup_layer;
	dynamic_string filename_shu;
	bool error = false;
	int within_check = PROXIMITY_WITHIN_POLYGON_DONT_DETECT;
	char abbrev[25];

	// Delete schools_hospitals_universities from previous state
	if ((cleanup_layer = map_watershed->match("Hospitals")) != NULL)
		map_watershed->remove(cleanup_layer);
	if ((cleanup_layer = map_watershed->match("Schools")) != NULL)
		map_watershed->remove(cleanup_layer);
	if ((cleanup_layer = map_watershed->match("Universities")) != NULL)
		map_watershed->remove(cleanup_layer);

	MapLayerProximity proximator;

	proximator.delimiter = ',';

	proximator.output_filename = path_output;
	// proximator.output_filename += "Zillow to school_hospital_university";
	// proximator.output_filename += incident_name[incident_type - 1];
	// proximator.output_filename += "Hospital School University ";
	proximator.output_filename += prefix_output;
	proximator.output_filename += " Hospital School University ";
	proximator.output_filename.add_formatted(" State %02d.csv", state_fips);

	state_abbreviation(state_fips, abbrev, 25);

	if (abbrev[0] != '\0') {
		filename_shu.format("State_%02d.shp", state_fips);
		// Hos_ID,C,80	State,C,80	fips,C,80
		if (proximator.read_proximity_shapefile("I:\\TDC\\Pipeline_Accidents\\school_hospital_university_shapefiles\\Hospital\\", filename_shu, "Hospitals", "", "",
		map_watershed, view, log)
		// Sch_ID,C,80	State,C,80	fips,C,80
		&& proximator.read_proximity_shapefile("I:\\TDC\\Pipeline_Accidents\\school_hospital_university_shapefiles\\PubSchool\\", filename_shu, "Schools", "", "",
		map_watershed, view, log)
		// Univ_ID,C,80	State,C,80	fips,C,80
		&& proximator.read_proximity_shapefile("I:\\TDC\\Pipeline_Accidents\\school_hospital_university_shapefiles\\University\\", filename_shu, "Universities", "", "",
		map_watershed, view, log)) {

			proximator.base_layer = layer_zillow;
			proximator.write_point_function = &zillow_point_callback_3;
			if (proximator.delimiter == '\t')
				proximator.base_layer_column_headers = "ID\tRow ID\tTrans ID\tImportParcelID";
			else
				proximator.base_layer_column_headers = "\"ID\",\"Row ID\",\"Trans ID\",\"ImportParcelID\"";

			proximator.thread_count = number_of_threads;
			proximator.write_proximity_table(within_check, -1, view, log);
		}
		else
			error = true;
	}

	return !error;
}

void frame_CARDGIS::On_Zillow_Schools_Hospitals
	(wxCommandEvent&)

// 2021-05-06 Nieyang Cheng: show distances & ids from Zillow sales to I:\TDC\Pipeline_Accidents\school_hospital_university_shapefiles

{
	dynamic_string log, area_name, path_pipeline, filename_county_master, path_zillow_csv, prefix_output;
	ZillowSales zillow;
	CountyMaster counties;
	PipelineAccidents pipes;
	int number_of_threads = 20, first_sales_record, last_sales_record;
	std::chrono::system_clock::time_point start_time, end_time;
	std::chrono::duration <double, std::milli> elapsed_milliseconds;
	int first_state_fips, last_state_fips, state_fips;
	map_layer* cleanup_layer;

	first_state_fips = last_state_fips = 1;
	first_sales_record = 1;
	last_sales_record = 5000000;

	path_pipeline = app->project_root; // F:\scratch\Pipelines

	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb";
	path_zillow_csv = path_pipeline + "housing_transactions\\";

	// 2022-04-06
	path_pipeline = "I:\\TDC\\FarmlandFinder\\transmission lines\\zillow houses\\zillow house data\\house_clearing\\";
	dynamic_string prefix_sales[6] = { "house_17", "house_19", "house_20", "house_27", "house_29", "house_31" };
	path_zillow_csv = path_pipeline;
	path_zillow_csv += prefix_sales[0];
	path_zillow_csv += ".csv";

	dialog_project dj(this, &first_sales_record, &last_sales_record, &number_of_threads, &first_state_fips, &last_state_fips);
	if (dj.ShowModal() == wxID_OK) {

		interface_window_wx view;

		view.start_progress(this);
		/*
		dialog_run_threads *update_display = NULL;

		update_display = new dialog_run_threads (this, true, false);
		update_display->Show ();
		update_display->setup_interface_window (app, &view);
		*/

		start_time = std::chrono::system_clock::now();

		counties.read(filename_county_master.get_text_ascii(), &view, log);

		for (state_fips = first_state_fips; state_fips <= last_state_fips; ++state_fips) {

			// Delete Zillow sales from previous state
			if ((cleanup_layer = map_watershed->match("Zillow Sales")) != NULL)
				map_watershed->remove(cleanup_layer);

			// Create a point map layer of sales from Zillow data
			if (zillow.read_csv_format_3(path_zillow_csv, state_fips, first_sales_record, last_sales_record, &counties, map_watershed, &view, log)) {
				if (last_sales_record < (first_sales_record + (int)zillow.layer_zillow[0]->objects.size() - 1))
					prefix_output.format ("Zillow to Hospital School University State %02d %07d-%07d.csv", state_fips, first_sales_record, last_sales_record);
				else
					prefix_output.format ("Zillow to Hospital School University State %02d %07d-%07d.csv", state_fips, first_sales_record, first_sales_record + (int)zillow.layer_zillow[0]->objects.size() - 1);

				read_schools_hospitals_universities_state (state_fips, number_of_threads, path_pipeline, prefix_output, zillow.layer_zillow[0], &view, log);
			}
		}

		end_time = std::chrono::system_clock::now();
		elapsed_milliseconds = end_time - start_time;
		log.add_formatted("Time Elapsed\t%.3lf\tseconds\n", (double)elapsed_milliseconds.count() / 1000.0);

		/*
		update_display->Destroy ();
		delete update_display;
		*/

		map_watershed->check_extent();
		panel_watershed->change_layers();
		enable_map_controls();
		panel_watershed->redraw("LAGOS");

		view.update_status_bar("Map Updated");
	}

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"File Open", L"", log);
	d.ShowWindowModal();
}

void frame_CARDGIS::OnMatchAFIDA
	(wxCommandEvent&)

// 2022-04-13 Nieyan: match foreign sales records without locations to detailed sales records

// 1 Both AFIDA and FarmlandFinder lists are filtered to state or counties awdjacent to the state.
// 2 AFIDA records with valid FIPS are added to a container's foreign vector once for the record's county.
// 3 AFIDA records with valid FIPS are added to a container's foreign vector for each adjacent county.
// 4 AFIDA records with valid FIPS are added to a container's foreign vector for each county sharing the same CRD.
// 5 FarmlandFinder sales with valid FIPS are added to a container's domestic vector once for the record's county.
// 6 FarmlandFinder sales with valid FIPS are added to a container's domestic vector for each adjacent county.
// 7 FarmlandFinder sales with valid FIPS are added to a container's domestic vector for each county sharing the same CRD.
// 8 The container's foreign vector and domestic vector are traversed comparing each pair that maches by year and CRD or county.
// 9 If acres, tillable flag, high wind flagand investor flag are identical, an exact match is reported.
// 10 A score is created using weighted differences in each of these values, and matches are reported by that value.

{
	dynamic_string log, filename_afida, filename_county_master, filename_ff, filename_county_shp, filename_CRD, filename_county_wind, filename_output;
	CountyMaster counties;
	// int number_of_threads = 20;
	// int first_state_fips, last_state_fips;
	AFIDA foreign_sales;
	map_layer* layer_county;
	std::map <int, std::set <int>> adjacent_counties;
	int *CRD_by_county;
	std::set <int> run_states;
	std::set <int>::iterator state_fips;
	char abbreviation_holder [3];

	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb";
	// filename_afida = "E:\\GIS\\AFIDA\\AFIDA_Database_updated_Iowa.csv";
	// filename_afida = "E:\\GIS\\AFIDA\\AFIDA_Database_updated_Kansas.csv";
	// filename_afida = "I:\\TDC\\AFIDA\\2022-09-19\\AFIDA_all_states_0922.csv";
	filename_afida = "F:\\FarmlandFinder\\AFIDA\\AFIDA_all_states_0922.csv";
	// filename_kansas = "E:\\GIS\\AFIDA\\Kansas_Analysis_Data.csv";
	// filename_iowa = "E:\\GIS\\AFIDA\\Iowa_Ag_Sales_append_all.csv";
	filename_ff = "I:\\TDC\\AFIDA\\2022-09-19\\FarmlandFinder_use_to_match_AFIDA.csv";
	// filename_CRD = "I:\\TDC\\AFIDA\\sheets\\Iowa_Kansas_CRD_County.csv";
	filename_CRD = "I:\\TDC\\AFIDA\\2022-09-19\\CRD_all states.csv";
	filename_county_wind = "I:\\TDC\\AFIDA\\2022-09-19\\State_average_wind_speed_all.csv";

	run_states.insert(17);
	run_states.insert(18);
	run_states.insert(19);
	run_states.insert(20);
	run_states.insert(26);
	run_states.insert(27);
	run_states.insert(29);
	run_states.insert(31);
	run_states.insert(38);
	// run_states.insert(39); // Ohio
	run_states.insert(46);
	run_states.insert(55);

	// interface_window_wx view;

	// view.start_progress(this);
	dialog_run_threads* update_display = NULL;
	interface_window_wx view;

	update_display = new dialog_run_threads(this, true, false);
	update_display->Show();
	update_display->setup_interface_window(app, &view);

	counties.read(filename_county_master.get_text_ascii(), &view, log);

	CRD_by_county = read_CropReportingDistricts (filename_CRD, &foreign_sales.max_county_fips, &view, log);

	filename_county_shp = "r:\\Census\\TIGER 2012\\County\\tl_2012_us_county\\tl_2012_us_county.shp";

	layer_county = map_watershed->create_new(MAP_OBJECT_POLYGON);
	layer_county->name = "County Boundaries";
	layer_county->initialize_attributes = true;
	layer_county->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_county->color = RGB(0, 255, 0);
	map_watershed->layers.push_back(layer_county);

	// std::set <int> state_set;
	// state_set.insert(30);
	read_county_boundaries (filename_county_shp, map_watershed, layer_county, NULL, &view, log);
	find_adjacent_counties (layer_county, &adjacent_counties, false, &view, log);

	if (foreign_sales.read_county_wind(filename_county_wind, &counties, log)) {
		if (foreign_sales.read_FarmlandFinder_2022_09_22(filename_ff, &counties, CRD_by_county, log)) {
			for (state_fips = run_states.begin ();
			state_fips != run_states.end (); 
			++state_fips) {
				if (foreign_sales.read(filename_afida, *state_fips, &counties, CRD_by_county, log)) {
					filename_output = "f:\\farmlandfinder\\afida\\2023-01-24\\AFIDA ";
					filename_output += state_abbreviation (*state_fips, abbreviation_holder, 3);
					filename_output += ".csv";

					// if (foreign_sales.read_Kansas(filename_ff, &counties, log))
					//	foreign_sales.county_report(20, &counties, &adjacent_counties, &CRD_by_county, 3, &view, log);

					// if (foreign_sales.read_Iowa(filename_ff, &counties, log))
					//	foreign_sales.county_report(19, &counties, &adjacent_counties, &CRD_by_county, 3, &view, log);

					foreign_sales.county_report(filename_output, *state_fips, &counties, &adjacent_counties, CRD_by_county, 3, &view, log);
					foreign_sales.potential_matches.clear ();
					foreign_sales.foreign.clear();
				}
			}
			filename_output = "f:\\farmlandfinder\\afida\\2023-01-24\\AFIDA.csv";
			foreign_sales.append_and_filter_output_files(filename_output, ',', true, &view, log);
		}
	}

	delete [] CRD_by_county;

	view.update_progress ("After County Report", 1);

	update_display->Destroy();
	delete update_display;

	map_watershed->check_extent();
	panel_watershed->change_layers();
	enable_map_controls();
	panel_watershed->redraw("LAGOS");

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"File Open", L"", log);
	d.ShowWindowModal();

}

void frame_CARDGIS::OnMatch_AFIDA_Kansas
	(wxCommandEvent&)

// 2023-02-24 Wendong: Match foreign sales records to newer Kansas sales records
// 2022-04-13 Nieyan: match foreign sales records without locations to detailed sales records

// See comments for OnMatchAFIDA above.

{
	dynamic_string log, filename_afida, filename_county_master, filename_ff, filename_county_shp, filename_CRD, filename_county_wind, filename_output;
	CountyMaster counties;
	// int number_of_threads = 20;
	// int first_state_fips, last_state_fips;
	AFIDA foreign_sales;
	map_layer* layer_county;
	std::map <int, std::set <int>> adjacent_counties;
	int* CRD_by_county;
	std::set <int> run_states;
	std::set <int>::iterator state_fips;
	char abbreviation_holder[3];

	filename_county_master = "r:\\ceepes\\database\\County Master 2000.mdb";
	filename_afida = "I:\\TDC\\AFIDA\\Received 2023-02-22\\AFIDA_Database_update_Kansas_2023.csv";
	filename_ff = "F:\\FarmlandFinder\\AFIDA\\2023-02-24\\Kansas_Analysis_Data.csv";
	filename_CRD = "I:\\TDC\\AFIDA\\2022-09-19\\CRD_all states.csv";
	filename_county_wind = "I:\\TDC\\AFIDA\\2022-09-19\\State_average_wind_speed_all.csv";

	run_states.insert(20);

	dialog_run_threads* update_display = NULL;
	interface_window_wx view;

	update_display = new dialog_run_threads(this, true, false);
	update_display->Show();
	update_display->setup_interface_window(app, &view);

	counties.read(filename_county_master.get_text_ascii(), &view, log);

	CRD_by_county = read_CropReportingDistricts(filename_CRD, &foreign_sales.max_county_fips, &view, log);

	filename_county_shp = "r:\\Census\\TIGER 2012\\County\\tl_2012_us_county\\tl_2012_us_county.shp";

	layer_county = map_watershed->create_new(MAP_OBJECT_POLYGON);
	layer_county->name = "County Boundaries";
	layer_county->initialize_attributes = true;
	layer_county->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
	layer_county->color = RGB(0, 255, 0);
	map_watershed->layers.push_back(layer_county);

	// std::set <int> state_set;
	// state_set.insert(30);
	read_county_boundaries(filename_county_shp, map_watershed, layer_county, NULL, &view, log);
	find_adjacent_counties(layer_county, &adjacent_counties, false, &view, log);

	if (foreign_sales.read_county_wind(filename_county_wind, &counties, log)) {
		// if (foreign_sales.read_FarmlandFinder_2022_09_22(filename_ff, &counties, CRD_by_county, log))
		if (foreign_sales.read_Kansas(filename_ff, &counties, CRD_by_county, log)) {
			for (state_fips = run_states.begin();
			state_fips != run_states.end();
			++state_fips) {
				if (foreign_sales.read_2023_02_24(filename_afida, *state_fips, &counties, CRD_by_county, log)) {
					filename_output = "f:\\farmlandfinder\\afida\\2023-02-24\\AFIDA ";
					filename_output += state_abbreviation(*state_fips, abbreviation_holder, 3);
					filename_output += ".csv";

					// if (foreign_sales.read_Kansas(filename_ff, &counties, log))
					//	foreign_sales.county_report(20, &counties, &adjacent_counties, &CRD_by_county, 3, &view, log);

					// if (foreign_sales.read_Iowa(filename_ff, &counties, log))
					//	foreign_sales.county_report(19, &counties, &adjacent_counties, &CRD_by_county, 3, &view, log);

					foreign_sales.county_report(filename_output, *state_fips, &counties, &adjacent_counties, CRD_by_county, 3, &view, log);
					foreign_sales.potential_matches.clear();
					foreign_sales.foreign.clear();
				}
			}
			filename_output = "f:\\farmlandfinder\\afida\\2023-02-24\\AFIDA.csv";
			foreign_sales.append_and_filter_output_files(filename_output, ',', true, &view, log);
		}
	}

	delete[] CRD_by_county;

	view.update_progress("After County Report", 1);

	update_display->Destroy();
	delete update_display;

	map_watershed->check_extent();
	panel_watershed->change_layers();
	enable_map_controls();
	panel_watershed->redraw("LAGOS");

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"File Open", L"", log);
	d.ShowWindowModal();

}

void frame_CARDGIS::OnScanSWNET
	(wxCommandEvent&)

// 2023-02-10 Read all SWNET TIFF headers to create a list of their locations

{
	filename_struct f;
	dynamic_string log, report, path_swnet;
	std::vector <dynamic_string> swnet_folders, filenames_tiff;
	std::vector <dynamic_string>::iterator folder, filename;
	bool error = false;
	double llx, lly;
	map_layer *layer_tiff_outline;
	map_object_tiff tiff;
	map_polygon *tiff_rect;
	long id = 0;
	std::set <dynamic_string> unique_folders;

	path_swnet = "R:\\NHDPlusv2\\SWNET\\";

	if (list_of_matching_folders ("*", path_swnet, true, &swnet_folders, log)) {
		SetStatusText("OnScanSWNET");

		interface_window_wx view;
		view.start_progress(this);
		view.update_progress("Reading TIFF Folder", 1);

		wxInitAllImageHandlers();

		if ((layer_tiff_outline = map_watershed->match ("TIFF Rect")) == NULL) {
			layer_tiff_outline = map_watershed->create_new(MAP_OBJECT_POLYGON);
			layer_tiff_outline->name = "TIFF Rect";
			layer_tiff_outline->include_in_legend = false;
			layer_tiff_outline->color = RGB(0, 0, 255);
			layer_tiff_outline->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
			layer_tiff_outline->enclosure = MAP_POLYGON_ADD_LAST_SEGMENT;
			map_watershed->layers.push_back(layer_tiff_outline);
		}

		for (folder = swnet_folders.begin();
		!error && (folder != swnet_folders.end());
		++folder) {
			filenames_tiff.clear ();
			if (list_of_matching_filenames("*.TIF", *folder, true, &filenames_tiff, log)) {


				for (filename = filenames_tiff.begin ();
				!error && (filename != filenames_tiff.end ());
				++filename) {
					view.update_progress("Reading TIFF", 1);
					view.update_progress(*filename, 2);
					tiff.clear ();
					if (tiff.read_header_only (filename->get_text_ascii(), log)) {


						/*boost_ll_albers_3174(tiff.tiff->logical_extent.x[0], tiff.tiff->logical_extent.y[0], llx, lly, log);
						tiff.tiff->logical_extent.x[0] = llx;
						tiff.tiff->logical_extent.y[0] = lly;
						boost_ll_albers_3174(tiff.tiff->logical_extent.x[1], tiff.tiff->logical_extent.y[1], llx, lly, log);
						tiff.tiff->logical_extent.x[1] = llx;
						tiff.tiff->logical_extent.y[1] = lly;
						*/

						Albers2LL(&lly, &llx, tiff.tiff->logical_extent.y[0], tiff.tiff->logical_extent.x[0], tiff.tiff->longitude_central_meridian,
						tiff.tiff->LatitudeofOrigin, tiff.tiff->StandardParallel1, tiff.tiff->StandardParallel2);
						tiff.tiff->logical_extent.x[0] = llx;
						tiff.tiff->logical_extent.y[0] = lly;

						Albers2LL(&lly, &llx, tiff.tiff->logical_extent.y[1], tiff.tiff->logical_extent.x[1], tiff.tiff->longitude_central_meridian,
							tiff.tiff->LatitudeofOrigin, tiff.tiff->StandardParallel1, tiff.tiff->StandardParallel2);
						tiff.tiff->logical_extent.x[1] = llx;
						tiff.tiff->logical_extent.y[1] = lly;

						// reset lat & long after conversion
						tiff.latitude = (long)(tiff.tiff->logical_extent.y[0] * 1.0e6);
						tiff.longitude = (long)(tiff.tiff->logical_extent.x[0] * 1.0e6);


						report += *filename;
						report.add_formatted ("\t%.9lf\t%.9lf\t%.9lf\t%.9lf",
						tiff.tiff->logical_extent.x [0], tiff.tiff->logical_extent.x [1],
						tiff.tiff->logical_extent.y [0], tiff.tiff->logical_extent.y [1]);
						report += "\n";

						tiff_rect = (map_polygon *) layer_tiff_outline->create_new (MAP_OBJECT_POLYGON);

						tiff_rect->id = ++id;
						tiff_rect->name = *filename;

						tiff_rect->add_point((long)(tiff.tiff->logical_extent.y [0] * 1.0e6), (long)(tiff.tiff->logical_extent.x[0] * 1.0e6));
						tiff_rect->add_point((long)(tiff.tiff->logical_extent.y[0] * 1.0e6), (long)(tiff.tiff->logical_extent.x[1] * 1.0e6));
						tiff_rect->add_point((long)(tiff.tiff->logical_extent.y[1] * 1.0e6), (long)(tiff.tiff->logical_extent.x[1] * 1.0e6));
						tiff_rect->add_point((long)(tiff.tiff->logical_extent.y[1] * 1.0e6), (long)(tiff.tiff->logical_extent.x[0] * 1.0e6));
						tiff_rect->set_extent();

						layer_tiff_outline->objects.push_back(tiff_rect);

					}
					else
						error = true;
				}
			}
		}

		log += "\n";
		log += report;

		map_watershed->set_extent();  // set dynamic_map.logical_extent
		panel_watershed->change_layers();
		panel_watershed->change_selection_layer(layer_tiff_outline);
		enable_map_controls();
		panel_watershed->Refresh();

	}

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"File Open TIFF", L"", log);
	d.ShowWindowModal();
}

void frame_CARDGIS::OnDeleteSubPolygon
(wxCommandEvent&)

// 2023-06-30 If one polygon is selected, and it is complex, target & delete one subpolygon

{
	dynamic_string log;

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POLYGON)
	&& (panel_watershed->layer_selection->objects.size () == 1)) {
		std::vector <map_object*>::iterator selected_item;
		map_polygon_complex *complex;
		int target_polygon = 1;

		selected_item = panel_watershed->layer_selection->objects.begin ();
		if ((*selected_item)->type == MAP_OBJECT_POLYGON_COMPLEX) {

			dialog_prompt dp (this, "Enter sub-polygon number", &target_polygon);
			if (dp.ShowModal() == wxID_OK) {
				interface_window_wx view;
				view.setup_wx(this);
				view.start_progress(this);

				complex = (map_polygon_complex *) panel_watershed->layer_selection_source->match_id ((*selected_item)->id);
				if (complex->polygon_count >= target_polygon) {

					map_layer* new_layer = map_watershed->create_new (panel_watershed->layer_selection_source->type);

					if (complex->remove_polygon (target_polygon - 1)) {

						// update selection layer to match
						panel_watershed->select_id (complex->id);

						enable_map_controls();
						panel_watershed->redraw("OnDeleteSubPolygon");
					}
					else
						log += "ERROR, unable to delete sub-polygon.\n";
				}
				else
					log.add_formatted ("ERROR, ID %lld has only %d sub-polygons.\n", complex->id, complex->polygon_count);
			}
		}
		else
			log += "ERROR, selected object is not a complex polygon.\n";
	}
	else
		log += "ERROR, must select a single complex POLYGON.\n";

	if (log.get_length() > 0) {
		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"Convert Multipoint", L"", log);
		d.ShowWindowModal();
	}
}

void frame_CARDGIS::OnWriteCOMIDFromTo
	(wxCommandEvent&)

// 2023-07-06 For Yongjie, write all COMIDs with destination COMID

{
	dynamic_string filename_output, log, area_name;
	FILE * file_csv;
	bool error = false, include_divergences = false;
	long record_count = 0;
	filename_struct filenamer;
	std::map <long long, long long> from_to_map;
	std::map <long long, long long>::iterator from_to;
	std::map <long long, flow_network_link*>::const_iterator comid;
	std::vector <flow_network_link*>::const_iterator from;
	std::map <long long, flow_network_divergence>::const_iterator divergence;
	std::set <long long>::const_iterator source, drain;
	std::vector <std::pair <long long, long long>> divergence_pairs;
	std::vector <std::pair <long long, long long>>::const_iterator divergence_pair;

	filename_output = "F:\\scratch\\Yongjie\\FromTo.csv";

	interface_window_wx update_display;
	update_display.start_progress(this);

	router_NHD* router = relevent_router();
	extract_area_name(filename_nhd_area, &area_name);

	filenamer.parse(&filename_output);
	filename_output = filenamer.prefix;
	filename_output += area_name;
	filenamer.set_prefix(filename_output.get_text());
	filenamer.write_file_specification(&filename_output);

	if (fopen_s(&file_csv, filename_output.get_text_ascii(), "w") == 0) {

		fprintf(file_csv, "COMID,Downstream COMID\n");

		for (comid = router->rivers.all_comids.begin();
		comid != router->rivers.all_comids.end();
		++comid) {
			for (from = comid->second->from.begin();
			from != comid->second->from.end();
			++from) {
				if ((from_to = from_to_map.find ((*from)->id)) == from_to_map.end ())
					from_to_map.insert (std::pair <long long, long long> ((*from)->id, comid->first));
				else
					log += "ERROR duplicate from-to\n";
			}
			if ((++record_count % 1000) == 0)
				update_display.update_progress_formatted(1, "All COMIDs %ld", record_count);
		}

		if (include_divergences) {
			record_count = 0;
			for (divergence = router->rivers.divergences.divergences.begin();
			divergence != router->rivers.divergences.divergences.end();
			++divergence) {
				for (source = divergence->second.sources.begin();
				source != divergence->second.sources.end();
				++source) {
					if (divergence->second.id != 0)
						divergence_pairs.push_back (std::pair <long long, long long>(divergence->second.id, *source));
				}
				for (drain = divergence->second.drains.begin();
				drain != divergence->second.drains.end();
				++drain)
					divergence_pairs.push_back(std::pair <long long, long long>(*drain, divergence->second.id));

				if ((++record_count % 1000) == 0)
					update_display.update_progress_formatted(1, "divergences %ld", record_count);
			}
		}

		record_count = 0;
		for (from_to = from_to_map.begin ();
		from_to != from_to_map.end ();
		++from_to) {
			fprintf(file_csv, "%lld,%lld\n", from_to->first, from_to->second);

			if ((++record_count % 1000) == 0)
				update_display.update_progress_formatted(1, "Writing %ld", record_count);
		}

		if (include_divergences) {
			for (divergence_pair = divergence_pairs.begin();
			divergence_pair != divergence_pairs.end();
			++divergence_pair) {
				fprintf(file_csv, "%lld,%lld\n", divergence_pair->first, divergence_pair->second);

				if ((++record_count % 1000) == 0)
					update_display.update_progress_formatted(1, "Writing %ld", record_count);
			}
		}

		fclose(file_csv);
	}
	else {
		error = true;
		log += "ERROR, can't open output CSV file \"";
		log += filename_output;
		log += "\"\n";
	}

	update_display.destroy_dialog_progress ();

	if (log.get_length() > 0) {

		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"River", L"", log);
		d.ShowWindowModal();
	}
}


void frame_CARDGIS::OnIntersectTwoLayers
(wxCommandEvent&)

// 2023-08-14 Intersect two polygon layers
// Created for NYS ag sale circles vs Urban Areas

{
	dynamic_string log;
	map_layer*layer_1, *layer_2, *layer_clipped;
	std::vector <long long> downstream_ids, upstream_ids;
	ClipperBuffer clipper;
	ClipperLib::Paths paths_1, paths_clipped;
	std::vector <dynamic_string> tract_id_names;
	std::vector <double> tract_areas;
	std::set <long long> two_point_instances;
	router_NHD* router = relevent_router();

	if (panel_watershed->layer_selection_source
	&& (panel_watershed->layer_selection_source->type == MAP_OBJECT_POLYGON))
		layer_1 = panel_watershed->layer_selection_source;

	dialog_intersect dsl(this, map_watershed, &layer_1, &layer_2);
	if (dsl.ShowModal() == wxID_OK) {

		interface_window_wx view;
		view.setup_wx(this);
		view.start_progress(this);

		layer_clipped = map_watershed->create_new(MAP_OBJECT_POLYGON);
		layer_clipped->name = "Intersection";
		layer_clipped->draw_as = MAP_OBJECT_DRAW_OUTLINE_LAYER_COLOR;
		layer_clipped->color = RGB(255, 0, 0);
		layer_clipped->attribute_count_numeric = 6; // Space for 2 ids and 4 area fields in ClipperBuffer.Paths_to_map_layer
		layer_clipped->column_names_numeric.push_back("Layer 1 ID");
		layer_clipped->column_names_numeric.push_back("Layer 2 ID");
		layer_clipped->column_names_numeric.push_back("Geodesic m^2");
		layer_clipped->column_names_numeric.push_back("Exact m^2");
		layer_clipped->column_names_numeric.push_back("Authalic m^2");
		layer_clipped->column_names_numeric.push_back("Rhumb m^2");
//		layer_clipped->attribute_count_text = 1;
layer_clipped->attribute_count_text = 2;
layer_clipped->column_names_text.push_back("Sale Name");
	layer_clipped->column_names_text.push_back("City Name");
	layer_clipped->initialize_attributes = true;
		map_watershed->layers.push_back(layer_clipped);

		// clipper.tract_id_source_1 = CLIPPED_ID_SOURCE_ID;
	clipper.tract_id_source_1 = CLIPPED_ID_SOURCE_ID_AND_NAME;
		clipper.tract_index_source_1 = -1;
		clipper.layer_1_id_column_name = layer_1->name;
	clipper.layer_1_name_column_name = "Layer 1 Name";

		clipper.tract_id_source_2 = CLIPPED_ID_SOURCE_ID_AND_NAME;
		clipper.tract_index_source_2 = -1;
		clipper.layer_2_id_column_name = layer_2->name;
		clipper.layer_2_name_column_name = "Layer 2 Name";

		clipper.intersect_threaded(layer_1, layer_2, layer_clipped, app->multithread_count, &view, log);

		panel_watershed->change_layers();
		panel_watershed->change_selection_layer(layer_clipped);
		enable_map_controls();

		clipper.write_area_table(layer_clipped, "Clipped", log);
		clipper.write_area_table_group_by_1(layer_1, layer_clipped, log);
	}

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"OnIntersectTwoLayers", L"", log);
	d.ShowWindowModal();
}

void frame_CARDGIS::OnGSSURGO_SSURGO
(wxCommandEvent&)

// 2023-10-05 Wendong: Determine % of each Iowa county with HEL according to GSSURGO

{
	dynamic_string log, filename_output, path_ssurgo, report;
	FarmlandFinder farms;
	char delimiter = ',';
	bool multithreaded;
	filename_struct file_parser;
	std::map <dynamic_string, dynamic_string> farmlndcl_by_mukey;
	std::map <dynamic_string, dynamic_string> muhelcl_by_mukey;
	std::map <dynamic_string, std::tuple <double, double, double, double>> nccp_by_mukey;

	interface_window_wx view;

	path_ssurgo = "e:\\SSURGO\\";

	multithreaded = false;

	view.setup_wx(this);
	view.start_progress(this);

	// if (write_soils)
	//	report = "State\tSale\tMukey\tArea m^2\tFraction of Sale Area\tsandtotal_r\tsilttotal_r\tclaytotal_r\tslope_r\tIACSR\tnccpi3corn\tnccpi3soy\tnccpi3sg\tnccpi3all\n";
	//else
	// report = "State\tSale\tArea m^2\tSoil Count\tsandtotal_r\tsilttotal_r\tclaytotal_r\tslope_r\tIACSR\tnccpi3corn\tnccpi3soy\tnccpi3sg\tnccpi3all\n";

	filename_output = "e:\\scratch\\Iowa GSSURGO HEL Area.csv";

	farms.read_gSSURGO_mapunit(path_ssurgo, 19, &farmlndcl_by_mukey, &muhelcl_by_mukey, &view, log);
	farms.read_gSSURGO_Valu1(path_ssurgo, 19, &nccp_by_mukey, &view, log);

	view.update_progress_formatted(0, "Reading CSV State %d", 19);
	log += report;
	view.destroy_dialog_progress();

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"File Open", L"", log);
	d.ShowWindowModal();
}

void frame_CARDGIS::OnPointPolygonProximity
(wxCommandEvent&)

// 2021-02-22 Wendong: Shapefiles for roads & features
// produce table of distance & id of closest, also NHD streams & lakes

{
	dynamic_string log, filename_polygons, filename_output;
	map_layer* layer_points = NULL;
	importer_shapefile shapefile;
	MapLayerProximity proximator;
	ProximityLayer* proxy_layer;
	int number_of_threads = 24, add_to_base_shapefile_index;
	int within_check;
	std::map <int, shapefile_dbase_field> shapefile_column_order;

	interface_window_wx view;
	dialog_run_threads* update_display = NULL;

	update_display = new dialog_run_threads(this, true, false);
	update_display->Show();
	update_display->setup_interface_window(app, &view);

	// 2023-10-09
	shapefile.filename_source = "H:\\NewYorkAgSales\\ny_websales_shapefile\\FarmlandSales\\ny_trans_point_ag.shp";
	filename_polygons = "H:\\NewYorkAgSales\\ny_websales_shapefile\\2020UrbanAreas\\";
	proximator.output_filename = "H:\\NewYorkAgSales\\ny_websales_shapefile\\Urban Proximity.csv";
	proximator.delimiter = ',';
	within_check = PROXIMITY_WITHIN_POLYGON_ZERO;

	shapefile.id_field_name = "FID_1";
	shapefile.name_field_name = "V1";
	shapefile.projection_from_prj_file = true;
	shapefile.normalize_longitude = false;
	shapefile.take_dbf_columns = true;

	layer_points = map_watershed->create_new(MAP_OBJECT_POLYGON);
	layer_points->name = shapefile.filename_source;
	layer_points->draw_as = MAP_OBJECT_DRAW_SYMBOL_LAYER_COLOR;
	layer_points->symbol = MAP_OBJECT_SYMBOL_PLUS;
	layer_points->include_in_legend = true;
	map_watershed->layers.push_back(layer_points);

	panel_watershed->pause_rendering = true;
	if (shapefile.import(layer_points, &shapefile_column_order, map_watershed, &view, log)) {

		panel_watershed->pause_rendering = false;

		// layer_points->objects.resize (1000);

		// Add a column for distance-to-urban
		add_to_base_shapefile_index = layer_points->attribute_count_numeric;
		layer_points->resize (layer_points->attribute_count_numeric + 1, layer_points->attribute_count_text);
		layer_points->column_names_numeric.push_back ("dUrban_m");
		{
			// new field will be 93rd column
			int new_column_number = shapefile_column_order.size ();
			shapefile_dbase_field new_column;
			std::map <int, shapefile_dbase_field>::reverse_iterator previous_last;

			previous_last = shapefile_column_order.rbegin ();

			strcpy_s (new_column.Name, DBASE_TEXT_FIELD_LENGTH, "dUrban_m");
			new_column.map_layer_index_numeric = layer_points->attribute_count_numeric - 1;
			new_column.Type = DBASE_FIELD_DOUBLE;
			new_column.Length = 19;
			new_column.DecCount = 11;
			new_column.Offset = previous_last->second.Offset + previous_last->second.Length;
			shapefile_column_order.insert (std::pair <int, shapefile_dbase_field> (new_column_number, new_column));
		}

		{
			// Fix field 87 "Dist_Solar,N,13,11", many values are thousands, so "%13.11lf" crashes
			// Change to 19.11 to match other columns
			std::map <int, shapefile_dbase_field>::iterator field;
			for (field = shapefile_column_order.begin ();
			_stricmp (field->second.Name, "Dist_Solar") != 0;
			++field);

			if (field->second.Length == 13) {
				field->second.Length = 19;
				log += "Changing field length to 19 for column \"";
				log += field->second.Name;
				log += "\".\n";

				// 2023-10-16 must change offset of subsequent fields
				while (++field != shapefile_column_order.end())
					field->second.Offset += 6;
			}
		}

		proxy_layer = proximator.read_proximity_shapefile (filename_polygons, "tl_2020_us_uac20.shp", "Cities", "UACE20", "NAME20", map_watershed, &view, log);
		proxy_layer->write_column_headers = &proximity_id_name_distance_column_headers_callback;
		proxy_layer->write_layer_point = proximity_id_name_distance_callback;

		proximator.base_layer = layer_points;
		proximator.write_point_function = &proximity_id_name_callback;
		proximator.base_layer_column_headers = "ID";
		proximator.base_layer_column_headers += proximator.delimiter;
		proximator.base_layer_column_headers += "Name";

		proximator.thread_count = number_of_threads;
		proximator.write_proximity_table(within_check, add_to_base_shapefile_index, &view, log);

		{
			importer_shapefile s;
			s.filename_source = "H:\\NewYorkAgSales\\ny_websales_shapefile\\FarmlandSalesWithUrbanDistance\\ny_trans_point_ag.shp";
			s.write_from_map_layer(layer_points, &shapefile_column_order, false, &view, log);
		}

		map_watershed->check_extent();
		panel_watershed->change_layers();
		enable_map_controls();
		panel_watershed->redraw ("OnPointPolygonProximity");
		view.update_status_bar("Map Updated");
	}

	update_display->Destroy();

	map_watershed->check_extent();
	panel_watershed->change_layers();
	enable_map_controls();
	panel_watershed->redraw("FarmlandFinder");

	view.update_status_bar("Map Updated");

	log.convert_linefeeds_for_CEdit();
	dialog_error_list d(this, L"File Open", L"", log);
	d.ShowWindowModal();
}

void frame_CARDGIS::OnReadIowaCatchments
(wxCommandEvent&)

// 2024-01-26 Read layer of Iowa N sampling points
// Find nearest COMID
// List upstream

{
	dynamic_string log, filename_sites;
	map_layer* site_layer;
	interface_window_wx view;
	view.start_progress(this);
	Ledger preview;
	int index_comid, index_distance_m;

	map_layer* layer_rivers = map_watershed->match("Rivers");

	if (layer_rivers) {
		int id_column_index, lat_column_index, lon_column_index;
		std::vector <dynamic_string> raw_text;
		std::set <int> text_columns, skip_columns;

		filename_sites = "R:\\NHDPlusv2\\2024-01-26\\Nsites.csv";
		// id_column_index = 0;
		// lat_column_index = 2;
		// lon_column_index = 3;

		if (preview_position_file(filename_sites, CSV_POINT_FORMAT_AUTOMATIC, "", 10, &preview, &id_column_index, &lat_column_index, &lon_column_index, &text_columns, &raw_text, log)) {

			SetCursor(*wxHOURGLASS_CURSOR);
			site_layer = read_point_layer(filename_sites, -1, false, true, CSV_POINT_FORMAT_AUTOMATIC, &preview.column_names, &text_columns, &skip_columns,
			id_column_index, lat_column_index, lon_column_index, log);

			// Make sure site layer numeric_attributes can hold nearest COMID and distance
			index_comid = site_layer->attribute_count_numeric;
			index_distance_m = index_comid + 1;
			site_layer->resize (index_distance_m + 1, 0);

			site_layer->column_names_numeric.push_back("Nearest COMID");
			site_layer->column_names_numeric.push_back("Distance");

			site_layer->set_extent ();
			map_watershed->set_extent();

			panel_watershed->pause_rendering = false;
			if (site_layer) {
				panel_watershed->change_layers();
				panel_watershed->change_selection_layer(site_layer);
			}
			enable_map_controls();
			panel_watershed->redraw("OnReadIowaCatchments");
			SetCursor(*wxSTANDARD_CURSOR);


			view.update_progress("Matching centroids to COMIDs");
			// find nearest comid to each centerpoint
			{
				std::vector <map_object*>::const_iterator center;
				device_coordinate centerpoint;
				std::vector <long long> upstream_comids;
				std::vector <long long>::iterator source;
				map_object* segment;
				long distance;
				int step_count;

				for (center = site_layer->objects.begin();
				center != site_layer->objects.end();
				++center) {
					view.update_progress_formatted(1, "Centroid %lld", (*center)->id);
					centerpoint.x = (*center)->longitude;
					centerpoint.y = (*center)->latitude;
					if ((segment = layer_rivers->find_nearest_object_fast(centerpoint, &distance)) != NULL)
						(*center)->attributes_numeric[index_comid] = (double)segment->id;

					// Distance is NOT meters
					(*center)->attributes_numeric[index_distance_m] = (double)distance;

					upstream_comids.clear ();
					relevent_router()->rivers.accumulate_upstream(segment->id, &upstream_comids);

					log.add_formatted ("%lld", (*center)->id);
					for (source = upstream_comids.begin(), step_count = 0;
					source != upstream_comids.end();
					++source, ++step_count) {
						log.add_formatted ("\t%lld", *source);
						if (step_count % 48 == 47)
							log += "\n";
					}
					log += "\n";
				}
			}
		}
		enable_map_controls();
	}
	else
		log += "ERROR, no river layer.\n";

	if (log.get_length() > 0) {
		log.convert_linefeeds_for_CEdit();
		dialog_error_list d(this, L"Iowa Catchments", L"", log);
		d.ShowWindowModal();
	}
}
