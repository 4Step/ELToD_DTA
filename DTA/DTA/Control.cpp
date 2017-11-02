//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void DTA::Program_Control (void)
{
	int i, j, num;
	String key;
	Strings values;
	Str_Itr str_itr;
	Flt_Itr val_itr;
	
	//---- create the network files ----

	Execution_Service::Program_Control ();

	num_period = time_periods.Num_Periods ();
	if (num_period == 0) {
		time_periods.Set_Periods (15, 0, 24 * 60);
		num_period = time_periods.Num_Periods ();
	}

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

	//---- express node types ----

	node_types.Add_Ranges (Get_Control_Text (EXPRESS_NODE_TYPES));

	//---- node zone type ----

	zone_type = Get_Control_Integer (ZONE_NODE_TYPE);

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

	//---- look for mode fields in the link file ----

	mode_fields.assign (num_mode, -1);

	for (i=0, str_itr = mode_names.begin (); str_itr != mode_names.end (); str_itr++, i++) {
		mode_fields [i] = link_file.Field_Number (*str_itr);
	}

	//---- number of iteration ----

	Print (1);
	num_iter = Get_Control_Integer (NUMBER_OF_ITERATIONS);

	//---- time value ----

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
	}

	//---- read report types ----

	List_Reports ();
}

