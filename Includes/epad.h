/* 
 * File:   dymaniclib.h
 * Author: PeyJiun
 *
 * Created on 2010�~12��6��, �U�� 8:57
 */

#ifndef _EPAD_H
#define	_EPAD_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct TagDISPLAY_REC
{
    BYTE        szDisplay1[50+1];
    BYTE        szDisplay2[50+1];
    BYTE        szDisplay3[50+1];
    BYTE        szDisplay4[50+1];
    BYTE        szDisplay5[50+1];
    BYTE        szDisplay6[50+1];
	BYTE        szDisplay7[50+1];
	BYTE        szDisplay8[50+1];
} DISPLAY_REC;


int ePad_SignatureCaptureLib(void);
int ePad_PrintSignatureCaptureLib(void);
int ePad_ClearSignatureCaptureLib(void);
int ePad_SignatureCaptureLibEx(void);
void vdCTOSS_CombineBMP(char *szBKBMPname, char *szOrgBMPname,char *szNewBMPname,int FromX1 , int FromY1, int FromX2, int FromY2,int ToX, int ToY);
void vdCTOSS_CombineMenuBMP(char *szBKBMPname);
void vdCTOSS_CombineIdleBMP(void);
void vdCTOSS_DelBackupS3DB(void);
void vdBackUpWhiteBMP(char *szBMPname,char *szOrgBMP);


int inCovertBMPToLGO(char *szBMPFileName, char *szLGOFileName);
int inCovertLGOToBMP(char *szLGOFileName, char *szBMPFileName);

void vdCTOSS_PrinterStart(int inLineCount);
void vdCTOSS_PrinterBMPPicEx(unsigned int usX,unsigned int y,  char *path);
int inCCTOS_PrinterBufferOutput(unsigned char* pchTemp,CTOS_FONT_ATTRIB* ATTRIB,int ERMCapture);
void vdCTOSS_PrinterEnd(void);
void vdCTOSS_SetLineSpace(int inLineSpace);
void vdCTOSS_PrinterEnd_CConvert2BMP(char *szBMPfilename);

int inCTOSS_SetERMFontType(char *szFontName,int inFontStyle);
int inCTOSS_FormERMReceipt(char *szFileName);
int inCTOSS_ReadERMSignatureFile(char *szSIGPfilename);



#ifdef	__cplusplus
}
#endif

#endif	/* _DYNAMICLIB_H */

