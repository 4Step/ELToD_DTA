//*********************************************************
//	Db_Field.hpp - database field
//*********************************************************

#ifndef DB_FIELD_HPP
#define DB_FIELD_HPP

#include "System_Defines.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Db_Field Class definition
//---------------------------------------------------------

class Db_Field
{
public:
	Db_Field (void);
	Db_Field (const char *name, Field_Type type, int offset = 0, double size = 0) 
	{
		Name (name); Type (type); Offset (offset); Size (size);
	}
	Db_Field (string name, Field_Type type, int offset = 0, double size = 0) 
	{
		Name (name); Type (type); Offset (offset); Size (size);
	}
	bool Set_Field (const char *name, Field_Type type, int offset = 0, double size = 0);
	bool Set_Field (string name, Field_Type type, int offset = 0, double size = 0);

	String &   Name (void)                 { return (_name); }
	Field_Type Type (void)                 { return (_type); }
	int        Offset (void)               { return (_offset); }
	double     Size (void);
	int        Width (void)                { return (_width); }
	int        Decimal (void)              { return (_decimal); }
	String &   Buffer (void)               { return (_buffer); }

	void  Name (const char *name)          { _name = name; }
	void  Name (string name)               { _name = name; }
	void  Type (Field_Type value)          { _type = value; }
	void  Offset (int value)               { _offset = (value > 0) ? value : 0; }
	void  Size (double value);
	void  Width (int value)                { _width = (short) value;	}
	void  Decimal (int value)              { _decimal = (short) value; }
	void  Buffer (const char *buffer)      { _buffer = buffer; }
	void  Buffer (string buffer)           { _buffer = buffer; }

	void  Clear (void)                     { _name.clear (); _buffer.clear (); _offset = 0; _width = _decimal = 0; }

private:
	String      _name;
	Field_Type  _type;
	int         _offset;
	short       _width;
	short       _decimal;
	String      _buffer;
};
#endif
