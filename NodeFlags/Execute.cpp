//*********************************************************
//	Execute.cpp - main processing
//*********************************************************

#include "NodeFlags.hpp"

//---------------------------------------------------------
//	Execute program
//---------------------------------------------------------

void NodeFlags::Execute (void)
{
	//---- read the node file ----

	Read_Nodes ();

	//---- read the network ----

	Read_Links ();

	//---- set flag values ----

	Set_Flags ();

	//---- write new nodes ----

	Write_Nodes ();

	Exit_Stat (DONE);
}

