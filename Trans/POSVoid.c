/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>


#include "..\Includes\POSTypedef.h"


#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Includes\POSVoid.h"
#include "../Debug/Debug.h"

/* For ECR -- sidumili */
extern BOOL fECRBuildSendOK; 
extern BOOL fTimeOutFlag; /*BDO: Flag for timeout --sidumili*/
/* For ECR -- sidumili */

int inCTOS_VoidFlowProcess(void)
{
    int inRet = d_NO;

	fECRBuildSendOK = FALSE; /* BDO: For ECR -- sidumili */
	fTimeOutFlag = FALSE; /*BDO: Flag for timeout --sidumili*/
	
    vdCTOS_SetTransType(VOID);
    
    //display title
    //vdDispTransTitle(VOID); //aaronnino for BDOCLG ver 9.0 fix on issue #00096 Flickering display title upon search of invoice number for void and tip adjust 2 of 2
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GeneralGetInvoice();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_VOID_SALE);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetVoid();
            if(d_OK != inRet)
                return inRet;
        }       
        inRet = inCTOS_BatchSearch();
        if(d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckVOID();
    if(d_OK != inRet){
		
		//if (fGetECRTransactionFlg() == TRUE){
		    strcpy(srTransRec.szRespCode,"");
			strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);
		//}
		
        return inRet;
    }
    
    inRet = inCTOS_LoadCDTandIIT();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_VoidSelectHost();
    if(d_OK != inRet)
        return inRet;

	/*sidumili: Issue#: 000170 [Void not allowed on debit transaction]*/
	inRet = inCTOS_CheckHostVoidAllwd();
    if(d_OK != inRet){
		
		//if (fGetECRTransactionFlg() == TRUE){
		    strcpy(srTransRec.szRespCode,"");
			strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);
		//}
		
        return inRet;
    }

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

	//inRet = inCTOS_CheckHostVoidAllwd();
	//	if(d_OK != inRet)
    //    return inRet;
    
    inRet = inCTOS_ConfirmInvAmt();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet){

		/*BDO: Set ECR response code to EN - COMM ERROR -- sidumili*/
		if (strlen(srTransRec.szRespCode) <= 0) 
			strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
		
        return inRet;
    }

    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

    // patrick add code 20141205 start, modified -- sidumili
	fECRBuildSendOK = TRUE;	
	if (fECRBuildSendOK){	
	    inRet = inMultiAP_ECRSendSuccessResponse();
	}
	// patrick add code 20141205 start, modified -- sidumili

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
        
    return d_OK;
}

int inCTOS_VOID(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_VoidFlowProcess();

	/* Send response to ECR -- sidumili */
	if (!fECRBuildSendOK){	
		inMultiAP_ECRSendSuccessResponse();
	}	
	fECRBuildSendOK = FALSE;
	/* Send response to ECR -- sidumili */
	
    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

/*sidumili: Issue#: 000170 [Void not allowed on debit transaction]*/
int inCTOS_CheckHostVoidAllwd(void){	
	vdDebug_LogPrintf("-->>inCTOS_CheckHostVoidAllwd HDTid[%d]:::byVoided[d]", srTransRec.HDTid, srTransRec.byVoided);
	//if ((srTransRec.HDTid == 5) && (!srTransRec.byVoided)){
	if (!srTransRec.byVoided){
		//vdSetErrorMessage("VOID NOT ALLOWED");
		vdDisplayErrorMsgResp2("","TRANSACTION","NOT ALLOWED");
		return d_NO;
}

return d_OK;

}

