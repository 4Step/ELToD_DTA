//*********************************************************
//	Control_Service.hpp - array of control keys
//*********************************************************

#ifndef CONTROL_SERVICE_HPP
#define CONTROL_SERVICE_HPP

#include "Report_Service.hpp"
#include "TypeDefs.hpp"

#include <stdio.h>

//---------------------------------------------------------
//	Control_Key -- key specification structure and array
//---------------------------------------------------------

struct Control_Key {
	int         code;
	String      key;
	Level_Type  levels;
	Option_Type option;
	Key_Type    type;
	String      value;
	String      range;
	Help_Type   help;
};

typedef vector <Control_Key>    Control_Keys;
typedef Control_Keys::iterator  Control_Key_Itr;

//---------------------------------------------------------
//	Key_Value -- key value structure and array
//---------------------------------------------------------

typedef struct key_value {
	String                value;
	map <int, key_value>  nest;
} Key_Value;

typedef map <int, Key_Value>        Key_Values;
typedef pair <int, Key_Value>       Key_Value_Data;
typedef Key_Values::iterator        Key_Value_Itr;
typedef pair <Key_Value_Itr, bool>  Key_Value_Stat;

//---------------------------------------------------------
//	Control_Service -- class definition
//---------------------------------------------------------

class Control_Service : public Report_Service
{
public:
	Control_Service (void);

	void   Add_Control_Key (Control_Key key);

	bool   Read_Control_File (string control_file);
	void   Write_Control_File (void);
	
	void   Process_Control_String (String control);

	String Get_Control_String (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Set_Control_Flag (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 

	String Get_Control_Text (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Get_Control_Flag (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 
	int    Get_Control_Integer (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 
	double Get_Control_Double (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 

	String Get_Default_Text (int code);
	bool   Get_Default_Flag (int code);
	int    Get_Default_Integer (int code);
	double Get_Default_Double (int code);

	String Get_Range_String (int code);
	String Get_Key_Description (int code);

	void   Set_Range_String (int code, String options);
	void   Set_Range_String (int code, Strings options);
	
	int    Integer_Range_Check (int value, String range);	
	double Double_Range_Check (double value, String range);

	bool   Control_Key_Empty (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Check_Control_Key (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	void   Control_Key_Error (int code, string range = "", int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);

	int    Highest_Control_Group (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	int    Number_Control_Group (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);

	bool   Required_Control_Key (int code);
	bool   Required_Control_Key (int code, bool flag);
	bool   Ignore_Control_Key (int code);

	Print_Stream & Output_Control_Label (int code);
	void   Output_Control_Key (int code, string value)  { Output_Control_Label (code) << value; }

	String Current_Label (void);
	String Current_Key (void);

	bool   Control_Key_Status (int code);
	void   Show_Program_Keys (void);
	void   Show_Extra_Keys (void);
	bool   Check_Keys (void);
	void   Document_Controls (FILE *file, bool flag = false);

	bool   Add_Variable (string variable_value);

private:
	int _current_key;
	String _nesting;

	Control_Key * Get_Control_Key (int code);

	Control_Keys  _keys;
	Key_Values    _values;
	Strings       _extra_keys;

	Str_Key_Map   _variable_map;

	void Print_Key (FILE *file, String &key, String &value, String &range);
};

#endif
