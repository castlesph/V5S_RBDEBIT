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
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSbatch.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\ui\Display.h"
#include "..\accum\accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"


int inCTOS_TipAdjustFlowProcess(void)
{
    int inRet = d_NO;


    vdCTOS_SetTransType(SALE_TIP);
    
    //display title
    //vdDispTransTitle(SALE_TIP); //aaronnino for BDOCLG ver 9.0 fix on issue #00096 Flickering display title upon search of invoice number for void and tip adjust 1 of 2

    inRet = inCTOS_CheckTipAllowd();
    if(d_OK != inRet)
        return inRet;
       
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GeneralGetInvoice();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_TIP_ADJUST);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetVoid();
            if(d_OK != inRet)
                return inRet;
        }       
        inRet = inCTOS_BatchSearch();
        if(d_OK != inRet)
            return inRet;

    }

	inRet = inCTOS_CheckHOSTTipAllowd();	
	if(d_OK != inRet)
		return inRet;

    inRet = inCTOS_CheckTipadjust();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_LoadCDTandIIT();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_VoidSelectHost();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_ConfirmInvAmt();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTipAfjustAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
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
    else
        vdSetErrorMessage("");

    
    return d_OK;
}

int inCTOS_TIPADJUST(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_TipAdjustFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_CheckHOSTTipAllowd(void)
{   
#if 1
	/* BDO: Tip Allow flag moved to HDT - start -- jzg */
	inHDTRead(srTransRec.HDTid);

	if(strHDT.fHDTTipAllow != TRUE)
	{
		//vdSetErrorMessage("TIP NOT ALLOWED");
		CTOS_LCDTClearDisplay();
		vdDisplayErrorMsgResp2(" ", "TIP", "NOT ALLOWED");
		return d_NO;
	}
	else
		return d_OK;
	/* BDO: Tip Allow flag moved to HDT - end -- jzg */
#else
    // allow only tip for bdo, amex, bpi, diners and bankard
	if ((srTransRec.HDTid == 1) || (srTransRec.HDTid == 2) || (srTransRec.HDTid == 3) ||(srTransRec.HDTid == 4) ||(srTransRec.HDTid == 6) ||(srTransRec.HDTid == 23) ||(srTransRec.HDTid == 24)){    
		return d_OK;
	}else{
		vdSetErrorMessage("TIP NOT ALLOWED");
		return d_NO;
	}
#endif
}


