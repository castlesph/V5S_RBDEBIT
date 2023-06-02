/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <emv_cl.h>
#include <vwdleapi.h>

#include <sys/stat.h>   
#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\display.h"

#include "..\Debug\Debug.h"

#include "..\Includes\CTOSInput.h"

#include "..\comm\v5Comm.h"
#include "..\Accum\Accum.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Comm\V5Comm.h"
#include "..\debug\debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\CardUtil.h"
#include "..\Debug\Debug.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\myEZLib.h"
#include "..\ApTrans\MultiShareEMV.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Ctls\POSCtls.h"
#include "..\Ctls\PosWave.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\print\print.h"
#include "..\Pinpad\Pinpad.h"


int inFallbackToMSR = FAIL;
static char szBaseAmount[6+1];

static BOOL fECRTxnFlg = 0;

extern BOOL fPinAlreadyEntered;

BOOL ErmTrans_Approved;
extern BOOL fPrinterStart;
extern BOOL fGPRSConnectOK;
BOOL fISOLog = FALSE;

extern BOOL fERMTransaction;
int gblinGPRSFallback;

void vdSetECRTransactionFlg(BOOL flg)
{
    put_env_int("ECRTRANS",flg);
    fECRTxnFlg = flg;
}

BOOL fGetECRTransactionFlg(void)
{
    fECRTxnFlg = get_env_int("ECRTRANS");
    return fECRTxnFlg;
}


extern int isdigit ( int c );

//android-remove
#if 0

USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{
    
    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[128];
    USHORT  usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));
    
    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);

    vdDebug_LogPrintf("start [%d] data[%s]", strlen(baBuf), baBuf);
    if(strlen(baBuf) > 0 )
    {
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bDisplayStr, 0x20, usMaxLen*2);
        usInputStrLen = strlen(baBuf);
        strcpy(bInputStrData, baBuf);
        if(0x01 == usLeftRight)
        {
            strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
        }
        else
        {
            memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
            CTOS_LCDTPrintXY(1, usY, bDisplayStr);
        }
    }
    
    while(1)
    {
//        vduiLightOn(); // patrick remark for flash light always
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        }
        
        CTOS_KBDHit(&bKey);

        switch(bKey)
        {
        case d_KBD_DOT:
            break;
        case d_KBD_CLEAR:
            if (usInputStrLen)
            {
                usInputStrLen--;
                bInputStrData[usInputStrLen] = 0x00;
                
                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, usMaxLen*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                    CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                }
            }
            break;
        case d_KBD_CANCEL:
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            if (usInputStrLen < usMaxLen )
            {
                bInputStrData[usInputStrLen++] = bKey;

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, usMaxLen*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                    CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                }
            }
            break;
        case d_KBD_ENTER:
            if(usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen)
            {   
                *usStrLen = usInputStrLen;
                strcpy(baBuf, bInputStrData);
                return *usStrLen;
            }
            else if(usByPassAllow && 0 == usInputStrLen)
            {   
                *usStrLen = usInputStrLen;
                baBuf[0] = 0x00;
                return *usStrLen;
            }
            break;
        default :
            break;
        }
    }

    return 0;
}
#endif

USHORT shCTOS_GetExpDate(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usTimeOutMS)
{
    
    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[20];
    BYTE    bInputFormatStr[20];
    USHORT  usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));
    
    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);
    
    while(1)
    {
        vduiLightOn();
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        }
        
        CTOS_KBDHit(&bKey);

        switch(bKey)
        {
        case d_KBD_DOT:
            break;
        case d_KBD_CLEAR:
            if (usInputStrLen)
            {
                usInputStrLen--;
                bInputStrData[usInputStrLen] = 0x00;

                memset(bInputFormatStr, 0x00, sizeof(bInputFormatStr));
                if(usInputStrLen >= 2)
                {
                    memcpy(bInputFormatStr, bInputStrData, 2);
                    strcat(bInputFormatStr, "/");
                    if(usInputStrLen > 2)
                        strcat(bInputFormatStr, &bInputStrData[2]);
                }
                else
                {
                    strcpy(bInputFormatStr, bInputStrData);
                }

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, (usMaxLen+1)*2);
                
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen+1-strlen(bInputFormatStr))*2], bInputFormatStr);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                    CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                }
            }
            break;
        case d_KBD_CANCEL:
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            if (usInputStrLen < usMaxLen )
            {
                bInputStrData[usInputStrLen++] = bKey;

                memset(bInputFormatStr, 0x00, sizeof(bInputFormatStr));
                if(usInputStrLen >= 2)
                {
                    memcpy(bInputFormatStr, bInputStrData, 2);
                    strcat(bInputFormatStr, "/");
                    if(usInputStrLen > 2)
                        strcat(bInputFormatStr, &bInputStrData[2]);
                }
                else
                {
                    strcpy(bInputFormatStr, bInputStrData);
                }

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, (usMaxLen+1)*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen+1-strlen(bInputFormatStr))*2], bInputFormatStr);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                    CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                }
            }
            break;
        case d_KBD_ENTER:
            if(usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen)
            {   
                *usStrLen = usInputStrLen;
                strcpy(baBuf, bInputStrData);
                return *usStrLen;
            }
            break;
        default :
            break;
        }
    }

    return 0;
}

USHORT getExpDate( OUT BYTE *baBuf)
{
    BYTE    szMonth[3];    
    USHORT  usRet;
    USHORT  usLens;
    USHORT  usMinLen = 4;
    USHORT  usMaxLen = 4;
    USHORT usInputLine = 8;
    
    while(1)
    {
        usRet = shCTOS_GetExpDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);
        memset(szMonth, 0x00, sizeof(szMonth));
        memcpy(szMonth, baBuf, 2);
        if(atol(szMonth) > 12 || 0 == atol(szMonth))
        {
            baBuf[0] = 0x00;
            vduiClearBelow(8);
            vdDisplayErrorMsg(1, 8, "INVALID FORMAT"); 
            vduiClearBelow(8);
            continue;
        }
        else
        {
            return (d_OK);
        }
    }
}

USHORT getCardNO(OUT BYTE *baBuf)
{
    USHORT usRet;
    USHORT usLens;
    USHORT usMinLen = 14;
    USHORT usMaxLen = 19;
    USHORT usInputLine = 8;
    
    while(1)
    {
        usRet = shCTOS_GetNum(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 0, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);
        if (usRet >= usMinLen && usRet <= usMaxLen)
        {
            return (d_OK);       
        }

        baBuf[0] = 0x00;
    }
}
    

unsigned char WaitKey(short Sec)
{
    unsigned char c;
    long mlsec;
    
    mlsec=Sec*100;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3,mlsec);
    while(1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey){ //If isKey is TRUE, represent key be pressed //
            
            vduiLightOn();
            CTOS_KBDGet(&c);
            return c;   
        }
        else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {      
            return d_KBD_CANCEL;
        }
    }
}


void vduiApiAmount(unsigned char *ou, unsigned char *ascamt, unsigned char len)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned int    ii, jj, tt;
    unsigned char   ch;
    unsigned char   aa;
    unsigned char   buf[3];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    jj = 0;
    tt = 0;
    ii = 0;
    
    
    ou[jj++] = strCST.szCurSymbol[0];
    ou[jj++] = strCST.szCurSymbol[1];
    ou[jj++] = strCST.szCurSymbol[2];
    
    for(ii = 0; ii < len; ii++)
    {
        ch = ascamt[ii];
        if((tt == 0) && (ch == 'C'))
        {
            tt = 1;
        }
        else if((tt == 0) && (ch == 'D'))
        {
            tt = 1;
            ou[jj++] = '-';
        }
        else if(ch<0x30 && ch>0x39 )
        {
            break;
        }
    }


    len = ii;
    aa = 0;
    for(ii = tt; (ii + 3) < len; ii++)
    {
        ch = ascamt[ii];
        if((ch == '0') && (aa == 0))
        {
            continue;
        }

        if(ch>0x29 && ch<0x40 )//if(isdigit(ch) /* && (ch !='0') */ )
        {
            aa = 1;
            ou[jj++] = ch;
        }
    }

    tt = ii;
    len = len - ii;
    buf[0] = '0', buf[1] = '0', buf[2] = '0';
    for(ii = 0; ii < len; ii++)
    {
        buf[3 - len + ii] = ascamt[tt++];
    }

    ou[jj++] = buf[0];
    ou[jj++] = '.';
    ou[jj++] = buf[1];
    ou[jj++] = buf[2];
    ou[jj++] = '\0';
}

void vduiApiPoint(unsigned char *ou, unsigned char *ascamt, unsigned char len)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned int    ii, jj, tt;
    unsigned char   ch;
    unsigned char   aa;
    unsigned char   buf[3];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    jj = 0;
    tt = 0;

    ou[jj++] = 'P';
    ou[jj++] = 'T';
    ou[jj++] = 'S';
    ou[jj++] = ' ';
    for(ii = 0; ii < len; ii++)
    {
        ch = ascamt[ii];
        if((tt == 0) && (ch == 'C'))
        {
            tt = 1;
        }
        else if((tt == 0) && (ch == 'D'))
        {
            tt = 1;
            ou[jj++] = '-';
        }
        else if(ch<0x30 && ch>0x39 )
        {
            break;
        }
    }


    len = ii;
    aa = 0;
    for(ii = tt; (ii + 3) < len; ii++)
    {
        ch = ascamt[ii];
        if((ch == '0') && (aa == 0))
        {
            continue;
        }

        if(ch>0x29 && ch<0x40 )
        {
            aa = 1;
            ou[jj++] = ch;
        }
    }

    tt = ii;
    len = len - ii;
    buf[0] = '0', buf[1] = '0', buf[2] = '0';
    for(ii = 0; ii < len; ii++)
    {
        buf[3 - len + ii] = ascamt[tt++];
    }

    ou[jj++] = buf[0];
    ou[jj++] = '.';
    ou[jj++] = buf[1];
    ou[jj++] = buf[2];
    ou[jj++] = '\0';
}

//mode  1=amount , 2=string, 3=IP  4=password, 5=Point
unsigned char struiApiGetStringSub
(
    unsigned char   *strDisplay,
    short   x,
    short   y,
    unsigned char   *ou,
    unsigned char   mode,
    short   minlen,
    short   maxlen
)
{
    
    unsigned char srDestIP[MAX_CHAR_PER_LINE+1];
    unsigned char amtdis[MAX_CHAR_PER_LINE+1];
    unsigned char c;
    int n;
    int i;
    
    memset(srDestIP,0x00,sizeof(srDestIP));
    n= 0;   

    vduiClearBelow(y);

    if(mode == MODE_AMOUNT)
    {
        vduiClearBelow(8);
        vduiApiAmount(amtdis, srDestIP, n);                     
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);  
        
    }
    else if(mode == MODE_POINT)
    {
        vduiClearBelow(8);
        vduiApiPoint(amtdis, srDestIP, n);                      
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);  
        
    }
    
    while(1)
    {
        
        CTOS_LCDTPrintXY(x,y,strDisplay);   
        c=WaitKey(30);//CTOS_KBDGet(&c);
        vduiLightOn();
        
        if (c == d_KBD_ENTER)
        {
            if((n>=minlen) && (n<=maxlen))
            {
                ou[n]=0;
                memcpy(ou,srDestIP,n+1);    
                if(mode==MODE_FOODITEM && atoi(ou)==0)
                {
                    return d_KBD_CANCEL;
                }
                
                if(mode==MODE_AMOUNT && atoi(ou)==0)
                {
                    i = atoi(ou);
                    memset(srDestIP,0x00,sizeof(srDestIP));
                    n=0;
                    vduiWarningSound();
                }
                else
                    return d_KBD_ENTER;
            }
            
        }       
        else if((c == d_KBD_CANCEL) && (n==0))          
        {
            memset(srDestIP,0x00,sizeof(srDestIP));         
            return d_KBD_CANCEL;
        }           

        else
        {
            if (c==d_KBD_CLEAR)
            {   
                if(n>0) 
                {
                    n--;
                    srDestIP[n]='\0';                   
                }       
            }
            else if((c == d_KBD_CANCEL))            
            {
                memset(srDestIP,0x00,sizeof(srDestIP));
                n=0;                
            }   
            else if ((c == d_KBD_DOT) & (mode==MODE_IPADDRESS) )
            {
                srDestIP[n]='.';
                n++;    
            }
            else if (c == d_KBD_DOT || c == d_KBD_F3  || c == d_KBD_F4 || c == d_KBD_00) 
            {
                ;
            }
            else if(c == d_KBD_UP || c == d_KBD_DOWN)
            {
                return c;
            }
            else if(n<maxlen)
            {
                srDestIP[n]=c;
                n++;    
            }
            else
            {

            }

            if(mode == MODE_AMOUNT)
            {
                vduiClearBelow(8);
                vduiApiAmount(amtdis, srDestIP, n);                     
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);  
                
            }
            else if(mode == MODE_POINT)
            {
                vduiClearBelow(8);
                vduiApiPoint(amtdis, srDestIP, n);                      
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);  
                
            }
            else if(mode == MODE_PASSWORD)
            {
                for(i=0;i<n;i++)
                    amtdis[i]='*';
                amtdis[n]=0;
                vduiClearBelow(8);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-n*2,8,amtdis);                       
            }
            else
            {   
                vduiClearBelow(8);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-n*2,8,srDestIP);                     
            }
        }
        
    }

}

void vdCTOSS_GetMemoryStatus(char *Funname)
{
    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;
    
    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];
    
    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];

    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;
    
    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize); 
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);
    
    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);    
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("[%s][%ld],[%ld][%ld][%ld]",Funname,ulUsedDiskSize,ulTotalRamSize,ulUsedRamSize,ulAvailableRamSize);

    CTOS_LCDTClearDisplay();
    
    CTOS_LCDTPrintXY(1, 1, szTotalDiskSize);
    CTOS_LCDTPrintXY(1, 2, szUsedDiskSize);
    CTOS_LCDTPrintXY(1, 3, szAvailableDiskSize);
    
    CTOS_LCDTPrintXY(1, 5, szTotalRamSize);
    CTOS_LCDTPrintXY(1, 6, szUsedRamSize);
    CTOS_LCDTPrintXY(1, 7, szAvailableRamSize);
    WaitKey(60);
    
}

int inCTOSS_CheckMemoryStatus()
{
#define SAFE_LIMIT_SIZE 5000

    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;
    
    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];
    
    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];
    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));
    
    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;
    
    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize); 
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);
    
    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);    
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]",ulAvailDiskSize,ulAvailableRamSize);

    if (ulAvailDiskSize < SAFE_LIMIT_SIZE)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1, 7, "Settle  soon");
        vdDisplayErrorMsg(1, 8,  "Insufficient Memory");
        return FAIL;
    }

    if (ulAvailableRamSize < SAFE_LIMIT_SIZE)
    {
        CTOS_LCDTClearDisplay();
        vdSetErrorMessage("Insufficient RAM");
        return FAIL;
    }
    
    return d_OK;
    
}

void vdCTOS_SyncHostDateTime()
{
    CTOS_RTC SetRTC;
    char szDate[4+1];
    char szTime[6+1];
    char szBuf[2+1];
	BYTE szDateTime[30+1];


	vdDebug_LogPrintf("saturn vdCTOS_SyncHostDateTime");

    if(srTransRec.byOffline == CN_TRUE)
        return;
   // CTOS_RTCGet(&SetRTC);
   
   //usGetRTCAndroid(&SetRTC);
   //vdDebug_LogPrintf("sys year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay,SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);

  //vdDebug_LogPrintf("year[%02X],time[%02x:%02x:%02x]date[%02x][%02x]",SetRTC.bYear,srTransRec.szTime[0],srTransRec.szTime[1],srTransRec.szTime[2],srTransRec.szDate[0],srTransRec.szDate[1]);

    memset(szDate,0,sizeof(szDate));
    memset(szTime,0,sizeof(szTime));

	//srTransRec.szDate[0] = 0x07;
	//srTransRec.szDate[1] = 0x17;

	//srTransRec.szTime[0] = 0x05;
	//srTransRec.szTime[1] = 0x06;
	//srTransRec.szTime[2] = 0x01;
	
	

    wub_hex_2_str(srTransRec.szDate, szDate, 2);
    wub_hex_2_str(srTransRec.szTime, szTime, 3);
	vdDebug_LogPrintf("saturn date %s", szDate);
	
	vdDebug_LogPrintf("saturn time %s", szTime);
    if((strlen(szDate)<=0) ||(strlen(szTime)<=0))
        return;

	memset(szDateTime, 0x00, sizeof(szDateTime));
	
    //sprintf(szBuf,"%02x",(unsigned int)atol(szDate)%100);
    //wub_str_2_hex(szBuf, &(SetRTC.bDay), 2);
	strncpy(szDateTime,&szDate[0],2);
	strcat(szDateTime,"|");

    //sprintf(szBuf,"%02x",(unsigned int)atol(szDate)/100);
    //wub_str_2_hex(szBuf, &(SetRTC.bMonth), 2);
	strncat(szDateTime,&szDate[2],2);
	strcat(szDateTime,"|");

	//if year is "x", app will get the year setting of the terminal
	strcat(szDateTime,"x|");

	//strcat(szDateTime,"20");
	//strcat(szDateTime,"|");

    //sprintf(szBuf,"%02x",(unsigned int)atol(szTime)/10000);
    //wub_str_2_hex(szBuf, &(SetRTC.bHour), 2);
	strncat(szDateTime,&szTime[0],2);
	strcat(szDateTime,"|");

    //sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%10000/100);
    //wub_str_2_hex(szBuf, &(SetRTC.bMinute), 2);
	strncat(szDateTime,&szTime[2],2);
	strcat(szDateTime,"|");

    //sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%100);
    //wub_str_2_hex(szBuf, &(SetRTC.bSecond), 2);
	strncat(szDateTime,&szTime[4],2);

    CTOS_RTCSet(&SetRTC);
    vdDebug_LogPrintf("saturn szDatetime %s", szDateTime);
//    usSetDateTime(szDateTime);			
	
    //vdDebug_LogPrintf("set year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay,SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);

    return;
}

int inCLearTablesStructure(void){

memset(&strCDT,0x00, sizeof(STRUCT_CDT));
memset(&strIIT,0x00, sizeof(STRUCT_IIT));
memset(&strEMVT,0x00, sizeof(STRUCT_EMVT));
memset(&strHDT,0x00, sizeof(STRUCT_HDT));
memset(srTransRec.szBaseAmount, 0x00, sizeof(srTransRec.szBaseAmount));
memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
memset(&srTransRec,0x00, sizeof(TRANS_DATA_TABLE));
    return d_OK;
}

void vdCTOS_TxnsBeginInit(void)
{
    BYTE bEntryMode = 0;
	int inRet = d_NO;

	vdDebug_LogPrintf("--vdCTOS_TxnsBeginInit--");

	//vdCTOSS_ReleaseAllDrvHandle();

	fPinAlreadyEntered = FALSE;
	
    if(d_OK == inCTOS_ValidFirstIdleKey())
        bEntryMode = srTransRec.byEntryMode;
    
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    
    //Clear Previous Transaction Data
    inCLearTablesStructure();
    
    if(0 != bEntryMode)
        srTransRec.byEntryMode = bEntryMode;

    vdSetErrorMessage("");
	vdCTOSS_SetWaveTransType(0);

    vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check if main app");
//add for ECR only MianAP get the ECR command, if Sub AP will cannot error
	//if (inMultiAP_CheckMainAPStatus() == d_OK)
	if (inMultiAP_CheckSubAPStatus() != d_OK)//only 1 APP or main APP
	{
	    vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check ECR on?[%d]", strTCT.fECR);
	    if (strTCT.fECR) // tct
	    {
	        vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check Database exist");
	    	if (file_exist (DB_MULTIAP))
	    	{
	    	    vdDebug_LogPrintf("vdCTOS_TxnsBeginInit Read ECR Data");
	    		inRet = inCTOS_MultiAPGetData();
                vdDebug_LogPrintf("vdCTOS_TxnsBeginInit Read ECR Data ret[%d]", inRet);
	    		if(d_OK != inRet)
	    			return ;
	    	}
	    }

	}
}

void vdCTOS_TransEndReset(void)
{
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    char szErrMsg[30+1];

	vdDebug_LogPrintf("--vdCTOS_TransEndReset--");
	
    vdDebug_LogPrintf("vdCTOS_TransEndReset ECR?[%d]", fGetECRTransactionFlg());
    CTOS_LCDTClearDisplay();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
    }

//test

    //if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
    //    vdRemoveCard();
   // else
    //    CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
//test

	vdRemoveCard();

	// Minimize the white screend display -- sidumili
	if (srTransRec.byTransType == SETTLE)
		vdCTOS_DispStatusMessage("PLEASE WAIT...");

	
    if(!fGetECRTransactionFlg())
    {
        memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    }

	// Minimize the white screend display -- sidumili
	if (isCheckTerminalMP200() == d_OK && strTCT.byERMMode != 0)			
		vdCTOS_DispStatusMessage("PLEASE WAIT...");
	
    CTOS_KBDBufFlush();

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    vdSetErrorMessage("");
	vdCTOSS_SetWaveTransType(0);

	inCLearTablesStructure(); // fix to clear saved card info on the terminal. - mfl
	
	ushCTOS_ClearePadSignature();	
	
	fPinAlreadyEntered = FALSE;
	ErmTrans_Approved = FALSE;
	fPrinterStart = FALSE;
	gblinGPRSFallback = 0;

    if(get_env_int("ECRPROCESS")==1)
         put_env_int("ECRPROCESS",0);
	
    return;
}

void vdCTOS_SetTransEntryMode(BYTE bEntryMode)
{
    srTransRec.byEntryMode = bEntryMode;
}

void vdCTOS_SetTransType(BYTE bTxnType)
{
    srTransRec.byTransType = bTxnType;
    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
}


void vdCTOS_SetMagstripCardTrackData(BYTE* baTk1Buf, USHORT usTk1Len, BYTE* baTk2Buf, USHORT usTk2Len, BYTE* baTk3Buf, USHORT usTk3Len)
{
    memcpy(srTransRec.szTrack1Data, &baTk1Buf[1], (usTk1Len -3));
    srTransRec.usTrack1Len = (usTk1Len - 3);// REMOVE %, ? AND LRC
    
    memcpy(srTransRec.szTrack2Data, &baTk2Buf[1], (usTk2Len - 3));
    srTransRec.usTrack2Len = (usTk2Len - 3);// REMOVE %, ? AND LRC
    
    memcpy(srTransRec.szTrack3Data, baTk3Buf, usTk3Len);
    srTransRec.usTrack3Len = usTk3Len;

    vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);

    //Fix for card without Track 1
    //vdGetCardHolderName(baTk1Buf, usTk1Len);
    if (usTk1Len > 0)
       vdAnalysisTrack1( &baTk1Buf[1], usTk1Len); 
    if (usTk2Len > 0)
    //Fix for card without Track 2

    vdAnalysisTrack2( &baTk2Buf[1], usTk2Len);
}

void vdCTOS_ResetMagstripCardData(void)
{
    memset(srTransRec.szTrack1Data, 0x00, sizeof(srTransRec.szTrack1Data));
    srTransRec.usTrack1Len=0;

    memset(srTransRec.szTrack2Data, 0x00, sizeof(srTransRec.szTrack2Data));
    srTransRec.usTrack2Len=0;

    memset(srTransRec.szTrack3Data, 0x00, sizeof(srTransRec.szTrack3Data));
    srTransRec.usTrack3Len=0;

    memset(srTransRec.szCardholderName, 0x00, sizeof(srTransRec.szCardholderName));

    memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
    srTransRec.byPanLen = 0;
    memset(srTransRec.szExpireDate, 0x00, sizeof(srTransRec.szExpireDate));
    memset(srTransRec.szServiceCode, 0x00, sizeof(srTransRec.szServiceCode));

    srTransRec.byEntryMode = 0;
}

int inCTOS_CheckEMVFallbackTimeAllow(char* szStartTime, char* szEndTime, int inAllowTime)
{
    char szTempBuf[20];
    int inHH1, inHH2, inMM1, inMM2, inSS1, inSS2, inGap;

    if ((strlen(szStartTime) == 0) || (strlen(szStartTime) == 0)) 
        return (d_OK);

    if(0 == atoi(szStartTime))
        return (d_OK);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[0], 2);
    inHH1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[2], 2);
    inMM1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[4], 2);
    inSS1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[0], 2);
    inHH2 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[2], 2);
    inMM2 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[4], 2);
    inSS2 = atoi(szTempBuf);

    inGap = ((inHH2*3600)+(inMM2*60)+inSS2) - ((inHH1*3600)+(inMM1*60)+inSS1);

    if (inGap < 0 )
        return (d_OK);

    if(inGap > inAllowTime)
        return (d_NO);

    return d_OK;;

}


int inCTOS_CheckEMVFallback(void)
{
    BYTE szFallbackStartTime[20];
    BYTE szCurrentTime[20];
    int inRet;
    CTOS_RTC SetRTC;
    
    if (strCDT.fChkServiceCode)
    {
        if(((srTransRec.szServiceCode[0] == '2') || (srTransRec.szServiceCode[0] == '6'))
            && (CARD_ENTRY_ICC != srTransRec.byEntryMode))
        {
            if(inFallbackToMSR == SUCCESS)
            {
                CTOS_RTCGet(&SetRTC);
                sprintf(szCurrentTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
                inRet = inCTOS_CheckEMVFallbackTimeAllow(strTCT.szFallbackTime, szCurrentTime, strTCT.inFallbackTimeGap);

                inFallbackToMSR = FAIL;
                memset(strTCT.szFallbackTime,0x00,sizeof(strTCT.szFallbackTime));
                
                if(d_OK != inRet)
                    return FAIL;
                else
                    vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
            }
            else
                return FAIL;
        }
    }

    return d_OK;;
}

int inCTOS_CheckIssuerEnable(void)
{
    int inEnable = 0;

    inEnable = strIIT.inCheckHost;
    vdDebug_LogPrintf("inCTOS_CheckIssuerEnable: [%d]", inEnable);

    if(0 == inEnable)
    {
       vdSetErrorMessage("TRANS NOT ALLWD");
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}


int inCTOS_CheckTranAllowd(void)
{
    int inEnable = 0;

	vdDebug_LogPrintf("--inCTOS_CheckTranAllowd--");
	vdDebug_LogPrintf("byTransType=[%d], byEntryMode=[%d]", srTransRec.byTransType, srTransRec.byEntryMode);
	
	if (srTransRec.byTransType == CASH_OUT && srTransRec.byEntryMode == CARD_ENTRY_FALLBACK)
	{
	   vdSetErrorMessage("TRANS NOT ALLWD");
       return(ST_ERROR);
	}
	
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    inEnable = strPIT.fTxnEnable;
    vdDebug_LogPrintf("inCTOS_CheckTranAllowd: [%d]", inEnable);
    
    if(0 == inEnable)
    {
       vdSetErrorMessage("TRANS NOT ALLWD");
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

int inCTOS_CheckMustSettle(void)
{
    int inEnable = 0;

    inEnable = strMMT[0].fMustSettFlag;
    
    if(1 == inEnable)
    {
       //vdSetErrorMessage("MUST SETTLE");
       vdDisplayMessageBox(1, 8, "", "MUST SETTLE", "", MSG_TYPE_WARNING);
       CTOS_Beep();
       CTOS_Delay(1500);
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

void vdCTOS_FormatPAN(char *szFmt,char* szInPAN,char* szOutPAN, BOOL fMask)
{
    char szCurrentPAN[20+1], szCurrentPANFormat[24+1];
    int inFmtIdx = 0, inIndex=0;
    int inPANIdx = 0;
    int inFmtPANSize;
    int inPanSize;

	memset(szCurrentPANFormat, 0x00, sizeof(szCurrentPANFormat));
	
    inPanSize = strlen(srTransRec.szPAN);
	switch(inPanSize)
	{
		case 12:
            if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** ****");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN");
			break;
		case 13:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** *");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN N");
			break;
		case 14:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** **");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NN");
			break;
		case 15:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** ***");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NNN");
			break;
        case 16:
            if(fMask == TRUE)
                strcpy(szCurrentPANFormat,"**** **** **** ****");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NNNN");
            break;
			
		case 17:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** **** *");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NNNN N");
			break;
		case 18:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"**** **** **** **** **");
			else
				strcpy(szCurrentPANFormat,"NNNN NNNN NNNN NNNN NN");
			break;
		case 19:
			if(fMask == TRUE)
			    strcpy(szCurrentPANFormat,"*******************");
			else
				strcpy(szCurrentPANFormat,"NNNNNNNNNNNNNNNNNNN");
			break;
	}

    if(fMask == TRUE)
    {
        inFmtPANSize = strlen(szCurrentPANFormat); 
        inFmtPANSize--;
        vdDebug_LogPrintf("1. inFmtPANSize:[%d]", inFmtPANSize);
        for(inFmtIdx=inFmtPANSize; strIIT.inMaskDigit > inIndex; inFmtIdx--)
        {
            if(szCurrentPANFormat[inFmtIdx] == '*')
            {
                szCurrentPANFormat[inFmtIdx]='N';
                inIndex++;
            }
        }
    }
	
    inFmtPANSize = strlen(szCurrentPANFormat);
    if (strlen(szCurrentPANFormat) == 0) 
    {
      strncpy(szOutPAN,szInPAN,inFmtPANSize);
      return;
    }

    memset(szCurrentPAN, 0x00, sizeof(szCurrentPAN));
    memcpy(szCurrentPAN,szInPAN,strlen(szInPAN));

    inFmtIdx = 0;
	
    while(szCurrentPANFormat[inFmtIdx]!= 0x00)
    {
      if(szCurrentPANFormat[inFmtIdx] == 'N' || szCurrentPANFormat[inFmtIdx] == 'n')
      {
          szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; 
          inFmtIdx++;
          inPANIdx++;
      }
      else if (szCurrentPANFormat[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' ||szCurrentPANFormat[inFmtIdx] == '*')   
      {
                     
          memcpy(&szOutPAN[inFmtIdx],&szCurrentPANFormat[inFmtIdx],1);
          inFmtIdx++;
          inPANIdx++;
      }
      else if (!isdigit(szCurrentPANFormat[inFmtIdx]))
      {
          szOutPAN[inFmtIdx] = szCurrentPANFormat[inFmtIdx];
          inFmtIdx++;
      }
    }

    while(szCurrentPAN[inPANIdx]!= 0x00)
    {
     szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; 
     inFmtIdx++;
     inPANIdx++;
    }

    return;
}
//aaronnino BANCNET bancnet fix on issue #0004 Inccorrect PAN Masking 2 of 4 start 
void vdCTOS_FormatPAN2(char *szFmt,char* szInPAN,char* szOutPAN)
{
    char szCurrentPAN[20];
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int inFmtPANSize;
	int inPanSize;
    
    inFmtPANSize = strlen(szFmt);
    if (strlen(szFmt) == 0) 
    {
      strncpy(szOutPAN,szInPAN,inFmtPANSize);
      return;
    }

	inPanSize = strlen(srTransRec.szPAN);

	switch(inPanSize)
	{
		case 12:
			szFmt = "NNNN NNNN NNNN";
			break;
		case 13:
			szFmt = "NNNN NNNN NNNN N";
			break;
		case 14:
			szFmt = "NNNN NNNN NNNN NN";
			break;
		case 15:
			szFmt = "NNNN NNNN NNNN NNN";
			break;
		case 17:
			szFmt = "NNNN NNNN NNNN NNNN N";
			break;
		case 18:
			szFmt = "NNNN NNNN NNNN NNNN NN";
			break;
		case 19:
			szFmt = "NNNNNNNNNNNNNNNNNNN";
			break;
	}


    memset(szCurrentPAN, 0x00, sizeof(szCurrentPAN));
    memcpy(szCurrentPAN,szInPAN,strlen(szInPAN));

    while(szFmt[inFmtIdx]!= 0x00)
    {
      if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
      {
          szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; 
          inFmtIdx++;
          inPANIdx++;
      }
      else if (szFmt[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' ||szFmt[inFmtIdx] == '*')   
      {
                     
          memcpy(&szOutPAN[inFmtIdx],&szFmt[inFmtIdx],1);
          inFmtIdx++;
          inPANIdx++;
      }
      else if (!isdigit(szFmt[inFmtIdx]))
      {
          szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
          inFmtIdx++;
      }
    }

    while(szCurrentPAN[inPANIdx]!= 0x00)
    {
     szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; 
     inFmtIdx++;
     inPANIdx++;
    }

    return;
}
//aaronnino BANCNET bancnet fix on issue #0004 Incorrect PAN Masking 2 of 4 end


void vdCTOS_FormatAmount(char *szFmt,char* szInAmt,char* szOutAmt)
{
    char szCurrentAmt[20];
    char szBuf[20];
    char szFinalFmt[20];
    int inFmtIdx = 0;
    int inTotaActNum = 0;
    int inPadNum = 0;
    int inignore = 0;
    int inAmtIdx = 0;
    int inFmtAmtSize;

	vdDebug_LogPrintf("1.vdCTOS_FormatAmount");
		
    inFmtAmtSize = strlen(szFmt);
    if (strlen(szFmt) == 0) 
    {
      strncpy(szOutAmt,szInAmt,inFmtAmtSize);
      return;
    }

    vdDebug_LogPrintf("2.vdCTOS_FormatAmount");
    inFmtIdx = 0;
    while(szFmt[inFmtIdx]!= 0x00)
    {
        if(szFmt[inFmtIdx] == 'n')
            inPadNum ++;

        if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
            inTotaActNum ++;

        inFmtIdx ++;
    }
	
    vdDebug_LogPrintf("3.vdCTOS_FormatAmount szFmt:%s", szFmt);
    vdDebug_LogPrintf("3.vdCTOS_FormatAmount inTotaActNum:%d", inTotaActNum);
    inFmtIdx = 0;
    while(0x30 == szInAmt[inFmtIdx])
    {
        inFmtIdx ++;
    }

    memset(szCurrentAmt, 0x00, sizeof(szCurrentAmt));
    strcpy(szCurrentAmt,&szInAmt[inFmtIdx]);

    if(inPadNum > strlen(szCurrentAmt))
    {
        memset(szBuf, 0x00, sizeof(szBuf));
        memset(szBuf, 0x30, inPadNum-strlen(szCurrentAmt));
        strcat(szBuf, szCurrentAmt);

        strcpy(szCurrentAmt, szBuf);
        strcpy(szFinalFmt, szFmt);
    }

    vdDebug_LogPrintf("4.vdCTOS_FormatAmount szCurrentAmt:%s", szCurrentAmt);
	vdDebug_LogPrintf("4.vdCTOS_FormatAmount szFinalFmt:%s", szFinalFmt);
	
    if(inTotaActNum > strlen(szCurrentAmt))
    {
        inFmtIdx = 0;
        while(szFmt[inFmtIdx]!= 0x00)
        {
            if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
                inignore ++;

            inFmtIdx ++;
            
            if((inignore >= (inTotaActNum - strlen(szCurrentAmt))) && (szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n'))
                break;
        }

        strcpy(szFinalFmt, &szFmt[inFmtIdx]);
    }

    vdDebug_LogPrintf("5.vdCTOS_FormatAmount szFinalFmt:%s", szFinalFmt);
	
    inFmtIdx = 0;
    inAmtIdx = 0;
    while(szFinalFmt[inFmtIdx]!= 0x00)
    {
      if(szFinalFmt[inFmtIdx] == 'N' || szFinalFmt[inFmtIdx] == 'n')
      {
          szOutAmt[inFmtIdx] = szCurrentAmt[inAmtIdx]; 
          inFmtIdx++;
          inAmtIdx++;
      }
      else if ((szFinalFmt[inFmtIdx]) == ' ' || (szFinalFmt[inFmtIdx]) == ',' || (szFinalFmt[inFmtIdx]) == '.')
      {
          szOutAmt[inFmtIdx] = szFinalFmt[inFmtIdx];
          inFmtIdx++;
      }
      else
      {
        inFmtIdx ++ ;
      }
    }

    vdDebug_LogPrintf("6.vdCTOS_FormatAmount szOutAmt:%s", szOutAmt);
    szOutAmt[inFmtIdx] = 0x00;

    return;
}


int inGetIssuerRecord(int inIssuerNumber) 
{
    int inRec = 1;
    do 
    {
        if (inIITRead(inRec) != d_OK) 
        {
            vdDebug_LogPrintf("saturn IIT read error");
            return(d_NO);
        }
		vdDebug_LogPrintf("saturn IIT read");
        inRec++;
    } while (inIssuerNumber != strIIT.inIssuerNumber);

    return(d_OK);
}


int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine, BOOL fDisplayLogo)
{
    char szStr[50 + 1]; 
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE szTemp1[30+1];

	vdDebug_LogPrintf("--inCTOS_DisplayCardTitle--");
	
    memset(szStr,0x00,sizeof(szStr));
    memset(EMVtagVal,0x00,sizeof(EMVtagVal));
  
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        
        if(EMVtagLen > 0)
        {
            sprintf(szStr, "%s", EMVtagVal);
            vdDebug_LogPrintf("Card label(str): %s",szStr);       
        }
    }

    if(0 == strlen(szStr))
    {
        sprintf(szStr, "%s", strCDT.szCardLabel);
        vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
    }

	vdDebug_LogPrintf("inCTOS_DisplayCardTitle|Length[%d]|srTransRec.szPAN[%s]|fDisplayLogo[%d]", strlen(srTransRec.szPAN), srTransRec.szPAN, fDisplayLogo);	

	if (strlen(srTransRec.szPAN) > 0)
	{
		memset(szTemp1,0,sizeof(szTemp1));
	    vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1, strIIT.fMaskPan);

	    if(fDisplayLogo == TRUE)
	    {
		    vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
		    displayAppbmpDataEx(140, 35, strIIT.szIssuerLogo, TRUE);
	    }
	    if(0>= usCardTypeLine || 0 >= usPANLine)
	    {
	        usCardTypeLine = 3;
	        usPANLine = 4;
	    }
		
	    setLCDPrint(usCardTypeLine, DISPLAY_POSITION_LEFT , szStr);
		setLCDPrint(usPANLine, DISPLAY_POSITION_LEFT , szTemp1);
	}
   
	vdDebug_LogPrintf("strIIT.szIssuerLogo[%s]", strIIT.szIssuerLogo);
	vdDebug_LogPrintf("fDisplayLogo[%d]", fDisplayLogo);
	vdDebug_LogPrintf("usCardTypeLine[%d]| szStr[%s]", usCardTypeLine, szStr);
	vdDebug_LogPrintf("usPANLine[%d]| szTemp1[%s]", usPANLine, szTemp1);
	
    return(d_OK);
}

short inCTOS_LoadCDTIndex(void)
{
	signed int inRetVal=0;
	short   shStatus;
	int     i=0, selectedRDTIndex;
	BYTE    shSuitableRDTIndex[10];
	int inIssuer = 0;
	int inNumberOfMatches = 0;
	int inRecNumArray[4];
	int    inCardLen, j;
	int inMaxCDTid = 0;
	int inFindRecordNum=0;
	char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, key=0; 
    char szCardLabel[3][12+1];
	int inCardLabelIndex=0;
	
	//CTOS_LCDTClearDisplay();
	if (2 == strTCT.byTerminalType)
		CTOS_LCDTPrintXY(1, V3_ERROR_LINE_ROW, "Checking Card... ");
	else
		CTOS_LCDTPrintXY(1, 8, "Checking Card... ");
  
	inCDTReadMulti(srTransRec.szPAN, &inFindRecordNum);
	if(inFindRecordNum == 0)
	{
		vdDebug_LogPrintf("Not find in CDT");
		vdSetErrorMessage("CARD NOT SUPPORTED");
		return INVALID_CARD;
	}

	memset(szCardLabel, 0x00, sizeof(szCardLabel));
	
    for(j=0;j<inFindRecordNum;j++)
    {
        //if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) && !(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)) 
        if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) /*&& !(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)*/)
        { 
            if ((strMCDT[j].inType == DEBIT_CARD) || (strMCDT[j].inType == EBT_CARD) || (strMCDT[j].IITid != inIssuer)) 
            {
    
                if((srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE) && strMCDT[j].inType == DEBIT_CARD)
                    continue;			
                
                if (strMCDT[j].inType != DEBIT_CARD && strMCDT[j].inType != EBT_CARD)
                    inIssuer = strMCDT[j].IITid;

                if(srTransRec.fDebit == TRUE && strMCDT[j].inType != DEBIT_CARD)
                    continue;
                
                if(srTransRec.fCredit == TRUE && strMCDT[j].inType != CREDIT_CARD)
                    continue;					

                if(srTransRec.fInstallment == TRUE && strMCDT[j].inType == DEBIT_CARD)
					continue;

                if(srTransRec.fInstallment == TRUE && strMCDT[j].inInstGroup == 0)
					continue;

                vdDebug_LogPrintf("srTransRec.fLoyalty:(%02x)", srTransRec.fLoyalty);
				vdDebug_LogPrintf("strMCDT[j].inLoyaltyGroup:(%d)", strMCDT[j].inLoyaltyGroup);
				
				if(srTransRec.fLoyalty == TRUE && strMCDT[j].inLoyaltyGroup == 0)
				    continue;
				
                inRecNumArray[inNumberOfMatches++] = strMCDT[j].CDTid;

                //if(inNumberOfMatches > 0)
                //break;
                
                //if (inNumberOfMatches > 1)
                //szChoiceMsg[strlen(szChoiceMsg)] = '~';

                strcat(&szCardLabel[inCardLabelIndex][0],strMCDT[j].szCardLabel); 
                inCardLabelIndex++;                
                
                switch (strMCDT[j].inType)
                {
                    case DEBIT_CARD:
                        break;
                    case EBT_CARD:
                        break;
                    case PURCHASE_CARD:
                        break;
                    default:                    
                    break;
                }
                
                if (inNumberOfMatches > 3)
                    break;
            }
        }
    }

    vdDebug_LogPrintf("inNumberOfMatches:[%d]", inNumberOfMatches);
	vdDebug_LogPrintf("inFindRecordNum:[%d]",inFindRecordNum);

    if(inNumberOfMatches == 1) 
    {
        inRetVal = inRecNumArray[0];
    }
    else if(inNumberOfMatches > 1) 
    {
        char szChoiceMsg[48+1];
		int inLoop;
		
        memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
        for (inLoop = 0; inLoop < 50; inLoop++)
        {
            if (szCardLabel[inLoop][0]!= 0)
            {
                strcat((char *)szChoiceMsg, szCardLabel[inLoop]);
                if (szCardLabel[inLoop+1][0]!= 0)
                    strcat((char *)szChoiceMsg, (char *)" \n"); 
                vdDebug_LogPrintf("inLoop:(%d), szTerm[%s]", inLoop, szCardLabel[inLoop]);
            }
            else
                break;
        }
        		
		strcpy(szHeaderString, "SELECT CARD");
		key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
		if (key > 0)
		{
			if (key == d_KBD_CANCEL)
			{
				return -1;
			}

			inRetVal = inRecNumArray[key-1];
			
			vdDebug_LogPrintf("inRetVal:[%d]",inRetVal);
			
            CTOS_LCDTClearDisplay();
            vdDispTransTitle(srTransRec.byTransType);
		}
    }

    if (inRetVal > 0) 
    {
        inCDTRead(inRetVal);

		if(strCDT.fManEntry == FALSE && srTransRec.byEntryMode == CARD_ENTRY_MANUAL) 
		{
			vdSetErrorMessage("NO MANUAL ENTRY");
			return INVALID_CARD;
		}
		
        srTransRec.CDTid = inRetVal;// save the current CDT that is loaded

        /* Get the CDT also for card labels */
        inGetIssuerRecord(strCDT.IITid);
    }
    else
    {
        vdDebug_LogPrintf("Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    /* Check for proper card length */
    inCardLen = strlen(srTransRec.szPAN);

	vdDebug_LogPrintf("--LoadCDTIndex--");	
	vdDebug_LogPrintf("strCDT.IITid[%d]", strCDT.IITid);
	vdDebug_LogPrintf("strCDT.HDTid[%d]", strCDT.HDTid);
    vdDebug_LogPrintf("inCardLen: (%d)", inCardLen);
    vdDebug_LogPrintf("strCDT.inMinPANDigit: (%d)", strCDT.inMinPANDigit);
    vdDebug_LogPrintf("strCDT.inMaxPANDigit: (%d)", strCDT.inMaxPANDigit);
    vdDebug_LogPrintf("srTransRec.CDTid: (%d)",srTransRec.CDTid);	
	vdDebug_LogPrintf("strCDT.szPANLo[%s]", strCDT.szPANLo);
	vdDebug_LogPrintf("strCDT.szPANHi[%s]", strCDT.szPANHi);
	vdDebug_LogPrintf("strCDT.szCardLabel[%s]", strCDT.szCardLabel);
	vdDebug_LogPrintf("strCDT.inType[%d]", strCDT.inType);
	vdDebug_LogPrintf("--LoadCDTIndex--");
	
    if ((inCardLen < strCDT.inMinPANDigit) ||
        (inCardLen > strCDT.inMaxPANDigit))
    {
        vdDebug_LogPrintf("BAD CARD LEN");
        vdSetErrorMessage("BAD CARD LEN");
        return INVALID_CARD;
    }

    if (strCDT.fluhnCheck == VS_TRUE)   /* Check Luhn */
    {
        if (chk_luhn(srTransRec.szPAN))
        {
            vdDisplayErrorMsg(1, 8, "INVALID LUHN");
            return INVALID_CARD;
        }
    }


    if (strTCT.fDebitFlag != DEBIT && strCDT.inType == DEBIT_CARD )
    {
        vdDisplayErrorMsg(1, 8, "INVALID CARD");
        return INVALID_CARD;
    }

    if(strCDT.fExpDtReqd)
    {
        if(shChk_ExpireDate() != d_OK)
        {
            vdDisplayErrorMsg(1, 8, "CARD EXPIRED");
            return CARD_EXPIRED;
        }
    }
    
    //for save Accum file
    srTransRec.IITid= strCDT.IITid;
    srTransRec.HDTid = strCDT.HDTid;
    srTransRec.inCardType = strCDT.inType;

	CTOS_LCDTClearDisplay();
//    CTOS_LCDTPrintXY(1, 8, "                   ");  
    return d_OK;;
}
/*
short inCTOS_LoadCDTIndex(void)
{
	signed int inRetVal;
	short   shStatus;
	int     inIndex = 1,i=0, selectedRDTIndex;
	BYTE    shSuitableRDTIndex[10];
	int inIssuer = 0;
	int inNumberOfMatches = 0;
	int inRecNumArray[4];
	char szChoiceMsg[20 + 1];
	int    inCardLen;
	int inMaxCDTid = 0;

    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
  
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 8, "Checking Card... ");

    inMaxCDTid = inCDTMAX();
	if (inMaxCDTid <= 0)
		inMaxCDTid = 99;

    vdDebug_LogPrintf("inCDTMAX [%ld]", inMaxCDTid);
    
    do
    {
        if (inMaxCDTid <inIndex)
        {
            vdDebug_LogPrintf("inCDTMAX [%ld] inIndex[%d]", inMaxCDTid, inIndex);
        	vdSetErrorMessage("CARD NOT SUPPORTED");
            return INVALID_CARD;
        }

        shStatus = inCDTRead(inIndex);
        vdDebug_LogPrintf("--EMV load cdt--[%d]-shStatus[%d]srTransRec.szPAN[%s]-",inIndex ,shStatus,srTransRec.szPAN);

        if(shStatus != d_OK)
        {   
            inIndex++;        
            continue;           
        }

        if (( memcmp ( strCDT.szPANLo , srTransRec.szPAN , strlen(strCDT.szPANLo) ) <= 0 ) &&
            ( memcmp ( strCDT.szPANHi , srTransRec.szPAN , strlen(strCDT.szPANHi) ) >= 0 ) )
        {

          if (!(strTCT.fDebitFlag == VS_FALSE && strCDT.inType == DEBIT_CARD) && !(!strCDT.fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)) 
            { 
                if ((strCDT.inType == DEBIT_CARD) || (strCDT.inType == EBT_CARD) || (strCDT.IITid != inIssuer)) 
                {
                    if (strCDT.inType != DEBIT_CARD && strCDT.inType != EBT_CARD)
                        inIssuer = strCDT.IITid;

                    inRecNumArray[inNumberOfMatches++] = inIndex;
                    if(inNumberOfMatches > 0)
                        break;

                    if (inNumberOfMatches > 1)
                        szChoiceMsg[strlen(szChoiceMsg)] = '~';

                    switch (strCDT.inType) {
                    case DEBIT_CARD:
                        break;
                    case EBT_CARD:
                        break;
                    case PURCHASE_CARD:
                        break;
                    default:                    
                        break;
                    }

                    if (inNumberOfMatches > 3)
                        break;
                }
            }
        }
        inIndex++;
       
     }while(1);

    if (inNumberOfMatches == 1) 
    {
        inRetVal = inRecNumArray[0];
    }
    else if (inNumberOfMatches > 1) 
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
    }

    if (inRetVal >= 0) 
    {
        inCDTRead(inRetVal);
        srTransRec.CDTid = inRetVal;// save the current CDT that is loaded

        // Get the CDT also for card labels
        inGetIssuerRecord(strCDT.IITid);
    }
    else
    {
        vdDebug_LogPrintf("Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    // Check for proper card length 
    inCardLen = strlen(srTransRec.szPAN);

    if ((inCardLen < strCDT.inMinPANDigit) ||
        (inCardLen > strCDT.inMaxPANDigit))
    {
        vdDebug_LogPrintf("BAD CARD LEN");
        vdSetErrorMessage("BAD CARD LEN");
        return INVALID_CARD;
    }

    if (strCDT.fluhnCheck == VS_TRUE)   // Check Luhn
    {
        if (chk_luhn(srTransRec.szPAN))
        {
            vdDisplayErrorMsg(1, 8, "INVALID LUHN");
            return INVALID_CARD;
        }
    }


    if (strTCT.fDebitFlag != DEBIT && strCDT.inType == DEBIT_CARD )
    {
        vdDisplayErrorMsg(1, 8, "INVALID CARD");
        return INVALID_CARD;
    }

    if(strCDT.fExpDtReqd)
    {
        if(shChk_ExpireDate() != d_OK)
        {
            vdDisplayErrorMsg(1, 8, "CARD EXPIRED");
            return CARD_EXPIRED;
        }
    }
    
    //for save Accum file
    srTransRec.IITid= strCDT.IITid;
    srTransRec.HDTid = strCDT.HDTid;
    srTransRec.inCardType = strCDT.inType;

	CTOS_LCDTClearDisplay();
//    CTOS_LCDTPrintXY(1, 8, "                   ");  
    return d_OK;;
}
*/
int inCTOS_EMVCardReadProcess (void)
{
    short shResult = 0;
    USHORT usMsgFailedResult = 0;
    
    vdDebug_LogPrintf("-------shCT0S_EMVInitialize1---[%d]--",shResult); 
    shResult = shCTOS_EMVAppSelectedProcess();
    
    vdDebug_LogPrintf("-------shCT0S_EMVInitialize---[%d]--",shResult); 

    if(shResult == EMV_CHIP_FAILED)
    {
        usMsgFailedResult = MSG_TRANS_ERROR;
        return usMsgFailedResult;
    }
    else if(shResult == EMV_USER_ABORT)
    {
        usMsgFailedResult = MSG_USER_CANCEL;
        return usMsgFailedResult;
    }
    shCTOS_EMVGetChipDataReady();

    return (d_OK);
}


int inCTOS_ManualEntryProcess (BYTE *szPAN)
{
    USHORT  usMaxLen = 19;
    BYTE    szTempBuf[10];
    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "CARD NO: ");
    
    if(getCardNO(szPAN) != d_OK)
    {
        return USER_ABORT;
    }

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(4, DISPLAY_POSITION_LEFT, "CARD NO: ");
    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
    memset(bDisplayStr, 0x20, usMaxLen*2);
    strcpy(&bDisplayStr[(usMaxLen-strlen(szPAN))*2], szPAN);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-usMaxLen*2, 5, bDisplayStr);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");
    
    memset(szTempBuf,0,sizeof(szTempBuf));
    if(getExpDate(szTempBuf) != d_OK)
    {
        return USER_ABORT;
    }
    wub_str_2_hex(szTempBuf, &srTransRec.szExpireDate[1], 2);
    wub_str_2_hex(&szTempBuf[2], srTransRec.szExpireDate, 2);
    CTOS_KBDBufFlush ();

    vdCTOS_SetTransEntryMode(CARD_ENTRY_MANUAL);
    return d_OK;;
}

int inCTOS_GetCardFields(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;

 DebugAddSTR("inCTOS_GetCardFields","Processing...",20);
    
     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
 
 SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
     
        inCTOS_DisplayIdleBMP();
		displayAppbmpDataEx(1, 30, "INSERT.bmp", FALSE);
    }
// patrick ECR 20140516 start
    if (strTCT.fECR) // tct
    {
    	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    	{
    		char szDisplayBuf[30];	
    		BYTE szTemp1[30+1];

    		CTOS_LCDTPrintXY(1, 7, "AMOUNT:");		 
    		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    		sprintf(szDisplayBuf, "%s %10lu.%02lu", strCST.szCurSymbol,atol(szTemp1)/100, atol(szTemp1)%100);		
    		CTOS_LCDTPrintXY(1, 8, szDisplayBuf);		
    	}
    }
// patrick ECR 20140516 end
    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return READ_CARD_TIMEOUT ;

        CTOS_KBDInKey(&byKeyBuf);
    
        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
            BYTE bKey = 0x00;
			
            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
            if(d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();

            /*albert - start - only accept 0-9, exit on cancel key*/ 
            CTOS_KBDHit(&bKey);
            if((bKey >= d_KBD_1) && (bKey <= d_KBD_9))
                srTransRec.szPAN[0]=bKey;
            else if(bKey == d_KBD_CANCEL)
            {
				vdSetErrorMessage("USER CANCEL");
                return USER_ABORT;				
            }
            else
                continue;
            /*albert - end - only accept 0-9, exit on cancel key*/ 
			
            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush ();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }
	
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
            
            vdDebug_LogPrintf("--EMV flow----" );
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                }
                else
                {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }

             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD"); 
                
                goto SWIPE_AGAIN;

             }
                     
             break;
         
         }
        
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
		//Fix for Track2 Len < 35
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
		//Fix for Track2 Len < 35
        {
            vdCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }
            
            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD"); 
                
                goto SWIPE_AGAIN;

             }
                
            break;
        }

       }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;
    
    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}

int inCTOS_GetCardFieldsNoEMV(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;

 DebugAddSTR("inCTOS_GetCardFields","Processing...",20);
    
     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
 
 SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
        
        inCTOS_DisplayIdleBMP();
        if(srTransRec.fDebit == TRUE)
            displayAppbmpDataEx(1, 30, "SWIPE_ONLY.bmp", FALSE);		
        else			
            displayAppbmpDataEx(1, 30, "SWIPE.bmp", FALSE);
    }
// patrick ECR 20140516 start
    if (strTCT.fECR) // tct
    {
    	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    	{
    		char szDisplayBuf[30];	
    		BYTE szTemp1[30+1];

    		CTOS_LCDTPrintXY(1, 7, "AMOUNT:");		 
    		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    		sprintf(szDisplayBuf, "%s %10lu.%02lu", strCST.szCurSymbol,atol(szTemp1)/100, atol(szTemp1)%100);		
    		CTOS_LCDTPrintXY(1, 8, szDisplayBuf);		
    	}
    }
// patrick ECR 20140516 end
    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return READ_CARD_TIMEOUT ;

        CTOS_KBDInKey(&byKeyBuf);
    
        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
            BYTE bKey = 0x00;
			
            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
            if(d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();

            /*albert - start - only accept 0-9, exit on cancel key*/ 
            CTOS_KBDHit(&bKey);
			if(srTransRec.fDebit == TRUE)
			{
                if(bKey == d_KBD_CANCEL)
                {
					vdSetErrorMessage("USER CANCEL");
                    return USER_ABORT;				
                }
               // else
                //    continue;
			}
			else
			{
                if((bKey >= d_KBD_1) && (bKey <= d_KBD_9))
                    srTransRec.szPAN[0]=bKey;
                else if(bKey == d_KBD_CANCEL)
                {
					vdSetErrorMessage("USER CANCEL");
                    return USER_ABORT;				
                }
                //else
                 //   continue;
			}
            /*albert - end - only accept 0-9, exit on cancel key*/ 

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush ();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }

        #if 0 /*do not process installment with EMV*/
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
            
            vdDebug_LogPrintf("--EMV flow----" );
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                }
                else
                {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }
        #endif
		
        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }
             #if 0
             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD"); 
                
                goto SWIPE_AGAIN;

             }
             #endif        
             break;
         
         }
        
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
		//Fix for Track2 Len < 35
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
		//Fix for Track2 Len < 35
        {
            vdCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }
            #if 0
            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD"); 
                
                goto SWIPE_AGAIN;

             }
            #endif                  
            break;
        }

       }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;
    
    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}

int inCTOS_WaveGetCardFields(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
	ULONG ulAPRtn;
	BYTE temp[64];
	char szTotalAmount[AMT_ASC_SIZE+1];
	EMVCL_RC_DATA_EX stRCDataEx;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[3+1],szCurrCode[3+1];

 	vdDebug_LogPrintf("inCTOS_WaveGetCardFields.........");
    
     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
 
 SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
     
        inCTOS_DisplayIdleBMP();
    }

	memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
	memset(szOtherAmt,0x00,sizeof(szOtherAmt));
	memset(szTransType,0x00,sizeof(szTransType));
	memset(szCatgCode,0x00,sizeof(szCatgCode));
	memset(szCurrCode,0x00,sizeof(szCurrCode));
	memset(temp,0x00,sizeof(temp));
	memset(szTotalAmount,0x00,sizeof(szTotalAmount));
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
	CTOS_LCDTPrintXY(1, 4, temp);

	if (srTransRec.byTransType == REFUND)
		szTransType[0] = 0x20;

	sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
	strcpy(szCurrCode, szCatgCode);

	ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
//	ulAPRtn = EMVCL_InitTransaction(atol(szTotalAmount));
	if(ulAPRtn != d_EMVCL_NO_ERROR)
	{
		vdSetErrorMessage("CTLS InitTrans Fail!");
		return d_NO;
	}
    
    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
        	inCTOSS_CLMCancelTransaction();
            return READ_CARD_TIMEOUT ;
        }

        CTOS_KBDInKey(&byKeyBuf);
    
        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
        	inCTOSS_CLMCancelTransaction();
            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
            if(d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();
            
            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush ();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
        	inCTOSS_CLMCancelTransaction();
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
            
            vdDebug_LogPrintf("--EMV flow----" );
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                }
                else
                {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
         	inCTOSS_CLMCancelTransaction();
             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }

             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD"); 
                
                goto SWIPE_AGAIN;

             }
                     
             break;
         
         }
        
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        if((byMSR_status == d_OK ) && (usTk2Len > 35))
        {
        	inCTOSS_CLMCancelTransaction();
            vdCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }
            
            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD"); 
                
                goto SWIPE_AGAIN;

             }
                
            break;
        }

		ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);
		
		if(ulAPRtn == d_EMVCL_RC_DEK_SIGNAL)
		{
			vdDebug_LogPrintf("DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen,stRCDataEx.baChipData);
		}
		else if(ulAPRtn == d_EMVCL_TX_CANCEL)
		{
			vdDisplayErrorMsg(1, 8, "USER CANCEL");
			return USER_ABORT;
		}
		else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
		{
			inCTOSS_CLMCancelTransaction();
			vdDisplayErrorMsg(1, 8, "TIMEOUT");
			return USER_ABORT;
		}
		else if(ulAPRtn != d_EMVCL_PENDING)
		{
			vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
			break;
		}

	}

	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
	{
		if(ulAPRtn != d_EMVCL_RC_DATA)
		{
			vdCTOSS_WaveCheckRtCode(ulAPRtn);
			inCTOSS_CLMCancelTransaction();
			return d_NO;
		}

		if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
		{
			inCTOSS_CLMCancelTransaction();
			return d_NO;
		}

		//Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
        	inCTOSS_CLMCancelTransaction();
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		inCTOSS_CLMCancelTransaction();
	}

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;
    
    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}


int inCTOS_PreConnect(void)
{
    int inRetVal, inReturn;
	BYTE szTitle[20+1];
	BYTE szDisMsg[40];
	BYTE szErrorMsg[40];


	vdDebug_LogPrintf("saturn --inCTOS_PreConnect--");

    if (strTCT.fDemo==VS_TRUE)
		return (d_OK);
	
    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
    
    vdDebug_LogPrintf("strTCT.byTerminalType[%d]",strTCT.byTerminalType);
	vdDebug_LogPrintf("ErmTrans_Approved[%d]",ErmTrans_Approved);
	vdDebug_LogPrintf("srTransRec.usTerminalCommunicationMode[%d]", srTransRec.usTerminalCommunicationMode);    
    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);

	vdDebug_LogPrintf("fDHCPEnable[%d]",strTCP.fDHCPEnable);
	vdDebug_LogPrintf("fShareComEnable[%d]",strTCT.fShareComEnable);
	vdDebug_LogPrintf("Primary[%s]port[%ld]",strCPT.szPriTxnHostIP,strCPT.inPriTxnHostPortNum);
	vdDebug_LogPrintf("Secondary[%s]port[%ld]",strCPT.szSecTxnHostIP,strCPT.inSecTxnHostPortNum);
//strCPT.inSecTxnHostPortNum = 5010;//for RH testing
	vdDebug_LogPrintf("strTCT.fTLESec[%d], strHDT.fHostTLESec[%d]",strTCT.fTLESec, strHDT.fHostTLESec);
	vdDebug_LogPrintf("strCPT.inIPHeader[%d]",strCPT.inIPHeader);

	// Minimize the white screend display -- sidumili
	if ((isCheckTerminalMP200() == d_OK) && (ErmTrans_Approved)) {
		//vdCTOS_DispStatusMessage("PROCESSING...");
		vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	}
	
	if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
    {
        vdSetErrorMessage("COMM INIT ERR");
        return(d_NO);
    }

	vdDebug_LogPrintf("saturn --inCTOS_PreConnect2--");
	
    inReturn = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode); 

	vdDebug_LogPrintf("saturn inCTOS_CheckInitComm | inReturn[%d]|fGPRSConnectOK[%d]", inReturn, fGPRSConnectOK);

#if 1	
	if (inReturn != d_OK || !fGPRSConnectOK)
	{
		switch (srTransRec.usTerminalCommunicationMode )
		{
			case GPRS_MODE:
				//vdDisplayMultiLineMsgAlign("", "GPRS PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdDisplayErrorMsg2(1, 8, "GPRS PROBLEM", "PLESE TRY AGAIN", MSG_TYPE_ERROR);
				break;
			case WIFI_MODE:
				//vdDisplayMultiLineMsgAlign("", "WIFI PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdDisplayErrorMsg2(1, 8, "WIFI PROBLEM", "PLESE TRY AGAIN", MSG_TYPE_ERROR);
				break;
			case ETHERNET_MODE:
				//vdDisplayMultiLineMsgAlign("", "ETHERNET PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdDisplayErrorMsg2(1, 8, "ETHERNET PROBLEM", "PLESE TRY AGAIN", MSG_TYPE_ERROR);
				break;
			default:
				//vdDisplayMultiLineMsgAlign("", "COMM INIT", "ERROR", DISPLAY_POSITION_CENTER);
				vdDisplayErrorMsg2(1, 8, "COMM INIT", "PLESE TRY AGAIN", MSG_TYPE_ERROR);
				break;

			//test
		}

		vdSetErrorMessage("");
		if(srTransRec.byTransType == SETTLE)
			vdDisplayErrorMsg2(1, 8, "SETTLE FAILED", " ", MSG_TYPE_ERROR);		
			
	}
#else
     if (inReturn != d_OK || !fGPRSConnectOK){

		memset(szErrorMsg, 0x00, sizeof(szErrorMsg));
	
		vdDebug_LogPrintf("saturn --inCTOS_PreConnect2 error ******************");
		if(srTransRec.byTransType == SETTLE)
			//vdDisplayMultiLineMsgAlign("", "", "SETTLE FAILED", DISPLAY_POSITION_CENTER);
			strcpy(szErrorMsg, "SETTLE FAILED");
		else{
			
			switch (srTransRec.usTerminalCommunicationMode )
			{
				case GPRS_MODE:
					//vdDisplayMultiLineMsgAlign("", "GPRS PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
					strcpy(szErrorMsg, "GPRS PROBLEM|PLESE TRY AGAIN");
					break;
				case WIFI_MODE:
					//vdDisplayMultiLineMsgAlign("", "WIFI PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
					strcpy(szErrorMsg, "WIFI PROBLEM|PLESE TRY AGAIN");
					break;
				case ETHERNET_MODE:
					//vdDisplayMultiLineMsgAlign("", "ETHERNET PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
					strcpy(szErrorMsg, "ETHERNET PROBLEM|PLESE TRY AGAIN");
					break;	
				
				default:
					strcpy(szErrorMsg, "COMM INIT|ERROR");
					break;
			}
		}

			
		memset(szDisMsg, 0x00, sizeof(szDisMsg));
		memset(szTitle, 0x00, sizeof(szTitle));
			
		szGetTransTitle(srTransRec.byTransType, szTitle);
			
			
        memset(szDisMsg, 0x00, sizeof(szDisMsg));
        strcpy(szDisMsg, szTitle);
    	strcat(szDisMsg, "|");
		strcat(szDisMsg, "");
		strcat(szDisMsg, "|");		
		strcat(szDisMsg, "");
		strcat(szDisMsg, "|");
    	strcat(szDisMsg, szErrorMsg);
    	usCTOSS_LCDDisplay(szDisMsg);           
        CTOS_Beep();
        CTOS_Delay(2000);

		vdSetErrorMessage("");

		return d_NO;

	}

#endif

	vdDebug_LogPrintf("--inCTOS_PreConnect3--");
	
    if (CN_FALSE == srTransRec.byOffline)
    {   
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);        

    }

	vdDebug_LogPrintf("saturn --inCTOS_PreConnect4--");
	
    return(d_OK);
}



int inCTOS_CheckAndSelectMutipleMID(void)
{
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;
    short shFalshMenu = 1;
     BYTE isUP = FALSE, isDOWN = FALSE;
	 BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT MERCHANT";
    char szMitMenu[1024];
    int inLoop = 0;

    unsigned char bstatus = 0; 

    memset(szMitMenu, 0x00, sizeof(szMitMenu));
    vdDebug_LogPrintf("inCTOS_CheckAndSelectMutipleMID=[%d]",strHDT.inHostIndex);
    //get the index , then get all MID from the MMT list and prompt to user to select
    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfRecords);

	CTOS_KBDBufFlush();//cleare key buffer
	//if(srTransRec.fDebit == TRUE && (srTransRec.byTransType == BAL_INQ || srTransRec.byTransType == LOG_ON))
        //inNumOfRecords=1;
		
    if(inNumOfRecords > 1)
	{
	    for (inLoop = 0; inLoop < inNumOfRecords; inLoop++)
	    {
            strcat((char *)szMitMenu, strMMT[inLoop].szMerchantName);
			if(inLoop + 1 != inNumOfRecords)
            	strcat((char *)szMitMenu, (char *)" \n");
	    }
	        
	    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);

		if (key == 0xFF) 
		{
		    CTOS_LCDTClearDisplay();
		    setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
		    vduiWarningSound();
		    return -1;  
		}

    	if(key > 0)
	    {
	        if(d_KBD_CANCEL == key)
	            return -1;
	        
	        vdDebug_LogPrintf("key[%d]-------", key);		
			memcpy(&strMMT[0],&strMMT[key-1],sizeof(STRUCT_MMT));
	    }
	}
	
    srTransRec.MITid = strMMT[0].MITid;
    strcpy(srTransRec.szTID, strMMT[0].szTID);
    strcpy(srTransRec.szMID, strMMT[0].szMID);
    memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

    if(inGetMerchantPassword() != d_OK)
    {
        return d_NO;		
    }
    
    vdDebug_LogPrintf("szATCMD1=[%s] [%s] [%s] [%s] [%s]",strMMT[0].szATCMD1,strMMT[0].szATCMD2,strMMT[0].szATCMD3,strMMT[0].szATCMD4,strMMT[0].szATCMD5);
	vdDebug_LogPrintf("MITid[%d] . szTID[%s] . szMID[%s] . szBatchNo[%s] . szHostLabel[%s]", srTransRec.MITid, srTransRec.szTID, srTransRec.szMID, srTransRec.szBatchNo, srTransRec.szHostLabel);
    
    return SUCCESS;
}



int inCTOS_CheckAndSelectMID(void)
{
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;
    
    unsigned char key;
    unsigned char bstatus = 0; 

    DebugAddSTR("inCTOS_CheckAndSelectMutipleMID","Processing...",20);
    
    //get the index , then get all MID from the MMT list and prompt to user to select
    inMMTReadNumofRecords(srTransRec.HDTid,&inNumOfRecords);
    shCurrentPageNum = 1;
    CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords > 1)
    {
        shTotalPageNum = (inNumOfRecords/ITEMS_PER_PAGE == 0) ? (inNumOfRecords/ITEMS_PER_PAGE) :(inNumOfRecords/ITEMS_PER_PAGE + 1); 
        shLastPageItems = (inNumOfRecords/ITEMS_PER_PAGE == 0) ? (ITEMS_PER_PAGE) : (inNumOfRecords%ITEMS_PER_PAGE);
        
        do
        {
                //display items perpage
                if(shTotalPageNum == 0)//the total item is amaller than ITEMS_PER_PAGE
                {
                    shPageItems = inNumOfRecords;
                }
                else if(shCurrentPageNum == shTotalPageNum)//Last page
                    shPageItems = shLastPageItems;
                else
                    shPageItems = ITEMS_PER_PAGE;
                
                CTOS_LCDTClearDisplay();
                for(shLoop=0 ; shLoop < shPageItems/*ITEMS_PER_PAGE*/ ; shLoop++)
                {
                
                    memset(szDisplay,0,sizeof(szDisplay));
                    sprintf(szDisplay,"%d: %s",shLoop+1,strMMT[shLoop + (shCurrentPageNum -1)*ITEMS_PER_PAGE].szMerchantName);
                    CTOS_LCDTPrintXY(2,shLoop + 2,szDisplay);

                }
                
                key=WaitKey(60);
                
                switch(key)
                {
                    case d_KBD_DOWN:
                        
                        shCurrentPageNum ++;
                        if(shCurrentPageNum > shTotalPageNum)
                            shCurrentPageNum = 1;
                        bstatus = 2;
                        break;
                        
                        
                    case d_KBD_UP:

                        shCurrentPageNum --;
                        if(shCurrentPageNum < 1)
                            shCurrentPageNum = shTotalPageNum;
                        bstatus = 2;        
                        break;          
                        
                    case d_KBD_CANCEL:                          
                        return FAIL;
                    case d_KBD_1:
                        //set the unique MMT num
                                            
                        memcpy(&strMMT[0],&strMMT[0 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                        //vduiDisplayStringCenter(2,strMMT[0].szMID);
                        bstatus = 0;
                        break;
                    case d_KBD_2:
                        if(shPageItems < 2)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[1 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;
                    case d_KBD_3:   
                        if(shPageItems < 3)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[2 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;
                                            
                    case d_KBD_4:
                        if(shPageItems < 4)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[3 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;
                    case d_KBD_5:
                        if(ITEMS_PER_PAGE < 5)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 5)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[4 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_6:
                        if(ITEMS_PER_PAGE < 6)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 6)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[5 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_7:
                        if(ITEMS_PER_PAGE < 7)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 7)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[6 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_8:   //Max 8 items for one page
                        if(ITEMS_PER_PAGE < 8)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 8)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[7 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    default:
                        bstatus = -1 ;
                        break;

                }
                
                if((-1) == bstatus)
                {
                    return FAIL;
                    
                }
                else if(0 == bstatus)
                {
                    break;
                }
                
            }while(1);

    }
    else
    {
        //One merchant only     
        //vduiDisplayStringCenter(1,strMMT[0].szMID);
        
    }

    srTransRec.MITid = strMMT[0].MITid;
    return SUCCESS;
    

}


int inCTOS_GetTxnPassword(void)
{
#define NO_PW           0
#define SUPER_PW            1
#define SYSTERM_PW          2
#define ENGINEERPW         3

    DebugAddSTR("inCTOS_GetTxnPassword","Processing...",20);


    unsigned char szOutput[30],szDisplay[30];
    int  inTxnTypeID;
    short ret = 0;
    short shMinLen;
    short shMaxLen;
    BYTE key;
    BYTE Bret;
    short shCount =0;
    short shRsesult = d_OK;
    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    
    memset(szOutput,0,sizeof(szOutput));
    memset(szDisplay,0,sizeof(szDisplay));
    inTxnTypeID = srTransRec.byTransType;

    vduiLightOn();
    
    inPITRead(inTxnTypeID);
    if (NO_PW== strPIT.inPasswordLevel)
    {
        return d_OK;
    }
    
    CTOS_KBDHit(&key);//clear key buffer
    while(shCount < 3)
    {
        switch(srTransRec.byTransType)
        {
            case SALE:
            case PRE_AUTH:
            case REFUND:
            case SALE_OFFLINE:
            case EPP_SALE:
            case VOID:
            case SALE_TIP:
            case SALE_ADJUST:
            case SETTLE:
            case BATCH_REVIEW:
            case BATCH_TOTAL:
			case SETUP:	
			case CASH_OUT:
			case CASH_IN:
			case CARDLESS_CASH_OUT:
			case CARDLESS_BAL_INQ:
                if(SUPER_PW == strPIT.inPasswordLevel)
                {
                	shMinLen = PWD_MIN_LEN;
					shMaxLen = PWD_MAX_LEN;
		
                    CTOS_LCDTClearDisplay();
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 7,"SUPER PASSWORD:");
                    
                    Bret = InputString(1, 8, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
					if(d_KBD_CANCEL == Bret)
                        return Bret;
                    else if(Bret == 0xFF)
                        return Bret;
					else if(strcmp(szOutput,strTCT.szSuperPW) == 0)
                        return d_OK;
                    else 
                    {
                        CTOS_LCDTClearDisplay();    
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }
                }
                else if(SYSTERM_PW== strPIT.inPasswordLevel)
                {
                	shMinLen = PWD_MIN_LEN;
					shMaxLen = PWD_MAX_LEN;
		
                    CTOS_LCDTClearDisplay();
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 7,"SYSTEM PASSWORD:");

                    Bret = InputString(1, 8, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
					if(d_KBD_CANCEL == Bret)
                        return Bret;
                    else if(Bret == 0xFF)
                        return Bret;
                    else if((strcmp(szOutput,strTCT.szSuperPW) == 0) || 
                        (strcmp(szOutput,strTCT.szSystemPW) == 0)||
                        (strcmp(szOutput,strTCT.szEngineerPW) == 0))
                    {       
                        return d_OK;
                    }
                    else 
                    {
                        CTOS_LCDTClearDisplay();    
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;                       
                        break;
                    }
                }   
                else if(ENGINEERPW== strPIT.inPasswordLevel)            
                {
                	shMinLen = PWD_MIN_LEN;
					shMaxLen = PWD_MAX_LEN;
		
                    CTOS_LCDTClearDisplay();
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 7,"PASSWORD:");

                    Bret = InputString(1, 8, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
					if(d_KBD_CANCEL == Bret)
                        return Bret;
                    else if(Bret == 0xFF)
                        return Bret;
                    else if(strcmp(szOutput,strTCT.szEngineerPW) == 0)
                    {       
                        return d_OK;
                    }
                    else 
                    {
                        CTOS_LCDTClearDisplay();    
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;                       
                        break;
                    }

                }
                else
                {            
                    return d_OK;
                }
                

            default:
                
                return d_OK;        

        }
        if(FAIL == shRsesult)
            shCount ++ ;
    }

    return shRsesult;

}



int inCTOS_SelectHost(void) 
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;

    inHostIndex = (short) strCDT.HDTid;
    
     vdDebug_LogPrintf("SATURN inCTOS_SelectHost =[%d]",inHostIndex);
    
     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
     
    if ( inHDTRead(inHostIndex) != d_OK)
    {
        vdSetErrorMessage("HOST SELECTION ERR 5");
        return(d_NO);
    } 
    else 
    {
        
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTRead(inCurrencyIdx) != d_OK) 
        {
            vdSetErrorMessage("LOAD CST ERR");
            return(d_NO);
        }


        if ( inCPTRead(inHostIndex) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }

        return (d_OK);
    }
}

int inCTOS_getCardCVV2(BYTE *baBuf)
{
    USHORT usRet;
    USHORT usInputLen;
    USHORT usLens;
    USHORT usMinLen = 3;
    USHORT usMaxLen = 4;
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

    if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
    {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr+3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);
        
        usInputLen = 7;
    }
    else
    {
        inCTOS_DisplayCardTitle(4, 5, FALSE);
        usInputLen = 7;
    }
    
    setLCDPrint(usInputLen, DISPLAY_POSITION_LEFT, "CVV2: ");
    
    while(1)
    {
        usRet = shCTOS_GetNum(usInputLen+1, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 1, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);
        if (usRet >= usMinLen && usRet <= usMaxLen)
        {
            return (d_OK);   
        }
        else if(0 == usRet)
        {
            return (d_OK);   
        }

        baBuf[0] = 0x00;
    }
}

int inCTOS_GetCVV2()
{
    BYTE szCVV2Code[CVV2_SIZE + 1];
    int inResult = FAIL;
    BYTE key;
    short shCount = 0;
    DebugAddSTR("inCTOS_GetCVV2","Processing...",20);

    memset(srTransRec.szCVV2 , 0x00, sizeof(srTransRec.szCVV2));
    memset(szCVV2Code , 0x00, sizeof(szCVV2Code));

    if(CVV2_NONE == strCDT.inCVV_II)
    {
        return d_OK;;
    }
    
    while(shCount < 3)
    {
        
        if(((CVV2_MANUAL == strCDT.inCVV_II) &&( CARD_ENTRY_MANUAL == srTransRec.byEntryMode))
        || ((CVV2_MSR == strCDT.inCVV_II) &&(( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode)))
        || ((CVV2_MANUAL_MSR == strCDT.inCVV_II) &&(( CARD_ENTRY_MANUAL == srTransRec.byEntryMode) ||( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode)))
        || ((CVV2_MANUAL_MSR_CHIP == strCDT.inCVV_II) &&(( CARD_ENTRY_MANUAL == srTransRec.byEntryMode) ||( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode) || ( CARD_ENTRY_ICC == srTransRec.byEntryMode))))
        {
            CTOS_KBDBufFlush();

            inResult = inCTOS_getCardCVV2(szCVV2Code);
            if(d_OK == inResult)
            {
                strcpy(srTransRec.szCVV2,szCVV2Code);
                return d_OK;;
            }
            else
            {
                if(d_EDM_USER_CANCEL == inResult)
                {
                    vdSetErrorMessage("USER CANCEL");
                    return inResult;
                }
                
                memset(szCVV2Code , 0x00, sizeof(szCVV2Code));
                vdDisplayErrorMsg(1, 8, "INVALID CVV");

                break;  
            }   
        }
        else
        {
            return d_OK;;
        }

        shCount ++ ;
    }
    vdSetErrorMessage("Get CVV ERR");
    return FAIL;
    
}

int inCTOS_EMVSetTransType(BYTE byTransType)
{
    switch(byTransType)
    {
        case REFUND:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x20");
        break;
		
        case PRE_AUTH:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x30");
        break;

        case SALE:
		case CASH_OUT:	
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x00");
        break;

        case BAL_INQ:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x31");
        break;
		
        default:
        break;
    }
}


int inCTOS_EMVProcessing()
{
    int inRet;
    BYTE   EMVtagVal[64];
    BYTE bySC_status; //Issue# 000065 -- jzg
	BYTE t9F33[4];

	vdDebug_LogPrintf("saturn --inCTOS_EMVProcessing--start");
	
    if( CARD_ENTRY_ICC == srTransRec.byEntryMode)
    {	

		
		vdDebug_LogPrintf("saturn 1. szEMVTermCapabilities [%02x%02x%02x]",strEMVT_Temp.szEMVTermCapabilities[0],strEMVT_Temp.szEMVTermCapabilities[1],strEMVT_Temp.szEMVTermCapabilities[2]);
		
		//Use the terminal capabilities set in Bancnet
		memset(&strEMVT_Temp,0x00, sizeof(STRUCT_EMVT));
		memset(t9F33, 0x00, sizeof(t9F33));
		
		inEMVReadData(7);
		vdDebug_LogPrintf("saturn 2. szEMVTermCapabilities [%02x%02x%02x]",strEMVT_Temp.szEMVTermCapabilities[0],strEMVT_Temp.szEMVTermCapabilities[1],strEMVT_Temp.szEMVTermCapabilities[2]);
		strcpy(t9F33, strEMVT_Temp.szEMVTermCapabilities);
		ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,t9F33);

		vdDebug_LogPrintf("saturn after ushCTOS_EMV_NewTxnDataSet");

    	// Fix for not overwrite the value tag 9F33 -- sidumili
    	/*
    	memset(t9F33, 0x00, sizeof(t9F33));
		strcpy(t9F33, srTransRec.stEMVinfo.T9F33);		
		//t9F33[1] = 0x40;
		ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,t9F33);
		*/
		
        vdCTOSS_EMV_SetAmount();//for improve transaction speed 
        inCTOS_EMVSetTransType(srTransRec.byTransType);

		inRet = usCTOSS_EMV_TxnPerform();

		vdDebug_LogPrintf("saturn --inCTOS_EMVProcessing--usCTOSS_EMV_TxnPerform--inRet[%d]", inRet);

		
		if (inRet != d_OK)
		{
			//vdSetErrorMessage("");
			return inRet;
		}
		else
		{
			inCTOS_FirstGenACGetAndSaveEMVData();

			EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

			vdDebug_LogPrintf("saturn inCTOS_EMVProcessing::EMVtagVal[%d]", EMVtagVal[0] & 0xC0);
		
	        switch( EMVtagVal[0] & 0xC0)
	        {
	            case 0: //Declined --- AAC  
	                strcpy(srTransRec.szAuthCode,"Z1");
	                vdSetErrorMessage("EMV Decline");
	                vdDebug_LogPrintf("saturn 1st ACs, card dec");
	               
	                return EMV_CRITICAL_ERROR;
	        
	            case 0x40: //Approval --- TC
	                //strcpy(srTransRec.szAuthCode,"Y1");
	                //srTransRec.shTransResult = TRANS_AUTHORIZED;
	                vdDebug_LogPrintf("saturn 1nd AC app");
	                //break;
	                vdDisplayErrorMsg(1, 8, "EMV Decline");
                    CTOS_Beep();
				    return EMV_CRITICAL_ERROR;
	                
	            case 0x80: //ARQC
	                vdDebug_LogPrintf("saturn go online");
	                break; 
	                
	            default:
	                strcpy(srTransRec.szAuthCode,"Z1");
	                vdSetErrorMessage("EMV Decline");
	                return EMV_CRITICAL_ERROR;
	        
	        }
			
		}
		
        //if (inRet != d_OK)
        //    vdSetErrorMessage("First GenAC ERR");
        
        return inRet;

    }

	vdDebug_LogPrintf("saturn --inCTOS_EMVProcessing--end");
	
    return d_OK;
}

int inCTOS_CheckTipAllowd()
{   

    if(SALE_TIP == srTransRec.byTransType)
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;
    }
    
    if (TRUE !=strHDT.fTipAllowFlag)
    {
        if (SALE_TIP == srTransRec.byTransType)
            vdSetErrorMessage("TIP NOT ALLWD");
        
        return d_NO;
    }

    return d_OK;
}



SHORT shCTOS_EMVAppSelectedProcess(void)
{
    short shResult;
    BYTE SelectedAID[16]; 
    USHORT SelectedAIDLen = 0;
    BYTE label[32];
    USHORT label_len = 0;
    BYTE PreferAID[128];
    CTOS_RTC SetRTC;
    
    vdDebug_LogPrintf("-------EMV_TxnAppSelect-----"); 
    memset(PreferAID,0,sizeof(PreferAID));
    memset(label,0,sizeof(label));
    
    shResult = usCTOSS_EMV_TxnAppSelect(PreferAID, 0, SelectedAID, &SelectedAIDLen, label, &label_len);
    vdDebug_LogPrintf("-EMV_TxnAppSelect=[%d] SelectedAIDLen[%d] label[%s]fback[%d]",shResult, SelectedAIDLen, label,strEMVT.inEMVFallbackAllowed);
    if(d_OK == shResult)
        DebugAddHEX("SelectedAIDLen", SelectedAID, SelectedAIDLen);
    
    if((shResult != PP_OK) && (shResult != EMV_USER_ABORT))
    {
		//EMV: should display "CHIP NOT DETECTED" instead of doing fallback - start -- jzg
		if (shResult == EMV_CHIP_NOT_DETECTED)
		{
			vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
			return EMV_CHIP_FAILED;
		}
		//EMV: should display "CHIP NOT DETECTED" instead of doing fallback - end -- jzg

		//VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - start -- jzg
		if (shResult == EMV_CARD_BLOCKED)
		{
			vdDisplayErrorMsg(1, 8, "CARD BLOCKED");
			return EMV_CHIP_FAILED;
		}
		//VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - end -- jzg

        if(EMV_FALLBACK == shResult)
        {
            vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
            
            CTOS_RTCGet(&SetRTC);
            inFallbackToMSR = SUCCESS;
            sprintf(strTCT.szFallbackTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        }
        else
        {
            vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
        }
        
        return EMV_CHIP_FAILED;
    }
    
    if(shResult == EMV_USER_ABORT)
    {
        

        if(strEMVT.inEMVFallbackAllowed)
        {
            vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
            
            CTOS_RTCGet(&SetRTC);
            inFallbackToMSR = SUCCESS;
            sprintf(strTCT.szFallbackTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        }
        else
        {
            vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
        }

        
        return EMV_USER_ABORT;
    }

    return d_OK;
        
}

short shCTOS_EMVSecondGenAC(BYTE *szIssuerScript, UINT inIssuerScriptlen)
{
#define ACT_ONL_APPR 1
#define ACT_ONL_DENY 2
#define ACT_UNAB_ONL 3
#define ACT_ONL_ISSUER_REFERRAL 4           //From Host
#define ACT_ONL_ISSUER_REFERRAL_APPR 4
#define ACT_ONL_ISSUER_REFERRAL_DENY 5

    USHORT usResult;
    EMV_ONLINE_RESPONSE_DATA st2ACResponseData;
    BYTE   EMVtagVal[64];
    BOOL isRemove;
	BYTE bySC_status;
	
    memset(&st2ACResponseData,0,sizeof(st2ACResponseData));

    //st2ACData.iAction will decide trans approve or not
    if(srTransRec.shTransResult == TRANS_COMM_ERROR)
        st2ACResponseData.bAction = ACT_UNAB_ONL;
    else if(srTransRec.shTransResult == TRANS_REJECTED)
        st2ACResponseData.bAction = ACT_ONL_DENY;
    else if(srTransRec.shTransResult == TRANS_CALL_BANK)
        st2ACResponseData.bAction = ACT_ONL_ISSUER_REFERRAL;
    else if(srTransRec.shTransResult == TRANS_AUTHORIZED)
        st2ACResponseData.bAction = ACT_ONL_APPR;
    //Fix for Issue Script data
    //memset(szIssuerScript,0,sizeof(szIssuerScript));
    st2ACResponseData.pAuthorizationCode = srTransRec.szRespCode;  
    st2ACResponseData.pIssuerAuthenticationData = srTransRec.stEMVinfo.T91;
    st2ACResponseData.IssuerAuthenticationDataLen = srTransRec.stEMVinfo.T91Len;
    st2ACResponseData.pIssuerScript = szIssuerScript;
    st2ACResponseData.IssuerScriptLen = inIssuerScriptlen;

    usResult = TRANS_AUTHORIZED;
    DebugAddHEX("shCTOS_EMVSecondGenAC ", st2ACResponseData.pIssuerScript, st2ACResponseData.IssuerScriptLen);
    usResult = usCTOSS_EMV_TxnCompletion(&st2ACResponseData);

    vdDebug_LogPrintf("PP_iCompletion:%d ", usResult);

    inCTOS_SecondGenACGetAndSaveEMVData();

	//test
	//usResult = PP_OK;
	//EMVtagVal[0] = 0x40;
    //test
    if(VS_TRUE == strTCT.fDemo)
    {
        usResult = PP_OK;
        EMVtagVal[0] = 0x40;
    }
    if(usResult != PP_OK)
    {
        if(strcmp((char *)srTransRec.szRespCode, "00") ==  0)
		{
			isRemove = isCheckEMVCardIsRemove(); //Check if card is remove
			if (isRemove)
			{
				//CTOS_LCDTClearDisplay();
				//vdDisplayMultiLineErrorCenter("","CHIP NOT DETECTED","", DISPLAY_POSITION_CENTER);
				//vdSetErrorMessage("CHIP NOT DETECTED");
				CTOS_LCDTClearDisplay();
				vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
				return EMV_CHIP_NOT_DETECTED;
			}
			else
			{
				//vdSetErrorMessage("EMV Decline");
				CTOS_LCDTClearDisplay();
				vdDisplayErrorMsg(1, 8, "EMV Decline");
				return EMV_CRITICAL_ERROR;
			}
		}	        
    }

    EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

	vdDebug_LogPrintf("shCTOS_EMVSecondGenAC::EMVtagVal[%d]", EMVtagVal[0] & 0xC0);
	
    switch( EMVtagVal[0] & 0xC0)
    {
        case 0: //Declined --- AAC
            if(strcmp((char *)srTransRec.szRespCode, "00") ==  0)  //approve by host, but not by card
            {
				CTOS_SCStatus(d_SC_USER, &bySC_status);
				if (!(bySC_status & d_MK_SC_PRESENT))
				{
					CTOS_LCDTClearDisplay();
					vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
					return EMV_CHIP_NOT_DETECTED;
					
				}
				else
				{
					CTOS_LCDTClearDisplay();
					vdDisplayErrorMsg(1, 8, "EMV Decline");									
				}
            }
            else
            {
                vdDebug_LogPrintf("Host reject");
            }
            return EMV_CRITICAL_ERROR;

        case 0x40: //Approval --- TC

			if(srTransRec.shTransResult != TRANS_AUTHORIZED)
			{
				CTOS_SCStatus(d_SC_USER, &bySC_status);
				if (!(bySC_status & d_MK_SC_PRESENT))
				{	
					CTOS_LCDTClearDisplay();
					vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
					return EMV_CHIP_NOT_DETECTED;
				}
				else
				{
					CTOS_LCDTClearDisplay();
					vdDisplayErrorMsg(1, 8, "EMV DECLINE");
				}

				return EMV_CRITICAL_ERROR;	
			}

            vdDebug_LogPrintf("2nd AC app");
            break;

        default:
			CTOS_SCStatus(d_SC_USER, &bySC_status);
			if (!(bySC_status & d_MK_SC_PRESENT))
			{
				CTOS_LCDTClearDisplay();
				vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");	
				return EMV_CHIP_NOT_DETECTED;
			}
			else
			{
				CTOS_LCDTClearDisplay();
				vdDisplayErrorMsg(1, 8, "EMV Decline");
			}

            //vdSetErrorMessage("EMV Decline");
            return EMV_CRITICAL_ERROR;

    }
    
    vdDebug_LogPrintf("SATURN End 2nd GenAC shTransResult=%d iAction=%d",srTransRec.shTransResult, st2ACResponseData.bAction);
    return PP_OK;
    
}

int inCTOS_EMVTCUpload(void)
{
    int inRet;
    
    if( CARD_ENTRY_ICC == srTransRec.byEntryMode)
    {
        inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
        inRet = inProcessEMVTCUpload(&srTransRec, -1);// TC upload

            vdDebug_LogPrintf("szFileName, %s%02d%02d.rev"
                                , strHDT.szHostLabel
                                , strHDT.inHostIndex
                                , srTransRec.MITid);
        inCTOS_inDisconnect();
    }
    
    return d_OK;
}


int inCTOS_GetPubKey(const char *filename, unsigned char *modulus, int *mod_len, unsigned char *exponent, int *exp_len)
{
    unsigned char tmp[1024];
    int iRead;
    int iMod;
    int iExp;
    FILE  *fPubKey;                   
    UINT uintRet ;             

    fPubKey = fopen( (char*)filename, "rb" );
    if (fPubKey == NULL)
        return CTOS_RET_PARAM;

    uintRet = fread( tmp, 1, sizeof(tmp), fPubKey );
    fclose(fPubKey);  
    vdDebug_LogPrintf("CAPK=Len[%d]==[%s]",uintRet,tmp);
    
    if(uintRet >0)
    {
        iMod=(int)(tmp[0]-0x30)*100+(tmp[1]-0x30)*10+(tmp[2]-0x30);     
        vdDebug_LogPrintf("iMod===[%d]",iMod);
        if(iMod%8 != 0)
            return(CTOS_RET_PARAM);

        if(iMod > CTOS_PED_RSA_MAX)
            return(CTOS_RET_PARAM);

        *mod_len=iMod;      
        wub_str_2_hex((tmp+3), (modulus), iMod*2);
        
        vdDebug_LogPrintf("*mod_len===[%d]",*mod_len);
        DebugAddHEX("Module HEX string===", modulus, iMod);

        
        iExp=(int)tmp[iMod*2+4] - 0x30;
        wub_str_2_hex((&tmp[5+(iMod*2)]), (exponent), iExp*2);

        vdDebug_LogPrintf("iExp===[%d]",iExp);      
        DebugAddHEX("Exponent HEX string===", exponent, iExp);

        *exp_len = iExp;        
    }
    else
        return(CTOS_RET_CALC_FAILED);

    return(CTOS_RET_OK);
}

USHORT ushCTOS_EMV_NewDataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue)
{
    USHORT usResult;
    USHORT usTagLen;
    static USHORT usGetEMVTimes = 0;
    
    usResult = usCTOSS_EMV_DataGet(usTag, &usTagLen, pValue);
    *pLen = usTagLen;

    usGetEMVTimes ++;
    vdDebug_LogPrintf("ushCTOS_EMV_NewDataGet Times[%d] usTagLen[%d]",usGetEMVTimes, usTagLen);
    return usResult;

}

USHORT ushCTOS_EMV_NewTxnDataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue)
{
    USHORT usResult;
    static USHORT usSetEMVTimes = 0;

    usResult = usCTOSS_EMV_DataSet(usTag, usLen, pValue);
    usSetEMVTimes ++;
    vdDebug_LogPrintf("ushCTOS_EMV_NewTxnDataSet Times[%d] usResult[%d]",usSetEMVTimes, usResult);

    return usResult;
}

short shCTOS_EMVGetChipDataReady(void)
{
    short       shResult;
    BYTE        byDataTmp1[64];
    BYTE        byVal[64];
    USHORT      usLen;
    USHORT      inIndex ;
    BYTE        szDataTmp[5];
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    char chTemp;
    USHORT inTagLen = 0;
    

    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szOutEMVData,0,sizeof(szOutEMVData));

    shResult = usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_SELECT_APP, &inTagLen, szOutEMVData);
    DebugAddHEX("GET_EMV_TAG_AFTER_SELECT_APP",szOutEMVData,inTagLen);
        
    shResult = usCTOSS_FindTagFromDataPackage(TAG_57, byVal, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_57[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, byVal[0], byVal[1], byVal[2]);

    memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
    wub_hex_2_str(byVal, byDataTmp1, usLen);
    memcpy(srTransRec.szTrack2Data, byDataTmp1, (usLen*2));

	vdDebug_LogPrintf("saturn getchipdataready track 2 %s", srTransRec.szTrack2Data);
    for(inIndex = 0; inIndex < (usLen*2); inIndex++)
    {
        if(byDataTmp1[inIndex] == 'F')
            srTransRec.szTrack2Data[inIndex]=0;
    }
    vdDebug_LogPrintf("szTrack2Data: %s %d", srTransRec.szTrack2Data, inIndex);
    
    for(inIndex = 0; inIndex < (usLen*2); inIndex++)
    {
        if(byDataTmp1[inIndex] != 'D')
            srTransRec.szPAN[inIndex] = byDataTmp1[inIndex];
        else
            break;
    }
    srTransRec.byPanLen = inIndex;
    vdDebug_LogPrintf("PAN: %s %d", srTransRec.szPAN, inIndex);
    inIndex++;
    memset(szDataTmp, 0x00, sizeof(szDataTmp));
    wub_str_2_hex(&byDataTmp1[inIndex], szDataTmp, 4);
    srTransRec.szExpireDate[0] = szDataTmp[0];
    srTransRec.szExpireDate[1] = szDataTmp[1];
    vdMyEZLib_LogPrintf("EMV functions Expiry Date [%02x%02x]",srTransRec.szExpireDate[0],srTransRec.szExpireDate[1]);
    inIndex = inIndex + 4;
    memcpy(srTransRec.szServiceCode, &byDataTmp1[inIndex], 3);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5A_PAN, srTransRec.stEMVinfo.T5A, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_5A_PAN[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, srTransRec.stEMVinfo.T5A[0], srTransRec.stEMVinfo.T5A[1], srTransRec.stEMVinfo.T5A[2]);

    srTransRec.stEMVinfo.T5A_len = (BYTE)usLen;
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F30_SERVICE_CODE, srTransRec.stEMVinfo.T5F30, &usLen, szOutEMVData, inTagLen);
    
    memset(byVal, 0x00, sizeof(byVal));
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F34_PAN_IDENTFY_NO, byVal, &usLen, szOutEMVData, inTagLen);
    vdMyEZLib_LogPrintf("5F34: %02x %d", byVal[0], usLen);
    srTransRec.stEMVinfo.T5F34_len = usLen;
    srTransRec.stEMVinfo.T5F34 = byVal[0];

    shResult = usCTOSS_FindTagFromDataPackage(TAG_82_AIP, srTransRec.stEMVinfo.T82, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_82_AIP-[%02x][%02x]-", srTransRec.stEMVinfo.T82[0], srTransRec.stEMVinfo.T82[1]); 

    shResult = usCTOSS_FindTagFromDataPackage(TAG_84_DF_NAME, srTransRec.stEMVinfo.T84, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T84_len = (BYTE)usLen;

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F24_EXPIRE_DATE, srTransRec.stEMVinfo.T5F24, &usLen, szOutEMVData, inTagLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F08_IC_VER_NUMBER, szDataTmp, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_9F08_IC_VER_NUMBER-[%02x][%02x]-",szDataTmp[0],szDataTmp[1]);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_9F09_TERM_VER_NUMBER-[%02x][%02x]-",srTransRec.stEMVinfo.T9F09[0],srTransRec.stEMVinfo.T9F09[1]); 
    if(usLen == 0)
        memcpy(srTransRec.stEMVinfo.T9F09, "\x00\x4C", 2);// can not get value from api like verifone,so i hardcode a value from EMV level 2 cert document
    vdDebug_LogPrintf("9F09: %02x%02x %d", srTransRec.stEMVinfo.T9F09[0],srTransRec.stEMVinfo.T9F09[1], usLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F20, srTransRec.szCardholderName, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("5F20,szCardholderName: %s", srTransRec.szCardholderName);    

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F12, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("9F12,szCardholderName: %s", srTransRec.stEMVinfo.szChipLabel);

    chTemp=(char)srTransRec.stEMVinfo.szChipLabel[0];
    if(chTemp > 0x5a)
    {
		shResult = usCTOSS_FindTagFromDataPackage(TAG_50, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
		vdDebug_LogPrintf("50,szCardholderName: %s", srTransRec.stEMVinfo.szChipLabel);
    }   
}


int inCTOS_FirstGenACGetAndSaveEMVData(void)
{
    USHORT usLen = 64;
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;    
    int ret = 0;
    unsigned char szTransSeqCounter[6+1];
	unsigned char szHEXTransSeqCounter[3+1];
    
	BYTE bAppLabel[64] = {0}; // EMV: Get Application Label -- jzg 
	BYTE bAppPrefName[64] = {0}; // EMV: Get Application Preferred Name -- jzg
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	
    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szGetEMVData,0,sizeof(szGetEMVData));

    usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_1STAC, &inTagLen, szOutEMVData);
    DebugAddHEX("GET_EMV_TAG_AFTER_1STAC",szOutEMVData,inTagLen);

    vdDebug_LogPrintf("inCTOS_FirstGenACGetAndSaveEMVData");

	if (strCST.inCurrencyIndex > 1){
		memset(szAscBuf, 0x00, sizeof(szAscBuf));	
		memset(szBcdBuf, 0x00, sizeof(szBcdBuf));
	
		sprintf(szAscBuf,"%4s",strCST.szCurCode);	
		
		wub_str_2_hex(szAscBuf, szBcdBuf, 4);
		memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);
	}
	else
    	usCTOSS_FindTagFromDataPackage(TAG_5F2A_TRANS_CURRENCY_CODE, srTransRec.stEMVinfo.T5F2A, &usLen, szOutEMVData, inTagLen);

	/* EMV: Get Application Preferred Name - start -- jzg */
	usCTOSS_FindTagFromDataPackage(TAG_9F12, bAppPrefName, &usLen, szOutEMVData, inTagLen);
	vdDispAppLabel(bAppPrefName, usLen, srTransRec.stEMVinfo.szChipLabel);
	vdDebug_LogPrintf("TAG 9F12 = [%s]", srTransRec.stEMVinfo.szChipLabel);
	/* EMV: Get Application Preferred Name - end -- jzg */

	vdDebug_LogPrintf("TAG 9F12 = [0x%02X]", srTransRec.stEMVinfo.szChipLabel[0]);
	
	/* EMV: Get Application Label - start -- jzg */
	if ((!((srTransRec.stEMVinfo.szChipLabel[0] >= 'a') && (srTransRec.stEMVinfo.szChipLabel[0] <= 'z'))) ||
	(!((srTransRec.stEMVinfo.szChipLabel[0] >= 'A') && (srTransRec.stEMVinfo.szChipLabel[0] <= 'Z'))))
	{
		//memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));
		usCTOSS_FindTagFromDataPackage(TAG_50, bAppLabel, &usLen, szOutEMVData, inTagLen);

		if (strlen(bAppLabel) > 0){
			memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));
			vdDispAppLabel(bAppLabel, usLen, srTransRec.stEMVinfo.szChipLabel);
			vdDebug_LogPrintf("TAG 50 = [%s]", srTransRec.stEMVinfo.szChipLabel);
		}
	}
            
    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9A_TRANS_DATE, srTransRec.stEMVinfo.T9A, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9C_TRANS_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9C), &usLen, szOutEMVData, inTagLen);
        
    usCTOSS_FindTagFromDataPackage(TAG_9F06, srTransRec.stEMVinfo.T9F06, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;

	if (strCST.inCurrencyIndex > 1){
		memset(szAscBuf, 0x00, sizeof(szAscBuf));	
		memset(szBcdBuf, 0x00, sizeof(szBcdBuf));
	
		sprintf(szAscBuf,"%4s",strCST.szCurCode);	
		
		wub_str_2_hex(szAscBuf, szBcdBuf, 4);
		memcpy((char *)srTransRec.stEMVinfo.T9F1A, &szBcdBuf[0], 2);
	}
	else
	    usCTOSS_FindTagFromDataPackage(TAG_9F1A_TERM_COUNTRY_CODE, srTransRec.stEMVinfo.T9F1A, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F1E, srTransRec.stEMVinfo.T9F1E, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F33_TERM_CAB, srTransRec.stEMVinfo.T9F33, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F35_TERM_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9F35), &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F36_ATC, srTransRec.stEMVinfo.T9F36, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F36_len = usLen;
    
    usCTOSS_FindTagFromDataPackage(TAG_9F37_UNPREDICT_NUM, srTransRec.stEMVinfo.T9F37, &usLen, szOutEMVData, inTagLen);

    
    ret = inIITRead(srTransRec.IITid);
    vdDebug_LogPrintf("inIITRead[%d]",ret);
    sprintf(szTransSeqCounter, "%08ld", strIIT.ulTransSeqCounter);
    wub_str_2_hex(szTransSeqCounter, (char *)szHEXTransSeqCounter, 8);
    memcpy(srTransRec.stEMVinfo.T9F41,szHEXTransSeqCounter,4);
    strIIT.ulTransSeqCounter++;
    ret = inIITSave(srTransRec.IITid);    
    vdDebug_LogPrintf(" ret[%d] srTransRec.IITid[%d]strIIT.ulTransSeqCounter[%ld]",ret, srTransRec.IITid,strIIT.ulTransSeqCounter);

	DebugAddHEX("2. srTransRec.stEMVinfo.T9F41",srTransRec.stEMVinfo.T9F41,4);
	
	//usCTOSS_FindTagFromDataPackage(TAG_9F41, srTransRec.stEMVinfo.T9F41, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F53, (BYTE *)&(srTransRec.stEMVinfo.T9F53), &usLen, szOutEMVData, inTagLen);

    return d_OK;
}

int inCTOS_SecondGenACGetAndSaveEMVData(void)
{

    USHORT usLen = 64;
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;
    
    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szGetEMVData,0,sizeof(szGetEMVData));

    usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_2NDAC, &inTagLen, szOutEMVData);
    DebugAddHEX("GET_EMV_TAG_AFTER_1STAC",szOutEMVData,inTagLen);

    vdDebug_LogPrintf("inCTOS_SecondGenACGetAndSaveEMVData");
                
    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);
                    
    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;
            
    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, szOutEMVData, inTagLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, szOutEMVData, inTagLen);
        
    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, szOutEMVData, inTagLen);
                    
    return d_OK;
}

int inCTOS_showEMV_TagLog (void)
{
    #define TOTAL_TAGS  24
    int i;
    unsigned short tagLen;
    char outp[40];
    
    typedef struct
    {
            unsigned short Tags;
            char description[20];
    }print_tag;
    
    print_tag EMVTag[TOTAL_TAGS] = 
    { 
        {   TAG_95,                     "TVR"},         
        {   TAG_9B,                     "TSI"},
        {   TAG_9F26_EMV_AC,            "ARQC"},
        {   TAG_9F27,                   "Crypt Info Data"},
        {   TAG_9F10_IAP,               "Issuer Appl Data"},
        {   TAG_9F37_UNPREDICT_NUM,     "Unpredicte number"},
        {   TAG_9F36_ATC,               "Appl Trans Counter"},
        {   TAG_9A_TRANS_DATE,          "Trans Date"},
        {   TAG_9C_TRANS_TYPE,          "Trans Type"},
        {   TAG_5A_PAN,                 "PAN"},
        {   TAG_5F34_PAN_IDENTFY_NO,    "Sequence Num"},
        {   TAG_9F02_AUTH_AMOUNT,       "Amount Authorized"},
        {   TAG_9F03_OTHER_AMOUNT,      "Add Amount"},
        {   TAG_5F2A_TRANS_CURRENCY_CODE, "Trans Currency Code"},
        {   TAG_82_AIP,                 "AIP"},
        {   TAG_9F1A_TERM_COUNTRY_CODE, "Term Country Code"},
        {   TAG_9F34_CVM,               "CVR"},
        {   TAG_9F10_IAP,               "Issuer auth Data"},
        {   TAG_9F06,                   "AID"},
        {   TAG_50,                     "Appl Label Name"},
        {   TAG_8F,                     "CA Public Key Index"},
        {   TAG_9F0D,                   "IAC Default"},
        {   TAG_9F0E,                   "IAC Denial"},
        {   TAG_9F0F,                   "IAC Online"}

    };


    for(i = 0; i<TOTAL_TAGS; i++)
    {
        memset(outp,0x00,sizeof(outp));
        ushCTOS_EMV_NewDataGet(EMVTag[i].Tags, &tagLen, outp);
        vdDebug_LogPrintf("----TAG[%s][%x]=====Len[%d]----",EMVTag[i].description,EMVTag[i].Tags,tagLen); 
        DebugAddHEX("Value===",outp,tagLen);
    }
    return d_OK;
}

void vdCTOSS_GetAmt(void)
{
	memcpy(srTransRec.szBaseAmount, szBaseAmount, 6);	
}

void vdCTOSS_SetAmt(BYTE *baAmount)
{
	BYTE szTemp[20];

	memset(szTemp, 0x00, sizeof(szTemp));
	sprintf(szTemp, "%012ld", atol(baAmount));
	wub_str_2_hex(szTemp, szBaseAmount,12);	
}

void vdDispAppLabel(unsigned char *ucHex, int inLen, char *szOutStr)
{
	int i;
	char szBuf[80] = {0};

	for (i = 0; i < inLen; i++)
		szBuf[i] = ucHex[i];
	szBuf[i] = 0x00;

	memcpy(szOutStr, szBuf, inLen);
}
int inCTOS_FraudControl(void)
{
    int inRet=0,inPANLen;
	BYTE strOut[4+1];//, szPAN[4+1];
	short shMaxLen=4, shMinLen=4;
	
    if(strHDT.fLast4Digit != TRUE)
        return d_OK;	  	

    if(srTransRec.byEntryMode != CARD_ENTRY_MSR)
        return d_OK;		
		
    //CTOS_LCDTClearDisplay();
    //vduiLightOn();
    
    //vdDispTransTitle(srTransRec.byTransType);
	vdClearBelowLine(2);
	
	displayAppbmpDataEx(140, 35, strIIT.szIssuerLogo, TRUE);
    
    
    inPANLen=strlen(srTransRec.szPAN)-4;
    //memset(szPAN, 0, sizeof(szPAN));
	//memcpy(szPAN, &srTransRec.szPAN[inPANLen-4], 4);

	while(1)
	{	
		vduiClearBelow(8);
		CTOS_LCDTPrintXY(1, 7, "LAST 4 DIGITS ACCNT:");
		memset(strOut,0x00, sizeof(strOut));
		inRet = InputString(1, 8, 0x00, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
		if (inRet == d_KBD_CANCEL )
		{
			vdSetErrorMessage("USER CANCEL");
			return d_NO;
		}
		else if(inRet == 0)
			return d_NO;
        else if(inRet == 0xFF)
        {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }
		else if(inRet>=1)
		{
			if(memcmp(strOut, srTransRec.szPAN+inPANLen, 4) == 0) 	
				break;
			else
				vdDisplayErrorMsg(1, 8, "INVALID ACCOUNT NO.");
		}	
	}	

	return d_OK;
}

void vdCTOS_SetDateTime(void)
{
    CTOS_RTC SetRTC;
    BYTE szCurrentTime[20];
    
    CTOS_RTCGet(&SetRTC);
    memset(szCurrentTime, 0, sizeof(szCurrentTime));
    sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
    wub_str_2_hex(szCurrentTime,srTransRec.szDate,DATE_ASC_SIZE);
    
    memset(szCurrentTime, 0, sizeof(szCurrentTime));
    sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
    wub_str_2_hex(szCurrentTime,srTransRec.szTime,TIME_ASC_SIZE);
}

int inCheckValidApp(int HDTid)
{
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    
    if(strCDT.HDTid != HDTid)
    {
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;			 
    }

    return d_OK;
}

int inGetMerchantPassword(void)
{
    BYTE strOut[6+1]={0};
    int inRet;
    USHORT shMaxLen, shMinLen;

    if(strMMT[0].fPasswordEnable == TRUE)
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
		
        while(1)
        {	
            shMinLen = PWD_MIN_LEN;
			shMaxLen = PWD_MAX_LEN;
			
            vduiClearBelow(8);		
            CTOS_LCDTPrintXY(1, 7, "MERCHANT PASSWORD:");
            memset(strOut,0x00, sizeof(strOut));
            inRet = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

			if(inGetKeyPressMsg(inRet) != d_OK)
                return d_NO; 				
			
            if(strcmp(strOut,strMMT[0].szPassword) == 0)
                return d_OK;
            else
                vdDisplayErrorMsg(1, 8, "INVALID PASSWORD");
        }		
	}
	else
        return d_OK;		
}
	
int inGetKeyPressMsg(int key)
{
	if (key == d_KBD_CANCEL || key == 0)
		vdSetErrorMessage("USER CANCEL");
	else if(key == 0xFF)
		vdSetErrorMessage("TIME OUT");
    else if(key > 0)
        return d_OK;

    return d_NO;
}

void vdCTOSS_ChangeDefaultApplication(void)
{
    CTOS_stCAPInfo stinfo;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;
	short shHostIndex;
	int inResult;
    unsigned char ckey;

	if (inMultiAP_CheckSubAPStatus() != d_OK)
	{
        char szChoiceMsg[30 + 1];
        char szHeaderString[24+1];
        int bHeaderAttr = 0x01+0x04, key=0; 
		
        memset(szHeaderString, 0, sizeof(szHeaderString));
        memset(szChoiceMsg, 0, sizeof(szChoiceMsg));
        
        strcpy(szHeaderString, "SWITCH APP");
        strcat(szChoiceMsg,"NEW HOST");
        key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
        /*
              BANCNET shHostIndex=1
              BANCNET ONE APP shHostIndex=19
		*/
        if (key > 0)
        {
            if (key == 1)
            {
				inCTOSS_ForceSettle();
                shHostIndex=19;
            }
            else
                return;
        }
        
        //vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
        srTransRec.HDTid = shHostIndex;
        strHDT.inHostIndex = shHostIndex;
        inHDTRead(shHostIndex);
        inCPTRead(shHostIndex);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);		
	}
	
    CTOS_LCDTClearDisplay();
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
		inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_FORCE_SETTLE);
		
        inResult = inCTOS_MultiAPSaveData(d_IPC_CMD_CHANGE_DEF_APP);
        if(d_OK != inResult)
            return ;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inResult = inCTOS_MultiAPGetData();
            if(d_OK != inResult)
                return ;
            
            inResult = inCTOS_MultiAPReloadHost();
            if(d_OK != inResult)
                return ;
        }
    }

    getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	strcpy(exe_subdir, &exe_dir[9]);
	inExeAPIndex = atoi(exe_subdir);
	memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
    if(CTOS_APGet(inExeAPIndex, &stinfo) != d_OK)
		CTOS_APGet(inExeAPIndex, &stinfo);

	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 7, "PLEASE WAIT"); 		
	CTOS_LCDTPrintXY(1, 8, "SWITCHING...");

    //android_removed
    //inResult=CTOS_APSet(inExeAPIndex, d_AP_FLAG_DEF_SEL);
	//end

	CTOS_Delay(3000);

    if(strHDT.inHostIndex == 1)
        inUpdateDefaultApp(1, 19);
    else if(strHDT.inHostIndex == 19)
        inUpdateDefaultApp(19, 1);
	
	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 7, "PLEASE PRESS POWER"); 		
	CTOS_LCDTPrintXY(1, 8, "BUTTON TO RESTART");

    while(1)
	    CTOS_KBDGet(&ckey);
	
    CTOS_SystemReset();	
}

int inCTOSS_ForceSettle(void)
{
	int inResult;
	int shHostIndex = 1;
	int inNum = 0;
	int inNumOfHost = 0;
	int inNumOfMerchant = 0;
	int inLoop =0 ;
	ACCUM_REC srAccumRec;
	STRUCT_FILE_SETTING strFile;
	char szAPName[50];
	int inAPPID;

	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 7, "PLEASE WAIT");		
	CTOS_LCDTPrintXY(1, 8, "CHECKING BATCH...");

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
					strMMT[0].HDTid = strHDT.inHostIndex;
					strMMT[0].MITid = strMMT[inLoop-1].MITid;
                    strcpy(strMMT[0].szTID, strMMT[inLoop-1].szTID);
                    strcpy(strMMT[0].szMID, strMMT[inLoop-1].szMID);                       
					memcpy(strMMT[0].szBatchNo, strMMT[inLoop-1].szBatchNo, 4);

					srTransRec.HDTid = strMMT[0].HDTid;
					srTransRec.MITid = strMMT[inLoop-1].MITid;					
					strcpy(srTransRec.szTID, strMMT[0].szTID);
					strcpy(srTransRec.szMID, strMMT[0].szMID);
					memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
					strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

					vdDebug_LogPrintf("srTransRec.MITid[%d]strHDT.inHostIndex[%d]", srTransRec.MITid, strHDT.inHostIndex);
					memset(&srAccumRec,0,sizeof(srAccumRec));
					memset(&strFile,0,sizeof(strFile));
					memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
					memset(&strFile,0,sizeof(strFile));
					vdCTOS_GetAccumName(&strFile, &srAccumRec);

				if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) > 0)
				{
                    inCTOS_PrintSettleReport();
                    inCTOS_SettlementClearBathAndAccum();
                    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);					
				}
			}
		}
		else
			continue;

	}
	
	return (d_OK);
}




//sidumili: pad string
void vdCTOS_Pad_String(char* str,int padlen,char padval,int padtype) {
    int padno;

    if ((padno = padlen - strlen(str)) > 0) {
        if (padtype == POSITION_LEFT)
            memmove(str + padno, str, strlen(str) + 1);
        else
            str += strlen(str);
        memset(str, padval, padno);
        if (padtype == POSITION_RIGHT)
            *(str + padno) = '\0';
    } else if (padno < 0) {
        // Truncate string if too long!!
        memmove(str, str + abs(padno), padlen + 1);
    }
}
//sidumili: pad string
/*albert albert (ecr tct update fail) - save last invoice number, came from ushCTOS_printAll*/
void vdSaveLastInvoiceNo(void)
{
    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);   
    if(inTCTSave(1) != ST_SUCCESS)
        vdDisplayErrorMsg(1, 8, "Update TCT fail");
}

BOOL isCheckEMVCardIsRemove(void)
{
	BYTE bySC_status;
	BOOL isRemove = TRUE;

	if (srTransRec.byEntryMode != CARD_ENTRY_ICC)
		return FALSE;
	
	CTOS_SCStatus(d_SC_USER, &bySC_status);
	if(bySC_status & d_MK_SC_PRESENT)
		isRemove = FALSE;

	vdDebug_LogPrintf("::isCheckEMVCardIsRemove::isRemove[%d]::szRespCode[%s]::byTransType[%d]::byEntryMode[%d]", isRemove, srTransRec.szRespCode, srTransRec.byTransType, srTransRec.byEntryMode);
	
	return(isRemove);
}

// BancNet Requirements (GMT) -- sidumili
int inGMTCompute(int inLocalTime)
{
	int inGMT = 0;
	int inOffSet = 0;

	if ((inLocalTime >= 0) && (inLocalTime <= 7))
	{
		inOffSet = 16;
		inGMT = inLocalTime + inOffSet;
	}
	else
	{
		inOffSet = 8;
		inGMT = inLocalTime - inOffSet;
	}
	
	return(inGMT);
		
}

//for improve transaction speed 
void vdCTOSS_EMV_SetAmount(void)
{
	BYTE szBaseAmount[20];
	BYTE szTipAmount[20];
	BYTE szTotalAmount[20];
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	BYTE  byDataTmp1[32];
	BYTE  byDataTmp2[32];
	BYTE  bPackSendBuf[2048];
	USHORT usPackSendLen = 0;
	USHORT ushEMVtagLen;
	ULONG lnTmp;
	//OFFLINEPINDISPLAY_REC szDisplayRec;
		
  memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
  memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
  wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
  lnTmp = atol(byDataTmp1);
  wub_long_2_array(lnTmp, byDataTmp2);

  memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
  bPackSendBuf[usPackSendLen++] = 0x04;
  memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
  usPackSendLen += 4;
  
  if (srTransRec.byTransType == CASH_OUT)
  	  memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szBaseAmount, 6);
  else
	  memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szTotalAmount, 6);
  
  memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x02", 2);
  usPackSendLen += 2;
  bPackSendBuf[usPackSendLen++] = 0x06;
  memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F02, 6);
  usPackSendLen += 6;

  if(atol(szTipAmount) > 0)
  {
      memcpy(srTransRec.stEMVinfo.T9F03, srTransRec.szTipAmount, 6);
  }
  else
  {
      memset(szTipAmount, 0x00, sizeof(szTipAmount));
      memcpy(srTransRec.stEMVinfo.T9F03, szTipAmount, 6);
  }

  memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x03", 2);
  usPackSendLen += 2;
  bPackSendBuf[usPackSendLen++] = 0x06;
  memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F03, 6);
  usPackSendLen += 6;

#if 0
  //for offline pin display
  memcpy(&bPackSendBuf[usPackSendLen], "\xFF\xFE", 2);
  usPackSendLen += 2;
  //bPackSendBuf[usPackSendLen++] = 0x06;
  memset(&szDisplayRec,0x00,sizeof(OFFLINEPINDISPLAY_REC));
  vdCTOSS_PackOfflinepinDisplay(&szDisplayRec);
  memcpy(&bPackSendBuf[usPackSendLen], &szDisplayRec, sizeof(OFFLINEPINDISPLAY_REC));
  usPackSendLen += sizeof(OFFLINEPINDISPLAY_REC);
#endif
  vdPCIDebug_HexPrintf("EMV_SetAmount",bPackSendBuf,usPackSendLen);
  usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);

}

/*sidumili: [Void not allowed on debit transaction]*/
int inCTOS_CheckHostVoidAllwd(void)
{	
	vdDebug_LogPrintf("--inCTOS_CheckHostVoidAllwd--");
	vdDebug_LogPrintf("HDTid[%d]:::byVoided[d]", srTransRec.HDTid, srTransRec.byVoided);
	
	if (!srTransRec.byVoided){
		vdSetErrorMessage("VOID NOT ALLOWED");
		return d_NO;
	}

	return d_OK;

}

int isCheckTerminalNonTouch(void) 
{
    if (strTCT.byTerminalType == 3)
        return d_OK;
	
    return d_NO;
}

int inCTOS_PreConnectAndInit(void)
{
    int inRetVal;

	vdDebug_LogPrintf("--inCTOS_PreConnectAndInit--");
	vdDebug_LogPrintf("inCommunicationMode[%d]", strCPT.inCommunicationMode);
	vdDebug_LogPrintf("fGPRSConnectOK[%d]", fGPRSConnectOK);

    if(strCPT.inCommunicationMode == DIAL_UP_MODE || strCPT.inCommunicationMode == ETHERNET_MODE)
		return d_OK;	

    vdDebug_LogPrintf("inCTOS_PreConnectAndInit1");
	vdSetInit_Connect(1);
	
    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;	
		    
    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);

    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
    {
		vdDisplayMultiLineMsgAlign("", "INITIALIZATION", "ERROR", DISPLAY_POSITION_CENTER);
		vdSetErrorMessage("");
        return(d_NO);
    }

	if (get_env_int("CONNECTED") == 1)
		return(d_OK);

	if(VS_TRUE == strTCT.fDemo)
		return(d_OK);

	vdDebug_LogPrintf("inCTOS_PreConnectAndInit2");
    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode); 

		if (inRetVal != d_OK)
		{
			if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
			{
				 vdDisplayMultiLineMsgAlign("", "GPRS PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
				 vdSetErrorMessage("");
			}
			//wifi-mod2
			else if (srTransRec.usTerminalCommunicationMode == WIFI_MODE)		
			{
				vdDisplayMultiLineMsgAlign("", "WIFI PROBLEM", "PLESE TRY AGAIN", DISPLAY_POSITION_CENTER);
				vdSetErrorMessage("");
			}
			//wifi-mod2

			else
			{
				vdDisplayMultiLineMsgAlign("", "INITIALIZATION", "ERROR", DISPLAY_POSITION_CENTER);
				vdSetErrorMessage("");
			}

			if(srTransRec.byTransType == SETTLE)
			{
				vdDisplayMultiLineMsgAlign("", "", "SETTLE FAILED", DISPLAY_POSITION_CENTER);
				vdSetErrorMessage("");
			}
			
			return(d_NO);
		}

	vdDebug_LogPrintf("inCTOS_PreConnectAndInit3");
	
	if (CN_FALSE == srTransRec.byOffline)
    {   
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);        
	    //for improve transaction speed
    }

	put_env_int("CONNECTED",1);

	vdDebug_LogPrintf("inCTOS_PreConnectAndInit4");
	
    return(d_OK);
}

int inConfirmPAN(void)
{
    unsigned char key;
	int inRet = d_NO;

	vdDebug_LogPrintf("--inConfirmPAN--");
	vdDebug_LogPrintf("inConfirmPAN[%d]", strTCT.fConfirmPAN);

	setLCDPrint(8, DISPLAY_POSITION_CENTER, "CONFIRM?YES[OK]NO[X]");
	
	if (strTCT.fConfirmPAN)
	{
		while (1)
		{
			CTOS_KBDGet(&key);

			if (key == d_KBD_ENTER)
			{
				inRet = d_OK;
				break;
			}

			if (key == d_KBD_CANCEL)
			{
				inRet = d_NO;
				break;
			}
		}
	}
	else
		inRet = d_OK;

	if (inRet != d_OK)
		vdSetErrorMessage("USER CANCEL");

	return(inRet);
}

void vdCheckISOLog(void)
{
	int inLog = get_env_int("ISOLOG");	
	
	vdDebug_LogPrintf("--vdCheckISOLog--");
	
	if (inLog > 0)
		fISOLog = TRUE;
	else
		fISOLog = FALSE;

	vdDebug_LogPrintf("inLog[%d] | fISOLog[%d]", inLog, fISOLog);
}

void vdGetCRC(void)
{
	FILE* file;
	int nLen;
	unsigned char chBuffer[1024];
	BYTE baTemp[10+1];
	BOOL fHD = TRUE;

	vdDebug_LogPrintf("--vdGetCRC--");
		
	memset(chBuffer, 0, 1024);
	
	if ((file = fopen ("V5S_BANCNET", "rb")) != NULL)
	{
		while (nLen = fread (chBuffer, 1, 512, file))
		{
		}
		
		memset(baTemp, 0x00, sizeof(baTemp));
		sprintf(baTemp, "%02x%02x%02x%02x", 
			wub_lrc((unsigned char *) &chBuffer[0], 128),
			wub_lrc((unsigned char *) &chBuffer[127], 128),
			wub_lrc((unsigned char *) &chBuffer[255], 128),
			wub_lrc((unsigned char *) &chBuffer[511], 128));


		if (fHD)
		{
			put_env_char("V5S_BANCNET_CRC","1e12246f");
		}
		else
		{
			put_env_char("V5S_BANCNET_CRC",baTemp);
		}
				
		fclose (file);
	}

}

// Enhancement for printing speed
void vdCheckERMMode(void)
{
	vdDebug_LogPrintf("--vdCheckERMMode--");
	
	if(strTCT.byERMMode != 0)
		fERMTransaction = TRUE;
	else
		fERMTransaction = FALSE;

	//vdDebug_LogPrintf("byERMMode[%d] | fERMTransaction[%d]", strTCT.byERMMode, fERMTransaction);
}

void SetTransYear(void)
{
	CTOS_RTC SetRTC;
	char szYear[4 + 1];

	vdDebug_LogPrintf("--SetTransYear--");

	memset(szYear, 0x00, sizeof(szYear));	
	CTOS_RTCGet(&SetRTC);
	sprintf(szYear, "%02lu", SetRTC.bYear);
	vdDebug_LogPrintf("szYear[%s]", szYear);

	memset(srTransRec.szTranYear, 0x00, sizeof(srTransRec.szTranYear));
	strcpy(srTransRec.szTranYear, szYear);

	vdDebug_LogPrintf("srTransRec.szTranYear[%s]", srTransRec.szTranYear);
}

// sidumili: For global variable
void vdSetGlobalVariable(void)
{
	vdDebug_LogPrintf("--vdSetGlobalVariable--");
		
	strGBLVar.fGBLvConfirmPrint= (get_env_int("CONFIRMPRINTRPT") > 0 ? TRUE : FALSE);

	vdDebug_LogPrintf("fGBLvConfirmPrint=[%d]", strGBLVar.fGBLvConfirmPrint);
	
}

void vdFormatPANForECR(char* szInPAN, char* szOutPAN){
int inLen1, inLen2;
char szTemp1[6 + 1] = {0};
char szTemp2[4 + 1] = {0};
char szTemp3[10 + 1] = {0};

inLen1 = strlen(szInPAN);

memset(szTemp1, 0x00, sizeof(szTemp1));
memset(szTemp2, 0x00, sizeof(szTemp2));
memset(szTemp3, 0x00, sizeof(szTemp3));

memcpy(szTemp1, &szInPAN[0], 6);
memcpy(szTemp2, &szInPAN[inLen1 - 4], 4);

inLen2 = inLen1 - 10;

vdCTOS_Pad_String(szTemp3, inLen2, '0', POSITION_LEFT);

sprintf(szOutPAN, "%s%s%s", szTemp1, szTemp3, szTemp2);

vdDebug_LogPrintf("szTemp1[%s]|szTemp2[%s]|szTemp3[%s]", szTemp1, szTemp2, szTemp3);
vdDebug_LogPrintf("inLen1[%d]|inLen2[%d]", inLen1, inLen2);
vdDebug_LogPrintf("szOutPAN[%s]", szOutPAN);

}

void vdSetECRResponse(char* szECRResponse)
{
	vdDebug_LogPrintf("vdSetECRResponse :: szECRResponse :: [%s]",szECRResponse);
	strcpy(srTransRec.szRespCode,"");
	strcpy(srTransRec.szECRRespCode, szECRResponse); //make responce code as -1 - for ECR transaction 
}

int inCTOS_SelectHostEx(void) 
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;

    inHostIndex = (short) strCDT.HDTid;
    
     vdDebug_LogPrintf("SATURN inCTOS_SelectHost =[%d]",inHostIndex);
    
     //if (inMultiAP_CheckSubAPStatus() == d_OK)
        //return d_OK;
     
    if ( inHDTRead(inHostIndex) != d_OK)
    {
        vdSetErrorMessage("HOST SELECTION ERR 5");
        return(d_NO);
    } 
    else 
    {
        
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTRead(inCurrencyIdx) != d_OK) 
        {
            vdSetErrorMessage("LOAD CST ERR");
            return(d_NO);
        }


        if ( inCPTRead(inHostIndex) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }

        return (d_OK);
    }
}

int inCTOS_SelectBiller(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1, i=0, inNumRecs=0, Bret=0;
    BYTE key;
    char szHeaderString[50] = "SELECT BILLER";
	char szBillerName[1800+1];
    char szTerm[12][2+1];
	int inTermIndex=0;
	
    int inCPTID[50];
    int inLoop = 0;
    int inIndex=0;

	//char szTrxnListMenu[1800 + 1], szTransFunc[256 + 1];
	
	unsigned char szOutput[30], szBillerCode[4+1];
	short shMinLen;
	short shMaxLen; //#00228 - PASSWORD and SUPER PASSWORD should limit to 4 digit entry//12;
    char szMsg[100];
	
    memset(szBillerName, 0x00, sizeof(szBillerName));

//    CTOS_PrinterPutString("inCTOS_SelectInstallmentTerm");

	vdDebug_LogPrintf("inLogin");

	inNumRecs=inBLRNumRecord();
    
	vdDebug_LogPrintf("inNumRecs: %d", inNumRecs);

	memset(strMultiBLR, 0, sizeof(strMultiBLR));
	inBLRRead(1);

	for (i=0; i < inNumRecs; i++)
	{
		if (strMultiBLR[i].szBillerName[0] != 0)
		{
			inIndex=i;
			strcat((char *)szBillerName, strMultiBLR[i].szBillerName);

			if (strMultiBLR[i+1].szBillerName[0] != 0)
				strcat((char *)szBillerName, (char *)" \n");
		}
	}

    vdDebug_LogPrintf("all szUserName[%s]: length[%d]", szBillerName, strlen(szBillerName));

	if(inNumRecs > 0)
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szBillerName, TRUE);
    else
        key = inIndex+1;   
	
    if (key == 0xFF) 
    {
        //CTOS_LCDTClearDisplay();
        //setLCDPrint(1, DISPLAY_POSITION_CENTER, "INVALID TERMS!!!");
        //vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;
        
		strcpy(srTransRec.szBillerName,strMultiBLR[key-1].szBillerName); //key-1
        strcpy(srTransRec.szBillerCode,strMultiBLR[key-1].szBillerCode); //key-1
        srTransRec.szBillerCode[4]=0x00;
		strcpy(srTransRec.szBillerFee,strMultiBLR[key-1].szBillerFee); //key-1
		srTransRec.szBillerFee[12]=0x00;
        vdDebug_LogPrintf("srTransRec.szBillerName[%s]", srTransRec.szBillerName);
        vdDebug_LogPrintf("srTransRec.szBillerCode[%s]", srTransRec.szBillerCode);
        
        key=d_OK;
    }

	// to print the correct installment terms
	//
	CTOS_LCDTClearDisplay();
	
    return key;
}

int inCTOS_GetOffRefNoEntry(void)
{
    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 12;
    BYTE bShowAttr = 0x02; 
     USHORT inRefNoLen = 12, inMinLen = 12;
    BYTE baPIN[6 + 1];
    BYTE   szRefNo[12+1];
    BYTE   szInvNoBcd[6];
    BYTE bRet;
    int iLens = 6;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];

    BYTE szTitle[25+1];
    BYTE szDisplay[100];
    BYTE szDisMsg[100];


    memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

    memset(szRefNo, 0x00, sizeof(szRefNo));
    memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));

	memset(szDisplay, 0, sizeof(szDisplay));
    strcpy(szDisplay, "1");
	strcat(szDisplay, "|");
    strcat(szDisplay, "12");
	strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Enter RRN:");
	
    while(TRUE)
    {
         inRefNoLen = 12;
         vdDebug_LogPrintf("11bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szRefNo),inRefNoLen);
         //bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
         //bRet = InputString2(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
         
         //bRet = InputStringUI(0x01, 0x02, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT,"", "ENTER INVOICE NO.");
         bRet = InputStringUI(0x01, 0x02, szRefNo, &inRefNoLen, inMinLen, 0, szDisplay);
         
         vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szRefNo),inRefNoLen);
         if (bRet == d_KBD_CANCEL )
         {
              //memset(szDisMsg, 0x00, sizeof(szDisMsg));
              //strcpy(szDisMsg, szTitle);
             // strcat(szDisMsg, "|");
              //strcat(szDisMsg, "USER CANCEL");
             // usCTOSS_LCDDisplay(szDisMsg);
             // CTOS_Beep();
                //strcpy(szDisMsg, "|PROCESSING...");
                //usCTOSS_LCDDisplay(szDisMsg);
              
	          vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
              //CTOS_Delay(1500);
              return (d_EDM_USER_CANCEL);
         }
         else if (bRet == d_KBD_ENTER ) 
         {
              if(strlen(szRefNo) == 12)
              {
                   //strcpy(srTransRec.szRRN, szRefNo);
                   memcpy(srTransRec.szPAN, szRefNo, strlen(szRefNo));
				   memcpy(srTransRec.szTrack2Data, szRefNo, strlen(szRefNo));
				   vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
				   CTOS_Delay(300);
                   break;
              }
              else
              {
              	   vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
                   CTOS_Beep();
                   CTOS_Delay(1500);
                   CTOS_Beep();
              }
         }
         else
         {
         	  vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
              CTOS_Beep();
              CTOS_Delay(1500);
              CTOS_Beep();
         }
    }
	
   //  strcpy(szDisMsg, "|PROCESSING...");
     //usCTOSS_LCDDisplay(szDisMsg);
     //vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS); // sidumili: remove to fix crash on preauth void after entry of RRN
     return ST_SUCCESS;
}

#if 0
int inCTOS_GetSubscriberNo(void)
{
    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 12;
    BYTE bShowAttr = 0x02; 
    USHORT inRefNoLen = 19, inMinLen = 6;
    BYTE baPIN[6+1];
    BYTE szRefNo[19+1];
    BYTE szInvNoBcd[6];
    BYTE bRet;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];

    BYTE szTitle[25+1];
    BYTE szDisplay[100];
    BYTE szDisMsg[100];

    char szMinLen[2+1];
    char szMaxLen[2+1];
	
    memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

    memset(szRefNo, 0x00, sizeof(szRefNo));
    memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));

	memset(szMinLen, 0, sizeof(szMinLen));
    sprintf(szMinLen, "%d", 6);
	memset(szMaxLen, 0, sizeof(szMaxLen));
	sprintf(szMaxLen, "%d", 19);
	
	memset(szDisplay, 0, sizeof(szDisplay));
    strcpy(szDisplay, szMinLen);
	strcat(szDisplay, "|");
    strcat(szDisplay, szMaxLen);
	strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Enter Subscriber No:");

	vdDebug_LogPrintf("inCTOS_GetSubscriberNo szDisplay:%s", szDisplay);
	
    while(TRUE)
    {
         inRefNoLen = 0;
         vdDebug_LogPrintf("11bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szRefNo),inRefNoLen);
         bRet = InputStringUI(0x01, 0x02, szRefNo, &inRefNoLen, inMinLen, 0, szDisplay);
         
         vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szRefNo),inRefNoLen);
         if (bRet == d_KBD_CANCEL )
         {
	          vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
              return (d_EDM_USER_CANCEL);
         }
         else if (bRet == d_KBD_ENTER ) 
         {
              if(strlen(szRefNo) >= 6)
              {
                   strcpy(srTransRec.szSubscriberNo, szRefNo);
				   vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
				   CTOS_Delay(300);
                   break;
              }
              else
              {
              	   vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
                   CTOS_Beep();
                   CTOS_Delay(1500);
                   CTOS_Beep();
              }
         }
         else
         {
         	  vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
              CTOS_Beep();
              CTOS_Delay(1500);
              CTOS_Beep();
         }
    }
    return ST_SUCCESS;
}
#else



//copied from LibInput ctosapi.c
USHORT shCTOS_InputGetNum(USHORT usX, USHORT usY, BYTE *szPrompt, USHORT usMinLen, USHORT usMaxLen, BYTE *baInput, ULONG *ulInput, USHORT usTimeOutMS, BYTE bIgnoreEnter, USHORT usAlignType)
{
	int inBeginLine=0;
	unsigned char ucKey;
	int iDisplayIdx = 0;
	BYTE szDisplayStr[MAX_CHAR_PER_LINE+1];
	int inInputY = 0;

    /*memset(szDisplayStr, 0x00, sizeof(szDisplayStr));
    memset(szDisplayStr, 0x20, MAX_CHAR_PER_LINE);
    memcpy(szDisplayStr, szPrompt, strlen(szPrompt));
	CTOS_LCDTGotoXY(usX, usY);
	CTOS_LCDTClear2EOL();
	CTOS_LCDTPrintXY(usX, usY, szDisplayStr);*/
		
	CTOS_KBDBufFlush();//cleare key buffer
	CTOS_TimeOutSet(TIMER_ID_1, usTimeOutMS);
	while (1)
	{
		CTOS_KBDInKey(&ucKey);
		if (ucKey)
		{
			CTOS_KBDGet(&ucKey);
			
			if (ucKey == d_KBD_CANCEL)
			{
				return ucKey; 
			} 
			
			//Done input and output a string // 
			if (ucKey == d_KBD_ENTER)
			{ 
				*ulInput = strlen(baInput); 
				if (*ulInput >= usMinLen)
					return d_OK; 
				else
				{
					if (bIgnoreEnter != 0)
						return d_OK; 
				}
			}
			else if (ucKey == d_KBD_CLEAR)
			{ //Backspace // 
				if (iDisplayIdx > 0)
				{
					iDisplayIdx--; 
					baInput[iDisplayIdx] = 0x00; 
					*ulInput = strlen(baInput);
									
				    /*memset(szDisplayStr, 0x00, sizeof(szDisplayStr));
				    memset(szDisplayStr, 0x20, MAX_CHAR_PER_LINE);
					memcpy(szDisplayStr, szPrompt, strlen(szPrompt));
					inInputY = MAX_CHAR_PER_LINE-(strlen(baInput)+strlen(szPrompt))*2+strlen(szPrompt);
					if (((inInputY % 2 == 0) && (inInputY < 31) && strstr(szPrompt, "."))	// 31 is the leftmost position viewable by user, for prompt with ".", e.g. "Ref No.:"
					|| (strstr(szPrompt, "Ref:")))	// needed for "EDDA Ref:" also to show max 12digits of input without overlapping the prompt wording
						inInputY+=2;
				    strcpy(&szDisplayStr[inInputY], baInput);//strcpy(&szDisplayStr[MAX_CHAR_PER_LINE-(strlen(baInput)+strlen(szPrompt))*2+strlen(szPrompt)], baInput);
					CTOS_LCDTGotoXY(usX,usY);
					CTOS_LCDTClear2EOL();
					CTOS_LCDTPrintXY(usX, usY, szDisplayStr);*/
					clearLine(usY);
					if (usAlignType == DISPLAY_POSITION_CENTER)
						CTOS_LCDTPrintAligned(usY, baInput, d_LCD_ALIGNCENTER);
					else
						CTOS_LCDTPrintAligned(usY, baInput, d_LCD_ALIGNLEFT);
				}
			}
			
			if (ucKey >= d_KBD_0 && ucKey <= d_KBD_9)
			{
				if (iDisplayIdx < usMaxLen)
				{
					baInput[iDisplayIdx] = ucKey;
					*ulInput = strlen(baInput);
				
				    /*memset(szDisplayStr, 0x00, sizeof(szDisplayStr));
				    memset(szDisplayStr, 0x20, MAX_CHAR_PER_LINE);
				    memcpy(szDisplayStr, szPrompt, strlen(szPrompt));
					inInputY = MAX_CHAR_PER_LINE-(strlen(baInput)+strlen(szPrompt))*2+strlen(szPrompt);
					if (((inInputY % 2 == 0) && (inInputY < 31) && strstr(szPrompt, "."))	// 31 is the leftmost position viewable by user, for prompt with ".", e.g. "Ref No.:"
					|| (strstr(szPrompt, "Ref:")))	// needed for "EDDA Ref:" also to show max 12digits of input without overlapping the prompt wording
						inInputY+=2;
				    strcpy(&szDisplayStr[inInputY], baInput);//strcpy(&szDisplayStr[MAX_CHAR_PER_LINE-(strlen(baInput)+strlen(szPrompt))*2+strlen(szPrompt)], baInput);              
					CTOS_LCDTGotoXY(usX,usY);
					CTOS_LCDTClear2EOL();
					CTOS_LCDTPrintXY(usX, usY, szDisplayStr);*/					
					if (usAlignType == DISPLAY_POSITION_CENTER)
						CTOS_LCDTPrintAligned(usY, baInput, d_LCD_ALIGNCENTER);
					else
						CTOS_LCDTPrintAligned(usY, baInput, d_LCD_ALIGNLEFT);
				
					iDisplayIdx++;
				}
			}
		}

		if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		{	   
			return 0xFF;
		}
	}

}

int inCTOS_GetSubscriberNo(void)
{
    #define d_INPUT_TIMEOUT     100*90

    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 12;
    BYTE bShowAttr = 0x02; 
    USHORT inRefNoLen = 19, inMinLen = 6;
    BYTE baPIN[6+1];
    BYTE szRefNo[19+1];
    BYTE szInvNoBcd[6];
    BYTE bRet;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];

    BYTE szTitle[25+1];
    BYTE szDisplay[100];
    BYTE szDisMsg[100];

    char szMinLen[2+1];
    char szMaxLen[2+1];

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
                
    memset(szRefNo, 0x00, sizeof(szRefNo));
    memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));

	memset(szMinLen, 0, sizeof(szMinLen));
    sprintf(szMinLen, "%d", 6);
	memset(szMaxLen, 0, sizeof(szMaxLen));
	sprintf(szMaxLen, "%d", 19);
	
	memset(szDisplay, 0, sizeof(szDisplay));
    strcpy(szDisplay, szMinLen);
	strcat(szDisplay, "|");
    strcat(szDisplay, szMaxLen);
	strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Enter Subscriber No:");
CTOS_LCDTPrintXY(1, 2, "Enter Subscriber No:");
	vdDebug_LogPrintf("inCTOS_GetSubscriberNo szDisplay:%s", szDisplay);
	
    while(TRUE)
    {
         inRefNoLen = 0;
         memset(szRefNo, 0, sizeof(szRefNo));
         bRet = shCTOS_InputGetNum(0x01,0x03, "", 6, 19, szRefNo, &inRefNoLen, d_INPUT_TIMEOUT, FALSE, DISPLAY_POSITION_CENTER);
         
         vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szRefNo),inRefNoLen);
         if (bRet == d_KBD_CANCEL )
         {
	          vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
              return (d_EDM_USER_CANCEL);
         }
         else if (bRet == 0xFF)
         {
             vdSetErrorMessage("TIMEOUT");
             return (d_EDM_USER_CANCEL);
         }
         else if (bRet == d_OK ) 
         {
              if(strlen(szRefNo) >= 6)
              {
                   strcpy(srTransRec.szSubscriberNo, szRefNo);
                    vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
                    CTOS_Delay(300);
                   break;
              }
              else
              {
              	   vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
                   CTOS_Beep();
                   CTOS_Delay(1500);
                   CTOS_Beep();
              }
         }
         else
         {
         	  vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
              CTOS_Beep();
              CTOS_Delay(1500);
              CTOS_Beep();
         }
    }
    return ST_SUCCESS;
}
#endif

int inCTOS_GetAccountNo(void)
{
    BYTE key;
    USHORT usX =1, usY = 6;
    BYTE baString[100+1];
    USHORT iStrLen = 12;
    BYTE bShowAttr = 0x02; 
    USHORT inRefNoLen = 19, inMinLen = 4;
    BYTE baPIN[6+1];
    BYTE szAccNo[19+1];
    BYTE szInvNoBcd[6];
    BYTE bRet;
    int  inResult;
    char szBcd[INVOICE_BCD_SIZE+1];

    BYTE szTitle[25+1];
    BYTE szDisplay[100];
    BYTE szDisMsg[100];

    char szMinLen[2+1];
    char szMaxLen[2+1];

	if(srTransRec.inFundTransferAccountType != SA_OTHERS && srTransRec.inFundTransferAccountType != CA_OTHERS)
		return d_OK;
	
    memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

    memset(szAccNo, 0x00, sizeof(szAccNo));
    memset(szInvNoBcd, 0x00, sizeof(szInvNoBcd));

	memset(szMinLen, 0, sizeof(szMinLen));
    sprintf(szMinLen, "%d", 12);
	memset(szMaxLen, 0, sizeof(szMaxLen));
	sprintf(szMaxLen, "%d", 19);
	
	memset(szDisplay, 0, sizeof(szDisplay));
    strcpy(szDisplay, szMinLen);
	strcat(szDisplay, "|");
    strcat(szDisplay, szMaxLen);
	strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Enter Account No To:");

	vdDebug_LogPrintf("inCTOS_GetAccountNo szDisplay:%s", szDisplay);
	
    while(TRUE)
    {
         inRefNoLen = 0;
         vdDebug_LogPrintf("11bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szAccNo),inRefNoLen);
         bRet = InputStringUI(0x01, 0x02, szAccNo, &inRefNoLen, inMinLen, 0, szDisplay);
         
         vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]",bRet,atoi(szAccNo),inRefNoLen);
         if (bRet == d_KBD_CANCEL )
         {
	          vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
              return (d_EDM_USER_CANCEL);
         }
         else if (bRet == d_KBD_ENTER ) 
         {
              if(strlen(szAccNo) >= 12)
              {
                   strcpy(srTransRec.szAccountNoTo, szAccNo);
				   vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
				   CTOS_Delay(300);
                   break;
              }
              else
              {
              	   vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
                   CTOS_Beep();
                   CTOS_Delay(1500);
                   CTOS_Beep();
              }
         }
         else
         {
         	  vdDisplayMessageBox(1, 8, "", "INVALID INPUT", "", MSG_TYPE_ERROR);
              CTOS_Beep();
              CTOS_Delay(1500);
              CTOS_Beep();
         }
    }
    return ST_SUCCESS;
}

int file_exist (char *filename)
{
  struct stat buffer;
  return (stat (filename, &buffer) == 0);
}