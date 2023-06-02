#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
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
#include "POSWave.h"

int inWaveTransType = 0;

void vdCTOSS_SetWaveTransType(int type)
{
	inWaveTransType = type;
}

int inCTOSS_GetWaveTransType(void)
{
	return inWaveTransType;
}


void vdCTOS_InitWaveData(void)
{
	vdCTOSS_CLMSetALLCAPK();
	vdCTOSS_CLMSetAIDAndEMVTag();	
}

void vdCTOSS_GetALLWaveSetting(void)
{
	BYTE OutRBuf[2048];
	UINT outLen,rtn;
	EMVCL_SCHEME_DATA stScheme;
	EMVCL_CA_PUBLIC_KEY stCAPubKey;

	vdDebug_LogPrintf("saturn vdCTOSS_GetALLWaveSetting");

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
		
		rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_ADMIN,OutRBuf);
		vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,OutRBuf);
	}
		
	inCTOSS_CLMGetEMVTag(OutRBuf,&outLen);
	inCTOSS_CLMVisaGetCapability(&stScheme);

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		rtn = VW_SwitchToAdministrativeMode(d_VW_MODE_NORMAL,OutRBuf);
		vdDebug_LogPrintf("rtn =[%x],baBuf=[%s].....",rtn,OutRBuf);
		
	vdCTOSS_CLMClose();
		
	inCTOSS_CLMListAllCAPKID(OutRBuf,&outLen);

	inCTOSS_CLMGetACAPK("\xA0\x00\x00\x00\x03",0x08,&stCAPubKey);
}



int inCTOS_MultiAPReloadWaveData(void)
{
	if (srTransRec.usChipDataLen > 0)
	{			
		vdCTOSS_WaveGetEMVData(srTransRec.baChipData, srTransRec.usChipDataLen);
	}
		
	if (srTransRec.usAdditionalDataLen > 0)
	{			
		vdCTOSS_WaveGetEMVData(srTransRec.baAdditionalData, srTransRec.usAdditionalDataLen);
	}
	
	return (d_OK);
}

int inCTOS_WaveFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
	int inEVENT_ID = 0;

	memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));
    
    //vdDispTransTitle(SALE);

    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
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
    
    inRet = inCTOS_WaveGetCardFields();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {

        
		vdDebug_LogPrintf("saturn GO TO SUB-AP");
        if (srTransRec.byTransType == REFUND)
            inEVENT_ID = d_IPC_CMD_REFUND;
        else
            inEVENT_ID = d_IPC_CMD_ONLINES_SALE;
        inRet = inCTOS_MultiAPSaveData(inEVENT_ID);
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

			inRet = inCTOS_MultiAPReloadWaveData();
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

    inRet = inCTOS_EMVTCUpload();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return d_OK;
}


int inCTOS_WAVE_SALE(void)
{
    int inRet = d_NO;


	vdDebug_LogPrintf("saturn start wave sale");

    //CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("saturn vdCTOS_TxnsBeginInit");


    vdCTOS_TxnsBeginInit();

	vdDebug_LogPrintf("saturn vdCTOS_SetTransType");

	vdCTOS_SetTransType(SALE);
	
	vdDebug_LogPrintf("saturn vdCTOSS_SetWaveTransType");
	vdCTOSS_SetWaveTransType(1);

	vdDebug_LogPrintf("saturn vdCTOSS_GetAmt");

	vdCTOSS_GetAmt();

	vdDebug_LogPrintf("saturn inCTOS_WAVE_SALE");

	inRet = inCTOSS_CLMOpenAndGetVersion();
	if(d_OK != inRet)
        return inRet;
	
	//display title
    vdDispTransTitle(SALE);
	
    inRet = inCTOS_WaveFlowProcess();

	vdCTOSS_CLMClose();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}



int inCTOS_WAVE_REFUND(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

	vdCTOS_SetTransType(REFUND);
	vdCTOSS_SetWaveTransType(1);

	vdCTOSS_GetAmt();

	vdDebug_LogPrintf("saturn inCTOS_WAVE_REFUND");

	inRet = inCTOSS_CLMOpenAndGetVersion();
	if(d_OK != inRet)
        return inRet;
	//display title
    vdDispTransTitle(REFUND);

    inRet = inCTOS_WaveFlowProcess();

	vdCTOSS_CLMClose();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}



