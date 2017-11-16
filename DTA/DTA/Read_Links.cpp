//*********************************************************
//	Read_Links.cpp - read link file
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Read_Links
//---------------------------------------------------------

void DTA::Read_Links (void)
{
	int i, link, anode, bnode, index, num_express;
	double cap, cap_factor, speed;
	bool flag;

	Int_Map_Itr map_itr;
	Link_Data link_data;
	Link_Itr link_itr;
	Volume_Data volume_data (num_period, num_mode);

	cap_factor = time_periods.Increment () / 60.0;
	
	Show_Message ("Reading Link File -- Record");
	Set_Progress ();

	num_express = 0;

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
		anode = map_itr->second; // index of Anode

		//---- convert the bnode ----

		map_itr = node_map.find (bnode);
		if (map_itr == node_map.end ()) {
			Warning (String ("Node %d was Not Found in the Node File") % bnode);
			continue;
		}
		bnode = map_itr->second; // index of Bnode

		link_data.Clear ();
		link_data.Anode (anode);	
		link_data.Bnode (bnode);
		link_data.Type (link_file.Type ());
		link_data.Length (link_file.Length ());

		if (express_types.In_Range (link_data.Type ())) num_express++;

		cap = link_file.Cap () * cap_factor;         // link capacity by time period
		if (link_file.Lanes_Flag ()) {               // if lanes > 0
			cap *= link_file.Lanes ();               // lanes * capacity / period
		}
		link_data.Cap (cap);
		link_data.Toll (link_file.Toll ());
		link_data.Alpha (link_file.Alpha ());
		link_data.Beta (link_file.Beta ());

		for (i = 0; i < num_mode; i++) {
			index = mode_fields [i];

			if (index >= 0) {
				flag = (link_file.Get_Integer (index) != 0);
			} else {
				flag = true;
			}
			link_data.Mode_Flag (i, flag);
		}

		if (link_file.Time_Flag ()) {
			link_data.Time0 (link_file.Time ());
		} else if (link_file.Speed_Flag ()) {
			speed = link_file.Speed ();
			if (speed == 0) speed = 25.0;

			link_data.Time0 (link_data.Length () * 60.0 / speed); // free flow travel time
		}
		link_data.Num_Periods (num_period);

		link_array.push_back (link_data);
	}
	End_Progress (clock ());

	Print (1, "Number of Link Records = ") << link_array.size ();
	Write (1, "Number of Express Link Records = ") << num_express;
	Write (1);

	link_file.Close ();

	num_link = (int) link_array.size ();
	num_node = (int) node_array.size ();

	volume_array.assign (num_link, volume_data);

	node_link.assign (num_node, -1);
	next_link.assign (num_link, -1);

	for (link = 0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		next_link [link] = node_link [link_itr->Anode ()];
		node_link [link_itr->Anode ()] = link;
	}
}

