/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>

#include <unistd.h>
#include <pwd.h>


#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>


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
#include "..\TMS\TMS.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"


#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"

#if 0

#ifndef d_CTMS_INFO_LAST_UPDATE_TIME
#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
#endif

USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo)
{
    CTOS_RTC SetRTC;
    
    CTOS_RTCGet(&SetRTC);

    SetRTC.bMonth -= 2;

    memcpy(stInfo, &SetRTC, sizeof(CTOS_RTC));

    return d_OK;
}

#endif

int inCTOSS_TMSCheckIfDefaultApplication(void)
{
    CTOS_stCAPInfo stinfo;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;

    getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	strcpy(exe_subdir, &exe_dir[9]);
	inExeAPIndex = atoi(exe_subdir);
	memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
    if(CTOS_APGet(inExeAPIndex, &stinfo) != d_OK)
	{
		CTOS_APGet(inExeAPIndex, &stinfo);
	}

    vdDebug_LogPrintf("stinfo.bFlag[%02X]", stinfo.bFlag);
    if (stinfo.bFlag != d_AP_FLAG_DEF_SEL_EX)
        return d_NO;
    else
        return d_OK;
    
}

int inCTOSS_TMSChkBatchEmpty(void)
{
    int inResult;

	vdDispTitleString("CTMS Init");
	CTOS_LCDTPrintXY (1,8, "Please Wait");

    vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty start");
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOSS_TMSChkBatchEmptyProcess();
        if(inResult != d_OK)
            return inResult;
        else
		    inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_CHK_BATCH_EMPTY);

        if(inResult != d_FAIL)
            inResult = d_OK;
	}
	else
	{
		inResult = inCTOSS_TMSChkBatchEmptyProcess();
	}
    
    vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty return[%d]", inResult);
    
    return (inResult);
}


int inCTOSS_TMSChkBatchEmptyProcess(void)
{
    int inResult;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
    char szAPName[25];
	int inAPPID;

	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    //check host num
    inNumOfHost = inHDTNumRecord();
    
    vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
			if (strcmp(szAPName, strHDT.szAPName)!=0)
			{
				continue;
			}
            
            inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
        
            vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
            for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
            {
            /*
                if((inResult = inMMTReadRecord(strHDT.inHostIndex, strMMT[inLoop-1].MMTid)) !=d_OK)
                {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MMTid,strHDT.inHostIndex,inResult);
                    continue;
                    //break;
                }
                else    // delete batch where hostid and mmtid is match  
                { 
                */
                    strMMT[0].HDTid = strHDT.inHostIndex;
                    strMMT[0].MITid = strMMT[inLoop-1].MITid;
                    srTransRec.MITid = strMMT[inLoop-1].MITid;

                    vdDebug_LogPrintf("srTransRec.MITid[%d]strHDT.inHostIndex[%d]", srTransRec.MITid, strHDT.inHostIndex);
                    memset(&srAccumRec,0,sizeof(srAccumRec));
                    memset(&strFile,0,sizeof(strFile));
                    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
                    memset(&strFile,0,sizeof(strFile));
                    vdCTOS_GetAccumName(&strFile, &srAccumRec);

                if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) > 0)
                {
                    //vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty Not Empty");
                    //vdDisplayErrorMsg(1, 8, "BATCH NOT", "EMPTY,SKIPPED.");
                    vdSetErrorMessage("");
                    return (d_FAIL);
                }
                else
                {
                    vdDebug_LogPrintf("EMPTY BATCH");
                }
                
            //    }
            }
        }
        else
            continue;

    }
    
    return (d_OK);
}

int inCTOSS_TMSPreConfig(int inComType)
{    
    CTMS_GPRSInfo stgprs;    
    CTMS_ModemInfo stmodem;    
    CTMS_EthernetInfo st;
    int inNumOfRecords = 0;
    BYTE szSerialNum[17+1], szInputBuf[21+1];
    BYTE count = 2,i;
	BYTE tmpbuf[16 + 1];
	int len;
    unsigned char ckey;
	USHORT usStatusLine=8, usRes=0;
	
    inHDTRead(1);
    inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfRecords);
    
    memset(szSerialNum, 0x00, sizeof(szSerialNum));
	memset(tmpbuf, 0x00, sizeof(tmpbuf));

	CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
	if(strlen(szSerialNum) <= 0)
	    CTOS_GetFactorySN(szSerialNum);
	for (i=0; i<strlen(szSerialNum); i++)
	{
		if (szSerialNum[i] < 0x30 || szSerialNum[i] > 0x39)
		{
			szSerialNum[i] = 0;
			break;
		}
	}
	len = strlen(szSerialNum);
	strcpy(tmpbuf,"0000000000000000");
	memcpy(&tmpbuf[16-len],szSerialNum,len);
	vdDebug_LogPrintf("szSerialNum=[%s].tmpbuf=[%s]..",szSerialNum,tmpbuf);
	
    CTOS_CTMSSetConfig(d_CTMS_SERIALNUM, tmpbuf);//if TID is 12345678, SN is 0000000012345678
    CTOS_CTMSSetConfig(d_CTMS_RECOUNT, &count);

    if(strTCP.fDHCPEnable)
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, "0.0.0.0");
    else
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, strTCP.szTerminalIP);// If it is DHCP, also need to configure any value, otherwise please put the value from database

    if(ETHERNET_MODE == strTCT.inTMSComMode)
    {
        CTOS_LCDTClearDisplay();
        vdDispTitleString("ETHERNET SETTINGS");
	
    	vdDebug_LogPrintf("ETHERNET_MODE..");
        memset(&st, 0x00, sizeof (CTMS_EthernetInfo));
        
        strcpy(st.strGateway, strTCP.szGetWay);
        strcpy(st.strMask, strTCP.szSubNetMask);
        st.bDHCP = strTCP.fDHCPEnable;

        strcpy(st.strRemoteIP, strTCT.szTMSRemoteIP);
        st.usRemotePort = strTCT.usTMSRemotePort;


		CTOS_LCDTClearDisplay();
		vdDispTitleString("ETHERNET SETTINGS");

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", st.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", st.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
	
        usRes=CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &st);
    }
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
    {
    	vdDebug_LogPrintf("DIAL_UP_MODE..");

        CTOS_LCDTClearDisplay();
        vdDispTitleString("DIALUP SETTINGS");

        memset(&stmodem, 0x00, sizeof (CTMS_ModemInfo));
        stmodem.bMode = d_M_MODE_AYNC_FAST;
        stmodem.bHandShake = d_M_HANDSHAKE_V32BIS_AUTO_FB;
        stmodem.bCountryCode = d_M_COUNTRY_SINGAPORE;

        strcpy(stmodem.strRemotePhoneNum, strTCT.szTMSRemotePhone);
		vdDebug_LogPrintf("strRemotePhoneNum[%s]..",stmodem.strRemotePhoneNum);
        strcpy(stmodem.strID, strTCT.szTMSRemoteID);	
		vdDebug_LogPrintf("strID[%s]..",stmodem.strID);
        strcpy(stmodem.strPW, strTCT.szTMSRemotePW);
		vdDebug_LogPrintf("strPW[%s]..",stmodem.strPW);

        strcpy(stmodem.strRemoteIP, strTCT.szTMSRemoteIP);
        stmodem.usRemotePort = strTCT.usTMSRemotePort;

		stmodem.usPPPRetryCounter = 2;
		stmodem.ulPPPTimeout = 34463;
		stmodem.ulDialTimeout = 34463;
		vdDebug_LogPrintf("strRemoteIP[%s].usRemotePort=[%d].",stmodem.strRemoteIP,stmodem.usRemotePort);

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);


        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stmodem.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stmodem.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
        
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PHONE: %s", stmodem.strRemotePhoneNum);         
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf); 
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "ID: %s", stmodem.strID);         
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stmodem.strPW);         
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
		
        usRes=CTOS_CTMSSetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
    }
    else if(GPRS_MODE == strTCT.inTMSComMode)
    {
		CTMS_GPRSInfo stgprs;

        CTOS_LCDTClearDisplay();
        vdDispTitleString("GPRS SETTINGS");
		
		memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
		CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
		
        if(strlen(stgprs.strAPN) > 0)
            strcpy(strTCP.szAPN, stgprs.strAPN);

        if(strlen(stgprs.strID) > 0)
            strcpy(strTCP.szUserName, stgprs.strID);

        if(strlen(stgprs.strPW) > 0)
		    strcpy(strTCP.szPassword, stgprs.strPW);
		
              vdDebug_LogPrintf("GPRS_MODE..");
        memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
        strcpy(stgprs.strAPN, strTCP.szAPN);
        strcpy(stgprs.strID, strTCP.szUserName);
        strcpy(stgprs.strPW, strTCP.szPassword);
 
        strcpy(stgprs.strRemoteIP, strTCT.szTMSRemoteIP);
        stgprs.usRemotePort = strTCT.usTMSRemotePort;
        stgprs.ulSIMReadyTimeout = 10000;
        stgprs.ulGPRSRegTimeout = 10000;
        stgprs.usPPPRetryCounter = 5;
        stgprs.ulPPPTimeout = 10000;
        stgprs.ulTCPConnectTimeout = 10000;

		//test
		stgprs.ulTCPTxTimeout=10000;
		stgprs.ulTCPRxTimeout=10000;
		stgprs.bSIMSlot=1;

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);


        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stgprs.strRemoteIP);		 
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stgprs.usRemotePort);		 
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "APN: %s", stgprs.strAPN);		 
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "USER NAME: %s", stgprs.strID);		 
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stgprs.strPW);		 
        setLCDPrint(7, DISPLAY_POSITION_LEFT, szInputBuf);
		
        usRes=CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    }


	setLCDPrint(8, DISPLAY_POSITION_LEFT, "ANY KEY TO CONTINUE");
	CTOS_KBDGet(&ckey);

	return usRes;
}

int inCTOSS_CheckIntervialDateFrom2013(int y,int m,int d)
{
    int x[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    long i,s=0;
    
    for(i=2013;i<y;i++)
    {
        if(i%4==0&&i%100!=0||i%400==0)
            s+=366;
        else 
            s+=365;
    }
            
    if(y%4==0&&y%100!=0||y%400==0)
        x[2]=29;
    
    for(i=1;i<m;i++)
        s+=x[i];
        
    s+=d;

    vdDebug_LogPrintf("Date[%ld]", s);
    return s;
}


int inCTOSS_SettlementCheckTMSDownloadRequest(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    //only default APP support TMS download
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;

	if(DIAL_UP_MODE == strTCT.inTMSComMode)
		return d_NO;
    //#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
    //USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo);
    usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	//if(d_OK != usResult && d_CTMS_NO_INFO_DATA != usResult)
    //    return d_NO;

/*
	if (d_OK == usResult)
	{
	    inYear = SetRTC.bYear;
	    inMonth = SetRTC.bMonth;
	    inDate = SetRTC.bDay;
	    CTOS_RTCGet(&SetRTC);

	    inDateGap = inCTOSS_CheckIntervialDateFrom2013(SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013(inYear, inMonth, inDate);
		vdDebug_LogPrintf("inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);

		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	}
*/
    //check if batch settle
    //should check all application?
    vdDebug_LogPrintf("Check Batch Empty");
    if(inCTOSS_TMSChkBatchEmpty() != d_OK)
        return d_NO;

    //check if TMS is downloading
    //vdDebug_LogPrintf("Check Get Status");
    //usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    //if (usResult == d_CTMS_UPDATE_FINISHED)
    //{
    //    strTCT.usTMSStatus = FALSE;
    //    inTCTSave(1);        
    //}
    //else
    //{
    //    return d_NO;
    //}
    //vdDebug_LogPrintf("Check Get Status %d %d", usStatus, usReterr);
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;

    
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
    usResult = inCTOSS_TMSPreConfig2(usComType);
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);

//test only
/*
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "TMS Download");
    CTOS_Delay(10000);
    return d_OK;
*/

    CTOS_CTMSUtility(usComType);
    
    /*usResult = CTOS_CTMSInitDaemonProcess(usComType);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]", usResult);
    
    usResult = CTOS_CTMSGetUpdateType(&st);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]st.bNums[%d]", usResult, st.bNums);
    if(usResult == d_OK && st.bNums > 0)
    {
        strTCT.usTMSStatus = TRUE;
        inTCTSave(1);
        
        CTOS_CTMSUtility(usComType);
    }*/

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    return d_OK;
}

int inCTOSS_CheckIfPendingTMSDownload(void)
{
    USHORT usResult;
    USHORT usStatus, usReterr;
    BYTE bDisplayBuf[50];
    USHORT i, loop = 0;
    USHORT usComType = d_CTMS_NORMAL_MODE;


    //if(FALSE == strTCT.usTMSStatus)
    //    return d_OK;

    CTOS_LCDTClearDisplay();

    if(ETHERNET_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_GPRS;


    usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    vdDebug_LogPrintf("CTOS_CTMSGetStatus usResult[%d] usStatus[%d] usReterr[%d]", usResult, usStatus, usReterr);
    if (usResult == d_CTMS_UPDATE_FINISHED)
    {
        //strTCT.usTMSStatus = FALSE;
        //inTCTSave(1);
            
        return d_OK;
    }
    else
    {
        CTOS_CTMSUtility(usComType);
    }
}


int inCTOSS_TMSDownloadRequest(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	CTOS_LCDTClearDisplay();
	vdDispTitleString("CTMS Init");
    CTOS_LCDTPrintXY (1,8, "Please Wait");
	
    //only default APP support TMS download
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;
	
    //#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
    //USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo);
    usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	//if(d_OK != usResult && d_CTMS_NO_INFO_DATA != usResult)
    //    return d_NO;

/*
	if (d_OK == usResult)
	{
	    inYear = SetRTC.bYear;
	    inMonth = SetRTC.bMonth;
	    inDate = SetRTC.bDay;
	    CTOS_RTCGet(&SetRTC);

	    inDateGap = inCTOSS_CheckIntervialDateFrom2013(SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013(inYear, inMonth, inDate);
		vdDebug_LogPrintf("inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);

		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	}
*/
    //check if batch settle
    //should check all application?
    vdDebug_LogPrintf("Check Batch Empty");
    //if(inCTOSS_TMSChkBatchEmpty() != d_OK)
     //   return d_NO;

    //check if TMS is downloading
    //vdDebug_LogPrintf("Check Get Status");
    //usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    //if (usResult == d_CTMS_UPDATE_FINISHED)
    //{
    //    strTCT.usTMSStatus = FALSE;
    //    inTCTSave(1);        
    //}
    //else
    //{
    //    return d_NO;
    //}
    //vdDebug_LogPrintf("Check Get Status %d %d", usStatus, usReterr);
    
    inCTOS_TMSPreConfigSetting();

	
    
	//if(inCTOS_TMSPreConfigSetting() != d_OK)
	//{
        //vdDisplayErrorMsg(1, 8, "USER CANCEL");
        //return d_NO;		
	//}
	
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;

    
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
    usResult = inCTOSS_TMSPreConfig(usComType);
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);

//test only
/*
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "TMS Download");
    CTOS_Delay(10000);
    return d_OK;
*/

    CTOS_CTMSUtility(usComType);
    
    /*usResult = CTOS_CTMSInitDaemonProcess(usComType);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]", usResult);
    
    usResult = CTOS_CTMSGetUpdateType(&st);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]st.bNums[%d]", usResult, st.bNums);
    if(usResult == d_OK && st.bNums > 0)
    {
        strTCT.usTMSStatus = TRUE;
        inTCTSave(1);
        
        CTOS_CTMSUtility(usComType);
    }*/

    return d_OK;
}

void inCTOSS_TMS_USBUpgrade(void)
{
    char szSystemCmdPath[250];
    //char szSystemTime[50];
    //CTOS_RTC SetRTC;

	//CTOS_RTCGet(&SetRTC);
    //memset(szSystemTime, 0x00, sizeof(szSystemTime));
    //sprintf(szSystemTime, "%02d" ,SetRTC.bDay);
    
	vduiClearBelow(3);
	CTOS_LCDTPrintXY (1,7, "PLEASE WAIT"); 	  

    memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));                    
    //sprintf(szSystemCmdPath, "cp -f /media/mdisk/vxupdate/V5S_MCCCREDITx.prm .%s", PUBLIC_PATH);
    sprintf(szSystemCmdPath, "cp -f /home/ap/pub/menu.bmp ./media/mdisk/vxupdate/");
    system(szSystemCmdPath);	

    vdDebug_LogPrintf(szSystemCmdPath); 	
}


int inCTOSS_TMSPreConfig2(int inComType)
{    
    CTMS_GPRSInfo stgprs;    
    CTMS_ModemInfo stmodem;    
    CTMS_EthernetInfo st;
    int inNumOfRecords = 0;
    BYTE szSerialNum[17+1], szInputBuf[21+1];
    BYTE count = 2,i;
	BYTE tmpbuf[16 + 1];
	int len;
    unsigned char ckey;
	USHORT usStatusLine=8, usRes=0;
	
    inHDTRead(1);
    inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfRecords);
    
    memset(szSerialNum, 0x00, sizeof(szSerialNum));
	memset(tmpbuf, 0x00, sizeof(tmpbuf));

	CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
	if(strlen(szSerialNum) <= 0)
	    CTOS_GetFactorySN(szSerialNum);
	for (i=0; i<strlen(szSerialNum); i++)
	{
		if (szSerialNum[i] < 0x30 || szSerialNum[i] > 0x39)
		{
			szSerialNum[i] = 0;
			break;
		}
	}
	len = strlen(szSerialNum);
	strcpy(tmpbuf,"0000000000000000");
	memcpy(&tmpbuf[16-len],szSerialNum,len);
	vdDebug_LogPrintf("szSerialNum=[%s].tmpbuf=[%s]..",szSerialNum,tmpbuf);
	
    CTOS_CTMSSetConfig(d_CTMS_SERIALNUM, tmpbuf);//if TID is 12345678, SN is 0000000012345678
    CTOS_CTMSSetConfig(d_CTMS_RECOUNT, &count);

    if(strTCP.fDHCPEnable)
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, "0.0.0.0");
    else
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, strTCP.szTerminalIP);// If it is DHCP, also need to configure any value, otherwise please put the value from database

    if(ETHERNET_MODE == strTCT.inTMSComMode)
    {
        //CTOS_LCDTClearDisplay();
        //vdDispTitleString("ETHERNET SETTINGS");
	
    	vdDebug_LogPrintf("ETHERNET_MODE..");
        memset(&st, 0x00, sizeof (CTMS_EthernetInfo));
        
        strcpy(st.strGateway, strTCP.szGetWay);
        strcpy(st.strMask, strTCP.szSubNetMask);
        st.bDHCP = strTCP.fDHCPEnable;

        strcpy(st.strRemoteIP, strTCT.szTMSRemoteIP);
        st.usRemotePort = strTCT.usTMSRemotePort;

/*
		CTOS_LCDTClearDisplay();
		vdDispTitleString("ETHERNET SETTINGS");

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", st.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", st.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
*/		
        usRes=CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &st);
    }
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
    {
    	vdDebug_LogPrintf("DIAL_UP_MODE..");

        //CTOS_LCDTClearDisplay();
        //vdDispTitleString("DIALUP SETTINGS");

        memset(&stmodem, 0x00, sizeof (CTMS_ModemInfo));
        stmodem.bMode = d_M_MODE_AYNC_FAST;
        stmodem.bHandShake = d_M_HANDSHAKE_V32BIS_AUTO_FB;
        stmodem.bCountryCode = d_M_COUNTRY_SINGAPORE;

        strcpy(stmodem.strRemotePhoneNum, strTCT.szTMSRemotePhone);
		vdDebug_LogPrintf("strRemotePhoneNum[%s]..",stmodem.strRemotePhoneNum);
        strcpy(stmodem.strID, strTCT.szTMSRemoteID);	
		vdDebug_LogPrintf("strID[%s]..",stmodem.strID);
        strcpy(stmodem.strPW, strTCT.szTMSRemotePW);
		vdDebug_LogPrintf("strPW[%s]..",stmodem.strPW);

        strcpy(stmodem.strRemoteIP, strTCT.szTMSRemoteIP);
        stmodem.usRemotePort = strTCT.usTMSRemotePort;

		stmodem.usPPPRetryCounter = 2;
		stmodem.ulPPPTimeout = 34463;
		stmodem.ulDialTimeout = 34463;
		vdDebug_LogPrintf("strRemoteIP[%s].usRemotePort=[%d].",stmodem.strRemoteIP,stmodem.usRemotePort);

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

/*
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stmodem.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stmodem.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
        
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PHONE: %s", stmodem.strRemotePhoneNum);         
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf); 
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "ID: %s", stmodem.strID);         
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stmodem.strPW);         
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
*/		
        usRes=CTOS_CTMSSetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
    }
    else if(GPRS_MODE == strTCT.inTMSComMode)
    {
		CTMS_GPRSInfo stgprs;

        //CTOS_LCDTClearDisplay();
        //vdDispTitleString("GPRS SETTINGS");
		
		memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
		CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
		
        if(strlen(stgprs.strAPN) > 0)
            strcpy(strTCP.szAPN, stgprs.strAPN);

        if(strlen(stgprs.strID) > 0)
            strcpy(strTCP.szUserName, stgprs.strID);

        if(strlen(stgprs.strPW) > 0)
		    strcpy(strTCP.szPassword, stgprs.strPW);
		
              vdDebug_LogPrintf("GPRS_MODE..");
        memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
        strcpy(stgprs.strAPN, strTCP.szAPN);
        strcpy(stgprs.strID, strTCP.szUserName);
        strcpy(stgprs.strPW, strTCP.szPassword);
 
        strcpy(stgprs.strRemoteIP, strTCT.szTMSRemoteIP);
        stgprs.usRemotePort = strTCT.usTMSRemotePort;
        stgprs.ulSIMReadyTimeout = 10000;
        stgprs.ulGPRSRegTimeout = 10000;
        stgprs.usPPPRetryCounter = 5;
        stgprs.ulPPPTimeout = 10000;
        stgprs.ulTCPConnectTimeout = 10000;

		//test
		stgprs.ulTCPTxTimeout=10000;
		stgprs.ulTCPRxTimeout=10000;
		stgprs.bSIMSlot=1;

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

/*
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stgprs.strRemoteIP);		 
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stgprs.usRemotePort);		 
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "APN: %s", stgprs.strAPN);		 
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "USER NAME: %s", stgprs.strID);		 
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stgprs.strPW);		 
        setLCDPrint(7, DISPLAY_POSITION_LEFT, szInputBuf);
*/		
        usRes=CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    }

/*
	setLCDPrint(8, DISPLAY_POSITION_LEFT, "ANY KEY TO CONTINUE");
	CTOS_KBDGet(&ckey);
*/
	return usRes;
}

