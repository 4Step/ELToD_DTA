//*********************************************************
//	Message_Service.hpp - general message services
//*********************************************************

#ifndef MESSAGE_SERVICE_HPP
#define MESSAGE_SERVICE_HPP

#include "Static_Service.hpp"
#include "Streams.hpp"

//---------------------------------------------------------
//	Message_Service - general message class
//---------------------------------------------------------

class Message_Service : public Static_Service
{
public:
	Message_Service (void);
	virtual ~Message_Service (void) {}

	enum Exit_Code {DONE, FATAL, WARNING, CANCEL, RUNNING};

	Exit_Code Program_Status (void)                  { return (_status); }
	void Program_Status (Exit_Code stat)             { _status = stat; }

	bool Quiet (void)                                { return (_message.Quiet ()); }
	void Quiet (bool flag)                           { _message.Quiet (flag); }

	const char * Program (void)                      { return (_program); }
	void Program (const char *p)                     { _program = p; }

	int Version (void)                               { return (_version); }
	void Version (int number)                        { _version = number; }

	const char * CopyRight (void)                    { return (_copyright); }
	void CopyRight (const char *c)                   { _copyright = c; }

	time_t StartTime (void)                          { return (_starttime); }
	void StartTime (time_t dt)                       { _starttime = dt; }
	
	bool Send_Messages (void)                        { return (_send_messages); }
	void Send_Messages (bool send)                   { _send_messages = send; }

	bool No_Warnings (void)                          { return (_no_warnings); }
	void No_Warnings (bool flag)                     { _no_warnings = flag; }

	void Show_Banner (void);

	Message_Stream & Show_Message (int num_lines)    { return (_message.Message (num_lines)); }
	Message_Stream & Show_Message (string text)      { return (_message.Message (text.c_str ())); }
	Message_Stream & Show_Message (int num_lines, string text)	
	                                                 { return (_message.Message (num_lines, text.c_str ())); }

	Message_Stream & Show_Message (const char *text) { return (_message.Message (text)); }
	Message_Stream & Show_Message (int num_lines, const char *text)
	                                                 { return (_message.Message (num_lines, text)); }

	bool Show_Question (string text)                 { return (Show_Question (text.c_str ())); }  
	bool Show_Question (const char *text);

	void Show_Error (string text)                    { Show_Error (text.c_str ()); }
	void Show_Error (const char *text);

	Message_Stream & Show_Warning (string text)      { return (Show_Warning (text.c_str ())); }
	Message_Stream & Show_Warning (const char *text);

	void Set_Progress (void);
	void Show_Dot (void);

	void Show_Progress (int value = 0);
	void Show_Progress (string text)                 { Show_Progress (text.c_str ()); }
	void Show_Progress (const char *text);          

	void End_Progress (bool new_line = true);
	void End_Progress (string text)                  { End_Progress (text.c_str ()); }
	void End_Progress (const char *text);
	clock_t End_Progress (clock_t clock_time);
	clock_t End_Progress (int count, clock_t clock_time);

	int  Progress_Count (void)                       { return (_progress); }
	String Process_Duration (clock_t duration);

	String Processing_Time (int seconds);
	String Processing_Time (clock_t clock_time);
	
	String Processing_String (int seconds);
	String Processing_String (clock_t clock_time);

	void Pause_Process (void);

	String Get_Control_File (void);

	int Return_Code (void);

	virtual void Exit_Stat (Exit_Code stat);

protected:
	void Banner (ofstream *file = 0);

	int  Warnings (void)                             { return (_warnings); }
	void Warnings (int value)                        { _warnings = value; }

	int  Max_Warnings (void)                         { return (_max_warnings); }
	void Max_Warnings (int value)                    { _max_warnings = value; }

	bool Warning_Flag (void)                         { return (_warning_flag); }
	void Warning_Flag (bool flag)                    { _warning_flag = flag; }

	bool Pause_Flag (void)                           { return (_pause); }
	void Pause_Flag (bool flag)                      { _pause = flag; }

	bool No_Pause (void)                             { return (_no_pause); }
	void No_Pause (bool flag)                        { _no_pause = flag; }

	bool Detail (void)                               { return (_detail); }
	void Detail (bool flag)                          { _detail = flag; }

	Message_Stream _message;

private:
	int _version, _progress, _warnings, _max_warnings;
	bool _pause, _no_pause, _detail, _send_messages, _warning_flag, _no_warnings;
	const char *_program, *_copyright;
	time_t _starttime;
	clock_t _first, _last;

	Exit_Code _status;
};
#endif
