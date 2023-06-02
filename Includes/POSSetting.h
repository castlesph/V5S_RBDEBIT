#ifndef ___POS_SETTING_H___
#define ___POS_SETTING_H___

#define STR_HEAD            0
#define STR_BOTTOM          1
#define STR_ALL             2


void vdCTOS_uiPowerOff(void);
void vdCTOS_IPConfig(void);
int inCTOS_SelectHostSetting(void);
void vdCTOS_DialConfig(void);
void vdCTOS_ModifyEdcSetting(void);
void vdCTOS_DeleteBatch(void);
void vdCTOS_DeleteReversal(void);

void vdCTOS_PrintEMVTerminalConfig(void);
void vdCTOS_GPRSSetting(void);
void vdCTOS_Debugmode(void);
void vdCTOSS_CtlsMode(void);
void vdCTOS_DemoMode(void);
void DelCharInStr(char *str, char c, int flag);
void vdConfigEditAddHeader(void);
void vdCTOSS_DownloadMode(void);
void vdCTOSS_CheckMemory(void);
void CTOSS_SetRTC(void);
int inCTOSS_GetCtlsMode(void);
int inCTOS_PromptPassword(void);
void vdCTOS_uiRestart(void);
void vdCTOSS_PrintTerminalConfig(void);
void vdCTOSS_SelectPinpadType(void);
void vdCTOSS_InjectMKKey(void);
void vdCTOS_TMSSetting(void);
int  inCTOS_TMSPreConfigSetting(void);
void put_env_char(char *tag, char *value);

#endif //end ___POS_SETTING_H___

