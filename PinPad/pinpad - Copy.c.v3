#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

#include "../Database/DatabaseFunc.h"
#include "pinpad.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSTypedef.h"
#include "..\Debug\Debug.h"
#include "../Ui/Display.h"
#include "..\Includes\CTOSInput.h"

#define PIN_POSITION_X	6
#define PIN_POSITION_Y	7

extern BOOL fTimeOutFlag; /*BDO: Flag for timeout --sidumili*/

int inInitializePinPad(void)
{
    CTOS_KMS2Init();
    //TEST_Write3DES_Plaintext();
    return d_OK;
}

//For testing and development only, hardcode the key
void TEST_Write3DES_Plaintext(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];
	
	KeySet = 0x0002;
	KeyIndex = 0x0002;
	memcpy(KeyData, "\x11\x11\x11\x11\x22\x22\x22\x22\x33\x33\x33\x33\x44\x44\x44\x44", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	CTOS_KMS2KeyWrite(&para);

/*
	strcpy(srTransRec.szPAN,"1234567890123456");
	srTransRec.HDTid = 6;
	srTransRec.byTransType = SALE;
	strcpy(strCST.szCurSymbol,"SGD");
	strcpy(srTransRec.szTotalAmount,"1");
	inGetIPPPin();
	memset(szDataIn, 'A', 100);
	memset(szInitialVector, 0, 8);
	memset(szMAC, 0, 8);
	inIPPGetMAC(szDataIn, 8, szInitialVector, szMAC);*/
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00209 Total amount display not align in the middle start
#if 0	
void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine, int inDisplayBalancePos) //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE start *added parameter inDisplayBalancePos 3 of 6
{
	char szDisplayBuf[30];
	BYTE baAmount[20];

	DebugAddSTR("inCTOS_DisplayCurrencyAmount","Processing...",20);
	
	memset(baAmount, 0x00, sizeof(baAmount));
	wub_hex_2_str(szAmount, baAmount, 6);

	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
	sprintf(szDisplayBuf, "%s%10lu.%02lu", strCST.szCurSymbol, atol(baAmount)/100, atol(baAmount)%100);
	setLCDPrint(inLine, inDisplayBalancePos, szDisplayBuf);
	
}
#else
void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine, int inDisplayBalancePos) //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE start *added parameter inDisplayBalancePos 3 of 6
{
	char szDisplayBuf[30];
	char szRawAmount[20+1], szFmtAmount[20+1], szWholeAmt[20+1], szRemainderAmt[20+1];
	BYTE baAmount[20];

	DebugAddSTR("inCTOS_DisplayCurrencyAmount","Processing...",20);
	memset(baAmount, 0x00, sizeof(baAmount));
	wub_hex_2_str(szAmount, baAmount, 6);

	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
	//sprintf(szDisplayBuf, "%s%10lu.%02lu", strCST.szCurSymbol, atol(baAmount)/100, atol(baAmount)%100);
    sprintf(szWholeAmt,"%lu",atol(baAmount)/100);
	sprintf(szRemainderAmt,"%02lu",atol(baAmount)%100);

	if(strcmp(szRemainderAmt,"00") == 0)
    {
		memset(szRemainderAmt,0,sizeof(szRemainderAmt));
		memcpy(&szRemainderAmt,"00",2);
    }

	strcat(szWholeAmt,szRemainderAmt);
	strcpy(szRawAmount,szWholeAmt);
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szRawAmount,szFmtAmount);
	strcat(szDisplayBuf,szFmtAmount);

	setLCDPrint(inLine, inDisplayBalancePos, szDisplayBuf);
	
}
#endif
//aaronnino for BDOCLG ver 9.0 fix on issue #00209 Total amount display not align in the middle end


void OnGetPINDigit(BYTE NoDigits)
{
	BYTE i;
	
	for(i=0;i<NoDigits;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y,"*");
		
	for(i=NoDigits;i<12;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y," ");
	
   DebugAddINT("OnGetPINDigit", NoDigits);

}

void OnGetPINCancel(void)
{
   DebugAddINT("OnGetPINCancel", 1);

}

void OnGetPINBackspace(BYTE NoDigits)
{
	BYTE i;
	
	for(i=0;i<NoDigits;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y,"*");
		
	for(i=NoDigits;i<12;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y," ");
   DebugAddINT("OnGetPINBackspace", NoDigits);
}

int inGetIPPPin(void)
{
	CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

	BYTE str[40],key;
	USHORT ret;
	char szDebug[40 + 1]={0};
	char szDisplayBalAcctTitle [40+1];

	int inRet;
    BYTE szBaseAmt[20+1], szAmtBuff[20+1], szCurAmtBuff[20+1];

	vdDebug_LogPrintf("inGetIPPPin");

	vdDebug_LogPrintf("::byEntryMode[%d]", srTransRec.byEntryMode);
	
	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		return d_OK;
	
	DebugAddSTR("inGetIPPPin","Processing...",20);
	//inDCTRead(srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid,srTransRec.MITid);

	
	/*********************************/
	
	vdDebug_LogPrintf("inGetIPPPin | MITid[%d]", srTransRec.MITid);
	vdDebug_LogPrintf("inGetIPPPin | HDTid[%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("inGetIPPPin | byTransType[%d]", (int)srTransRec.byTransType);
	vdDebug_LogPrintf("inGetIPPPin | usKeySet[%d]", strDCT.usKeySet);
	vdDebug_LogPrintf("inGetIPPPin | usKeyIndex[%d]", strDCT.usKeyIndex);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.inMinPINDigit[%d]", strDCT.inMinPINDigit);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.inMaxPINDigit[%d]", strDCT.inMaxPINDigit);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.szDisplayLine1[%s]", strDCT.szDisplayLine1);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.szDisplayLine2[%s]", strDCT.szDisplayLine2);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.szPINKey[%s]", strDCT.szPINKey);
	vdDebug_LogPrintf("inGetIPPPin | PAN[%s] | SourceCard[%s] | DestinationCard[%s]", srTransRec.szPAN, srTransRec.szSourceCardNo, srTransRec.szDestCardNo);
	/*********************************/

	/*check for keys if injected -- sidumili*/
	inRet = inCheckKeys(strDCT.usKeySet, strDCT.usKeyIndex);
	if (inRet != d_OK)
	return(inRet);
	/*check for keys if injected -- sidumili*/

//aaronnino for BDOCLG ver 9.0 fix on issue #00143	Change display for Pin Entry for Balance Inquiry, Debit Sale, CUP Sale, Credit Sale start	
    memset(szDisplayBalAcctTitle,0,sizeof(szDisplayBalAcctTitle));

	CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
    
    if (srTransRec.byTransType == BALANCE_INQUIRY)
    {
		
       strcpy(szDisplayBalAcctTitle,"BALANCE      ");
    
       if (inAccountType == CURRENT_ACCOUNT)
          strcat (szDisplayBalAcctTitle, "CURRENT");
       else
           strcat (szDisplayBalAcctTitle, "SAVINGS");
    
       CTOS_LCDTClearDisplay();
       vdDispTitleString(szDisplayBalAcctTitle);
    }	
    else if (srTransRec.byTransType == SALE)
    {
       strcpy(szDisplayBalAcctTitle,"DEBIT        ");
    
       if (inAccountType == CURRENT_ACCOUNT)
          strcat (szDisplayBalAcctTitle, "CURRENT");
       else
          strcat (szDisplayBalAcctTitle, "SAVINGS");
    
       CTOS_LCDTClearDisplay();
       vdDispTitleString(szDisplayBalAcctTitle);
    }
    else if (srTransRec.byTransType == RELOAD)
    {
       strcpy(szDisplayBalAcctTitle,"RELOAD       ");
    
       if (inAccountType == CURRENT_ACCOUNT)
          strcat (szDisplayBalAcctTitle, "CURRENT");
       else
          strcat (szDisplayBalAcctTitle, "SAVINGS");
    
       CTOS_LCDTClearDisplay();
       vdDispTitleString(szDisplayBalAcctTitle);
    }

    else
       vdDispTransTitle(srTransRec.byTransType);

	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(srTransRec.byTransType);
//aaronnino for BDOCLG ver 9.0 fix on issue #00143	Change display for Pin Entry for Balance Inquiry, Debit Sale, CUP Sale, Credit Sale end

	// sidumili: [NO AMOUNT DISPLAY DURING BALANCE INQUIRY]
	if (srTransRec.byTransType != BALANCE_INQUIRY){
		//inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3, DISPLAY_POSITION_LEFT);
		//aaronnino for BDOCLG ver 9.0 fix on issue #00217 Enter PIN display(transactional only) start
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "TOTAL:");
		//FIX FOR 99,999,999.99 AMOUNT	
		wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
		vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBaseAmt,szAmtBuff);
		sprintf(szCurAmtBuff,"%s%s",strCST.szCurSymbol, szAmtBuff);
		setLCDPrint(4, DISPLAY_POSITION_CENTER, szCurAmtBuff);
		//inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 4, 1);
		//END
		//aaronnino for BDOCLG ver 9.0 fix on issue #00217 Enter PIN display(transactional only) end
	}
	
	if (srTransRec.byTransType == BALANCE_INQUIRY)
	{
		//setLCDPrint(4, DISPLAY_POSITION_LEFT, "HAND PIN PAD TO CUST");
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "PLEASE HAND PIN PAD");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "TO CUSTOMER"); 
	}
		
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(6, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	
	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
	stPinGetPara.Version = 0x02;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;

	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
//add start        
	stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
    stPinGetPara.Protection.pSK = strDCT.szPINKey;
//add end        
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;

	/* BDO: For Reload, save source card -- sidumili */
	if (srTransRec.byTransType == RELOAD)
		strcpy(srTransRec.szPAN, srTransRec.szSourceCardNo);	
	/* BDO: For Reload, save source card -- sidumili */
	
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
	//stPinGetPara.Control.Timeout = 30; /*BDO: Set 30secs timeout --sidumili*/
	stPinGetPara.Control.Timeout = inGetIdleTimeOut(TRUE);
	stPinGetPara.Control.NULLPIN = FALSE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
	stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
	stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE); // Fix for undisplay of * during pin -- sidumili	
	ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);

	/*********************************/
	vdDebug_LogPrintf("inGetIPPPin | ret[%d]", ret);
	vdDebug_LogPrintf("inGetIPPPin | szPINBlock[%02X][%02X][%02X]", (unsigned char)srTransRec.szPINBlock[0], (unsigned char)srTransRec.szPINBlock[1], (unsigned char)srTransRec.szPINBlock[2]);
	vdDebug_LogPrintf("inGetIPPPin | szPAN[%s]", srTransRec.szPAN);
	/*********************************/

	
	if(ret != d_OK)
	{
		if (ret == 10511){
			//vdDispErrMsg("PLEASE INJECT KEY");
			//vdDispErrMsg("USER CANCEL");
			vdSetErrorMessage("USER CANCEL");
			vdDisplayErrorMsg(1, 7, "PIN CANCELLED");

		}
		else if (ret == 10512) /*Timeout --sidumili*/ 
		{
			fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
			vdSetErrorMessage("PIN TIMEOUT");
			vdDisplayErrorMsg(1, 7, "PIN TIMEOUT");

		}
		else{
			if(ret != 10500)
			{
				sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
				CTOS_LCDTPrintXY(1, 8, str);
				CTOS_KBDGet(&key);
				vdDebug_LogPrintf("inGetIPPPin | key[%s]", key);
			}
			else// if return is 10500, terminal will prompt user for PIN entry retry
			{
				CTOS_LCDTClearDisplay();			
				vdDisplayErrorMsgResp2("PIN CAPTURE","ERROR","PLEASE TRY AGAIN");
			}
		}
		
		return ret;
		
	}
	return d_OK;
}

int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC)
{
	CTOS_KMS2MAC_PARA para;
	USHORT ret;
	BYTE key,str[40];
	
	CTOS_LCDTClearDisplay();
	DebugAddSTR("inGetIPPMAC","Processing...",20);    
	
	memset(&para, 0x00, sizeof(CTOS_KMS2MAC_PARA));
	para.Version = 0x01;
	para.Protection.CipherKeySet = strDCT.usKeySet;
	para.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	para.Protection.CipherMethod = KMS2_MACMETHOD_CBC;
	para.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	para.Protection.pSK = strDCT.szMACKey;
	para.ICV.Length = 8;
	para.ICV.pData = szInitialVector;
	para.Input.Length = inLengthIn;
	para.Input.pData = szDataIn;
	para.Output.pData = szMAC;
	
	ret = CTOS_KMS2MAC(&para);
	if(ret != d_OK)
		return ret;
	return d_OK;		
}

int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC)
{
	BYTE szInitialVector[8];

	inIPPGetMAC(szDataIn, inLengthIn,  szInitialVector, szMAC);
}
	

/*sidumili: [check key if injected]*/
int inCheckKeys(USHORT ushKeySet, USHORT ushKeyIndex){
	USHORT rtn;

	vdDebug_LogPrintf("inCheckKeys SET[%04X] IDX[%04X]", ushKeySet, ushKeyIndex);

	/*just call once when terminal startup*/
	//CTOS_KMS2Init();

	rtn = CTOS_KMS2KeyCheck(ushKeySet, ushKeyIndex);

	vdDebug_LogPrintf("inCheckKeys CTOS_KMS2KeyCheck rtn[%d]", rtn);

	//if (rtn == d_OK)
              //vdDisplayErrorMsg(1, 7, "Key Check OK");
	//else
	if (rtn != d_OK)
    	vdDisplayErrorMsg(1, 7, "PLEASE INJECT KEY");

	return rtn;

}


/*   
int inOldGetIPPPin1(void)
{
	CTOS_KMS2PINGET_PARA para;
	BYTE str[17],key;
	USHORT ret;
	
	DebugAddSTR("inGetIPPPin","Processing...",20);
	inDCTRead(srTransRec.HDTid);
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	
	inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);

	memset(&para, 0x00, sizeof(CTOS_KMS2PINGET_PARA));
	para.Version = 0x01;
	para.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	para.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	para.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
	para.Protection.CipherKeySet = strDCT.usKeySet;
	para.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	para.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	para.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	para.Protection.pSK = strDCT.szPINKey;
	para.AdditionalData.InLength = strlen(srTransRec.szPAN);
	para.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
	para.PINOutput.EncryptedBlockLength = 8;
	para.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	para.Control.Timeout = 0;
	para.Control.AsteriskPositionX = 8;
	para.Control.AsteriskPositionY = 7;
	para.Control.NULLPIN = FALSE;
	para.Control.piTestCancel = NULL;
	
	ret = CTOS_KMS2PINGet(&para);
	if(ret != d_OK)
	{
		sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return ret;
	}
	return d_OK;
}
*/
