#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <vwdleapi.h>


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
#include "..\Pinpad\Pinpad.h"
#include "..\Includes\POSLogon.h"


#include "POSCtls.h"
#include "POSWave.h"
extern BOOL fBinVerInitiatedSALE;
extern int inPINEntryCount;

extern BOOL fECRBuildSendOK; /* For ECR -- sidumili */
extern BOOL fTimeOutFlag; /*BDO: Flag for timeout --sidumili*/
extern BOOL fPINTrxnRetry;
	
int inWaveTransType = 0;

extern BOOL fPinAlreadyEntered;

void vdCTOSS_SetWaveTransType(int type)
{
	inWaveTransType = type;
}

int inCTOSS_GetWaveTransType(void)
{
	return inWaveTransType;
}


void vdCTOS_InitWaveData(void)
{
	vdCTOSS_CLMSetALLCAPK();
	vdCTOSS_CLMSetAIDAndEMVTag();	
}

void vdCTOSS_GetALLWaveSetting(void)
{
	BYTE OutRBuf[2048];
	UINT outLen,rtn;
	EMVCL_SCHEME_DATA stScheme;
	EMVCL_CA_PUBLIC_KEY stCAPubKey;

	inCTOSS_CLMOpenAndGetVersion();

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		rtn = MsessionAuthenAdmin();
		vdDebug_LogPrintf("rtn =[%x]........",rtn);
        if (rtn != d_NO_ERROR) 
		{
			vdDisplayErrorMsg(1, 8, "Authen Fail");
			return ;
        }
		
		rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_ADMIN,OutRBuf);
		vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,OutRBuf);
	}
		
	inCTOSS_CLMGetEMVTag(OutRBuf,&outLen);
	inCTOSS_CLMVisaGetCapability(&stScheme);

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_NORMAL,OutRBuf);
		vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,OutRBuf);
		
	vdCTOSS_CLMClose();
		
	inCTOSS_CLMListAllCAPKID(OutRBuf,&outLen);

	inCTOSS_CLMGetACAPK("\xA0\x00\x00\x00\x03",0x08,&stCAPubKey);
}



int inCTOS_MultiAPReloadWaveData(void)
{
	if (srTransRec.usChipDataLen > 0)
	{			
		vdCTOSS_WaveGetEMVData(srTransRec.baChipData, srTransRec.usChipDataLen);
	}
		
	if (srTransRec.usAdditionalDataLen > 0)
	{			
		vdCTOSS_WaveGetEMVData(srTransRec.baAdditionalData, srTransRec.usAdditionalDataLen);
	}
	
	return (d_OK);
}

int inCTOS_WaveFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
	int inEVENT_ID = 0;
	int inSMCardRetryCount = 0;
	int inPINCaptureErrRetry=0;
	char szErrorMsg[30+1];
	
    unsigned char key;

	vdDebug_LogPrintf("inCTOS_WaveFlowProcess  ---- BEGIN");

    vdCTOS_SetTransType(SALE);		
	//vdDispTransTitle(SALE);
	vdCTOS_DispStatusMessage("PROCESSING...");

	//sidumili: [for autologon]
	int inRespCode;
	VS_BOOL fOKToProceed = TRUE;

	fECRBuildSendOK = FALSE;
	fTimeOutFlag = FALSE; /*BDO: Flag for timeout --sidumili*/
	
	memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));
   	
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

#if 0
//for enhance the treansaction speed
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

	inRet = inCTOS_PreConnectAndInit();
		if(d_OK != inRet)
			return inRet;

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

/*  inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;
*/
//gcitra
#if 0
    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
#endif
//gcitra
    
    inRet = inCTOS_WaveGetCardFields();
    if(d_OK != inRet)
        return inRet;

	//gcitra
	inRet=inCTOS_DisplayCardTitle(6, 7); //Display Issuer logo: re-aligned Issuer label and PAN lines -- jzg
	if(d_OK != inRet)
		return inRet;
	
	inRet = inConfirmPAN();
	if(d_OK != inRet)
		return inRet;
	inRet = inCTOS_UpdateTxnTotalAmount();
		if(d_OK != inRet)
			return inRet;
	//gcitra
#if 0
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;
#endif

	if (fBinVerInitiatedSALE == VS_FALSE){

	    if (inMultiAP_CheckMainAPStatus() == d_OK)
	    {
	    	if (srTransRec.byTransType == REFUND)
				inEVENT_ID = d_IPC_CMD_WAVE_REFUND;
			else
				inEVENT_ID = d_IPC_CMD_WAVE_SALE;
	        inRet = inCTOS_MultiAPSaveData(inEVENT_ID);
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

				inRet = inCTOS_MultiAPReloadWaveData();
	            if(d_OK != inRet)
	                return inRet;
	        }
	        inRet = inCTOS_MultiAPCheckAllowd();
	        if(d_OK != inRet)
	            return inRet;
	    }
	}
	
	inRet = inCTOS_PreConnectAndInit();
	if(d_OK != inRet)
	{
		vdSetECRResponse(ECR_COMM_ERROR_RESP);
		return inRet;
	}
	
#if 0
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC){

		vdDispTransTitle(SALE);

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

    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

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

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

		
#if 0
	inTCTReadLogonFailedFlag(1);

		
		vdDebug_LogPrintf("inCTOS_WaveFlowProcess  ---- CHK AUTOLOGON   ---- strTCT.fLogonFailed[%d]", strTCT.fLogonFailed);
	
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

		vdDebug_LogPrintf("inCTOS_WaveFlowProcess  ---- CHK AUTOLOGON   ---- strMMT[0].fLogon_Failed[%d]", strMMT[0].fLogon_Failed);

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
	//CTOS_LCDTClearDisplay();
	//vdCTOS_DispStatusMessage("PROCESSING...");//fix for blank screen display before the PIN entry display
	
	/*BDO PHASE 2: [Fix on Acount selection to display immediately] - sidumili*/
#if 0
	inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
	{
		vdSetECRResponse(ECR_COMM_ERROR_RESP);
		return inRet;
    }
#endif
	/*BDO PHASE 2: [Fix on Acount selection to display immediately] - sidumili*/

/*
    inRet = inGetIPPPin();
    if(d_OK != inRet)
      return inRet;
*/

 
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
    {
		vdSetECRResponse(ECR_OPER_CANCEL_RESP);
		memset(srTransRec.szRespCode,0x00,RESP_CODE_SIZE);
        return inRet;   
    }

	//gcitra
	//if(get_env_int("BINROUTE") != TRUE)//Fix to invoice number incrementing twice on Bin Routing setup.
	{
	    inRet = inCTOS_GetInvoice();
	    if(d_OK != inRet)
	        return inRet;
	}


    inPINEntryCount=0;
PIN_AGAIN:


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
    	{
			vdSetECRResponse(ECR_OPER_CANCEL_RESP);
			memset(srTransRec.szRespCode,0x00,RESP_CODE_SIZE);
      		return inRet;
    	}
	}

    inRet = inBuildAndSendIsoData();
	if(d_OK != inRet)
	{

		// sidumili: [CHECK FOR HOST RESPONSE RC81(DO NOT HONOR)]
		inRespCode = atoi(srTransRec.szRespCode);

		vdDebug_LogPrintf("-->>BDODEBIT inRespCode[%d] resp=%s", inRespCode, srTransRec.szRespCode);

		
		switch (inRespCode)
		{
			case 81:
				//strDCT.fDebitLogonRequired = VS_TRUE;	
				//inDCTSave(srTransRec.HDTid);
			
				inRet = inCTOS_DoAutoLogon();
				fOKToProceed = FALSE;
				break;
	         case 55:
			 	fPinAlreadyEntered=FALSE;
				fPINTrxnRetry = TRUE;
				if (inPINEntryCount == 3)
					break;
				else
					goto PIN_AGAIN;
			
			default:
			break;
		}

		vdDebug_LogPrintf("-->>BDODEBIT inRet[%d]--fOKToProceed[%d]", inRet, fOKToProceed);
		
		if (d_OK != inRet)
		{
			/*BDO: Set ECR response code to EN - COMM ERROR -- sidumili*/
			if (strlen(srTransRec.szRespCode) <= 0)
				strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
			else
			{
				inSMCardRetryCount = get_env_int("SMCARDRETRYCOUNT");
				inSMCardRetryCount++;
				put_env_int("SMCARDRETRY",1);
				put_env_int("SMCARDRETRYCOUNT",inSMCardRetryCount);
			}
			return inRet;
		}
	
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


		/* BDOCLG-00031: line should be cut if there's no pending advice/ tcupload - start -- jzg  */
		inRet=inDisconnectIfNoPendingADVICEandUPLOAD(&srTransRec, strHDT.inNumAdv);
		if(d_OK != inRet)
			return inRet;
		/* BDOCLG-00031: line should be cut if there's no pending advice/ tcupload - end -- jzg  */


    // patrick add code 20141205 start, modified -- sidumili
	fECRBuildSendOK = TRUE;	
	if (fECRBuildSendOK){	
	    inRet = inMultiAP_ECRSendSuccessResponse();
	}
	// patrick add code 20141205 start, modified -- sidumili

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;


	CTOS_LCDTClearDisplay(); 

	if(strTCT.fDebitTCUpload == TRUE)
	{
	    inRet = inCTOS_EMVTCUpload();
	    if(d_OK != inRet)
	        return inRet;
	    else
	        vdSetErrorMessage("");
	}
	
    return d_OK;
}


int inCTOS_WAVE_SALE(void)
{
    int inRet = d_NO;

    //CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

	vdCTOS_SetTransType(SALE);
	//vdCTOSS_SetWaveTransType(1);

	//BDO: No CTLS txn for DEBIT - start -- jzg
	#if 1	
		vdCTOSS_SetWaveTransType(0);
	#else	
		vdCTOSS_SetWaveTransType(1);

	// patrick remark ECR "OUT OF RANGE"
	//vdCTOSS_GetAmt();

	inRet = inCTOSS_CLMOpenAndGetVersion();
	if(d_OK != inRet)
        return inRet;
	#endif
	//BDO: No CTLS txn for DEBIT - end -- jzg

	
	//display title
    //vdDispTransTitle(SALE);
	
    inRet = inCTOS_WaveFlowProcess();

	//vdCTOSS_CLMClose();

	/* Send response to ECR -- sidumili */
	if (!fECRBuildSendOK){	
		inMultiAP_ECRSendSuccessResponse();
	}	
	fECRBuildSendOK = FALSE;
	/* Send response to ECR -- sidumili */
	
    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();
	
    return inRet;
}



int inCTOS_WAVE_REFUND(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

	vdCTOS_SetTransType(REFUND);
	vdCTOSS_SetWaveTransType(1);

	vdCTOSS_GetAmt();

	inRet = inCTOSS_CLMOpenAndGetVersion();
	if(d_OK != inRet)
        return inRet;
	//display title
    vdDispTransTitle(REFUND);

    inRet = inCTOS_WaveFlowProcess();

	vdCTOSS_CLMClose();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}



