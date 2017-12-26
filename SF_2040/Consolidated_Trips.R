library(dplyr)
library(plotly)
library(tidyr)
library(purrr)

path <- "C:\\projects\\Advanced_Projects\\ELToD_DTA\\SF_2040"

################################################################################
files <- dir(paste0(path,"\\Inputs"), pattern = ".CSV")

# Read each of 96 trip tables
for (f in 1:length(files)) {
  df <- read.csv(paste0(path,"\\Inputs\\",files[f]),header = FALSE)
  colnames(df) <- c("I", "J", "TP", "SOV", "HOV3", "TRK")
  df$TP <- f
  print(colSums(df))
  ifelse(f==1, df_all <- df, df_all <- rbind(df_all, df))
  
}

# Check some summaries
df_all %>% group_by(TP) %>% summarise_at(c("SOV","HOV3","TRK"), sum) 
df_all %>% summarise_at(c("SOV","HOV3","TRK"), sum)

          
# Write out trip table
df_all <- df_all %>% arrange(TP, I, J)
write.csv(df_all, paste0(path,"\\TRIP_TABLE.csv"), row.names = FALSE)


################################################################################
EA <- c(1:24, 89:96)
AM <- c(25:36)
MD <- c(37:60)
PM <- c(61:76)
EV <- c(77:88)

# Redistribute fractional trips
diurnal_factors <- read.csv("TimeStep_Distribution.csv") %>%
                   mutate(hour = ceiling(`X.Hours` / 4)) %>%
                   rename(segment = `X.Hours`, period_factors = Direction_1)%>%
                   mutate(period = case_when(.$segment %in% EA ~ "EA",
                          .$segment %in% AM ~ "AM",
                          .$segment %in% MD ~ "MD",
                          .$segment %in% PM ~ "PM",
                          .$segment %in% EV ~ "EV")) %>%
                   group_by(hour) %>%
                   mutate(hourly_factors = period_factors/sum(period_factors)) %>%
                   ungroup() %>%
                   mutate(daily_factors = period_factors/sum(period_factors)) %>%
                   select(segment, hour, period, hourly_factors, period_factors, daily_factors)

# Check factors (hour total to 24, period to 5 and daily to 1)
diurnal_factors %>% summarise_at(vars(hourly_factors:daily_factors),sum)

# Plot the distribution
diurnal_factors %>% plot_ly(x = ~segment) %>% 
  add_trace(y = ~period_factors, name = 'period', type = 'scatter', mode = 'lines+markers', color = ~period) %>%
  add_trace(y = ~daily_factors, name = 'day', type = 'scatter', mode = 'lines') %>%
  add_trace(y = ~hourly_factors, name = 'hour ', mode = 'markers', type = 'scatter') 


# Simplified hourly lookup
getRandomSegment_hour <- function(x) {
  y <- sample(diurnal_factors$segment[which(diurnal_factors$hour == x)], 1,
         prob = diurnal_factors$hourly_factors[which(diurnal_factors$hour == x)])
  return(y)
} 

# Simplified period lookup
getRandomSegment_period <- function(x) {
  y <- sample(diurnal_factors$segment[which(diurnal_factors$period == x)], 1,
         prob = diurnal_factors$period_factors[which(diurnal_factors$period == x)])
  return(y)
} 

# Simplified daily lookup
getRandomSegment_daily <- function(x) {
  y <- sample(diurnal_factors$segment, 1, prob = diurnal_factors$daily_factors)
  return(y)
}

################################################################################
# Process OD with fractional trips
df_fac <- df_all %>% 
          filter(SOV < 1)

# Add Time period
df_fac <- df_fac %>%
          mutate(hour = ceiling(TP / 4)) %>%
          mutate(period = case_when(.$TP %in% EA ~ "EA",
                                    .$TP %in% AM ~ "AM",
                                    .$TP %in% MD ~ "MD",
                                    .$TP %in% PM ~ "PM",
                                    .$TP %in% EV ~ "EV"))

# Make subsets of data to randomly distribute
# Aggregate by hour
df_hour <- df_fac %>% 
      group_by(I, J, period, hour) %>% 
      summarise(SOV = sum(SOV)) %>%
      ungroup()
     
# Aggregate by period (if by hour is less than unit)
df_period <- df_hour %>% 
             filter(SOV < 1) %>% 
             group_by(I, J, period) %>% 
             summarise(SOV = sum(SOV))

# Aggregate by day (if by period is less than unit)
df_day <- df_period %>% 
             filter(SOV < 1) %>% 
             group_by(I, J) %>% 
             summarise(SOV = sum(SOV))

# Check if there are any fractional trips for the day (by OD)
discard <- df_day %>% filter(SOV < 1) %>% group_by(I, J) %>% summarise(SOV = sum(SOV))


df_hour_assigned         <- df_hour %>% filter(SOV >= 1) 
df_hour_assigned$segment <- sapply(df_hour_assigned$hour, getRandomSegment_hour)

df_period_assigned         <- df_period %>% filter(SOV >= 1) 
df_period_assigned$segment <- sapply(df_period_assigned$period, getRandomSegment_period)

df_day_assigned         <- df_day %>% filter(SOV >= 1) 
df_day_assigned$segment <- sapply(df_day_assigned$SOV, getRandomSegment_daily)


df_hour_assigned <- df_hour_assigned %>% ungroup() %>% select(-period,-hour)
df_period_assigned <- df_period_assigned %>% ungroup() %>% select(-period)

df_assigned <- rbind(df_hour_assigned, df_period_assigned, df_day_assigned)


# Summary
all <- df_all %>% filter(SOV < 1) %>% ungroup() %>% summarise(trips = sum(SOV), count = n()) 
hour <- df_hour %>% filter(SOV < 1) %>% ungroup() %>%summarise(trips = sum(SOV), count = n())
period <- df_period %>% filter(SOV < 1) %>% ungroup() %>% summarise(trips = sum(SOV), count = n())
day <- df_day %>% filter(SOV < 1) %>% ungroup() %>% summarise(trips = sum(SOV), count = n())

summary <- rbind(all, hour, period, day)


# Scale the trips
df_assigned_scaled <- df_assigned %>% 
                      mutate(SOV = SOV * (all$trips/ sum(SOV)),
                             HOV3 = 0,
                             TRK = 0) %>%
                      select(I, J, TP = segment, SOV, HOV3, TRK)

df_unfactored_OD <- df_all %>% filter(SOV >= 1)
df_final <- rbind(df_unfactored_OD, df_assigned_scaled)
df_final %>% summarise(trips = sum(SOV), count = n())

write.csv(df_final, paste0(path,"\\TRIP_TABLE_noFactored_trips.csv"), row.names = FALSE)
