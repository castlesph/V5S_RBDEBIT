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
#include "..\Debug\Debug.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSDebit.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Pinpad\Pinpad.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

BOOL fECRApproved;

extern BOOL fPinAlreadyEntered;
extern BOOL ErmTrans_Approved;
extern int inSendECRResponseFlag;


int inCTOS_DebitSaleFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	TRANS_DATA_TABLE srLocalTransRec;
	
    BYTE szTitle[20];
    BYTE szDisMsg[528];

	vdDebug_LogPrintf("saturn --inCTOS_DebitSaleFlowProcess--");
	vdDebug_LogPrintf("saturn iOrientation[%d]", iOrientation);
	
	fECRApproved=FALSE;
	
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(SALE);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
	//display title
	//vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	vdDebug_LogPrintf("saturn --get password--");
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn select debit");
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

#if 1
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
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
		
		inRet = inCTOS_MultiAPReloadTable();
		if(d_OK != inRet)
			return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}
#endif
	
	vdDebug_LogPrintf("saturn --select account type");
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --multiple MID--");

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --check issuer enable--");
	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --checktranallowed--");
	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --get invoice--");
	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);

#if 1
  	if((strTCT.fAutoDebitLogon == TRUE) && (strTCT.fDemo==VS_FALSE))
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("SATURN strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("SATURN szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);

		if(memcmp(strTCT.szLogonDate, szCurrentTimeBCD, 2) != 0)
		{
            //inCTOS_LocalAPSaveData();
            //if(d_OK != inRet)
            //    return inRet;
            
			if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();
            
            memset(&srLocalTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            memcpy(&srLocalTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));
            
            vdCTOS_SetTransType(LOG_ON);						   
            vdDispTransTitle(LOG_ON);

            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
		
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;

			
		    vdDebug_LogPrintf("SATURN AFTER PROCESS LOGON");
			
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(SALE);						   
            //vdDispTransTitle(SALE);

			//memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);
			//strcpy(szDisMsg, szTitle);
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "PROCESSING...");
			//usCTOSS_LCDDisplay(szDisMsg);
			//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPGetData 1");
			
            //inRet = inCTOS_LocalAPGetData();            
            //if(d_OK != inRet)
            //    return inRet;

            memcpy(&srTransRec, &srLocalTransRec, sizeof(TRANS_DATA_TABLE));
	
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPReloadTable 1");
            //inRet = inCTOS_LocalAPReloadTable();
            //if(d_OK != inRet)
            //    return inRet;
            
		}
	}	
#endif

	//vdDebug_LogPrintf("saturn --select account type");
	//inRet = inCTOS_SelectAccountType();
	//if(d_OK != inRet)
	//	return inRet;

	vdDebug_LogPrintf("saturn --get base amount--");
	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("SATURN AFTER inCTOS_GetTxnBaseAmount");
	
	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

	#if 0  //tine - testing only
	inInitializePinPad();

    else
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;	
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
	{
	     if(fGetECRTransactionFlg())
         {
              inRet = inMultiAP_Database_BatchInsert(&srTransRec);
              if(d_OK != inRet)
              {
                   vdDebug_LogPrintf("saturn test 1=[%d]", inRet);
                   vdSetErrorMessage("MultiAP BatchInsert ERR");
                   return inRet;
              }   
         }
	     return inRet;
	}
	vdDebug_LogPrintf("saturn --emv processing--");
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
		return inRet;	

	if (fPinAlreadyEntered == FALSE)
	{
	    
		vdDebug_LogPrintf("saturn inInitializePinPad");
		inInitializePinPad();
		
//		inRet = inGetIPPPinEx();
                inRet = inGetIPPPin();
                
		vdDebug_LogPrintf("sturn inGetIPPPinEx result1 %d", inRet);
		if(d_OK != inRet)
		return inRet;	
	}
	
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);

	//usCTOSS_BackToProgress(" ");


	vdDebug_LogPrintf("saturn --pre connect--");
	
    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }
	
	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
	{
	/*
		if ((fGetECRTransactionFlg()) && (strcmp((char *)srTransRec.szRespCode, "00") !=  0))
			{
				 inMultiAP_ECRSendSuccessResponse();
			}
       */
	     return inRet;
	}
	
	vdDebug_LogPrintf("saturn --AFTER inBuildAndSendIsoData");
	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;


	
	vdDebug_LogPrintf("saturn --AFTER inCTOS_SaveBatchTxn");
	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

	vdDebug_LogPrintf("saturn --AFTER inCTOS_UpdateAccumTotal");

	vdDebug_LogPrintf("inCTOS_WaveFlowProcess: inMultiAP_ECRSendSuccessResponse -- END, inret=[%d]", inRet);
	inRet = inMultiAP_ECRSendSuccessResponse();

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;
	}

	//fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	
	vdDebug_LogPrintf("saturn --BEFORE  PRINT RECEIPT");
	inRet = ushCTOS_printReceipt();
	if(d_OK != inRet)
		return inRet;
	else
    {
         if(get_env_int("#ORIENTATION")==1)
         {  
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }
         vdSetErrorMessage("");
    }

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	//inSendECRResponseFlag = 0;
	//put_env_int("ECRRESP",0);
	//vdDebug_LogPrintf("AAA<<>>> inSendECRResponseFlag is ECRRESP[%d]", get_env_int("ECRRESP"));
	
	return d_OK;
}


int inCTOS_DebitSale(void)
{
    int inRet = d_NO;


	vdDebug_LogPrintf("saturn --inCTOS_DebitSale--");
	
	//CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();	

	vdDebug_LogPrintf("saturn call debitsaleflowprocess");
    inRet = inCTOS_DebitSaleFlowProcess();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

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

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}

    if(fGetECRTransactionFlg())
    {
         inMultiAP_ECRSendSuccessResponse();
    }	
	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DebitBalanceInquiryFlowProcess(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	TRANS_DATA_TABLE srLocalTransRec;

    BYTE szTitle[20];
    BYTE szDisMsg[528];

	srTransRec.fDebit=TRUE;

	vdCTOS_SetTransType(BAL_INQ);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(300);
	
	//display title
	//vdDispTransTitle(BAL_INQ);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BAL_INQ);
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
			
			inRet = inCTOS_MultiAPReloadTable();
			if(d_OK != inRet)
				return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}

	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);

 	if(strTCT.fAutoDebitLogon == TRUE)
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);
		
		if(memcmp(strTCT.szRequestKeyDate, szCurrentTimeBCD, 2) != 0)
		{
            //inCTOS_LocalAPSaveData();
            //if(d_OK != inRet)
            //    return inRet;

            memset(&srLocalTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            memcpy(&srLocalTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));

			
			if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();

            vdCTOS_SetTransType(REQUEST_TERMINAL_KEY);						   
            vdDispTransTitle(REQUEST_TERMINAL_KEY);

			inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
			
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;
            
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(BAL_INQ);						   
            vdDispTransTitle(BAL_INQ);
            
            //inRet = inCTOS_LocalAPGetData();
            
            //if(d_OK != inRet)
            //    return inRet;
            
            memcpy(&srTransRec, &srLocalTransRec, sizeof(TRANS_DATA_TABLE));

			vdDebug_LogPrintf("saturn inCTOS_DebitBalanceInquiryFlowProcess -CDTid[%d]--IITid[%d]--",srTransRec.CDTid,srTransRec.IITid);
			
            //inRet = inCTOS_LocalAPReloadTable();
            //if(d_OK != inRet)
                //return inRet;
		}
	}

	//inRet = inCTOS_SelectAccountType();
	//if(d_OK != inRet)
	//	return inRet;

	#if 0
	inInitializePinPad();
	
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;			
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;

	//inRet = inCTOS_PreConnect();
	//if(d_OK != inRet)
		//return inRet;

	//inRet = inCTOS_DisplayCardNum();
	//if(d_OK != inRet)
	//	return inRet;
	
	inRet = inCTOS_EMVProcessing();	
	if(d_OK != inRet)
		return inRet;	
	
	if (fPinAlreadyEntered == FALSE)
	{
		inInitializePinPad();
		
//		inRet = inGetIPPPinEx();
		inRet = inGetIPPPin();
		vdDebug_LogPrintf("sturn inGetIPPPinEx result2 %d", inRet);
		if(d_OK != inRet)
		return inRet;	
	}

	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
		return inRet;
	
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);

    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }

	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

	inRet = InDisplayBalance();
	if(d_OK != inRet)
		return inRet;
    else
        vdSetErrorMessage("");		

	inPrintBalanceInqReceipt(); // Print Balance Receipt
	
	#if 0
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");
    #endif
	
	return d_OK;
}

int inCTOS_DebitBalInq(void)
{
    int inRet = d_NO;

	CTOS_LCDTClearDisplay();
	
    //if (inMultiAP_CheckSubAPStatus() != d_OK)
    //    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	
    inRet = inCTOS_DebitBalanceInquiryFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DebitLogonFlowProcess(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

    srTransRec.fDebit=TRUE;
	
	vdCTOS_SetTransType(LOG_ON);

    if(get_env_int("BANCNET_TESTKEY") == 1)
	    vdSetDummyMKey();
	
	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
	//display title
	//vdDispTransTitle(LOG_ON);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	inTCTRead(1);
	
	strCDT.HDTid=strTCT.inLogonHostIndex;

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_LOGON);
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
			
			inRet = inCTOS_MultiAPReloadTable();
			if(d_OK != inRet)
				return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}

    srTransRec.fDebit=TRUE;

	//inRet = inCTOS_CheckAndSelectMutipleMID();
	//if(d_OK != inRet)
		//return inRet;

	inRet = inCTOS_PreConnect();
	if(d_OK != inRet)
		return inRet;

	//inRet = inCTOS_CheckIssuerEnable();
	//if(d_OK != inRet)
		//return inRet;

	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	inRet = inProcessLogon();
	if(d_OK != inRet)
		return inRet;

	//vdSetDummyMKey(); //sidumili: Dummy Master Key (to be comment or remove, testing purpose only)
	
	return d_OK;
}

int inCTOS_DebitLogon(void)
{
    int inRet = d_NO;

	CTOS_LCDTClearDisplay();
	
    //if (inMultiAP_CheckSubAPStatus() != d_OK)
    //    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

    inRet = inCTOS_DebitLogonFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DebitSelection(void)
{
	char szChoiceMsg[30 + 1];
	char szHeaderString[24+1];
	int bHeaderAttr = 0x01+0x04, key=0; 

	memset(szHeaderString, 0x00, sizeof(szHeaderString));
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

	strcpy(szHeaderString, "SELECT TRANSACTION");
	strcat(szChoiceMsg,"SALE \n");
	strcat(szChoiceMsg,"BALANCE INQUIRY \n");
	strcat(szChoiceMsg,"LOGON");	

    key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    
    if (key > 0)
    {
        if (key == 1)
        {
			inCTOS_DebitSale();
        }
        else if(key == 2)
        {
            inCTOS_DebitBalInq();			
        }
        else if(key == 3)
        {
            inCTOS_DebitLogon();
        }
    }
	return d_OK;	
}

int inCTOS_SelectAccountType(void)
{
	//BYTE key;
	int inRet;
	char szDebug[40 + 1]={0};
	char szChoiceMsg[30 + 1];
	char szHeaderString[24+1];
	int bHeaderAttr = 0x01+0x04, key=0; 
	
	vdDebug_LogPrintf("saturn inCTOS_SelectAccountType[START]");

	vdDebug_LogPrintf("-->>SELECT ACNT byTransType[%d]", srTransRec.byTransType);

    memset(szHeaderString, 0x00, sizeof(szHeaderString));
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

	//if (2 == strTCT.byTerminalType)
	{
		strcpy(szHeaderString, "SELECT ACCOUNT");
		strcat(szChoiceMsg,"SAVINGS \n");
		strcat(szChoiceMsg,"CURRENT");
		key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);

		
		if (key > 0)
		{
			if (key == 1)
			{
				srTransRec.inAccountType =SAVINGS_ACCOUNT;
				return d_OK;
			}
			if (key == 2)
			{
				srTransRec.inAccountType =CURRENT_ACCOUNT;
				return d_OK;
			}
			
			if (key == d_KBD_CANCEL)
			{
			    if(fGetECRTransactionFlg())
                {
                     vdSetECRResponse(ECR_UNKNOWN_ERR);
					 //inMultiAP_ECRSendSuccessResponse();
                }	
				return -1;
			}

			if (key == 0xFF)
			{
				vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
				CTOS_Beep();
				CTOS_Delay(1500);
                if(fGetECRTransactionFlg())
                {
                     vdSetECRResponse(ECR_TIMEOUT_ERR);
                     //inMultiAP_ECRSendSuccessResponse();
                }	
				return d_TIMEOUT;
			}

			return d_NO;
			
		}
	}
	
	return inRet;
}

int inCTOS_CheckSelectDebit(void)
{
    int key;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
	
    if(strCDT.inType == DEBIT_CARD)
		key=d_OK;
	else
	{
		vdSetErrorMessage("DEBIT CARD ONLY");
		key=ST_ERROR;
	}
	return key;
}

int InDisplayBalance(void)
{
    char szAmount[12+1];
    char szStr[12+1];
    char szTemp[21+1];
    USHORT result;
	BYTE szDisplayBuf[40];
    BYTE   key;
    
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    CTOS_LCDTPrintXY(1, 2, "AVAILABLE BALANCE:");
    
    wub_hex_2_str(srTransRec.szTotalAmount, szAmount, AMT_BCD_SIZE);  
	vdDebug_LogPrintf("1.InDisplayBalance: %s", szAmount);
    memset(szStr, 0x00, sizeof(szStr));
    vdCTOS_FormatAmount("N,NNN,NNN,NNn.nn",szAmount, szStr);
	vdDebug_LogPrintf("2.InDisplayBalance: %s", szStr);
        
	//0500 0001 2300
    //setLCDPrint(6, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szStr)+1)*2,  6, szStr);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "%3s %s", strCST.szCurSymbol, szStr);
   setLCDPrint(3, DISPLAY_POSITION_LEFT, szTemp);


	
   // memset(szDisplayBuf,0x00,sizeof(szDisplayBuf));
   // WaitKey(30);
   
   //strcpy(szDisplayBuf, "BALANCE INQUIRY|");
   //strcat(szDisplayBuf,"BALANCE:");
   //strcat(szDisplayBuf,"|");
   //strcat(szDisplayBuf,szTemp);
   //strcat(szDisplayBuf,"|");
   //strcat(szDisplayBuf, "0"); // sidumili: Issuer Logo for PNG file(Bancnet), start with index 0
   
   //vdDebug_LogPrintf("saturn usCTOSS_ConfirmMenu szDisMsg[%s]", szDisplayBuf);
	   
   //key = usCTOSS_ConfirmInvAndAmt(szDisplayBuf);
   //key = usCTOSS_Confirm(szDisplayBuf);
   //key = usCTOSS_ConfirmOK(szDisplayBuf);
//   key = inDisplayMessageBoxWithButton(1,8,"BALANCE INQUIRY","BALANCE:",szTemp, MSG_TYPE_LOGO); // sidumili: added
   key = usCTOSS_Confirm("PRESS ANY KEY EXIT");

	    vdRemoveCard();

    return d_OK;
}

int inCTOS_DebitSale_Diag(void)
{
    int inRet = d_NO;

	vdDebug_LogPrintf("--inCTOS_DebitSale_Diag--");
	
	CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();	
	
    inRet = inCTOS_DebitSaleFlowProcess_Diag();
	if(inRet == d_OK)
		put_env_int("BANCNETOK",TRUE);
	
	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();
#if 0
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

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}
#endif
	
	if (inRet == d_OK || ErmTrans_Approved)
    {
    	inRet = inCTOS_PreConnect();
		if (inRet != d_OK)
		{
			vdCTOS_TransEndReset();
			return inRet;
		}

		inCTOSS_UploadReceipt(FALSE, FALSE);
    }
	
    vdCTOS_TransEndReset();

    return inRet;
}


int inCTOS_DebitSaleFlowProcess_Diag(void)
{
	int inRet = d_NO;
	int inSavings = 0;
	fECRApproved=FALSE;
	
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(SALE);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
	//display title
	//vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
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
		
		inRet = inCTOS_MultiAPReloadTable();
		if(d_OK != inRet)
			return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	
	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);
	
 	if(strTCT.fAutoDebitLogon == TRUE)
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);
		
		if(memcmp(strTCT.szLogonDate, szCurrentTimeBCD, 2) != 0)
		{
            inCTOS_LocalAPSaveData();
            if(d_OK != inRet)
                return inRet;
            
            vdCTOS_SetTransType(LOG_ON);						   
            vdDispTransTitle(LOG_ON);

            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
		
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;
            
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(SALE);						   
            vdDispTransTitle(SALE);
            
            inRet = inCTOS_LocalAPGetData();            
            if(d_OK != inRet)
                return inRet;
            
            inRet = inCTOS_LocalAPReloadTable();
            if(d_OK != inRet)
                return inRet;
		}
	}	
	
#if 0	
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;
#else
	inSavings = get_env_int("SAVINGS");

	if(inSavings == TRUE)
		srTransRec.inAccountType = SAVINGS_ACCOUNT;
	else
		srTransRec.inAccountType = CURRENT_ACCOUNT;
	
	inRet = inCTOS_GetTxnBaseAmount_Diag();
	if(d_OK != inRet)
		return inRet;
#endif

	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

	#if 0
	inInitializePinPad();
	
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;	
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
#if 0	
	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
		return inRet;
#endif	
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
		return inRet;	
	
	if (fPinAlreadyEntered == FALSE)
	{
		inInitializePinPad();
		
		inRet = inGetIPPPin();
		if(d_OK != inRet)
		return inRet;	
	}
	
    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }
	
	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;
	}

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();
	
	inRet = ushCTOS_printReceipt_Diag();
	if(d_OK != inRet)
		return inRet;

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");
	
	return d_OK;
}

int inCTOS_GetTxnBaseAmount_Diag(void)
{
    char szDisplayBuf[30];
	char szBaseAmount[50+1];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;

	//gcitra-0728
	BYTE bFirstKey;
	//gcitra-0728

	inTCTRead(1);	
    if(strTCT.fRegUSD == 1) 
        inCSTRead(2);
    else
        inCSTRead(1);

	if(srTransRec.fInstallment == TRUE)
		inCTOSS_GetEnvDB("TESTINSTAMT", baAmount);
	else
		inCTOSS_GetEnvDB("TESTTXNAMT", baAmount);
		
	memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%012.0f", atof(baAmount));
    wub_str_2_hex(szTemp,srTransRec.szBaseAmount,12);

	vdDebug_LogPrintf("srTransRec.szBaseAmount[%s]",srTransRec.szBaseAmount);
	DebugAddHEX("BASE AMOUNT",szTemp,6);
	
    return d_OK;
}

// *********************************************************************
// BANCNET CASH_OUT -- sidumili
// *********************************************************************
int inCTOS_DebitCashOut(void)
{
    int inRet = d_NO;

	vdDebug_LogPrintf("--inCTOS_DebitCashOut--");
	
	CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();	
	
    inRet = inCTOS_DebitCashOutFlowProcess();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

	if (isCheckTerminalMP200() == d_OK)
	{
		if (inRet == d_OK || ErmTrans_Approved)
	    {
	    	// Preconnect transferred to inCTOSS_UploadReceipt
	    	/*
	    	inRet = inCTOS_PreConnect();
			if (inRet != d_OK)
			{
				vdCTOS_TransEndReset();
				return inRet;
			}
			*/

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}
		
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DebitCashOutFlowProcess(void)
{
	int inRet = d_NO;	
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	BOOL fFSRMode = (BOOL)get_env_int("#ORIENTATION");	
	
	vdDebug_LogPrintf("--inCTOS_DebitCashOutFlowProcess--");
	
	fECRApproved=FALSE;
	
	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(CASH_OUT);
	vdCTOSS_SetWaveTransType(0);

	vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	
	//display title
	//vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_DEBIT_CASH_OUT);
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
		
		inRet = inCTOS_MultiAPReloadTable();
		if(d_OK != inRet)
			return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);
	
 	if(strTCT.fAutoDebitLogon == TRUE)
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);
		
		if(memcmp(strTCT.szLogonDate, szCurrentTimeBCD, 2) != 0)
		{
            inCTOS_LocalAPSaveData();
            if(d_OK != inRet)
                return inRet;
            
            vdCTOS_SetTransType(LOG_ON);						   
            vdDispTransTitle(LOG_ON);

            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
		
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;
            
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(CASH_OUT);						   
            vdDispTransTitle(CASH_OUT);
            
            inRet = inCTOS_LocalAPGetData();            
            if(d_OK != inRet)
                return inRet;
            
            inRet = inCTOS_LocalAPReloadTable();
            if(d_OK != inRet)
                return inRet;
		}
	}

	//inRet = inCTOS_DisplayServiceFee();
	//if(d_OK != inRet)
		//return inRet;
	
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

	#if 0
	inInitializePinPad();
	
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;	
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
		return inRet;	
	
	if (fPinAlreadyEntered == FALSE)
	{
		inInitializePinPad();
		
		inRet = inGetIPPPin();
//		inRet = inGetIPPPinEx();
		if(d_OK != inRet)
		return inRet;	
	}
	
    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }

	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

    inRet = ushCTOS_ePadSignature();
    if(d_OK != inRet)
        return inRet;

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;
	}

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();
	
	inRet = ushCTOS_printReceipt();
	if(d_OK != inRet)
		return inRet;
	else
    {
         if (fFSRMode == TRUE)
         {  
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }
         vdSetErrorMessage("");
    }

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");
	
	return d_OK;
}

int inCTOS_DisplayServiceFee(void)
{
    CTOS_RTC SetRTC;
    BYTE    szTotalAmt[12+1];
    BYTE    szTempBuf[12+1];
    BYTE    szTempBuf1[12+1]; 
    BYTE    szDisplayBuf[30];
    BYTE    szStr[45];
	BYTE    szStr1[45];
	BYTE szTemp[20];
    BYTE baAmount[20];
	char *strPTR;
	double dbServiceFee = 0.00;
	BYTE szDisMsg[200];
	int inRet = d_NO;

	vdDebug_LogPrintf("--inCTOS_DisplayServiceFee--");
	
    vdDispTransTitle(srTransRec.byTransType);

	memset(szStr, 0x00, sizeof(szStr));
	memset(szStr1, 0x00, sizeof(szStr1));
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", strTCT.szServiceFee, szStr1);
	sprintf(szStr, "%s %s", strCST.szCurSymbol, szStr1);

	/*
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	vduiDisplayStringCenter(3, "AMT OF SERVICE FEE");
	vduiDisplayStringCenter(4, "ADDED TO THIS TXN");
	vduiDisplayStringCenter(6, szStr);
    CTOS_LCDTSelectFontSize(d_FONT_16x30);

	// Confirmation
	if (inConfirmPAN() != d_OK)
		return d_NO;
	*/

	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, "CASHOUT");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "AMT OF SERVICE FEE");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "ADDED TO THIS TXN");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, szStr);
		
	vdDebug_LogPrintf("szDisMsg[%s]", szDisMsg);
	
//	inRet = usCTOSS_ConfirmationUI(szDisMsg);
inRet = usCTOSS_Confirm(szDisMsg);
	vdDebug_LogPrintf("saturn confirm PAN %d", inRet);

	if (inRet != d_OK)
		return d_NO;
	
	dbServiceFee = strtod(strTCT.szServiceFee, &strPTR);	
	if (dbServiceFee > 0)
	{
		memset(szTemp, 0x00, sizeof(szTemp));
		sprintf(szTemp, "%012ld", atol(strTCT.szServiceFee));
		wub_str_2_hex(szTemp,srTransRec.szServiceFee, AMT_ASC_SIZE);
	}

	vdDebug_LogPrintf("SERVICE FEE: [%.0f]", dbServiceFee);
	vdDebug_LogPrintf("SERVICE FEE: [%s]", szTemp);
	
    return(d_OK);
}

int inCTOS_CashIn(void)
{
    int inRet = d_NO;


	vdDebug_LogPrintf("saturn --inCTOS_CashIn--");
	
	//CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();	

	vdDebug_LogPrintf("saturn call inCTOS_CashInFlowProcess");
    inRet = inCTOS_CashInFlowProcess();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

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

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}

    if(fGetECRTransactionFlg())
    {
         inMultiAP_ECRSendSuccessResponse();
    }	
	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_CashInFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	TRANS_DATA_TABLE srLocalTransRec;
	
    BYTE szTitle[20];
    BYTE szDisMsg[528];

	vdDebug_LogPrintf("saturn --inCTOS_DebitSaleFlowProcess--");
	vdDebug_LogPrintf("saturn iOrientation[%d]", iOrientation);
        if(get_env_int("BANCNET_TESTKEY") == 1)
                vdSetDummyMKey();
	fECRApproved=FALSE;
	
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(CASH_IN);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
	//display title
	//vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	vdDebug_LogPrintf("saturn --get password--");
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn select debit");
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

#if 1
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
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
		
		inRet = inCTOS_MultiAPReloadTable();
		if(d_OK != inRet)
			return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}
#endif
	
	vdDebug_LogPrintf("saturn --select account type");
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --multiple MID--");

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --check issuer enable--");
	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --checktranallowed--");
	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --get invoice--");
	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);
	if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();
#if 1
  	if((strTCT.fAutoDebitLogon == TRUE) && (strTCT.fDemo==VS_FALSE))
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("SATURN strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("SATURN szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);

		if(memcmp(strTCT.szRequestKeyDate, szCurrentTimeBCD, 2) != 0)
		{
            //inCTOS_LocalAPSaveData();
            //if(d_OK != inRet)
            //    return inRet;
            
			if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();
            
            memset(&srLocalTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            memcpy(&srLocalTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));
            
            vdCTOS_SetTransType(REQUEST_TERMINAL_KEY);						   
            vdDispTransTitle(REQUEST_TERMINAL_KEY);

            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
		
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;

			
		    vdDebug_LogPrintf("SATURN AFTER PROCESS LOGON");
			
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(SALE);						   
            //vdDispTransTitle(SALE);

			//memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);
			//strcpy(szDisMsg, szTitle);
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "PROCESSING...");
			//usCTOSS_LCDDisplay(szDisMsg);
			//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPGetData 1");
			
            //inRet = inCTOS_LocalAPGetData();            
            //if(d_OK != inRet)
            //    return inRet;

            memcpy(&srTransRec, &srLocalTransRec, sizeof(TRANS_DATA_TABLE));
	
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPReloadTable 1");
            //inRet = inCTOS_LocalAPReloadTable();
            //if(d_OK != inRet)
            //    return inRet;
            
		}
	}	
#endif

	//vdDebug_LogPrintf("saturn --select account type");
	//inRet = inCTOS_SelectAccountType();
	//if(d_OK != inRet)
	//	return inRet;

	vdDebug_LogPrintf("saturn --get base amount--");
	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("SATURN AFTER inCTOS_GetTxnBaseAmount");
	
	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

	#if 0  //tine - testing only
	inInitializePinPad();

    else
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;	
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	vdDebug_LogPrintf("saturn --emv processing--");
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
		return inRet;	

	if (fPinAlreadyEntered == FALSE)
	{
	    
		vdDebug_LogPrintf("saturn inInitializePinPad");
		inInitializePinPad();
		
//		inRet = inGetIPPPinEx();
                inRet = inGetIPPPin();
		vdDebug_LogPrintf("sturn inGetIPPPinEx result1 %d", inRet);
		if(d_OK != inRet)
		return inRet;	
	}

	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
	{
	     if(fGetECRTransactionFlg())
         {
              inRet = inMultiAP_Database_BatchInsert(&srTransRec);
              if(d_OK != inRet)
              {
                   vdDebug_LogPrintf("saturn test 1=[%d]", inRet);
                   vdSetErrorMessage("MultiAP BatchInsert ERR");
                   return inRet;
              }   
         }
	     return inRet;
	}
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);

	//usCTOSS_BackToProgress(" ");


	vdDebug_LogPrintf("saturn --pre connect--");
	
    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }
	
	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
	{
	/*
		if ((fGetECRTransactionFlg()) && (strcmp((char *)srTransRec.szRespCode, "00") !=  0))
			{
				 inMultiAP_ECRSendSuccessResponse();
			}
       */
	     return inRet;
	}
	
	vdDebug_LogPrintf("saturn --AFTER inBuildAndSendIsoData");
	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;


	
	vdDebug_LogPrintf("saturn --AFTER inCTOS_SaveBatchTxn");
	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

	vdDebug_LogPrintf("saturn --AFTER inCTOS_UpdateAccumTotal");

	vdDebug_LogPrintf("inCTOS_WaveFlowProcess: inMultiAP_ECRSendSuccessResponse -- END, inret=[%d]", inRet);
	inRet = inMultiAP_ECRSendSuccessResponse();

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;
	}

	//fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	
	vdDebug_LogPrintf("saturn --BEFORE  PRINT RECEIPT");
	inRet = ushCTOS_printReceipt();
	if(d_OK != inRet)
		return inRet;
	else
    {
         if(get_env_int("#ORIENTATION")==1)
         {  
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }
         vdSetErrorMessage("");
    }

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	//inSendECRResponseFlag = 0;
	//put_env_int("ECRRESP",0);
	//vdDebug_LogPrintf("AAA<<>>> inSendECRResponseFlag is ECRRESP[%d]", get_env_int("ECRRESP"));
	
	return d_OK;
}

int inCTOS_CashOut(void)
{
    int inRet = d_NO;


	vdDebug_LogPrintf("saturn --inCTOS_CashIn--");
	
	//CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();	

	vdDebug_LogPrintf("saturn call inCTOS_CashInFlowProcess");
    inRet = inCTOS_CashOutFlowProcess();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

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

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}

    if(fGetECRTransactionFlg())
    {
         inMultiAP_ECRSendSuccessResponse();
    }	
	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_CashOutFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	TRANS_DATA_TABLE srLocalTransRec;
	
    BYTE szTitle[20];
    BYTE szDisMsg[528];

	vdDebug_LogPrintf("saturn --inCTOS_DebitSaleFlowProcess--");
	vdDebug_LogPrintf("saturn iOrientation[%d]", iOrientation);
	
	fECRApproved=FALSE;
	
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(CASH_OUT);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
	//display title
	//vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	vdDebug_LogPrintf("saturn --get password--");
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn select debit");
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

#if 1
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
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
		
		inRet = inCTOS_MultiAPReloadTable();
		if(d_OK != inRet)
			return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}
#endif
	
	vdDebug_LogPrintf("saturn --select account type");
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --multiple MID--");

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --check issuer enable--");
	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --checktranallowed--");
	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --get invoice--");
	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);

#if 1
  	if((strTCT.fAutoDebitLogon == TRUE) && (strTCT.fDemo==VS_FALSE))
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("SATURN strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("SATURN szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);

		if(memcmp(strTCT.szRequestKeyDate, szCurrentTimeBCD, 2) != 0)
		{
            //inCTOS_LocalAPSaveData();
            //if(d_OK != inRet)
            //    return inRet;
            
			if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();
            
            memset(&srLocalTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            memcpy(&srLocalTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));
            
            vdCTOS_SetTransType(REQUEST_TERMINAL_KEY);						   
            vdDispTransTitle(REQUEST_TERMINAL_KEY);

            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
		
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;

			
		    vdDebug_LogPrintf("SATURN AFTER PROCESS LOGON");
			
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(SALE);						   
            //vdDispTransTitle(SALE);

			//memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);
			//strcpy(szDisMsg, szTitle);
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "PROCESSING...");
			//usCTOSS_LCDDisplay(szDisMsg);
			//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPGetData 1");
			
            //inRet = inCTOS_LocalAPGetData();            
            //if(d_OK != inRet)
            //    return inRet;

            memcpy(&srTransRec, &srLocalTransRec, sizeof(TRANS_DATA_TABLE));
	
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPReloadTable 1");
            //inRet = inCTOS_LocalAPReloadTable();
            //if(d_OK != inRet)
            //    return inRet;
            
		}
	}	
#endif

	//vdDebug_LogPrintf("saturn --select account type");
	//inRet = inCTOS_SelectAccountType();
	//if(d_OK != inRet)
	//	return inRet;

	vdDebug_LogPrintf("saturn --get base amount--");
	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("SATURN AFTER inCTOS_GetTxnBaseAmount");
	
	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

	#if 0  //tine - testing only
	inInitializePinPad();

    else
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;	
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	vdDebug_LogPrintf("saturn --emv processing--");
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
		return inRet;	

	if (fPinAlreadyEntered == FALSE)
	{
	    
		vdDebug_LogPrintf("saturn inInitializePinPad");
		inInitializePinPad();
		
//		inRet = inGetIPPPinEx();
                inRet = inGetIPPPin();
		vdDebug_LogPrintf("sturn inGetIPPPinEx result1 %d", inRet);
		if(d_OK != inRet)
		return inRet;	
	}

	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
	{
	     if(fGetECRTransactionFlg())
         {
              inRet = inMultiAP_Database_BatchInsert(&srTransRec);
              if(d_OK != inRet)
              {
                   vdDebug_LogPrintf("saturn test 1=[%d]", inRet);
                   vdSetErrorMessage("MultiAP BatchInsert ERR");
                   return inRet;
              }   
         }
	     return inRet;
	}
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);

	//usCTOSS_BackToProgress(" ");


	vdDebug_LogPrintf("saturn --pre connect--");
	
    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }
	
	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
	{
	/*
		if ((fGetECRTransactionFlg()) && (strcmp((char *)srTransRec.szRespCode, "00") !=  0))
			{
				 inMultiAP_ECRSendSuccessResponse();
			}
       */
	     return inRet;
	}
	
	vdDebug_LogPrintf("saturn --AFTER inBuildAndSendIsoData");
	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;


	
	vdDebug_LogPrintf("saturn --AFTER inCTOS_SaveBatchTxn");
	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

	vdDebug_LogPrintf("saturn --AFTER inCTOS_UpdateAccumTotal");

	vdDebug_LogPrintf("inCTOS_WaveFlowProcess: inMultiAP_ECRSendSuccessResponse -- END, inret=[%d]", inRet);
	inRet = inMultiAP_ECRSendSuccessResponse();

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;
	}

	//fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	
	vdDebug_LogPrintf("saturn --BEFORE  PRINT RECEIPT");
	inRet = ushCTOS_printReceipt();
	if(d_OK != inRet)
		return inRet;
	else
    {
         if(get_env_int("#ORIENTATION")==1)
         {  
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }
         vdSetErrorMessage("");
    }

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	//inSendECRResponseFlag = 0;
	//put_env_int("ECRRESP",0);
	//vdDebug_LogPrintf("AAA<<>>> inSendECRResponseFlag is ECRRESP[%d]", get_env_int("ECRRESP"));
	
	return d_OK;
}

int inCTOS_CardlessCashOut(void)
{
    int inRet = d_NO;


	vdDebug_LogPrintf("saturn --inCTOS_CardlessCashOut--");
	
	//CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();	

	vdDebug_LogPrintf("saturn call inCTOS_CardlessCashOutFlowProcess");
    inRet = inCTOS_CardlessCashOutFlowProcess();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

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

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}

    if(fGetECRTransactionFlg())
    {
         inMultiAP_ECRSendSuccessResponse();
    }	
	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_CardlessCashOutFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	TRANS_DATA_TABLE srLocalTransRec;
	
    BYTE szTitle[20];
    BYTE szDisMsg[528];

	vdDebug_LogPrintf("saturn --inCTOS_CardlessCashOutFlowProcess--");
	vdDebug_LogPrintf("saturn iOrientation[%d]", iOrientation);
	
	fECRApproved=FALSE;
	
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(CARDLESS_CASH_OUT);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(300);
	
	//display title
	//vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	vdDebug_LogPrintf("saturn --get password--");
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;
	
	vdDebug_LogPrintf("saturn select debit");
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;

	strCDT.HDTid = BANCNET_HOST;

    #if 1
    strCDT.HDTid = BANCNET_HOST;
    
    inRet = inCTOS_SelectHostEx();
    if(d_OK != inRet)
        return inRet;
    #else
	inRet = inCTOS_SelectHostEx();
	if(d_OK != inRet)
		return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
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
		
		inRet = inCTOS_MultiAPReloadTable();
		if(d_OK != inRet)
			return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}
    #endif

    vdDebug_LogPrintf("saturn inCTOS_GetOffRefNoEntry");
    inRet = inCTOS_GetOffRefNoEntry();
    if(d_OK != inRet)
        return inRet;
	
#if 0
	vdDebug_LogPrintf("saturn --select account type");
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;
#endif

	vdDebug_LogPrintf("saturn --multiple MID--");

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --check issuer enable--");
	//inRet = inCTOS_CheckIssuerEnable();
	//if(d_OK != inRet)
		//return inRet;

	vdDebug_LogPrintf("saturn --checktranallowed--");
	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --get invoice--");
	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);

#if 1
  	if((strTCT.fAutoDebitLogon == TRUE) && (strTCT.fDemo==VS_FALSE))
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("SATURN strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("SATURN szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);

		if(memcmp(strTCT.szRequestKeyDate, szCurrentTimeBCD, 2) != 0)
		{
            //inCTOS_LocalAPSaveData();
            //if(d_OK != inRet)
            //    return inRet;
            
			if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();
            
            memset(&srLocalTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            memcpy(&srLocalTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));
            
            vdCTOS_SetTransType(REQUEST_TERMINAL_KEY);						   
            vdDispTransTitle(REQUEST_TERMINAL_KEY);

            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
		
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;

			
		    vdDebug_LogPrintf("SATURN AFTER PROCESS LOGON");
			
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(SALE);						   
            //vdDispTransTitle(SALE);

			//memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);
			//strcpy(szDisMsg, szTitle);
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "PROCESSING...");
			//usCTOSS_LCDDisplay(szDisMsg);
			//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPGetData 1");
			
            //inRet = inCTOS_LocalAPGetData();            
            //if(d_OK != inRet)
            //    return inRet;

            memcpy(&srTransRec, &srLocalTransRec, sizeof(TRANS_DATA_TABLE));
	
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPReloadTable 1");
            //inRet = inCTOS_LocalAPReloadTable();
            //if(d_OK != inRet)
            //    return inRet;
            
		}
	}	
#endif

	//vdDebug_LogPrintf("saturn --select account type");
	//inRet = inCTOS_SelectAccountType();
	//if(d_OK != inRet)
	//	return inRet;

	vdDebug_LogPrintf("saturn --get base amount--");
	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("SATURN AFTER inCTOS_GetTxnBaseAmount");
	
	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --select account type");
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	#if 0  //tine - testing only
	inInitializePinPad();

    else
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;	
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	vdDebug_LogPrintf("saturn --emv processing--");
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
		return inRet;	

	if (fPinAlreadyEntered == FALSE)
	{
	    
		vdDebug_LogPrintf("saturn inInitializePinPad");
		inInitializePinPad();
		
//		inRet = inGetIPPPinEx();
                inRet = inGetIPPPin();
		vdDebug_LogPrintf("sturn inGetIPPPinEx result1 %d", inRet);
		if(d_OK != inRet)
		return inRet;	
	}

	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
	{
	     if(fGetECRTransactionFlg())
         {
              inRet = inMultiAP_Database_BatchInsert(&srTransRec);
              if(d_OK != inRet)
              {
                   vdDebug_LogPrintf("saturn test 1=[%d]", inRet);
                   vdSetErrorMessage("MultiAP BatchInsert ERR");
                   return inRet;
              }   
         }
	     return inRet;
	}

	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);

	//usCTOSS_BackToProgress(" ");


	vdDebug_LogPrintf("saturn --pre connect--");
	
    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }
	
	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
	{
	/*
		if ((fGetECRTransactionFlg()) && (strcmp((char *)srTransRec.szRespCode, "00") !=  0))
			{
				 inMultiAP_ECRSendSuccessResponse();
			}
       */
	     return inRet;
	}
	
	vdDebug_LogPrintf("saturn --AFTER inBuildAndSendIsoData");
	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;


	
	vdDebug_LogPrintf("saturn --AFTER inCTOS_SaveBatchTxn");
	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

	vdDebug_LogPrintf("saturn --AFTER inCTOS_UpdateAccumTotal");

	vdDebug_LogPrintf("inCTOS_WaveFlowProcess: inMultiAP_ECRSendSuccessResponse -- END, inret=[%d]", inRet);
	inRet = inMultiAP_ECRSendSuccessResponse();

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;
	}

	//fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	
	vdDebug_LogPrintf("saturn --BEFORE  PRINT RECEIPT");
	inRet = ushCTOS_printReceipt();
	if(d_OK != inRet)
		return inRet;
	else
    {
         if(get_env_int("#ORIENTATION")==1)
         {  
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }
         vdSetErrorMessage("");
    }

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	//inSendECRResponseFlag = 0;
	//put_env_int("ECRRESP",0);
	//vdDebug_LogPrintf("AAA<<>>> inSendECRResponseFlag is ECRRESP[%d]", get_env_int("ECRRESP"));
	
	return d_OK;
}

int inCTOS_CardlessBalInq(void)
{
    int inRet = d_NO;

	CTOS_LCDTClearDisplay();
	
    //if (inMultiAP_CheckSubAPStatus() != d_OK)
    //    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	
    inRet = inCTOS_CardlessBalanceInquiryFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_CardlessBalanceInquiryFlowProcess(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	TRANS_DATA_TABLE srLocalTransRec;

    BYTE szTitle[20];
    BYTE szDisMsg[528];

	srTransRec.fDebit=TRUE;

	vdCTOS_SetTransType(CARDLESS_BAL_INQ);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(300);
	
	//display title
	//vdDispTransTitle(BAL_INQ);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;

	#if 0
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;
    #else
    inRet = inCTOS_GetOffRefNoEntry(); // new for pre completion module
    if(d_OK != inRet)
        return inRet;
	#endif
	
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	#if 1
	strCDT.HDTid = BANCNET_HOST;
	
	inRet = inCTOS_SelectHostEx();
	if(d_OK != inRet)
		return inRet;
	#else
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BAL_INQ);
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
			
			inRet = inCTOS_MultiAPReloadTable();
			if(d_OK != inRet)
				return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}
    #endif
	
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;
	
#if 0
	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;
#endif

	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);

 	if(strTCT.fAutoDebitLogon == TRUE)
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);
		
		if(memcmp(strTCT.szRequestKeyDate, szCurrentTimeBCD, 2) != 0)
		{
            //inCTOS_LocalAPSaveData();
            //if(d_OK != inRet)
            //    return inRet;

			
            memset(&srLocalTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            memcpy(&srLocalTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));
			
            vdCTOS_SetTransType(REQUEST_TERMINAL_KEY);						   
            vdDispTransTitle(REQUEST_TERMINAL_KEY);

			inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
			
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;
            
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(SALE);						   
            vdDispTransTitle(SALE);
            
            //inRet = inCTOS_LocalAPGetData();
            
            //if(d_OK != inRet)
            //    return inRet;
            
            memcpy(&srTransRec, &srLocalTransRec, sizeof(TRANS_DATA_TABLE));
            inRet = inCTOS_LocalAPReloadTable();
            if(d_OK != inRet)
                return inRet;
		}
	}

	//inRet = inCTOS_SelectAccountType();
	//if(d_OK != inRet)
	//	return inRet;

	#if 0
	inInitializePinPad();
	
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;			
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_PreConnect();
	if(d_OK != inRet)
		return inRet;

	//inRet = inCTOS_DisplayCardNum();
	//if(d_OK != inRet)
	//	return inRet;
	
	inRet = inCTOS_EMVProcessing();	
	if(d_OK != inRet)
		return inRet;	
	
	if (fPinAlreadyEntered == FALSE)
	{
		inInitializePinPad();
		
//		inRet = inGetIPPPinEx();
		inRet = inGetIPPPin();
		vdDebug_LogPrintf("sturn inGetIPPPinEx result2 %d", inRet);
		if(d_OK != inRet)
		return inRet;	
	}
	
	//usCTOSS_BackToProgress(" ");
	
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "PROCESSING...");
	//usCTOSS_LCDDisplay(szDisMsg);
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	//CTOS_Delay(1000);

	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
		return inRet;

	inRet = InDisplayBalance();
	if(d_OK != inRet)
		return inRet;
    else
        vdSetErrorMessage("");		

	inPrintBalanceInqReceipt(); // Print Balance Receipt
	
	#if 0
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");
    #endif
	
	return d_OK;
}

/*BILLS PAYMENT*/
int inCTOS_Payment(void)
{
    int inRet = d_NO;

	vdDebug_LogPrintf("saturn --inCTOS_CashIn--");
	
	//CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();	

	vdDebug_LogPrintf("saturn call inCTOS_CashInFlowProcess");
    inRet = inCTOS_PaymentFlowProcess();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

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

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}

    if(fGetECRTransactionFlg())
    {
         inMultiAP_ECRSendSuccessResponse();
    }	
	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_PaymentFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	TRANS_DATA_TABLE srLocalTransRec;
	
    BYTE szTitle[20];
    BYTE szDisMsg[528];

	vdDebug_LogPrintf("saturn --inCTOS_DebitSaleFlowProcess--");
	vdDebug_LogPrintf("saturn iOrientation[%d]", iOrientation);
	
	fECRApproved=FALSE;
	
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(BILLS_PAYMENT);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(300);
	
	//display title
	//vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	vdDebug_LogPrintf("saturn --get password--");
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn select debit");
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

    inRet = inCTOS_SelectBiller();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetSubscriberNo();
    if(d_OK != inRet)
        return inRet;
	
#if 1
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		vdDebug_LogPrintf("inCTOS_PaymentFlowProcess inMultiAP_CheckMainAPStatus");
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
		if(d_OK != inRet)
			return inRet;
	}
	else
	{
	if (inMultiAP_CheckSubAPStatus() == d_OK)
	{
		vdDebug_LogPrintf("inCTOS_PaymentFlowProcess inMultiAP_CheckSubAPStatus");
		inRet = inCTOS_MultiAPGetData();
		if(d_OK != inRet)
			return inRet;
		
		inRet = inCTOS_MultiAPReloadTable();
		if(d_OK != inRet)
			return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}
#endif

    if(srTransRec.fBillsPaymentCash == VS_FALSE)
    {
        vdDebug_LogPrintf("saturn --select account type");
        inRet = inCTOS_SelectAccountType();
        if(d_OK != inRet)
            return inRet;
    }
	
	vdDebug_LogPrintf("saturn --multiple MID--");

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --check issuer enable--");
	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --checktranallowed--");
	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --get invoice--");
	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);

#if 1
  	if((strTCT.fAutoDebitLogon == TRUE) && (strTCT.fDemo==VS_FALSE))
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("SATURN strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("SATURN szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);

		if(memcmp(strTCT.szRequestKeyDate, szCurrentTimeBCD, 2) != 0)
		{
            //inCTOS_LocalAPSaveData();
            //if(d_OK != inRet)
            //    return inRet;
            
			if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();
            
            memset(&srLocalTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            memcpy(&srLocalTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));
            
            vdCTOS_SetTransType(REQUEST_TERMINAL_KEY);						   
            vdDispTransTitle(REQUEST_TERMINAL_KEY);

            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
		
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;

			
		    vdDebug_LogPrintf("SATURN AFTER PROCESS LOGON");
			
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(SALE);						   
            //vdDispTransTitle(SALE);

			//memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);
			//strcpy(szDisMsg, szTitle);
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "PROCESSING...");
			//usCTOSS_LCDDisplay(szDisMsg);
			//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPGetData 1");
			
            //inRet = inCTOS_LocalAPGetData();            
            //if(d_OK != inRet)
            //    return inRet;

            memcpy(&srTransRec, &srLocalTransRec, sizeof(TRANS_DATA_TABLE));
	
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPReloadTable 1");
            //inRet = inCTOS_LocalAPReloadTable();
            //if(d_OK != inRet)
            //    return inRet;
            
		}
	}	
#endif

	//vdDebug_LogPrintf("saturn --select account type");
	//inRet = inCTOS_SelectAccountType();
	//if(d_OK != inRet)
	//	return inRet;

	vdDebug_LogPrintf("saturn --get base amount--");
	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("SATURN AFTER inCTOS_GetTxnBaseAmount");
	
	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

	#if 0  //tine - testing only
	inInitializePinPad();

    else
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;	
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	vdDebug_LogPrintf("saturn --emv processing--");
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
		return inRet;	

    if(srTransRec.fBillsPaymentCash == VS_FALSE)
    {
        if (fPinAlreadyEntered == FALSE)
        {
            vdDebug_LogPrintf("saturn inInitializePinPad");
            inInitializePinPad();
            
//            inRet = inGetIPPPinEx();
            inRet = inGetIPPPin();
            vdDebug_LogPrintf("sturn inGetIPPPinEx result1 %d", inRet);
            if(d_OK != inRet)
                return inRet;	
        }
    }
	
	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
	{
	     if(fGetECRTransactionFlg())
         {
              inRet = inMultiAP_Database_BatchInsert(&srTransRec);
              if(d_OK != inRet)
              {
                   vdDebug_LogPrintf("saturn test 1=[%d]", inRet);
                   vdSetErrorMessage("MultiAP BatchInsert ERR");
                   return inRet;
              }   
         }
	     return inRet;
	}
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);

	//usCTOSS_BackToProgress(" ");


	vdDebug_LogPrintf("saturn --pre connect--");
	
    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }
	
	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
	{
	/*
		if ((fGetECRTransactionFlg()) && (strcmp((char *)srTransRec.szRespCode, "00") !=  0))
			{
				 inMultiAP_ECRSendSuccessResponse();
			}
       */
	     return inRet;
	}
	
	vdDebug_LogPrintf("saturn --AFTER inBuildAndSendIsoData");
	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;


	
	vdDebug_LogPrintf("saturn --AFTER inCTOS_SaveBatchTxn");
	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

	vdDebug_LogPrintf("saturn --AFTER inCTOS_UpdateAccumTotal");

	vdDebug_LogPrintf("inCTOS_WaveFlowProcess: inMultiAP_ECRSendSuccessResponse -- END, inret=[%d]", inRet);
	inRet = inMultiAP_ECRSendSuccessResponse();

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;
	}

	//fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	
	vdDebug_LogPrintf("saturn --BEFORE  PRINT RECEIPT");
	inRet = ushCTOS_printReceipt();
	if(d_OK != inRet)
		return inRet;
	else
    {
         if(get_env_int("#ORIENTATION")==1)
         {  
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }
         vdSetErrorMessage("");
    }

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	//inSendECRResponseFlag = 0;
	//put_env_int("ECRRESP",0);
	//vdDebug_LogPrintf("AAA<<>>> inSendECRResponseFlag is ECRRESP[%d]", get_env_int("ECRRESP"));
	
	return d_OK;
}

int inCTOS_FundTransfer(void)
{
    int inRet = d_NO;

	vdDebug_LogPrintf("saturn --inCTOS_FundTransfer--");
	
	//CTOS_LCDTClearDisplay();
	
    vdCTOS_TxnsBeginInit();	

	vdDebug_LogPrintf("saturn call inCTOS_FundTransferFlowProcess");
    inRet = inCTOS_FundTransferFlowProcess();

	//vdSetECRTransactionFlg(0); // Fix: Send host message depends on response code -- sidumili
    //inMultiAP_ECRSendSuccessResponse();

    inCTOS_inDisconnect();

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

			inCTOSS_UploadReceipt(FALSE, FALSE);
	    }
	}

    if(fGetECRTransactionFlg())
    {
         inMultiAP_ECRSendSuccessResponse();
    }	
	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_FundTransferFlowProcess(void)
{
	int inRet = d_NO;
	int iOrientation = get_env_int("#ORIENTATION");
	TRANS_DATA_TABLE srLocalTransRec;
	
    BYTE szTitle[20];
    BYTE szDisMsg[528];

	vdDebug_LogPrintf("saturn --inCTOS_DebitSaleFlowProcess--");
	vdDebug_LogPrintf("saturn iOrientation[%d]", iOrientation);
	
	fECRApproved=FALSE;
	
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

	srTransRec.fDebit=TRUE;
		
	vdCTOS_SetTransType(FUND_TRANSFER);
	vdCTOSS_SetWaveTransType(0);

	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(300);
	
	//display title
	//vdDispTransTitle(SALE);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	vdDebug_LogPrintf("saturn --get password--");
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetCardFieldsNoEMV();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn select debit");
	inRet = inCTOS_CheckSelectDebit();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
#if 0
    inRet = inCTOS_SelectBiller();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetSubscriberNo();
    if(d_OK != inRet)
        return inRet;
#endif

    inRet = inCTOS_SelectFundTransferAccountType();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetAccountNo();
    if(d_OK != inRet)
        return inRet;
	
#if 1
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DEBIT_SALE);
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
		
		inRet = inCTOS_MultiAPReloadTable();
		if(d_OK != inRet)
			return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}
#endif

    #if 0
	vdDebug_LogPrintf("saturn --select account type");
	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;
    #endif
	
	vdDebug_LogPrintf("saturn --multiple MID--");

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --check issuer enable--");
	inRet = inCTOS_CheckIssuerEnable();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --checktranallowed--");
	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn --get invoice--");
	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("saturn strTCT.fAutoDebitLogon:(%d)", strTCT.fAutoDebitLogon);

#if 1
  	if((strTCT.fAutoDebitLogon == TRUE) && (strTCT.fDemo==VS_FALSE))
	{
		BYTE szCurrentTime[4];
		BYTE szCurrentTimeBCD[2];
		CTOS_RTC SetRTC;
		
        CTOS_RTCGet(&SetRTC);
		memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,szCurrentTimeBCD,DATE_ASC_SIZE);

        vdDebug_LogPrintf("SATURN strTCT.szLogonDate:(%02x)(%02x)", (unsigned char)strTCT.szLogonDate[0], (unsigned char)strTCT.szLogonDate[1]);
        vdDebug_LogPrintf("SATURN szCurrentTimeBCD:(%02x)(%02x)", (unsigned char)szCurrentTimeBCD[0], (unsigned char)szCurrentTimeBCD[1]);

		if(memcmp(strTCT.szRequestKeyDate, szCurrentTimeBCD, 2) != 0)
		{
            //inCTOS_LocalAPSaveData();
            //if(d_OK != inRet)
            //    return inRet;
            
			if(get_env_int("BANCNET_TESTKEY") == 1)
				vdSetDummyMKey();
            
            memset(&srLocalTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
            memcpy(&srLocalTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));
            
            vdCTOS_SetTransType(REQUEST_TERMINAL_KEY);						   
            vdDispTransTitle(REQUEST_TERMINAL_KEY);

            inRet = inCTOS_PreConnect();
            if(d_OK != inRet)
                return inRet;
		
            inRet = inProcessLogon();
            if(d_OK != inRet)
                return inRet;

			
		    vdDebug_LogPrintf("SATURN AFTER PROCESS LOGON");
			
            srTransRec.fLogon=TRUE;
            
            vdCTOS_SetTransType(SALE);						   
            //vdDispTransTitle(SALE);

			//memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);
			//strcpy(szDisMsg, szTitle);
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "PROCESSING...");
			//usCTOSS_LCDDisplay(szDisMsg);
			//vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPGetData 1");
			
            //inRet = inCTOS_LocalAPGetData();            
            //if(d_OK != inRet)
            //    return inRet;

            memcpy(&srTransRec, &srLocalTransRec, sizeof(TRANS_DATA_TABLE));
	
		    //vdDebug_LogPrintf("SATURN inCTOS_LocalAPReloadTable 1");
            //inRet = inCTOS_LocalAPReloadTable();
            //if(d_OK != inRet)
            //    return inRet;
            
		}
	}	
#endif

	//vdDebug_LogPrintf("saturn --select account type");
	//inRet = inCTOS_SelectAccountType();
	//if(d_OK != inRet)
	//	return inRet;

	vdDebug_LogPrintf("saturn --get base amount--");
	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;

	vdDebug_LogPrintf("SATURN AFTER inCTOS_GetTxnBaseAmount");
	
	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;

	#if 0  //tine - testing only
	inInitializePinPad();

    else
	inRet = inGetIPPPin();
	if(d_OK != inRet)
		return inRet;	
	#endif
	
	inRet = inCTOS_GetCVV2();
	if(d_OK != inRet)
		return inRet;
	
	vdDebug_LogPrintf("saturn --emv processing--");
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
		return inRet;	

	if (fPinAlreadyEntered == FALSE)
	{
	    
		vdDebug_LogPrintf("saturn inInitializePinPad");
		inInitializePinPad();
		
//		inRet = inGetIPPPinEx();
                inRet = inGetIPPPin();
		vdDebug_LogPrintf("sturn inGetIPPPinEx result1 %d", inRet);
		if(d_OK != inRet)
		return inRet;	
	}

	inRet = inCTOS_CustComputeAndDispTotal();
	if(d_OK != inRet)
	{
	     if(fGetECRTransactionFlg())
         {
              inRet = inMultiAP_Database_BatchInsert(&srTransRec);
              if(d_OK != inRet)
              {
                   vdDebug_LogPrintf("saturn test 1=[%d]", inRet);
                   vdSetErrorMessage("MultiAP BatchInsert ERR");
                   return inRet;
              }   
         }
	     return inRet;
	}
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);

	//usCTOSS_BackToProgress(" ");


	vdDebug_LogPrintf("saturn --pre connect--");
	
    if(srTransRec.fLogon != TRUE)
    {
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
		    return inRet;
    }
	
	inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
	{
	/*
		if ((fGetECRTransactionFlg()) && (strcmp((char *)srTransRec.szRespCode, "00") !=  0))
			{
				 inMultiAP_ECRSendSuccessResponse();
			}
       */
	     return inRet;
	}
	
	vdDebug_LogPrintf("saturn --AFTER inBuildAndSendIsoData");
	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;


	
	vdDebug_LogPrintf("saturn --AFTER inCTOS_SaveBatchTxn");
	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;    

	vdDebug_LogPrintf("saturn --AFTER inCTOS_UpdateAccumTotal");

	vdDebug_LogPrintf("inCTOS_WaveFlowProcess: inMultiAP_ECRSendSuccessResponse -- END, inret=[%d]", inRet);
	inRet = inMultiAP_ECRSendSuccessResponse();

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);

		inRet = inCTOSS_ERM_ReceiptRecvVia();
		if(d_OK != inRet)
	    	return inRet;
	}

	//fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	
	vdDebug_LogPrintf("saturn --BEFORE  PRINT RECEIPT");
	inRet = ushCTOS_printReceipt();
	if(d_OK != inRet)
		return inRet;
	else
    {
         if(get_env_int("#ORIENTATION")==1)
         {  
              inCTOSS_UploadReceipt(FALSE, FALSE);	 
         }
         vdSetErrorMessage("");
    }

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved  = TRUE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
		
    inRet = inProcessAdviceTCTrail(&srTransRec);
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");

	fECRApproved=TRUE;
	//inMultiAP_ECRSendSuccessResponse();

	//inSendECRResponseFlag = 0;
	//put_env_int("ECRRESP",0);
	//vdDebug_LogPrintf("AAA<<>>> inSendECRResponseFlag is ECRRESP[%d]", get_env_int("ECRRESP"));
	
	return d_OK;
}

int inCTOS_SelectFundTransferAccountType(void)
{
	//BYTE key;
	int inRet;
	char szDebug[40 + 1]={0};
	char szChoiceMsg[30 + 1];
	char szHeaderString[24+1];
	int bHeaderAttr = 0x01+0x04, key=0; 
	
	vdDebug_LogPrintf("saturn inCTOS_SelectAccountType[START]");

	vdDebug_LogPrintf("-->>SELECT ACNT byTransType[%d]", srTransRec.byTransType);

    memset(szHeaderString, 0x00, sizeof(szHeaderString));
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

	//if (2 == strTCT.byTerminalType)
	{
		strcpy(szHeaderString, "SELECT ACCOUNT");
		strcat(szChoiceMsg,"SA > CA \n");
		strcat(szChoiceMsg,"CA > SA \n");
		strcat(szChoiceMsg,"SA > OTHERS \n");
		strcat(szChoiceMsg,"CA > OTHERS");
		key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);

		
		if (key > 0)
		{
			if (key == 1)
			{
				srTransRec.inFundTransferAccountType=SA_CA;
				return d_OK;
			}
			
			if (key == 2)
			{
				srTransRec.inFundTransferAccountType=CA_SA;
				return d_OK;
			}

			if (key == 3)
			{
				srTransRec.inFundTransferAccountType=SA_OTHERS;
				return d_OK;
			}

			if (key == 4)
			{
				srTransRec.inFundTransferAccountType=CA_OTHERS;
				return d_OK;
			}
						
			if (key == d_KBD_CANCEL)
			{
			    if(fGetECRTransactionFlg())
                {
                     vdSetECRResponse(ECR_UNKNOWN_ERR);
					 //inMultiAP_ECRSendSuccessResponse();
                }	
				return -1;
			}

			if (key == 0xFF)
			{
				vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
				CTOS_Beep();
				CTOS_Delay(1500);
                if(fGetECRTransactionFlg())
                {
                     vdSetECRResponse(ECR_TIMEOUT_ERR);
                     //inMultiAP_ECRSendSuccessResponse();
                }	
				return d_TIMEOUT;
			}

			return d_NO;
			
		}
	}
	
	return inRet;
}

int inCTOS_RequestTerminalKey(void)
{
    int inRet = d_NO;

    vdDebug_LogPrintf("inCTOS_RequestTerminalKey");
	
	CTOS_LCDTClearDisplay();
	
    //if (inMultiAP_CheckSubAPStatus() != d_OK)
    //    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

    inRet = inCTOS_RequestTerminalKeyFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_RequestTerminalKeyFlowProcess(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];

    srTransRec.fDebit=TRUE;

	vdDebug_LogPrintf("inCTOS_RequestTerminalKeyFlowProcess");
	
	vdCTOS_SetTransType(REQUEST_TERMINAL_KEY);

    if(get_env_int("BANCNET_TESTKEY") == 1)
	    vdSetDummyMKey();
	
	//vdCTOS_DispStatusMessage("PROCESSING..."); // Minimize white screen
	vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	
	//display title
	//vdDispTransTitle(LOG_ON);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;
	inTCTRead(1);
	
	strCDT.HDTid=strTCT.inLogonHostIndex;

#if 1
    strCDT.HDTid = BANCNET_HOST;
    
    inRet = inCTOS_SelectHostEx();
    if(d_OK != inRet)
        return inRet;

#else
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_LOGON);
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
			
			inRet = inCTOS_MultiAPReloadTable();
			if(d_OK != inRet)
				return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}
#endif

    srTransRec.fDebit=TRUE;

	//inRet = inCTOS_CheckAndSelectMutipleMID();
	//if(d_OK != inRet)
		//return inRet;

	inRet = inCTOS_PreConnect();
	if(d_OK != inRet)
		return inRet;

	//inRet = inCTOS_CheckIssuerEnable();
	//if(d_OK != inRet)
		//return inRet;

	inRet = inCTOS_CheckTranAllowd();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
		return inRet;

	inRet = inProcessLogon();
	if(d_OK != inRet)
		return inRet;

	//vdSetDummyMKey(); //sidumili: Dummy Master Key (to be comment or remove, testing purpose only)
	
	return d_OK;
}

