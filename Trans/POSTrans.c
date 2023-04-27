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
#include "..\Includes\PoSSetting.h"
#include "..\Aptrans\MultiShareCOM.h"

extern int inReversalType;

extern BOOL fPinAlreadyEntered;

//1030
extern BOOL fBinVerInitiatedSALE;
//1030

extern BOOL fGPRSConnectOK;

int inFallbackToMSR = FAIL;
static char szBaseAmount[6+1];

BOOL fECRTxnFlg = 0;
extern BOOL fAdviceTras;

extern int isPredial;

// BDO: Added flag to supress merc password during auto Log-On -- jzg
extern BOOL fAutoLogOn;

/* BDO: For ECR, variable -- sidumili */
BOOL fECRBuildSendOK; /* For ECR -- sidumili */
/* BDO: For ECR, variable -- sidumili */

BOOL fTimeOutFlag = FALSE; /*For ECR timeout flag --sidumili*/

//Display Issuer logo: Clear the display first, then re-display trans title -- jzg

extern BOOL fAUTOManualSettle;
BOOL fPINTrxnRetry = FALSE;
extern BOOL fRePrintFlag;


extern void displayAppbmpDataEx(unsigned int x,unsigned int y, const char *path);
void vdSetECRTransactionFlg(BOOL flg)
{
    fECRTxnFlg = flg;
}

BOOL fGetECRTransactionFlg(void)
{
    return fECRTxnFlg;
}

extern int isdigit ( int c );
extern BOOL fConnTerminatedDisplayed;


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
        	fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
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
        //usRet = shCTOS_GetExpDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, d_INPUT_TIMEOUT);
        usRet = shCTOS_GetExpDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
        if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);
        memset(szMonth, 0x00, sizeof(szMonth));
        memcpy(szMonth, baBuf, 2);
        if(atol(szMonth) > 12 || 0 == atol(szMonth))
        {
            baBuf[0] = 0x00;
            clearLine(8);
            vdDisplayErrorMsg(1, 8, "INVALID FORMAT");
            clearLine(8);
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
        //usRet = shCTOS_GetNum(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 0, d_INPUT_TIMEOUT);
        usRet = shCTOS_GetNum(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 0, inGetIdleTimeOut(FALSE));
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

    CTOS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
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

    CTOS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
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

    if(srTransRec.byOffline == CN_TRUE)
        return;
    CTOS_RTCGet(&SetRTC);
    vdDebug_LogPrintf("sys year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay,SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);

    vdDebug_LogPrintf("year[%02X],time[%02x:%02x:%02x]date[%02x][%02x]",SetRTC.bYear,srTransRec.szTime[0],srTransRec.szTime[1],srTransRec.szTime[2],srTransRec.szDate[0],srTransRec.szDate[1]);

    memset(szDate,0,sizeof(szDate));
    memset(szTime,0,sizeof(szTime));

    wub_hex_2_str(srTransRec.szDate, szDate, 2);
    wub_hex_2_str(srTransRec.szTime, szTime, 3);
    if((strlen(szDate)<=0) ||(strlen(szTime)<=0))
        return;

#if 0
    sprintf(szBuf,"%02x",(unsigned int)atol(szDate)%100);
    wub_str_2_hex(szBuf, &(SetRTC.bDay), 2);

    sprintf(szBuf,"%02x",(unsigned int)atol(szDate)/100);
    wub_str_2_hex(szBuf, &(SetRTC.bMonth), 2);

    sprintf(szBuf,"%02x",(unsigned int)atol(szTime)/10000);
    wub_str_2_hex(szBuf, &(SetRTC.bHour), 2);

    sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%10000/100);
    wub_str_2_hex(szBuf, &(SetRTC.bMinute), 2);

    sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%100);
    wub_str_2_hex(szBuf, &(SetRTC.bSecond), 2);
#else
	vdDebug_LogPrintf("srTransRec.fSetDate: %d", srTransRec.fSetDate);
	vdDebug_LogPrintf("srTransRec.fSetTime: %d", srTransRec.fSetTime);
	
	if(srTransRec.fSetDate == TRUE)
	{
		sprintf(szBuf,"%02x",(unsigned int)atol(szDate)%100);
		wub_str_2_hex(szBuf, &(SetRTC.bDay), 2);
		
		sprintf(szBuf,"%02x",(unsigned int)atol(szDate)/100);
		wub_str_2_hex(szBuf, &(SetRTC.bMonth), 2);
	}
	
	if(srTransRec.fSetTime == TRUE)
	{
		sprintf(szBuf,"%02x",(unsigned int)atol(szTime)/10000);
		wub_str_2_hex(szBuf, &(SetRTC.bHour), 2);
		
		sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%10000/100);
		wub_str_2_hex(szBuf, &(SetRTC.bMinute), 2);
		
		sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%100);
		wub_str_2_hex(szBuf, &(SetRTC.bSecond), 2);
	}
#endif

    //Host date & time synchronization fix -- jzg 
    CTOS_RTCSet(&SetRTC);
    
    vdDebug_LogPrintf("set year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay,SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);

    return;
}

int file_exist (char *filename)
{
  struct stat buffer;
  return (stat (filename, &buffer) == 0);
}

void vdCTOS_TxnsBeginInit(void)
{
    BYTE bEntryMode = 0;
	int inRet = d_NO;


	fPinAlreadyEntered = FALSE;
	fPINTrxnRetry = FALSE;

    /*albert - do not prompt swipe/insert/etc... card*/
    if(srTransRec.byEntryMode == CARD_ENTRY_MSR || srTransRec.byEntryMode == CARD_ENTRY_ICC)
	    bEntryMode = srTransRec.byEntryMode;
    if(d_OK == inCTOS_ValidFirstIdleKey())
        bEntryMode = srTransRec.byEntryMode;

    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
	//gcitra
	//inCLearTablesStructure();
	//gcitra
	inFallbackToMSR = FAIL;

    if(0 != bEntryMode)
        srTransRec.byEntryMode = bEntryMode;

    vdSetErrorMessage("");
	vdCTOSS_SetWaveTransType(0);
	vdSetInit_Connect(0);

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
	int inReturn = 0;

    vdDebug_LogPrintf("vdCTOS_TransEndReset ECR?[%d]", fGetECRTransactionFlg());
    CTOS_LCDTClearDisplay();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
    }

    if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
        vdRemoveCard();
    else
        CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

    if(!fGetECRTransactionFlg())
    {
        memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    }

    CTOS_KBDBufFlush();

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    vdSetErrorMessage("");
	vdCTOSS_SetWaveTransType(0);

	//inMultiAP_Database_BatchDelete();
	
	//gcitra
	inCLearTablesStructure();
	//gcitra
	inFallbackToMSR = FAIL;

	//06112015
	fCommAlreadyOPen = FALSE;		
	isPredial=0;
	//06112015

	fAUTOManualSettle = FALSE;
	//inReversalType =0;
    fAdviceTras = VS_FALSE;


	fPinAlreadyEntered = FALSE;
	fPINTrxnRetry = FALSE;
	vdSetECRTransactionFlg(0);
	//vdCTOSS_MainAPMemoryRecover();
	put_env_int("BINROUTE",0);
	fConnTerminatedDisplayed = FALSE;

#if 0
	if(strCPT.inCommunicationMode == ETHERNET_MODE)
		{
			 // if(srCommFuncPoint.inDisconnect)
			DWORD dwStatus;
			dwStatus = 0;
			short shRet = 0;
			
			//shRet = CTOS_EthernetStatus(&dwStatus);
				 
			inReturn = CTOS_EthernetOpen();
			
			vdDebug_LogPrintf("CTOS_EthernetOpen -- [%d] ----- CTOS_EthernetStatus [%d]", inReturn, shRet);

			//if (inReturn != 0)
				
		}
#endif

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

short shCTOS_SetMagstripCardTrackData(BYTE* baTk1Buf, USHORT usTk1Len, BYTE* baTk2Buf, USHORT usTk2Len, BYTE* baTk3Buf, USHORT usTk3Len) //Invalid card reading fix -- jzg
{
	short shRet = d_OK;

	memcpy(srTransRec.szTrack1Data, &baTk1Buf[1], (usTk1Len -3));
	srTransRec.usTrack1Len = (usTk1Len - 3);// REMOVE %, ? AND LRC

	memcpy(srTransRec.szTrack2Data, &baTk2Buf[1], (usTk2Len - 3));
	srTransRec.usTrack2Len = (usTk2Len - 3);// REMOVE %, ? AND LRC

	memcpy(srTransRec.szTrack3Data, baTk3Buf, usTk3Len);
	srTransRec.usTrack3Len = usTk3Len;

	vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);

	//if (usTk1Len >= 20)
	//	vdAnalysisTrack1(&baTk1Buf[1], usTk1Len); 

	//if (usTk2Len >= 20)
	//	shRet = shAnalysisTrack2(&baTk2Buf[1], usTk2Len); //Invalid card reading fix -- jzg
	
	//remove later -- jzg
	vdDebug_LogPrintf("JEFF::TK1 = [%d][%s]", usTk1Len, baTk1Buf);
	vdAnalysisTrack1(&baTk1Buf[1], usTk1Len); 

	//remove later -- jzg
	vdDebug_LogPrintf("JEFF::TK2 = [%d][%s]", usTk2Len, baTk2Buf);
	if (usTk2Len >= 20)
	{
		shRet = shAnalysisTrack2(&baTk2Buf[1], usTk2Len); //Invalid card reading fix -- jzg
		if(shRet != d_OK)
			return INVALID_CARD;
	}
	else
	{
		vdDebug_LogPrintf("JEFF::T2 INVALID! [%d]", usTk2Len);
		return INVALID_CARD;
	}

	return shRet;
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
       //vdSetErrorMessage("TRANS NOT ALLOWED");
       CTOS_LCDTClearDisplay();
       vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}


int inCTOS_CheckTranAllowd(void)
{
    int inEnable = 0;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    inEnable = strPIT.fTxnEnable;
    vdDebug_LogPrintf("inCTOS_CheckTranAllowd: [%d]", inEnable);

    if(0 == inEnable)
    {
       //vdSetErrorMessage("TRANS NOT ALLOWED");
       CTOS_LCDTClearDisplay();
       vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

int inCTOS_CheckMustSettle(void)
{
    int inEnable = 0;
	BYTE szCurrDate[8] = {0};
	CTOS_RTC SetRTC;
    STRUCT_FILE_SETTING strFile;
    ACCUM_REC srAccumRec;

	vdDebug_LogPrintf("REINER :: fChangeDateMustSettle[%d]",strTCT.fChangeDateMustSettle);
	if(strTCT.fChangeDateMustSettle == TRUE)
	{
		memset(szCurrDate, 0x00, sizeof(szCurrDate));
		CTOS_RTCGet(&SetRTC);
		sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
		vdDebug_LogPrintf("CURRENT DATE: [%s] :: SETTLE DATE: [%s]",szCurrDate,strMMT[0].szSettleDate); 

		if ((strMMT[0].fMMTEnable == TRUE) && (wub_str_2_long(szCurrDate) >= wub_str_2_long(strMMT[0].szSettleDate)))
		{
			
			srTransRec.MITid=strMMT[0].MITid; 								 
			vdCTOS_GetAccumName(&strFile, &srAccumRec);
			if ((inMyFile_CheckFileExist(strFile.szFileName)) > 0)
			{
				 strMMT[0].fMustSettFlag = 1;
				 inMMTSave(strMMT[0].MMTid);
			}
		}
	}
	
    inEnable = strMMT[0].fMustSettFlag;

    if(1 == inEnable)
    {
       vdSetErrorMessage("MUST SETTLE");
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

void vdCTOS_FormatPAN(char *szFmt,char* szInPAN,char* szOutPAN)
{
    char szCurrentPAN[20];
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int inFmtPANSize;

    inFmtPANSize = strlen(szFmt);
    if (strlen(szFmt) == 0)
    {
      strncpy(szOutPAN,szInPAN,inFmtPANSize);
      return;
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

//0721
void vdCTOS_FormatPAN2(char *szFmt,char* szInPAN,char* szOutPAN)
{
    char szCurrentPAN[20];
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int inFmtPANSize;
		int inPANless4;

		int i=0;
    
    inFmtPANSize = strlen(szInPAN);
		
		inPANless4 = inFmtPANSize - 4;
		

		
    if (strlen(szFmt) == 0) 
    {
      strncpy(szOutPAN,szInPAN,inFmtPANSize);
      return;
    }

    memset(szCurrentPAN, 0x00, sizeof(szCurrentPAN));
    memcpy(szCurrentPAN,szInPAN,strlen(szInPAN));

    while(i<=inFmtPANSize)
    {
      
		  if (i >= inPANless4){

	 
        if (szFmt[inFmtIdx] == ' '){
          szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
          inFmtIdx++;
				}else{		
					szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; ; 
					inFmtIdx++;
					inPANIdx++;
					i++;
				}
				continue;

		  }
				
       
      if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
      {
          szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; 
          inFmtIdx++;
          inPANIdx++;
					i++;
      }
      else if (szFmt[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' ||szFmt[inFmtIdx] == '*')   
      {
                     
          memcpy(&szOutPAN[inFmtIdx],&szFmt[inFmtIdx],1);
          inFmtIdx++;
          inPANIdx++;
					i++;
      }
      else if (!isdigit(szFmt[inFmtIdx]))
      {
          szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
          inFmtIdx++;
      }
    }

    //while(szCurrentPAN[inPANIdx]!= 0x00)
    //{
    // szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; 
    // inFmtIdx++;
    // inPANIdx++;
    //}

    return;
}


//0721


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

    inFmtAmtSize = strlen(szFmt);
    if (strlen(szFmt) == 0)
    {
      strncpy(szOutAmt,szInAmt,inFmtAmtSize);
      return;
    }

    inFmtIdx = 0;
    while(szFmt[inFmtIdx]!= 0x00)
    {
        if(szFmt[inFmtIdx] == 'n')
            inPadNum ++;

        if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
            inTotaActNum ++;

        inFmtIdx ++;
    }

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

    szOutAmt[inFmtIdx] = 0x00;

    return;
}


int inGetIssuerRecord(int inIssuerNumber)
{
    int inRec = 1;
    do
    {
        //if (inIITRead(inRec) != d_OK)
        if (inIITRead(inIssuerNumber) != d_OK)
        {
            return(d_NO);
        }
        inRec++;
    } while (inIssuerNumber != strIIT.inIssuerNumber);

    return(d_OK);
}


#if 0
int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine)
{
    char szStr[50 + 1];
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE szTemp1[30+1];
	//gcitra
    unsigned char key;


	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);


   // CTOS_LCDTClearDisplay();
	//gcitra

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

    memset(szTemp1,0,sizeof(szTemp1));
    vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);

    if(0>= usCardTypeLine || 0 >= usPANLine)
    {
        usCardTypeLine = 3;
        usPANLine = 4;
    }

	//gcitra

    CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);
  	//setLCDPrint27(usCardTypeLine, DISPLAY_POSITION_RIGHT, szStr);
    CTOS_LCDTPrintXY(1, usPANLine, szTemp1);



    //gcitra-add PAN confirmation
	if (strTCT.fConfirmPAN){
		key=WaitKey(15);

		if(key != d_KBD_ENTER)
			return d_NO;
	}
	//gcitra

    return(d_OK);
}
#endif
int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine)
{
    char szStr[50 + 1]; 
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE szTemp1[30+1];

		BYTE szPAN1[20+1];
		BYTE szPAN2[20+1];
		int inRemaining=0;

		
    BYTE szTitle[16+1];


	//gcitra-0806
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		   return d_OK;
	//gcitra-0806	



 

	CTOS_LCDTClearDisplay();

	
	//1102
	if (srTransRec.byTransType != BIN_VER){
		vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
		if (strCDT.HDTid != 5) 
			displayAppbmpDataEx(1,1, strIIT.szIssuerLogo);
	}
	//1102




  memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
 




#if 1

    memset(szStr,0x00,sizeof(szStr));
    memset(EMVtagVal,0x00,sizeof(EMVtagVal));

	//Display Issuer logo: Clear the display first, then re-display trans title - start -- jzg
	//CTOS_LCDTClearDisplay(); 
	//DispTransTitle(srTransRec.byTransType);
	
	setLCDPrint27(1, DISPLAY_POSITION_RIGHT, strCDT.szCardLabel);
	setLCDPrint27(2, DISPLAY_POSITION_RIGHT, szTitle);
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg
  
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        
        if(EMVtagLen > 0)
        {
            sprintf(szStr, "%s", EMVtagVal);
            vdDebug_LogPrintf("Card label(str): %s",szStr);       
        }
    }

/*
    if(0 == strlen(szStr))
    {
        //1102
        if (srTransRec.byTransType != BIN_VER){
        	sprintf(szStr, "%s", strCDT.szCardLabel);
        	vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
        }
		//1102
    }
*/
    strcpy(szStr,"PAN:");
    memset(szTemp1,0,sizeof(szTemp1));
	if (strTCT.fMaskPanDisplay == TRUE){
    	//vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
    	strcpy(szTemp1, srTransRec.szPAN);	
		cardMasking(szTemp1, 3);		
	}else{ 	
		vdCTOS_FormatPAN2(strTCT.DisplayPANFormat, srTransRec.szPAN, szTemp1);
	}	


//	displayAppbmpDataEx(140, 35, "mc.bmp"); 

    if(0>= usCardTypeLine || 0 >= usPANLine)
    {
        usCardTypeLine = 5;
        usPANLine = 6;
    }
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg
    
    CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);

//0721
    if (strlen(szTemp1) > 20){
			memset(szPAN1, 0x00, sizeof(szPAN1));
			memset(szPAN2, 0x00, sizeof(szPAN2));
			inRemaining = strlen(szTemp1) - 20;

			memcpy(szPAN1, szTemp1, 20);
			memcpy(szPAN2, &szTemp1[20], inRemaining);
			
    	//CTOS_LCDTPrintXY(1, usPANLine, szPAN1);
			
			setLCDPrint27(7, DISPLAY_POSITION_RIGHT, szPAN1);
    	CTOS_LCDTPrintXY(1, 8, szPAN2);

    }else
    	//CTOS_LCDTPrintXY(1, usPANLine, szTemp1);
    	
		setLCDPrint27(7, DISPLAY_POSITION_RIGHT, szTemp1);
//0721
#endif    
    return(d_OK);
}

short inCTOS_LoadCDTIndex(void)
{
	signed int inRetVal;
    short   shStatus;
    int  i=0, selectedRDTIndex;
    BYTE    shSuitableRDTIndex[10];
    int inIssuer = 0;
    int inNumberOfMatches = 0;
    int inRecNumArray[4];
    char szChoiceMsg[20 + 1];
    int    inCardLen, j;
    int inMaxCDTid = 0;
    int inFindRecordNum=0;


    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 8, "Checking Card... ");

    inCDTReadMulti(srTransRec.szPAN, &inFindRecordNum);
    if(inFindRecordNum == 0)
    {
    	vdDebug_LogPrintf("Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    for(j=0;j<inFindRecordNum;j++)
    {
    	if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) && !(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL))
    	{
        	if ((strMCDT[j].inType == DEBIT_CARD) || (strMCDT[j].inType == EBT_CARD) || (strMCDT[j].IITid != inIssuer))
      		{
            	if (strMCDT[j].inType != DEBIT_CARD && strMCDT[j].inType != EBT_CARD)
        			inIssuer = strMCDT[j].IITid;

        		inRecNumArray[inNumberOfMatches++] = strMCDT[j].CDTid;

        		if(inNumberOfMatches > 0)
              		break;

                if (inNumberOfMatches > 1)
                    szChoiceMsg[strlen(szChoiceMsg)] = '~';

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
        vdDisplayErrorMsg(1, 8, "shCTOS_EMVAppSelectedProcess");
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
    return d_OK;
}


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

    //CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, "CARD NO: ");

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
    return d_OK;
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
    
    short shReturn = d_OK; //Invalid card reading fix -- jzg
    
 DebugAddSTR("inCTOS_GetCardFields","Processing...",20);

	//1030
		//if (inMultiAP_CheckSubAPStatus() == d_OK)
		if ((inMultiAP_CheckSubAPStatus() == d_OK) && (fBinVerInitiatedSALE == VS_FALSE))
	//1030
        return d_OK;




 SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
        //gcitra-0728
        //inCTOS_DisplayIdleBMP();
        //gcitra-0728
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
    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

    CTOS_LCDTClearDisplay();
	//vdMyEZLib_Printf("srTransRec.byEntryMode: (%02x)", srTransRec.byEntryMode);
    if(srTransRec.byEntryMode != CARD_ENTRY_MSR && srTransRec.byEntryMode != CARD_ENTRY_ICC)
    {
        vdDispTransTitle(srTransRec.byTransType);

		vdCTOS_DisplayReloadCardType(); // BDO: For reload trxn, display source/destination card label
		
        CTOS_LCDTPrintXY(1, 3, "SWIPE/ENTER/INSERT");
        CTOS_LCDTPrintXY(1, 4, "TAP/CANCEL:");
    }
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return READ_CARD_TIMEOUT ;

        CTOS_KBDInKey(&byKeyBuf);

//gcitra-removed - remove part where card entry is allowed during IDLE mode

        //if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        if (byKeyBuf)
        {

 			//gcitra-0728
			if (byKeyBuf == d_KBD_CANCEL){
				CTOS_KBDBufFlush();
				return USER_ABORT;
			}
			//gcitra-0728

            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

            //if(d_OK == inCTOS_ValidFirstIdleKey())
            //    srTransRec.szPAN[0] = chGetFirstIdleKey();
            //gcitra

            //vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                //gcitra - remove part where card entry is allowed during IDLE mode
				//vdSetFirstIdleKey(0x00);
				//gcitra
                CTOS_KBDBufFlush();
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
		//gcitra
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
		//gcitra
        {
            //Invalid card reading fix - start -- jzg
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                    CTOS_KBDBufFlush();
                    vdDisplayErrorMsg(1, 8, "READ CARD ERROR"); 
                    return INVALID_CARD;
            }
            //Invalid card reading fix - end -- jzg

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
    return d_OK;
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

    short shReturn = d_OK; //Invalid card reading fix -- jzg        
        
 	vdDebug_LogPrintf("inCTOS_WaveGetCardFields.........");

     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	CTOS_LCDTClearDisplay();
	
 SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
        //gcitra-0728
        inCTOS_DisplayIdleBMP();
        //gcitra-0728
    }

	memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
	memset(szOtherAmt,0x00,sizeof(szOtherAmt));
	memset(szTransType,0x00,sizeof(szTransType));
	memset(szCatgCode,0x00,sizeof(szCatgCode));
	memset(szCurrCode,0x00,sizeof(szCurrCode));
	memset(temp,0x00,sizeof(temp));
	memset(szTotalAmount,0x00,sizeof(szTotalAmount));
//gcitra
#if 0
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
	CTOS_LCDTPrintXY(1, 4, temp);
#endif

	if (srTransRec.byTransType == REFUND)
		szTransType[0] = 0x20;

	ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
//	ulAPRtn = EMVCL_InitTransaction(atol(szTotalAmount));
	if(ulAPRtn != d_EMVCL_NO_ERROR)
	{
		vdSetErrorMessage("CTLS InitTrans Fail!");
		return d_NO;
	}

    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

	vdCTOS_DisplayReloadCardType(); // BDO: For reload trxn, display source/destination card label
	
	//gcitra-0728
	CTOS_LCDTPrintXY(1, 3, "SWIPE/ENTER/INSERT");
	CTOS_LCDTPrintXY(1, 4, "TAP/CANCEL:");
	//gcitra-0728
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
        	inCTOSS_CLMCancelTransaction();
            return READ_CARD_TIMEOUT ;
        }

        CTOS_KBDInKey(&byKeyBuf);
    	//gcitra
        //if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        if (byKeyBuf)
		//gcitra
        {
        	inCTOSS_CLMCancelTransaction();
            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
			//gcitra
            //if(d_OK == inCTOS_ValidFirstIdleKey())
            //    srTransRec.szPAN[0] = chGetFirstIdleKey();
			//gcitra

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
            //Invalid card reading fix - start -- jzg
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                CTOS_KBDBufFlush();
                vdDisplayErrorMsg(1, 8, "READ CARD ERROR"); 
                return INVALID_CARD;
            }
            //Invalid card reading fix - end -- jzg

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
    return d_OK;
}


int inCTOS_PreConnect(void)
{
    int inRetVal;
	vdDebug_LogPrintf("enter inCTOS_PreConnect");
	if (chGetInit_Connect() == TRUE)
		{
			vdDebug_LogPrintf("inCTOS_PreConnect  --- A");
			return(d_OK);
		}
#if 0	
    if (strTCT.fSingleComms){	
		if(inCPTRead(1) != d_OK)
		{
			vdSetErrorMessage("LOAD CPT ERR");
			return(d_NO);
		}
    }
#endif
    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;


	if ((fAutoLogOn == VS_TRUE) && (srTransRec.usTerminalCommunicationMode == GPRS_MODE || srTransRec.usTerminalCommunicationMode == WIFI_MODE))		
		{
			vdDebug_LogPrintf("inCTOS_PreConnect  --- B");
			return(d_OK);
		}
		
		//if ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) && (fGPRSConnectOK != TRUE)){
		//	vdSetErrorMessage("GPRS NOT ESTABLISHED");	
      //return(d_NO);
	//	}
		
    
    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);


    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
    {
        //vdSetErrorMessage("COMM INIT ERR");
        vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
        return(d_NO);
    }


		if(VS_TRUE == strTCT.fDemo)
		return(d_OK);
	
    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode); 

    if (inRetVal != d_OK)
    {
        if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
		{
			vdDisplayErrorMsgResp2(" ", "GPRS Problem","Please Try Again");
			vdSetErrorMessage("");

			if (srTransRec.byTransType == LOG_ON)
			{
				CTOS_LCDTClearDisplay();
				vdDisplayErrorMsgResp2("","LOGON FAILED","PLEASE RETRY");
			}
		}
        //wifi-mod2
        else if (srTransRec.usTerminalCommunicationMode == WIFI_MODE)
        {
			vdDisplayErrorMsgResp2(" ", "WIFI Problem","Please Try Again");
			vdSetErrorMessage("");
        }
        //wifi-mod2
        else
        {
           vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
           vdSetErrorMessage("");
        }
        return(d_NO);
    }

				
	
    if (CN_FALSE == srTransRec.byOffline)
    {   
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);        
				//for improve transaction speed
    }


    return(d_OK);
}

int inCTOS_PreConnect2(void)
{
    int inRetVal;
    
    //srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
    
    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);


    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
    {
        //vdSetErrorMessage("COMM INIT ERR");
        vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
	    vdSetErrorMessage("");
        return(d_NO);
    }
    inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode); 
	

    if (CN_FALSE == srTransRec.byOffline)
    {   
        inRetVal = inModem_CheckComm2(&srTransRec);        

    }

    return(d_OK);
}

extern isPredial;
int inModem_CheckComm2(TRANS_DATA_TABLE *srTransPara)
{

  int shRet;
	
	vdDebug_LogPrintf("byTransType=[%d],szPriTxnPhoneNumber=[%s],szPriSettlePhoneNumber=[%s]",srTransPara->byTransType,strCPT.szPriTxnPhoneNumber,strCPT.szPriSettlePhoneNumber);

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

	vdDebug_LogPrintf("default phone num =[%s],fPreDial[%d]",srModemRec.strRemotePhoneNum,strCPT.fPreDial);
	isPredial = 0;
	
	if (strCPT.fPreDial == TRUE)
	{
    	shRet = CTOS_ModemDialup((BYTE *)srModemRec.strRemotePhoneNum,strlen(srModemRec.strRemotePhoneNum));
	
 		isPredial = 1;		 
 	}

	return ST_SUCCESS;
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

    unsigned char key;
    unsigned char bstatus = 0;

	//1217
			short shMinLen = 4;
			short shMaxLen = 12;
			BYTE Bret;
			unsigned char szOutput[30];
	//1217


    vdDebug_LogPrintf("inCTOS_CheckAndSelectMutipleMID=[%d]",strHDT.inHostIndex);
    //get the index , then get all MID from the MMT list and prompt to user to select
    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfRecords);
    shCurrentPageNum = 1;
    CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords > 1)
    {
        if (inNumOfRecords > ITEMS_PER_PAGE)
            isDOWN = TRUE;

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

                if(shFalshMenu)
                {
                    CTOS_LCDTClearDisplay();
                    vdDispTitleString("SELECT MERCHANT");
                    CTOS_LCDTPrintXY(2,8,"PLS SELECT");
                    for(shLoop=0 ; shLoop < shPageItems/*ITEMS_PER_PAGE*/ ; shLoop++)
                    {

                        memset(szDisplay,0,sizeof(szDisplay));
                        sprintf(szDisplay,"%d: %s",shLoop+1,strMMT[shLoop + (shCurrentPageNum -1)*ITEMS_PER_PAGE].szMerchantName);
                        CTOS_LCDTPrintXY(2,shLoop + 3,szDisplay);

                    }
                    vdCTOS_LCDGShowUpDown(isUP,isDOWN);
                }


                key=WaitKey(60);

                switch(key)
                {
                    case d_KBD_DOWN:
					case d_KBD_DOT:

                        shFalshMenu = 1;

                        shCurrentPageNum ++;
                        if(shCurrentPageNum > shTotalPageNum)
                            shCurrentPageNum = 1;
                        bstatus = 2;
                        break;

                    /*
                    case d_KBD_UP:

                        shCurrentPageNum --;
                        if(shCurrentPageNum < 1)
                            shCurrentPageNum = shTotalPageNum;
                        bstatus = 2;
                        break;
                        */
                    case d_KBD_CANCEL:
                        return FAIL;
                    case d_KBD_1:
                        //set the unique MMT num

                        memcpy(&strMMT[0],&strMMT[0 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
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
                        shFalshMenu = 0;
                        bstatus = -1 ;
                        break;

                }

                if((-1) == bstatus)
                {
                    vdSetErrorMessage("LOAD MMT ERR");
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

	if (strMMT[0].fEnablePSWD == 1)
	{
			CTOS_LCDTClearDisplay();
			vdDispTransTitle(srTransRec.byTransType);
			CTOS_LCDTPrintXY(1, 3,"ENTER PASSWORD:");
			
			while (1)
			{
					memset(szOutput,0x00,sizeof(szOutput)); 																		 
					shMinLen = strlen(strMMT[0].szPassWord);
																															 
					//Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
					Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
					if(strncmp(szOutput,strMMT[0].szPassWord,shMinLen) == 0)
					//if(strcmp(szOutput,strMMT[0].szPassWord) == 0)
					{		 
							break;
					}
					else if(d_KBD_CANCEL == Bret)
							return Bret;
					else
					{
							vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
							CTOS_LCDTClearDisplay();
							vdDispTransTitle(srTransRec.byTransType);
							CTOS_LCDTPrintXY(1, 3,"ENTER PASSWORD:");
					}
			}
	}


    srTransRec.MITid = strMMT[0].MITid;
    strcpy(srTransRec.szTID, strMMT[0].szTID);
    strcpy(srTransRec.szMID, strMMT[0].szMID);
    memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

    vdDebug_LogPrintf("szATCMD1=[%s] [%s] [%s] [%s] [%s]",strMMT[0].szATCMD1,strMMT[0].szATCMD2,strMMT[0].szATCMD3,strMMT[0].szATCMD4,strMMT[0].szATCMD5);

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
    short shMinLen = 4;
	//issue 00441
    //short shMaxLen = 12;
	short shMaxLen = 6;
    BYTE key;
    BYTE Bret;
    short shCount =0;
    short shRsesult = d_OK;


//1030
    //if (inMultiAP_CheckSubAPStatus() == d_OK)
    if ((inMultiAP_CheckSubAPStatus() == d_OK) && (fBinVerInitiatedSALE == VS_FALSE))
//1030
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
				vduiClearBelow(3); /* BDOCLG-V3-00005: Overlapping "WRONG PASSWORD" message -- jzg */
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

                if(SUPER_PW== strPIT.inPasswordLevel)
                {
                    CTOS_LCDTClearDisplay();
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 3,"SUPER PASSWORD:");

                    //Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
                    Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
					//issue:285
					if (Bret == 255) //timeout
						return Bret;
					//issue:285
                    if((strcmp(szOutput,strTCT.szSuperPW) == 0) ||
                        (strcmp(szOutput,strTCT.szSystemPW) == 0)||
                        (strcmp(szOutput,strTCT.szEngineerPW) == 0))
                    {
                        return d_OK;
                    }
                    else if(d_KBD_CANCEL == Bret)
                        return Bret;
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
                    CTOS_LCDTClearDisplay();
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 3,"SYSTEM PASSWORD:");

                    //Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
                    Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
					//issue:285
					if (Bret == 255) //timeout
						return Bret;
					//issue:285

                    if( (strcmp(szOutput,strTCT.szSystemPW) == 0) ||
                        (strcmp(szOutput,strTCT.szEngineerPW) == 0)
                        )
                    {
                        return d_OK;
                    }
                    else if(d_KBD_CANCEL == Bret)
                        return Bret;
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
                    CTOS_LCDTClearDisplay();
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 3,"PASSWORD:");

                    //Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
                    Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
					//issue:285
					if (Bret == 255) //timeout
						return Bret;
					//issue:285

					if(strcmp(szOutput,strTCT.szEngineerPW) == 0)
                    {
                        return d_OK;
                    }
                    else if(d_KBD_CANCEL == Bret)
                        return Bret;
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



	if ((inMultiAP_CheckSubAPStatus() == d_OK) && (fBinVerInitiatedSALE == VS_FALSE))
        return d_OK;

    inHostIndex = (short) strCDT.HDTid;

     vdDebug_LogPrintf("inCTOS_SelectHost =[%d]",inHostIndex);

 
    if ( inHDTRead(inHostIndex) != d_OK)
    {
        vdSetErrorMessage("HOST SELECTION ERR");
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
//gcitra
/*
    else
    {
		inCTOS_DisplayCardTitle(6, 7); //Display Issuer logo: re-aligned Issuer label and PAN lines -- jzg
        usInputLen = 7;
    }
*/
//gcitra

    setLCDPrint(usInputLen, DISPLAY_POSITION_LEFT, "CVV2: ");

    while(1)
    {
        //usRet = shCTOS_GetNum(usInputLen+1, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 1, d_INPUT_TIMEOUT);
        usRet = shCTOS_GetNum(usInputLen+1, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 1, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
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

		/*sidumili: Issue#:000051 [REMOVE CVV2 ENTRY ON MANUAL ENTRY]*/
		if (srTransRec.byEntryMode == CARD_ENTRY_MANUAL){
			return d_OK;
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


void vdCTOSS_EMV_SetAmount(void)
{
	BYTE szBaseAmount[20];
	BYTE szTipAmount[20];
	BYTE szTotalAmount[20];
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	BYTE  byDataTmp1[32];
	BYTE  byDataTmp2[32];
	BYTE  bPackSendBuf[256];
	USHORT usPackSendLen = 0;
	USHORT ushEMVtagLen;
	ULONG lnTmp;
		
  memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
  memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
  wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);

  inCTOSS_PutEnvDB ("BDOAMOUT", byDataTmp1);
  inCTOSS_PutEnvDB ("BDOCURS", strCST.szCurSymbol);

	if(strTCT.fATPBinRoute == TRUE)
	{
		memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
		memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
		wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
		lnTmp = atol(byDataTmp1);
		wub_long_2_array(lnTmp, byDataTmp2);

		memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
		bPackSendBuf[usPackSendLen++] = 0x04;
		memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
		usPackSendLen += 4;

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

		usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);
	}

}

int inCTOS_EMVProcessing()
{
    int inRet;
    BYTE   EMVtagVal[64];
	BYTE t9F33[4];
	char szErrMsg[30+1];
	
    if( CARD_ENTRY_ICC == srTransRec.byEntryMode)
    {
		vdDebug_LogPrintf("1. szEMVTermCapabilities [%02x%02x%02x]",strEMVT_Temp.szEMVTermCapabilities[0],strEMVT_Temp.szEMVTermCapabilities[1],strEMVT_Temp.szEMVTermCapabilities[2]);
		//Use the terminal capabilities set in Bancnet
		memset(&strEMVT_Temp,0x00, sizeof(STRUCT_EMVT));
		inEMVReadData(7);
		vdDebug_LogPrintf("2. szEMVTermCapabilities [%02x%02x%02x]",strEMVT_Temp.szEMVTermCapabilities[0],strEMVT_Temp.szEMVTermCapabilities[1],strEMVT_Temp.szEMVTermCapabilities[2]);

		strcpy(t9F33, strEMVT_Temp.szEMVTermCapabilities);
		ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,t9F33);
		
#if 0	//Remove to use T9F33 on EMVConfig.s3db
		memset(t9F33, 0x00, sizeof(t9F33));
		strcpy(t9F33, srTransRec.stEMVinfo.T9F33);
		
		t9F33[1] = 0x40;	
		ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,t9F33);
#endif
		
        vdCTOSS_EMV_SetAmount();
		inCTOS_EMVSetTransType(srTransRec.byTransType);

        inRet = usCTOSS_EMV_TxnPerform();

		//inCTOS_EMVSetTransType(srTransRec.byTransType);

        inCTOS_FirstGenACGetAndSaveEMVData();

        EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;
		memset(szErrMsg,0x00,sizeof(szErrMsg));
		inGetErrorMessage(szErrMsg);
		if(strcmp(szErrMsg,"USER CANCEL") == 0 || strcmp(szErrMsg,"PIN TIMEOUT") == 0)
			vdSetErrorMessage("");
		else
        switch( EMVtagVal[0] & 0xC0)
        {
            case 0: //Declined --- AAC
                strcpy(srTransRec.szAuthCode,"Z1");
                vdSetErrorMessage("EMV Decline");
                vdDebug_LogPrintf("1st ACs, card dec");

                return EMV_CRITICAL_ERROR;

            case 0x40: //Approval --- TC
                strcpy(srTransRec.szAuthCode,"Y1");
                srTransRec.shTransResult = TRANS_AUTHORIZED;
                vdDebug_LogPrintf("1nd AC app");
                break;

            case 0x80: //ARQC
                vdDebug_LogPrintf("go online");
                break;

            default:
                strcpy(srTransRec.szAuthCode,"Z1");
                vdSetErrorMessage("EMV Decline");
                return EMV_CRITICAL_ERROR;

        }

        vdDebug_LogPrintf("usCTOSS_EMV_TxnPerform return[%d]", inRet);

		
        /*if (inRet != d_OK)
            vdSetErrorMessage("First GenAC ERR");*/
        return inRet;

    }

    return d_OK;
}

int inCTOS_CheckTipAllowd()
{

    if(SALE_TIP == srTransRec.byTransType)
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;
    }

    if (TRUE !=strTCT.fTipAllowFlag)
    {
        if (SALE_TIP == srTransRec.byTransType)
        {
            //vdSetErrorMessage("TIP NOT ALLOWED");
            CTOS_LCDTClearDisplay();
			vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");	
        }
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

		// BANCNET - T007- TIC024 - APPLICATION BLOCKED -- sidumili
		if (shResult == EMV_APP_BLOCKED)
		{
			inFallbackToMSR = FAIL;
			vdDisplayErrorMsg(1, 8, "APPLICATION BLOCKED");
			return EMV_CHIP_FAILED;
		}
		// BANCNET - T007- TIC024 - APPLICATION BLOCKED -- sidumili

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
	BYTE bySC_status; //Issue# 000065 -- jzg

	BOOL isRemove = FALSE;

	vdDebug_LogPrintf("***shCTOS_EMVSecondGenAC START***");
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

    //memset(szIssuerScript,0,sizeof(szIssuerScript));
    st2ACResponseData.pAuthorizationCode = srTransRec.szRespCode;
    st2ACResponseData.pIssuerAuthenticationData = srTransRec.stEMVinfo.T91;
    st2ACResponseData.IssuerAuthenticationDataLen = srTransRec.stEMVinfo.T91Len;
    st2ACResponseData.pIssuerScript = szIssuerScript;
    st2ACResponseData.IssuerScriptLen = inIssuerScriptlen;

    usResult = TRANS_AUTHORIZED;

    usResult = usCTOSS_EMV_TxnCompletion(&st2ACResponseData);

    vdDebug_LogPrintf("PP_iCompletion:%d ", usResult);

    inCTOS_SecondGenACGetAndSaveEMVData();

    if(VS_TRUE == strTCT.fDemo)
    {
        usResult = PP_OK;
        EMVtagVal[0] = 0x40;
    }

	if(strcmp((char *)srTransRec.szRespCode, "00") == 0 && fPINTrxnRetry == TRUE)
	{
        usResult = PP_OK;
        EMVtagVal[0] = 0x40;
	}
	else
		EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;
	
	if(usResult != PP_OK)
	{
		if(strcmp((char *)srTransRec.szRespCode, "00") ==  0)
		{
			isRemove = isCheckEMVCardIsRemove(); //Check if card is remove

			if (isRemove)
			{
				CTOS_LCDTClearDisplay();
				//vdDisplayErrorMsgResp2("","CHIP NOT DETECTED","");
				vdSetErrorMessage("READ CARD ERROR");
				return EMV_CHIP_NOT_DETECTED;
			}
			else
			{
				CTOS_LCDTClearDisplay();
				vdDisplayErrorMsgResp2("","EMV DECLINE","");
				return EMV_CRITICAL_ERROR;
			}
		}	
	}

    //EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

    switch( EMVtagVal[0] & 0xC0)
    {
        case 0: //Declined --- AAC
            if(strcmp((char *)srTransRec.szRespCode, "00") ==  0)  //approve by host, but not by card
            {
				CTOS_SCStatus(d_SC_USER, &bySC_status);
				if (!(bySC_status & d_MK_SC_PRESENT))
				{
					CTOS_LCDTClearDisplay();
					vdDisplayErrorMsgResp2("","READ CARD ERROR","");
					return EMV_CHIP_NOT_DETECTED;
					
				}
				else
				{
					CTOS_LCDTClearDisplay();
					vdDisplayErrorMsgResp2("","EMV DECLINE","");									
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
					vdDisplayErrorMsgResp2("","READ CARD ERROR","");
					return EMV_CHIP_NOT_DETECTED;
				}
				else
				{
					CTOS_LCDTClearDisplay();
					vdDisplayErrorMsgResp2("","EMV DECLINE","");
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
				vdDisplayErrorMsgResp2("","READ CARD ERROR","");	
				return EMV_CHIP_NOT_DETECTED;
			}
			else
			{
				CTOS_LCDTClearDisplay();
				vdDisplayErrorMsgResp2("","EMV DECLINE","");
			}

            //vdSetErrorMessage("EMV Decline");
            return EMV_CRITICAL_ERROR;

    }

    vdDebug_LogPrintf("End 2nd GenAC shTransResult=%d iAction=%d",srTransRec.shTransResult, st2ACResponseData.bAction);
    return PP_OK;

}

int inCTOS_EMVTCUpload(void)
{
    int inRet;

    if( CARD_ENTRY_ICC == srTransRec.byEntryMode)
    {
        inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
        //inRet = inProcessEMVTCUpload(&srTransRec, -1);// TC upload
        inRet = inProcessEMVTCUpload(&srTransRec, 1);// Only upload the current TC

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

    USHORT inTagLen = 0;


    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szOutEMVData,0,sizeof(szOutEMVData));

    //shResult = usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_SELECT_APP, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
    DebugAddHEX("GET_EMV_TAG_AFTER_SELECT_APP",szOutEMVData,inTagLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_57, byVal, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_57[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, byVal[0], byVal[1], byVal[2]);

    memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
    wub_hex_2_str(byVal, byDataTmp1, usLen);
    memcpy(srTransRec.szTrack2Data, byDataTmp1, (usLen*2));
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

    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szGetEMVData,0,sizeof(szGetEMVData));

    //for improve transaction speed
    //usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_1STAC, &inTagLen, szOutEMVData);
    inDatabase_TerminalOpenDatabaseEx(DB_EMV);
    inMultiAP_Database_EMVTransferDataReadEx(&inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataInitEx();
	inDatabase_TerminalCloseDatabase();
    DebugAddHEX("GET_EMV_TAG_AFTER_1STAC",szOutEMVData,inTagLen);

    vdDebug_LogPrintf("inCTOS_FirstGenACGetAndSaveEMVData");

    usCTOSS_FindTagFromDataPackage(TAG_5F2A_TRANS_CURRENCY_CODE, srTransRec.stEMVinfo.T5F2A, &usLen, szOutEMVData, inTagLen);

	memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));
	/* EMV: Get Application Preferred Name - start -- jzg */
	usCTOSS_FindTagFromDataPackage(TAG_9F12, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
	vdDebug_LogPrintf("TAG 9F12 = [%s]", srTransRec.stEMVinfo.szChipLabel);
	/* EMV: Get Application Preferred Name - end -- jzg */

	if(strlen(srTransRec.stEMVinfo.szChipLabel)<=0)
		usCTOSS_FindTagFromDataPackage(TAG_50, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9A_TRANS_DATE, srTransRec.stEMVinfo.T9A, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9C_TRANS_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9C), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F06, srTransRec.stEMVinfo.T9F06, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;

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


	inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);
    ret = inIITReadEx(srTransRec.IITid);
    vdDebug_LogPrintf("inIITRead[%d]",ret);
    sprintf(szTransSeqCounter, "%06ld", strIIT.ulTransSeqCounter);
    wub_str_2_hex(szTransSeqCounter, (char *)szHEXTransSeqCounter, 6);
    memcpy(srTransRec.stEMVinfo.T9F41,szHEXTransSeqCounter,3);
    strIIT.ulTransSeqCounter++;
    ret = inIITSaveEx(srTransRec.IITid);    
	inDatabase_TerminalCloseDatabase();
    vdDebug_LogPrintf(" ret[%d] srTransRec.IITid[%d]strIIT.ulTransSeqCounter[%ld]",ret, srTransRec.IITid,strIIT.ulTransSeqCounter);
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

    //usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_2NDAC, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);//Improve 2nd Gen AC speed
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
        {   TAG_50,                     "Appl Lable Name"},
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

#if 1
int inCTOS_SelectAccountType(void)
{
    int inRet=d_NO;
    char szDebug[40 + 1]={0};
    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04;
	BYTE key = 0; 
    
    vdDebug_LogPrintf("inCTOS_SelectAccountType[START]");
    
    vdDebug_LogPrintf("-->>SELECT ACNT byTransType[%d]", srTransRec.byTransType);
    
    if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        return d_OK;


#if 0
    memset(szHeaderString, 0x00, sizeof(szHeaderString));
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    
    strcpy(szHeaderString, "SELECT ACCOUNT TYPE");
    strcat(szChoiceMsg,"SAVINGS \n");
    strcat(szChoiceMsg,"CURRENT");
	
	inSetColorMenuMode();
    //key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //key = MenuDisplayEx(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE, 30); /*Menu with timeout parameter*/
    //key = MenuDisplayEx(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE, inGetIdleTimeOut(TRUE)); /*Menu with timeout parameter*/

    key = MenuDisplayEx(szHeaderString, strlen(szHeaderString), bHeaderAttr, 2, 0, szChoiceMsg, TRUE, inGetIdleTimeOut(TRUE)); /*Menu with timeout parameter*/

	vdDebug_LogPrintf("::inCTOS_SelectAccountType::MenuDisplayEx::key[%d]", key);
    inSetTextMode();
#endif

    CTOS_LCDTClearDisplay();    

	vdDispTransTitle(srTransRec.byTransType);

	CTOS_LCDTPrintAligned(3, "Select account type", d_LCD_ALIGNLEFT);
	CTOS_LCDTPrintAligned(4, "1.SAVINGS", d_LCD_ALIGNLEFT);
	CTOS_LCDTPrintAligned(5, "2.CURRENT", d_LCD_ALIGNLEFT);


	while(1){
		key=WaitKey(inGetIdleTimeOut(TRUE));

       if(key == d_KBD_1)
        {
            inRet = d_OK;
            inAccountType=SAVINGS_ACCOUNT;
			srTransRec.inAccountType = SAVINGS_ACCOUNT;
			break;
        }
        else if(key == d_KBD_2)
        {
            inRet = d_OK;
            inAccountType=CURRENT_ACCOUNT;
			srTransRec.inAccountType = CURRENT_ACCOUNT;
			break;
        }
        else if(key == d_KBD_CANCEL)
        {
			if(fGetECRTransactionFlg() == TRUE)
				vdDisplayErrorMsgResp2("","TRANS CANCELLED","");
			
            inRet = d_NO;
			break;
        } 
		else if (key == 0xFF) /*BDO: For timeout occured -- sidumili*/
		{
			fTimeOutFlag = TRUE;	/*BDO: Flag for timeout --sidumili*/
			inRet = d_NO;
			break;
		}

	}
#if 1 //Removed extra screen after account selection
    // display selected account
    if (inRet != d_NO)
    {
        CTOS_LCDTClearDisplay();
		vdCTOS_DispStatusMessage("PROCESSING...");
        //vdDispTransTitle(srTransRec.byTransType);
        
        switch (inAccountType)
        {
            case CURRENT_ACCOUNT:
                //setLCDPrint27(7,DISPLAY_POSITION_CENTER, "CURRENT ACCOUNT");
            break;

            case SAVINGS_ACCOUNT:
                //setLCDPrint27(7,DISPLAY_POSITION_CENTER, "SAVINGS ACCOUNT");
            break;

            default:
            break;
        }
        //WaitKey(1);
    }
#endif

	if (inRet == d_NO)
	{
		/* Send response to ECR -- sidumili */
			vdSetECRResponse(ECR_OPER_CANCEL_RESP);
			memset(srTransRec.szRespCode,0x00,RESP_CODE_SIZE);
			fECRBuildSendOK = FALSE;
			if (!fECRBuildSendOK){	
				inMultiAP_ECRSendSuccessResponse();
			}	
			fECRBuildSendOK = FALSE;
		/* Send response to ECR -- sidumili */

	return d_NO;
	}
	
    vdDebug_LogPrintf("inCTOS_SelectAccountType[END]");
    
    return inRet;
}

#else
int inCTOS_SelectAccountType(void)
{
	BYTE key = 0;
	int inRet = 0;
	char szDebug[40+1] = {0};

	vdDebug_LogPrintf("inCTOS_SelectAccountType[START]");

	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		return d_OK;

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);

	do
	{
		setLCDPrint27(3,DISPLAY_POSITION_LEFT, "SELECT ACCOUNT:"); 	
		CTOS_LCDTPrintXY(1, 4, "[1]SAVINGS");
		CTOS_LCDTPrintXY(1, 5, "[2]CURRENT");

		key = WaitKey(60);
		if (key == d_KBD_1)
		{
			inRet = d_OK;
			inAccountType = SAVINGS_ACCOUNT;
			break;
		}
		else if (key == d_KBD_2)
		{
			inRet = d_OK;	
			inAccountType = CURRENT_ACCOUNT;
			break;
		}
		else if (key == d_KBD_CANCEL)
		{
			inRet = d_NO;
			break;
		}
		else
			vduiWarningSound();

	}
	while(1);

	// display selected account
	if (inRet != d_NO)
	{
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);

		switch (inAccountType)
		{
			case CURRENT_ACCOUNT:
				setLCDPrint27(7,DISPLAY_POSITION_CENTER, "CURRENT ACCOUNT");
				break;
			case SAVINGS_ACCOUNT:
				setLCDPrint27(7,DISPLAY_POSITION_CENTER, "SAVINGS ACCOUNT");
				break;
			default:
				break;
		}
		WaitKey(1);
	}

	vdDebug_LogPrintf("inCTOS_SelectAccountType[END]");

	return inRet;
}
#endif

int inCLearTablesStructure(void){

	memset(&strCDT,0x00, sizeof(STRUCT_CDT));
	memset(&strIIT,0x00, sizeof(STRUCT_IIT));
	memset(&strEMVT,0x00, sizeof(STRUCT_EMVT));
	memset(&strHDT,0x00, sizeof(STRUCT_HDT));
	//memset(srTransRec.szBaseAmount, 0x00, sizeof(srTransRec.szBaseAmount));
	//memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
	memset(&srTransRec,0x00, sizeof(TRANS_DATA_TABLE));
    return d_OK;
}



int inConfirmPAN(void){

    unsigned char key;
	int inRet;

	//1030
		//if (inMultiAP_CheckSubAPStatus() == d_OK)
		if ((inMultiAP_CheckSubAPStatus() == d_OK) && (fBinVerInitiatedSALE == VS_FALSE))
	//1030
        return d_OK;


	if (strTCT.fConfirmPAN){
		while(1){
			key=WaitKey(15);

			if (key == d_KBD_CANCEL){
				inRet = d_NO;
				break;

			}
				
			if(key == d_KBD_ENTER){
				inRet =  d_OK;	
				break;
			}

		}
	}

	return inRet;
}

int inCTOS_EMVSetTransType(BYTE byTransType)
{
              switch(byTransType)
              {
                             case SALE:
                                      ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x00");
                                        break;
                             case REFUND:
                                      ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x20");
                                           break;
                             case PRE_AUTH:
                                      ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x30");
                                           break;
							case CASH_ADVANCE:
								ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x01");
								break;
							case BALANCE_INQUIRY:
								ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x31");
								break;
                             default:
            break;
              }
}


//gcitra

/*********************************************************/
/*sidumili: Issue#000136 [code by albert]*/
/*********************************************************/
BYTE InputAmount2(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter)
{
    char szTemp[24+1];
    USHORT inRet;
int inCtr=0;
char szAmount[24+1];
char chAmount=0x00;
char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;;
memset(szAmount, 0x00, sizeof(szAmount));
// patrick fix code 20141211
if (first_key != 0x00)
{
        szAmount[0] = first_key;
}


while(1)
{
memset(szTemp, 0x00, sizeof(szTemp));         
if(strlen(szAmount) > 0)
{
  vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szAmount, szTemp);
}
else
{
  vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szTemp);
}

 
       clearLine(usY);
       CTOS_LCDTPrintXY(usX, usY, szCurSymbol);
       int x=0;
       int len, index;
       len=strlen(szTemp);
       for(index=0; index < len; index++)
       {
           if(szTemp[index] == '.')
               x+=1;
           else
               x+=2;
       }
       
       CTOS_LCDTPrintXY(36-x, usY, szTemp);
CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
while(1)//loop for time out
{
CTOS_KBDInKey(&isKey);
if (isKey){ //If isKey is TRUE, represent key be pressed //
vduiLightOn();
//Get a key from keyboard //
CTOS_KBDGet(&c);
inRet=c;
break;
}
else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
{
 
//inRet=67;
//break;
return 0xFF;
}
}

if(inRet >= 48 && inRet <= 57)
{
if(inCtr < 10)
{
  memset(szTemp, 0x00, sizeof(szTemp));
  sprintf(szTemp, "%c", inRet);
               strcat(szAmount, szTemp);
               inCtr++; 
	   
			   if(inCtr == 1 && szAmount[0] == 48)
			   {
				   memset(szAmount, 0x00, sizeof(szAmount));
				   inCtr=0;
			   }
}
}
else if(inRet == 67) /*cancel key*/
{
return d_USER_CANCEL;
}
else if(inRet == 65) /*entery key*/
    {
     
        if(strlen(szAmount) > 0)
        {
            memcpy(baAmount, szAmount, strlen(szAmount));
            return d_OK;
        }
        else
        {
            if(bIgnoreEnter == 1)
                return d_OK;
        }
    }
else if(inRet == 82) /*clear key*/
{
 
if(inCtr > 0)
               inCtr--;
           szAmount[inCtr]=0x00;
}
}
}

int inCTOS_GetCardFieldsFallback(void)
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

    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;
	unsigned char c;

    BYTE szTempPAN[PAN_SIZE+1];
    short shReturn = d_OK; //Invalid card reading fix -- jzg

	memset(szTempPAN,0x00, sizeof(szTempPAN));

	strcpy(szTempPAN,srTransRec.szPAN);

	
	memset(srTransRec.szPAN,0x00, sizeof(srTransRec.szPAN));


 	DebugAddSTR("inCTOS_GetDestinationCardFields","Processing...",20);

	vdDisplayErrorMsg(1, 8, "CHIP NOT ALLOWED");

	
	vduiClearBelow(2);
	
	vdRemoveCard();

	
	vduiClearBelow(2);


	
 	CTOS_LCDTPrintXY(1, 3,"PLEASE SWIPE CARD");


 SWIPE_AGAIN:

    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

    CTOS_KBDBufFlush();
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return READ_CARD_TIMEOUT ;

		
        CTOS_KBDInKey(&byKeyBuf);

		if(byKeyBuf){
			
			CTOS_KBDGet(&byKeyBuf);
			if (byKeyBuf == d_KBD_CANCEL)
				return d_NO;
		}
		

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

		if ((byMSR_status == d_OK) && (usTk2Len < 10)){
			vdCTOS_ResetMagstripCardData();
			CTOS_LCDTClearDisplay();
			vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
			vduiWarningSound();
			return USER_ABORT;
		}

        if(byMSR_status == d_OK )
        {
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                    CTOS_KBDBufFlush();
                    vdDisplayErrorMsg(1, 8, "READ CARD ERROR"); 
                    return INVALID_CARD;
            }

            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }


            break;
        }

    }

    strcpy(srTransRec.szCardLable, strCDT.szCardLabel);
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);

	CTOS_KBDBufFlush();


    if (memcmp(szTempPAN,srTransRec.szPAN, strlen(srTransRec.szPAN)) != 0){
		vdDisplayErrorMsg(1, 8, "INVALID CARD");
		return d_NO;
    }

/*
    inHostIndex = (short) strCDT.HDTid;

     vdDebug_LogPrintf("inCTOS_SelectHost =[%d]",inHostIndex);


    if ( inHDTRead(inHostIndex) != d_OK)
    {
        vdSetErrorMessage("HOST SELECTION ERR");
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

*/	
    return d_OK;;
}


int inCTOS_DisplayCardTitle2(USHORT usCardTypeLine, USHORT usPANLine)
{
    char szStr[50 + 1];
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE szTemp1[30+1];

	BYTE szPAN1[20+1];
	BYTE szPAN2[20+1];
	int inRemaining=0;


    memset(szStr,0x00,sizeof(szStr));
    memset(EMVtagVal,0x00,sizeof(EMVtagVal));

	//Display Issuer logo: Clear the display first, then re-display trans title - start -- jzg
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg

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
        //1102
        if (srTransRec.byTransType != BIN_VER){
        	sprintf(szStr, "%s", strCDT.szCardLabel);
        	vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
        }
		//1102
    }

		strcpy(szStr,"CARD NUMBER:");
		memset(szTemp1,0,sizeof(szTemp1));
		//vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
        vdCTOS_FormatPAN(strTCT.DisplayPANFormat, srTransRec.szPAN, szTemp1);

	//Display Issuer logo: Clear the display first, then re-display trans title - start -- jzg
	//1102
	//if (srTransRec.byTransType != BIN_VER){
	//	vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
	//	displayAppbmpDataEx(110, 35, strIIT.szIssuerLogo);
	//}

//	displayAppbmpDataEx(140, 35, "mc.bmp");

    if(0>= usCardTypeLine || 0 >= usPANLine)
    {
        usCardTypeLine = 5;
        usPANLine = 6;
    }
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg
    CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);

	
	//0721
	if (strlen(szTemp1) > 20){
		memset(szPAN1, 0x00, sizeof(szPAN1));
		memset(szPAN2, 0x00, sizeof(szPAN2));
		inRemaining = strlen(szTemp1) - 20;
	
		memcpy(szPAN1, szTemp1, 20);
		memcpy(szPAN2, &szTemp1[20], inRemaining);
				
		CTOS_LCDTPrintXY(1, usPANLine, szPAN1);
		CTOS_LCDTPrintXY(1, usPANLine+1, szPAN2);
	
	}else
		CTOS_LCDTPrintXY(1, usPANLine, szTemp1);
	//0721

    return(d_OK);
}


int inConfirmPAN2(void){

    unsigned char key;
	int inRet;


	if (strTCT.fConfirmPAN){
		while(1){
			key=WaitKey(15);

			if (key == d_KBD_CANCEL){
				inRet = d_NO;
				break;

			}
				
			if(key == d_KBD_ENTER){
				inRet =  d_OK;	
				break;
			}

		}
	}

	return inRet;
}

//hubing enhance ECR
int get_env_int (char *tag)
{
	int     ret = -1;
	char    buf[6];

    memset (buf, 0, sizeof (buf));
    if ( inCTOSS_GetEnvDB (tag, buf) == d_OK )
    {
        ret = atoi (buf);
    }

	vdDebug_LogPrintf("get_env_int [%s]=[%d]", tag, ret);

    return ret;
}
//hubing enhance ECR
void put_env_int(char *tag, int value)
{
	int     ret = -1;
	char    buf[6];

    memset (buf, 0, sizeof (buf));
    //int2str (buf, value);
    sprintf(buf, "%d", value);
    ret = inCTOSS_PutEnvDB (tag, buf);

	vdDebug_LogPrintf("put_env_int [%s]=[%d] ret[%d]", tag, value, ret);
}

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

int inCTOS_TEMPCheckAndSelectMutipleMID(void)
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
    
    unsigned char key;
    unsigned char bstatus = 0; 

//1217
		short shMinLen = 4;
		short shMaxLen = 6;
		BYTE Bret;
		unsigned char szOutput[30];
//1217

    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    char szHeaderString[50] = "SELECT MERCHANT";
    char szMitMenu[1024];
    int inLoop = 0;

    if(inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	inNMTReadNumofRecords(&inNumOfRecords);
    CTOS_KBDBufFlush();//cleare key buffer

    if(inNumOfRecords > 1)
    {
        
        //issue-00378 - remove trailing key entry for amount if multi merchant setup
        vdSetFirstIdleKey(0x00);

        //issue-00436: clear the buffer first to avoid garbage display
		memset(szMitMenu, 0x00, sizeof(szMitMenu));
		
        for (inLoop = 0; inLoop < inNumOfRecords; inLoop++)
        {
            strcat((char *)szMitMenu, strNMT[inLoop].szMerchName);
            if(inLoop + 1 != inNumOfRecords)
                strcat((char *)szMitMenu, (char *)" \n");
        }

				if((strTCT.byTerminalType % 2) == 0)
				{
					CTOS_LCDForeGndColor(RGB(13, 43, 112));
					CTOS_LCDBackGndColor(RGB(255, 255, 255));
				}
			
        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);

        inSetTextMode(); 
				 
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
            memcpy(&strNMT[0],&strNMT[key-1],sizeof(STRUCT_MMT));
        }
    }

#if 0    
    vdDebug_LogPrintf("inCTOS_TEMPCheckAndSelectMutipleMID=[%d]",strHDT.inHostIndex);
    //get the index , then get all MID from the MMT list and prompt to user to select
    inNMTReadNumofRecords(&inNumOfRecords);
    shCurrentPageNum = 1;
    CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords > 1)
    {
        //issue-00378 - remove trailing key entry for amount if multi merchant setup
        vdSetFirstIdleKey(0x00);
		
        if (inNumOfRecords > ITEMS_PER_PAGE)
            isDOWN = TRUE;
        
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


                if(shFalshMenu)
                {
                    CTOS_LCDTClearDisplay();
                    vdDispTitleString("MULTI MERCHANT");
                    //CTOS_LCDTPrintXY(2,8,"PLS SELECT");
                    for(shLoop=0 ; shLoop < shPageItems/*ITEMS_PER_PAGE*/ ; shLoop++)
                    {
          
                        memset(szDisplay,0,sizeof(szDisplay));
                        sprintf(szDisplay,"%d: %s",shLoop+1,strNMT[shLoop + (shCurrentPageNum -1)*ITEMS_PER_PAGE].szMerchName);
                        CTOS_LCDTPrintXY(2,shLoop + 3,szDisplay);

                    }
					
					//#issue#=000137
					if (shCurrentPageNum > 1)
						isUP = TRUE;
					else
						isUP = FALSE;

					
					//if (inNumOfRecords < ITEMS_PER_PAGE){
					//	isUP = FALSE;
					//	isDOWN = FALSE;
					//}
					
					//000137
                    vdCTOS_LCDGShowUpDown(isUP,isDOWN);
                }
                
                
                key=WaitKey(60);
                
                switch(key)
                {
                    case d_KBD_DOWN:
					case d_KBD_DOT:

                        shFalshMenu = 1;
                        
                        shCurrentPageNum ++;
                        if(shCurrentPageNum > shTotalPageNum)
                            shCurrentPageNum = 1;
                        bstatus = 2;
                        break;
                        
                    //issue#-000137
                    case d_KBD_UP:
											if(shTotalPageNum == 0)
												{
                            shFalshMenu = 1;
                            shCurrentPageNum ++;
                            if(shCurrentPageNum > shTotalPageNum)
                            shCurrentPageNum = 1;
                            bstatus = 2;
                            break;
												}

                        shCurrentPageNum --;
                        if(shCurrentPageNum < 1)
                            shCurrentPageNum = shTotalPageNum;
                        bstatus = 2;        
                        break;          
                    //000137
                    case d_KBD_CANCEL:                          
                        return FAIL;
                    case d_KBD_1:
                        //set the unique MMT num
                                            
                        memcpy(&strNMT[0],&strNMT[0 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                        bstatus = 0;
                        break;
                    case d_KBD_2:
                        if(shPageItems < 2)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strNMT[0],&strNMT[1 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
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
                            memcpy(&strNMT[0],&strNMT[2 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
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
                            memcpy(&strNMT[0],&strNMT[3 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
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
                                memcpy(&strNMT[0],&strNMT[4 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
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
                                memcpy(&strNMT[0],&strNMT[5 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
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
                                memcpy(&strNMT[0],&strNMT[6 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
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
                                memcpy(&strNMT[0],&strNMT[7 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    default:
                        shFalshMenu = 0;
                        bstatus = -1 ;
                        break;

                }
                
                if((-1) == bstatus)
                {
                    vdSetErrorMessage("LOAD MMT ERR");
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
#endif

#if 1
		//vdDebug_LogPrintf("key[%d]--fEnablePSWD[%d]-----", key,strMMT[key-1].fEnablePSWD);
		if (strNMT[0].fMerchEnablePassword == 1)
		{
				CTOS_LCDTClearDisplay();

				vdDispTitleString("MULTI MERCHANT");
				CTOS_LCDTPrintXY(1, 3,strNMT[0].szMerchName);
				CTOS_LCDTPrintXY(1, 4,"ENTER PASSWORD:");
				
				while (1)
				{
						memset(szOutput,0x00,sizeof(szOutput)); 																		 
						shMinLen = strlen(strNMT[0].szMercPassword);
																																 
						//Bret = InputString(1, 5, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
						Bret = InputString(1, 5, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE));
						//if(strncmp(szOutput,strNMT[0].szMercPassword,shMinLen) == 0)
						//if(strcmp(szOutput,strMMT[0].szPassWord) == 0)
						
						if (Bret == d_KBD_CANCEL)
							return d_NO;
						
						if(strncmp(szOutput,strNMT[0].szMercPassword,strlen(szOutput)) == 0)
						{ 		
								break;
						}else
						{
								vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
								vduiClearBelow(5);
						}
				}
		}
#endif


    srTransRec.MITid = strNMT[0].NMTID;

	vdDebug_LogPrintf("srTransRec.MITid :::: [%d]", srTransRec.MITid);  
    //strcpy(srTransRec.szTID, strMMT[0].szTID);
    //strcpy(srTransRec.szMID, strMMT[0].szMID);
    //memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
    //strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

    //vdDebug_LogPrintf("szATCMD1=[%s] [%s] [%s] [%s] [%s]",strMMT[0].szATCMD1,strMMT[0].szATCMD2,strMMT[0].szATCMD3,strMMT[0].szATCMD4,strMMT[0].szATCMD5);


    return d_OK;
    

}

int inCTOS_GetSourceCardFields(void)
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

    short shReturn = d_OK; //Invalid card reading fix -- jzg
    
 DebugAddSTR("inCTOS_GetSourceCardFields","Processing...",20);

 CTOS_LCDTClearDisplay();

 SWIPE_AGAIN:

	/*BDO: Do not display amount if ecr is enable[Reload Trxn] -- sidumili */
   #if 0
 // patrick ECR 20140516 start
    if ((strTCT.fECR) && (fECRTxnFlg == TRUE)) // tct
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
 #endif
 /*BDO: Do not display amount if ecr is enable[Reload Trxn] -- sidumili */

    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

	vdDispTransTitle(srTransRec.byTransType);
	vdCTOS_DisplayReloadCardType(); // BDO: For reload trxn, display source/destination card label
	CTOS_LCDTPrintXY(1, 4, "SWIPE/ENTER/CANCEL:");
		
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return READ_CARD_TIMEOUT ;

        CTOS_KBDInKey(&byKeyBuf);


        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
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

				if ((byMSR_status == d_OK) && (usTk2Len < 10)){
						vdCTOS_ResetMagstripCardData();
						CTOS_LCDTClearDisplay();
						vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
 						vduiWarningSound();
						return USER_ABORT;
				}

		//gcitra
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
		//gcitra
        {
            //Invalid card reading fix - start -- jzg
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                    CTOS_KBDBufFlush();
                    vdDisplayErrorMsg(1, 8, "READ CARD ERROR"); 
                    return INVALID_CARD;
            }
            //Invalid card reading fix - end -- jzg

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
            //strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
            strcpy(srTransRec.szCardLable, strCDT.szCardLabel);
        }
    }
    else
    {
        //strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        strcpy(srTransRec.szCardLable, strCDT.szCardLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}


int inCTOS_GetDestinationCardFields(void)
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

    short shReturn = d_OK; //Invalid card reading fix -- jzg
    
 DebugAddSTR("inCTOS_GetDestinationCardFields","Processing...",20);

 CTOS_LCDTClearDisplay();

 SWIPE_AGAIN:

/*BDO: Do not display amount if ecr is enable[Reload Trxn] -- sidumili */
#if 0
// patrick ECR 20140516 start
    if ((strTCT.fECR) && (fECRTxnFlg == TRUE)) // tct
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
#endif
/*BDO: Do not display amount if ecr is enable[Reload Trxn] -- sidumili */

    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

	vdDispTransTitle(srTransRec.byTransType);
	vdCTOS_DisplayReloadCardType(); // BDO: For reload trxn, display source/destination card label
	CTOS_LCDTPrintXY(1, 4, "SWIPE/ENTER/CANCEL:");
		
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return READ_CARD_TIMEOUT ;

        CTOS_KBDInKey(&byKeyBuf);


        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
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

				if ((byMSR_status == d_OK) && (usTk2Len < 10)){
						vdCTOS_ResetMagstripCardData();
						CTOS_LCDTClearDisplay();
						vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
						vduiWarningSound();
						return USER_ABORT;
				}

		//gcitra
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
		//gcitra
        {
            //Invalid card reading fix - start -- jzg
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                    CTOS_KBDBufFlush();
                    vdDisplayErrorMsg(1, 8, "READ CARD ERROR"); 
                    return INVALID_CARD;
            }
            //Invalid card reading fix - end -- jzg

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
            //strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
            strcpy(srTransRec.szCardLable, strCDT.szCardLabel);
        }
    }
    else
    {
        //strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        strcpy(srTransRec.szCardLable, strCDT.szCardLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;;
}


int inCTOS_DisplayReloadCardTitle(USHORT usCardTypeLine, USHORT usPANLine)
{
    char szStr[50 + 1]; 
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE szTemp1[30+1];

		BYTE szPAN1[20+1];
		BYTE szPAN2[20+1];
		int inRemaining=0;

		
    BYTE szTitle[16+1];
	
	//1102
	if (srTransRec.byTransType != BIN_VER){
		vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
		if (strCDT.HDTid != 5) 
			displayAppbmpDataEx(1,1, strIIT.szIssuerLogo);
	}
	//1102




  memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
 




#if 1

    memset(szStr,0x00,sizeof(szStr));
    memset(EMVtagVal,0x00,sizeof(EMVtagVal));

	//Display Issuer logo: Clear the display first, then re-display trans title - start -- jzg
	//CTOS_LCDTClearDisplay(); 
	//DispTransTitle(srTransRec.byTransType);
	
	setLCDPrint27(1, DISPLAY_POSITION_RIGHT, strCDT.szCardLabel);
	setLCDPrint27(2, DISPLAY_POSITION_RIGHT, szTitle);
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg
  
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        
        if(EMVtagLen > 0)
        {
            sprintf(szStr, "%s", EMVtagVal);
            vdDebug_LogPrintf("Card label(str): %s",szStr);       
        }
    }

/*
    if(0 == strlen(szStr))
    {
        //1102
        if (srTransRec.byTransType != BIN_VER){
        	sprintf(szStr, "%s", strCDT.szCardLabel);
        	vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
        }
		//1102
    }
*/
    strcpy(szStr,"PAN:");
    memset(szTemp1,0,sizeof(szTemp1));
	if (strTCT.fMaskPanDisplay == TRUE){
    	//vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
    	strcpy(szTemp1, srTransRec.szPAN);	
		cardMasking(szTemp1, 3);		
	}else{ 	
		vdCTOS_FormatPAN2(strTCT.DisplayPANFormat, srTransRec.szPAN, szTemp1);
	}	


//	displayAppbmpDataEx(140, 35, "mc.bmp"); 

    if(0>= usCardTypeLine || 0 >= usPANLine)
    {
        usCardTypeLine = 5;
        usPANLine = 6;
    }
	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg
    
    CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);

//0721
    if (strlen(szTemp1) > 20){
			memset(szPAN1, 0x00, sizeof(szPAN1));
			memset(szPAN2, 0x00, sizeof(szPAN2));
			inRemaining = strlen(szTemp1) - 20;

			memcpy(szPAN1, szTemp1, 20);
			memcpy(szPAN2, &szTemp1[20], inRemaining);
			
    	//CTOS_LCDTPrintXY(1, usPANLine, szPAN1);
			
			setLCDPrint27(7, DISPLAY_POSITION_RIGHT, szPAN1);
    	CTOS_LCDTPrintXY(1, 8, szPAN2);

    }else
    	//CTOS_LCDTPrintXY(1, usPANLine, szTemp1);
    	
		setLCDPrint27(7, DISPLAY_POSITION_RIGHT, szTemp1);
//0721
#endif    
    return(d_OK);
}

int inReloadConfirmPAN(void){

    unsigned char key;
	int inRet;

	if (strTCT.fConfirmPAN){
		while(1){
			key=WaitKey(15);

			if (key == d_KBD_CANCEL){
				inRet = d_NO;
				break;

			}
				
			if(key == d_KBD_ENTER){
				inRet =  d_OK;	
				break;
			}

		}
	}

	return inRet;
}

/* BDO: PAN Format requirements -- sidumili */
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

memset(szOutPAN, 0x00, sizeof(szOutPAN));
sprintf(szOutPAN, "%s%s%s", szTemp1, szTemp3, szTemp2);

vdDebug_LogPrintf("szTemp1[%s]|szTemp2[%s]|szTemp3[%s]", szTemp1, szTemp2, szTemp3);
vdDebug_LogPrintf("inLen1[%d]|inLen2[%d]", inLen1, inLen2);
vdDebug_LogPrintf("szOutPAN[%s]", szOutPAN);

}
/* BDO: PAN Format requirements -- sidumili */

int inCTOS_MerchantCheckBatchAllOperation(void)
{
  	int inRet = d_NO;
  	int inNumOfHost = 0,inNum;
  	char szBcd[INVOICE_BCD_SIZE+1];
  	char szErrMsg[30+1];
  	char szAPName[25];
  	int inAPPID;

    CTOS_LCDTClearDisplay();
		
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inCTOS_MerchantTagMustSettle();
        inCTOS_MultiAPALLAppEventID(d_IPC_CMD_MERCHANT_CHECK_BATCH_ALL);
    }
    else
        inCTOS_MerchantTagMustSettle();
    
    CTOS_LCDTClearDisplay();
    return ST_SUCCESS;
}

void inCTOS_MerchantTagMustSettle(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,i,inCount,inRet;
    //int inTranCardType;
    //int inReportType;
    int inBatchNumOfRecord;
    //int *pinTransDataid;

    STRUCT_FILE_SETTING strFile;
    ACCUM_REC srAccumRec;
		
    //BYTE baTemp[PAPER_X_SIZE * 64];
    //char szStr[d_LINE_SIZE + 1];
		//int inRet = d_NO;
		int inNumOfHost = 0, inNum = 0;
		char szBcd[INVOICE_BCD_SIZE+1] = {0};
		char szErrMsg[31] = {0};
		char szAPName[25] = {0};
		int inAPPID = 0;
    int inMerchantCount=0;
		int inBatchRecordNum = 0;
		int  *pinTransDataid = NULL;

		vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
		

		int inMMTTotalNumofMerchants=0;
		
		
		CTOS_RTC SetRTC;
		BYTE szCurrDate[8] = {0};
		
		
		memset(szCurrDate, 0x00, sizeof(szCurrDate));
		CTOS_RTCGet(&SetRTC);
		sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);

		//inMMTTotalNumofMerchants=inMMTNumAlbertRecord();

		//pinTransDataid = (int*)malloc(inMMTTotalNumofMerchants * sizeof(int));

		//inBatchReviewByMerchandHostAlbert(pinTransDataid);

   	memset(szAPName,0x00,sizeof(szAPName));
   	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    inNumOfHost = inHDTNumRecord();
    vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);

    vduiDisplayStringCenter(3 ,"CHECKING MERCHANT");
	vduiDisplayStringCenter(4 ,"SETTLE STATUS");
	vduiDisplayStringCenter(5 ,"PLEASE WAIT...");		
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            if(strHDT.fHostEnable != TRUE)
						     continue;
						
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
						
						
						
					
						
            if (strcmp(szAPName, strHDT.szAPName)!=0)
                continue;
						else
						{
							srTransRec.HDTid=strHDT.inHostIndex;
							inMMTTotalNumofMerchants=inMMTNumAlbertRecord(srTransRec.HDTid);
							pinTransDataid = (int*)malloc(inMMTTotalNumofMerchants * sizeof(int));
							inBatchReviewByMerchandHostAlbert(pinTransDataid,srTransRec.HDTid);
							vdDebug_LogPrintf("inMMTTotalNumofMerchants=[%d]----",inMMTTotalNumofMerchants);
							
							for (inMerchantCount = 1; inMerchantCount<=inMMTTotalNumofMerchants; inMerchantCount++)
							{
                  inMMTReadRecord(srTransRec.HDTid, pinTransDataid[inMerchantCount]);
									if(strMMT[0].fMustSettFlag == TRUE)
									    continue;
									
				if ((strMMT[0].fMMTEnable == TRUE) &&
					(wub_str_2_long(szCurrDate) >= wub_str_2_long(strMMT[0].szSettleDate)))
                  {										
                     srTransRec.MITid=strMMT[0].MITid; 								 
                     vdCTOS_GetAccumName(&strFile, &srAccumRec);
                     if ((inResult = inMyFile_CheckFileExist(strFile.szFileName)) > 0)
                     {
                         strMMT[0].fMustSettFlag = 1;
                         inMMTSave(strMMT[0].MMTid);
                     }
                  }
							}
							
							free(pinTransDataid);
						}
        }
    }

	
}

/*BDO: Get Idle TimeOut -- sidumili*/
int inGetIdleTimeOut(BOOL fSecond)
{
	int inTimeOut;

	inTimeOut = strTCT.inIdleTimeOut;

	if (inTimeOut <= 0) inTimeOut = 30;

	// MS
	if (fSecond == FALSE)
		inTimeOut = (inTimeOut * 100);

	vdDebug_LogPrintf("::inGetIdleTimeOut inTimeOut[%d]", inTimeOut);

	return(inTimeOut);

}

void vdPrintReportDisplayBMP(void)
{
   int inNumOfLine = 8;
   
   CTOS_LCDTClearDisplay();
   
   if((strTCT.byTerminalType % 2) == 0)
         inNumOfLine = 16;
   
   if (strTCT.fSMMode	== TRUE)
   {
		if(strTCT.fDisplayPrintBitmap == TRUE)
		{ 	
			if((strTCT.byTerminalType % 2) == 0)
				vdDisplayAnimateBmp(0,0, "Print4.bmp", "Print5.bmp", "Print6.bmp", NULL, NULL);
			else
				vdDisplayAnimateBmp(0,0, "Print4#.bmp", "Print5#.bmp", "Print6#.bmp", NULL, NULL);
		}
	  	else
			CTOS_LCDTPrintXY(1, inNumOfLine, "PRINTING...");
	  
	}
	else
	{
		if(strTCT.fDisplayPrintBitmap == TRUE)
		{	
			if((strTCT.byTerminalType % 2) == 0)
	    		vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
			else
				vdDisplayAnimateBmp(0,0, "Print1#.bmp", "Print2#.bmp", "Print3#.bmp", NULL, NULL);
		}
	    else
			CTOS_LCDTPrintXY(1, inNumOfLine, "PRINTING...");
	}
}




int inSETIssuerUsingAID(void)
{


	if (srTransRec.byEntryMode == CARD_ENTRY_ICC){
	
		if ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x00\x03",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x00\x03",5) == 0)) 		
		 	srTransRec.IITid = 2;
		else if ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x00\x04",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x00\x04",5) == 0))		
		 	srTransRec.IITid = 4;
		else if ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x00\x65",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x00\x65",5) == 0))		
			srTransRec.IITid = 12;
		else if ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x00\x25",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x00\x25",5) == 0))		
			srTransRec.IITid = 3;
		else if ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x03\x33",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x03\x33",5) == 0))		
			srTransRec.IITid = 9;    
		else if ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x06\x35",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x06\x35",5) == 0))		
			srTransRec.IITid = 6;
	}

	vdDebug_LogPrintf("inSETIssuerForCatchAll %d", srTransRec.IITid);

	if (srTransRec.IITid > 0){
		inIITRead(srTransRec.IITid);
		strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
		strcpy(strCDT.szCardLabel, srTransRec.szCardLable);
		
		vdDebug_LogPrintf("strIIT.szIssuerLabel %s", strIIT.szIssuerLabel);
		vdDebug_LogPrintf("strIIT.szIssuerLabel %s", strIIT.szIssuerLogo);
		
	}

	

	return d_OK;

}

void vdGetCRC(void)
{
	FILE* file;
	int nLen;
	unsigned char chBuffer[1024];

	BYTE baTemp[10+1];
	
	memset(chBuffer, 0, 1024);
	vdDebug_LogPrintf("*** vdGetCRC V5S_BDODEBIT***");
	
	if ((file = fopen ("V5S_RBDEBIT", "rb")) != NULL)
	{
		vdDebug_LogPrintf("REINER :: fopen[%d]",file);
		while (nLen = fread (chBuffer, 1, 512, file))
		{
		}
		
		memset(baTemp, 0x00, sizeof(baTemp));
		sprintf(baTemp, "%02x%02x%02x%02x", 
			wub_lrc((unsigned char *) &chBuffer[0], 128),
			wub_lrc((unsigned char *) &chBuffer[127], 128),
			wub_lrc((unsigned char *) &chBuffer[255], 128),
			wub_lrc((unsigned char *) &chBuffer[511], 128));


		vdDebug_LogPrintf("REINER :: baTemp[%s]", baTemp);
		//inPrint(baTemp);
		put_env_char("DEBITCRC",baTemp);
		fclose (file);
	}

}

void vdGetCRC_ALL(void)
{
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		vdGetCRC();
		inCTOS_MultiAPALLAppEventID(d_IPC_CMD_GET_CRC_ALL);
	}
	else
		vdGetCRC();

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

void vdSetECRResponse(char* szECRResponse)
{
	vdDebug_LogPrintf("vdSetECRResponse :: szECRResponse :: [%s]",szECRResponse);
	strcpy(srTransRec.szRespCode,"");
	strcpy(srTransRec.szECRRespCode, szECRResponse); //make responce code as -1 - for ECR transaction 
}

int inCTOS_PreConnectAndInit(void)
{
    int inRetVal;



    vdDebug_LogPrintf("inCTOS_PreConnectAndInit");
	vdDebug_LogPrintf("inCommunicationMode[%d] :: usTerminalCommunicationMode[%d]",strCPT.inCommunicationMode,srTransRec.usTerminalCommunicationMode);
	if(strCPT.inCommunicationMode == DIAL_UP_MODE || strCPT.inCommunicationMode == ETHERNET_MODE) 
		return d_OK;
		
	vdSetInit_Connect(1);
	
    if (strTCT.fSingleComms == TRUE){	
		if(inCPTRead(1) != d_OK)
		{
			vdSetErrorMessage("LOAD CPT ERR");
			return(d_NO);
		} 
    }

	
    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

	//if ((fBINVer == VS_TRUE) && ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) || (srTransRec.usTerminalCommunicationMode == ETHERNET_MODE) || (srTransRec.usTerminalCommunicationMode == WIFI_MODE)))		
	//		return(d_OK);
		
		

		
	//if ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) && (fGPRSConnectOK != TRUE)){
	//	vdSetErrorMessage("GPRS NOT ESTABLISHED");	
	//  return(d_NO);
	//}
		
    
    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);


    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
    {
        //vdSetErrorMessage("COMM INIT ERR");
        //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
		vdDisplayErrorMsgResp2("","Initialization","Error");
				vdSetErrorMessage("");
        return(d_NO);
    }

	//if(srTransRec.usTerminalCommunicationMode == WIFI_MODE)
		//return(d_OK);


	if (get_env_int("CONNECTED") == 1)
		return(d_OK);

	if(VS_TRUE == strTCT.fDemo)
		return(d_OK);
	
    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode); 

		if (inRetVal != d_OK)
		{
			if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
			{
				 vdDisplayErrorMsgResp2(" ", "GPRS Problem","Please Try Again");
				 vdSetErrorMessage("");
			}
			//wifi-mod2
			else if (srTransRec.usTerminalCommunicationMode == WIFI_MODE)		
			{
				vdDisplayErrorMsgResp2(" ", "WIFI Problem","Please Try Again");
				vdSetErrorMessage("");
			}
			//wifi-mod2

			else
			{
				//vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
				vdDisplayErrorMsgResp2("","Initialization","Error");
				vdSetErrorMessage("");
			}

			if(srTransRec.byTransType == SETTLE)
				vdDisplayErrorMsgResp2(" ", " ","SETTLE FAILED");
			
			return(d_NO);
		}

	
	if (CN_FALSE == srTransRec.byOffline)
    {   
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);        
				//for improve transaction speed
    }

	put_env_int("CONNECTED",1);

    return(d_OK);
}

int inCTOS_SelectHostEx2(void) 
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szStr[16+1]={0};

	//inDatabase_TerminalOpenDatabase();
	inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);

	/* BDO: Make sure we use the BDO Credit host details first - start -- jzg */
#if 0
	if(srTransRec.byTransType == BIN_VER)
		//inHostIndex = inGetCreditHostIndex();
		inHostIndex = inGetCreditHostIndexEx();
	else
#endif
	inHostIndex = (short)strCDT.HDTid;
	/* BDO: Make sure we use the BDO Credit host details first - end -- jzg */

	vdDebug_LogPrintf("inCTOS_SelectHost::Txn Type = [%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("inCTOS_SelectHost = [%d]", inHostIndex);

	if(inMultiAP_CheckSubAPStatus() == d_OK)
	{
		//inDatabase_TerminalCloseDatabase();
		return d_OK;
	}

	//if(inHDTRead(inHostIndex) != d_OK)
	if(inHDTReadEx(inHostIndex) != d_OK)
	{
		inHDTReadDataEx(inHostIndex);
		strcpy(szStr,strHDT_Temp.szHostLabel);
		memset(strHDT_Temp.szHostLabel,0x00,16+1);
		sprintf(strHDT_Temp.szHostLabel," %s ",szStr);
		vdDisplayErrorMsgResp2(strHDT_Temp.szHostLabel, "TRANSACTION", "NOT ALLOWED");
		inDatabase_TerminalCloseDatabase();
		return(d_NO);
	} 
	else 
	{
		srTransRec.HDTid = inHostIndex;

		inCurrencyIdx = strHDT.inCurrencyIdx;

		//if(inCSTRead(inCurrencyIdx) != d_OK) 
		if(inCSTReadEx(inCurrencyIdx) != d_OK) 
		{
			vdSetErrorMessage("LOAD CST ERR");
			inDatabase_TerminalCloseDatabase();
			return(d_NO);
		}

        if(strTCT.fSingleComms == TRUE)
			inHostIndex=1;
		//if(inCPTRead(inHostIndex) != d_OK)
		if(inCPTReadEx(inHostIndex) != d_OK)
		{
			vdSetErrorMessage("LOAD CPT ERR");
			inDatabase_TerminalCloseDatabase();
			return(d_NO);
		}

		if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		if (strCST.inCurrencyIndex > 1)
		{
			memset(szAscBuf, 0x00, sizeof(szAscBuf));	
			memset(szBcdBuf, 0x00, sizeof(szBcdBuf));
		
			sprintf(szAscBuf,"%4s",strCST.szCurCode);	
			
			wub_str_2_hex(szAscBuf, szBcdBuf, 4);
			memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);
			DebugAddHEX("T5F2A..",srTransRec.stEMVinfo.T5F2A,2);
		}
		//inDatabase_TerminalCloseDatabase();
		return (d_OK);
	}

	
	vdDebug_LogPrintf("inCTOS_SelectHost = [%d]", inHostIndex);
}

void vdSaveLastInvoiceNo(void)
{
	if(VS_FALSE == fRePrintFlag)
	    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);   
    if(inTCTSave(1) != ST_SUCCESS)
        vdDisplayErrorMsg(1, 8, "Update TCT fail");
}

void vdGetTimeDate(TRANS_DATA_TABLE *srTransPara)
{
#if 0
    char temp[40+1];

    memset(temp,0x00,sizeof(temp));
    sprintf(temp, "vdGetTimeDate::[%d]", srTransPara->byPackType);    
        CTOS_PrinterPutString(temp);
#endif

    //if(srTransPara->byPackType == SALE || srTransPara->byPackType == PRE_AUTH 
        //|| srTransPara->byPackType == LOG_ON 
        //||srTransPara->byPackType == BAL_INQ ||srTransPara->byPackType == CASH_ADV
        //||srTransPara->byPackType == PRE_COMP ||srTransPara->byPackType == REVERSAL)
    {
        CTOS_RTC SetRTC;
        BYTE szCurrentTime[20];

//        CTOS_PrinterPutString("vdGetTimeDate.1");
        
        CTOS_RTCGet(&SetRTC);
        memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,srTransPara->szDate,DATE_ASC_SIZE);

        memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        wub_str_2_hex(szCurrentTime,srTransPara->szTime,TIME_ASC_SIZE);


		memset(srTransPara->szDateTime, 0x00, sizeof(srTransPara->szDateTime));
        sprintf(srTransPara->szDateTime,"20%02d%02d%02d%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
		
		//memcpy(srTransPara->szTime, szCurrentTime, TIME_ASC_SIZE);
    }
}


int inCheckEthernetConnected(void)
{
	int shRet,usRtn;
	DWORD dwStatus;
	static USHORT usNetworkType = 0;
    static USHORT usEthType = 1;
    static BYTE szNetworkName[128+1] = {0};
	STRUCT_SHARLS_COM Sharls_COMData;
	
	vdDebug_LogPrintf("*** inCheckEthernetConnected start ***");
	//if ( CTOS_CradleAttached() == FALSE){
		//shRet = CTOS_EthernetOpenEx();
		//vdDebug_LogPrintf("*** CTOS_EthernetOpenEx [%d] ***");
		
		dwStatus = 0;
	
		usRtn = CTOS_EthernetStatus(&dwStatus);
		//flag = 0;
		vdDebug_LogPrintf("CTOS_EthernetStatus,usRtn=[%x],dwStatus=[%x]", usRtn, dwStatus);
		if (dwStatus & d_STATUS_ETHERNET_PHYICAL_ONLINE)
		{
			vdDebug_LogPrintf("ETHERNET CONNECTED");
			vdDebug_LogPrintf("BEFORE SUCCESS WITHOUT DOCK");
			return d_OK;
		}
		else
		{
			vdDebug_LogPrintf("ETHERNET DISCONNECTED");
			return ST_COMMS_DISCONNECT;
		}
			
	//}
#if 0
	else
	{
	
		inCTOSS_GetGPRSSignalEx1(&usNetworkType, szNetworkName, &usEthType, &Sharls_COMData);
		vdDebug_LogPrintf("usEthType[%s]",usEthType);
		if (usEthType != 1)//ETH connection not detected
		{
			vdDebug_LogPrintf("ETHERNET DISCONNECTED");
			return ST_COMMS_DISCONNECT;
		}
		else
		{
			vdDebug_LogPrintf("ETHERNET CONNECTED");
			vdDebug_LogPrintf("BEFORE SUCCESS WITH DOCK");
			return SUCCESS;
		}

	}
#endif
}

