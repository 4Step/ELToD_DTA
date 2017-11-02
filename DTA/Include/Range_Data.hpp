//*********************************************************
//	Range_Data.hpp - range data classes
//*********************************************************

#ifndef RANGE_DATA_HPP
#define RANGE_DATA_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Range_Data class definition
//---------------------------------------------------------

class Range_Data
{
public:
	Range_Data (void);
	Range_Data (int value)         { _low = _high = value; }
	Range_Data (int low, int high) { _low = low; _high = high; }

	int  Low (void)           { return (_low); }
	int  High (void)          { return (_high); }

	void Low (int value)      { _low = value; }
	void High (int value)     { _high = value; }

private:
	int  _low;
	int  _high;
};

//---------------------------------------------------------
//	Range_Array
//---------------------------------------------------------

class Range_Array : public vector <Range_Data> 
{
public:
	Range_Array (int max_records = 0)  { if (max_records > 0) reserve (max_records); }

	bool Add_Range (int low, int high, int increment = 0);
	int  Num_Ranges (void)              { return ((int) size ()); };

	bool In_Range (int value)           { return (In_Index (value) >= 0); }
	bool Span_Range (int low, int high) { return (Span_Low_Index (low, high) >= 0); }
	bool Span_Range (Range_Data range)  { return (Span_Range (range.Low (), range.High ())); }

	int  Max_Value (void);
	int  Min_Value (void);
	int  Max_Count (void);
	int  Range_Count (int value);
	int  Range_Count (void);
	
	int  At_Index (int value);
	int  In_Index (int value);
	int  Span_Low_Index (int low, int high);
	int  Span_High_Index (int low, int high);
};

typedef vector <Range_Data>::iterator          Range_Array_Itr;
typedef vector <Range_Data>::reverse_iterator  Range_Array_RItr;
#endif
