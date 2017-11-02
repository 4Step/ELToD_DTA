//*********************************************************
//	Update_Time_Cost.cpp - apply VDF functions
//*********************************************************

#include "DTA.hpp"

#include <math.h>

//---------------------------------------------------------
//	Update_Time_Cost
//---------------------------------------------------------

void DTA::Update_Time_Cost (int iteration)
{
	int link, period, mode, index;
	double volume, vol, time, vc, factor, cost, toll;

	Volume_Itr vol_itr;
	Volume_Ptr old_vol_ptr;
	Link_Data *link_ptr;
	Toll_Data *toll_ptr;

	factor = 1.0 / iteration;

	for (link = 0, vol_itr = volume_array.begin (); vol_itr != volume_array.end (); vol_itr++, link++) {
		if (iteration > 1) {
			old_vol_ptr = &old_volume_array [link];
		} else {
			old_vol_ptr = 0;
		}
		link_ptr = &link_array [link];

		for (period = 0; period < num_period; period++) {
			volume = 0;

			for (mode = 0; mode < num_mode; mode++) {
				vol = vol_itr->Volume (mode, period);

				if (iteration > 1) {
					vol = vol * factor + (1.0 - factor) * old_vol_ptr->Volume (mode, period);
					vol_itr->Volume (mode, period, vol);
				}

				volume += vol;
			}
			vc = (volume / link_ptr->Cap ());

			if (volume == 0) {
				time = link_ptr->Time0 ();
			} else {
				time = link_ptr->Time0 () * (1 + link_ptr->Alpha () * pow ((volume / (link_ptr->Cap ())), link_ptr->Beta ()));
			}
			link_ptr->TTime (period, time);

			//---- adjust cost ----

			if (link_ptr->Ex_Toll () >= 0) {
				toll_ptr = &toll_array [link_ptr->Ex_Toll ()];

				if (toll_ptr->Segment () > 0) {
					cost = 0;

					index = policy_range.At_Index (toll_ptr->Policy ());

					if (vc > max_vc [index]) vc = max_vc [index];
					cost = min_toll [index] + (max_toll [index] - min_toll [index]) * pow (vc + vc_offset [index], exponent [index]);
					toll = toll_ptr->Toll (period);
					//toll = min_toll + toll_ptr->Toll (period);

					if (abs (cost - toll) > max_change [index]) {
						if (cost < toll) {
							cost = toll - max_change [index];
						} else {
							cost = toll + max_change [index];
						}
					}
					if (cost < min_toll [index]) {
						cost = min_toll [index];
					} else if (cost > max_toll [index]) {
						cost = max_toll [index];
					}
					//cost -= min_toll;
	
					toll_ptr->Toll (period, cost);
				}
			}
		}
	}
}

