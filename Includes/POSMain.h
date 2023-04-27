
#ifndef ___POS_MAIN_H___
#define ___POS_MAIN_H___


int inCTOS_DisplayIdleBMP(void);
int inCTOS_DisplayComTypeICO(void);
int inCTOS_ValidFirstIdleKey(void);
BYTE chGetFirstIdleKey(void);
void vdSetFirstIdleKey(BYTE bFirstKey);
int inCTOS_IdleEventProcess(void);
int inCTOSS_InitAP(void);

int inDoAutosettlement(void);

//0723
int inPINEntryCount;
//0723

#endif //end ___POS_MAIN_H___

