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

int inSaleGroupIndex=0;

int inCTOS_SaleFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

    srTransRec.fCredit=TRUE;
		
    vdCTOS_SetTransType(SALE);
    
    //display title
    vdDispTransTitle(SALE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    if(inSaleGroupIndex)
    {
        inRet = inCTOS_GetCardFieldsNoEMV();   
        if(d_OK != inRet)
            return inRet;
		
        if(inCheckValidApp(DINERS_HOST) != d_OK)
            return INVALID_CARD;
    }
	else
	{
        inRet = inCTOS_GetCardFields();   
        if(d_OK != inRet)
            return inRet;
		
        if(inCheckValidApp(BANCNET_HOST) != d_OK)
            return INVALID_CARD;			
	}
	
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

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

    inRet = inCTOS_GetTxnTipAmount();
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

    inRet = inCTOS_UpdateAccumTotal();
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

int inCTOS_SALE(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();

    inSaleGroupIndex=0;
    
    inRet = inCTOS_SaleFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DinersSALE(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	inSaleGroupIndex=6;

    inRet = inCTOS_SaleFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inDisplayMsg1(void)
{
     int i = 1;

	  for(i=1;i<=16;i++)
	  {
	      CTOS_LCDTPrintXY(1, i, "测试");
	  }

	  CTOS_Delay(5*1000);

	 
	 

   // CTOS_LCDTPrintXY(1, 2, "开始");
   //CTOS_LCDTPrintXY(10, 8, "结束");

/*    CTOS_LCDTPrintAligned(1, "First Line", d_LCD_ALIGNCENTER);
    CTOS_LCDTPrintAligned(2, "Second Line",d_LCD_ALIGNRIGHT);
    CTOS_LCDTPrintAligned(3, "Third Line", d_LCD_ALIGNLEFT);
    CTOS_LCDTPrintAligned(4, "Fourth Line",d_LCD_ALIGNCENTER);
	CTOS_LCDTPrintAligned(5, "Fifth Line", d_LCD_ALIGNRIGHT);
	CTOS_LCDTPrintAligned(6, "Six Line",   d_LCD_ALIGNLEFT);*/
	CTOS_LCDTPrintAligned(1, "First Line", d_LCD_ALIGNCENTER);
	CTOS_LCDTPrintAligned(2, "Second Line",   d_LCD_ALIGNLEFT);
	CTOS_LCDTPrintAligned(3, "Third Line",   d_LCD_ALIGNRIGHT);
	CTOS_LCDTPrintAligned(16, "Finish",	 d_LCD_ALIGNRIGHT);

	 CTOS_Delay(10*1000);

	 CTOS_LCDTClearDisplay();

	

    return d_OK;
}

int inDisplayImage(void)
{
   char szFileName[50+1];
   memset(szFileName, 0x00, sizeof(szFileName));
 
   sprintf(szFileName, "%s" , "p1");
   CTOS_LCDGShowBMPPic(0, 800, szFileName);

    return d_OK;
}



