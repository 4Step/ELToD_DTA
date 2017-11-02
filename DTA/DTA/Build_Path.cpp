//*********************************************************
//	Build_Path.cpp - build a node-based path and load
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Build_Path
//---------------------------------------------------------

void DTA::Assign_Trips::Build_Path (Floats *trip_ptr)
{
	int anode, bnode, link, period, org, des;
	double vtime, vlen, trips, time, length, cost, imp_a, imp_b, cost_factor;
	bool out_flag;

	Path_Data *path_ptr, *first_ptr, *last_ptr, path_root;
	Path_Array path_array;
	Path_Leg_Data leg_rec;
	Path_Leg_Array leg_array;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Toll_Data *toll_ptr;
	Int_Map_Itr map_itr;

	//---- initialie the queue pointers ----

	path_root.Clear ();
	path_array.assign (exe->num_node, path_root);

	cost_factor = exe->cost_factors [mode];

	first_ptr = &path_root;
	first_ptr->Next (root);
	last_ptr = &path_array [root];

	period = exe->time_periods.Period ((int) start);

	out_flag = false;

	if (exe->path_leg_flag) {
		node_ptr = &exe->node_array [root];
		org = node_ptr->Node ();

		if (!exe->sel_org_flag || exe->sel_org_range.In_Range (org)) {
			if (!exe->sel_per_flag || exe->sel_per_range.In_Range (period + 1)) {
				if (!exe->sel_mode_flag || exe->sel_mode_range.In_Range (mode)) {
					exe->path_leg_file.Origin (org);
					exe->path_leg_file.Period (period + 1);
					exe->path_leg_file.Mode (exe->mode_names [mode]);
					out_flag = true;
				}
			}
		}
	}
	vtime = exe->value_time;
	vlen = exe->value_len;

	//---- add links leaving the origin node ----

	for (link = exe->node_link [root]; link >= 0; link = exe->next_link [link]) {
		link_ptr = &exe->link_array [link];

		if (!link_ptr->Mode_Flag (mode)) continue;

		bnode = link_ptr->Bnode ();
		time = link_ptr->TTime (period);
		length = link_ptr->Length ();
		cost = link_ptr->Toll () * cost_factor;

		imp_b = vtime * time + vlen * length + vcost * cost;
		
		if (link_ptr->Ex_Toll () >= 0) {
			toll_ptr = &exe->toll_array [link_ptr->Ex_Toll ()];
			cost += toll_ptr->Toll (period) * cost_factor;
		}
		path_ptr = &path_array [bnode];

		if (path_ptr->Next () == -1 && last_ptr != path_ptr) {
			last_ptr->Next (bnode);
			last_ptr = path_ptr;
		}
		path_ptr->Imped (imp_b);
		path_ptr->Time (start + time);
		path_ptr->Cost (cost);
		path_ptr->Length (length);
		path_ptr->From (root);
		path_ptr->Link (link);
	}

	if (last_ptr == &path_array [root]) return;

	//---- build a path to the destination zones ----

	for (;;) {
		anode = first_ptr->Next ();
		if (anode < 0) break;

		first_ptr->Next (-2);

		first_ptr = &path_array [anode];
		if (first_ptr->Link () < 0) continue;

		//---- check the cumulative impedance ----

		imp_a = first_ptr->Imped ();

		period = exe->time_periods.Period ((int) first_ptr->Time ());

		//---- process each link leaving the node ----

		for (link = exe->node_link [anode]; link >= 0; link = exe->next_link [link]) {
			link_ptr = &exe->link_array [link];
			
			bnode = link_ptr->Bnode ();
			if (bnode == root) continue;
			
			if (!link_ptr->Mode_Flag (mode)) continue;

			time = link_ptr->TTime (period);
			length = link_ptr->Length ();
			cost = link_ptr->Toll () * cost_factor;

			imp_b = imp_a + vtime * time + vlen * length + vcost * cost;

			if (link_ptr->Ex_Toll () >= 0) {
				toll_ptr = &exe->toll_array [link_ptr->Ex_Toll ()];
				cost += toll_ptr->Toll (period) * cost_factor;
			}
			path_ptr = &path_array [bnode];

			//---- check the cumulative impedance ----

			if (imp_b >= path_ptr->Imped () && path_ptr->Imped () > 0) continue;

			time += first_ptr->Time ();
			cost += first_ptr->Cost ();
			length += first_ptr->Length ();

			node_ptr = &exe->node_array [bnode];

			if (node_ptr->Type () != exe->zone_type) {

				//---- add to the tree ----

				if (path_ptr->Next () == -2) {
					path_ptr->Next (first_ptr->Next ());
					first_ptr->Next (bnode);
				} else if (path_ptr->Next () == -1 && last_ptr != path_ptr) {
					last_ptr->Next (bnode);
					last_ptr = path_ptr;
				}
			}
			path_ptr->Imped (imp_b);
			path_ptr->Time (time);
			path_ptr->Cost (cost);
			path_ptr->Length (length);
			path_ptr->From (anode);
			path_ptr->Link (link);
		}
	}

	//---- trace the paths ----

	for (des = 1; des <= exe->max_zone; des++) {
		trips = trip_ptr->at (des - 1);
		if (trips == 0.0) continue;

		map_itr = exe->node_map.find (des);
		if (map_itr == exe->node_map.end ()) continue;

		des_node = map_itr->second;

		//---- set the output flag ----

		output_flag = false;

		if (out_flag) {
			if (!exe->sel_des_flag || exe->sel_des_range.In_Range (des)) {
				exe->path_leg_file.Destination (des);
				output_flag = true;
			}
		}

		//---- build the path array ----

		leg_array.clear ();

		for (bnode = des_node; bnode != root && bnode > 0; bnode = path_ptr->From ()) {
			path_ptr = &path_array [bnode];

			if (path_ptr->Link () < 0) break;

			leg_rec.Node (bnode);
			leg_rec.Link (path_ptr->Link ());
			leg_rec.Time (path_ptr->Time ());
			leg_rec.Cost (path_ptr->Cost ());
			leg_rec.Length (path_ptr->Length ());

			leg_array.push_back (leg_rec);
		}

		//---- load the path ----

		Load_Path (start, trips, leg_array);
	}
}
