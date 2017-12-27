//*********************************************************
//	NodeFlags.cpp - main processing
//*********************************************************

#include "NodeFlags.hpp"

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

NodeFlags::NodeFlags (void) : Execution_Service ()
{
	Program ("NodeFlags");
	Version (0);
	Title ("Toll Decision Points");

	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ LINK_FILE, "LINK_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_FORMAT, "LINK_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ EXPRESS_FACILITY_TYPES, "EXPRESS_FACILITY_TYPES", LEVEL0, OPT_KEY, INT_KEY, "96", "1..100", NO_HELP },
		{ EXPRESS_ENTRY_TYPES, "EXPRESS_ENTRY_TYPES", LEVEL0, OPT_KEY, INT_KEY, "97", "1..100", NO_HELP },
		{ EXPRESS_EXIT_TYPES, "EXPRESS_EXIT_TYPES", LEVEL0, OPT_KEY, INT_KEY, "98", "1..100", NO_HELP },

		{ NODE_FILE, "NODE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NODE_FORMAT, "NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NODE_FLAG_FIELD, "NODE_FLAG_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "DTA_Type", "", NO_HELP},

		{ NEW_NODE_FILE, "NEW_NODE_FILE", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_NODE_FORMAT, "NEW_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "COMMA_DELIMITED", FORMAT_RANGE, FORMAT_HELP },

		END_CONTROL
	};

	Key_List (keys);

	flag_field = -1;
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	NodeFlags *program = new NodeFlags ();
	return (program->Start_Execution (commands, control));
}
