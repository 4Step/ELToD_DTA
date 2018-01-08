//*********************************************************
//	Assign_Trips.cpp - build paths and load trips
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Assign_Trips constructor / destructor
//---------------------------------------------------------

DTA::Assign_Trips::Assign_Trips (DTA *_exe, int id)
{
	exe = _exe;
	thread = id;

	if (exe->thread_flag) {
		Volume_Data volume_data (exe->num_period, exe->num_mode);
		volume_array.assign (exe->num_link, volume_data);

		volume_array_ptr = &volume_array;

		num_alt_path = num_path_build = num_od_loads = num_choices = 0;

		alt_path_ptr = &num_alt_path;
		path_build_ptr = &num_path_build;
		od_loads_ptr = &num_od_loads;
		choice_ptr = &num_choices;
	} else {
		volume_array_ptr = &exe->volume_array;

		alt_path_ptr = &exe->num_alt_path;
		path_build_ptr = &exe->num_path_build;
		od_loads_ptr = &exe->num_od_loads;
		choice_ptr = &exe->num_choices;
	}
}

DTA::Assign_Trips::~Assign_Trips (void)
{
	if (exe->thread_flag) {
		volume_array.clear ();
	}
}

//---------------------------------------------------------
//	Assign_Trips operator
//---------------------------------------------------------

void DTA::Assign_Trips::operator()()
{
	//---- process each plan ----

	Assign_Data assign_data;

	while (exe->assign_queue.Get (assign_data)) {
		Process_Trips (assign_data);
		exe->assign_queue.Finished ();
	}
}

//---------------------------------------------------------
//	Assign_Trips::Combine_Volumes
//---------------------------------------------------------

void DTA::Assign_Trips::Combine_Volume (void)
{
	if (exe->thread_flag) {
		int link, period;
		Volume_Itr vol_itr;
		Volume_Data *vol_ptr;

		for (link = 0, vol_itr = volume_array.begin (); vol_itr != volume_array.end (); vol_itr++, link++) {
			vol_ptr = &exe->volume_array [link];

			for (period = 0; period < exe->num_period; period++) {
				for (mode = 0; mode < exe->num_mode; mode++) {
					vol_ptr->Add_Volume (mode, period, vol_itr->Volume (mode, period));
					vol_itr->Volume (mode, period, 0);
				}
			}
		}
		exe->num_alt_path += num_alt_path;
		exe->num_path_build += num_path_build;
		exe->num_od_loads += num_od_loads;
		exe->num_choices += num_choices;

		num_alt_path = num_path_build = num_od_loads = num_choices = 0;
	}
}

//---------------------------------------------------------
//	Assign_Trips::Clear
//---------------------------------------------------------

void DTA::Assign_Trips::Clear (void) 
{
	volume_array.clear ();
}
