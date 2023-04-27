/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>
#include <EMVAPLib.h>
#include <EMVLib.h>


#include "..\Includes\POSTypedef.h"
#include "..\Debug\Debug.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSAutoReport.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "../Includes/wub_lib.h"
#include "..\database\par.h"
#include "..\Includes\POSAutoReport.h"
#include "../Database/DatabaseFunc.h"


int inCTOS_AutoReportFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];




    vdCTOS_SetTransType(POS_AUTO_REPORT);

    //display title
    vdDispTransTitle(POS_AUTO_REPORT);


    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;
        return inRet;

    inRet = inCTOS_SelectAutoReportHost();
    if(d_OK != inRet)
        return inRet;
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_POS_AUTO_REPORT);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;
            
            inRet = inCTOS_MultiAPReloadTable();
            if(d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_MultiAPCheckAllowd();
        if(d_OK != inRet)
            return inRet;
    }

    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);


    inRet = inCTOS_GetPOSAutoReportData();
     if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;



    return d_OK;
}


int inCTOS_SelectAutoReportHost(void) 
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;



    inHostIndex =  1;
    
     vdDebug_LogPrintf("inCTOS_SelectHost =[%d]",inHostIndex);
    
    // if (inMultiAP_CheckSubAPStatus() == d_OK)
    //    return d_OK;
     
    if ( inHDTRead(inHostIndex) != d_OK)
    {
        
        vdSetErrorMessage("HOST SELECTION ERR");
        return(d_NO);
    } 
    else 
    {
        
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTRead(inCurrencyIdx) != d_OK) 
        {
            
            vdSetErrorMessage("LOAD CST ERR");
            return(d_NO);
        }


        if ( inCPTRead(inHostIndex) != d_OK)
        {
            
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }


        return (d_OK);
    }
}


int inCTOS_GetPOSAutoReportData(void){


	inPARRead(1);
 
	memcpy(strHDT.szTPDU,strPAR.szPARTPDU,5);

	vdDebug_LogPrintf("gladytest strHDT.szTPDU - %s", strHDT.szTPDU);

	strcpy(srTransRec.szTID, strPAR.szPARTID);
   	strcpy(srTransRec.szMID, strPAR.szPARMID);
	
    	return d_OK;

}


int inCTOS_POS_AUTO_REPORT(void)
{
    int inRet = d_NO;



    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_AutoReportFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

