#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <vwdleapi.h>
#include <sqlite3.h>

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
#include "..\Includes\Wub_lib.h"
#include "..\Database\DatabaseFunc.h"
#include "..\ApTrans\MultiShareEMV.h"
#include "..\Includes\CardUtil.h"
#include "..\Includes\POSSetting.h"

#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Includes\POSReload.h"

extern int inGetIPPPin(void);

extern BOOL fBinVerInitiatedSALE;

int inCTOS_ReloadFlowProcess(void)
{
	int inRet = d_NO;

	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	int inResult;

	//sidumili: [for autologon]
	int inRespCode;
	VS_BOOL fOKToProceed = TRUE;

	fBinVerInitiatedSALE = FALSE;
	
	vdCTOS_SetTransType(RELOAD);

	//display title
	vdDispTransTitle(RELOAD);
	
	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
		return inRet;

#if 1
	inRet = inCTOS_PreConnectAndInit();
	if(d_OK != inRet)
	{
		vdSetECRResponse(ECR_COMM_ERROR_RESP);
		return inRet;
	}
#endif

	
	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_GetTxnBaseAmount();
	if(d_OK != inRet)
		return inRet;
#if 0
	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;
#endif
	inRet = inCTOS_UpdateTxnTotalAmount();
	if(d_OK != inRet)
		return inRet;
				
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_RELOAD);
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

			srTransRec.IITid = 1; //Set IIT to 1 for BDODebit
			srTransRec.CDTid = 53; //Set IIT to 1 for BDODebit
				
			inRet = inCTOS_MultiAPReloadTable();
			if(d_OK != inRet)
				return inRet;
		}
		inRet = inCTOS_MultiAPCheckAllowd();
		if(d_OK != inRet)
			return inRet;
	}

	
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
	
	
	// Swipe Destination Card
	inRet = inCTOS_SwipeEnterDestinationCard();
	if(d_OK != inRet)
		return inRet;

	// Validate Destination Card
	inRet = inCTOS_ValidateSourceDestinationCard();
	if(d_OK != inRet)
		return inRet;
	
	// Swipe Source Card
	inRet = inCTOS_SwipeEnterSourceCard();
	if(d_OK != inRet)
		return inRet;

	// Validate Source Card
	inRet = inCTOS_ValidateSourceDestinationCard();
	if(d_OK != inRet)
		return inRet;
	
	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

	inRet = inCTOS_CheckMustSettle();
	if(d_OK != inRet)
		return inRet;


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


	inRet = inCTOS_SelectAccountType();
	if(d_OK != inRet)
		return inRet;

	//to remove account selected when doing gprs establish
	CTOS_LCDTClearDisplay();

	if (srTransRec.usTerminalCommunicationMode != GPRS_MODE && srTransRec.usTerminalCommunicationMode != WIFI_MODE)
	{
		inRet = inCTOS_PreConnect();
		if(d_OK != inRet)
			return inRet;
	}

	inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
			return inRet;

	inInitializePinPad();
	inPINEntryCount=0;
PIN_AGAIN:
		
	inRet = inGetIPPPin();
    if(d_OK != inRet)
      return inRet;


    inRet = inBuildAndSendIsoData();
	if(d_OK != inRet){

	// sidumili: [CHECK FOR HOST RESPONSE RC81(DO NOT HONOR)]
	inRespCode = atoi(srTransRec.szRespCode);

	vdDebug_LogPrintf("-->>BDODEBIT inRespCode[%d]", inRespCode);
	
	switch (inRespCode){
		case 81:
			//strDCT.fDebitLogonRequired = VS_TRUE;	
			//inDCTSave(srTransRec.HDTid);
		
			inRet = inCTOS_DoAutoLogon();
			fOKToProceed = FALSE;
			break;
         case 55:
			if (inPINEntryCount == 3)
				break;
			else
				goto PIN_AGAIN;
		
		default:
		break;
	}

	vdDebug_LogPrintf("-->>BDODEBIT inRet[%d]--fOKToProceed[%d]", inRet, fOKToProceed);
	
	if (d_OK != inRet)
		return inRet;
	
	}

	if (fOKToProceed == FALSE){
		vdDisplayMessage("PLEASE", "RETRY", "TRANSACTION");
		CTOS_Beep();
		CTOS_Delay(1000);
		return USER_ABORT;
	}

	inRet = inCTOS_SaveBatchTxn();
	if(d_OK != inRet)
		return inRet;

	inRet = inCTOS_UpdateAccumTotal();
	if(d_OK != inRet)
		return inRet;
	
	inRet = ushCTOS_printReceipt();
	if(d_OK != inRet)
		return inRet;


	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

	vdSetErrorMessage("");
	
	return inRet;
}


int inCTOS_RELOAD(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	
    inRet = inCTOS_ReloadFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_SwipeEnterSourceCard(void){
	int inRet = d_NO;
	char szDisplay[30];
	char szTemp1[30 + 1];

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	
	srTransRec.inReloadSwipeCard = RELOAD_SWIPE_SOURCE_CARD;

	inRet = inCTOS_GetSourceCardFields();
	if (inRet != d_OK) 
		return(inRet);
	
	// source card
	memset(srTransRec.szSourceCardNo, 0x00, sizeof(srTransRec.szSourceCardNo));
	strcpy(srTransRec.szSourceCardNo, srTransRec.szPAN);
	strcpy(srTransRec.szSourceTrack2Data, srTransRec.szTrack2Data);
	
	inRet = inCTOS_DisplayReloadCardTitle(6, 7); //Display Issuer logo: re-aligned Issuer label and PAN lines -- jzg
	if(d_OK != inRet)
		return inRet;

	inRet = inReloadConfirmPAN();
	if(d_OK != inRet)
		return inRet;

	memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
	memset(srTransRec.szTrack2Data, 0x00, sizeof(srTransRec.szTrack2Data));
	
 	return inRet;
	
}

int inCTOS_SwipeEnterDestinationCard(void){
	int inRet = d_NO;
	char szDisplay[30];
	char szTemp1[30 + 1];

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	
	srTransRec.inReloadSwipeCard = RELOAD_SWIPE_DEST_CARD;
	
	inRet = inCTOS_GetDestinationCardFields();
	if (inRet != d_OK) 
		return(inRet);
	
	// source card
	memset(srTransRec.szDestCardNo, 0x00, sizeof(srTransRec.szDestCardNo));
	strcpy(srTransRec.szDestCardNo, srTransRec.szPAN);	
	
	inRet = inCTOS_DisplayReloadCardTitle(6, 7); //Display Issuer logo: re-aligned Issuer label and PAN lines -- jzg
	if(d_OK != inRet)
		return inRet;

	inRet = inReloadConfirmPAN();
	if(d_OK != inRet)
		return inRet;

	memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
	memset(srTransRec.szTrack2Data, 0x00, sizeof(srTransRec.szTrack2Data));
	
 	return inRet;
	
}

int inCTOS_ValidateSourceDestinationCard(void){
	int inRet;

	vdDebug_LogPrintf("Validate Reload Card fReloadCard[%d]", strCDT.fReloadCard);
	
	if (strCDT.fReloadCard == FALSE){
		CTOS_LCDTClearDisplay();
		vdSetErrorMessage("CARD NOT SUPPORTED");
		return(d_NO);
	}

	if (strcmp(srTransRec.szSourceCardNo, srTransRec.szDestCardNo) == 0){
		CTOS_LCDTClearDisplay();
		setLCDPrint27(7, DISPLAY_POSITION_LEFT, "DESTINATION CARD");
		setLCDPrint27(8, DISPLAY_POSITION_LEFT, "NOT ALLOWED");

		CTOS_Beep();
		WaitKey(1);
		return(d_NO);
	}
	
	return(d_OK);	
}

/* BDO: Display reload type card -- sidumili */
void vdCTOS_DisplayReloadCardType(void){
	if (srTransRec.byTransType == RELOAD){
		if (srTransRec.inReloadSwipeCard == RELOAD_SWIPE_SOURCE_CARD)
			CTOS_LCDTPrintXY(1, 3, "Source Card");
		if (srTransRec.inReloadSwipeCard == RELOAD_SWIPE_DEST_CARD)	
			CTOS_LCDTPrintXY(1, 3, "Destination Card");
	}
}

