//*********************************************************
//	Read_Nodes.cpp - read node file
//*********************************************************

#include "NodeFlags.hpp"

//---------------------------------------------------------
//	Read_Nodes
//---------------------------------------------------------

void NodeFlags::Read_Nodes (void)
{
	int node, index;
	
	Show_Message ("Reading Node File -- Record");
	Set_Progress ();

	while (node_file.Read ()) {
		Show_Progress ();

		node = node_file.Node ();
		if (node <= 0) continue;

		index = (int) node_map.size ();

		if (!node_map.insert (Int_Map_Data (node, index)).second) {
			Warning (String ("Duplicate Node Number %d") % node);
		}
	}
	End_Progress (clock ());

	Print (1, "Number of Node Records = ") << node_map.size ();

	node_file.Rewind ();
}

