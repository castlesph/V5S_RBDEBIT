
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

#include "..\includes\LocalAptrans.h"
#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSTrans.h"
#include "..\Debug\Debug.h"
#include "..\UI\Display.h"
#include "..\Database\DatabaseFunc.h"
#include "..\includes\LocalFunc.h"
#include "..\powrfail\POSPOWRFAIL.h"

#include <sqlite3.h>



int inCTOS_LocalAPSaveData(void)
{
	BYTE bret;
	
	inTCTSave(1);
	bret= inLocalAP_Database_BatchDelete();
	vdDebug_LogPrintf("inLocalAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("LocalAP BatchDelete ERR");
    	return bret;
	}
	
	bret = inLocalAP_Database_BatchInsert(&srTransRec);
	vdDebug_LogPrintf("inLocalAP_Database_BatchInsert=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("LocalAP BatchInsert ERR");
    	return bret;
	}

	return d_OK;
}

int inCTOS_LocalAPGetData(void)
{
	BYTE bret;
	
	vdDebug_LogPrintf("SATURN inCTOS_LocalAPGetData-----");
	
	bret = inLocalAP_Database_BatchRead(&srTransRec);
	vdDebug_LogPrintf("saturn inLocalAP_Database_BatchReadbret=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("LocalAP BatchRead ERR");
    return bret;
	}

	bret= inLocalAP_Database_BatchDelete();
	vdDebug_LogPrintf("saturn inLocalAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("LocalAP Batch Delete ERR");
    	return bret;
	}

	return d_OK;
	
}

int inCTOS_LocalAPReloadTable(void)
{
	short shGroupId ;
    int inHostIndex;
    short shCommLink;
	int inCurrencyIdx=0;
	
	vdDebug_LogPrintf("saturn inCTOS_LocalAPReloadTable-CDTid[%d]--IITid[%d]--",srTransRec.CDTid,srTransRec.IITid);
	if (srTransRec.CDTid > 0)
	{
		if ( inCDTRead(srTransRec.CDTid) != d_OK)
		{
			vdSetErrorMessage("LOAD CDT ERROR");
			return(d_NO);
	    }
	}

 	if (srTransRec.IITid > 0) 
    	inGetIssuerRecord(srTransRec.IITid);

    inHostIndex = (short) strCDT.HDTid;
	
	vdDebug_LogPrintf("saturn inCTOS_SelectHost =[%d],HDTid=[%d]",inHostIndex,strCDT.HDTid); 
	if ( inHDTRead(inHostIndex) != d_OK)
	{
		vdDisplayErrorMsg(1, 8, "HOST SELECTION ERR 1");
		return(d_NO);
    } 
    else 
	{
		srTransRec.HDTid = inHostIndex;
		inCurrencyIdx = strHDT.inCurrencyIdx;

		if (inCSTRead(inCurrencyIdx) != d_OK)
		{	
	        vdDisplayErrorMsg(1, 8, "CST ERR");  
	        return(d_NO);
        }

		if ( inCPTRead(inHostIndex) != d_OK)
		{
	        vdDisplayErrorMsg(1, 8, "CPT ERR");
			return(d_NO);
	    }
    }

	return (d_OK);
	
}
