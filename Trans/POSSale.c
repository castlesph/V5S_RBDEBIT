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
#include "..\database\dct.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\Pinpad\Pinpad.h"
#include "..\includes\LocalAptrans.h"
#include "..\Includes\POSLogon.h"


//1030
extern BOOL fBinVerInitiatedSALE;
//1030

//gcitra

/*sidumili: */
extern BOOL fGlobalSettleAll;


int inCTOS_SaleFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

    vdCTOS_SetTransType(SALE);
    
    //display title
    vdDispTransTitle(SALE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

//gcitra
    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

	
    inRet = inCTOS_GetCardFields(); 
    //inRet = inCTOS_WaveGetCardFields();
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

	inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
		return inRet;

//1030
    if (fBinVerInitiatedSALE == VS_FALSE){
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
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
}
//1030
    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

    //inRet = inCTOS_PreConnect();
    //if(d_OK != inRet)
    //   return inRet;

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
	
    //gcitra
    inRet = inCTOS_SelectAccountType();
    if(d_OK != inRet)
        return inRet;
	//gcitra

	//to remove account selected when doing gprs establish
	CTOS_LCDTClearDisplay();

		/*BDO PHASE 2: [Fix on Acount selection to display immediately] - sidumili*/
		inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;
		/*BDO PHASE 2: [Fix on Acount selection to display immediately] - sidumili*/
		
    inRet = inGetIPPPin();
    if(d_OK != inRet)
       return inRet;
 
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;   

	//gcitra

	inCTOS_LocalAPSaveData();
	if(d_OK != inRet)
		return inRet;

	vdCTOS_SetTransType(SIGN_ON);
	vdDispTransTitle(SIGN_ON);

	inRet = inDoAutoLogon();
	if(d_OK != inRet)
		return inRet;	

	vdCTOS_SetTransType(SALE);
						   
	//display title
	vdDispTransTitle(SALE);
	
	inRet = inCTOS_LocalAPGetData();
	if(d_OK != inRet)
	  return inRet;
						 
	inRet = inCTOS_LocalAPReloadTable();
	if(d_OK != inRet)
	  return inRet;

	//gcitra

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

		/* BDO: Terminal should disconnect if there no pending TC or advise - start -- jzg */
		inRet = inDisconnectIfNoPendingADVICEandUPLOAD(&srTransRec, strHDT.inNumAdv);
		if(d_OK != inRet)
			return inRet;
		/* BDO: Terminal should disconnect if there no pending TC or advise - end -- jzg */


	inRet = inMultiAP_ECRSendSuccessResponse();//enhance ecr

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;

    //inRet = inCTOS_EMVTCUpload();
    //if(d_OK != inRet)
    //    return inRet;
    //else
    //    vdSetErrorMessage("");


    return d_OK;
}


int inDoAutoLogon(void){

	int inRet;

	char test[30];


	inDCTRead(srTransRec.HDTid,srTransRec.MITid);

	vdDebug_LogPrintf("-->>inDoAutoLogon[START] fGlobalSettleAll[%d]", fGlobalSettleAll);
	
	vdDebug_LogPrintf("-->>inDoAutoLogon[1] fGlobalSettleAll[%d]", fGlobalSettleAll);
	
   // if (srTransRec.byTransType == SETTLE){
//		strDCT.fDebitLogonRequired = VS_TRUE;
//		inDCTSave(srTransRec.HDTid);
		
//    }

   	if(strDCT.fDebitLogonRequired == VS_FALSE)
		return d_OK;
   
	CTOS_LCDTClearDisplay();
	vdCTOS_SetTransType(LOG_ON);
	vdDispTransTitle(LOG_ON);

    inRet = inCTOS_SelectLogonHost();
    if(d_OK != inRet)
        return inRet;

//	if(strDCT.fDebitLogonRequired == VS_FALSE)
//		return d_OK;


	vdDebug_LogPrintf("-->>inDoAutoLogon[2] fGlobalSettleAll[%d]", fGlobalSettleAll);

	//CTOS_LCDTClearDisplay();
	//vdDispTransTitle(SIGN_ON);

	#if 0
	//120214
	setLCDPrint(4,DISPLAY_POSITION_LEFT, "LOGON REQUIRED");	
	setLCDPrint(5,DISPLAY_POSITION_LEFT, "ERROR CODE:81       ");	
	CTOS_Delay(2000);
	#endif
	
	setLCDPrint(4,DISPLAY_POSITION_LEFT, "PLEASE WAIT...");	
	setLCDPrint(5,DISPLAY_POSITION_LEFT, "PROCESSING LOGON");	
	CTOS_Delay(2000);
	//120214

	vdDebug_LogPrintf("-->>inDoAutoLogon[3] fGlobalSettleAll[%d]", fGlobalSettleAll);
	
	inRet = inCTOS_PreConnect();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inBuildAndSendIsoData();

	vdDebug_LogPrintf("-->>inDoAutoLogon[4] fGlobalSettleAll[%d]--inRet[%d]", fGlobalSettleAll, inRet);

//012215
	
//    if(ST_SUCCESS != inRet){	
//		strDCT.fDebitLogonRequired = VS_TRUE;	
//		inDCTSave(srTransRec.HDTid);
		//return inRet;
//    }

//	strDCT.fDebitLogonRequired = VS_FALSE;
//012215

	//inDCTSave(srTransRec.HDTid);

	vdDebug_LogPrintf("-->>inDoAutoLogon[5] fGlobalSettleAll[%d]", fGlobalSettleAll);
	
	if (fGlobalSettleAll){
		if (ST_SUCCESS == inRet)
			inDisplayLogonResponse();
		return(d_OK);
	}else{
			if (ST_SUCCESS != inRet){
				return inRet;
			}
	}

	vdDebug_LogPrintf("-->>inDoAutoLogon[END] fGlobalSettleAll[%d]", fGlobalSettleAll);
	
	return d_OK;
}

//gcitra
/*
int inGetPolicyNumber(void)
{
    USHORT usRet;
    USHORT usLens;
    USHORT usMinLen = 1;
    USHORT usMaxLen;
    USHORT usInputLine = 6;

     BYTE    baBuf[21+1];



    CTOS_LCDTClearDisplay();
    vdDispTransTitle(SALE);

    memset(srTransRec.szPolicyNumber, 0x00, sizeof(srTransRec.szPolicyNumber));

    CTOS_KBDBufFlush();


   usMaxLen = strIIT.inPolicyNumMaxChar;

   if (usMaxLen >19)
   	usMaxLen = 19;

    setLCDPrint(4, DISPLAY_POSITION_LEFT, "POLICY NO: ");

    memset(baBuf, 0x00, sizeof(baBuf));	

    while(1)
    {
        usRet = shCTOS_GetNum(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 1, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);
        if (usRet >= usMinLen && usRet <= usMaxLen)
        {

	     memcpy(srTransRec.szPolicyNumber,baBuf,usRet);
            return (d_OK);   
        }
        else if(0 == usRet)
        {
            return (d_OK);   
        }

        baBuf[0] = 0x00;
    }


}
*/
//gcitra

int inCTOS_SALE(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	
    inRet = inCTOS_SaleFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

