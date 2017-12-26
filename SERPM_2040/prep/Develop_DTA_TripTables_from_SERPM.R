
# stop_period <- c(3, 9, 21, 29,  35)
# numberOfPeriods <- c ("EA", "AM", "MD", "PM", "EV")
# start_time <- c("4:30 AM - 6:00 AM", 
#                 "6:00 AM - 9:00 AM",
#                 "9:00 AM - 3:00 PM",
#                 "3:00 PM - 7:00 PM",
#                 "7:00 PM - 10:00 PM")

# SERPM_ABM_properties (use a flat 3.33, if it varies then use a table to update SR3+)
# occ3plus.purpose.Work = 3.33
# occ3plus.purpose.University = 3.33
# occ3plus.purpose.School = 3.33
# occ3plus.purpose.Escort = 3.33
# occ3plus.purpose.Shop = 3.33
# occ3plus.purpose.Maintenance = 3.33
# occ3plus.purpose.EatingOut = 3.33
# occ3plus.purpose.Visiting = 3.33
# occ3plus.purpose.Discretionary = 3.33
# occ3plus.purpose.WorkBased = 3.33

# Notes
# SERPM Model EA starts at 4:30 and goes till mid-night (40 half-hour bins)
# So in DTA 15 minute bins, EA starts at 17th bin.
# We need to split the each half-hour into two equal 15 min bins


#===============================================================================
data_dir  <- "C:\\serpm7\\Output\\ABMTEMP\\ctramp"
equiv_dir <- "C:\\serpm_popsyn3\\data"
output_dir <- "C:\\projects\\Advanced_Projects\\ELToD_DTA\\SERPM_2040"
  
IndivTripDataFile = "indivTripData_1.csv"
JointTripDataFile = "jointTripData_1.csv"
VisitorTripDataFile = "visitorTrips.csv"
maz_taz <- "geographicCWalk2PUMA.csv"
out_table <- "TRIP_TABLE.csv"

# Define auto modes
SOV_ALTS  <- c(1, 2)
HOV2_ALTS <-c( 3, 4, 5 )
HOV3_ALTS <-c( 6, 7, 8 )
HOV_ALTS   <- c(HOV2_ALTS, HOV3_ALTS)

sampleRateResidents = 0.25
sampleRateVisitors = 0.25
start_96bin_equiv <- 17
split_15min_fac <- 0.5

#===============================================================================
library(dplyr)
library(tidyr)

# Process Trip Files
processTrips <- function(df, isJointTour, samplerate){
  
  df_1 <- df %>%
          mutate(dest_mgra = ifelse(parking_mgra > 0, parking_mgra, dest_mgra),
                 mode = ifelse(trip_mode %in% SOV_ALTS, "SOV", 
                            ifelse(trip_mode %in% HOV_ALTS, "HOV", "non-Auto")),
                 vehTrips = ifelse(isJointTour, 1,
                                ifelse(trip_mode %in% SOV_ALTS, 1, 
                                  ifelse(trip_mode %in% HOV2_ALTS, 0.5, 
                                        ifelse(trip_mode %in% HOV3_ALTS, 1/3.33, 0)))),
                   period = stop_period * 2 + start_96bin_equiv,
                   vehTrips = vehTrips / samplerate / smplRate_geo,
                   vehTrips = vehTrips * split_15min_fac) %>%
            filter(mode != "non-Auto") %>%
            select(orig_mgra, dest_mgra, mode, stop_period, period, vehTrips)
  
  # Second 15 min data (create period as next)
  df_2 <- df_1 %>%
           mutate(period = stop_period * 2 + (start_96bin_equiv + 1))

  # Merge two 15 min datasets
  df_15min <- rbind(df_1, df_2) %>%
          select(- stop_period) 
}

#===============================================================================
# Read data files
indivTripData <- read.csv(paste0(data_dir,"\\",IndivTripDataFile))
jointTripData <- read.csv(paste0(data_dir,"\\",JointTripDataFile))
VisitorTripData <- read.csv(paste0(data_dir,"\\",VisitorTripDataFile))

# Rename fields to match to resident trip tables for processing function
VisitorTripData <- VisitorTripData %>%
                   select(orig_mgra = originMGRA,
                          dest_mgra = destinationMGRA,
                          stop_period = period,
                          trip_mode =   tripMode) %>%
                   mutate(parking_mgra = 0,
                          smplRate_geo = 1) 

# Process trip tables
df_indv <- processTrips(indivTripData, FALSE, sampleRateResidents)
df_join <- processTrips(jointTripData, TRUE, sampleRateResidents)
df_vist <- processTrips(VisitorTripData, TRUE, sampleRateVisitors)

df_all <- rbind(df_indv, df_join, df_vist) 


# Append TAZ
df_equiv <- read.csv(paste0(equiv_dir,"\\",maz_taz)) %>%
            select(MAZ, TAZ)

df_all <- df_all %>% 
          left_join(df_equiv, by = c("orig_mgra" = "MAZ")) %>%
          rename(I = TAZ)

df_all <- df_all %>% 
          left_join(df_equiv, by = c("dest_mgra" = "MAZ")) %>%
          rename(J = TAZ)

# Aggregate OD trips by period and mode
df_aggregate <- df_all %>%
                group_by(I, J, period, mode) %>%
                summarise(Trips = sum(vehTrips)) %>%
                spread(mode, Trips) %>%
                mutate(TRK = 0)%>%
                rename(HOV3 = HOV, TP = period)%>%
                arrange(I, TP, J) %>%
                select(I, J, TP, SOV, HOV3, TRK)

# Move this to DataTable to handle big data operations
df_aggregate$HOV3[is.na(df_aggregate$HOV3)] <- 0
df_aggregate$SOV[is.na(df_aggregate$SOV)] <- 0

write.csv(df_aggregate, paste0(output_dir,"\\",out_table), row.names = FALSE) 
#===============================================================================




