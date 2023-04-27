#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include "../Includes/POSbatch.h"
#include "../Includes/POSTypedef.h"

#include "../FileModule/myFileFunc.h"
#include "../POWRFAIL/POSPOWRFAIL.h"
#include "..\debug\debug.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\myEZLib.h"
#include "..\ui\Display.h"
#include "..\Includes\POSHost.h"
#include "..\debug\debug.h"
#include "..\Includes\date.h"


int inCTOS_SaveBatchTxn(void)
{
    int inResult ;
    int inSaveType;

//save first transaction date in MMT
//BYTE szCurrDate[8+1] = {0};
	CTOS_RTC SetRTC;
	myDate newdate;
	myDate szCurrDate;

	BYTE szPOSTID[10] = {0};
	
	vdDebug_LogPrintf("inCTOS_SaveBatchTxn");

	vdSaveLastInvoiceNo();
		
	if (wub_str_2_long(strMMT[0].szSettleDate) <= 0){	
	
		vdDebug_LogPrintf("inCTOS_SaveBatchTxn 2");
	
		//memset(szCurrDate, 0x00, sizeof(szCurrDate));
		CTOS_RTCGet(&SetRTC);
		//sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
	
		szCurrDate.year = SetRTC.bYear;
		szCurrDate.month = SetRTC.bMonth;
		szCurrDate.day = SetRTC.bDay;
	
		//newdate = getNewDate(&szCurrDate, strMMT[0].inMustSettleNumOfDays);
		newdate = getNewDate(&szCurrDate, strMMT[0].inMustSettleNumOfDays>0?strMMT[0].inMustSettleNumOfDays:strTCT.inBackupSettNumDays);
		sprintf(strMMT[0].szSettleDate, "%04d%02d%02d" ,newdate.year, newdate.month, newdate.day);	
			
		vdDebug_LogPrintf("settle days %s :: inMustSettleNumOfDays %d strTCT.inBackupSettNumDays %d", strMMT[0].szSettleDate, strMMT[0].inMustSettleNumOfDays, strTCT.inBackupSettNumDays);	
		inMMTSave(strMMT[0].MMTid);
	}
//end save

    vdDebug_LogPrintf("inCTOS_SaveBatchTxn");
	inCTLOS_Updatepowrfail(PFR_BEGIN_BATCH_UPDATE);
	
	/*Saved flag for footer logo -- sidumili*/
	srTransRec.fFooterLogo = strHDT.fPrintFooterLogo && strTCT.fPrintFooterLogo;

	if ((srTransRec.fECRTriggerTran == TRUE) && (strTCT.fSMMode == TRUE) && (srTransRec.byTransType != VOID)){
		inCTOSS_GetEnvDB("PTID", szPOSTID);
		strcpy(srTransRec.szPOSTID, szPOSTID);
	}
	
    vdDebug_LogPrintf("inCTOS_SaveBatchTxn byTransType[%d].byOffline[%d].byUploaded[%d].szPOSTID[%s].fECRTriggerTran[%d].fSMMode[%d]", srTransRec.byTransType, srTransRec.byOffline, srTransRec.byUploaded, srTransRec.szPOSTID, srTransRec.fECRTriggerTran, strTCT.fSMMode);
	
    if(srTransRec.byTransType == VOID && srTransRec.byOffline == CN_TRUE)
    {
        //OFFLINE SALE, then VOID it, the VOID is offline, should delete the OFFLINE SALE Advice
        if(srTransRec.byUploaded == CN_FALSE)
        {
            inCTOSS_DeleteAdviceByINV(srTransRec.szInvoiceNo);
        }
    }

	/*BDO: Add Reload trxn type to add on batch -- sidumili*/
    if((srTransRec.byTransType == SALE) || (srTransRec.byTransType == REFUND) || (srTransRec.byTransType == SALE_OFFLINE) || (srTransRec.byTransType == PRE_AUTH) || (srTransRec.byTransType == RELOAD))
    {

	    
	    strMMT[0].fBatchNotEmpty = TRUE;
		inMMTSave(strMMT[0].MMTid);
		
        inSaveType = DF_BATCH_APPEND;
        if(srTransRec.byOffline == CN_FALSE)
        {
            //use szStoreID to store how much amount fill up in DE4 for VOID
            //either enter tip or not enter tip are ok
            memcpy(srTransRec.szStoreID, srTransRec.szTotalAmount, 6);
        }
    }
    else
    {
        inSaveType = DF_BATCH_UPDATE;
    }
    
    if(srTransRec.byTransType == VOID)
        srTransRec.byVoided = TRUE;
    
	inResult = inDatabase_BatchSave(&srTransRec, inSaveType);
    
    if(inResult != ST_SUCCESS)
    {
    	vdSetErrorMessage("Batch Save Error");
        return ST_ERROR;
    }
            
    if((memcmp(srTransRec.szAuthCode, "Y3",2))  && 
       (srTransRec.byTransType != SALE_OFFLINE) &&
       (srTransRec.byTransType != SALE_ADJUST)  &&
       (srTransRec.byTransType != SALE_TIP)&&		
	   ( srTransRec.byOffline != VS_TRUE))
    {
        inMyFile_ReversalDelete();
    }
        
    if((srTransRec.byTransType == SALE_TIP) || (srTransRec.byTransType == SALE_ADJUST) || (srTransRec.byTransType == SALE_OFFLINE))
    {
        //fix send tip adjust advie two times issue
        inCTOSS_DeleteAdviceByINV(srTransRec.szInvoiceNo);
        inMyFile_AdviceSave(&srTransRec, srTransRec.byPackType);
    }
    //if original txn is offline, then do a online txn piggy back the offline advice, the offline advice been deleted, then void it, the void is offline, need to create advice for this offline void
    else if(srTransRec.byTransType == VOID && srTransRec.byOffline == CN_TRUE && srTransRec.byUploaded == CN_TRUE)
    {
        vdDebug_LogPrintf("Create Advice for offline VOID");
        inMyFile_AdviceSave(&srTransRec, OFFLINE_VOID);
    }
    else if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
    {
       if((srTransRec.byTCuploaded == 0) ||
          (!memcmp(srTransRec.szAuthCode, "Y1",2)) || 
          (!memcmp(srTransRec.szAuthCode, "Y3",2)) 
         )
       {
           if((memcmp(srTransRec.szAuthCode, "Y1",2)) && 
              (memcmp(srTransRec.szAuthCode, "Y3",2)) 
             )
           {
		   	   if(strTCT.fDebitTCUpload == TRUE)
			   {
	               srTransRec.byPackType = TC_UPLOAD;
	               inMyFile_TCUploadFileSave(&srTransRec, srTransRec.byPackType);
		   	   }
           }
           else
           {
               srTransRec.byPackType = SEND_ADVICE;
               inMyFile_AdviceSave(&srTransRec, srTransRec.byPackType);
           }
       }
    }
    else if((srTransRec.byTransType == REFUND || srTransRec.byTransType == PRE_AUTH) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
    {
       
        srTransRec.byPackType = TC_UPLOAD;
        inMyFile_TCUploadFileSave(&srTransRec, srTransRec.byPackType);
           
    }

    return d_OK;
}



int inCTOS_BatchReviewFlow(void)
{
    int inResult;
    int inReadRecResult;
 	int inCount = 0;
	int inBatchRecordNum = 0;
    BYTE key;
	char szPanDisplay[24+1];
    BYTE szTemp1[30+1];
	int  *pinTransDataid = NULL;

	inBatchRecordNum = inBatchNumRecord();

	pinTransDataid = (int*)malloc(inBatchRecordNum * sizeof(int));
	
	inBatchByMerchandHost(inBatchRecordNum, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);

	inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
	
	if(inResult == d_NO)
	{
	    vdDisplayErrorMsg(1, 8, "EMPTY BATCH");
		free(pinTransDataid);
	    return d_OK;
	}

    inCTOS_DisplayBatchRecordDetail(BATCH_REVIEW);
    
	while(1)
	{ 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
			free(pinTransDataid);
			return; 
		} 
		else if((key == d_KBD_DOWN)||(key == d_KBD_DOT))
		{
			inCount++;     
			DebugAddIntX("inCount", inCount);

			if(inCount > inBatchRecordNum - 1)
			{
				vdDisplayErrorMsg(1, 8, "END OF BATCH");
				inCount = inBatchRecordNum - 1;			  		  			  
			}
			else
			{
				inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
				inCTOS_DisplayBatchRecordDetail(BATCH_REVIEW);	
			}
		} 
		else if((key == d_KBD_UP)||(key == d_KBD_00))
		{ 
			inCount--;
			DebugAddIntX("inCount", inCount);

			if(inCount >= 0)
			{
				inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
				inCTOS_DisplayBatchRecordDetail(BATCH_REVIEW);
				
			}
			else
            {
                if(inCount < 0)
                {
                    vdDisplayErrorMsg(1, 8, "END OF BATCH");
                    inCount = 0;				
                }
            }
		} 
	} 	

	free(pinTransDataid);
	return d_OK;

}


