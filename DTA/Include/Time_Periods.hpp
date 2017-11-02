//*********************************************************
//	Time_Periods.hpp - time of day range class
//*********************************************************

#ifndef TIME_PERIODS_HPP
#define TIME_PERIODS_HPP

#include "Range_Data.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Time_Periods Class definition
//---------------------------------------------------------

class Time_Periods : public Range_Array, public Static_Service
{
public:
	Time_Periods (void);

	int    Period (int time);
	int    Period (int low, int high);
	bool   At_Increment (int time);
	bool   In_Range (int time)                 { return (Period (time) >= 0); }
	bool   Span_Range (int low, int high);
	bool   Span_Range (Range_Data range)       { return (Span_Range (range.Low (), range.High ())); }

	int    Num_Periods (void);
	int    Range_Length (void);
	int    Max_Count (void)                    { return (Range_Length ()); }

	int    Start (void)                        { return (_start); }
	void   Start (int value)                   { _start = value; }

	int    End (void)                          { return (_end_time); }
	void   End (int value)                     { _end_time = value; }

	int    Increment (void)                    { return (_increment); }
	void   Increment (int value)               { Set_Periods (value, Start (), End ()); }

	bool   Range_Flag (void)                   { return (_range_flag); }
	void   Range_Flag (bool flag)              { _range_flag = flag; }

	bool   Set_Periods (int increment, int start = 0, int end = 0);

	bool   Add_Ranges (string ranges);

	bool   Period_Range (int num, int &low, int &high);

	int    Period_Time (int num);

	bool   Copy_Periods (Time_Periods &periods);

	void   Clear ();

private:
	bool   _range_flag;
	int  _start, _end_time, _increment;
};

#endif
