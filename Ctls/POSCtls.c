#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <emv_cl.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
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


#include "POSCtls.h"

#define	d_MsessionFlag		0x00
#define d_AsessionFlag		0x01


ULONG inCTOSS_CLMOpen(void)
{
	ULONG ulAPRtn;
	ulAPRtn = EMVCL_Open();
	if(d_EMVCL_NO_ERROR != ulAPRtn) 
	{
		return ulAPRtn;
	}

	return d_OK;
}

void vdCTOSS_GetCLMVersion(char *cVer)
{
	EMVCL_GetAPIVersion(cVer);
}

void vdCTOSS_CLMClose(void)
{
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		EMVCL_Close();

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		VW_CloseComm(d_VW_COMM_COM2);

}


//------------------------------------------------------------------------

ULONG MsessionAuthen(void) {
    AUTHKEY_DATA stKeyData;
	ULONG ulAPRtn;
    BYTE baRN_Reader[9], baTmpRN[17], baOutput[17];
    BYTE baIMEAAuthSK[17];

    //CIT_ClearDisplay();
    CTOS_BackLightSet(d_BKLIT_LCD, d_ON);
    //Initialize Communication
    ulAPRtn = VW_InitializeCommunication(d_VW_MEK, 0x01, "\x01\x02\x03\x04\x05\x06\x07\x08", &stKeyData);
    if (ulAPRtn == d_NO_ERROR) {
        //Mutual Authentication
        memcpy(&baRN_Reader, &stKeyData.baRND_R[0], 8);
        //IMEK Authentication SK=3DES[IMEK, RND_R(5:8),RND_B(1:4),RND_R(1:4),RND_B(5:8)]
        memcpy(&baTmpRN[0], &baRN_Reader[4], 4);
        memcpy(&baTmpRN[4], "\x01\x02\x03\x04", 4);
        memcpy(&baTmpRN[8], &baRN_Reader[0], 4);
        memcpy(&baTmpRN[12], "\x05\x06\x07\x08", 4);
        VW_DES(d_ENCRYPTION, 16, "\x00\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x00\x00\x00\x00\x00\x00\x00\x00", baTmpRN, baIMEAAuthSK);
        VW_DES(d_ENCRYPTION, 16, "\x00\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x00\x00\x00\x00\x00\x00\x00\x00", baTmpRN + 8, baIMEAAuthSK + 8);

        //crypto2=3DES[AuthSK, RN_B(5:8),RN_R(1:4),RN_B(1:4),RND_R(5:8)]
        memcpy(&baTmpRN[0], "\x05\x06\x07\x08", 4);
        memcpy(&baTmpRN[4], &baRN_Reader[0], 4);
        memcpy(&baTmpRN[8], "\x01\x02\x03\x04", 4);
        memcpy(&baTmpRN[12], &baRN_Reader[4], 4);
        VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN, baOutput);
        VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN + 8, baOutput + 8);

        stKeyData.bKeyType = d_VW_MEK;
        stKeyData.bKeyIndex = 0x01;
        memcpy(stKeyData.baEnRND, baOutput, 16);
        ulAPRtn = VW_MutualAuthenticate(&stKeyData);
        if (ulAPRtn == d_NO_ERROR) {
            //Generate Keys
            stKeyData.bKeyType = d_VW_Msession;
            stKeyData.bKeyIndex = 0x01;
            memcpy(&baTmpRN[0], "\x22\x22\x22\x22\x22\x22\x22\x22\x33\x33\x33\x33\x33\x33\x33\x33", 16);
            VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN, baOutput);
            VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN + 8, baOutput + 8);
            memcpy(stKeyData.baEnRND, baOutput, 16);
            ulAPRtn = VW_GenerateKeys(&stKeyData);
            if (ulAPRtn != d_NO_ERROR)
                return ulAPRtn;
            else {
                ulAPRtn = VW_SetSessionKey(d_MsessionFlag, "\x22\x22\x22\x22\x22\x22\x22\x22\x33\x33\x33\x33\x33\x33\x33\x33");
                if (ulAPRtn != d_NO_ERROR)
                    return ulAPRtn;
            }
        } else
            return ulAPRtn;
    } else
        return ulAPRtn;

    return ulAPRtn;
}




//------------------------------------------------------------------------

ULONG MsessionAuthenAdmin(void) {
    AUTHKEY_DATA stKeyData;
	ULONG ulAPRtn;
    BYTE baRN_Reader[9], baTmpRN[17], baOutput[17];
    BYTE baIMEAAuthSK[17];

    //CIT_ClearDisplay();
    CTOS_BackLightSet(d_BKLIT_LCD, d_ON);
    //Initialize Communication
    ulAPRtn = VW_InitializeCommunication(d_VW_AEK, 0x01, "\x11\x12\x13\x14\x15\x16\x17\x18", &stKeyData);
    if (ulAPRtn == d_NO_ERROR) {
        //Mutual Authentication
        memcpy(&baRN_Reader, &stKeyData.baRND_R[0], 8);
        //IMEK Authentication SK=3DES[IMEK, RND_R(5:8),RND_B(1:4),RND_R(1:4),RND_B(5:8)]
        memcpy(&baTmpRN[0], &baRN_Reader[4], 4);
        memcpy(&baTmpRN[4], "\x11\x12\x13\x14", 4);
        memcpy(&baTmpRN[8], &baRN_Reader[0], 4);
        memcpy(&baTmpRN[12], "\x15\x16\x17\x18", 4);
        VW_DES(d_ENCRYPTION, 16, "\x00\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x00\x00\x00\x00\x00\x00\x00\x00", baTmpRN, baIMEAAuthSK);
        VW_DES(d_ENCRYPTION, 16, "\x00\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x00\x00\x00\x00\x00\x00\x00\x00", baTmpRN + 8, baIMEAAuthSK + 8);

        //crypto2=3DES[AuthSK, RN_B(5:8),RN_R(1:4),RN_B(1:4),RND_R(5:8)]
        memcpy(&baTmpRN[0], "\x15\x16\x17\x18", 4);
        memcpy(&baTmpRN[4], &baRN_Reader[0], 4);
        memcpy(&baTmpRN[8], "\x11\x12\x13\x14", 4);
        memcpy(&baTmpRN[12], &baRN_Reader[4], 4);
        VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN, baOutput);
        VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN + 8, baOutput + 8);

        stKeyData.bKeyType = d_VW_AEK;
        stKeyData.bKeyIndex = 0x01;
        memcpy(stKeyData.baEnRND, baOutput, 16);
        ulAPRtn = VW_MutualAuthenticate(&stKeyData);
        if (ulAPRtn == d_NO_ERROR) {
            //Generate Keys
            stKeyData.bKeyType = d_VW_Asession;
            stKeyData.bKeyIndex = 0x01;
            memcpy(&baTmpRN[0], "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC", 16);
            VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN, baOutput);
            VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN + 8, baOutput + 8);
            memcpy(stKeyData.baEnRND, baOutput, 16);
            ulAPRtn = VW_GenerateKeys(&stKeyData);
            if (ulAPRtn != d_NO_ERROR)
                return ulAPRtn;
            else {
                ulAPRtn = VW_SetSessionKey(d_AsessionFlag, "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC");
                if (ulAPRtn != d_NO_ERROR)
                    return ulAPRtn;
            }
        } else
            return ulAPRtn;
    } else
        return ulAPRtn;

    return ulAPRtn;
}


/* ==========================================================================
 * Transaction Related Data include : the all parameter must ASCII string
 *	szAmount    Tag 9F02   (Amount Authorized(Numeric))
 *	szOtherAmt  Tag 9F03   (Amount Other(Numeric))
 *	szTransType Tag 9C     (Transaction Type)
 *	szCatgCode  Tag 9F53   (Transaction Category Code)
 *	szCurrCode  Tag 5F2A   (Transaction Currency Code)
 * ========================================================================== */
ULONG inCTOSS_CLMInitTransaction(BYTE *szAmount,BYTE *szOtherAmt,BYTE *szTransType,BYTE *szCatgCode,BYTE *szCurrCode)
{
	BYTE TagNum;
	BYTE TransaRelatedData[100];
	BYTE index;
	ULONG ulValue;
	BYTE temp[64];
	ULONG ulAPRtn;
	
	// perform transaction : EMVCL_InitTransactionEx + EMVCL_PollTransactionEx
	TagNum = 0;
	index = 0;
	memset(TransaRelatedData, 0, sizeof(TransaRelatedData));

	vdDebug_LogPrintf("szAmount=[%s],szOtherAmt=[%s],szTransType=[%s],szCatgCode=[%s],szCurrCode=[%s]",szAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
	//Put 0x9F02 Amount, Authorized (Numeric)
	vdDebug_LogPrintf("strlen(szAmount)=[%d]",strlen(szAmount));
	if (strlen(szAmount) > 0)
	{
		TagNum ++;
		memset(temp, 0, sizeof(temp));
		wub_str_2_hex(szAmount,temp,12);
		
		TransaRelatedData[index++] = 0x9F;
		TransaRelatedData[index++] = 0x02;
		TransaRelatedData[index++] = 0x06;
		memcpy(&TransaRelatedData[index], temp, 6);
		index += 6;
	}
	//Put 0x9F03 Amount, Amount Other(Numeric)
	vdDebug_LogPrintf("strlen(szOtherAmt)=[%d]",strlen(szOtherAmt));
	if (strlen(szOtherAmt) > 0)
	{
		TagNum ++;
		memset(temp, 0, sizeof(temp));
		wub_str_2_hex(szOtherAmt,temp,12);
		
		TransaRelatedData[index++] = 0x9F;
		TransaRelatedData[index++] = 0x03;
		TransaRelatedData[index++] = 0x06;
		memcpy(&TransaRelatedData[index], temp, 6);
		index += 6;
	}
	//Put 9C
	vdDebug_LogPrintf("strlen(szTransType)=[%d]",strlen(szTransType));
	//if (strlen(szTransType) > 0)
	{
		TagNum ++;
		
		TransaRelatedData[index++] = 0x9C;
		TransaRelatedData[index++] = 0x01;
		TransaRelatedData[index++] = szTransType[0];
	}
	//Put 0x9F53 Transaction Category Code
	vdDebug_LogPrintf("strlen(szCatgCode)=[%d]",strlen(szCatgCode));
	if (strlen(szCatgCode) > 0)
	{
		TagNum ++;
		memset(temp, 0, sizeof(temp));
		wub_str_2_hex(szCatgCode,temp,4);
		
		TransaRelatedData[index++] = 0x9F;
		TransaRelatedData[index++] = 0x53;
		TransaRelatedData[index++] = 0x02;
		memcpy(&TransaRelatedData[index], temp, 2);
		index += 2;
	}
	//Put 0x5F2A Transaction Currency Code
	vdDebug_LogPrintf("strlen(szCurrCode)=[%d]",strlen(szCurrCode));
	if (strlen(szCurrCode) > 0)
	{
		TagNum ++;
		memset(temp, 0, sizeof(temp));
		wub_str_2_hex(szCurrCode,temp,4);
		
		TransaRelatedData[index++] = 0x5F;
		TransaRelatedData[index++] = 0x2A;
		TransaRelatedData[index++] = 0x02;
		memcpy(&TransaRelatedData[index], temp, 2);
		index += 2;
	}

	//Init a CL transaction : send out strat transction request  
	vdDebug_LogPrintf("TagNum=[%d],index=[%d]",TagNum,index);
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = EMVCL_InitTransactionEx(TagNum, TransaRelatedData, index);
	else if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		ulAPRtn = MsessionAuthen();
        if (ulAPRtn != d_NO_ERROR) 
		{
			vdDisplayErrorMsg(1, 8, "Authen Fail");
			return d_NO;
        }
		ulAPRtn = VW_InitTransactionEx(TagNum, TransaRelatedData, index);
	}
	
	if(ulAPRtn != d_EMVCL_NO_ERROR)
	{
		return ulAPRtn;
	}
	
	return d_OK;
		
}

ULONG inCTOSS_CLMPollTransaction(EMVCL_RC_DATA_EX *stRCDataEx,ULONG ulMS)
{
	ULONG ulAPRtn;
	
	// Get Transaction Result
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode()) 
		ulAPRtn = EMVCL_PollTransactionEx(stRCDataEx, ulMS);
	else if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = VW_PollTransactionEx((RC_DATA_EX *)stRCDataEx, ulMS);
	
	return ulAPRtn;
}

ULONG inCTOSS_SetTimeOut(ULONG ulMS)
{
	ULONG ulAPRtn;
	
	ulAPRtn = EMVCL_SetTimeOut(ulMS);	
	return ulAPRtn;
}

ULONG inCTOSS_CLMSetDateTime(BYTE *baDateTime)
{
	ULONG ulAPRtn;
	
	ulAPRtn = EMVCL_SetDateTime(baDateTime);
	if(ulAPRtn != 0)
	{
		vdDebug_LogPrintf("SetDateTime error");
	}
	return ulAPRtn;
}

ULONG inCTOSS_CLMGetDateTime(BYTE *baDateTime)
{
	ULONG ulAPRtn;
	
	ulAPRtn = EMVCL_GetDateTime(baDateTime);
	if(ulAPRtn != 0)
	{
		vdDebug_LogPrintf("SetDateTime error");
	}
	return ulAPRtn;
}

ULONG inCTOSS_CLMListAllCAPKID(BYTE *OutRBuf, UINT *outLen)
{
	ULONG ulAPRtn;
	BYTE baRBuf1[1024];
	UINT uiRLen1;


	vdDebug_LogPrintf("saturn inCTOSS_CLMListAllCAPKID");

	inCTOSS_CLMOpenAndGetVersion();

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = EMVCL_ListAllCAPKID(baRBuf1, &uiRLen1);
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = VW_ListAllCAPKID(baRBuf1, &uiRLen1);
	
	if(ulAPRtn != 0)
	{
		vdDebug_LogPrintf("List CAPK Fail  ");
	}
	else
	{
		DebugAddHEX(" List CAPK", baRBuf1, uiRLen1);
		memcpy(OutRBuf,baRBuf1,uiRLen1);
		*outLen = uiRLen1;
	}
	vdCTOSS_CLMClose();
	return ulAPRtn;
}


ULONG inCTOSS_CLMGetACAPK(char * szAID,int index,EMVCL_CA_PUBLIC_KEY *stCAPubKey)
{
	ULONG ulAPRtn;

	vdDebug_LogPrintf("saturn inCTOSS_CLMGetACAPK");

	inCTOSS_CLMOpenAndGetVersion();

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = EMVCL_GetCAPK(szAID, index, stCAPubKey);
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = VW_GetCAPK(szAID, index, (CA_PUBLIC_KEY *)stCAPubKey);
	
	DebugAddINT("EMVCL_GetCAPK Rtn", ulAPRtn);
	if(ulAPRtn != 0)
	{
		vdDebug_LogPrintf("List CAPK Fail  ");
	}
	else
	{
		DebugAddHEX(" stCAPubKey.szAID", szAID,5);
		DebugAddINT(" stCAPubKey.bAction", stCAPubKey->bAction);
		DebugAddINT(" stCAPubKey.bIndex", stCAPubKey->bIndex);		
		DebugAddHEX(" stCAPubKey.baExponent", stCAPubKey->baExponent, stCAPubKey->uiExponentLen); 	
		DebugAddHEX(" stCAPubKey.baModulus", stCAPubKey->baModulus, stCAPubKey->uiModulusLen);		
	}
	vdCTOSS_CLMClose();
	return ulAPRtn;
}




ULONG inCTOSS_CLMCancelTransaction(void)
{
	ULONG ulAPRtn;

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode()) 
		ulAPRtn = EMVCL_CancelTransaction();
	else if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = VW_CancelTransaction();
	
	return ulAPRtn;
}


void vdCTOSS_CLMAnalyzeTransaction(EMVCL_RC_DATA_EX *stRCData, EMVCL_RC_DATA_ANALYZE *stRCDataAnalyze)
{
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode()) 
		EMVCL_AnalyzeTransactionEx(stRCData, stRCDataAnalyze);
	else if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		VW_AnalyzeTransactionEx((RC_DATA_EX *)stRCData, (RC_DATA_ANALYZE *)stRCDataAnalyze);
	
}


//------------------------------------------------------------------------
//The Tag 0xDF8F30 is a special tag for setting AID
//Format : tag (DF8F30) + len + Value (AID1 Len + AID1 + ASI1 + AID2 Len + AID2 + ASI2 + ......) 
//\x01										(set tag number)
//\xDF\x8F\x30								(tag)
//\x2C										(len)
//\x07\xA0\x00\x00\x00\x04\x10\x10\x00		(data)
//\x07\xA0\x00\x00\x00\x03\x10\x10\x00
//\x07\xA0\x00\x00\x00\x65\x10\x10\x00
//\x06\xA0\x00\x00\x00\x25\x01\x00
//\x07\xA0\x00\x00\x03\x24\x10\x10\x00
#define d_SET_AID	"\x01\xDF\x8F\x30\x2C\x07\xA0\x00\x00\x00\x04\x10\x10\x00\x07\xA0\x00\x00\x00\x03\x10\x10\x00\x07\xA0\x00\x00\x00\x65\x10\x10\x00\x06\xA0\x00\x00\x00\x25\x01\x00\x07\xA0\x00\x00\x03\x24\x10\x10\x00"
int inCTOSS_CLMSetAID(void)
{
	BYTE buf[100];
	UINT len;
	ULONG rtn;
	char *p="\x01\xDF\x8F\x30\x2C";
	char setbuf[100];
	int inlen=0;
	int loop=0;
	int inNumRecord = 0;
	
	
	memset(setbuf,0x00,sizeof(setbuf));
	memcpy(setbuf,p,5);
	inlen += 5;

	inNumRecord = szWave_data.number;
	for (loop = 0;loop<inNumRecord;loop++)
	{		
		DebugAddHEX("szAID", szWave_data.pbAID[loop], szWave_data.inAIDLen[loop]);		
		setbuf[inlen++] = szWave_data.inAIDLen[loop];
		memcpy(setbuf+inlen,szWave_data.pbAID[loop],szWave_data.inAIDLen[loop]);
		inlen += szWave_data.inAIDLen[loop];
		setbuf[inlen++] = 0x00;
	}
	//set the lengh
	setbuf[4] = inlen-5;
//Set AID via EMVCL_SetEMVTags
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		rtn = EMVCL_SetEMVTags(setbuf, inlen, buf, &len);
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_SetEMVTags(setbuf, inlen, buf, &len);

	DebugAddHEX("Set AID Data Resopnse", setbuf, inlen);
	vdDebug_LogPrintf("rtn =[%x],len=[%d].....",rtn,inlen);
	if(rtn != d_EMVCL_NO_ERROR)
	{
		return rtn;
	}
	
//Check Response	
	if(inlen != len)
	{
		return 0xD0000001;
	}
	DebugAddHEX("Set AID Data Resopnse", buf, len);
	if(memcmp(setbuf, buf, len) != 0)
	{
		return 0xD0000002;
	}
	vdDebug_LogPrintf("len=[%d].....",len);
	return 0;
}
//------------------------------------------------------------------------
//Data Format : Tag number (1B) + TLV1 + TLV2 + TLV3 + ....
//\x37									(set tag number)

//EMV data
//\x9F\x1A\x02\x01\x58                      (Tag 9F1A Terminal Country Code)
//\x9F\x1B\x04\x00\x00\x07\xD4              (Tag 9F1B Floor Limit)
//\xDF\x8F\x40\x03\x9F\x08\x02              (Tag DF8F40 Default TDOL)
//\xDF\x8F\x41\x03\x9F\x37\x04              (Tag DF8F41 Default DDOL)
//\xDF\x8F\x42\x01\x00                      (Tag DF8F42 Target Percent)
//\xDF\x8F\x43\x01\x00                      (Tag DF8F43 Max Target Percent)
//\xDF\x8F\x44\x04\x00\x00\x03\xE8          (Tag DF8F44 Threshold Value)
//\xDF\x8F\x4F\x01\x01                      (Tag DF8F4F Transaction Result) 
//\x9F\x1E\x08\x31\x32\x33\x34\x35\x36\x37\x38  (Tag 9F1E IFD Serial Number)
//\x9F\x15\x02\x01\x01                      (Tag 9F15 Merchant Category Code)
//\x9F\x16\x00                              (Tag 9F16 Merchant Identifier)
//\x9F\x4E\x00                              (Tag 9F4E Merchart Name and Location)
//\x9F\x1C\x00                              (Tag 9F1C Terminal Identifier)
//\x9F\x33\x03\x00\x08\xC8					(Tag 9F33 Terminal Capability)

//visa data
//\x9F\x66\x04\x20\x00\x00\x00              (Tag 9F66 TTQ)
//\xDF\x00\x06\x00\x00\x00\x00\x30\x00      (Tag DF00 Visa CL Trans Limit)
//\xDF\x01\x06\x00\x00\x00\x00\x22\x00      (Tag DF01 Visa CL CVM Limit)
//\xDF\x02\x06\x00\x00\x00\x00\x27\x00      (Tag DF02 Visa CL Floor Limit)
//\xDF\x25\x01\xFF                          (Tag DF25 Visa Status Check)
//\xDF\x8F\x4B\x01\x00                      (Tag DF8F4B Visa Amount 0 Check)
//\xDF\x05\x01\x01                          (Tag DF05 Display Offline Funds Indicator)
//\xDF\x06\x01\x01                          (Tag DF06 Visa MSD CVN17 Enable Indicator)
//\xDF\x21\x01\x01                          (Tag DF21 Visa MSD Track1 Enable Indicator)
//\xDF\x22\x01\x01                          (Tag DF22 Visa MSD Track2 Enable Indicator)

//paypass data
//\x5F\x57\x00                              (Tag 5F57 Account Type)
//\x9F\x01\x00                              (Tag 9F01 Acquirer Identifier)
//\x9F\x09\x02\x00\x02                      (Tag 9F09 Application Version Number)
//\xDF\x81\x17\x01\x00                      (Tag DF8117 Card Data Input Capability)
//\xDF\x81\x18\x01\x60                      (Tag DF8118 CVM Capability-CVM Required)
//\xDF\x81\x19\x01\x08                      (Tag DF8119 CVM Capability-NO CVM Required)
//\xDF\x81\x1A\x03\x9F\x6A\x04              (Tag DF811A Default UDOL)
//\xDF\x81\x1B\x01\x20                      (Tag DF811B Kernel Configuration)
//\xDF\x81\x0C\x01\x02                      (Tag DF810C Kernel ID)
//\x9F\x6D\x02\x00\x01                      (Tag 9F6D Mag-stripe Application Version Number)
//\xDF\x81\x1E\x01\x10                      (Tag DF811E Mag-stripe CVM Capability-CVM Required)
//\xDF\x81\x2C\x01\x00                      (Tag DF812C Mag-stripe CVM Capability-NO CVM Required)
//\xDF\x81\x1C\x02\x00\x00                  (Tag DF811C Max Lifetime of Torn Trans Log Record)
//\xDF\x81\x1D\x01\x00                      (Tag DF811D Max Number of Torn Trans Log Record)
//\x9F\x7E\x00                              (Tag 9F7E Mobile Support Indicator)
//\xDF\x81\x23\x06\x00\x00\x00\x01\x00\x00  (Tag DF8123 MC CL Floor Limit)
//\xDF\x81\x24\x06\x00\x00\x00\x03\x00\x00  (Tag DF8124 MC CL Trans Limit-No on device CVM)
//\xDF\x81\x25\x06\x00\x00\x00\x05\x00\x00  (Tag DF8125 MC CL Trans Limit-on device CVM)
//\xDF\x81\x26\x06\x00\x00\x00\x00\x10\x00  (Tag DF8126 MC CL CVM Limit)
//\xDF\x81\x1F\x01\x08                      (Tag DF811F Security Capability)
//\xDF\x81\x20\x05\x00\x00\x00\x00\x00      (Tag DF8120 TAC-Default)
//\xDF\x81\x21\x05\x00\x00\x00\x00\x00      (Tag DF8121 TAC-Denial)
//\xDF\x81\x22\x05\x00\x00\x00\x00\x00      (Tag DF8122 TAC-Online)


//AE data
//\xDF\x51\x06\x00\x00\x00\x00\x00\x3C      (Tag DF51)
//\xDF\x57\x01\x80                          (Tag DF57)
//\xDF\x58\x01\x01                          (Tag DF58)
//\xDF\x5B\x02\x00\x01                      (Tag DF5B)
//\xDF\x50\x06\x00\x00\x00\x30\x00\x00      (Tag DF50)
//\xDF\x53\x06\x00\x00\x00\x20\x00\x00      (Tag DF53)
//\xDF\x52\x06\x00\x00\x00\x25\x00\x00      (Tag DF52)

//overwrite tag
// visa :  9F33030008C8 , 9F40056F00002001, 9F350100
// MC   :  9F33030008C8 , 9F40050000000000, 9F350122
//DF8F4D\x4C\x05\xA0\x00\x00\x00\x03\x12\x9F\x33\x03\x00\x08\xC8\x9F\x40\x05\x6F\x00\x00\x20\x01\x9F\x35\x01\x00
//          \x05\xA0\x00\x00\x00\x04\x12\x9F\x33\x03\x00\x08\xC8\x9F\x40\x05\x00\x00\x00\x00\x00\x9F\x35\x01\x22
//          \x06\xA0\x00\x00\x00\x25\x01\x12\x9F\x33\x03\x00\x08\xC8\x9F\x40\x05\x00\x00\x00\x00\x00\x9F\x35\x01\x22

#define d_SET_TAG "\x37\x9F\x1A\x02\x07\x02\x9F\x1B\x04\x00\x00\x07\xD4\xDF\x8F\x40\x03\x9F\x08\x02\xDF\x8F\x41\x03\x9F\x37\x04\xDF\x8F\x42\x01\x00\xDF\x8F\x43\x01\x00\xDF\x8F\x44\x04\x00\x00\x03\xE8\xDF\x8F\x4F\x01\x01\x9F\x1E\x08\x31\x32\x33\x34\x35\x36\x37\x38\x9F\x15\x02\x01\x01\x9F\x16\x00\x9F\x4E\x00\x9F\x1C\x00\x9F\x33\x03\x00\x08\xC8\x9F\x66\x04\x20\x00\x00\x00\xDF\x00\x06\x00\x00\x00\x00\x30\x00\xDF\x01\x06\x00\x00\x00\x00\x22\x00\xDF\x02\x06\x00\x00\x00\x00\x27\x00\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x05\x01\x01\xDF\x06\x01\x01\xDF\x21\x01\x01\xDF\x22\x01\x01\x5F\x57\x00\x9F\x01\x00\x9F\x09\x02\x00\x02\xDF\x81\x17\x01\x00\xDF\x81\x18\x01\x60\xDF\x81\x19\x01\x08\xDF\x81\x1A\x03\x9F\x6A\x04\xDF\x81\x1B\x01\x20\xDF\x81\x0C\x01\x02\x9F\x6D\x02\x00\x01\xDF\x81\x1E\x01\x10\xDF\x81\x2C\x01\x00\xDF\x81\x1C\x02\x00\x00\xDF\x81\x1D\x01\x00\x9F\x7E\x00\xDF\x81\x23\x06\x00\x00\x00\x01\x00\x00\xDF\x81\x24\x06\x00\x00\x00\x03\x00\x00\xDF\x81\x25\x06\x00\x00\x00\x05\x00\x00\xDF\x81\x26\x06\x00\x00\x00\x00\x10\x00\xDF\x81\x1F\x01\x08\xDF\x81\x20\x05\x00\x00\x00\x00\x00\xDF\x81\x21\x05\x00\x00\x00\x00\x00\xDF\x81\x22\x05\x00\x00\x00\x00\x00\xDF\x51\x06\x00\x00\x00\x00\x00\x3C\xDF\x57\x01\x80\xDF\x58\x01\x01\xDF\x5B\x02\x00\x01\xDF\x50\x06\x00\x00\x00\x30\x00\x00\xDF\x53\x06\x00\x00\x00\x20\x00\x00\xDF\x52\x06\x00\x00\x00\x25\x00\x00\xDF\x8F\x4D\x4C\x05\xA0\x00\x00\x00\x03\x12\x9F\x33\x03\x00\x08\xC8\x9F\x40\x05\x6F\x00\x00\x20\x01\x9F\x35\x01\x00\x05\xA0\x00\x00\x00\x04\x12\x9F\x33\x03\x00\x08\xC8\x9F\x40\x05\x00\x00\x00\x00\x00\x9F\x35\x01\x22\x06\xA0\x00\x00\x00\x25\x01\x12\x9F\x33\x03\x00\x08\xC8\x9F\x40\x05\x00\x00\x00\x00\x00\x9F\x35\x01\x22"

int inCTOSS_Set2lentag(char *p,char *tag,int len, char *buf)
{
	int inlen = 0;
	
	memcpy(p+inlen,tag,2);//tag
	inlen += 2;//tag len

	if (len == 0)
	{
		p[inlen++] = 0x00;
	}
	else
	{
		p[inlen++] = len;//value len
		memcpy(p+inlen,buf,len);//value
		inlen += len;//value len
	}
	DebugAddHEX("Set Tag Data", p, inlen);
	vdDebug_LogPrintf("inlen=[%d]....",inlen);
	return inlen;
}

int inCTOSS_Set3lentag(char *p,char *tag,int len, char *buf)
{
	int inlen = 0;
	
	memcpy(p+inlen,tag,3);//tag
	inlen += 3;//tag len

	if (len == 0)
	{
		p[inlen++] = 0x00;
	}
	else
	{
		p[inlen++] = len;//value len
		memcpy(p+inlen,buf,len);//value
		inlen += len;//value len
	}
	DebugAddHEX("Set Tag Data", p, inlen);
	vdDebug_LogPrintf("inlen=[%d]....",inlen);
	return inlen;
}

int inCTOSS_SetOverwritetag(char *p,char *tag,int len, char *buf)
{
	int inlen = 0;
	char inbuf[100];
	
	memcpy(p+inlen,tag,3);//tag
	inlen += 3;//tag len

	if (len == 0)
	{
		p[inlen++] = 0x00;
	}
	else
	{
		p[inlen++] = len;//value len
		memcpy(p+inlen,buf,len);//value
		inlen += len;//value len
	}
	DebugAddHEX("Set Tag Data", p, inlen);
	vdDebug_LogPrintf("inlen=[%d]....",inlen);
	return inlen;
}




int inCTOSS_CLMSetEMVTag(STRUCT_WAVE_EMVT WaveEMVT[], int inNumRecord)
{
	BYTE buf[1024];
	BYTE inbuf[1024];
	char tembuf[50];
	UINT len;
	ULONG rtn;
	char *p;
	int inlen=0;
	int tagnumber=0;
	
	memset(inbuf,0x00,sizeof(inbuf));
	p = inbuf;

	inlen = 1;

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x1A",2,WaveEMVT[0].szEMVTermCountryCode);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x1B",4,WaveEMVT[0].lnEMVFloorLimit);

	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x8F\x40",3, WaveEMVT[0].szDefaultTDOL);

	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x8F\x41",3, WaveEMVT[0].szDefaultDDOL);

	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x8F\x42",1, WaveEMVT[0].inEMVTargetRSPercent);

	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x8F\x43",1, WaveEMVT[0].inEMVMaxTargetRSPercent);

	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x8F\x44",4, WaveEMVT[0].lnEMVRSThreshold);

	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x8F\x4F",1, WaveEMVT[0].szTransactionResult);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x1E",8,WaveEMVT[0].szIFDSerialNumber);

	tagnumber++;
	memset(tembuf,0x00,sizeof(tembuf));
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x15",0,tembuf);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x16",15,WaveEMVT[0].szMerchantIdentifier);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x4E",0,tembuf);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x1C",8,WaveEMVT[0].szTerminalIdentification);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x33",3,WaveEMVT[0].szEMVTermCapabilities);


	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x35",1,"\x22");

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x09",2,"\x00\x8C");

//visa data
	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x66",4,WaveEMVT[0].szVisaWaveTTQ);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\xDF\x00",6,WaveEMVT[0].szCLTranslimit);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\xDF\x01",6,WaveEMVT[0].szCLCVMlimit);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\xDF\x02",6,WaveEMVT[0].szCLFloorlimit);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\xDF\x25",1,WaveEMVT[0].szStatusCheck);
	
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x8F\x4B",1, WaveEMVT[0].szAmount0Check);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\xDF\x05",1,WaveEMVT[0].inDisplayOfflineIndicator);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\xDF\x06",1,WaveEMVT[0].szVisMSDCVN17Enable);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\xDF\x21",1,WaveEMVT[0].inVisaMSDTrack1Enable);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\xDF\x22",1,WaveEMVT[0].inVisaMSDTrack2Enable);

	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x5F\x57",0,tembuf);
	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x01",0,tembuf);
//	tagnumber++;
//	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x09",2,"\x00\x02");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x17",1, "\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x18",1, "\x60");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x19",1, "\x08");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x1A",3, "\x9F\x6A\x04");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x1B",1, "\x20");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x0C",1, "\x02");
	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x6D",2,"\x00\x01");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x1E",1, "\x10");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x2C",1, "\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x1C",2, "\x00\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x1D",1, "\x00");
	tagnumber++;
	inlen += inCTOSS_Set2lentag(p+inlen,"\x9F\x7E",0,tembuf);
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x23",6, "\x00\x00\x00\x01\x00\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x25",6, "\x00\x00\x00\x05\x00\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x24",6, "\x00\x00\x00\x03\x00\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x26",6, "\x00\x00\x00\x00\x10\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x1F",1, "\x00\x00\x00\x00\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x20",5, "\x00\x00\x00\x00\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x21",5, "\x00\x00\x00\x00\x00");
	tagnumber++;
	inlen += inCTOSS_Set3lentag(p+inlen,"\xDF\x81\x22",5, "\x00\x00\x00\x00\x00");

	p[0] = tagnumber;
//Set AID via EMVCL_SetEMVTags
//	rtn = EMVCL_SetEMVTags(d_SET_TAG, sizeof(d_SET_TAG)-1, buf, &len);
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		rtn = EMVCL_SetEMVTags(inbuf, inlen, buf, &len);
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_SetEMVTags(inbuf, inlen, buf, &len);

	DebugAddHEX("Set Tag Data", inbuf, inlen);
	vdDebug_LogPrintf("rtn =[%x].inlen=[%d].len=[%d]...",rtn,inlen,len);
	if(rtn != d_EMVCL_NO_ERROR)
	{
		return rtn;
	}
	
//Check Response	
//	if(sizeof(d_SET_TAG)-1 != len)
	if(inlen != len)
	{
		return 0xD0000001;
	}
	DebugAddHEX("Set Tag Data Resopnse", buf, len);
//	if(memcmp(d_SET_TAG, buf, len) != 0)
	if(memcmp(inbuf, buf, len) != 0)
	{
		return 0xD0000002;
	}
	vdDebug_LogPrintf("len=[%d].....",len);
	return 0;
}


int inCTOSS_CLMGetEMVTag(BYTE *OutRBuf, UINT *outLen)
{
	BYTE baRBuf1[1024];
	UINT uiRLen1;
	int rtn;

	//Get All Tag
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		rtn = EMVCL_GetEMVTags(1, "\xFF", 1, baRBuf1, &uiRLen1);
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_GetEMVTags(1, "\xFF", 1, baRBuf1, &uiRLen1);
	
	if(rtn != 0)
	{
		vdDebug_LogPrintf("Get Tag Fail");
	}
	else
	{
		DebugAddHEX("Get Tag", baRBuf1, uiRLen1);
		memcpy(OutRBuf,baRBuf1,uiRLen1);
		*outLen = uiRLen1;
	}

	return rtn;
}



int inCTOSS_CLMVisaSetCapability(STRUCT_WAVE_EMVT WaveEMVT[], int inNumRecord)
{
	int flag=0;
	ULONG rtn;
	int inIDlen=0,inAclen=0;
	int loop=0;
	EMVCL_SCHEME_DATA stScheme,stRsp;
	char tmpbuf[20+1];
	
	memset(&stScheme,0x00,sizeof(EMVCL_SCHEME_DATA));
	memset(tmpbuf,0x00,sizeof(tmpbuf));

	for (loop = 0;loop<inNumRecord;loop++)
	{
//		if (1 == WaveEMVT[loop].fEVDEnable)
		{	
			flag++;
			vdDebug_LogPrintf("szSchemeID =[%s].szActivate=[%d]...",WaveEMVT[loop].szSchemeID,WaveEMVT[loop].szActivate);
			strcat(tmpbuf,WaveEMVT[loop].szSchemeID);
			stScheme.baAction[inAclen++] = WaveEMVT[loop].szActivate;
		}
	}
	stScheme.bNoS = flag;
	inIDlen = strlen(tmpbuf);
	wub_str_2_hex(tmpbuf,stScheme.baID,inIDlen);
	
	vdDebug_LogPrintf("bNoS =[%x].....",stScheme.bNoS);
	DebugAddHEX("stCAPubKey.baModulus", stScheme.baID, 5);
	DebugAddHEX("stCAPubKey.baExponent", stScheme.baAction, 5);

//android-removed
#if 0
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_SetCapability((SCHEME_DATA *)&stScheme, (SCHEME_DATA *)&stRsp);
#endif
//end	
	vdDebug_LogPrintf("rtn =[%x].....",rtn);
	if(rtn != d_EMVCL_NO_ERROR)
	{
		vdDebug_LogPrintf("Set Capability Fail");
		return rtn;
	}
	return 0;
}


int inCTOSS_CLMVisaGetCapability(EMVCL_SCHEME_DATA *stScheme)
{
	ULONG rtn;
	int i;

	stScheme->bNoS = 0xFF;

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		rtn = EMVCL_VisaGetCapability(stScheme);
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_GetCapability((SCHEME_DATA *)stScheme);
	
	vdDebug_LogPrintf("Get Capability [%x]",rtn);
	//If the response code is 0xA0000004, the reader provides the scheme information below
	if(rtn != 0xA0000004)
	{
		vdDebug_LogPrintf("Get Capability Fail");
	}
	else
	{
		DebugAddINT(" stScheme.bNoS", stScheme->bNoS);
		for(i=0;i<stScheme->bNoS;i++)
		{
			DebugAddINT(" ID", stScheme->baID[i]);
			DebugAddINT(" Active", stScheme->baAction[i]);
		}
	}

	return rtn;
}



void vdCTOSS_CLMSetAIDAndEMVTag(void)
{
	int inNumRecord  = 0;
	int inLoop = 0;
	int index = 0;
	STRUCT_WAVE_EMVT WaveEMVT[10];
	BYTE baBuf[16+1];
	ULONG rtn;
	
	inNumRecord = szWave_data.number; 

	memset(WaveEMVT,0x00,sizeof(STRUCT_WAVE_EMVT)*10);
	
	vdDebug_LogPrintf("inWaveAIDNumRecord=[%d]", inNumRecord); 
	for (inLoop = 0; inLoop < inNumRecord; inLoop ++)
	{
		memset(&strWaveEMVT,0x00,sizeof(STRUCT_WAVE_EMVT));
		inWaveEMVRead(szWave_data.EMVid[inLoop]);

		memcpy(&WaveEMVT[inLoop],&strWaveEMVT,sizeof(STRUCT_WAVE_EMVT));
	}

	if (inNumRecord > 0)
	{
	    
		vdDebug_LogPrintf("saturn vdCTOSS_CLMSetAIDAndEMVTag");
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
			
			rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_ADMIN,baBuf);
			vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,baBuf);
		}
		
		inCTOSS_CLMSetAID();
		inCTOSS_CLMSetEMVTag(WaveEMVT,inNumRecord);
		inCTOSS_CLMVisaSetCapability(WaveEMVT,inNumRecord);

		if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_NORMAL,baBuf);
		vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,baBuf);
		
		vdCTOSS_CLMClose();
	}
	
}


void vdCTOSS_CLMSetALLCAPK(void)
{
	int inNumRecord  = 0;
	int inLoop = 0;
	int index = 0;
	int number = 0;
	
	inNumRecord = inWaveAIDNumRecord(); 
	
	vdDebug_LogPrintf("inWaveAIDNumRecord=[%d]", inNumRecord);
	memset(&szWave_data,0x00,sizeof(STRUCT_WAVE_DATA));

	
	vdDebug_LogPrintf("saturn vdCTOSS_CLMSetALLCAPK");
	inCTOSS_CLMOpenAndGetVersion();
	for (inLoop = 1; inLoop <= inNumRecord; inLoop ++)
	{
		number = 0;
		memset(&strWaveAIDT,0x00,sizeof(STRUCT_WAVE_AIDT));
		inWaveAIDRead(inLoop);

		if (1 ==strWaveAIDT.fAIDEnable)
		{
			number = szWave_data.number;
			vdDebug_LogPrintf("number=[%d]", number);
			if (number > 0)
			{
				for (index = 0; index < number;index++)
				{
					vdDebug_LogPrintf("number=[%d],AID=[%d]=[%d]------------", number,strWaveAIDT.EMVid,szWave_data.EMVid[index]);
					if (szWave_data.EMVid[index] == strWaveAIDT.EMVid)
						break;
				}
			}

			vdDebug_LogPrintf("number=[%d],index=[%d]", number,index);
			if (number == index)
			{
				if (number < MAX_AID_NO)
				{
					szWave_data.EMVid[number] = strWaveAIDT.EMVid;
					memcpy(szWave_data.pbAID[number],strWaveAIDT.pbAID,strWaveAIDT.inAIDLen);
					szWave_data.inAIDLen[number] = strWaveAIDT.inAIDLen;
					szWave_data.number++;
					
					DebugAddHEX("szAID", szWave_data.pbAID[number], szWave_data.inAIDLen[number]);		
					vdDebug_LogPrintf("number=[%d],szWave_data.number[%d]=EMVid[%d]------------", number,szWave_data.number,szWave_data.EMVid[number]);
				}
			}
		}
		
		if (strWaveAIDT.inCAPKindex1 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex1,strWaveAIDT.pbCAPKExponent1);
		}
		if (strWaveAIDT.inCAPKindex2 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex2,strWaveAIDT.pbCAPKExponent2);
		}
		if (strWaveAIDT.inCAPKindex3 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex3,strWaveAIDT.pbCAPKExponent3);
		}
		if (strWaveAIDT.inCAPKindex4 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex4,strWaveAIDT.pbCAPKExponent4);
		}
		if (strWaveAIDT.inCAPKindex5 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex5,strWaveAIDT.pbCAPKExponent5);
		}
		if (strWaveAIDT.inCAPKindex6 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex6,strWaveAIDT.pbCAPKExponent6);
		}
		if (strWaveAIDT.inCAPKindex7 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex7,strWaveAIDT.pbCAPKExponent7);
		}
		if (strWaveAIDT.inCAPKindex8 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex8,strWaveAIDT.pbCAPKExponent8);
		}
		if (strWaveAIDT.inCAPKindex9 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex9,strWaveAIDT.pbCAPKExponent9);
		}
		if (strWaveAIDT.inCAPKindex10 > 0)
		{
			vdCTOSS_CLMSetACAPK(strWaveAIDT.pbAID,strWaveAIDT.inAIDLen,strWaveAIDT.inCAPKindex10,strWaveAIDT.pbCAPKExponent10);
		}
	}

	vdCTOSS_CLMClose();
}

int inCTOSS_CLMReadACAPK(char * szAID,int inAIDLen, int index,char *outbuf,char *exponent,int *exp_len)
{
	int     uintRet;
	char szFileName[50+1];
    BYTE    bStorageType = d_STORAGE_FLASH;
	ULONG   iMod;
	BYTE    tmp[1024];
	char	szStrAID[20];
	FILE  *fPubKey;
	int iExp;

    memset(szFileName, 0x00, sizeof(szFileName));
	memset(szStrAID, 0x00, sizeof(szStrAID));
	wub_hex_2_str(szAID,szStrAID,inAIDLen);
		
    sprintf(szFileName,"%s%s.%02x", PUBLIC_PATH,szStrAID,index);
	vdDebug_LogPrintf("szBmpfile=[%s]szAID=[%s],index=[%d]",szFileName,szStrAID,index);

	
	fPubKey = fopen( (char*)szFileName, "rb" );
	if (fPubKey == NULL)
		return CTOS_RET_PARAM;

	uintRet = fread( tmp, 1, sizeof(tmp), fPubKey );
	fclose(fPubKey);  
	vdDebug_LogPrintf("CAPK=Len[%d]==[%s]",uintRet,tmp);
	
	if(uintRet >0)
	{
		iMod=(int)(tmp[0]-0x30)*100+(tmp[1]-0x30)*10+(tmp[2]-0x30);		
		vdDebug_LogPrintf("iMod===[%d]",iMod);
		if(iMod%8 != 0)
			return(CTOS_RET_PARAM);

		if(iMod > CTOS_PED_RSA_MAX)
			return(CTOS_RET_PARAM);

		wub_str_2_hex((tmp+3), outbuf, iMod*2);		
		
		iExp=(int)tmp[iMod*2+4] - 0x30;
		wub_str_2_hex((&tmp[5+(iMod*2)]), (exponent), iExp*2);

		vdDebug_LogPrintf("iExp===[%d]",iExp);		
		DebugAddHEX("Exponent HEX string===", exponent, iExp);

		*exp_len = iExp;

	}
	else
		return(CTOS_RET_CALC_FAILED);

	return(iMod);

}

/***************************************************
#define d_CAPK_RID	"\xA0\x00\x00\x00\x03"
#define d_CAPK_INDEX	0xF6
#define d_CAPK_M	"\xA2\x5A\x6B\xD7\x83\xA5\xEF\x6B\x8F\xB6\xE8\x30\x55\xC2\x60\xF5\xF9\x9E\xA1\x66\x78\xF3\xB9\x05\x3E\x0F\x64\x38\xE8\x2C\x3F\x5D\x1E\x8C\x38\xF1\x35\x88\x01\x7E\x2B\x12\xB3\xD8\xFF\x6F\x50\x16\x7F\x46\x44\x29\x30\x72\x9E\x9E\x4D\x1B\x37\x39\xE5\x06\x7C\x0A\xC7\xA1\xF4\x48\x7A\x35\xF6\x75\xBC\x16\xE2\x33\x31\x51\x65\xC3\x14\x2B\xFD\xB2\x5E\x30\x1A\x63\x2A\x54\xA3\x37\x1E\xBA\xB6\x57\x2D\xEE\xBA\xF3\x70\xF3\x37\xF0\x57\xEE\x73\xB4\xAE\x46\xD1\xA8\xBC\x4D\xA8\x53\xEC\x3C\xC1\x2C\x8C\xBC\x2D\xA1\x83\x22\xD6\x85\x30\xC7\x0B\x22\xBD\xAC\x35\x1D\xD3\x60\x68\xAE\x32\x1E\x11\xAB\xF2\x64\xF4\xD3\x56\x9B\xB7\x12\x14\x54\x50\x05\x55\x8D\xE2\x60\x83\xC7\x35\xDB\x77\x63\x68\x17\x2F\x38\xC2\xF5\xC8\x5E\x8B\x5B\x89\x0C\xC6\x82\x91\x1D\x2D\xE7\x1F\xA6\x26\xB8\x81\x7F\xCC\xC0\x89\x22\xB7\x03\x86\x9F\x3B\xAE\xAC\x14\x59\xD7\x7C\xD8\x53\x76\xBC\x36\x18\x2F\x42\x38\x31\x4D\x6C\x42\x12\xFC\xDD\x7F\x23\xD3"
#define d_CAPK_E	"\x03"
*****************************************/

void vdCTOSS_CLMSetACAPK(char * szAID, int inAIDLen, int index, char *szCAPKExponent)
{
	EMVCL_CA_PUBLIC_KEY stCAPubKey;
	ULONG rtn;
	BYTE outbuf[1024];
	char szHexExponent[10];
	int inlenExponent;
	int inlenM;
	SHA_CTX sha;
	
//Set CAPK via EMVCL_SetCAPK function
	
	//bAction = 0x00 -> Add, bAction = 0x01 -> Del
	stCAPubKey.bAction = 0x00;
	//CAPK Index
	stCAPubKey.bIndex = index;
	//Modulus
	inlenM = inCTOSS_CLMReadACAPK(szAID,inAIDLen,index,outbuf,szHexExponent,&inlenExponent);
	if (inlenM < 0)
	{
		vdDebug_LogPrintf("Read CAPK file error..");
		return ;
	}
	stCAPubKey.uiModulusLen = inlenM;
	memcpy(stCAPubKey.baModulus, outbuf, stCAPubKey.uiModulusLen);

	//Exponent
	stCAPubKey.uiExponentLen = inlenExponent;
	memcpy(stCAPubKey.baExponent, szHexExponent, stCAPubKey.uiExponentLen);

	//HAsh
	CTOS_SHA1Init (&sha);
	CTOS_SHA1Update (&sha, szAID, inAIDLen);
	CTOS_SHA1Update (&sha, &stCAPubKey.bIndex, 1);
	CTOS_SHA1Update (&sha, stCAPubKey.baModulus, stCAPubKey.uiModulusLen);
	CTOS_SHA1Update (&sha, stCAPubKey.baExponent, stCAPubKey.uiExponentLen);
	CTOS_SHA1Final (stCAPubKey.baHash, &sha);

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		rtn = EMVCL_SetCAPK(szAID, &stCAPubKey);
	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_SetCAPK(szAID,(CA_PUBLIC_KEY *) &stCAPubKey);
	
	vdDebug_LogPrintf("EMVCL_SetCAPK=[%x].inAIDLen=[%d][%d][%d]",rtn,inAIDLen,stCAPubKey.uiModulusLen,stCAPubKey.uiExponentLen);
	DebugAddHEX("stCAPubKey.szAID", szAID, inAIDLen);
	DebugAddHEX("stCAPubKey.bIndex", &stCAPubKey.bIndex, 1);
	DebugAddHEX("stCAPubKey.baModulus", stCAPubKey.baModulus, stCAPubKey.uiModulusLen);
	DebugAddHEX("stCAPubKey.baExponent", stCAPubKey.baExponent, stCAPubKey.uiExponentLen);
	DebugAddHEX("stCAPubKey.baHash", stCAPubKey.baHash, 20);

}

void vdCTOSS_WaveGetEMVData(BYTE *baChipData, USHORT usChipDataLen)
{
	USHORT usLen = 64;
	BYTE szHexT57[100],szT57[200];        // Terminal Verification Results

	memset(szHexT57,0x00,sizeof(szHexT57));
	memset(szT57,0x00,sizeof(szT57));
	usCTOSS_FindTagFromDataPackage(TAG_57, szHexT57, &usLen, baChipData, usChipDataLen);
	if (usLen > 0)
	{
		wub_hex_2_str(szHexT57,szT57,usLen);
		vdAnalysisTrack2(szT57, usLen*2);
	}
	
	usCTOSS_FindTagFromDataPackage(TAG_9F02_AUTH_AMOUNT, srTransRec.stEMVinfo.T9F02, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F03_OTHER_AMOUNT, srTransRec.stEMVinfo.T9F03, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_82_AIP, srTransRec.stEMVinfo.T82, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_5F2A_TRANS_CURRENCY_CODE, srTransRec.stEMVinfo.T5F2A, &usLen, baChipData, usChipDataLen);
			
	usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen,  baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9A_TRANS_DATE, srTransRec.stEMVinfo.T9A, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9C_TRANS_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9C), &usLen,  baChipData, usChipDataLen);
		
	usCTOSS_FindTagFromDataPackage(TAG_9F06, srTransRec.stEMVinfo.T9F06, &usLen,  baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen,  baChipData, usChipDataLen);

	if (srTransRec.stEMVinfo.T9F10_len <= 0)
	{
		usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen,  baChipData, usChipDataLen);
		srTransRec.stEMVinfo.T9F10_len = usLen;
	}
	vdDebug_LogPrintf("T9F10_len =[%d]-----",srTransRec.stEMVinfo.T9F10_len);

	usCTOSS_FindTagFromDataPackage(TAG_9F1A_TERM_COUNTRY_CODE, srTransRec.stEMVinfo.T9F1A, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F1E, srTransRec.stEMVinfo.T9F1E, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen,  baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen,  baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F33_TERM_CAB, srTransRec.stEMVinfo.T9F33, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F35_TERM_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9F35), &usLen,  baChipData, usChipDataLen);

	if (srTransRec.stEMVinfo.T9F36_len <= 0)
	{
		usCTOSS_FindTagFromDataPackage(TAG_9F36_ATC, srTransRec.stEMVinfo.T9F36, &usLen,  baChipData, usChipDataLen);
		srTransRec.stEMVinfo.T9F36_len = usLen;
	}

	usCTOSS_FindTagFromDataPackage(TAG_9F37_UNPREDICT_NUM, srTransRec.stEMVinfo.T9F37, &usLen,  baChipData, usChipDataLen);
	
    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, baChipData, usChipDataLen);
            
    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, baChipData, usChipDataLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, baChipData, usChipDataLen);
        
    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, baChipData, usChipDataLen);
}


void vdCTOSS_WaveGetCardLable(BYTE SID)
{
	char msg[50+1];
	int len;
	memset(msg,0x00,sizeof(msg));
	switch(SID) 
	{
		case d_VW_SID_PAYPASS_MAG_STRIPE:
			strcpy(msg,"PayPass M-Stripe");
			break;
		case d_VW_SID_PAYPASS_MCHIP:
            strcpy(msg," PayPass MChip  ");
			break;
		case d_VW_SID_VISA_OLD_US:
            strcpy(msg," VISA Old US MSD");
			break;
		case d_VW_SID_VISA_WAVE_MSD:
            strcpy(msg," VisaWave MSD   ");
			break;
		case d_VW_SID_VISA_WAVE_2:
            strcpy(msg," VisaWave 2     ");
			break;
		case d_VW_SID_VISA_WAVE_QVSDC:
            strcpy(msg," VisaWave qVSDC ");
			break;
		case d_VW_SID_AE_EMV:
            strcpy(msg," ExpressPay EMV ");
			break;
		case d_VW_SID_AE_MAG_STRIPE:
            strcpy(msg," AE M-Stripe    ");
			break;
		case d_VW_SID_JCB_WAVE_2:
            strcpy(msg," J/Speedy Wave 2");
			break;
		case d_VW_SID_JCB_WAVE_QVSDC:
            strcpy(msg," J/Speedy qVSDC ");
			break;
		case d_VW_SID_DISCOVER:
            strcpy(msg," Zip            ");
			break;
		default:
            strcpy(msg,"CardType No Def.");
			break;
	}

	len = strlen(msg);
	if (len > 20)
		len = 20;
	
	strncpy(srTransRec.szCardLable, msg,len);
	
}

int inCTOSS_WaveAnalyzeTransaction(EMVCL_RC_DATA_EX *stRCDataEx)
{
	ULONG ulAPRtn;
	BYTE cVer[128];
	int inRet = d_NO;
	USHORT usLen = 64;

//	TLVDataParse(stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
//	TLVDataParse(stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);

	DebugAddHEX("SCDataEx DateTime", stRCDataEx->baDateTime, sizeof(stRCDataEx->baDateTime));
	DebugAddHEX("SCDataEx Track1", stRCDataEx->baTrack1Data, stRCDataEx->bTrack1Len);
	DebugAddHEX("SCDataEx Track2", stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);
	DebugAddHEX("SCDataEx Chip", stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
	DebugAddHEX("SCDataEx Additional", stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);
	
	vdCTOSS_CLMAnalyzeTransaction(stRCDataEx, &stRCDataAnalyze);

	if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_SIGNATURE)
	{
		vdDebug_LogPrintf("CVM->Signature REQUIRED");
		//add signature funcation
	} 
	else if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_ONLPIN)
	{
		vdDebug_LogPrintf("CVM->ONLPIN REQUIRED");
		//add online pin funcation
	} 
	else if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_NOCVM)
	{
		vdDebug_LogPrintf("No CVM REQUIRED");
	}

	
	//Online
	if(stRCDataAnalyze.usTransResult == d_EMV_PAYPASS_MSD_ONLINE || stRCDataAnalyze.usTransResult == d_EMV_CHIP_ONL || stRCDataAnalyze.usTransResult == d_EMV_VISA_MSD_ONLINE) 
	{
		vdDebug_LogPrintf("Go Online...  ");
		srTransRec.byUploaded = CN_TRUE;
        srTransRec.byOffline = CN_FALSE;
		inRet = d_OK;
	}
	//Offline Approval
	else if(stRCDataAnalyze.usTransResult == d_EMV_CHIP_OFF_APPROVAL) 
	{
		vdDebug_LogPrintf("Off Approval...  ");
		//ulAPRtn = EMVCL_SetLED(0x0F, 0x02);
		//srTransRec.byUploaded = CN_FALSE;
        //srTransRec.byOffline = CN_TRUE;
		//inRet = d_OK;

		vdDisplayErrorMsg(1, 8, "EMV Decline");
		CTOS_Beep();
		inRet = d_NO;
	}
	//Offline Declined
	else if(stRCDataAnalyze.usTransResult == d_EMV_CHIP_OFF_DECLINED) 
	{
		vdDebug_LogPrintf("Off Declined...  ");
		vdDisplayErrorMsg(1, 8, "Offline Declined. ");
		ulAPRtn = EMVCL_SetLED(0x0F, 0x01);
		inRet = d_NO;
//		inRet = d_OK;
	}
	else 
	{
		vdDebug_LogPrintf("Unknow Result...  ");
//		vdDisplayErrorMsg(1, 8, "Unknow Result...  ");
//		inRet = d_NO;
		inRet = d_OK;        
	}

	if (inRet == d_OK)
	{
		EMVCL_SetLED(0x0F, 0x02);
		
		vdCTOSS_WaveGetCardLable(stRCDataEx->bSID);
		srTransRec.bWaveSID = stRCDataEx->bSID;
		srTransRec.usWaveSTransResult = stRCDataAnalyze.usTransResult;
		srTransRec.bWaveSCVMAnalysis = stRCDataAnalyze.bCVMAnalysis;
		vdDebug_LogPrintf("bWaveSID=[%x]",srTransRec.bWaveSID);
	
		vdDebug_LogPrintf("baDateTime=[%s]",stRCDataEx->baDateTime);
		memcpy(srTransRec.szYear,&stRCDataEx->baDateTime[2],2);
		wub_str_2_hex(&stRCDataEx->baDateTime[4],srTransRec.szDate,4);
		wub_str_2_hex(&stRCDataEx->baDateTime[8],srTransRec.szDate,6);

		srTransRec.usTrack1Len = stRCDataEx->bTrack1Len;
		memcpy(srTransRec.szTrack1Data, stRCDataEx->baTrack1Data, stRCDataEx->bTrack1Len);
		vdDebug_LogPrintf("usTrack1Len=[%d][%s]...  ",srTransRec.usTrack1Len,srTransRec.szTrack1Data);
		if (srTransRec.usTrack1Len > 0)
			vdGetCardHolderName(srTransRec.szTrack1Data, srTransRec.usTrack1Len);
		
		srTransRec.usTrack2Len = (stRCDataEx->bTrack2Len * 2);
		wub_hex_2_str(stRCDataEx->baTrack2Data,srTransRec.szTrack2Data,stRCDataEx->bTrack2Len);
		vdDebug_LogPrintf("usTrack2Len=[%d][%s]...  ",srTransRec.usTrack2Len,srTransRec.szTrack2Data);
		if (srTransRec.usTrack2Len > 0)
			vdAnalysisTrack2(srTransRec.szTrack2Data, srTransRec.usTrack2Len);
		vdDebug_LogPrintf("szPAN=[%s]",srTransRec.szPAN);
		
		vdDebug_LogPrintf("usChipDataLen=[%d][%s]...  ",stRCDataEx->usChipDataLen,stRCDataEx->baChipData);
		if (stRCDataEx->usChipDataLen > 0)
		{			
			srTransRec.usChipDataLen = stRCDataEx->usChipDataLen;
			memcpy(srTransRec.baChipData, stRCDataEx->baChipData, stRCDataEx->usChipDataLen); 
			vdCTOSS_WaveGetEMVData(stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
		}
		
		vdDebug_LogPrintf("baAdditionalData=[%d][%s]...  ",stRCDataEx->usAdditionalDataLen,stRCDataEx->baAdditionalData);
		if (stRCDataEx->usAdditionalDataLen > 0)
		{			
			srTransRec.usAdditionalDataLen = stRCDataEx->usAdditionalDataLen;
			memcpy(srTransRec.baAdditionalData, stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen); 
			vdCTOSS_WaveGetEMVData(stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);
		}
	}

	return 	inRet;
}


void vdCTOSS_WaveCheckRtCode(ULONG ulAPRtn)
{
	//The returned code other than 0xA0000001, the transaction is terminated.
	vdDebug_LogPrintf("ulAPRtn=[%x]---------",ulAPRtn);
	if(ulAPRtn == d_EMVCL_RC_MORE_CARDS)
	{
		CTOS_LCDTPrintXY(1, 7, "Please Select 1 ");
		vdDisplayErrorMsg(1, 8, "Card            ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_DDA_AUTH_FAILURE)
	{
		CTOS_LCDTPrintXY(1, 7, "Please Use Other");
		vdDisplayErrorMsg(1, 8, "Card            ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_US_CARDS)
	{
		CTOS_LCDTPrintXY(1, 7, "US Card...      ");
		vdDisplayErrorMsg(1, 8, "Please Swipe    ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_CARD_DIFF_FROM_OTHER_AP_COUNTRY)
	{
		CTOS_LCDTPrintXY(1, 7, "Oversea Card... ");
		vdDisplayErrorMsg(1, 8, "Please Insert   ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_FAILURE)
	{
		CTOS_LCDTPrintXY(1, 7, "Terminated, Pls ");
		vdDisplayErrorMsg(1, 8, "Use Other Card  ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_EXCEED_CL_TX_LIMIT)
	{
		vdDisplayErrorMsg(1, 8, "Insert Card     ");
	}
	else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
	{
		vdDisplayErrorMsg(1, 8, "Timeout");
	}
	else
	{
		vdDisplayErrorMsg(1, 8, "Please Try Again");
	}
}



ULONG inCTOSS_CLMOpenAndGetVersion(void)
{
	ULONG ulAPRtn;
	BYTE cVer[128];

	vdDebug_LogPrintf("saturn inCTOSS_CLMOpenAndGetVersion");


	if (NO_CTLS == inCTOSS_GetCtlsMode())
	{
		vdDisplayErrorMsg(1, 8, "CTLS NOT ALLOWED");
		return d_NO;
	}

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
	{
		ulAPRtn = inCTOSS_CLMOpen();
		if(d_EMVCL_NO_ERROR == ulAPRtn) 
		{
			memset(cVer,0x00,sizeof(cVer));
			vdCTOSS_GetCLMVersion(cVer);
		}

		if (8 ==strTCT.byRS232DebugPort)
			EMVCL_SetDebug(1,d_DEBUG_PORT_USB);
	}

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		ulAPRtn = VW_InitComm(d_VW_COMM_COM2, d_BAUDRATE);
		if(d_EMVCL_NO_ERROR == ulAPRtn) 
		{
			memset(cVer,0x00,sizeof(cVer));
			VW_GetAPIVersion(cVer);
		}
	}
	vdDebug_LogPrintf("ulAPRtn=[%d],cVer=[%s]",ulAPRtn,cVer);
	return ulAPRtn;
}



