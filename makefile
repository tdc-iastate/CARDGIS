##
d1=../util/
d2=../odbc/
d3=../map/
d4=../i_epic_database/
d5=../i_swat_database/
d6=../weather/
BINARIES = wxswat

CXXFLAGS_STANDARD = -g `wx-config --cppflags`
# -D UNICODE
CXXFLAGS_TDC = -D _ODBC_TDC -D UNICODE -D __WX__ -std=c++11 -m64
CXXFLAGS = ${CXXFLAGS_STANDARD} ${CXXFLAGS_TDC}

LDFLAGS_STANDARD = -g -m64
LDFLAGS = ${LDFLAGS_STANDARD} `wx-config --libs` -L/usr/lib/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu/odbc -lodbc -lmysqlclient


all: wxswat

dialog_configure.o: dialog_configure.cpp dialog_configure.h
	g++ ${CXXFLAGS} -c dialog_configure.cpp
dialog_open_database.o: dialog_open_database.cpp dialog_open_database.h
	g++ ${CXXFLAGS} -c dialog_open_database.cpp
FilesRecentlyOpened.o: FilesRecentlyOpened.cpp FilesRecentlyOpened.h
	g++ ${CXXFLAGS} -c FilesRecentlyOpened.cpp
frame_field_view.o: frame_field_view.cpp frame_field_view.h
	g++ ${CXXFLAGS} -c frame_field_view.cpp
gridwidget.o: gridwidget.cpp gridwidget.h
	g++ ${CXXFLAGS} -c gridwidget.cpp
list_control_records.o: list_control_records.cpp list_control_records.h
	g++ ${CXXFLAGS} -c list_control_records.cpp
app_field_view.o: app_field_view.cpp app_field_view.h
	g++ ${CXXFLAGS} -c app_field_view.cpp
EPICDeclarations.o: $(d4)EPICDeclarations.cpp $(d4)EPICDeclarations.h
	g++ ${CXXFLAGS} -c $(d4)EPICDeclarations.cpp
map_object_swat.o: $(d5)map_object_swat.cpp $(d5)map_object_swat.h
	g++ ${CXXFLAGS} -c $(d5)map_object_swat.cpp
utility_afx.o: $(d1)utility_afx.cpp $(d1)utility_afx.h
	g++ ${CXXFLAGS} -c $(d1)utility_afx.cpp
dynamic_string.o: $(d1)dynamic_string.cpp $(d1)dynamic_string.h
	g++ ${CXXFLAGS} -c $(d1)dynamic_string.cpp
list.o: $(d1)list.cpp $(d1)list.h
	g++ ${CXXFLAGS} -c $(d1)list.cpp
listbox_files.o: $(d1)listbox_files.cpp $(d1)listbox_files.h
	g++ ${CXXFLAGS} -c $(d1)listbox_files.cpp
utility.o: $(d1)utility.cpp $(d1)utility.h
	g++ ${CXXFLAGS} -c $(d1)utility.cpp
filename_struct.o: $(d1)filename_struct.cpp $(d1)filename_struct.h
	g++ ${CXXFLAGS} -c $(d1)filename_struct.cpp
ARMS_Declarations.o: $(d1)ARMS_Declarations.cpp $(d1)ARMS_Declarations.h
	g++ ${CXXFLAGS} -c $(d1)ARMS_Declarations.cpp
interface_tree.o: $(d1)interface_tree.cpp $(d1)interface_tree.h
	g++ ${CXXFLAGS} -c $(d1)interface_tree.cpp
interface_window.o: $(d1)interface_window.cpp $(d1)interface_window.h
	g++ ${CXXFLAGS} -c $(d1)interface_window.cpp
interface_window_wx.o: $(d1)interface_window_wx.cpp $(d1)interface_window_wx.h
	g++ ${CXXFLAGS} -c $(d1)interface_window_wx.cpp
Timestamp.o: $(d1)Timestamp.cpp $(d1)Timestamp.h
	g++ ${CXXFLAGS} -c $(d1)Timestamp.cpp
custom.o: $(d1)custom.cpp $(d1)custom.h
	g++ ${CXXFLAGS} -c $(d1)custom.cpp
exact_decimal.o: $(d1)exact_decimal.cpp $(d1)exact_decimal.h
	g++ ${CXXFLAGS} -c $(d1)exact_decimal.cpp
dialog_error_list.o: $(d1)dialog_error_list.cpp $(d1)dialog_error_list.h
	g++ ${CXXFLAGS} -c $(d1)dialog_error_list.cpp
odbc_database.o: $(d2)odbc_database.cpp $(d2)odbc_database.h
	g++ ${CXXFLAGS} -c $(d2)odbc_database.cpp
odbc_field_set.o: $(d2)odbc_field_set.cpp $(d2)odbc_field_set.h
	g++ ${CXXFLAGS} -c $(d2)odbc_field_set.cpp
odbc_database_access.o: $(d2)odbc_database_access.cpp $(d2)odbc_database_access.h
	g++ ${CXXFLAGS} -c $(d2)odbc_database_access.cpp
odbc_database_mysql.o: $(d2)odbc_database_mysql.cpp $(d2)odbc_database_mysql.h
	g++ ${CXXFLAGS} -c $(d2)odbc_database_mysql.cpp
odbc_database_sql_server.o: $(d2)odbc_database_sql_server.cpp $(d2)odbc_database_sql_server.h
	g++ ${CXXFLAGS} -c $(d2)odbc_database_sql_server.cpp
dynamic_map.o: $(d3)dynamic_map.cpp $(d3)dynamic_map.h
	g++ ${CXXFLAGS} -c $(d3)dynamic_map.cpp
map_scale.o: $(d3)map_scale.cpp $(d3)map_scale.h
	g++ ${CXXFLAGS} -c $(d3)map_scale.cpp
color_set.o: $(d3)color_set.cpp $(d3)color_set.h
	g++ ${CXXFLAGS} -c $(d3)color_set.cpp
