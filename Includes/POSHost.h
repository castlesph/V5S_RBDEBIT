
#ifndef ___POS_HOST___
#define ___POS_HOST___



int inCTOS_GetTxnBaseAmount(void);
int inCTOS_GetTxnTipAmount(void);
int inCTOS_UpdateTxnTotalAmount(void);
int inCTOS_GetInvoice(void);
int inCTOS_CustComputeAndDispTotal(void);

int inCTOS_DisplayResponse(void);
int inCTOS_BatchSearch(void);

int inCTOS_GeneralGetInvoice(void);
int inCTOS_CheckTipadjust(void);
int inCTOS_CheckVOID(void);
VS_BOOL fAmountLessThanFloorLimit(void) ;


int inCTOS_VoidSelectHost(void);
int inCTOS_ChkBatchEmpty(void);
int inCTOS_ConfirmInvAmt(void);
int inCTOS_GetTipAfjustAmount(void);
int inCTOS_GetOffApproveNO(void);
int inCTOS_SettlementSelectAndLoadHost(void);
int inCTOS_LoadCDTandIIT(void);
int inCTOS_DisplayBatchRecordDetail(int inType);
int inCTOS_SettlementClearBathAndAccum(BOOL fManualSettlement);

int inCTOS_DisplayBatchTotal(void);

int inCTOSS_DeleteAdviceByINV(BYTE *szInvoiceNo);
int inCTOSS_BatchCheckDuplicateInvoice(void);

//sidumili: added function
int inCTOS_ValidateTrxnAmount(void);

#endif //end ___POS_HOST___

