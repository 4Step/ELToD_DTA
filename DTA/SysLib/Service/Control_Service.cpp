//*********************************************************
//	Control_Service.cpp - array of control keys
//*********************************************************

#include "Control_Service.hpp"

#include "Db_File.hpp"
#include "String.hpp"

#define MAX_LEVELS		5

//---------------------------------------------------------
//	Control_Service - constructor
//---------------------------------------------------------

Control_Service::Control_Service (void) : Report_Service ()
{
	_current_key = 0;
}

//-----------------------------------------------------------
//	Add_Control_Key - add a key to the group
//-----------------------------------------------------------

void Control_Service::Add_Control_Key (Control_Key key) 
{
	if (key.code == 0) return;

	for (Control_Key_Itr itr = _keys.begin (); itr != _keys.end (); itr++) {
		if (key.key.Equals (itr->key)) {
			*itr = key;
			return;
		}
	}
	_keys.push_back (key);
}

//---------------------------------------------------------
//	Read_Control_File
//---------------------------------------------------------

bool Control_Service::Read_Control_File (string control_file) 
{
	Db_File file;
	bool message_flag = Send_Messages ();
	bool status;

	//---- open the control file ----

	Send_Messages (false);

	status = file.Open (control_file);

	Send_Messages (message_flag);

	if (!status) return (false);

	//---- read the control file ----

	while (file.Read ()) {
		Process_Control_String (file.Record_String ());
	}
	file.Close ();

	return (true);
}

//---------------------------------------------------------
//	Write_Control_File
//---------------------------------------------------------

void Control_Service::Write_Control_File (void) 
{
	String key, value, range;
	bool found;

	FILE *file;
	Control_Key_Itr key_itr;

	key = Control_File ();

	if (key.empty ()) {
		key = Program ();
		key += ".ctl";
	}
	file = f_open (key, "wt");
	if (file == 0) return;

	for (key_itr = _keys.begin (); key_itr != _keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;

		//---- check section breaks ----

		if (key_itr->code == 1) {
			fprintf (file, "\n#---- %s Control Keys ----\n\n", Program ());
		} else if (key_itr->code == Execution_Service::PROGRAM_REPORTS) {
			fprintf (file, "\n#---- Program Report Keys ----\n\n");
		}
		//---- write the key ----

		key = key_itr->key;
		range = key_itr->range;

		//---- check for values ----

		if (key_itr->levels > LEVEL0) {
			found = false;

			if (key_itr->levels == LEVEL1) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					value = Get_Control_String (key_itr->code, i);
					if (value.empty ()) continue;

					key = key_itr->key;
					key += String ("_%d") % i;
					found = true;

					Print_Key (file, key, value, range);
				}
			} else if (key_itr->levels == LEVEL2) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					int num2 = Highest_Control_Group (key_itr->code, i, 0);

					for (int j=1; j <= num2; j++) {
						value = Get_Control_String (key_itr->code, i, j);
						if (value.empty ()) continue;

						key = key_itr->key;
						key += String ("_%d_%d") % i % j;
						found = true;

						Print_Key (file, key, value, range);
					}
				}
			} else if (key_itr->levels == LEVEL3) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					int num2 = Highest_Control_Group (key_itr->code, i, 0);

					for (int j=1; j <= num2; j++) {
						int num3 = Highest_Control_Group (key_itr->code, i, j, 0);

						for (int k=1; k <= num3; k++) {
							value = Get_Control_String (key_itr->code, i, j, k);
							if (value.empty ()) continue;

							key = key_itr->key;
							key += String ("_%d_%d_%d") % i % j % k;
							found = true;

							Print_Key (file, key, value, range);
						}
					}
				}
			} else if (key_itr->levels == LEVEL4) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					int num2 = Highest_Control_Group (key_itr->code, i, 0);

					for (int j=1; j <= num2; j++) {
						int num3 = Highest_Control_Group (key_itr->code, i, j, 0);
					
						for (int k=1; k <= num3; k++) {
							int num4 = Highest_Control_Group (key_itr->code, i, j, k, 0);

							for (int l=1; l <= num4; l++) {
								value = Get_Control_String (key_itr->code, i, j, k, l);
								if (value.empty ()) continue;

								key = key_itr->key;
								key += String ("_%d_%d_%d_%d") % i % j % k % l;
								found = true;

								Print_Key (file, key, value, range);
							}
						}
					}
				}
			}
			if (!found) {
				for (int i=1; i <= key_itr->levels; i++) {
					key += "_1";
				}
				value = key_itr->value;

				Print_Key (file, key, value, range);

				if (key_itr->code == Execution_Service::PROGRAM_REPORTS) {
					for (report_name_itr = _report_name.begin (); report_name_itr != _report_name.end (); report_name_itr++) {
						fprintf (file, "%64s//---- %s\n", "", report_name_itr->name.c_str ());
					}
				}
			}
		} else {
			value = Get_Control_String (key_itr->code);
			if (value.empty ()) {
				if (key_itr->key.Equals ("TITLE")) {
					value = Program ();
					value += " Default Control Keys";
				} else {
					value = key_itr->value;
				}
			}
			Print_Key (file, key, value, range);
		}
	}
	fclose (file);
	Show_Message (1, "Control File was Updated");
	Show_Message (1);
}

//---------------------------------------------------------
//	Print_Key
//---------------------------------------------------------

void Control_Service::Print_Key (FILE *file, String &key, String &value, String &range)
{
	if (range.empty ()) {
		fprintf (file, "%-40s%s\n", key.c_str (), value.c_str ());
	} else if (value.size () > 20) {
		if (range.Equals (FILE_RANGE)) {
			fprintf (file, "%-40s%s\n", key.c_str (), value.c_str ());
		} else {
			fprintf (file, "%-40s%s\t//---- %s\n", key.c_str (), value.c_str (), range.c_str ());
		}
	} else {
		fprintf (file, "%-40s%-24s//---- %s\n", key.c_str (), value.c_str (), range.c_str ());
	}
}

//---------------------------------------------------------
//	Process_Control_String
//---------------------------------------------------------

void Control_Service::Process_Control_String (String control) 
{
	int i, code;
	bool found;
	char *env_var;
	String key, nest, buffer, variable, name;
	string::iterator itr;
	Control_Key_Itr key_itr;
	Key_Value value_data;
	Key_Value_Itr value_itr;
	Key_Value_Stat value_stat;
	Str_Key_Map_Itr map_itr;
	Str_Key_Map_Stat map_stat;

	//---- remove comments ----

	if (control.empty ()) return;

	control.Trim (" \t");

	//---- get the key and key value ----

	size_t index = control.find_first_of (" \t=");
	if (index == string::npos) return;

	key = control.substr (0, index);

	if (key [0] == '#') return;

	control.erase (0, index);
	control.Trim_Left (" \t=");

	if (control.Equals ("NULL")) control.clear ();

	//---- check for a variable value ----

	found = false;
	buffer.clear();
	variable.clear();

	for (itr = control.begin(); itr != control.end(); itr++) {
		if (*itr == '@' || *itr == '%') {
			variable.push_back(*itr);
			if (found) {
				found = false;
				map_itr = _variable_map.find(variable);

				if (map_itr != _variable_map.end()) {
					buffer += map_itr->second;
				} else {
					name = variable.substr (1, variable.length () - 2);
					env_var = get_env (name.c_str ());

					if (env_var != 0) {
						name = env_var;
						_variable_map.insert (Str_Key_Map_Data (variable, name));
						buffer += name;
					}
				}
				variable.clear();
			} else {
				found = true;
			}
		} else if (found) {
			variable.push_back(*itr);
		} else {
			buffer.push_back(*itr);
		}
	}
	if (found) {
		buffer += variable;
	}
	control = buffer;

	found = false;
	buffer.clear();
	variable.clear();

	for (itr = key.begin(); itr != key.end(); itr++) {
		if (*itr == '@' || *itr == '%') {
			variable.push_back(*itr);
			if (found) {
				found = false;
				if (variable.size() == key.size()) {
					map_stat = _variable_map.insert(Str_Key_Map_Data(variable, control));
					if (!map_stat.second) {
						map_stat.first->second = control;
					}
					return;
				} else {
					map_itr = _variable_map.find(variable);

					if (map_itr != _variable_map.end()) {
						buffer += map_itr->second;
					} else {
						name = variable.substr (1, variable.length () - 2);
						env_var = get_env (name.c_str ());

						if (env_var != 0) {
							name = env_var;
							_variable_map.insert (Str_Key_Map_Data (variable, name));
							buffer += name;
						}
					}
				}
				variable.clear();
			} else {
				found = true;
			}
		} else if (found) {
			variable.push_back(*itr);
		} else {
			buffer.push_back(*itr);
		}
	}
	if (found) {
		buffer == variable;
	}
	key = buffer;
	if (key [0] == '#') return;

	//---- check for a control file ----

	if (key.Equals("CONTROL_KEY_FILE")) {
		if (!Read_Control_File (control)) {
			Error (String ("Control Key File %s was Not Found") % control);
		}
		return;
	}

	//---- find the key ----

	found = false;

	for (key_itr = _keys.begin (); key_itr != _keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) {
			if ((key_itr->levels == 0 && !key.Equals (key_itr->key)) ||
				!key.Starts_With (key_itr->key)) continue;

			found = true;
			break;
		}

		//---- match a standard key ----

		if (key_itr->levels == 0) {
			if (key.Equals (key_itr->key)) {
				found = true;

				value_stat = _values.insert (Key_Value_Data (key_itr->code, value_data));

				value_stat.first->second.value = control;
				break;
			}

		} else if (key.Starts_With (key_itr->key)) {

			//---- match a nested key ----

			nest = key.substr (key_itr->key.length ());

			if (nest.empty ()) {
				if (key_itr->levels > 1) break;
				nest = "_1";
			} else if (nest [1] < '0' || nest [1] > '9') continue;

			Strings levels;
			nest.Parse (levels, "_");

			if ((int) levels.size () != key_itr->levels + 1) break;

			found = true;

			value_stat = _values.insert (Key_Value_Data (key_itr->code, value_data));

			Key_Values *values_ptr = &(value_stat.first->second.nest);

			for (i=1; i <= key_itr->levels; i++) {
				code = levels [i].Integer ();

				value_stat = values_ptr->insert (Key_Value_Data (code, value_data));

				if (i == key_itr->levels) {
					value_stat.first->second.value = control;
				} else {
					values_ptr = &(value_stat.first->second.nest);
				}
			}
		}
	}
	
	//---- add the key to the list of unrecognized keys ----

	if (!found && !control.empty ()) {
		Str_Itr str_itr;
		for (str_itr = _extra_keys.begin (); str_itr != _extra_keys.end (); str_itr++) {
			if (key.Equals (*str_itr)) break;
		}
		if (str_itr == _extra_keys.end ()) {
			_extra_keys.push_back (key);
		}
	}
}

//-----------------------------------------------------------
//	Get_Control_String - search for a specified record 
//-----------------------------------------------------------

String Control_Service::Get_Control_String (int code, int nest1, int nest2, int nest3, int nest4)
{
	int i, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Values *values_ptr;

	if (code == 0) return ("");

	codes [0] = _current_key = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;
	_nesting.clear ();

	values_ptr = &(_values);

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) break;

		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) break;

		if (i) _nesting += String (" #%d") % code;

		if (itr->second.nest.size () == 0) {
			if (i == 1 && code == 1 && Highest_Control_Group (_current_key, 0) == 1) {
				_nesting.clear ();
			}
			return (itr->second.value);
		} else {
			values_ptr = &(itr->second.nest);
		}
	}
	return ("");
}

//-----------------------------------------------------------
//	Set_Control_Flag - search for a flag parameter 
//-----------------------------------------------------------
	
bool Control_Service::Set_Control_Flag (int code, int nest1, int nest2, int nest3, int nest4)
{
	String key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		Control_Key *key_ptr = Get_Control_Key (code);
		if (key_ptr == 0 || key_ptr->value.empty ()) return (false);
		key = key_ptr->value;
	}
	return (key.Bool ());
}

//-----------------------------------------------------------
//	Get_Control_Text - search for a string parameter 
//-----------------------------------------------------------
	
String Control_Service::Get_Control_Text (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return ("");

	bool flag;
	String key, default_value;

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return ("");

		key = key_ptr->value;
		flag = false;
	} else {
		flag = true;
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + _nesting;
		Print (1, default_value.To_Title ()) << " = " << key;
	}
	return (key);
}

//-----------------------------------------------------------
//	Get_Control_Flag - search for a flag parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_Flag (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);

	bool flag, value_flag;
	
	String key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (false);

		key = key_ptr->value;
		value_flag = true;
	} else {
		value_flag = false;
	}
	flag = key.Bool ();

	//---- print the key message ----

	if (!value_flag) {
		key = key_ptr->key + _nesting;
		Print (1, key.To_Title ()) << ((flag) ? " = TRUE" : " = FALSE");
	}
	return (flag);
}

//-----------------------------------------------------------
//	Get_Control_Integer - search for a integer parameter 
//-----------------------------------------------------------
	
int Control_Service::Get_Control_Integer (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	int value;
	bool flag;
	String key, default_value;

	default_value = key_ptr->value;

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0);
		flag = false;
		key = default_value;
	} else {
		flag = true;
	}
	value = key.Integer ();

	//---- create the external key value ----

	key ("%d") % value;

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + _nesting;
		Print (1, default_value.To_Title ()) << " = " << key;

		return (Integer_Range_Check (value, key_ptr->range));
	}
	return (value);
}

//-----------------------------------------------------------
//	Get_Control_Double - search for a double parameter 
//-----------------------------------------------------------
	
double Control_Service::Get_Control_Double (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	double value;
	bool flag;
	String key, default_value;

	default_value = key_ptr->value;

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0.0);
		flag = false;
		key = default_value;
	} else {
		flag = true;
	}
	value = key.Double ();

	//---- create the external key value ----

	key ("%g") % value;

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + _nesting;
		Print (1, default_value.To_Title ()) << " = " << key;

		return (Double_Range_Check (value, key_ptr->range));
	}
	return (value);
}

//-----------------------------------------------------------
//	Get_Default_Text - get the default text parameter 
//-----------------------------------------------------------
	
String Control_Service::Get_Default_Text (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return ("");

	return (key_ptr->value);
}

//-----------------------------------------------------------
//	Get_Default_Flag - get the default flag parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Default_Flag (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);

	return (key_ptr->value.Bool ());
}

//-----------------------------------------------------------
//	Get_Default_Integer - get the default integer parameter 
//-----------------------------------------------------------
	
int Control_Service::Get_Default_Integer (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	String key = key_ptr->value;

	return (key.Integer ());
}

//-----------------------------------------------------------
//	Get_Default_Double - get the default double parameter 
//-----------------------------------------------------------
	
double Control_Service::Get_Default_Double (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	String key = key_ptr->value;

	return (key.Double ());
}

//-----------------------------------------------------------
//	Get_Range_String - get the range string parameter 
//-----------------------------------------------------------
	
String Control_Service::Get_Range_String (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return ("");

	return (key_ptr->range);
}

//-----------------------------------------------------------
//	Set_Range_String - set the range string parameter 
//-----------------------------------------------------------
	
void Control_Service::Set_Range_String (int code, String options)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return;

	key_ptr->range = options;
}
	
void Control_Service::Set_Range_String (int code, Strings options)
{
	Str_Itr itr;
	String range;

	for (itr = options.begin (); itr != options.end (); itr++) {
		if (itr != options.begin ()) {
			range += ", ";
		}
		range += *itr;
	}
	Set_Range_String (code, range);
}

//-----------------------------------------------------------
//	Get_Key_Description - get the key description string 
//-----------------------------------------------------------
	
String Control_Service::Get_Key_Description (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return ("");

	String label = key_ptr->key;
	label.To_Title ();
	label += " = ";
	label += key_ptr->range;

	return (label);
}


//-----------------------------------------------------------
//	Integer_Range_Check - check/write control key range error
//-----------------------------------------------------------
	
int Control_Service::Integer_Range_Check (int value, String range)
{
	if (range.empty ()) return (value);

	int low, high;
	double d1, d2;
	Strings ranges;
	Str_Itr itr;
	String number;

	//---- unpack the range string ----	

	range.Parse (ranges, COMMA_DELIMITERS);

	for (itr = ranges.begin (); itr != ranges.end (); itr++) {
		if (itr->Starts_With (">=")) {
			number = itr->substr (2);
			low = DTOI (number.Double ());
			if (value >= low) return (value);
		} else if (itr->Starts_With ("<=")) {
			number = itr->substr (2);
			high = DTOI (number.Double ());
			if (value <= high) return (value);
		} else if (itr->Starts_With (">")) {
			number = itr->substr (1);
			low = DTOI (number.Double ());
			if (value > low) return (value);
		} else if (itr->Starts_With ("<")) {
			number = itr->substr (1);
			high = DTOI (number.Double ());
			if (value < high) return (value);
		} else {
			if (!itr->Range (d1, d2)) continue;

			low = DTOI (d1);
			high = DTOI (d2);

			if (value >= low && value <= high) return (value);
		}
	}

	//---- write the error message ----

	Control_Key *key_ptr = Get_Control_Key (_current_key);
	if (key_ptr == 0) return (value);

	int i;
	String message, key;
	
	key = key_ptr->key;
	key.To_Title ();
	key += " = ";

	//---- create the external key value ----

	message ("%d") % value;

	message += " is Out of Range (";

	//---- external range values ----

	for (i=0, itr = ranges.begin (); itr != ranges.end (); itr++, i++) {
		if (i > 0) message += ", ";

		if (itr->Starts_With (">=")) {
			number = itr->substr (2);
			message += ">= ";
		} else if (itr->Starts_With (">")) {
			number = itr->substr (1);
			message += "> ";
		} else {
			number = *itr;
		}
		if (!number.Range (d1, d2)) continue;

		low = DTOI (d1);
		high = DTOI (d2);

		if (low != high) message += String ("%d..") % low;
		message += String ("%d") % high;
	}
	Error (key + message + ")");

	return (value);
}

//-----------------------------------------------------------
//	Double_Range_Check - check/write control key range error
//-----------------------------------------------------------
	
double Control_Service::Double_Range_Check (double value, String range)
{
	if (range.empty ()) return (value);

	double low, high;
	Strings ranges;
	Str_Itr itr;
	String number;

	//---- unpack the range string ----	

	range.Parse (ranges, COMMA_DELIMITERS);

	for (itr = ranges.begin (); itr != ranges.end (); itr++) {
		if (itr->Starts_With (">=")) {
			number = itr->substr (2);
			low = number.Double ();
			if (value >= low) return (value);
		} else if (itr->Starts_With ("<=")) {
			number = itr->substr (2);
			high = number.Double ();
			if (value <= high) return (value);
		} else if (itr->Starts_With (">")) {
			number = itr->substr (1);
			low = number.Double ();
			if (value > low) return (value);
		} else if (itr->Starts_With ("<")) {
			number = itr->substr (1);
			high = number.Double ();
			if (value < high) return (value);
		} else {
			if (!itr->Range (low, high)) continue;

			if (value >= low && value <= high) return (value);
		}
	}

	//---- write the error message ----

	Control_Key *key_ptr = Get_Control_Key (_current_key);
	if (key_ptr == 0) return (value);

	int i;
	String message, key;
	
	key = key_ptr->key;
	key.To_Title ();
	key += " = ";

	//---- create the external key value ----

	message ("%g") % value;

	message += " is Out of Range (";

	//---- external range values ----

	for (i=0, itr = ranges.begin (); itr != ranges.end (); itr++, i++) {
		if (i > 0) message += ", ";

		if (itr->Starts_With (">=")) {
			number = itr->substr (2);
			message += ">= ";
		} else if (itr->Starts_With (">")) {
			number = itr->substr (1);
			message += "> ";
		} else {
			number = *itr;
		}
		if (!number.Range (low, high)) continue;

		if (low != high) message += String ("%g..") % low;
		message += String ("%g") % high;
	}
	Error (key + message + ")");

	return (value);
}

//-----------------------------------------------------------
//	Highest_Control_Group - the highest group number 
//-----------------------------------------------------------

int Control_Service::Highest_Control_Group (int code, int nest1, int nest2, int nest3, int nest4)
{
	int i, highest, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Value_Stat stat;
	Key_Values *values_ptr;

	if (code == 0) return (0);

	codes [0] = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;

	values_ptr = &(_values);
	highest = 0;

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) {
			if (values_ptr->size () > 0) {
				itr = values_ptr->end ();
				highest = (--itr)->first;
			}
			break;
		}
		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) break;
		
		values_ptr = &(itr->second.nest);
	}
	return (highest);
}

//-----------------------------------------------------------
//	Number_Control_Group - the number of records in a group 
//-----------------------------------------------------------

int Control_Service::Number_Control_Group (int code, int nest1, int nest2, int nest3, int nest4)
{
	int i, count, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Values *values_ptr;

	if (code == 0) return (0);

	codes [0] = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;

	values_ptr = &(_values);
	count = 0;

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) break;

		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) {
			count = 0;
			break;
		}
		values_ptr = &(itr->second.nest);
		count = (int) (values_ptr->size ());
	}
	return (count);
}

//-----------------------------------------------------------
//	Control_Key_Empty - check for key data 
//-----------------------------------------------------------
	
bool Control_Service::Control_Key_Empty (int code, int nest1, int nest2, int nest3, int nest4)
{
	String key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		return (Highest_Control_Group (code, 0) == 0);
	} else {
		return (false);
	}
}

//-----------------------------------------------------------
//	Check_Control_Key - search for a key code 
//-----------------------------------------------------------
	
bool Control_Service::Check_Control_Key (int code, int nest1, int nest2, int nest3, int nest4)
{
	int i, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Values *values_ptr;

	if (code == 0) return (false);

	codes [0] = _current_key = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;

	values_ptr = &(_values);

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) break;

		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) break;

		if (itr->second.nest.size () == 0) {
			return (true);
		} else {
			values_ptr = &(itr->second.nest);
		}
	}
	return (false);
}

//-----------------------------------------------------------
//	Control_Key_Error - write a control key error message 
//-----------------------------------------------------------
	
void Control_Service::Control_Key_Error (int code, string range, int nest1, int nest2, int nest3, int nest4)
{
	int i, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Values *values_ptr;
	String message;

	if (code == 0) return;

	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return;

	message = key_ptr->key;
	message.To_Title ();

	codes [0] = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;

	values_ptr = &(_values);

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) break;

		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) break;

		if (i) {
			message += String ("_%d") % code;
		}
		if (itr->second.nest.size () == 0) {
			message += " = " + itr->second.value + " is Out of Range " + range;
			Error (message);
			return;
		} else {
			values_ptr = &(itr->second.nest);
		}
	}
}

//-----------------------------------------------------------
//	Output_Control_Label - construct a key for printing 
//-----------------------------------------------------------
	
Print_Stream & Control_Service::Output_Control_Label (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (Print (0));

	//---- print the label message ----

	String label = key_ptr->key;
	return (Print (1, label.To_Title ()) << " = ");
}

//-----------------------------------------------------------
//	Current_Key - return the current key text 
//-----------------------------------------------------------

String Control_Service::Current_Key (void) 
{
	Control_Key_Itr key_itr;
	String key;

	if (_current_key == 0) return (key);

	for (key_itr = _keys.begin (); key_itr != _keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		if (key_itr->code == _current_key) {
			key = key_itr->key;

			for (int i=1; i <= key_itr->levels; i++) {
				key += "_#";
			}
			break;
		}
	}
	return (key);
}

//-----------------------------------------------------------
//	Current_Label - return the current label text 
//-----------------------------------------------------------

String Control_Service::Current_Label (void) 
{
	if (_current_key == 0) return ("");

	Control_Key *key_ptr = Get_Control_Key (_current_key);
	if (key_ptr == 0) return ("");

	String label = key_ptr->key + _nesting;
	label.To_Title ();

	return (label);
}

//-----------------------------------------------------------
//	Show_Program_Keys - show the program control keys
//-----------------------------------------------------------

void Control_Service::Show_Program_Keys (void) 
{
	Control_Key_Itr key_itr;
	string key;

	cout << "\n\n\tControl File Keys:";

	for (key_itr = _keys.begin (); key_itr != _keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		key = key_itr->key;

		for (int i=1; i <= key_itr->levels; i++) {
			key += "_#";
		}
		cout << String ("\n\t  %-32.32s") % key;

		if (key_itr->option == REQ_KEY) {
			cout << "Req.";
		} else {
			cout << "Opt.";
		}
		switch (key_itr->type) {
			case INT_KEY:
				cout << "Int.";
				break;
			case FLOAT_KEY:
				cout << "Dec.";
				break;
			case BOOL_KEY:
				cout << "Bool";
				break;
			case IN_KEY:
				cout << "File";
				break;
			case OUT_KEY:
				cout << "New ";
				break;
			case PATH_KEY:
				cout << "Path";
				break;
			case TIME_KEY:
				cout << "Time";
				break;
			case LIST_KEY:
				cout << "List";
				break;
			default:
				cout << "Text";
				break;
		}
		if (!key_itr->value.empty ()) {
			cout << " = " << key_itr->value;
		}
	}
	cout << flush;
}

//-----------------------------------------------------------
//	Show_Extra_Keys - show the extra control keys
//-----------------------------------------------------------

void Control_Service::Show_Extra_Keys (void) 
{
	if (_extra_keys.size () == 0) return;

	Print (1);
	for (Str_Itr itr = _extra_keys.begin (); itr != _extra_keys.end (); itr++) {
		Warning ("Control Key \"") << *itr << "\" was Not Used";
	}
	Show_Message (1);
}

//-----------------------------------------------------------
//	Check_Keys - check for required keys
//-----------------------------------------------------------

bool Control_Service::Check_Keys (void) 
{
	Control_Key_Itr key_itr;
	Key_Value_Itr itr;
	String key;
	bool first = true;

	for (key_itr = _keys.begin (); key_itr != _keys.end (); key_itr++) {
		if (key_itr->option == REQ_KEY) {
			itr = _values.find (key_itr->code);

			if (itr != _values.end ()) continue;

			key = key_itr->key;

			for (int i=1; i <= key_itr->levels; i++) {
				key += "_#";
			}
			if (first) {
				first = false;
				Write (2, "Missing Control Keys:");
			}
			Write (1, "\t") << key;
		}
	}
	if (!first) {
		Flush ();
	}
	return (first);
}

//-----------------------------------------------------------
//	Required_Control_Key
//-----------------------------------------------------------

bool Control_Service::Required_Control_Key (int code) 
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);

	return (key_ptr->option == REQ_KEY);
}

bool Control_Service::Required_Control_Key (int code, bool flag) 
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);
	key_ptr->option = (flag) ? REQ_KEY : OPT_KEY;
	return (flag);
}

//-----------------------------------------------------------
//	Ignore_Control_Key
//-----------------------------------------------------------

bool Control_Service::Ignore_Control_Key (int code) 
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);
	key_ptr->option = IGNORE_KEY;
	return (true);
}

//-----------------------------------------------------------
//	Get_Control_Key - return the key data 
//-----------------------------------------------------------
	
Control_Key * Control_Service::Get_Control_Key (int code)
{
	Control_Key_Itr key_itr;

	for (key_itr = _keys.begin (); key_itr != _keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		if (key_itr->code == code) {
			return (&(*key_itr));
		}
	}
	return (0);
}

//-----------------------------------------------------------
//	Control_Key_Status - true if a key is currently active
//-----------------------------------------------------------
	
bool Control_Service::Control_Key_Status (int code)
{
	Control_Key_Itr key_itr;

	for (key_itr = _keys.begin (); key_itr != _keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		if (key_itr->code == code) {
			return (true);
		}
	}
	return (false);
}

//-----------------------------------------------------------
//	Document_Controls - write the program control keys
//-----------------------------------------------------------

void Control_Service::Document_Controls (FILE *file, bool flag) 
{
	Control_Key_Itr key_itr;
	String key, value, range;
	bool first_exe;

	first_exe = true;

	for (key_itr = _keys.begin (); key_itr != _keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;

		//---- check section breaks ----

		if (key_itr->code == 1) {
			fprintf (file, "\n\n%s Control Keys:", Program ());
		} else if (first_exe && key_itr->code >= EXECUTION_OFFSET) {
			first_exe = false;
			fprintf (file, "\n\nExecution Service Keys:");
		} else if (key_itr->code == Execution_Service::PROGRAM_REPORTS) {
			fprintf (file, "\n\nProgram Report Keys:");
		}
		key = key_itr->key;

		for (int i=1; i <= key_itr->levels; i++) {
			key += "_#";
		}
		fprintf (file, "\n\t%s", key.c_str ());

		if (key_itr->option == REQ_KEY) {
			fprintf (file, ((flag) ? "     (required" : "\tRequired"));
		} else {
			fprintf (file, ((flag) ? "     (optional" : "\tOptional"));
		}
		switch (key_itr->type) {
			case INT_KEY:
				fprintf (file, ((flag) ? ", integer" : "\tInteger"));
				break;
			case FLOAT_KEY:
				fprintf (file, ((flag) ? ", decimal" : "\tDecimal"));
				break;
			case BOOL_KEY:
				fprintf (file, ((flag) ? ", flag" : "\tBoolean"));
				break;
			case IN_KEY:
				fprintf (file, ((flag) ? ", input file" : "\tInput File"));
				break;
			case OUT_KEY:
				fprintf (file, ((flag) ? ", output file" : "\tOutput File"));
				break;
			case PATH_KEY:
				fprintf (file, ((flag) ? ", directory" : "\tDirectory"));
				break;
			case TIME_KEY:
				fprintf (file, ((flag) ? ", time" : "\tTime"));
				break;
			case LIST_KEY:
				fprintf (file, ((flag) ? ", list" : "\tList"));
				break;
			default:
				fprintf (file, ((flag) ? ", text" : "\tText"));
				break;
		}
		fprintf (file, ((flag) ? ", %s" : "\t%s"), key_itr->value.c_str ());
		fprintf (file, ((flag) ? ", %s" : "\t%s"), key_itr->range.c_str ());
		if (flag) fprintf (file, ")");
	}
}

//-----------------------------------------------------------
//	Add_Variable - process a variable string
//-----------------------------------------------------------

bool Control_Service::Add_Variable (string variable_value)
{
	String variable, value;
	Str_Key_Map_Stat map_stat;
	if (variable_value [0] != '@' && variable_value [0] != '%') return (false);

	size_t index = variable_value.find_first_of(" \t=");
	if (index == string::npos) return (false);

	variable = variable_value.substr(0, index);
	if (!variable.Ends_With("@") && !variable.Ends_With("%")) return (false);

	value = variable_value.erase(0, index);
	value.Trim_Left(" \t=");

	map_stat = _variable_map.insert(Str_Key_Map_Data(variable, value));
	if (!map_stat.second) {
		map_stat.first->second = value;
	}
	return (true);
}
