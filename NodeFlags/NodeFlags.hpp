//*********************************************************
//	NodeFlags.hpp - NodeFlags classes
//*********************************************************

#include "Execution_Service.hpp"
#include "Data_Types.hpp"
#include "File_Types.hpp"
#include "Data_Range.hpp"

//---------------------------------------------------------
//	NodeFlags class definition
//---------------------------------------------------------

class NodeFlags : public Execution_Service
{
public:
	NodeFlags (void);

	virtual void Execute (void);

protected:
	enum DTA_Keys {
		LINK_FILE = 1, LINK_FORMAT, EXPRESS_FACILITY_TYPES, EXPRESS_ENTRY_TYPES, EXPRESS_EXIT_TYPES,
		NODE_FILE, NODE_FORMAT, NODE_FLAG_FIELD, 
		NEW_NODE_FILE, NEW_NODE_FORMAT,
	};
	virtual void Program_Control (void);

private:
	int flag_field;
	Data_Range express_types, entry_types, exit_types;

	Link_File link_file;
	Node_File node_file, new_file;

	Int_Map node_map;

	Link_Array link_array;
	Int2_Map link_map;

	Integers from_node, to_node, flags;
	Integers link_in, link_out;

	//---- read the network ----

	void Read_Links (void);
	void Read_Nodes (void);
	void Set_Flags (void);
	void Write_Nodes (void);
};
