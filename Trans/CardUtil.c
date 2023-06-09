/*=========================================*       
 *             I N C L U D E S             *     
 *=========================================*/ 
#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <EMVAPLib.h>
#include <EMVLib.h>


#include "../Includes/myEZLib.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSTypedef.h"

#include "../FileModule/myFileFunc.h"
#include "..\Includes\CTOSInput.h"
#include "../UI/Display.h"
#include "../Debug/Debug.h"


void vdRemoveCard(void)
{
    BYTE bySC_status;

	#ifdef APP_AUTO_TEST
	if (inCTOS_GetAutoTestCnt() > 1)
	{
		vdDebug_LogPrintf("Auto Test ---> Remove card");
		vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"REMOVE CARD");
		CTOS_Delay(2000);
		return;
	}
    #endif
	
    while(1)
    {
            
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
			if ((strTCT.byTerminalType%2) == 0)
				vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"PLS REMOVE CARD");
			else
	            vduiDisplayStringCenter(7,"PLS REMOVE CARD");
            CTOS_Beep();
            CTOS_Delay(300);
            CTOS_Beep();
            continue;
        }
        
        break;
    }
}

short shChk_ExpireDate(void)
{
    BYTE     szYear[3];
    BYTE     szMonth[3];
    BYTE     szTemp[5];
    CTOS_RTC SetRTC;


    memset(szYear, 0x00, sizeof(szYear));
    memset(szMonth, 0x00, sizeof(szMonth));
    
    //Read the date and the time //
    CTOS_RTCGet(&SetRTC); 
    wub_hex_2_str(srTransRec.szExpireDate, szTemp, 2);    
    memcpy(szYear,&szTemp[0],2);   
    memcpy(szMonth,&szTemp[2],2);    

    
    vdDebug_LogPrintf("System Year[%d]-[%s] Month[%d]-[%s]",SetRTC.bYear ,szYear, SetRTC.bMonth, szMonth);
    /* modified as month and date copied wrongly - meena 08/01/2013*/
    if(SetRTC.bYear > atoi(szYear))
    {
        return CARD_EXPIRED;
    }
    else if(SetRTC.bYear == atoi(szYear))
    {
        if(SetRTC.bMonth > atoi(szMonth))
        {
            return CARD_EXPIRED;
        }
    }
    return d_OK;
}

short chk_luhn(BYTE *szPAN)
{
    int i ; 
    USHORT usSum , usWeight , usTmp ;
    BYTE byLen ;

    usSum = 0 ;
    usWeight = 2 ;
    byLen = strlen ( szPAN ) ;
       
    for ( i = byLen - 2 ; i >= 0 ; i -- )
    {
        usTmp = usWeight * ( szPAN[i] - '0' ) ;
        usSum += usTmp / 10 ;
        usSum += usTmp % 10 ;
        if ( usWeight == 2 )
            usWeight = 1 ;
        else
            usWeight = 2 ;
    }

    usTmp = szPAN[byLen - 1] - '0' ;

    if ( ( usTmp + usSum ) % 10 == 0 )
        return ( SUCCESS ) ;
    else
        return ( FAIL ) ;

}

void vdGetCardHolderName(BYTE *szTrack1, USHORT usTRack1Len)
{
    USHORT  usIndex ;
    BYTE    byNameFlag = 0;
    USHORT  usNameIndex = 0;
    
    for (usIndex = 0; usIndex < usTRack1Len;  usIndex++)
    {
        //if(szTrack1[usIndex] == '^')
        if((szTrack1[usIndex] == '^') || (szTrack1[usIndex] == '>')) // sidumili: [added separator '>' for BPI Card]
        {
            if(byNameFlag == 0)
            {
                byNameFlag = 1;
                continue;
            }
            else
                return;
        }
        
        if( byNameFlag == 1)
        {
            srTransRec.szCardholderName[usNameIndex] = szTrack1[usIndex];
            usNameIndex++;
        }
            
    }
}

void vdAnalysisTrack2(BYTE *szTrack2, USHORT usTRack2Len)
{
    USHORT  usIndex ;
    BYTE    byPanFlag = 0;
    BYTE    byExpFlag = 0;
    BYTE    bySerCodeFlag = 0;
    USHORT  usDataIndex  = 0;
    BYTE    szDataTmp[5];

    if(usTRack2Len > 0)
    {
        srTransRec.byPanLen=0;
        memset(srTransRec.szPAN, 0, sizeof(srTransRec.szPAN));
    }
        
    byPanFlag = 1;
    for(usIndex = 0; usIndex < usTRack2Len; usIndex++)
    {
        if(byPanFlag)
        {
            if((szTrack2[usIndex] == 'D') || (szTrack2[usIndex] == '=') || (szTrack2[usIndex] == '>'))
            {
                byPanFlag = 0;
                byExpFlag = 1;
                bySerCodeFlag = 0;
                srTransRec.byPanLen = usDataIndex;
                usDataIndex = 0;
                continue;
            }
            else
            {
                srTransRec.szPAN[usDataIndex] = szTrack2[usIndex];
                usDataIndex++;

            }
        }


		
        if(byExpFlag)
        {
            memset(szDataTmp, 0x00, sizeof(szDataTmp));
            wub_str_2_hex(&szTrack2[usIndex], szDataTmp, 4);
            srTransRec.szExpireDate[0] = szDataTmp[0];
            srTransRec.szExpireDate[1] = szDataTmp[1];
            usIndex = usIndex + 4;
            byPanFlag = 0;
            byExpFlag = 0;
            bySerCodeFlag = 1;
        }
        
        if(bySerCodeFlag)
        {
            memcpy(srTransRec.szServiceCode, &szTrack2[usIndex], 3);

            return;
        }
    }
	
		//WaitKey(60);

    return;
}


//gcitra
void vdAnalysisTrack1(BYTE *szTrack1, USHORT usTRack1Len)
{
    USHORT  usIndex ;
    BYTE    byPanFlag = 0;
    BYTE    byExpFlag = 0;
    BYTE    bySerCodeFlag = 0;
    USHORT  usDataIndex  = 0;
    BYTE    byNameFlag = 0;
    BYTE    szDataTmp[5];
    USHORT  usNameIndex = 0;


	
    byPanFlag = 1;
    for(usIndex = 0; usIndex < usTRack1Len; usIndex++)
    {
        if(byPanFlag)
        {
        if((szTrack1[usIndex] == '^') || (szTrack1[usIndex] == '>'))
            {
                byPanFlag = 0;
		  byNameFlag = 1;
                byExpFlag = 0;
                bySerCodeFlag = 0;
                srTransRec.byPanLen = usDataIndex;
                usDataIndex = 0;
                continue;
            }
            else
            {
                if(szTrack1[usIndex] != 'B'){

                srTransRec.szPAN[usDataIndex] = szTrack1[usIndex];
                usDataIndex++;
                }
            }
        }



        if( byNameFlag == 1)
        {
            //if(szTrack1[usIndex] == '^')
            if((szTrack1[usIndex] == '^') || (szTrack1[usIndex] == '>')) // sidumili: [added separator '>' for BPI Card]
						{
                byPanFlag = 0;
		  byNameFlag = 0;
                byExpFlag = 1;
                bySerCodeFlag = 0;

		  continue;
            }else{
            	srTransRec.szCardholderName[usNameIndex] = szTrack1[usIndex];
           	usNameIndex++;
	     }
	    
	     
        }

        
        if(byExpFlag)
        {
            memset(szDataTmp, 0x00, sizeof(szDataTmp));
            wub_str_2_hex(&szTrack1[usIndex], szDataTmp, 4);
            srTransRec.szExpireDate[0] = szDataTmp[0];
            srTransRec.szExpireDate[1] = szDataTmp[1];
            usIndex = usIndex + 4;
                byPanFlag = 0;
		  byNameFlag = 0;
                byExpFlag = 0;
                bySerCodeFlag = 1;

        }
        
        if(bySerCodeFlag)
        {
            memcpy(srTransRec.szServiceCode, &szTrack1[usIndex], 3);

            return;
        }
    }

	
	
    return;
}


//gcitra

