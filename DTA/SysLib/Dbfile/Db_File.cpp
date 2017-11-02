//*********************************************************
//	Db_File.cpp - standard database interface
//*********************************************************

#include "Db_File.hpp"

#include "String.hpp"

//---------------------------------------------------------
//	Db_File constructors
//---------------------------------------------------------

Db_File::Db_File (Access_Type access, Format_Type format) : Db_Record ()
{
	Initialize (access, format);
}

Db_File::Db_File (string filename, Access_Type access, Format_Type format) : Db_Record ()
{
	Initialize (access, format);

	Open (filename);
}

//---------------------------------------------------------
//	Db_File destructor
//---------------------------------------------------------

Db_File::~Db_File (void)
{
	Close ();
#ifdef SQLITE
	if (read_stmt != 0) {
		sqlite3_finalize (read_stmt);
		read_stmt = 0;
	}
	if (read_nest != 0) {
		sqlite3_finalize (read_nest);
		read_nest = 0;
	}
	if (insert_stmt != 0) {
		sqlite3_finalize (insert_stmt);
		insert_stmt = 0;
	}
	if (insert_nest != 0) {
		sqlite3_finalize (insert_nest);
		insert_nest = 0;
	}
#endif
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Db_File::Initialize (Access_Type access, Format_Type format) 
{
	_first_offset = 0;
	_num_records = _record_num = _max_record_num = _parent_id = 0;
	_first_open = _clean_flag = true;

	Version (VERSION_CODE);
	File_Access (access);
	File_Format (format);

	switch (format) {
		default:
			File_Type ("Text File");
			break;
		case BINARY:
			File_Type ("Binary File");
			break;
		case FIXED_COLUMN:
			File_Type ("Fixed Column File");
			break;
		case COMMA_DELIMITED:
			File_Type ("Comma Delimited File");
			break;
		case SPACE_DELIMITED:
			File_Type ("Space Delimited File");
			break;
		case VERSION3:
		case TAB_DELIMITED:
			File_Type ("Tab Delimited File");
			break;
		case CSV_DELIMITED:
			File_Type ("CSV Delimited File");
			break;
		case DBASE:
			File_Type ("dBase File");
			break;
		case ARCVIEW:
			File_Type ("ArcView File");
			break;
		case MATRIX:
			File_Type ("Matrix File");
			break;
		case STRUCTURE:
			File_Type ("Structure File");
			break;
	}
}

//---------------------------------------------------------
//	File_Format
//---------------------------------------------------------

void Db_File::File_Format (Format_Type format)
{
	if (format == DEFAULT_FORMAT && exe) {
		format = (Format_Type) exe->Default_Format ();
	}
	Record_Format (format);

	if (format == COMMA_DELIMITED || format == SPACE_DELIMITED || 
		format == TAB_DELIMITED || format == VERSION3) {
		_file_format = CSV_DELIMITED;
	} else if (format == DBASE || format == ARCVIEW || format == STRUCTURE) {
		_file_format = BINARY;
	} else {
		_file_format = format;
	}
}

//---------------------------------------------------------
//	Filename
//---------------------------------------------------------

bool Db_File::Filename (string filename)
{
	if (filename == _filename) return (true);

	_filename.clear ();

	if (filename.empty ()) return (false);

	_filename = filename;
	_rootname = _filename;

	return (true);
}

String Db_File::Filename (int index) 
{
	if (index <= 0) {
		_filename = _rootname;
	} else {
		String path, extension, part;

		part ("_%d") % index;
		path = _rootname;

		if (path.find ('.') != path.npos) {
			path.Split_Last (extension, ".");
			_filename = path + part + "." + extension;
		} else {
			_filename = path + part;
		}
	}
	return (_filename);
}

//---------------------------------------------------------
//	File_Parse
//---------------------------------------------------------

Strings Db_File::File_Parse (void)
{
	Strings file_parse;
	String path, name;

	path = _filename;
	path.Split_Last (name, "\\//");
	file_parse.push_back (path);
	name.Split_Last (path, ".");
	file_parse.push_back (name);
	file_parse.push_back (path);

	return (file_parse);
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Db_File::Open (string path)
{
	if (!path.empty ()) {
		Filename (path);
	} else if (_filename.empty ()) {
		return (false);
	}
	if (Is_Open ()) {
		Close ();
	} else if (File_Format () == DEFAULT_FORMAT) {
		File_Format (DEFAULT_FORMAT);
	}
	bool create_flag, exist_flag;

	if (File_Access () == CREATE) {
		create_flag = true;
		exist_flag = Find_File ();
		_num_records = _record_num = _max_record_num = 0;
	} else {
		create_flag = exist_flag = false;
	}
	if (!Db_Open (_filename)) {
		if (exe->Send_Messages ()) {
			string message (((create_flag) ? "Creating " : "Opening "));
			message += File_Type ();
			exe->File_Error (message, _filename);
		}
		return (Status (NOT_OPEN));
	} else {
		if (create_flag && !File_Type ().Starts_With ("New") && !File_Type ().Starts_With ("Output")) {
			File_Type (String ("New %s") % File_Type ());
		}

		//---- first open processing ----

		if (_first_open && exe->Send_Messages ()) {
			exe->Print_Filename (1, File_Type (), _filename);
		}
		return (Status (OK));
	}
}

bool Db_File::Open (int index) 
{
	bool messages = exe->Send_Messages ();
	if (index > 0) exe->Send_Messages (false);

	bool stat = false;

	if (!Filename (index).empty ()) {
		stat = Open ();
	}
	exe->Send_Messages (messages);
	return (stat);
}

//---------------------------------------------------------
//	Db_Open
//---------------------------------------------------------

bool Db_File::Db_Open (string filename)
{
	ios_base::openmode code;

	_file.clear ();

	switch (File_Access ()) {
		default:
			return (Status (DB_ERROR));
			break;
		case READ:
			code = ios_base::in;
			break;
		case MODIFY:
			code = ios_base::in | ios_base::out;
			break;
		case APPEND:
			code = ios_base::in | ios_base::out | ios_base::app;
			break;
		case CREATE:
			code = ios_base::in | ios_base::out | ios_base::trunc;
			break;
	}
	if (File_Format () == BINARY) {
		code |= ios_base::binary;
	}
	_file.open (filename.c_str (), code);
	return (_file.is_open ());
}

//---------------------------------------------------------
//	Find_File - safe file existence check
//---------------------------------------------------------

bool Db_File::Find_File (void)
{
#ifdef _MSC_EXTENSIONS
	return (_access_s (_filename.c_str (), 0) == 0);
#else
	return (_access (_filename.c_str (), 0) == 0);
#endif
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Db_File::Read (void)
{
	//---- read the record into the buffer ----

	return (Read (Record_Pointer (), Max_Read ()));
}

bool Db_File::Read (void *record, int size)
{
	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (record == 0) return (Status (NULL_POINTER));
	if (size <= 0) return (Status (RECORD_SIZE));
	
	//---- read the record ----

	switch (File_Format ()) {
		case BINARY:
			if (_file.read ((char *) record, size).fail ()) {
				memset (record, '\0', size);
				return (_file.eof () ? false : Status (FILE_IO));
			}
			break;
		case SQLITE3:
			break;
		case FIXED_COLUMN:
			memset (record, ' ', size);
			if (_file.getline ((char *) record, size + 2).fail ()) {
				return (_file.eof () ? false : Status (FILE_IO));
			}
			((char *) record) [size] = '\0';
			break;
		default:
			if (_file.getline ((char *) record, size).fail ()) {
				return (_file.eof () ? false : Status (FILE_IO));
			}
			if (_clean_flag) Clean ((char *) record);
			size = (int) strlen ((char *) record);
			if (_file.eof () && size == 0) return (false);
			break;
	}
	if (!Record (record, size)) return (false);

	if (File_Format () == CSV_DELIMITED || File_Format () == SQLITE3) {
		return (Input_Record ());
	}
	return (true);
}

bool Db_File::Read (void *record, int size, size_t offset)
{
	if (!Offset (offset)) return (false);

	return (Read (record, size));
}

bool Db_File::Read_Record (int number) 
{
	if (!Record_Number (number)) return (false);

	return (Read ());
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool Db_File::Write (void)
{
	if (!Check_File ()) return (false);

	if (File_Format () == CSV_DELIMITED || File_Format () == SQLITE3) {
		if (!Output_Record ()) return (Status (NO_FIELD));
	}
	return (Write (Record_Pointer (), Record_Size ()));
}
	
bool Db_File::Write (void *record, int size)
{
	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (record == 0) return (Status (NULL_POINTER));
	if (size <= 0) return (Status (RECORD_SIZE));
		
	//---- write the record ----

	if (File_Format () == SQLITE3) return (true);

	char * rec = (char *) record;

	switch (File_Format ()) {
		case BINARY:
			_file.write (rec, size);
			break;
		case FIXED_COLUMN:
			_file << (String ("%-*.*s") % size % size % rec) << endl;
			break;
		case CSV_DELIMITED:
			_file << (String ("%.*s") % size % rec) << endl;
			break;
		default:
			_file << rec << endl;
			break;
	}
	if (_file.fail ()) {
		return (Status (FILE_IO));
	}
	return (true);
}

bool Db_File::Write (void *record, int size, size_t offset)
{
	if (!Offset (offset)) return (false);

	return (Write (record, size));
}

bool Db_File::Write_Record (int number) 
{
	if (!Record_Number (number)) return (false);

	return (Write ());
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool Db_File::Close (void)
{
	bool stat = true;

	if (Is_Open ()) {
		_first_open = false;

		if (File_Format () == SQLITE3) {
#ifdef SQLITE
			if (read_stmt != 0) {
				sqlite3_finalize (read_stmt);
				read_stmt = 0;
			}
			if (read_nest != 0) {
				sqlite3_finalize (read_nest);
				read_nest = 0;
			}
			if (insert_stmt != 0) {
				sqlite3_finalize (insert_stmt);
				insert_stmt = 0;
			}
			if (insert_nest != 0) {
				sqlite3_finalize (insert_nest);
				insert_nest = 0;
			}
			if (db_file != 0) {
				sqlite3_exec (db_file, "COMMIT", 0, 0, 0);
				sqlite3_close (db_file);
				db_file = 0;
			}
#endif
		} else {
			_file.close ();
		}
	}
	return (stat);
}

//---------------------------------------------------------
//	Rewind
//---------------------------------------------------------

bool Db_File::Rewind (void)
{
	Record_Number (-1); 

	if (File_Format () == BINARY) {
		return (Offset (First_Offset ()));
	} else if (File_Format () != SQLITE3) {
		_file.clear ();
		_file.seekg (0);

		if (First_Offset () > 0) {
			size_t len = 0;

			while (!_file.getline (Record_Pointer (), Record ().Max_Read ()).fail ()) {
				Set_Size ();
				len += Record_Size ();

				if (len >= First_Offset ()) {
					return (true);
				}
			}
			return (Status (FILE_IO));
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Offset
//---------------------------------------------------------

bool Db_File::Offset (size_t offset)
{
	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			if (offset == 0) {
				Record_Number (-1);
			} else if (Max_Size () > 0) {
				Record_Number ((int) ((offset / Max_Size ()) + 1));
			} else {
				exe->Warning (String ("Db_File::Offset on a SQLITE3 Database offset=%d") % (size_t) offset);
				return (Status (POSITIONING));
			}
		} else if (File_Format () != MATRIX) {
			if (offset >= 0) {
				_file.clear ();
				_file.seekg (offset);
				if (!_file.fail ()) return (true);
			}
			return (Status (POSITIONING));
		}
		return (true);
	}
	return (false);
}

size_t Db_File::Offset (void)
{
	streampos offset = -1;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			offset = 0;
			exe->Warning ("Db_File::Offset on a SQLITE3 Database");
		} else if (File_Format () != MATRIX) {
			offset = _file.tellg ();

			if (offset < 0) {
				if (File_Format () == BINARY) {
					Status (POSITIONING);
				} else {
					char *rec = Record_Pointer ();

					if (rec != 0) {
						offset = (int) strlen (rec) + 1;
					} else {
						offset = 0;
					}
				}
			}
		}
	}
	return ((size_t) offset);
}

//-----------------------------------------------------------
//	File_Size
//-----------------------------------------------------------

size_t Db_File::File_Size (void)
{
	streampos offset = -1;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			exe->Warning ("Db_File::File_Size on a SQLITE3 Database");
			return (Status (POSITIONING));
		} else if (File_Format () != MATRIX) {
			streampos current = _file.tellg ();

			_file.seekg (0, ios_base::end);
			offset = (size_t) _file.tellg ();

			if (offset < 0) {
				Status (POSITIONING);
			}
			if (current >= 0) {
				_file.seekg (current);
			}
		}
	}
	return ((size_t) offset);
}

//-----------------------------------------------------------
//	Num_Records
//-----------------------------------------------------------

int Db_File::Num_Records (void)
{
	if (!_num_records && Check_File ()) {
		if (File_Format () == SQLITE3) {
#ifdef SQLITE
			sqlite3_stmt *record_stmt;
			String sql;

			//---- get the number of rows ----

			sql ("SELECT COUNT(*) FROM %s;") % File_ID ();

			if (sqlite3_prepare_v2 (db_file, sql.c_str (), -1, &record_stmt, 0) != SQLITE_OK) {
				exe->Warning ("Preparing Record Statement: ") << sqlite3_errmsg (db_file);
			}
			sqlite3_step (record_stmt);
			_num_records = sqlite3_column_int (record_stmt, 0);

			sqlite3_finalize (record_stmt);
#endif
		} else if (File_Format () == BINARY || File_Format () == FIXED_COLUMN) {
			int size = Max_Size ();
			if (size > 0) {
				_num_records = (int) ((File_Size () - First_Offset () + size - 1) / size);
			}
		} else {
			FILE *temp = f_open (Filename (), "rt");

			if (temp != 0) {
				char buffer [4096];

				while (fgets (buffer, sizeof (buffer), temp) != 0) {
					_num_records++;
				}
				fclose (temp);
			}
		}
	}
	return (_num_records);
}

//-----------------------------------------------------------
//	Estimate_Records
//-----------------------------------------------------------

int Db_File::Estimate_Records (void)
{
	int num = 0;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			num = Num_Records ();
		} else if (File_Format () == BINARY || File_Format () == FIXED_COLUMN) {
			int size = Max_Size ();
			if (size > 0) {
				num = (int) ((File_Size () - First_Offset () + size - 1) / size);
			}
		} else {
			FILE *temp = f_open (Filename (), "rt");

			if (temp != 0) {
				int cum_size = 0;
				streampos offset;
				char buffer [4096];

				while (fgets (buffer, sizeof (buffer), temp) != 0) {
					cum_size += (int) strlen (buffer);
					if (++num == 100) break;
				}
				if (cum_size > 0) {
					if (!fseek (temp, 0, SEEK_END)) {
						offset = ftell (temp);

						if (!fseek (temp, -10 * (int) sizeof (buffer), SEEK_END)) {
							int size = 0;
							int n = 0;

							while (fgets (buffer, sizeof (buffer), temp) != 0) {
								if (n != 0) size += (int) strlen (buffer);
								if (++n == 100) break;
							}
							if (n > 1) {
								cum_size = (size / (--n) + cum_size / num) / 2 + 1;
								num = 1;
							}
						}
						num *= (int) (offset / cum_size) + 1;
					}
				}
				fclose (temp);
			}
		}
	}
	return (num);
}

//-----------------------------------------------------------
//	Record_Number
//-----------------------------------------------------------

bool Db_File::Record_Number (int number)
{
	if (number == 0) {
		_record_num++;
	} else if (number < 0) {
		_record_num = 0;
	} else {
		if (number != _record_num + 1 && (File_Format () == BINARY || File_Format () == FIXED_COLUMN)) {
			size_t offset = First_Offset () + (number - 1) * Record_Size ();
			if (!Offset (offset)) return (false);
		}
		_record_num = number;
	}
	if (_record_num > _max_record_num) _max_record_num = _record_num;
	return (true);
}

//---------------------------------------------------------
//	Binary_Read
//---------------------------------------------------------

bool Db_File::Binary_Read (void *buffer, int num_rec)
{
	//---- check the file status ----

	if (buffer == 0 || num_rec < 1 || !Check_File () || File_Format () != BINARY) return (false);

	int size = Max_Size ();
	if (size <= 0) return (Status (RECORD_SIZE));
	
	//---- read the record ----

	_file.read ((char *) buffer, num_rec * size);

	int num = (int) _file.gcount () / size;

	if (_record_num < 0) {
		_record_num = num;
	} else {
		_record_num += num;
	}
	if (num != num_rec) {
		char *ptr = ((char *) buffer) + num * size;
		memset (ptr, '\0', (num_rec - num) * size);
		return (_file.eof () ? false : Status (FILE_IO));
	}
	return (true);
}
//---------------------------------------------------------
//	Binary_Write
//---------------------------------------------------------

bool Db_File::Binary_Write (void *buffer, int num_rec)
{
	//---- check the file status ----

	if (buffer == 0 || num_rec < 1 || !Check_File () || File_Format () != BINARY) return (false);

	int size = Max_Size ();
	if (size <= 0) return (Status (RECORD_SIZE));
	
	//---- write the record ----

	_file.write ((char *) buffer, num_rec * size);

	int num = (int) _file.gcount () / size;

	if (_record_num < 0) {
		_record_num = num;
	} else {
		_record_num += num;
	}
	if (num != num_rec) {
		return (Status (FILE_IO));
	}
	return (true);
}
