//*********************************************************
//	Gap_Data.cpp - gap calculation methods
//*********************************************************

#include "Data_Types.hpp"

//---------------------------------------------------------
//	Gap
//---------------------------------------------------------

double Gap_Data::Gap (void) 
{
	if (total > 1) {
		return (diff / total);
	} else if (diff > 0) {
		return (1.0);
	} else {
		return (0.0);
	}
}

//---------------------------------------------------------
//	Std_Dev
//---------------------------------------------------------

double Gap_Data::Std_Dev (void) 
{
	if (count > 1 && total > 1.0) {
		double std_dev = (diff_sq - diff * diff / count) / (count - 1.0);
		if (std_dev > 0) {
			return (sqrt (std_dev) * count / total);
		}
	}
	return (0);
}

//---------------------------------------------------------
//	RMSE
//---------------------------------------------------------

double Gap_Data::RMSE (void) 
{
	if (count > 0 && total > 1.0) {
		return (100.0 * sqrt (diff_sq / count) * count / total);
	} else {
		return (0.0);
	}
}

//---------------------------------------------------------
//	Add
//---------------------------------------------------------

void Gap_Data::Add (double previous, double current)
{
	double abs_diff = abs (current - previous);
	previous = MAX (current, previous);

	total += previous;
	diff += abs_diff;
	diff_sq += abs_diff * abs_diff;
	count++;

	if (previous > 1.0) {
		abs_diff = abs_diff / previous;
		if (abs_diff > max_gap) max_gap = abs_diff;
	}
	//if (current > 1.0) {
	//	abs_diff = abs_diff / current;
	//	if (abs_diff > max_gap) max_gap = abs_diff;
	//}
}

