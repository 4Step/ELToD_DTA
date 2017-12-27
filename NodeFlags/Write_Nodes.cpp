//*********************************************************
//	Write_Nodes.cpp - write node file
//*********************************************************

#include "NodeFlags.hpp"

//---------------------------------------------------------
//	Write_Nodes
//---------------------------------------------------------

void NodeFlags::Write_Nodes (void)
{
	int node, num_entry, num_exit, num_join, count, flag;
	
	Int_Map_Itr map_itr;
	
	Show_Message ("Write Node File -- Record");
	Set_Progress ();

	num_entry = num_exit = num_join = count = 0;

	while (node_file.Read ()) {
		Show_Progress ();

		node = node_file.Node ();
		if (node <= 0) continue;

		new_file.Copy_Fields (node_file);

		map_itr = node_map.find (node);

		node = map_itr->second;

		flag = flags [node];

		if (flag > 0) {
			if (flag == 1) {
				flag = 90;
				num_entry++;
			} else if (flag == 10) {
				flag = 91;
				num_join++;
			} else if (flag == 100) {
				flag = 92;
				num_exit++;
			} else if (flag == 11) {
				flag = 93;
				num_entry++;
				num_exit++;
			}
			new_file.Put_Field (flag_field, flag);
		}

		if (!new_file.Write ()) {
			Error ("Error Writing New Node File");
		}
		count++;
	}
	End_Progress (clock ());

	Print (1, "Number of New Node Records = ") << count;
	Write (1, "Number of Express Entry Records = ") << num_entry;
	Write (1, "Number of Express Exit Records = ") << num_exit;
	Write (1, "Number of General Join Records = ") << num_join;

	node_file.Close ();
	new_file.Close ();
}

