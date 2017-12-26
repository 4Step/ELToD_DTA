//********************************************************* 
//	Db_File.hpp - database file
//*********************************************************

#ifndef DB_FILE_HPP
#define DB_FILE_HPP

#include "Threads.hpp"
#include "Db_Record.hpp"
#include "String.hpp"

#include <io.h>
#include <iostream>
#include <fstream>

//---------------------------------------------------------
//	Db_File Class definition
//---------------------------------------------------------

class Db_File : public Db_Record
{
public:
	Db_File (Access_Type access = READ, Format_Type format = UNFORMATED);
	Db_File (string filename, Access_Type access = READ, Format_Type format = UNFORMATED);
	virtual ~Db_File (void);

	void    Initialize (Access_Type access = READ, Format_Type format = UNFORMATED);

	bool    Filename (string filename);
	String  Filename (int index);
	String& Filename (void)                  { return (_filename); }

	virtual bool Find_File (void);
	
	virtual bool Open (string path = "");
	virtual bool Open (int index);

	bool    Create (string path = "")        { File_Access (CREATE); return (Open (path)); }

	bool    First_Open (void)                { return (_first_open); }
	virtual void First_Open (bool flag)      { _first_open = flag; }

	Strings File_Parse (void);

	fstream &File (void)                     { return (_file); }
	bool    Check_File (void)                { return (Is_Open () ? true : Status (NOT_OPEN)); }

	virtual bool Is_Open (void)              { return (_file.is_open ()); }

	int     Version (void)                   { return (_version); }
	void    Version (int code)               { _version = code; }

	virtual bool  Read_Record (int number = 0);

	bool  Read (void *record, int size);
	bool  Read (void);
	bool  Read (void *record, size_t size)   { return (Read (record, (int) size)); }
	bool  Read (void *record, int size, size_t offset);

	virtual bool  Write_Record (int number = 0);

	bool  Write (void *record, int size);
	bool  Write (void);
	bool  Write (char *record)               { return (Write (record, (int) strlen (record))); }
	bool  Write (void *record, size_t size)  { return (Write (record, (int) size)); };
	bool  Write (void *record, int size, size_t offset);

	virtual bool  Close (void);
	virtual bool  Rewind (void);
	virtual size_t File_Size (void);

	size_t Offset (void);
	bool  Offset (size_t offset);

	void  First_Offset (size_t offset)      { _first_offset = offset; }
	size_t First_Offset (void)              { return (_first_offset); }

	void  File_Access (Access_Type access)  { _file_access = access; }
	Access_Type  File_Access (void)         { return (_file_access); }
	
	Format_Type  File_Format (void)         { return (_file_format); }
	void File_Format (Format_Type format);
	void File_Format (string text)          { File_Format (Format_Code (text)); }	

	Format_Type  Model_Format (void)        { return (_model_format); }
	void Model_Format (Format_Type format)  { _model_format = format; }
	void Model_Format (string text)         { _model_format = Model_Code (text); }

	virtual int  Num_Records (void);
	void Num_Records (int num)              { _num_records = num; }
	void Add_Records (int num)              { _num_records += num; }

	virtual int Estimate_Records (void);

	int  Record_Number (void)               { return (_record_num); }
	virtual bool Record_Number (int number);

	int  Max_Record_Number (void)           { return (_max_record_num); }

	void Flush (void)                       { _file.flush (); }

	void Clean_Flag (bool flag)             { _clean_flag = flag; }

	void Lock (void)                        { file_lock.lock (); }
	void UnLock (void)                      { file_lock.unlock (); }

protected:

	virtual bool Db_Open (string filename);
	virtual bool Input_Record (void)        { return (true); }
	virtual bool Output_Record (void)       { return (true); }
	
	bool  Binary_Read (void *buffer, int num_records = 1);
	bool  Binary_Write (void *buffer, int num_records = 1);

	bool _first_open, _clean_flag;
	int _parent_id;

private:
	fstream _file;
	mutex  file_lock;

	Access_Type _file_access;
	Format_Type _file_format;
	Format_Type _model_format;

	int  _num_records, _record_num, _max_record_num, _version;
	size_t _first_offset;

	String _filename, _rootname;
};
#endif
