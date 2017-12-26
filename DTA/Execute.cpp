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
		}
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

	for (i = First_Report (); i != 0; i = Next_Report ()) {
		switch (i) {
			case CONVERGENCE_REPORT:		//---- Statistics Report ----
				Gap_Report ();
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
		case CONVERGENCE_REPORT:		//---- Statistics Report ----
			Gap_Header ();
			break;
		default:
			break;
	}
}
