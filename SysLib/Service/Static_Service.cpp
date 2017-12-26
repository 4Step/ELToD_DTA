//*********************************************************
//	Static_Service.cpp - static program service
//*********************************************************

#include "Static_Service.hpp"

#include "Execution_Service.hpp"

#include <iostream>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_EXTENSIONS
#include <io.h>
#else
#include <sys/stat.h>
#endif

mutex  main_mutex;

Format_Type Static_Service::_default_format = COMMA_DELIMITED;

const char * SPACES           = " ";
const char * COMMA_DELIMITERS = ",";
const char * SPACE_DELIMITERS = " \t";
const char * TAB_DELIMITERS   = "\t";
const char * CSV_DELIMITERS   = ", \t";

//---- format type codes ----

Static_Service::Code_Text Static_Service::format_codes [] = {
	{ UNFORMATED, "TEXT" }, { UNFORMATED, "UNFORMATED" }, { BINARY, "BINARY" },
	{ FIXED_COLUMN, "FIXED_COLUMN" }, { COMMA_DELIMITED, "COMMA_DELIMITED" },
	{ SPACE_DELIMITED, "SPACE_DELIMITED"}, { TAB_DELIMITED,  "TAB_DELIMITED" },
	{ CSV_DELIMITED, "CSV_DELIMITED" }, { DBASE, "DBASE" }, { TPPLUS, "CUBE" }, 
	{ 0, 0 }
};

//---- model type codes ----

Static_Service::Code_Text Static_Service::model_codes [] = {
	{ TRANSIMS, "TRANSIMS" }, { TPPLUS, "CUBE" }, { TPPLUS, "TPPLUS" }, 
	{ TRANPLAN, "TRANPLAN" }, { TRANSCAD, "TRANSCAD"}, { TRANSIMS, "DEFAULT_FORMAT" }, 
	{ TRANSIMS, "TEXT" }, { TRANSIMS, "UNFORMATED" }, { TRANSIMS, "BINARY" },
	{ TRANSIMS, "FIXED_COLUMN" }, { TRANSIMS, "COMMA_DELIMITED" },
	{ TRANSIMS, "SPACE_DELIMITED"}, { TRANSIMS,  "TAB_DELIMITED" },
	{ TRANSIMS, "CSV_DELIMITED" }, { TRANSIMS, "DBASE" }, { 0, 0 }
};

//---- field type codes ----

Static_Service::Code_Text Static_Service::field_codes [] = {
	{ DB_INTEGER, "INTEGER" }, { DB_UNSIGNED, "UNSIGNED" }, { DB_DOUBLE, "DOUBLE" }, 
	{ DB_FIXED, "FIXED" }, { DB_STRING, "STRING" }, { DB_CHAR, "CHAR" }, { DB_TIME, "TIME" }, 
	{ DB_SHORT, "SHORT" }, { DB_FLOAT, "FLOAT" }, { DB_STRUCT, "STRUCTURE" }, { 0, 0 }
};

//---- travel mode codes ----

Static_Service::Code_Text Static_Service::mode_codes [] = {
	{ WAIT_MODE, "WAIT" }, { WALK_MODE, "WALK" }, { BIKE_MODE, "BIKE" }, { DRIVE_MODE, "DRIVE" }, 
	{ RIDE_MODE, "RIDE" }, { TRANSIT_MODE, "TRANSIT" }, { PNR_OUT_MODE,"PNR_OUT" },
	{ PNR_IN_MODE, "PNR_IN" }, { KNR_OUT_MODE, "KNR_OUT" }, { KNR_IN_MODE,"KNR_IN" }, 
	{ TAXI_MODE, "TAXI" }, { OTHER_MODE, "OTHER" }, { HOV2_MODE, "HOV2" }, { HOV3_MODE, "HOV3" }, 
	{ HOV4_MODE, "HOV4" }, { BUS_MODE, "BUS" }, { RAIL_MODE, "RAIL"}, 
	{ WAIT_MODE, "NONE" }, { WALK_MODE, "ALL_WALK" }, { BIKE_MODE, "BICYCLE" }, 
	{ DRIVE_MODE, "DRIVE_ALONE" }, { RIDE_MODE, "PASSENGER" }, { PNR_OUT_MODE, "PARKRIDE" }, 
	{ PNR_IN_MODE, "RIDEPARK" }, { KNR_OUT_MODE, "KISSRIDE" }, { KNR_IN_MODE, "RIDEKISS" }, 
	{ TAXI_MODE, "PICKUP" }, { OTHER_MODE, "MAGIC_MOVE" }, { HOV2_MODE, "CARPOOL2" }, 
	{ HOV3_MODE, "CARPOOL3" }, { HOV4_MODE,"CARPOOL4" }, { HOV4_MODE, "HOV4+" },
	{ DRIVE_MODE, "AUTO_DRIVER" }, { DRIVE_MODE, "SOV" }, { RIDE_MODE, "AUTO_PASS" }, 
	{ BUS_MODE, "SCHOOL_BUS" }, { RAIL_MODE, "METRO" }, { 0, 0 }
};

//---- bool codes ----

Static_Service::Code_Text  Static_Service::bool_codes [] = {
	{ 1, "TRUE" }, { 0, "FALSE" }, { 1, "T" }, { 0, "F" }, 
	{ 1, "YES" }, { 0, "NO" }, { 1, "Y" }, { 0, "N" }, 
	{ 1, "1" }, { 0, "0" }, {0, "" }, { 0, 0 }
};

//---- Db_file status codes ----

Static_Service::Code_Text  Static_Service::db_file_codes [] = {
	{ OK, "OK" }, { DB_ERROR, "Software Error" }, { MEMORY, "Insufficient Memory" }, 
	{ RECORD_SIZE, "Insufficient Record Size" }, { NOT_OPEN, "Not Open" }, 
	{ NULL_POINTER, "Null Data Pointer" }, { FILE_IO, "I/O Error" }, { CLOSING, "Closing" },
	{ POSITIONING, "Positioning" }, { FIELD_LIST, "Field List" }, { NO_FIELD, "Field Not Found" },
	{ FIELD_BYTES, "Incompatible Field Size" }, { NULL_NAME, "Field Name is NULL" }, 
	{ FIELD_UNITS, "Field Type and Units are Incompatible" },
	{ EXTENSION, "Extension Number" }, { PLAN_FIELDS, "Token Fields don't Match" },
	{ FILE_HEADER, "Header" }, { RECORD_SYNTAX, "Record Syntax" }, { FIELD_TYPE, "Field Type" },
	{ OPEN_DEF, "Opening Field Definition File" }, { DEF_FORMAT, "Field Definition Format" }, 
	{ DEF_NEST, "Nested Definition Fields" }, { HEADER_MEMORY, "Insufficient Memory for Header Record" }, 
	{ INDEX_OPEN, "Opening" }, { HEADER_READ, "Reading Header" }, 
	{ HEADER_VERSION, "Incompatible Index Version" }, { NODE_MEMORY, "Insufficient Node Memory" },
	{ NODE_READ, "Reading Node Records" }, { PATH_NAME, "Original Data Path Name was Not Found" },
	{ HEADER_WRITE, "Writing Header" }, { TOO_MANY_FILES, "Too Many Data File Names" },
	{ NODE_WRITE, "Writing Node Records" }, { NO_TPPLUS, "TPPlus DLL Not Installed" }, 
	{ TPPLUS_LICENSE, "TPPlus License" }, { NO_TRANSCAD, "TransCAD DLL Not Installed" }, 
	{ TRANSCAD_LICENSE, "TransCAD License" }, { 0, 0 }
};

//---- problem type codes ----

Static_Service::Code_Text  Static_Service::problem_codes [] = {
	{ TOTAL_PROBLEM, "Total" }, { PATH_PROBLEM, "Path Building" }, { TIME_PROBLEM, "Time Schedule", },
	{ ZERO_PROBLEM, "Zero Length" }, { TYPE_PROBLEM, "Vehicle Type" }, { DIST_PROBLEM, "Path Circuity" },
	{ MODE_PROBLEM, "Travel Mode" }, { ACCESS_PROBLEM, "Vehicle Access" }, { WALK_PROBLEM, "Walk Distance" },
	{ WAIT_PROBLEM, "Wait Time" }, { LINK_PROBLEM, "Walk Access" }, { LOAD_PROBLEM, "Load Time" },
	{ PARK_PROBLEM, "Park-&-Ride Lot" }, { BIKE_PROBLEM, "Bike Distance" },
	{ DEPARTURE_PROBLEM, "Departure Time" }, { ARRIVAL_PROBLEM, "Arrival Time" },
	{ LINK_ACCESS_PROBLEM, "Link Access" }, { CONNECT_PROBLEM, "Link Connection" },
	{ PARKING_PROBLEM, "Parking Access" }, { MERGE_PROBLEM, "Lane Merging" },
	{ LANE_PROBLEM, "Lane Changing" }, { TURN_PROBLEM, "Turning Speed" }, { POCKET_PROBLEM, "Pocket Merge" },
	{ SPACING_PROBLEM, "Vehicle Spacing" }, { CONTROL_PROBLEM, "Traffic Control" },
	{ USE_PROBLEM, "Access Restriction" }, { STOP_PROBLEM, "Transit Stop" },
	{ LOCATION_PROBLEM,  "Activity Location" }, { PASSENGER_PROBLEM, "Vehicle Passenger" },
	{ DURATION_PROBLEM, "Activity Duration" }, { KISS_PROBLEM, "Kiss-&-Ride Lot", },
	{ VEHICLE_PROBLEM, "Vehicle ID" }, { SORT_PROBLEM, "Data Sort" }, { WALK_LOC_PROBLEM, "Walk Location" },
	{ BIKE_LOC_PROBLEM, "Bike Location" }, { TRANSIT_LOC_PROBLEM, "Transit Location" },
	{ MATCH_PROBLEM,  "Person Match" }, { PARK_CAP_PROBLEM, "Parking Capacity" },
	{ BOARDING_PROBLEM, "Transit Capacity" }, { DWELL_PROBLEM, "Transit Dwell" },
	{ TRANSFER_PROBLEM, "Number of Transfers" }, { LOCAL_PROBLEM, "Local Facility" },
	{ TRACE_PROBLEM, "Path Tracing" }, { PARK_USE_PROBLEM, "Parking Restriction" },
	{ FUEL_PROBLEM, "Fuel Supply" }, { DES_CAP_PROBLEM, "Destination Demand" }, { 0, 0 }
};

//---------------------------------------------------------
//	Static_Service constructor
//---------------------------------------------------------

Static_Service::Static_Service (void)
{

}

//---------------------------------------------------------
//	local_time - safe local time function
//---------------------------------------------------------

struct tm * Static_Service::local_time (time_t *tim)
{
#ifdef _MSC_EXTENSIONS
	static struct tm dt;

	localtime_s (&dt, tim);
	return (&dt);
#else
	return (localtime (tim));
#endif
}

//---------------------------------------------------------
//	c_time - safe time string function
//---------------------------------------------------------

char * Static_Service::c_time (time_t *tim)
{
	static char buffer [40];
#ifdef _MSC_EXTENSIONS
	ctime_s (buffer, sizeof (buffer), tim);
	buffer [24] = '\0';
#else
	sprintf (buffer, "%.24s", ctime (tim));
#endif
	return (buffer);
}

//---------------------------------------------------------
//	Clean - remove comments and extra characters
//---------------------------------------------------------

char * Static_Service::Clean (char *text)
{
	if (text) {
		char *ch1, *ch2;
		bool quote, tick;

		ch1 = ch2 = text;
		quote = tick = false;

		//--- remove leading blanks ----

		while (*ch2 == ' ') ch2++;

		//---- remove comment statements ----

		while (*ch2 != '\0') {
			if (!tick && *ch2 == '"') {
				quote = !quote;
			} else if (!quote && *ch2 == '\'') {
				tick = !tick;
			} else if (!quote && !tick && 
				((*ch2 == '#' && (*(ch2+1) == '#' || *(ch2+1) == '-' || *(ch2+1) == '*')) ||
				(*ch2 == '/' && (*(ch2+1) == '/' || *(ch2+1) == '-' || *(ch2+1) == '*')) ||
				(*ch2 == ';' && (*(ch2+1) == ';' || *(ch2+1) == '-' || *(ch2+1) == '*')))) {
				*ch1 = '\0';
				break;
			}
			*ch1++ = *ch2++;
		}
		*ch1 = '\0';

		//---- remove white space ----

		while (--ch1 >= text) {
			if (*ch1 != ' ' && *ch1 != '\t' && *ch1 != '\n' && *ch1 != '\r' && *ch1 != '\f') break;
			*ch1 = '\0';
		}
	}
	return (text);
}

//---------------------------------------------------------
//	f_open - safe file open
//---------------------------------------------------------

FILE * Static_Service::f_open (string filename, const char *control)
{
#ifdef _MSC_EXTENSIONS
	FILE *file;

	if (fopen_s (&file, filename.c_str (), control) != 0) {
		return (0);
	} else {
		return (file);
	}
#else
	return (fopen (filename.c_str (), control));
#endif
}

//---------------------------------------------------------
//	get_env - safe environment variables
//---------------------------------------------------------

char * Static_Service::get_env (const char *var)
{
#ifdef _MSC_EXTENSIONS
   char * value;
   size_t size;

   getenv_s (&size, 0, 0, var);
   if (size <= 1) return (0);

   value = (char *) malloc (size * sizeof (char));

   getenv_s (&size, value, size, var);

   return (value);
#else
	return (getenv (var));
#endif
}

//---------------------------------------------------------
//	Type_Code
//---------------------------------------------------------

int Static_Service::Type_Code (String text, Code_Text *codes, const char *label, int error_code)
{
	text.Trim ();
	if (text.empty ()) return (error_code);

	if (text [0] >= '0' && text [0] <= '9' && !text.Equals ("24_HOUR_CLOCK")) {
		int code = text.Integer ();

		for (int i=0; codes [i].text != 0; i++) {
			if (codes [i].code == code) {
				return (code);
			}
		}
	} else {
		for (int i=0; codes [i].text != 0; i++) {
			if (text.Equals (codes [i].text)) {
				return (codes [i].code);
			}
		}
	}
	if (exe->Send_Messages ()) {
		exe->Error (String ("Converting %s Type %s") % label % text);
	}
	return (error_code);
}

const char * Static_Service::Type_Code (int code, Code_Text *codes)
{
	for (int i=0; codes [i].text != 0; i++) {
		if (code == codes [i].code) {
			return (codes [i].text);
		}
	}
	return ("Error");
}

//---------------------------------------------------------
//	Data_Format
//---------------------------------------------------------

bool Static_Service::Data_Format (String format, Format_Type &file_format, Format_Type &model_format)
{
	if (format.empty ()) {
		file_format = DEFAULT_FORMAT;
		model_format = TRANSIMS;
		return (file_format == BINARY || file_format == SQLITE3);
	} else {
		Strings strings;
		String text = format;
		text.Parse (strings, ":");

		if (strings.size () >= 3) {
			model_format = Model_Code (strings [0]);
			file_format = Format_Code (strings [1]);
			return (strings [2].Equals ("CODES"));
		} else if (strings.size () >= 2) {
			if (strings [1].Equals ("CODES")) {
				model_format = Model_Code (strings [0]);
				if (model_format == TRANSIMS) {
					file_format = Format_Code (strings [0]);
				} else if (exe) {
					file_format = (Format_Type) exe->Default_Format ();
				} else {
					file_format = MATRIX;
				}
				return (true);
			} else {
				model_format = Model_Code (strings [0]);
				file_format = Format_Code (strings [1]);
				return (file_format == BINARY || file_format == SQLITE3);
			}
		} else {
			model_format = Model_Code (format);
			if (model_format == TRANSIMS) {
				file_format = Format_Code (format);
			} else if (exe) {
				file_format = (Format_Type) exe->Default_Format ();
			} else {
				file_format = MATRIX;
			}
			return (file_format == BINARY || file_format == SQLITE3);
		}
	}
}

String Static_Service::Data_Format (Format_Type file_format, Format_Type model_format, bool code_flag)
{
	if (model_format == TRANSIMS) {
		if (code_flag && (file_format != BINARY && file_format != SQLITE3)) {
			String format = Format_Code (file_format);
			format += ":CODES";
			return (format);
		} else {
			return (Format_Code (file_format));
		}
	} else {
		String model = Model_Code (model_format);
		String format = Format_Code (file_format);
		model += ":" + format;
		if (code_flag && (file_format != BINARY && file_format != SQLITE3)) {
			model += ":CODES";
		}
		return (model);
	}
}
