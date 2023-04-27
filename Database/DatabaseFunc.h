
#ifndef _DATABASEFUNC__H
#define	_DATABASEFUNC__H

#ifdef	__cplusplus
extern "C" {
#endif
#include "../Database/dct.h"
#include "../Database/pas.h" //Auto-settlement: added AST table -- jzg
//Added other database headers - start -- jzg
#include "../Database/bvt.h"
//Added other database headers - end -- jzg
/*albert - start - July2014 - EFTSEC Server*/ 
#include "../Database/eft.h"
/*albert - end - July2014 - EFTSEC Server*/ 
#include "../Includes/POSTypedef.h"

#include "../Includes/Trans.h"
#include "../FileModule/myFileFunc.h"

//请注意，需要区分文件名的大小写
#define DB_TERMINAL "/home/ap/pub/TERMINAL.S3DB"
#define DB_BATCH	"./fs_data/V5S_RBDEBIT.S3DB"
#define DB_EMV	"/home/ap/pub/EMV.S3DB"
#define DB_COM	"/home/ap/pub/COM.S3DB"

#define DB_MULTIAP	"/home/ap/pub/MULTIAP.S3DB"
#define DB_WAVE		"/home/ap/pub/WAVE.S3DB"
#define DB_ERM	"/home/ap/pub/ERM.S3DB"
#define DB_MULTIAP_JOURNAL "/home/ap/pub/MULTIAP.S3DB-journal"
#define DB_INVOICE "/home/ap/pub/INVOICE.S3DB"



int inAIDNumRecord(void);
int inHDTNumRecord(void);
int inTCTNumRecord(void);
int inCDTNumRecord(void);
int inRDTNumRecord(void);
int inEMVNumRecord(void);
int inAIDNumRecord(void);
int inMSGNumRecord(void);
int inTLENumRecord(void);
int inIITNumRecord(void);
int inMITRead(int inSeekCnt);

int inCPTRead(int inSeekCnt);
int inCPTSave(int inSeekCnt);

int inPCTRead(int inSeekCnt);
int inPCTSave(int inSeekCnt);

int inHDTRead(int inSeekCnt);
int inHDTSave(int inSeekCnt);
int inCDTRead(int inSeekCnt);
int inCDTReadMulti(char *szPAN, int *inFindRecordNum);
int inCDTMAX(void);

int inEMVRead(int inSeekCnt);
int inAIDRead(int inSeekCnt);
int inAIDReadbyRID(int inSeekCnt, char * PRid);
int inTCTRead(int inSeekCnt);
int inTCTSave(int inSeekCnt);
int inTCPRead(int inSeekCnt);
int inTCPSave(int inSeekCnt);
int inMSGRead(int inSeekCnt);
int inTLERead(int inSeekCnt);
int inTLESave(int inSeekCnt);
int inIITRead(int inSeekCnt);
int inIITSave(int inSeekCnt);

int inCSTNumRecord(void);
int inCSTRead(int inSeekCnt);
int inCSTSave(int inSeekCnt);

int inMMTReadRecord(int inHDTid,int inMITid);

int inMMTNumRecord(void);

int inMMTReadNumofRecords(int inSeekCnt,int *inFindRecordNum);

int inMMTSave(int inSeekCnt);

int inPITNumRecord(void);
int inPITRead(int inSeekCnt);
int inPITSave(int inSeekCnt);

int inWaveAIDRead(int inSeekCnt);
int inWaveAIDNumRecord(void);
int inWaveEMVNumRecord(void);
int inWaveEMVRead(int inSeekCnt);



//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated start 2 of 5
#if 0
int inMSGResponseCodeRead(char* szMsg, int inMsgIndex, int inHostIndex);
#else
int inMSGResponseCodeRead(char* szMsg, char* szMsg2, char* szMsg3, int inMsgIndex, int inHostIndex);
#endif
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated end 2 of 5


int inDatabase_BatchDeleteHDTidMITid(void);
int inDatabase_BatchDelete(void);
int inDatabase_BatchInsert(TRANS_DATA_TABLE *transData);
int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inDatabase_BatchRead(TRANS_DATA_TABLE *transData, int inSeekCnt);
int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inDatabase_BatchSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo);
int inDatabase_BatchCheckDuplicateInvoice(char *hexInvoiceNo);
int inBatchNumRecord(void);
int inDatabase_BatchReadByHostidAndMITid(TRANS_DATA_TABLE *transData,int inHDTid,int inMITid);
int inBatchByMerchandHost(int inNumber, int inHostIndex, int inMerchIndex, char *szBatchNo, int *inTranID);
int inDatabase_BatchReadByTransId(TRANS_DATA_TABLE *transData, int inTransDataid);


int inHDTReadHostName(char szHostName[][100], int inCPTID[]);

int inMultiAP_Database_BatchRead(TRANS_DATA_TABLE *transData);
int inMultiAP_Database_BatchUpdate(TRANS_DATA_TABLE *transData);
int inMultiAP_Database_BatchDelete(void);
int inMultiAP_Database_BatchInsert(TRANS_DATA_TABLE *transData);

int inMultiAP_Database_EMVTransferDataInit(void);
int inMultiAP_Database_EMVTransferDataWrite(USHORT usDataLen, BYTE *bEMVData);
int inMultiAP_Database_EMVTransferDataRead(USHORT *usDataLen, BYTE *bEMVData);

int inMultiAP_Database_COM_Read(void);
int inMultiAP_Database_COM_Save(void);
int inMultiAP_Database_COM_Clear(void);
int inMMTReadRecord_SettleAll(int inHDTid,int inMITid);

void vdSetJournalModeOff(void);

int inNMTReadNumofRecords(int *inFindRecordNum);

int inNMTReadRecord(int NMTID);
int inDatabase_InvoiceNumInsert(TRANS_DATA_TABLE *transData);
int inDatabase_InvoiceNumSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo);
int inDatabase_InvoiceNumDelete(int HDTid, int MITid);

int inMMTResetReprintSettle(int inMITid);
int inMMTSetReprintSettle(int inMMTid);
int inERMAP_Database_BatchDelete(void);
int inERMAP_Database_BatchRead(ERM_TransData *strERMTransData);
int inERMAP_Database_BatchInsert(ERM_TransData *strERMTransData);

int inHDTReadByApname(char *szAPName);
/*albert - start - 20161202 - Reprint of Detail Report for Last Settlement Report*/
int inDatabase_BackupDetailReport(int HDTid, int MITid);
int inDatabase_DeleteDetailReport(int HDTid, int MITid);
int inBatchByDetailReport(int inNumber, int inHostIndex, int inMerchIndex, int *inTranID);
int inDatabase_ReadDetailReport(TRANS_DATA_TABLE *transData, int inTransDataid);
int inBackupDetailReportNumRecord(void);
/*albert - end - 20161202 - Reprint of Detail Report for Last Settlement Report*/
int inEMVReadData(int inSeekCnt);	
int inTCTReadLogonFailedFlag(int inSeekCnt);
int inTCTSaveLogonFailedFlag(int inSeekCnt);

int inDatabase_TerminalOpenDatabaseEx(const char *szDBName);
int inDatabase_TerminalCloseDatabase(void);
int inHDTReadEx(int inSeekCnt);
int inHDTSaveEx(int inSeekCnt);
int inMMTSaveEx(int inSeekCnt);
int inBatchByMerchandHostEx(int inNumber, int inHostIndex, int inMerchIndex, char *szBatchNo, int *inTranID);
int inDatabase_BatchReadByTransIdEx(TRANS_DATA_TABLE *transData, int inTransDataid);
int inIITReadEx(int inSeekCnt);
int inCDTReadEx(int inSeekCnt);
int inCSTReadEx(int inSeekCnt);
int inCPTReadEx(int inSeekCnt);
int inMultiAP_Database_BatchReadEx(TRANS_DATA_TABLE *transData);
int inMMTReadRecordEx(int inHDTid,int inMITid);
int inBatchNumRecordEx(void);
int inHDTReadDataEx(int inSeekCnt);

int inIITSaveEx(int inSeekCnt);
int inFLGReadEx(int inSeekCnt);
int inTCTReadEx(int inSeekCnt);



#endif	/* _DATABASEFUNC__H */

