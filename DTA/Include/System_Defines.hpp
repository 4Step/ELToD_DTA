//*********************************************************
//	System_Defines.hpp - global type definitions
//*********************************************************

#ifndef SYSTEM_DEFINES_HPP
#define SYSTEM_DEFINES_HPP

#define VERSION_CODE    71

#define EXECUTION_OFFSET             200

#define END_CONTROL    { 0, "", LEVEL0, OPT_KEY, END_KEY, "", "", NO_HELP }

#define BLANK           ' '
#define FINISH          '\0'

extern const char * SPACES;
extern const char * COMMA_DELIMITERS;
extern const char * SPACE_DELIMITERS;
extern const char * TAB_DELIMITERS;
extern const char * CSV_DELIMITERS;

#define MAX(a,b)        ((a >= b) ? a : b)
#define MIN(a,b)        ((a <= b) ? a : b)

#define MIDNIGHT        86400
#define MAX_INTEGER     0x7FFFFFFF
#define MAX_IMPEDANCE   0xFFFFFFFF
#define MAX_USHORT      0xFFFF

#define PI              3.1415926535897932384626433832795
#define DEG2RAD         (PI / 180.0)
#define RAD2DEG         (180.0 / PI)

#define _NAN                      (-1.79769e+308)
#define END_OF_RECORD             -1
#define SCAN_ALL_RECORDS          -1

#define STRING_FIELD_SIZE         128

//---- standard field names ----

#define NUM_NEST_FIELD_NAMES      "NUM_NEST", "RECORDS", "NUM_REC", "COUNT", "MEMBERS"
#define ORIGIN_FIELD_NAMES        "ORIGIN", "ORG", "O", "I", "LOCATION"
#define DESTINATION_FIELD_NAMES   "DESTINATION", "DES", "D", "J"
#define TRAVEL_TIME_FIELD_NAMES   "TTIME", "TIME", "AVG_TIME", "SUM"
#define ANODE_FIELD_NAMES         "NODE_A", "A_NODE", "A", "NODEA", "ANODE"
#define BNODE_FIELD_NAMES         "NODE_B", "B_NODE", "B", "NODEB", "BNODE"
#define LENGTH_FIELD_NAMES        "LENGTH", "DISTANCE", "LEN", "DIST"
#define FTYPE_FIELD_NAMES         "TYPE", "FUNCTCLASS", "FUNCL", "CLASS", "FTYPE"
#define SPD_FIELD_NAMES           "SPEED", "FFSPEED", "SPD", "FSPD", "FREESPD"
#define CAP_FIELD_NAMES           "CAP", "CAPACITY"
#define ZONE_FIELD_NAMES          "ZONE", "TAZ", "ZONENUM", "ZONE_ID", "Z"
#define TOLL_FIELD_NAMES          "TOLL", "COST", "RATE"
#define LANES_FIELD_NAMES         "LANES", "NUM_LANES", "NLANES", "LANES_AB"
#define NODE_FIELD_NAMES          "NODE", "N", "ID"

#define FILE_RANGE                "[project_directory]filename"
#define FORMAT_RANGE              "TEXT, BINARY, FIXED_COLUMN, COMMA_DELIMITED, SPACE_DELIMITED, TAB_DELIMITED, CSV_DELIMITED, DBASE, CUBE"
#define BOOL_RANGE                "TRUE/FALSE, YES/NO, 1/0, T/F, Y/N"
#define RANGE_RANGE               "e.g., 1, 2, 4..10, 100..200, 300"
#define MODE_RANGE                "WALK, BIKE, DRIVE, RIDE, TRANSIT, PNR_OUT, PNR_IN, KNR_OUT, KNR_IN, TAXI, OTHER, HOV2, HOV3, HOV4"
#define USE_RANGE                 "ANY, WALK, BIKE, CAR, TRUCK, BUS, RAIL, SOV, HOV2+, HOV3+, HOV2, HOV3, HOV4, LIGHTTRUCK, HEAVYTRUCK, TAXI, RESTRICTED, NONE"
#define VOLUME_FIELD_RANGE        "PERSONS, SOV, HOV2, HOV3P, TRUCK, TRANSIT, VEHICLES, VOLUME, ENTER, EXIT, FLOW, VMT, PMT, VHT, PHT"
#define ANODE_FIELD_RANGE         "NODE_A, A_NODE, A, NODEA, ANODE"
#define BNODE_FIELD_RANGE         "NODE_B, B_NODE, B, NODEB, BNODE"

//---- Control Key Codes ----

enum Level_Type {       //----- level types ----
	LEVEL0, LEVEL1, LEVEL2, LEVEL3, LEVEL4 
};
enum Option_Type {      //---- option types ----
	OPT_KEY, REQ_KEY, IGNORE_KEY 
};
enum Key_Type {         //---- key types ----
	END_KEY, INT_KEY, FLOAT_KEY, TEXT_KEY, BOOL_KEY, IN_KEY, OUT_KEY, PATH_KEY, TIME_KEY, LIST_KEY 
};
enum Help_Type {		//---- help types ----
	NO_HELP, REPORT_HELP, FORMAT_HELP
};

//---- Db_File -----

enum Format_Type {      //---- file format options ----
	DEFAULT_FORMAT, UNFORMATED, BINARY, FIXED_COLUMN, COMMA_DELIMITED, SPACE_DELIMITED, 
	TAB_DELIMITED, CSV_DELIMITED, DBASE, ARCVIEW, VERSION3, SQLITE3, MATRIX, STRUCTURE, 
	TRANSIMS, TPPLUS, TRANPLAN, TRANSCAD, INTERNAL_ID, EXTERNAL_ID
};
enum Access_Type {      //---- file access options ----
	READ, MODIFY, APPEND, CREATE 
};
enum Field_Type {       //---- field data type options ----
	DB_INTEGER, DB_UNSIGNED, DB_DOUBLE, DB_FIXED, DB_STRING, DB_CHAR, DB_TIME, DB_SHORT, DB_FLOAT, DB_STRUCT 
};
;
enum Status_Code {      //---- file error messages ----
	OK, DB_ERROR, MEMORY, RECORD_SIZE,                         //---- Db_Record ----
	NOT_OPEN, NULL_POINTER, FILE_IO, CLOSING, POSITIONING,     //---- Db_File ----
	FIELD_LIST, NO_FIELD, FIELD_BYTES, NULL_NAME, FIELD_UNITS, //---- Db_Base ----
	EXTENSION,                                                 //---- Ext_File ----
	PLAN_FIELDS,                                               //---- Plan_IO ----
	FILE_HEADER, RECORD_SYNTAX, FIELD_TYPE,                    //---- Csv_File ----
	OPEN_DEF, DEF_FORMAT, DEF_NEST,                            //---- Db_Header ----
	HEADER_MEMORY, INDEX_OPEN, HEADER_READ, HEADER_VERSION,    //---- Btree_Index ----
	NODE_MEMORY, NODE_READ, PATH_NAME, HEADER_WRITE, 
	TOO_MANY_FILES, NODE_WRITE,
	NO_TPPLUS, TPPLUS_LICENSE,                                   //---- TPPlus ----
	NO_TRANSCAD, TRANSCAD_LICENSE                               //---- TransCAD ----
};

enum Problem_Type {     //---- problem type options ----
	TOTAL_PROBLEM, PATH_PROBLEM, TIME_PROBLEM, ZERO_PROBLEM, 
	TYPE_PROBLEM, DIST_PROBLEM, MODE_PROBLEM, ACCESS_PROBLEM, WALK_PROBLEM, 
	WAIT_PROBLEM, LINK_PROBLEM, LOAD_PROBLEM, PARK_PROBLEM, BIKE_PROBLEM, 
	DEPARTURE_PROBLEM, ARRIVAL_PROBLEM, LINK_ACCESS_PROBLEM, CONNECT_PROBLEM, 
	PARKING_PROBLEM, MERGE_PROBLEM, LANE_PROBLEM, TURN_PROBLEM, POCKET_PROBLEM, 
	SPACING_PROBLEM, CONTROL_PROBLEM, USE_PROBLEM, STOP_PROBLEM, 
	LOCATION_PROBLEM, PASSENGER_PROBLEM, DURATION_PROBLEM, KISS_PROBLEM, 
	VEHICLE_PROBLEM, SORT_PROBLEM, WALK_LOC_PROBLEM, BIKE_LOC_PROBLEM,
	TRANSIT_LOC_PROBLEM, MATCH_PROBLEM, PARK_CAP_PROBLEM, BOARDING_PROBLEM, 
	DWELL_PROBLEM, TRANSFER_PROBLEM, LOCAL_PROBLEM, TRACE_PROBLEM, PARK_USE_PROBLEM, 
	FUEL_PROBLEM, DES_CAP_PROBLEM, MAX_PROBLEM
};

enum Mode_Type {       //---- trip mode options ----
	WAIT_MODE, WALK_MODE, BIKE_MODE, DRIVE_MODE, RIDE_MODE, 
	TRANSIT_MODE, PNR_OUT_MODE, PNR_IN_MODE, KNR_OUT_MODE, KNR_IN_MODE,
	TAXI_MODE, OTHER_MODE, HOV2_MODE, HOV3_MODE, HOV4_MODE, BUS_MODE, RAIL_MODE, MAX_MODE
};

#endif
