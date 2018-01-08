//*********************************************************
//	Process_Trips.cpp - build a node-based path and load
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Process_Trips
//---------------------------------------------------------

void DTA::Assign_Trips::Process_Trips (Assign_Data data)
{
	double trips;

	Floats *trip_ptr;
	Flt_Itr trip_itr, cost_itr;
	Int_Map_Itr map_itr;

	org = data.Origin ();
	start = data.Start ();

	map_itr = exe->node_map.find (org);
	if (map_itr == exe->node_map.end ()) return;

	root = map_itr->second;

	if (exe->node_file.Cost_Flag ()) {
		vcost = exe->node_array [root].VCost ();
	} else {
		vcost = exe->value_cost;
	}

	for (mode = 0; mode < exe->num_mode; mode++) {
		trips = 0;
		trip_ptr = data.Trips (mode);

		for (trip_itr = trip_ptr->begin (); trip_itr != trip_ptr->end (); trip_itr++) {
			trips += *trip_itr;
		}
		if (trips == 0.0) continue;

		Build_Path (trip_ptr);
	}
}
