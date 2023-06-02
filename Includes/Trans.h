/* 
 * File:   TransFunc.h
 * Author: Administrator
 *
 * Created on 2012年8月23日, 下午 6:35
 */

#ifndef TRANSFUNC_H
#define	TRANSFUNC_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct  
{
    USHORT usSaleCount;
    ULONG  ulSaleTotalAmount;
    USHORT usRefundCount;
    ULONG  ulRefundTotalAmount;
    USHORT usTipCount;
    ULONG  ulTipTotalAmount;   
    USHORT usVoidSaleCount;
    ULONG  ulVoidSaleTotalAmount;
    USHORT usOffSaleCount;
    ULONG  ulOffSaleTotalAmount; 

    USHORT usRedeemCount;
    ULONG  ulRedeemTotalAmount; 

	//For Bancnet CashOut -- sidumili
	USHORT usCashOutCount;
	ULONG  ulCashOutTotalAmount;

    /*cash mo*/
	USHORT usCashInCount;
	ULONG  ulCashInTotalAmount;
	USHORT usBillsPaymentCount;
	ULONG  ulBillsPaymentTotalAmount;
	USHORT usBalanceInquiryCount;
	USHORT usFundTransferCount;
	ULONG  ulFundTransferTotalAmount;
	USHORT usServiceFeeCount;
	ULONG  ulServiceFeeTotalAmount;
	USHORT usAcquirerFeeCount;
	ULONG  ulAcquirerFeeTotalAmount;
	USHORT usBillerFeeCount;
	ULONG  ulBillerFeeTotalAmount;
    /*cash mo*/	
}STRUCT_TOTAL;

typedef struct
{
    STRUCT_TOTAL stCardTotal[20];
    STRUCT_TOTAL stHOSTTotal;
    USHORT usEMVTCCount;
}TRANS_TOTAL;


BYTE szOriginTipTrType;


typedef struct
{
    STRUCT_TOTAL stSettledCardTotal[10];
    BYTE	szSettledCardLabel[10][25] ;
    BYTE        szSettledDate[3+1];
    BYTE	szSettledBatchNo[3+1];
    BYTE        szSettledTime[3+1];
    BYTE	szSettledHostLabel[16] ;
    BYTE	szSettledMID[15+1];
    BYTE	szSettledTID[8+1];
}SETTLEDTRANS_TOTAL;



#ifdef	__cplusplus
}
#endif

#endif	/* TRANSFUNC_H */

