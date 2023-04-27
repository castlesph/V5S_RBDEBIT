
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

#include "..\Includes\CTOSInput.h"

#include "..\ui\Display.h"
#include "..\print\print.h"
#include "..\Debug\Debug.h"
#include "..\Includes\DMenu.h"
#include "..\Ctls\POSWave.h"


/*gcitra*/
#include "..\Includes\POSBinVer.h"
#include "..\Includes\POSAutoReport.h"
#include "..\Includes\POSLogon.h"
#include "..\Includes\POSBalanceInq.h"
#include "..\Includes\POSCashAdvance.h"
#include "..\Includes\PosReload.h"
/*gcitra*/
#include "..\TMS\TMS.h"


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
	{"vdCTOS_TipAllowd", (DMENU_FUN)vdCTOS_TipAllowd},
	{"vdCTOS_Debugmode", (DMENU_FUN)vdCTOS_Debugmode},
	{"vdCTOSS_CtlsMode", (DMENU_FUN)vdCTOSS_CtlsMode},
	{"vdCTOS_DemoMode", (DMENU_FUN)vdCTOS_DemoMode},	
	{"vdCTOSS_DownloadMode", (DMENU_FUN)vdCTOSS_DownloadMode},	
	{"vdCTOSS_CheckMemory", (DMENU_FUN)vdCTOSS_CheckMemory},	
	{"CTOSS_SetRTC", (DMENU_FUN)CTOSS_SetRTC},	
	{"vdCTOS_TMSSetting", (DMENU_FUN)vdCTOS_TMSSetting},
	{"vdCTOS_TMSReSet", (DMENU_FUN)vdCTOS_TMSReSet},
	{"inCTOSS_TMSDownloadRequest", inCTOSS_TMSDownloadRequest},
	//gcitra
	{"inCTOS_BINCHECK", (DMENU_FUN)inCTOS_BINCHECK},	
	{"inCTOS_POS_AUTO_REPORT", (DMENU_FUN)inCTOS_POS_AUTO_REPORT},	
	{"inCTOS_BALANCE_INQUIRY", inCTOS_BALANCE_INQUIRY},
	{"inCTOS_LOGON",inCTOS_LOGON},
	{"inCTOS_CASH_ADVANCE",inCTOS_CASH_ADVANCE},
	{"inCTOS_RELOAD",inCTOS_RELOAD},
	//gcitra
	{"vdCTOS_DeleteReversal", (DMENU_FUN)vdCTOS_DeleteReversal},
	/*albert - start - Aug2014 - manual settlement*/	
    {"inCTOS_ManualSettle", (DMENU_FUN)inCTOS_ManualSettle},
	/*albert - end - Aug2014 - manual settlement*/	
	
	{"", (DFUNCTION_LIST)NULL},
};

// can declare any functions type and link with string.
Func_inISOPack stFunctionList_inISOPack[] = {
	{"inPackIsoFunc02", inPackIsoFunc02},
	{"inPackIsoFunc03", inPackIsoFunc03},
	{"inPackIsoFunc04", inPackIsoFunc04},
	{"inPackIsoFunc11", inPackIsoFunc11},
	{"inPackIsoFunc12", inPackIsoFunc12},
	{"inPackIsoFunc13", inPackIsoFunc13},
	{"inPackIsoFunc14", inPackIsoFunc14},
	{"inPackIsoFunc18", inPackIsoFunc18},
	{"inPackIsoFunc22", inPackIsoFunc22},
	{"inPackIsoFunc23", inPackIsoFunc23},
	{"inPackIsoFunc24", inPackIsoFunc24},
	{"inPackIsoFunc25", inPackIsoFunc25},
	{"inPackIsoFunc35", inPackIsoFunc35},
	{"inPackIsoFunc37", inPackIsoFunc37},
	{"inPackIsoFunc38", inPackIsoFunc38},
	{"inPackIsoFunc39", inPackIsoFunc39},
	{"inPackIsoFunc41", inPackIsoFunc41},
	{"inPackIsoFunc42", inPackIsoFunc42},
	{"inPackIsoFunc45", inPackIsoFunc45},
	{"inPackIsoFunc48", inPackIsoFunc48},
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
	{"", (DFUNCTION_inISOPack)NULL},
};

// can declare any functions type and link with string.
Func_inISOUnPack stFunctionList_inISOUnPack[] = {
	{"inUnPackIsoFunc12", inUnPackIsoFunc12},
	{"inUnPackIsoFunc13", inUnPackIsoFunc13},
	{"inUnPackIsoFunc37", inUnPackIsoFunc37},
	{"inUnPackIsoFunc38", inUnPackIsoFunc38},
	{"inUnPackIsoFunc39", inUnPackIsoFunc39},
	{"inUnPackIsoFunc55", inUnPackIsoFunc55},
	{"inUnPackIsoFunc57", inUnPackIsoFunc57},
 	//gcitra
	{"inUnPackIsoFunc62", inUnPackIsoFunc62},
	{"inUnPackIsoFunc04", inUnPackIsoFunc04},
	//gcitra
	{"inUnPackBINVer63",  inUnPackBINVer63},
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

