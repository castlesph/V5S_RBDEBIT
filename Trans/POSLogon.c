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
#include "..\Includes\POSLogon.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
//#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
//#include "../Database/dct.h"
#include "..\Includes\MultiApLib.h"
#include "../DataBase/DataBaseFunc.h"

//TEMP FLAGS FOR HOSTid AND miTID FOR USE IN AUTOLOGON
BYTE        TEMPHDTid; 
UINT 		TEMPMITid;
BOOL fLogon;


// BDO: Added flag to supress merc password during auto Log-On -- jzg
BOOL fAutoLogOn = FALSE;

int inLogonSingleHost = 0;
BOOL fDBChk = FALSE;
UINT MID_LogonSingleHost;


int inCTOS_LogonFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];


	vdDebug_LogPrintf("inCTOS_LogonFlowProcess --- BEGIN");
	vdDebug_LogPrintf("inCTOS_LogonFlowProcess --- fAutoLogOn[%d]", fAutoLogOn);

	fLogon = TRUE;

	//CTOS_LCDTClearDisplay();
    vdCTOS_SetTransType(LOG_ON);
    
    //display title
    vdDispTransTitle(LOG_ON);
    vdCTOS_DispStatusMessage("PROCESSING...");
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;


	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

#if 1
	vdDebug_LogPrintf("select IIT");
#if 0
	inRet = inCTOS_SelectLogonIIT();
	if(d_OK != inRet)
        return inRet;
#endif
//	vdDebug_LogPrintf("select PIT");
//	inRet = inCTOS_SeletLogonPIT();
//	if(d_OK != inRet)
//        return inRet;
#endif
	
	inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);//Optimize start
	if (inIITReadEx(1) != d_OK)
    {
		inDatabase_TerminalCloseDatabase();
        return(d_NO);
    }

	vdDebug_LogPrintf("JEFF inCTOS_LogonFlowProcess::HDT = [%d]    mit = [%d]", srTransRec.HDTid,  srTransRec.MITid );
	//inDCTRead(srTransRec.HDTid,srTransRec.MITid);
	inDCTReadEx(srTransRec.HDTid,srTransRec.MITid);
	inDatabase_TerminalCloseDatabase();//Optimize end
	
	vdDebug_LogPrintf("select hdt");
#if 0
    inRet = inCTOS_SelectLogonHost();
    if(d_OK != inRet)
        return inRet;
#endif	

	
	if (fAutoLogOn == FALSE){

		vdDebug_LogPrintf("inCTOS_LogonFlowProcess --- A");

		
	    if (inMultiAP_CheckMainAPStatus() == d_OK)
	    {
	        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_LOGON);
	        if(d_OK != inRet)
	            return inRet;
	    }
	    else
	    {
	        if (inMultiAP_CheckSubAPStatus() == d_OK)
	        {

			if (fDBChk ==FALSE)  //for testing
			{
				inRet = inCTOS_MultiAPGetData();

				if(d_OK != inRet)
					return inRet;
				else  //for testing
					{
						fDBChk = TRUE;  //for testing
						MID_LogonSingleHost = srTransRec.MITid;
						
					}
			}
			
	            srTransRec.IITid = 1; //Set IIT to 1 for BDODebit

				inRet = inCTOS_MultiAPReloadTable();
	            if(d_OK != inRet)
	                return inRet;
	        }
	        inRet = inCTOS_MultiAPCheckAllowd();
	        if(d_OK != inRet)
	            return inRet;
	    }
	}
	else
	{
		vdDebug_LogPrintf("inCTOS_LogonFlowProcess --- B");
	
		inRet = inCTOS_SelectLogonHost();
		if(d_OK != inRet)
			return inRet;
	}

//Enhance start
	inRet = inCTOS_PreConnectAndInit();
	if(d_OK != inRet)
	{
		vdSetECRResponse(ECR_COMM_ERROR_RESP);
		return inRet;
	}
//Enhance end

#if 1
	//remove later
	vdDebug_LogPrintf("JEFF inCTOS_LogonFlowProcess::MIT = [%d]", strMMT[0].MITid);
	vdDebug_LogPrintf("JEFF inCTOS_LogonFlowProcess::szMerchantName = [%s]", strMMT[0].szMerchantName);
	vdDebug_LogPrintf("JEFF inCTOS_LogonFlowProcess::fAutoLogOn = [%d]", fAutoLogOn);
#endif
	
	//CTOS_Delay(1000);

	// BDO: Added flag to supress merc password during auto Log-On -- jzg
	if (fAutoLogOn == TRUE)
	{
		strcpy(srTransRec.szTID, strMMT[0].szTID);
		strcpy(srTransRec.szMID, strMMT[0].szMID);

		
		inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);


		strMMT[0].fLogon_Failed = TRUE;
		 inRet = inMMTSave(strMMT[0].MMTid);

		vdDebug_LogPrintf("inMMTSave [%d]", inRet);
	}
	else
	{
		int inReturn=0;
	
	
		//inRet = inCTOS_CheckAndSelectMutipleMID();
		//if(d_OK != inRet)
		//	  return inRet;
		vdDebug_LogPrintf("JEFF inCTOS_LogonFlowProcess::MIT = [%d]  --- HDT = [%d]  ---   fDBChk[%d]", srTransRec.MITid, srTransRec.HDTid, fDBChk);

		//inRet = inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

		if (fDBChk ==TRUE) 	
			inReturn = inMMTReadRecord(srTransRec.HDTid,MID_LogonSingleHost);
		else
			inReturn = inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);
		
		vdDebug_LogPrintf("inMMTReadRecord inret = [%d]  ------- MMTid[%d]", inReturn, strMMT[0].MMTid);

		
		strMMT[0].fLogon_Failed = TRUE;
		inReturn = inMMTSave(strMMT[0].MMTid);

		vdDebug_LogPrintf("inMMTSave [%d]", inReturn);

	}

	//if ((fAutoLogOn != TRUE) && (strCPT.inCommunicationMode == GPRS_MODE)){
	
	vdDebug_LogPrintf("strCPT.inCommunicationMode = [%d]", strCPT.inCommunicationMode);
	//if ((fAutoLogOn != TRUE) && (strCPT.inCommunicationMode == GPRS_MODE)){

	if (srTransRec.usTerminalCommunicationMode != GPRS_MODE && srTransRec.usTerminalCommunicationMode != WIFI_MODE)
	{
		inRet = inCTOS_PreConnect();
		if(d_OK != inRet)
	    	return inRet;
	}

    inRet = inCTOS_CheckIssuerEnable();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet){
			
				// sidumili: delete created reversal
			  //inMyFile_ReversalDelete();
				
        return inRet;
    }

	if (fDBChk ==FALSE) 	
		inDisplayLogonResponse();


	fLogon = FALSE;

    return d_OK;
}


int inCTOS_SelectLogonHost(void) 
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;
	char szDebug[40]={0};


    //inHostIndex =  53;
    inHostIndex = (short) srTransRec.HDTid;
    
     vdDebug_LogPrintf("inCTOS_SelectHost =[%d]",inHostIndex);

//Removed to properly pickup the correct host - start -- jzg
//    if (inMultiAP_CheckSubAPStatus() == d_OK)
//        return d_OK;
//Removed to properly pickup the correct host - end -- jzg
	inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);//Optimize start
    //if (inHDTRead(inHostIndex) != d_OK)
    if (inHDTReadEx(inHostIndex) != d_OK)
    {
		inDatabase_TerminalCloseDatabase();
        vdSetErrorMessage("HOST SELECTION ERR");
        return(d_NO);
    } 
    else 
    {
        
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        //if (inCSTRead(inCurrencyIdx) != d_OK) 
        if (inCSTReadEx(inCurrencyIdx) != d_OK) 
        {
            inDatabase_TerminalCloseDatabase();
            vdSetErrorMessage("LOAD CST ERR");
            return(d_NO);
        }

		if (strTCT.fSingleComms)	
			inHostIndex = 1;
		
        //if ( inCPTRead(inHostIndex) != d_OK)
        if ( inCPTReadEx(inHostIndex) != d_OK)
        {
            inDatabase_TerminalCloseDatabase();
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }

		inDatabase_TerminalCloseDatabase();//Optimize End
        return (d_OK);
    }
}

int inCTOS_LOGON(void)
{
    int inRet = d_NO;
		int result;

	vdDebug_LogPrintf("inCTOS_LOGON");
	
    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

	if (fAutoLogOn == TRUE){
		srTransRec.HDTid = TEMPHDTid;
		srTransRec.MITid = TEMPMITid;	
	}
	else
		{
			inCTOS_LOGON_SingleDebitHost();
			return d_OK;
		}

	strTCT.fLogonFailed = TRUE;
	inTCTSaveLogonFailedFlag(1);

    inRet = inCTOS_LogonFlowProcess();

	if(inRet == d_OK)
	{
		strTCT.fLogonFailed = FALSE;
		inTCTSaveLogonFailedFlag(1);

		
		vdDebug_LogPrintf("inCTOS_LOGON [%d]  --- MMTid[%d]", inRet,  strMMT[0].MMTid);
		strMMT[0].fLogon_Failed = FALSE;
		inMMTSave(strMMT[0].MMTid);
	}
	
    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

	fLogon = FALSE;  //clear logon flag

    return inRet;
}

int inCTOS_DoAutoLogon(void){

	int inRet;



	inDCTRead(srTransRec.HDTid,srTransRec.MITid);

  	if(strDCT.fDebitLogonRequired == VS_FALSE)
		return d_OK;

	CTOS_LCDTClearDisplay();
	vdCTOS_SetTransType(LOG_ON);
	vdDispTransTitle(LOG_ON);

	#if 0
	//120214
	setLCDPrint(4,DISPLAY_POSITION_LEFT, "LOGON REQUIRED");	
	setLCDPrint(5,DISPLAY_POSITION_LEFT, "ERROR CODE:81       ");	
	CTOS_Delay(2000);
	#endif
	
	setLCDPrint(4,DISPLAY_POSITION_LEFT, "PLEASE WAIT...");	
	setLCDPrint(5,DISPLAY_POSITION_LEFT, "PROCESSING LOGON");
	CTOS_Delay(1000);
	//120214

	// BDO: Added flag to supress merc password during auto Log-On -- jzg
	fAutoLogOn = TRUE;

	
	TEMPHDTid = srTransRec.HDTid;
    TEMPMITid = srTransRec.MITid;
	
	inRet = inCTOS_LOGON();
		if(d_OK != inRet)
			return inRet;

	//sidumili
	//vdDisplayMessage("LOGON", "SUCCESSFULLY", "COMPLETED");
	//CTOS_Beep();
	//CTOS_Delay(1000);
	
//012215
//  	if(ST_SUCCESS != inRet){	
//		strDCT.fDebitLogonRequired = VS_TRUE;	
//		inDCTSave(srTransRec.HDTid);
//		return inRet;
//  	}

	//strDCT.fDebitLogonRequired = VS_FALSE;


	inDCTSave(srTransRec.HDTid,srTransRec.MITid);
//012215
		
	return d_OK;
}

//sidumili: [LOAD IIT TABLE FOR LOGON TRXN]
int inCTOS_SelectLogonIIT(void){
	int inRet = d_OK;
	int inIITId;

	inIITId = 1;
	
	inRet = inGetIssuerRecord(inIITId);

	if (inRet != d_OK)
		return(inRet);
	
  return d_OK;
}


int inCTOS_SeletLogonPIT(void){
	int inRet = d_OK;
	int inTxnTypeID;

	inTxnTypeID = srTransRec.byTransType;

	inRet = inPITRead(inTxnTypeID);

	if (inRet != d_OK)
		return(inRet);
	
  return d_OK;
	
}

int inDisplayLogonResponse(void){

	vdDisplayMessage("LOGON", "SUCCESSFULLY", "COMPLETED");
	CTOS_Beep();
	CTOS_Delay(500);

	return d_OK;

}




int inCTOS_LOGON_SingleDebitHost(void)
{
	int inRet = d_NO;
	int result;
	int inNumOfHost = 0, inNum =0;
	int inAPPID;
	char szAPName[25] = {0};
	char szHostName[16] = {0};
	int inDebitHostCount=0;


	int inHDTIndex=0, inHDTid[150];

	 
	 inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

		
	inNumOfHost=inHDTReadHostID(szAPName, inHDTid);
	
		
	vdDebug_LogPrintf("ZZZ ---- Debit inNumOfHost[%d]--- szAPName[%s]",inNumOfHost, szAPName);
	
	vdDispTransTitle(LOG_ON);
	
	 for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	 {

		if (inNumOfHost > 1)
			vdCTOS_DispStatusMessage("PROCESSING...");
			 
		
		inHDTIndex=inHDTid[inNum-1];

		
		vdDebug_LogPrintf("inNum [%d]  ---- inHDTIndex[%d] ", inNum, inHDTIndex );
	 
		 if(inHDTRead(inHDTIndex) == d_OK)
		 {
			 vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
			 if (strcmp(szAPName, strHDT.szAPName)!=0)
			 {
				 continue;
			 }
					
				vdDebug_LogPrintf("inCTOS_LOGON_SingleDebitHost ---- MID");
			 
					
				srTransRec.HDTid = inHDTIndex;
				inLogonSingleHost = inHDTIndex;

				//strTCT.fLogonFailed = TRUE;
				//inTCTSaveLogonFailedFlag(1);

				inRet = inCTOS_LogonFlowProcess();
				vdDebug_LogPrintf("inCTOS_LogonFlowProcess [%d]", inRet );
				
				if(inRet == d_OK)
				{
					vdDebug_LogPrintf("inCTOS_LogonFlowProcess [%d]  --- MMTid[%d]", inRet,  strMMT[0].MMTid);
					strMMT[0].fLogon_Failed = FALSE;
					inMMTSave(strMMT[0].MMTid);
				}

				memset(szHostName, 0x00, sizeof(szHostName));
				strcpy(szHostName, strHDT.szHostLabel);
				if(inRet == d_OK)
					{
						if (inNumOfHost > 1)
							vdDisplayMessage(szHostName, "Logon", "Successful");
						else
							vdDisplayMessage("LOGON", "SUCCESSFULLY", "COMPLETED");
					}
				else
					vdDisplayMessage(szHostName, "Logon", "Failed");

				CTOS_Beep();
				CTOS_Delay(500);
				
				vduiClearBelow(2);

				
				if (inNumOfHost == 1)
					break;
		
		 	}

							
	 }


	inCTOS_inDisconnect();

	vdCTOS_TransEndReset();

	inLogonSingleHost = 0;
	fDBChk =FALSE;
	fLogon = FALSE;
	
	//inDisplayLogonResponse();

	return d_OK;
	
}


