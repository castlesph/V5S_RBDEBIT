
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

#include "MultiAptrans.h"
#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSRefund.h"
#include "..\Includes\POSAuth.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSSettlement.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSOffline.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\MultiApLib.h"

#include "..\Print\Print.h"
#include "..\UI\Display.h"
#include "..\Debug\Debug.h"
#include "..\FileModule\myFileFunc.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\powrfail\POSPOWRFAIL.h"
#include "..\Ctls\POSWave.h"

#include "..\Includes\DMenu.h"
#include "..\ApTrans\MultiApTrans.h"
#include "..\Includes\POSAutoReport.h"
#include "..\Includes\POSBalanceInq.h"
#include "..\Includes\POSCashAdvance.h"
#include "..\Includes\POSLogon.h"
#include "..\Includes\PosReload.h"
#include "..\Includes\CTOSInput.h"


// patrick add code 20141205 start
static int inSendECRResponseFlag = 0;
char inSendECRResponseCommandID[2+1];
//int inIPC_KEY_SEND_Parent = 0;
BOOL fAutoSettle=FALSE;

extern int inLogonSingleHost;

extern int inCTOS_BINCHECK(void);
//int inIPC_KEY_SEND_Parent = 0;

/* For ECR -- sidumili */
extern BOOL fECRBuildSendOK; 
extern BOOL fTimeOutFlag; /*BDO: Flag for timeout --sidumili*/
extern BOOL fLogon;
/* For ECR -- sidumili */

extern BOOL fRePrintFlag;
extern BOOL fERMTransaction;
// patrick this function need provide to application level and response code
/*================================================================
  Handle IPC function.
  [Return] IPC process status return code
  ================================================================ */
USHORT inMultiAP_HandleIPC(BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int inRet = d_NO;
	USHORT len;

	len = 0;
	fRePrintFlag = FALSE; //initialize reprint flag to false
    //ECR txn no need this, make it fast
    //if(0 == fGetECRTransactionFlg()) /*BDO PHASE 2: Suggesed by Hu Bing in relation with swithing apps*/
    {
        //if main APP change the parameter, sub need read again
    	//inTCTRead(1);
		
		inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);//Optimize Start
		inTCTReadEx(1);
		inFLGReadEx(1);
		inDatabase_TerminalCloseDatabase();
		
		if(strTCT.byERMMode)
		   fERMTransaction=TRUE;
		else
		   fERMTransaction=FALSE;

    	//inCPTRead(1); /*BDO PHASE 2: Suggesed by Hu Bing in relation with swithing apps*/
    	//inPCTRead(1); /*BDO PHASE 2: Suggesed by Hu Bing in relation with swithing apps*/
    	//inCSTRead(1); /*BDO PHASE 2: Suggesed by Hu Bing in relation with swithing apps*/
    	//inTCPRead(1); /*BDO PHASE 2: Suggesed by Hu Bing in relation with swithing apps*/
    }

		// patrick add code 20141205 start
	inSendECRResponseCommandID[0] = inbuf[0];
	vdDebug_LogPrintf("inMultiAP_HandleIPC::[%02X][%02X][%02X]", inbuf[0], inbuf[1], inbuf[2]);
		
	/* Process IPC command code action */
	switch (inbuf[0])
	{
	case d_IPC_CMD_ONLINES_SALE:
		inRet = inCTOS_SALE();
		outbuf[len++] = d_IPC_CMD_ONLINES_SALE;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;

	case d_IPC_CMD_WAVE_SALE:
			inRet = inCTOS_WAVE_SALE();
			outbuf[len++] = d_IPC_CMD_WAVE_SALE;
			if (inRet == d_OK)
				outbuf[len++] = d_SUCCESS;
			else
				outbuf[len++] = d_FAIL;
			outbuf[len] = 0x00;
			break;

	case d_IPC_CMD_WAVE_REFUND:
		inRet = inCTOS_WAVE_REFUND();
		outbuf[len++] = d_IPC_CMD_WAVE_REFUND;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;


/*	case d_IPC_CMD_OFFLINE_SALE:
		inRet = inCTOS_SALE_OFFLINE();
		outbuf[len++] = d_IPC_CMD_OFFLINE_SALE;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_PRE_AUTH:
		inRet = inCTOS_PREAUTH();
		outbuf[len++] = d_IPC_CMD_PRE_AUTH;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;
*/
	case d_IPC_CMD_OFFLINE_SALE:
	case d_IPC_CMD_PRE_AUTH:
		//vdDisplayErrorMsg(1, 8, "TRANS NOT ALLOWED");
		vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
		break;
		
	case d_IPC_CMD_REFUND:
		inRet = inCTOS_REFUND();
		outbuf[len++] = d_IPC_CMD_REFUND;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_POWER_FAIL:
		inRet = inCTLOS_Getpowrfail();
		outbuf[len++] = d_IPC_CMD_POWER_FAIL;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;
		
	case d_IPC_CMD_GET_PID:
//		inMultiAP_APforkMainroutine();
		outbuf[len++] = d_IPC_CMD_GET_PID;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_VOID_SALE:
		inRet = inCTOS_VOID();
		outbuf[len++] = d_IPC_CMD_VOID_SALE;
		
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;

	case d_IPC_CMD_TIP_ADJUST:
		inRet = inCTOS_TIPADJUST();
		outbuf[len++] = d_IPC_CMD_TIP_ADJUST;
		
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;

	case d_IPC_CMD_SETTLEMENT:
		inCTOS_SETTLEMENT();
		outbuf[len++] = d_IPC_CMD_SETTLEMENT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_SETTLE_ALL:
		inRet = inCTOS_SETTLE_ALL();
		outbuf[len++] = d_IPC_CMD_SETTLE_ALL;
		if (inRet == d_OK)
			outbuf[len++] = d_OK;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
    break;
    
    case d_IPC_CMD_SETTLE_MERCHANT_ALL:
			  fAutoSettle=TRUE;
        inRet = inCTOS_SETTLE_ALL();
        outbuf[len++] = d_IPC_CMD_SETTLE_MERCHANT_ALL;
        if (inRet == d_OK)
        outbuf[len++] = d_OK;
        else
        outbuf[len++] = d_FAIL;
        outbuf[len] = 0x00;
    break;
		
    case d_IPC_CMD_CHK_BATCH_EMPTY:
		inRet = inCTOSS_TMSChkBatchEmpty();
		outbuf[len++] = d_IPC_CMD_CHK_BATCH_EMPTY;
		if (inRet == d_OK)
			outbuf[len++] = d_OK;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_BATCH_REVIEW:
		inCTOS_BATCH_REVIEW();
		outbuf[len++] = d_IPC_CMD_BATCH_REVIEW;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_BATCH_TOTAL:
		inCTOS_BATCH_TOTAL();
		outbuf[len++] = d_IPC_CMD_BATCH_TOTAL;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_REPRINT_LAST:
		inRet = inCTOS_REPRINT_LAST();
		outbuf[len++] = d_IPC_CMD_REPRINT_LAST;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_DELETE_BATCH:
		vdCTOS_DeleteBatch();
		outbuf[len++] = d_IPC_CMD_DELETE_BATCH;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	//1027
		case d_IPC_CMD_MANUAL_SETTLE:
			inCTOS_ManualSettle();
			outbuf[len++] = d_IPC_CMD_MANUAL_SETTLE;
			outbuf[len++] = d_SUCCESS;
			outbuf[len] = 0x00;
	//1027
    case d_IPC_CMD_DELETE_REVERSAL:
		vdCTOS_DeleteReversal();
		outbuf[len++] = d_IPC_CMD_DELETE_REVERSAL;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_REPRINT_ANY:
		inRet = inCTOS_REPRINT_ANY();
		outbuf[len++] = d_IPC_CMD_REPRINT_ANY;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_REPRINT_LAST_SETT:
		inCTOS_REPRINTF_LAST_SETTLEMENT();
		outbuf[len++] = d_IPC_CMD_REPRINT_LAST_SETT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_SUMMARY_REPORT:
		inCTOS_PRINTF_SUMMARY();
		outbuf[len++] = d_IPC_CMD_SUMMARY_REPORT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_DETAIL_REPORT:
		inCTOS_PRINTF_DETAIL();
		outbuf[len++] = d_IPC_CMD_DETAIL_REPORT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

    case d_IPC_CMD_CHECK_DUP_INV:
		inCTOSS_BatchCheckDuplicateInvoice();
		outbuf[len++] = d_IPC_CMD_CHECK_DUP_INV;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
	case d_IPC_CMD_FUN_KEY1:
		inF1KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY1;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY2:
		inF2KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY2;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY3:
		inF3KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY3;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY4:
		inF4KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY4;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;		break;

	//gcitra
	case d_IPC_CMD_BINVER:

		if (strTCT.fEnableDebitBinCheck == VS_FALSE){	
			//vdDisplayErrorMsg(1, 8, "TRANS NOT ALLOWED");
			vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
			break;
		}
				
		inRet = inCTOS_BINCHECK();
		outbuf[len++] = d_IPC_CMD_BINVER;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
			outbuf[len] = 0x00;
		break;
	
	case d_IPC_CMD_POS_AUTO_REPORT:
		inRet = inCTOS_POS_AUTO_REPORT();
		outbuf[len++] = d_IPC_CMD_POS_AUTO_REPORT;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_BALANCE_INQUIRY:
		inRet = inCTOS_BALANCE_INQUIRY();
		outbuf[len++] = d_IPC_CMD_BALANCE_INQUIRY;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_CASH_ADVANCE:
		inRet = inCTOS_CASH_ADVANCE();
		outbuf[len++] = d_IPC_CMD_CASH_ADVANCE;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_LOGON:
		inRet = inCTOS_LOGON();
		outbuf[len++] = d_IPC_CMD_LOGON;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;
		
	case d_IPC_CMD_RELOAD:
		inRet = inCTOS_RELOAD();
		outbuf[len++] = d_IPC_CMD_RELOAD;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;


	//gcitra
	case d_IPC_CMD_EDC_SETTING:		
		inRet = vdCTOS_ModifyEdcSetting();
		outbuf[len++] = d_IPC_CMD_EDC_SETTING;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
			outbuf[len] = 0x00;
		break;

    case d_IPC_CMD_DETAIL_REPORT_ALL:
        inCTOS_PRINT_DETAIL_ALL();
        outbuf[len++] = d_IPC_CMD_DETAIL_REPORT_ALL;
        //outbuf[len++] = d_SUCCESS;
        outbuf[len++]=d_OK;
        outbuf[len] = 0x00;
    break;

	
		
    case d_IPC_CMD_MERCHANT_CHECK_BATCH_ALL:
        inCTOS_MerchantCheckBatchAllOperation();
        outbuf[len++] = d_IPC_CMD_MERCHANT_CHECK_BATCH_ALL;
        //outbuf[len++] = d_SUCCESS;
        outbuf[len++]=d_OK;
        outbuf[len] = 0x00;
    break;



    case d_IPC_CMD_SUMMARY_REPORT_ALL:
        inCTOS_PRINT_SUMMARY_ALL();
        outbuf[len++] = d_IPC_CMD_DETAIL_REPORT_ALL;
        //outbuf[len++] = d_SUCCESS;
        outbuf[len++]=d_OK;
        outbuf[len] = 0x00;
    break;

    case d_IPC_CMD_ERM_INIT:
	inCTOSS_ERMInit();	
	outbuf[len++] = d_IPC_CMD_ERM_INIT;
	//outbuf[len++] = d_SUCCESS;
	outbuf[len] = 0x00;
    break;

	case d_IPC_CMD_GET_CRC_ALL:
	vdGetCRC_ALL();	
	outbuf[len++] = d_IPC_CMD_GET_CRC_ALL;
	//outbuf[len++] = d_SUCCESS;
	outbuf[len] = 0x00;
    break;
	
	default:
		outbuf[len++] = 0xFF;
		break;	
	}

	inTCTSave(1);
	
	*outlen = len;

	CTOS_LCDTClearDisplay();

	return len;
}

int inMultiAP_RunIPCCmdTypes(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int insendloop = 0;
	int status;
	int ipc_len;
	pid_t wpid;	
	BYTE processID[100];
	pid_t pid = -1;
	USHORT IPC_IN_LEN, IPC_OUT_LEN;
	BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
	BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
	char szAppname[100+1];

	strcpy(szAppname, Appname);

	memset(processID,0x00,sizeof(processID));
	vdMultiAP_getPID(szAppname,processID);
	pid = atoi(processID);

	vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes =[%d][%s]",pid, szAppname);
	if (pid <= 0)
	{
		inCTOS_ReForkSubAP(szAppname);
		memset(processID,0x00,sizeof(processID));
		vdMultiAP_getPID(szAppname,processID);
		pid = atoi(processID);
		//here need delay, wait sub AP initialization 
		if (pid > 0)
			CTOS_Delay(3000);
		
		vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes22 =[%d][%s]",pid, szAppname);
	}

	if (pid > 0)
	{
//		inIPC_KEY_SEND_Parent = pid;
		IPC_IN_LEN = 0;
		memset(IPC_IN_BUF_STR,0x00,sizeof(IPC_IN_BUF_STR));
		memset(IPC_OUT_BUF_STR,0x00,sizeof(IPC_OUT_BUF_STR));
		
		IPC_IN_BUF[IPC_IN_LEN ++] = IPC_EVENT_ID;
		if (inlen > 0)
		{
			memcpy(&IPC_IN_BUF[IPC_IN_LEN], inbuf, inlen);
			IPC_IN_LEN = IPC_IN_LEN + inlen;
		}
		
		do
		{
			if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
			{
				vdDebug_LogPrintf("Multi UnForkAPName=[%s] ....", szAppname);
				break;
			}
			memset(IPC_IN_BUF_STR, 0x00, sizeof(IPC_IN_BUF_STR));
			inlen = wub_hex_2_str(IPC_IN_BUF ,IPC_IN_BUF_STR ,IPC_IN_LEN);
			IPC_OUT_LEN = inMultiAP_IPCCmdParentEx(IPC_IN_BUF_STR, IPC_OUT_BUF_STR, pid);
			wub_str_2_hex(IPC_OUT_BUF_STR, IPC_OUT_BUF, strlen(IPC_OUT_BUF_STR));
			
			//vdDebug_LogPrintf("CloseAllDrvHandle IPC_OUT_LEN=[%d][%2X][%d]",IPC_OUT_LEN, IPC_OUT_BUF[0],IPC_IN_LEN);

			if (IPC_OUT_BUF[0] == d_SUCCESS)
			{
				break;
			}

			insendloop++;
			if (insendloop > 2)
			{
				if (insendloop > 3)
					return d_NO;
				
				inCTOS_ReForkSubAP(szAppname);
				memset(processID,0x00,sizeof(processID));
				vdMultiAP_getPID(szAppname,processID);
				pid = atoi(processID);
				vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes22 =[%d][%s]",pid, szAppname);
				//here need delay, wait sub AP initialization 
				if (pid > 0)
					CTOS_Delay(3000);
				else
					return d_NO;
			}
		} while (1);

		vdDebug_LogPrintf("inWaitTime =[%d]",strTCT.inWaitTime);
		if (strTCT.inWaitTime <= 0)
			strTCT.inWaitTime = 100;
		memset(IPC_OUT_BUF_STR, 0x00, sizeof(IPC_OUT_BUF_STR));
		while ( (wpid=waitpid(pid, &status, WNOHANG)) != pid )
		{			
			memset(processID,0x00,sizeof(processID));
			vdMultiAP_getPID(szAppname,processID);
			if (atoi(processID) <= 0)
			{
				inCTOS_ReForkSubAP(szAppname);
				return d_NO;
			}

			CTOS_Delay(strTCT.inWaitTime);
			//CTOS_Delay(150);

			ipc_len = d_MAX_IPC_BUFFER;
			// patrick need hendle check status if transaction fail or success.
			if (inMultiAP_IPCGetParent(IPC_OUT_BUF_STR, &ipc_len) == d_OK)
			{
				ipc_len = wub_str_2_hex(IPC_OUT_BUF_STR, outbuf, ipc_len);
				*outlen = ipc_len;
				return d_OK;
			}
		}
	}
	else
	{
		return d_NO;
	}
}

int inMultiAP_RunIPCCmdTypesEx(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int insendloop = 0;
	int status;
	int ipc_len;
	pid_t wpid;	
	BYTE processID[100];
	pid_t pid = -1;
	USHORT IPC_IN_LEN, IPC_OUT_LEN;
	BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
	BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
	char szAppname[100+1];

	strcpy(szAppname, Appname);

	memset(processID,0x00,sizeof(processID));
	vdMultiAP_getPID(szAppname,processID);
	pid = atoi(processID);

	vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypesEx =[%d][%s],[%x]",pid, szAppname,IPC_EVENT_ID);
	if (pid <= 0)
	{
		inCTOS_ReForkSubAP(szAppname);
		memset(processID,0x00,sizeof(processID));
		vdMultiAP_getPID(szAppname,processID);
		pid = atoi(processID);
		//here need delay, wait sub AP initialization 
		if (pid > 0 && d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
			CTOS_Delay(3000);
		
		vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypesEx22 =[%d][%s]",pid, szAppname);
	}

	if (pid > 0)
	{
//		inIPC_KEY_SEND_Parent = pid;
		IPC_IN_LEN = 0;
		memset(IPC_IN_BUF_STR,0x00,sizeof(IPC_IN_BUF_STR));
		memset(IPC_OUT_BUF_STR,0x00,sizeof(IPC_OUT_BUF_STR));
		
		IPC_IN_BUF[IPC_IN_LEN ++] = IPC_EVENT_ID;
		if (inlen > 0)
		{
			memcpy(&IPC_IN_BUF[IPC_IN_LEN], inbuf, inlen);
			IPC_IN_LEN = IPC_IN_LEN + inlen;
		}
		
		do
		{
			memset(IPC_IN_BUF_STR, 0x00, sizeof(IPC_IN_BUF_STR));
			inlen = wub_hex_2_str(IPC_IN_BUF ,IPC_IN_BUF_STR ,IPC_IN_LEN);
			IPC_OUT_LEN = inMultiAP_IPCCmdParentEx(IPC_IN_BUF_STR, IPC_OUT_BUF_STR, pid);
			wub_str_2_hex(IPC_OUT_BUF_STR, IPC_OUT_BUF, strlen(IPC_OUT_BUF_STR));
			
			//vdDebug_LogPrintf("CloseAllDrvHandle IPC_OUT_LEN=[%d][%2X][%d]",IPC_OUT_LEN, IPC_OUT_BUF[0],IPC_IN_LEN);

			if (IPC_OUT_BUF[0] == d_SUCCESS)
			{
				break;
			}

			insendloop++;
			if (insendloop > 0)
			{
				vdDebug_LogPrintf("inMultiAP_IPCCmdParentEx TIMEOUT.!!!");
				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					vdDebug_LogPrintf("inCTOS_Kill unfork SubAP = [%s]",szAppname);
					inCTOS_KillASubAP(szAppname);
				}
            	return d_OK;
			}
		} while (1);

		vdDebug_LogPrintf("inWaitTime =[%d]",strTCT.inWaitTime);
		if (strTCT.inWaitTime <= 0)
			strTCT.inWaitTime = 100;
		
		CTOS_TimeOutSet (TIMER_ID_1 , 500);
		memset(IPC_OUT_BUF_STR, 0x00, sizeof(IPC_OUT_BUF_STR));
		while ( (wpid=waitpid(pid, &status, WNOHANG)) != pid )
		{
			CTOS_Delay(strTCT.inWaitTime);

			if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
			{
				vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypesEx TIMEOUT.!!!");
            	return d_OK;
			}

			ipc_len = d_MAX_IPC_BUFFER;
			// patrick need hendle check status if transaction fail or success.
			if (inMultiAP_IPCGetParent(IPC_OUT_BUF_STR, &ipc_len) == d_OK)
			{
				ipc_len = wub_str_2_hex(IPC_OUT_BUF_STR, outbuf, ipc_len);
				*outlen = ipc_len;

				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					inCTOS_KillASubAP(szAppname);
				}
				
				return d_OK;
			}
		}
	}
	else
	{
		return d_NO;
	}
}


int inMultiAP_SendChild(BYTE *inbuf, USHORT inlen)
{
	BYTE bret;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	int out_len = 0;
	memset(outbuf,0x00,sizeof(outbuf));
	
	out_len = wub_hex_2_str(inbuf ,outbuf ,inlen);

	bret = inMultiAP_IPCSendChild(outbuf , out_len);
	
	//vdDebug_LogPrintf("inMultiAP_SendChild=[%s] [%ld] [%d],[%d]",outbuf, out_len,inlen,bret);

	return bret;
}

int inMultiAP_ECRSendSuccessResponse(void)
{
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;

	BYTE szTemp4[d_LINE_SIZE + 1] = {0};
	BYTE szTemp5[d_LINE_SIZE + 1] = {0};
	BYTE szTemp6[d_LINE_SIZE + 1] = {0};
	char szBuffer[100+1];
	char szHeader[23+1];
	char szMsg1[40], szMsg2[40], szMsg3[40];
	int inMsgid;

	inSendECRResponseFlag = get_env_int("ECRRESP");

	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,fECR=[%d]", strTCT.fECR);
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse::fECRBuildSendOK[%d]::szRespCode[%s]", fECRBuildSendOK, srTransRec.szRespCode);
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,fGetECRTransactionFlg=[%d]", fGetECRTransactionFlg());
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,inSendECRResponseFlag[%d]", inSendECRResponseFlag);
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse,fTimeOutFlag=[%d]", fTimeOutFlag);

	/*BDO: For ECR response code to ECR_TIMEOUT_ERR -- sidumili*/
	if ((fTimeOutFlag) && strlen(srTransRec.szRespCode) <= 0)
		strcpy(srTransRec.szECRRespCode, ECR_TIMEOUT_ERR);
	
	
	if (inSendECRResponseFlag == 1)
		return(d_OK);
	
	vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse::RUN EXECUTE SHALRS!!!");	
	
	if (strTCT.fECR)
	{
				bret= inMultiAP_Database_BatchDelete();
				vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
				if(d_OK != bret)
				{
					vdSetErrorMessage("MultiAP BatchDelete ERR");
				}
				
				inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

				//----------------------------------------------------------------------------------------------------------------------
				// SAVE TO MULTIAP.S3DB -- sidumili
				//----------------------------------------------------------------------------------------------------------------------
				// For PAN Format
				memset(szTemp4, 0x00, sizeof(szTemp4));
				vdFormatPANForECR(srTransRec.szPAN, szTemp4);
				strcpy(srTransRec.szECRPANFormatted, szTemp4);
				
				// For ResponseText
				memset(szMsg1, 0x00, sizeof(szMsg1));
				memset(szMsg2, 0x00, sizeof(szMsg2));
				memset(szMsg3, 0x00, sizeof(szMsg3));

				
				if ((strlen(srTransRec.szRespCode) > 0) && (memcmp(srTransRec.szRespCode,"00",2) == 0)){
					strcpy(szMsg1, ECR_APPROVED_RESP);
					strcpy(srTransRec.szECRRespCode, ECR_APPROVED);
				}
				else{
						if (strlen(srTransRec.szRespCode) > 0){
							inMSGResponseCodeRead(szMsg1, szMsg2, szMsg3, atoi(srTransRec.szRespCode), 1);
							strcpy(srTransRec.szECRRespCode, ECR_DECLINED_ERR);

						}else if (memcmp(srTransRec.szAuthCode, "Y1", 2) == 0){
								strcpy(szMsg1, ECR_APPROVED_RESP);
								strcpy(srTransRec.szECRRespCode, ECR_APPROVED);
						}else{
							memset(szMsg1, 0x00, sizeof(szMsg1));
							if (memcmp(srTransRec.szECRRespCode, ECR_TIMEOUT_ERR, 2) == 0) /*TIMEOUT*/
							{
								strcpy(szMsg1, ECR_TIMEOUT_RESP);
								strcpy(srTransRec.szECRRespCode, ECR_UNKNOWN_ERR);
							}
							else if (memcmp(srTransRec.szECRRespCode, ECR_COMMS_ERR, 2) == 0) /*COMM ERROR*/
							{
								strcpy(szMsg1, ECR_COMM_ERROR_RESP);
								strcpy(srTransRec.szECRRespCode, ECR_UNKNOWN_ERR);
								
							}
							else if (memcmp(srTransRec.szECRRespCode, ECR_DECLINED_ERR, 2) == 0)
							{		
								strcpy(szMsg1, "INVALID RESPONSE CODE");
								strcpy(srTransRec.szECRRespCode, ECR_DECLINED_ERR);

							}else{	
								strcpy(szMsg1, ECR_OPER_CANCEL_RESP);
								strcpy(srTransRec.szECRRespCode, ECR_UNKNOWN_ERR);              /*OPERATOR CANCEL*/
							}
						}
				}
				
				vdCTOS_Pad_String(szMsg1, 40, IS_SPACE, POSITION_RIGHT);
				strcpy(srTransRec.szECRRespText, szMsg1);

				inNMTReadRecord(srTransRec.MITid);
				
				// For Merchant Name
								memset(srTransRec.szECRMerchantName, 0, sizeof(srTransRec.szECRMerchantName));
				memset(szBuffer, 0, sizeof(szBuffer));
				memset(szHeader, 0, sizeof(szHeader));
				//strcpy(szHeader, strSingleNMT.szMerchName);
				memcpy(szHeader, strSingleNMT.szMerchName,23);
				if(strlen(szHeader)<23)
					vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(szBuffer, szHeader, 23);

				memset(szHeader, 0, sizeof(szHeader));
				//strcpy(szHeader, strSingleNMT.szRctHdr1);	
				memcpy(szHeader, strSingleNMT.szRctHdr1,23);
				if(strlen(szHeader)<23)
					vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(&szBuffer[23], szHeader, 23);

				memset(szHeader, 0, sizeof(szHeader));
				//strcpy(szHeader, strSingleNMT.szRctHdr2);
				memcpy(szHeader, strSingleNMT.szRctHdr2,23);
				if(strlen(szHeader)<23)
					vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(&szBuffer[46], szHeader, 23);

				strcpy(srTransRec.szECRMerchantName, szBuffer);
				
#if 0
				// For Merchant Name
				memset(srTransRec.szECRMerchantName, 0, sizeof(srTransRec.szECRMerchantName));
				memset(szBuffer, 0, sizeof(szBuffer));
				memset(szHeader, 0, sizeof(szHeader));
				strcpy(szHeader, strMMT[0].szRctHdr1);
				vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(szBuffer, szHeader, 23);

				memset(szHeader, 0, sizeof(szHeader));
				strcpy(szHeader, strMMT[0].szRctHdr2);				  
				vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(&szBuffer[23], szHeader, 23);

				memset(szHeader, 0, sizeof(szHeader));
				strcpy(szHeader, strMMT[0].szRctHdr3);
				vdCTOS_Pad_String(szHeader, 23, IS_SPACE, POSITION_RIGHT);
				strncpy(&szBuffer[46], szHeader, 23);
				strcpy(srTransRec.szECRMerchantName, szBuffer);
#endif
				//----------------------------------------------------------------------------------------------------------------------
				// SAVE TO MULTIAP.S3DB -- sidumili
				//----------------------------------------------------------------------------------------------------------------------
				
				vdDebug_LogPrintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
				vdDebug_LogPrintf("RX ECR::LEN[%d] PAN FORMAT[%s]",strlen(srTransRec.szECRPANFormatted), srTransRec.szECRPANFormatted);
				vdDebug_LogPrintf("RX ECR::LEN[%d] MERCHANT NAME[%s]",strlen(srTransRec.szECRMerchantName), srTransRec.szECRMerchantName);
				vdDebug_LogPrintf("RX ECR::LEN[%d] RESPONSE CODE[%s]|inMsgid[%d]",strlen(srTransRec.szECRRespCode), srTransRec.szECRRespCode, inMsgid);
				vdDebug_LogPrintf("RX ECR::LEN[%d] RESPONSE TEXT[%s]",strlen(srTransRec.szECRRespText), srTransRec.szECRRespText);
				vdDebug_LogPrintf("RX ECR::LEN[%d] PAN[%s]", strlen(srTransRec.szPAN), srTransRec.szPAN);
				vdDebug_LogPrintf("RX ECR::LEN[%d] DE39[%s]",strlen(srTransRec.szRespCode), srTransRec.szRespCode);
				vdDebug_LogPrintf("RX ECR::LEN[%d] RREF[%s]",strlen(srTransRec.szRRN), srTransRec.szRRN);
				vdDebug_LogPrintf("RX ECR::ExpDate[%02X%02X]",srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
				vdDebug_LogPrintf("RX ECR::LEN[%d] TID[%s]",strlen(srTransRec.szTID), srTransRec.szTID);
				vdDebug_LogPrintf("RX ECR::LEN[%d] MID[%s]",strlen(srTransRec.szMID), srTransRec.szMID);
				vdDebug_LogPrintf("RX ECR::HDTID:[%02d]",srTransRec.HDTid);
				vdDebug_LogPrintf("RX ECR::IITID:[%02d]",srTransRec.IITid);
				vdDebug_LogPrintf("RX ECR::Inv Num:[%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);
				vdDebug_LogPrintf("RX ECR::szBatchNo:[%02X%02X%02X]",srTransRec.szBatchNo[0], srTransRec.szBatchNo[1], srTransRec.szBatchNo[2]);

				#if 0
				strcpy(srTransRec.baChipData,strMMT[0].szRctHdr1);
				strcat(srTransRec.baChipData,strMMT[0].szRctHdr2);
				strcat(srTransRec.baChipData,strMMT[0].szRctHdr3);
				strcat(srTransRec.baChipData,strMMT[0].szRctHdr4);
				strcat(srTransRec.baChipData,strMMT[0].szRctHdr5);
				vdDebug_LogPrintf("RX ECR::LEN[%d] szRctHdr1:%s",strlen(srTransRec.baChipData), srTransRec.baChipData);
				#endif
				
				vdDebug_LogPrintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
				
				bret = inMultiAP_Database_BatchInsert(&srTransRec);
				vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
				if(d_OK != bret)
				{
					vdSetErrorMessage("MultiAP BatchInsert ERR");
				}
		
						vdSetECRTransactionFlg(0);
						//memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

				outbuf[out_len++] = inSendECRResponseCommandID[0];//d_IPC_CMD_WAVE_SALE;
				outbuf[out_len++] = d_SUCCESS;

				inSendECRResponseFlag = 1;
				put_env_int("ECRRESP",1);

				//set as ECRtriggered trans
				srTransRec.fECRTriggerTran = TRUE;
				
				//bret = inMultiAP_SendChild(outbuf,out_len);
				//hubing enhance ECR
				bret = inMultiAP_SendAP_IPCCmd("SHARLS_ECR", outbuf,out_len);
				if (bret != d_OK)
				{
					return d_OK;
				}
			}

	return d_OK;		
}

// patrick add code 20141205 end


// patrick move to postrans?
int inMultiAP_ECRGetMainroutine(void)
{
	BYTE ipc[d_MAX_IPC_BUFFER] , str[d_MAX_IPC_BUFFER];
	int ipc_len;
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	pid_t my_pid;
		
	ipc_len = sizeof(ipc);
	memset(str,0x00,sizeof(str));
	memset(outbuf,0x00,sizeof(outbuf));


	if (inMultiAP_IPCGetChildEx(str, &ipc_len) == d_OK)
	{
		
		vdDebug_LogPrintf("inMultiAP_ECRGetMainroutine = inMultiAP_IPCGetChildEx");
		outbuf[out_len++] = d_SUCCESS;
		outbuf[out_len] = 0x00;
		bret = inMultiAP_SendChild(outbuf,out_len);
		if (bret != d_OK)
			return d_OK;

		vdDebug_LogPrintf("before vdSetECRTransactionFlg");

		vdSetECRTransactionFlg(1);

		vdDebug_LogPrintf("after  vdSetECRTransactionFlg %d", fGetECRTransactionFlg());
		
		memset(outbuf,0x00,sizeof(outbuf));
		memset(ipc,0x00,sizeof(ipc));
		out_len = 0;
		ipc_len = wub_str_2_hex(str , ipc , ipc_len);
		// patrick add code 20141205 start		
		inSendECRResponseFlag = 0;
		put_env_int("ECRRESP",0);//hubing enhance ECR
		ipc_len = inMultiAP_HandleIPC(ipc , ipc_len, outbuf, &out_len);//Do IPC request
	// patrick add code 20141205 start
	inSendECRResponseFlag = get_env_int("ECRRESP");//hubing enhance ECR
	if (inSendECRResponseFlag == 1) 
		return d_OK;
	// patrick add code 20141205 end

		if (out_len == 0)
		{
			out_len = 0;
			outbuf[out_len++] = d_FAIL;
			outbuf[out_len] = 0x00;
		}

		inTCTSave(1);
		
		bret= inMultiAP_Database_BatchDelete();
		vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchDelete ERR");
		}
		vdDebug_LogPrintf("Resp PAN:%s",srTransRec.szPAN);

		// patrick fix code #00207
		strcpy(srTransRec.szRespCode, "98");				
		
		vdDebug_LogPrintf("Resp DE39:%s",srTransRec.szRespCode);
		vdDebug_LogPrintf("Resp RREF:%s",srTransRec.szRRN);
		vdDebug_LogPrintf("Resp ExpDate:[%02X%02X]",srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
		vdDebug_LogPrintf("Resp MID:%s",srTransRec.szTID);
		vdDebug_LogPrintf("Resp TID:%s",srTransRec.szMID);
		vdDebug_LogPrintf("Inv Num:[%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);
	
		bret = inMultiAP_Database_BatchInsert(&srTransRec);
		vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchInsert ERR");
		}

		vdSetECRTransactionFlg(0);
		memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
		
		out_len = 0;
		outbuf[out_len++] = inSendECRResponseCommandID[0];//d_IPC_CMD_WAVE_SALE;
		outbuf[out_len++] = d_SUCCESS;
		
		inSendECRResponseFlag = 1;
		put_env_int("ECRRESP",1);
		
		//bret = inMultiAP_SendChild(outbuf,out_len);
		//hubing enhance ECR
		bret = inMultiAP_SendAP_IPCCmd("SHARLS_ECR", outbuf,out_len);
		
//		bret = d_NO;
		if (bret != d_OK)
		{
			// patrick add code 20141205 start		
			//exit(1);
			/*
			my_pid = getpid();
			memset(outbuf,0x00,sizeof(outbuf));
			sprintf(outbuf,"kill -9 %d",my_pid);
			vdDebug_LogPrintf("kill,[%d][%s]",my_pid,outbuf);
			system(outbuf);
			*/
			return d_OK;
		}
		// patrick add code 20141205 start
		return d_OK;
	}
}


// patrick move to postrans?
int inMultiAP_GetMainroutine(void)
{
	BYTE ipc[d_MAX_IPC_BUFFER] , str[d_MAX_IPC_BUFFER];
	int ipc_len;
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	pid_t my_pid;
		
	ipc_len = sizeof(ipc);
	memset(str,0x00,sizeof(str));
	memset(outbuf,0x00,sizeof(outbuf));

	if (inMultiAP_IPCGetChildEx(str, &ipc_len) == d_OK)
	{
		outbuf[out_len++] = d_SUCCESS;
		outbuf[out_len] = 0x00;
		bret = inMultiAP_SendChild(outbuf,out_len);
		if (bret != d_OK)
			return d_OK;

		vdDebug_LogPrintf("inCTOSS_MultiStopResumeAP V5S_BDOCREDIT");
		inCTOSS_MultiStopResumeAP("V5S_BDOCREDIT");

		memset(outbuf,0x00,sizeof(outbuf));
		memset(ipc,0x00,sizeof(ipc));
		out_len = 0;
		ipc_len = wub_str_2_hex(str , ipc , ipc_len);
		ipc_len = inMultiAP_HandleIPC(ipc , ipc_len, outbuf, &out_len);//Do IPC request

		if (out_len == 0)
		{
			out_len = 0;
			outbuf[out_len++] = d_FAIL;
			outbuf[out_len] = 0x00;
		}

		vdDebug_LogPrintf("inCTOSS_MultiResumeContinueAP V5S_BDOCREDIT");
		inCTOSS_MultiResumeContinueAP("V5S_BDOCREDIT");

		bret = inMultiAP_SendChild(outbuf,out_len);
//		bret = d_NO;
		if (bret != d_OK)
		{
			exit(1);
			/*
			my_pid = getpid();
			memset(outbuf,0x00,sizeof(outbuf));
			sprintf(outbuf,"kill -9 %d",my_pid);
			vdDebug_LogPrintf("kill,[%d][%s]",my_pid,outbuf);
			system(outbuf);
			*/
			return d_OK;
		}
	}
}

int inCTOS_MultiAPALLAppEventID(int IPC_EVENT_ID)
{
	int inResult = d_NO, inRetVal = d_NO;
//	int inLoop = 0;
	USHORT inLoop = 0;
	CTOS_stCAPInfo stinfo;
	BYTE byStr[25];
	USHORT Outlen; 
    char szAPName[25];
	int inAPPID;
	BYTE bret;
	BYTE szTempDebug [100+1];
	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		memset(byStr, 0x00, sizeof(byStr));
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
			memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
			CTOS_APGet(inLoop, &stinfo);
		}

//		CTOS_Delay(50);
//		sprintf(szTempDebug, "inLoop=[%d", inLoop);
//		CTOS_LCDTPrintXY(1, 6, szTempDebug);

		vdDebug_LogPrintf("baName=[%s],inLoop=[%d] ", stinfo.baName, inLoop);

		if (strcmp(szAPName, stinfo.baName)!=0)
		{	
			if (stinfo.baName[0] != 0x00)
			{
				if (memcmp(stinfo.baName, "SHARLS_", 7)==0)
					continue;

				if (d_OK == inCTOSS_MultiCheckUnForkAPName(stinfo.baName))
				{
					vdDebug_LogPrintf("Multi UnForkAPName22=[%s] ....", stinfo.baName);
					continue;
				}
//				CTOS_LCDTPrintXY(1, 7, stinfo.baName);

				inTCTSave(1);

				bret= inMultiAP_Database_BatchDelete();
				vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
				if(d_OK != bret)
			    	return bret;

//				CTOS_LCDTPrintXY(1, 3, "BatchDelete");

				bret = inMultiAP_Database_BatchInsert(&srTransRec);
				vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
				if(d_OK != bret)
			    	return bret;

//				CTOS_LCDTPrintXY(1, 4, "BatchInsert");
				
				inRetVal = inMultiAP_RunIPCCmdTypes(stinfo.baName, IPC_EVENT_ID, srTransRec.szInvoiceNo,INVOICE_BCD_SIZE,byStr,&Outlen);    
//				CTOS_LCDTPrintXY(1, 8, "PFR");

				inTCTRead(1);

				vdDebug_LogPrintf("baName[%x][%d] ",byStr[1],byStr[1]);
				if ((byStr[1] == d_SUCCESS)||(byStr[1] == d_FAIL))
					return byStr[1];
				else
					continue;
			}
		}
	}
	return d_NOT_RECORD;
}


int inCTOS_MultiAPBatchSearch(int IPC_EVENT_ID)
{
	int inResult = d_NO;

	inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);

	DebugAddSTR("inCTOS_BatchSearch","PROCESSING...",20);
	
	DebugAddINT("inCTOS_BatchSearch", inResult);

	if(inResult != d_OK)
    {
		inResult = inCTOS_MultiAPALLAppEventID(IPC_EVENT_ID);
		if ((inResult == d_SUCCESS)||(inResult == d_FAIL))
			return d_NO;
			
		//vdSetErrorMessage("NO RECORD FOUND");
		vdDisplayErrorMsgResp2("","TRANSACTION","NOT FOUND");
		return d_NO;
    }
    
    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 2);
	
	return inResult;
}

int inCTOS_MultiAPSaveData(int IPC_EVENT_ID)
{
	BYTE bret;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	char szAPName[25];
	int inAPPID;
	
	memset(outbuf,0x00,sizeof(outbuf));
	memset(szAPName,0x00,sizeof(szAPName));

	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	vdDebug_LogPrintf("szAPName[%s]=[%s]", strHDT.szAPName,szAPName);

	if (strlen(strHDT.szAPName) == 0)
		return d_NO;
	
	if (strcmp (strHDT.szAPName,szAPName) == 0)
		return d_OK;

	inTCTSave(1);

	bret= inMultiAP_Database_BatchDelete();
	vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("MultiAP BatchDelete ERR");
    	return bret;
	}
	
	bret = inMultiAP_Database_BatchInsert(&srTransRec);
	vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("MultiAP BatchInsert ERR");
    	return bret;
	}


	vdDebug_LogPrintf("szAPName[%s],bret=[%d]", strHDT.szAPName,bret);
	
	inMultiAP_RunIPCCmdTypes(strHDT.szAPName,IPC_EVENT_ID,"",0, outbuf,&out_len);

	inTCTRead(1);
	
	if (outbuf[0] != IPC_EVENT_ID)
	{
		//vdSetErrorMessage("MultiAP Type ERR");
		return d_NO;
	}

	if (outbuf[1] != d_SUCCESS)
	{
		//vdSetErrorMessage("MultiAP ERR");
		return d_NO;
	}
	
	return d_ON;
}

int inCTOS_MultiAPCheckAllowd(void)
{
	char szAPName[25];
	int inAPPID;
	
	memset(szAPName,0x00,sizeof(szAPName));

	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	vdDebug_LogPrintf("szAPName[%s]=[%s]", strHDT.szAPName,szAPName);

	if (strcmp (strHDT.szAPName,szAPName) == 0)
		return d_OK;

	//vdDisplayErrorMsg(1, 8, "TRANS NOT ALLOWED");
	vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
	return d_ON;
}

int inCTOS_MultiAPGetData(void)
{
	BYTE bret;
	
	vdDebug_LogPrintf("inCTOS_MultiAPGetData-----");
	//CTOS_LCDTPrintXY(1, 8, "LOADING TABLE... ");
	inDatabase_TerminalOpenDatabaseEx(DB_MULTIAP);//Optimize start

	//bret = inMultiAP_Database_BatchRead(&srTransRec);
	bret = inMultiAP_Database_BatchReadEx(&srTransRec);

	vdDebug_LogPrintf("inMultiAP_Database_BatchReadbret=[%d]", bret);
    
    DebugAddHEX("ECR AMOUNT", srTransRec.szBaseAmount, 6);
	
	if(d_OK != bret)
	{
		//vdSetErrorMessage("MultiAP BatchRead ERR");
		inDatabase_TerminalCloseDatabase();//Optimize end
    	return bret;
	}

	//bret= inMultiAP_Database_BatchDelete();
	bret= inMultiAP_Database_BatchDeleteEx();
	inDatabase_TerminalCloseDatabase();//Optimize end
	vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		//vdSetErrorMessage("MultiAP Batch Delete ERR");
    	return bret;
	}

	return d_OK;
	
}

int inCTOS_MultiAPReloadHost(void)
{
	short shGroupId ;
    int inHostIndex;
    short shCommLink;
	int inCurrencyIdx=0;
	
    inHostIndex = (short) srTransRec.HDTid;

	vdDebug_LogPrintf("inCTOS_MultiAPReloadTable,HDTid=[%d]",inHostIndex);

	inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);
		
	if ( inHDTReadEx(inHostIndex) != d_OK)
	{
		vdSetErrorMessage("HOST SELECTION ERR");
		inDatabase_TerminalCloseDatabase();
		return(d_NO);
    } 
    else 
	{
		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

		inCurrencyIdx = strHDT.inCurrencyIdx;
		
		if (inCSTReadEx(inCurrencyIdx) != d_OK)
		{	
			vdSetErrorMessage("LOAD CST ERR");
			inDatabase_TerminalCloseDatabase();
	        return(d_NO);
        }
		
        if(strTCT.fSingleComms == TRUE)
            inHostIndex=1;
		
		if ( inCPTReadEx(inHostIndex) != d_OK)
		{
			vdSetErrorMessage("LOAD CST ERR");
			inDatabase_TerminalCloseDatabase();
			return(d_NO);
	    }
    }
    inDatabase_TerminalCloseDatabase();
	return (d_OK);
	
}


int inCTOS_MultiAPReloadTable(void)
{
	short shGroupId ;
    int inHostIndex;
    short shCommLink;
	int inCurrencyIdx=0;
	
	inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);;
	vdDebug_LogPrintf("inCTOS_MultiAPReloadTable-CDTid[%d]--IITid[%d]--fECRTrxFlg[%d] -- HDTid[%d]",srTransRec.CDTid,srTransRec.IITid, srTransRec.fECRTrxFlg, srTransRec.HDTid);
	if (srTransRec.CDTid > 0)
	{
		//if ( inCDTRead(srTransRec.CDTid) != d_OK)
		if ( inCDTReadEx(srTransRec.CDTid) != d_OK)
		{
		    vdDebug_LogPrintf("LOAD CDT ERROR [%d]", srTransRec.CDTid);
			vdSetErrorMessage("LOAD CDT ERROR");
			inDatabase_TerminalCloseDatabase();
			return(d_NO);
	    }
	}
#if 0
 	if (srTransRec.IITid > 0) 
    	inGetIssuerRecord(srTransRec.IITid);
#endif
	vdDebug_LogPrintf("5. IITid[%d]",srTransRec.IITid);
	if (inIITReadEx(srTransRec.IITid) != d_OK)
    {
		vdDisplayErrorMsg(1, 8, "IIT ERR");  
		inDatabase_TerminalCloseDatabase();
        return(d_NO);
    }

	vdDebug_LogPrintf("strTCT.fATPBinRoute[%d]  fLogon[%d]   inLogonSingleHost[%d]",strTCT.fATPBinRoute,fLogon, inLogonSingleHost); 
	

	if (fLogon == TRUE)
		{
			inHostIndex = inLogonSingleHost;
		}
	else
		{
			if(strTCT.fATPBinRoute == TRUE)
    				inHostIndex = srTransRec.HDTid;  
			else
				inHostIndex = (short) strCDT.HDTid;
		}



	if (srTransRec.fECRTrxFlg == TRUE)
	{
		vdSetECRTransactionFlg(1);
		srTransRec.fECRTriggerTran = TRUE;
	}
	
	vdDebug_LogPrintf("inCTOS_SelectHost =[%d],HDTid=[%d]",inHostIndex,strCDT.HDTid); 
	//if ( inHDTRead(inHostIndex) != d_OK)
	if ( inHDTReadEx(inHostIndex) != d_OK)
	{
		vdDisplayErrorMsg(1, 8, "HOST SELECTION ERR");
		inDatabase_TerminalCloseDatabase();
		return(d_NO);
    } 
    else 
	{
		srTransRec.HDTid = inHostIndex;
		inCurrencyIdx = strHDT.inCurrencyIdx;

		//if (inCSTRead(inCurrencyIdx) != d_OK)
		if (inCSTReadEx(inCurrencyIdx) != d_OK)
		{	
	        vdDisplayErrorMsg(1, 8, "CST ERR");  
			inDatabase_TerminalCloseDatabase();
	        return(d_NO);
        }

		if (strTCT.fSingleComms)	
			inHostIndex = 1;
		
   		//if ( inCPTRead(inHostIndex) != d_OK)
		if ( inCPTReadEx(inHostIndex) != d_OK)
		{
	        vdDisplayErrorMsg(1, 8, "CPT ERR");
			inDatabase_TerminalCloseDatabase();
			return(d_NO);
	    }
    }

	inDatabase_TerminalCloseDatabase();
	return (d_OK);
	
}

int inCTOS_MultiAPGetpowrfail(void)
{
	int inResult = d_NO;
	
	inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_POWER_FAIL);
	if ((inResult == d_SUCCESS)||(inResult == d_FAIL))
			return d_NO;
}

int inCTOS_MultiAPGetVoid(void)
{
	BYTE bret;
	TRANS_DATA_TABLE srTransRectmp;
	
	vdDebug_LogPrintf("inCTOS_MultiAPGetVoid-----");
	inDatabase_TerminalOpenDatabaseEx(DB_MULTIAP);
	//bret = inMultiAP_Database_BatchRead(&srTransRectmp);
	bret = inMultiAP_Database_BatchReadEx(&srTransRectmp);
	vdDebug_LogPrintf("inMultiAP_Database_BatchReadbret=[%d]", bret);
	if(d_OK != bret)
	{
		inDatabase_TerminalCloseDatabase();//Optimize end
    	return bret;
	}
	
	//bret= inMultiAP_Database_BatchDelete();
	bret= inMultiAP_Database_BatchDeleteEx();
	vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		inDatabase_TerminalCloseDatabase();//Optimize end
    	return bret;
	}
	
	inDatabase_TerminalCloseDatabase();//Optimize end
	memcpy(srTransRec.szInvoiceNo,srTransRectmp.szInvoiceNo,3);
	vdDebug_LogPrintf("inCTOS_MultiAPGetVoid =[%02x%02x%02x]",srTransRec.szInvoiceNo[0],srTransRec.szInvoiceNo[1],srTransRec.szInvoiceNo[2]);

	return d_OK;
}

void vdCTOSS_DislplayMianAPMenu(int IPC_EVENT_ID)
{
	CTOS_LCDTClearDisplay();
		
	switch(IPC_EVENT_ID)
    {
        case d_IPC_CMD_FUN_KEY1:
            inF1KeyEvent();
            break;
        case d_IPC_CMD_FUN_KEY2:
            inF2KeyEvent();
            break;
        case d_IPC_CMD_FUN_KEY3:
            inF3KeyEvent();
            break;
		case d_IPC_CMD_FUN_KEY4:
            inF4KeyEvent();
            break;
		default:
        	break;
    }

	return;
}

int inCTOSS_MultiDislplayMenu(int IPC_EVENT_ID)
{
	CTOS_stCAPInfo stinfo;
	int inLoop = 0;
    char szHostMenu[1024];
	int inHostindex = 0;
    char szHostName[d_MAX_APP+1][25];
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	char szAPName[25];
	int inAPPID;

	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
	char szHeaderString[50] = "SELECT AP";

	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
	
	vdDebug_LogPrintf("begain inCTOSS_MultiDislplayMenu............");
	memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
	
  	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
 			CTOS_APGet(inLoop, &stinfo);
		}
		
 		vdDebug_LogPrintf("baName[%s] ", stinfo.baName);
 
		if (memcmp(stinfo.baName, "SHARLS_", 7) == 0)
			continue;

		if (stinfo.baName[0] != (char)0x00)
		{
			strncpy(szHostName[inHostindex++],stinfo.baName,25);
		}
	}

	for (inLoop = 0; inLoop < d_MAX_APP; inLoop++)
    {
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");      
        }
        else
            break;
    }

	vdDebug_LogPrintf("inHostindex=[%d]..........", inHostindex);
	if (inHostindex == 1)//only one app 
	{
		vdCTOSS_DislplayMianAPMenu(IPC_EVENT_ID);
		return d_OK;
	}
	
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;
        
        vdDebug_LogPrintf("key=[%d],szHostName =[%s]", key, szHostName[key-1]);

		if (strcmp(szAPName, szHostName[key-1])==0)
		{
			vdCTOSS_DislplayMianAPMenu(IPC_EVENT_ID);
			return d_OK;
		}
		inTCTSave(1);
		
		inMultiAP_RunIPCCmdTypes(szHostName[key-1],IPC_EVENT_ID,"",0, outbuf,&out_len);

		inTCTRead(1);

		vdDebug_LogPrintf("end inCTOSS_MultiDislplayMenu............");
		
		if (outbuf[0] != IPC_EVENT_ID)
		{
			return d_NO;
		}

		if (outbuf[1] != d_SUCCESS)
		{
			return d_NO;
		}
	
	}
	
 	return d_OK;
}




