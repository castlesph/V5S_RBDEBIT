
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>


#include "accum.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Debug\Debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\Includes\POSSetting.h"
#include "..\ui\Display.h"


void vdCTOS_GetAccumName(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal)
{
    sprintf(strFile->szFileName, "ACC%02d%02d.total"
                                , strHDT.inHostIndex
                                , srTransRec.MITid);
                                
    strFile->bSeekType           = d_SEEK_FROM_BEGINNING;
    strFile->bStorageType        = d_STORAGE_FLASH ;
    strFile->fCloseFileNow       = TRUE;
    strFile->ulRecSize           = sizeof(ACCUM_REC);
    strFile->ptrRec              = strTotal;    
    
}


int inCTOS_ReadAccumTotal(ACCUM_REC *strTotal)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    
    vdCTOS_GetAccumName(&strFile, strTotal);

    return (inMyFile_RecRead(&strFile));    
}

int inCTOS_SaveAccumTotal(ACCUM_REC *strTotal)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    
    vdCTOS_GetAccumName(&strFile, strTotal);

    return (inMyFile_RecSave(&strFile));    
}



int inCTOS_UpdateAccumTotal(void)
{
	ACCUM_REC srAccumRec;
    BYTE        szTransAmt[12+1];
    BYTE        szTipAmt[12+1];
    BYTE        szOrgAmt[12+1];
    BYTE        szOrgTipAmt[12+1];
	BYTE        szServiceFeeAmt[12+1];
	BYTE        szBaseAmt[12+1];
	BYTE szServiceFee[12+1];
	BYTE szAcquirerFee[12+1];
	BYTE szBillerFee[12+1];
    int         inResult;
	int inTranCardType = 0;

    vdDebug_LogPrintf("**inCTOS_UpdateAccumTotal** - start");
    
    memset(szTransAmt, 0x00, sizeof(szTransAmt));
    memset(szTipAmt, 0x00, sizeof(szTipAmt));
    memset(szOrgAmt, 0x00, sizeof(szOrgAmt));
    memset(szOrgTipAmt, 0x00, sizeof(szOrgTipAmt));
	memset(szServiceFeeAmt, 0x00, sizeof(szServiceFeeAmt));
	memset(szBaseAmt, 0x00, sizeof(szBaseAmt));
	memset(szServiceFee, 0x00, sizeof(szServiceFee));
	memset(szAcquirerFee, 0x00, sizeof(szAcquirerFee));
	memset(szBillerFee, 0x00, sizeof(szBillerFee));

    wub_hex_2_str(srTransRec.szTotalAmount, szTransAmt, 6);
    wub_hex_2_str(srTransRec.szTipAmount, szTipAmt, 6);
    wub_hex_2_str(srTransRec.szOrgAmount, szOrgAmt, 6);
    wub_hex_2_str(srTransRec.szOrgTipAmount, szOrgTipAmt, 6);
	//wub_hex_2_str(srTransRec.szServiceFee, szServiceFeeAmt, 6);
	wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmt, 6);

	memcpy(szServiceFee, srTransRec.szServiceFee, 12);
	memcpy(szAcquirerFee, srTransRec.szAcquirerFee, 12);
    memcpy(szBillerFee, srTransRec.szBillerFee, 12);
	
#if 1
    inTranCardType = 1;//save debit card accue total
#else
	if (srTransRec.inCardType == CREDIT_CARD)
	{
		inTranCardType = 0;//save credit card accue total
	}
    else
    {
    	inTranCardType = 1;//save debit card accue total
    }     
#endif

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;    
    }  
	
    vdDebug_LogPrintf("byTransType[%d].byOrgTransType[%d].szOriginTipTrType[%d]IITid[%d]", srTransRec.byTransType, srTransRec.byOrgTransType, szOriginTipTrType, srTransRec.IITid);
    
	vdDebug_LogPrintf("szTotalAmount=[%s],szTipAmount=[%s],szOrgAmount=[%s],.inCardType[%d]",szTransAmt,szTipAmt,szOrgAmt,srTransRec.inCardType);
    vdDebug_LogPrintf("CardTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount);            
    vdDebug_LogPrintf("CardTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("CardTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount);

    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);
    
    switch(srTransRec.byTransType)
    {
        case SALE:
        case LOY_REDEEM_5050:
        case LOY_REDEEM_VARIABLE: 	
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt);            
            if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
                srAccumRec.stBankTotal[inTranCardType].usEMVTCCount++;

			if(atol(szTipAmt) > 0)
			{
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount+=atol(szTipAmt);
				
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount+=atol(szTipAmt);
			}
            break;        

        case SALE_OFFLINE:
            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount + atol(szTransAmt);

            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt);

            if(atol(szTipAmt) > 0)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount+=atol(szTipAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount+=atol(szTipAmt);
            }
            break;
        case REFUND:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount + atol(szTransAmt);
            
            break;
        case VOID:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount + atol(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount + atol(szTransAmt);
            
            if(srTransRec.byOrgTransType == SALE || srTransRec.byOrgTransType == LOY_REDEEM_5050 || srTransRec.byOrgTransType == LOY_REDEEM_VARIABLE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szTransAmt);

				if(atol(szTipAmt) > 0)
				{
					srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
					srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount-=atol(szTipAmt);
					
					srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
					srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount-=atol(szTipAmt);
				}
            }
            else if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szTransAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount - atol(szTransAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount - atol(szTransAmt);

                if(atol(szTipAmt) > 0)
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount-=atol(szTipAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount-=atol(szTipAmt);
                }
            }
            else if(srTransRec.byOrgTransType == SALE_TIP)
            {
                if (szOriginTipTrType == SALE_OFFLINE)
                {                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szTransAmt);
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount - atol(szTransAmt);            
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount - atol(szTransAmt);
    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount - atol(szTipAmt);            
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount - atol(szTipAmt);
                }
                else if(szOriginTipTrType == SALE) 
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount - atol(szTipAmt);            
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount - atol(szTipAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szTransAmt);            
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szTransAmt);
                }
            }
            else if(srTransRec.byOrgTransType == REFUND)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount - atol(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount - atol(szTransAmt);
            }
            
            break;
        case SALE_TIP:
            if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);  

                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);

                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTransAmt) - atol(szOrgAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt) - atol(szOrgAmt);		   
            }
            else 
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount + atol(szTipAmt) - atol(szOrgTipAmt);
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTransAmt) - atol(szOrgAmt);

                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt) - atol(szOrgAmt); 		   
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount + atol(szTipAmt);
            }
            break;
			
        case SALE_ADJUST:            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atol(szOrgAmt);
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atol(szOrgAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atol(szTransAmt);
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atol(szTransAmt);
            break;
			
        case CASH_OUT:
            // CashOut Amount
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCashOutCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashOutTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashOutTotalAmount + atol(szBaseAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashOutCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashOutTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashOutTotalAmount + atol(szBaseAmt); 
        break;	
			
        case CASH_IN:			 
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCashInCount++;
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashInTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashInTotalAmount + atol(szBaseAmt);
			
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashInCount++;
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashInTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashInTotalAmount + atol(szBaseAmt);
        break;

        case BILLS_PAYMENT:			 
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usBillsPaymentCount++;
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBillsPaymentTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBillsPaymentTotalAmount + atol(szBaseAmt);
			
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBillsPaymentCount++;
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBillsPaymentTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBillsPaymentTotalAmount + atol(szBaseAmt);
        break;

        case FUND_TRANSFER:			 
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usFundTransferCount++;
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulFundTransferTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulFundTransferTotalAmount + atol(szBaseAmt);
			
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usFundTransferCount++;
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulFundTransferTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulFundTransferTotalAmount + atol(szBaseAmt);
        break;

		case BAL_INQ:
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usBalanceInquiryCount++;
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBalanceInquiryCount++;
		break;
		
        default:
            break;
        
    }

    if(srTransRec.byTransType == CASH_IN || srTransRec.byTransType == CASH_OUT || srTransRec.byTransType == FUND_TRANSFER
	|| srTransRec.byTransType == BAL_INQ)
    {		
		if(atol(szAcquirerFee) > 0)
		{
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usAcquirerFeeCount++;
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulAcquirerFeeTotalAmount= srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulAcquirerFeeTotalAmount+ atol(szAcquirerFee);
		
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usAcquirerFeeCount++;
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulAcquirerFeeTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulAcquirerFeeTotalAmount + atol(szAcquirerFee);
		}
		
    }
	else if(srTransRec.byTransType == BILLS_PAYMENT)
	{
		if(atol(szServiceFee) > 0)
		{
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usServiceFeeCount++;
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulServiceFeeTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulServiceFeeTotalAmount+ atol(szServiceFee);
			
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usServiceFeeCount++;
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulServiceFeeTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulServiceFeeTotalAmount + atol(szServiceFee);
		}	

		
		if(atol(szBillerFee) > 0)
		{
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usBillerFeeCount++;
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBillerFeeTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulBillerFeeTotalAmount+ atol(szBillerFee);
			
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBillerFeeCount++;
			srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBillerFeeTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulBillerFeeTotalAmount + atol(szBillerFee);
		}	
	}
	
    vdDebug_LogPrintf("**inCTOS_UpdateAccumTotal** - 1");
	
    vdDebug_LogPrintf("szTotalAmount=[%s],szTipAmount=[%s],szOrgAmount=[%s],.inCardType[%d],inTranCardType[%d],IITid[%d]",szTransAmt,szTipAmt,szOrgAmt,srTransRec.inCardType,inTranCardType,srTransRec.IITid);
    vdDebug_LogPrintf("CardTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount);            
    vdDebug_LogPrintf("CardTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("CardTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount);

	vdDebug_LogPrintf("**inCTOS_UpdateAccumTotal** - 2");

	vdDebug_LogPrintf("szTotalAmount=[%s],szTipAmount=[%s],szOrgAmount=[%s],.inCardType[%d],inTranCardType[%d],IITid[%d]",szTransAmt,szTipAmt,szOrgAmt,srTransRec.inCardType,inTranCardType,srTransRec.IITid);
    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);

	vdDebug_LogPrintf("**inCTOS_UpdateAccumTotal** - 3");
	
    if((inResult = inCTOS_SaveAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Save Total Rec. error");
		vdSetErrorMessage("Save Accum Error");
        return ST_ERROR;    
    }

	vdDebug_LogPrintf("**inCTOS_UpdateAccumTotal** - 4");
	
   	inCTLOS_Updatepowrfail(PFR_BATCH_UPDATE_COMPLETE);

    vdMyEZLib_LogPrintf("total file saved successfully");

	vdDebug_LogPrintf("**inCTOS_UpdateAccumTotal** - end");
	
    return ST_SUCCESS;
}


int inCTOS_ClearAccumTotal(void)
{
	short shHostIndex;
	int inResult;
	ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
	
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return ST_ERROR;
	
	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	memset(&strFile,0,sizeof(strFile));
	inResult = inCTOS_CheckAndSelectMutipleMID();
    vdCTOS_GetAccumName(&strFile, &srAccumRec);
	
	if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
    {
        vdMyEZLib_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
        return ST_ERROR;
    }     
	
    return ST_SUCCESS;
	
}

void vdCTOS_SetBackupAccumFile(char *szOriFileName)
{
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
	char szBKAccumeFileName[30];
	ULONG ulFileSize;
	int inResult;
	
	memset(&srAccumRec,0x00,sizeof(srAccumRec));
	memset(&strFile,0x00,sizeof(strFile));
	memset(szBKAccumeFileName,0x00,sizeof(szBKAccumeFileName));
	strcpy(szBKAccumeFileName,szOriFileName);
	CTOS_FileGetSize(szOriFileName, &ulFileSize);
	strcat(szBKAccumeFileName,".BK");
		
	vdDebug_LogPrintf("delete old BK acculFileSize[%d],del[%d]szOriFileName[%s][%s]",ulFileSize,inResult,szOriFileName,szBKAccumeFileName);
	inCTOS_FileCopy(szOriFileName,szBKAccumeFileName,ulFileSize);

	vdDebug_LogPrintf("szOriFileName[%s]set BKAccum[%s]ulFileSize[%ld]",szOriFileName,szBKAccumeFileName,ulFileSize);
	CTOS_FileGetSize(szBKAccumeFileName, &ulFileSize);
	vdDebug_LogPrintf("BK Acc ulFileSize[%ld]",ulFileSize);

	
}

void vdCTOS_GetBackupAccumFile(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal, int HostIndex, int MITid)
{

	sprintf(strFile->szFileName, "ACC%02d%02d.total%s"
								, HostIndex
								, MITid,".BK");
	
	vdDebug_LogPrintf("get BKAccum[%s]",strFile->szFileName);

								
	strFile->bSeekType			 = d_SEEK_FROM_BEGINNING;
	strFile->bStorageType		 = d_STORAGE_FLASH ;
	strFile->fCloseFileNow		 = TRUE;
	strFile->ulRecSize			 = sizeof(ACCUM_REC);
	strFile->ptrRec 			 = strTotal;	
		
}

int inCTOS_ReadBKAccumTotal(ACCUM_REC *strTotal,int HostIndex, int MITid)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    vdCTOS_GetBackupAccumFile(&strFile, strTotal,HostIndex, MITid);

    return (inMyFile_RecRead(&strFile));    
}

int inCTOS_DeleteBKAccumTotal(ACCUM_REC *strTotal,int HostIndex, int MITid)
{
	
    STRUCT_FILE_SETTING strFile;
	int inResult;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    vdCTOS_GetBackupAccumFile(&strFile, strTotal,HostIndex, MITid);
	
	if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
    {
        vdMyEZLib_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
        return ST_ERROR;
    }     
	
}
int inCTOS_FileCopy(char *szSourceFile, char *szDesFile,ULONG ulFileSize)
{

	FILE *in = NULL;
	FILE *out = NULL;
	unsigned long ulFileHandle;
	unsigned long ulOutFileHandle;
	char ch;
	ULONG ulFileSizeTemp;
	char *pAccumeData;
	int inresult;
	
	CTOS_FileGetSize(szSourceFile, &ulFileSizeTemp);
    inresult = CTOS_FileOpen(szSourceFile, d_STORAGE_FLASH, &ulFileHandle);
	vdDebug_LogPrintf("CTOS_FileOpen[%d]ulFileSizeTemp[%d]",inresult,ulFileSizeTemp);

	inresult = CTOS_FileSeek(ulFileHandle, 0, d_SEEK_FROM_BEGINNING);
	
	vdDebug_LogPrintf("CTOS_FileSeek[%d]ulFileSizeTemp[%d]",inresult,ulFileSizeTemp);
	pAccumeData = (unsigned char *) malloc(ulFileSizeTemp+1);
	vdDebug_LogPrintf("ulFileSizeTemp[%d]",ulFileSizeTemp);
	
	if(pAccumeData == NULL)
	{	
		vdDisplayErrorMsg(1, 8,	"accume bk fail");
		free(pAccumeData);
		return FAIL;
	}
	
	
    inresult = CTOS_FileRead(ulFileHandle, pAccumeData, &ulFileSizeTemp);
	
	vdDebug_LogPrintf("CTOS_FileRead[%d]ulFileSizeTemp[%d]ulFileSize[%d]",inresult,ulFileSizeTemp,ulFileSize);

	if((inresult  = CTOS_FileClose(ulFileHandle)) != d_OK)
	{				 
		vdDebug_LogPrintf("[CTOS_FileClose]---FileClz err[%04x]", inresult);
		free(pAccumeData);

		return ST_ERROR;
	}

	inresult = CTOS_FileDelete(szDesFile);
	vdDebug_LogPrintf("CTOS_FileDelete[%d]",inresult);


	vdDebug_LogPrintf("33333[%d]",ulFileSize);

	CTOS_Delay(50);
	
	
	inresult = CTOS_FileOpen(szDesFile , d_STORAGE_FLASH , &ulOutFileHandle);
	vdDebug_LogPrintf("CTOS_FileOpen[%d]",inresult);
	if(inresult == d_OK)
	{																						
		/* Move the file pointer to a specific position. 
		* Move backward from the end of the file.		 */
		inresult = CTOS_FileSeek(ulOutFileHandle, 0, d_SEEK_FROM_BEGINNING);
		if (inresult != d_OK)
		{
			vdDebug_LogPrintf("[inMyFile_RecSave]---Rec Seek inResult[%04x]", inresult);
			CTOS_FileClose(ulOutFileHandle);
			free(pAccumeData);
			return ST_ERROR;																		
		}
		else
			;
		/* Write data into this opened file */
		inresult = CTOS_FileWrite(ulOutFileHandle ,pAccumeData ,ulFileSize); 										
		if (inresult != d_OK)
		{
			vdDebug_LogPrintf("[inMyFile_RecSave]---Rec Write error, inResult[%04x]", inresult);
			CTOS_FileClose(ulOutFileHandle);
			
			free(pAccumeData);
			return ST_ERROR; 
		}																		
		
		vdDebug_LogPrintf("[inMyFile_RecSave]---Write finish,  inResult[%d]",  inresult);
		
		{	 
			if((inresult  = CTOS_FileClose(ulOutFileHandle)) != d_OK)
			{				 
				vdDebug_LogPrintf("[inMyFile_RecSave]---FileClz err[%04x]", inresult);
				free(pAccumeData);

				return ST_ERROR;
			}
			else
				ulOutFileHandle = 0x00;

			vdDebug_LogPrintf("[inMyFile_RecSave]---User close immed.");
		}

        vdDebug_LogPrintf("[inMyFile_RecSave]---User did not close immed.");
	}

	free(pAccumeData);
	
	vdDebug_LogPrintf(("last--CTOS_FileWrite,inresult[%d]", (char*)&inresult));
} 




