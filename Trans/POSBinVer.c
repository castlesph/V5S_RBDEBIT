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
#include "..\Includes\POSBinVer.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "../Includes/wub_lib.h"
#include "..\database\bvt.h"
#include "..\Includes\POSSale.h"


//1030
BOOL fBinVerInitiatedSALE=0;
//1030


extern void setLCDPrint27(int line,int position,char * pbBuf);

int inCTOS_BINCheckFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];




    vdCTOS_SetTransType(BIN_VER);
    
    //display title
    vdDispTransTitle(BIN_VER);


    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;
    
    inRet = inCTOS_GetCardFields();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;


 
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BINVER);
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
 
    inRet = inCTOS_GetBINVerData();
    if (d_OK != inRet)
      return inRet;


    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

     inRet = inBuildAndSendIsoData();

    if(d_OK != inRet)
        return inRet;


    inRet= inCTOS_ProceedToSale();
    if(d_OK != inRet)
        return inRet;

	vdSetFirstIdleKey(0x00);


	if(srTransRec.usTerminalCommunicationMode == DIAL_UP_MODE){
		inCTOS_inDisconnect();
	}
		
	fBinVerInitiatedSALE=VS_TRUE;

    inRet = inCTOS_WaveFlowProcess();
	
	fBinVerInitiatedSALE = VS_FALSE;

    return d_OK;
}


int inCTOS_GetBINVerData(void){


	inBVTRead(1);
 
	memcpy(strHDT.szTPDU,strBVT.szBINVerTPDU,5);
	strcpy(srTransRec.szTID, strBVT.szBINVerTID);
   	strcpy(srTransRec.szMID, strBVT.szBINVerMID);
	
    	return d_OK;

}

int inCTOS_ProceedToSale(void){

	BYTE key;
       int inRet;
       
	char szBIN[6+1];


    	CTOS_LCDTClearDisplay();
    	vdDispTransTitle(BIN_VER);

	memset(szBIN, 0x00, sizeof(szBIN));
	strncpy(szBIN, srTransRec.szPAN, 6);


	do{

		if(VS_TRUE == strTCT.fDemo)
			strcpy(szField63,"BANCO DE ORO");

 		setLCDPrint27(3,DISPLAY_POSITION_LEFT, "CARD ISSUER:");		
       	setLCDPrint27(4, DISPLAY_POSITION_RIGHT, szField63);
		
		setLCDPrint27(6, DISPLAY_POSITION_LEFT, "BIN:");
       	setLCDPrint27(6, DISPLAY_POSITION_RIGHT, szBIN);

		CTOS_LCDTPrintXY(1, 8, "PROCEED[F1]CANCEL[F4]"); //sidumili: Fix on Issue#:000212

		key=WaitKey(60);

		if (key == d_KBD_F1){
			inRet = d_OK;
			break;
		}else if ((key == d_KBD_F4) || (key == d_KBD_CANCEL)){
			inRet = d_NO;
			break;
		}

	}while(1);

	return inRet;

}




int inUnPackBINVer63(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{


    int inLen;


 
   memset(szField63, 0x00, sizeof(szField63));
    inLen =((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) *100;
    inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;

    memcpy(szField63, (char *)&uszUnPackBuf[2], inLen);

    return ST_SUCCESS;
}


int inCTOS_BINCHECK(void)
{
    int inRet = d_NO;



    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_BINCheckFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

