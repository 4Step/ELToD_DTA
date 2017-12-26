//*********************************************************
//	Report_Service.hpp - general reporting services
//*********************************************************

#ifndef REPORT_SERVICE_HPP
#define REPORT_SERVICE_HPP

#include "Message_Service.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Report_Service - general reporting class
//---------------------------------------------------------

class Report_Service : public Message_Service
{
public:
	Report_Service (void);

	String  Control_File (void)               { return (_control_file); }
	void    Control_File (string filename)    { _control_file = filename; }
	
	String  Title (void)                      { return (_title); }
	void    Title (string name)               { _title = name; }

	bool    Report_Flag (int type);
	int     Report_Type (int number);
	double  Report_Data (int number = 0);

	int     First_Report (void);
	int     Next_Report (void);

	void    Exit_Stat (Exit_Code stat);

	Write_Stream & Write (int num_lines)                    { return (_write.Write (num_lines)); }
	Write_Stream & Write (int num_lines, string text)       { return (_write.Write (num_lines, text.c_str ())); }
	Write_Stream & Write (int num_lines, const char *text)  { return (_write.Write (num_lines, text)); };

	Print_Stream & Print (int num_lines)                    { return (_print.Print (num_lines)); }
	Print_Stream & Print (int num_lines, string text)       { return (_print.Print (num_lines, text.c_str ())); }
	Print_Stream & Print (int num_lines, const char *text)  { return (_print.Print (num_lines, text)); }

	Write_Stream & Warning (string text)                    { return (Warning (text.c_str ())); }
	Write_Stream & Warning (const char *text);
	Write_Stream & Active (bool flag)                       { return (_write.Active (flag));  }

	void    Error (string text)                             { Error (text.c_str ()); }
	void    Error (const char *text);

	void    Mem_Error (string type)                         { Mem_Error (type.c_str ()); }
	void    Mem_Error (const char *type);

	void    Print_Filename (int num_lines, string label, string filename);
	void    File_Error (string label, string error);
	
	void    Print_Status (int num_lines, string text, clock_t time)      { Print_Status (num_lines, text.c_str (), time); }
	void    Print_Status (int num_lines, const char *text, clock_t time);

	void    Write_Time (int num_lines, string text)         { Write_Time (num_lines, text.c_str ()); } 
	void    Write_Time (int num_lines, const char *text);

	void    Header_Number (int number)        { _header = number; }
	int     Header_Number (void)              { return (_header); }

	bool    Break_Check (int num_lines)       { return (_print.Break_Check (num_lines)); }
	void    Page_Size (int width, int lines)  { _print.Page_Size (width, lines); }
	void    New_Page (void)                   { return (_print.New_Page ()); }
	void    Flush (void)                      { _print.Flush (); }

	int     Page_Lines (void)                 { return (_print.Page_Lines ()); }
	int     Page_Width (void)                 { return (_print.Page_Width ()); }

	virtual void Page_Header ()               {}

	string& Fix_Directory (string& dir);
	string  Replace_Directory (string dir, string filename);

	//---- problem services ----

	void Max_Problems (int number)            { _max_problems = number; };
	int  Max_Problems (void)                  { return (_max_problems); };

	int  Num_Problems (void)                  { return (_num_problems [TOTAL_PROBLEM]); };
	void Reset_Problems (void)                {	memset (_num_problems, '\0', sizeof (_num_problems)); }

	int  Set_Problem (Problem_Type problem);

	void Report_Problems (int total_records = 0, bool show_flag = true);

	void Add_Problems (int *problem_array);
	int * Get_Problems (void)                 { return (_num_problems); }

protected:
	void Open_Report (string report_dir, string report_name, bool report_flag);

	bool Page_Check (int new_lines)           { return (_print.Page_Check (new_lines)); }

	typedef struct {
		String name;
		bool   flag;
	} Report_Name;

	typedef vector <Report_Name> Report_Name_Array;
	typedef vector <Report_Name>::iterator Report_Name_Itr;

	typedef struct {
		int    type;
		double data;
	} Report_Number;

	typedef vector <Report_Number> Report_Number_Array;
	typedef vector <Report_Number>::iterator Report_Number_Itr;

	Report_Name_Array _report_name;
	Report_Name_Itr report_name_itr;

	Report_Number_Array _report_number;
	Report_Number_Itr report_num_itr;

private:
	Print_Stream _print;
	Write_Stream _write;

	String _title, _control_file;
	int _header, _max_problems, _num_problems [MAX_PROBLEM];
};
#endif
