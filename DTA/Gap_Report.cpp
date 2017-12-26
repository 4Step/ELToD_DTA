//*********************************************************
//	Gap_Report.cpp - print gap reports
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Gap_Report
//---------------------------------------------------------

void DTA::Gap_Report (void)
{
	int i;
	double factor, max;

	Gap_Itr gap_itr;

	Header_Number (CONVERGENCE_REPORT);

	if (!Break_Check ((int) gap_array.size () + 7)) {
		Print (1);
		Gap_Header ();
	}
	factor = 1.0;

	for (i=1, gap_itr = gap_array.begin (); gap_itr != gap_array.end (); gap_itr++, i++) {
		if (gap_itr->Count () == 0) continue;

		//---- print the data record ----

		max = gap_itr->Max_Gap ();
		if (max >= 1000000000.0) max = 999999999.999;

		Print (1, String ("%6d   %13.6lf %13.6lf %13.3lf %9.1lf %13.0lf %13.0lf") %
			i % gap_itr->Gap () % gap_itr->Std_Dev () % max % gap_itr->RMSE () % 
			(gap_itr->Difference () / factor) % (gap_itr->Total () / factor));
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Gap_Header
//---------------------------------------------------------

void DTA::Gap_Header (void)
{
	Print (1, "Convergence Gap Report");
	Print (2, "          --------------- Link Gap ---------------      %     --------- Hours ---------");
	Print (1, "Iteration        Total       Std.Dev       Maximum     RMSE     Difference         Total");
	Print (1);
}

/*********************************************|***********************************************

	Convergence Gap Report

              --------------- Link Gap ---------------      %     --------- Hours ---------
	Iteration        Total       Std.Dev       Maximum     RMSE    Difference         Total

	dddddd    fffff.ffffff  fffff.ffffff  fffffff.ffff   fffff.f  ffffffffffff ffffffffffff

**********************************************|***********************************************/ 
