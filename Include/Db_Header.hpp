//********************************************************* 
//	Db_Header.hpp - generic database header interface
//*********************************************************

#ifndef DB_HEADER_HPP
#define DB_HEADER_HPP

#include "Db_Base.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Db_Header Class definition
//---------------------------------------------------------

class Db_Header : public Db_Base
{
public:
	Db_Header (Access_Type access, string format);
	Db_Header (string filename, Access_Type access, string format);
	Db_Header (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Db_Header (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Db_Header (void);
	
	void Initialize (Access_Type access, string format);
	void Initialize (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	virtual bool Open (string path = "");
	virtual bool Open (int index);
	virtual bool Close (void);
	
	void Header_Lines (int number)              { _header_lines = number; }
	int  Header_Lines (void)                    { return (_header_lines); }

	void Scan_Lines (int number = 100)          { _scan_lines = number; }

	String & Header_Record (void)               { return (_header_record); }
	void Header_Record (string header)          { _header_record = header; }
	void Header_Record (const char *header)     { _header_record = header; }

	bool Custom_Header (void)                   { return (_custom_header); } 
	void Custom_Header (bool flag)              { _custom_header = flag; }

	int  Notes_Field (void)                     { return (_notes); }
	void Notes_Field (int field)                { if (exe->Notes_Name_Flag()) _notes = field; }

	String Notes (void)                         { return (Get_String (_notes)); }
	void Notes (char *value)                    { Put_Field (_notes, value); }
	void Notes (string value)                   { Put_Field (_notes, value); }

	virtual bool Write_Header (string user_lines = "");

	bool Update_Def_Header (void)               { return (Write_Def_Header (Header_Record ())); }

	static string Def_Format (string filename);
	static Strings Def_Fields (string filename);

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Default_Definition (void)      { return (false); }

	bool Write_Def_Header (string user_lines);

private:

	//---- header structure -----

	typedef struct {
		char  version;
		char  date [3];
		int   num_records;
		short header_size;
		short record_size;
		char  reserve1 [2];
		char  transaction_flag;
		char  encryption_flag;
		char  reserve2 [12];
		char  mdx_flag;
		char  reserve3 [3];
	} dBase_Header;

	//---- field structure ----

	typedef  struct {
		char  name [11];	
		unsigned char  type;
		unsigned char  reserve1 [4];
		unsigned char  width;
		unsigned char  decimal;
		unsigned short offset;
		unsigned char  work_id;
		unsigned char  reserve3 [11];
	} dBase_Field;

	bool Read_CSV_Header (bool stat);
	bool Write_CSV_Header (string user_lines);

	bool Read_dBase_Header (bool stat);
	bool Write_dBase_Header (void);
	
	bool Read_TCAD_Header (void);

	bool Read_Def_Header (void);

	bool _custom_header;
	int  _header_lines, _scan_lines, _notes, _num_nest, _linkdir_type;
	String _header_record;
};

typedef vector <Db_Header *>      Db_Header_Array;
typedef Db_Header_Array::iterator Db_Header_Itr;
typedef Db_Header_Array::pointer  Db_Header_Ptr;

#endif
