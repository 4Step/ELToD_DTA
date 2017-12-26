//*********************************************************
//	Execution_Service.hpp - general execution class
//*********************************************************

#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include "Control_Service.hpp"
#include "Random.hpp"
#include "Time_Periods.hpp"

//---------------------------------------------------------
//	Execution_Service - general execution class
//---------------------------------------------------------

class Execution_Service : public Control_Service
{
	friend class Control_Service;

public:
	Execution_Service (void);

	void   Key_List (Control_Key keys []);
	void   Report_List (const char *names []);

	void   Ignore_Keys (int *keys);

	void   Project_Directory (string dir);
	String Project_Directory (void)          { return (_project_directory); }

	String Project_Filename (string name);
	String Project_Filename (string name, string ext)
	                                         { return (Project_Filename (name) + ext); }
	Random random;
	int    Random_Seed (void)                { return (_random_seed); }

	virtual void Execute (void);

	int    Start_Execution (String command_line);
	int    Start_Execution (int commands, char *command []);
	int    Start_Execution (Strings keys);

	int    Num_Threads (void)                { return (_num_threads); }
	void   Num_Threads (int value)           { _num_threads = MAX (value, 1); }

	int    Max_Imp_Penalty (void)            { return (_max_penalty); }
	void   Max_Imp_Penalty (int value)       { _max_penalty = MAX (value, 1); }

	int    Max_Input_Penalty (void)          { return (_input_penalty); }
	void   Max_Input_Penalty (int value)     { _input_penalty = MAX (value, 0); }

	bool   Memory_Flag (void)                { return (_memory_flag); }
	void   Memory_Flag (bool flag)           { _memory_flag = flag; }

	void   Notes_Name_Flag (bool flag)       { _notes_flag = flag; }
	bool   Notes_Name_Flag (void)            { return (_notes_flag); }

	int  Model_Start_Time (void)           { return (_start_time); }
	void   Model_Start_Time (int value)    { _start_time = value; }

	int  Model_End_Time (void)             { return (_end_time); }
	void   Model_End_Time (int value)      { _end_time = value; }

	Time_Periods time_periods;

	void   Document (bool user_flag = false);

	bool debug;

protected:
	enum Execution_Service_Keys { TITLE = EXECUTION_OFFSET, REPORT_DIRECTORY, REPORT_FILE, REPORT_FLAG, PAGE_LENGTH, 
		PROJECT_DIRECTORY, DEFAULT_FILE_FORMAT, TIME_OF_DAY_FORMAT, MODEL_START_TIME, MODEL_END_TIME, 
		MODEL_TIME_INCREMENT, UNITS_OF_MEASURE, DRIVE_SIDE_OF_ROAD, NUMBER_OF_THREADS, RANDOM_NUMBER_SEED, 
		MAX_WARNING_MESSAGES, MAX_WARNING_EXIT_FLAG, MAX_PROBLEM_COUNT, MAX_IMPEDANCE_PENALTY, MAX_INPUT_PENALTY,
		PROGRAM_REPORTS };

	virtual void Program_Control (void);
	void List_Reports (void);

private:

	int  Execution (Strings &commands);
	bool Command_Line (Strings &commands);
	void Show_Syntax (void);

	void Syntax_Help (void);

	void Read_Reports (void);
	bool _report_flag, _notes_flag;

	String _report_key;
	String _project_directory;

	int _num_threads, _random_seed, _max_penalty, _input_penalty;
	bool _memory_flag;

	String _filename;
	int _start_time, _end_time;
};

extern Execution_Service *exe;

#endif

