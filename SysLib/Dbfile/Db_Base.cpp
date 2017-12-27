//********************************************************* 
//	Db_Base.cpp - database record
//*********************************************************

#include "Db_Base.hpp"

#include "String.hpp"

#include <stdio.h>
#include <string.h>
#include <math.h>

//-----------------------------------------------------------
//	Db_Base constructor
//-----------------------------------------------------------

Db_Base::Db_Base (Access_Type access, string format) : Db_File ()
{
	Initialize (access, format);
}

Db_Base::Db_Base (string filename, Access_Type access, string format) : Db_File ()
{
	Initialize (access, format);

	Open (filename);
}

Db_Base::Db_Base (Access_Type access, Format_Type format) : Db_File ()
{
	Initialize (access, format);
}

Db_Base::Db_Base (string filename, Access_Type access, Format_Type format) : Db_File ()
{
	Initialize (access, format);

	Open (filename);
}

//-----------------------------------------------------------
//	Db_Base destructor
//-----------------------------------------------------------

Db_Base::~Db_Base (void)
{
	Clear_Fields ();
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Db_Base::Initialize (Access_Type access, string format) 
{
	Format_Type model_format, file_format;
	Code_Flag (Data_Format (format, file_format, model_format));

	Model_Format (model_format);
	Dbase_Format (file_format);

	Db_File::Initialize (access, file_format);
}

void Db_Base::Initialize (Access_Type access, Format_Type format) 
{
	Model_Format (TRANSIMS);
	Dbase_Format (format);
	Code_Flag (format == BINARY || format == SQLITE3);

	Db_File::Initialize (access, format);
}

//---------------------------------------------------------
//	Dbase_Format
//---------------------------------------------------------

void Db_Base::Dbase_Format (Format_Type type) 
{
	if (type == DEFAULT_FORMAT && exe) {
		type = (Format_Type) exe->Default_Format ();
	}
	_format_type = type;
	File_Format (type);
}

void  Db_Base::Dbase_Format (string text)
{
	Format_Type file_format, model_format;

	Code_Flag (Data_Format (text, file_format, model_format));

	Model_Format (model_format);
	Dbase_Format (file_format);
}

//-----------------------------------------------------------
//	Add_Field
//-----------------------------------------------------------

int Db_Base::Add_Field (const char *name, Field_Type type, double size, bool binary, int offset) 
{
	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	if (_field.capacity () == 0) {
		_field.reserve (10);
	}
	int length, num;
	Field_Itr itr;

	switch (Dbase_Format ()) {
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
			num = 1;
			length = 0;

			for (itr = _field.begin (); itr != _field.end (); itr++) {
				length += itr->Width () + 1;
				num++;
			}
			if (binary) {
				Binary_Text_Size (type, &size);
			}
			length += (int) size;
			if (offset <= 0) offset = num;
			break;
		case UNFORMATED:
		case BINARY:
		case FIXED_COLUMN:
		case DBASE:
		default:
			if (offset < 0) {
				offset = (Dbase_Format () == DBASE || Dbase_Format () == ARCVIEW) ? 1 : 0;

				for (itr = _field.begin (); itr != _field.end (); itr++) {
					length = itr->Offset () + itr->Width ();
					if (offset < length) offset = length;
				}
			}
			if (binary) {
				if (Record_Format () != BINARY) {
					Binary_Text_Size (type, &size);
				}
			} else if (Record_Format () == BINARY) {
				Text_Binary_Size (type, &size);
			}
			length = offset + (int) size;
			break;
	}

	//---- convert to internal units ----

	Db_Field fld (name, type, offset, size);

	num = (int) _field.size ();
	_field.push_back (fld);

	if (!Record_Size (length)) return (-1);
	Reset_Record ();

	return (num);
}

//---------------------------------------------------------
//	Required_Field
//---------------------------------------------------------

int Db_Base::Required_Field (string name)
{
	int num = Field_Number (name);

	if (num < 0 && exe->Send_Messages ()) {
		exe->Error (String ("%s does not include \"%s\" field") % File_Type () % name);
	}
	return (num);
}

//---------------------------------------------------------
//	Optional_Field
//---------------------------------------------------------

int Db_Base::Optional_Field (string name) 
{
	return (Field_Number (name));
}

//---------------------------------------------------------
//	Required_Field
//---------------------------------------------------------

int Db_Base::Required_Field (const char *name1, const char *name2, const char *name3, const char *name4, const char *name5)
{
	int num = Optional_Field (name1, name2, name3, name4, name5);

	if (num < 0 && exe->Send_Messages ()) {
		if (name5) {
			exe->Error (String ("%s does not include field:\n\t\t\"%s\", \"%s\", \"%s\", \"%s\" or \"%s\"") % 
						File_Type () % name1 % name2 % name3 % name4 % name5);
		} else if (name4) {
			exe->Error (String ("%s does not include field:\n\t\t\"%s\", \"%s\", \"%s\" or \"%s\"") % 
						File_Type () % name1 % name2 % name3 % name4);
		} else if (name3) {
			exe->Error (String ("%s does not include field:\n\t\t\"%s\", \"%s\" or \"%s\"") % 
						File_Type () % name1 % name2 % name3);
		} else if (name2) {
			exe->Error (String ("%s does not include \"%s\" or \"%s\" field") % File_Type () % name1 % name2);
		} else if (name1) {
			exe->Error (String ("%s does not include \"%s\" field") % File_Type () % name1);
		} else {
			exe->Error (String ("%s Required Field Name is NULL") % File_Type ());
		}
	}
	return (num);
}

//---------------------------------------------------------
//	Optional_Field
//---------------------------------------------------------

int Db_Base::Optional_Field (const char *name1, const char *name2, const char *name3, const char *name4, const char *name5)
{
	if (!name1) {
		Status (NULL_NAME);
		return (0);
	}
	int num = Field_Number (name1);

	if (num < 0 && name2) {
		num = Field_Number (name2);

		if (num < 0 && name3) {
			num = Field_Number (name3);

			if (num < 0 && name4) {
				num = Field_Number (name4);

				if (num < 0 && name5) {
					num = Field_Number (name5);
				}
			}
		}
	}
	return (num);
}

//-----------------------------------------------------------
//	Field - by name
//-----------------------------------------------------------

Field_Ptr Db_Base::Field (int fld)
{
	if (fld >= 0 && fld < (int) _field.size ()) {
		return (&_field [fld]); 
	} else {
		return (0);
	}
}

Field_Ptr Db_Base::Field (const char *name) 
{
	if (name) {
		int num = Field_Number (name);
		if (num >= 0) {
			return (&_field [num]);
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Field_Number
//---------------------------------------------------------

int Db_Base::Field_Number (const char *name)
{
	if (name) {
		Field_Itr itr, itr0;

		if (*name == '[') {
			string sname = name;
			sname = sname.substr (1, sname.length ()-2);

			for (itr = itr0 = _field.begin (); itr != _field.end (); itr++) {
				if (itr->Name ().Equals (sname)) {
					return ((int) (itr - itr0));
				}
			}
		} else {
			for (itr = itr0 = _field.begin (); itr != _field.end (); itr++) {
				if (itr->Name ().Equals (name)) {
					return ((int) (itr - itr0));
				}
			}
		}
	}
	return (-1);
}

//----------------------------------------------------------
//	Read_Field
//----------------------------------------------------------

bool Db_Base::Read_Field (Db_Field *fld, void *data, Field_Type type)
{
	if (data == 0) return (Status (NULL_POINTER));
	
	//---- initialize the data field ----

	switch (type) {
		case DB_INTEGER:
			*((int *) data) = 0;
			break;
		case DB_DOUBLE:
			*((double *) data) = 0.0;
			break;
		case DB_STRING:
			((string *) data)->clear ();
			break;
			break;
		default:
			return (Status (DB_ERROR));
	}
	if (fld == 0) return (Status (NO_FIELD, false));

	char *field = Record_Pointer ();

	if (field == 0) return (Status (RECORD_SIZE));

	Field_Type fld_type;
	bool asc_flag = true;
	int len, lvalue = 0;
	double dvalue = 0.0;
	String svalue;

	int size = fld->Width ();
	int offset = fld->Offset ();

	switch (Record_Format ()) {
		case BINARY:
			if (fld->Type () != DB_STRING) {
				field += offset;
				asc_flag = false;
				break;
			} else {
				len = (int) strlen (field + offset);
				if (len < size) size = len;
			}
		case FIXED_COLUMN:
			svalue.assign (field + offset, size);
			svalue.Clean ();
			break;
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
			svalue (fld->Buffer ());
			size = (int) svalue.size ();
			break;
		default:
			svalue  = Get_Field_Number (offset);
			size = (int) svalue.size ();
			break;
	}

	//---- read the field from the current record ----

	switch (fld->Type ()) {
		default:
			return (Status (DB_ERROR));

		case DB_INTEGER:
			if (asc_flag) {
				lvalue = svalue.Integer ();
			} else if (size == sizeof (int)) {
				lvalue = *((int *) field);
			} else if (size == sizeof (short)) {
				lvalue = *((short *) field);
			} else if (size == sizeof (char)) {
				lvalue = *((char *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DB_INTEGER;
			break;
			
		case DB_UNSIGNED:
			if (asc_flag) {
				lvalue = svalue.Integer ();
			} else if (size == sizeof (int)) {
				lvalue = *((unsigned int *) field);
			} else if (size == sizeof (short)) {
				lvalue = *((unsigned short *) field);
			} else if (size == sizeof (char)) {
				lvalue = *((unsigned char *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DB_INTEGER;
			break;
			
		case DB_DOUBLE:
			if (asc_flag) {
				dvalue = svalue.Double ();
			} else if (size == sizeof (double)) {
				dvalue = *((double *) field);
			} else if (size == sizeof (float)) {
				dvalue = *((float *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DB_DOUBLE;
			break;
			
		case DB_FIXED:
			if (asc_flag) {
				dvalue = svalue.Double ();
				if (svalue.find ('.') == 0) {
					dvalue = dvalue / pow (10.0, fld->Decimal ());
				}
			} else {
				if (size == sizeof (int)) {
					lvalue = *((int *) field);
				} else if (size == sizeof (short)) {
					lvalue = *((short *) field);
				} else if (size == sizeof (char)) {
					lvalue = *((char *) field);
				} else {
					return (Status (FIELD_BYTES));
				}
				dvalue = (double) lvalue / pow (10.0, fld->Decimal ());
			}
			fld_type = DB_DOUBLE;
			break;
			
		case DB_STRING:
			fld_type = DB_STRING;
			break;

		case DB_CHAR:
			if (asc_flag) {
				if (svalue.empty ()) {
					svalue = " ";
				}
			} else {
				if (*field == '\0') {
					svalue = " ";
				} else {
					svalue.insert (0, field, 1);
				}
			}
			fld_type = DB_STRING;
			break;
	}

	//---- convert to the output data type ----

	switch (type) {
		case DB_INTEGER:
			switch (fld_type) {
				case DB_INTEGER:
					*((int *) data) = lvalue;
					break;
				case DB_DOUBLE:
					*((int *) data) = DTOI (dvalue);
					break;
				case DB_STRING:
					*((int *) data) = svalue.Integer ();
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_DOUBLE:
			switch (fld_type) {
				case DB_INTEGER:
					*((double *) data) = lvalue;
					break;
				case DB_DOUBLE:
					*((double *) data) = dvalue;
					break;
				case DB_STRING:
					*((double *) data) = svalue.Double ();
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_STRING:
			if (fld_type == DB_INTEGER) {
				svalue (lvalue);
			} else if (fld_type == DB_DOUBLE) {
				svalue (dvalue, fld->Decimal ());
			}
			if (fld_type == DB_STRING) {
				len = (int) svalue.size ();
				if (len > 0) {
					if (len > fld->Width ()) len = fld->Width ();

					*((string *) data) = svalue.substr (0, len);
				} else {
					(*((string *) data)).clear ();
				}
			} else {
				*((string *) data) = svalue;
			}
			break;

		default:
			return (Status (DB_ERROR));
	}
	return (true);
}

//----------------------------------------------------------
//	Write_Field
//----------------------------------------------------------

bool Db_Base::Write_Field (Db_Field *fld, void *data, Field_Type type)
{
	if (data == 0) return (Status (NULL_POINTER));
	if (fld == 0) return (Status (NO_FIELD, false));
	if (!Record ().OK ()) return (Status (RECORD_SIZE));

	int lvalue = 0;
	unsigned uvalue = 0;
	double dvalue = 0.0;
	String svalue;
	Field_Type fld_type;

	//---- convert the input data type to generic variables ----

	fld_type = fld->Type ();

	switch (fld_type) {
		default:
			return (Status (DB_ERROR));
			
		case DB_INTEGER:
			switch (type) {
				case DB_INTEGER:
					lvalue = *((int *) data);
					break;
				case DB_DOUBLE:
					dvalue = *((double *) data);
					if (dvalue > MAX_INTEGER) {
						lvalue = MAX_INTEGER;
					} else if (dvalue < -MAX_INTEGER) {
						lvalue = -MAX_INTEGER;
					} else {
						lvalue = DTOI (dvalue);
					}
					break;
				case DB_STRING:
					lvalue = ((String *) data)->Integer ();
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_UNSIGNED:
			switch (type) {
				case DB_INTEGER:
					uvalue = *((unsigned *) data);
					break;
				case DB_DOUBLE:
					dvalue = *((double *) data);
					if (dvalue > 2.0 * MAX_INTEGER) {
						uvalue = MAX_INTEGER;
						uvalue *= 2;
					} else if (dvalue < 0.0) {
						uvalue = 0;
					} else {
						uvalue = (unsigned) (dvalue + 0.5);
					}
					break;
				case DB_STRING:
					uvalue = ((String *) data)->Integer ();
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_FIXED:
		case DB_DOUBLE:
			switch (type) {
				case DB_INTEGER:
					dvalue = (double) *((int *) data);
					break;
				case DB_DOUBLE:
					dvalue = *((double *) data);
					break;
				case DB_STRING:
					dvalue = ((String *) data)->Double ();
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;
		
		case DB_STRING:
		case DB_CHAR:
			switch (type) {
				case DB_INTEGER:
					svalue (*((int *) data));
					break;
				case DB_DOUBLE:
					svalue (*((double *) data), fld->Decimal ());
					break;
				case DB_STRING:
					svalue = *((string *) data);
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;
	}

	//---- place the data onto the data record -----

	char *field = Record_Pointer ();

	bool asc_flag = false;
	bool justify = false;
	int len, position;
	int size = fld->Width ();
	int offset = fld->Offset ();

	switch (Record_Format ()) {
		case BINARY:
			field += offset;
			asc_flag = false;
			break;
		case FIXED_COLUMN:
			field += offset;
			memset (field, ' ', size);
			asc_flag = justify = true;
			break;
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
		default:		
			asc_flag = true;
			justify = false;
			break;
	}

	switch (fld_type) {
		default:
			return (Status (DB_ERROR));
			
		case DB_INTEGER:
			if (asc_flag) {
				svalue (lvalue);

				if (justify) {
					len = (int) svalue.size ();
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue.c_str (), len);
				}
			} else if (size == sizeof (int)) {
				*((int *) field) = lvalue;
			} else if (size == sizeof (short)) {
				if (lvalue > 32767) lvalue = 32767;
				*((short *) field) = (short) lvalue;
			} else if (size == sizeof (char)) {
				if (lvalue > 127) lvalue = 127;
				*((char *) field) = (char) lvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;
			
		case DB_UNSIGNED:
			if (asc_flag) {
				svalue ((size_t) uvalue);

				if (justify) {
					len = (int) svalue.size ();
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue.c_str (), len);
				}
			} else if (size == sizeof (int)) {
				*((unsigned int *) field) = (unsigned int) uvalue;
			} else if (size == sizeof (short)) {
				if (uvalue > 65535) uvalue = 65535;
				*((unsigned short *) field) = (unsigned short) uvalue;
			} else if (size == sizeof (char)) {
				if (uvalue > 255) uvalue = 255;
				*((unsigned char *) field) = (unsigned char) uvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;

		case DB_DOUBLE:
			if (asc_flag) {
				svalue (dvalue, fld->Decimal ());
				if (justify) {
					len = (int) svalue.size ();
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue.c_str (), len);
				}
			} else if (size == sizeof (double)) {
				*((double *) field) = dvalue;
			} else if (size == sizeof (float)) {
				*((float *) field) = (float) dvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;

		case DB_FIXED:
			if (asc_flag) {
				svalue (dvalue, fld->Decimal ());

				if (justify) {
					len = (int) svalue.size ();
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue.c_str (), len);
				}
			} else {
				if (dvalue < 0.0) {
					lvalue = (int) (dvalue * pow (10.0, fld->Decimal ()) - 0.5);
				} else {
					lvalue = (int) (dvalue * pow (10.0, fld->Decimal ()) + 0.5);
				}

				if (size == sizeof (int)) {
					*((int *) field) = lvalue;
				} else if (size == sizeof (short)) {
					*((short *) field) = (short) lvalue;
				} else if (size == sizeof (char)) {
					*((char *) field) = (char) lvalue;
				} else {
					return (Status (FIELD_BYTES));
				}
			}
			break;

		case DB_STRING:
			if (asc_flag) {
				if (justify) {
					len = (int) svalue.size ();
					if (len > size) len = size;
					memcpy (field, svalue.c_str (), len);
				}
			} else {
				memset (field, '\0', size);
				memcpy (field, svalue.c_str (), MIN ((int) svalue.size (), size));
			}
			break;
			
		case DB_CHAR:
			if (!asc_flag || justify) {
				memset (field, '\0', size);
				field [0] = svalue [0];
			}
			break;
	}
	if (asc_flag && !justify) {
		if (Record_Format () == UNFORMATED) {
			return (Set_Field_Number (offset, svalue));
		} else {
			fld->Buffer (svalue);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Blank_Field
//-----------------------------------------------------------

void Db_Base::Blank_Field (Db_Field *fld)
{
	if (fld == 0) return;

	if (Record_Format () == BINARY || Record_Format () == FIXED_COLUMN) {
		char *field = Record_Pointer ();

		field += fld->Offset ();
		memset (field, ' ', fld->Width ());
	} else {
		fld->Buffer ().erase ();
	}
}

//---------------------------------------------------------
//	Copy_Fields
//---------------------------------------------------------

bool Db_Base::Copy_Fields (Db_Base &input)
{
	Field_Ptr in_fld, out_fld;

	int lvalue;
	double dvalue;
	string svalue;

	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	int num = input.Num_Fields ();

	for (int i=0; i < num; i++) {
		in_fld = input.Field (i);

		if (in_fld != 0) {
			out_fld = Field (in_fld->Name ());

			if (out_fld != 0) {
				switch (in_fld->Type ()) {
					case DB_INTEGER:
					case DB_UNSIGNED:
					case DB_SHORT:
						input.Read_Field (in_fld, (void *) &lvalue, DB_INTEGER);
						if (!Write_Field (out_fld, (void *) &lvalue, DB_INTEGER)) return (false);
						break;
					case DB_DOUBLE:
					case DB_FIXED:
					case DB_FLOAT:
						input.Read_Field (in_fld, (void *) &dvalue, DB_DOUBLE);
						if (!Write_Field (out_fld, (void *) &dvalue, DB_DOUBLE)) return (false);
						break;
					case DB_STRING:
					case DB_CHAR:
						input.Read_Field (in_fld, (void *) &svalue, DB_STRING);
						if (!Write_Field (out_fld, (void *) &svalue, DB_STRING)) return (false);
						break;
				}
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Add_New_Fields
//---------------------------------------------------------

bool Db_Base::Add_New_Fields (Db_Base *file)
{
	int i, j, offset, nfield;
	bool convert, binary, skip;
	Field_Ptr fld;

	if (file == 0) return (false);
	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	convert = (file->Dbase_Format () != Dbase_Format () || Num_Fields () > 0);
	binary = (file->Record_Format () == BINARY);

	offset = END_OF_RECORD;
	nfield = file->Num_Fields ();

	for (i = 0; i < nfield; i++) {
		fld = file->Field (i);
		if (fld == 0) continue;

		if (!convert) {
			offset = fld->Offset ();
		}
		for (j = 0, skip=false; j < Num_Fields (); j++) {
			if (fld->Name ().Equals (Field (j)->Name ())) {
				skip = true;
				break;
			}
		}
		if (!skip) Add_Field (fld->Name (), fld->Type (), fld->Size (), binary, offset);
	}
	return (Write_Fields ());
}

//---------------------------------------------------------
//	Replicate_Fields
//---------------------------------------------------------

bool Db_Base::Replicate_Fields (Db_Base *file, bool notes_flag) 
{
	int i, offset, nfield;
	bool convert, binary;
	Field_Ptr fld;

	if (file == 0) return (false);
	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	convert = (file->Dbase_Format () != Dbase_Format () || Num_Fields () > 0);
	binary = (file->Record_Format () == BINARY);

	offset = END_OF_RECORD;
	nfield = file->Num_Fields ();

	for (i=0; i < nfield; i++) {
		fld = file->Field (i);
		if (fld == 0) continue;

		if (!convert) {
			offset = fld->Offset ();
		}
		if (notes_flag == false) {
			if (fld->Name ().Equals ("NOTES")) continue;
		}
		Add_Field (fld->Name (), fld->Type (), fld->Size (), binary, offset);
	}
	return (Write_Fields ());
}

//-----------------------------------------------------------
//	Input_Record
//-----------------------------------------------------------

bool Db_Base::Input_Record (void)
{
	if (File_Format () == CSV_DELIMITED) {
		Strings fields;
		Db_Field *fld;

		int count = String (Record_Pointer ()).Parse (fields, Delimiters ());

		for (int i=0; i < Num_Fields (); i++) {
			fld = Field (i);
			if (fld->Offset () <= count) {
				fld->Buffer (fields [fld->Offset () - 1]);
			} else {
				fld->Buffer ("");
			}
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Output_Record
//-----------------------------------------------------------

bool Db_Base::Output_Record (void)
{
	if (File_Format () == CSV_DELIMITED) {
		int i, j, max;
		string *ptr;
		char *delimiter, *record;
		bool first = true;
		Db_Field *fld;

		delimiter = Delimiters ();
		record = Record_Pointer ();

		for (i=0; i < Num_Fields (); i++) {
			fld = Field (i);
			ptr = &fld->Buffer ();
			max = (int) ptr->size ();
			if (max > fld->Width ()) max = fld->Width ();

			if (first) {
				first = false;
			} else {
				*record++ = *delimiter;
			}
			for (j=0; j < max; j++) {
				*record++ = (*ptr) [j];
			}
			*record = '\0';
		}
		Set_Size ();
	}
	return (true);
}

//-----------------------------------------------------------
//	Reset_Record
//-----------------------------------------------------------

string Db_Base::Reset_Record (void)
{
	if (File_Format () == CSV_DELIMITED) {
		Field_Itr itr;

		for (itr = _field.begin (); itr != _field.end (); itr++) {
			itr->Buffer ().erase ();
		}
	}
	return (Db_Record::Reset_Record ().String ());
}

//---------------------------------------------------------
//	Binary_Text_Size
//---------------------------------------------------------

void Db_Base::Binary_Text_Size (Field_Type type, double *size) 
{
	size_t length = (size_t) *size;
	int decimal = DTOI ((*size - length) * 10.0);

	switch (type) {
		case DB_INTEGER:
		case DB_UNSIGNED:
		case DB_SHORT:
			if (length == sizeof (char)) {
				*size = 3;
			} else if (length == sizeof (short)) {
				*size = 5;
			} else {
				*size = 10;
			}
			break;
		case DB_DOUBLE:
		case DB_FLOAT:
			if (decimal == 0) {
				if (length == sizeof (float)) {
					*size = 8.2;
				} else {
					*size = 12.4;
				}
			} else {
				if (length == sizeof (float)) {
					*size = ((int) ((decimal + 7) * 10 + decimal)) / 10.0;
				} else {
					*size = ((int) ((decimal + 10) * 10 + decimal)) / 10.0;
				}
			}
			break;
		case DB_FIXED:
			if (decimal == 0) {
				if (length == sizeof (char)) {
					*size = 4.1;
				} else if (length == sizeof (short)) {
					*size = 6.1;
				} else {
					*size = 10.1;
				}
			} else {
				if (length == sizeof (char)) {
					*size = 4 + decimal / 10.0;
				} else if (length == sizeof (short)) {
					*size = 6 + decimal / 10.0;
				} else {
					*size = 10 + decimal / 10.0;
				}
			}
			break;
		case DB_STRING:
		case DB_CHAR:
			break;
	}
}

//---------------------------------------------------------
//	Text_Binary_Size
//---------------------------------------------------------

void Db_Base::Text_Binary_Size (Field_Type type, double *size) 
{
	int length = (int) *size;
	double decimal = *size - length;

	switch (type) {
		case DB_INTEGER:
		case DB_UNSIGNED:
			if (length < 3) {
				*size = sizeof (char);
			} else if (length < 5) {
				*size = sizeof (short);
			} else {
				*size = sizeof (int);
			}
			break;
		case DB_SHORT:
			if (length < 3) {
				*size = sizeof (char);
			} else {
				*size = sizeof (short);
			}
			break;
		case DB_FLOAT:
			*size = sizeof (float);
			break;
		case DB_DOUBLE:
			if (length < 9) {
				*size = sizeof (float);
			} else {
				*size = sizeof (double);
			}
			break;
		case DB_FIXED:
			if (length < 4) {
				*size = sizeof (char);
			} else if (length < 6) {
				*size = sizeof (short);
			} else {
				*size = sizeof (int);
			}
			break;
		case DB_STRING:
		case DB_CHAR:
			break;
	}
	*size += decimal;
}