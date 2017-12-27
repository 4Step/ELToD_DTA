//********************************************************* 
//	Db_Header.cpp - general database interface
//*********************************************************

#include "Db_Header.hpp"

#include "String.hpp"

//-----------------------------------------------------------
//	Db_Header constructors
//-----------------------------------------------------------

Db_Header::Db_Header (Access_Type access, string format) : Db_Base ()
{
	Initialize (access, format);
}

Db_Header::Db_Header (string filename, Access_Type access, string format) : Db_Base ()
{
	Initialize (access, format);

	Open (filename);
}

Db_Header::Db_Header (Access_Type access, Format_Type format) : Db_Base ()
{
	Initialize (access, format);
}

Db_Header::Db_Header (string filename, Access_Type access, Format_Type format) : Db_Base ()
{
	Initialize (access, format);

	Open (filename);
}

//---------------------------------------------------------
//	Db_Header destructor
//---------------------------------------------------------

Db_Header::~Db_Header (void)
{
	Close ();
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Db_Header::Initialize (Access_Type access, string format) 
{
	Header_Lines (1);
	Scan_Lines ();
	Custom_Header (false);
	_notes = -1;

	Db_Base::Initialize (access, format);
}

void Db_Header::Initialize (Access_Type access, Format_Type format) 
{
	Header_Lines (1);
	Scan_Lines ();
	Custom_Header (false);
	_notes = -1;

	Db_Base::Initialize (access, format);
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Db_Header::Open (string filename)
{
	bool stat;
	Format_Type format_type;

	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	if (!filename.empty ()) {
		Filename (filename);
	}

	//---- read the definition file if it exists ----

	filename = Filename ();

	if (!filename.empty () && File_Access () != CREATE && (File_Format () != UNFORMATED || Model_Format () == TRANSCAD)) {
		stat = Read_Def_Header ();
	} else {
		stat = false;
	}

	//---- open the file ----

	if (Db_File::Open ()) {
		if (File_Access () != CREATE && File_Format () != UNFORMATED) {

			//---- process the header ----

			if (!stat && Header_Lines () == 0 && Num_Fields () != 0) {
				stat = Write_Def_Header ("");
			}
			if (!Read_Header (stat)) {
				return (Status (OPEN_DEF));
			}
		} else {
			if (!Custom_Header () || Num_Fields () == 0) {
				Create_Fields ();
			}
			if (Num_Fields () > 0) {
				Write_Header ();
			}
		}
		format_type = Dbase_Format ();

		if (format_type != exe->Default_Format () && First_Open () && exe->Send_Messages ()) {
			String text, result, nesting;
			text = File_Type ();

			for (;;) {
				text.Split_Last (result);
				if (!result.Starts_With ("#")) break;
				if (!nesting.empty ()) {
					result += " " + nesting;
				}
				nesting = result;
			}
			if (nesting.empty ()) {
				exe->Print (1, text) << " Format = " << Data_Format (format_type, Model_Format (), Code_Flag ());
			} else {
				exe->Print (1, text) << " Format " << nesting << " = " << Data_Format (format_type, Model_Format (), Code_Flag ());
			}
		}
		return (true);
	}
	return (false);
}

bool Db_Header::Open (int index) 
{
	if (Num_Fields () > 0) Clear_Fields ();

	return (Db_File::Open (index));
}

//-----------------------------------------------------------
//	Close
//-----------------------------------------------------------

bool Db_Header::Close (void)
{
	if (!Is_Open ()) return (Db_File::Close ());

	if ((Dbase_Format () == DBASE || Dbase_Format () == ARCVIEW) && File_Access () != READ) {
		int records = Max_Record_Number ();
		if (records < Num_Records ()) {
			records = Num_Records ();
		}
		if (records > 0) {
			dBase_Header header;
			if (!Read (&header, sizeof (header), 0L)) {
				return (Status (FILE_HEADER));
			}
			//---- update the database header ----

			if (header.num_records < records) {
				header.num_records = records;
				if (!Write (&header, sizeof (header), 0L)) {
					return (Status (HEADER_WRITE));
				}
			}
		}
		Reset_Buffers ();
	}
	return (Db_File::Close ());
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Db_Header::Read_Header (bool stat)
{
	Offset (0L);

	if (File_Format () == CSV_DELIMITED) {
		if (!Read_CSV_Header (stat)) {
			return (Status (FILE_HEADER));
		}
	} else if (Dbase_Format () == DBASE || Dbase_Format () == ARCVIEW) {
		if (!Read_dBase_Header (stat)) return (false);
	} else if (!stat) {
		return (Status (OPEN_DEF));
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Write_Header
//-----------------------------------------------------------

bool Db_Header::Write_Header (string user_lines) 
{
	Offset (0L);

	if (user_lines.empty ()) {
		user_lines = Header_Record ();
	}
	if (File_Format () == CSV_DELIMITED) {
		if (!Write_CSV_Header (user_lines)) return (false);
	} else if (Dbase_Format () == DBASE || Dbase_Format () == ARCVIEW) {
		if (!Write_dBase_Header ()) return (false);
	}
	if (!Write_Def_Header (user_lines)) return (false);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Read_CSV_Header
//-----------------------------------------------------------

bool Db_Header::Read_CSV_Header (bool stat)
{
	int i, j, k, len, nfield, decimal, exponent;
	String buffer, field;
	string::iterator str_itr;
	char *ptr, *p, ch;
	Db_Field *fld_ptr;
	Format_Type type;

	if (Header_Lines () == 0) return (stat);

	type = Dbase_Format ();
	File_Format (UNFORMATED);
	Record_Format (type);

	//---- read the database header ----

	len = 0;

	for (i=1; i <= Header_Lines (); i++) {
		if (!Read ()) return (false);
		len += Record_Size ();

		if (i < Header_Lines ()) {
			buffer += Record_String ();
		}
	}
	Header_Record (buffer);
	First_Offset (len);

	//---- validate the definition file ----

	if (stat) {
		nfield = 0;
		buffer = Record_String ();

		while (buffer.Split (field, Delimiters ())) {
			fld_ptr = Field (nfield++);
			if (field [0] == '"') {
				field = field.substr (1, field.length () - 2);
			}
			if (fld_ptr == 0 || !field.Equals (fld_ptr->Name ())) {
				exe->Write (1, String ("Field %d does not match %s != %s") % nfield % 
					((fld_ptr != 0) ? fld_ptr->Name () : "NULL") % field);
				nfield = 0;
				break;
			}
		}
		if (nfield != Num_Fields ()) {
			Clear_Fields ();
			stat = Default_Definition ();
		}
	}

	//---- if a definition file was read, skip the field scan ----

	if (!stat) {
		buffer = ptr = Record_Pointer();

		//---- refine the delimiter type ----

		if (Record_Format() == CSV_DELIMITED || Filename().Ends_With(".csv")) {

			//---- scan for tabs ----

			i = 0;
			p = ptr;

			while (p != 0) {
				p = strchr(p, '\t');
				if (p != 0) {
					i++;
					p++;
				}
			}

			//---- scan for commas ----

			j = 0;
			p = ptr;

			while (p != 0) {
				p = strchr(p, ',');
				if (p != 0) {
					j++;
					p++;
				}
			}

			//---- reset the delimiter ----

			if (i == 0 && j == 0) {
				Record_Format(type = SPACE_DELIMITED);
			}
			else if (i == 0 || j > i * 2) {
				Record_Format(type = COMMA_DELIMITED);
			}
			else if (j == 0 || i > j * 2) {
				Record_Format(type = TAB_DELIMITED);
			}
		}

		//---- process the header record ----

		nfield = 0;

		while (buffer.Split(field, Delimiters())) {
			nfield++;
			ch = field [0];
			if (ch == '"') {
				field = field.substr(1, field.length() - 2);
				ch = field [0];
			}
			if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
				if (Add_Field (field, DB_INTEGER, 1) < 0) return (false);
			} else {
				for (str_itr = field.begin(); str_itr != field.end(); str_itr++) {
					if ((*str_itr >= 'a' && *str_itr <= 'z') || (*str_itr >= 'A' && *str_itr <= 'Z') || *str_itr == '_') {
						if (Add_Field (field, DB_INTEGER, 1) < 0) return (false);
						break;
					}
				}
			}
		}
		if (nfield != Num_Fields()) {
			if (Num_Fields() == 0) return (Status(NO_FIELD));
			File_Format( type);
			return (false);
		}
		nfield = Num_Fields();

		//---- scan the file for field information ----

		for (k=0; k < _scan_lines || _scan_lines == SCAN_ALL_RECORDS; k++) {
			if (!Read ()) break;

			//---- read the data fields ----

			buffer = Record_String ();

			for (i=0; i < nfield; i++) {
				if (!buffer.Split (field, Delimiters ())) continue;

				//---- change NA to zero ----

				if (field.Equals ("NA")) {
					field = "0";
				}
				fld_ptr = Field (i);
				if (fld_ptr == 0) return (Status (NO_FIELD));

				//---- check the field size ----

				len = (int) field.size ();
				if (_scan_lines != SCAN_ALL_RECORDS && len < 10) {
					if (fld_ptr->Width () < 10) {
						fld_ptr->Size (10);
					}
				} else {
					if (fld_ptr->Width () < len) fld_ptr->Size (len);
				}

				//---- check for non-numbers and decimal points ----

				decimal = exponent = 0;

				for (j=0; j < len; j++) {
					ch = field [j];

					if (ch < '0' || ch > '9') {
						if (ch == '.' && decimal == 0) {
							decimal = len - j - 1;
							continue;
						}
						if (ch == '-') {
							if (j == 0) continue;
							if (exponent > 0 && j == exponent + 1) continue;
						}
						if (ch == 'E' && exponent == 0 && decimal != 0) {
							exponent = j;
							continue;
						}
						break;
					}
				}
				if (j < len) {
					fld_ptr->Type (DB_STRING);
				} else if (decimal) {
					fld_ptr->Type (DB_DOUBLE);
					if (decimal > fld_ptr->Decimal ()) {
						fld_ptr->Size ((double) fld_ptr->Width () + (decimal / 10.0));
					}
				}
			}
		}

		//---- set the number of records ----

		if (_scan_lines == SCAN_ALL_RECORDS) {
			Num_Records (k);
		}
	}

	//---- check the header alignment ----

	if (_scan_lines != 0) {
		Rewind ();
	}

	//---- restore the file format ----

	Dbase_Format (type);

	if (!stat) {
		Write_Def_Header (Header_Record ());
	}
	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Write_CSV_Header
//-----------------------------------------------------------

bool Db_Header::Write_CSV_Header (string user_lines) 
{
	int i, len, nfield, lines;
	char *name, *ptr, *nptr, delimiter;
	bool first;
	Db_Field *fld_ptr;

	if (Header_Lines () <= 0) return (true);

	fstream &fh = File ();
	if (!fh.is_open ()) return (Status (NOT_OPEN));

	//---- write extra header lines ----

	lines = 1;

	if (Header_Lines () > lines) {
		ptr = (char *) user_lines.c_str ();

		for (i=lines; i < Header_Lines (); i++) {
			if (ptr != 0) {
				nptr = strchr (ptr, '\n');

				if (nptr != 0) {
					len = (int) (nptr - ptr);
					fh << (String ("%*.*s") % len % len % ptr) << endl;
					ptr = nptr + 1;
				} else {
					fh << ptr << endl;
					ptr = 0;
				}
			} else {
				fh << endl;
			}
		}
	}
	nfield = Num_Fields ();
	if (nfield == 0) return (Status (NO_FIELD));

	//---- setup the output record ----

	delimiter = *(Delimiters ());

	//--- write each field name ----

	first = true;

	for (i=0; i < nfield; i++) {
		fld_ptr = Field (i);

		if (!first) {
			fh << delimiter;
		} else {
			first = false;
		}
		name = (char *) fld_ptr->Name ().c_str ();

		if (name != 0) {
			if (strstr (name, Delimiters ())) {
				fh << "\"" << name << "\"";
			} else {
				fh << name;
			}
		}
	}
	fh << endl;

	//---- end the header line ----
	
	First_Offset (Offset ());

	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Read_dBase_Header
//-----------------------------------------------------------

bool Db_Header::Read_dBase_Header (bool stat)
{
	int i, nfield, offset;
	double size;
	char marker;
	String name;
	Field_Type type;

	dBase_Header header;
	dBase_Field field;

	//---- read the database header ----

	if (!Read (&header, sizeof (header), 0L)) {
		return (Status (FILE_HEADER));
	}
	nfield = (header.header_size - sizeof (header) - 1) / sizeof (field);
	offset = 1;		//---- delete marker ----

	//---- validate the definition file ----

	if (stat) {
		if (Num_Fields () == nfield && Num_Fields () > 0) {
			Db_Field *fld_ptr;

			for (i=0; i < nfield; i++) {
				if (!Read (&field, sizeof (field))) {
					return (Status (FIELD_LIST));
				}
				fld_ptr = Field (i);
				name = field.name;

				if (fld_ptr == 0 || !fld_ptr->Name ().Starts_With (name)) {
					stat = false;
					break;
				}
			}
			if (!stat) {
				Read (&header, sizeof (header), 0L);
				Clear_Fields ();
			}
		} else {
			stat = false;
			Clear_Fields ();
		}
	}

	//---- read each field ----

	if (!stat) {
		for (i=0; i < nfield; i++) {
			if (!Read (&field, sizeof (field))) {
				return (Status (FIELD_LIST));
			}
			marker = field.type;
			if (marker > 90) marker -= 32;

			switch (marker) {
				case 'C':
					type = DB_STRING;
					break;
				case 'D':
					type = DB_STRING;
					break;
				case 'L':
					type = DB_CHAR;
					break;
				case 'N':
					if (field.decimal) {
						type = DB_DOUBLE;
					} else {
						type = DB_INTEGER;
					}
					break;
				case 'F':
					type = DB_DOUBLE;
					break;
				default:
					offset += field.width;
					continue;
			}
			name = field.name;

			if (field.decimal > 9) {
				size = (double) field.width + (field.decimal / 100.0);
			} else {
				size = (double) field.width + (field.decimal / 10.0);
			}
			if (Add_Field (name, type, size, false, offset) < 0) {
				return (false);
			}
			offset += field.width;
		}
	}

	//---- read the end of header marker ----

	Read (&marker, sizeof (char));

	if (marker != 13) {
		return (Status (RECORD_SYNTAX));
	}

	//---- set the record information ----

	Max_Size (header.record_size);
	First_Offset (header.header_size);
	Num_Records (header.num_records);

	if (!stat) {
		Write_Def_Header ("");
	}
	return (Rewind ());
}

//-----------------------------------------------------------
//	Write_dBase_Header
//-----------------------------------------------------------

bool Db_Header::Write_dBase_Header (void) 
{
	char marker, *name;
	int i, j, nfield, size, max_size, num_field;
	time_t ltime;
	struct tm *dt;
	Db_Field *fld;

	dBase_Header header;
	dBase_Field field;

	nfield = Num_Fields ();

	if (nfield == 0) return (Status (NO_FIELD));

	//---- get the record size ----

	max_size = num_field = 0;

	for (i=0; i < nfield; i++) {
		fld = Field (i);
		if (fld == 0) continue;

		size = fld->Offset () + fld->Width ();

		if (size > max_size) {
			max_size = size;
		}
		num_field++;
	}

	//---- write the header ----

	time (&ltime);
	dt = local_time (&ltime);

	memset (&header, '\0', sizeof (header));

	header.date [0] = (char) dt->tm_year;
	header.date [1] = (char) (dt->tm_mon + 1);
	header.date [2] = (char) dt->tm_mday;

	header.version = 3;
	header.num_records = Num_Records ();
	header.header_size = (short) (sizeof (header) + num_field * sizeof (dBase_Field) + 1);
	header.record_size = (short) max_size;

	if (!Write (&header, sizeof (header), 0L)) {
		return (Status (HEADER_WRITE));
	}

	//---- write each field ----

	for (i=0; i < nfield; i++) {
		fld = Field (i);
		if (fld == 0) continue;

		memset (&field, '\0', sizeof (field));

		switch (fld->Type ()) {
			case DB_STRING:
			case DB_CHAR:
				field.type = 'C';
				break;
			case DB_DOUBLE:
			case DB_FLOAT:
				field.type = 'F';
				break;
			case DB_INTEGER:
			case DB_UNSIGNED:
			case DB_FIXED:
			case DB_SHORT:
				field.type = 'N';
				break;
		}
		name = (char *) fld->Name ().c_str ();
		size = (int) fld->Name ().size ();
		
		if (size > (int) sizeof (field.name)) {
			size = (int) sizeof (field.name);
		}
		for (j=0; j < size; j++, name++) {
			field.name [j] = *name;
		}
		field.width = (char) fld->Width ();
		field.decimal = (char) fld->Decimal ();

		if (!Write (&field, sizeof (field))) {
			return (Status (HEADER_WRITE));
		}
	}
	marker = 13;

	if (!Write (&marker, sizeof (char))) {
		return (Status (HEADER_WRITE));
	}
	Record_Size (0);
	Record_Size (header.record_size);
	First_Offset (header.header_size);

	return (Record_Number (-1));
}

//---------------------------------------------------------
//	Def_Format
//---------------------------------------------------------

string Db_Header::Def_Format (string filename) 
{
	char buffer [4094];
	const char *delim = ",\t";
	String name, record, token;
	FILE *def;

	//---- construct the definition filename ----

	if (filename.empty ()) return ("");

	name = filename + ".def";

	def = f_open (name, "rt");
	if (def == 0) return ("");

	//---- get the header line ----

	if (fgets (buffer, sizeof (buffer), def) == 0) return ("");

	record = buffer;
	fclose (def);

	//---- TRANSIMS version code ----

	record.Split (token, delim);

	if (!token.Starts_With ("TRANSIMS")) return ("");

	//---- file format ----

	record.Split (token, delim);

	return (token);
}

//---------------------------------------------------------
//	Def_Fields
//---------------------------------------------------------

Strings Db_Header::Def_Fields (string filename) 
{
	int headers;
	char buffer [4094];
	const char *delim = ",\t";
	String name, record, token;
	Strings fields;
	FILE *def;

	//---- construct the definition filename ----

	if (filename.empty ()) return (fields);

	name = filename + ".def";

	def = f_open (name, "rt");
	if (def == 0) return (fields);

	//---- get the header line ----

	if (fgets (buffer, sizeof (buffer), def) == 0) return (fields);

	record = buffer;

	//---- TRANSIMS version code ----

	record.Split (token, delim);

	if (!token.Starts_With ("TRANSIMS")) return (fields);

	//---- file format ----

	record.Split (token, delim);

	//---- file format ----

	record.Split (token, delim);

	//---- number of header lines ----

	record.Split (token, delim);

	headers = token.Integer ();

	for (int i=1; i < headers; i++) {
		if (fgets (buffer, sizeof (buffer), def) == 0) return (fields);
	}

	//---- read the fields ----

	while (fgets (buffer, sizeof (buffer), def) != 0) {
		record = buffer;
		record.Clean ();

		record.Split (name, delim);
		if (name.empty ()) continue;

		fields.push_back (name);
	}
	fclose (def);
	return (fields);
}

//-----------------------------------------------------------
//	Read_Def_Header
//-----------------------------------------------------------

bool Db_Header::Read_Def_Header (void) 
{
	bool binary, matrix;
	int num, offset;
	double size;
	Field_Type type;
	char buffer [4094];
	const char *delim = ",\t";
	String name, record, token;
	FILE *def;

	//---- construct the definition filename ----

	name = Filename () + ".def";

	//---- open the definition file ----

	def = f_open (name, "rt");

	if (def == 0 && Model_Format () == TRANSCAD) {
		if (File_Format () == MATRIX) return (true);

		String fname = Filename ();
		fname.Split_Last (token, ".");
		if (fname.Equals ("MTX")) return (true);

		if (!Read_TCAD_Header ()) goto error;
		def = f_open (name, "rt");
	}
	if (def == 0) return (Default_Definition ());

	//---- get the header line ----

	if (fgets (buffer, sizeof (buffer), def) == 0) goto error;

	record = buffer;

	//---- TRANSIMS version code ----

	record.Split (token, delim);

	if (token.Starts_With ("AECOM HEADER")) {
		Version (40);
	} else {
		if (!token.Starts_With ("TRANSIMS")) goto error;
		token.erase (0, 8);
		num = token.Integer ();
		if (num < 40 || num > 99) goto error;
		Version (num);
	}

	//---- file format ----

	record.Split (token, delim);

	Dbase_Format (token);

	matrix = (Dbase_Format () == MATRIX);
	binary = (matrix || Dbase_Format () == BINARY || Dbase_Format () == SQLITE3 || Dbase_Format () == STRUCTURE);

	//---- number of header lines ----

	record.Split (token, delim);

	Header_Lines (token.Integer ());

	if (binary || Dbase_Format () == FIXED_COLUMN) {
		Header_Lines (Header_Lines () + 1);
	}

	for (int i=1; i < Header_Lines (); i++) {
		if (fgets (buffer, sizeof (buffer), def) == 0) {
			exe->Error ("Reading the Header Lines");
		}
		Header_Record (buffer);
	}

	//---- read the fields ----

	while (fgets (buffer, sizeof (buffer), def) != 0) {
		record = buffer;
		record.Clean ();

		record.Split (name, delim);
		if (name.empty ()) continue;

		record.Split (token, delim);
		type = Field_Code (token);

		record.Split (token, delim);
		offset = token.Integer ();
		if (matrix) {
			if (offset != Num_Fields () + 1) goto error;
			offset = -1;
		}
		record.Split (token, delim);
		size = token.Double ();

		if (Add_Field (name, type, size, binary, offset) < 0) goto error;
	}
	fclose (def);
	return (true);

error:
	return (Status (DEF_FORMAT));
}

//-----------------------------------------------------------
//	Write_Def_Header
//-----------------------------------------------------------

bool Db_Header::Write_Def_Header (string user_lines) 
{
	int i, num_lines, lines, len, offset;
	Format_Type def_type;
	string filename;
	String format;
	char *ptr, *nptr;
	FILE *def;
	Field_Ptr fld;

	//---- construct the definition filename ----

	filename = Filename () + ".def";

	//---- create the definition file ----

	def = f_open (filename, "wt");

	if (def == 0) {
		return (Status (OPEN_DEF));
	}

	lines = 1;
	num_lines = Header_Lines ();

	if (Dbase_Format () == BINARY || Dbase_Format () == FIXED_COLUMN) {
		num_lines -= lines;
	}

	//---- write the header line ----

	def_type = Dbase_Format ();
	if (def_type == CSV_DELIMITED) def_type = COMMA_DELIMITED;

	format = Data_Format (def_type, Model_Format (), Code_Flag ());

	if (fprintf (def, "TRANSIMS%d, %s, %d\n", Version (), format.c_str (), num_lines) < 0) goto error;
	
	//---- write extra header lines ----

	if (Header_Lines () > lines) {
		ptr = (char *) user_lines.c_str ();

		for (i=lines; i < Header_Lines (); i++) {
			if (ptr != 0) {
				nptr = strchr (ptr, '\n');

				if (nptr != 0) {
					len = (int) (nptr - ptr);
					fprintf (def, "%*.*s\n", len, len, ptr);
					ptr = nptr + 1;
				} else {
					fprintf (def, "%s\n", ptr);
					ptr = 0;
				}
			} else {
				fprintf (def, "\n");
			}
		}
	}

	//---- write the fields ----

	for (i=0; i < Num_Fields (); i++) {
		fld = Field (i);

		offset = fld->Offset ();

		if (fprintf (def, "%s, %s, %d, %d", fld->Name ().c_str (), Field_Code (fld->Type ()), 
			offset, fld->Width ()) < 0) goto error;

		if (fld->Type () == DB_DOUBLE || fld->Type () == DB_FIXED || 
			fld->Type () == DB_FLOAT || fld->Decimal () > 0) {

			if (fprintf (def, ".%d", fld->Decimal ()) < 0) goto error;
		}
		fprintf (def, "\n");
	}

	fclose (def);
	return (true);

error:
	return (Status (DEF_FORMAT));
}

//-----------------------------------------------------------
//	Read_TCAD_Header
//-----------------------------------------------------------

bool Db_Header::Read_TCAD_Header (void) 
{
	bool binary;
	int width, decimal, offset;
	double size;
	Field_Type type;
	char buffer [4094];
	const char *delim = ",";
	String name, record, token;
	Strings parts;
	FILE *def;

	//---- construct the definition filename ----

	name = Filename ();

	if (name.Ends_With (".bin")) {
		if (name.size () > 4 && name [name.size () - 4] == '.') {
			name.erase (name.size () - 3, 3);
		} else {
			name += ".";
		}
		name += "dcb";
		binary = true;
		Dbase_Format (BINARY);
	} else {
		if (name.size () > 4 && name [name.size () - 4] == '.') {
			name.erase (name.size () - 3, 3);
		} else {
			name += ".";
		}
		name += "dct";
		binary = false;
		Dbase_Format (FIXED_COLUMN);
	}

	//---- open the definition file ----

	def = f_open (name, "rt");

	if (def == 0) return (false);

	//---- get the header line ----

	if (fgets (buffer, sizeof (buffer), def) == 0) return (false);
	if (fgets (buffer, sizeof (buffer), def) == 0) return (false);

	Header_Lines (1);

	//---- read the fields ----

	while (fgets (buffer, sizeof (buffer), def) != 0) {
		record = buffer;
		record.Clean ();

		record.Parse (parts, delim);

		if (parts.size () < 4) continue;

		name = parts [0];

		token = parts [1];

		if (token.Starts_With ("I")) {
			type = DB_INTEGER;
		} else if (token.Starts_With ("F") || token.Starts_With ("R")) {
			type = DB_DOUBLE;
		} else if (token.Starts_With ("C") || token.Starts_With ("S")) {
			type = DB_STRING;
		} else {
			return (false);
		}

		token = parts [2];

		offset = token.Integer () - 1;

		token = parts [3];

		width = token.Integer ();

		if (type == DB_DOUBLE && parts.size () > 6) {
			token = parts [6];

			decimal = token.Integer ();

			size = (double) width + (decimal / 10.0);
		} else {
			size = width;

			if (width == 1 && type == DB_STRING) {
				type = DB_CHAR;
			}
		}
		if (Add_Field (name, type, size, binary, offset) < 0) return (false);
	}
	fclose (def);

	Write_Def_Header ("");

	Clear_Fields ();
	return (true);
}
