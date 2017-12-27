//*********************************************************
//	Alt_Path.cpp - build an alternate path
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Alt_Path
//---------------------------------------------------------

void DTA::Assign_Trips::Alt_Path (Path_Leg_Data root_data, int from_node, int to_node, int skip_link, Path_Leg_Array &leg_array)
{
	int anode, bnode, link, period, best;
	double vtime, vlen, time, length, cost, imp_a, imp_b, max_imp, cost_factor;

	Path_Data *path_ptr, *first_ptr, *last_ptr, path_root;
	Path_Array path_array;
	Path_Leg_Data leg_rec;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Toll_Data *toll_ptr;
	Int_Map_Itr map_itr;

	//---- initialie the queue pointers ----

	leg_array.clear ();
	path_root.Clear ();
	path_array.assign (exe->num_node, path_root);
	
	path_root.Time (root_data.Time ());
	path_root.Cost (root_data.Cost ());
	path_root.Length (root_data.Length ());

	first_ptr = &path_root;
	first_ptr->Next (from_node);
	last_ptr = &path_array [from_node];
	
	last_ptr->Link (root_data.Link ());
	last_ptr->Time (root_data.Time ());
	last_ptr->Cost (root_data.Cost ());
	last_ptr->Length (root_data.Length ());

	period = exe->time_periods.Period ((int) first_ptr->Time ());

	vtime = exe->value_time;
	vlen = exe->value_len;

	cost_factor = exe->cost_factors [mode];

	max_imp = MAX_INTEGER;
	best = -1;

	//---- add links leaving the origin node ----

	for (link = exe->node_link [from_node]; link >= 0; link = exe->next_link [link]) {
		if (link == skip_link) continue;

		link_ptr = &exe->link_array [link];
		
		if (!link_ptr->Mode_Flag (mode)) continue;

		bnode = link_ptr->Bnode ();
		time = link_ptr->TTime (period);
		length = link_ptr->Length ();
		cost = link_ptr->Toll () * cost_factor;

		if (link_ptr->Ex_Toll () >= 0) {
			toll_ptr = &exe->toll_array [link_ptr->Ex_Toll ()];
			cost += toll_ptr->Toll (period) * cost_factor;
		}
		imp_b = vtime * time + vlen * length + vcost * cost;

		time += first_ptr->Time ();
		cost += first_ptr->Cost ();
		length += first_ptr->Length ();

		path_ptr = &path_array [bnode];

		if (path_ptr->Next () == -1 && last_ptr != path_ptr) {
			last_ptr->Next (bnode);
			last_ptr = path_ptr;
		}
		path_ptr->Imped (imp_b);
		path_ptr->Time (time);
		path_ptr->Cost (cost);
		path_ptr->Length (length);
		path_ptr->From (from_node);
		path_ptr->Link (link);

		if (bnode == to_node || bnode == des_node) {
			max_imp = imp_b;
			best = bnode;
		}
	}

	if (last_ptr == &path_array [from_node]) return;

	//---- build a path to the destination zones ----

	for (;;) {
		anode = first_ptr->Next ();
		if (anode < 0) break;

		first_ptr->Next (-2);

		first_ptr = &path_array [anode];
		if (first_ptr->Link () < 0) continue;

		//---- check the cumulative impedance ----

		imp_a = first_ptr->Imped ();
		if (imp_a > max_imp) continue;

		period = exe->time_periods.Period ((int) first_ptr->Time ());

		//---- process each link leaving the node ----

		for (link = exe->node_link [anode]; link >= 0; link = exe->next_link [link]) {
			if (link == skip_link) continue;

			link_ptr = &exe->link_array [link];

			bnode = link_ptr->Bnode ();
			if (bnode == from_node) continue;
			
			if (!link_ptr->Mode_Flag (mode)) continue;

			time = link_ptr->TTime (period);
			length = link_ptr->Length ();
			cost = link_ptr->Toll () * cost_factor;

			if (link_ptr->Ex_Toll () >= 0) {
				toll_ptr = &exe->toll_array [link_ptr->Ex_Toll ()];
				cost += toll_ptr->Toll (period) * cost_factor;
			}
			imp_b = imp_a + vtime * time + vlen * length + vcost * cost;

			path_ptr = &path_array [bnode];

			//---- check the cumulative impedance ----

			if (imp_b > max_imp || (imp_b >= path_ptr->Imped () && path_ptr->Imped () > 0)) continue;

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

			if (bnode == to_node || bnode == des_node) {
				max_imp = imp_b;
				best = bnode;
			}
		}
	}

	//---- trace the paths ----

	for (bnode = best; bnode >= 0; bnode = path_ptr->From ()) {
		path_ptr = &path_array [bnode];

		leg_rec.Node (bnode);
		leg_rec.Link (path_ptr->Link ());
		leg_rec.Time (path_ptr->Time ());
		leg_rec.Cost (path_ptr->Cost ());
		leg_rec.Length (path_ptr->Length ());

		leg_array.push_back (leg_rec);
	}
}
