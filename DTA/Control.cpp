//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void DTA::Program_Control (void)
{
	int i, j, num, period_field, constant_field;

	String key;
	Strings values;
	Str_Itr str_itr;
	Flt_Itr val_itr;
	Choice_Model model;
	
	//---- create the network files ----

	Execution_Service::Program_Control ();

	num_period = time_periods.Num_Periods ();
	if (num_period == 0) {
		time_periods.Set_Periods (15, 0, 24 * 60);
		num_period = time_periods.Num_Periods ();
	}
	toll_constant.assign (num_period, -1.5);

	Print (2, String ("%s Control Keys:") % Program ());

	//---- open link file ----

	key = Get_Control_String (LINK_FILE);

	//---- get the file format ----

	if (Check_Control_Key (LINK_FORMAT)) {
		link_file.Dbase_Format (Get_Control_String (LINK_FORMAT));
	}

	if (!link_file.Open (Project_Filename (key))) {
		Error ("Link File Not Found");
	}
	if (!link_file.Time_Flag () && !link_file.Speed_Flag ()) {
		Error ("A Time or Speed Field was Not Found");
	}

	//---- express facility types ----

	express_types.Add_Ranges (Get_Control_Text (EXPRESS_FACILITY_TYPES));

	//---- open node file ----

	Print (1);
	key = Get_Control_String (NODE_FILE);

	//---- get the file format ----

	if (Check_Control_Key (NODE_FORMAT)) {
		node_file.Dbase_Format (Get_Control_String (NODE_FORMAT));
	}

	if (!node_file.Open (Project_Filename (key))) {
		Error ("Node File Not Found");
	}

	//---- express entry types ----

	entry_types.Add_Ranges (Get_Control_Text (EXPRESS_ENTRY_TYPES));

	//---- express exit types ----

	exit_types.Add_Ranges (Get_Control_Text (EXPRESS_EXIT_TYPES));

	//---- general join types ----

	join_types.Add_Ranges (Get_Control_Text (GENERAL_JOIN_TYPES));

	//---- node zone type ----

	zone_type = Get_Control_Integer (ZONE_NODE_TYPE);

	//---- open trip file ----

	Print (1);
	key = Get_Control_String (TRIP_FILE);

	//---- get the file format ----

	if (Check_Control_Key (TRIP_FORMAT)) {
		trip_file.Dbase_Format (Get_Control_String (TRIP_FORMAT));
	}
	if (!trip_file.Open (Project_Filename (key))) {
		Error ("Trip File Not Found");
	}
	num_mode = trip_file.Num_Modes ();
	Print (0, "  (Modes=");

	mode_names = trip_file.Mode_Names ();

	for (i = 0, str_itr = mode_names.begin (); str_itr != mode_names.end (); str_itr++, i++) {
		if (i > 0) Print (0, ", ");
		Print (0, *str_itr);
	}
	Print (0, ")");

	//---- minimum trip split ----

	min_trip_split = Get_Control_Double (MINIMUM_TRIP_SPLIT);

	//---- store trips in memory ----

	memory_flag = Get_Control_Flag (STORE_TRIPS_IN_MEMORY);

	//---- look for mode fields in the link file ----

	mode_fields.assign (num_mode, -1);

	for (i = 0, str_itr = mode_names.begin (); str_itr != mode_names.end (); str_itr++, i++) {
		mode_fields [i] = link_file.Field_Number (*str_itr);
	}

	//---- open toll file ----

	Print (1);
	key = Get_Control_String (TOLL_FILE);

	//---- get the file format ----

	if (Check_Control_Key (TOLL_FORMAT)) {
		toll_file.Dbase_Format (Get_Control_String (TOLL_FORMAT));
	}
	if (!toll_file.Open (Project_Filename (key))) {
		Error ("Toll File Not Found");
	}

	//---- open toll constant file ----

	key = Get_Control_String (TOLL_CONSTANT_FILE);

	if (!key.empty ()) {
		constant_file.File_Type ("Toll Constant File");

		//---- get the file format ----

		if (Check_Control_Key (TOLL_CONSTANT_FORMAT)) {
			constant_file.Dbase_Format (Get_Control_String (TOLL_CONSTANT_FORMAT));
		}
		if (!constant_file.Open (Project_Filename (key))) {
			Error ("Toll Constant File Not Found");
		}
		period_field = constant_file.Required_Field ("PERIOD", "TSTEP", "TIME");
		constant_field = constant_file.Required_Field ("CONSTANT", "FACTOR", "CONST");

		while (constant_file.Read ()) {
			i = constant_file.Get_Integer (period_field);

			if (i > 0 && i <= num_period) {
				toll_constant [i - 1] = (float) constant_file.Get_Double (constant_field);
			}
		}
	}

	//---- number of iteration ----

	Print (1);
	num_iter = Get_Control_Integer (NUMBER_OF_ITERATIONS);

	//---- gap convergence criteria ----

	exit_gap = Get_Control_Double (GAP_CONVERGENCE_CRITERIA);

	//---- time value ----

	Print (1);
	value_time = Get_Control_Double (TIME_VALUE);

	//---- distance value ----

	value_len = Get_Control_Double (DISTANCE_VALUE);

	//---- cost value ----

	value_cost = Get_Control_Double (COST_VALUE);

	//---- mode cost factors ----

	cost_factors.assign (num_mode, 1.0);

	if (Check_Control_Key (MODE_COST_FACTORS)) {
		key = Get_Control_String (MODE_COST_FACTORS);
		key.Parse (values);

		for (i = j = 0; i < num_mode; i++) {
			if (i < (int) values.size ()) {
				j = i;
			}
			cost_factors [i] = (float) values [j].Double ();
		}
	}
	Output_Control_Label (MODE_COST_FACTORS);

	for (i=0, str_itr = mode_names.begin (); str_itr != mode_names.end (); str_itr++, i++) {
		if (i > 0) Print (0, ", ");
		Print (0, String ("%s=%.2lf") % *str_itr % cost_factors [i]);
	}

	//---- toll policy codes ----

	Print (1);
	key = Get_Control_Text (TOLL_POLICY_CODES);

	if (!key.empty () && !key.Equals ("0")) {
		policy_range.Add_Ranges (key);

		num = policy_range.Range_Count ();

		//---- minimum toll ----

		key = Get_Control_Text (MINIMUM_TOLL);
		key.Parse (values);

		for (i = j = 0; i < num; i++) {
			if (i < (int) values.size ()) {
				j = i;
			}
			min_toll.push_back ((float) values [j].Double ());
		}

		//---- maximum toll ----

		key = Get_Control_Text (MAXIMUM_TOLL);
		key.Parse (values);

		for (i = j = 0; i < num; i++) {
			if (i < (int) values.size ()) {
				j = i;
			}
			max_toll.push_back ((float) values [j].Double ());
		}

		//---- maximum vc ratio ----

		key = Get_Control_Text (MAXIMUM_VC_RATIO);
		key.Parse (values);

		for (i = j = 0; i < num; i++) {
			if (i < (int) values.size ()) {
				j = i;
			}
			max_vc.push_back ((float) values [j].Double ());
		}

		//---- vc ratio offset ----

		key = Get_Control_Text (VC_RATIO_OFFSET);
		key.Parse (values);

		for (i = j = 0; i < num; i++) {
			if (i < (int) values.size ()) {
				j = i;
			}
			vc_offset.push_back ((float) values [j].Double ());
		}
		//---- toll exponent ----

		key = Get_Control_Text (TOLL_EXPONENT);
		key.Parse (values);

		for (i = j = 0; i < num; i++) {
			if (i < (int) values.size ()) {
				j = i;
			}
			exponent.push_back ((float) values [j].Double ());
		}

		//---- maximum toll change ----

		key = Get_Control_Text (MAXIMUM_TOLL_CHANGE);
		key.Parse (values);

		for (i = j = 0; i < num; i++) {
			if (i < (int) values.size ()) {
				j = i;
			}
			max_change.push_back ((float) values [j].Double ());
		}

	} else {
		policy_range.Add_Range (1, 1);

		max_vc.push_back (5.0);
		min_toll.push_back (0.25);
		max_toll.push_back (7.0);
		vc_offset.push_back ((float) 0.175);
		exponent.push_back (3.5);
		max_change.push_back (10.0);
	}

	//---- toll choice model ----

	Print (1);
	model.Time_Factor (Get_Control_Double (MODEL_TIME_FACTOR));
	model.Toll_Factor (Get_Control_Double (MODEL_TOLL_FACTOR));
	model.Rely_Ratio (Get_Control_Double (MODEL_RELIABILITY_RATIO));
	model.Rely_Time (Get_Control_Double (MODEL_RELIABILITY_TIME));
	model.Rely_Dist (Get_Control_Double (MODEL_RELIABILITY_DISTANCE));
	model.Perceive_Time (Get_Control_Double (MODEL_PERCEIVED_TIME));
	model.Perceive_Mid_VC (Get_Control_Double (MODEL_PERCEIVED_MID_VC));
	model.Perceive_Max_VC (Get_Control_Double (MODEL_PERCEIVED_MAX_VC));
	model.Express_Weight (Get_Control_Double (MODEL_EXPRESS_WEIGHT));
	model.Scale_Length (Get_Control_Double (MODEL_SCALE_LENGTH));
	model.Scale_Alpha (Get_Control_Double (MODEL_SCALE_ALPHA));

	model_array.push_back (model);

	//---- open the volume file ----

	Print (1);
	key = Get_Control_String (NEW_VOLUME_FILE);

	if (!key.empty ()) {
		volume_file.Modes (mode_names);
		volume_flag = true;

		//---- get the file format ----

		if (Check_Control_Key (NEW_VOLUME_FORMAT)) {
			volume_file.Dbase_Format (Get_Control_String (NEW_VOLUME_FORMAT));
		}
		if (!volume_file.Create (Project_Filename (key))) {
			Error ("Volume File Not Created");
		}
	}

	//---- open the path leg file ----

	Print (1);
	key = Get_Control_String (NEW_PATH_LEG_FILE);

	if (!key.empty ()) {
		path_leg_flag = true;

		//---- get the file format ----

		if (Check_Control_Key (NEW_PATH_LEG_FORMAT)) {
			path_leg_file.Dbase_Format (Get_Control_String (NEW_PATH_LEG_FORMAT));
		}
		if (!path_leg_file.Create (Project_Filename (key))) {
			Error ("Path Leg File Not Created");
		}
	}

	//---- open the new model data file ----

	Print (1);
	key = Get_Control_String (NEW_MODEL_DATA_FILE);

	if (!key.empty ()) {
		model_data_flag = true;

		//---- get the file format ----

		if (Check_Control_Key (NEW_MODEL_DATA_FORMAT)) {
			model_data_file.Dbase_Format (Get_Control_String (NEW_MODEL_DATA_FORMAT));
		}
		if (!model_data_file.Create (Project_Filename (key))) {
			Error ("Model Data File Not Created");
		}
	}

	if (path_leg_flag || model_data_flag) {
		Print (1);

		//---- select origins ----

		key = Get_Control_Text (SELECT_ORIGINS);

		if (!key.empty () && !key.Equals ("NONE")) {
			sel_org_flag = true;
			sel_org_range.Add_Ranges (key);
		}

		//---- select destinations ----

		key = Get_Control_Text (SELECT_DESTINATIONS);

		if (!key.empty () && !key.Equals ("NONE")) {
			sel_des_flag = true;
			sel_des_range.Add_Ranges (key);
		}

		//---- select time periods ----

		key = Get_Control_Text (SELECT_TIME_PERIODS);

		if (!key.empty () && !key.Equals ("NONE")) {
			sel_per_flag = true;
			sel_per_range.Add_Ranges (key);
		}

		//---- select iterations ----

		key = Get_Control_Text (SELECT_ITERATIONS);

		if (!key.empty () && !key.Equals ("NONE")) {
			sel_iter_flag = true;
			sel_iter_range.Add_Ranges (key);
		}

		//---- select modes ----

		key = Get_Control_Text (SELECT_MODES);

		if (!key.empty () && !key.Equals ("NONE")) {
			sel_mode_flag = true;
			key.Parse (values);

			for (str_itr = values.begin (); str_itr != values.end (); str_itr++) {
				for (i = 0; i < num_mode; i++) {
					if (str_itr->Equals (trip_file.Mode_Name (i))) {
						sel_mode_range.Add_Range (i, i);
						break;
					}
				}
			}
		}

		if (!sel_iter_flag && (sel_mode_flag || sel_org_flag || sel_des_flag || sel_per_flag)) {
			sel_iter_flag = true;
			sel_iter_range.Add_Ranges ((key ("%d") % num_iter));
		}

		//---- select iterations ----

		key = Get_Control_Text (SELECT_DECISION_NODES);

		if (!key.empty () && !key.Equals ("NONE")) {
			sel_node_flag = true;
			sel_nodes.Add_Ranges (key);
		}
	}

	//---- open the link gap_file ----

	key = Get_Control_String (NEW_LINK_GAP_FILE);

	if (!key.empty ()) {
		Print (1);
		if (!link_gap_file.Create (Project_Filename (key))) {
			Error ("Link Gap File was Not Created");
		}
	}

	//---- open the toll gap_file ----

	key = Get_Control_String (NEW_TOLL_GAP_FILE);

	if (!key.empty ()) {
		Print (1);
		if (!toll_gap_file.Create (Project_Filename (key))) {
			Error ("Toll Gap File was Not Created");
		}
	}
	
	//---- open period gap file ----

	key = Get_Control_String (NEW_PERIOD_GAP_FILE);

	if (!key.empty ()) {
		period_gap_flag = true;
		period_gap_file.Period_Flag (true);

		//---- get the file format ----

		if (Check_Control_Key (NEW_PERIOD_GAP_FORMAT)) {
			period_gap_file.Dbase_Format (Get_Control_String (NEW_PERIOD_GAP_FORMAT));
		}
		if (!period_gap_file.Create (Project_Filename (key))) {
			Error ("Period Gap File Not Created");
		}
	}

	//---- read report types ----

	List_Reports ();

	link_gap_flag = (Report_Flag (LINK_GAP_REPORT) || link_gap_file.Is_Open ());
	toll_gap_flag = (Report_Flag (TOLL_GAP_REPORT) || toll_gap_file.Is_Open ());
}

