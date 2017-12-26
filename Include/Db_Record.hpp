//*********************************************************
//	Db_Record.hpp - database record
//*********************************************************

#ifndef DB_RECORD_HPP
#define DB_RECORD_HPP

#include "Db_Status.hpp"
#include "Buffer.hpp"

//---------------------------------------------------------
//	Db_Record Class definition
//---------------------------------------------------------

class Db_Record : public Db_Status
{
public:
	Db_Record (Format_Type format = UNFORMATED);

	bool  Record_Size (int size)            { return (Record ().Size (size)); }
	int   Record_Size (void)                { return (Record ().Size ()); }

	int   Max_Size (void)                   { return (Record ().Max_Size ()); }
	int   Max_Read (void)                   { return ((String_Flag ()) ? Record ().Max_Read () : Max_Size ()); }
	
	int   Set_Size (void)                   { return (Record ().Set_Size (String_Flag ())); }

	void  Record_Format (Format_Type format);
	Format_Type  Record_Format (void)       { return (_record_format); }

	void  Delimiters (const char *limits)   { if (limits != 0) _delimiters = limits; }
	char *Delimiters (void)                 { return ((char *) _delimiters); }

	bool  Record (void *data, int size = 0);
	bool  Record (Buffer &buf)              { return (Record ().Data (buf)); }

	Buffer & Record (void)                  { return (_record); }
	Buffer & Reset_Record (void);

	void   Reset_Buffers (void);

	String Record_String (void)             { return ((String) Record ().String ()); }
	char * Record_Pointer (void)            { return (Record ().Pointer ()); }

	string Get_Field_Number (int number);
	bool   Set_Field_Number (int number, string field);

protected:
	bool Max_Size (int size)                { return (Record ().Max_Size (size)); }

private:
	Format_Type  _record_format;

	bool _nested;

	Buffer _record;

	const char *_delimiters;

	bool String_Flag (void)                 { return (_record_format != BINARY && _record_format != FIXED_COLUMN); }
};
#endif
