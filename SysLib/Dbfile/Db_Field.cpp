//********************************************************* 
//	Db_Field.cpp - database field
//*********************************************************

#include "Db_Field.hpp"

#include "Execution_Service.hpp"

//-----------------------------------------------------------
//	Db_Field constructor
//-----------------------------------------------------------

Db_Field::Db_Field (void)
{
	_offset = 0;
	_width = _decimal = 0;
	_type = DB_INTEGER;
}

//-----------------------------------------------------------
//	Set_Field
//-----------------------------------------------------------

bool Db_Field::Set_Field (const char *name, Field_Type type, int offset, double size) 
{
	if (*name == '\0') return (false);
	Name (name);
	Type (type);
	Offset (offset);
	Size (size);
	return (Size () > 0);
}

bool Db_Field::Set_Field (string name, Field_Type type, int offset, double size) 
{
	if (name.empty ()) return (false);
	Name (name);
	Type (type);
	Offset (offset);
	Size (size);
	return (Size () > 0);
}

//-----------------------------------------------------------
//	Size
//-----------------------------------------------------------

double Db_Field::Size (void)
{
	if (_decimal > 9) {
		return ((double) _width + (_decimal / 100.0));
	} else {
		return ((double) _width + (_decimal / 10.0));
	}
}

void  Db_Field::Size (double size)
{
	if (size > 0) {
		_width = (short) size;
		_decimal = (short) ((size - _width) * 100.0 + 0.5);
		if (_decimal > 0 && (_decimal % 10) == 0) {
			_decimal /= 10;
		}
	} else {
		_width = _decimal = 0;
	}
}

