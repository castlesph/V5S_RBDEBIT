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
    double  ulSaleTotalAmount;
    USHORT usRefundCount;
    double  ulRefundTotalAmount;
    USHORT usTipCount;
    double  ulTipTotalAmount;   
    USHORT usVoidSaleCount;
    double  ulVoidSaleTotalAmount;
    USHORT usOffSaleCount;
    double  ulOffSaleTotalAmount; 

//1010
	USHORT usReloadCCCount;
	double  ulReloadCCTotalAmount;
//1010
}STRUCT_TOTAL;

typedef struct
{
    STRUCT_TOTAL stCardTotal[30];
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

//aaa fix on issue #000210 Terminal displays "Batch empty, skip" on all hosts when trying to settle hosts with no transactions 6 of 6 start
#define SINGLE_SETTLE 0
#define MULTI_SETTLE 1
//aaa fix on issue #000210 Terminal displays "Batch empty, skip" on all hosts when trying to settle hosts with no transactions 6 of 6 end


#ifdef	__cplusplus
}
#endif

#endif	/* TRANSFUNC_H */

