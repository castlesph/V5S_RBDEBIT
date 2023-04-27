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
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"

//gcitra
#include "..\Includes\CTOSInput.h"
#include "..\Includes\ISOEnginee.h"
//gcitra

#include "..\Pinpad\Pinpad.h"
#include "..\Includes\POSBalanceInq.h"
extern int inPINEntryCount;

extern BOOL fPinAlreadyEntered;
BOOL fBalanceInquiry;


int inCTOS_BalanceInquiryFlowProcess(void)
{
    int inRet = d_NO;
	char szErrorMsg[30+1];
    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
	int inPINCaptureErrRetry=0;
	int inRespCode;
	VS_BOOL fOKToProceed;

	fOKToProceed = TRUE;

    vdCTOS_SetTransType(BALANCE_INQUIRY);
    vdCTOS_DispStatusMessage("PROCESSING...");
    //display title
    //vdDispTransTitle(BALANCE_INQUIRY);
	//vdCTOS_DispStatusMessage("PROCESSING...");
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

    inRet = inCTOS_GetCardFields();
    if(d_OK != inRet)
        return inRet;
#if 0    
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;
#endif
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BALANCE_INQUIRY);
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

    #if 0
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC){

		vdDispTransTitle(BALANCE_INQUIRY);

		inRet=inCTOS_GetCardFieldsFallback();	
		if(d_OK != inRet)
			return inRet;

		inRet=inCTOS_DisplayCardTitle2(6, 7); //Display Issuer logo: re-aligned Issuer label and PAN lines -- jzg
		if(d_OK != inRet)
			return inRet;


		inRet = inConfirmPAN2();
		if(d_OK != inRet)
			return inRet;

    }
    #endif

	//Enhance start
	inRet = inCTOS_PreConnectAndInit();
	if(d_OK != inRet)
	{
		vdSetECRResponse(ECR_COMM_ERROR_RESP);
		return inRet;
	}
//Enhance end

    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;
#if 0
//Enhance start
	vdDebug_LogPrintf("GOC 3 = [%d]", strCDT.HDTid);

    //inRet = inCTOS_SelectHost(); // Check if Host is enabled
    inRet = inCTOS_SelectHostEx2();
    if(d_OK != inRet)
        return inRet;

    //if (inMMTReadRecord(strCDT.HDTid,srTransRec.MITid) != d_OK) // Also check if Merchant using host is enabled
    if (inMMTReadRecordEx(strCDT.HDTid,srTransRec.MITid) != d_OK) // Also check if Merchant using host is enabled
    {
        vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
		inDatabase_TerminalCloseDatabase();
        return(d_NO);
    }	
	inDatabase_TerminalCloseDatabase();
//Enhance End	
#endif
	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

	if (srTransRec.usTerminalCommunicationMode != GPRS_MODE && srTransRec.usTerminalCommunicationMode != WIFI_MODE)
	{
	    inRet = inCTOS_PreConnect();
	    if(d_OK != inRet)
	        return inRet;
	}

	//inSETIssuerUsingAID();


    inRet = inCTOS_CheckIssuerEnable();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

#if 0	
	inTCTReadLogonFailedFlag(1);
	if(strTCT.fLogonFailed == TRUE)//Perform Auto Logon if previous logon failed before proceeding to transaction.
	{
		//inRet = inCTOS_LogonFlowProcess();
		inRet = inCTOS_DoAutoLogon();
		
		if(d_OK != inRet)
			return inRet;
		else
		{
			vdDisplayMessage("PLEASE", "RETRY", "TRANSACTION");
			CTOS_Beep();
			CTOS_Delay(1000);
			return USER_ABORT;
		}
	}

#else

	if(strMMT[0].fLogon_Failed == TRUE)//Perform Auto Logon if previous logon failed before proceeding to transaction.
	{
		//inRet = inCTOS_LogonFlowProcess();
		inRet = inCTOS_DoAutoLogon();
		
		if(d_OK != inRet)
			return inRet;
		else
		{
			vdDisplayMessage("PLEASE", "RETRY", "TRANSACTION");
			CTOS_Beep();
			CTOS_Delay(1000);
			return USER_ABORT;
		}
	}

#endif
		
    //gcitra
    inRet = inCTOS_SelectAccountType();
    if(d_OK != inRet)
        return inRet;
	//gcitra


    //gcitra
    inInitializePinPad();
	//gcitra

	inPINEntryCount=0;
PIN_AGAIN:

    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;   

	if (fPinAlreadyEntered == FALSE){
    	inRet = inGetIPPPin();
		memset(szErrorMsg,0x00,sizeof(szErrorMsg));
		inGetErrorMessage(szErrorMsg);
		if(strcmp(szErrorMsg,"USER CANCEL") == 0 || strcmp(szErrorMsg,"PIN TIMEOUT") == 0)
		{
			vdSetErrorMessage("");
			CTOS_LCDTClearDisplay();
		}
		else if(inRet == 10500)//Handling or inDisplayProcessing Error 2904
		{
			inPINCaptureErrRetry++;
			if (inPINCaptureErrRetry < 3)
				goto PIN_AGAIN;
		}
		
    	if(d_OK != inRet)
      	return inRet;
	}


    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();

	
	inRespCode = atoi(srTransRec.szRespCode);

	if (inRespCode == 55){
		fPinAlreadyEntered=FALSE;
		if (inPINEntryCount < 3)
			goto PIN_AGAIN;
	}
 	 
	if (inRespCode == 81){ 
		inRet = inCTOS_DoAutoLogon();
		fOKToProceed = FALSE;
			 
		if (d_OK != inRet)
			return inRet;			 
	}
	  
	if (fOKToProceed == FALSE){
		vdDisplayMessage("PLEASE", "RETRY", "TRANSACTION");
		CTOS_Beep();
		CTOS_Delay(1000);
		return USER_ABORT;
	}

	if(d_OK != inRet){
		return inRet;
	}


	 inRet = InDisplayBalance();
	 if(d_OK != inRet)
		 return inRet;

	 inRet = inCTOS_PrintBalanceInq();
	 if(d_OK != inRet)
		 return inRet;

   vdSetErrorMessage("");  

    return d_OK;
}

int InDisplayBalance(void){

	char szAmount[12+1];
	char szStr[12+1];
	BYTE szDisplayBuf[40];
	BYTE szAmtBuf[20];
	BYTE szBuf[20];
	ULONG  ulAmount;
	char szCurAmt[4 + 1]="PHP";

	if (srTransRec.byTransType != BALANCE_INQUIRY)
		return d_OK;

	CTOS_LCDTClearDisplay();

	//display title
	vdDispTransTitle(BALANCE_INQUIRY);
	CTOS_LCDTPrintXY(1, 4, "APPROVED");
	
	memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
	memset(szBuf, 0x00, sizeof(szBuf));

	memset(szBuf, 0x00, sizeof(szBuf));
	//sprintf(szBuf, "%02X%02X%02X%02X%02X%02X", (unsigned char)srTransRec.szBalInqAmount[0], (unsigned char)srTransRec.szBalInqAmount[1], (unsigned char)srTransRec.szBalInqAmount[2], 
	//																					 (unsigned char)srTransRec.szBalInqAmount[3], (unsigned char)srTransRec.szBalInqAmount[4], (unsigned char)srTransRec.szBalInqAmount[5]);
    //issue-443
	wub_hex_2_str(srTransRec.szBalInqAmount, szBuf, 6); 

	if (strTCT.fDemo){
		strcpy(szBuf, "1234567890");
	}

	
	
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);

	setLCDPrint(5, DISPLAY_POSITION_LEFT, "BAL:");
	//strcat(szCurAmt,szAmtBuf); //aaronnino for BDOCLG ver 9.0 fix on issue #00144 AMOUNT SHOULD HAVE PHP ON AMOUNT ON BAL INQUIRY
    setLCDPrint(6, DISPLAY_POSITION_LEFT, szCurAmt);
	//setLCDPrint(6, DISPLAY_POSITION_RIGHT, szAmtBuf);
	
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szAmtBuf)+1)*2,  6, szAmtBuf);

	vduiPressAnyKey();
	
	return d_OK;

}


int inCTOS_BALANCE_INQUIRY(void)
{
    int inRet = d_NO;
    fBalanceInquiry = TRUE;
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
	
    inRet = inCTOS_BalanceInquiryFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();
	fBalanceInquiry = FALSE;
    return inRet;
}

