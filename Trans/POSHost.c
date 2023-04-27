/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>


#include "..\Includes\Wub_lib.h"

#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\Display.h"

#include "..\Includes\V5IsoFunc.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\Trans.h"   


#include "..\Includes\CTOSInput.h"


#include "..\debug\debug.h"
#include "..\Accum\Accum.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\accum\accum.h"
#include "..\Includes\POSSetting.h"
#include "..\Debug\Debug.h"
#include "..\filemodule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\CTOSInput.h"
#include "..\Ctls\PosWave.h"


#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"

//1030
extern BOOL fBinVerInitiatedSALE;
//1030



extern int gvSettleType; 
BOOL gblfBatchEmpty = FALSE;  //aaronnino for BDOCLG ver 9.0 fix on issue #0033 and #0034 "settle failed" error response after "comm error"  1 of 8

/*BDO: For ECR hold response code -- sidumili */
extern BOOL fECRTxnFlg;
BYTE szORIGInvNo[TRACE_NO_BCD_SIZE+1];

#define d_NOT_RECORD            102

#define d_GETPIN_TIMEOUT        6000
#define d_DUPLICATE_INVOICE     0x0080

int inCTOS_GetTxnBaseAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;

	//if (1 == inCTOSS_GetWaveTransType())
		//if (inMultiAP_CheckSubAPStatus() == d_OK)
		if ((inMultiAP_CheckSubAPStatus() == d_OK) && (fBinVerInitiatedSALE == VS_FALSE))
	//1030		
        return d_OK;

	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
		return d_OK;
	
    CTOS_LCDTClearDisplay();
    vduiLightOn();
    
    vdDispTransTitle(srTransRec.byTransType);

    if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
    {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr+3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);
    }
    //else
    //{
    //	if (1 != inCTOSS_GetWaveTransType())
    //    	inCTOS_DisplayCardTitle(4, 5);
    //}
    
    CTOS_KBDHit(&key);//clear key buffer
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    CTOS_LCDTPrintXY(1, 7, "AMOUNT:");

    memset(baAmount, 0x00, sizeof(baAmount));
    //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
	key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);

    if(d_OK == key)
    {
        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp, "%012ld", atol(baAmount));
        wub_str_2_hex(szTemp,srTransRec.szBaseAmount,12);
        
        // sidumili: Issue#:000076 [check transaction maximum amount]
		if (inCTOS_ValidateTrxnAmount()!= d_OK){
			return(d_NO);
		}
    }
        
    if(0xFF == key)
    {
        vdSetErrorMessage("Amt entry cancelled");
        return d_NO;
    }

	/*sidumili: [USER press cancel]*/
	if (d_USER_CANCEL == key){
			vdDispTransTitle(srTransRec.byTransType);
			vdSetErrorMessage("Amt entry cancelled");
			return d_NO;
	}
	/*sidumili: [USER press cancel]*/

    return key;
}

int inCTOS_GetTxnTipAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;

	//1030
		BYTE szBaseAmount[12+1];
		BYTE szStr[45];
	//1030

    DebugAddSTR("inCTOS_GetTxnTipAmount","Processing...",20);
	
	//1030
		//if (inMultiAP_CheckSubAPStatus() == d_OK)
		if ((inMultiAP_CheckSubAPStatus() == d_OK) && (fBinVerInitiatedSALE == VS_FALSE))
	//1030		
        return d_OK;


    if (d_OK != inCTOS_CheckTipAllowd())
        return d_OK;

	//if (1 == inCTOSS_GetWaveTransType())
	//if (inMultiAP_CheckSubAPStatus() == d_OK)
    //    return d_OK;
    
    memset(baBaseAmount, 0x00, sizeof(baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
        
    do
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
//1030
		//gcitra - remove card display
		/*
		

        if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
        {
            setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
            memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
            strcpy(bDisplayStr, srTransRec.szPAN);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

            memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
            memset(bBuf, 0x00, sizeof(bBuf));
            wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
            memcpy(bDisplayStr, bBuf, 2);
            bDisplayStr[2] = '/';
            memset(bBuf, 0x00, sizeof(bBuf));
            wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
            memcpy(bDisplayStr+3, bBuf, 2);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);
        }
        else
        {
        	if (1 != inCTOSS_GetWaveTransType())
            	inCTOS_DisplayCardTitle(4, 5);
        }

*/
//gcitra
//1030

        CTOS_KBDHit(&key);//clear key buffer

//1030
		//gcitra - display Base amount during TIP prompt
				CTOS_LCDTPrintXY(1, 4, "AMOUNT:");
		
				memset(szBaseAmount,0x00,sizeof(szBaseAmount));
		
				wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, AMT_BCD_SIZE); 	
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "%lu.%02lu", atol(szBaseAmount)/100, atol(szBaseAmount)%100);
				setLCDPrint(5, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
				CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szStr)+1)*2,  5, szStr);
		//gcitra
//1030

        CTOS_LCDTPrintXY(1, 7, "TIP:");

        memset(baAmount, 0x00, sizeof(baAmount));
        //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
        key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

        if(d_OK == key)
        {
        
            if(0 == strlen(baAmount)) //by pass TIP
                return d_OK;
            
            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%012ld", atol(baAmount));
            if(strcmp(szTemp, baBaseAmount) > 0)
            {
                clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(8);
                continue;
            }
            
            wub_str_2_hex(szTemp,srTransRec.szTipAmount,12);
            
            break;
        }
        else if(d_USER_CANCEL == key)
        {   
            clearLine(8);
            vdSetErrorMessage("USER CANCEL");
            clearLine(8);
            return d_NO;
        }
        else if(0xFF == key)
        {   
            clearLine(8);
            vdSetErrorMessage("TIME OUT");
            clearLine(8);
            return d_NO;
        }
        
        
    }while(1);
    
    return key;
}


int inCTOS_GetTipAfjustAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE baTipAmount[20];
    ULONG ulAmount = 0L;
    BYTE szAmtTmp2[16+1];

    DebugAddSTR("inCTOS_GetTipAfjustAmount","Processing...",20);
    memset(baBaseAmount, 0x00, sizeof(baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(baTipAmount, 0x00, sizeof(baTipAmount));
    memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
    wub_hex_2_str(srTransRec.szTipAmount, baTipAmount, 6);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(3, 4);
    
    
    memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
    sprintf(szAmtTmp2, "%lu.%02lu", atol(baTipAmount)/100, atol(baTipAmount)%100);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ORG TIP");
    setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szAmtTmp2)+1)*2,  6, szAmtTmp2);
    
    CTOS_LCDTPrintXY(1, 7, "TIP");
             
    do
    {
        CTOS_KBDHit(&key);//clear key buffer

        memset(baAmount, 0x00, sizeof(baAmount));
        //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
        key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

        if(strlen(baAmount) > 0)
        {
            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%012ld", atol(baAmount));
            if(strcmp(szTemp, baBaseAmount) > 0)
            {
                clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(8);
                continue;
            }
            
            wub_str_2_hex(szTemp,srTransRec.szTipAmount,12);
            
            break;
        }
        else if(d_USER_CANCEL == key)
        {   
            vdSetErrorMessage("USER CANCEL");
            return d_NO;
        }
        else if(0xFF == key)
        {   
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }
        
    }while(1);
    
    return key;
}



int inCTOS_UpdateTxnTotalAmount(void)
{
    BYTE szBaseAmount[20];
    BYTE szTipAmount[20];
    BYTE szTotalAmount[20];
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
    BYTE  byDataTmp1[32];
    BYTE  byDataTmp2[32];
    BYTE  bPackSendBuf[256];
    USHORT usPackSendLen = 0;
    USHORT ushEMVtagLen;
    ULONG lnTmp;


	//1030
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		//if ((inMultiAP_CheckSubAPStatus() == d_OK) && (fBinVerInitiatedSALE == VS_FALSE))
	//1030		
        return d_OK;

    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szTipAmount, 0x00, sizeof(szTipAmount));
    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));

    wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);

    sprintf(szTotalAmount, "%012ld", atol(szBaseAmount) + atol(szTipAmount));
    wub_str_2_hex(szTotalAmount,srTransRec.szTotalAmount,12);
    
    DebugAddSTR(szTotalAmount,szTipAmount,12); 
    DebugAddSTR("Total amount","Tip amount",12); 
    
    DebugAddHEX("Tip amount",srTransRec.szTipAmount,6); 
    DebugAddHEX("Total amount",srTransRec.szTotalAmount,6); 

    if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
    {
        memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
        memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
        wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
        lnTmp = atol(byDataTmp1);
        wub_long_2_array(lnTmp, byDataTmp2);

        memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
        bPackSendBuf[usPackSendLen++] = 0x04;
        memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
        usPackSendLen += 4;
        
        memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szTotalAmount, 6);
        
        memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x02", 2);
        usPackSendLen += 2;
        bPackSendBuf[usPackSendLen++] = 0x06;
        memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F02, 6);
        usPackSendLen += 6;

        if(atol(szTipAmount) > 0)
        {
            memcpy(srTransRec.stEMVinfo.T9F03, srTransRec.szTipAmount, 6);
        }
        else
        {
            memset(szTipAmount, 0x00, sizeof(szTipAmount));
            memcpy(srTransRec.stEMVinfo.T9F03, szTipAmount, 6);
        }

        memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x03", 2);
        usPackSendLen += 2;
        bPackSendBuf[usPackSendLen++] = 0x06;
        memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F03, 6);
        usPackSendLen += 6;

        usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);
    }
    
    return d_OK;
}



int inCTOS_GetOffApproveNO(void)
{
    USHORT usX =1, usY = 6;
    BYTE bShowAttr = 0x02; 
    USHORT szAuthCodeLen = 6;
    BYTE baPIN[6 + 1];
    BYTE   szAuthCode[6+1];
    BYTE bRet;


    memset(szAuthCode, 0x00, sizeof(szAuthCode));


    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Approve NO: ");

    while(TRUE)
    {
        bRet = InputStringAlpha(1, 8, 0x00, 0x02, szAuthCode, &szAuthCodeLen, 0, d_INPUT_TIMEOUT);

        if (bRet == d_KBD_CANCEL )
        {
            CTOS_LCDTClearDisplay();    
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }

        if(strlen(szAuthCode) >= 2)
        {
            strcpy(srTransRec.szAuthCode, szAuthCode);
            break;
        }
        else
        {
            memset(szAuthCode, 0x00, sizeof(szAuthCode));
            szAuthCodeLen = 6;
        }
    }

    return ST_SUCCESS;
}

int inCTOS_GetInvoice(void)
{

    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02; 
    USHORT usInvoiceLen = 6;
    BYTE baPIN[6 + 1];
    BYTE   szInvNoAsc[6+1];
    BYTE   szInvNoBcd[3];
    BYTE bRet;
    int iLens = 6;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];
    
    TRANS_DATA_TABLE srTransRecTemp;
    
    DebugAddSTR("inCTOS_GetInvoice","Processing...",20);
	
    if (!strTCT.fManualEntryInv)
    {
        memset(szBcd, 0x00, sizeof(szBcd));
        memcpy(szBcd, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);

		memset(szORIGInvNo, 0x00, sizeof(szORIGInvNo));
		memcpy(szORIGInvNo, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);	

		if(get_env_int("BINROUTE") == TRUE)//Fix to invoice number incrementing twice on Bin Routing setup.
			return d_OK;
		
        inBcdAddOne(szBcd, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);
        
        if((inResult = inTCTSave(1)) != ST_SUCCESS)
        {
            vdSetErrorMessage("LOAD TCT ERR");
            return ST_ERROR;
        }

        memcpy(srTransRec.szInvoiceNo,strTCT.szInvoiceNo, 3);
    }
    else
    {
        memset(szInvNoAsc, 0x00, sizeof(szInvNoAsc));
        memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));
        
        memset((char*)&srTransRecTemp, 0x00, sizeof(TRANS_DATA_TABLE));
        memcpy(&srTransRecTemp, &srTransRec, sizeof(TRANS_DATA_TABLE));
        
        while(TRUE)
        {
            if (strTCT.fECR) // tct
            {
                //If ECR send INV
                if (memcmp(srTransRec.szInvoiceNo, "\x00\x00\x00", 3) != 0)
                {                    
                    inResult = inCTOSS_BatchCheckDuplicateInvoice();
                    vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice[%d]", inResult);
                    
                    memcpy(&srTransRec, &srTransRecTemp, sizeof(TRANS_DATA_TABLE));
                    if (d_OK == inResult)
                    {
                        CTOS_LCDTClearDisplay();    
                        vdDisplayErrorMsg(1, 8, "DUPLICATE INVOICE");

                        return (d_NO);
                    }
                    else
                    {
                        return (d_OK);
                    }
                }
            }
            
            CTOS_LCDTClearDisplay();
            vdDispTransTitle(srTransRecTemp.byTransType);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "Invoice No: ");

            bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
            if (bRet == d_KBD_CANCEL )
            {
                CTOS_LCDTClearDisplay();    
                vdSetErrorMessage("USER CANCEL");
                memcpy(&srTransRec, &srTransRecTemp, sizeof(TRANS_DATA_TABLE));
                return (d_EDM_USER_CANCEL);
            }
            
            
            if(atoi(szInvNoAsc) != 0)
            {
                inAscii2Bcd(szInvNoAsc, szInvNoBcd, INVOICE_BCD_SIZE);
        
                memcpy(srTransRec.szInvoiceNo,szInvNoBcd,3);
                inResult = inCTOSS_BatchCheckDuplicateInvoice();
                vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice[%d]", inResult);
                if (d_OK == inResult)
                {
                    CTOS_LCDTClearDisplay();    
                    vdDisplayErrorMsg(1, 8, "DUPLICATE INVOICE");
                    memset(szInvNoAsc, 0x00, sizeof(szInvNoAsc));
                    usInvoiceLen = 6;

                    memset(srTransRec.szInvoiceNo, 0x00, sizeof(srTransRec.szInvoiceNo));
                    continue;
                }
                else
                {
                    break;
                }
            }       
        }   

        memcpy(&srTransRec, &srTransRecTemp, sizeof(TRANS_DATA_TABLE));
        memcpy(srTransRec.szInvoiceNo,szInvNoBcd,3);
    }
    
    return (d_OK);
}

VS_BOOL fAmountLessThanFloorLimit(void) 
{
    long lnTotalAmount = 0;
    BYTE   szTotalAmt[12+1];
    
    /* If the transaction amount is less than the floor limit,
        Set the transaction type to OFFLINE */

    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
    lnTotalAmount = atol(szTotalAmt);
    
    if (lnTotalAmount < strCDT.InFloorLimitAmount)
        return(d_OK);

    return(d_NO);
}

int inCTOS_CustComputeAndDispTotal(void)
{
    CTOS_RTC SetRTC;
    BYTE    szTotalAmt[12+1];
    BYTE    szTempBuf[12+1];
    BYTE    szTempBuf1[12+1]; 
    BYTE    szDisplayBuf[30];
    BYTE    szStr[45];
    
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

    CTOS_LCDTPrintXY(1, 5, "TOTAL AMOUNT :");

    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
    memset(szStr, 0x00, sizeof(szStr));
    sprintf(szStr, "%lu.%02lu", atol(szTotalAmt)/100, atol(szTotalAmt)%100);
    setLCDPrint(6, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szStr)+1)*2,  6, szStr);
            
    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        return (d_OK);
    }

    if(srTransRec.byTransType != SALE)
    {
        return (d_OK);
    }
    
    if (fAmountLessThanFloorLimit() == d_OK)
    {
        srTransRec.shTransResult = TRANS_AUTHORIZED;
        srTransRec.byOffline = CN_TRUE;

        //Read the date and the time //
        CTOS_RTCGet(&SetRTC);  
        /* Set Month & Day*/
        memset(szTempBuf,0,sizeof(szTempBuf));
        sprintf(szTempBuf,"%02d%02d",SetRTC.bMonth,SetRTC.bDay);
        wub_str_2_hex(szTempBuf,srTransRec.szDate,4);
        sprintf(szTempBuf1,"%02d",SetRTC.bYear);
        memcpy(srTransRec.szYear,szTempBuf1,2);
        memset(szTempBuf,0,sizeof(szTempBuf));
        sprintf(szTempBuf,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        wub_str_2_hex(szTempBuf,srTransRec.szTime,6);
    }
        
    return(d_OK);
}



//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated start 5 of 5
#if 0
int inCTOS_DisplayResponse(void)
{
    int inMsgid = atoi(srTransRec.szRespCode);
    int inHostIndex = srTransRec.HDTid;
    int inResult = 0;
    char szResponseCode[40];

    memset(szResponseCode, 0x00, sizeof(szResponseCode));
    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
    inMSGResponseCodeRead(szResponseCode, inMsgid, inHostIndex);  
    if(0 == strlen(szResponseCode) )
    {
        inMSGResponseCodeRead(szResponseCode, inMsgid, 1);
        if(0 != strlen(szResponseCode) )
            vdDisplayErrorMsg(1, 8, szResponseCode);
    }
    else
    {
        vdDisplayErrorMsg(1, 8, szResponseCode);
    }

    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
    return(d_OK);  
}
#else
int inCTOS_DisplayResponse(void)
{
    int inMsgid = atoi(srTransRec.szRespCode);
    int inHostIndex = srTransRec.HDTid;
    int inResult = 0;
    char szResponseCode[40], szResponseCode2[40], szResponseCode3[40];

    CTOS_LCDTClearDisplay();
    
    memset(szResponseCode, 0x00, sizeof(szResponseCode));
    memset(szResponseCode2, 0x00, sizeof(szResponseCode2));
    memset(szResponseCode3, 0x00, sizeof(szResponseCode3));
    
    inMSGResponseCodeRead(szResponseCode, szResponseCode2, szResponseCode3, inMsgid, 1);

	if (inMsgid == 81){
		strcpy(szResponseCode, "LOGON REQUIRED");	
		memset(szResponseCode2, 0x00, sizeof(szResponseCode2));
		memset(szResponseCode3, 0x00, sizeof(szResponseCode3));
	}
    
    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode2[%s]", inMsgid, inHostIndex, szResponseCode2);
    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode3[%s]", inMsgid, inHostIndex, szResponseCode3);
    
    vdDebug_LogPrintf("szResponseCode[%d]szResponseCode2[%d]szResponseCode3[%d]",strlen(szResponseCode),strlen(szResponseCode2),strlen(szResponseCode3));
    
    if((0 != strlen(szResponseCode)) && (0 != strlen(szResponseCode2)) && (0 != strlen(szResponseCode3)))
    {
          //vdDisplayErrorMsgResp(1, 1, 1, 6, 7, 8, szResponseCode, szResponseCode2, szResponseCode3); 
          vdDisplayErrorMsgResp2(szResponseCode, szResponseCode2, szResponseCode3); 
    }
    if((0 != strlen(szResponseCode)) && (0 != strlen(szResponseCode2)) && (0 == strlen(szResponseCode3)))
    {
          //vdDisplayErrorMsgResp(1, 1, 1, 6, 7, 8, " ", szResponseCode, szResponseCode2);
          vdDisplayErrorMsgResp2(" ", szResponseCode, szResponseCode2); 
    }
    if((0 != strlen(szResponseCode)) && (0 == strlen(szResponseCode2)) && (0 == strlen(szResponseCode3)))
    {
          //vdDisplayErrorMsgResp(1, 1, 1, 6, 7, 8, " ", " ", szResponseCode);	
          vdDisplayErrorMsgResp2(" ", " ", szResponseCode);
    }

     return(d_OK);  
}
#endif
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated end 5 of 5




int inCTOS_GeneralGetInvoice(void)
{
    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02; 
    USHORT usInvoiceLen = 6;
    BYTE baPIN[6 + 1];
    BYTE   szInvNoAsc[6+1];
    BYTE   szInvNoBcd[3];
    BYTE bRet;
    int iLens = 6;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];

    if (inMultiAP_CheckSubAPStatus() == d_OK)
    {
        return d_OK;
    }

    memset(szInvNoAsc, 0x00, sizeof(szInvNoAsc));
    memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));
    
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Invoice No: ");
    
    while(TRUE)
    {
        usInvoiceLen = 6;
        vdDebug_LogPrintf("11bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szInvNoAsc),usInvoiceLen);
        bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
        vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szInvNoAsc),usInvoiceLen);
        if (bRet == d_KBD_CANCEL )
        {
            CTOS_LCDTClearDisplay();    
            vdSetErrorMessage("USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }
        
        
        if(atoi(szInvNoAsc) != 0)
        {
            inAscii2Bcd(szInvNoAsc, szInvNoBcd, INVOICE_BCD_SIZE);
            DebugAddSTR("INV NUM:",szInvNoAsc,12);  
            memcpy(srTransRec.szInvoiceNo,szInvNoBcd,3);
            break;
        } 
    }

    return ST_SUCCESS;
}


int inCTOS_BatchSearch(void)
{
    int inResult = d_NO;

    inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);

    DebugAddSTR("inCTOS_BatchSearch","Processing...",20);
    
    DebugAddINT("inCTOS_BatchSearch", inResult);

    if(inResult != d_OK)
    {   
        if (inMultiAP_CheckSubAPStatus() != d_OK)
            //vdSetErrorMessage("NO RECORD FOUND");
            vdDisplayErrorMsgResp2("","TRANSACTION","NOT FOUND");
        return d_NOT_RECORD;
    }
    
    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 2);
    
    return inResult;
}

int inCTOS_LoadCDTandIIT(void)
{
	if ( inCDTRead(srTransRec.CDTid) != d_OK)
	{
        vdDebug_LogPrintf("inCTOS_LoadCDTandIIT LOAD CDT ERROR [%d]", srTransRec.CDTid);
		vdSetErrorMessage("LOAD CDT ERROR");
		return(d_NO);
    }
    inGetIssuerRecord(strCDT.IITid);

    return ST_SUCCESS;
}


int inCTOS_CheckTipadjust()
{
    if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_TIP))
    {
        if(srTransRec.byTransType == SALE)
            srTransRec.byOrgTransType = srTransRec.byTransType;
        
        memcpy(srTransRec.szOrgAmount, srTransRec.szTotalAmount, 6);
    }
    else
    {
        //vdSetErrorMessage("TIP NOT ALLOWED");
        CTOS_LCDTClearDisplay();
		vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
        return d_NO;
    }

    srTransRec.byTransType = SALE_TIP;

    return d_OK;
}




int inCTOS_CheckVOID()
{
    if(srTransRec.byVoided == TRUE)
    {
        vdSetErrorMessage("RECORD VOIDED");
        return d_NO;
    }

    if(srTransRec.byTransType == PRE_AUTH)
    {
        //vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED"); 
        vdDisplayErrorMsgResp2("","TRANSACTION","NOT ALLOWED");
        return d_NO;
    }

    //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - START
    if(srTransRec.byTransType == SALE_TIP)
    {
        szOriginTipTrType = srTransRec.byOrgTransType;
        srTransRec.byOrgTransType = srTransRec.byTransType;
    }
    else //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - END
        srTransRec.byOrgTransType = srTransRec.byTransType;

    srTransRec.byTransType = VOID;

    return d_OK;
}


int inCTOS_VoidSelectHost(void) 
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;

    inHostIndex = (short) srTransRec.HDTid;
    
     DebugAddSTR("inCTOS_SelectHost","Processing...",20);
     
    if ( inHDTRead(inHostIndex) != d_OK)
    {
        vdSetErrorMessage("HOST SELECTION ERR");
        return(d_NO);
    } 
    else 
    {
        
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTRead(inCurrencyIdx) != d_OK) {
            
        vdSetErrorMessage("LOAD CST ERR");
        return(d_NO);
        }

        if ( inMMTReadRecord(inHostIndex,srTransRec.MITid) != d_OK)
        {
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }

        if ( inCPTRead(inHostIndex) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }

        inCTOS_PreConnect();

        return (d_OK);
    }

    
}

int inCTOS_ChkBatchEmpty()
{
    int         inResult;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
    
    memset(&srAccumRec,0,sizeof(srAccumRec));
    memset(&strFile,0,sizeof(strFile));
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    memset(&strFile,0,sizeof(strFile));
    vdCTOS_GetAccumName(&strFile, &srAccumRec);

    if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) < 0)
    {
        if(CN_TRUE == strMMT[0].fMustSettFlag)
        {
            strMMT[0].fMustSettFlag = CN_FALSE;
            inMMTSave(strMMT[0].MMTid);
        }

		
        CTOS_LCDTClearDisplay();
        //if (srTransRec.byTransType == SETTLE)  // sidumili: Issue#:000109 [Display hostname during settle all]
		if ((srTransRec.byTransType == SETTLE) && (gvSettleType==MULTI_SETTLE))  //aaa fix on issue #000210 Terminal displays "Batch empty, skip" on all hosts when trying to settle hosts with no transactions 2 of 6
		{
			    gblfBatchEmpty = TRUE;  //aaronnino for BDOCLG ver 9.0 fix on issue #0033 and #0034 "settle failed" error response after "comm error" 2 of 8
				vdSetErrorMessage("EMPTY BATCH, SKIP");
		}
		else
                vdSetErrorMessage("EMPTY BATCH          ");

        return (d_NO);
    }
    
    return (d_OK);
}

int inCTOS_ConfirmInvAmt()
{
    BYTE szAmtTmp1[16+1];
    BYTE szAmtTmp2[16+1];
    char szDisplayBuf[30];
    BYTE   key;
    USHORT result;
	BYTE szTemp1[30+1];
	int inRemaining=0;
    BYTE szPAN1[20+1];
    BYTE szPAN2[20+1];
    
    CTOS_LCDTClearDisplay();
    vduiLightOn();
    
    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(3, 4);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtTmp1, 0x00, sizeof(szAmtTmp1));
    memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    wub_hex_2_str(srTransRec.szTotalAmount, szAmtTmp1, 6);
    sprintf(szAmtTmp2, "%lu.%02lu", atol(szAmtTmp1)/100, atol(szAmtTmp1)%100);
    
    //CTOS_LCDTPrintXY(1, 5, "AMOUNT");
    //setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szAmtTmp2)+1)*2,  6, szAmtTmp2);
    
    
	if(srTransRec.byTransType == VOID)
		 {
			memset(szTemp1,0,sizeof(szTemp1));
			
			inIITRead(strCDT.IITid);
			if (strIIT.fMaskPanDisplay == TRUE)
			{
			   vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
			   strcpy(szTemp1, srTransRec.szPAN);
			   cardMasking(szTemp1, 5); 	
			}
			else
			{	
			   vdCTOS_FormatPAN2(strTCT.DisplayPANFormat, srTransRec.szPAN, szTemp1);
			}	
			if (strlen(szTemp1) > 20)
			{
			   memset(szPAN1, 0x00, sizeof(szPAN1));
			   memset(szPAN2, 0x00, sizeof(szPAN2));
			   inRemaining = strlen(szTemp1) - 20;
			   
			   memcpy(szPAN1, szTemp1, 20);
			   memcpy(szPAN2, &szTemp1[20], inRemaining);
			   CTOS_LCDTPrintXY(1, 3, szPAN1);
			   CTOS_LCDTPrintXY(1, 4, szPAN2);
			}
			else
			   CTOS_LCDTPrintXY(1, 3, szTemp1);
			
			CTOS_LCDTPrintXY(1, 5, "AMOUNT");
			setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
			CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szAmtTmp2)+1)*2,  6, szAmtTmp2); 	
		 
		 }
		else
		{
		   CTOS_LCDTPrintXY(1, 4, "AMOUNT");
		   setLCDPrint(5, DISPLAY_POSITION_LEFT, szDisplayBuf);
		   CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szAmtTmp2)+1)*2,	5, szAmtTmp2);
		}
		
		if(srTransRec.byTransType == SALE_TIP)
			  setLCDPrint(7, DISPLAY_POSITION_CENTER,"CONFIRM TIP ADJUST");
		if(srTransRec.byTransType == VOID)
		{
		   setLCDPrint(7, DISPLAY_POSITION_CENTER,"CONFIRM VOID");
		   setLCDPrint(8, DISPLAY_POSITION_CENTER,"NO[X] YES[OK]");
		}
			else
				 setLCDPrint(8, DISPLAY_POSITION_CENTER,"NO[X] YES[OK]");
		
			CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
		vduiWarningSound();
	
		CTOS_KBDBufFlush();//cleare key buffer
		
		while(1)
		{ 
			
			CTOS_KBDHit(&key);
			if(key == d_KBD_ENTER)
			{
				result = d_OK;
				break;
			}
			else if((key == d_KBD_CANCEL))
			{
				result = d_NO;
				vdSetErrorMessage("USER CANCEL");
				break;
			}
			if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
			{
				result = d_NO;
				vdSetErrorMessage("TIME OUT");
				break;
			}		
		}
		
		return result;
	}
    

int inCTOS_SettlementSelectAndLoadHost(void)
{
    int key;
    char szBcd[INVOICE_BCD_SIZE+1];
    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    
    key = inCTOS_SelectHostSetting();
    if (key == -1)
    {
        return key;
    }

    memset(szBcd, 0x00, sizeof(szBcd));
    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
    inHDTSave(strHDT.inHostIndex);

    return d_OK;
}

int inCTOS_SettlementClearBathAndAccum(BOOL fManualSettlement)
{
    int         inResult;
    BYTE    szBcd[INVOICE_BCD_SIZE+1];
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    vdDebug_LogPrintf("inCTOS_SettlementClearBathAndAccum");

#if 0
    memset(szBcd, 0x00, sizeof(szBcd));
    memcpy(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);    
    inBcdAddOne(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);
    strMMT[0].fMustSettFlag = CN_FALSE;
    inMMTSave(strMMT[0].MMTid);
#endif        
    /*albert - start - 20161202 - Reprint of Detail Report for Last Settlement Report*/
    inDatabase_DeleteDetailReport(srTransRec.HDTid, srTransRec.MITid);
    inDatabase_BackupDetailReport(srTransRec.HDTid, srTransRec.MITid);
    /*albert - end - 20161202 - Reprint of Detail Report for Last Settlement Report*/
    
    inDatabase_BatchDelete();

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    inCTOS_ReadAccumTotal(&srAccumRec);
    strcpy(srAccumRec.szTID, srTransRec.szTID);
    strcpy(srAccumRec.szMID, srTransRec.szMID);
    memcpy(srAccumRec.szYear, srTransRec.szYear, DATE_BCD_SIZE);
    memcpy(srAccumRec.szDate, srTransRec.szDate, DATE_BCD_SIZE);
    memcpy(srAccumRec.szTime, srTransRec.szTime, TIME_BCD_SIZE);
    memcpy(srAccumRec.szBatchNo, srTransRec.szBatchNo, BATCH_NO_BCD_SIZE);
	srAccumRec.fManualSettlement=fManualSettlement;
    inCTOS_SaveAccumTotal(&srAccumRec);
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    memset(&strFile,0,sizeof(strFile));
    vdCTOS_GetAccumName(&strFile, &srAccumRec);
    vdDebug_LogPrintf("[strFile.szFileName[%s]", strFile.szFileName);
    vdCTOS_SetBackupAccumFile(strFile.szFileName);
    if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
    {
        vdDebug_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
    }

    inDatabase_InvoiceNumDelete(srTransRec.HDTid, srTransRec.MITid);
    inMyFile_ReversalDelete();

    inMyFile_AdviceDelete();

    inMyFile_TCUploadDelete();

#if 1
		memset(szBcd, 0x00, sizeof(szBcd));
		memcpy(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);	 
		inBcdAddOne(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);
		
		strMMT[0].fBatchNotEmpty = 0;	
		strMMT[0].fPendingReversal= 0;	
		strMMT[0].fMustSettFlag = CN_FALSE;
		strcpy(strMMT[0].szSettleDate,"00000000");// Reset Settle Date during clear batch
		inMMTSave(strMMT[0].MMTid);
#endif        

    return d_OK;
}

int inCTOS_DisplayBatchTotal(void)
{
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[20];
    BYTE szBuf[20];
    USHORT usSaleCount;
    double ulSaleTotalAmount;
    USHORT usRefundCount;
    ULONG  ulRefundTotalAmount;

		//BDO UAT 0003: Changed batch totals from refund to void - start -- jzg
    USHORT usVoidCount;
    double ulVoidTotalAmount;
		//BDO UAT 0003: Changed batch totals from refund to void - end -- jzg
		
    ACCUM_REC srAccumRec;

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(BATCH_TOTAL);
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return ST_ERROR;    
    }

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 1;

    usSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount) + (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReloadCCCount);
    ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) + (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReloadCCTotalAmount);

    usRefundCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
    ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);

		//BDO UAT 0003: Changed batch totals from refund to void - start -- jzg
    usVoidCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount);
    ulVoidTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);
		//BDO UAT 0003: Changed batch totals from refund to void - end -- jzg

#if 0		
		CTOS_LCDTSelectFontSize(d_FONT_12x24);
		
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    strcpy(szDisplayBuf, "Type      Cnt");
    CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
	CTOS_LCDTPrintAligned(3, "Amt", d_LCD_ALIGNRIGHT);


    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%012.0f", ulSaleTotalAmount);
    vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216		
    sprintf(szDisplayBuf, "SALE      %03d", usSaleCount);
    CTOS_LCDTPrintXY(1, 5, szDisplayBuf);	
	CTOS_LCDTPrintAligned(5, szAmtBuf, d_LCD_ALIGNRIGHT);
    
    
#if 0
		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
		memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
		memset(szBuf, 0x00, sizeof(szBuf));
		sprintf(szBuf, "%ld", ulRefundTotalAmount);
		vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216		
		sprintf(szDisplayBuf, "RFUD  %03d  %s", usRefundCount, szAmtBuf);
		CTOS_LCDTPrintXY(1, 6, szDisplayBuf);
#else
		//BDO UAT 0003: Changed batch totals from refund to void - start -- jzg
		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
		memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
		memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%012.0f", ulVoidTotalAmount);
    vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216		
		sprintf(szDisplayBuf, "VOID      %03d", usVoidCount);
		CTOS_LCDTPrintXY(1, 6, szDisplayBuf);	
		CTOS_LCDTPrintAligned(6, szAmtBuf, d_LCD_ALIGNRIGHT);
		//BDO UAT 0003: Changed batch totals from refund to void - end -- jzg
#endif
    

		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));

#if 0		
    sprintf(szBuf, "%ld", (ulSaleTotalAmount > ulRefundTotalAmount) ? (ulSaleTotalAmount-ulRefundTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount));
    vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216		
    memset(szBuf, 0x00, sizeof(szBuf));
    if(ulSaleTotalAmount > ulRefundTotalAmount)
        szBuf[0] = ' ';
    else
        szBuf[0] = '-';
    sprintf(szDisplayBuf, "TOTL  %03d %s%s", (usSaleCount + usRefundCount), szBuf, szAmtBuf);
#else
		//BDO UAT 0003: Changed batch totals from refund to void - start -- jzg
    sprintf(szBuf, "%012.0f", ulSaleTotalAmount);
    vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216		
		sprintf(szDisplayBuf, "TOTL      %03d", usSaleCount);
		//BDO UAT 0003: Changed batch totals from refund to void - end -- jzg
#endif
    CTOS_LCDTPrintXY(1, 9, szDisplayBuf);
	CTOS_LCDTPrintAligned(9, szAmtBuf, d_LCD_ALIGNRIGHT);

    CTOS_LCDTSelectFontSize(d_FONT_16x30);
		
    WaitKey(30);
#endif
	CTOS_LCDTSelectFontSize(d_FONT_12x24);
	//CTOS_LCDTSelectFontSize(d_FONT_16x30);

	inCSTRead(strHDT.inCurrencyIdx);
	CTOS_LCDTPrintXY(1, 3, "TYPE         CNT                   AMT");
	
	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
	memset(szBuf, 0x00, sizeof(szBuf));
	sprintf(szBuf, "%012.0f", ulSaleTotalAmount);
	//vdCTOS_FormatAmount(strCST.szAmountFormat, szBuf,szAmtBuf);// patrick add code 20141216 	
    vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216
    sprintf(szDisplayBuf,"%s%s",strCST.szCurSymbol,szAmtBuf);

	memset(szBuf,0x00,sizeof(szBuf));
	sprintf(szBuf,"SALES:        %d",usSaleCount);
	
#if 0
	if((strTCT.byTerminalType % 2) == 0){
    	CTOS_LCDTPrintXY(1, 5,"SALES:");	
    	CTOS_LCDTPrintAligned(5, szDisplayBuf, d_LCD_ALIGNRIGHT);
	}else{	
		CTOS_LCDTPrintXY(1, 3,"SALES:");	
		CTOS_LCDTPrintAligned(3, szDisplayBuf, d_LCD_ALIGNRIGHT);
	}
#else
	CTOS_LCDTPrintXY(1, 5, szBuf);
	CTOS_LCDTPrintAligned(5, szDisplayBuf, d_LCD_ALIGNRIGHT);
#endif

	//BDO UAT 0003: Changed batch totals from refund to void - start -- jzg
	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
	memset(szBuf, 0x00, sizeof(szBuf));
	sprintf(szBuf, "%012.0f", ulVoidTotalAmount);
	//vdCTOS_FormatAmount(strCST.szAmountFormat, szBuf,szAmtBuf);// patrick add code 20141216 	
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216
	sprintf(szDisplayBuf,"%s%s",strCST.szCurSymbol,szAmtBuf);

	memset(szBuf,0x00,sizeof(szBuf));
	sprintf(szBuf,"VOIDS:        %d",usVoidCount);
	
#if 0	 
	if((strTCT.byTerminalType % 2) == 0){	
    	CTOS_LCDTPrintXY(1, 7, "VOIDS:");
    	CTOS_LCDTPrintAligned(7, szDisplayBuf, d_LCD_ALIGNRIGHT);
	}else{
    	CTOS_LCDTPrintXY(1, 5, "VOIDS:");
		CTOS_LCDTPrintAligned(5, szDisplayBuf, d_LCD_ALIGNRIGHT);
	}
#else
		CTOS_LCDTPrintXY(1, 7, szBuf);
		CTOS_LCDTPrintAligned(7, szDisplayBuf, d_LCD_ALIGNRIGHT);
#endif

		//BDO UAT 0003: Changed batch totals from refund to void - end -- jzg


	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
	memset(szBuf, 0x00, sizeof(szBuf));

	//BDO UAT 0003: Changed batch totals from refund to void - start -- jzg
	sprintf(szBuf, "%012.0f", ulSaleTotalAmount);
	//vdCTOS_FormatAmount(strCST.szAmountFormat, szBuf,szAmtBuf);// patrick add code 20141216 	
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216		
	sprintf(szDisplayBuf,"%s%s",strCST.szCurSymbol,szAmtBuf);

	memset(szBuf,0x00,sizeof(szBuf));
	sprintf(szBuf,"TOTAL:        %d",usSaleCount);
#if 0
	if((strTCT.byTerminalType % 2) == 0){	
		CTOS_LCDTPrintXY(1, 9, "TOTAL:");
    	CTOS_LCDTPrintAligned(9, szDisplayBuf, d_LCD_ALIGNRIGHT);
	}else{
    	CTOS_LCDTPrintXY(1, 8, "TOTAL:");
		CTOS_LCDTPrintAligned(8, szDisplayBuf, d_LCD_ALIGNRIGHT);
	}
#else
	CTOS_LCDTPrintXY(1, 9, szBuf);
    CTOS_LCDTPrintAligned(9, szDisplayBuf, d_LCD_ALIGNRIGHT);
#endif
	CTOS_LCDTSelectFontSize(d_FONT_16x30);
	WaitKey(30);

    return d_OK;
}

int inCTOS_DisplayBatchRecordDetail(int inType)
{
    BYTE szTotalAmount[12+1];
    BYTE szAmtOut[12+1];
    BYTE szINV[6+1];
    BYTE szDisplayBuf[40+1];
    BYTE szTitle[16+1];
	int inPanlen;
   
    CTOS_LCDTClearDisplay();  

    vdDispTransTitle(inType);

    inIITRead(srTransRec.IITid);
    
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //sprintf(szDisplayBuf, "%s   %s", szTitle, strIIT.szIssuerLabel);
    sprintf(szDisplayBuf, "%s", szTitle);//Removed issuer on Batch Review. Issue 667
    CTOS_LCDTPrintXY(1, 3, szDisplayBuf);


	memset(szDisplayBuf,0,sizeof(szDisplayBuf));
	strcpy(szDisplayBuf, srTransRec.szPAN); 
	//if (strlen(szDisplayBuf) > 16)	
	//	cardMasking(szDisplayBuf, 4);
	//else
	//	cardMasking(szDisplayBuf, 3);

	cardMasking(szDisplayBuf, 5);

	CTOS_LCDTPrintXY(1, 4, szDisplayBuf);

/*
	inPanlen = strlen(srTransRec.szPAN);
    
    switch (inPanlen)
    {
       case 19:
                memset(szDisplayBuf,0,sizeof(szDisplayBuf));		
                vdCTOS_FormatPAN("***************NNNN", srTransRec.szPAN, szDisplayBuf);
                CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
                break;
       case 20:
                memset(szDisplayBuf,0,sizeof(szDisplayBuf));		
                vdCTOS_FormatPAN("****************NNNN", srTransRec.szPAN, szDisplayBuf);
                CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
                break;
       case21:
                memset(szDisplayBuf,0,sizeof(szDisplayBuf));		
                vdCTOS_FormatPAN("*****************NNNN", srTransRec.szPAN, szDisplayBuf);
                CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
                break;
   
       default:
           		memset(szDisplayBuf,0,sizeof(szDisplayBuf));		
               vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szDisplayBuf);
               CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
               break;
		}
*/
    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    vdDebug_LogPrintf("szTotalAmount[%s]", szTotalAmount);
    vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount,szAmtOut);
    vdDebug_LogPrintf("szTotalAmount[%s]", szDisplayBuf);
    memset(szDisplayBuf,0,sizeof(szDisplayBuf));
    sprintf(szDisplayBuf, "%s   %s", strCST.szCurSymbol, szAmtOut);
    //CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
	vdCTOSS_DisplayAmount(1,5,strCST.szCurSymbol,szAmtOut);

    memset(szINV, 0x00, sizeof(szINV));
    wub_hex_2_str(srTransRec.szInvoiceNo, szINV, 3);
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    sprintf(szDisplayBuf, "Invoice:  %ld", atol(szINV));
    CTOS_LCDTPrintXY(1, 6, szDisplayBuf);

	if ((strTCT.byTerminalType%2) == 0)
	{
		CTOS_LCDTPrintAligned(8, "00->UP", d_LCD_ALIGNLEFT);
		CTOS_LCDTPrintAligned(8, "DOT->DOWN", d_LCD_ALIGNRIGHT);
	}
	else
    vdCTOS_LCDGShowUpDown(1,1);
    
    return d_OK;
}

int inCTOSS_DeleteAdviceByINV(BYTE *szInvoiceNo)
{
    int inResult,inUpDateAdviceIndex;
    TRANS_DATA_TABLE srAdvTransTable;
    STRUCT_ADVICE strAdvice;
    
    memset((char *)&srAdvTransTable, 0, sizeof(TRANS_DATA_TABLE));
    memset((char *)&strAdvice, 0, sizeof(strAdvice));
    
    memcpy((char *)&srAdvTransTable, (char *)&srTransRec, sizeof(TRANS_DATA_TABLE));
    
    inResult = ST_SUCCESS;
    inUpDateAdviceIndex = 0;
    while(1)
    {
        inResult = inMyFile_AdviceReadByIndex(inUpDateAdviceIndex,&strAdvice,&srAdvTransTable);
        
        vdDebug_LogPrintf("ADVICE inUpDateAdviceIndex[%d] inMyFile_AdviceRead Rult(%d)(%d)(%d)(%d)", inUpDateAdviceIndex, inResult, srAdvTransTable.ulTraceNum, srAdvTransTable.byPackType, strAdvice.byTransType);
        
        if(inResult == ST_ERROR || inResult == RC_FILE_READ_OUT_NO_DATA)
        {
            inResult = ST_SUCCESS;
            break;
        }
        
        vdDebug_LogPrintf("ulnInvoiceNo[%02X %02X %02X] strAdvice->szInvoiceNo [%02X %02X %02X]", szInvoiceNo[0], szInvoiceNo[1], szInvoiceNo[2], strAdvice.szInvoiceNo[0], strAdvice.szInvoiceNo[1], strAdvice.szInvoiceNo[2]);
        if(0 != memcmp(szInvoiceNo, strAdvice.szInvoiceNo, INVOICE_BCD_SIZE))
        {
            inUpDateAdviceIndex ++;
            continue;
        }
        else
        {
            srAdvTransTable.byUploaded = CN_TRUE;
            inResult = inMyFile_AdviceUpdate(inUpDateAdviceIndex);
            break;
        }
        
    }

    return ST_SUCCESS;
}

int inCTOSS_BatchCheckDuplicateInvoice(void)
{
    int inRet = d_NO;
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_CHECK_DUP_INV);

        vdSetErrorMessage("");
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
        
        vdSetErrorMessage("");
        if(d_OK != inRet)
            return inRet;
    }

    return inRet;
}

/***********************************************************/
//sidumili: added function
//check for transaction amount entry base on szMaxTrxnAmt limit
/***********************************************************/
int inCTOS_ValidateTrxnAmount(void)
{
	double dbAmt1 = 0.00, dbAmt2 = 0.00;
	BYTE szBaseAmt[30+1] = {0};
	char *strPTR;

	//inTCTRead(1);
	memset(szBaseAmt, 0x00, sizeof(szBaseAmt));
	wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmt, AMT_BCD_SIZE);
	dbAmt1 = strtod(szBaseAmt, &strPTR);
	dbAmt2 = strtod(strTCT.szMaxTrxnAmt, &strPTR);

	vdDebug_LogPrintf("JEFF::STR TXN AMT: [%s]", szBaseAmt);
	vdDebug_LogPrintf("JEFF::STR MAX AMT: [%s]", strTCT.szMaxTrxnAmt);
	vdDebug_LogPrintf("JEFF::TXN AMT: [%.0f]", dbAmt1);
	vdDebug_LogPrintf("JEFF::MAX AMT: [%.0f]", dbAmt2);

	if((dbAmt1 >= dbAmt2) || (dbAmt1 <= 0))
	{
		vdSetErrorMessage("OUT OF RANGE");
		return(d_NO);
	}

	return(d_OK);
}


