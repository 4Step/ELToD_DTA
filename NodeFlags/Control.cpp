//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "NodeFlags.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void NodeFlags::Program_Control (void)
{
	String key;
	
	//---- create the network files ----

	Execution_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- open link file ----

	key = Get_Control_String (LINK_FILE);

	//---- get the file format ----

	if (Check_Control_Key (LINK_FORMAT)) {
		link_file.Dbase_Format (Get_Control_String (LINK_FORMAT));
	}

	if (!link_file.Open (Project_Filename (key))) {
		Error ("Link File Not Found");
	}
	//---- express facility types ----

	express_types.Add_Ranges (Get_Control_Text (EXPRESS_FACILITY_TYPES));

	//---- express entry types ----

	entry_types.Add_Ranges (Get_Control_Text (EXPRESS_ENTRY_TYPES));

	//---- express exit types ----

	exit_types.Add_Ranges (Get_Control_Text (EXPRESS_EXIT_TYPES));

	//---- open node file ----

	Print (1);
	key = Get_Control_String (NODE_FILE);

	//---- get the file format ----

	if (Check_Control_Key (NODE_FORMAT)) {
		node_file.Dbase_Format (Get_Control_String (NODE_FORMAT));
	}

	if (!node_file.Open (Project_Filename (key))) {
		Error ("Node File Not Found");
	}

	//---- node flag field ----

	key = Get_Control_Text (NODE_FLAG_FIELD);

	flag_field = node_file.Required_Field (key);

	Print (0, String (" (Field=%d)") % (flag_field + 1));

	//---- open the new node file ----

	Print (1);
	key = Get_Control_String (NEW_NODE_FILE);

	//---- get the file format ----

	if (Check_Control_Key (NEW_NODE_FORMAT)) {
		new_file.Dbase_Format (Get_Control_String (NEW_NODE_FORMAT));
	}

	if (!new_file.Create (Project_Filename (key))) {
		Error ("New Node File Not Created");
	}
	new_file.Clear_Fields ();
	new_file.Replicate_Fields (&node_file);

	new_file.Write_Header ();
}
