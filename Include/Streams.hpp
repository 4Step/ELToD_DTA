//*********************************************************
//	Streams.hpp - custom stream processing
//*********************************************************

#ifndef STREAMS_HPP
#define STREAMS_HPP

#include <time.h>

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

//---------------------------------------------------------
//	Message_Stream - output message processing
//---------------------------------------------------------

class Message_Stream
{
public:
	Message_Stream (bool quiet = false)              { Quiet (quiet); }

	Message_Stream & Message (int num_lines, const char *text);
	
	Message_Stream & Message (const char *text)      { if (!_quiet) cout << "\n\t" << text << flush; return (Active (!_quiet)); }
	Message_Stream & Message (int num_lines)         { if (!_quiet) while (num_lines-- > 0) cout << endl; return (Active (!_quiet)); }

	Message_Stream & operator<< (int right)          { if (_active) cout << right << flush; return (*this); } 
	Message_Stream & operator<< (short right)        { if (_active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (double right)       { if (_active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (bool right)         { if (_active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (char right)         { if (_active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (const char *right)  { if (_active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (size_t right)       { if (_active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (void *right)        { if (_active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (string right)       { if (_active) cout << right << flush; return (*this); }

	bool Quiet (void)                                { return (_quiet); }
	void Quiet (bool flag)                           { _quiet = flag; }
	
	bool Active (void)                               { return (_active); }
	Message_Stream & Active (bool flag)              { _active = flag; return (*this); }

private:
	bool _quiet, _active;
};

//---------------------------------------------------------
//	Print_Stream - print output processing
//---------------------------------------------------------

class Print_Stream
{
public:
	Print_Stream (int lines = 65, int width = 95);

	Print_Stream & Print (int num_lines, const char *text);
	Print_Stream & Print (int num_lines);

	Print_Stream & operator<< (int right)          { if (Open ()) _report << right; return (*this); } 
	Print_Stream & operator<< (short right)        { if (Open ()) _report << right; return (*this); }
	Print_Stream & operator<< (double right)       { if (Open ()) _report << right; return (*this); }
	Print_Stream & operator<< (bool right)         { if (Open ()) _report << right; return (*this); }
	Print_Stream & operator<< (char right)         { if (Open ()) _report << right; return (*this); }
	Print_Stream & operator<< (char *right)        { if (Open ()) _report << right; return (*this); }
	Print_Stream & operator<< (const char *right)  { if (Open ()) _report << right; return (*this); }
	Print_Stream & operator<< (size_t right)       { if (Open ()) _report << right; return (*this); }
	Print_Stream & operator<< (void *right)        { if (Open ()) _report << right; return (*this); }
	Print_Stream & operator<< (string right)       { if (Open ()) _report << right; return (*this); }

	bool Open (string &report_name, bool report_flag);
	bool Open (void)                               { return (_report.is_open () && _active); }
	void Close (void)                              { _report.close (); }

	void Page_Size (int width, int num_lines)      { _page_width = width; _page_lines = num_lines; }
	int  Page_Lines (void)                         { return (_page_lines); }
	int  Page_Width (void)                         { return (_page_width); }

	bool Break_Check (int num_lines);
	bool Page_Check (int new_lines);
	void New_Page (void);
	void Flush (void)                              { if (Open ()) _report << flush; }
	
	bool Active (void)                             { return (_active); }
	Print_Stream & Active (bool flag)              { _active = flag; return (*this); }

	ofstream * Report (void)                       { return ((Open ()) ? &_report : 0); }

private:
	ofstream _report;

	bool _active;
	int _lines, _page, _page_width, _page_lines;
};

//---------------------------------------------------------
//	Write_Stream - write output processing
//---------------------------------------------------------

class Write_Stream
{
public:
	Write_Stream (void)                            { _message = 0; _print = 0; _active = true; }

	void Initialize (Message_Stream *msg, Print_Stream *prn)
	                                               { _message = msg; _print = prn; }

	Write_Stream & Write (int num_lines, const char *text);
	Write_Stream & Write (int num_lines);

	Write_Stream & operator<< (int right)          { if (Ok ()) { *_message << right; *_print << right; } return (*this); } 
	Write_Stream & operator<< (short right)        { if (Ok ()) { *_message << right; *_print << right; } return (*this); }
	Write_Stream & operator<< (double right)       { if (Ok ()) { *_message << right; *_print << right; } return (*this); }
	Write_Stream & operator<< (bool right)         { if (Ok ()) { *_message << right; *_print << right; } return (*this); }
	Write_Stream & operator<< (char right)         { if (Ok ()) { *_message << right; *_print << right; } return (*this); }
	Write_Stream & operator<< (char *right)        { if (Ok ()) { *_message << right; *_print << right; } return (*this); }
	Write_Stream & operator<< (const char *right)  { if (Ok ()) { *_message << right; *_print << right; } return (*this); }
	Write_Stream & operator<< (size_t right)       { if (Ok ()) { *_message << right; *_print << right; } return (*this); }
	Write_Stream & operator<< (void *right)        { if (Ok ()) { *_message << right; *_print << right; } return (*this); }
	Write_Stream & operator<< (string right)       { if (Ok ()) { *_message << right; *_print << right; } return (*this); }

	bool Active (void)                             { return (_active); }
	Write_Stream & Active (bool flag)              { _active = flag; return (*this); }

private:
	bool Ok (void)                                 { return (_message != 0 && _print != 0 && _active); }

	bool _active;
	Message_Stream *_message;
	Print_Stream *_print;
};

#endif
