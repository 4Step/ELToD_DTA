//*********************************************************
//	DTA.cpp - main processing
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

DTA::DTA (void) : Execution_Service ()
{
	Program ("DTA");
	Version (5);
	Title ("Dynamnic Traffic Assignment");

	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ LINK_FILE, "LINK_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_FORMAT, "LINK_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ EXPRESS_FACILITY_TYPES, "EXPRESS_FACILITY_TYPES", LEVEL0, OPT_KEY, INT_KEY, "96..98", "1..100", NO_HELP },
		{ MINIMUM_TRIP_SPLIT, "MINIMUM_TRIP_SPLIT", LEVEL0, OPT_KEY, FLOAT_KEY, "0.01", "0.0..10.0", NO_HELP },

		{ NODE_FILE, "NODE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NODE_FORMAT, "NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ EXPRESS_ENTRY_TYPES, "EXPRESS_ENTRY_TYPES", LEVEL0, OPT_KEY, INT_KEY, "90", "1..100", NO_HELP },
		{ EXPRESS_EXIT_TYPES, "EXPRESS_EXIT_TYPES", LEVEL0, OPT_KEY, INT_KEY, "92", "1..100", NO_HELP },
		{ GENERAL_JOIN_TYPES, "GENERAL_JOIN_TYPES", LEVEL0, OPT_KEY, INT_KEY, "91", "1..100", NO_HELP },
		{ ZONE_NODE_TYPE, "ZONE_NODE_TYPE", LEVEL0, OPT_KEY, INT_KEY, "99", "1..100", NO_HELP },

		{ TRIP_FILE, "TRIP_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_FORMAT, "TRIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TOLL_FILE, "TOLL_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TOLL_FORMAT, "TOLL_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },

		{ NUMBER_OF_ITERATIONS, "NUMBER_OF_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "1", "1..1000", NO_HELP },
		{ TIME_VALUE, "TIME_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..100.0", NO_HELP },
		{ DISTANCE_VALUE, "DISTANCE_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..100.0", NO_HELP },
		{ COST_VALUE, "COST_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..100.0", NO_HELP },
		{ MODE_COST_FACTORS, "MODE_COST_FACTORS", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..100.0", NO_HELP },

		{ TOLL_POLICY_CODES, "TOLL_POLICY_CODES", LEVEL0, OPT_KEY, INT_KEY, "1", "0..100", NO_HELP },		
		{ MINIMUM_TOLL, "MINIMUM_TOLL", LEVEL0, OPT_KEY, FLOAT_KEY, "0.25", "0.0..100.0", NO_HELP },
		{ MAXIMUM_TOLL, "MAXIMUM_TOLL", LEVEL0, OPT_KEY, FLOAT_KEY, "7.0", "0.0..100.0", NO_HELP },
		{ MAXIMUM_VC_RATIO, "MAXIMUM_VC_RATIO", LEVEL0, OPT_KEY, FLOAT_KEY, "5.0", "1.0..10.0", NO_HELP },
		{ VC_RATIO_OFFSET, "VC_RATIO_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "0.175", "0.0..10.0", NO_HELP },
		{ TOLL_EXPONENT, "TOLL_EXPONENT", LEVEL0, OPT_KEY, FLOAT_KEY, "3.5", "1.0..10.0", NO_HELP },
		{ MAXIMUM_TOLL_CHANGE, "MAXIMUM_TOLL_CHANGE", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0", "1.0..100.0", NO_HELP },
		
		{ NEW_VOLUME_FILE, "NEW_VOLUME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_VOLUME_FORMAT, "NEW_VOLUME_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_PATH_LEG_FILE, "NEW_PATH_LEG_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PATH_LEG_FORMAT, "NEW_PATH_LEG_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ SELECT_ORIGINS, "SELECT_ORIGINS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", RANGE_RANGE, FORMAT_HELP },
		{ SELECT_DESTINATIONS, "SELECT_DESTINATIONS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", RANGE_RANGE, FORMAT_HELP },
		{ SELECT_TIME_PERIODS, "SELECT_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", RANGE_RANGE, FORMAT_HELP },
		{ SELECT_ITERATIONS, "SELECT_ITERATIONS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", RANGE_RANGE, FORMAT_HELP },
		{ SELECT_MODES, "SELECT_MODES", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", RANGE_RANGE, FORMAT_HELP },
		{ NEW_CONVERGENCE_FILE, "NEW_CONVERGENCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERGENCE_REPORT",
		""
	};
	Key_List (keys);
	Report_List (reports);

	thread_flag = volume_flag = path_leg_flag = false;
	sel_org_flag = sel_des_flag = sel_per_flag = sel_iter_flag = sel_mode_flag = false;
	max_zone = 1000;
	num_period = 96;
	num_mode = 0;
	num_iter = 10;
	value_time = value_len = 0.0;
	zone_type = 99;
	min_speed = 0.1;

	min_trip_split = 0.01;
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	DTA *dta = new DTA ();
	return (dta->Start_Execution (commands, control));
}
