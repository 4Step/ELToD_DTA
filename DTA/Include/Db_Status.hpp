//********************************************************* 
//	Db_Status.hpp - database status code
//*********************************************************

#ifndef DB_STATUS_HPP
#define DB_STATUS_HPP

#include "System_Defines.hpp"
#include "Static_Service.hpp"
#include "Execution_Service.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Db_Status Class definition
//---------------------------------------------------------

class Db_Status : public Static_Service
{
public:
	Db_Status (void);

	bool    Status (void)                    { return (_status == OK); }
	bool    Status (Status_Code stat, bool message = true);

	void    File_ID (string id)              { _file_id = id; }
	String& File_ID (void)                   { return ((!_file_id.empty ()) ? _file_id : _default_id); }

	void    File_Type (string label)         { _file_type = label; }
	String& File_Type (void)                 { return ((!_file_type.empty ()) ? _file_type : _default_type); }

	Status_Code Get_Status (void)            { return (_status); }

	const char * Status_Message (void)       { return (Db_File_Code (Get_Status ())); }

private:
	Status_Code _status;

	String _file_type;
	String _file_id;

	static String _default_type;
	static String _default_id;
};
#endif
