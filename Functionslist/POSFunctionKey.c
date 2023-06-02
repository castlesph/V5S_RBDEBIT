
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <ctosapi.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "POSFunctionsList.h"
#include "..\Includes\POSTypedef.h"

#include <ctosapi.h>
#include <semaphore.h>
#include <pthread.h>

#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSAuth.h"
#include "..\Includes\POSConfig.h"
#include "..\Includes\POSOffline.h"
#include "..\Includes\POSRefund.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSBatch.h"
#include "..\Includes\POSSettlement.h"

#include "..\Includes\CTOSInput.h"

#include "..\ui\Display.h"
#include "..\print\print.h"
#include "..\Debug\Debug.h"
#include "..\Includes\DMenu.h"
#include "..\Ctls\POSWave.h"

#include "POSFunctionKey.h"

#include "..\FileModule\myFileFunc.h"
#include "..\Database\DatabaseFunc.h"

#include "../Comm/V5Comm.h"

/*display host name, nii, tid, mid*/
void vdDisplayHostDetails(void)    
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
	char szStrNII[4+1];
    BYTE key;
	    
    //by host and merchant
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;
	
    strHDT.inHostIndex = shHostIndex;
            
    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return;

	CTOS_LCDTClearDisplay();
	vdDispTitleString("TERMINAL CONFIG");

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%s", strHDT.szHostLabel);
	CTOS_LCDTPrintXY(1, 3, szStr);


	memset(szStrNII, 0, sizeof(szStrNII));
	wub_hex_2_str(strHDT.szNII, szStrNII, 2);
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "NII : %s", szStrNII);
	CTOS_LCDTPrintXY(1, 5, szStr);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "TID : %s", strMMT[0].szTID);
	CTOS_LCDTPrintXY(1, 6, szStr);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "MID : %s", strMMT[0].szMID);
	CTOS_LCDTPrintXY(1, 7, szStr);

    CTOS_KBDGet(&key);
	
    return; 
}

void vdTraceNo(BOOL fGetTraceNo)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
	inTCTRead(1);
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("TRACE NO.");

	CTOS_LCDTPrintXY(1, 3, "TRACE NO.");

	ulTraceNo=wub_bcd_2_long(strTCT.szInvoiceNo,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%06ld", ulTraceNo);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fGetTraceNo == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
                if(ulTraceNo >= atoi(strOut))
                {
					vdDisplayErrorMsg2(1, 8, "INVALID INPUT, VALUE", "MUST BE GREATER!", MSG_TYPE_ERROR);
					vduiClearBelow(7);
                }
				else
				{
                    ulTraceNo=atoi(strOut);
                     memset(szStr, 0, sizeof(szStr));
                    sprintf(szStr, "%06ld", ulTraceNo);
                    inAscii2Bcd(szStr, strTCT.szInvoiceNo, 3);
                    inTCTSave(1);
                    break;
				}
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

void vdSTANNo(BOOL fGetSTABNo)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
    //by host and merchant
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;
		
	CTOS_LCDTClearDisplay();
	vdDispTitleString("STAN NO.");

	CTOS_LCDTPrintXY(1, 3, "STAN NO.");

	ulTraceNo=wub_bcd_2_long(strHDT.szTraceNo,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%06ld", ulTraceNo);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fGetSTABNo == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
                ulTraceNo=atoi(strOut);
				memset(szStr, 0, sizeof(szStr));
				sprintf(szStr, "%06ld", ulTraceNo);
				inAscii2Bcd(szStr, strHDT.szTraceNo, 3);
				inHDTSave(shHostIndex);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

void vdBatchNo(BOOL fGetBatchNo)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
    //by host and merchant
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    strHDT.inHostIndex = shHostIndex;
            
    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return;
		
	CTOS_LCDTClearDisplay();
	vdDispTitleString("BATCH NO.");

	CTOS_LCDTPrintXY(1, 3, "BATCH NO.");

	ulTraceNo=wub_bcd_2_long(strMMT[0].szBatchNo,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%06ld", ulTraceNo);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fGetBatchNo == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
                ulTraceNo=atoi(strOut);
				memset(szStr, 0, sizeof(szStr));
				sprintf(szStr, "%06ld", ulTraceNo);
				inAscii2Bcd(szStr, strMMT[0].szBatchNo, 3);
				inMMTSave(strMMT[0].MITid);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }
	}
}

void vdModifyPassword(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "CHANGE PASSWORD";
    char szHostMenu[1024];
    char szHostName[50][100];
    int inCPTID[50];
    int inLoop = 0;
    int inRet;
	BYTE strOut[30];
	USHORT usLen=0;
    USHORT shMaxLen=12, shMinLen=4;
	
    memset(szHostMenu, 0, sizeof(szHostMenu));
	
    strcpy((char *)szHostMenu, (char *)"SUPER PASSWORD \n"); 
	strcat((char *)szHostMenu, (char *)"SYSTEM PASSOWRD\n");
	strcat((char *)szHostMenu, (char *)"ENGINEER PASSWORD");
	
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

    if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return;  
    }

    if(key > 0)
    {
		UCHAR szPassword[12+1];
		
        if(d_KBD_CANCEL == key)
            return;

		CTOS_LCDTClearDisplay();
		vdDispTitleString("MODIFY PASSWORD");

        inTCTRead(1);

		memset(szPassword, 0, sizeof(szPassword));
        if(key == 1)
            strcpy(szPassword, strTCT.szSuperPW);
		else if(key == 2)
            strcpy(szPassword, strTCT.szSystemPW);
		else if(key == 3)
            strcpy(szPassword, strTCT.szEngineerPW);

        while(1)
        {   
			if (strTCT.byTerminalType == 2)
				clearLine(V3_ERROR_LINE_ROW);
			else	
				clearLine(8);
            CTOS_LCDTPrintXY(1, 7, "Old:");
            memset(strOut,0x00, sizeof(strOut));
            inRet = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                return;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
				if(memcmp(strOut, szPassword, strlen(szPassword)) == 0) 	
                    break;
				else
					vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
            }   
        }
		
        while(1)
        {   
			if (strTCT.byTerminalType == 2)
				clearLine(V3_ERROR_LINE_ROW);
			else	
				clearLine(8);
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                return;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
				if(key == 1)
					strcpy(strTCT.szSuperPW, strOut);
				else if(key == 2)
					strcpy(strTCT.szSystemPW, strOut);
				else if(key == 3)
					strcpy(strTCT.szEngineerPW, strOut);
				inTCTSave(1);
                break;    
            }   
        }
		
    }
}

void vdSetDebugISO(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulTraceNo=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("DEBUG");

	CTOS_LCDTPrintXY(1, 3, "DEBUG");

    inTCTRead(1);

	if(strTCT.fPrintISOMessage == TRUE)
	  CTOS_LCDTPrintXY(1, 4, "1");  	
	else
	  CTOS_LCDTPrintXY(1, 4, "0"); 	

    CTOS_LCDTPrintXY(1, 5, "0-DISABLE      1-ENABLE");     

    while(1)
    {   
		if (strTCT.byTerminalType == 2)
            clearLine(V3_ERROR_LINE_ROW);
		else	
            clearLine(8);
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(strOut,0x00, sizeof(strOut));
        inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            break;
        else if(0 == inRet )
            break;
        else if(inRet>=1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.fPrintISOMessage=1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.fPrintISOMessage=0;
                 }
                 inTCTSave(1);
				 break;
             }
             else
             {
			     vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
             }
        }   
        if(inRet == d_KBD_CANCEL)
            break;
    }
}

void vdShareCom(BOOL fGetShareCom)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
    ULONG ulTraceNo=0L;
    BYTE strOut[30];
    USHORT usLen=0;
    
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("SHARE COM");
    
    CTOS_LCDTPrintXY(1, 3, "SHARE COM");
    
    inTCTRead(1);
    
    if(strTCT.fShareComEnable== TRUE)
        CTOS_LCDTPrintXY(1, 4, "1");		
    else
        CTOS_LCDTPrintXY(1, 4, "0");	
    
    CTOS_LCDTPrintXY(1, 5, "0-DISABLE     1-ENABLE");	   
    
    while(1)
    {	
        if (strTCT.fShareComEnable == 2)
            clearLine(V3_ERROR_LINE_ROW);
        else	
            clearLine(8);
		
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(strOut,0x00, sizeof(strOut));
        inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            break;
        else if(0 == inRet )
            break;
        else if(inRet>=1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31)
            {
                if(strOut[0] == 0x31)
                {
                    strTCT.fShareComEnable=1;
                }
                if(strOut[0] == 0x30)
                {
                    strTCT.fShareComEnable=0;
                }
                inTCTSave(1);
                break;
            }
            else
            {
                vdDisplayErrorMsg(1, 8, "INVALID INPUT");	
            }
        }	
		
        if(inRet == d_KBD_CANCEL)
            break;
    }
}


int inFunctionKeyPassword(unsigned char *ptrTitle)
{
    //char szPassword[6+1]={0}; 
	BYTE strOut[6+1]={0};
	int inRet;
	USHORT shMaxLen, shMinLen;
	
    inTCTRead(1);
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString(ptrTitle);

    while(1)
    {	
    	shMinLen = PWD_MIN_LEN;
		shMaxLen = PWD_MAX_LEN;
		
		if (strTCT.byTerminalType == 2)
            clearLine(V3_ERROR_LINE_ROW);
		else	
            clearLine(8);
        CTOS_LCDTPrintXY(1, 7, "PASSWORD:");
        memset(strOut,0x00, sizeof(strOut));
        inRet = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        if (inRet == d_KBD_CANCEL )
            return d_NO;
        else if(0 == inRet )
            return d_NO;
        else if(inRet>=1)
        {
            if((strcmp(strOut,strTCT.szSuperPW) == 0) || 
            (strcmp(strOut,strTCT.szSystemPW) == 0)||
            (strcmp(strOut,strTCT.szEngineerPW) == 0))
            {		
                return d_OK;
            }
            else
                vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
        }	
    }
}

void vdSetConnectiontype(void)
{
	int inResult;
	short shHostIndex;
    BYTE strOut[21+1], strtemp[21+1];
    USHORT usLen=1;
	BOOL BolDetachLANChange=FALSE,BolDetachDIALChangeSetting=FALSE,BolDetachGPRSChangeSetting=FALSE;
	
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;
                
    inResult = inCPTRead(shHostIndex);
    if(inResult != d_OK)
        return;

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SELECT COMMS");
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Connection Type");
        if((strCPT.inCommunicationMode)== ETHERNET_MODE)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");
        if((strCPT.inCommunicationMode)== DIAL_UP_MODE)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");        
        if((strCPT.inCommunicationMode)== GPRS_MODE)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
        
        CTOS_LCDTPrintXY(1, 5, "0-DIAL-UP     1-LAN");
        CTOS_LCDTPrintXY(1, 6, "2-GPRS             ");
   
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        inResult=shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (inResult == d_KBD_CANCEL )
            break;
        else if(0 == inResult)
            break;
        else if(inResult==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32)
            {
                 if(strOut[0] == 0x31)
                 {
                        strCPT.inCommunicationMode = ETHERNET_MODE;
                        BolDetachLANChange = TRUE;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strCPT.inCommunicationMode = DIAL_UP_MODE;
                        BolDetachDIALChangeSetting = TRUE;
                 }
                 if(strOut[0] == 0x32)
                 {
                        strCPT.inCommunicationMode = GPRS_MODE;
                        BolDetachGPRSChangeSetting = TRUE;
                 }
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiClearBelow(6);
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"CONNECTION MODE");
                CTOS_Delay(2000);       
            }
        }
    }
    inResult = inCPTSave(shHostIndex);
    inResult = inCPTRead(shHostIndex);

    if((BolDetachLANChange==TRUE) || (BolDetachDIALChangeSetting==TRUE) || (BolDetachGPRSChangeSetting==TRUE))
    {
        srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
        vduiClearBelow(3);
        CTOS_LCDTPrintXY (1,7, "Please Wait 	"); 	  
        if(strCPT.inCommunicationMode == ETHERNET_MODE)
            CTOS_LCDTPrintXY(1, 8, "Init LAN... 	");
        else if(strCPT.inCommunicationMode == GPRS_MODE)
            CTOS_LCDTPrintXY(1, 8, "Init GPRS...	 ");
		else if(strCPT.inCommunicationMode == DIAL_UP_MODE)
            CTOS_LCDTPrintXY(1, 8, "Init Modem...	 ");
		
        if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
        {        
            vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
            return;
        }
        
        srCommFuncPoint.inInitComm(&srTransRec,strCPT.inCommunicationMode);
        srCommFuncPoint.inGetCommConfig(&srTransRec);
        srCommFuncPoint.inSetCommConfig(&srTransRec);		
    }

}		   
	
void vdCTOS_FunctionKey(void)
{
    int ret;
    BYTE strOut[2], szErrMsg[21+1];
    USHORT usLen=2;
	
    CTOS_LCDTClearDisplay();
    vdDispTitleString("FUNCTION MENU");
    
    CTOS_LCDTPrintXY(1, 7, "FUNCTION?");
	
    memset(strOut, 0, sizeof(strOut));
    ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
    if(ret==d_KBD_ENTER)
    {
		switch(atoi(strOut))
		{
            case 0: /*Print terminal config*/
                vdCTOS_PrintEMVTerminalConfig();				
            break;
            
            case 1: /*Print terminal/host info*/
                vdCTOS_HostInfo();				
            break;
            
            case 4:
                vdDisplayHostDetails();				
            break;
            
            case 5:
				inCTOS_BATCH_TOTAL_Process();
            break;
            
            case 6: /*get and display trace no.*/
                vdTraceNo(FALSE);				
            break;
            
            case 7: /*set trace no.*/
                if(inFunctionKeyPassword("TRACE NO.") != d_OK)
                    return;					
                vdTraceNo(TRUE);				
            break;
            
            case 8: /*get and display batch no.*/
                if(inFunctionKeyPassword("BATCH NO.") != d_OK)
                    return;					
                vdBatchNo(TRUE);
            break;
            
            case 9: /*set batch no.*/
                if(inFunctionKeyPassword("CONNECTION TYPE") != d_OK)
                    return;					
				vdSetConnectiontype();
            break;
            
            case 10:
            break;
            
            case 11:
            break;

			case 16:
                if(inFunctionKeyPassword("SHARE COM") != d_OK)
                    return;					
                vdShareCom(TRUE);
            break;				
            					
            case 18:
                vdSTANNo(FALSE);				
            break;
            
            case 19:
                if(inFunctionKeyPassword("STAN NO.") != d_OK)
                    return;									
                vdSTANNo(TRUE);  				
            break;

			case 22:
                if(inFunctionKeyPassword("DEV MENU") != d_OK)
                    return;									
				vdCTOS_DevFunctionKey();
			break;
			
            case 50:
                if(inFunctionKeyPassword("BATCH REVIEW") != d_OK)
                    return;					
                inCTOS_BATCH_REVIEW_Process();			  	
            break;
            
            case 51:
            break;
            
            case 71: /*Reprint any transaction receipt*/
                if(inFunctionKeyPassword("REPRINT ANY") != d_OK)
                    return;					
                inCTOS_REPRINT_ANY();				
            break;
            
            case 72: /*Reprint last transaction receipt*/				
                if(inFunctionKeyPassword("REPRINT LAST") != d_OK)
                    return;					
                inCTOS_REPRINT_LAST();				
            break;
            
            case 73: /*Reprint last settement receipt*/
                if(inFunctionKeyPassword("REPRINT LAST SETTLE") != d_OK)
                    return;					
                inCTOS_REPRINTF_LAST_SETTLEMENT();				
            break;
            
            case 75: /*Print Detail Report*/
			    inCTOS_PRINTF_DETAIL();	
            break;
            
            case 76: /*Print Summary Report*/
                inCTOS_PRINTF_SUMMARY();				
            break;
            
            case 81: /*CRC Report*/
                if(inFunctionKeyPassword("CRC REPORT") != d_OK)
                    return;					
                vdCTOS_PrintCRC();				
            break;
            
            case 90:
                vdModifyPassword();				
            break;
            
            case 91:
            break;
            
            case 93:
                if(inFunctionKeyPassword("DEBUG") != d_OK)
                    return;					
                vdSetDebugISO();				
            break;
            
            case 97: /*Clear Reversal*/
                if(inFunctionKeyPassword("CLEAR REVERSAL") != d_OK)
                    return;					
                vdCTOS_DeleteReversal();				
            break;
            
            case 98: /*Delete Batch*/
                if(inFunctionKeyPassword("DELETE BATCH") != d_OK)
                    return;					
                vdCTOS_DeleteBatch(); 				
            break;
            
            case 99: /*restart terminal*/
                vdCTOS_uiRestart();				
            break;	

			default:
			break;	
		}
		
		memset(szErrMsg,0x00,sizeof(szErrMsg));
		if (inGetErrorMessage(szErrMsg) > 0)
		{
			vdDisplayErrorMsg(1, 8, szErrMsg);
			vdSetErrorMessage("");
		}    
    }
}

void vdTerminalType(BOOL fTerminalType)
{
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulValue=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
	inTCTRead(1);

	CTOS_LCDTClearDisplay();
	vdDispTitleString("TERMINAL TYPE");

	CTOS_LCDTPrintXY(1, 3, "TERMINAL TYPE");

	//ulValue=wub_bcd_2_long(strTCT.byTerminalType,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", strTCT.byTerminalType);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fTerminalType == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
				strTCT.byTerminalType=(unsigned char)strOut[0] & 0x0F;
				inTCTMenuSave(1);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

void vdHostEnable(int inHostIndex, BOOL fEnable)
{
	inHDTRead(inHostIndex);
	strHDT.fHostEnable=fEnable;
	inHDTSave(inHostIndex);
}

void vdSelectPhase(void)
{
    //BYTE key;
    int inRet;
    char szDebug[40 + 1]={0};
    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, key=0; 
        
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    
    strcpy(szHeaderString, "SELECT PHASE");
    strcat(szChoiceMsg,"FLEET \n");
    strcat(szChoiceMsg,"FLEET/DINERS");
    key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    
    if (key > 0)
        {
        if (key == 1)
        {
            vdHostEnable(1, TRUE);
            vdHostEnable(2, TRUE);
            vdHostEnable(3, TRUE);
            vdHostEnable(4, TRUE);
            vdHostEnable(5, FALSE);
            vdHostEnable(6, FALSE);
            vdHostEnable(7, FALSE);
            vdHostEnable(8, FALSE);
            vdHostEnable(9, FALSE);
            vdHostEnable(10, FALSE);
            vdHostEnable(11, FALSE);
            vdHostEnable(12, TRUE);
            vdHostEnable(13, FALSE);	
            vdHostEnable(14, FALSE);	
            vdHostEnable(15, FALSE);	
            vdHostEnable(16, FALSE);	
            vdHostEnable(17, FALSE);	
            vdHostEnable(18, FALSE);	
            
            inTCTRead(1);       
            strTCT.inMenuid=2;
            inTCTMenuSave(1);
        }
        else if (key == 2)
        {
            //vdHostEnable(1, TRUE);
            //vdHostEnable(2, TRUE);
            //vdHostEnable(3, TRUE);
            //vdHostEnable(4, TRUE);
            //vdHostEnable(5, FALSE);
            vdHostEnable(6, TRUE);
            vdHostEnable(7, TRUE);
            vdHostEnable(8, TRUE);
            vdHostEnable(9, TRUE);
            vdHostEnable(10, TRUE);
            vdHostEnable(11, TRUE);
            vdHostEnable(12, TRUE);
            //vdHostEnable(13, FALSE);	
            vdHostEnable(14, TRUE);	
            vdHostEnable(15, TRUE);	
            vdHostEnable(16, TRUE);	
            vdHostEnable(17, TRUE);	
            vdHostEnable(18, TRUE);	
            
            inTCTRead(1);		
            strTCT.inMenuid=3;
            inTCTMenuSave(1);
        }
        
        if (key == d_KBD_CANCEL)
        {
            return;
        }        
        return;
    }

return;
}


void vdMenuID(BOOL fTerminalType)
{
    int inResult,inRet;
    char szStr[d_LINE_SIZE + 1];
    BYTE key;
	ULONG ulValue=0L;
	BYTE strOut[30];
	USHORT usLen=0;
	
	inTCTRead(1);
	
	CTOS_LCDTClearDisplay();
	vdDispTitleString("MENU ID");

	CTOS_LCDTPrintXY(1, 3, "MENU ID");

	//ulValue=wub_bcd_2_long(strTCT.byTerminalType,3); 
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%d", strTCT.inMenuid);
	CTOS_LCDTPrintXY(1, 4, szStr);

    if(fTerminalType == FALSE)	
        CTOS_KBDGet(&key);
	else
	{
        while(1)
        {   
            CTOS_LCDTPrintXY(1, 7, "New:");
            memset(strOut,0x00, sizeof(strOut));
            inRet= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
            if (inRet == d_KBD_CANCEL )
                break;
            else if(0 == inRet )
                break;
            else if(inRet>=1)
            {
				strTCT.inMenuid=atoi(strOut);
				inTCTMenuSave(1);
                break;
            }   
            if(inRet == d_KBD_CANCEL)
                break;
        }		
	}
}

void vdCTOS_DevFunctionKey(void)
{
    int ret;
    BYTE strOut[2], szErrMsg[21+1];
    USHORT usLen=2;
	
    CTOS_LCDTClearDisplay();
    vdDispTitleString("DEV MENU");
    
    CTOS_LCDTPrintXY(1, 7, "FUNCTION?");
	
    memset(strOut, 0, sizeof(strOut));
    ret = InputString(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
    if(ret==d_KBD_ENTER)
    {
		switch(atoi(strOut))
		{
            case 1: /*set trace no.*/
                if(inFunctionKeyPassword("DOWNLOAD") != d_OK)
                    return; 												
                vdCTOSS_DownloadMode();
            break;
			
			case 2:
                if(inFunctionKeyPassword("TERMINAL TYPE") != d_OK)
                    return;													
				vdTerminalType(TRUE);
			break;

            case 3:
                if(inFunctionKeyPassword("MENU ID") != d_OK)
                    return;													
                vdMenuID(TRUE);
            break;

			case 4:
                if(inFunctionKeyPassword("SELECT PHASE") != d_OK)
                    return;													
				vdSelectPhase();
			break;	

			default:
			break;	
		}
		
		memset(szErrMsg,0x00,sizeof(szErrMsg));
		if (inGetErrorMessage(szErrMsg) > 0)
		{
			vdDisplayErrorMsg(1, 8, szErrMsg);
			vdSetErrorMessage("");
		}    
    }
}

