//*********************************************************
//	Write_Volumes.cpp - write the link volume file
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Write_Volumes
//---------------------------------------------------------

void DTA::Write_Volumes (void)
{
	int period, mode;
	double speed, time;

	Int2_Map_Itr map_itr;
	Volume_Data *vol_ptr;
	Link_Data *link_ptr;
	
	Show_Message ("Writing Volume File -- Record");
	Set_Progress ();

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];

		volume_file.Node_A (node_array [link_ptr->Anode ()].Node ());
		volume_file.Node_B (node_array [link_ptr->Bnode ()].Node ());

		vol_ptr = &volume_array [map_itr->second];

		for (period = 0; period < num_period; period++) {
			volume_file.Period (period + 1);

			for (mode = 0; mode < num_mode; mode++) {
				volume_file.Volume (mode, vol_ptr->Volume (mode, period));
			}
			time = link_ptr->TTime (period);

			if (time > 0) {
				speed = link_ptr->Length () * 60.0 / time;
			} else {
				speed = 0.0;
			}
			volume_file.Speed (speed);
			
			volume_file.Write ();
		}
	}
	End_Progress (clock ());

	volume_file.Close ();
}

