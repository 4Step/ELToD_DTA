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
	int branch_node, join_node, period;
	double trips1, trips2, fftime1, fftime2, time1, time2, cost1, cost2, dist1, dist2, scale, rely, utility, prob, toll_const;
	bool first;

	Link_Data *link_ptr;
	Path_Leg_RItr itr1, itr2;
	Model_Itr model;

	model = exe->model_array.begin ();

	//---- check for a path ----

	itr1 = path1_array.rbegin ();
	if (itr1 == path1_array.rend ()) return;

	itr2 = path2_array.rbegin ();
	if (itr2 == path2_array.rend ()) return;

	branch_node = 0;
	period = exe->time_periods.Period ((int) itr1->Time ());
	toll_const = exe->toll_constant [period];

	//---- look for a common join node ----

	for (join_node = -1, ++itr1; itr1 != path1_array.rend (); itr1++) {
		for (itr2 = path2_array.rbegin (); itr2 != path2_array.rend (); itr2++) {
			if (itr1->Node () == itr2->Node ()) {
				join_node = itr1->Node ();
				break;
			}
		}
	}

	//---- free flow time ----

	fftime1 = 0;
	time1 = cost1 = dist1 = 0;

	for (first = true, itr1 = path1_array.rbegin (); itr1 != path1_array.rend (); itr1++, first = false) {
		if (!first) {
			link_ptr = &exe->link_array [itr1->Link ()];
			fftime1 += link_ptr->Time0 ();

			if (itr1->Node () == join_node) {
				time1 = itr1->Time () - time1;
				cost1 = itr1->Cost () - cost1;
				dist1 = itr1->Length () - dist1;
				break;
			}
		} else {
			time1 = itr1->Time ();
			cost1 = itr1->Cost ();
			dist1 = itr1->Length ();
			branch_node = itr1->Node ();
		}
	}
	fftime2 = 0;
	time2 = cost2 = dist2 = 0;

	for (first = true, itr2 = path2_array.rbegin (); itr2 != path2_array.rend (); itr2++, first = false) {
		if (!first) {
			link_ptr = &exe->link_array [itr2->Link ()];
			fftime2 += link_ptr->Time0 ();

			if (itr2->Node () == join_node) {
				time2 = itr2->Time () - time2;
				cost2 = itr2->Cost () - cost2;
				dist2 = itr2->Length () - dist2;
				break;
			}
		} else {
			time2 = itr2->Time ();
			cost2 = itr2->Cost ();
			dist2 = itr2->Length ();
		}
	}

	if (join_node < 0) {

		//---- path 1 time, distance, cost ----

		itr1 = path1_array.rbegin ();
		itr2 = --path1_array.rend ();

		time1 = itr2->Time () - itr1->Time ();
		cost1 = itr2->Cost () - itr1->Cost ();
		dist1 = itr2->Length () - itr1->Length ();

		//---- path 2 time, distance, cost ----

		itr1 = path2_array.rbegin ();
		itr2 = --path2_array.rend ();

		time2 = itr2->Time () - itr1->Time ();
		cost2 = itr2->Cost () - itr1->Cost ();
		dist2 = itr2->Length () - itr1->Length ();
	}

	//---- toll choice ----
	
	if (dist1 <= 0) dist1 = 1;
	scale = pow (model->Scale_Length () / dist1, model->Scale_Alpha ());

	rely = model->Time_Factor () * model->Rely_Ratio () * model->Rely_Time () * ((fftime2 - time2) * pow (dist2, -model->Rely_Dist ())
		- (fftime1 - time1) * pow (dist1, -model->Rely_Dist ()));

	utility = -toll_const - scale * (model->Time_Factor () * (time1 - time2) + model->Toll_Factor () * (cost1 - cost2) + rely);

	prob = 1.0 / (1.0 + exp (utility));

	trips1 = trips * prob;
	trips2 = trips - trips1;

	(*choice_ptr)++;

	if (exe->sel_node_flag && data_out_flag) {
		Node_Data *node_ptr = &exe->node_array [branch_node];

		if (exe->sel_nodes.In_Range (node_ptr->Node ())) {
			MAIN_LOCK
			exe->model_data_file.Iteration (exe->iter);
			exe->model_data_file.Origin (org);
			exe->model_data_file.Destination (des);
			exe->model_data_file.Start (start_period);

			exe->model_data_file.Period (period + 1);
			exe->model_data_file.From_Node (node_ptr->Node ());

			node_ptr = &exe->node_array [join_node];

			exe->model_data_file.To_Node (node_ptr->Node ());
			exe->model_data_file.Distance1 (dist1);
			exe->model_data_file.Distance2 (dist2);
			exe->model_data_file.Time1 (time1);
			exe->model_data_file.Time2 (time2);
			exe->model_data_file.FFTime1 (fftime1);
			exe->model_data_file.FFTime2 (fftime2);
			exe->model_data_file.Toll1 (cost1);
			exe->model_data_file.Toll2 (cost2);
			exe->model_data_file.Utility (utility);
			exe->model_data_file.Share1 (prob);

			exe->model_data_file.Write ();
			END_LOCK
		}
	}

	Load_Path (trips1, path1_array);

	Load_Path (trips2, path2_array);
}
