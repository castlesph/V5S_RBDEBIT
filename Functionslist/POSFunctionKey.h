
#ifndef POSFUNCTIONKEY_H
#define POSFUNCTIONKEY_H

void vdCTOS_FunctionKey(void);
void vdDisplayHostDetails(void);
void vdSTANNo(BOOL fGetSTABNo);
void vdTraceNo(BOOL fGetTraceNo);
void vdBatchNo(BOOL fGetBatchNo);
void vdShareCom(BOOL fGetShareCom);

extern int inCTOS_CheckAndSelectMutipleMID(void);
extern int inCTOS_BATCH_TOTAL_Process(void);
extern int inCTOS_BATCH_REVIEW_Process(void);
extern USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS);
extern unsigned int wub_hex_2_str(unsigned char *hex, unsigned char *str, unsigned int len);
extern unsigned long wub_bcd_2_long(unsigned char *buf, unsigned char len);
extern int inAscii2Bcd(BYTE *szAscii, BYTE *szBcd, int inBcdLen);
void vdCTOS_DevFunctionKey(void);

#endif //end POSFUNCTIONKEY_H

