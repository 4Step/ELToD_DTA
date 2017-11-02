//*********************************************************
//	Db_Status.cpp - database status code
//*********************************************************

#include "Db_Status.hpp"

#include "String.hpp"

String Db_Status::_default_type = "Db_Record";
String Db_Status::_default_id = "Db_File";

//---------------------------------------------------------
//	Db_Status constructors
//---------------------------------------------------------

Db_Status::Db_Status (void) : Static_Service ()
{
	Status (OK);
}

//---------------------------------------------------------
//	Status
//---------------------------------------------------------

bool Db_Status::Status (Status_Code stat, bool message)
{
	_status = stat;

	if (_status == OK) return (true);

	if (message && exe->Send_Messages ()) {
		exe->Error (String ("%s %s") % _file_type % Status_Message ());
	}
	return (false);
}
