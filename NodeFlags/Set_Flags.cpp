//*********************************************************
//	Set_Flags.cpp - set node flags
//*********************************************************

#include "NodeFlags.hpp"

//---------------------------------------------------------
//	Set_Flags
//---------------------------------------------------------

void NodeFlags::Set_Flags (void)
{
	int node, from, to, flag, toll_in, toll_out, entry_in, entry_out, exit_in, exit_out, other_in, other_out;

	Link_Data *link_ptr;
	Int_Map_Itr itr;
	
	Show_Message ("Set Node Flags -- Record");
	Set_Progress ();

	for (itr = node_map.begin (); itr != node_map.end (); itr++) {
		Show_Progress ();
		node = itr->second;

		flag = 0;
		toll_in = toll_out = entry_in = entry_out = exit_in = exit_out = other_in = other_out = -1;

		for (from = from_node [node]; from >= 0; from = link_out [from]) {
			link_ptr = &link_array [from];

			if (express_types.In_Range (link_ptr->Type ())) {
				toll_out = link_ptr->Bnode ();
			} else if (entry_types.In_Range (link_ptr->Type ())) {
				entry_out = link_ptr->Bnode ();
			} else if (exit_types.In_Range (link_ptr->Type ())) {
				exit_out = link_ptr->Bnode ();
			} else {
				other_out = link_ptr->Bnode ();
			}
		}
		for (to = to_node [node]; to >= 0; to = link_in [to]) {
			link_ptr = &link_array [to];

			if (express_types.In_Range (link_ptr->Type ())) {
				if (link_ptr->Anode () != toll_out) {
					toll_in = link_ptr->Anode ();
				}
			} else if (entry_types.In_Range (link_ptr->Type ())) {
				if (link_ptr->Anode () != entry_out) {
					entry_in = link_ptr->Anode ();
				}
			} else if (exit_types.In_Range (link_ptr->Type ())) {
				if (link_ptr->Anode () != exit_out) {
					exit_in = link_ptr->Anode ();
				}
			} else {
				if (link_ptr->Anode () != other_out) {
					other_in = link_ptr->Anode ();
				}
			}
		}
		if (other_in >= 0 && other_out >= 0 && other_in != other_out) {
			if (entry_out >= 0) {
				flag += 1;
			}
			if (exit_in >= 0) {
				flag += 10;
			}
		}
		if (toll_in >= 0 && toll_out >= 0 && toll_in != toll_out && exit_out >= 0) {
			flag += 100;

		}
		if (flag > 0) {
			flags [node] = flag;
		}
	}
	End_Progress (clock ());
}

