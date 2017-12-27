//*********************************************************
//	Read_Links.cpp - read link file
//*********************************************************

#include "NodeFlags.hpp"

//---------------------------------------------------------
//	Read_Links
//---------------------------------------------------------

void NodeFlags::Read_Links (void)
{
	int num, link, anode, bnode, num_express, num_entry, num_exit;

	Int_Map_Itr map_itr;
	Link_Data link_data;
	Link_Itr link_itr;
	
	Show_Message ("Reading Link File -- Record");
	Set_Progress ();

	num_express = num_entry = num_exit = 0;

	while (link_file.Read ()) {
		Show_Progress ();

		anode = link_file.Node_A ();
		bnode = link_file.Node_B ();

		link = (int) link_array.size ();

		if (!link_map.insert (Int2_Map_Data (Int2_Key (anode, bnode), link)).second) {
			Warning (String ("Duplicate Link %d-%d Skipped") % anode % bnode);
			continue;
		}

		//---- convert the anode ----

		map_itr = node_map.find (anode);
		if (map_itr == node_map.end ()) {
			Warning (String ("Node %d was Not Found in the Node File") % anode);
			continue;
		}
		anode = map_itr->second;

		//---- convert the bnode ----

		map_itr = node_map.find (bnode);
		if (map_itr == node_map.end ()) {
			Warning (String ("Node %d was Not Found in the Node File") % bnode);
			continue;
		}
		bnode = map_itr->second;

		link_data.Clear ();
		link_data.Anode (anode);	
		link_data.Bnode (bnode);
		link_data.Type (link_file.Type ());
		link_data.Length (link_file.Length ());

		if (express_types.In_Range (link_data.Type ())) num_express++;
		if (entry_types.In_Range (link_data.Type ())) num_entry++;
		if (exit_types.In_Range (link_data.Type ())) num_exit++;

		link_array.push_back (link_data);
	}
	End_Progress (clock ());

	Print (1, "Number of Link Records = ") << link_array.size ();
	Write (1, "Number of Express Link Records = ") << num_express;
	Write (1, "Number of Express Entry Records = ") << num_entry;
	Write (1, "Number of Express Exit Records = ") << num_exit;
	Write (1);

	link_file.Close ();

	num = (int) node_map.size ();

	from_node.assign (num, -1);
	to_node.assign (num, -1);
	flags.assign (num, 0);

	num = (int) link_array.size ();

	link_in.assign (num, -1);
	link_out.assign (num, -1);

	for (link = 0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		link_out [link] = from_node [link_itr->Anode ()];
		from_node [link_itr->Anode ()] = link;

		link_in [link] = to_node [link_itr->Bnode ()];
		to_node [link_itr->Bnode ()] = link;

	}
}

