
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <error.h>         //perror
#include <stdio.h>
#include <errno.h>         //errno

#include "../Includes/wub_lib.h"
#include "../Includes/msg.h"
#include "../Includes/myEZLib.h"

#include "../FileModule/myFileFunc.h"
#include "../Includes/POSTypedef.h"
#include "V5Comm.h"
#include "../UI/Display.h"
#include "..\Debug\Debug.h"

#include "..\ApTrans\MultiShareCOM.h"
#include "..\Includes\POSTrans.h"
#include "../Database/bvt.h"


struct sockaddr_in	sockHost;
int sockType;

#define GPRS_RX_BUF_SIZE (1500) // must bigger than TCP windows size 1460
#define TCP_GPRS_STATE_NOERROR  0x00
#define TCP_GPRS_STATE_NOERROR2  0x10000000

#define TCP_GPRS_SERVER_CONNECTING  0x22
#define TCP_GPRS_STATE_SENDING2   0x42
#define TCP_GPRS_STATE_RECEIVING2 0x82
#define TCP_GPRS_STATE_DISCONNECTING2 0x102

BYTE Temp_Ip_Addr[] = "192.168.1.107";    //IP Address
BYTE Temp_Mask[] = "255.255.255.0";     //Mask
BYTE Temp_GateWay[] = "192.168.1.1";        //Gateway IP
BYTE Temp_Host_Ip[] = "192.168.1.106";  //Default Host IP 
BYTE Temp_Host_Port[] = "8012";            //Default Host Port

BYTE Temp_Phone_Num[] = "23301900,,124";        //Default Host Port

static USHORT shRet, ret;
DWORD dwStatus;
char szDebugBrffer[100];
BYTE key;
BYTE bgGPRSSocket;
int isDialUp, inTerminalCommunicationMode,isPredial;

USHORT GPRSCONNETSTATUS;
static USHORT inEthernetOpen = 0;
BOOL fGPRSConnectOK;
static USHORT inGPRSOpen = 0;

static int 		sockHandle = -1;

ULONG ulStrToLong(IN BYTE *baStr)
{
    ULONG ulValue;

    if (baStr == NULL)
        return 0;

    ulValue = 0;
    while (*baStr != 0)
    {
        if ( (*baStr >= '0') && (*baStr <= '9') )
            ulValue = (ulValue * 10) + (*baStr - '0');
        else
            return 0;

        baStr++;
    }

    return ulValue;
}


USHORT ushStrToShort(IN BYTE *baStr)
{
    return (USHORT) ulStrToLong(baStr);
}

BYTE bUnSingeLongToStr(IN ULONG ulValue, OUT BYTE *baStr)
{
    BYTE    bDigit;
    ULONG    i;

    bDigit = 0;
    i = ulValue;

    while(i)
    {
        i /= 10;
        bDigit++;
    }
    
    if(!bDigit)
        bDigit = 1;
    
    for(i = 0; i < bDigit; i++)
    {
        baStr[bDigit - 1 - i] = (BYTE)(ulValue % 10) + '0';
        ulValue /= 10;        
    }
    
    baStr[bDigit] = '\0';
    
    return bDigit;
}


int inFirstInitConnectFuncPoint(void)
{
    srCommFuncPoint.inInitComm = 0;
    srCommFuncPoint.inGetCommConfig = 0;
    srCommFuncPoint.inSetCommConfig = 0;
	srCommFuncPoint.inCheckComm = 0;
    srCommFuncPoint.inConnect = 0;
    srCommFuncPoint.inDisconnect = 0;
    srCommFuncPoint.inSendData = 0;
    srCommFuncPoint.inRecData = 0;
    srCommFuncPoint.inEndComm = 0;
    
    return ST_SUCCESS;
}
#if 0
int inInitConnectFuncPoint(TRANS_DATA_TABLE *srTransPara)
{
        
    if(srTransPara->usTerminalCommunicationMode == ETHERNET_MODE)
    {   
        vdMyEZLib_LogPrintf("**usTerminalCommunicationMode ETHERNET_MODE START**");
        //if( strTCT.fShareComEnable != TRUE)
        //{
            srCommFuncPoint.inInitComm = inEthernet_InitEthernet;
            srCommFuncPoint.inGetCommConfig = inEthernet_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inEthernet_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inEthernet_CheckComm;
            srCommFuncPoint.inConnect = inEthernet_ConnectHost;
            srCommFuncPoint.inDisconnect = inEthernet_Disconnection;
            srCommFuncPoint.inSendData = inEthernet_SendData;
            srCommFuncPoint.inRecData = inEthernet_ReceiveData;
            srCommFuncPoint.inEndComm = inEthernet_CutOffConnection;
#if 0
		}
        else
        {
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
        }
#endif

    }
    else if(srTransPara->usTerminalCommunicationMode == DIAL_UP_MODE)
    {
        vdMyEZLib_LogPrintf("**usTerminalCommunicationMode MODEM_MODE START**");

//        if( strTCT.fShareComEnable != TRUE)
//        {
        
            srCommFuncPoint.inInitComm = inModem_InitModem;
            srCommFuncPoint.inGetCommConfig = inModem_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inModem_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inModem_CheckComm;
            srCommFuncPoint.inConnect = inModem_ConnectHost;
            srCommFuncPoint.inDisconnect = inModem_Disconnection;
            srCommFuncPoint.inSendData = inModem_SendData;

			if(strTCT.byTerminalModel == 2 /*V3_MOBILE*/)
				srCommFuncPoint.inRecData = inModem_ReceiveData_New;
			else
	            srCommFuncPoint.inRecData = inModem_ReceiveData;
	
            srCommFuncPoint.inEndComm = inModem_CutOffConnection;
#if 0
        }
        else
        {
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End; 
        }
#endif      
        
    }
    
    else if(srTransPara->usTerminalCommunicationMode == GPRS_MODE)
    {
    
        vdMyEZLib_LogPrintf("**usTerminalCommunicationMode GPRS_MODE START**");
#if 0
        if( strTCT.fShareComEnable != TRUE)
        {
           
            srCommFuncPoint.inInitComm = inGPRS_InitGPRS;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inGPRS_ConnectHost;
            srCommFuncPoint.inDisconnect = inGPRS_Disconnection;
            srCommFuncPoint.inSendData = inGPRS_SendData;
            srCommFuncPoint.inRecData = inGPRS_ReceiveData;
            srCommFuncPoint.inEndComm = inGPRS_CutOffConnection;
        }
        else
        {
#endif
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
            srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
        //}

    }
    return ST_SUCCESS;
}
#endif
int inCTOS_inDisconnect(void)
{
	int result=-1;

	vdSetInit_Connect(0);
    if(VS_TRUE == strTCT.fDemo)
    {
        vdDebug_LogPrintf("DEMO Call inDisconnect!!");
        result = d_OK;
    }
    else
    {
        vdDebug_LogPrintf("start inCTOS_inDisconnect result[%d],strTCT.fShareComEnable[%d]", result,strTCT.fShareComEnable);
		
		if(srTransRec.usTerminalCommunicationMode == DIAL_UP_MODE)
		{
            if(srCommFuncPoint.inDisconnect)
    	        result = srCommFuncPoint.inDisconnect(&srTransRec);
            vdDebug_LogPrintf("N0-inCTOS_inDisconnect result[%d]", result);

    		if(srCommFuncPoint.inEndComm)
    	        result = srCommFuncPoint.inEndComm(&srTransRec);
    		vdDebug_LogPrintf("inEndComm result[%d]", result);
			fCommAlreadyOPen = VS_FALSE;
		}
        else if( strTCT.fShareComEnable != TRUE)
        {
            if(srCommFuncPoint.inDisconnect)
    	        result = srCommFuncPoint.inDisconnect(&srTransRec);
            vdDebug_LogPrintf("N0-inCTOS_inDisconnect result[%d]", result);

    		if(srCommFuncPoint.inEndComm)
    	        result = srCommFuncPoint.inEndComm(&srTransRec);
    		vdDebug_LogPrintf("inEndComm result[%d]", result);

			
			//gcitra
			 fCommAlreadyOPen = VS_FALSE;			
			//gcitra
        }
        else
        {
            if(srCommFuncPoint.inDisconnect)
                result = srCommFuncPoint.inDisconnect(&srTransRec);
            vdDebug_LogPrintf("inCTOS_inDisconnect result[%d]", result);

			//gcitra
			 fCommAlreadyOPen = VS_FALSE;
			//gcitra
			
            if(srCommFuncPoint.inEndComm)
              result = srCommFuncPoint.inEndComm(&srTransRec);
            vdDebug_LogPrintf("inEndComm result[%d]", result);
        }

    }
    
	return result;
}

int inCTOS_CheckInitComm(int inCommType)
{
	
	BYTE bLength;
	int result;
	BYTE tmpbabuff[d_BUFF_SIZE];

    if(VS_TRUE == strTCT.fDemo)
    {
        return ST_SUCCESS;
    }
	
	//only check ETHERNET_MODE, only need do one time INit, please check dial up and GPRS
	if(inCommType == ETHERNET_MODE)
	{
        
		result=srCommFuncPoint.inInitComm(&srTransRec,ETHERNET_MODE);        

	}
	else if(inCommType == DIAL_UP_MODE)
	{
		result = srCommFuncPoint.inInitComm(&srTransRec,DIAL_UP_MODE);
	    srCommFuncPoint.inGetCommConfig(&srTransRec);
	    srCommFuncPoint.inSetCommConfig(&srTransRec); 

	}	
	else if(inCommType == GPRS_MODE)
	{

		result = srCommFuncPoint.inInitComm(&srTransRec,GPRS_MODE);
	    srCommFuncPoint.inGetCommConfig(&srTransRec);
	    srCommFuncPoint.inSetCommConfig(&srTransRec); 
    
	}
	//wifi-mod
	else if(inCommType == WIFI_MODE)
	{
		result = srCommFuncPoint.inInitComm(&srTransRec,WIFI_MODE);
	}
	//wifi-mod

	//return ST_SUCCESS;
	return result;
}


int inCTOS_InitComm(int inCommType)
	{
			
		if(inCommType == ETHERNET_MODE)
		{	
inGPRSOpen = 0;
	
//			if( strTCT.fShareComEnable != TRUE)
//			{
				inEthernetOpen = 0;
				vdDebug_LogPrintf("**usTerminalCommunicationMode ETHERNET_MODE START**");
				srCommFuncPoint.inInitComm = inEthernet_InitEthernet;
				srCommFuncPoint.inGetCommConfig = inEthernet_GetConfigFromTable;
				srCommFuncPoint.inSetCommConfig = inEthernet_SetConnectConfig;
				srCommFuncPoint.inCheckComm = inEthernet_CheckComm;
				srCommFuncPoint.inConnect = inEthernet_ConnectHost;
				srCommFuncPoint.inDisconnect = inEthernet_Disconnection;
				srCommFuncPoint.inSendData = inEthernet_SendData;
				srCommFuncPoint.inRecData = inEthernet_ReceiveData;
				srCommFuncPoint.inEndComm = inEthernet_CutOffConnection;
#if 0
			}
			else
			{
				srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
				srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
				srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
				srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
				srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
				srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
				srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
				srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
				srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
			}
#endif
		}
		else if(inCommType == DIAL_UP_MODE)
		{
				#if 1
				vdDebug_LogPrintf("**usTerminalCommunicationMode MODEM_MODE START**");
				srCommFuncPoint.inInitComm = inModem_InitModem;
				srCommFuncPoint.inGetCommConfig = inModem_GetConfigFromTable;
				srCommFuncPoint.inSetCommConfig = inModem_SetConnectConfig;
				srCommFuncPoint.inCheckComm = inModem_CheckComm;
				srCommFuncPoint.inConnect = inModem_ConnectHost;
				srCommFuncPoint.inDisconnect = inModem_Disconnection;
				srCommFuncPoint.inSendData = inModem_SendData;

				if(strTCT.byTerminalModel == 2 /*V3_MOBILE*/)
					srCommFuncPoint.inRecData = inModem_ReceiveData_New;
				else
	    	        srCommFuncPoint.inRecData = inModem_ReceiveData;

				srCommFuncPoint.inEndComm = inModem_CutOffConnection;
				#else
				if( strTCT.fShareComEnable != TRUE)
				{
					vdDebug_LogPrintf("**usTerminalCommunicationMode MODEM_MODE START**");
					srCommFuncPoint.inInitComm = inModem_InitModem;
					srCommFuncPoint.inGetCommConfig = inModem_GetConfigFromTable;
					srCommFuncPoint.inSetCommConfig = inModem_SetConnectConfig;
					srCommFuncPoint.inCheckComm = inModem_CheckComm;
					srCommFuncPoint.inConnect = inModem_ConnectHost;
					srCommFuncPoint.inDisconnect = inModem_Disconnection;
					srCommFuncPoint.inSendData = inModem_SendData;
					srCommFuncPoint.inRecData = inModem_ReceiveData;
					srCommFuncPoint.inEndComm = inModem_CutOffConnection;
				}
				else
				{
					srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
					srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
					srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
					srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
					srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
					srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
					srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
					srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
					srCommFuncPoint.inEndComm = inCTOSS_COMM_End; 
				}	 
 
#endif
			}
		
		else if(inCommType == GPRS_MODE)
		{
			vdDebug_LogPrintf("**usTerminalCommunicationMode GPRS_MODE START**");
	
				#if 1
				srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
				srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
				srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
				srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
				srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
				srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
				srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
				srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
				srCommFuncPoint.inEndComm = inCTOSS_COMM_End;
				#else
			if( strTCT.fShareComEnable != TRUE)
			{
				srCommFuncPoint.inInitComm = inGPRS_InitGPRS;
				srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
				srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
				srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
				srCommFuncPoint.inConnect = inGPRS_ConnectHost;
				srCommFuncPoint.inDisconnect = inGPRS_Disconnection;
				srCommFuncPoint.inSendData = inGPRS_SendData;
				srCommFuncPoint.inRecData = inGPRS_ReceiveData;
				srCommFuncPoint.inEndComm = inGPRS_CutOffConnection;
			}
			else
			{
				srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
				srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
				srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
				srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
				srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
				srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
				srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
				srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
				srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
			} 
				#endif
		}
		//wifi-mod
		else if(inCommType == WIFI_MODE)
		{
			    srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
				srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
				srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
				srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
				srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
				srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
				srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
				srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
				srCommFuncPoint.inEndComm = inCTOSS_COMM_End;   
			    #if 0
				vdDebug_LogPrintf("**usTerminalCommunicationMode WIFI_MODE START**fShareComEnable= [%d]",strTCT.fShareComEnable);
				if( strTCT.fShareComEnable != TRUE)
				{
					srCommFuncPoint.inInitComm = inDummyFlow;
					srCommFuncPoint.inGetCommConfig = inDummyFlow;
					srCommFuncPoint.inSetCommConfig = inDummyFlow;
					srCommFuncPoint.inCheckComm = inDummyFlow;
					srCommFuncPoint.inConnect = inDummyFlow;
					srCommFuncPoint.inDisconnect = inDummyFlow;
					srCommFuncPoint.inSendData = inDummyFlow;
					srCommFuncPoint.inRecData = inDummyFlow;
					srCommFuncPoint.inEndComm = inDummyFlow;
				}
				else
				{
					srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
					srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
					srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
					srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
					srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
					srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
					srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
					srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
					srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
				}	
				#endif
		}
		//wifi-mod


		return ST_SUCCESS;
	}




void vdcommGPRSDisplyState(DWORD State)
{
	 BYTE str[20];
         GPRSCONNETSTATUS=0x23B0;

	fGPRSConnectOK = FALSE;
	
	if(State == TCP_GPRS_STATE_ESTABLISHING)
		vdCTOS_DispStatusMessage("Establishing...");
	else if(State == TCP_GPRS_STATE_ESTABLISHED)
    {
            GPRSCONNETSTATUS=d_OK;	
						vdCTOS_DispStatusMessage("GRPS Established");
						fGPRSConnectOK = TRUE;
    }
	else if(State == TCP_GPRS_STATE_CONNECTING)
		vdCTOS_DispStatusMessage("GRPS Connecting");
	else if(State == TCP_GPRS_STATE_SENDING || State == TCP_GPRS_STATE_SENDING2 )
		vdCTOS_DispStatusMessage("PROCESSING..."); //aaronnino for BDOCLG ver 9.0 fix on issue #00140 CHANGE MESSAGE DISPLAY FOR SENDING... INTO PROCESSING... 1 of 4
	else if(State == TCP_GPRS_STATE_RECEIVING || State == TCP_GPRS_STATE_RECEIVING2)
		vdCTOS_DispStatusMessage("RECEIVING...");
	else if(State == TCP_GPRS_STATE_DISCONNECTING || State == TCP_GPRS_STATE_DISCONNECTING2)
		vdCTOS_DispStatusMessage("Disconnecting...");
	else if(State == TCP_GPRS_STATE_ONHOOKING)
		vdCTOS_DispStatusMessage("On Hooking...");
	else if(State == TCP_GPRS_SERVER_CONNECTING)
		vdCTOS_DispStatusMessage("Connecting...");
	else if(State == TCP_GPRS_STATE_NOERROR || State == TCP_GPRS_STATE_NOERROR2)
		vdCTOS_DispStatusMessage("OK");
	else 
	{
		sprintf(str, "Status = %X      ", State);        
		vdCTOS_DispStatusMessage(str);        
    }
}


int incommGPRSCheckState(USHORT RET, int TimeOutValue)
{
    BYTE key = 0;
    DWORD State;
	
    
    if (TimeOutValue!=GPRS_TIMEOUT_DISABLE)
    	CTOS_TimeOutSet(TIMER_ID_2, TimeOutValue);
    
    while(RET == d_TCP_IO_PROCESSING)
    {
    	RET = CTOS_TCP_GPRSStatus(&State);
        vdcommGPRSDisplyState(State);

        CTOS_KBDHit(&key);
        if(key == d_KBD_CANCEL)
           break;
		
        if (TimeOutValue!=GPRS_TIMEOUT_DISABLE)
        {
			if(CTOS_TimeOutCheck(TIMER_ID_2) == d_YES)
			{
				RET=d_TCP_IO_BUSY;
				vdCTOS_DispStatusMessage("TimeOut");
				break;
			}
		}
	    
    }
    vdcommGPRSDisplyState(State);
    if(RET != d_OK)
    {
        return FALSE;
    }
    //CTOS_Beep();
    return TRUE;
    
}

int inGPRS_InitGPRS(TRANS_DATA_TABLE *srTransPara,int mode)
{
  BYTE key,  i;
  BYTE baIP_S[] =  "\x00\x00\x00\x00";  
  BYTE baIP_G[4];              
  BOOL bolState;   
  int RET = d_TCP_IO_PROCESSING;
  DWORD State;
	
	vdMyEZLib_LogPrintf("**inGPRS_InitGPRS START**");
    CTOS_TCP_GPRSInit( );
        
        // GPRSOpen //
		#if 1
		vdCTOS_DispStatusMessage("Opening GPRS...");
		#else
			CTOS_LCDTPrintXY (1,7, "Please Wait 		"); 		
			CTOS_LCDTPrintXY (1,8, "Opening GPRS....");
		#endif	
	CTOS_TCP_GPRSClose();        
	incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);
  	ret = CTOS_TCP_GPRSOpen(baIP_S, strTCP.szAPN, strTCP.szUserName, strTCP.szPassword);
  	bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_DISABLE);
  	if(bolState != TRUE)
  	{
				vdCTOS_DispStatusMessage("GPRS Init Fail");
		CTOS_TCP_GPRSClose();
	  	CTOS_KBDGet ( &key );
        vdMyEZLib_LogPrintf(". CTOS_GPRSOpen Err=(%04x)",ret);
        vdMyEZLib_LogPrintf("**inGPRS_InitGPRS END**");
	  	return ST_ERROR;
	}
	else
	{
				inGPRSOpen= 1;
		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);//check state
		vduiClearBelow(8);
		ret = CTOS_TCP_GPRSGetIP(baIP_G);
        vdMyEZLib_LogPrintf(". CTOS_GPRSOpen Ret=(%04x)",ret);
        vdMyEZLib_LogPrintf("**inGPRS_InitGPRS END**");
		return ST_SUCCESS;
	}
        
 }



int inGPRS_GetConfigFromTable(TRANS_DATA_TABLE *srTransPara)
{
    vdMyEZLib_LogPrintf("**inGPRS_GetConfigFromTable START**");    
    
    vdMyEZLib_LogPrintf(". ----------GPRS Set Config---------------");
    
    vdMyEZLib_LogPrintf("strGPRSCarrier.strAPN[%s]", strTCP.szAPN); 
    vdMyEZLib_LogPrintf("strGPRSCarrier.strUID[%s]", strTCP.szUserName);
    vdMyEZLib_LogPrintf("strGPRSCarrier.strPWD[%s]", strTCP.szUserName);

	vdMyEZLib_LogPrintf("**inGPRS_GetConfigFromTable END**");
    return ST_SUCCESS;
}


int inGPRS_SetConnectConfig(TRANS_DATA_TABLE *srTransPara)
{
    vdMyEZLib_LogPrintf("**inGPRS_ setconnectconfig START**"); 
    vdMyEZLib_LogPrintf("**inGPRS_ setconnectconfig END**");
    return ST_SUCCESS;    
}

int inGPRS_CheckComm(TRANS_DATA_TABLE *srTransPara)
{
	return ST_SUCCESS; 
}

int inGPRS_ConnectHost(TRANS_DATA_TABLE *srTransPara)
{
  DWORD dwStatus;
  BOOL bolState;  
  USHORT iRetry = 0, usPort;
  BYTE strHostIP[50];
  int inResult = d_OK;

  int inHideConnectionDisplay = 0;

  inHideConnectionDisplay = get_env_int("BDOSM"); 


	vdMyEZLib_LogPrintf("**inGPRS_ ConnectHost START**"); 

	vdDispTransTitleCardTypeandTotal(srTransRec.byTransType);  /*BDO: Display TOTAL AMOUNT during connection -- sidumili*/
	
	#if 1
		/************************************************************************/
	  // sidumili: Connecting to Primary IP [START]
	  /************************************************************************/
		#if 1
		if (inHideConnectionDisplay == 0)
		     vdCTOS_DispStatusMessage("CONNECTING TO PRI IP");
		#else
		CTOS_LCDTPrintXY (1,7, "					");
		CTOS_LCDTPrintXY (1,8, "Connecting to Pri IP");
		CTOS_Delay(500);
		#endif

		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_ENABLE_VALUE);
		wub_ASCIIIP2hex(strCPT.szPriTxnHostIP,strHostIP);
		usPort = strCPT.inPriTxnHostPortNum;

		vdMyEZLib_LogPrintf("GPRSServer Pri IP[%s]: Port[%d]", strCPT.szPriTxnHostIP, usPort);

		ret = CTOS_TCP_GPRSConnectEx(&bgGPRSSocket, strHostIP, usPort);
		bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_ENABLE_VALUE);
		
		if(bolState != TRUE){
			#if 1
			vdCTOS_DispStatusMessage("Connect Failed");
			#else
			vdDisplayErrorMsg(1, 8, "Connect Fail	 ");
			CTOS_Delay(500);
			#endif
			srCommFuncPoint.inEndComm(&srTransRec);
		  vdDebug_LogPrintf("CTOS_TCP_GPRSConnectEx Pri ret[%d]-bolState[%d]",shRet, bolState);
			ret = srCommFuncPoint.inInitComm(&srTransRec,GPRS_MODE); 
			if (ret!=TRUE){
					#if 1
					vdCTOS_DispStatusMessage("Reset GPRS Fail");
					#else
					vdDisplayErrorMsg (1,8, "Reset GPRS Fail");
				  CTOS_Delay(500);
					#endif
					return ST_ERROR;
			}
			inResult = d_NO;
		}
		else{
			
			return(ST_SUCCESS);
		}
			
		/************************************************************************/
	  // sidumili: Connecting to Primary IP [END]
	  /************************************************************************/

		/************************************************************************/
	  // sidumili: Connecting to Secondary IP [START]
	  /************************************************************************/
		#if 1
		if (inHideConnectionDisplay == 0)
		    vdCTOS_DispStatusMessage("CONNECTING TO SEC IP");
		#else
		CTOS_LCDTPrintXY (1,7, "					");
		CTOS_LCDTPrintXY (1,8, "Connecting to Sec IP");
		CTOS_Delay(500);
		#endif

		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_ENABLE_VALUE);
		wub_ASCIIIP2hex(strCPT.szSecTxnHostIP,strHostIP);
		usPort = strCPT.inSecTxnHostPortNum;

		vdMyEZLib_LogPrintf("GPRSServer Sec IP[%s]: Port[%d]", strCPT.szSecTxnHostIP, usPort);

		ret = CTOS_TCP_GPRSConnectEx(&bgGPRSSocket, strHostIP, usPort);
		bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_ENABLE_VALUE);
		
		if(bolState != TRUE){
			#if 1
			vdCTOS_DispStatusMessage("Connect Failed");
			#else
			vdDisplayErrorMsg(1, 8, "Connect Fail	 ");
			CTOS_Delay(500);
			#endif
			srCommFuncPoint.inEndComm(&srTransRec);
			vdDebug_LogPrintf("CTOS_TCP_GPRSConnectEx Sec ret[%d]-bolState[%d]",shRet, bolState);
			inResult = d_NO;
		}
		else{
			
			return(ST_SUCCESS);
		}
			
		/************************************************************************/
	  // sidumili: Connecting to Secondary IP [END]
	  /************************************************************************/
		
		if (inResult != d_OK)
			return(inResult);
		
		
	#else
	Reconnect:		
		CTOS_LCDTPrintXY(1, 8, "G Connecting... ");
		// Use default Host IP address & Port number //
		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_ENABLE_VALUE);
		vduiClearBelow(8);
		//wub_ASCIIIP2hex(strHDT.strGPRSServerIP,strHostIP);
		wub_ASCIIIP2hex(strCPT.szPriTxnHostIP,strHostIP);
		//memcpy(strHostIP,strHDT.strGPRSServerIP,strlen(strHDT.strGPRSServerIP));
		vdMyEZLib_LogPrintf("strGPRSServerIP[%s]", strCPT.szPriTxnHostIP);				  
		//usPort = strHDT.inGPRSPortNo;
		usPort = strCPT.inPriTxnHostPortNum;
		vdMyEZLib_LogPrintf("inGPRSPortNo[%d]", usPort);
		ret = CTOS_TCP_GPRSConnectEx(&bgGPRSSocket, strHostIP, usPort);
		bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_ENABLE_VALUE);
		if(bolState != TRUE)
		{
			inCTOS_inDisconnect();
			vdDisplayErrorMsg(1, 8, "Connect Fail	 ");
			
			if (iRetry==1)
			{
				CTOS_Delay(1500);//CTOS_KBDGet(&key);
				vdMyEZLib_LogPrintf(". CTOS_GPRSConnectEx Err=(%04x)",bolState);	
				return -1;
			}
			else
			{
				iRetry+=1;
				// reinit
				CTOS_LCDTPrintXY (1,8, "Reset GPRS		");
				CTOS_Delay(500);

				ret = srCommFuncPoint.inInitComm(&srTransRec,GPRS_MODE); 
		   

				if(ret != TRUE)
				{
					vdDisplayErrorMsg (1,8, "Reset GPRS FAIL ");
					vdMyEZLib_LogPrintf(". CTOS_GPRSConnectEx Err=(%04x)",ret);
					return -1;
				}
				else
					goto Reconnect;
			}
			
		}	
#endif
	
  vdMyEZLib_LogPrintf(". CTOS_GPRSConnectEx Ret=(%04x)",bolState);
  vdMyEZLib_LogPrintf("**inGPRS_ ConnectHost END**");	
	
  return ST_SUCCESS;	 
}



int inGPRS_Disconnection(TRANS_DATA_TABLE *srTransPara)
{
    int shRet;
    BOOL bolState; 
    
    vdMyEZLib_LogPrintf("**inGPRS_Disconnection START**");    
    
    incommGPRSCheckState(d_TCP_IO_PROCESSING,500);//check state
    vduiClearBelow(8);    
    shRet = CTOS_TCP_GPRSDisconnect(bgGPRSSocket);
    bolState = incommGPRSCheckState(shRet,500);
    vdMyEZLib_LogPrintf("**inGPRS_Disconnection END**");
    vduiClearBelow(7);
    return d_OK;
}



int inGPRS_SendData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
	ULONG ulRultSendLen;
	BYTE bySendTemp[1048];
	BOOL bolState;
	int i;
	
	vdMyEZLib_LogPrintf("**inGPRS_SendData START**");
	dwStatus = 0;

	vdMyEZLib_LogPrintf(". GPRS Send Len%d",ulSendLen);
		bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
		bySendTemp[1] = (ulSendLen & 0x000000FF);
		vdMyEZLib_LogPrintf((". *DataHead == H*"));
		vdMyEZLib_LogPrintf((". %02x%02x"),bySendTemp[0],bySendTemp[1]);
				
	
	ulRultSendLen = ulSendLen;
	
	memcpy(&bySendTemp[2], uszSendData, ulSendLen);
	
	ulRultSendLen += 2;
	
	for(i=0; i<3; i++)
	{
		#if 1		
		vdDispTransTitleCardTypeandTotal(srTransRec.byTransType); //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE CARD LABEL 4 of 6
		vdCTOS_DispStatusMessage("PROCESSING...");//aaronnino for BDOCLG ver 9.0 fix on issue #00140 CHANGE MESSAGE DISPLAY FOR SENDING... INTO PROCESSING... 2 of 4
		#else
		CTOS_LCDTPrintXY (1,8, "Sending.........    ");
		#endif
		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);//check state
		//vduiClearBelow(8); //aaronnino for BDOCLG ver 9.0 fix on issue #00247 Void display/title display on connecting to comms GPRS
		ret = CTOS_TCP_GPRSTx(bgGPRSSocket, bySendTemp, ulRultSendLen);
		bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_DISABLE);
		if(bolState == TRUE)
		{
			vdMyEZLib_LogPrintf("**inGPRS_SendData END**"); 
					return ST_SUCCESS;		
		}
	}

  return ST_ERROR;
}

    

int inGPRS_ReceiveData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
	int i;
	USHORT usMaxRecLen,usRealLen;
	BYTE usTempRecData[1024];
	BOOL bolState;
	
	
	vdMyEZLib_LogPrintf("**inGPRS_ReceiveData START**");
	usMaxRecLen = 0;
	usRealLen = 0;
	memset(usTempRecData, 0, sizeof(usTempRecData));
	#if 0
		CTOS_TimeOutSet(TIMER_ID_1, 2000);
	#else
			//sidumili: [FOR GPRS RESPONSE TIMEOUT]
			if (strCPT.inTCPResponseTimeout <= 0){
			strCPT.inTCPResponseTimeout = 60;
			}

			CTOS_TimeOutSet(TIMER_ID_1, strCPT.inTCPResponseTimeout * 100); // in secs
			//sidumili: [FOR GPRS RESPONSE TIMEOUT] --- end
	
	#endif
	usMaxRecLen = GPRS_RX_BUF_SIZE;
	while(1)
	{
    #if 1
		vdCTOS_DispStatusMessage("RECEIVING...");
		#else
		CTOS_LCDTPrintXY (1,8, "Receiving.......	");
		#endif
		//Check if the data is currently availabl in Ethernet , CTOS_TCP_GPRSRxReady cannot use, castle says so//
		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);//check state
		vduiClearBelow(8);
		ret = CTOS_TCP_GPRSRx(bgGPRSSocket,&usTempRecData[usRealLen],&usMaxRecLen);
		bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_DISABLE);
				if(bolState == TRUE && usMaxRecLen>=2)
		{
		vdMyEZLib_LogPrintf(". Aft CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);	 
				usRealLen += usMaxRecLen;
				memcpy(uszRecData, &usTempRecData[2], usRealLen);
		break;
		}
		else if (bolState != TRUE)
		{
			#if 1
			vdCTOS_DispStatusMessage("Rx Not Ready");
			#else
		  CTOS_LCDTPrintXY(1, 8, "Rx Not Ready    ");
		  CTOS_Delay(300);
			#endif
				  vdMyEZLib_LogPrintf(". CTOS_GPRSRx NOT Ready");
				  vdMyEZLib_LogPrintf(". CTOS_GPRSRxReady Err=(%04x)",shRet);
		}
				
				if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
				{
                		#if 1
										vdCTOS_DispStatusMessage("Receive TimeOut");
										#else
                    CTOS_LCDTPrintXY(1, 8, "Receive Timeout ");
										#endif
					vdMyEZLib_LogPrintf(". Receive timeout");
			CTOS_Delay(1000);
					shRet = ST_ERROR;
					break;
				}

				CTOS_KBDHit(&key);
				if (key == d_KBD_CANCEL)
				{
					usRealLen = ST_ERROR;
					vdMyEZLib_LogPrintf(". Keb Return");
					return ST_ERROR;
				}
				
				
	
 }
vdMyEZLib_LogPrintf("**inGPRS_ReceiveData END**");
return usRealLen;
}

    


int inGPRS_CutOffConnection(TRANS_DATA_TABLE *srTransPara)
{
    BOOL bolState;
    
    vdMyEZLib_LogPrintf("**inGPRS_CutOffConnection START**");
    
			if(1 == inGPRSOpen)
			{
				#if 1
				vdCTOS_DispStatusMessage("Closing GPRS");
				#else
				vduiDisplayStringCenter(3,"PLEASE WAIT,");
				vduiDisplayStringCenter(4,"CLOSING GPRS.");
				#endif

	ret = CTOS_TCP_GPRSClose();

    bolState = incommGPRSCheckState(ret,1000);
    if(bolState != TRUE)
    {
						vdCTOS_DispStatusMessage("Close Error");
					 }
	
					 inGPRSOpen = 0;
     }
    
    vdMyEZLib_LogPrintf("**inGPRS_CutOffConnection END**");
    return bolState;
}


int inModem_InitModem(TRANS_DATA_TABLE *srTransPara,int mode)
{
	BYTE bkey;
	BYTE response[128];
	USHORT value;
	int shRet;
	vdDebug_LogPrintf("**inModem_InitModem START**inParaMode=[%d],inHandShake[%d],inCountryCode[%d]",strCPT.inParaMode,strCPT.inHandShake,strCPT.inCountryCode);
    shRet = CTOS_ModemOpenEx(strCPT.inParaMode, strCPT.inHandShake, strCPT.inCountryCode);  /// d_M_MODE_SDLC_FAST	
	if (shRet != d_OK){
		vdMyEZLib_LogPrintf(". shRet OpenErr(%04x)", shRet);
		vdMyEZLib_LogPrintf("**inModem_InitModem OpenErr**");

		shRet = CTOS_ModemOpen(strCPT.inParaMode, strCPT.inHandShake, strCPT.inCountryCode);	/// d_M_MODE_SDLC_FAST
		if (shRet != d_OK){
			vdMyEZLib_LogPrintf(". shRet OpenErr(%04x)", shRet);
			vdMyEZLib_LogPrintf("**inModem_InitModem OpenErr**");
			//CTOS_KBDGet(&bkey); 		 
			//return ST_ERROR;
			shRet = CTOS_ModemOpen(strCPT.inParaMode, strCPT.inHandShake, strCPT.inCountryCode);	/// d_M_MODE_SDLC_FAST
			vdDebug_LogPrintf("CTOS_ModemOpen[%d]",shRet);
			if (shRet != d_OK){
				vdMyEZLib_LogPrintf(". shRet OpenErr(%04x)", shRet);
				vdMyEZLib_LogPrintf("**inModem_InitModem OpenErr**");

				return ST_ERROR;
    		}
    	}
	 }
    
    vdDebug_LogPrintf("**inModem_InitModem END**");
    return ST_SUCCESS;
}

int inModem_GetConfigFromTable(TRANS_DATA_TABLE *srTransPara)
{
    vdMyEZLib_LogPrintf("**inModem_GetConfigFromTable START**");
    memset(&srModemRec,0x00,sizeof(srModemRec));
    srModemRec.usDialDuration = ushStrToShort("30");
    srModemRec.usHookDuration = ushStrToShort("3");
    srModemRec.usPreDialDelayTime = ushStrToShort("0");
    srModemRec.usDialToneDetectDuration = ushStrToShort("7000");
    srModemRec.usDialToneMinOnTime = ushStrToShort("14400");
    srModemRec.usDTMFOnTime = ushStrToShort("100");
    srModemRec.usDTMFOffTime = ushStrToShort("100");
    srModemRec.usBusyToneMinTotalTime = ushStrToShort("1728");
    srModemRec.usBusyToneDeltaTime = ushStrToShort("7632");
    srModemRec.usBusyToneMinOnTime = ushStrToShort("864");
    srModemRec.usRingBackToneMinTotalTime = ushStrToShort("18000");
    srModemRec.usRingBackToneDeltaTime = ushStrToShort("61200");
    srModemRec.usRingBackToneMinOnTime = ushStrToShort("4608");
    srModemRec.usAnswerToneWaitDuration = ushStrToShort("50");
    srModemRec.usBlindDialDelayTime = ushStrToShort("2");
    srModemRec.usCarrierPresentTime = ushStrToShort("6");
    
    
    vdMyEZLib_LogPrintf(". ----------Modem Set Config---------------");
    vdMyEZLib_LogPrintf(". srModemRec.strRemotePhoneNum %s",srModemRec.strRemotePhoneNum);
    vdMyEZLib_LogPrintf(". srModemRec.usDialDuration %d",srModemRec.usDialDuration);
    vdMyEZLib_LogPrintf(". srModemRec.usHookDuration %d",srModemRec.usHookDuration);
    vdMyEZLib_LogPrintf(". srModemRec.usPreDialDelayTime %d",srModemRec.usPreDialDelayTime);
    vdMyEZLib_LogPrintf(". srModemRec.usDialToneDetectDuration %d",srModemRec.usDialToneDetectDuration);
    vdMyEZLib_LogPrintf(". srModemRec.usDialToneMinOnTime %d",srModemRec.usDialToneMinOnTime);
    vdMyEZLib_LogPrintf(". srModemRec.usDTMFOnTime %d",srModemRec.usDTMFOnTime);
    vdMyEZLib_LogPrintf(". srModemRec.usDTMFOffTime %d",srModemRec.usDTMFOffTime);
    vdMyEZLib_LogPrintf(". srModemRec.usBusyToneMinTotalTime %d",srModemRec.usBusyToneMinTotalTime);
    vdMyEZLib_LogPrintf(". srModemRec.usBusyToneDeltaTime %d",srModemRec.usBusyToneDeltaTime);
    vdMyEZLib_LogPrintf(". srModemRec.usRingBackToneMinTotalTime %d",srModemRec.usRingBackToneMinTotalTime);
    vdMyEZLib_LogPrintf(". srModemRec.usRingBackToneDeltaTime %d",srModemRec.usRingBackToneDeltaTime);
    vdMyEZLib_LogPrintf(". srModemRec.usRingBackToneMinOnTime %d",srModemRec.usRingBackToneMinOnTime);
    vdMyEZLib_LogPrintf(". srModemRec.usAnswerToneWaitDuration %d",srModemRec.usAnswerToneWaitDuration);
    vdMyEZLib_LogPrintf(". srModemRec.usBlindDialDelayTime %d",srModemRec.usBlindDialDelayTime);
    vdMyEZLib_LogPrintf(". srModemRec.usCarrierPresentTime %d",srModemRec.usCarrierPresentTime);
    
    
    
    
    vdMyEZLib_LogPrintf("**inModem_GetConfigFromTable END**");
    return ST_SUCCESS;
}

int inModem_SetConnectConfig(TRANS_DATA_TABLE *srTransPara)
{
	BYTE temp_buffer[128];
	USHORT temp_len;
	int len;

	
	vdDebug_LogPrintf("szATCMD1=[%s] [%s] [%s] [%s] [%s]",strMMT[0].szATCMD1,strMMT[0].szATCMD2,strMMT[0].szATCMD3,strMMT[0].szATCMD4,strMMT[0].szATCMD5);

	len = strlen(strMMT[0].szATCMD1);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD1,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD1=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}

	len = strlen(strMMT[0].szATCMD2);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD2,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD2=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}

	len = strlen(strMMT[0].szATCMD3);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD3,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD3=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}

	len = strlen(strMMT[0].szATCMD4);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD4,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD4=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}

	len = strlen(strMMT[0].szATCMD5);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD5,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD5=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}
	
    
    return ST_SUCCESS;
}

int inModem_CheckComm(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("byTransType=[%d],szPriTxnPhoneNumber=[%s],szPriSettlePhoneNumber=[%s]",srTransPara->byTransType,strCPT.szPriTxnPhoneNumber,strCPT.szPriSettlePhoneNumber);

	if ((srTransPara->byTransType == SETTLE) || (srTransPara->byTransType == CLS_BATCH))
	{
		if(strlen(strCPT.szPriSettlePhoneNumber)>0)
		{
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szPriSettlePhoneNumber);
		}
	}
	/*BDO: Separate dialing number for BIN Ver - start -- jzg */
	else if (srTransPara->byTransType == BIN_VER)
	{
		inBVTRead(1);
		memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
		sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strBVT.szBINVerPriNum);
	}
	/*BDO: Separate dialing number for BIN Ver - end -- jzg */
	else
	{
		if(strlen(strCPT.szPriTxnPhoneNumber)>0)
		{
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szPriTxnPhoneNumber);
		}
	}

	vdDebug_LogPrintf("default phone num =[%s],fPreDial[%d]",srModemRec.strRemotePhoneNum,strCPT.fPreDial);
	isPredial = 0;
	
	if (strCPT.fPreDial == TRUE)
	{
    	shRet = CTOS_ModemDialup((BYTE *)srModemRec.strRemotePhoneNum,strlen(srModemRec.strRemotePhoneNum));
	
 		isPredial = 1;		 
 	}

	return ST_SUCCESS;
}

int inModem_ReConnectHost(TRANS_DATA_TABLE *srTransPara)
{
	int inRet = d_OK;
	vdDebug_LogPrintf("RECONNECT byTransType=[%d],szSecTxnPhoneNumber=[%s],szSecSettlePhoneNumber=[%s]",srTransPara->byTransType,strCPT.szSecTxnPhoneNumber,strCPT.szSecSettlePhoneNumber);

	if ((srTransPara->byTransType == SETTLE) || (srTransPara->byTransType == CLS_BATCH))
	{
		if(strlen(strCPT.szSecSettlePhoneNumber)>0)
		{
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szSecSettlePhoneNumber);
		}
	}
	/*BDO: Separate dialing number for BIN Ver - start -- jzg */
	else if (srTransPara->byTransType == BIN_VER)
	{
		inBVTRead(1);
		memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
		sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strBVT.szBINVerSecNum);
	}
	/*BDO: Separate dialing number for BIN Ver - end -- jzg */

	else
	{
		if(strlen(strCPT.szSecTxnPhoneNumber)>0)
		{
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szSecTxnPhoneNumber);
		}
	}

	#if 0
	inCTOS_inDisconnect();
	CTOS_Delay (500);
	vdDebug_LogPrintf("RECONNECT default phone num =[%s]",srModemRec.strRemotePhoneNum);
	
	CTOS_ModemDialup((BYTE *)srModemRec.strRemotePhoneNum,strlen(srModemRec.strRemotePhoneNum));
	vdMyEZLib_LogPrintf(" Ring Back..");
//1105
	//CTOS_LCDTPrintXY(1, 7, (BYTE *)srModemRec.strRemotePhoneNum);
//1105
	CTOS_LCDTPrintXY(1, 8, "Ring Back   ");
	#else
		//-----------------------------------------------------------
		//sidumili: reconnecting to host
//		inCTOS_MultiAPReloadTable();
		inCTOS_inDisconnect();
		//inFirstInitConnectFuncPoint();
		CTOS_Delay (500);
		//vduiClearBelow(7);
		vdMyEZLib_LogPrintf(" Ring Back..");
		//CTOS_LCDTPrintXY(1, 7, (BYTE *)srModemRec.strRemotePhoneNum);
		//CTOS_LCDTPrintXY(1, 8, "Ring Back       ");
		inRet = inCTOS_PreConnect2();
		//-----------------------------------------------------------
	#endif


	return ST_SUCCESS;
	
}


int inModem_ConnectHost(TRANS_DATA_TABLE *srTransPara)
{
    int isListen;
    char szBabuff[20];
    USHORT usRetry = 0;
    char szDebugbuff[50+1];

	BYTE str[17];
	DWORD start;			
	DWORD status;

	BYTE key;
	int inRet = ST_SUCCESS;
	BOOL isKey;
	BOOL fPressCancel = FALSE; //sidumili: terminate transaction when USER PRESS CANCEL KEY
	unsigned char c;
	BOOL fPrimaryDial = TRUE;  //sidumili: [variable for message of PRIMARY/SECONDARY DIAL]

	BOOL fChangeDialDisplay;
	char szTempModemStatusRet[8+1];

	int inHideConnectionDisplay = 0;

	inHideConnectionDisplay = get_env_int("BDOSM");

	
    vdMyEZLib_LogPrintf("**inModem_ConnectHost START**");
    
    isDialUp = FALSE;
    isListen = FALSE;

	vdDispTransTitleCardTypeandTotal(srTransRec.byTransType);  /*BDO: Display TOTAL AMOUNT during connection -- sidumili*/
	
	if (isPredial != 1)
	{	
	    vdDebug_LogPrintf("byTransType=[%d],szPriTxnPhoneNumber=[%s],szPriSettlePhoneNumber=[%s]",srTransPara->byTransType,strCPT.szPriTxnPhoneNumber,strCPT.szPriSettlePhoneNumber);

		if ((srTransPara->byTransType == SETTLE) || (srTransPara->byTransType == CLS_BATCH))
		{
			if(strlen(strCPT.szPriSettlePhoneNumber)>0)
			{
				memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
				sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szPriSettlePhoneNumber);
			}
		}
		/*BDO: Separate dialing number for BIN Ver - start -- jzg */
		else if (srTransPara->byTransType == BIN_VER)
		{
			inBVTRead(1);
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strBVT.szBINVerPriNum);
		}
		/*BDO: Separate dialing number for BIN Ver - end -- jzg */

		else
		{
			if(strlen(strCPT.szPriTxnPhoneNumber)>0)
			{
				memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
				sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szPriTxnPhoneNumber);
			}
		}
		
	    vdDebug_LogPrintf("default phone num =[%s]",srModemRec.strRemotePhoneNum);
	    shRet = CTOS_ModemDialup((BYTE *)srModemRec.strRemotePhoneNum,strlen(srModemRec.strRemotePhoneNum));
	         //PREDIAL=TRUE;
	}
	else
		isPredial = 0;
//1105	
    //CTOS_LCDTPrintXY(1, 7, (BYTE *)srModemRec.strRemotePhoneNum);
//1105
	/*BDO: Separate dialing number for BIN Ver - start -- jzg */
	if (srTransPara->byTransType == BIN_VER)
	{
		inBVTRead(1);
		memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
		sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strBVT.szBINVerPriNum);
	}
	/*BDO: Separate dialing number for BIN Ver - end -- jzg */

    //CTOS_LCDTPrintXY(1, 8, "DialUp......");
    
    /* Set 300 ms = 30*10 */
    /* Set 30000 ms = 3000*10 */
    CTOS_TimeOutSet(TIMER_ID_1, 3000);
		key = 0;
		CTOS_KBDBufFlush();

	fChangeDialDisplay = TRUE;
   
    while(1)
    {   
        /* Get the status of modem */
        CTOS_ModemStatus(&dwStatus);
				//---
				//vdDebug_LogPrintf("inModem_ConnectHost dwStatus=[%X]", dwStatus);
				//---

		 vdDebug_LogPrintf("AAA - dwStatus[%08X]", dwStatus);
        memset(szTempModemStatusRet,0x00,sizeof(szTempModemStatusRet));
        sprintf(szTempModemStatusRet,"%08X",dwStatus);		

				//sidumili: terminate transaction when USER PRESS CANCEL KEY
				CTOS_KBDHit (&key);
				if (key == d_KBD_CANCEL){
					CTOS_KBDBufFlush ();
					srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
					fPressCancel = TRUE;
				}
				//sidumili: terminate transaction when USER PRESS CANCEL KEY

        {
            /* Connect success to the host */
            if (dwStatus & d_M_STATUS_DIALING)
            {
            		#if 1
								
					if (fChangeDialDisplay == TRUE){
						 if ((fPrimaryDial) && (inHideConnectionDisplay == 0))
							vdCTOS_DispStatusMessage("DIAL PRIMARY...");
						 else if (inHideConnectionDisplay == 0)
							vdCTOS_DispStatusMessage("DIAL SECONDARY...");
						fChangeDialDisplay = FALSE;
					}
									
					#else
                	CTOS_LCDTPrintXY(1, 8, "Dialing...      ");
					#endif
            }
            else if (dwStatus & d_M_STATUS_MODEM_ONLINE)
            {
                vdMyEZLib_LogPrintf("Modem d_M_STATUS_MODEM_ONLINE");
                //vdCTOS_DispStatusMessage("Connected");
                isDialUp = TRUE;
                break;
            }
            else if(dwStatus & d_M_STATUS_NO_DIAL_TONE)
            {
                vdMyEZLib_LogPrintf("Modem no dial tone");
                //vdDispErrMsg("NO DIAL TONE");
                //vdDisplayErrorMsg(1, 8, "NO DIAL TONE"); // sidumili: [DISPLAY ERROR MESSAGE]
                #if 0
				vdCTOS_DispStatusMessageEx("Check Phone Line");
				vdDisplayErrorMsg(1, 8, "Connectivity");
				#else
				//vdCTOS_MultiLineDisplayMessage("", "Check Phone Line", "Connectivity");
				
				if (usRetry > 0){
					
					if (srTransRec.byTransType == LOG_ON)  
					{
						vdDisplayErrorMsgResp2("","LOGON FAILED","PLEASE RETRY");
					}
					else
						vdDisplayErrorMsgResp2(" ", "Check Phone Line", "Connectivity");
				}
				#endif
				
				srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
                if(usRetry == 0)
                {
					CTOS_TimeOutSet(TIMER_ID_1, 3000);
					fPrimaryDial = FALSE; //sidumili: [variable for message of PRIMARY/SECONDARY DIAL]
					fChangeDialDisplay = TRUE;
                    usRetry++;
					inModem_ReConnectHost(srTransPara);
                    continue;
                }
                else
                isDialUp = FALSE;
                CTOS_Delay(1000);
                return MODEM_FAILED;
                
            }
            else if(dwStatus & d_M_STATUS_NO_CARRIER)
            {
                vdMyEZLib_LogPrintf("Modem no carrier");
                //vdDispErrMsg("NO CARRIER");
                //vdDisplayErrorMsg(1, 8, "NO CARRIER");// sidumili: [DISPLAY ERROR MESSAGE]
                #if 0
                vdCTOS_DispStatusMessageEx("Connect Failed");
				vdDisplayErrorMsg(1, 8, "Please Call");
				#else
				//vdCTOS_MultiLineDisplayMessage("", "Connect Failed", "Please Call");
				
				if (fPrimaryDial == FALSE)
					vdDisplayErrorMsgResp2(" ", "Connect Failed", "Please Call");
				#endif
				
                srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
                if(usRetry == 0)
                {
					CTOS_TimeOutSet(TIMER_ID_1, 3000);
					fPrimaryDial = FALSE; //sidumili: [variable for message of PRIMARY/SECONDARY DIAL]
					fChangeDialDisplay = TRUE;
                    usRetry++;
					inModem_ReConnectHost(srTransPara);
                    continue;
                }
                else
                isDialUp = FALSE;
                CTOS_Delay(1000);
                return MODEM_FAILED;
                
            }
            else if(dwStatus & d_M_STATUS_LINE_BUSY)
            {
                vdMyEZLib_LogPrintf("line busy %d", usRetry);

				#if 0
				/* BDO-00122: Change line busy to line busy, please try again - start -- jzg */
				setLCDPrint(7, DISPLAY_POSITION_LEFT, "LINE BUSY");
				setLCDPrint(8, DISPLAY_POSITION_LEFT, "PLEASE TRY AGAIN    ");
				CTOS_Beep();
				CTOS_Delay(1500);
				/* BDO-00122: Change line busy to line busy, please try again - end -- jzg */
				#else
				//vdCTOS_MultiLineDisplayMessage("", "Line Busy", "Please Try Again"); 
				
				if (usRetry > 0)
					vdDisplayErrorMsgResp2(" ", "Line Busy", "Please Try Again");
				#endif

                srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
                if(usRetry == 0)
                {
					CTOS_TimeOutSet(TIMER_ID_1, 3000);
					fPrimaryDial = FALSE; //sidumili: [variable for message of PRIMARY/SECONDARY DIAL]
				    fChangeDialDisplay = TRUE;	
                    usRetry++;
					inModem_ReConnectHost(srTransPara);
                    continue;
                }
                else
                    //vdMyEZLib_LogPrintf(" Remote No Answer");
                    //CTOS_LCDTPrintXY(1, 8, "Remote No Answer");
                    //isDialUp = FALSE;
                	isDialUp = FALSE;
                	CTOS_Delay(1000);
                	return MODEM_FAILED;
                

            }
			else if(strcmp(szTempModemStatusRet,"00000804")==0) //AAA dwStatus returns 0x00000804 not defined on ctosapi.h 
            {
               
			   if (srTransRec.byTransType == LOG_ON)  
			   {
               		vdDisplayErrorMsgResp2("","LOGON FAILED","PLEASE RETRY");
			   }
			   else
			   		vdDisplayErrorMsgResp2(" ", "Check Phone Line", "Connectivity");
               isDialUp = FALSE;
               CTOS_Delay(1000);
               srTransRec.shTransResult = TRANS_TERMINATE;
               return MODEM_FAILED;
            }
			/* Connect fail to the host */
            else{  
                //vdMyEZLib_LogPrintf("Modem fail %d", dwStatus);// even does not plug phone line still come to here
                //gcitra
                //vdMyEZLib_LogPrintf("Other Error");
                //CTOS_LCDTPrintXY(1, 8, "Other Error     ");    
                vdMyEZLib_LogPrintf("Comm Error");
                //vdCTOS_DispStatusMessage("Comm Error");
				//gcitra
                isDialUp = FALSE;

	            srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
	            return USER_ABORT;
            }	            
        }
                
				//sidumili: terminate transaction when USER PRESS CANCEL KEY
				if (fPressCancel) break;
        
        
        if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
        {
            vdMyEZLib_LogPrintf("Modem Connect TimeOut");
            //CTOS_LCDTPrintXY(1, 8, "Connect TimeOut      ");   
            //CTOS_LCDTPrintXY(1, 8, "NO CARRIER          ");
			//vdCTOS_DispStatusMessageEx("Connect Failed");
			//vdCTOS_DispStatusMessage("Please Call");
			//vdCTOS_MultiLineDisplayMessage("", "Connect Failed", "Please Call"); /*BDO: Remove not display this message. Already displayed inBuildAndSendIsoData function -- sidumili*/
            if(usRetry == 0)
        	{
				CTOS_TimeOutSet(TIMER_ID_1, 3000);
				fPrimaryDial = FALSE; //sidumili: [variable for message of PRIMARY/SECONDARY DIAL]
				fChangeDialDisplay = TRUE;
                usRetry++;
				inModem_ReConnectHost(srTransPara);
                continue;
            }
            else
                isDialUp = FALSE;

			
			//0723
			srTransRec.shTransResult = TRANS_TERMINATE;
			//0723

            if (fPrimaryDial == FALSE)
					      vdDisplayErrorMsgResp2(" ", "Connect Failed", "Please Call");
 			
            return TIME_OUT;
        }
    }
    
		//sidumili: terminate transaction when USER PRESS CANCEL KEY
		if (fPressCancel){
			vdDisplayErrorMsg(1, 8, "USER CANCEL");
			vdDebug_LogPrintf("inModem_ConnectHost [USER CANCEL]");
			return USER_ABORT;
		}
		//sidumili: terminate transaction when USER PRESS CANCEL KEY
		
    vdMyEZLib_LogPrintf("**inModem_ConnectHost END**");
    
    return ST_SUCCESS;
    
}


int inModem_Disconnection(TRANS_DATA_TABLE *srTransPara)
{
    if(isDialUp == FALSE)
    {
        vdMyEZLib_LogPrintf("**inModem_Disconnected Already**");
        return ST_SUCCESS; //check what suppose to 
    }
    vdMyEZLib_LogPrintf("**inModem_Disconnection START**");
    shRet = CTOS_ModemHookOn();
    
    vdMyEZLib_LogPrintf(". CTOS_ModemHookOn(%04x)",shRet);
    vdMyEZLib_LogPrintf("**inModem_Disconnection END**");
    isDialUp = FALSE;
    return d_OK;
}

int inModem_SendData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
	vdMyEZLib_LogPrintf("**inModem_SendData START**");

	#if 0
	CTOS_TimeOutSet(TIMER_ID_1, 6000);/* 6000 means 60 sec	*/
	#else
		
			//sidumili: [FOR MODEM SEND TIMEOUT]
			if (strCPT.inMCarrierTimeOut <= 0){
			strCPT.inMCarrierTimeOut = 60;
			}	
			CTOS_TimeOutSet(TIMER_ID_1, strCPT.inMCarrierTimeOut * 100); // in secs
		
	#endif
	CTOS_ModemFlushRxData();
	
	while(1)
	{
		if (CTOS_ModemTxReady()== d_OK){
        	//CTOS_LCDTPrintXY(1, 8, "Sending...");        	
			if ((strTCT.byTerminalType%2) == 0)
			{
				clearLine(14);
			}
			vdDispTransTitleCardTypeandTotal(srTransRec.byTransType); //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE CARD LABEL 5 of  6
			vdCTOS_DispStatusMessage("PROCESSING...");//#issue35 //aaronnino for BDOCLG ver 9.0 fix on issue #00140 CHANGE MESSAGE DISPLAY FOR SENDING... INTO PROCESSING... 3 of  4
			shRet = CTOS_ModemTxData((BYTE *)uszSendData,ulSendLen);
			CTOS_Delay(1000);
			if (shRet == d_OK)
				break;
		}
		
		if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		{
			vdMyEZLib_LogPrintf(". ModemTxData Time out");
            //CTOS_LCDTPrintXY(1, 8, "Send Timeout    "); //BDO: removed -- jzg
			shRet = ST_ERROR;
			break;
		}
		
	}
	vdMyEZLib_LogPrintf(". CTOS_ModemTxData %d",shRet);
	vdMyEZLib_LogPrintf("**inModem_SendData END**");
	return shRet;
}
#if 0
int inModem_ReceiveData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
	int i;
	USHORT usMaxRecLen,usRealLen;
	BYTE usTempRecData[2048];
	
	vdDebug_LogPrintf("**inModem_ReceiveData START**");
	usMaxRecLen = sizeof(usTempRecData);
	usRealLen = 0;
	memset(usTempRecData, 0, sizeof(usTempRecData));
	if ((strTCT.byTerminalType%2) == 0)
	{
				clearLine(14);
	}
	#if 0
	CTOS_TimeOutSet(TIMER_ID_1, 6000);
	#else
			//sidumili: [FOR MODEM RESPONSE TIMEOUT]
			if (strCPT.inMRespTimeOut <= 0){
			strCPT.inMRespTimeOut = 60;
			}

			CTOS_TimeOutSet(TIMER_ID_1, strCPT.inMRespTimeOut * 100); // in secs
	#endif
	vdCTOS_DispStatusMessage("RECEIVING...");
	while(1){

		//Check if the data is currently available in Modem //
        //CTOS_LCDTPrintXY(1, 8, "Receiving...");
         //CTOS_LCDTPrintXY (1,8, "Receiving.......    ");//#issue:35
         //vdCTOS_DispStatusMessage("Receiving...");
		shRet = CTOS_ModemRxReady(&usMaxRecLen);
		if (17 < (USHORT)usMaxRecLen)
		{
			if (shRet == d_OK)
			{
				//Receive data via the modem channel //
				vdDebug_LogPrintf(". Bef CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
				shRet = CTOS_ModemRxData(&usTempRecData[usRealLen],&usMaxRecLen);
				vdDebug_LogPrintf(". Aft CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
			  
			  
				usRealLen += usMaxRecLen;
				memcpy(uszRecData, usTempRecData, usRealLen);
				break;
			}else
			{
//				CTOS_LCDTPrintXY(1, 8, "RxReady Failed");
				vdDebug_LogPrintf(". CTOS_ModemRxReady Err=(%04x)",shRet);
			}
		}

		//if (dwStatus != 2062){	
		//Error if Comms was disconnected during receiving part
		if (shRet == 16387){
				 CTOS_ModemFlushRxData();
				 return ST_COMMS_DISCONNECT;
		}
		
		if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		{
			shRet = ST_ERROR;
			//Clear the receive buffer of Modem //
			CTOS_ModemFlushRxData();			
			break;
		}
  
		CTOS_KBDHit(&key);
		if (key == d_KBD_CANCEL){
			usRealLen = ST_COMMS_DISCONNECT;
			//Clear the receive buffer of Modem //
			CTOS_ModemFlushRxData();			
			vdDebug_LogPrintf(". Keb Return");
			return ST_COMMS_DISCONNECT;
		}
		
	}
	
	vdMyEZLib_LogPrintf("**inModem_ReceiveData END**");
	return usRealLen;
}
#else
int inModem_ReceiveData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
   int i, inModemReceivingTime = strTCT.inModemReceivingTime * 100;
   USHORT usMaxRecLen,usRealLen;
   BYTE usTempRecData[2048];
   
   vdDebug_LogPrintf("**inModem_ReceiveData START**");
	 vdDebug_LogPrintf("inModemReceivingTime[%d]", inModemReceivingTime);
   usMaxRecLen = sizeof(usTempRecData);
   usRealLen = 0;
   memset(usTempRecData, 0, sizeof(usTempRecData));

   if ((strTCT.byTerminalType%2) == 0)
   {
      clearLine(14);
   }
   
   #if 0
   CTOS_TimeOutSet(TIMER_ID_1, 6000);
   #else
   //sidumili: [FOR MODEM RESPONSE TIMEOUT]
   if (strCPT.inMRespTimeOut <= 0)
	 {
      strCPT.inMRespTimeOut = 60;
   }
   
   CTOS_TimeOutSet(TIMER_ID_1, strCPT.inMRespTimeOut * 100 + inModemReceivingTime); // in secs
   CTOS_TimeOutSet(TIMER_ID_2, inModemReceivingTime); 
   #endif
   
   vdCTOS_DispStatusMessage("RECEIVING...");
	 
   while(1)
    {
      
         shRet = CTOS_ModemRxReady(&usMaxRecLen);
				 
        if (strTCT.inModemReceivingTime==0)
        {
           if (17 < (USHORT)usMaxRecLen)
           {
              if (shRet == d_OK)
              {
                 //Receive data via the modem channel //
                 vdDebug_LogPrintf("AAA - inModemReceivingTime is 0 Bef CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
                 shRet = CTOS_ModemRxData(&usTempRecData[usRealLen],&usMaxRecLen);
                 vdDebug_LogPrintf("AAA - inModemReceivingTime is 0 Aft CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
                 
                 
                 usRealLen += usMaxRecLen;
                 memcpy(uszRecData, usTempRecData, usRealLen);
                 break;
              }
              else
              {
                 vdDebug_LogPrintf("inModemReceivingTime is 0 CTOS_ModemRxReady Err=(%04x)",shRet);
              }
           }
         }
         else
         {
            if ((shRet == d_OK) && (usMaxRecLen > 0))
            {
               //Receive data via the modem channel //
               vdDebug_LogPrintf("AAA - inModemReceivingTime is >= 1 Bef CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
               shRet = CTOS_ModemRxData(&usTempRecData[usRealLen],&usMaxRecLen);
               vdDebug_LogPrintf("AAA - inModemReceivingTime is >= 1 Aft CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);

               usRealLen += usMaxRecLen;

            }
            else
            {
               vdDebug_LogPrintf("inModemReceivingTime is >= 1 CTOS_ModemRxReady Err=(%04x)",shRet);
            }
         }
         
         
         //Error if Comms was disconnected during receiving part
         if (shRet == 16387)
         {
            CTOS_ModemFlushRxData();
            return ST_COMMS_DISCONNECT;
         }
         
         if((CTOS_TimeOutCheck(TIMER_ID_2) == d_YES) && (strTCT.inModemReceivingTime>=1))
         { 
            memcpy(uszRecData, usTempRecData, usRealLen);
            break;
         }
         
         if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
         {
            shRet = ST_ERROR;
            //Clear the receive buffer of Modem //
            CTOS_ModemFlushRxData();			
            break;
         }
         
         CTOS_KBDHit(&key);
         if (key == d_KBD_CANCEL)
         {
            usRealLen = ST_COMMS_DISCONNECT;
            //Clear the receive buffer of Modem //
            CTOS_ModemFlushRxData();   
            vdDebug_LogPrintf(". Keb Return");
            return ST_COMMS_DISCONNECT;
         }
         
         usleep(50 * 1000);
   
   }
   
   vdDebug_LogPrintf(". fft CTOS_ModemRxData usRealLen(%d)",usRealLen);
   vdMyEZLib_LogPrintf("**inModem_ReceiveData END**");
	 
   return usRealLen;
}

#endif
int inModem_CutOffConnection(TRANS_DATA_TABLE *srTransPara)
{
    vdMyEZLib_LogPrintf("**inModem_CutOffConnection START**");
    
    shRet = CTOS_ModemCloseEx();
    
    vdMyEZLib_LogPrintf(". CTOS_ModemCloseEx Ret=(%04x)",shRet);
    vdMyEZLib_LogPrintf("**inModem_CutOffConnection END**");
    return shRet;
}

int inEthernet_InitEthernet(TRANS_DATA_TABLE *srTransPara,int mode)
{   
    shRet = CTOS_EthernetOpenEx();

	inEthernetOpen = 1;

    vdDebug_LogPrintf(". CTOS_EthernetOpenEx Ret=(%04x)",shRet);

	if (shRet == 16908)//ETHERNET ALREADY OPEN
		shRet = d_OK;

    return shRet;    
}


int inEthernet_GetConfigFromTable(TRANS_DATA_TABLE *srTransPara)
{
    vdDebug_LogPrintf("**inEthernet_GetConfigFromTable START**");
    memset(&srEthernetRec,0,sizeof(srEthernetRec));
    
    /* Terminal Ip */
	if(strTCP.fDHCPEnable == IPCONFIG_STATIC)
    {
	    memcpy(srEthernetRec.strLocalIP,strTCP.szTerminalIP,strlen(strTCP.szTerminalIP));
	    vdMyEZLib_LogPrintf(". srEthernetRec.strLocalIP(%s)",srEthernetRec.strLocalIP);
	    /* Mask */
	    memcpy(srEthernetRec.strMask,strTCP.szSubNetMask,strlen(strTCP.szSubNetMask));
	    /* Gateway */
	    memcpy(srEthernetRec.strGateway,strTCP.szGetWay,strlen(strTCP.szGetWay));
	    /* DHCP */
	    srEthernetRec.bDHCP = '0';
	     vdDebug_LogPrintf(". srEthernetRec.strLocalIP(%s)",srEthernetRec.strLocalIP);
	    vdDebug_LogPrintf(". srEthernetRec.strMask(%s)",srEthernetRec.strMask);
	    vdDebug_LogPrintf(". srEthernetRec.strGateway(%s)",srEthernetRec.strGateway);
    }

    /* AutoConnect */
    srEthernetRec.bAutoConn = '2';

    return ST_SUCCESS;
}

 /*Set up the configuration of Ethernet */
int inEthernet_SetConnectConfig(TRANS_DATA_TABLE *srTransPara)
{
    BYTE hPort[5];
    unsigned char const manual_connect[] = "2";
    BYTE babuff[d_BUFF_SIZE];
    
    memset(babuff, 0, sizeof(babuff));
    vdDebug_LogPrintf("**inEthernet_SetConnectConfig START**");
    vdDebug_LogPrintf("IP CONFIG = %d",strTCP.fDHCPEnable);
    if (strTCP.fDHCPEnable == IPCONFIG_DHCP)
    {
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, "1", 1);
        srEthernetRec.bDHCP = '1';
        vdDebug_LogPrintf(". Set DHCP (%d)",srEthernetRec.bDHCP);
    }
	else
    {
  		shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, "0", 1);
        srEthernetRec.bDHCP = '0';
        vdDebug_LogPrintf(". Set DHCP (%d)",srEthernetRec.bDHCP);
    }

	if(shRet != d_OK)
	{
	  	vdDebug_LogPrintf(". Set DHCP Not OK(%04x)",shRet);
        vdDisplayErrorMsg(1, 8, "Set DHCP Not OK ");
	  	CTOS_KBDGet(&key);  
	  	return ST_ERROR;
	}
	 
   
    if (strTCP.fDHCPEnable == IPCONFIG_STATIC)//is static
    {
        //Set IP Address
        vdDebug_LogPrintf(". Set IP (%s)",srEthernetRec.strLocalIP);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_IP, srEthernetRec.strLocalIP, strlen(srEthernetRec.strLocalIP));
		
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set IP Not OK(%04x)",shRet);
            vdDisplayErrorMsg(1, 8, "Set H_IP Not OK ");
            CTOS_KBDGet(&key);  
            return ST_ERROR;
        }

        // Set Mask //
        vdDebug_LogPrintf(". Set Mask (%s)",srEthernetRec.strMask);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_MASK, srEthernetRec.strMask, strlen(srEthernetRec.strMask));
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set Mask Not OK(%04x)",shRet);
            vdDisplayErrorMsg(1, 8, "Set Mask Not OK ");
            CTOS_KBDGet(&key);
            return ST_ERROR;
        }

        // Set Geteway IP //
        vdDebug_LogPrintf(". Set Gateway (%s)",srEthernetRec.strGateway);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_GATEWAY, srEthernetRec.strGateway, strlen(srEthernetRec.strGateway));
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set Gateway Not OK(%04x)",shRet);
            vdDisplayErrorMsg(1, 8, "Set GATE Not OK ");
            CTOS_KBDGet(&key);
            return ST_ERROR;
        }

        // Set DNS IP //
        vdDebug_LogPrintf(". Set DNSIP (%s)",strTCP.szHostDNS1);
        if (wub_strlen(strTCP.szHostDNS1)>0)//only set DNS if value assign, it is due to older firmware doesn't support
        {
            shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DNSIP, strTCP.szHostDNS1, strlen(strTCP.szHostDNS1));
            if(shRet != d_OK)
            {
                vdDebug_LogPrintf(". Set DNSIP Not OK(%04x)",shRet);
                vdDisplayErrorMsg(1, 8, "Set DNSIP Not OK ");
                CTOS_KBDGet(&key);
                return ST_ERROR;
            }
        }
    }
    
		/*************************************************************/
		//sidumili: Issue#: 000095
		// 30000 - 30 secs before display of "Connect Fail"
		shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_CONNTIMEOUT, "30000", 5);
		if(shRet != d_OK)
		{
			 vdDebug_LogPrintf(". Set TimeOut Not OK(%04x)",shRet);
			 vdDisplayErrorMsg(1, 8, "Set TimeOut Not OK ");
			 CTOS_KBDGet(&key);
			 return ST_ERROR;
		}
		/*************************************************************/
    vdDebug_LogPrintf(". Set AutoConn (%02x)",srEthernetRec.bAutoConn);

    shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_AUTOCON,&srEthernetRec.bAutoConn, 1);
    if(shRet != d_OK)
    {
        vdDebug_LogPrintf(". Set AutoConn Not OK(%04x)",shRet);
        vdDisplayErrorMsg(1, 8, "Set AutoConn Not OK ");
        CTOS_KBDGet(&key);
        return ST_ERROR;
    }
 
    shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_UPDATE_EXIT, babuff, 0);
    if(shRet != d_OK)
    {
        vdDebug_LogPrintf(". save Exit Not OK(%04x)",shRet);
        vdDisplayErrorMsg(1, 8, "Save Exit Not OK ");
        CTOS_KBDGet(&key);
        return ST_ERROR;
    }
     
    vdDebug_LogPrintf("**inEthernet_SetConnectConfig END**");
    return ST_SUCCESS;
}

int inEthernet_GetTerminalIP(char *szTerminalIP)
{
	BYTE bLength;
	int result;
	BYTE tmpbabuff[d_BUFF_SIZE];

	// Get the configuration value of Ethernet //
	memset(tmpbabuff,0x00,sizeof(tmpbabuff));
	bLength = sizeof(tmpbabuff);
	result = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_IP, tmpbabuff, &bLength);
    vdDebug_LogPrintf("inEthernet_CheckComm =[%d][%s]",result,tmpbabuff);
    strcpy(szTerminalIP, tmpbabuff);
	return ST_SUCCESS;

}
int inEthernet_CheckComm(TRANS_DATA_TABLE *srTransPara)
{
	BYTE bLength;
	int result;
	BYTE tmpbabuff[d_BUFF_SIZE];
#ifdef CTOS_API
	// Get the configuration value of Ethernet //
	memset(tmpbabuff,0x00,sizeof(tmpbabuff));
	bLength = sizeof(tmpbabuff);
	result = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_IP, tmpbabuff, &bLength);

	vdDebug_LogPrintf("inEthernet_CheckComm =[%d][%s]",result,tmpbabuff);
	
	if (strcmp(tmpbabuff,"0.0.0.0") == 0)
	{
		CTOS_LCDTClearDisplay();
		CTOS_LCDTPrintXY(1, 7, "Please Set");
		vdDisplayErrorMsg(1, 8, "Ethernet");

		return ST_ERROR;
	}
#endif
	return ST_SUCCESS;

}

int inEthernet_ConnectHost(TRANS_DATA_TABLE *srTransPara)
{
	BYTE byPortLen,byIpLen;
	BYTE baPort[10],bytemp[10];
	BYTE bkey;
	USHORT usRealLen;
	BYTE babuff[d_BUFF_SIZE];
		int inResult = 0;
	BYTE szErrResult[10 + 1] = {0};

	int inHideConnectionDisplay = 0;

	inHideConnectionDisplay = get_env_int("BDOSM");

	DebugAddSTR("inEthernet_ConnectHost","START**...",100); 
	memset(srEthernetRec.strRemoteIP, 0x00, sizeof(srEthernetRec.strRemoteIP));
    srEthernetRec.usRemotePort=0;

	vdDispTransTitleCardTypeandTotal(srTransRec.byTransType);  /*BDO: Display TOTAL AMOUNT during connection -- sidumili*/
	
	 /************************************************************************/
	 // sidumili: Connecting to Primary IP [START]
	 /************************************************************************/
     if (inHideConnectionDisplay == 0)
	    vdCTOS_DispStatusMessage("CONNECTING TO PRI IP");
		
	 usRealLen = 0;
	 if(srTransRec.byTransType == SETTLE){
		byIpLen = strlen(strCPT.szPriSettlementHostIP);
		memcpy(srEthernetRec.strRemoteIP,strCPT.szPriSettlementHostIP,byIpLen);
		vdDebug_LogPrintf(". srEthernetRec.strRemoteIP =(%s) byIpLen =(%d)",srEthernetRec.strRemoteIP,byIpLen);
	 
		/* Host Port */
		memset(baPort,0,sizeof(baPort));
		srEthernetRec.usRemotePort = strCPT.inPriSettlementHostPort;
		byPortLen = bUnSingeLongToStr(srEthernetRec.usRemotePort,baPort); 
		vdDebug_LogPrintf(". Set Port (%d)  baPort =(%s) byPortLen =(%d)",strCPT.inPriSettlementHostPort,baPort,byPortLen);
	 
	 }else{
		/* Host Ip */	
		byIpLen = strlen(strCPT.szPriTxnHostIP);
		memcpy(srEthernetRec.strRemoteIP,strCPT.szPriTxnHostIP,byIpLen);
		vdDebug_LogPrintf(". srEthernetRec.strRemoteIP =(%s) byIpLen =(%d)",srEthernetRec.strRemoteIP,byIpLen);

		/* Host Port */
		memset(baPort,0,sizeof(baPort));
		srEthernetRec.usRemotePort = strCPT.inPriTxnHostPortNum;
		byPortLen = bUnSingeLongToStr(srEthernetRec.usRemotePort,baPort); 
		vdDebug_LogPrintf(". Set Port (%d)	baPort =(%s) byPortLen =(%d)",strCPT.inPriTxnHostPortNum,baPort,byPortLen);
	 }
#ifdef CTOS_API	
	shRet = CTOS_EthernetConnectEx(srEthernetRec.strRemoteIP, byIpLen, baPort, byPortLen);
#else
	
	memset (&sockHost, 0, sizeof (struct sockaddr_in));

    sockHost.sin_family = AF_INET;
    memset(sockHost.sin_zero,0,8);
    sockHost.sin_addr.s_addr = inet_addr((char *)srEthernetRec.strRemoteIP);
    sockHost.sin_port = htons(srEthernetRec.usRemotePort);

    sockType = SOCK_STREAM;
    sockHandle = socket(AF_INET, sockType, 0);
	
    if (sockHandle < 0) {
		vdDebug_LogPrintf("Socket error is: %d", sockHandle);
        return d_NO;
    }
	
    vdDebug_LogPrintf("Socket handle is: %d", sockHandle);
		errno = 0;
    shRet = connect(sockHandle, (struct sockaddr*)&sockHost, sizeof(struct sockaddr_in));
	memset(szErrResult, 0x00, sizeof(szErrResult));
	sprintf(szErrResult, "%d", errno); /*BDO: Use variable due to "errno" return incorrect value -- sidumili*/
	vdDebug_LogPrintf("Socket connect is: [%d],errno=[%d]", shRet,errno);	

	/*BDO: Return this error when NO LAN connected -- sidumili*/
	vdDebug_LogPrintf("::szErrResult[%s]", szErrResult);
#if 0
	if ((atoi(szErrResult) == 113) || (atoi(szErrResult) == 111))
	{
		CTOS_Delay(500);

		vdDisplayErrorMsgResp2(" ", "Check LAN","Connectivity");
			
		srCommFuncPoint.inEndComm(&srTransRec);
		srTransRec.shTransResult = TRANS_TERMINATE;
		return(d_NO);
	}
#endif
	/*BDO: Return this error when NO LAN connected -- sidumili*/
	
#endif	
	//CTOS_LCDTPrintXY(1, 8, "Connecting...   ");
	if (shRet != d_OK)
	{
		//vdCTOS_DispStatusMessage("Connect Failed");
			//CTOS_KBDGet(&key);
			CTOS_Delay(500);
				srCommFuncPoint.inEndComm(&srTransRec);
				inResult = srCommFuncPoint.inInitComm(&srTransRec, ETHERNET_MODE);
				if(inResult != ST_SUCCESS){
					vdCTOS_DispStatusMessage("Reset Ethernet Fail");
				  CTOS_Delay(500);
				  
				  //0724
				  srTransRec.shTransResult = TRANS_TERMINATE;
				  //0724
					return ST_ERROR;
				}
		vdDebug_LogPrintf(". CTOS_EthernetConnectEx Err=(%04x)",shRet);
		
				//0724
				srTransRec.shTransResult = TRANS_TERMINATE;
				//0724
				shRet = d_NO;
	}
	//gcitra
	else{
		fCommAlreadyOPen = VS_TRUE; 
		return(d_OK);
	}
	//gcitra

	vdDebug_LogPrintf(". CTOS_EthernetConnectEx Ret=(%04x)",shRet);
	/************************************************************************/
	 // sidumili: Connecting to Secondary IP [END]
	 /************************************************************************/
	

	/************************************************************************/
	// sidumili: Connecting to Secondary IP [START]
	/************************************************************************/

    if (inHideConnectionDisplay == 0)
	    vdCTOS_DispStatusMessage("CONNECTING TO SEC IP");
		
		usRealLen = 0;
	if(srTransRec.byTransType == SETTLE){
		byIpLen = strlen(strCPT.szSecSettlementHostIP);
		memcpy(srEthernetRec.strRemoteIP,strCPT.szSecSettlementHostIP,byIpLen);
		vdDebug_LogPrintf(". srEthernetRec.strRemoteIP =(%s) byIpLen =(%d)",srEthernetRec.strRemoteIP,byIpLen);
	
		/* Host Port */
		memset(baPort,0,sizeof(baPort));
		srEthernetRec.usRemotePort = strCPT.inSecSettlementHostPort;
		byPortLen = bUnSingeLongToStr(srEthernetRec.usRemotePort,baPort); 
		vdDebug_LogPrintf(". Set Port (%d)	baPort =(%s) byPortLen =(%d)",strCPT.inSecSettlementHostPort,baPort,byPortLen);
	
	}else{

	  	/* Host Ip */ 
	  	byIpLen = strlen(strCPT.szSecTxnHostIP);
	  	memcpy(srEthernetRec.strRemoteIP,strCPT.szSecTxnHostIP,byIpLen);
	  	vdDebug_LogPrintf(". srEthernetRec.strRemoteIP =(%s) byIpLen =(%d)",srEthernetRec.strRemoteIP,byIpLen);

	  	/* Host Port */
	  	memset(baPort,0,sizeof(baPort));
	  	srEthernetRec.usRemotePort = strCPT.inSecTxnHostPortNum;
	  	byPortLen = bUnSingeLongToStr(srEthernetRec.usRemotePort,baPort); 
	  	vdDebug_LogPrintf(". Set Port (%d)  baPort =(%s) byPortLen =(%d)",strCPT.inSecTxnHostPortNum,baPort,byPortLen);
	}
#ifdef CTOS_API
	  shRet = CTOS_EthernetConnectEx(srEthernetRec.strRemoteIP, byIpLen, baPort, byPortLen);
#else
	
	memset (&sockHost, 0, sizeof (struct sockaddr_in));

    sockHost.sin_family = AF_INET;
    memset(sockHost.sin_zero,0,8);
    sockHost.sin_addr.s_addr = inet_addr((char *)srEthernetRec.strRemoteIP);
    sockHost.sin_port = htons(srEthernetRec.usRemotePort);

    sockType = SOCK_STREAM;
    sockHandle = socket(AF_INET, sockType, 0);
	
    if (sockHandle < 0) {
		vdDebug_LogPrintf("Socket error is: %d", sockHandle);
        return d_NO;
    }
	
    vdDebug_LogPrintf("Socket handle is: %d", sockHandle);
		errno = 0;
    shRet = connect(sockHandle, (struct sockaddr*)&sockHost, sizeof(struct sockaddr_in));
	memset(szErrResult, 0x00, sizeof(szErrResult));
	sprintf(szErrResult, "%d", errno); /*BDO: Use variable due to "errno" return incorrect value -- sidumili*/
	vdDebug_LogPrintf("Socket connect is: [%d],errno=[%d]", shRet,errno);	

	/*BDO: Return this error when NO LAN connected -- sidumili*/
	vdDebug_LogPrintf("::szErrResult[%s]", szErrResult);
	//check LAN cable if connected		
			shRet = CTOS_EthernetStatus(&dwStatus);
			vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d) 1dwStatus=[%X]",shRet, dwStatus);
			//if(((d_STATUS_ETHERNET_CONNECTED | d_STATUS_ETHERNET_PHYSICAL_ONLINE) & dwStatus) != (d_STATUS_ETHERNET_CONNECTED | d_STATUS_ETHERNET_PHYSICAL_ONLINE))
		
			if (dwStatus & d_STATUS_ETHERNET_PHYICAL_ONLINE){
				
		
			}else{
				CTOS_Delay(500);

		
				if (srTransRec.byTransType == LOG_ON)
				{
			 		vdDisplayErrorMsgResp2("","LOGON FAILED","PLEASE RETRY");
				}
				else
					vdDisplayErrorMsgResp2(" ", "Check LAN","Connectivity");
				//srCommFuncPoint.inEndComm(&srTransRec);
				srCommFuncPoint.inDisconnect(&srTransRec);
				srTransRec.shTransResult = TRANS_TERMINATE;
			
				return(d_NO);
			}
	//check LAN cable if connected		
	
	
		vdDebug_LogPrintf("::szErrResult[%s]", szErrResult);
#if 0
		if ((atoi(szErrResult) == 111) || (atoi(szErrResult) == 101) || (atoi(szErrResult) == 110))
		{
			CTOS_Delay(500);
	
			vdDisplayErrorMsgResp2(" ", "Check LAN","Connectivity");
			srCommFuncPoint.inEndComm(&srTransRec);
			srTransRec.shTransResult = TRANS_TERMINATE;
			return(d_NO);
		}
		/*BDO: Return this error when NO LAN connected -- sidumili*/
		//else if (atoi(szErrResult) == 113)
#endif
		if ((atoi(szErrResult) == 113) || (atoi(szErrResult) == 110) || (atoi(szErrResult) == 101) || (atoi(szErrResult) == 111))
		{
			CTOS_Delay(500);
			vdDisplayErrorMsgResp2(" ", "Connect Failed","Please Try Again");
			srCommFuncPoint.inEndComm(&srTransRec);
			srTransRec.shTransResult = TRANS_TERMINATE;
			return(d_NO);
		}
	/*BDO: Return this error when NO LAN connected -- sidumili*/
	
#endif
	  //CTOS_LCDTPrintXY(1, 8, "Connecting...	");
	  if (shRet != d_OK)
	  {
			//vdCTOS_DispStatusMessage("Connect Failed");
			//CTOS_KBDGet(&key);
			CTOS_Delay(500);
				srCommFuncPoint.inEndComm(&srTransRec);
		  vdDebug_LogPrintf(". CTOS_EthernetConnectEx Err=(%04x)",shRet);
		  
		  		//0724
		  		srTransRec.shTransResult = TRANS_TERMINATE;
		  		//0724
				shRet = d_NO;
	  }
	//gcitra
	else{
		fCommAlreadyOPen = VS_TRUE; 
		return(d_OK);
	}
	//gcitra

	vdDebug_LogPrintf(". CTOS_EthernetConnectEx Ret=(%04x)",shRet);
	
	/************************************************************************/
	// sidumili: Connecting to Secondary IP [END]
	/************************************************************************/

	return shRet;
}

int inEthernet_Disconnection(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("Disconnect,inEthernetOpen=[%d]",inEthernetOpen);
#ifdef CTOS_API	
   if (1 ==inEthernetOpen)
   	{
		shRet = CTOS_EthernetDisconnect();
		//inEthernetOpen= 0;
   	}
#else
   if( sockHandle == NULL )
    {
        vdDebug_LogPrintf("Invalid socket handle");
        return d_NO;
    }
    vdDebug_LogPrintf("Try to closesocket[handle=%d]....", sockHandle);
    if( sockHandle <= 0)
        return d_NO ;

    errno = 0;
	shutdown(sockHandle,2);
	close(sockHandle);
    vdDebug_LogPrintf("closesocket[handle=%d] ...[errno=%d]", sockHandle, errno);
    sockHandle = 0;
#endif    
    vdDebug_LogPrintf(". CTOS_EthernetDisconnect Ret=(%04x)",shRet);
    return d_OK;
}

int inEthernet_SendData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
	{
		ULONG ulRultSendLen;
		BYTE bySendTemp[1048];
		
		//gcitra
		unsigned char szTmp[25];
		unsigned char szHex[25];
			
		static USHORT usNetworkType = 0;
		static USHORT usEthType = 0;
		static BYTE szNetworkName[128+1] = {0};
				
		vdMyEZLib_LogPrintf("**inEthernet_SendData START**");
		dwStatus = 0;
			
#if 0
		inCTOSS_GetGPRSSignalEx(&usNetworkType, szNetworkName,&usEthType);
			
		if (usEthType != 1)
			return -7;
#endif
		
		/*
			{
				vdMyEZLib_LogPrintf(". Ethernet Send Len%d",ulSendLen);
				bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
				bySendTemp[1] = (ulSendLen & 0x000000FF);
				vdMyEZLib_LogPrintf((". *DataHead == H*"));
				vdMyEZLib_LogPrintf((". %02x%02x"),bySendTemp[0],bySendTemp[1]);
			}
		  */ 
		if(strCPT.inIPHeader == 1)
			sprintf((char *) szTmp, "%04X", (unsigned int)ulSendLen);
		else
		sprintf((char *) szTmp, "%04d", (int)ulSendLen);
			
		wub_str_2_hex(szTmp,szHex,4);
		memcpy(bySendTemp, szHex, 2);
		//gcitra  
		
		ulRultSendLen = ulSendLen;
		
		memcpy(&bySendTemp[2], uszSendData, ulSendLen);
		
		ulRultSendLen += 2;
		
 #ifdef CTOS_API	   
		shRet = CTOS_EthernetFlushRxData();
		vdMyEZLib_LogPrintf(". Send CTOS_EthernetFlushRxData shRet=(%04x)",shRet);
	
		#if 0
		CTOS_TimeOutSet(TIMER_ID_1, 200);
		#else
	
				//sidumili: [FOR ETHERNET SEND TIMEOUT]
				if (strCPT.inTCPConnectTimeout <= 0){
					strCPT.inTCPConnectTimeout = 60;
				}	
			CTOS_TimeOutSet(TIMER_ID_1, strCPT.inTCPConnectTimeout * 100); // in secs
	
		#endif
		while(1)
		{
#ifdef _TRANS_CONNECT_USE_RS232_   
			if(CTOS_RS232TxReady(DBG_PORT) == d_OK)
			{
			   shRet = CTOS_RS232TxData(DBG_PORT, uszSendData, ulSendLen);
			   vdMyEZLib_LogPrintf(". CTOS_RS232TxData Ret=(%04x)",shRet);
				if (shRet == d_OK)
					break;
			}
#else        
			shRet = CTOS_EthernetTxReady(); // Wait Ethernet ready to transmit data to remote
			if (shRet == d_OK)
			{
				shRet = CTOS_EthernetTxData(bySendTemp, ulRultSendLen); // Ethernet transmit data to remote
				 vdMyEZLib_LogPrintf(". CTOS_EthernetTxData Ret=(%04x)",shRet);
				if (shRet == d_OK)
					break;
			}
#endif
		 
			if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
			{
				vdCTOS_DispStatusMessage("Tx Data Fail");
				CTOS_Delay(1000);
				shRet = ST_ERROR;
	
				vdMyEZLib_LogPrintf(". Time Out ErrRet=(%04x)",shRet);
				break;
			}
			
		}
#else
	vdDispTransTitleCardTypeandTotal(srTransRec.byTransType); //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE CARD LABEL 6 of 6
	vdCTOS_DispStatusMessage("PROCESSING..."); //BDO: [Sending message display] -- sidumili  //aaronnino for BDOCLG ver 9.0 fix on issue #00140 CHANGE MESSAGE DISPLAY FOR SENDING... INTO PROCESSING... 4 of 4

	if(inCheckEthernetSocketConnected() == BROKEN_PIPE)
		return ST_SEND_DATA_ERR;
		
	//shRet = send(sockHandle, bySendTemp, ulRultSendLen, 0);
	shRet = send(sockHandle, bySendTemp, ulRultSendLen, MSG_DONTWAIT | MSG_NOSIGNAL);
	vdDebug_LogPrintf("send is: [%d],ulRultSendLen=[%d],szBuf=[%s]", shRet,ulRultSendLen,bySendTemp);
	if (shRet == ulRultSendLen)
			shRet = d_OK;
#endif    
		vdMyEZLib_LogPrintf("**inEthernet_SendData END**");
		return shRet;
	}

int inEthernet_ReceiveData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
	DWORD dwStatus;
	int inRealLen;
	USHORT usMaxRecLen;
	BYTE usTempRecData[1024];
	
	vdMyEZLib_LogPrintf("**inEthernet_ReceiveData START**");
	usMaxRecLen = 0;
	inRealLen = 0;
	dwStatus = 0;
	short inLen = 0;

    static USHORT usNetworkType = 0;
    static USHORT usEthType = 0;
    static BYTE szNetworkName[128+1] = {0};
	
	STRUCT_SHARLS_COM Sharls_COMData;

	
	memset(usTempRecData,0,sizeof(usTempRecData));
		CTOS_TimeOutSet(TIMER_ID_2, 50);
#ifdef CTOS_API    
   while(1)
	{
		if(CTOS_TimeOutCheck(TIMER_ID_2) == d_YES)
		{
			vdMyEZLib_LogPrintf("Ethernet Connect TimeOut");
			break;
		}
	}  //---remarked - Meena 07/03/13
	#if 0
	CTOS_TimeOutSet(TIMER_ID_1, 6000); //------ 1500 to 6000 -Meena 07/03/13
#else

			//sidumili: [FOR GPRS RESPONSE TIMEOUT]
			if (strCPT.inTCPResponseTimeout <= 0){
				strCPT.inTCPResponseTimeout = 60;
			}
		
	  CTOS_TimeOutSet(TIMER_ID_1, strCPT.inTCPResponseTimeout * 100); // in secs
			
	#endif
	while(1)
	{	 
		/* Check if sockect disconnect or LAN disconnect, ROOTFS38 ok, ROOTFS35 nok */ 
		shRet = CTOS_EthernetStatus(&dwStatus);
		vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d) 1dwStatus=[%X]",shRet, dwStatus);
		if(((d_STATUS_ETHERNET_CONNECTED | d_STATUS_ETHERNET_PHYSICAL_ONLINE) & dwStatus) != (d_STATUS_ETHERNET_CONNECTED | d_STATUS_ETHERNET_PHYSICAL_ONLINE))
		{
			inRealLen = ST_ERROR;
			break;
		}
		
		/* Check if the data is currently availabl in Ethernet */
		shRet = CTOS_EthernetRxReady(&usMaxRecLen);

		//vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d)",shRet);
		if (shRet == d_OK && usMaxRecLen>=2){
			
			vdMyEZLib_LogPrintf(". Bef RxData usMaxRecLen(%d)",usMaxRecLen);
			/* Receive data via Ethernet channel */
			shRet = CTOS_EthernetRxData(&usTempRecData[inRealLen],&usMaxRecLen);
			
			vdMyEZLib_LogPrintf(". Aft RxData usMaxRecLen(%d)",usMaxRecLen);

			vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d) usMaxRecLen[%d]",shRet, usMaxRecLen);
			if(shRet != d_OK)
			{
				inRealLen = ST_ERROR;
				vdMyEZLib_LogPrintf(". CTOS_EthernetRxData shRet=(%04x)",shRet);
				usMaxRecLen =0;
			}
			else if(usMaxRecLen>=2)
			{
				vdMyEZLib_LogPrintf(". Aft RxData usMaxRecLen(%d)",usMaxRecLen);
				inRealLen =  inRealLen+usMaxRecLen;
				vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData length received=(%d)",inRealLen);
				if(inLen == 0)
				{
					inLen = ((usTempRecData[0] / 16 * 10) + usTempRecData[0] % 16) * 100;
					inLen += (usTempRecData[1] / 16 * 10) + usTempRecData[1] % 16;
				}
				
				
				vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData length send by host=(%d)",inLen);
				if(inRealLen >= inLen)
				{
				memcpy(uszRecData,&usTempRecData[2],inRealLen);
				//shRet = CTOS_EthernetFlushRxData();
				vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData shRet=(%04x)",shRet);
				break;
				}
			}

		}

/*
		CTOS_KBDHit(&key);
		if (key == d_KBD_CANCEL){
			usRealLen = ST_ERROR;
			vdMyEZLib_LogPrintf(". Keb Return");
			shRet = CTOS_EthernetFlushRxData();
			vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData shRet=(%04x)",shRet);
			vdDisplayErrorMsg(1, 8, "Cancel");
			return ST_ERROR;
		}
*/		  
		
		if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		{
			CTOS_EthernetFlushRxData();
			vdDebug_LogPrintf("Ethernet Connect TimeOut");
			//vdDisplayErrorMsg(1, 8, "No Response");
			//CTOS_Delay(1000);
			return ST_ERROR;
		}
		

	}
#else
 struct timeval timeout;

	if (strCPT.inTCPResponseTimeout <= 0){
				strCPT.inTCPResponseTimeout = 60;
			}

 //CTOS_LCDTPrintXY (1,8, "Receiving....... 	 "); //BDO: [Receiving message display] -- sidumili  
 vdCTOS_DispStatusMessage("RECEIVING...");
 timeout.tv_sec = strCPT.inTCPResponseTimeout;
 timeout.tv_usec = 100;
 setsockopt(sockHandle, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

 {
				usMaxRecLen = 1024;
				inRealLen = 0;
       shRet = recv(sockHandle, (char *)&usTempRecData[inRealLen], usMaxRecLen, 0);
			 vdDebug_LogPrintf("recv usMaxRecLen(%d)inRealLen=[%d],shRet=[%d]",usMaxRecLen,inRealLen,shRet);
       if(shRet <= 0)
       {
              vdDebug_LogPrintf("Error Reading from SSL");
              return ST_ERROR;
       }
        if (shRet > 0)
    	{
        	vdDebug_LogPrintf(". Aft RxData usMaxRecLen(%d)inRealLen=[%d]",usMaxRecLen,inRealLen);
            //inRealLen =  inRealLen+usMaxRecLen;
            inRealLen = shRet;
            vdDebug_LogPrintf(". recv length received=(%d)",inRealLen);
            if(inLen == 0)
            {
                inLen = ((usTempRecData[0] / 16 * 10) + usTempRecData[0] % 16) * 100;
                inLen += (usTempRecData[1] / 16 * 10) + usTempRecData[1] % 16;
            }
            //BDO PHASE 2: [Check for len receive] -- sidumili
						if ((inRealLen == 0) || (inLen == 0)){
							vdDebug_LogPrintf("recv() no data [ret=%d errno=%d ]--inRealLen[%d]--inLen[%d]", shRet, errno, inRealLen, inLen);

							inCTOSS_GetGPRSSignalEx1(&usNetworkType, szNetworkName, &usEthType, &Sharls_COMData);
							
							if (usEthType != 1)
								return -7;
							else
								return ST_ERROR;
						}
						//BDO PHASE 2: [Check for len receive] -- sidumili
            
            vdDebug_LogPrintf(". recv length send by host=(%d)",inLen);
            if(inRealLen >= inLen)
            {
	            memcpy(uszRecData,&usTempRecData[2],inRealLen);
	            vdMyEZLib_LogPrintf(". recv shRet=(%04x)",shRet);
            	//break;
            }
        }
        else if (shRet==0)
        {
            vdDebug_LogPrintf("Host closed [ret=%d errno=%d ]", shRet, errno);
            return ST_ERROR;
        }
        else
        {
            if (errno == EWOULDBLOCK)
            {
                vdDebug_LogPrintf("recv() no data [ret=%d errno=%d ]", shRet, errno);
                return ST_ERROR;
            }
            else
            {
                vdDebug_LogPrintf("recv() failed [ret=%d errno=%d ]", shRet, errno);
                return ST_ERROR;
            }
        }
    }

 #endif    
    vdMyEZLib_LogPrintf("**inEthernet_ReceiveData END**");
    return inRealLen;
}

int inEthernet_CutOffConnection(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("inEthernet_CutOffConnection,inEthernetOpen=[%d]",inEthernetOpen);

   if (1 ==inEthernetOpen)
   	{
		shRet = CTOS_EthernetClose();
		inEthernetOpen= 0;
   	}
    
    vdDebug_LogPrintf(". CTOS_EthernetClose Ret=(%04x)",shRet);
    vdDebug_LogPrintf("**inEthernet_CutOffConnection END**");
    return shRet;
}

USHORT incommSignal(void)
{
	BYTE bpStrength;
	USHORT inSignal;
	USHORT  usRtn;
        
  	usRtn=CTOS_GSMSignalQuality(&bpStrength); 	
  	if (usRtn != d_OK)
			return -1;

		if ((USHORT)bpStrength>=99)
		{
			return -1;
		}
		else
		{
			inSignal=(USHORT)bpStrength;	        
			/*if(inSignal>1 && inSignal <= 10 )
				inSignal=2;
			else if(inSignal>10 && inSignal <= 20 )
				inSignal=2;
			else if(inSignal>20 && inSignal <= 30 )
				inSignal=3;
			else if(inSignal>30)
				inSignal=4;*/
		}
		return inSignal;
}

/*************************************************************************/
//sidumili: display status message
void vdCTOS_DispStatusMessage(char* szMsg)
	{
	if ((strTCT.byTerminalType%2) == 0){
			//clearLine(14);// this function is slowly
			CTOS_LCDTPrintXY(1, 14, "										 ");
			CTOS_LCDTPrintXY(1, 14, szMsg);
	}
		else{
			//clearLine(8);
			CTOS_LCDTPrintXY(1, 8, "                                        ");
			CTOS_LCDTPrintXY(1, 8, szMsg);
		}
//CTOS_Delay(100); 
}

void vdCTOS_DispStatusMessageEx(char* szMsg)
{
if ((strTCT.byTerminalType%2) == 0){
//clearLine(13);
CTOS_LCDTPrintXY(1, 13, "                                        ");
CTOS_LCDTPrintXY(1, 13, szMsg);
}
else{
//clearLine(7);
CTOS_LCDTPrintXY(1, 7, "                                        ");
CTOS_LCDTPrintXY(1, 7, szMsg);
}
//CTOS_Delay(100); 
}



int inDummyFlow(TRANS_DATA_TABLE *srTransPara)
{
	return d_OK;
}

/*BDO: Display 3 lines of message, disappear both display immediately -- sidumili*/
void vdCTOS_MultiLineDisplayMessage(char *szLine1Msg, char *szLine2Msg, char *szLine3Msg)
{
	if ((strTCT.byTerminalType%2) == 0)
    {
        vduiClearBelow(12);
		CTOS_LCDTPrintXY(1, 12, szLine1Msg);
		CTOS_LCDTPrintXY(1, 13, szLine2Msg);
        CTOS_LCDTPrintXY(1, 14, szLine3Msg);
		CTOS_Delay(1000);
		CTOS_Beep(); 
		vduiClearBelow(12);
    }
    else
    {
		vduiClearBelow(6);
		CTOS_LCDTPrintXY(1, 6, szLine1Msg);
		CTOS_LCDTPrintXY(1, 7, szLine2Msg);
        CTOS_LCDTPrintXY(1, 8, szLine3Msg);
		CTOS_Delay(1000);
		CTOS_Beep();
		vduiClearBelow(6);
    }
}

int inModem_ReceiveData_New(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
    int i;
    USHORT usMaxRecLen,usRealLen;
    BYTE usTempRecData[2048];
    BOOL fExit=FALSE;
	
    vdDebug_LogPrintf("**inModem_ReceiveData START**");

    usMaxRecLen = sizeof(usTempRecData);
    usRealLen = 0;
    memset(usTempRecData, 0, sizeof(usTempRecData));
	if ((strTCT.byTerminalType%2) == 0)
	{
				clearLine(14);
	}
	#if 0
	CTOS_TimeOutSet(TIMER_ID_1, 6000);
	#else
			//sidumili: [FOR MODEM RESPONSE TIMEOUT]
			if (strCPT.inMRespTimeOut <= 0){
			strCPT.inMRespTimeOut = 60;
			}

			CTOS_TimeOutSet(TIMER_ID_1, strCPT.inMRespTimeOut * 100); // in secs
	#endif
	vdCTOS_DispStatusMessage("RECEIVING...");
    while(1){

        //Check if the data is currently available in Modem //
        //CTOS_LCDTPrintXY(1, 8, "Receiving...");
        // CTOS_LCDTPrintXY (1,8, "Receiving.......    ");//#issue:35
        //vdCTOS_DispStatusMessage("Receiving...");
        shRet = CTOS_ModemRxReady(&usMaxRecLen);
		vdDebug_LogPrintf("shRet[%d] :: usMaxRecLen[%d]",shRet,usMaxRecLen);
        if(fExit == TRUE && usMaxRecLen <= 0)
            break;
		
        if ((17 < (USHORT)usMaxRecLen) || (fExit == TRUE && usMaxRecLen))
        {
            if (shRet == d_OK)
            {
				memset(usTempRecData, 0, sizeof(usTempRecData));
                //Receive data via the modem channel //
                vdDebug_LogPrintf(". Bef CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
                //shRet = CTOS_ModemRxData(&usTempRecData[usRealLen],&usMaxRecLen);
                shRet = CTOS_ModemRxData(usTempRecData,&usMaxRecLen);
                vdDebug_LogPrintf(". Aft CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
              				
                //usRealLen += usMaxRecLen;
                //memcpy(uszRecData, usTempRecData, usRealLen);
                memcpy(&uszRecData[usRealLen], usTempRecData, usMaxRecLen);
				usRealLen += usMaxRecLen;
	         			
                if(fExit == FALSE)
                {
					fExit = TRUE;
					CTOS_Delay(100);
					continue;
                }
				else
                    break;
            }else
            {
//                CTOS_LCDTPrintXY(1, 8, "RxReady Failed");
                vdDebug_LogPrintf(". CTOS_ModemRxReady Err=(%04x)",shRet);
            }
        }
		
		//if (dwStatus != 2062){	
		//Error if Comms was disconnected during receiving part
		if (shRet == 16387){
				 CTOS_ModemFlushRxData();
				 return ST_COMMS_DISCONNECT;
		}
       
        if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
        {
            shRet = ST_ERROR;
			//Clear the receive buffer of Modem //
			CTOS_ModemFlushRxData();			
            break;
        }
  
        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL){
            usRealLen = ST_COMMS_DISCONNECT;
			//Clear the receive buffer of Modem //
			CTOS_ModemFlushRxData();			
            vdDebug_LogPrintf(". Keb Return");
            return ST_COMMS_DISCONNECT;
        }
        
    }
    
    vdMyEZLib_LogPrintf("**inModem_ReceiveData END**");
    return usRealLen;
}

int inCheckEthernetSocketConnected(void)
{
	int error = 0;
	int retval = 0;
	char tmpbuf[100];
	socklen_t len = sizeof (error);

	
	retval = getsockopt(sockHandle, SOL_SOCKET, SO_ERROR, &error, &len);
	vdDebug_LogPrintf( "1. eror getting socket error code: %s", strerror(retval));
	if (retval != 0) 
	{
		/* there was a problem getting the error code */
		vdDebug_LogPrintf( "2. error getting socket error code: %s", strerror(retval));
	}

	if (error != 0) 
	{
		/* socket has a non zero error status */
		vdDebug_LogPrintf("socket error: %s", strerror(error));

		memset(tmpbuf,0x00,sizeof(tmpbuf));
		sprintf(tmpbuf,"%s",strerror(error));
		vdDebug_LogPrintf("tmpbuf=[%s],error=[%d]",tmpbuf,error);
		
		//please refer errno.h, if seckot problem need return;
		//if (error == 32)
		//{
			return BROKEN_PIPE;
		//}
	}
	
	//end check the socket is exist  
	
}

