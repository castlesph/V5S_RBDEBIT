#ifndef ___POS_SETTING_H___
#define ___POS_SETTING_H___

#define STR_HEAD            0
#define STR_BOTTOM          1
#define STR_ALL             2


void vdCTOS_uiPowerOff(void);
void vdCTOS_IPConfig(void);
int inCTOS_SelectHostSetting(void);
void vdCTOS_DialConfig(void);
int vdCTOS_ModifyEdcSetting(void);
void vdCTOS_DeleteBatch(void);
void vdCTOS_DeleteReversal(void);

void vdCTOS_PrintEMVTerminalConfig(void);
void vdCTOS_GPRSSetting(void);
void vdCTOS_Debugmode(void);
void vdCTOSS_CtlsMode(void);
void vdCTOS_TipAllowd(void);
void vdCTOS_DemoMode(void);
void DelCharInStr(char *str, char c, int flag);
void vdConfigEditAddHeader(void);
void vdCTOSS_DownloadMode(void);
void vdCTOSS_CheckMemory(void);
void CTOSS_SetRTC(void);
int inCTOSS_GetCtlsMode(void);
void vdCTOS_PrintDetailReportForManualSettlement(int shHostIndex);
int inCTOS_CommsFallback(int shHostIndex);

//gcitra
int inCTOS_DialBackupConfig(int shHostIndex);
//gcitra
/*albert - start - August2014 - manual settlement*/
int inCTOS_ManualSettle(void);
/*albert - end - August2014 - manual settlement*/

void vdCTOS_TMSSetting(void);
void vdCTOS_TMSReSet(void);
int  inCTOS_TMSPreConfigSetting(void);
int inAutoManualSettle(); //aaronnino for BDOCLG ver 9.0 fix on issue #00241 No Manual Settle/Clear Batch prompt after 3 failed
void vdCTOSS_DisplayAmount(USHORT usX, USHORT usY, char *szCurSymbol,char *szAmount);
void put_env_char(char *tag, char *value);

#endif //end ___POS_SETTING_H___

