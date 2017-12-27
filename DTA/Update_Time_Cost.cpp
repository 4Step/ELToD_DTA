//*********************************************************
//	Update_Time_Cost.cpp - apply VDF functions
//*********************************************************

#include "DTA.hpp"

#include <math.h>

//---------------------------------------------------------
//	Update_Time_Cost
//---------------------------------------------------------

double DTA::Update_Time_Cost (int iteration)
{
	int link, period, mode, index;
	double volume, vol, time, vc, factor, cost, toll, old_time, old_vol, old, max_time;

	Volume_Itr vol_itr;
	Volume_Ptr old_vol_ptr;
	Link_Data *link_ptr;
	Toll_Data *toll_ptr;
	Gap_Data gap_data, toll_gap;

	factor = 1.0 / iteration;

	for (link = 0, vol_itr = volume_array.begin (); vol_itr != volume_array.end (); vol_itr++, link++) {
		if (iteration > 1) {
			old_vol_ptr = &old_volume_array [link];
		} else {
			old_vol_ptr = 0;
		}
		link_ptr = &link_array [link];

		max_time = link_ptr->Length () * 60.0 / min_speed;

		for (period = 0; period < num_period; period++) {
			volume = old_vol = 0;

			for (mode = 0; mode < num_mode; mode++) {
				vol = vol_itr->Volume (mode, period);

				if (iteration > 1) {
					old = old_vol_ptr->Volume (mode, period);

					vol = vol * factor + (1.0 - factor) * old;
					vol_itr->Volume (mode, period, vol);
				} else {
					old = 0;
				}
				old_vol += old;
				volume += vol;
			}
			vc = (volume / link_ptr->Cap ());

			if (volume == 0) {
				time = link_ptr->Time0 ();
			} else {
				time = link_ptr->Time0 () * (1 + link_ptr->Alpha () * pow ((volume / (link_ptr->Cap ())), link_ptr->Beta ()));
				if (time > max_time) time = max_time;
			}
			old_time = link_ptr->TTime (period);

			gap_data.Add (old_time * old_vol / 60.0, time * volume / 60.0);

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

					if (toll_gap_flag) {
						toll_gap.Add (toll_ptr->Toll (period) * old_vol, cost * volume);
					}
					toll_ptr->Toll (period, cost);
				}
			}
		}
	}
	if (link_gap_flag) {
		link_gap_array.push_back (gap_data);

		if (link_gap_file.Is_Open ()) {
			link_gap_file.Iteration (iteration);
			link_gap_file.Gap (gap_data.Gap ());
			link_gap_file.Std_Dev (gap_data.Std_Dev ());
			link_gap_file.Maximum (gap_data.Max_Gap ());
			link_gap_file.RMSE (gap_data.RMSE ());
			link_gap_file.Difference (gap_data.Difference ());
			link_gap_file.Total (gap_data.Total ());

			link_gap_file.Write ();
		}
	}
	if (toll_gap_flag) {
		toll_gap_array.push_back (toll_gap);

		if (toll_gap_file.Is_Open ()) {
			toll_gap_file.Iteration (iteration);
			toll_gap_file.Gap (toll_gap.Gap ());
			toll_gap_file.Std_Dev (toll_gap.Std_Dev ());
			toll_gap_file.Maximum (toll_gap.Max_Gap ());
			toll_gap_file.RMSE (toll_gap.RMSE ());
			toll_gap_file.Difference (toll_gap.Difference ());
			toll_gap_file.Total (toll_gap.Total ());

			toll_gap_file.Write ();
		}
	}
	return (gap_data.Gap ());
}

