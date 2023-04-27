#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>


#include "../Includes/msg.h"
#include "../Includes/wub_lib.h"
#include "../Includes/myEZLib.h"
#include "../Includes/POSTypedef.h"
#include "../Includes/POSTrans.h"

#include "display.h"
#include "../FileModule/myFileFunc.h"
#include "../print/Print.h"
#include "../Includes/CTOSinput.h"
#include "../UI/Display.h"
#include "../Comm/V5Comm.h"

extern USHORT GPRSCONNETSTATUS;
extern BOOL fAdviceTras;

#define ERRORLEN 30
static char szErrorMessage[ERRORLEN+1];

void setLCDReverse(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;

    //Set the reverse attribute of the character //
    CTOS_LCDTSetReverse(TRUE);  //the reverse enable // 
    
    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (16 - lens) / 2 + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 16 - lens + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
    }

    //Set the reverse attribute of the character //
    CTOS_LCDTSetReverse(FALSE); //the reverse enable //     
}

/* BDO-00122: Change line busy to line busy, please try again - start -- jzg */
void setLCDPrint(int line,int position, char *pbBuf)
{
	int iInitX = 0;
	int lens = 0;

	CTOS_LCDTPrintXY(1, line, "                      ");
	
	switch(position)
	{
		case DISPLAY_POSITION_LEFT:
			CTOS_LCDTPrintXY(1, line, pbBuf);
			break;

		case DISPLAY_POSITION_CENTER:
			lens = strlen(pbBuf);

			if((strTCT.byTerminalType == 1) || (strTCT.byTerminalType == 2))
				iInitX = ((22 - lens) / 2) + 1;
			else
				iInitX = ((30 - lens) / 2) + 1;

			CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			break;

		case DISPLAY_POSITION_RIGHT:
			lens = strlen(pbBuf);
			iInitX = 16 - lens + 1;
			CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			break;
	}
}
/* BDO-00122: Change line busy to line busy, please try again - end -- jzg */

void showAmount(IN  BYTE bY, BYTE bStrLen, BYTE *baBuf)
{
    int i;
    
    if(bStrLen > 2)
    {
        CTOS_LCDTPrintXY(13, bY, "0.00");
        for(i = 0;i < bStrLen; i++)
        {
            if ((16 - bStrLen + 1 + i) > 14)
                CTOS_LCDTPutchXY(16 - bStrLen + 1 + i, bY, baBuf[i]);
            else
                CTOS_LCDTPutchXY(16 - bStrLen + i, bY, baBuf[i]);
        }
    }
    else
    {
        CTOS_LCDTPrintXY(13, bY, "0.00");
        for(i = 0;i < bStrLen; i++)
        {
            CTOS_LCDTPutchXY(16 - bStrLen + 1 + i, bY, baBuf[i]);
        }
    }
}

void vduiDisplayInvalidTLE(void)
{
    
    vduiClearBelow(2);
    vduiWarningSound();
    vduiDisplayStringCenter(3,"INVALID SESSION");
    vduiDisplayStringCenter(4,"KEY, PLEASE");
    vduiDisplayStringCenter(5,"DWD SESSION KEY");
    vduiDisplayStringCenter(6,"--INSTRUCTION---");
    CTOS_LCDTPrintXY(1,7,"PRESS [F2] THEN");
    CTOS_LCDTPrintXY(1,8,"PRESS [3]");
    
    CTOS_Delay(2500);
}


void szGetTransTitle(BYTE byTransType, BYTE *szTitle)
{    
    int i;
    szTitle[0] = 0x00;
    
    //vdDebug_LogPrintf("**szGetTransTitle START byTransType[%d]Orig[%d]**", byTransType, srTransRec.byOrgTransType);
    switch(byTransType)
    {
        case SALE:
            strcpy(szTitle, "DEBIT SALE");
            break;
        case PRE_AUTH:
            strcpy(szTitle, "PRE AUTH");
            break;
        case PRE_COMP:
            strcpy(szTitle, "AUTH COMP");
            break;
        case REFUND:
            strcpy(szTitle, "REFUND");
            break;
        case VOID:
            if(REFUND == srTransRec.byOrgTransType)
                strcpy(szTitle, "VOID REFUND");
            else
                strcpy(szTitle, "VOID");
            break;
        case SALE_TIP:
            strcpy(szTitle, "TIP ADJUST");
            break;
        case SALE_OFFLINE:
            strcpy(szTitle, "OFFLINE");
            break;
        case SALE_ADJUST: 
            strcpy(szTitle, "ADJUST");
            break;
        case SETTLE:
            strcpy(szTitle, "SETTLE");
            break;
        case SIGN_ON:
			//gcitra
			//strcpy(szTitle, "SIGN ON");
			strcpy(szTitle, "LOGON");
			//gcitra
            break;
        case BATCH_REVIEW:
            strcpy(szTitle, "BATCH REVIEW");
            break;
        case BATCH_TOTAL:
            strcpy(szTitle, "BATCH TOTAL");
            break;
        case REPRINT_ANY:
            strcpy(szTitle, "REPRINT RECEIPT");
            break;

		//gcitra
		case BIN_VER:
			strcpy(szTitle, "BIN CHECK");
			break;
		case CASH_LOYALTY:
			strcpy(szTitle, "REWARD INQUIRY");
			break;	
		case POS_AUTO_REPORT:
			strcpy(szTitle, "POS AUTO REPORT");
			break;	
		case BALANCE_INQUIRY:
			strcpy(szTitle, "BALANCE");
			break;	
		
		case RELOAD:
			strcpy(szTitle, "RELOAD");
			break;			
		//gcitra

		//sidumili: [for LOGON]
		case LOG_ON:
			strcpy(szTitle, "LOG-ON");
			break;
		//sidumili: [for LOGON]	
		

        default:
            strcpy(szTitle, "");
            break;
    }
    i = strlen(szTitle);
    szTitle[i]=0x00;
    return ;
}

void vdDispTransTitle(BYTE byTransType)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1];
    int iInitX = 1;
   
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(byTransType, szTitle);
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2 ;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    //memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle));
	memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
    CTOS_LCDTSetReverse(TRUE);
    CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTSetReverse(FALSE);

	
}

//aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE start 1 of 6
#if 0
void vdDispTransTitleCardTypeandTotal(BYTE byTransType)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
    int iInitX = 1;
		int inCardLabellen, inCardDispStart, inMaxDisplen;
		char szDisplayCardLable [MAX_CHAR_PER_LINE+1];
   
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(byTransType, szTitle);
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2 ;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
		memcpy(&szDisplayCardLable[0],srTransRec.szCardLable,sizeof(szDisplayCardLable));
		
    inCardLabellen = strlen(szDisplayCardLable);
   
    switch(inCardLabellen)
    	{
    	  case 3:
             inMaxDisplen = 29;
             break;
          case 4:
             inMaxDisplen = 30;
             break;
          case 6:
             inMaxDisplen = 32;
			 break;
          case 7:
             inMaxDisplen = 33;
             break;
          case 8:
             inMaxDisplen = 34;
             break;
          case 9:
             inMaxDisplen = 35;
             break;
          
          default:
             inMaxDisplen = 26;
             break;
				
    	}
       if (srTransRec.byTransType == SALE) 
       {
            inCardDispStart = inMaxDisplen - inCardLabellen;
            memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
            CTOS_LCDTSetReverse(TRUE);
            CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
            CTOS_LCDTSetReverse(FALSE);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "Total:");;
            inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 5, DISPLAY_POSITION_CENTER);
       }
}
#else
void vdDispTransTitleCardTypeandTotal(BYTE byTransType)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
    int iInitX = 1;
	int inCardLabellen, inCardDispStart, inMaxDisplen;
	char szDisplayCardLable [MAX_CHAR_PER_LINE+1];
	//issue-00371
		BYTE szBaseAmt[20+1];

	if(srTransRec.byPackType == TC_UPLOAD || fAdviceTras == TRUE)
		return;


		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
   
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(byTransType, szTitle);
    iInitX = 20;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
	memcpy(&szDisplayCardLable[0],srTransRec.szCardLable,sizeof(szDisplayCardLable));
		
    inCardLabellen = strlen(szDisplayCardLable);

   	  if (srTransRec.byTransType == SALE){
	  	memset(szTitle, 0x00, sizeof(szTitle));
	  		memcpy(szTitle,"SALE",5);
   	  }
	  
      if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == RELOAD))
      {
         memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
         memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
		 
         memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
		 
         inCardDispStart = iInitX  - inCardLabellen;
         memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
         CTOS_LCDTSetReverse(TRUE);
         CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
         CTOS_LCDTSetReverse(FALSE);
		 CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
         sprintf(szCurAmtBuff,"%s%s",strCST.szCurSymbol,szAmtBuff);
         setLCDPrint(3, DISPLAY_POSITION_LEFT, "TOTAL:");
         //inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 5, DISPLAY_POSITION_CENTER);
         //issue-371
 
		 wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
		 vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBaseAmt,szAmtBuff);
		 
		 if (srTransRec.byTransType == VOID)   
		 {
			 sprintf(szCurAmtBuff,"%s-%s",strCST.szCurSymbol, szAmtBuff);
			 setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
		 }else{
			 sprintf(szCurAmtBuff,"%s%s",strCST.szCurSymbol, szAmtBuff);
			 //issue-00371
			 //inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 5, DISPLAY_POSITION_CENTER);	  
			 setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
		 }

      }
	  else if (srTransRec.byTransType == BALANCE_INQUIRY)
      {
         memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
         memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
		 
         memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
		 
         inCardDispStart = iInitX  - inCardLabellen;
         memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
         CTOS_LCDTSetReverse(TRUE);
         CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
         CTOS_LCDTSetReverse(FALSE);
		 CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
         

      }
	  else
	     CTOS_LCDFontSelectMode(d_FONT_TTF_MODE); 
}

#endif
//aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE end 1 of 6 


void vdDispTitleString(BYTE *szTitle)
{
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1];
    int iInitX = 1;
       
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle));
    CTOS_LCDTSetReverse(TRUE);
    CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTSetReverse(FALSE);
}

USHORT clearLine(int line)
{
    CTOS_LCDTGotoXY(1,line);
    CTOS_LCDTClear2EOL();
    //CTOS_LCDTPrintXY(1, line, "                     ");
}

void vdDisplayTxnFinishUI(void)
{
	/*sidumili: Issue:#000139 [do not display for LOG ON trxn]*/
	if (srTransRec.byTransType == LOG_ON)
		return;

    //CTOS_LCDTClearDisplay();
    //setLCDPrint(3, DISPLAY_POSITION_CENTER, "Transaction");
    //setLCDPrint(4, DISPLAY_POSITION_CENTER, "Approved");
    //setLCDPrint(5, DISPLAY_POSITION_CENTER, srTransRec.szAuthCode);   

	if(strTCT.fDisplayAPPROVED == TRUE && srTransRec.byTransType != BALANCE_INQUIRY) // Terminal will display the SMAC balance instead of the "APPROVED" message. 
	{
		CTOS_LCDTClearDisplay();
		if ((strTCT.byTerminalType % 2) == 1) 
	        vduiDisplayStringCenter(5,"APPROVED");
	    else 
	        vduiDisplayStringCenter(8, "APPROVED");
		CTOS_Beep();
	}
#if 0
	else
	{
		if(srTransRec.byTransType != BALANCE_INQUIRY && srTransRec.byTransType != LOG_ON)
		{
			if ((strTCT.byTerminalType % 2) == 1) 
				CTOS_LCDTPrintXY(1, 8, "PRINTING...");
			else 
				CTOS_LCDTPrintXY(1, 16, "PRINTING...");
		}
	}
#endif
	


	
}

void vdDispErrMsg(IN BYTE *szMsg)
{
    char szDisplayMsg[40];
    BYTE byKeyBuf;
    
    CTOS_LCDTClearDisplay();
    if(srTransRec.byTransType != 0)
        vdDispTransTitle(srTransRec.byTransType);

    memset(szDisplayMsg, 0x00, sizeof(szDisplayMsg));
    strcpy(szDisplayMsg, szMsg);
    vduiClearBelow(8);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, szDisplayMsg);
    CTOS_TimeOutSet (TIMER_ID_2 , 2*100);
    CTOS_Sound(1000, 50);
    
    while (1)
    {        
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL ||byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            return ;
        }
    }
}


int vdDispTransactionInfo(void)
{
    BYTE byKeyBuf;
    BYTE szTmp1[16+1];
    BYTE szTmp2[16+1];
    
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "Card NO.");
    setLCDPrint(3, DISPLAY_POSITION_LEFT, srTransRec.szPAN);
    memset(szTmp1, 0x00, sizeof(szTmp1));
    memset(szTmp2, 0x00, sizeof(szTmp2));
    wub_hex_2_str(srTransRec.szInvoiceNo, szTmp1, 3);
    sprintf(szTmp2,"Inv No:%s", szTmp1);
    setLCDPrint(4, DISPLAY_POSITION_LEFT, szTmp2);
    
    wub_hex_2_str(srTransRec.szTotalAmount, szTmp1, 6);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Amount:");
    sprintf(szTmp2,"SGD%7lu.%02lu", (atol(szTmp1)/100), (atol(szTmp1)%100));
    setLCDPrint(6, DISPLAY_POSITION_RIGHT, szTmp2);  
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRS ENTR TO CONF");
    CTOS_TimeOutSet (TIMER_ID_2 , 30*100);
    
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
            return  READ_CARD_TIMEOUT;
        
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL)
        {
            CTOS_KBDBufFlush ();
            return USER_ABORT;
        }
        else if (byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            return d_OK;
        }
    }
}

USHORT showBatchRecord(TRANS_DATA_TABLE *strTransData)
{
    char szStr[DISPLAY_LINE_SIZE + 1];
    char szTemp[DISPLAY_LINE_SIZE + 1];
    BYTE byKeyBuf;
    CTOS_LCDTClearDisplay();
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    sprintf(szStr, "%s", strTransData->szPAN);
    setLCDPrint(1, DISPLAY_POSITION_LEFT, "Card NO:");
    setLCDPrint(2, DISPLAY_POSITION_LEFT, szStr);
    
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    memset(szTemp, ' ', DISPLAY_LINE_SIZE);
    wub_hex_2_str(strTransData->szBaseAmount, szTemp, AMT_BCD_SIZE);
    sprintf(szStr, "%lu.%lu", atol(szTemp)/100, atol(szTemp)%100);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Amount:");
    setLCDPrint(4, DISPLAY_POSITION_LEFT, szStr);

    
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    sprintf(szStr, "%s", strTransData->szAuthCode);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Auth Code:");
    setLCDPrint(6, DISPLAY_POSITION_LEFT,  szStr);


    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    memset(szTemp, ' ', DISPLAY_LINE_SIZE);
    wub_hex_2_str(strTransData->szInvoiceNo, szTemp, INVOICE_BCD_SIZE);
    sprintf(szStr, "%s", szTemp);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "Invoice NO:");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, szTemp);
     
    CTOS_TimeOutSet (TIMER_ID_2 , 30*100);   
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
        {
            CTOS_LCDTClearDisplay();
            return  READ_CARD_TIMEOUT;
        }
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL)
        {
            CTOS_KBDBufFlush ();
            CTOS_LCDTClearDisplay();
            return USER_ABORT;
        }
        else if (byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            CTOS_LCDTClearDisplay();
            return d_OK;
        }
    }
}

void vduiLightOn(void)
{
    if (strTCT.fHandsetPresent)  
        CTOS_BackLightSetEx(d_BKLIT_LCD,d_ON,80000);
    else
        CTOS_BackLightSet (d_BKLIT_LCD, d_ON);
}

void vduiKeyboardBackLight(BOOL fKeyBoardLight)
{
    if (strTCT.fHandsetPresent) 
    {
        if(VS_TRUE == fKeyBoardLight)
        {
            
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffff);
            CTOS_BackLightSetEx(d_BKLIT_LCD,d_ON,0xffffff);
        }
        else
        {
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_OFF,100);
            CTOS_BackLightSetEx(d_BKLIT_LCD,d_OFF,3000);
        }

    }
    else
    {
        if(VS_TRUE == fKeyBoardLight)
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffff);
        else
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_OFF,100);
    }
}

void vduiPowerOff(void)
{
    BYTE block[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    USHORT ya,yb,xa,xb;
    unsigned char c;
        
    vduiClearBelow(1);
    vduiDisplayStringCenter(3,"ARE YOU SURE");
    vduiDisplayStringCenter(4,"WANT TO POWER");
    vduiDisplayStringCenter(5,"OFF TERMINAL");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
    c=WaitKey(60);
    
    if(c!=d_KBD_ENTER)
    {
            return;
    }    
    
    for(ya =1; ya<5; ya++)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,ya);
        CTOS_LCDTClear2EOL();
    }
    for(yb=8; yb>4; yb--)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,yb);
        CTOS_LCDTClear2EOL();
    }
    CTOS_LCDTPrintXY(1,4,"----------------");
    for(xa=1; xa<8; xa++)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xa,4," ");
    }
    for(xb=16; xb>7; xb--)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xb,4," ");
    }
            
    CTOS_LCDGShowPic(58, 6, block, 0, 6);
    CTOS_Delay(250);
    CTOS_LCDTGotoXY(7,4);
    CTOS_LCDTClear2EOL();
    CTOS_Delay(250);

    CTOS_PowerOff();
}

void vduiDisplayStringCenter(unsigned char  y,unsigned char *sBuf)
{
    setLCDPrint27(y, DISPLAY_POSITION_CENTER, sBuf);
}

void vduiClearBelow(int line)
{
	int i = 0,
		inNumOfLine = 8;

	/* BDOCLG-00005: should clear the rest of the line even for V3 terminals - start -- jzg */
	inTCTRead(1);
	if((strTCT.byTerminalType % 2) == 0)
		inNumOfLine = 16;

	for(i=line; i<=inNumOfLine; i++)
		clearLine(i);
	/* BDOCLG-00005: should clear the rest of the line even for V3 terminals - end -- jzg */
}



void vduiWarningSound(void)
{
    CTOS_LEDSet(d_LED1, d_ON);
    CTOS_LEDSet(d_LED2, d_ON);
    CTOS_LEDSet(d_LED3, d_ON);
    
    CTOS_Beep();
    CTOS_Delay(300);
    CTOS_Beep();
    
    CTOS_LEDSet(d_LED1, d_OFF);
    CTOS_LEDSet(d_LED2, d_OFF);
    CTOS_LEDSet(d_LED3, d_OFF);
}


void vdDisplayErrorMsg(int inColumn, int inRow,  char *msg)
{
	int inRowtmp;
	
    if ((strTCT.byTerminalType % 2) == 0)
		inRowtmp = V3_ERROR_LINE_ROW;
	else
        inRowtmp = inRow;

    clearLine(inRowtmp);
		
    CTOS_LCDTPrintXY(inColumn, inRowtmp, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp, msg);
    CTOS_Beep();
    CTOS_Delay(1500);

	clearLine(inRowtmp);
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated start 3 of 5
void vdDisplayErrorMsgResp (int inColumn, int inColumn2, int inColumn3, int inRow, int inRow2, int inRow3,  char *msg, char *msg2, char *msg3)
{
     CTOS_LCDTPrintXY(inColumn, inRow, "                                        ");
     CTOS_LCDTPrintXY(inColumn2, inRow2, "                                        ");
     CTOS_LCDTPrintXY(inColumn3, inRow3, "                                        ");
     CTOS_LCDTPrintXY(inColumn, inRow, msg);
     CTOS_LCDTPrintXY(inColumn2, inRow2, msg2);
     CTOS_LCDTPrintXY(inColumn3, inRow3, msg3);
     CTOS_Beep();
     CTOS_Delay(1500);
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated end 3 of 5

void vdDisplayErrorMsgResp2 (char *msg, char *msg2, char *msg3)
{
   CTOS_LCDTClearDisplay();

   if ((strTCT.byTerminalType % 2) == 1) 
   {
      vduiDisplayStringCenter(3,msg);
      vduiDisplayStringCenter(4,msg2);
      vduiDisplayStringCenter(5,msg3);		
   }
   else 
   {
      vduiDisplayStringCenter(6, msg);
      vduiDisplayStringCenter(7, msg2);
      vduiDisplayStringCenter(8, msg3);
   }
	 
   CTOS_Beep();
   CTOS_Delay(1500);
   CTOS_LCDTClearDisplay();
}


/* functions for loyalty - Meena 15/01/2012 - start*/
short vduiAskConfirmContinue(int inDisplay)
{
    unsigned char key;
  
    //vduiClearBelow(1);
    CTOS_LCDTClearDisplay();/*BDO: Clear window -- sidumili*/
    vduiDisplayStringCenter(3,"ARE YOU SURE");
    vduiDisplayStringCenter(4,"YOU WANT TO");
	if (inDisplay == 1)		
    	vduiDisplayStringCenter(5,"CLEAR BATCH?");
	else if (inDisplay == 2)	
    	vduiDisplayStringCenter(5,"DELETE REVERSAL?");
	else		
		vduiDisplayStringCenter(5,"CONTINUE?");
	//gcitra-0728
    //CTOS_LCDTPrintXY(1,7,"NO[X]   YES[OK] ");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
	//gcitra-0728
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            return d_OK;
        else if (key==d_KBD_CANCEL)
            return -1;
        else
            vduiWarningSound();
    }
    
}



BYTE struiGetchWithTimeOut(void)
{
    unsigned char c;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3,3000);
    
    while(1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey){ //If isKey is TRUE, represent key be pressed //
            vduiLightOn();
            //Get a key from keyboard //
            CTOS_KBDGet(&c);
            return c;   
        }
        else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {      
            return d_KBD_CANCEL;
        }
    }
}

/* functions for loyalty - Meena 15/01/2012 - End*/

short inuiAskSettlement(void)
{
    unsigned char key;
    while(1) 
    {
        vduiClearBelow(2);
        vduiDisplayStringCenter(2,"DAILY SETTLEMENT");
        vduiDisplayStringCenter(3,"NOTIFICATION");

        vduiDisplayStringCenter(5,"PERFORM");
        vduiDisplayStringCenter(6,"SETTLEMENT?");
        vduiDisplayStringCenter(8,"NO[X] YES[OK]");

        CTOS_KBDGet(&key);
        if(key==d_KBD_ENTER)
            return d_OK;
        else if(key==d_KBD_CANCEL)
            return d_KBD_CANCEL;
        else if(key==d_KBD_F1)
            vduiPowerOff();
    }
        
}

void vduiDisplaySignalStrengthBatteryCapacity(void)
{
    
    BYTE bCapacity, msg2[50];
    USHORT dwRet;
    short insign;
    
    
    if(GPRSCONNETSTATUS== d_OK && strTCT.inMainLine == GPRS_MODE)
    {
        insign=incommSignal();
        if(insign==-1)
        {
            CTOS_LCDTPrintXY (9,1, "SIGNAL:NA");
        }
        else
        {           
            if(insign/6 == 0)
                CTOS_LCDTPrintXY (9,1, "NO SIGNAL");
            else if(insign/6 == 1)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:l____"); 
            }
            else if(insign/6 == 2)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:ll___"); 
            }
            else if(insign/6 == 3)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:lll__"); 
            }
            else if(insign/6 == 4)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:llll_"); 
            }
            else if(insign/6 == 5)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:lllll"); 
            }
            
        }
    }
    
    dwRet= CTOS_BatteryGetCapacityByIC(&bCapacity);  
    if(dwRet==d_OK)
    {
        sprintf(msg2, "B:%d%% ", bCapacity);
        CTOS_LCDTPrintXY (3,1, msg2);
    }
                
}

void vdSetErrorMessage(char *szMessage)
{
    int inErrLen=0;

    inErrLen = strlen(szMessage);
    memset(szErrorMessage,0x00,sizeof(szErrorMessage));
    
    if (inErrLen > 0)
    {
        if (inErrLen > ERRORLEN)
            inErrLen = ERRORLEN;
        
        memcpy(szErrorMessage,szMessage,inErrLen);
    }
}

int inGetErrorMessage(char *szMessage)
{
    int inErrLen=0;

    inErrLen = strlen(szErrorMessage);

    if (inErrLen > 0)
    {       
        memcpy(szMessage,szErrorMessage,inErrLen);
    }
    
    return inErrLen;
}

//gcitra
void setLCDPrint27(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;

		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);

    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (20- lens) / 2 + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 20- lens + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
    }

		
		CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
}
//gcitra


//sidumili: display message
void vdDisplayMessage(char *szLine1Msg, char *szLine2Msg, char *szLine3Msg)
{
    CTOS_LCDTClearDisplay();
		vduiClearBelow(2);

		if ((strTCT.byTerminalType%2) == 0){
			CTOS_LCDTPrintAligned(8, szLine1Msg, d_LCD_ALIGNCENTER);	
			CTOS_LCDTPrintAligned(9, szLine2Msg, d_LCD_ALIGNCENTER);	
			CTOS_LCDTPrintAligned(10, szLine3Msg, d_LCD_ALIGNCENTER);
		}else{
			CTOS_LCDTPrintAligned(4, szLine1Msg, d_LCD_ALIGNCENTER);	
			CTOS_LCDTPrintAligned(5, szLine2Msg, d_LCD_ALIGNCENTER);	
			CTOS_LCDTPrintAligned(6, szLine3Msg, d_LCD_ALIGNCENTER);
		}
		//vduiDisplayStringCenter(4, szLine1Msg);
		//vduiDisplayStringCenter(5, szLine2Msg);
		//vduiDisplayStringCenter(6, szLine3Msg);
		WaitKey(1);
}
//sidumili

//sidumili: confirmation
short vduiAskEnterToConfirm(void)
{
    unsigned char key;
  
    
    CTOS_LCDTPrintXY(1,7,"CONFIRM?");
	
    CTOS_LCDTPrintXY(1,8,"[X]NO [OK]YES");
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            return d_OK;
        else if (key==d_KBD_CANCEL){
					
						//sidumili: disconnect communication when USER PRESS CANCEL KEY
						if (strCPT.inCommunicationMode == DIAL_UP_MODE){
										inCTOS_inDisconnect();
						}
						//sidumili: disconnect communication when USER PRESS CANCEL KEY
					
            return -1;
        	}
        else
            vduiWarningSound();
    }
    
}
//sidumili:

short vduiPressAnyKey(void)
{
    unsigned char key;
  
    vduiClearBelow(8);
    CTOS_LCDTPrintXY(1,8,"PRESS ANY KEY");

		#if 0
    while(1)
    {
        key = struiGetchWithTimeOut();

        break;
    }
		#else
			WaitKey(5);
		#endif
		
	return;
}

void vdClearBelowLine(int inLine)
{
    int i=0, inIndex=0;
    int inNoLine=8;
    char szTemp[360+1];

    memset(szTemp, 0, sizeof(szTemp));
    
    if ((strTCT.byTerminalType % 2) == 0)
        inNoLine=16;


    inNoLine-=inLine;
    inNoLine--;

    memset(szTemp, 0, sizeof(szTemp));
    for(i=0; i<=inNoLine; i++)
    {
        memcpy(&szTemp[inIndex], "                    \n", 21);
        inIndex+=21;
    }

    memcpy(&szTemp[inIndex], "                     ", 21);
    
    //vdDebug_LogPrintf("vdClearBelowLine");
    //vdDebug_LogPrintf("szTemp:[%s]", szTemp);
    //vdDebug_LogPrintf("inNoLine:[%d]", inNoLine);
    //vdDebug_LogPrintf("inLine:[%d]", inLine);

    setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szTemp);
    
}

