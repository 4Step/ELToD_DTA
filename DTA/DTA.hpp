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
		NODE_FILE, NODE_FORMAT, EXPRESS_ENTRY_TYPES, EXPRESS_EXIT_TYPES, GENERAL_JOIN_TYPES, ZONE_NODE_TYPE,
		TRIP_FILE, TRIP_FORMAT, MINIMUM_TRIP_SPLIT, STORE_TRIPS_IN_MEMORY,
		TOLL_FILE, TOLL_FORMAT, TOLL_CONSTANT_FILE, TOLL_CONSTANT_FORMAT,
		NUMBER_OF_ITERATIONS, GAP_CONVERGENCE_CRITERIA, TIME_VALUE, DISTANCE_VALUE, COST_VALUE, MODE_COST_FACTORS,
		TOLL_POLICY_CODES, MINIMUM_TOLL, MAXIMUM_TOLL, MAXIMUM_VC_RATIO, VC_RATIO_OFFSET, TOLL_EXPONENT, MAXIMUM_TOLL_CHANGE, 
		MODEL_TIME_FACTOR, MODEL_TOLL_FACTOR, MODEL_RELIABILITY_RATIO, MODEL_RELIABILITY_TIME, MODEL_RELIABILITY_DISTANCE,
		MODEL_PERCEIVED_TIME, MODEL_PERCEIVED_MID_VC, MODEL_PERCEIVED_MAX_VC, MODEL_EXPRESS_WEIGHT, MODEL_SCALE_LENGTH, MODEL_SCALE_ALPHA,
		NEW_VOLUME_FILE, NEW_VOLUME_FORMAT, NEW_PATH_LEG_FILE, NEW_PATH_LEG_FORMAT, NEW_MODEL_DATA_FILE, NEW_MODEL_DATA_FORMAT,
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TIME_PERIODS, SELECT_ITERATIONS, SELECT_MODES, SELECT_DECISION_NODES,
		NEW_LINK_GAP_FILE, NEW_TOLL_GAP_FILE, NEW_PERIOD_GAP_FILE, NEW_PERIOD_GAP_FORMAT,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum DTA_Reports { LINK_GAP_REPORT = 1, TOLL_GAP_REPORT };

	Link_File link_file;
	Node_File node_file;
	Toll_File toll_file;
	Trip_File trip_file;
	Volume_File volume_file;
	Path_Leg_File path_leg_file;
	Gap_File link_gap_file, toll_gap_file, period_gap_file;
	Model_Data_File model_data_file;
	Db_Header constant_file;

	Gap_Array link_gap_array, toll_gap_array;

	bool volume_flag, path_leg_flag, sel_org_flag, sel_des_flag, sel_per_flag, sel_iter_flag, sel_mode_flag, sel_node_flag;
	bool link_gap_flag, toll_gap_flag, memory_flag, model_data_flag, period_gap_flag;
	int max_zone, num_node, num_link, num_period, num_mode, num_iter, zone_type, report_num;
	Data_Range sel_org_range, sel_des_range, sel_per_range, sel_iter_range, sel_mode_range, sel_nodes;
	Data_Range policy_range, express_types, entry_types, exit_types, join_types;

	int iter, num_alt_path, num_path_build, num_od_loads, num_choices;
	double value_time, value_len, value_cost, min_trip_split, min_speed, exit_gap;
	Floats cost_factors, max_vc, min_toll, max_toll, vc_offset, exponent, max_change, toll_constant;

	Model_Array model_array;

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
	double Update_Time_Cost (int iteration);
	void Write_Volumes (void);

	void Gap_Report (void);
	void Gap_Header (void);

	bool thread_flag;
	Threads threads;
	typedef Bounded_Queue <Assign_Data> Assign_Queue;
	
	Assign_Queue assign_queue;
	Assign_Array assign_array;

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
		int thread, root, mode, org, des, des_node, start_period;
		int num_alt_path, num_path_build, num_od_loads, num_choices, *alt_path_ptr, *path_build_ptr, *od_loads_ptr, *choice_ptr;
		double vcost, start;
		bool leg_out_flag, data_out_flag;

		DTA *exe;
		Volume_Array volume_array, *volume_array_ptr;

		void Build_Path (Floats *trips);
		void Load_Path (double trips, Path_Leg_Array &leg_array);
		void Alt_Path (Path_Leg_Data root_data, int from_node, int to_node, int skip_link, Path_Leg_Array &leg_array);
		void Toll_Choice (double trips, Path_Leg_Array &path1_array, Path_Leg_Array &path2_array);
	};

	Assign_Trips **assign_trips;
};
