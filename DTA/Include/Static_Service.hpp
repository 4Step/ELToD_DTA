//*********************************************************
//	Static_Service.hpp - static program services
//*********************************************************

#ifndef STATIC_SERVICE_HPP
#define STATIC_SERVICE_HPP

#include "System_Defines.hpp"
#include "String.hpp"
#include "Threads.hpp"

#include <stdexcept>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

//---------------------------------------------------------
//	Static_Service - static program services
//---------------------------------------------------------

class Static_Service
{
public:
	Static_Service (void);

	static Format_Type Format_Code (String text)                 { return ((Format_Type) Type_Code (text, format_codes, "Format")); }
	static const char * Format_Code (Format_Type code)           { return (Type_Code (code, format_codes)); }

	static Format_Type Model_Code (String text)                  { return ((Format_Type) Type_Code (text, model_codes, "Model")); }
	static const char * Model_Code (Format_Type code)            { return (Type_Code (code, model_codes)); }

	static Field_Type Field_Code (String text)                   { return ((Field_Type) Type_Code (text, field_codes, "Field")); }
	static const char * Field_Code (Field_Type code)             { return (Type_Code (code, field_codes)); }
	
	static Mode_Type Mode_Code (String text)                     { return ((Mode_Type) Type_Code (text, mode_codes, "Mode")); }
	static const char * Mode_Code (Mode_Type code)               { return (Type_Code (code, mode_codes)); }

	static Problem_Type Problem_Code (String text);
	static const char * Problem_Code (Problem_Type code) { return (Type_Code (code, problem_codes)); }

	static Status_Code Db_File_Code (String text)                { return ((Status_Code) Type_Code (text, db_file_codes, "Status Code", DB_ERROR)); }
	static const char * Db_File_Code (Status_Code code)          { return (Type_Code (code, db_file_codes)); }

	static int Bool_Code (String text)                           { return (Type_Code (text, bool_codes, "True/False")); }
	static string Bool_Code (int code)                           { return (Type_Code (code, bool_codes)); }

	static struct tm *local_time (time_t *tim);
	static char * c_time (time_t *tim);
	
	static FILE *f_open (string filename, const char *control);
	static char *get_env (const char *var);

	static double Scale (double value)    { return (value * 10.0); }
	static int Round (double value)       { return ((value < 0) ? -((int) (-value * 10 + 0.5)) : (int) (value * 10 + 0.5)); }
	static int Round (int value)          { return (value * 10); }
	static double UnRound (int value)     { return ((double) value / 10.0); }
	static double UnRound (double value)  { return (value / 10.0); }
	static int Resolve (int value)        { return ((value < 0) ? -((-value + 5) / 10) : ((value + 5) / 10)); }
	static int Resolve (double value)     { return ((value < 0) ? -((int) ((-value + 5.0) / 10.0)) : (int) ((value + 5.0) / 10.0)); }
	static int Resolve (unsigned value)   { return ((value + 5) / 10); }
	static int DTOI (double value)        { return ((value < 0) ? -((int) (-value + 0.5)) : (int) (value + 0.5)); }

	static char * Clean (char *text);

	static Format_Type  Default_Format (void)       { return (_default_format); }
	static void Default_Format (Format_Type format) { _default_format = format; }

	static bool Data_Format (String text, Format_Type &file_format, Format_Type &model_format);
	static String Data_Format (Format_Type file_format, Format_Type model_format = TRANSIMS, bool code_flag = false); 

private:
	static Format_Type _default_format;

	struct Code_Text {
		int code;
		const char * text;
	};
	static Code_Text format_codes [];
	static Code_Text model_codes [];
	static Code_Text field_codes [];
	static Code_Text mode_codes [];
	static Code_Text db_file_codes [];
	static Code_Text problem_codes [];
	static Code_Text bool_codes [];

	static int Type_Code (String text, Code_Text *codes, const char *label, int error_code = 0);
	static const char * Type_Code (int code, Code_Text *codes);
};
#endif
