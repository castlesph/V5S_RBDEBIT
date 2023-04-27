
#ifndef ___POS_BINVER___
#define ___POS_BINVER___

int inCTOS_BINCheckFlowProcess(void);
int inCTOS_BINCHECK(void);
int inCTOS_GetBINVerData(void);
int inCTOS_ProceedToSale(void);
int inUnPackBINVer63(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);


char szField63[999];

#endif //end ___POS_SALE___

