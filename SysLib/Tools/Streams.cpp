//*********************************************************
//	Streams.cpp - custom stream processing
//*********************************************************

#include "Streams.hpp"
#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Message_Stream - Message
//---------------------------------------------------------

Message_Stream & Message_Stream::Message (int num_lines, const char *text) 
{	
	if (!_quiet) {
		if (num_lines > 0) {
			while (num_lines-- > 0) cout << endl;
			cout << '\t';
		}
		cout << text << flush;
	}
	return (Active (!_quiet));
}

//---------------------------------------------------------
//	Print_Stream - constructor
//---------------------------------------------------------

Print_Stream::Print_Stream (int max_lines, int max_width)
{
	Page_Size (max_width, max_lines);
	_lines = 0;
	_page = 1;
	_active = true;
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Print_Stream::Open (string &report_name, bool report_flag) 
{
	if (report_flag) {
		_report.open (report_name.c_str (), ios_base::out | ios_base::app);
	} else {
		_report.open (report_name.c_str (), ios_base::out);
	}
	return (Open ());
}

//---------------------------------------------------------
//	Print_Stream - Print
//---------------------------------------------------------

Print_Stream & Print_Stream::Print (int num_lines, const char *text) 
{ 
	if (Open ()) {
		Page_Check (num_lines);

		//---- add the new line commands ----

		if (num_lines > 0) {
			while (num_lines-- > 0) {
				_report << endl;
			}
			_report << '\t';
		}

		//---- sent the message to the print file ----

		if ((int) strlen (text) > _page_width) {
			_report << string (text, _page_width);
		} else {
			_report << text;
		}
	}
	return (Active (true));
}

Print_Stream & Print_Stream::Print (int num_lines) 
{
	if (Open ()) {
		Page_Check (num_lines);

		while (num_lines-- > 0) {
			_report << endl;
		}
	}
	return (Active (true));
}

//---------------------------------------------------------
//	Break_Check
//---------------------------------------------------------

bool Print_Stream::Break_Check (int num_lines)
{
	if (_lines + num_lines > _page_lines) {
		New_Page ();
		return (true);
	} else {
		return (false);
	}
}

//---------------------------------------------------------
//	Page_Check
//---------------------------------------------------------

bool Print_Stream::Page_Check (int new_lines) 
{ 
	_lines += new_lines;

	if (_lines > _page_lines) {
		New_Page ();
		_lines += new_lines;
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	New_Page
//---------------------------------------------------------

void Print_Stream::New_Page (void) 
{
	//---- print the system header ----

	if (Open ()) {
		_page++;
		_lines = 3;

		time_t ltime;
		ltime = time (&ltime);

		_report << String ("\f\t%s\n\t%s  %s  page %d\n") % exe->Title () %
			exe->c_time (&ltime) % exe->Program () % _page;

		//---- print the user header ----

		if (exe->Header_Number () != 0) exe->Page_Header ();
	}
}

//---------------------------------------------------------
//	Write_Stream - Write
//---------------------------------------------------------

Write_Stream & Write_Stream::Write (int num_lines, const char *text) 
{ 
	_message->Message (num_lines, text);
	_print->Print (num_lines, text);
	return (Active (true));
}

Write_Stream & Write_Stream::Write (int num_lines) 
{
	_message->Message (num_lines);
	_print->Print (num_lines);
	return (Active (true));
}
