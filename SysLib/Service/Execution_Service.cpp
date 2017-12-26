 //*********************************************************
//	Execution_Service.cpp - general execution class
//*********************************************************

#include "Execution_Service.hpp"

#include "String.hpp"
#include "Db_File.hpp"

#include <new>
using namespace std;

Execution_Service *exe = 0;

//-----------------------------------------------------------
//	Insufficient_Memory
//-----------------------------------------------------------

void Insufficient_Memory (void)
{
	exe->Show_Error (String ("%s: Insufficient Memory") % exe->Program ());
	throw bad_alloc ();
}

//---------------------------------------------------------
//	Execution_Service - constructor
//---------------------------------------------------------

Execution_Service::Execution_Service (void) : Control_Service ()
{
	Control_Key exe_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ TITLE, "TITLE", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ REPORT_DIRECTORY, "REPORT_DIRECTORY", LEVEL0, OPT_KEY, PATH_KEY, "", "", NO_HELP },
		{ REPORT_FILE, "REPORT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", "[report_directory]filename[_partition][.prn]", NO_HELP },
		{ REPORT_FLAG, "REPORT_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PAGE_LENGTH, "PAGE_LENGTH", LEVEL0, OPT_KEY, INT_KEY, "65", ">= 0", NO_HELP },
		{ PROJECT_DIRECTORY, "PROJECT_DIRECTORY", LEVEL0, OPT_KEY, PATH_KEY, "", "", NO_HELP },
		{ DEFAULT_FILE_FORMAT, "DEFAULT_FILE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ MODEL_START_TIME, "MODEL_START_TIME", LEVEL0, OPT_KEY, TIME_KEY, "0:00", ">= 0 [seconds], 0.0 [hours], 0:00", NO_HELP },
		{ MODEL_END_TIME, "MODEL_END_TIME", LEVEL0, OPT_KEY, TIME_KEY, "24:00", "> [model_start_time]", NO_HELP },
		{ MODEL_TIME_INCREMENT, "MODEL_TIME_INCREMENT", LEVEL0, OPT_KEY, TIME_KEY, "15", "0, 15..60", NO_HELP },
		{ NUMBER_OF_THREADS, "NUMBER_OF_THREADS", LEVEL0, OPT_KEY, INT_KEY, "1", "1..128", NO_HELP },
		{ RANDOM_NUMBER_SEED, "RANDOM_NUMBER_SEED", LEVEL0, OPT_KEY, INT_KEY, "0", ">=0", NO_HELP },
		{ MAX_WARNING_MESSAGES, "MAX_WARNING_MESSAGES", LEVEL0, OPT_KEY, INT_KEY, "100000", ">= 0", NO_HELP },
		{ MAX_WARNING_EXIT_FLAG, "MAX_WARNING_EXIT_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ MAX_PROBLEM_COUNT, "MAX_PROBLEM_COUNT", LEVEL0, OPT_KEY, INT_KEY, "0", ">= 0", NO_HELP },
		{ MAX_IMPEDANCE_PENALTY, "MAX_IMPEDANCE_PENALTY", LEVEL0, OPT_KEY, INT_KEY, "100000", "> 0", NO_HELP },
		{ MAX_INPUT_PENALTY, "MAX_INPUT_PENALTY", LEVEL0, OPT_KEY, INT_KEY, "100000", ">= 0", NO_HELP },
		END_CONTROL
	};

	Key_List (exe_keys);

	_random_seed = 0;
	_num_threads = 1;
	_max_penalty = _input_penalty = 1000000;
	_memory_flag = false;
	_report_flag = false;
	debug = false;

	_start_time = 0;
	_end_time = Round (MIDNIGHT);

	Notes_Name_Flag (false);
	
	exe = this;
}

//-----------------------------------------------------------
//	Key_List - set the control key list
//-----------------------------------------------------------

void Execution_Service::Key_List (Control_Key keys []) 
{
	for (int i=0; keys [i].code != 0; i++) {
		Add_Control_Key (keys [i]);
	}
}

//---------------------------------------------------------
//	Ignore_Keys - set keys to ignore
//---------------------------------------------------------

void Execution_Service::Ignore_Keys (int *keys)
{
	if (keys == 0) return;

	for (int i=0; keys [i] != 0; i++) {
		Ignore_Control_Key (keys [i]);
	}
}

//-----------------------------------------------------------
//	Start_Execution - run the program 
//-----------------------------------------------------------

int Execution_Service::Start_Execution (String command_line) 
{
	Strings commands;
	command_line.Parse (commands);
	return (Execution (commands));
}

//-----------------------------------------------------------
//	Start_Execution - run the program 
//-----------------------------------------------------------

int Execution_Service::Start_Execution (int commands, char *command []) 
{
	Strings command_line;

	for (int i=1; i < commands; i++) {
		command_line.push_back (command [i]);
	}
	return (Execution (command_line));
}

//-----------------------------------------------------------
//	Start_Execution - run the program 
//-----------------------------------------------------------

int Execution_Service::Start_Execution (Strings keys) 
{
	Str_Itr key_itr;
	char *config_file;

	Program_Status (RUNNING);
	
	Show_Banner ();

	//---- check for a config file ----

	config_file = get_env ("TRANSIMS_CONFIG_FILE");

	if (config_file != 0) {
		if (!Read_Control_File (config_file)) {
			return (Return_Code ());
		}
	}

	//---- process the control keys ----
	
	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		Process_Control_String (key_itr->c_str ());
	}

	//---- open the output report ----

	if (Check_Control_Key (TITLE)) {
		Title (Get_Control_String (TITLE));
	}
	Open_Report (Get_Control_String (REPORT_DIRECTORY), Get_Control_String (REPORT_FILE), Set_Control_Flag (REPORT_FLAG));
	
	if (_report_flag) Read_Reports ();
		
	set_new_handler (Insufficient_Memory);

	//---- start processing ----

	Program_Control ();

	if (_report_flag) {
		List_Reports ();
	} else if (!Highest_Control_Group (PROGRAM_REPORTS, 0)) {
		Print (1);
		Flush ();
	}
	Execute ();

	return (Return_Code ());
}

//-----------------------------------------------------------
//	Execution - run the program 
//-----------------------------------------------------------

int Execution_Service::Execution (Strings &commands) 
{
	Program_Status (RUNNING);

	if (Command_Line (commands)) {
		if (_report_flag) Read_Reports ();

		set_new_handler (Insufficient_Memory);

		Program_Control ();

		if (_report_flag) {
			List_Reports ();
		} else if (!Highest_Control_Group (PROGRAM_REPORTS, 0)) {
			Print (1);
			Flush ();
		}
		Execute ();
	}
	return (Return_Code ());
}

//---------------------------------------------------------
//	Command_Line
//---------------------------------------------------------

bool Execution_Service::Command_Line (Strings &commands)
{
	bool banner_flag, syntax_flag, help_flag, command_flag, key_flag, param_flag, control_flag, exit_flag, doc_flag, user_flag;
	String message;
	Str_Itr str_itr;
	char ch, *config_file;

	banner_flag = syntax_flag = help_flag = command_flag = key_flag = param_flag = control_flag = exit_flag = doc_flag = user_flag = false;

	if ((int) commands.size () < 1) {
		Show_Banner ();
		Show_Syntax ();
		banner_flag = syntax_flag = true;

		Control_File (Get_Control_File ());
		if (Control_File ().empty ()) return (false);

	} else {

		//---- process the command line -----

		for (str_itr = commands.begin (); str_itr != commands.end (); str_itr++) {
			if (str_itr->at (0) == '-') {
				if (str_itr->length () == 1) {
					message ("Syntax Error in Command Line Option");
					goto error_message;
				}
				ch = str_itr->at (1);
				if (ch == 'Q' || ch == 'q') {
					Quiet (true);
				} else if (ch == 'H' || ch == 'h') {
					Syntax_Help ();
					help_flag = syntax_flag = banner_flag = true;
				} else if (ch == 'P' || ch == 'p') {
					Pause_Flag (true);
				} else if (ch == 'N' || ch == 'n') {
					No_Pause (true);
				} else if (ch == 'D' || ch == 'd' ||
					ch == 'B' || ch == 'b') {	//---- version 4.0 compatibility ----
					Detail (true);
				} else if (ch == 'K' || ch == 'k') {
					key_flag = true;
				} else if (ch == 'C' || ch == 'c') {
					control_flag = true;
					if (str_itr->size () > 2 && (str_itr->at (2) == 'X' || str_itr->at (2) == 'x')) {
						exit_flag = true;
					}
				} else if (ch == 'R' || ch == 'r') {
					doc_flag = true;
					if (str_itr->size () > 2 && (str_itr->at (2) == 'U' || str_itr->at (2) == 'u')) {
						user_flag = true;
					}
				} else {
					message ("Unrecognized Command Line Option = ");
					message += *str_itr;
					goto error_message;
				}
			} else if (str_itr->at(0) == '@' || str_itr->at (0) == '%') {
				if (!Add_Variable(*str_itr)) {
					message("Unrecognized Variable Syntax = ");
					message += *str_itr;
					goto error_message;
				}
			} else if (Control_File ().empty ()) {
				Control_File (*str_itr);
			} else {
				command_flag = true;
			}
		}
		if (Control_File ().empty ()) {
			if (control_flag) Write_Control_File ();
			if (doc_flag) Document (user_flag);
			if (help_flag) {
				if (command_flag) {
					message = Program ();
					message += " has Too Many Parameters";
					Show_Warning (message);
				}
				Pause_Flag (true);
				Exit_Stat (DONE);
				return (false);
			} else if (control_flag || doc_flag) {
				Exit_Stat (DONE);
				return (false);
			}
			message ("No Control File");
			goto error_message;
		}
		if (!banner_flag) {
			Show_Banner ();
			banner_flag = true;
		}
	}

	//---- check for a config file ----

	config_file = get_env ("TRANSIMS_CONFIG_FILE");

	if (config_file != 0) {
		if (!Read_Control_File (config_file)) {
			message ("Reading Configuration File: \"%s\"") % config_file;
			goto error_message;
		}
	}

	//---- read the control file ----

	if (!Read_Control_File (Control_File ())) {
		if (control_flag) {
			Write_Control_File ();
			Exit_Stat (DONE);
			return (false);
		} else {
			message ("Reading Control File: \"%s\"") % Control_File ();
			goto error_message;
		}
	}
	if (control_flag) Write_Control_File ();
	if (doc_flag) Document (user_flag);

	if (exit_flag) {
		Exit_Stat (DONE);
		return (false);
	}

	//---- open the output report ----

	if (Check_Control_Key (TITLE)) {
		Title (Get_Control_String (TITLE));
	}
	Open_Report (Get_Control_String (REPORT_DIRECTORY), Get_Control_String (REPORT_FILE), Set_Control_Flag (REPORT_FLAG));
	
	//---- show parameter warning ----

	if (command_flag) {
		message (Program ()) += " has Too Many Parameters";
		Warning (message);
		Write (1);
	}

	//---- show unrecognized control key warnings ----

	if (key_flag) {
		Show_Extra_Keys ();
	}
	if (!Check_Keys ()) {
		Error ("Required Control Keys were Not Found");
	}
	return (true);

error_message: 
	Quiet (false);
	if (!banner_flag) Show_Banner ();
	if (!syntax_flag) Show_Syntax ();
	Show_Error (message);
	return (false);
}

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Execution_Service::Program_Control (void) 
{
	String key;
	int increment;

	//---- override default page length ----

	if (Check_Control_Key (PAGE_LENGTH)) {
		int len = Get_Control_Integer (PAGE_LENGTH);
		if (len == 0) len = MAX_INTEGER;
		Page_Size (Page_Width (), len);
	}

	//---- project directory ----

	key = Get_Control_String (PROJECT_DIRECTORY);

	if (!key.empty ()) {
		Print_Filename (1, "Project Directory", key);
		Project_Directory (key);
	}

	//---- default file format ----

	Default_Format (Format_Code (Get_Control_Text (DEFAULT_FILE_FORMAT)));

	//---- model start time ----

	Model_Start_Time ((int) (Get_Control_Double (MODEL_START_TIME) * 60.0));

	//---- model end time ----

	Model_End_Time ((int) (Get_Control_Double (MODEL_END_TIME) * 60.0));

	//---- model time increment ----

	increment = Get_Control_Integer (MODEL_TIME_INCREMENT);

	time_periods.Set_Periods (increment, Model_Start_Time (), Model_End_Time ());

	//---- number of threads ----

	Num_Threads (Get_Control_Integer (NUMBER_OF_THREADS));

	//---- random number seed ----

	if (Control_Key_Status (RANDOM_NUMBER_SEED)) {
		key = Get_Control_String (RANDOM_NUMBER_SEED);

		if (!key.empty ()) {
			if (key.length () > 9) key.erase (9);
			random.Seed (key.Integer ());
		}
		_random_seed = random.Seed ();
		Output_Control_Label (RANDOM_NUMBER_SEED) << _random_seed;
	}

	//---- warning messages ----

	if (Control_Key_Status (MAX_WARNING_MESSAGES)) {
		Max_Warnings (Get_Control_Integer (MAX_WARNING_MESSAGES));
	}
	Warning_Flag (Get_Control_Flag (MAX_WARNING_EXIT_FLAG));

	//---- problem count ----

	Max_Problems (Get_Control_Integer (MAX_PROBLEM_COUNT));

	//---- max impedance penalty ----

	Max_Imp_Penalty (Round (Get_Control_Integer (MAX_IMPEDANCE_PENALTY)));

	//---- max input penalty ----

	Max_Input_Penalty (Round (Get_Control_Integer (MAX_INPUT_PENALTY)));
}

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Execution_Service::Execute (void) 
{
	Show_Error (String ("%s is Not Yet Implemented") % Program ());
}

//-----------------------------------------------------------
//	Show_Syntax - show the program syntax
//-----------------------------------------------------------

void Execution_Service::Show_Syntax (void) 
{
	Show_Message (String ("Syntax is %s [-flag] [control_file] [@variable@=value ...]\n") % Program ());
}

//-----------------------------------------------------------
//	Syntax_Help - show the program syntax and keys
//-----------------------------------------------------------

void Execution_Service::Syntax_Help (void) 
{
	bool quiet_flag = Quiet ();

	Report_Name_Itr itr;

	Show_Banner ();
	Show_Syntax ();
	Show_Message (2, "Optional Flags:");
	Show_Message ("  -H[elp]     = show program syntax and control keys");
	Show_Message ("  -C[ontrol]  = create/update a default control file");
	Show_Message ("  -K[eyCheck] = list unrecognized control file keys");
	Show_Message ("  -P[ause]    = pause before exiting");
	Show_Message ("  -N[oPause]  = never pause before exiting");
	Show_Message ("  -Q[uiet]    = execute without screen messages");
	Show_Message ("  -D[etail]   = execute with detailed status messages");
	Show_Message ("  -X[ML]      = write an XML file with control keys");
	Show_Message ("  -R[eport]   = write control keys and report names"); 

	Show_Program_Keys ();

	//---- list the report options ----

	if (_report_name.size () > 0) {
		Show_Message (2, "Report Options:");

		for (itr = _report_name.begin (); itr != _report_name.end (); itr++) {
			string message = "  ";
			Show_Message (message += itr->name);
		}
	}
	Show_Message (1);
	Quiet (quiet_flag);
}

//-----------------------------------------------------------
//	Document - write the program syntax and keys
//-----------------------------------------------------------

void Execution_Service::Document (bool user_flag) 
{
	Report_Name_Itr itr;

	String text = Program ();
	text += ".doc";

	FILE *file = f_open (text, "wt");
	if (file == 0) return;

	fprintf (file, "%s (%d.%d.%d)\n", Program (), (VERSION_CODE / 10), (VERSION_CODE % 10), Version ());

	fprintf (file, "Syntax:\n\t%s [-flag] [control_file] [@variable@=value ...]", Program ());

	fprintf (file, "\n\nOptional Flags:");
	fprintf (file, "\n\t-H[elp]\tshow program syntax and control keys");
	fprintf (file, "\n\t-C[ontrol]\tcreate/update a default control file");
	fprintf (file, "\n\t-K[eyCheck]\tlist unrecognized control file keys");
	fprintf (file, "\n\t-P[ause]\tpause before exiting");
	fprintf (file, "\n\t-N[oPause]\tnever pause before exiting");
	fprintf (file, "\n\t-Q[uiet]\texecute without screen messages");
	fprintf (file, "\n\t-D[etail]\texecute with detailed status messages");
	fprintf (file, "\n\t-X[ML]\twrite an XML file with control keys");
	fprintf (file, "\n\t-R[eport]\twrite control keys and report names"); 

	Document_Controls (file, user_flag);

	//---- list the report options ----

	if (_report_name.size () > 0) {
		fprintf (file, "\n\nReport Options:");

		for (itr = _report_name.begin (); itr != _report_name.end (); itr++) {
			fprintf (file, "\n\t%s", itr->name.c_str ());
		}
	}
}

//-----------------------------------------------------------
//	Report_List - set the report list
//-----------------------------------------------------------

void Execution_Service::Report_List (const char *names []) 
{
	int i;
	Report_Name rec;
	Report_Name_Itr itr;

	if (_report_name.capacity () == 0) {
		_report_name.reserve (10);
	}
	rec.flag = false;

	for (i=0; names [i][0] != '\0'; i++) {
		rec.name = names [i];

		_report_name.push_back (rec);
	}

	//---- add the report key ----

	if (i > 0) {
		String key = Program ();
		key.To_Upper ();
		key += "_REPORT";

		Control_Key report_key [] = { //--- code, key, level, status, type, default, range, help ----
			{ PROGRAM_REPORTS, "", LEVEL1, OPT_KEY, TEXT_KEY, "", "program report name", REPORT_HELP },
			END_CONTROL
		};
		report_key [0].key = key;

		Key_List (report_key);
		_report_flag = true;
	}
}

//---------------------------------------------------------
//	Read_Reports
//---------------------------------------------------------

void Execution_Service::Read_Reports (void) 
{
	int i, num, num_reports;
	String key;

	Report_Name_Itr itr;
	Report_Number report;

	//---- get the number of selected reports ----

	num_reports = Highest_Control_Group (PROGRAM_REPORTS, 0);
	
	if (num_reports <= 0) {
		_report_flag = false;
		return;
	}
	_report_flag = true;
	_report_number.reserve (num_reports);

	for (i=1; i <= num_reports; i++) {
		key = Get_Control_String (PROGRAM_REPORTS, i);

		if (key.empty ()) continue;

		report.type = 0;
		report.data = 0.0;

		//---- identify the report ----

		num = 1;

		for (itr = _report_name.begin (); itr != _report_name.end (); itr++, num++) {
			if (itr->name.Equals (key)) {
				itr->flag = true;
				report.type = num;
				break;
			} else {

				//---- check for a variable length report label ----

				size_t index = itr->name.find_last_of ('*');

				if (index != string::npos) {
					String temp = itr->name.substr (0, index);

					if (key.Starts_With (temp)) {
						itr->flag = true;
						report.type = num;
						temp = key.substr (index);
						report.data = temp.Double ();
						break;
					}
				}
			}
		}

		//---- check for unrecognized report types ----

		if (report.type == 0) {
			Error (String ("Unrecognized Report Type = %s") % key);
		} else {
			_report_number.push_back (report);
		}
	}
}

//-----------------------------------------------------------
//	List_Reports - print the report keys
//-----------------------------------------------------------

void Execution_Service::List_Reports (void) 
{
	int i, offset, num_reports;
	string key;
	bool first;

	if (!_report_flag) return;
	_report_flag = false;

	//---- get the number of selected reports ----

	num_reports = Highest_Control_Group (PROGRAM_REPORTS, 0);
	offset = 0;

	if (num_reports > 0) {
		first = true;

		for (i=1; i <= num_reports; i++) {
			key = Get_Control_String (PROGRAM_REPORTS, i);

			if (key.empty ()) continue;

			if (first) {
				Print (2, Program ()) << " Reports: ";
				first = false;
				offset = (int) strlen (Program ()) + 10;
			} else {
				Print (1, String ("%*c") % offset % BLANK);
			}
			Print (0, String ("%2d. ") % i) << key;
		}
	}
	Print (1);
	Flush ();
}

//-----------------------------------------------------------
//	Project_Directory
//-----------------------------------------------------------

void Execution_Service::Project_Directory (string dir) 
{
	_project_directory = dir;
	Fix_Directory (_project_directory);
}

//-----------------------------------------------------------
//	Project_Filename
//-----------------------------------------------------------

String Execution_Service::Project_Filename (string name)
{
	if (name.length () > 2) {
		if (name [1] == ':') {
			return (name);
		}
	}
	return (_project_directory + name);
}
