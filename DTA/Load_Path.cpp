//*********************************************************
//	Load_Path.cpp - build a node-based path and load
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Load_Path
//---------------------------------------------------------

void DTA::Assign_Trips::Load_Path (double trips, Path_Leg_Array &leg_array)
{
	int bnode, period, toll_link, to_node, from_node, exit_node, next_toll_link;
	bool express_flag, toll_flag, exit_flag, next_toll_flag, first;

	Path_Leg_RItr leg_ritr;
	Path_Leg_Itr leg_itr;
	Path_Leg_Array path1_array, path2_array;
	Path_Leg_Data leg_data;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Volume_Data *volume_ptr;
	Int_Map_Itr map_itr;

	//---- set the output flag ----

	express_flag = toll_flag = exit_flag = next_toll_flag = false;
	toll_link = to_node = from_node = exit_node = next_toll_link = -1;

	if (trips > exe->min_trip_split) {

		//---- scan the path for express links ----

		for (first = true, leg_ritr = leg_array.rbegin (); leg_ritr != leg_array.rend (); leg_ritr++, first = false) {
			if (first) continue;

			bnode = leg_ritr->Node ();

			node_ptr = &exe->node_array [bnode];
			link_ptr = &exe->link_array [leg_ritr->Link ()];

			if (express_flag) {
				if (exe->join_types.In_Range (node_ptr->Type ())) {
					to_node = bnode;
				}
			} else {
				if (exe->entry_types.In_Range (node_ptr->Type ())) {
					from_node = bnode;
				}
			}

			if (exe->express_types.In_Range (link_ptr->Type ())) {
				express_flag = true;
			} else {
				if (toll_flag) break;
			}
			if (link_ptr->Ex_Toll () >= 0) {
				if (!toll_flag) {
					toll_flag = true;
					toll_link = leg_ritr->Link ();

					if (!exit_flag) {
						if (exe->exit_types.In_Range (node_ptr->Type ())) {
							exit_node = bnode;
							exit_flag = true;
						}
					}
				} else if (exit_flag && !next_toll_flag) {
					next_toll_flag = true;
					next_toll_link = leg_ritr->Link ();
				}
			}
		}

		if (!toll_flag) {
			from_node = -1;
		} else if (from_node < 0) {
			if (exit_flag && next_toll_flag) {
				from_node = exit_node;
				toll_link = next_toll_link;
			} else {
				toll_flag = false;
			}
		}
	}

	//---- load the trips to the path ----

	for (first = true, leg_ritr = leg_array.rbegin (); leg_ritr != leg_array.rend (); leg_ritr++, first = false) {
		if (!first) {
			period = exe->time_periods.Period ((int) leg_data.Time ());

			if (period >= 0) {
				volume_ptr = &volume_array_ptr->at (leg_ritr->Link ());
				volume_ptr->Add_Volume (mode, period, trips);
			}

			//---- save the path links ----

			if (output_flag) {
				link_ptr = &exe->link_array [leg_ritr->Link ()];

				exe->path_leg_file.Node_A (exe->node_array [link_ptr->Anode ()].Node ());
				exe->path_leg_file.Node_B (exe->node_array [link_ptr->Bnode ()].Node ());
				exe->path_leg_file.Time (leg_ritr->Time () - start);
				exe->path_leg_file.Cost (leg_ritr->Cost ());
				exe->path_leg_file.Length (leg_ritr->Length ());
				exe->path_leg_file.Trips (trips);

				exe->path_leg_file.Write ();
			}
		}
		leg_data = *leg_ritr;
		if (leg_ritr->Node () == from_node) break;
	}

	//---- process an express path ----

	if (toll_flag) {

		//---- copy the express legs ----

		express_flag = false;

		for (leg_itr = leg_array.begin (); leg_itr != leg_array.end (); leg_itr++) {
			if (express_flag) {
				if (leg_itr->Node () == from_node) {
					path1_array.push_back (*leg_itr);
					leg_data = *leg_itr;
					break;
				}
			} else {
				if (leg_itr->Node () == to_node) {
					express_flag = true;
				} else {
					continue;
				}
			}
			path1_array.push_back (*leg_itr);
		}
		leg_array.clear ();

		//---- build the non-express path ----

		Alt_Path (leg_data, from_node, to_node, toll_link, path2_array);

		if (path2_array.size () == 0) {
			
			Load_Path (trips, path1_array);

		} else {

			//---- split the trips between the two paths ----

			Toll_Choice (trips, path1_array, path2_array);
		}

	} else {

		//---- complete the path to the destination ----

		leg_itr = leg_array.begin ();
		if (leg_itr == leg_array.end ()) return;

		to_node = leg_itr->Node ();

		if (to_node != des_node) {
			leg_data = *leg_itr;

			Alt_Path (leg_data, to_node, des_node, -1, leg_array);

			Load_Path (trips, leg_array);
		}
	}
}
