/* 
 * File:   V5IsoFunc.h
 * Author: Administrator
 *
 * Created on 2012年8月6日, 下午 9:23
 */

#ifndef V5ISOFUNC_H
#define    V5ISOFUNC_H

#ifdef    __cplusplus
extern "C" {
#endif

#include "../Includes/POSTypedef.h"
#include "../Includes/ISOEnginee.h"


#define BIT_MAP_MAX_ARRAY    	30

#define BIT_MAP_SIZE        	8

#define ISO_SEND_SIZE       	2048
#define ISO_REC_SIZE        	2048

//BDO: Revised reversal function - start -- jzg
#define PRE_TXN 0
#define POST_TXN 1
//BDO: Revised reversal function - end -- jzg

void vdDispTextMsg(char *szTempMsg);
void vdDecideWhetherConnection(TRANS_DATA_TABLE *srTransPara);
int inBuildAndSendIsoData(void);
int inSnedReversalToHost(TRANS_DATA_TABLE *srTransPara, int inTransCode, short shTxnFlag);
int inSaveReversalFile(TRANS_DATA_TABLE *srTransPara, int inTransCode);
int inProcessReversal(TRANS_DATA_TABLE *srTransPara, short shTxnFlag); //BDO: Revised reversal function -- jzg
int inProcessAdviceTrans(TRANS_DATA_TABLE *srTransPara, int inAdvCnt);
int inProcessEMVTCUpload(TRANS_DATA_TABLE *srTransPara, int inAdvCnt);
int inPackSendAndUnPackData(TRANS_DATA_TABLE *srTransPara, int inTransCode);
int inBuildOnlineMsg(TRANS_DATA_TABLE *srTransPara);
int inSetBitMapCode(TRANS_DATA_TABLE *srTransPara, int inTransCode);
int inPackMessageIdData(int inTransCode, unsigned char *uszPackData, char *szMTI);
int inPackPCodeData(int inTransCode, unsigned char *uszPackData, char *szPCode);
void vdModifyBitMapFunc(int inTransCode, int *inBitMap);
int inCheckIsoHeaderData(char *szSendISOHeader, char *szReceISOHeader);
int inSendAndReceiveFormComm(TRANS_DATA_TABLE* srTransPara,
                                unsigned char* uszSendData,
                                 int inSendLen,
                                unsigned char* uszReceData);
int inProcessOfflineTrans(TRANS_DATA_TABLE *srTransPara);
int inAnalyseIsoData(TRANS_DATA_TABLE *srTransPara);

int inCheckHostRespCode(TRANS_DATA_TABLE *srTransPara);
int inCheckTransAuthCode(TRANS_DATA_TABLE *srTransPara);

int inAnalyseChipData(TRANS_DATA_TABLE *srTransPara);
int inAnalyseNonChipData(TRANS_DATA_TABLE *srTransPara);

int inAnalyseReceiveData(void);
int inAnalyseAdviceData(int inPackType);
void vdDiapTransFormatDebug(unsigned char *bSendBuf, int inSendLen);
int inPorcessTransUpLoad(TRANS_DATA_TABLE *srTransPara);

int inPackIsoFunc02(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc03(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc04(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc11(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc12(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc13(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc14(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc22(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc23(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc24(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc25(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc35(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc37(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc38(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc39(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc41(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc42(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc45(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc48(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc52(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc54(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc55(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc56(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc57(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc60(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc61(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc62(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc63(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc64(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);

int inUnPackIsoFunc04(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc12(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc13(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc37(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc38(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc39(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc55(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc57(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);

//sidumili: move to V5IsoFunc.h [inUnPackIsoFunc62]
int inUnPackIsoFunc62(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);

int inPackISOEMVData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inPackISOPayWaveData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inPackISOPayPassData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inCTOS_PackDemoResonse(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData);

void vdInitialISOFunction(ISO_FUNC_TABLE *srPackFunc);

//gcitra
int inPackIsoFunc18(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
//gcitra

int inBDOAutoReversal(TRANS_DATA_TABLE *srTransPara, short shTxnFlag); //BDO: Revised reversal function -- jzg
extern int inCTOSS_ISOEngCheckEncrypt(IN int inHDTid, INOUT unsigned char *uszISOData, INOUT int *inISOLen);
extern int inCTOSS_PackIsoDataNoEncryp(int inHDTid, unsigned char *szTPDU, unsigned char *uszSendData, int inTransCode);
int inBaseRespValidation(TRANS_DATA_TABLE *srOrgTransPara,TRANS_DATA_TABLE *srTransPara);
extern int inPrintISOPacket(unsigned char *pucTitle,unsigned char *pucMessage, int inLen);


/* BDO: Terminal should disconnect if there no pending TC or advise -- jzg */
int inDisconnectIfNoPendingADVICEandUPLOAD(TRANS_DATA_TABLE *srTransPara, int inAdvCnt);
int inCheckReversalExist(void);
int inPOSTErrorMessage(void);
int inCheckPendingTCUploadAndReversal(TRANS_DATA_TABLE *transData);
//int inProcessReversalEx(TRANS_DATA_TABLE *srTransPara, short shTxnFlag);



#ifdef    __cplusplus
}
#endif

#endif    /* V5ISOFUNC_H */

