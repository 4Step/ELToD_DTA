//*********************************************************
//	Toll_Choice.cpp - split trips between path options
//*********************************************************

#include "DTA.hpp"

#include <math.h>

//---------------------------------------------------------
//	Toll_Choice
//---------------------------------------------------------

void DTA::Assign_Trips::Toll_Choice (double trips, Path_Leg_Array &path1_array, Path_Leg_Array &path2_array)
{
	double trips1, trips2, fftime1, fftime2, time1, time2, cost1, cost2, dist1, dist2, scale, rely, utility, prob, toll_const;
	bool first;

	Link_Data *link_ptr;
	Path_Leg_RItr itr1, itr2;
	Model_Itr model;

	model = exe->model_array.begin ();
	toll_const = -1.5;

	//---- check for a path ----

	itr1 = path1_array.rbegin ();
	if (itr1 == path1_array.rend ()) return;

	itr2 = path2_array.rbegin ();
	if (itr2 == path2_array.rend ()) return;

	//---- free flow time ----

	fftime1 = 0;

	for (first = true; itr1 != path1_array.rend (); itr1++, first = false) {
		if (!first) {
			link_ptr = &exe->link_array [itr1->Link ()];
			fftime1 += link_ptr->Time0 ();
		}
	}
	fftime2 = 0;

	for (first = true; itr2 != path2_array.rend (); itr2++, first = false) {
		if (!first) {
			link_ptr = &exe->link_array [itr2->Link ()];
			fftime2 += link_ptr->Time0 ();
		}
	}

	//---- path 1 time, distance, cost ----

	itr1 = path1_array.rbegin ();
	itr2 = --path1_array.rend ();

	time1 = itr2->Time () - itr1->Time ();
	cost1 = itr2->Cost () - itr1->Cost ();
	dist1 = itr2->Length () - itr1->Length ();
	if (dist1 <= 0) dist1 = 1;

	//---- path 2 time, distance, cost ----

	itr1 = path2_array.rbegin ();
	itr2 = --path2_array.rend ();

	time2 = itr2->Time () - itr1->Time ();
	cost2 = itr2->Cost () - itr1->Cost ();
	dist2 = itr2->Length () - itr1->Length ();

	//---- toll choice ----

	scale = pow (model->Scale_Length () / dist1, model->Scale_Alpha ());

	rely = model->Time_Factor () * model->Rely_Ratio () * model->Rely_Time () * ((fftime2 - time2) * pow (dist2, -model->Rely_Dist ())
		- (fftime1 - time1) * pow (dist1, -model->Rely_Dist ()));

	utility = -toll_const - scale * (model->Time_Factor () * (time1 - time2) + model->Toll_Factor () * (cost1 - cost2) + rely);

	prob = 1.0 / (1.0 + exp (utility));

	trips1 = trips * prob;
	trips2 = trips - trips1;

	Load_Path (trips1, path1_array);

	Load_Path (trips2, path2_array);
}
