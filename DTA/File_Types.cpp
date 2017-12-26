//********************************************************* 
//	Link_File.cpp - Link File Input/Output
//*********************************************************

#include "File_Types.hpp"

//-----------------------------------------------------------
//	Link_File constructors
//-----------------------------------------------------------

Link_File::Link_File (void) : Db_Header ()
{
	File_Type ("Link File");
	File_ID ("Link");

	link = node_a = node_b = length = type = speed = lanes = cap = alpha = beta = time = toll = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_File::Create_Fields (void) 
{
	Clear_Fields ();

	Add_Field ("A", DB_INTEGER, 10);
	Add_Field ("B", DB_INTEGER, 10);
	Add_Field ("FTYPE", DB_INTEGER, 2);
	Add_Field ("DIST", DB_DOUBLE, 8.2);
	Add_Field ("LANES", DB_INTEGER, 8);
	Add_Field ("CAP", DB_INTEGER, 8);
	Add_Field ("SPD", DB_DOUBLE, 8.2);
	Add_Field ("TOLL", DB_DOUBLE, 8.2);
	Add_Field ("ALPHA", DB_DOUBLE, 8.2);
	Add_Field ("BETA", DB_DOUBLE, 8.2);
	Add_Field ("TIME", DB_DOUBLE, 8.2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node_a = Required_Field (ANODE_FIELD_NAMES);
	node_b = Required_Field (BNODE_FIELD_NAMES);
	type = Required_Field (FTYPE_FIELD_NAMES);
	length = Required_Field (LENGTH_FIELD_NAMES);
	speed = Required_Field (SPD_FIELD_NAMES);
	cap = Required_Field (CAP_FIELD_NAMES);

	if (node_a < 0 || node_b < 0 || length < 0 || type < 0 || speed < 0 || cap < 0) return (false);

	//---- optional fields ----

	link = Optional_Field ("LINK", "ID");
	lanes = Optional_Field (LANES_FIELD_NAMES);
	toll = Optional_Field (TOLL_FIELD_NAMES);
	alpha = Optional_Field ("ALPHA", "BPR_A");
	beta = Optional_Field ("BETA", "BPR_B");
	time = Optional_Field ("TIME", "TIME0", "TIM");
	
	return (true);
}

//-----------------------------------------------------------
//	Node_File constructors
//-----------------------------------------------------------

Node_File::Node_File (void) : Db_Header ()
{
	File_Type ("Node File");
	File_ID ("Node");

	node = type = cost = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Node_File::Create_Fields (void)
{
	Clear_Fields ();

	Add_Field ("NODE", DB_INTEGER, 10);
	Add_Field ("TYPE", DB_INTEGER, 10);
	Add_Field ("VCOST", DB_DOUBLE, 8.2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Node_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node = Required_Field (NODE_FIELD_NAMES);
	type = Required_Field ("TYPE", "DTA_TYPE", "FLAG", "DECISION");

	if (node < 0 || type < 0) return (false);

	//---- optional fields ----

	cost = Optional_Field ("VCOST", "VOC", "VALUE", "COST_VALUE", "VOT");

	return (true);
}

//-----------------------------------------------------------
//	Trip_File constructors
//-----------------------------------------------------------

Trip_File::Trip_File (void) : Db_Header ()
{
	File_Type ("Trip File");
	File_ID ("Trip");

	org = des = period = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Trip_File::Create_Fields (void)
{
	Clear_Fields ();

	Add_Field ("ORG", DB_INTEGER, 10);
	Add_Field ("DES", DB_INTEGER, 10);
	Add_Field ("PERIOD", DB_INTEGER, 3);

	for (Str_Itr itr = mode_names.begin (); itr != mode_names.end (); itr++) {
		Add_Field (*itr, DB_DOUBLE, 8.2);
	}

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Trip_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	org = Required_Field (ORIGIN_FIELD_NAMES);
	des = Required_Field (DESTINATION_FIELD_NAMES);
	period = Required_Field ("PERIOD", "TP", "TIME");

	if (org < 0 || des < 0 || period < 0) return (false);

	int i, num;
	Field_Ptr ptr;

	num = Num_Fields ();

	for (i = 0; i < num; i++) {
		if (i == org || i == des || i == period) continue;

		ptr = Field (i);
		mode_names.push_back (ptr->Name ());
		mode.push_back (i);
	}
	if (mode.size () == 0) return (false);

	//---- optional fields ----

	return (true);
}

//-----------------------------------------------------------
//	Toll_File constructors
//-----------------------------------------------------------

Toll_File::Toll_File (void) : Db_Header ()
{
	File_Type ("Toll File");
	File_ID ("Toll");

	node_a = node_b = segment = policy = toll = length = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Toll_File::Create_Fields (void)
{
	Clear_Fields ();

	Add_Field ("A", DB_INTEGER, 10);
	Add_Field ("B", DB_INTEGER, 10);
	Add_Field ("SEGMENT", DB_INTEGER, 4);
	Add_Field ("POLICY", DB_INTEGER, 4);
	Add_Field ("TOLL", DB_DOUBLE, 8.2);
	Add_Field ("LENGTH", DB_DOUBLE, 8.2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Toll_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node_a = Required_Field (ANODE_FIELD_NAMES);
	node_b = Required_Field (BNODE_FIELD_NAMES);

	segment = Required_Field ("SEGMENT", "TOLLSEGNUM");
	policy = Required_Field ("POLICY", "TOLL_POLICY");

	toll = Required_Field (TOLL_FIELD_NAMES);
	length = Required_Field ("LENGTH", "DISTANCE", "DIST", "SEG_DISTANCE");

	if (node_a < 0 || node_b < 0 || segment < 0 || policy < 0 || toll < 0 || length < 0) return (false);

	//---- optional fields ----

	return (true);
}

//-----------------------------------------------------------
//	Volume_File constructors
//-----------------------------------------------------------

Volume_File::Volume_File (void) : Db_Header ()
{
	File_Type ("Volume File");
	File_ID ("Volume");

	node_a = node_b = period = speed = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Volume_File::Create_Fields (void)
{
	Str_Itr itr;

	Clear_Fields ();

	Add_Field ("A", DB_INTEGER, 10);
	Add_Field ("B", DB_INTEGER, 10);
	Add_Field ("PERIOD", DB_INTEGER, 4);

	for (itr = _modes.begin (); itr != _modes.end (); itr++) {
		Add_Field (*itr, DB_DOUBLE, 10.2);
	}
	Add_Field ("SPEED", DB_DOUBLE, 10.2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Volume_File::Set_Field_Numbers (void)
{
	Str_Itr itr;

	//---- required fields ----

	node_a = Required_Field (ANODE_FIELD_NAMES);
	node_b = Required_Field (BNODE_FIELD_NAMES);
	period = Required_Field ("PERIOD", "TP", "TIME");

	if (node_a < 0 || node_b < 0 || period < 0) return (false);

	for (itr = _modes.begin (); itr != _modes.end (); itr++) {
		volume.push_back (Required_Field (*itr));
	}
	speed = Optional_Field ("SPEED");

	//---- optional fields ----

	return (true);
}

//-----------------------------------------------------------
//	Path_Leg_File constructors
//-----------------------------------------------------------

Path_Leg_File::Path_Leg_File (void) : Db_Header ()
{
	File_Type ("Path Leg File");
	File_ID ("Path");

	org = des = period = iteration = mode = node_a = node_b = length = time = cost = trips = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Path_Leg_File::Create_Fields (void)
{
	Clear_Fields ();

	Add_Field ("ORG", DB_INTEGER, 10);
	Add_Field ("DES", DB_INTEGER, 10);
	Add_Field ("PERIOD", DB_INTEGER, 3);
	Add_Field ("ITERATION", DB_INTEGER, 3);
	Add_Field ("MODE", DB_STRING, 8);
	Add_Field ("A", DB_INTEGER, 10);
	Add_Field ("B", DB_INTEGER, 10);
	Add_Field ("TIME", DB_DOUBLE, 8.2);
	Add_Field ("COST", DB_DOUBLE, 8.2);
	Add_Field ("LENGTH", DB_DOUBLE, 8.2);
	Add_Field ("TRIPS", DB_DOUBLE, 8.3);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Path_Leg_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	org = Required_Field (ORIGIN_FIELD_NAMES);
	des = Required_Field (DESTINATION_FIELD_NAMES);
	period = Required_Field ("PERIOD", "TP", "TOD"); 
	node_a = Required_Field (ANODE_FIELD_NAMES);
	node_b = Required_Field (BNODE_FIELD_NAMES);

	if (org < 0 || des < 0 || period < 0 || node_a < 0 || node_b < 0) return (false);

	iteration = Optional_Field ("ITERATION", "ITER", "NUMBER");
	mode = Optional_Field ("MODE", "TYPE");
	time = Optional_Field ("TIME", "TIME0", "TIM");
	cost = Optional_Field ("COST");
	length = Optional_Field (LENGTH_FIELD_NAMES);
	trips = Optional_Field ("TRIPS", "VOLUME", "VOL");

	return (true);
}

//-----------------------------------------------------------
//	Gap_File constructors
//-----------------------------------------------------------

Gap_File::Gap_File (void) : Db_Header ()
{
	File_Type ("Gap File");
	File_ID ("Gap");

	iteration = gap = std_dev = maximum = rmse = diff = total = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Gap_File::Create_Fields (void)
{
	Clear_Fields ();

	Add_Field ("ITERATION", DB_INTEGER, 6);
	Add_Field ("GAP", DB_DOUBLE, 13.6);
	Add_Field ("STD_DEV", DB_DOUBLE, 13.6);
	Add_Field ("MAXIMUM", DB_DOUBLE, 13.6);
	Add_Field ("RMSE", DB_DOUBLE, 9.1);
	Add_Field ("DIFFERENCE", DB_DOUBLE, 13.0);
	Add_Field ("TOTAL", DB_DOUBLE, 13.0);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Gap_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	iteration = Required_Field ("ITERATION", "ID");
	gap = Required_Field ("GAP", "ERROR");

	if (iteration < 0 || gap < 0) return (false);

	//---- optional fields ----

	std_dev = Optional_Field ("STD_DEV", "STDDEV");
	maximum = Optional_Field ("MAXIMUM", "MAX");
	rmse = Optional_Field ("RMSE");
	diff = Optional_Field ("DIFFERENCE", "DIFF", "HOURS");
	total = Optional_Field ("TOTAL", "TOT", "IMPED");

	return (true);
}

