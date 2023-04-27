
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>


#include "POSPOWRFAIL.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\myEZLib.h"
#include "..\ui\Display.h"
#include "..\debug\debug.h"
#include "..\Includes\POSHost.h"
#include "..\Comm\V5Comm.h"
#include "..\print\Print.h"
#include "..\includes\POSbatch.h"
#include "..\accum\accum.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Includes\V5IsoFunc.h"

int inCTOS_Readpowrfail(STRUCT_FILE_SETTING *strFile)
{

    return (inMyFile_RecRead(strFile));    
}

int inCTOS_Savepowrfail(STRUCT_FILE_SETTING *strFile)
{
    return (inMyFile_RecSave(strFile));    
}


int inCTOS_Clearpowrfail(char *szFileName)
{
	int inResult;
	
	if((inResult = CTOS_FileDelete(szFileName)) != d_OK)
    {
    	CTOS_Delay(150);
    	if((inResult = CTOS_FileDelete(szFileName)) != d_OK)
	    {
	        return ST_ERROR;
	    }
    }     

	
    return ST_SUCCESS;
	
}


int inCTLOS_Updatepowrfail(short inPowrfailtag)
{
	STRUCT_FILE_SETTING strFile;
	POWRFAIL_REC strTotal;
	int inResult;

	memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(&strTotal, 0x00, sizeof(POWRFAIL_REC));

	strTotal.inPowrfailtag = inPowrfailtag;
	strTotal.srTransPowerRec = srTransRec;

	strcpy(strFile.szFileName, PFR_FIME_NAME);
	
	strFile.bSeekType           = d_SEEK_FROM_EOF;
    strFile.bStorageType        = d_STORAGE_FLASH ;
    strFile.fCloseFileNow       = TRUE;
    strFile.ulRecSize           = sizeof(POWRFAIL_REC);
    strFile.ptrRec              = &strTotal;
    strFile.inSeekCnt           = 0;//fseek 0 from end of file   

	if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) >= 0)
    {
        if (ST_SUCCESS != inCTOS_Clearpowrfail(strFile.szFileName))
			return ST_ERROR;
    }

	if (inPowrfailtag == PFR_IDLE_STATE)
		return ST_SUCCESS;
	
	inResult = inCTOS_Savepowrfail(&strFile);

	vdDebug_LogPrintf("inCTLOS_Updatepowrfail =[%d],[%d]", inResult,inPowrfailtag);

	return inResult;
}

int inCTLOS_Getpowrfail(void)
{
	STRUCT_FILE_SETTING strFile;
	POWRFAIL_REC strTotal;
	int inResult;
	char szFileName[d_BUFF_SIZE];
    int  inFileSize;
	int inRet;
 
    memset(szFileName, 0x00, sizeof(szFileName));
	strcpy(szFileName, PFR_FIME_NAME);
	vdDebug_LogPrintf("inCTLOS_Getpowrfail =[%s]", szFileName);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPGetpowrfail();
	}

	if((inResult = inMyFile_CheckFileExist(szFileName)) < 0)
    {
        return d_NOT_RECORD;
    }

	memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(&strTotal, 0x00, sizeof(POWRFAIL_REC));
    strcpy(strFile.szFileName, PFR_FIME_NAME);    
    strFile.bSeekType           = d_SEEK_FROM_BEGINNING;
    strFile.bStorageType        = d_STORAGE_FLASH ;
    strFile.fCloseFileNow       = TRUE;
    strFile.ulRecSize           = sizeof(POWRFAIL_REC);
    strFile.ptrRec              = &strTotal;
    strFile.inSeekCnt           = 0;//fseek 0 from end of file 

	inResult = inCTOS_Readpowrfail(&strFile);

vdDebug_LogPrintf("inPowrfailtag =[%d]", strTotal.inPowrfailtag);
	

	if (strTotal.inPowrfailtag == PFR_IDLE_STATE)
		return d_NOT_RECORD;

	if (strTotal.inPowrfailtag == PFR_BEGIN_SEND_ISO)
	{
		//before update batch after send ISO, print error reciep and disp error message
#if 0
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

		srTransRec= strTotal.srTransPowerRec;

		inRet = inCTOS_LoadCDTandIIT();

		inRet = inCTOS_VoidSelectHost();

		inRet = inProcessReversalEx(&srTransRec, POST_TXN);

		inRet = inCTOS_inDisconnect();

		inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
#endif		
		/*sidumili: [unneeded]*/
		#if 0
		CTOS_LCDTClearDisplay();
		CTOS_LCDTPrintXY(1, 7, "TRANS NOT SUC.");
		vdDisplayErrorMsg(1, 8, "PLS TRY AGAIN");
		/*sidumili: [unneeded]*/

		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

		srTransRec= strTotal.srTransPowerRec;

		inRet = inCTOS_LoadCDTandIIT();

		inRet = inCTOS_VoidSelectHost();

		inRet = inCTOS_inDisconnect();

		/*sidumili: [unneeded]*/
		#if 0
		inRet = ushCTOS_printErrorReceipt();
		#endif
		/*sidumili: [unneeded]*/

#endif

		inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	}
	
	if (strTotal.inPowrfailtag == PFR_BEGIN_BATCH_UPDATE)
	{
		//befor update batch, update bach & accum, print the lastrecipt
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

		srTransRec= strTotal.srTransPowerRec;

		inRet = inCTOS_LoadCDTandIIT();

		inRet = inCTOS_VoidSelectHost();
	
		inRet = inCTOS_SaveBatchTxn();

    	inRet = inCTOS_UpdateAccumTotal();

		inRet = inCTOS_inDisconnect();
    
		/* sidumili: do not print receipt during powerfail */
		if ((srTransRec.byTransType != BALANCE_INQUIRY) && (srTransRec.byTransType != LOG_ON)){
			inRet = ushCTOS_printReceipt();
		}
		/* sidumili: do not print receipt during powerfail */

		inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	}
	
	if (strTotal.inPowrfailtag == PFR_BATCH_UPDATE_COMPLETE)
	{
		//after update batch, print the lastrecipt
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

		srTransRec= strTotal.srTransPowerRec;

		inRet = inCTOS_LoadCDTandIIT();

		inRet = inCTOS_VoidSelectHost();
		
		/* sidumili: do not print receipt during powerfail */
		if ((srTransRec.byTransType != BALANCE_INQUIRY) && (srTransRec.byTransType != LOG_ON)){
			inRet = ushCTOS_printReceipt();
		}
		/* sidumili: do not print receipt during powerfail */

		inRet = inCTOS_inDisconnect();

		inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	}

	  if (strTotal.inPowrfailtag == PFR_BATCH_SETTLEMENT_PRINT){
	
	
	  memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	  
	  srTransRec= strTotal.srTransPowerRec;
	  
	  //inRet = inCTOS_LoadCDTandIIT();
	  
	  inRet = inCTOS_VoidSelectHost();
	
	  strcpy(srTransRec.szBatchNo, strTotal.srTransPowerRec.szBatchNo);
	
	  //inRet = inCTOS_PrintSettleReport();
	  //inRet = inCTOS_SettlementClearBathAndAccum();
      inRet = inCTOS_PrintSettleReport(strTotal.srTransPowerRec.fManualSettlement);
      inRet = inCTOS_SettlementClearBathAndAccum(strTotal.srTransPowerRec.fManualSettlement);
	  inRet = inCTOS_inDisconnect();
	  
	  
	  inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
	  memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	
	
	  }


	return ST_SUCCESS;
}



