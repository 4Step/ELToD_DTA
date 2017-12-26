//*********************************************************
//	Message_Service.cpp - general message services
//*********************************************************

#include "Message_Service.hpp"
//---------------------------------------------------------
//	Message_Service - constructor
//---------------------------------------------------------

Message_Service::Message_Service (void) : Static_Service ()
{
	_status = DONE;
	Program ("Program");
	Version (0);
	CopyRight ("2017 by TRANSIMS Open-Source");
	time (&_starttime);
	_progress = 0;
	_warnings = 0;
	_max_warnings = 100000;
	_warning_flag = true;
	_no_warnings = false;
	_pause = false;
	_no_pause = false;
	_detail = false;
	_send_messages = true;
}

//-----------------------------------------------------------
//	Show_Banner
//-----------------------------------------------------------

void Message_Service::Show_Banner ()
{
	if (!Quiet ()) {
		Banner ();
		cout << endl;
	}
}

//-----------------------------------------------------------
//	Banner
//-----------------------------------------------------------

void Message_Service::Banner (ofstream *file)
{
	time_t ltime;
	const char *bar = "********************************************";
	String version, copyright, timestamp;

	if (file == 0) file = (ofstream *) &cout;
	ltime = StartTime ();

	version ("%s - Version %d.%d.%d") % Program () % (VERSION_CODE / 10) % (VERSION_CODE % 10) % Version ();
	copyright ("Copyright %s") % CopyRight ();
	timestamp (c_time (&ltime));

	*file << "\n\t" << bar;
	*file << String ("\n\t|%42c|") % BLANK;
	*file << "\n\t|" << version.Center (42) << "|";
	*file << "\n\t|" << copyright.Center (42) << "|";
	*file << "\n\t|" << timestamp.Center (42) << "|";
	*file << String ("\n\t|%42c|") % BLANK;
	*file << "\n\t" << bar << endl << flush;
}

//---------------------------------------------------------
//	Exit_Stat
//---------------------------------------------------------

void Message_Service::Exit_Stat (Exit_Code stat) 
{
	if (stat != Program_Status ()) {
		Program_Status (stat);
	}
	if (Pause_Flag ()) Pause_Process ();
	if (!Quiet ()) cout << endl;

	int code = Return_Code ();

	exit (code);
}

//---------------------------------------------------------
//	Return_Code
//---------------------------------------------------------
	
int Message_Service::Return_Code (void)
{ 
	if (Program_Status () == FATAL) {
		return (1);
	} else if (Program_Status () == WARNING) {
		return (2);
	} else {
		return (0);
	}
}

//---------------------------------------------------------
//	Show_Question
//---------------------------------------------------------

bool Message_Service::Show_Question (const char *text) 
{
	if (Detail ()) {
		Show_Error ("Show_Question in 'Detail' mode (-D)");
		return (false);
	}
	char answer [10];

	cout << "\n\t" << text << " (Y/N): " << flush;

	cin.getline (answer, sizeof (answer));

	return (answer [0] == 'y' || answer [0] == 'Y');
}

//---------------------------------------------------------
//	Show_Error
//---------------------------------------------------------

void Message_Service::Show_Error (const char *text) 
{
	if (Quiet ()) {
		Quiet (false);
		Show_Banner ();
	}
	if (Program_Status () == FATAL) {
		cout << "\n\t" << text << flush;
	} else {
		cout << "\n\n\tError: " << text << flush;

		Pause_Flag (true);

		Exit_Stat (FATAL);
	}
}

//---------------------------------------------------------
//	Show_Warning
//---------------------------------------------------------

Message_Stream & Message_Service::Show_Warning (const char *text) 
{
	if (No_Warnings ()) return (_message);

	if (Quiet () && _warnings == 0) {
		Quiet (false);
		Show_Banner ();
		Quiet (true);
	}
	_warnings++;
	if (_max_warnings > 0 && _warnings > _max_warnings) {
		if (_warning_flag) {
			Show_Error (String ("Maximum Warning Messages % Exceeded") % _max_warnings);
		}
		return (_message.Active (false));
	}
	return (Show_Message ("Warning: ") << text);
}


//---------------------------------------------------------
//	Set_Progress
//---------------------------------------------------------

void Message_Service::Set_Progress (void) 
{
	_progress = 0;
	_first = _last = clock ();
}

//---------------------------------------------------------
//	Show_Dot
//---------------------------------------------------------

void Message_Service::Show_Dot (void) 
{
	if (!Quiet ()) {
		clock_t now = clock ();

		if ((now - _last) >= 5 * CLOCKS_PER_SEC) {
			_last = now;
			cout << "." << flush;
		}
	}
}

//---------------------------------------------------------
//	Show_Progress
//---------------------------------------------------------

void Message_Service::Show_Progress (int value) 
{
	if (value < 1) {
		value = ++_progress;
	} else {
		_progress = value;
	}
	if (!Quiet ()) {
		clock_t now = clock ();

		if ((now - _last) >= CLOCKS_PER_SEC) {
			if (Detail ()) {
				cout << "\n\tRecord: " << value << flush; 
			} else {
				cout << String (" %-16d\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b") % value << flush;
			}
			_last = now;
		}
	}
}

void Message_Service::Show_Progress (const char *text) 
{
	_progress++;

	if (!Quiet ()) {
		clock_t now = clock ();

		if ((now - _last) >= CLOCKS_PER_SEC) {
			if (Detail ()) {
				cout << "\n\tRecord: " << text << flush; 
			} else {
				cout << String (" %-16.16s\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b") % text << flush;
			}
			_last = now;
		}
	}
}

//---------------------------------------------------------
//	End_Progress
//---------------------------------------------------------

void Message_Service::End_Progress (bool new_line) 
{
    if (!Quiet ()) {
		if (Detail ()) {
			cout << "\n\tRecord: " << _progress;
		} else {
			cout << " " << _progress;
		}
		if (new_line) {
			cout << endl;
		}
		cout << flush;
	}
}

void Message_Service::End_Progress (const char *text) 
{
    if (!Quiet ()) {
		if (Detail ()) {
			cout << "\n\tRecord: " << text << endl << flush; 
		} else {
			cout << String (" %-16.16s\n") % text;
		}
	}
}

clock_t Message_Service::End_Progress (clock_t now) 
{
	now = now - _first;
	if (!Quiet ()) {
		int seconds = DTOI (now / CLOCKS_PER_SEC);
		if (seconds >= 1) {
			cout << flush;
			cout << " " << _progress << Processing_String (seconds) << endl << flush;
		} else {
			cout << " " << _progress << endl << flush;
		}
	}
	return (now);
}

clock_t Message_Service::End_Progress (int count, clock_t now) 
{
	now = now - _first;
	if (!Quiet ()) {
		int seconds = DTOI (now / CLOCKS_PER_SEC);
		if (seconds >= 1) {
			cout << flush;
			cout << " " << count << Processing_String (seconds) << endl << flush;
		} else {
			cout << " " << count << endl << flush;
		}
	}
	return (now);
}

//---------------------------------------------------------
//	Process_Duration
//---------------------------------------------------------

String Message_Service::Process_Duration (clock_t duration)
{
	int seconds = DTOI (duration / CLOCKS_PER_SEC);
	if (seconds >= 1) {
		return (String (" (%s)") % Processing_Time (seconds));
	} else {
		return (String (""));
	}
}

//---------------------------------------------------------
//	Processing_Time
//---------------------------------------------------------

String Message_Service::Processing_Time (int seconds) 
{
	int hours = seconds / 3600;
	seconds = seconds % 3600;
	int minutes = seconds / 60;
	seconds = seconds % 60;

	return (String ("%d:%02d:%02d") % hours % minutes % seconds);
}

String Message_Service::Processing_Time (clock_t clock_time) 
{
	return (Processing_Time (DTOI (clock_time / CLOCKS_PER_SEC)));
}

//---------------------------------------------------------
//	Processing_String
//---------------------------------------------------------

String Message_Service::Processing_String (int seconds) 
{
	return (String (" (%s)") % Processing_Time (seconds));
}

String Message_Service::Processing_String (clock_t clock_time) 
{
	return (Processing_String (DTOI (clock_time / CLOCKS_PER_SEC)));
}

//---------------------------------------------------------
//	Pause_Process
//---------------------------------------------------------

void Message_Service::Pause_Process (void) 
{
    if (!Quiet () && !No_Pause () && !Detail ()) {
		char pause [10];

		cout << "\n\n\tPress Enter to Continue" << flush;

		cin.getline (pause, sizeof (pause));
	}
}

//---------------------------------------------------------
//	Get_Control_File
//---------------------------------------------------------

String Message_Service::Get_Control_File () 
{
	String filename;
	char buffer [512];
	size_t index;

	cout << "\n\tEnter Control File Name: ";

	cin.getline (buffer, sizeof (buffer));

	filename = buffer;

	filename.Trim ();

	index = filename.find ('\n');
	if (index != filename.npos) filename.erase (index);

	return (filename);
}
