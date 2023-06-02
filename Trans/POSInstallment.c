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
#include "..\Includes\POSInstallment.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"

#include "..\Database\DatabaseFunc.h"

int inInstallmentGroup=0;

int inCTOS_InstallmentFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

	srTransRec.fInstallment=TRUE;

    vdCTOS_SetTransType(SALE);
    
    //display title
    vdDispTransTitle(SALE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	if(inInstallmentGroup == 1)
	{
        inRet = inCTOS_GetCardFieldsNoEMV();	
        if(d_OK != inRet)
            return inRet;

        if (inMultiAP_CheckSubAPStatus() != d_OK)
        {
            if((strCDT.inInstGroup != 2) && (strCDT.inInstGroup != 3)) /*diners and sb mc installment*/
            {
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return INVALID_CARD;	
            }
        }
	}
	else if(inInstallmentGroup == 2)
	{
        inRet = inCTOS_GetCardFieldsNoEMV();	
        if(d_OK != inRet)
            return inRet;

        if (inMultiAP_CheckSubAPStatus() != d_OK)
        {
            if(strCDT.inInstGroup != 4) /*store card installment*/
            {
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return INVALID_CARD;	
            }
        }
	}
	else
	{
        inRet = inCTOS_GetCardFields();   
        if(d_OK != inRet)
            return inRet;
        
        if (inMultiAP_CheckSubAPStatus() != d_OK)
        {
            if(strCDT.inInstGroup != 1) /*BANCNET installment*/
            {
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return INVALID_CARD;	
            }
        }
	}
	
	/*Installment: Promo selection*/
    inRet = inCTOS_SelectInstallmentPromo(strCDT.inInstGroup);
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_INSTALLMENT);
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

    //inRet = inCTOS_GetTxnTipAmount();
    //if(d_OK != inRet)
        //return inRet;

    inRet = inCTOS_SelectInstallmentTerm();
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

int inCTOS_INSTALLMENT(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	inInstallmentGroup=0;

    inRet = inCTOS_InstallmentFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_DinersINST(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	inInstallmentGroup=1;

    inRet = inCTOS_InstallmentFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_StoreCardINST(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	inInstallmentGroup=2;

    inRet = inCTOS_InstallmentFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_SelectInstallmentPromo(int inGroup)
{
    int inNumRecs = 0,
    key = 0,
    i,
    bHeaderAttr = 0x01+0x04; 
    char szHeaderString[21];
    char szPromoLabelList[100] = {0};
    char szTransactionList[100] = {0};
    int inIndex=0;
	
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if (strCDT.fInstallmentEnable == 0 || strCDT.inType == DEBIT_CARD)
    {
		vdSetErrorMessage("INSTALLMENT NOT ALLWD");
		return(ST_ERROR);
    }

	srTransRec.fInstallment=TRUE;
	
    //Select Instalment Promo type
    memset(szHeaderString, 0x00,sizeof(szHeaderString));
    memset(szPromoLabelList, 0x00,sizeof(szPromoLabelList));
	strcpy(szHeaderString,"SELECT PROMO");

    memset(strMultiPRM, 0, sizeof(strMultiPRM));

	vdDebug_LogPrintf("sizeof(strMultiPRM):(%d)", sizeof(strMultiPRM));
	
	inPRMReadbyinInstGroup(inGroup, &inNumRecs);
	
	for (i=0; i<inNumRecs; i++)
	{
		if (strMultiPRM[i].szPromoLabel[0] != 0)
		{
			inIndex=i;
			strcat((char *)szPromoLabelList, strMultiPRM[i].szPromoLabel);
			if (strMultiPRM[i+1].szPromoLabel[0] != 0)
				strcat((char *)szPromoLabelList, (char *)" \n");			
		}
	}

    if(inNumRecs > 1)
        key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szPromoLabelList, TRUE);
    else
        key = inIndex+1;                		

	if (key > 0)
	{
		if (key == d_KBD_CANCEL)
		{
			return -1;
		}

		strCDT.HDTid = strMultiPRM[key-1].HDTid;
		memcpy(srTransRec.szPromo, strMultiPRM[key-1].szPromoCode, 2);	
        strcpy(srTransRec.szPromoLabel, strMultiPRM[key-1].szPromoLabel);
		vdDebug_LogPrintf("srTransRec.szPromo:(%s)\nkey:(%d)", srTransRec.szPromo, key);
		key = d_OK;
	}

	return(key);	
}

int inCTOS_SelectInstallmentTerm(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT TERM";
	char szTermMenu[1024];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;
    
    memset(szTermMenu, 0x00, sizeof(szTermMenu));
    memset(szTerm, 0x00, sizeof(szTerm));

    memset(&strINS, 0, sizeof(strINS));
	vdDebug_LogPrintf("inCTOS_SelectInstallmentTerm");
	inINSRead(strHDT.inHostIndex);
	
	vdDebug_LogPrintf("inINSRead[%d], strHDT.inHostIndex[%d]", inINSRead(strHDT.inHostIndex), strHDT.inHostIndex);
	vdDebug_LogPrintf("strINS.szTerm1[%s]", strINS.szTerm1);

		
    if(strINS.fTerm1Enable == TRUE)   
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm1); 
		inTermIndex++;
    }

    if(strINS.fTerm2Enable == TRUE)     
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm2); 
		inTermIndex++;
    }

    if(strINS.fTerm3Enable == TRUE) 
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm3); 
		inTermIndex++;
    }
	
    
    if(strINS.fTerm4Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm4); 
		inTermIndex++;
    }
    
    if(strINS.fTerm5Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm5); 
		inTermIndex++;
    }
    
    if(strINS.fTerm6Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm6); 
		inTermIndex++;
    }
    
    if(strINS.fTerm7Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm7); 
		inTermIndex++;
    }
     
    if(strINS.fTerm8Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm8); 
		inTermIndex++;
    }
    
    if(strINS.fTerm9Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm9); 
		inTermIndex++;
    }
    
    if(strINS.fTerm10Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm10); 
		inTermIndex++;
    }
    
    if(strINS.fTerm11Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm11); 
		inTermIndex++;
    }
    
    if(strINS.fTerm12Enable == TRUE) 	
    {
        strcat(&szTerm[inTermIndex][0],strINS.szTerm12); 
		inTermIndex++;
    }

    for (inLoop = 0; inLoop < 50; inLoop++)
    {
        if (szTerm[inLoop][0]!= 0)
        {
			inIndex=inLoop;
            strcat((char *)szTermMenu, szTerm[inLoop]);
            if (szTerm[inLoop+1][0]!= 0)
                strcat((char *)szTermMenu, (char *)" \n"); 
			vdDebug_LogPrintf("inLoop:(%d), szTerm[%s]", inLoop, szTerm[inLoop]);
        }
        else
            break;
    }

    vdDebug_LogPrintf("all szTermMenu[%s]", szTermMenu);

	if(inTermIndex > 1)
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szTermMenu, TRUE);
    else
        key = inIndex+1;   
	
    if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "INVALID TERMS!!!");
        vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;

        strcpy(srTransRec.szTerms,szTerm[key-1]);  
		key=d_OK;
    }
    
    return key;
}

