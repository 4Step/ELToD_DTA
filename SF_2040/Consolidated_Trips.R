library(dplyr)

path <- "C:\\projects\\Advanced_Projects\\ELToD_DTA\\SF_2040"
files <- dir(paste0(path,"\\Inputs"), pattern = ".CSV")

for (f in 1:length(files)) {
  df <- read.csv(paste0(path,"\\Inputs\\",files[f]),header = FALSE)
  colnames(df) <- c("I", "J", "TP", "SOV", "HOV3", "TRK")
  df$TP <- f
  ifelse(f==1, df_all <- df, df_all <- rbind(df_all, df))
  
}

# Write out trip table
df_all <- df_all %>% arrange(TP, I, J)
write.csv(df_all, paste0(path,"\\TRIP_TABLE.csv"), row.names = FALSE)


library(foreign)
setwd(path)
# df <- read.dbf("2040_LINK.dbf")
# df_2 <- df %>%
#         mutate(ALPHA = 0.75,	BETA= 4,
#                TOLL_POLICY = ifelse(SEGMENT_ID %in% c(2:10,19, 33:43), 6, 
#                                     ifelse(SEGMENT_ID %in% c(11:18,20-32), 5,
#                                            ifelse(SEGMENT_ID == 1, 1, 0)))) %>%
#         select(A, B, DISTANCE, CAPACITY = CAPACITY_E_,
#                NUM_LANES, FTYPE = FTC2, FFSPEED = FFSPD, 
#                ALPHA,	BETA, TOLL_SEGNUM = SEGMENT_ID,
#                TOLL_POLICY, TOLL_TYPE,	TOLL = CARTOLL, 
#                DIR1, DIR2, DIR3, DIR4)

node <- read.dbf("2040_NODE.dbf")
