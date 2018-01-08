//*********************************************************
//	Distribution_Report.cpp - print choice distribution reports
//*********************************************************

#include "DTA.hpp"

//---------------------------------------------------------
//	Distribution_Report
//---------------------------------------------------------

void DTA::Distribution_Report (void)
{
	int i, max, total, nonzero;
	double percent, weight;

	Header_Number (CHOICE_DISTRIBUTION);

	max = total = nonzero = 0;
	weight = 0;

	for (i = 0; i < num_distb; i++) {
		total += choice_distb [i];
		if (i) nonzero += choice_distb [i];
		weight += (double) i * choice_distb [i];

		if (choice_distb [i] > 0) max = i;
	}

	if (!Break_Check (max + 7)) {
		Print (1);
		Distribution_Header ();
	}
	if (total == 0) total = 1;

	for (i = 0; i <= max; i++) {
		percent = choice_distb [i] * 100.0 / total;
		Print (1, String (" %4d    %10d   %6.2lf") % i % choice_distb [i] % percent);
	}
	Print (2, String ("Total    %10d") % total);

	percent = nonzero * 100.0 / total;

	Print (1, String ("NonZero  %10d   %6.2lf") % nonzero % percent);

	Print (1, String ("Average  %10.1lf") % (weight / nonzero));

	Header_Number (0);
}

//---------------------------------------------------------
//	Distribution_Header
//---------------------------------------------------------

void DTA::Distribution_Header (void)
{
	Print (1, "Choice Distribution Report");
	Print (2, " Number      Number");
	Print (1, "Choices   O-D Pairs  Percent");
	Print (1);
}

/*********************************************|***********************************************

	Choice Distribution Report

	 Number      Number
	Choices   O-D Pairs  Percent

	 dddd    dddddddddd   fff.ff

	Total    dddddddddd
	NonZero  dddddddddd   fff.ff
	Average       fff.f

**********************************************|***********************************************/ 
