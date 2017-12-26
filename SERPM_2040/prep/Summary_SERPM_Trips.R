
library(dplyr)

hh_pop_file <- "C:\\serpm_popsyn3\\inputs\\2040_TAZ_Data_20160613.csv"

hh_file <- "C:\\serpm7\\Input\\IN-2040R\\ctramp\\hhFile.csv"
per_file <- "C:\\serpm7\\Input\\IN-2040R\\ctramp\\personFile.csv"

VisitorTripDataFile = "C:\\serpm7\\Output\\Out-2040R\\ctramp\\visitorTrips.csv"
IndivTripDataFile = "C:\\serpm7\\Output\\Out-2040R\\ctramp\\indivTripData_1.csv"
JointTripDataFile = "C:\\serpm7\\Output\\Out-2040R\\ctramp\\jointTripData_1.csv"

df_iPop <- read.csv(hh_pop_file) %>%
           mutate(TOTAL_PERSONS = MALE + FEMALE) %>%
           summarise_at(vars(TOTAL_PERSONS, TOTAL_HOUSEHOLDS), sum)

df_hh <- read.csv(hh_file)

df_per <- read.csv(per_file)

df_indiv <- read.csv(IndivTripDataFile)

df_joint <- read.csv(JointTripDataFile)

df_visit <- read.csv(VisitorTripDataFile)

summary_indiv <-  df_indiv %>% 
                  mutate(vehTrips =  ifelse(trip_mode %in% c(1:2), 1, 
                                        ifelse(trip_mode %in% c(3:5), 0.5, 
                                            ifelse(trip_mode %in% c(6:8), 1/3.33, 0)))) %>%
                  group_by(trip_mode) %>% summarise(vtrips = sum(vehTrips), ptrips=n())
# write.csv(summary_indiv, "summary_indiv.csv")



summary_joint <-  df_joint %>% 
                  mutate(vehTrips =  ifelse(trip_mode %in% c(1:8), 1, 0)) %>%
                  group_by(trip_mode) %>% summarise(vtrips = sum(vehTrips), ptrips=n())
# write.csv(summary_joint, "summary_joint.csv")



summary_visit <-  df_visit %>% 
                  mutate(vehTrips =  ifelse(tripMode %in% c(1:8), 1, 0)) %>%
                  group_by(tripMode) %>% summarise(vtrips = sum(vehTrips), ptrips=n())
# write.csv(summary_visit, "summary_visit.csv")


mode_labels <- c("SOV_GP"  ,"SOV_PAY"  ,"SR2_GP"  ,"SR2_HOV"  ,"SR2_PAY"  ,"SR3_GP"
,"SR3_HOV"  ,"SR3_PAY"  ,"WALK"  ,"BIKE"  ,"WLK_LOC"  ,"WLK_EXP"  ,"WLK_BRT"
,"WLK_LRT"  ,"WLK_HRT"  ,"WLK_CMR"  ,"PNR_LOC"  ,"PNR_EXP"  ,"PNR_BRT"
,"PNR_LRT"  ,"PNR_HRT"  ,"PNR_CMR"  ,"KNR_LOC"  ,"KNR_EXP"  ,"KNR_BRT"
,"KNR_LRT"  ,"KNR_HRT"  ,"KNR_CMR"  ,"SCHLBUS", "TAXI")

summary <- data.frame(trip_mode = c(1:30), model_label = mode_labels)

summary <- summary %>%
    left_join(summary_indiv, by = "trip_mode", suffix = c(".indiv",".joint")) %>%   
    left_join(summary_joint, by = "trip_mode", suffix = c(".indiv",".joint")) %>%
    left_join(summary_visit, by = c("trip_mode" = "tripMode")) %>%
    mutate_if(is.numeric, funs(ifelse(is.na(.),0, .)))
write.csv(summary, "summary.csv")


df <- read.csv(file.choose())

df <- df %>% 
     mutate(all_fraction = ifelse(SOV < 1 & TRK < 1, 1, 0)) %>% 
     group_by(all_fraction) 
     
  df %>% summarise(Trips = sum(SOV, TRK), sov_count = n())
     
  

