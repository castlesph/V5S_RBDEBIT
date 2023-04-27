/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>
#include <EMVAPLib.h>
#include <EMVLib.h>


#include "..\Includes\POSTypedef.h"


#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\print\Print.h"
#include "..\Includes\POSHost.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Database\DatabaseFunc.h"
#include "../Debug/Debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\Includes\POSLogon.h"

//tms
#include "..\TMS\TMS.h"
//tms

#define HOST_ONE_BY_ONE

BOOL fAUTOManualSettle = FALSE;
BOOL fGlobalSettleAll = FALSE;
BOOL fSettlement = FALSE;
int gvSettleType; //aaa fix on issue #000210 Terminal displays "Batch empty, skip" on all hosts when trying to settle hosts with no transactions 3 of 6
extern BOOL gblfBatchEmpty; //aaronnino for BDOCLG ver 9.0 fix on issue #0033 and #0034 "settle failed" error response after "comm error" 3 of 8


extern BOOL fAutoSettle;

int inCTOS_SettlementFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(SETTLE);
    
    //display title
    //vdDispTransTitle(SETTLE);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
	
	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

    inRet = inCTOS_SettlementSelectAndLoadHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLEMENT);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return inRet;
        }
    }


    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;

	//inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);
	inMMTReadRecord_SettleAll(srTransRec.HDTid,srTransRec.MITid);

	inRet = inCTOS_ChkBatchEmpty();

	//BDO: Added settlement status to settlement report - start -- jzg
	if(inRet != d_OK)
		strMMT[0].inSettleStatus = 0;
	else
		strMMT[0].inSettleStatus = 2;

	if(d_OK != inRet)
	{	
		inMMTSave(strMMT[0].MMTid);
		return inRet;
	}

		//set must settle flag once settlement wsa triggered
	strMMT[0].fMustSettFlag = CN_TRUE;	
	inMMTSave(strMMT[0].MMTid);
	
//BDO: Added settlement status to settlement report - end -- jzg

//012215
	
    //strDCT.fDebitLogonRequired = VS_TRUE;
    //if(d_OK != inRet){
	//	strDCT.fDebitLogonRequired = VS_FALSE;
	//	return inRet;
    //}
//012215
	
	/********************/	
	//inDCTSave(srTransRec.HDTid);
	/********************/

//debit settlment removal
	
	if ((strTCT.fDebitOnlineSettlment == TRUE) || (inCheckPendingTCUploadAndReversal(&srTransRec) == TRUE)){
    	inRet = inCTOS_PreConnect();
    	if(d_OK != inRet)
        	return inRet;
	}
	
    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
    {

       if(ST_REVERSAL_SETTLE_ERR == inRet) //aaronnino for BDOCLG ver 9.0 fix on issue #00500 and #00501 
	   	      return ST_ERROR;  
		
       //aaronnino for BDOCLG ver 9.0 fix on issue #00241 No Manual Settle/Clear Batch prompt after 3 failed start
       //vdSetErrorMessage("SETTLE FAILED"); //aaronnino for BDOCLG ver 9.0 fix on issue #0033 and #0034 "settle failed" error response after "comm error" 4 of 8
	   vdDisplayErrorMsgResp2(" ", " ", "SETTLE FAILED");
	   vdSetErrorMessage("");
	   
	   if(inRet == ST_NO_CONNECT_ERR)
		  return ST_ERROR; //aaronnino for BDOCLG ver 9.0 fix on issue #00460 Settle failed retries should not count terminal level connection error
			
	   if (strMMT[0].fManualMMTSettleTrigger != FALSE)
	   {
		   strMMT[0].inOnlineMMTSettleTries+=1;
		   inMMTSave(strMMT[0].MMTid);
		   vdDebug_LogPrintf("TEST SETTLE tries=%d	limit=%d", strMMT[0].inOnlineMMTSettleTries, strMMT[0].inOnlineMMTSettleLimit);
		    int inOnlineMMTSettleLimitVal = strMMT[0].inOnlineMMTSettleLimit; 
            if ((inOnlineMMTSettleLimitVal > 98) || (inOnlineMMTSettleLimitVal == 0))
                inOnlineMMTSettleLimitVal = 99;
			
		   if (strMMT[0].inOnlineMMTSettleTries >= inOnlineMMTSettleLimitVal)
		   {
			   fAUTOManualSettle = TRUE;
			   inRet = inAutoManualSettle();
			
			   if (inRet != d_OK)	 
			   {
				   strMMT[0].inSettleStatus = 2;
				   inMMTSave(strMMT[0].MMTid);
				   return inRet;
			   }else{
				   strMMT[0].inSettleStatus = 1;
				   strMMT[0].inOnlineMMTSettleTries = 0;
				   inMMTSave(strMMT[0].MMTid);				 
				   vdSetErrorMessage("");
					//inMMTResetReprintSettle(strMMT[0].MITid);
					inMMTSetReprintSettle(strMMT[0].MMTid);
				   return inRet;
			   }
		   }
	   }
			
	   return inRet;
    }

	   
	//inMMTResetReprintSettle(strMMT[0].MITid);
	inMMTSetReprintSettle(strMMT[0].MMTid);

    inRet = inCTOS_PrintSettleReport(FALSE);
    if(d_OK != inRet)
        return inRet; 

    inRet = inCTOS_SettlementClearBathAndAccum(FALSE);
	//BDO: Added settlement status to settlement report - start -- jzg
	if(inRet != d_OK)
		strMMT[0].inSettleStatus = 2;
	else
		strMMT[0].inSettleStatus = 1;
	inMMTSave(strMMT[0].MMTid);
	//BDO: Added settlement status to settlement report - end -- jzg

    if(d_OK != inRet)
        return inRet;

    //gcitra  
	//vdCTOS_SetTransType(SIGN_ON);
	//vdDispTransTitle(SIGN_ON);
	//inRet = inDoAutoLogon();
	//if(d_OK != inRet)
	//	return inRet;	
	//gcitra


    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
    

	/******************************/
	inRet = inCTOS_DoAutoLogon();
	if(d_OK != inRet)
		return inRet;	
	/******************************/

    return ST_SUCCESS;
}

int inCTOS_SETTLEMENT(void)
{
    int inRet = d_NO;

	fSettlement = TRUE;
	gvSettleType = SINGLE_SETTLE; //aaa fix on issue #000210 Terminal displays "Batch empty, skip" on all hosts when trying to settle hosts with no transactions 4 of 6
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_SettlementFlowProcess();

	inCTOSS_SettlementCheckTMSDownloadRequest();

    if(inRet == d_OK)
		vdSetErrorMessage("");
		
		inCTOS_inDisconnect();
		//if(d_OK != inRet)
		//	return inRet;
		//else
		//	vdSetErrorMessage("");


    vdCTOS_TransEndReset();
	
	fSettlement = FALSE;
    return inRet;
}



int inCTOS_SettleAMerchant(void)
{
	int inRet = d_NO;
	char szDisplayMsg[50];

	memset(szDisplayMsg,0x00,sizeof(szDisplayMsg));
#if 0
        sprintf(szDisplayMsg,"%s",strMMT[0].szMerchantName);
#else
        sprintf(szDisplayMsg,"SETTLE-%s",strHDT.szHostLabel); // sidumili: Issue#:000109 [Display hostname during settle all]
#endif
	CTOS_LCDTPrintXY(1, 8, "                   ");
	CTOS_LCDTPrintXY(1, 8, szDisplayMsg);
	vdDebug_LogPrintf("inCTOS_SettleAMerchant--hostname=[%s]-merchant=[%s]--",strHDT.szHostLabel,strMMT[0].szMerchantName);
	inRet = inCTOS_ChkBatchEmpty();

	//BDO: Added settlement status to settlement report - start -- jzg
	if(inRet != d_OK)
		strMMT[0].inSettleStatus = 0;
	else
		strMMT[0].inSettleStatus = 2;

	//BDO: Added settlement status to settlement report - end -- jzg

	if(d_OK != inRet)
	{	
		inMMTSave(strMMT[0].MMTid);
		return inRet;
	}

	//set must settle flag once settlement wsa triggered
	strMMT[0].fMustSettFlag = CN_TRUE;
	inMMTSave(strMMT[0].MMTid);
	
//	strDCT.fDebitLogonRequired = VS_TRUE;
//    if(d_OK != inRet){
//		strDCT.fDebitLogonRequired = VS_FALSE;
//		return inRet;
//    }
	
	/********************/	
	//inDCTSave(srTransRec.HDTid);
	/********************/

	vdDebug_LogPrintf("inCTOS_SettleAMerchant--MIDD  strTCT.fDebitOnlineSettlment[%d] ",strTCT.fDebitOnlineSettlment);

//debit settlment removal

	#if 1

		if ((strTCT.fDebitOnlineSettlment == TRUE) || (inCheckPendingTCUploadAndReversal(&srTransRec) == TRUE))
			{
				vdDebug_LogPrintf("inCTOS_SettleAMerchant  --- 1");

				inRet = inCTOS_PreConnect();
				if(d_OK != inRet)
				return inRet;
			}
		else
			{
				int inRetVal =0;
			
				inRetVal = inCTOS_InitComm(srTransRec.usTerminalCommunicationMode);
				
				vdDebug_LogPrintf("inCTOS_SettleAMerchant  --- 2 ---  inRetVal[%d]", inRetVal);
			}
		
	#else

	    	inRet = inCTOS_PreConnect();
	    	if(d_OK != inRet)
	        	return inRet;
	
	#endif
	
    inRet = inBuildAndSendIsoData();
     if(d_OK != inRet)
    	{

		 if(ST_REVERSAL_SETTLE_ERR == inRet)//aaronnino for BDOCLG ver 9.0 fix on issue #00500 and #00501 
	   	       return ST_ERROR; 
			
    	  //aaronnino for BDOCLG ver 9.0 fix on issue #00241 No Manual Settle/Clear Batch prompt after 3 failed start
    	  //vdSetErrorMessage("SETTLE FAILED"); //aaronnino for BDOCLG ver 9.0 fix on issue #0033 and #0034 "settle failed" error response after "comm error"
    	  vdDisplayErrorMsgResp2(" ", " ","SETTLE FAILED");
		  vdSetErrorMessage("");
        //inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

         if(inRet == ST_NO_CONNECT_ERR)
            return ST_ERROR; //aaronnino for BDOCLG ver 9.0 fix on issue #00460 Settle failed retries should not count terminal level connection error
		
        vdDisplayErrorMsgResp2(" ", " ", "SETTLE FAILED");
		vdSetErrorMessage("");
		
       	if(inRet == ST_NO_CONNECT_ERR)
           return ST_ERROR; //aaronnino for BDOCLG ver 9.0 fix on issue #00460 Settle failed retries should not count terminal level connection error
			 
		if (strMMT[0].fManualMMTSettleTrigger != FALSE)
		{
			strMMT[0].inOnlineMMTSettleTries+=1;
			inMMTSave(strMMT[0].MMTid);
			vdDebug_LogPrintf("TEST SETTLE tries=%d  limit=%d", strMMT[0].inOnlineMMTSettleTries, strMMT[0].inOnlineMMTSettleLimit);
			 int inOnlineMMTSettleLimitVal = strMMT[0].inOnlineMMTSettleLimit;
            if ((inOnlineMMTSettleLimitVal > 98) || (inOnlineMMTSettleLimitVal == 0))
                 inOnlineMMTSettleLimitVal = 99;
			
			if (strMMT[0].inOnlineMMTSettleTries >= inOnlineMMTSettleLimitVal)
			{
				fAUTOManualSettle = TRUE;
				inRet = inAutoManualSettle();
			 
				if (inRet != d_OK)    
				{
					strMMT[0].inSettleStatus = 2;
					inMMTSave(strMMT[0].MMTid);
					return inRet;
				}else{
					strMMT[0].inSettleStatus = 1;
					strMMT[0].inOnlineMMTSettleTries = 0;
					inMMTSave(strMMT[0].MMTid);				  
                         if(strTCT.fReprintSettleStatus == 0)
                         {
                             inMMTResetReprintSettle(strMMT[0].MITid);
                             strTCT.fReprintSettleStatus=1;
                             inTCTSave(1);
                         }
                         inMMTSetReprintSettle(strMMT[0].MMTid);
					vdSetErrorMessage("");
					return inRet;
				}
			}
		}
			 
		return inRet;
    }

    vdDebug_LogPrintf("1. strTCT.fReprintSettleStatus:(%d)", strTCT.fReprintSettleStatus);
	
    if(strTCT.fReprintSettleStatus == 0)
    {
        inMMTResetReprintSettle(strMMT[0].MITid);
		strTCT.fReprintSettleStatus=1;
		inTCTSave(1);
    }
    inMMTSetReprintSettle(strMMT[0].MMTid);

    vdDebug_LogPrintf("2. strTCT.fReprintSettleStatus:(%d)", strTCT.fReprintSettleStatus);
	
    inRet = inCTOS_PrintSettleReport(FALSE);
    if(d_OK != inRet)
        return inRet; 

    inRet = inCTOS_SettlementClearBathAndAccum(FALSE);
	//BDO: Added settlement status to settlement report - start -- jzg
	if(inRet != d_OK)
		strMMT[0].inSettleStatus = 2;
	else
		strMMT[0].inSettleStatus = 1;
	inMMTSave(strMMT[0].MMTid);
	//BDO: Added settlement status to settlement report - end -- jzg

	vdDebug_LogPrintf("inCTOS_SettleAMerchant-- BBBBB");

    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
       {
       	vdDebug_LogPrintf("inCTOS_SettleAMerchant ---  inCTOS_inDisconnect=!d_OK");
	
		
       	return inRet;
    	}
    else
        vdSetErrorMessage("");

	return ST_SUCCESS;
}



int inCTOS_SettleAHost(void)
{
	int inRet = d_NO;
	int inNumOfMit = 0,inNum;
	char szErrMsg[30+1];

	memset(szErrMsg,0x00,sizeof(szErrMsg));
	sprintf(szErrMsg,"SETTLE %s",strHDT.szHostLabel);
	CTOS_LCDTPrintXY(1, 8, "                   ");
	CTOS_LCDTPrintXY(1, 8, szErrMsg);
	inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
	vdDebug_LogPrintf("inNumOfMit=[%d]-----",inNumOfMit);
	//	inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
	//	vdDebug_LogPrintf("inNumOfMit=[%d]-----",inNumOfMit);
	//	for(inNum =0 ;inNum < inNumOfMit; inNum++)
		{
	//		memcpy(&strMMT[0],&strMMT[inNum],sizeof(STRUCT_MMT));
	//		srTransRec.MITid = strMMT[0].MITid;
	//		strcpy(srTransRec.szTID, strMMT[0].szTID);
	//		strcpy(srTransRec.szMID, strMMT[0].szMID);
	//		memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
	//		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
	/*
					 if ( inMMTReadRecord_SettleAll(srTransRec.HDTid , srTransRec.MITid) != d_OK)
					 {
							 vdSetErrorMessage("LOAD MMT ERR");
							 return(d_NO);
					}
	*/			  
		vdDebug_LogPrintf("HDTid=[%d]-MITid=[%d]----",srTransRec.HDTid,srTransRec.MITid);
		inMMTReadRecord_SettleAll(srTransRec.HDTid , srTransRec.MITid) ;

	    inRet = inCTOS_SettleAMerchant();
		if (d_OK != inRet)
		{
		
			inCTOS_inDisconnect();//FIX for COMM INIT ERROR ON settlement retry
			memset(szErrMsg,0x00,sizeof(szErrMsg));
		    if (inGetErrorMessage(szErrMsg) > 0)
		    {
		        vdDisplayErrorMsg(1, 8, szErrMsg);
		    }
			vdSetErrorMessage("");

			return(ST_ERROR); //Auto-settlement: return settlement status for each merchant -- jzg
		}
    }

	inRet = inDoAutoLogon();
	vdSetErrorMessage("");
//issue-00237
	//if(d_OK != inRet)
		//return inRet;	
//237

	return ST_SUCCESS;
}

int inCTOS_SettleAMerchantHost(void)
{
    int inRet = d_NO;
    int inNumOfMit = 0,inNum;
    char szErrMsg[30+1];

		vdDebug_LogPrintf("inCTOS_SettleAMerchantHost inHostIndex=[%d], szAPName=[%s]", strHDT.inHostIndex, strHDT.szAPName);
		
    memset(szErrMsg,0x00,sizeof(szErrMsg));
    sprintf(szErrMsg,"SETTLE %s",strHDT.szHostLabel);
    CTOS_LCDTPrintXY(1, 8, "									 ");
    CTOS_LCDTPrintXY(1, 8, szErrMsg);
    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
    vdDebug_LogPrintf("inNumOfMit=[%d]-----",inNumOfMit);
    for(inNum =0 ;inNum < inNumOfMit; inNum++)
    {
        memcpy(&strMMT[0],&strMMT[inNum],sizeof(STRUCT_MMT));
        srTransRec.MITid = strMMT[0].MITid;
        strcpy(srTransRec.szTID, strMMT[0].szTID);
        strcpy(srTransRec.szMID, strMMT[0].szMID);
        memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
        
        vdCTOS_SetTransType(SETTLE);
        
        inRet = inCTOS_SettleAMerchant();
        if (d_OK != inRet)
        {
            memset(szErrMsg,0x00,sizeof(szErrMsg));
            if (inGetErrorMessage(szErrMsg) > 0)
            {
                vdDisplayErrorMsg(1, 8, szErrMsg);
            }
            vdSetErrorMessage("");
        }
    }

    return ST_SUCCESS;
}


#ifdef HOST_ONE_BY_ONE
int inCTOS_SettleAllHosts(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];

    vdCTOS_SetTransType(SETTLE);
    
    //display title
    vdDispTransTitle(SETTLE);

    /*inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;*/

	if (inMultiAP_CheckSubAPStatus() != d_OK)//only 1 APP or main APP
	{
	    inNumOfHost = inHDTNumRecord();
		vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
		for(inNum =1 ;inNum <= inNumOfHost; inNum++)
		{
			if(inHDTRead(inNum) == d_OK)
			{
				inCPTRead(inNum);
				srTransRec.HDTid = inNum;
        		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
				memset(szBcd, 0x00, sizeof(szBcd));
			    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
			    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
			    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
				
				if (inMultiAP_CheckMainAPStatus() == d_OK)
				{
					//multi AP
			        put_env_int("BDOMMTID",srTransRec.MITid);
					put_env_int("BDOHDTID",srTransRec.HDTid);
					if(fAutoSettle == TRUE)
						inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLE_MERCHANT_ALL);
					else
			        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLE_ALL);
			        if (d_OK == inRet)//Current AP
					{
						inRet = inCTOS_SettleAHost();	
					}
					memset(szErrMsg,0x00,sizeof(szErrMsg));
				    if (inGetErrorMessage(szErrMsg) > 0)
				    {
				        vdDisplayErrorMsg(1, 8, szErrMsg);
				    }
					vdSetErrorMessage("");
					
				}
				else
				{
					// only one AP
					inRet = inCTOS_SettleAHost();
		                return inRet;
				}
			}
		}
	}
	else// Sub APP
    {
    	srTransRec.MITid = get_env_int("BDOMMTID");
		srTransRec.HDTid = get_env_int("BDOHDTID");
		
		inRet = inCTOS_MultiAPReloadHost();
        if(d_OK != inRet)
            return inRet;
		
		inRet = inCTOS_SettleAHost();
            return inRet;
    }

    return ST_SUCCESS;
}
#else
// for AP name settle All host
int inCTOS_SettleAllHosts(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[25];
	int inAPPID;

	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    inNumOfHost = inHDTNumRecord();
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
			if (strcmp(szAPName, strHDT.szAPName)!=0)
			{
				continue;
			}
			
			inCPTRead(inNum);
			srTransRec.HDTid = inNum;
    		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			memset(szBcd, 0x00, sizeof(szBcd));
		    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
		    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
		    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
			
			// only one AP
            //if(fAutoSettle == TRUE)
            //    inCTOS_SettleAMerchantHost();
            //else	
                inCTOS_SettleAHost();
           //aaronnino for BDOCLG ver 9.0 fix on issue #0033 and #0034 "settle failed" error response after "comm error" 5 of 8 start
           //if (gblfBatchEmpty != TRUE)
          // {
              //CTOS_LCDTClearDisplay();
              //CTOS_Sound(1000, 50);
              //CTOS_LCDTPrintXY(1,8,"SETTLE FAILED");
              //CTOS_Delay(1000);
           //}
           //aaronnino for BDOCLG ver 9.0 fix on issue #0033 and #0034 "settle failed" error response after "comm error" 5 of 8 end
		}
	}

	vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
    return ST_SUCCESS;
}
#endif


int inCTOS_SettleAllOperation(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[25];
	int inAPPID;

    vdCTOS_SetTransType(SETTLE);
    
    //display title
    //vdDispTransTitle(SETTLE);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inCTOS_SettleAllHosts();
		inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SETTLE_ALL);
	}
	else
	{
//to get correct merchant number - issue 00748		
		inRet = inCTOS_MultiAPGetData();
		if(d_OK != inRet)
			return inRet;
//end

		inCTOS_SettleAllHosts();
	}


    //gcitra  
	//vdCTOS_SetTransType(SIGN_ON);
	//vdDispTransTitle(SIGN_ON);
	
	//inRet = inDoAutoLogon();
	//if(d_OK != inRet)
	//	return inRet;	
	//gcitra
	

    return ST_SUCCESS;
}



int inCTOS_SETTLE_ALL(void)
{
    int inRet = d_NO;

	vdDebug_LogPrintf("inCTOS_SETTLE_ALL BEGIN----");
		
	fSettlement = TRUE;
	
	gvSettleType = MULTI_SETTLE; //aaa fix on issue #000210 Terminal displays "Batch empty, skip" on all hosts when trying to settle hosts with no transactions 5 of 6
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	fGlobalSettleAll = TRUE;
		
    inRet = inCTOS_SettleAllOperation();

    if(d_OK == inRet)
        inCTOSS_SettlementCheckTMSDownloadRequest();

	fGlobalSettleAll = FALSE;
	
    vdCTOS_TransEndReset();
	fSettlement = FALSE;
    return inRet;
}


int inCTOS_BATCH_TOTAL_Process(void)
{
    int inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
       return inRet;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

    inRet = inCTOS_SelectHostSetting();
    if (inRet == -1)
        return;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BATCH_TOTAL);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return inRet;
        }
    }


    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

    inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_DisplayBatchTotal();
    if(d_OK != inRet)
        return inRet;

    vdSetErrorMessage("");

    return d_OK;
    
}

int inCTOS_BATCH_REVIEW_Process(void)
{
    int inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
       return inRet;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

    inRet = inCTOS_SelectHostSetting();
    if (inRet == -1)
        return;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BATCH_REVIEW);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return inRet;
        }
    }


    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

    inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_BatchReviewFlow();    
    if(d_OK != inRet)
        return inRet;

    vdSetErrorMessage("");
    
    return d_OK;
    
}

int inCTOS_BATCH_TOTAL(void)
{
    int inRet;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit(); 

    vdDispTransTitle(BATCH_TOTAL);

    inCTOS_BATCH_TOTAL_Process();
    
    vdCTOS_TransEndReset();

    return d_OK;
    
}

int inCTOS_BATCH_REVIEW(void)
{
    int inRet;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit(); 

    vdDispTransTitle(BATCH_REVIEW);
    
    inCTOS_BATCH_REVIEW_Process();
    
    vdCTOS_TransEndReset();
    
    return d_OK;
    
}


