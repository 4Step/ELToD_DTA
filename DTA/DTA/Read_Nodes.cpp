//*********************************************************
//	Read_Nodes.cpp - read node file
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Read_Nodes
//---------------------------------------------------------

void DTA::Read_Nodes (void)
{
	int node, type, index, num_entry, num_exit, num_join;
	Node_Data node_data;
	
	Show_Message ("Reading Node File -- Record");
	Set_Progress ();
	
	node_data.VCost (value_cost);
	max_zone = num_entry = num_exit = num_join = 0;

	while (node_file.Read ()) {
		Show_Progress ();

		node = node_file.Node ();
		if (node <= 0) continue;

		node_data.Node (node);

		type = node_file.Type ();

		node_data.Type (type);

		if (type == zone_type && node > max_zone) max_zone = node;

		if (entry_types.In_Range (type)) num_entry++;
		if (exit_types.In_Range (type)) num_exit++;
		if (join_types.In_Range (type)) num_join++;

		if (node_file.Cost_Flag ()) {
			node_data.VCost (node_file.VCost ());
		}
		index = (int) node_array.size ();

		if (!node_map.insert (Int_Map_Data (node, index)).second) {
			Warning (String ("Duplicate Node Number %d") % node);
		} else {
			node_array.push_back (node_data);
		}
	}
	End_Progress (clock ());

	Print (1, "Number of Node Records = ") << node_array.size ();
	Write (1, "Number of Express Entry Records = ") << num_entry;
	Write (1, "Number of Express Exit Records = ") << num_exit;
	Write (1, "Number of General Join Records = ") << num_join;
	Show_Message (1);
	Write (1, "Maximum Zone Number = ") << max_zone;
	Write (1);

	node_file.Close ();
}

