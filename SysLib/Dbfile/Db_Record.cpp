//********************************************************* 
//	Db_Record.cpp - database record
//*********************************************************

#include "Db_Record.hpp"

#include "System_Defines.hpp"

#include <stdio.h>
#include <string.h>

//-----------------------------------------------------------
//	Db_Record constructor
//-----------------------------------------------------------

Db_Record::Db_Record (Format_Type format) : Db_Status ()
{
	_delimiters = 0;
	
	Record_Format (format);
}

//-----------------------------------------------------------
//	Record_Format
//-----------------------------------------------------------

void Db_Record::Record_Format (Format_Type format)
{
	if (format == DEFAULT_FORMAT && exe) {
		format = (Format_Type) exe->Default_Format ();
	}
	_record_format = format;

	switch (_record_format) {
		case DBASE:
		case ARCVIEW:
			_record_format = FIXED_COLUMN;
			break;
		case COMMA_DELIMITED:
			_delimiters = COMMA_DELIMITERS;
			break;
		case SPACE_DELIMITED:
			_delimiters = SPACE_DELIMITERS;
			break;
		case VERSION3:
			_record_format = TAB_DELIMITED;
		case TAB_DELIMITED:
			_delimiters = TAB_DELIMITERS;
			break;
		case BINARY:
		case SQLITE3:
		case MATRIX:
		case STRUCTURE:
			_record_format = BINARY;
			break;
		default:	//---- CSV ----	
			_delimiters = CSV_DELIMITERS;
			break;
	}
	Reset_Buffers ();
}

//-----------------------------------------------------------
//	Reset_Buffers
//-----------------------------------------------------------

void Db_Record::Reset_Buffers (void)
{
	if (_record_format == BINARY || _record_format == FIXED_COLUMN) {
		_record.Max_Size (0);
	} else {
		_record.Max_Size (4094);
	}
}

//-----------------------------------------------------------
//	Record
//-----------------------------------------------------------

bool Db_Record::Record (void *record, int size)
{
	Buffer &buf = Record ();

	if (String_Flag ()) {
		if (size <= 0) {
			size = (int) strlen ((char *) record);
			if (size > buf.Max_Size ()) {
				size = buf.Max_Size ();
			}
		}
	} else {
		if (size <= 0) size = buf.Max_Size ();
	}
	return (buf.Data (record, size));
}

//-----------------------------------------------------------
//	Reset_Record
//-----------------------------------------------------------

Buffer & Db_Record::Reset_Record (void)
{
	char fill = (_record_format == FIXED_COLUMN) ? ' ' : '\0';

	Buffer &buf = Record ();

	buf.Fill (fill);
	return (buf);
}

//----------------------------------------------------------
//	Get_Field_Number
//----------------------------------------------------------

string Db_Record::Get_Field_Number (int number)
{
	String field, buffer;

	if (!Record ().OK ()) {
		Status (RECORD_SIZE);
		return (field);
	}
	buffer = Record_String ();

	for (int i=0; i < number; i++) {
		if (!buffer.Split (field, Delimiters ())) break;
	}
	return (field);
}

//----------------------------------------------------------
//	Set_Field_Number
//----------------------------------------------------------

bool Db_Record::Set_Field_Number (int number, string text)
{
	Buffer &rec = Record ();

	if (!rec.OK ()) return (Status (RECORD_SIZE));

	int i, max_num, num;

	Strings fields;
	String buffer = rec.String ();

	String_Ptr (text)->Trim ();

	num = buffer.Parse (fields, _delimiters);

	max_num = MAX (num, number);
	number--;

	buffer.clear ();

	for (i=0; i < max_num; i++) {
		if (i > 0) buffer += *_delimiters;
		if (*_delimiters == ' ') {
			buffer += '\"';
		}
		if (i == number) {
			buffer += text;
		} else if (i < num) {
			buffer += fields [i];
		}
		if (*_delimiters == ' ') {
			buffer += '\"';
		}
	}
	return (Record ((void *) buffer.data (), (int) buffer.size ()));
}
