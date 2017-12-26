; Do not change filenames or add or remove FILEI/FILEO statements using an editor. Use Cube/Application Manager.
RUN PGM=NETWORK
FILEI LINKI[1] = "DAILY_LOADED_3.net"
FILEI LINKI[2] = "Toll_Link.dbf"
FILEO NETO = "SERPM_TOLLLINKS.net"

ENDRUN