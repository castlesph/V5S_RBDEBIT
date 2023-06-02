
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <ctosapi.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "POSFunctionsList.h"
#include "..\Includes\POSTypedef.h"

#include <ctosapi.h>
#include <semaphore.h>
#include <pthread.h>

#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSAuth.h"
#include "..\Includes\POSConfig.h"
#include "..\Includes\POSOffline.h"
#include "..\Includes\POSRefund.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSBatch.h"
#include "..\Includes\POSSettlement.h"
#include "..\Includes\POSInstallment.h"
#include "..\Includes\POSLoyalty.h"
#include "..\Includes\POSDebit.h"
#include "..\Includes\PosTrans.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSCardVer.h"

#include "..\Includes\CTOSInput.h"

#include "..\ui\Display.h"
#include "..\print\print.h"
#include "..\Debug\Debug.h"
#include "..\Includes\DMenu.h"
#include "..\Ctls\POSWave.h"

#include "..\tms\tms.h"

extern TRANS_DATA_TABLE* srGetISOEngTransDataAddress(void);

#define MAXFUNCTIONS 		2048

// can declare any functions type and link with string.
Func_vdFunc stFunctionList_vdFunc[] = {
	{"inCTOS_SALE_OFFLINE", inCTOS_SALE_OFFLINE},
	{"inCTOS_WAVE_SALE", inCTOS_WAVE_SALE},
	{"inCTOS_WAVE_REFUND", inCTOS_WAVE_REFUND},
	{"vdCTOS_InitWaveData", (DMENU_FUN)vdCTOS_InitWaveData},
	{"inCTOS_VOID", inCTOS_VOID},
	{"inCTOS_SETTLEMENT", inCTOS_SETTLEMENT},
	{"inCTOS_SETTLE_ALL", inCTOS_SETTLE_ALL},
	{"inCTOS_PREAUTH", inCTOS_PREAUTH},
	{"inCTOS_REFUND", inCTOS_REFUND},
	{"inCTOS_TIPADJUST", inCTOS_TIPADJUST},
	{"inCTOS_BATCH_REVIEW", inCTOS_BATCH_REVIEW},
	{"inCTOS_BATCH_TOTAL", inCTOS_BATCH_TOTAL},
	{"inCTOS_REPRINT_LAST", inCTOS_REPRINT_LAST},
	{"inCTOS_REPRINT_ANY", inCTOS_REPRINT_ANY},
	{"inCTOS_REPRINTF_LAST_SETTLEMENT", inCTOS_REPRINTF_LAST_SETTLEMENT},
	{"inCTOS_PRINTF_SUMMARY", inCTOS_PRINTF_SUMMARY},
	{"inCTOS_PRINTF_DETAIL", inCTOS_PRINTF_DETAIL},
	{"inCTOS_PRINTF_SUMMARY", inCTOS_PRINTF_SUMMARY},
	{"inCTOS_PRINTF_DETAIL", inCTOS_PRINTF_DETAIL},
	{"inCTOS_REPRINT_LAST", inCTOS_REPRINT_LAST},
	{"inCTOS_REPRINT_ANY", inCTOS_REPRINT_ANY},
	{"inCTOS_REPRINTF_LAST_SETTLEMENT", inCTOS_REPRINTF_LAST_SETTLEMENT},
	{"vdCTOS_uiPowerOff", (DMENU_FUN)vdCTOS_uiPowerOff},
	{"vdCTOS_IPConfig", (DMENU_FUN)vdCTOS_IPConfig},
	{"vdCTOS_DialConfig", (DMENU_FUN)vdCTOS_DialConfig},
	{"vdCTOS_ModifyEdcSetting", (DMENU_FUN)vdCTOS_ModifyEdcSetting},
	{"vdCTOS_GPRSSetting", (DMENU_FUN)vdCTOS_GPRSSetting},
	{"vdCTOS_DeleteBatch", (DMENU_FUN)vdCTOS_DeleteBatch},
	{"vdCTOS_PrintEMVTerminalConfig", (DMENU_FUN)vdCTOS_PrintEMVTerminalConfig},
	{"vdCTOS_Debugmode", (DMENU_FUN)vdCTOS_Debugmode},
//	{"vdCTOSS_SelectPinpadType", (DMENU_FUN)vdCTOSS_SelectPinpadType},
//	{"vdCTOSS_InjectMKKey", (DMENU_FUN)vdCTOSS_InjectMKKey},
	{"vdCTOSS_CtlsMode", (DMENU_FUN)vdCTOSS_CtlsMode},
	{"vdCTOS_DemoMode", (DMENU_FUN)vdCTOS_DemoMode},	
	{"vdCTOSS_DownloadMode", (DMENU_FUN)vdCTOSS_DownloadMode},	
	{"vdCTOSS_CheckMemory", (DMENU_FUN)vdCTOSS_CheckMemory},	
	{"CTOSS_SetRTC", (DMENU_FUN)CTOSS_SetRTC},	
	{"inCTOS_SALE", (DMENU_FUN)inCTOS_SALE},	
    {"vdCTOS_PrintCRC", (DMENU_FUN)vdCTOS_PrintCRC},
	{"vdCTOS_HostInfo", (DMENU_FUN)vdCTOS_HostInfo},
    {"vdCTOS_FunctionKey", (DMENU_FUN)vdCTOS_FunctionKey},
	{"inCTOS_INSTALLMENT", (DMENU_FUN)inCTOS_INSTALLMENT},	
    {"inCTOS_DebitSelection", (DMENU_FUN)inCTOS_DebitSelection},	
    {"inCTOS_DebitSale", (DMENU_FUN)inCTOS_DebitSale},	
    {"inCTOS_DebitBalInq", (DMENU_FUN)inCTOS_DebitBalInq},	
    {"inCTOS_DebitLogon", (DMENU_FUN)inCTOS_DebitLogon},	
//	{"inCTOS_PromptPassword", inCTOS_PromptPassword},
	{"vdCTOS_DeleteReversal", vdCTOS_DeleteReversal},	
    {"inCTOS_Redeem5050", inCTOS_Redeem5050},	
    {"inCTOS_RedeemVariable", inCTOS_RedeemVariable},	
    {"inCTOS_LoyaltyBalanceInquiry", inCTOS_LoyaltyBalanceInquiry},			
	{"inCTOS_DinersINST", inCTOS_DinersINST}, 
    {"inCTOS_DinersSALE", inCTOS_DinersSALE}, 
    {"inCTOS_DinersCardVer", inCTOS_DinersCardVer},		
	{"inCTOS_DinersSaleOffline", inCTOS_DinersSaleOffline},
	{"inCTOS_DinersRefund", inCTOS_DinersRefund},
	{"inCTOS_StoreCardINST", inCTOS_StoreCardINST},
    {"inCTOSS_TMSDownloadRequest", inCTOSS_TMSDownloadRequest},
    {"vdCTOS_TMSSetting", vdCTOS_TMSSetting},
	{"", (DFUNCTION_LIST)NULL},
};

// can declare any functions type and link with string.
Func_inISOPack stFunctionList_inISOPack[] = {
	{"inPackIsoFunc02", inPackIsoFunc02},
	{"inPackIsoFunc03", inPackIsoFunc03},
	{"inPackIsoFunc04", inPackIsoFunc04},
	{"inPackIsoFunc05", inPackIsoFunc05},
	{"inPackIsoFunc07", inPackIsoFunc07},
	{"inPackIsoFunc11", inPackIsoFunc11},
	{"inPackIsoFunc12", inPackIsoFunc12},
	{"inPackIsoFunc13", inPackIsoFunc13},
	{"inPackIsoFunc14", inPackIsoFunc14},
	{"inPackIsoFunc18", inPackIsoFunc18},
	{"inPackIsoFunc22", inPackIsoFunc22},
	{"inPackIsoFunc23", inPackIsoFunc23},
	{"inPackIsoFunc24", inPackIsoFunc24},
	{"inPackIsoFunc25", inPackIsoFunc25},
	{"inPackIsoFunc26", inPackIsoFunc26},
	{"inPackIsoFunc28", inPackIsoFunc28},
	{"inPackIsoFunc35", inPackIsoFunc35},
	{"inPackIsoFunc37", inPackIsoFunc37},
	{"inPackIsoFunc38", inPackIsoFunc38},
	{"inPackIsoFunc39", inPackIsoFunc39},
	{"inPackIsoFunc41", inPackIsoFunc41},
	{"inPackIsoFunc42", inPackIsoFunc42},
	{"inPackIsoFunc43", inPackIsoFunc43},
	{"inPackIsoFunc45", inPackIsoFunc45},
	{"inPackIsoFunc47", inPackIsoFunc47},
	{"inPackIsoFunc48", inPackIsoFunc48},
	{"inPackIsoFunc50", inPackIsoFunc50},
	{"inPackIsoFunc52", inPackIsoFunc52},
	{"inPackIsoFunc54", inPackIsoFunc54},
	{"inPackIsoFunc55", inPackIsoFunc55},
	{"inPackIsoFunc56", inPackIsoFunc56},
	{"inPackIsoFunc57", inPackIsoFunc57},
	{"inPackIsoFunc60", inPackIsoFunc60},
	{"inPackIsoFunc61", inPackIsoFunc61},
	{"inPackIsoFunc62", inPackIsoFunc62},
	{"inPackIsoFunc63", inPackIsoFunc63},
	{"inPackIsoFunc64", inPackIsoFunc64},
	{"inPackIsoFunc84", inPackIsoFunc84}, // For BancNet CashOut -- sidumili
	{"inPackIsoFunc100", inPackIsoFunc100}, 
	{"inPackIsoFunc102", inPackIsoFunc102}, 
	{"inPackIsoFunc103", inPackIsoFunc103},
	{"", (DFUNCTION_inISOPack)NULL},
};

// can declare any functions type and link with string.
Func_inISOUnPack stFunctionList_inISOUnPack[] = {
	{"inUnPackIsoFunc02", inUnPackIsoFunc02},
	{"inUnPackIsoFunc04", inUnPackIsoFunc04},
	{"inUnPackIsoFunc05", inUnPackIsoFunc05},
	{"inUnPackIsoFunc07", inUnPackIsoFunc07},
	{"inUnPackIsoFunc11", inUnPackIsoFunc11},
	{"inUnPackIsoFunc12", inUnPackIsoFunc12},
	{"inUnPackIsoFunc13", inUnPackIsoFunc13},
	{"inUnPackIsoFunc28", inUnPackIsoFunc28},
	{"inUnPackIsoFunc37", inUnPackIsoFunc37},
	{"inUnPackIsoFunc38", inUnPackIsoFunc38},
	{"inUnPackIsoFunc39", inUnPackIsoFunc39},
	{"inUnPackIsoFunc41", inUnPackIsoFunc41},
    {"inUnPackIsoFunc42", inUnPackIsoFunc42},
	{"inUnPackIsoFunc43", inUnPackIsoFunc43},
	{"inUnPackIsoFunc50", inUnPackIsoFunc50},
    {"inUnPackIsoFunc53", inUnPackIsoFunc53},
	{"inUnPackIsoFunc54", inUnPackIsoFunc54},
	{"inUnPackIsoFunc55", inUnPackIsoFunc55},
	{"inUnPackIsoFunc57", inUnPackIsoFunc57},
	{"inUnPackIsoFunc61", inUnPackIsoFunc61},
	{"inUnPackIsoFunc84", inUnPackIsoFunc84},
	{"inUnPackIsoUnknown", inUnPackIsoUnknown},	
	{"", (DFUNCTION_inISOUnPack)NULL},
};

// can declare any functions type and link with string.
Func_inISOCheck stFunctionList_inISOCheck[] = {
	{"", (DFUNCTION_inISOCheck)NULL},
};

int inPOSFunctionList(void)
{		
}

int inCTOSS_ExeFunction(char *INuszFunctionName)
{
	int inDex, inRetVal = -1;

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
			if (stFunctionList_vdFunc[inDex].uszFunctionName[0]==0x00)
			{
		        vduiWarningSound();
				vduiDisplayStringCenter(7,INuszFunctionName);
				vduiDisplayStringCenter(8,"FUNCTION INVALID");
				break;
			}
			
			if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_vdFunc[inDex].uszFunctionName))
			{
			   vdDebug_LogPrintf("%s", INuszFunctionName); 	  			
			   inRetVal = stFunctionList_vdFunc[inDex].d_FunctionP();
			   break;
			}
	 }
	 return(inRetVal);
}

int inExeFunction_PackISO(char *INuszFunctionName, unsigned char *uszSendData)
{
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
		  if (stFunctionList_inISOPack[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOPack[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		 
			   inRetVal = stFunctionList_inISOPack[inDex].d_FunctionP(srTransPara, uszSendData);
			   break;
		  }
	 }
	 return(inRetVal);
}

int inExeFunction_UnPackISO(char *INuszFunctionName, unsigned char *uszReceiveData)
{	
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

	if (INuszFunctionName[0] == 0x00)
		return inRetVal;
		
	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
		  if (stFunctionList_inISOUnPack[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOUnPack[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		  
			   inRetVal = stFunctionList_inISOUnPack[inDex].d_FunctionP(srTransPara, uszReceiveData);
			   break;
		  }
	 }
	 return(inRetVal);
}

int inExeFunction_CheckISO(char *INuszFunctionName, unsigned char *uszSendData, unsigned char *uszReceiveData)
{
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;
    
    srTransPara = srGetISOEngTransDataAddress();

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
		  if (stFunctionList_inISOCheck[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOCheck[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		 		  
			   inRetVal = stFunctionList_inISOCheck[inDex].d_FunctionP(srTransPara, uszSendData, uszReceiveData);
			   break;
		  }
	 }
	 return(inRetVal);
}

