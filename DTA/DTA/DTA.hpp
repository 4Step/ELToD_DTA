//*********************************************************
//	DTA.hpp - DTA classes
//*********************************************************

#include "Execution_Service.hpp"
#include "Data_Types.hpp"
#include "File_Types.hpp"
#include "Bounded_Queue.hpp"
#include "Data_Range.hpp"

//---------------------------------------------------------
//	DTA class definition
//---------------------------------------------------------

class DTA : public Execution_Service
{
public:
	DTA (void);

	virtual void Execute (void);

protected:
	enum DTA_Keys {
		LINK_FILE = 1, LINK_FORMAT, EXPRESS_FACILITY_TYPES, 
		NODE_FILE, NODE_FORMAT, EXPRESS_NODE_TYPES, ZONE_NODE_TYPE,
		TOLL_FILE, TOLL_FORMAT, TRIP_FILE, TRIP_FORMAT, MINIMUM_TRIP_SPLIT,
		NUMBER_OF_ITERATIONS, TIME_VALUE, DISTANCE_VALUE, COST_VALUE, MODE_COST_FACTORS,
		TOLL_POLICY_CODES, MINIMUM_TOLL, MAXIMUM_TOLL, MAXIMUM_VC_RATIO, VC_RATIO_OFFSET, TOLL_EXPONENT, MAXIMUM_TOLL_CHANGE, 
		NEW_VOLUME_FILE, NEW_VOLUME_FORMAT, NEW_PATH_LEG_FILE, NEW_PATH_LEG_FORMAT,
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TIME_PERIODS, SELECT_MODES,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum DTA_Reports { STATS_REPORT = 1 };

	Link_File link_file;
	Node_File node_file;
	Toll_File toll_file;
	Trip_File trip_file;
	Volume_File volume_file;
	Path_Leg_File path_leg_file;

	bool volume_flag, path_leg_flag, sel_org_flag, sel_des_flag, sel_per_flag, sel_mode_flag;
	int max_zone, num_node, num_link, num_period, num_mode, num_iter, zone_type;
	Data_Range sel_org_range, sel_des_range, sel_per_range, sel_mode_range;
	Data_Range policy_range, express_types, node_types;

	double value_time, value_len, value_cost, min_trip_split;
	Floats cost_factors, max_vc, min_toll, max_toll, vc_offset, exponent, max_change;

	Node_Array node_array;
	Int_Map node_map;

	Link_Array link_array;
	Int2_Map link_map;

	Integers mode_fields;
	Strings mode_names;

	Toll_Array toll_array;

	Volume_Array volume_array, old_volume_array;

	Integers node_link;
	Integers next_link;

	//---- read the network ----

	void Read_Links (void);
	void Read_Nodes (void);
	void Read_Trips (void);
	void Read_Tolls (void);
	void Update_Time_Cost (int iteration);
	void Write_Volumes (void);

	bool thread_flag;
	Threads threads;
	typedef Bounded_Queue <Assign_Data> Assign_Queue;
	
	Assign_Queue assign_queue;

	//---------------------------------------------------------
	//	Assign_Trips - build paths and load trips
	//---------------------------------------------------------

	class Assign_Trips
	{
	public:
		Assign_Trips (DTA *_exe, int id = 0);
		virtual ~Assign_Trips (void);

		void operator()();

		void Process_Trips (Assign_Data data);
		void Combine_Volume (void);

		void Clear (void);

	private:
		int thread, root, mode, des_node;
		double vcost, start;
		bool output_flag;

		DTA *exe;
		Volume_Array volume_array, *volume_array_ptr;

		void Build_Path (Floats *trips);
		void Load_Path (double tod, double trips, Path_Leg_Array &leg_array);
		void Alt_Path (double tod, double cost, double length, int from_node, int to_node, int skip_link, Path_Leg_Array &leg_array);
		void Toll_Choice (double tod, double trips, Path_Leg_Array &path1_array, Path_Leg_Array &path2_array);
	};

	Assign_Trips **assign_trips;
};
