//*********************************************************
//	Execute.cpp - main processing
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Execute program
//---------------------------------------------------------

void DTA::Execute (void)
{
	int i;
	double gap;
	clock_t start;

	//---- read the node file ----

	Read_Nodes ();

	//---- read the network ----

	Read_Links ();
	
	//---- read the tolls ----

	Read_Tolls ();
	
	//---- assignment threads ----

	thread_flag = (Num_Threads () > 1);
	assign_trips = new Assign_Trips * [Num_Threads ()];

	if (thread_flag) {
		assign_queue.Max_Records (Num_Threads () * 20);

		for (i = 0; i < Num_Threads (); i++) {
			Show_Progress ();
			Assign_Trips *ptr = assign_trips [i] = new Assign_Trips (this, i);
			threads.push_back (thread (ref (*ptr)));
		}
		Write (1, "Number of Assignment Threads = ") << Num_Threads ();
		Show_Message (1);
	} else {
		*assign_trips = new Assign_Trips (this);
	}

	//---- iterations ----

	for (iter = 1; iter <= num_iter; iter++) {

		if (num_iter > 1) {
			Show_Message (String ("Iteration %d") % iter);
		}
		start = clock ();

		//---- read trips ----

		Read_Trips ();

		gap = Update_Time_Cost (iter);

		if (num_iter > 1) {
			Show_Message (String ("Convergence Gap = %.4lf") % gap);
			Show_Message (1);
			Print (2, String ("Iteration %d Gap %.4lf Processing Time %s") % iter % gap % Processing_Time (clock () - start));
		} else {
			Print (2, "Processing Time ") << Processing_Time (clock () - start);
		}
		Print (1, "Number of Origin Path Trees Built = ") << num_path_build;
		Print (1, "Number of OD Pairs with Trips = ") << num_od_loads;
		Print (1, "Number of Alternate Paths Built = ") << num_alt_path;
		Print (1, "Number of Toll Model Choices = ") << num_choices;

		num_path_build = num_od_loads = num_alt_path = num_choices = 0;

		if (gap < exit_gap) break;
	}
	trip_file.Close ();

	//---- delete the threads ----

	if (thread_flag) {
		assign_queue.End_of_Work ();
		threads.Join_All ();
	}

	//---- close the path leg file ----

	if (path_leg_flag) {
		path_leg_file.Close ();
	}

	//---- close the model data file ----

	if (model_data_flag) {
		model_data_file.Close ();
	}

	//---- close the period gap file ----

	if (period_gap_flag) {
		period_gap_file.Close ();
	}

	//---- close the link gap file ----

	if (link_gap_flag) {
		link_gap_file.Close ();
	}

	//---- close the toll gap file ----

	if (toll_gap_flag) {
		toll_gap_file.Close ();
	}

	//---- write volumes ----

	if (volume_flag) {
		Write_Volumes ();
	}

	//---- delete the trip processors ----

	for (i = 0; i < Num_Threads (); i++) {
		delete assign_trips [i];
	}
	delete assign_trips;
	assign_trips = 0;

	//---- print reports ----

	for (report_num = First_Report (); report_num != 0; report_num = Next_Report ()) {
		switch (report_num) {
			case LINK_GAP_REPORT:		//---- link gap Report ----
				Gap_Report ();
				break;
			case TOLL_GAP_REPORT:		//---- toll gap Report ----
				Gap_Report ();
				break;
			case CHOICE_DISTRIBUTION:	//---- choice distribution ----
				Distribution_Report ();
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void DTA::Page_Header (void)
{
	switch (Header_Number ()) {
		case LINK_GAP_REPORT:		//---- link gap Report ----
			Gap_Header ();
			break;
		case TOLL_GAP_REPORT:		//---- toll gap Report ----
			Gap_Header ();
			break;
		case CHOICE_DISTRIBUTION:	//---- choice distribution ----
			Distribution_Header ();
			break;
		default:
			break;
	}
}
