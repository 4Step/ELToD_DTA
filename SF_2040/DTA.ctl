TITLE                                   DTA

PROJECT_DIRECTORY                       .\
DEFAULT_FILE_FORMAT                     COMMA_DELIMITED
MODEL_START_TIME                        0.0
MODEL_END_TIME                          24.0 
MODEL_TIME_INCREMENT                    15 minutes              //---- 0, 2..240 minutes

LINK_FILE                               LINK.CSV
LINK_FORMAT                             COMMA_DELIMITED
EXPRESS_FACILITY_TYPES                  96..98
MINIMUM_TRIP_SPLIT                      0.01

NODE_FILE                               NODE.CSV
NODE_FORMAT                             COMMA_DELIMITED
EXPRESS_ENTRY_TYPES                     90
EXPRESS_EXIT_TYPES                      92
GENERAL_JOIN_TYPES                      91
ZONE_NODE_TYPE                          99

TRIP_FILE                               TRIP_TABLE.CSV
TRIP_FORMAT                             COMMA_DELIMITED

TOLL_FILE                               Toll_Link.csv
TOLL_FORMAT                             COMMA_DELIMITED

NUMBER_OF_THREADS                       48
NUMBER_OF_ITERATIONS                    100

TIME_VALUE                              10.0
DISTANCE_VALUE                          1.0
COST_VALUE                              1.0
MODE_COST_FACTORS                       1.0, 0.2, 4.0

TOLL_POLICY_CODES                       1,3,5..6
MINIMUM_TOLL                            0.25
MAXIMUM_TOLL                            7.0
MAXIMUM_VC_RATIO                        5.0
VC_RATIO_OFFSET                         0.175
TOLL_EXPONENT                           3.5
MAXIMUM_TOLL_CHANGE                     10.0

NEW_VOLUME_FILE                         Volume.csv
NEW_VOLUME_FORMAT                       COMMA_DELIMITED

NEW_PATH_LEG_FILE                       Path_Leg.csv
NEW_PATH_LEG_FORMAT                     COMMA_DELIMITED

SELECT_ORIGINS                          2
SELECT_DESTINATIONS                     165
SELECT_TIME_PERIODS                     6..9
##SELECT_ITERATIONS                     1, 4..10
SELECT_MODES                            SOV, HOV3, TRK

NEW_CONVERGENCE_FILE                    gap_file.csv

DTA_REPORT                              CONVERGENCE_REPORT