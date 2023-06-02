/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>


#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\comm\V5comm.h"
#include "..\print\print.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"

#include "../FileModule/myFileFunc.h"

int inCTOS_PreAuthFlowProcess(void)
{
    int inRet = d_NO;

    srTransRec.fCredit=TRUE;

    vdCTOS_SetTransType(PRE_AUTH);
    
    vdDispTransTitle(PRE_AUTH);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCardFields();
    if(d_OK != inRet)
        return inRet;

	if(inCheckValidApp(BANCNET_HOST) != d_OK)
		return INVALID_CARD;

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRE_AUTH);
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

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_FraudControl();
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

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;   

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;

    inRet = inProcessAdviceTCTrail(&srTransRec);
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");      
    return d_OK;
}

int inCTOS_PREAUTH(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_PreAuthFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}
