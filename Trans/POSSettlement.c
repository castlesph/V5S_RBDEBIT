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
#include "..\TMS\TMS.h"

extern BOOL ErmTrans_Approved;

int inCTOS_SettlementFlowProcess(void)
{
    int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--inCTOS_SettlementFlowProcess--");
	vdDebug_LogPrintf("iOrientation[%d]", iOrientation);
	
	ErmTrans_Approved = FALSE;
	
    vdCTOS_SetTransType(SETTLE);
    
    //display title
    vdDispTransTitle(SETTLE);

    inRet = inCTOS_GetTxnPassword();
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
            vdDebug_LogPrintf("saturn before inCTOS_MultiAPGetData");
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;
			
            vdDebug_LogPrintf("saturn after inCTOS_MultiAPGetData");

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return inRet;
        }
    }

	vdDebug_LogPrintf("saturn inCTOS_CheckAndSelectMutipleMID");

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("saturn inCTOS_ChkBatchEmpty");

	inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;


	vdDebug_LogPrintf("saturn inCTOS_DisplaySettleBatchTotal");

	inRet=inCTOS_DisplaySettleBatchTotal(SETTLE, TRUE);
    if(d_OK != inRet)
        return inRet;

    if(strTCT.fDebitEMVCapable == TRUE)
    {
        if(inCheckTCExist() == d_OK)
        {
            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
            return inRet;
            
            inRet = inBuildAndSendIsoData();
            if(d_OK != inRet)
            return inRet;
        }
    }

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplaySettleSuccess();
	}
	
    inRet = inCTOS_PrintSettleReport();
    if(d_OK != inRet)
        return inRet; 

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;
	
    inRet = inCTOS_SettlementClearBathAndAccum();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
    
    

    return ST_SUCCESS;
}

int inCTOS_SETTLEMENT(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_SettlementFlowProcess();	

	if (isCheckTerminalMP200() == d_OK)
	{
		if (inRet == d_OK || ErmTrans_Approved)
	    {
	    	inRet = inCTOS_PreConnect();
			if (inRet != d_OK)
			{
				vdCTOS_TransEndReset();
				return inRet;
			}

			vdCTOS_SetTransType(SETTLE); // For ERM display -- sidumili
			inCTOSS_UploadReceipt(TRUE, FALSE);
	    }
	}
	
    vdCTOS_TransEndReset();

    vdDebug_LogPrintf("Call TMS %d", inRet);
    //if(d_OK == inRet)
        //inCTOSS_SettlementCheckTMSDownloadRequest();
    return inRet;
}



int inCTOS_SettleAMerchant(void)
{
	int inRet = d_NO;
	char szDisplayMsg[50];
	BOOL fFSRMode = (BOOL)get_env_int("#ORIENTATION");

	vdDebug_LogPrintf("saturn --inCTOS_SettleAMerchant--");
	vdDebug_LogPrintf("fFSRMode[%d]", fFSRMode);
	
	//memset(szDisplayMsg,0x00,sizeof(szDisplayMsg));
	//sprintf(szDisplayMsg,"%s",strMMT[0].szMerchantName);
	//CTOS_LCDTPrintXY(1, 8, "                   ");
	//CTOS_LCDTPrintXY(1, 8, szDisplayMsg);
	//CTOS_Delay(800);
	vdDebug_LogPrintf("saturn inCTOS_SettleAMerchant--hostname=[%s]-merchant=[%s]--",strHDT.szHostLabel,strMMT[0].szMerchantName);
	inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

	inRet=inCTOS_DisplaySettleBatchTotal(SETTLE, TRUE);
    if(d_OK != inRet)
        return inRet;

    if(strTCT.fDebitEMVCapable == TRUE)
    {
        if(inCheckTCExist() == d_OK)
        {
            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
            return inRet;
            
            inRet = inBuildAndSendIsoData();
            if(d_OK != inRet)
            return inRet;
        }
    }

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplaySettleSuccess();
	}
	
    inRet = inCTOS_PrintSettleReport();
    if(d_OK != inRet)
        return inRet; 

	if (strTCT.byERMMode != 0)
		ErmTrans_Approved = TRUE;

    inRet = inCTOS_SettlementClearBathAndAccum();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	if (isCheckTerminalMP200() == d_OK || fFSRMode == TRUE)
	{
		if (inRet == d_OK || ErmTrans_Approved)
	    {
	    	inRet = inCTOS_PreConnect();
			if (inRet != d_OK)
			{
				vdCTOS_TransEndReset();
				return inRet;
			}

			vdCTOS_SetTransType(SETTLE); // For ERM Display
			inCTOSS_UploadReceipt(TRUE, FALSE);
	    }
	}

	return ST_SUCCESS;
}



int inCTOS_SettleAHost(void)
{
	int inRet = d_NO;
	int inNumOfMit = 0,inNum;
	char szErrMsg[30+1];

	vdDebug_LogPrintf("saturn --inCTOS_SettleAHost--");
	
	//memset(szErrMsg,0x00,sizeof(szErrMsg));
	//sprintf(szErrMsg,"SETTLE %s",strHDT.szHostLabel);
	//CTOS_LCDTPrintXY(1, 8, "                   ");
	//CTOS_LCDTPrintXY(1, 8, szErrMsg);
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

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

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
        inRet = inCTOS_MultiAPGetData();
        if(d_OK != inRet)
            return inRet;

        inRet = inCTOS_MultiAPReloadHost();
        if(d_OK != inRet)
            return inRet;

		inRet = inCTOS_SettleAHost();
            return inRet;
    }

    return ST_SUCCESS;
}
#endif

int inCTOS_SettleAllHosts(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;

	vdDebug_LogPrintf("saturn --inCTOS_SettleAllHosts--");
	
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

			vdCTOS_SetDateTime();
			
			vdCTOS_SetTransType(SETTLE);
			inCTOS_SettleAHost();
		}
	}

	vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
    return ST_SUCCESS;
}


int inCTOS_SettleAllOperation(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[50];
	int inAPPID;

    vdCTOS_SetTransType(SETTLE);
    
    //display title
    vdDispTransTitle(SETTLE);

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
		inCTOS_SettleAllHosts();
	}

    return ST_SUCCESS;
}



int inCTOS_SETTLE_ALL(void)
{
    int inRet = d_NO;


	vdDebug_LogPrintf("saturn settle all");
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_SettleAllOperation();

    vdCTOS_TransEndReset();

    //if(d_OK == inRet)
        //inCTOSS_SettlementCheckTMSDownloadRequest();
    return inRet;
}


int inCTOS_BATCH_TOTAL_Process(void)
{
    int inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
       return inRet;

    inRet = inCTOS_SelectHostSetting();
    if (inRet == -1)
        return d_NO;

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


    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return d_NO;

    inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

    //inRet = inCTOS_DisplayBatchTotal();
    inRet = inCTOS_DisplaySettleBatchTotal(BATCH_TOTAL, FALSE);
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

    inRet = inCTOS_SelectHostSetting();
    if (inRet == -1)
        return d_NO;

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


    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return d_NO;

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
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit(); 

    //vdDispTransTitle(BATCH_REVIEW);
    
    inCTOS_BATCH_REVIEW_Process();
    
    vdCTOS_TransEndReset();
    
    return d_OK;
    
}


