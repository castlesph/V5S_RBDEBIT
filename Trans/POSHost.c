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
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "../Print/Print.h"



#define d_NOT_RECORD            102

#define d_GETPIN_TIMEOUT        6000
#define d_DUPLICATE_INVOICE     0x0080

extern int inSendECRResponseFlag;

int inCTOS_GetTxnBaseAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;
    BYTE szTitle[16+1];
    BYTE szDisplay[128];

    BYTE szDisMsg[128];

	//gcitra-0728
	BYTE bFirstKey;
	//gcitra-0728

	unsigned long amt1=0, amt2=0, amt3=0;
	BOOL isCashOutFlag = (BOOL)get_env_int("CASHOUTFLAG");

    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
	vdDebug_LogPrintf("saturn --inCTOS_GetTxnBaseAmount--");
	
	if (1 == inCTOSS_GetWaveTransType())
	if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
		return d_OK;

	vdDebug_LogPrintf("saturn inCTOSS_GetWaveTransType[%d]", inCTOSS_GetWaveTransType());
	vdDebug_LogPrintf("saturn srTransRec.byEntryMode[%d]", srTransRec.byEntryMode);
	vdDebug_LogPrintf("szCashoutMinAmt[%s]", strTCT.szCashoutMinAmt);
	vdDebug_LogPrintf("szCashoutMaxAmt[%s]", strTCT.szCashoutMaxAmt);
		
    	//CTOS_LCDTClearDisplay();
		//vdDispTransTitle(srTransRec.byTransType);
	
	SWIPE_AGAIN:
        //CTOS_KBDHit(&key);//clear key buffer
        //memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
        //sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
        //CTOS_LCDTPrintXY(1, 7, "AMOUNT:");

		
		memset(szDisplay, 0x00, sizeof(szDisplay));
		strcpy(szDisplay, szTitle);
		strcat(szDisplay, "|");
		strcat(szDisplay, "Please Enter Amount");
		strcat(szDisplay, "|");
		strcat(szDisplay, strCST.szCurSymbol);
    
        memset(baAmount, 0x00, sizeof(baAmount));
        //key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    
		vdDebug_LogPrintf("saturn nputAmountEx szDisplay[%s]", szDisplay);
		
		key = InputAmountEx(1, 8, szDisplay, 2, bFirstKey, baAmount, &ulAmount, 30, 0);
		
		vdDebug_LogPrintf("saturn InputAmountEx %d", key);
        if(d_OK == key)
        {
            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%012ld", atol(baAmount));
            wub_str_2_hex(szTemp,srTransRec.szBaseAmount,12);

			// Check for minimum amount -- sidumili
			amt1=0;
			amt2=0;
			amt1 = wub_str_2_long(baAmount);
	        amt2 = wub_str_2_long(strTCT.szCashoutMinAmt);
			
			vdDebug_LogPrintf("MIN:amt1[%ld]", amt1);
			vdDebug_LogPrintf("MIN:amt2[%ld]", amt2);

			if ((amt1 < amt2 && amt2 > 0) && (isCashOutFlag==TRUE))
	        {                                
				memset(baAmount, 0x00, sizeof(baAmount));		            
				vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strTCT.szCashoutMinAmt, baAmount);		            
				sprintf(szTemp,"MIN AMOUNT %s", baAmount);

				vdDisplayMessageBox(1, 8, "", szTemp, "", MSG_TYPE_ERROR);
				CTOS_Beep();
				CTOS_Delay(1000);
				CTOS_Beep();
				 usCTOSS_LCDDisplay(" ");
                goto SWIPE_AGAIN;
	        }          

			// Check for maximum amount -- sidumili
			amt1=0;
			amt2=0;
			amt1 = wub_str_2_long(baAmount);
	        amt2 = wub_str_2_long(strTCT.szCashoutMaxAmt);
			
			vdDebug_LogPrintf("MAX:amt1[%ld]", amt1);
			vdDebug_LogPrintf("MAX:amt2[%ld]", amt2);

			if ((amt1 > amt2 && amt2 > 0) && (isCashOutFlag==TRUE))
	        {                                
	            memset(baAmount, 0x00, sizeof(baAmount));		            
	            vdCTOS_FormatAmount("NNN,NNN,NNN,NNn.nn", strTCT.szCashoutMaxAmt, baAmount);		            
	            sprintf(szTemp, "%s", "MAX AMOUNT EXCEED");
	            
	            vdDisplayMessageBox(1, 8, "", szTemp, "", MSG_TYPE_ERROR);
				CTOS_Beep();
				CTOS_Delay(1000);
				CTOS_Beep();
				usCTOSS_LCDDisplay(" ");
                goto SWIPE_AGAIN;
	        }

			amt1=0;
			amt2=0;
			amt3=10000;
			amt1 = wub_str_2_long(baAmount);
	        amt2 = wub_str_2_long(strTCT.szCashoutMinAmt);
			if ((amt1 % amt3 > 0) && (isCashOutFlag==TRUE) && (strTCT.fCashoutAmt100 == TRUE))
	        {                                
				vdDisplayMessageBox(1, 8, "", "AMOUNT SHOULD BE", "DIVISIBLE BY 100", MSG_TYPE_ERROR);
				CTOS_Beep();
				CTOS_Delay(1000);
				CTOS_Beep();
				 usCTOSS_LCDDisplay(" ");
                goto SWIPE_AGAIN;
	        }
    
        }
		else if(d_USER_CANCEL == key)
		{
            //dSetErrorMessage("USER CANCEL");		
			memset(szTitle, 0x00, sizeof(szTitle));
			szGetTransTitle(srTransRec.byTransType, szTitle);
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "AMOUNT ENTRY CANCELLED");
			//usCTOSS_LCDDisplay(szDisMsg);
			vdDisplayMessageBox(1, 8, "", "AMOUNT ENTRY CANCELLED", "", MSG_TYPE_WARNING);
			CTOS_Beep();
			CTOS_Delay(1000);
			CTOS_Beep();
            return d_NO;
		}
        else if(0xFF == key)
        {
            //vdSetErrorMessage("TIME OUT");     
			memset(szTitle, 0x00, sizeof(szTitle));
			szGetTransTitle(srTransRec.byTransType, szTitle);
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "TIME OUT");
			//usCTOSS_LCDDisplay(szDisMsg);
			vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			CTOS_Delay(1500);
			CTOS_Beep();
			return d_NO;
        }
	
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

    DebugAddSTR("inCTOS_GetTxnTipAmount","Processing...",20);

    if (d_OK != inCTOS_CheckTipAllowd())
        return d_OK;

	if (1 == inCTOSS_GetWaveTransType())
	if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    
    memset(baBaseAmount, 0x00, sizeof(baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
        
    do
    {
        CTOS_LCDTClearDisplay();
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
        else
        {
        	if (1 != inCTOSS_GetWaveTransType())
        	{
				inCTOS_DisplayCardTitle(5, 6, TRUE);
        	}
        }
        
        CTOS_KBDHit(&key);//clear key buffer
        
        CTOS_LCDTPrintXY(1, 7, "TIP:");

        memset(baAmount, 0x00, sizeof(baAmount));
        key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

        if(d_OK == key)
        {
			ULONG ulTipPercent=0;
			
            if(0 == strlen(baAmount)) //by pass TIP
                return d_OK;

            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%012ld", atol(baAmount));

                        
			ulTipPercent=(atol(baBaseAmount) * strHDT.ulTipPercent)/100;
			//vdDebug_LogPrintf("atol(szTemp): %ld", atol(szTemp));
			//vdDebug_LogPrintf("strHDT.ulTipPercent: %ld", strHDT.ulTipPercent);
			//vdDebug_LogPrintf("ulTipPercent: %ld", ulTipPercent);
			
			if(atol(szTemp) > ulTipPercent)
			{
                clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(8);
                continue;				
			}
			
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

    inCTOS_DisplayCardTitle(3, 4, FALSE);
    
    
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
        key = InputAmount2(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);

        if(strlen(baAmount) > 0)
        {	
            ULONG ulTipPercent=0;			
			
            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%012ld", atol(baAmount));

			ulTipPercent=(atol(baBaseAmount) * strHDT.ulTipPercent)/100;
			//vdDebug_LogPrintf("atol(szTemp): %ld", atol(szTemp));
			//vdDebug_LogPrintf("strHDT.ulTipPercent: %ld", strHDT.ulTipPercent);
			//vdDebug_LogPrintf("ulTipPercent: %ld", ulTipPercent);
			if(atol(szTemp) > ulTipPercent)
			{
                clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(8);
                continue;				
			}
			
            if(strcmp(szTemp, baBaseAmount) > 0)
            {
                vduiClearBelow(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                vduiClearBelow(8);
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
	BYTE szServiceFee[20]; // For Bancnet CashOut -- sidumili

    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szTipAmount, 0x00, sizeof(szTipAmount));
    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
	memset(szServiceFee, 0x00, sizeof(szServiceFee));

    wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);
	//wub_hex_2_str(srTransRec.szServiceFee, szServiceFee, 6);

    if (srTransRec.byTransType == CASH_OUT)
	    sprintf(szTotalAmount, "%012ld", atol(szBaseAmount) + atol(szTipAmount) + atol(szServiceFee));
	else
		sprintf(szTotalAmount, "%012ld", atol(szBaseAmount) + atol(szTipAmount));
	
    wub_str_2_hex(szTotalAmount,srTransRec.szTotalAmount,12);
    
    DebugAddSTR(szTotalAmount,szTipAmount,12); 
    DebugAddSTR("Total amount","Tip amount",12); 
    
    DebugAddHEX("Tip amount",srTransRec.szTipAmount,6); 
    DebugAddHEX("Total amount",srTransRec.szTotalAmount,6); 
	//DebugAddHEX("Service Fee",srTransRec.szServiceFee,6);

    if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
    {
		inCTLOS_Updatepowrfail(PFR_EMV_PROCESSING);		
        memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
        memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
        wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
        lnTmp = atol(byDataTmp1);
        wub_long_2_array(lnTmp, byDataTmp2);

        memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
        bPackSendBuf[usPackSendLen++] = 0x04;
        memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
        usPackSendLen += 4;
        
        if (srTransRec.byTransType == CASH_OUT)
			memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szBaseAmount, 6);
		else	
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
        //bRet = InputString(1, 8, 0x00, 0x02, szAuthCode, &szAuthCodeLen, 0, d_INPUT_TIMEOUT);
        if (bRet == d_KBD_CANCEL )
        {
            CTOS_LCDTClearDisplay();    
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }
        else if (bRet == d_KBD_ENTER ) 
        {
            if(strlen(szAuthCode) >= 6)
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
		else
            return ST_ERROR;			
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
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRACE NO.: ");

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

VS_BOOL fAmountLessThanFloorLimit(void) {
    long lnTotalAmount = 0;
    BYTE szTotalAmt[12 + 1];

    /* If the transaction amount is less than the floor limit,
        Set the transaction type to OFFLINE */

    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
    lnTotalAmount = atol(szTotalAmt);

    if (lnTotalAmount < strCDT.InFloorLimitAmount)
        return (d_OK);

    return (d_NO);
}


int inCTOS_CustComputeAndDispTotal(void)
{
    CTOS_RTC SetRTC;
    BYTE    szTotalAmt[12+1];
    BYTE    szTempBuf[12+1];
    BYTE    szTempBuf1[12+1]; 
    BYTE    szDisplayBuf[30];
    BYTE    szStr[45];
	BYTE    szStr1[45];

	
	BYTE szDisMsg[512];
    BYTE szPan[30+1];
	char szLineBuf[40];
	char szCardLabel[50 + 1]; 
	int ret;
    BYTE szTemp1[30+1];
	BYTE szTitle[16+1];
    BYTE szExpYear[3];
    BYTE szExpMonth[3];
    BYTE szTemp[5];
    int inUTF8 = 1;
	BYTE szAmtMsg[40 + 1] = {0};
    BYTE szAcquirerFee[12+1];
	BYTE szServiceFee[12+1];
	BYTE szBillerFee[12+1];
	
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

	//inCTOS_DisplayCardTitle(5, 6, TRUE);
	
    //szGetTransTitle(srTransRec.byTransType, szTitle);
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, "TOTAL AMOUNT :");
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
    memset(szStr, 0x00, sizeof(szStr));
	memset(szStr1, 0x00, sizeof(szStr1));
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szStr);
	//setLCDPrint(7, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
	sprintf(szStr1, "%s %s", strCST.szCurSymbol,szStr);

	memset(szAmtMsg, 0x00, sizeof(szAmtMsg));
//	sprintf(szAmtMsg, "TOTAL AMOUNT\n%s", szStr1);
	sprintf(szAmtMsg, "%s", szStr1);
	//setLCDPrint(7, DISPLAY_POSITION_LEFT, szStr1);
	//setLCDPrint(7, DISPLAY_POSITION_RIGHT, szStr);

	/*
    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        return (d_OK);
    }

    if(srTransRec.byTransType != SALE)
    {
        return (d_OK);
    }
    */
    
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

	// Confirmation
	//if (inConfirmPAN() != d_OK)
	//	return d_NO;

    //TINE:  display ExpiryDate
    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szExpYear, 0x00, sizeof(szExpYear));
    memset(szExpMonth, 0x00, sizeof(szExpMonth));
    wub_hex_2_str(srTransRec.szExpireDate, szTemp, 2);
    memcpy(szExpYear,&szTemp[0],2);
    memcpy(szExpMonth,&szTemp[2],2);

	memset(szTemp1,0,sizeof(szTemp1));
	vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1, strIIT.fMaskPan);

#if 0
	vdDebug_LogPrintf("usCTOSS_ConfirmMenu");
	if (srTransRec.byTransType == BAL_INQ){	
		strcpy(szDisMsg, strIIT.szIssuerLabel);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, szTemp1);
	}else{
		strcpy(szDisMsg, strIIT.szIssuerLabel);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, szTemp1);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "TOTAL");	
		strcat(szDisMsg, "|");
		strcat(szDisMsg, szStr1);
	}
	
	vdDebug_LogPrintf("usCTOSS_ConfirmMenu szDisMsg[%s]", szDisMsg);

	ret = usCTOSS_Confirm(szDisMsg);
#else
    //TINE - replace usCTOSS_Confirm with ConfirmCardDetails
	memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);

	if (srTransRec.szCardholderName[0] != 0x20){
		TrimTrail(srTransRec.szCardholderName); // Trim Name -- sidumili
    	inUTF8 = is_utf8(srTransRec.szCardholderName);
    	vdDebug_LogPrintf("AAA - srTransRec.szCardholderName[%s]", srTransRec.szCardholderName);
    	if (inUTF8 == FALSE) //Cardholder name has special characters
    	{
        	memset(srTransRec.szCardholderName,0, sizeof(srTransRec.szCardholderName));
        	strcpy(srTransRec.szCardholderName, " ");
    	}
	}else{
	    memset(srTransRec.szCardholderName,0, sizeof(srTransRec.szCardholderName));
		strcpy(srTransRec.szCardholderName, " ");
	}
	
    vdDebug_LogPrintf("usCTOSS_ConfirmMenu");
	memset(szDisMsg, 0x00, sizeof(szDisMsg));		
    strcpy(szDisMsg, szTitle);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, szTemp1);	//PAN
    setLCDPrint(2, DISPLAY_POSITION_LEFT, szTemp1);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, szExpMonth);
    strcat(szDisMsg, "/");
    strcat(szDisMsg, szExpYear);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, srTransRec.szCardholderName);
    strcat(szDisMsg, "|");
	if (srTransRec.byTransType == BAL_INQ){
		strcat(szDisMsg, " ");
//                setLCDPrint(3, DISPLAY_POSITION_LEFT, szAmtMsg);
	}
	else
	{
    	//strcat(szDisMsg, "Total Amount: ");
    	//strcat(szDisMsg, szStr1);
    	strcat(szDisMsg, szAmtMsg);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szAmtMsg);
	}
    strcat(szDisMsg, "|");

    strcat(szDisMsg, strIIT.szIssuerLogo);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "0"); // sidumili: Issuer Logo for PNG file(Bancnet), start with index 0

    if(srTransRec.byTransType == CASH_IN || srTransRec.byTransType == CASH_OUT || srTransRec.byTransType == BILLS_PAYMENT
    || srTransRec.byTransType == FUND_TRANSFER || srTransRec.byTransType == BAL_INQ)
    {
		memset(szAcquirerFee, 0, sizeof(szAcquirerFee));
		memset(szServiceFee, 0, sizeof(szServiceFee));
		memset(szBillerFee, 0, sizeof(szBillerFee));
		if(strTCT.fAtmMsg == 0) /*POS*/
		{
			
			if(srTransRec.byTransType == CASH_IN) 
			{
				if(srTransRec.fOnUs == VS_TRUE)
				    memcpy(szAcquirerFee, strTCT.szPOSCashInAcquirerFeeOnUs, 12);
			}
			else  if(srTransRec.byTransType == CASH_OUT) 
			{
				if(srTransRec.fOnUs == VS_TRUE)
					memcpy(szAcquirerFee, strTCT.szPOSCashOutAcquirerFeeOnUs, 12);
				else
					memcpy(szAcquirerFee, strTCT.szPOSCashOutAcquirerFeeOffUs, 12);
			}
			else if(srTransRec.byTransType == BAL_INQ)
			{
				memcpy(szAcquirerFee, strTCT.szBalInqAcquirerFee, 12);
			}
			else if(srTransRec.byTransType == BILLS_PAYMENT)
			{
				if(srTransRec.fOnUs == VS_TRUE)
				{
				    memcpy(szServiceFee, strTCT.szBillsPaymentServiceFee, 12);
					memcpy(szBillerFee, srTransRec.szBillerFee, 12);
				}
				else /*Off-Us*/
				{
					if(srTransRec.fBillsPaymentCash == VS_TRUE)
					{
						memcpy(szServiceFee, strTCT.szBillsPaymentServiceFee, 12);
						memcpy(szBillerFee, srTransRec.szBillerFee, 12);
					}
				}
			}
		}
		else /*ATM*/
		{
            if(srTransRec.byTransType == CASH_OUT) 
			{
				if(srTransRec.fOnUs == VS_TRUE)
					memcpy(szAcquirerFee, strTCT.szATMCashOutAcquirerFeeOnUs, 12);
				else
					memcpy(szAcquirerFee, strTCT.szATMCashOutAcquirerFeeOffUs, 12);
			}		
			else if(srTransRec.byTransType == BAL_INQ)
			{
				memcpy(szAcquirerFee, strTCT.szBalInqAcquirerFee, 12);
			}
			else if(srTransRec.byTransType == BILLS_PAYMENT)
			{
				if(srTransRec.fOnUs == VS_TRUE)
				{
				    memcpy(szServiceFee, strTCT.szBillsPaymentServiceFee, 12);
					memcpy(szBillerFee, srTransRec.szBillerFee, 12);
				}
				else /*Off-Us*/
				{
					if(srTransRec.fBillsPaymentCash == VS_TRUE)
					{
						memcpy(szServiceFee, strTCT.szBillsPaymentServiceFee, 12);
						memcpy(szBillerFee, srTransRec.szBillerFee, 12);
					}
				}
			}
		}

        strcat(szDisMsg, "|");
		memset(szStr, 0, sizeof(szStr));
		memset(szTotalAmt, 0, sizeof(szTotalAmt));
		vdDebug_LogPrintf("szAcquirerFee:%s", szAcquirerFee);
		vdDebug_LogPrintf("szServiceFee:%s", szServiceFee);
		vdDebug_LogPrintf("szBillerFee:%s", szBillerFee);
		if(atol(szAcquirerFee) > 0)
		{
			memcpy(srTransRec.szAcquirerFee, szAcquirerFee, 12);
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szAcquirerFee, szTotalAmt);
            sprintf(szStr, "ACQUIRER FEE\n%s %s", strCST.szCurSymbol,szTotalAmt);
		}
        else if(atol(szServiceFee) > 0)
        {
			memcpy(srTransRec.szServiceFee, szServiceFee, 12);
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szServiceFee, szTotalAmt);
            sprintf(szStr, "SERVICE FEE\n%s %s", strCST.szCurSymbol,szTotalAmt);			
        }
		else
		{
			strcpy(szStr, "NO FEE");
		}
		strcat(szDisMsg, szStr);	

        if(atol(szBillerFee) > 0) /*got biller fee*/
        {
			memcpy(srTransRec.szBillerFee, szBillerFee, 12);
            strcat(szDisMsg, "|");
            memset(szStr, 0, sizeof(szStr));
			memset(szTotalAmt, 0, sizeof(szTotalAmt));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBillerFee, szTotalAmt);
            sprintf(szStr, "BILLER FEE\n%s %s", strCST.szCurSymbol,szTotalAmt);			
			
        }
		else
		{
			strcat(szDisMsg, "|");
			memset(szStr, 0, sizeof(szStr));
			strcpy(szStr, "NO FEE");
		}
		strcat(szDisMsg, szStr);

		strcat(szDisMsg, "|");
		memset(szStr, 0, sizeof(szStr));
        if(srTransRec.byTransType == BILLS_PAYMENT && srTransRec.fBillsPaymentCash == VS_TRUE)	
			strcat(szStr, "1");
		else
			strcat(szStr, "0");
		strcat(szDisMsg, szStr);
		
    }

    /*subscriber name*/
    strcat(szDisMsg, "|");
	if(srTransRec.byTransType == BILLS_PAYMENT && srTransRec.fBillsPaymentCash == VS_TRUE)	
	    strcat(szDisMsg, srTransRec.szBillerName);

    /*subscriber name*/
    strcat(szDisMsg, "|");
	if(srTransRec.byTransType == BILLS_PAYMENT && srTransRec.fBillsPaymentCash == VS_TRUE)	
	    strcat(szDisMsg, srTransRec.szSubscriberNo);
    {
        
    }
	ret = usCTOSS_Confirm("PLS ENTER TO PROCEED");
//    ret = ConfirmCardDetails(szDisMsg);

    vdDebug_LogPrintf("ConfirmCardDetails szDisMsg[%s]", szDisMsg);
#endif

	vdDebug_LogPrintf("saturn confirm PAN %d", ret);

	if (ret != d_OK)
        {
            CTOS_Beep();
		CTOS_Delay(2000);
            if(fGetECRTransactionFlg())
            {
                 vdSetECRResponse(ECR_UNKNOWN_ERR);
                             //inMultiAP_ECRSendSuccessResponse();
                 return d_NO;
            }
        }
		
    return(d_OK);
}


int inCTOS_DisplayResponse(void)
{
    int inMsgid = atoi(srTransRec.szRespCode);
    int inHostIndex = srTransRec.HDTid;
    int inResult = 0;
    char szResponseCode[40], szResponseCode2[40];
	
    memset(szResponseCode, 0x00, sizeof(szResponseCode2));	
    memset(szResponseCode2, 0x00, sizeof(szResponseCode));
    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
    inMSGResponseCodeReadByHostName(szResponseCode, szResponseCode2, inMsgid, strHDT.szAPName);  
    if(0 == strlen(szResponseCode) )
    {
        inMSGResponseCodeRead(szResponseCode, szResponseCode2, inMsgid, 12);
        if(0 != strlen(szResponseCode) )
        {
        	vdDisplayMessageBox(1, 8, szResponseCode, szResponseCode2, "", MSG_TYPE_ERROR);			
			CTOS_Beep();
		    CTOS_Delay(1500);
		    CTOS_Beep();
			//vdDisplayErrorMsg2(1, 8, szResponseCode, szResponseCode2);
        }
    }
    else
    {
       		vdDisplayMessageBox(1, 8, szResponseCode, szResponseCode2, "", MSG_TYPE_ERROR);
			CTOS_Beep();
			CTOS_Delay(1500);
			CTOS_Beep();
        //vdDisplayErrorMsg2(1, 8, szResponseCode, szResponseCode2);
    }

    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
    return(d_OK);  
}



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

    //if (inMultiAP_CheckSubAPStatus() == d_OK)
    //{
        //return d_OK;
    //}

    memset(szInvNoAsc, 0x00, sizeof(szInvNoAsc));
    memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));
    
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRACE NO.: ");
    
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
    char szInvoiceNo[6+1];

    memset(szInvoiceNo, 0x00, sizeof(szInvoiceNo));
	wub_hex_2_str(srTransRec.szInvoiceNo, szInvoiceNo, 3);
#if 0
	inResult = inDatabase_BancnetBatchSearch(&srTransRec, szInvoiceNo);
    if(inResult != d_OK)
    {
        inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);
    }
#else
vdDebug_LogPrintf("AAA - srTransRec.szTranYear[%s]", srTransRec.szTranYear);
inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);
if(inResult != d_OK)
{
     if (inMultiAP_CheckSubAPStatus() != d_OK)
          vdSetErrorMessage("NO RECORD FOUND");
     return d_NOT_RECORD;
}
#endif
    DebugAddSTR("inCTOS_BatchSearch","Processing...",20);
    
    DebugAddINT("inCTOS_BatchSearch", inResult);

    if(inResult != d_OK)
    {   
        if (inMultiAP_CheckSubAPStatus() != d_OK)
            vdSetErrorMessage("NO RECORD FOUND");
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
        vdDebug_LogPrintf("SATURN inCTOS_LoadCDTandIIT LOAD CDT ERROR [%d]", srTransRec.CDTid);
		vdSetErrorMessage("LOAD CDT ERROR");
		return(d_NO);
    }
    inGetIssuerRecord(strCDT.IITid);

    return ST_SUCCESS;
}


int inCTOS_CheckTipadjust()
{
    inCDTRead(srTransRec.CDTid);
    if(srTransRec.byTransType == PRE_AUTH || strCDT.inType == DEBIT_CARD
    || srTransRec.fInstallment == TRUE || srTransRec.fDebit == TRUE  || srTransRec.fLoyalty == TRUE)
    {
        vdSetErrorMessage("TRANS NOT ALLWD");
        return d_NO;
    }
	
    if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_TIP) || (srTransRec.byTransType == SALE_OFFLINE) || (srTransRec.byTransType == CASH_OUT))
    {
        if((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_OFFLINE) || (srTransRec.byTransType == CASH_OUT))
            srTransRec.byOrgTransType = srTransRec.byTransType;
        
        memcpy(srTransRec.szOrgAmount, srTransRec.szTotalAmount, 6);
        memcpy(srTransRec.szOrgTipAmount, srTransRec.szTipAmount, 6);
    }
    else
    {
        vdSetErrorMessage("TIP NOT ALLWD");
        return d_NO;
    }

    srTransRec.byTransType = SALE_TIP;

    return d_OK;
}

int inCTOS_CheckVOID()
{
    inSendECRResponseFlag = get_env_int("ECRRESP");
    if(srTransRec.byVoided == TRUE)
    {
        vdSetErrorMessage("RECORD VOIDED");
        return d_NO;
    }

	inCDTRead(srTransRec.CDTid);
    if(srTransRec.byTransType == PRE_AUTH || strCDT.inType == DEBIT_CARD)
    {
        vdSetErrorMessage("TRANS NOT ALLWD");
		if(inSendECRResponseFlag)
        {
             vdSetECRResponse(ECR_UNKNOWN_ERR);
			 inMultiAP_ECRSendSuccessResponse();
        }	
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
        vdSetErrorMessage("HOST SELECTION ERR 4");
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

	vdDebug_LogPrintf("saturn check batch empty");
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
        //vdSetErrorMessage("BATCH EMPTY");
        vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
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
    
    CTOS_LCDTClearDisplay();
    vduiLightOn();
    
    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(3, 4, FALSE);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtTmp1, 0x00, sizeof(szAmtTmp1));
    memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    wub_hex_2_str(srTransRec.szTotalAmount, szAmtTmp1, 6);
    //sprintf(szAmtTmp2, "%lu.%02lu", atol(szAmtTmp1)/100, atol(szAmtTmp1)%100);
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szAmtTmp1, szAmtTmp2);
    
    CTOS_LCDTPrintXY(1, 5, "AMOUNT");
    setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szAmtTmp2)+1)*2,  6, szAmtTmp2);
    
    if(srTransRec.byTransType == SALE_TIP)
        vduiDisplayStringCenter(7,"CONFIRM TIPADJ");
    if(srTransRec.byTransType == VOID)
        vduiDisplayStringCenter(7,"CONFIRM VOID");
    
    vduiDisplayStringCenter(8,"NO[X] YES[OK]");
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

    /*albert - start - fix stan issue*/
    //memset(szBcd, 0x00, sizeof(szBcd));
    //memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
    //inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    //srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
    //inHDTSave(strHDT.inHostIndex);
	/*albert - end - fix stan issue*/

    return d_OK;
}

int inCTOS_SettlementClearBathAndAccum(void)
{
    int         inResult;
    BYTE    szBcd[INVOICE_BCD_SIZE+1];
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    vdDebug_LogPrintf("inCTOS_SettlementClearBathAndAccum");
    
    memset(szBcd, 0x00, sizeof(szBcd));
    memcpy(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);    
    inBcdAddOne(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);
    strMMT[0].fMustSettFlag = CN_FALSE;
    inMMTSave(strMMT[0].MMTid);
      
	strMMT[0].fBatchNotEmpty = 0;	
	inMMTBatchNotEmptySave(strMMT[0].MMTid);       

    inDatabase_BatchDelete();

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    inCTOS_ReadAccumTotal(&srAccumRec);
    strcpy(srAccumRec.szTID, srTransRec.szTID);
    strcpy(srAccumRec.szMID, srTransRec.szMID);
    memcpy(srAccumRec.szYear, srTransRec.szYear, DATE_BCD_SIZE);
    memcpy(srAccumRec.szDate, srTransRec.szDate, DATE_BCD_SIZE);
    memcpy(srAccumRec.szTime, srTransRec.szTime, TIME_BCD_SIZE);
    memcpy(srAccumRec.szBatchNo, srTransRec.szBatchNo, BATCH_NO_BCD_SIZE);
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

    return d_OK;
}

int inCTOS_DisplayBatchTotal(void)
{
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[40];
    BYTE szBuf[40];
    USHORT usSaleCount;
    ULONG ulSaleTotalAmount;
    USHORT usRefundCount;
    ULONG  ulRefundTotalAmount;
    ACCUM_REC srAccumRec;

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(BATCH_TOTAL);
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) != ST_SUCCESS)
    {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        CTOS_LCDTClearDisplay();
        vdSetErrorMessage("BATCH EMPTY");
        return ST_ERROR;
    }

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;

    usSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
    ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);

    usRefundCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
    ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);
    
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    strcpy(szDisplayBuf, "Type  Cnt  Amt");
    CTOS_LCDTPrintXY(1, 3, szDisplayBuf);


    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulSaleTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    sprintf(szDisplayBuf, "SALE  %03d  %s", usSaleCount, szAmtBuf);
    CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
    
    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ulRefundTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    sprintf(szDisplayBuf, "RFND  %03d  %s", usRefundCount, szAmtBuf);
    CTOS_LCDTPrintXY(1, 6, szDisplayBuf);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", (ulSaleTotalAmount > ulRefundTotalAmount) ? (ulSaleTotalAmount-ulRefundTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount));
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    memset(szBuf, 0x00, sizeof(szBuf));
    if(ulSaleTotalAmount > ulRefundTotalAmount)
        szBuf[0] = ' ';
    else
        szBuf[0] = '-';
    sprintf(szDisplayBuf, "TOTL  %03d %s%s", (usSaleCount + usRefundCount), szBuf, szAmtBuf);
    CTOS_LCDTPrintXY(1, 8, szDisplayBuf);

    WaitKey(30);

    return d_OK;
}

int inCTOS_DisplaySettleBatchTotal(BYTE byTransType, BOOL fConfirm)
{
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[40];
    BYTE szBuf[40];
    USHORT usSaleCount=0, usTotalCount=0, usCashOutCount=0, usCashInCount=0, usBillsPaymentCount=0, usFundTransferCount=0;
    ULONG ulSaleTotalAmount=0, ulTotalAmount=0, ulCashOutTotalAmount=0, ulCashInTotalAmount=0, ulBillsPaymentTotalAmount=0, ulFundTransferTotalAmount=0;
	
    USHORT usRefundCount = 0;
    ULONG  ulRefundTotalAmount = 0;
    ACCUM_REC srAccumRec;
    BYTE byKeyBuf; 
    USHORT usVoidCount = 0;
    ULONG ulVoidTotalAmount = 0;

	int key=d_NO;
	BYTE szDispUI[128];

	BYTE szDisplayBuf0[40];
    BYTE szDisplayBuf1[40];
	BYTE szDisplayBuf2[40];
	BYTE szDisplayBuf3[40];

	int inConfirmTimeOut = 30;
	
	BYTE szTitle[20+1];

	BYTE szCount[40];
	BYTE szTemp[40];

	//USHORT usCashOutCount = 0;
    //ULONG ulCashOutTotalAmount = 0;
	BOOL isCashOutFlag = (BOOL)get_env_int("CASHOUTFLAG"); // For BancNet CashOut -- sidumili

	vdDebug_LogPrintf("AAA - srTransRec.fAutoSettleFlag[%d]", srTransRec.fAutoSettleFlag);

	if (srTransRec.fAutoSettleFlag == TRUE)
	{
		fConfirm = FALSE;
		inConfirmTimeOut = 1;
	}

    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(byTransType);
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) != ST_SUCCESS)
    {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        CTOS_LCDTClearDisplay();
        vdSetErrorMessage("BATCH EMPTY");
        return ST_ERROR;    
    }

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 1;

    
    usCashOutCount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashOutCount;
	usCashInCount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashInCount;
	usBillsPaymentCount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBillsPaymentCount;
	usFundTransferCount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usFundTransferCount;
	
    usTotalCount = usCashInCount+usCashOutCount+usBillsPaymentCount+usFundTransferCount;

	ulCashOutTotalAmount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashOutTotalAmount;
	ulCashInTotalAmount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashInTotalAmount;
	ulBillsPaymentTotalAmount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBillsPaymentTotalAmount;
	ulBillsPaymentTotalAmount=srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBillsPaymentTotalAmount;
	
    ulTotalAmount = ulCashInTotalAmount+ulCashOutTotalAmount+ulBillsPaymentTotalAmount+ulFundTransferTotalAmount;

    usVoidCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount);
	ulVoidTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);

	// For BancNet CashOut -- sidumili
	//usCashOutCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashOutCount);
    //ulCashOutTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashOutTotalAmount);
	
    //memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //strcpy(szDisplayBuf, "Type  Cnt  Amt");
    //CTOS_LCDTPrintXY(1, 3, szDisplayBuf);

    memset(szDisplayBuf1, 0, sizeof(szDisplayBuf1));
	if (isCashOutFlag)
	{
	}
	else
	{
		// SALE
		if (ulSaleTotalAmount > 0)
		{
			memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
		    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
		    memset(szBuf, 0x00, sizeof(szBuf));
		    sprintf(szBuf, "%ld", ulSaleTotalAmount);
		    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
			  
			if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch == d_OK)
			{
				sprintf(szDisplayBuf, "%s   %s", strCST.szCurSymbol, szAmtBuf); 
				inDisplayLeftRight(2, "SALE",szDisplayBuf,20);
			}
			else
			{
				sprintf(szDisplayBuf, "   %s   %s", strCST.szCurSymbol, szAmtBuf); 
				//CTOS_LCDTPrintXY(1, 2, "SALE");
				//CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
				
				memset(szDisplayBuf1, 0x00, sizeof(szDisplayBuf1));
				sprintf(szDisplayBuf1,"%s %s",strCST.szCurSymbol,szAmtBuf);
			}
		}
	}
	
	// VOID
	if (ulVoidTotalAmount > 0)
	{
		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
	    memset(szBuf, 0x00, sizeof(szBuf));
	    sprintf(szBuf, "%ld", ulVoidTotalAmount);
	    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
		
		
		if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch == d_OK)
		{
			sprintf(szDisplayBuf, "%s-  %s", strCST.szCurSymbol, szAmtBuf);
			inDisplayLeftRight(3, "VOID",szDisplayBuf,20);
		}
		else
		{	  
			sprintf(szDisplayBuf, "   %s-  %s", strCST.szCurSymbol, szAmtBuf);
			CTOS_LCDTPrintXY(1, 4, "VOID");	
		    CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
		}
	}

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
    memset(szBuf, 0x00, sizeof(szBuf));
    sprintf(szBuf, "%ld", ((ulSaleTotalAmount + ulCashOutTotalAmount) > ulRefundTotalAmount) ? ((ulSaleTotalAmount+ulCashOutTotalAmount)-ulRefundTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount - ulCashOutTotalAmount));
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    memset(szBuf, 0x00, sizeof(szBuf));
    if((ulSaleTotalAmount + ulCashOutTotalAmount) > ulRefundTotalAmount)
        szBuf[0] = ' ';
    else
        szBuf[0] = '-';
	szBuf[1]=0x00;


	if (isCheckTerminalMP200() == d_OK || isCheckTerminalNonTouch == d_OK)
	{
		sprintf(szDisplayBuf, "%s%s  %s", strCST.szCurSymbol, szBuf, szAmtBuf);
		inDisplayLeftRight(7, "TOTALS",szDisplayBuf,20);
	}
	else
	{
		//sprintf(szDisplayBuf, "   %s%s  %s", strCST.szCurSymbol, szBuf, szAmtBuf);
		//CTOS_LCDTPrintXY(1, 6, "TOTALS");
		//CTOS_LCDTPrintXY(1, 7, szDisplayBuf);
		sprintf(szDisplayBuf3, "%s%s%s", strCST.szCurSymbol, szBuf, szAmtBuf);
	}

#if 0		

#else
      //TEST
      //inConfirmTimeOut = 1;

       vdDebug_LogPrintf("SATURN  inCTOS_DisplaySettleBatchTotal confirm");
      memset(szDispUI, 0, sizeof(szDispUI));

   memset(szTemp, 0x00, sizeof(szTemp));
   if (srTransRec.byTransType == SETTLE)
   {	      				      	
      sprintf(szTemp, "SETTLEMENT|%s|",strMMT[0].szMerchantName);
      strcpy(szDispUI, szTemp);
   }
   else
   {	
      sprintf(szTemp, "BATCH TOTAL|%s|",strMMT[0].szMerchantName);
      strcpy(szDispUI, szTemp);
   }

      /*cash out*/
	  strcat(szDispUI, "CASH OUT|"); //trans type
	  memset(szTemp, 0x00, sizeof(szTemp));
	  sprintf(szTemp, "%03d|", usCashOutCount);
	  strcat(szDispUI, szTemp); //trans count

      memset(szBuf, 0x00, sizeof(szBuf));
      sprintf(szBuf, "%ld", ulCashOutTotalAmount);
      vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
	  memset(szTemp, 0x00, sizeof(szTemp));
	  sprintf(szTemp, "%s %s|", strCST.szCurSymbol, szAmtBuf);
	  strcat(szDispUI, szTemp); //trans amount
      /*end cash out*/

	  /*cash in*/
      strcat(szDispUI, "CASH IN|"); //trans type
      memset(szTemp, 0x00, sizeof(szTemp));
      sprintf(szTemp, "%03d|", usCashInCount);
      strcat(szDispUI, szTemp); //trans count
      
      memset(szBuf, 0x00, sizeof(szBuf));
      sprintf(szBuf, "%ld", ulCashInTotalAmount);
      vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
      memset(szTemp, 0x00, sizeof(szTemp));
      sprintf(szTemp, "%s %s|", strCST.szCurSymbol, szAmtBuf);
      strcat(szDispUI, szTemp); //trans amount
      /*end cash in*/

	  
	  /*bils payment*/
      strcat(szDispUI, "BILLS PAYMENT|"); //trans type
      memset(szTemp, 0x00, sizeof(szTemp));
      sprintf(szTemp, "%03d|", usBillsPaymentCount);
      strcat(szDispUI, szTemp); //trans count
      
      memset(szBuf, 0x00, sizeof(szBuf));
      sprintf(szBuf, "%ld", ulBillsPaymentTotalAmount);
      vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
      memset(szTemp, 0x00, sizeof(szTemp));
      sprintf(szTemp, "%s %s|", strCST.szCurSymbol, szAmtBuf);
      strcat(szDispUI, szTemp); //trans amount
      /*end bils payment*/

	  /*fund transfer*/
      strcat(szDispUI, "TRANSFER|"); //trans type
      memset(szTemp, 0x00, sizeof(szTemp));
      sprintf(szTemp, "%03d|", usFundTransferCount);
      strcat(szDispUI, szTemp); //trans count
      
      memset(szBuf, 0x00, sizeof(szBuf));
      sprintf(szBuf, "%ld", ulFundTransferTotalAmount);
      vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
      memset(szTemp, 0x00, sizeof(szTemp));
      sprintf(szTemp, "%s %s|", strCST.szCurSymbol, szAmtBuf);
      strcat(szDispUI, szTemp); //trans amount
      /*end fund transfer*/

     /*totals*/
     strcat(szDispUI, "TOTAL|"); //trans type
     memset(szTemp, 0x00, sizeof(szTemp));
     sprintf(szTemp, "%03d|", usTotalCount);
     strcat(szDispUI, szTemp); //trans count
     
     memset(szBuf, 0x00, sizeof(szBuf));
     sprintf(szBuf, "%ld", ulTotalAmount);
     vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szBuf,szAmtBuf);
     memset(szTemp, 0x00, sizeof(szTemp));
     sprintf(szTemp, "%s %s|", strCST.szCurSymbol, szAmtBuf);
     strcat(szDispUI, szTemp); //trans amoun	  
	 /*end totals*/
	 
     memset(szDisplayBuf0,0x00,sizeof(szDisplayBuf0));
     memset(szTemp, 0x00, sizeof(szTemp));		

	   vdDebug_LogPrintf("SATURN  inCTOS_DisplaySettleBatchTotal %s inConfirmTimeOut %d", szDispUI, inConfirmTimeOut);

	   
       #if 0
	   if ((get_env_int("AUTOSETTLETRIGGERED")== 1) && (srTransRec,byTransType==SETTLE))
	   {
			//usCTOSS_DisplayUI(szDispUI);
			key = usCTOSS_Confirm2(szDispUI);
			CTOS_Delay(3000);
			return d_OK;
	   }
	   else
	   #endif
	   {
	   	
    if(srTransRec,byTransType==SETTLE)
        key = usCTOSS_Confirm(szDispUI);
    else
        key = usCTOSS_Confirm3(szDispUI);
			
			if (key == d_USER_CANCEL) /*timeout and key cancel*/
			{
					 
				memset(szTitle, 0x00, sizeof(szTitle));
				szGetTransTitle(srTransRec.byTransType, szTitle);

				memset(szDispUI, 0x00, sizeof(szDispUI));
				strcpy(szDispUI, szTitle);
				strcat(szDispUI, "|");
				strcat(szDispUI, "USER CANCEL");
				//usCTOSS_LCDDisplay(szDispUI);
				vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
				CTOS_Beep();
				CTOS_Delay(1000);
				return d_NO;
			}

			// sidumili: timeout display message
			if (key == d_TIMEOUT)
			{
				vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
				CTOS_Beep();
		        CTOS_Delay(1000);
				return d_NO;
			}
				
			if (key != d_OK)
				return d_NO;
				
			return d_OK;
	   } 				  



#endif
		
    return d_OK;
}

int inCTOS_DisplayBatchRecordDetail(int inType, int inNavRecType)
{
    BYTE szTotalAmount[12+1];
    BYTE szAmtOut[12+1];
    BYTE szINV[6+1];
    BYTE szDisplayBuf[100+1];
    BYTE szTitle[16+1];

   
   BYTE szINVBuf[40+1];
   BYTE szPANBuf[40+1];
   BYTE szDisplayAmountBuf[40+1];
   BYTE inKey = d_NO;
   
   BYTE szDisplayBuf2[40+1];
   BYTE szDisplayInvNo[40+1];

	char szYear[3];
	char szTempDate[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szTemp3[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	BYTE szDisplayDateTime[40+1];
	BYTE szDisplayIssuer[40+1];
	CTOS_RTC SetRTC;
    BYTE szNavRecType[2+1]={0};
	
   	vdDebug_LogPrintf("--inCTOS_DisplayBatchRecordDetail--");
	vdDebug_LogPrintf("inNavRecType=[%d]", inNavRecType);

	memset(szNavRecType, 0x00, sizeof(szNavRecType));
	sprintf(szNavRecType, "%d", inNavRecType);
	
    //CTOS_LCDTClearDisplay();  

    vdDispTransTitle(inType);

    inIITRead(srTransRec.IITid);
    inCSTRead(1);
	
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
    memset(szDisplayBuf2, 0x00, sizeof(szDisplayBuf2));
    sprintf(szDisplayBuf2, "%s   %s", szTitle, strIIT.szIssuerLabel);
    //CTOS_LCDTPrintXY(1, 3, szDisplayBuf);

    memset(szDisplayBuf,0,sizeof(szDisplayBuf));
    vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szDisplayBuf, strIIT.fMaskPan);
    //CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
    memset(szPANBuf,0,sizeof(szPANBuf));
	strcpy(szPANBuf, szDisplayBuf);

    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    vdDebug_LogPrintf("szTotalAmount[%s]", szTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmount,szAmtOut);
    vdDebug_LogPrintf("szTotalAmount[%s]", szDisplayBuf);
    memset(szDisplayAmountBuf,0,sizeof(szDisplayAmountBuf));
    sprintf(szDisplayAmountBuf, "%s   %s", strCST.szCurSymbol, szAmtOut);
    //CTOS_LCDTPrintXY(1, 5, szDisplayBuf);

   // Invoice No.
	memset(szINV, 0x00, sizeof(szINV));
	wub_hex_2_str(srTransRec.szInvoiceNo, szINV, 3);
	memset(szDisplayInvNo, 0x00, sizeof(szDisplayInvNo));
	sprintf(szDisplayInvNo, "%06ld", atol(szINV));
    //CTOS_LCDTPrintXY(1, 6, szDisplayBuf);

#if 0
    if ((strTCT.byTerminalType%2) == 0)
    {
        CTOS_LCDTPrintAligned(8, "UP->UP", d_LCD_ALIGNLEFT);
        CTOS_LCDTPrintAligned(8, "DOT->DOWN", d_LCD_ALIGNRIGHT);
    }
    else
        vdCTOS_LCDGShowUpDown(1,1);
    
    return d_OK;
#endif

// Date Time	
	CTOS_RTCGet(&SetRTC);
	memset(szYear, 0x00, sizeof(szYear));
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	memset(szTemp, 0x00, sizeof(szTemp));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	memset(szTemp3, 0x00, sizeof(szTemp3));
	memset(szTemp4, 0x00, sizeof(szTemp4));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);
	memset(szTempDate, 0x00, sizeof(szTempDate));
	sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
	vdCTOS_FormatDate(szTempDate);
	sprintf(szTemp3, "%s", szTempDate);
	sprintf(szTemp4, "%02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	memset(szDisplayDateTime, 0x00, sizeof(szDisplayDateTime));
	sprintf(szDisplayDateTime, "%s / %s", szTemp3, szTemp4);

	// Issuer
	memset(szDisplayIssuer,0x00,sizeof(szDisplayIssuer));
	strcpy(szDisplayIssuer, strIIT.szIssuerLabel);
		
	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	strcpy(szDisplayBuf, "BATCH REVIEW");	
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szTitle);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szDisplayIssuer);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szDisplayDateTime);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szPANBuf);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szDisplayAmountBuf);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szDisplayInvNo);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szNavRecType);
	
	vdDebug_LogPrintf("inCTOS_DisplayBatchRecordDetail, szDisplayBuf[%s]", szDisplayBuf);
	
	inKey=usCTOSS_BatchReviewUI(szDisplayBuf);

return inKey;

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

BYTE InputAmount2(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter)
{
    char szTemp[24+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[24+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;;
	
	memset(szAmount, 0x00, sizeof(szAmount));
	
	while(1)
	{
		memset(szTemp, 0x00, sizeof(szTemp));         	 	
		if(strlen(szAmount) > 0)
		{
		    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szAmount, szTemp);
		}
		else
		{
		    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", "0", szTemp);
		}

 
        clearLine(usY);
        CTOS_LCDTPrintXY(usX, usY, szCurSymbol);
        int x=0;
        int len, index;
        len=strlen(szTemp);
        for(index=0; index < len; index++)
        {
            if(szTemp[index] == '.')
                x+=1;
            else
                x+=2;
        }
        
        CTOS_LCDTPrintXY(36-x, usY, szTemp);
		
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}
		}

		if(inRet >= 48 && inRet <= 57)
		{
			if(inCtr < 9)
			{
			    memset(szTemp, 0x00, sizeof(szTemp));
			    sprintf(szTemp, "%c", inRet);
                strcat(szAmount, szTemp);			
                inCtr++; 		
				if(inCtr == 1 && szAmount[0] == 48)
				{
					memset(szAmount, 0x00, sizeof(szAmount));
					inCtr=0;
				}
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_USER_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{
			if(strlen(szAmount) > 0)
			{
				memcpy(baAmount, szAmount, strlen(szAmount));
				return d_OK;
			}
			else
			{
				if(bIgnoreEnter == 1)
					return d_OK;			
			}
		}
		else if(inRet == 82) /*clear key*/
		{		
			if(inCtr > 0)
                inCtr--;
            szAmount[inCtr]=0x00;
		}
	}
}

int inChkAccumEmpty(void)
{
    ACCUM_REC srAccumRec;
    int inResult=ST_SUCCESS;
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) != ST_SUCCESS)
    {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        CTOS_LCDTClearDisplay();
        vdSetErrorMessage("BATCH EMPTY");
        return ST_ERROR;    
    }
    return ST_SUCCESS;	
}

BYTE InputStringAlpha2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    char szTemp[40+1], szTemp2[40+1];
    USHORT inRet;
	int inCtr=0;
	char szAmount[40+1];
	char chAmount=0x00;
	char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;
	int x=0;
	int inLastKey=255;
	int inKey=0;
	int inKey2=0;
    int inMax=*usStrLen;
	int inAtCtr=0;
	char KeyPad[10][4]={{'0', '0', '0', '0'},
		               {'1', 'Q', 'Z', '1'},
		               {'2', 'A', 'B', 'C'},
					   {'3', 'D', 'E', 'F'},
					   {'4', 'G', 'H', 'I'},
					   {'5', 'J', 'K', 'L'},
					   {'6', 'M', 'N', 'O'},
					   {'7', 'P', 'R', 'S'},
					   {'8', 'T', 'U', 'V'},
					   {'9', 'W', 'X', 'Y'}};

    char KeyPad2[10][10]={{'0', '~', '!', '@', '#', '$', '%', '^', '&', '.'},
					   {'1', 'Q', 'Z', '=', 'q', 'z', '+', '[', ']', '1'},
					   {'2', 'A', 'B', 'C', 'a', 'b', 'c', '(', ')', '2'},
					   {'3', 'D', 'E', 'F', 'd', 'e', 'f', '{', '}', '3'},
					   {'4', 'G', 'H', 'I', 'g', 'h', 'i', '<', '>', '4'},
					   {'5', 'J', 'K', 'L', 'j', 'k', 'l', '|', '5', '5'},
					   {'6', 'M', 'N', 'O', 'm', 'n', 'o', '6', '6', '6'},
					   {'7', 'P', 'R', 'S', 'p', 'r', 's', '7', '7', '7'},
					   {'8', 'T', 'U', 'V', 't', 'u', 'v', '?', '8', '8'},
					   {'9', 'W', 'X', 'Y', 'w', 'x', 'y', '9', '9', '9'}};

	char KeyPad3[10][4]={{'0', '@', '.', '_'},
		               {'q', 'z', '1', 'q'},
		               {'a', 'b', 'c', '2'},
					   {'d', 'e', 'f', '3'},
					   {'g', 'h', 'i', '4'},
					   {'j', 'k', 'l', '5'},
					   {'m', 'n', 'o', '6'},
					   {'p', 'r', 's', '7'},
					   {'t', 'u', 'v', '8'},
					   {'w', 'x', 'y', '9'}};

	memset(szAmount, 0x00, sizeof(szAmount));
	
	CTOS_TimeOutSet(TIMER_ID_1, 100);
	
	while(1)
	{	
		memset(szTemp, 0x00, sizeof(szTemp));  
         
        clearLine(usY);
		clearLine(usY+1);
        //CTOS_LCDTPrintXY(40-((strlen(szAmount)*2)+inAtCtr), usY, szAmount);
		
		if(strlen(szAmount)>20)
		{
			memset(szTemp, 0x00, sizeof(szTemp));
			memset(szTemp2, 0x00, sizeof(szTemp2));
			memcpy(szTemp, szAmount, 20);
			strcpy(szTemp2, szAmount+20);
		    setLCDPrint(usY, DISPLAY_POSITION_RIGHT, szTemp);	
		    setLCDPrint(usY+1, DISPLAY_POSITION_RIGHT, szTemp2);
		}
		else
			setLCDPrint(usY, DISPLAY_POSITION_RIGHT, szAmount);
		
		CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
		while(1)//loop for time out
		{
            if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                inLastKey=255;
			
			CTOS_KBDInKey(&isKey);
			if (isKey){ //If isKey is TRUE, represent key be pressed //
				vduiLightOn();
				//Get a key from keyboard //
				CTOS_KBDGet(&c);
				inRet=c;
				break;
			}
			else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
			{	   
				return 0xFF;
			}
		}
        vdDebug_LogPrintf("key: (%c)", inRet);
		if(inRet >= 48 && inRet <= 57)
		{
			char szTemp2[24+1];
			
			memset(szTemp, 0x00, sizeof(szTemp));
			sprintf(szTemp, "%c", inRet);
			inKey=atoi(szTemp);
			
			CTOS_TimeOutSet(TIMER_ID_1, 100);
			if((inCtr < inMax) || (inLastKey == inKey))
			{
                vdDebug_LogPrintf("1. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
				
                if(inLastKey == inKey)
                {
					inKey2++;
					if(bInputMode == 0x04)
					{
					    if(inKey2 > 9)
                            inKey2=0;
					}
                    else
                    {
                        if(inKey2 > 3)
                        inKey2=0;
                    }
                }
                else
					inKey2=0;
				                
                if(inLastKey == inKey)
                    inCtr--;

				if(bInputMode == 0x04)
				{
					szAmount[inCtr]=KeyPad2[inKey][inKey2];
					if(szAmount[inCtr] == '@')
						inAtCtr++;
				}
				else if(bInputMode == 0x05)
				{
					szAmount[inCtr]=KeyPad3[inKey][inKey2];
					if(szAmount[inCtr] == '@') /*@ symbol is occupying 2 spaces*/
						inAtCtr++;
				}
				else
					szAmount[inCtr]=KeyPad[inKey][inKey2];
				inCtr++;

                vdDebug_LogPrintf("2. inLastKey:(%d), inKey:(%d), inKey2:(%d),ctr:(%d)", inLastKey, inKey, inKey2, inCtr);
                vdDebug_LogPrintf("szAmount: (%s)", szAmount);
                				
                inLastKey=inKey;
                if(bInputMode != 0x04 && bInputMode != 0x05)
                {
                    if(inKey == 0)
				        inLastKey=255;
                }
			}
		}
		else if(inRet == 67) /*cancel key*/
		{
			return d_KBD_CANCEL;
		}
		else if(inRet == 65) /*entery key*/
		{			
			if(strlen(szAmount) > 0)
			{
				memcpy(pbaStr, szAmount, strlen(szAmount));
				*usStrLen=strlen(szAmount);
				return d_KBD_ENTER;
			}
			
			if(usMinLen == 0)
                return d_KBD_ENTER;			
		}
		else if(inRet == 81) /*dot key*/
		{	
			if(bInputMode == 0x04 || bInputMode == 0x05)
			{
                szAmount[inCtr]='.';
			    inCtr++;
			    inKey2=0;
			    inLastKey=255;
			}
		}
		else if(inRet == 82) /*clear key*/
		{	
            if(bInputMode == 0x04 || bInputMode == 0x05)
            {
                if(szAmount[inCtr] == '@')
                    inAtCtr--;			
            }
			
			inCtr--;
			if(inCtr <= 0)
                inCtr=0;
            szAmount[inCtr]=0x00;
			inKey2=0;
			inLastKey=255;
		}
	}
}

int inCTOS_DisplayCardNum(void)
{
    char szDisplayBuf[30];
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;

	BYTE szDisMsg[200];
    BYTE szPan[30+1];
	char szLineBuf[40];
	char szCardLabel[50 + 1]; 
	int ret;
    BYTE szTemp1[30+1];

	vdDebug_LogPrintf("--inCTOS_DisplayCardNum--");
	
	if (1 == inCTOSS_GetWaveTransType())
	if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	vdDebug_LogPrintf("inCTOSS_GetWaveTransType[%d]", inCTOSS_GetWaveTransType());
	vdDebug_LogPrintf("srTransRec.byEntryMode[%d]", srTransRec.byEntryMode);
		    
	CTOS_LCDTClearDisplay();
	vduiLightOn();
	vdDispTransTitle(srTransRec.byTransType);

#if 0
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
		{
			inCTOS_DisplayCardTitle(5, 6, TRUE);
		}
    }       
#endif

	memset(szTemp1,0,sizeof(szTemp1));
	vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1, strIIT.fMaskPan);

	vdDebug_LogPrintf("usCTOSS_ConfirmMenu");
	strcpy(szDisMsg, strIIT.szIssuerLabel);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, szTemp1);
	
	
	vdDebug_LogPrintf("usCTOSS_ConfirmMenu szDisMsg[%s]", szDisMsg);
	
	
	ret = usCTOSS_Confirm(szDisMsg);

	vdDebug_LogPrintf("saturn confirm PAN %d", ret);

	if (ret != d_OK)
		return d_NO;

	// Confirmation
	//if (inConfirmPAN() != d_OK)
	//	return d_NO;
	
    return d_OK;
}

