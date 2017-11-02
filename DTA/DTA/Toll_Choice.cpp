//*********************************************************
//	Toll_Choice.cpp - split trips between path options
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Toll_Choice
//---------------------------------------------------------

void DTA::Assign_Trips::Toll_Choice (double tod, double trips, Path_Leg_Array &path1_array, Path_Leg_Array &path2_array)
{
	double trips1, trips2, time_diff, cost_diff, len_diff;

	Path_Leg_Itr itr1, itr2;

	itr1 = path1_array.begin ();
	if (itr1 == path1_array.end ()) return;

	itr2 = path2_array.begin ();
	if (itr2 == path2_array.end ()) return;

	cost_diff = fabs (itr1->Cost () - itr2->Cost ());
	len_diff = fabs (itr1->Length () - itr2->Length ());
	time_diff = fabs (itr1->Time () - itr2->Time ());
	
	//---- toll choice ----

	trips1 = trips2 = trips / 2.0;

	Load_Path (tod, trips1, path1_array);

	Load_Path (tod, trips2, path2_array);
}
