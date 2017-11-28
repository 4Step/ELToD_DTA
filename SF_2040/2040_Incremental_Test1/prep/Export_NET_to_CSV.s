; Do not change filenames or add or remove FILEI/FILEO statements using an editor. Use Cube/Application Manager.
RUN PGM=NETWORK MSG='Export Network'
FILEI NODEI[1] = "SF_TollLinks.NET"
FILEI LINKI[1] = "SF_TollLinks.NET"
FILEO PRINTO[2] = "NODE.CSV"
FILEO PRINTO[1] = "LINK.CSV"
FILEO PRINTO[3] = "TOLL_LINK.CSV"
MERGE RECORD=FALSE

PROCESS PHASE=NODEMERGE  
  ; Print Node File header
  IF(_FIterNode = 0)
    PRINT CSV=T, LIST='N,DTA_Type,VCOST',PRINTO=2 
    _FIterNode = 1
  ENDIF

  ; ELTOD 4.0 C++ code additions
  DTA_Type=0
  IF (N<=229)
    DTA_Type = 99
  ELSE
   ; DTA_Type = N.1.TYPE
    DTA_Type = N.1.DTA_TYPE   ; Test portion of the network
  ENDIF
  
  VCOST = 10

  PRINT CSV=T, LIST=N(10.0L),DTA_Type(5.0L),VCOST(4.0),PRINTO=2

ENDPROCESS

; Print File header
IF(_FIterLink = 0)
  ; Print Link file header
  PRINT CSV=T, LIST='A,B,DISTANCE,CAPACITY,NUM_LANES,FTYPE,FFSPEED,ALPHA,BETA',PRINTO=1
  
  ; Print Toll Link File header
  PRINT CSV=T, LIST='A,B,TOLLSEGNUM,TOLL_POLICY,TOLL,SEG_DISTANCE',PRINTO=3
  _FIterLink = 1
ENDIF
 	 
ALPHA = 0.75
BETA = 4
  
; Print Links
; PRINT CSV=T, LIST=A(10.0L),B(10.0L),1(10.4L),CAPACITY_E_LANE(10.0L),NUM_LANES(5.0L),FTC2(5.0L),FFSPD(5.1L),ALPHA(5.2L),BETA(5.2L),PRINTO=1
PRINT CSV=T, LIST=A(10.0L),B(10.0L),1(10.4L),CAPACITY_E_LANE(10.0L),NUM_LANES(5.0L),FTYPE(5.0L),FFSPD(5.1L),ALPHA(5.2L),BETA(5.2L),PRINTO=1

; Print Toll Links
IF(TOLLSEGNUM > 0 & FTYPE > 0)
  PRINT CSV=T, LIST=A(10.0L),B(10.0L),TOLLSEGNUM(10.0L),TOLL_POLIC(5.0L),TOLL(5.2L),SEG_DISTAN(5.0L), PRINTO=3
ENDIF

PHASE=SUMMARY 
  IF(_ERR=1)
    ABORT MSG='ERROR! Facility Type in network does not match keys. Please check ERROR_LINK.CSV for link list.'
  ENDIF
ENDPHASE
ENDRUN
