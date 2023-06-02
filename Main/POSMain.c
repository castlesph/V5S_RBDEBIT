/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <ctoserr.h>
#include <stdlib.h>
#include <stdarg.h>
#include <emv_cl.h>

#include "..\Includes\POSTypedef.h"
#include "..\Debug\Debug.h"

#include "..\FileModule\myFileFunc.h"
#include "..\DataBase\DataBaseFunc.h"

#include "..\Includes\showbmp.h"
#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSConfig.h"
#include "..\ui\Display.h"
#include "..\Includes\Dmenu.h"
#include "..\Includes\POSVoid.h"
#include "..\powrfail\POSPOWRFAIL.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\CardUtil.h"

#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Includes\CfgExpress.h"
#include "..\Includes\Wub_lib.h"
#include "..\Aptrans\MultiShareECR.h"
#include "..\Aptrans\MultiShareCOM.h"

#include "..\Ctls\PosWave.h"
#include "..\Pinpad\Pinpad.h"

//1030
BOOL fBinVerInitiatedSALE;
//1030

extern BOOL fGPRSConnectOK;

// BDO: Added flag to supress merc password during auto Log-On -- jzg
BOOL fAutoLogOn;

extern int isPredial;
BOOL fAdviceTras;


static BYTE stgFirstIdleKey = 0x00;
//Auto-settlement: settle all function -- jzg
extern int inCTOS_SETTLE_ALL(void);

BYTE chGetFirstIdleKey(void)
{
    return stgFirstIdleKey;
}

void vdSetFirstIdleKey(BYTE bFirstKey)
{
    stgFirstIdleKey = bFirstKey;
}

int inCTOS_ValidFirstIdleKey(void)
{
    if((chGetFirstIdleKey() >= d_KBD_1) && (chGetFirstIdleKey() <= d_KBD_9))
        return d_OK;
    else
        return d_NO;
}

int inCTOS_ECRTask(void)
{

    if (strTCT.fECR) // tct
    {
	    inMultiAP_ECRGetMainroutine(); 
    }

    return SUCCESS;
}

int inDoAutosettlement(void)
{
}

int inCTOS_IdleRunningTestFun5(void)
{
    return SUCCESS;
}

int inCTOS_IdleRunningTestFun6(void)
{
    return SUCCESS;
}

int inCTOS_IdleRunningTestFun7(void)
{
    return SUCCESS;
}

int inCTOS_IdleRunningTestFun8(void)
{
    return SUCCESS;
}

int inCTOS_DisplayIdleBMP(void)
{
    vdDisplayAnimateBmp(0, 30, "Insert1.bmp", "Insert2.bmp", "Insert3.bmp", NULL, NULL);
//	vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);

    return SUCCESS;
}

int inCTOS_IdleDisplayDateAndTime(void)
{
    CTOS_RTC SetRTC;
    BYTE szCurrentTime[20];


    memset(szCurrentTime, 0x00, sizeof(szCurrentTime));
    CTOS_RTCGet(&SetRTC);
	sprintf(szCurrentTime,"%d-%02d-%02d  %02d:%02d     ",SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour,SetRTC.bMinute);

    CTOS_LCDTPrintXY (13,1, szCurrentTime);
    
    return SUCCESS;
}


int inCTOS_DisplayComTypeICO(void)
{    

    USHORT usRet;
    USHORT usRet1;
    DWORD pdwStatus ;
    BYTE  bPercentage ;
    static int inLoop = 0;
    static int inGPRSSignal = 0;

    //------
    static USHORT usNetworkType = 0;
    static USHORT usEthType = 1;
    static BYTE szNetworkName[128+1] = {0};
	static unsigned long linux_time = 0;
	unsigned long linux_crttime;	
    //------

		//------
		linux_crttime = CTOS_TickGet();
		//------
    if(strTCT.fDemo)
    {
        displayAppbmpDataEx(300, 90, "DEMO.bmp");
    }

    if(strTCT.fECR)
    {
        displayAppbmpDataEx(270, 180, "ECR1.bmp");
    }

    pdwStatus = 0x00;
    usRet = CTOS_BatteryStatus(&pdwStatus);
    //vdDebug_LogPrintf("**CTOS_BatteryStatus [%X] [%02X]**", usRet, pdwStatus);
    usRet1 = CTOS_BatteryGetCapacity(&bPercentage);
    //vdDebug_LogPrintf("**CTOS_BatteryGetCapacity [%X] [%d]**", usRet1, bPercentage);

    if(d_BATTERY_NOT_EXIST == usRet1)
    {
        displayAppbmpDataEx(35, 1, "0N.bmp");
    }
    else if(d_OK == usRet1)
    {
        if(bPercentage <= 25)
        {
            if(d_OK == usRet && (pdwStatus & d_MK_BATTERY_CHARGE))
            {
                displayAppbmpDataEx(35, 1, "25.bmp");
            }
            else
            {
                displayAppbmpDataEx(35, 1, "25N.bmp");
            }
        }
        else if(bPercentage <= 50)
        {
            if(d_OK == usRet && (pdwStatus & d_MK_BATTERY_CHARGE))
            {
                displayAppbmpDataEx(35, 1, "50.bmp");
            }
            else
            {
                displayAppbmpDataEx(35, 1, "50N.bmp");
            }
        }
        else if(bPercentage <= 75)
        {
            if(d_OK == usRet && (pdwStatus & d_MK_BATTERY_CHARGE))
            {
                displayAppbmpDataEx(35, 1, "75.bmp");
            }
            else
            {
                displayAppbmpDataEx(35, 1, "75N.bmp");
            }
            
        }
        else if(bPercentage <= 100)
        {
            displayAppbmpDataEx(35, 1, "100.bmp");
        }
    }

    inLoop ++;
	if(strCPT.inCommunicationMode == ETHERNET_MODE)
	{	
	    displayAppbmpDataEx(1, 1, "LAN.bmp");
		//CTOS_LCDTPrintXY (1,1, "E");		
	}
	else if(strCPT.inCommunicationMode == DIAL_UP_MODE)
	{
	    displayAppbmpDataEx(1, 1, "MODEM.bmp");
		//CTOS_LCDTPrintXY (1,1, "M");		
	}		
	else if(strCPT.inCommunicationMode == GPRS_MODE)
	{
	    if(0 == inLoop%20)
        {   
            
			inGPRSSignal = inCTOSS_GetGPRSSignalEx(&usNetworkType, szNetworkName,&usEthType);
            
            vdDebug_LogPrintf("**inCTOSS_GetGPRSSignalEx [%d]**", inGPRSSignal);
        }

        if(inLoop > 0xFFFF)
            inLoop = 0;

		
        if(fGPRSConnectOK == TRUE){	
					CTOS_LCDTSelectFontSize(d_FONT_8x16);
					CTOS_LCDTPrintXY (10,2, "GPRS");		
					CTOS_LCDTSelectFontSize(d_FONT_16x30);
        }else{      
					CTOS_LCDTSelectFontSize(d_FONT_8x16);
					CTOS_LCDTPrintXY (10,3, "        ");
					CTOS_LCDTSelectFontSize(d_FONT_16x30);
				}

        if(1 == inGPRSSignal)
            displayAppbmpDataEx(1, 1, "G1.bmp");
        else if(2 == inGPRSSignal)
            displayAppbmpDataEx(1, 1, "G2.bmp");
        else if(3 == inGPRSSignal)
            displayAppbmpDataEx(1, 1, "G3.bmp");
        else if(4 == inGPRSSignal)
            displayAppbmpDataEx(1, 1, "G4.bmp");
        else
            displayAppbmpDataEx(1, 1, "G0.bmp");

        //CTOS_LCDTPrintXY (1,1, "G");				
	}

    return SUCCESS;
}

typedef struct 
{
	int  (*inCTOS_IdleRunningFun)(void);
} IDLE_FUNC;


IDLE_FUNC g_IdleRunningFun[] = 
{
    inCTOS_DisplayIdleBMP,
    inCTOS_DisplayComTypeICO,
    inCTOS_IdleDisplayDateAndTime,
    inCTOS_ECRTask,
    inDoAutosettlement,
/*    inCTOS_IdleRunningTestFun4,
    inCTOS_IdleRunningTestFun5,
    inCTOS_IdleRunningTestFun6,
    inCTOS_IdleRunningTestFun7,
    inCTOS_IdleRunningTestFun8,*/
    NULL
        
};


int inCTOSS_InitAP(void)
{
	USHORT ret;
	
	ret = CTOS_SetFunKeyPassword(strTCT.szFunKeyPW, 1);
	vdDebug_LogPrintf("CTOS_SetFunKeyPassword=[%s],ret=[%d]", strTCT.szFunKeyPW,ret);
	
	ret = CTOS_SetPMEnterPassword(strTCT.szPMpassword, 1);	
	vdDebug_LogPrintf("CTOS_SetPMEnterPassword=[%s],ret=[%d]", strTCT.szPMpassword,ret);

		/**********************
			CTOS_PrinterSetHeatLevel 
			default value is 2. 
			= 0 : Ultra light. 	= 1 : Very light. 
			= 2 : Light. 		= 3 : Medium. 
			= 4 : Dark. 		= 5 : Very dark. 
			= 6 : Ultra dark.
		***********************/
	ret = CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);	
	vdDebug_LogPrintf("CTOS_PrinterSetHeatLevel=[%d],ret=[%d]", strTCT.inPrinterHeatLevel,ret);
	return SUCCESS;
}

int inCTOS_IdleEventProcess(void)
{
#define CTOS_ILDE_TIMER  60

    DWORD dwWait=0, dwWakeup=0;
    USHORT ret;
    USHORT byMSR_status;
    BYTE key;
    BYTE bySC_status;
    BYTE babuff[128];
    BYTE sT1Buf[128] ,sT2Buf[128] ,sT3Buf[128];
    USHORT bT1Len=128 ,bT2Len=128 ,bT3Len=128;
    int i = 0;
    int i1=0, i2=0;
    BYTE baTemp[128];

    CTOS_SCStatus(d_SC_USER, &bySC_status);
    if(bySC_status & d_MK_SC_PRESENT)
    {
        inSetTextMode();
        vdRemoveCard();
    }
    
    CTOS_TimeOutSet(TIMER_ID_4, CTOS_ILDE_TIMER);  //Only IDLE loop use TIMER_ID_4, please don't use this TIMER_ID_4 in other place in application
    vduiKeyboardBackLight(VS_FALSE);

    vdDebug_LogPrintf("**strTCT.fECR[%d]**", strTCT.fECR);
	if (strTCT.fECR) // tct
	{
		usCTOSS_ECRInitialize();	
	}

	inCTOSS_InitAP();
	fBinVerInitiatedSALE=VS_FALSE;

	//06112015
//	fCommAlreadyOPen = FALSE;		
	isPredial=0;
	//06112015

	//0723
//	inPINEntryCount=0;
	//0723

	// BDO: Added flag to supress merc password during auto Log-On -- jzg
	fAutoLogOn = FALSE;
    
	while(1)
    {
		//Open the Back Light in the LCD Display //
		//vduiLightOn();
		//return idle delay problem, move to head
		if(CTOS_TimeOutCheck(TIMER_ID_4) == d_YES)
        {
            i = 0;
            while(g_IdleRunningFun[i].inCTOS_IdleRunningFun != NULL)
                g_IdleRunningFun[i++].inCTOS_IdleRunningFun();
        }
        
        dwWait = d_EVENT_KBD | d_EVENT_MSR | d_EVENT_SC;
        //System waits the device the be set acts.   //System waits for target devices to be triggered and will not return until any target device is triggered //
        
        ret = CTOS_SystemWait(20, dwWait, &dwWakeup);

        vdSetFirstIdleKey(0x00);
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if ((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD)
        {

            vduiKeyboardBackLight(VS_TRUE);
            //Detect if any key is pressed and get which key, but not get actual key.
            //If you get actual key, use to "CTOS_KBDGet" function//
            CTOS_KBDGet(&key);
            
            switch(key)
            {
                case d_KBD_F1:                     
                    inF1KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;

                case d_KBD_F2:                     
					inF2KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;

                case d_KBD_F3:                     
					inF3KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;

                case d_KBD_F4:                    
					inF4KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;

                case d_KBD_ENTER:                    
					inEnterKeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;

                case d_KBD_CANCEL:
                    break;

                case d_KBD_0:
                    inF1KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;
                case d_KBD_1:	
                case d_KBD_2:
                case d_KBD_3:		
                case d_KBD_4:
                case d_KBD_5:
                case d_KBD_6:
                case d_KBD_7:
                case d_KBD_8:
                case d_KBD_9:
                    vdSetFirstIdleKey(key);
					//gcitra
                    //inCTOS_SALE();
                    inCTOS_WAVE_SALE();
					//gcitra
                    vdSetFirstIdleKey(0x00);
                    CTOS_LCDTClearDisplay();
                    break;
                    
                default:
	                break;
            }

            vduiKeyboardBackLight(VS_FALSE);
            
        }
        else if ((dwWakeup & d_EVENT_MSR) == d_EVENT_MSR)
        {
            //Open the Back Light in the LCD Display //
            vduiKeyboardBackLight(VS_TRUE);
            vduiLightOn();
            vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
            
            inCTOS_SALE();
            CTOS_LCDTClearDisplay();
	        vduiKeyboardBackLight(VS_FALSE);
            
            continue;
            
        }
        else if (((dwWakeup & d_EVENT_SC) == d_EVENT_SC) || (bySC_status & d_MK_SC_PRESENT))
        {
            //Open the Back Light in the LCD Display //
            vduiKeyboardBackLight(VS_TRUE);
            vduiLightOn();
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
            
            inCTOS_SALE();
            CTOS_LCDTClearDisplay();
            vduiKeyboardBackLight(VS_FALSE);
            
            continue;
        }
	#if 0
        else if(CTOS_TimeOutCheck(TIMER_ID_4) == d_YES)
        {
            i = 0;
            while(g_IdleRunningFun[i].inCTOS_IdleRunningFun != NULL)
                g_IdleRunningFun[i++].inCTOS_IdleRunningFun();
            
            continue;
        }
     #endif   
    }
}

int main(int argc,char *argv[])
{
	inSetTextMode();

	inCTOSS_ProcessCfgExpress();
    
    inTCTRead(1);

    inCPTRead(1);

    inPCTRead(1);

    inCSTRead(1);

    inTCPRead(1);

    vdThreadRunAppColdInit();
	
	inMultiAP_ForkTask();
	if(d_OK == inMultiAP_CheckSubAPStatus())
	{
		vdDebug_LogPrintf("inWaitTime =[%d]",strTCT.inWaitTime);
		if (strTCT.inWaitTime <= 0)
			strTCT.inWaitTime = 100;
		while (1)
		{
			inMultiAP_GetMainroutine();			
			CTOS_Delay(strTCT.inWaitTime);
		}
	}
	else
	{
		inMultiAP_ForkSharlsAp();
	}
	
    CTOS_LCDTClearDisplay();
	inCTOS_DisplayIdleBMP();
	CTOS_Delay(1000);

	inCTLOS_Getpowrfail();


    //gcitra
    inInitializePinPad();
	//gcitra
	
    inCTOS_IdleEventProcess();

}

