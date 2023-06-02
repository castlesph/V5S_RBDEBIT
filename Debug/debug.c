#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include "debug.h"
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>

#include "..\Includes\myEZLib.h"
#include "..\Includes\wub_lib.h"
#include "..\Includes\POSTypedef.h"

#include "..\FileModule/myFileFunc.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\CTOSInput.h"

#include <stdarg.h>
#include <Vwdleapi.h>

#define MAX_DEBUF_BUF 8096


static BYTE ifDebugMode = FALSE;
BOOL fDebugOpenedFlag = FALSE;

#define d_READY_TIMEOUT		100
#define d_DEBUG_PORT d_COM1

BYTE DebugLog[MAX_DEBUF_BUF + 2];
LONG DebugLen;
CTOS_RTC stRTC;
INT iDebugTOTimes = 0;
BYTE DebugPort = d_DEBUG_PORT;


BYTE byGetDebugMode(void)
{
    return ifDebugMode;
}

/****************
 * if bPort == 0xFF --> USB mode
 ****************/
void SetDebugMode(BYTE bMode, BYTE bPort)
{
	if (0 ==strTCT.byRS232DebugPort)
	{
		ifDebugMode = FALSE;
		return;
	}
	else
		ifDebugMode = TRUE;
}

void DebugInit(void)
{
    
    if (!ifDebugMode) return;

    DebugLen = 0;


	if (8 ==strTCT.byRS232DebugPort)
	{
		DebugPort = 0xFF;
        if(fDebugOpenedFlag == FALSE)    
        {
            CTOS_USBOpen();
            fDebugOpenedFlag = TRUE;
            
        }
        
	}
	
    if (1 ==strTCT.byRS232DebugPort)
    {
    	DebugPort= d_COM1;
        if(fDebugOpenedFlag == FALSE)    
        {
            CTOS_RS232Open(DebugPort, 115200, 'N', 8, 1);
            fDebugOpenedFlag = TRUE;
            
        }
    }

	if (2 ==strTCT.byRS232DebugPort)
	{
		DebugPort= d_COM2;
        if(fDebugOpenedFlag == FALSE)
        {
            CTOS_RS232Open(DebugPort, 115200, 'N', 8, 1);
            fDebugOpenedFlag = TRUE;
           
        }
	}
	
}

void DebugExport232(void)
{
	ULONG tick;
	USHORT ret;
	
	if (!ifDebugMode) return;
	
	tick = CTOS_TickGet();
	do {
        if (DebugPort == 0xFF)
          ret = CTOS_USBTxReady();
        else
          ret = CTOS_RS232TxReady(DebugPort);
		if (ret == d_OK)
			break;
		//CTOS_Delay(50);
	} while ((CTOS_TickGet() - tick) < d_READY_TIMEOUT);
	
	if (ret == d_OK) {
		DebugLog[DebugLen++] = 0x0D;
		DebugLog[DebugLen++] = 0x0A;
        if (DebugPort == 0xFF)
        {
			CTOS_USBTxFlush();			
			CTOS_USBTxData(DebugLog, DebugLen);
        }
        else
            CTOS_RS232TxData(DebugPort, DebugLog, DebugLen);
		tick = CTOS_TickGet();
		do {
          if (DebugPort == 0xFF)
          {
			CTOS_USBTxFlush();
			ret = CTOS_USBTxReady();
          }
          else
			ret = CTOS_RS232TxReady(DebugPort);
			if (ret == d_OK)
				break;
			//CTOS_Delay(50);
		} while ((CTOS_TickGet() - tick) < d_READY_TIMEOUT);
	}
    
    DebugLen = 0;
}

void DebugAddHEX(BYTE *title, BYTE *hex, USHORT len)
{
    if (0 ==strTCT.byRS232DebugPort)
        return;
    
	SetDebugMode(1, 0xFF);
	DebugInit();
	
	if (!ifDebugMode) return;

	if (len > (sizeof (DebugLog) / 2) - 8)
		len = (sizeof (DebugLog) / 2) - 8;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	//sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(DebugLog, "<%lu> ", CTOS_TickGet());
	DebugLen = wub_strlen(DebugLog);
	DebugLog[DebugLen++] = '[';
	DebugLog[DebugLen] = 0x00;
	wub_strcat(&DebugLog[DebugLen], title);
	DebugLen += wub_strlen(title);
	DebugLog[DebugLen++] = ']';
	DebugLog[DebugLen++] = ' ';
	DebugLen += wub_hex_2_str(hex, &DebugLog[DebugLen], len);
	
	DebugExport232();
}

void DebugAddINT(BYTE *title, LONG value)
{
	BYTE temp[50];

    if (0 ==strTCT.byRS232DebugPort)
        return;
    
	SetDebugMode(1, 0xFF);
	DebugInit();
	
	if (!ifDebugMode) return;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	//sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(DebugLog, "<%lu> ", CTOS_TickGet());
	wub_strcat(DebugLog, "[");
	wub_strcat(DebugLog, title);
	wub_strcat(DebugLog, "] ");
	wub_memset(temp, 0x00, sizeof (temp));
	sprintf(temp, "%ld", value);
	wub_strcat(DebugLog, temp);
	
	DebugLen = wub_strlen(DebugLog);
	DebugExport232();
}

void DebugAddIntX(BYTE *title, LONG value)
{
	BYTE temp[50];

    if (0 ==strTCT.byRS232DebugPort)
        return;
    
	SetDebugMode(1, 0xFF);
	DebugInit();


	if (!ifDebugMode) return;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	//sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(DebugLog, "<%lu> ", CTOS_TickGet());
	wub_strcat(DebugLog, "[");
	wub_strcat(DebugLog, title);
	wub_strcat(DebugLog, "] ");
	
	wub_memset(temp, 0x00, sizeof (temp));
	sprintf(temp, "0x%08lX", value);
	wub_strcat(DebugLog, temp);
	
	DebugLen = wub_strlen(DebugLog);
	DebugExport232();
}

void DebugAddSTR(BYTE *title, BYTE *baMsg, USHORT len)
{

    if (0 ==strTCT.byRS232DebugPort)
        return;
    
	SetDebugMode(1, 0xFF);
	DebugInit();
	
	if (!ifDebugMode) return;
	
	if (baMsg == NULL) return;
	
	CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	//sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(DebugLog, "<%lu> ", CTOS_TickGet());
	DebugLen = wub_strlen(DebugLog);
	
	DebugLog[DebugLen++] = '[';
	wub_strcat(&DebugLog[DebugLen], title);
	DebugLen += wub_strlen(title);
	DebugLog[DebugLen++] = ']';
	DebugLog[DebugLen++] = ' ';
	
	wub_memcpy(&DebugLog[DebugLen], baMsg, strlen(baMsg));
	DebugLen += strlen(baMsg);
	
	DebugExport232();
}


void vdDebug_LogPrintf_old(const char* fmt, ...)
{
    char printBuf[MAX_DEBUF_BUF];
	char msg[MAX_DEBUF_BUF];
	char space[100];
	int inSendLen;
	va_list marker;
	int j = 0;
    char szAPName[25];
	int inAPPID;

    if (0 ==strTCT.byRS232DebugPort)
        return;
    
    memset(msg, 0x00, sizeof(msg));
	memset(printBuf, 0x00, sizeof(printBuf));
	memset(space, 0x00, sizeof(space));
	
	va_start( marker, fmt );
	vsprintf( msg, fmt, marker );
	va_end( marker );
	
	memset(printBuf, 0x00, sizeof(printBuf));		
	strcat(printBuf, msg);
	strcat(printBuf, space);
	strcat(printBuf ,"\n" );
	
	inSendLen = strlen(printBuf);


	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);


    DebugAddSTR(szAPName,printBuf,inSendLen);
}


void PrintDebugMessage(const char* filePath, int lineNumber, const char * functionName, const char* format, ...)
{
    static char appName[25] = "";
    int appId = 0;
    va_list arg_list;
    char buffer[MAX_DEBUF_BUF];
    char debugLine[MAX_DEBUF_BUF]; // [application_name]|[filename:line_number]|[tick]: <debug msg>
    int result;
    //unsigned long tick;
	char szTickLog[50+1];
    char * pFileName;

	if (0 ==strTCT.byRS232DebugPort)
		return;
	
    va_start(arg_list, format);
    
    result = vsnprintf(buffer, sizeof(buffer), format, arg_list);
    
    va_end(arg_list);
    
    if (result == sizeof(buffer))
        buffer[result - 1] = 0;
    else 
        buffer[result] = 0;
    
    if (strlen(appName) == 0)     
        inMultiAP_CurrentAPNamePID(appName, &appId);    
    
    //tick = CTOS_TickGet();
    memset(szTickLog,0x00,sizeof(szTickLog));
	CTOS_RTCGet(&stRTC);
	//sprintf(szTickLog, "<%02d:%02d:%02d>", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(szTickLog, "<%ld>", CTOS_TickGet());
    
    pFileName = strrchr(filePath, '/');
    if (pFileName != NULL) 
        pFileName++;
    else 
        pFileName = (char*)filePath;
    sprintf(debugLine, "[%s]%s[%s:%d]: %s\n", appName, szTickLog,pFileName, lineNumber, buffer);
    
    //printf(debugLine);
    
	SetDebugMode(1, 0xFF);
	DebugInit();
	
	if (!ifDebugMode) return;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	
	wub_memcpy(&DebugLog[DebugLen], debugLine, strlen(debugLine));
	DebugLen += strlen(debugLine);
	
	DebugExport232();
	
    CTOS_Delay(1);
}

void vdPCIDebug_HexPrintf(char *display,char *szHexbuf, int len)
{
	char szbuf[MAX_DEBUF_BUF];
	memset(szbuf,0x00,sizeof(szbuf));

	if (0 ==strTCT.byRS232DebugPort)
        return;
	
	wub_hex_2_str(szHexbuf,szbuf,len);
	vdDebug_LogPrintf("[%s]=[%s]",display,szbuf);
}