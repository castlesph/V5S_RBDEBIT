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
#include "..\Includes\POSCashAdvance.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Pinpad\Pinpad.h"


int inCTOS_CashAdvanceFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

    vdCTOS_SetTransType(CASH_ADVANCE);
    
    //display title
    vdDispTransTitle(CASH_ADVANCE);
    
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
		
	
		//remove - BDO does not require display and Floor limit cheking is not applicable to BDO (Except for EMV) 
		//inRet = inCTOS_CustComputeAndDispTotal();
		//if(d_OK != inRet)
		//	return inRet;
	//gcitra
		
		inRet = inCTOS_GetCardFields(); 
		//inRet = inCTOS_WaveGetCardFields();
		if(d_OK != inRet)
			return inRet;
	
	//gcitra

		inRet = inCTOS_UpdateTxnTotalAmount();
		if(d_OK != inRet)
			return inRet;
	//gcitra
		
    
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_CASH_ADVANCE);
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

    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

	//inSETIssuerUsingAID();

    inRet = inCTOS_CheckIssuerEnable();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;


 /*   inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
*/
    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;



    //gcitra
    inRet = inCTOS_SelectAccountType();
    if(d_OK != inRet)
        return inRet;
	//gcitra

    inRet = inGetIPPPin();
    if(d_OK != inRet)
        return inRet;

	
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;   

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
    
    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;

		CTOS_LCDTClearDisplay(); //BDO UAT 0012: Merchant copy still being displayed during TC Upload -- jzg
		
    inRet = inCTOS_EMVTCUpload();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return d_OK;
}

int inCTOS_CASH_ADVANCE(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_CashAdvanceFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

