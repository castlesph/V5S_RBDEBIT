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
#include "..\Debug\Debug.h"
extern USHORT GPRSCONNETSTATUS;
extern BOOL fAdviceTras;

#define ERRORLEN 30
static char szErrorMessage[ERRORLEN+1];
static char szErrorMessage1[ERRORLEN+1];
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
    
    switch(byTransType)
    {
        case SALE:
			if(srTransRec.fInstallment == TRUE)
				strcpy(szTitle, "INSTALLMENT");
			else	
                strcpy(szTitle, "SALE");
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
            if(srTransRec.byOrgTransType == REFUND)
                strcpy(szTitle, "VOID REFUND");
			else if(srTransRec.fInstallment == TRUE) 
				strcpy(szTitle, "VOID INSTALLMENT");
			else if(srTransRec.byOrgTransType == LOY_REDEEM_5050 || srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE)
				strcpy(szTitle, "VOID REDEEM");
            else
                strcpy(szTitle, "VOID");
            break;
        case SALE_TIP:
			if(srTransRec.byOrgTransType == SALE)
                strcpy(szTitle, "SALE TIP ADJ");
			else if(srTransRec.byOrgTransType == SALE_OFFLINE)
                strcpy(szTitle, "OFFLINE TIP ADJ");
			else
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
            strcpy(szTitle, "SIGN ON");
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
        case BAL_INQ:
           strcpy(szTitle, "BALANCE INQUIRY");
           break;
        case LOG_ON:
           strcpy(szTitle, "LOGON");
           break;	
        case LOY_BAL_INQ:
            strcpy(szTitle, "INQUIRY");		
            break;
        case LOY_REDEEM_5050:
            strcpy(szTitle, "50/50 REDEEM");	   
            break;
			
        case LOY_REDEEM_VARIABLE:
            strcpy(szTitle, "VARIABLE REDEEM");	  
            break;
			
        case CARD_VERIFY:
            strcpy(szTitle, "CARD VERIFY");	  
        break;

        case CASH_OUT:
            strcpy(szTitle, "CASH OUT");
        break;
			
		case CASH_IN:
		    strcpy(szTitle, "CASH IN");
		break;

		case CARDLESS_CASH_OUT:
			strcpy(szTitle, "CARDLESS CASH OUT");
		break;
		
		case CARDLESS_BAL_INQ:
			strcpy(szTitle, "CARDLESS BAL INQ");
		break;

		case BILLS_PAYMENT:
			strcpy(szTitle, "BILLS PAYMENT");
		break;

        case FUND_TRANSFER:
            strcpy(szTitle, "FUND TRANSFER");
        break;

        case REQUEST_TERMINAL_KEY:
            strcpy(szTitle, "REQUEST TERMINAL KEY");
        break;

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
	unsigned char key;
	char szTemp1[24+1], szTemp2[24+1], szTemp3[24+1], szTotalAmt[24+1];

	if (srTransRec.byTransType == BAL_INQ) 
	return d_OK;
	
	if(isCheckTerminalMP200() == d_OK)
	{
		vdClearBelowLine(2);
		vdDispTransTitle(srTransRec.byTransType);

		if (srTransRec.byTransType == SALE_OFFLINE || srTransRec.byTransType == SALE_TIP)
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "TRANSACTION DETAILS");
		else
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "TRANSACTION APPROVED");
		
		memset(szTemp1, 0, sizeof(szTemp1));
		wub_hex_2_str(srTransRec.szInvoiceNo, szTemp1, INVOICE_BCD_SIZE);
		memset(szTemp2, 0, sizeof(szTemp2));
		sprintf(szTemp2, "TRACE NO.: %s", szTemp1);
		setLCDPrint(4, DISPLAY_POSITION_LEFT, szTemp2);   

		memset(szTemp1, 0, sizeof(szTemp1));
		sprintf(szTemp1, "APPR. CODE: %s", srTransRec.szAuthCode);
		setLCDPrint(5, DISPLAY_POSITION_LEFT, szTemp1);   

		if (srTransRec.byTransType == SALE_TIP)
		{
			// Tip Amount
			memset(szTemp1, 0, sizeof(szTemp1));
			memset(szTemp2, 0, sizeof(szTemp2));
			memset(szTotalAmt, 0, sizeof(szTotalAmt));
			wub_hex_2_str(srTransRec.szTipAmount, szTotalAmt, AMT_BCD_SIZE);
			vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szTemp1);
			sprintf(szTemp2, "TIP AMOUNT: %s%s", strCST.szCurSymbol, szTemp1);					
			setLCDPrint(6, DISPLAY_POSITION_LEFT, szTemp2);

			// Total Amount
			memset(szTemp1, 0, sizeof(szTemp1));
			memset(szTemp2, 0, sizeof(szTemp2));
			memset(szTotalAmt, 0, sizeof(szTotalAmt));
			wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
			vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szTemp1);
			sprintf(szTemp2, "AMOUNT: %s%s", strCST.szCurSymbol, szTemp1);					
			setLCDPrint(7, DISPLAY_POSITION_LEFT, szTemp2); 
		}
		else
		{
			// Total Amount
			memset(szTemp1, 0, sizeof(szTemp1));
			memset(szTemp2, 0, sizeof(szTemp2));
			memset(szTotalAmt, 0, sizeof(szTotalAmt));
			wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
			vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szTemp1);
			sprintf(szTemp2, "AMOUNT: %s%s", strCST.szCurSymbol, szTemp1);					
			setLCDPrint(6, DISPLAY_POSITION_LEFT, szTemp2); 
		}

		vduiDisplayStringCenter(8,"PLEASE PRESS ENTER");
		
		while(1)
		{
			key=WaitKey(1);
			if(key == d_KBD_ENTER)
				break;
			
			CTOS_Beep();
			CTOS_Delay(300);
			CTOS_Beep();
		}
	}
	else
	{
	#if 0
		CTOS_LCDTClearDisplay();
		setLCDPrint(3, DISPLAY_POSITION_CENTER, "Transaction");
		setLCDPrint(4, DISPLAY_POSITION_CENTER, "Approved");
		setLCDPrint(5, DISPLAY_POSITION_CENTER, srTransRec.szAuthCode);  
		
	#else //TINE:  android	
	strGBLVar.fGBLApprovedDisplay = (get_env_int("APPROVEDDISPLAY") > 0 ? TRUE : FALSE);

          if(strGBLVar.fGBLApprovedDisplay ==TRUE)
          {
               vdDisplayMessageBox(1, 8, "APPROVED", "Transaction", "", MSG_TYPE_SUCCESS);
               CTOS_Beep();
               CTOS_Delay(2000);
          }
		
	#endif
	}	
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
//CTOS_LCDTPrintXY(1,y,"                ");
//CTOS_LCDTPrintXY(1,y,"                          ");
//CTOS_LCDTPrintXY((20-strlen(sBuf))/2+1,y,sBuf);
    CTOS_LCDTPrintAligned(y,"                          ", d_LCD_ALIGNLEFT);	  
    CTOS_LCDTPrintAligned(y,sBuf, d_LCD_ALIGNCENTER);     
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

//from S1 code
void vdDisplayMessageBox(int inColumn, int inRow,  char *msg, char *msg2, char *msg3, int msgType)
{
//    setLCDPrint(8, DISPLAY_POSITION_LEFT, msg);
    CTOS_LCDTPrintAligned(8,"                          ", d_LCD_ALIGNLEFT);	  
    CTOS_LCDTPrintAligned(8,msg, DISPLAY_POSITION_LEFT);    
}

void vdDisplayMessageStatusBox(int inColumn, int inRow,  char *msg, char *msg2, int msgType)
{
}

int inCheckBatteryStatus(void)
{
}

int inCheckBattery(void)
{
}

int inCheckTMSBackUpFilesExists()
{
    
}

int inCTOSS_TMSReadDetailData()
{
    
}

int inCheckFileExist(const char *szFileName)
{
    
}

BYTE MenuTransactions(BYTE *szTrxnMenu, BYTE *szTransFunc)
{
}

int vdDisplayTrxn(int inSaleType, char *sztrxlogo)
{
}

BYTE InputStringUI(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *szInput)
{
     
}
void vdDisplayErrorMsg2(int inColumn, int inRow,  char *msg, char *msg2, int msgType)
{
    
}

void vdDisplayErrorMsg3(int inColumn, int inRow,  char *msg, char *msg2, char *msg3)
{
    
}

USHORT usCTOSS_LCDDisplay(BYTE *szStringMsg)
{
}

USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{
    
}

USHORT usCARDENTRY(BYTE *szStringMsg)
{
    
}

USHORT usCTOSS_BackToProgress(BYTE *szDispString)
{

}
void inDisplayLeftRight(int inLine, unsigned char *strLeft, unsigned char *strRight, int inMode)
{
    
}

USHORT usCTOSS_EditInfoListViewUI(BYTE *szDispString, BYTE *szOutputBuf)
{
    
}

short vduiAskConfirmation(char *szHeaderString)
{
    
}

USHORT usCTOSS_Confirm(BYTE *szDispString)
{
    BYTE key;
    int result = 0;
    CTOS_KBDBufFlush(); //cleare key buffer
    vdDebug_LogPrintf("usCTOSS_Confirm -- szDisMsg[%s],",szDispString);
    CTOS_LCDTPrintXY(1, 8, szDispString);
    CTOS_TimeOutSet (TIMER_ID_1 , 45*100);
    while (1) {

        CTOS_KBDHit(&key);
//        vdDebug_LogPrintf("key=%d", key);
        if (key == d_KBD_ENTER) {
			vduiClearBelow(7);
            result = d_OK;
            break;
        } else if ((key == d_KBD_CANCEL)) {
            result = d_NO;
            vdSetErrorMessage("USER CANCEL");
            break;
        }
        
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            vdSetErrorMessage("TIME OUT");
            return  d_NO;
        }
            
    }
    
    CTOS_KBDBufFlush ();
    return result;
}

USHORT usCTOSS_Confirm2(BYTE *szDispString)
{
    vdDebug_LogPrintf("usCTOSS_Confirm");
    return 0;
}

USHORT usCTOSS_Confirm3(BYTE *szDispString)
{
    vdDebug_LogPrintf("usCTOSS_Confirm");
    return 0;
}

void DisplayStatusLine(char *szDisplay) 
{
    
}

USHORT PrintReceiptUI(BYTE *szStringMsg)
{
    
    BYTE   key;
    CTOS_KBDBufFlush();
    key = 0;
    vdDebug_LogPrintf("PrintReceiptUI");
    //CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
    if ((strTCT.byTerminalType % 2) == 0) {
        //			vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"CUSTOMER COPY");
        vduiDisplayStringCenter(V3_STATUS_LINE_ROW, "MORE COPY");
        vduiDisplayStringCenter(V3_ERROR_LINE_ROW, "NO[X] YES[OK]");
    } else {
        //			vduiDisplayStringCenter(7,"CUSTOMER COPY");
        vduiDisplayStringCenter(7, "MORE COPY");
        vduiDisplayStringCenter(8, "NO[X] YES[OK]");
    }
    vduiWarningSound();

    while(1)
    {
        key = struiGetchWithTimeOut();
        if(key == d_KBD_ENTER)
        {

            return d_OK;
        }
        else if(key == d_KBD_CANCEL)
        {
            //needSecond=FALSE;
            vduiClearBelow(7);
                                    break;
        }
        else
            vduiWarningSound();
    }
    
    return d_USER_CANCEL;
    
}
BYTE InputQWERTY(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usMaxLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *display)
{

}
    
BYTE inDCCFxRate(BYTE *szDCCFxRateDetails)
{
}

USHORT usCTOSS_BatchReviewUI(BYTE *szDispString)
{
    
}

int inCallJAVA_Ping(BYTE *pbOutStr)
{
    
}

void vdGetCRC_ALL()
{
    
}

int inFLGRead(int id)
{
    
}

int inDetailReportHeader()
{
    
}

int usCTOSS_DisplayUI(char *szDisMsg)
{

}

int InputCVVUI(BYTE *baBuf, USHORT *usLens, USHORT usMinLen, int TO, char *szDisplay)
{
    
}

int is_utf8(const char * string)
{

//    vdDebug_LogPrintf("AAA -is_utf8 start ");
    if(!string)
        return 0;
//    vdDebug_LogPrintf("AAA -is_utf8 1 ");
    const unsigned char * bytes = (const unsigned char *)string;
    while(*bytes)
    {
        if( (// ASCII
             // use bytes[0] <= 0x7F to allow ASCII control characters
                bytes[0] == 0x09 ||
                bytes[0] == 0x0A ||
                bytes[0] == 0x0D ||
                (0x20 <= bytes[0] && bytes[0] <= 0x7E)
            )
        ) {
            bytes += 1;
            continue;
        }

        if( (// non-overlong 2-byte
                (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF)
            )
        ) {
            bytes += 2;
            continue;
        }

        if( (// excluding overlongs
                bytes[0] == 0xE0 &&
                (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// straight 3-byte
                ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                    bytes[0] == 0xEE ||
                    bytes[0] == 0xEF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// excluding surrogates
                bytes[0] == 0xED &&
                (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            )
        ) {
            bytes += 3;
            continue;
        }

        if( (// planes 1-3
                bytes[0] == 0xF0 &&
                (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// planes 4-15
                (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// plane 16
                bytes[0] == 0xF4 &&
                (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            )
        ) {
            bytes += 4;
            continue;
        }
//         vdDebug_LogPrintf("AAA -is_utf8 start return 0");
        return 0;
    }
//     vdDebug_LogPrintf("AAA -is_utf8 return 1 ");
    return 1;
}

int inMMTReadPreAuthHostName(char *szHostName, int inCPTID, int MITid, int inIndicator)
{

}

int inMMTReadHostName(char *szHostName, int inCPTID, int MITid, int inIndicator)
{
    
}

void vdDeleteISOLog()
{
}

void vdPrintISOLog(void)
{
    
}

void vdDeleteISOECRLog()
{
    
}
void vdPrintISOECRLog()
{
    
}


int inFunctionKeyPasswordEx(char *p, int inPasswordLevel)
{
    
}

int inPrintADCReport(BYTE flag)
{
    
}

int inCTOSS_TMSReadReprintDetailData(void)
{
    
}

int inCheckTMSReprintBackUpFilesExists(void)
{
    
}

int inDCCMMTUpdate()
{
    
}

int inCTOSS_TMSRestoreTxnData()
{
    
}

int inCTOS_SelectCurrencyfromIDLE()
{
    
}

USHORT usGetSerialNumber(BYTE baFactorySN)
{
    
}

int inDisplayMessageBoxWithButton(int inColumn, int inRow,  char *msg, char *msg2, char *msg3, int msgType)
{
    
}


int inMOBILE_GetConnectConfig(void)
{
    
}

USHORT getAppPackageInfo(BYTE *szAppName, BYTE *szStringMsg)
{
    
}


USHORT EliteReceiptUI(BYTE *szStringMsg)
{
    
}

void vdPrintReportDisplayBMP(void)
{
    
}

int inCTOS_TEMPCheckAndSelectMutipleMID(void)
{
    return 0;
}



int vdCTOS_EditBiller(void)
{
    
}

int vdCTOS_EditUser(void)
{
    
}

BYTE MenuTransType(BYTE *szTrxnMenu)
{
    
}

int inCTOS_CTMSUPDATE(void)
{
    
}

int inCTOSS_TMSDownloadRequestAfterSettle(void)
{
    
}

int inBVTRead(int inSeekCnt)
{
    
}
int inBVTSave(int inSeekCnt)
{
    
}

//BDO UAT 0007: Change comms shortcut -end -- jzg

//BDO: [Restart Function] -- sidumili
void vdCTOS_uiRestart(BOOL fConfirm)
{
    BYTE block[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    USHORT ya,yb,xa,xb;
    unsigned char c;

    CTOS_LCDTClearDisplay();
    
    if (fConfirm == TRUE)
    {
        vduiDisplayStringCenter(4, "RESTART TERMINAL?");
        vduiDisplayStringCenter(7, "YES[OK]   NO[X]"); 
        
        c=WaitKey(60);
        
        if(c!=d_KBD_ENTER)
        {
            return;
        }		
    }
    else
    {
        vduiDisplayStringCenter(3, "TO TAKE EFFECT");
        vduiDisplayStringCenter(4, "TERMINAL WILL");
        vduiDisplayStringCenter(5, "REBOOT");
        vduiDisplayStringCenter(6, "PLEASE WAIT...");
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

    CTOS_SystemReset();
}

void vdDisplayMultiLineMsgAlign(char *msg, char *msg2, char *msg3, int inPosition)
{
    
}

int inGetErrorMessages(char *szMessage, char *szMessage1)
{
    int inErrLen=0, inErrLen1=0;

    inErrLen = strlen(szErrorMessage);
	inErrLen1 = strlen (szErrorMessage1);
      
        memcpy(szMessage,szErrorMessage,inErrLen);
		memcpy(szMessage1,szErrorMessage1,inErrLen1);

    return inErrLen1;
}