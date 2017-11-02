//*********************************************************
//	Read_Tolls.cpp - read toll file
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Read_Tolls
//---------------------------------------------------------

void DTA::Read_Tolls (void)
{
	int anode, bnode, link;

	Toll_Data toll_data;
	Link_Data *link_ptr;
	Int2_Map_Itr map_itr;

	toll_data.Num_Periods (num_period);
	
	Show_Message ("Reading Toll File -- Record");
	Set_Progress ();

	while (toll_file.Read ()) {
		Show_Progress ();

		anode = toll_file.Node_A ();
		bnode = toll_file.Node_B ();

		map_itr = link_map.find (Int2_Key (anode, bnode));

		if (map_itr == link_map.end ()) {
			Warning (String ("Toll Link %d-%d was Not Found") % anode % bnode);
			continue;
		}
		link = map_itr->second;

		link_ptr = &link_array [link];

		toll_data.Policy (toll_file.Policy ());

		if (policy_range.In_Range (toll_data.Policy ())) {
			if (!express_types.In_Range (link_ptr->Type ())) {
				Warning (String ("Toll Link %d-%d is Not an Express Link Type %d") % anode % bnode % link_ptr->Type ());
			}
			link_ptr->Ex_Toll ((int) toll_array.size ());

			toll_data.Link (link);

			toll_data.Segment (toll_file.Segment ());
			toll_data.Length (toll_file.Length ());

			toll_data.Num_Periods (num_period);

			toll_array.push_back (toll_data);

			//link_ptr->Toll (min_toll);
		} else {
			link_ptr->Toll (toll_file.Toll ());
		}
	}
	End_Progress (clock ());

	Print (1, "Number of Toll Records = ") << Progress_Count ();
	
	Write (1, "Number of Express Toll Records = ") << toll_array.size ();
	Write (1);

	toll_file.Close ();
}

