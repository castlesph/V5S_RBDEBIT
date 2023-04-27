
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
#include "../Ctls/POSCtls.h"

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
    int         inResult;
	int inTranCardType = 0;

    vdDebug_LogPrintf("inCTOS_UpdateAccumTotal");
    
    memset(szTransAmt, 0x00, sizeof(szTransAmt));
    memset(szTipAmt, 0x00, sizeof(szTipAmt));
    memset(szOrgAmt, 0x00, sizeof(szOrgAmt));
    wub_hex_2_str(srTransRec.szTotalAmount, szTransAmt, 6);
    wub_hex_2_str(srTransRec.szTipAmount, szTipAmt, 6);
    wub_hex_2_str(srTransRec.szOrgAmount, szOrgAmt, 6);

	if (srTransRec.inCardType == CREDIT_CARD)
	{
		inTranCardType = 0;//save credit card accue total
	}
    else
    {
    	inTranCardType = 1;//save debit card accue total
    }

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
	vdDebug_LogPrintf("CardTotal ReloadCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usReloadCCCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulReloadCCTotalAmount);

    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);
	vdDebug_LogPrintf("HostTotal ReloadpCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReloadCCCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReloadCCTotalAmount);
    
    switch(srTransRec.byTransType)
    {

//1010
	case RELOAD:
			/* BDO: Include Reload on count/amount -- sidumili */
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usReloadCCCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulReloadCCTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulReloadCCTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReloadCCCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReloadCCTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReloadCCTotalAmount + atof(szTransAmt);
			/* BDO: Include Reload on count/amount -- sidumili */	
		break;			  
//1010
        case SALE:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atof(szTransAmt);            
            if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
                srAccumRec.stBankTotal[inTranCardType].usEMVTCCount++;
            break;            
        case SALE_OFFLINE:
            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount + atof(szTransAmt);

            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atof(szTransAmt);
                    
            break;
        case REFUND:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount + atof(szTransAmt);
            
            break;
        case VOID:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount + atof(szTransAmt);
            
            if(srTransRec.byOrgTransType == SALE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szTransAmt);
            }
            else if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szTransAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount - atof(szTransAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount - atof(szTransAmt);

            }
            else if(srTransRec.byOrgTransType == SALE_TIP)
            {
                if (szOriginTipTrType == SALE_OFFLINE)
                {                    
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szOrgAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szOrgAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount - atof(szTransAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount - atof(szTransAmt);

                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount - atof(szTipAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount - atof(szTipAmt);
                }
                else if(szOriginTipTrType == SALE) 
                {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount - atof(szTipAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount - atof(szTipAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szTransAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szTransAmt);
                }
            }
            else if(srTransRec.byOrgTransType == REFUND)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount - atof(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount - atof(szTransAmt);
            }
            
            break;
        case SALE_TIP:
            if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount + atof(szTipAmt);  
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount + atof(szTipAmt) ;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount + atof(szTipAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount + atof(szTipAmt);
            }
            else 
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount + atof(szTipAmt);

				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atof(szTransAmt) - atof(szOrgAmt);
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atof(szTransAmt) - atof(szOrgAmt); 		   

                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount + atof(szTipAmt);
            }
            break;
        case SALE_ADJUST:            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szOrgAmt);
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szOrgAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atof(szTransAmt);
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atof(szTransAmt);
            break;
        default:
            break;
        
    }

    
    vdDebug_LogPrintf("szTotalAmount=[%s],szTipAmount=[%s],szOrgAmount=[%s],.inCardType[%d]",szTransAmt,szTipAmt,szOrgAmt,srTransRec.inCardType);
    vdDebug_LogPrintf("CardTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount);            
    vdDebug_LogPrintf("CardTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("CardTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount);
	vdDebug_LogPrintf("CardTotal ReloadCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usReloadCCCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulReloadCCTotalAmount);

    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);
	vdDebug_LogPrintf("HostTotal ReloadCount [%d] [%ld]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usReloadCCCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulReloadCCTotalAmount);
    
    if((inResult = inCTOS_SaveAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Save Total Rec. error");
		vdSetErrorMessage("Save Accum Error");
        return ST_ERROR;    
    }

   	inCTLOS_Updatepowrfail(PFR_BATCH_UPDATE_COMPLETE);

    vdMyEZLib_LogPrintf("total file saved successfully");

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
		return;
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




