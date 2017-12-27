//*********************************************************
//	Data_Types.hpp - data types and classes
//*********************************************************

#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP

#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Link_Data class definition
//---------------------------------------------------------

class Link_Data
{
public:
	Link_Data (void)                { Clear (); }
	Link_Data (int num_period)      { Clear (); Num_Periods (num_period); }

	virtual ~Link_Data (void)       { Clear (); ttime.clear (); }

	int    Anode (void)             { return (anode); }
	int    Bnode (void)             { return (bnode); }
	int    Type (void)              { return (type); }
	int    Ex_Toll (void)           { return (ex_toll); }
	double Length (void)            { return (length); }
	double Time0 (void)             { return (time0); }
	double Cap (void)               { return (cap); }
	double Toll (void)              { return (toll); }
	double Alpha (void)             { return (alpha); }
	double Beta (void)              { return (beta); }
	double TTime (int period)       { return (ttime [period]); }
	bool   Mode_Flag (int mode)     { return ((mode_flag & (1 << mode)) != 0); }

	void   Anode (int value)        { anode = value; }
	void   Bnode (int value)        { bnode = value; }
	void   Type (int value)         { type = value; }
	void   Ex_Toll (int value)      { ex_toll = value; }
	void   Length (double value)    { length = (float) value; }
	void   Time0 (double value)     { time0 = (float) value; }
	void   Cap (double value)       { cap = (float) value; }
	void   Toll (double value)      { toll = (float) value; }
	void   Alpha (double value)     { alpha = (float) value; }
	void   Beta (double value)      { beta = (float) value; }
	void   TTime (int period, double value)   { ttime [period] = (float) value; }
	void   Mode_Flag (int mode, bool flag)    { if (flag) mode_flag |= (1 << mode); }

	void   Num_Periods (int value)  { ttime.assign (value, time0); }
	int    Num_Periods (void)       { return ((int) ttime.size ()); }

	void   Clear (void)	{
		anode = bnode = type = mode_flag = 0;  ex_toll = -1;  length = time0 = cap = toll = alpha = beta = 0;
	}

private:
	int     anode;
	int     bnode;
	int     type;
	int     ex_toll;
	int     mode_flag;
	float   length;
	float   time0;
	float   cap;
	float   toll;
	float   alpha;
	float   beta;
	Floats  ttime;
};

typedef vector <Link_Data>    Link_Array;
typedef Link_Array::iterator  Link_Itr;
typedef Link_Array::pointer   Link_Ptr;

//---------------------------------------------------------
//	Node_Data class definition
//---------------------------------------------------------

class Node_Data
{
public:
	Node_Data (void)             { Clear (); }

	int    Node (void)           { return (node); }
	int    Type (void)           { return (type); }
	double VCost (void)          { return (cost); }

	void   Node (int value)      { node = value; }
	void   Type (int value)      { type = value; }
	void   VCost (double value)  { cost = (float) value; }

	void   Clear (void) { node = type = 0; cost = 0.0; }

private:
	int   node;
	int   type;
	float cost;
};

typedef vector <Node_Data>    Node_Array;
typedef Node_Array::iterator  Node_Itr;
typedef Node_Array::pointer   Node_Ptr;

//---------------------------------------------------------
//	Volume_Data class definition
//---------------------------------------------------------

class Volume_Data
{
public:
	Volume_Data (void)                                     { Clear (); }
	Volume_Data (int num_period, int num_mode)             { Initialize (num_period, num_mode); }
	
	virtual ~Volume_Data (void)                            { Clear (); }

	double Volume (int mode, int period)                   { return (volume [mode] [period]); }

	void   Volume (int mode, int period, double value)     { volume [mode] [period] = (float) value; }

	void   Add_Volume (int mode, int period, double value) { volume [mode] [period] += (float) value; }

	void   Initialize (int num_period, int num_mode) {
		Floats temp (num_period, 0);
		volume.assign (num_mode, temp);
	}
	void   Clear (void)	{
		volume.clear ();
	}
private:
	Flts_Array volume;
};

typedef vector <Volume_Data>    Volume_Array;
typedef Volume_Array::iterator  Volume_Itr;
typedef Volume_Array::pointer   Volume_Ptr;

//---------------------------------------------------------
//	Assign_Data class definition
//---------------------------------------------------------

class Assign_Data
{
public:
	Assign_Data (void)                    { Clear (); }
	Assign_Data (int modes, int zones)    { Clear (); num_mode = modes; max_zone = zones; Zero_Trips (); }

	virtual ~Assign_Data (void)           { Clear (); trips.clear (); }

	int    Origin (void)                  { return (origin); }
	int    Start (void)                   { return (start); }
	double Trips (int mode, int des)      { return (trips [mode] [des]); }

	void   Origin (int value)             { origin = value; }
	void   Start (int value)              { start = value; }
	void   Trips (int mode, int des, double value)  { trips [mode] [des] = (float) value; }

	int    Num_Modes (void)               { return (num_mode); }
	int    Max_Zone (void)                { return (max_zone); }

	void   Num_Modes (int value)          { num_mode = value; }
	void   Max_Zone (int value)           { max_zone = value; }

	void   Zero_Trips (void)              { Floats row; row.assign (max_zone, 0); trips.assign (num_mode, row); }
	void   Clear (void)                   { origin = start = num_mode = max_zone = 0; }

	Floats * Trips (int mode)             { return (&trips [mode]); }

private:
	int  origin;
	int  start;
	int  num_mode;
	int  max_zone;
	Flts_Array trips;
};

typedef vector <Assign_Data>    Assign_Array;
typedef Assign_Array::iterator  Assign_Itr;
typedef Assign_Array::pointer   Assign_Ptr;

//---------------------------------------------------------
//	Toll_Data class definition
//---------------------------------------------------------

class Toll_Data
{
public:
	Toll_Data (void)                        { Clear (); }
	Toll_Data (int num_period)              { Clear (); Num_Periods (num_period); }

	virtual ~Toll_Data (void)               { Clear (); tolls.clear (); }

	int    Link (void)                      { return (link); }
	int    Segment (void)                   { return (segment); }
	int    Policy (void)                    { return (policy); }
	double Length (void)                    { return (length); }
	double Toll (int period)                { return (tolls [period]); }

	void   Link (int value)                 { link = value; }
	void   Segment (int value)              { segment = value; }
	void   Policy (int value)               { policy = value; }
	void   Length (double value)            { length = (float) value; }
	void   Toll (int period, double value)  { tolls [period] = (float) value; }

	void   Num_Periods (int value)          { tolls.assign (value, 0); }
	int    Num_Periods (void)               { return ((int) tolls.size ()); }

	void   Clear (void) {
		link = segment = policy = 0;  length = 0;
	}

private:
	int     link;
	int     segment;
	int     policy;
	float   length;
	Floats  tolls;
};

typedef vector <Toll_Data>    Toll_Array;
typedef Toll_Array::iterator  Toll_Itr;
typedef Toll_Array::pointer   Toll_Ptr;

//---------------------------------------------------------
//	Path_Data class definition
//---------------------------------------------------------

class Path_Data
{
public:
	Path_Data (void)             { Clear (); }

	double Imped (void)          { return (imped); }
	double Time (void)           { return (time); }
	double Cost (void)           { return (cost); }
	double Length (void)         { return (length); }
	int    Next (void)           { return (next); }
	int    From (void)           { return (from); }
	int    Link (void)           { return (link); }

	void   Imped (double value)  { imped = (float) value; }
	void   Time (double value)   { time = (float) value; }
	void   Cost (double value)   { cost = (float) value; }
	void   Length (double value) { length = (float) value; }
	void   Next (int value)      { next = value; }
	void   From (int value)      { from = value; }
	void   Link (int value)      { link = value; }

	void  Clear (void) {
		imped = cost = time = length = 0.0;  next = from = link = -1;
	}
private:
	float imped;
	float time;
	float cost;
	float length;
	int   next;
	int   from;
	int   link;
};

typedef vector <Path_Data>    Path_Array;
typedef Path_Array::iterator  Path_Itr;

//---------------------------------------------------------
//	Path_Leg_Data class definition
//---------------------------------------------------------

class Path_Leg_Data
{
public:
	Path_Leg_Data (void)          { Clear (); }

	int    Node (void)            { return (node); }
	int    Link (void)            { return (link); }
	double Time (void)            { return (time); }
	double Cost (void)            { return (cost); }
	double Length (void)          { return (length); }

	void   Node (int value)       { node = value; }
	void   Link (int value)       { link = value; }
	void   Time (double value)    { time = (float) value; }
	void   Cost (double value)    { cost = (float) value; }
	void   Length (double value)  { length = (float) value; }

	void   Clear (void)           { node = link = -1; time = cost = length = 0; }

private:
	int   node;
	int   link;
	float time;
	float cost;
	float length;
};

typedef vector <Path_Leg_Data>            Path_Leg_Array;
typedef Path_Leg_Array::iterator          Path_Leg_Itr;
typedef Path_Leg_Array::reverse_iterator  Path_Leg_RItr;

//---------------------------------------------------------
//	Gap_Data class definition
//---------------------------------------------------------

class Gap_Data
{
public:
	Gap_Data (void)           { Clear (); }

	double Gap (void);
	double Max_Gap (void)     { return (max_gap); }
	double Std_Dev (void);
	double RMSE (void);
	double Difference (void)  { return (diff); }
	double Total (void)       { return (total); }
	int    Count (void)       { return (count); }

	void   Add (double previous, double current);

	void   Clear (void) { total = diff = diff_sq = max_gap = 0; count = 0; }

private:
	double total;
	double diff;
	double diff_sq;
	double max_gap;
	int    count;
};

typedef vector <Gap_Data>    Gap_Array;
typedef Gap_Array::iterator  Gap_Itr;
typedef Gap_Array::pointer   Gap_Ptr;

//---------------------------------------------------------
//	Choice_Model class definition
//---------------------------------------------------------

class Choice_Model
{
public:
	Choice_Model (void)                    { Clear (); }

	double Time_Factor (void)              { return (time_fac); }
	double Toll_Factor (void)              { return (toll_fac); }
	double Rely_Ratio (void)               { return (rely_ratio); }
	double Rely_Time (void)                { return (rely_time); }
	double Rely_Dist (void)                { return (rely_dist); }
	double Perceive_Time (void)            { return (perceive_time); }
	double Perceive_Mid_VC (void)          { return (perceive_mid_vc); }
	double Perceive_Max_VC (void)          { return (perceive_max_vc); }
	double Express_Weight (void)           { return (express_weight); }
	double Scale_Length (void)             { return (scale_len); }
	double Scale_Alpha (void)              { return (scale_alpha); }
	
	void   Time_Factor (double value)      { time_fac = value; }
	void   Toll_Factor (double value)      { toll_fac = value; }
	void   Rely_Ratio (double value)       { rely_ratio = value; }
	void   Rely_Time (double value)        { rely_time = value; }
	void   Rely_Dist (double value)        { rely_dist = value; }
	void   Perceive_Time (double value)    { perceive_time = value; }
	void   Perceive_Mid_VC (double value)  { perceive_mid_vc = value; }
	void   Perceive_Max_VC (double value)  { perceive_max_vc = value; }
	void   Express_Weight (double value)   { express_weight = value; }
	void   Scale_Length (double value)     { scale_len = value; }
	void   Scale_Alpha (double value)      { scale_alpha = value; }

	void   Clear (void) { time_fac = toll_fac = rely_ratio = rely_time = rely_dist = perceive_time = perceive_mid_vc = perceive_max_vc = express_weight = scale_len = scale_alpha; }

private:
	double time_fac;
	double toll_fac;
	double rely_ratio;
	double rely_time;
	double rely_dist;
	double perceive_time;
	double perceive_mid_vc;
	double perceive_max_vc;
	double express_weight;
	double scale_len;
	double scale_alpha;
};

typedef vector <Choice_Model>  Model_Array;
typedef Model_Array::iterator  Model_Itr;
typedef Model_Array::pointer   Model_Ptr;

#endif