/* 
 * File:   display.h
 * Author: wade
 *
 * Created on 2012年8月11日, 下午 6:08
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#define DISPLAY_POSITION_LEFT 0
#define DISPLAY_POSITION_CENTER 1
#define DISPLAY_POSITION_RIGHT 2
#define DISPLAY_LINE_SIZE 16

void vdDispErrMsg(IN BYTE *szMsg);
void   vdDispTransTitle(BYTE byTransType);
void vdDispTransTitleCardTypeandTotal(BYTE byTransType); //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE 2 of 6
void vdDispTitleString(BYTE *szTitle);
int    vdDispTransactionInfo(void);
void   setLCDPrint(int line,int position, char *pbBuf);
void   showAmount(IN  BYTE bY, BYTE bStrLen, BYTE *baBuf);
void   szGetTransTitle(BYTE byTransType, BYTE *szTitle);
void   vdDisplayTxnFinishUI(void);
USHORT showBatchRecord(TRANS_DATA_TABLE *strTransData);
void vduiLightOn(void);
void vduiKeyboardBackLight(BOOL fKeyBoardLight);
void vduiPowerOff(void);
void vduiClearBelow(int line);
void vduiDisplayStringCenter(unsigned char  y,unsigned char *sBuf);
void vduiWarningSound(void);
short vduiAskConfirmContinue(int inDisplay);
BYTE struiGetchWithTimeOut(void); 
void vduiDisplayInvalidTLE(void);
short inuiAskSettlement(void);
void vduiDisplaySignalStrengthBatteryCapacity(void);
USHORT clearLine(int line);

void vdDisplayErrorMsg(int inColumn, int inRow,  char *msg);
void vdDisplayErrorMsgResp (int inColumn, int inColumn2, int inColumn3, int inRow, int inRow2, int inRow3,  char *msg, char *msg2, char *msg3); //aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated 4 of 5
void vdDisplayErrorMsgResp2 (char *msg, char *msg2, char *msg3);


void vdSetErrorMessage(char *szMessage);
int inGetErrorMessage(char *szMessage);

//gcitra
void setLCDPrint27(int line,int position, char *pbBuf);
//gcitra

//sidumili:
void vdDisplayMessage(char *szLine1Msg, char *szLine2Msg, char *szLine3Msg);
short vduiAskEnterToConfirm(void);
short vduiPressAnyKey(void);
//sidumili:
void vdClearBelowLine(int inLine);


#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

