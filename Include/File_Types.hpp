//******************************************************** 
//	File_Types.hpp - DTA File Input/Output
//********************************************************

#ifndef FILE_TYPES_HPP
#define FILE_TYPES_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Link_File Class definition
//---------------------------------------------------------

class Link_File : public Db_Header
{
public:
	Link_File (void);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Node_A (void)              { return (Get_Integer (node_a)); }
	int    Node_B (void)              { return (Get_Integer (node_b)); }
	double Length (void)              { return (Get_Double (length)); }
	int    Type (void)                { return (Get_Integer (type)); }
	double Speed (void)               { return (Get_Double (speed)); }
	int    Lanes (void)               { return (Get_Integer (lanes)); }
	int    Cap (void)                 { return (Get_Integer (cap)); }
	double Toll (void)                { return (Get_Double (toll)); }
	double Alpha (void)               { return (Get_Double (alpha)); }
	double Beta (void)                { return (Get_Double (beta)); }
	double Time (void)                { return (Get_Double (time)); }


	void   Link (int value)           { Put_Field (link, value); }
	void   Node_A (int value)         { Put_Field (node_a, value); }
	void   Node_B (int value)         { Put_Field (node_b, value); }
	void   Length (double value)      { Put_Field (length, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Speed (double value)       { Put_Field (speed, value); }
	void   Lanes (int value)          { Put_Field (lanes, value); }
	void   Cap (int value)            { Put_Field (cap, value); }
	void   Alpha (double value)       { Put_Field (alpha, value); }
	void   Beta (double value)        { Put_Field (beta, value); }
	void   Time (double value)        { Put_Field (time, value); }
	void   Toll (double value)        { Put_Field (toll, value); }

	virtual bool Create_Fields (void);

	bool   Lanes_Flag (void)          { return (lanes >= 0); }
	bool   Time_Flag (void)           { return (time >= 0); }
	bool   Speed_Flag (void)          { return (speed >= 0); }
	bool   Toll_Flag (void)           { return (toll >= 0); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	int link, node_a, node_b, length, type, speed, lanes, cap, alpha, beta, time, toll;
};

//---------------------------------------------------------
//	Node_File Class definition
//---------------------------------------------------------

class Node_File : public Db_Header
{
public:
	Node_File (void);

	int    Node (void)             { return (Get_Integer (node)); }
	int    Type (void)             { return (Get_Integer (type)); }
	double VCost (void)            { return (Get_Double (cost)); }

	void   Node (int value)        { Put_Field (node, value); }
	void   Type (int value)        { Put_Field (type, value); }
	void   VCost (double value)    { Put_Field (cost, value); }

	bool   Cost_Flag (void)        { return (cost >= 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	int node, type, cost;
};

//---------------------------------------------------------
//	Trip_File Class definition
//---------------------------------------------------------

class Trip_File : public Db_Header
{
public:
	Trip_File (void);

	int    Origin (void)                 { return (Get_Integer (org)); }
	int    Destination (void)            { return (Get_Integer (des)); }
	int    Period (void)                 { return (Get_Integer (period)); }
	double Mode (int num)                { return (Get_Double (mode [num])); }
	string Mode_Name (int num)           { return (mode_names [num]); }

	int    Num_Modes (void)              { return ((int) mode.size ()); }

	void   Origin (int value)            { Put_Field (org, value); }
	void   Destination (int value)       { Put_Field (des, value); }
	void   Period (int value)            { Put_Field (period, value); }
	void   Mode (int num, double value)  { Put_Field (mode [num], value); }

	Strings Mode_Names (void)            { return (mode_names); }
	void    Mode_Names (Strings names)   { mode_names = names; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	int org, des, period;
	Integers mode;
	Strings mode_names;
};

//---------------------------------------------------------
//	Toll_File Class definition
//---------------------------------------------------------

class Toll_File : public Db_Header
{
public:
	Toll_File (void);

	int    Node_A (void)            { return (Get_Integer (node_a)); }
	int    Node_B (void)            { return (Get_Integer (node_b)); }
	int    Segment (void)           { return (Get_Integer (segment)); }
	int    Policy (void)            { return (Get_Integer (policy)); }
	double Toll (void)              { return (Get_Double (toll)); }
	double Length (void)            { return (Get_Double (length)); }

	void   Node_A (int value)       { Put_Field (node_a, value); }
	void   Node_B (int value)       { Put_Field (node_b, value); }
	void   Segment (int value)      { Put_Field (segment, value); }
	void   Policy (int value)       { Put_Field (policy, value); }
	void   Toll (double value)      { Put_Field (toll, value); }
	void   Length (double value)    { Put_Field (length, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	int node_a, node_b, segment, policy, toll, length;
};

//---------------------------------------------------------
//	Volume_File Class definition
//---------------------------------------------------------

class Volume_File : public Db_Header
{
public:
	Volume_File (void);

	int    Node_A (void)                     { return (Get_Integer (node_a)); }
	int    Node_B (void)                     { return (Get_Integer (node_b)); }
	int    Period (void)                     { return (Get_Integer (period)); }
	double Volume (int mode)                 { return (Get_Double (volume [mode])); }
	double Speed (void)                      { return (Get_Double (speed)); }
	double Toll (void)                       { return (Get_Double (toll)); }
	int    Modes (void)                      { return ((int) volume.size ()); }

	void   Node_A (int value)                { Put_Field (node_a, value); }
	void   Node_B (int value)                { Put_Field (node_b, value); }
	void   Period (int value)                { Put_Field (period, value); }
	void   Volume (int mode, double value)   { Put_Field (volume [mode], value); }
	void   Speed (double value)              { Put_Field (speed, value); }
	void   Toll (double value)               { Put_Field (toll, value); }
	void   Modes (Strings modes)             { _modes = modes; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	int node_a, node_b, period, speed, toll;
	Integers volume;
	Strings _modes;
};

//---------------------------------------------------------
//	Path_Leg_File Class definition
//---------------------------------------------------------

class Path_Leg_File : public Db_Header
{
public:
	Path_Leg_File (void);
	
	int    Origin (void)           { return (Get_Integer (org)); }
	int    Destination (void)      { return (Get_Integer (des)); }
	int    Period (void)           { return (Get_Integer (period)); }
	int    Iteration (void)        { return (Get_Integer (iteration)); }
	string Mode (void)             { return (Get_String (mode)); }
	int    Node_A (void)           { return (Get_Integer (node_a)); }
	int    Node_B (void)           { return (Get_Integer (node_b)); }
	double Time (void)             { return (Get_Double (time)); }
	double Cost (void)             { return (Get_Double (cost)); }
	double Length (void)           { return (Get_Double (length)); }
	double Trips (void)            { return (Get_Double (trips)); }
	
	void   Origin (int value)      { Put_Field (org, value); }
	void   Destination (int value) { Put_Field (des, value); }
	void   Period (int value)      { Put_Field (period, value); }
	void   Iteration (int value)   { Put_Field (iteration, value); }
	void   Mode (string value)     { Put_Field (mode, value); }
	void   Node_A (int value)      { Put_Field (node_a, value); }
	void   Node_B (int value)      { Put_Field (node_b, value); }
	void   Time (double value)     { Put_Field (time, value); }
	void   Cost (double value)     { Put_Field (cost, value); }
	void   Length (double value)   { Put_Field (length, value); }
	void   Trips (double value)    { Put_Field (trips, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	int org, des, period, iteration, mode, node_a, node_b, time, cost, length, trips;
};

//---------------------------------------------------------
//	Gap_File Class definition
//---------------------------------------------------------

class Gap_File : public Db_Header
{
public:
	Gap_File (void);

	int    Iteration (void)           { return (Get_Integer (iteration)); }
	int    Period (void)              { return (Get_Integer (period)); }
	double Gap (void)                 { return (Get_Double (gap)); }
	double Std_Dev (void)             { return (Get_Double (std_dev)); }
	double Maximum (void)             { return (Get_Double (maximum)); }
	double RMSE (void)                { return (Get_Double (rmse)); }
	double Difference (void)          { return (Get_Double (diff)); }
	double Total (void)               { return (Get_Double (total)); }

	void   Iteration (int value)      { Put_Field (iteration, value); }
	void   Period (int value)         { Put_Field (period, value); }
	void   Gap (double value)         { Put_Field (gap, value); }
	void   Std_Dev (double value)     { Put_Field (std_dev, value); }
	void   Maximum (double value)     { Put_Field (maximum, value); }
	void   RMSE (double value)        { Put_Field (rmse, value); }
	void   Difference (double value)  { Put_Field (diff, value); }
	void   Total (double value)       { Put_Field (total, value); }

	bool   Period_Flag (void)         { return (period_flag); }
	void   Period_Flag (bool flag)    { period_flag = flag;  }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	int iteration, gap, std_dev, maximum, rmse, diff, total, period;
	bool period_flag;
};

//---------------------------------------------------------
//	Model_Data_File Class definition
//---------------------------------------------------------

class Model_Data_File : public Db_Header
{
public:
	Model_Data_File (void);

	int    Iteration (void)          { return (Get_Integer (iteration)); }
	int    Origin (void)             { return (Get_Integer (origin)); }
	int    Destination (void)        { return (Get_Integer (destination)); }
	int    Start (void)              { return (Get_Integer (start)); }
	int    Period (void)             { return (Get_Integer (period)); }
	int    From_Node (void)          { return (Get_Integer (from_node)); }
	int    To_Node (void)            { return (Get_Integer (to_node)); }
	double Distance1 (void)          { return (Get_Double (distance1)); }
	double Distance2 (void)          { return (Get_Double (distance2)); }
	double Time1 (void)              { return (Get_Double (time1)); }
	double Time2 (void)              { return (Get_Double (time2)); }
	double FFTime1 (void)            { return (Get_Double (fftime1)); }
	double FFTime2 (void)            { return (Get_Double (fftime2)); }
	double Toll1 (void)              { return (Get_Double (toll1)); }
	double Toll2 (void)              { return (Get_Double (toll2)); }
	double Utility (void)            { return (Get_Double (utility)); }
	double Share1 (void)             { return (Get_Double (share1)); }

	void   Iteration (int value)     { Put_Field (iteration, value); }
	void   Origin (int value)        { Put_Field (origin, value); }
	void   Destination (int value)   { Put_Field (destination, value); }
	void   Start (int value)         { Put_Field (start, value); }
	void   Period (int value)        { Put_Field (period, value); }
	void   From_Node (int value)     { Put_Field (from_node, value); }
	void   To_Node (int value)       { Put_Field (to_node, value); }
	void   Distance1 (double value)  { Put_Field (distance1, value); }
	void   Distance2 (double value)  { Put_Field (distance2, value); }
	void   Time1 (double value)      { Put_Field (time1, value); }
	void   Time2 (double value)      { Put_Field (time2, value); }
	void   FFTime1 (double value)    { Put_Field (fftime1, value); }
	void   FFTime2 (double value)    { Put_Field (fftime2, value); }
	void   Toll1 (double value)      { Put_Field (toll1, value); }
	void   Toll2 (double value)      { Put_Field (toll2, value); }
	void   Utility (double value)    { Put_Field (utility, value); }
	void   Share1 (double value)     { Put_Field (share1, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	int iteration, origin, destination, start, period, from_node, to_node;
	int distance1, distance2, time1, time2, fftime1, fftime2, toll1, toll2, utility, share1;
};

#endif
