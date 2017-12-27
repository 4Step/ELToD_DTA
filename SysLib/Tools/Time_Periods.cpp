//*********************************************************
//	Time_Periods.cpp - data range functions
//*********************************************************

#include "Time_Periods.hpp"

#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Time_Periods -- constructor
//---------------------------------------------------------

Time_Periods::Time_Periods (void) : Range_Array (1), Static_Service () 
{
	_range_flag = _wrap_flag = false;
	_increment = _start = _end_time = 0;
}

//---------------------------------------------------------
//	Period
//---------------------------------------------------------

int Time_Periods::Period (int time)
{
	if (_range_flag) {
		return (In_Index ((int) time));
	} else if (time >= _start) {
		if (time >= _end_time) {
			if (!_wrap_flag || _end_time <= _start) return (-1);
			time = (time - _start) % (_end_time - _start);
		} else {
			time -= _start;
		}
		if (_increment > 0) {
			return (time / _increment);
		} else {
			return (0);
		}
	}
	return (-1);
}

int Time_Periods::Period (int low, int high)
{
	low = (low + high) / 2;
	return (Period (low));
}

//---------------------------------------------------------
//	At_Increment
//---------------------------------------------------------

bool Time_Periods::At_Increment (int time)
{
	if (_range_flag) {
		int low = In_Index ((int) time);
		if (low < 0) {
			low = In_Index ((int) (time-1));
			if (low < 0) return (false);
			low = at (low).High () + 1;
		} else if (low == 0) {
			low = at (low).High () + 1;
		} else {
			low = at (low).Low ();
		}
		return ((int) time == low);
	} else if (time >= _start) {
		if (time > _end_time) {
			if (_end_time <= _start) return (false);
			time = (time - _start) % (_end_time - _start);
		} else {
			time -= _start;
		}
		if (_increment > 0) {
			return ((time % _increment) == 0);
		} else {
			return ((int) time == 0);
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Span_Range
//---------------------------------------------------------

bool Time_Periods::Span_Range (int low, int high)
{
	if (_range_flag) {
		return (Range_Array::Span_Range (low, high));
	} else if (high >= _start && low <= _end_time) {
		return (true);
	} else if (low > _end_time) {
		int day = MIDNIGHT;
		low = low % day;
		high = high % day;
		return (high >= _start && low <= _end_time);
	}
	return (false);
}

//---------------------------------------------------------
//	Num_Periods
//---------------------------------------------------------

int Time_Periods::Num_Periods (void)
{
	if (_range_flag) {
		return (Num_Ranges ());
	} else if (_increment <= 0) {
		if (_end_time > _start) {
			return (1);
		} else {
			return (0);
		}
	} else if (_end_time > _start) {
		return ((_end_time - _start - 1) / _increment + 1);
	}
	return (0);
}

//---------------------------------------------------------
//	Range_Length
//---------------------------------------------------------

int Time_Periods::Range_Length (void)
{
	if (_range_flag) {
		return (Range_Count ());
	} else if (_end_time > _start) {
		return (_end_time - _start);
	}
	return (0);
}

//---------------------------------------------------------
//	Set_Periods
//---------------------------------------------------------

bool Time_Periods::Set_Periods (int increment, int start, int end, bool wrap)
{
	_increment = increment;
	_wrap_flag = wrap;
	_range_flag = false;

	if (start == 0 && end == 0) {
		if (_end_time == 0) {
			_start = exe->Model_Start_Time ();
			if (_wrap_flag) {
				_end_time = MIN ((_start + Round (MIDNIGHT)), exe->Model_End_Time ());
			} else {
				_end_time = exe->Model_End_Time ();
			}
		}
	} else if (start > end) {
		if (end != 0) return (false);
		_start = start;
		if (_wrap_flag) {
			_end_time = MIN ((_start + Round (MIDNIGHT)), exe->Model_End_Time ());
		} else {
			_end_time = exe->Model_End_Time ();
		}
	} else {
		_start = start;
		if (_wrap_flag) {
			_end_time = MIN ((_start + Round (MIDNIGHT)), end);
		} else {
			_end_time = end;
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Add_Ranges
//---------------------------------------------------------

bool Time_Periods::Add_Ranges (string range)
{
	int low, high;
	int periods;
	String text, low_text, high_text;
	Strings ranges;
	Str_Itr itr;

	text = range;
	text.Trim ();
	if (text.empty ()) return (false);

	if (text.Equals ("All")) {
		if (_range_flag) {
			low = exe->Model_Start_Time ();
			high = exe->Model_End_Time ();

			return (Add_Range ((int) low, (int) high - 1, _increment));
		} else {
			return (Set_Periods (_increment, _start, _end_time, _wrap_flag));
		}
	}

	//---- unpack the range string ----	

	periods = text.Parse (ranges);
	_range_flag = (periods > 1 || _range_flag);

	for (itr = ranges.begin (); itr != ranges.end (); itr++) {
		if (!itr->Range (low_text, high_text)) return (false);

		low = low_text.Integer ();
		high = high_text.Integer ();

		if (low == high) high = high + 1;

		if (_range_flag) {
			if (!Add_Range (low, high - 1, _increment)) return (false);
		} else {
			return (Set_Periods (_increment, low, high, _wrap_flag));
		}
	}
	return (Num_Ranges () > 0);
}

//---------------------------------------------------------
//	Period_Range
//---------------------------------------------------------

bool Time_Periods::Period_Range (int num, int &low, int &high)
{
	low = high = 0;
	if (num < 0 || num >= Num_Periods ()) return (false);

	if (_range_flag) {
		Range_Data &ref = at (num);

		low = ref.Low ();
		high = ref.High () + 1;
	} else if (_increment <= 0) {
		low = _start;
		high = _end_time;
	} else {
		low = _start + num * (int) _increment;
		high = low + _increment;
		if (high > _end_time) high = _end_time;
	}
	return (true);
}

//---------------------------------------------------------
//	Period_Time
//---------------------------------------------------------

int Time_Periods::Period_Time (int num)
{
	int low, high;
	Period_Range (num, low, high);
	return ((low + high) / 2);
}

//---------------------------------------------------------
//	Copy_Periods
//---------------------------------------------------------

bool Time_Periods::Copy_Periods (Time_Periods &periods)
{
	if (periods.Num_Periods () == 0) return (false);

	_range_flag = periods.Range_Flag ();

	if (_range_flag) {
		assign (periods.begin (), periods.end ());
	} else {
		Set_Periods (periods.Increment (), periods.Start (), periods.End (), periods.Wrap_Flag ());
	}
	return (true);
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Time_Periods::Clear (void)
{
	_range_flag = _wrap_flag = false;
	_increment = _start = _end_time = 0;
	clear ();
}
