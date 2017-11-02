//*********************************************************
//	Report_Service.cpp - general reporting services
//*********************************************************

#include "Report_Service.hpp"

#include "Execution_Service.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Report_Service - constructor
//---------------------------------------------------------

Report_Service::Report_Service (void) : Message_Service ()
{
	_header = _max_problems = 0;
	_write.Initialize (&_message, &_print);
	Reset_Problems ();

	_report_name.reserve (10);
	_report_number.reserve (10);
}

//---------------------------------------------------------
//	Open_Report
//---------------------------------------------------------

void Report_Service::Open_Report (string report_dir, string report_name, bool report_flag) 
{
	size_t index;

	//---- construct defaule report file name ----

	if (report_name.empty ()) {
		report_name = Control_File ();

		index = report_name.find ('.', report_name.length () - 4);

		if (index != report_name.npos) {
			report_name.erase (index);
		}
		report_name += ".prn";
	}

	//---- open report file ----

	report_dir = Replace_Directory (report_dir, report_name);

	if (!_print.Open (report_dir, report_flag)) {
		File_Error ("Opening Report File", report_dir);
	}

	//---- print the banner ----

	Page_Check (12);

	Banner (_print.Report ());

	Write (1, "Control File = ") << Control_File ();

	Show_Message (1);
	Print (1, "Report File  = ") << report_dir << ((report_flag) ? " (Append)" : " (Create)"); 
	Print (2, Title ());
	Print (1);
	Flush ();
}

//---------------------------------------------------------
//	Exit_Stat
//---------------------------------------------------------

void Report_Service::Exit_Stat (Exit_Code stat) 
{
	if (_print.Open ()) {
		if (stat == CANCEL) {
			Write_Time (2, "Process Terminated by the User");
		} else if (stat == FATAL) {
			Write_Time (2, "Process Terminated Due to Errors");
		} else if (stat == WARNING) {
			Write_Time (2, String ("Process Terminated with %d Warnin%s") % Warnings () % ((Warnings () > 1) ? "gs" : "g"));
		} else if (Warnings () > 0) {
			stat = WARNING;
			Write_Time (2, String ("Process Complete with %d Warnin%s") % Warnings () % ((Warnings () > 1) ? "gs" : "g"));
		} else {
			Write_Time (2, "Process Complete");	
		}
		Print (1);
		_print.Close ();
	}
	Message_Service::Exit_Stat (stat);
}

//---------------------------------------------------------
//	Error
//---------------------------------------------------------

void Report_Service::Error (const char *text) 
{ 
	if (!text || text [0] == '\0') Exit_Stat (FATAL);

	//---- send the message to the report file ----

	if (_print.Open ()) {
		Page_Check (3);
		time_t ltime;
		time (&ltime);

		Print (0, "\n\n\t") << c_time (&ltime);
		Print (0, "\n\tError: ") << text;
		Flush ();
	}

	//---- send the message to the screen ----

	Show_Error (text);
}

void Report_Service::Mem_Error (const char *type)
{
	Error (String ("Insufficient Memory for %s Data") % type); 
}

//---------------------------------------------------------
//	Warning
//---------------------------------------------------------

Write_Stream & Report_Service::Warning (const char *text) 
{ 
	if (!text || text [0] == '\0') Exit_Stat (FATAL);

	if (No_Warnings ()) return (_write);

	//---- check the warning message count ----

	Warnings (Warnings () + 1);

	if (Warnings () > Max_Warnings ()) {
		if (Warning_Flag ()) {
			Error (String ("Maximum Warning Messages %d Exceeded") % Max_Warnings ());
		}
		return (Active (false));
	}
	return (Write (1, "Warning: ") << text);
}

//---------------------------------------------------------
//	Print_Filename
//---------------------------------------------------------

void Report_Service::Print_Filename (int num_lines, string label, string filename) 
{
	if (label.empty () || filename.empty () || !_print.Open ()) return;

	size_t len, max_len;
	string text;

	len = filename.length ();
	max_len = (int) (Page_Width () - label.length () - 14);

	if (len > max_len && max_len > 0) {
		text = filename.substr (len - max_len);

		text [0] = text [1] = text [2] = '.';
	} else {
		text = filename;
	}
	Print (num_lines, label) << " = " << text;
}

//---------------------------------------------------------
//	File_Error
//---------------------------------------------------------

void Report_Service::File_Error (string label, string text) 
{ 
	if (label.empty () || text.empty ()) {
		Exit_Stat (FATAL);
	}
	Print (2, "Error: ") << label;
	Print (1, "\t\"") << text << "\"";

	Show_Error (String ("%s\n\t\t\"%s\"") % label % text);
}

//---------------------------------------------------------
//	Print_Status
//---------------------------------------------------------

void Report_Service::Print_Status (int num_lines, const char *text, clock_t time) 
{
	Print (num_lines, String ("%s%s") % text % Processing_String (time));
	Flush ();
}

//---------------------------------------------------------
//	Write_Time
//---------------------------------------------------------

void Report_Service::Write_Time (int num_lines, const char *text)
{
	if (!text || text [0] == '\0') Exit_Stat (FATAL);

	time_t ltime;
	time (&ltime);

	int seconds = (int) (ltime - StartTime ());
	
	Write (num_lines, String ("%s -- %s%s") % c_time (&ltime) % text % Processing_String (seconds));
	Flush ();
}

//---------------------------------------------------------
//      Fix_Directory
//---------------------------------------------------------

string& Report_Service::Fix_Directory (string &dir)
{
	size_t len = dir.length () - 1;

	if (len > 0) {
#ifdef _WIN32
		if (dir [len] != '\\' && dir [len] != '/') {
			dir += '\\';
		}
#else
		if (dir [len] != '/') {
			dir += "/";
		}
#endif
	}
	return (dir);
}

//---------------------------------------------------------
//      Replace_Directory
//---------------------------------------------------------

string Report_Service::Replace_Directory (string dir, string filename)
{
	if (dir.empty ()) return (filename);
	dir = Fix_Directory (dir);

	size_t index = filename.find_last_of ("\\/");

	if (index != string::npos) {
		filename = filename.substr (index+1);
	}
	return (dir + filename);
}

//---------------------------------------------------------
//	Set_Problem
//---------------------------------------------------------

int Report_Service::Set_Problem (Problem_Type problem)
{
	if (problem > TOTAL_PROBLEM && problem < MAX_PROBLEM) {
		_num_problems [problem]++;
		_num_problems [0]++;
	}
	if ((_num_problems [0] > _max_problems && _max_problems > 0) || problem == MAX_PROBLEM) {
		Report_Problems ();
		Error ("Maximum Number of Problems Exceeded");
	}
	return (problem);
}

//---------------------------------------------------------
//	Report_Problems
//---------------------------------------------------------

void Report_Service::Report_Problems (int total_records, bool show_flag)
{
	int i, num;
	double percent;
	String text;

	if (_num_problems [0] == 0) return;

	percent = 100.0 / _num_problems [0];

	for (i=1, num=2; i < MAX_PROBLEM; i++) {
		if (_num_problems [i] > 0) num++;
	}
	Break_Check (num);
	
	text ("Total Number of Problems = %d") % _num_problems [0];
	if (total_records > 0) {
		text += String (" (%.1lf%%)") % (100.0 * _num_problems [0] / total_records) % FINISH;
	}
	if (show_flag) {
		Write (2, text);
	} else {
		Print (2, text);
	}
	for (i=1; i < MAX_PROBLEM; i++) {
		num = _num_problems [i];

		if (num > 0) {
			text ("Number of %s (#%d) Problems = %d (%.1lf%%)") % 
				Problem_Code ((Problem_Type) i) % i % num % (num * percent) % FINISH;

			if (show_flag) {
				Write (1, text);
			} else {
				Print (1, text);
			}
		}
	}
}

//---------------------------------------------------------
//	Add_Problems
//---------------------------------------------------------

void Report_Service::Add_Problems (int *problem_array)
{
	for (int i=0; i < MAX_PROBLEM; i++) {
		_num_problems [i] += problem_array [i];
	}
}

//-----------------------------------------------------------
//	Report_Flag - check report type flag
//-----------------------------------------------------------

bool Report_Service::Report_Flag (int type) 
{
	if (type > (int) _report_name.size ()) return (false);
	return (_report_name [type - 1].flag);
}

//-----------------------------------------------------------
//	Report_Type - get the report type
//-----------------------------------------------------------

int Report_Service::Report_Type (int number) 
{
	if (number > (int) _report_number.size ()) return (0);
	return (_report_number [number - 1].type);
}

//-----------------------------------------------------------
//	Report_Data - get the report data
//-----------------------------------------------------------

double Report_Service::Report_Data (int number) 
{
	if (number > (int) _report_number.size ()) return (0.0);
	if (number == 0) return (report_num_itr->data);
	return (_report_number [number - 1].data);
}

//-----------------------------------------------------------
//	First_Report - get the first report type
//-----------------------------------------------------------

int Report_Service::First_Report (void) 
{
	report_num_itr = _report_number.begin ();

	if (report_num_itr != _report_number.end ()) {
		return (report_num_itr->type);
	} else {
		return (0);
	}
}

//-----------------------------------------------------------
//	Next_Report - get the next report type
//-----------------------------------------------------------

int Report_Service::Next_Report (void) 
{
	if (++report_num_itr != _report_number.end ()) {
		return (report_num_itr->type);
	} else {
		return (0);
	}
}

