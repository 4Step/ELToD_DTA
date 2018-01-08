//*********************************************************
//	Read_Trips.cpp - read trip file
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void DTA::Read_Trips (void)
{
	int org, des, period, mode, increment;
	double trips;
	String mode_code;
	Str_ID_Itr mode_itr;
	Assign_Itr assign_itr;

	Assign_Data assign_data (num_mode, max_zone);

	old_volume_array.swap (volume_array);

	Volume_Data volume_data (num_period, num_mode);
	volume_array.assign (num_link, volume_data);

	memset (choice_distb, '\0', sizeof (choice_distb));

	increment = time_periods.Increment ();
	
	if (thread_flag) {
		assign_queue.Start_Work ();
	}

	if (memory_flag && iter > 1) {
		Show_Message ("Processing Trip Data -- Record");
		Set_Progress ();

		//---- process records stored in memory ----

		for (assign_itr = assign_array.begin (); assign_itr != assign_array.end (); assign_itr++) {
			Show_Progress ();
			if (thread_flag) {
				assign_queue.Put (*assign_itr);
			} else {
				(*assign_trips)->Process_Trips (*assign_itr);
			}
		}

	} else {
		Show_Message ("Reading Trip File -- Record");
		Set_Progress ();

		trip_file.Rewind ();

		while (trip_file.Read ()) {
			Show_Progress ();

			org = trip_file.Origin ();
			period = (trip_file.Period () - 1) * increment;

			if (org != assign_data.Origin () || period != assign_data.Start ()) {
				if (assign_data.Origin () > 0) {
					if (memory_flag) {
						assign_array.push_back (assign_data);
					}
					if (thread_flag) {
						assign_queue.Put (assign_data);
					} else {
						(*assign_trips)->Process_Trips (assign_data);
					}
				}
				assign_data.Origin (org);
				assign_data.Start (period);
				assign_data.Zero_Trips ();
			}
			des = trip_file.Destination () - 1;

			for (mode = 0; mode < num_mode; mode++) {
				trips = trip_file.Mode (mode);

				assign_data.Trips (mode, des, trips);
			}
		}
		if (assign_data.Origin () > 0) {
			if (memory_flag) {
				assign_array.push_back (assign_data);
			}
			if (thread_flag) {
				assign_queue.Put (assign_data);
			} else {
				(*assign_trips)->Process_Trips (assign_data);
			}
		}
		if (memory_flag) {
			trip_file.Close ();
		}
	}
	if (thread_flag) {
		assign_queue.Complete_Work ();

		for (int i = 0; i < Num_Threads (); i++) {
			assign_trips [i]->Combine_Volume ();
		}
	}
	End_Progress (clock ());
}
