/* 
 * File:   Print.h
 * Author: ASUS
 *
 * Created on 2012年8月25日, 上午 1:35
 */

#ifndef PRINT_H
#define	PRINT_H

#ifdef	__cplusplus
extern "C" {
#endif

#define ONE_LINE_DOT	"\n"

#define d_LINE_DOT 12
#define d_LINE_SIZE 44
#define d_FIRST_PAGE 1
#define d_SECOND_PAGE 2
#define d_THIRD_PAGE 3

#define BANK_COPY_RECEIPT 1
#define CUSTOMER_COPY_RECEIPT 2
#define MERCHANT_COPY_RECEIPT 3

#define TTF_FONT_REGULAR			0
#define TTF_FONT_ITALIC			1
#define TTF_FONT_BOLD				2
#define TTF_FONT_BOLD_ITALIC		3


#define NORMAL_SIZE	1
#define DOUBLE_SIZE	2

#define PRINT_SALE_REPORT 0
#define PRINT_REFUNT_REPORT 1
#define PRINT_SETTLE_REPORT 2

#define DIVIDING_LINE 1
#define DIVIDING_LINE_DOUBLE 2    
    
#define PRINT_HOST_TOTAL 0
#define PRINT_CARD_TOTAL 1    

#define SETTLE_PRINT 1
#define TOTAL_REPORT 0

#define PRINT_CARD_MASKING_1 0
#define PRINT_CARD_MASKING_2 1
#define PRINT_CARD_MASKING_3 3
#define PRINT_CARD_MASKING_4 4
#define PRINT_CARD_MASKING_5 5
#define V3_DESKTOP 1
#define V3_MOBILE 2

#define DB_SIGN_BMP "signtest.bmp"

#define SINGE_HEADER_FOOTER_TEXT 

short printCheckPaper(void);  
USHORT printDateTime(void);
USHORT ushCTOS_printReceipt(void);
USHORT ushCTOS_PrintAgreement(void);
USHORT batchAllRecord(BOOL isPrint);
void vdPrintEMVTags(void);
void vdPrintCenter(unsigned char *strIn);
USHORT printBatchInvoiceNO(void);
void setTotalCountandAmountCardWise(int i);
void PrintTotalCountandAmountCardWise (int inReportType, USHORT usSaleCount, 
    ULONG ulSaleTotalAmount,    
    USHORT usSaleAloneCount,
    ULONG  ulSaleAloneTotalAmount,
    USHORT usRefundCount,
    ULONG  ulRefundTotalAmount,    
    USHORT usVoidSaleCount,
    ULONG  ulVoidSaleTotalAmount,
    USHORT usOffSaleCount,
    ULONG  ulOffSaleTotalAmount);

void vdPrintTitleCenter(unsigned char *strIn);
void vdPrintTerminalConfig(void);
void printCardHolderName(void);

USHORT ushCTOS_printAll(int page);
USHORT ushCTOS_PrintHeader(int page);
USHORT ushCTOS_PrintBody(int page);
USHORT ushCTOS_PrintFooter(int page);
int inCTOS_rePrintReceipt(void);
int inCTOS_PrintSettleReport(BOOL fManualSettlement);

USHORT ushCTOS_printReceipt(void);
int inCTOS_REPRINT_ANY(void);
int inCTOS_REPRINT_LAST(void);
int inCTOS_PRINTF_SUMMARY(void);
int inCTOS_PRINTF_DETAIL(void);
int inCTOS_REPRINTF_LAST_SETTLEMENT(void);


int inCTOS_rePrintLastReceipt(void);
void vdCTOS_PrintSummaryReport(void);
void vdCTOS_PrintDetailReport(void);
USHORT ushCTOS_printErrorReceipt(void);
USHORT ushCTOS_printBatchRecordHeader(void);
USHORT ushCTOS_printBatchRecordFooter(void);
USHORT ushCTOS_GetFontInfor(void);
USHORT ushCTOS_ReprintLastSettleReport(void);
void vdCTOSS_PrinterBMPPic(unsigned int usX,unsigned int y, const char *path);


void vdCTOS_PrintAccumeByHostAndCard (int inReportType,
	USHORT usSaleCount, 
	double ulSaleTotalAmount,    
	USHORT usRefundCount,
	double  ulRefundTotalAmount,    
	USHORT usVoidSaleCount,
	double  ulVoidSaleTotalAmount,
	USHORT usOffSaleCount,
	double  ulOffSaleTotalAmount,
	//10101
	USHORT usReloadCount, 
	double  ulReloadTotalAmount	
	//10101
);

int inCTOS_SelectFont(int inFontMode,int inFontSize ,int inFontStyle,char * szFontName);

//gcitra
USHORT ushCTOS_PrintCashLoyaltyBody(void);
//int inPrintISOPacket(BOOL fSendPacket, unsigned char *pucMessage, int inLen);
int inPrintISOPacket(unsigned char *pucTitle,unsigned char *pucMessage, int inLen);

//gcitra

void vdCTOS_PrintManualPosting(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PRINT_H */

int inPrintLeftRight(unsigned char *strLeft, unsigned char *strRight, int inMode);

//sidumili: Fix on Issue#: 000149 [PRINT RECEIPT ON BALANCE INQ]
USHORT inCTOS_PrintBalanceInq(void);
int inPrint(unsigned char *strPrint);
void vdCTOSS_PrintFooterLogoBMPPic(unsigned int usX,unsigned int y, const char *path); //sidumili: Issue#000135 [Print Footer Logo]
void vdSetGolbFontAttrib(USHORT FontSize, USHORT X_Zoom, USHORT Y_Zoom, USHORT X_Space, USHORT Y_Space);
void vdTrimSpaces(char* pchString);
int inPrintLeftRight2(unsigned char *strLeft, unsigned char *strRight, int inMode);

//0720
#define DETAIL_REPORT_TOTAL 2
#define DETAIL_REPORT_GRANDTOTAL 3
//0720
void vdCTOS_PrintDetailTotals();

void vdRemoveSpace(char* szDestination, char* szSource);

void vdStrCat2(char szStr1[40], char szStr2[21], short shStr2Pos, char *szOutPut); /* BDOCLG-00134: Reformat cardtype line -- jzg */
int inCTOS_PRINT_DETAIL_ALL(void);
int inCTOS_PrintDetailAllOperation(void);
void vdCTOS_PrintDetailReportAll(void);

int inCTOS_PRINT_SUMMARY_ALL(void);
int inCTOS_PrintMerchantSummaryAllOperation(void);
void vdCTOS_PrintMerchantSummaryReportAll(void);
void vdCTOS_PrintMerchantSummaryReport(void);

void SetReportTime(void);

void printCardHolderNameV10(void);	// v10.0 change
USHORT ushCTOS_PrintSMHeader(int page);
USHORT ushCTOS_PrintSMBody(int page);
USHORT ushCTOS_PrintSMFooter(int page);
void vdCTOS_PrintReceiptCopies(BOOL BankCopy, BOOL CustCopy, BOOL MercCopy, BOOL fERM);
void vdLineFeed(BOOL fFooterLogo);
void vdPrintHeaderText(void);
void vdPrintFooterText(void);	
USHORT ushCTOS_PrintSMBodyError(int page);
void vdCTOS_PrinterFline(USHORT usLines);
void vdPrintISOOption(unsigned char *pucTitle,unsigned char *uszData, int inLen);
void vdCTOS_SettlementPrintDetailReport(BOOL fAddtoSettleReport, BOOL fLastSettleReport, BOOL fReprintLastSettleHeader);	
USHORT ushCTOS_ReprintBatchRecordHeader(void);
USHORT printLastSettleDateTime(void);
USHORT printBatchInvoiceNOEx(void);



