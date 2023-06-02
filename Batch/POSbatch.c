#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include "../Includes/POSbatch.h"
#include "../Includes/POSTypedef.h"

#include "../FileModule/myFileFunc.h"
#include "../POWRFAIL/POSPOWRFAIL.h"
#include "..\debug\debug.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\myEZLib.h"
#include "..\ui\Display.h"
#include "..\Includes\POSHost.h"
#include "..\debug\debug.h"


int inCTOS_SaveBatchTxn(void)
{
    int inResult ;
    int inSaveType;
	
	BYTE szMessage1[40+1]={0};
	BYTE szMessage2[40+1]={0};
	BYTE szMessage3[40+1]={0};
	BYTE szMessage4[40+1]={0};

	vdSaveLastInvoiceNo();

    vdDebug_LogPrintf("SATURN inCTOS_SaveBatchTxn");
	inCTLOS_Updatepowrfail(PFR_BEGIN_BATCH_UPDATE);

    
    vdDebug_LogPrintf("saturn inCTOS_SaveBatchTxn byTransType[%d].byOffline[%d].byUploaded[%d]", srTransRec.byTransType, srTransRec.byOffline, srTransRec.byUploaded);
    if(srTransRec.byTransType == VOID && srTransRec.byOffline == CN_TRUE)
    {
        //OFFLINE SALE, then VOID it, the VOID is offline, should delete the OFFLINE SALE Advice
        if(srTransRec.byUploaded == CN_FALSE)
        {
            inCTOSS_DeleteAdviceByINV(srTransRec.szInvoiceNo);
        }
    }

	// Get ECR Message from ENV -- sidumili
	if (srTransRec.fECRTxnFlg == TRUE)
	{
		memset(szMessage1, 0x00, sizeof(szMessage1));
		memset(szMessage2, 0x00, sizeof(szMessage2));
		memset(szMessage3, 0x00, sizeof(szMessage3));
		memset(szMessage4, 0x00, sizeof(szMessage4));
		
		inCTOSS_GetEnvDB("MSG1", szMessage1);
		strcpy(srTransRec.szECRMessage1, szMessage1);

		inCTOSS_GetEnvDB("MSG2", szMessage2);
		strcpy(srTransRec.szECRMessage2, szMessage2);

		inCTOSS_GetEnvDB("MSG3", szMessage3);
		strcpy(srTransRec.szECRMessage3, szMessage3);

		inCTOSS_GetEnvDB("MSG4", szMessage4);
		strcpy(srTransRec.szECRMessage4, szMessage4);
	}

    SetTransYear(); // Set Trans Year -- sidumili
	
    if((srTransRec.byTransType == SALE) || (srTransRec.byTransType == CASH_OUT) || (srTransRec.byTransType == REFUND) 
	|| (srTransRec.byTransType == SALE_OFFLINE) || (srTransRec.byTransType == PRE_AUTH)	|| (srTransRec.byTransType == LOY_REDEEM_5050) 
	|| (srTransRec.byTransType == LOY_REDEEM_VARIABLE) || (srTransRec.byTransType == CASH_IN) || (srTransRec.byTransType == CASH_OUT)
	|| (srTransRec.byTransType == BILLS_PAYMENT) || (srTransRec.byTransType == FUND_TRANSFER))
    {
		if (srTransRec.byTransType != PRE_AUTH){
	    	strMMT[0].fBatchNotEmpty = TRUE;
			inMMTBatchNotEmptySave(strMMT[0].MMTid);
        }
		
        inSaveType = DF_BATCH_APPEND;

        //Should be Online void the Intial SALE amount.
        if(srTransRec.byOffline == CN_FALSE)
        {
            //use szStoreID to store how much amount fill up in DE4 for VOID
            //either enter tip or not enter tip are ok
            memcpy(srTransRec.szStoreID, srTransRec.szTotalAmount, 6);
        }
    }
    else
    {
        inSaveType = DF_BATCH_UPDATE;
    }
    
    if(srTransRec.byTransType == VOID)
        srTransRec.byVoided = TRUE;
    
	inResult = inDatabase_BatchSave(&srTransRec, inSaveType);
    
    if(inResult != ST_SUCCESS)
    {
    	vdSetErrorMessage("Batch Save Error");
        return ST_ERROR;
    }
            
    if((memcmp(srTransRec.szAuthCode, "Y3",2))  && 
       (srTransRec.byTransType != SALE_OFFLINE) &&
       (srTransRec.byTransType != SALE_ADJUST)  &&
       (srTransRec.byTransType != SALE_TIP) &&
       ( srTransRec.byOffline != VS_TRUE))
    {
        inMyFile_ReversalDelete();
    }
        
    if((srTransRec.byTransType == SALE_TIP) || (srTransRec.byTransType == SALE_ADJUST) || (srTransRec.byTransType == SALE_OFFLINE))
    {
        //fix send tip adjust advie two times issue
        inCTOSS_DeleteAdviceByINV(srTransRec.szInvoiceNo);
        
        inMyFile_AdviceSave(&srTransRec, srTransRec.byPackType);
    }
    //if original txn is offline, then do a online txn piggy back the offline advice, the offline advice been deleted, then void it, the void is offline, need to create advice for this offline void
    else if(srTransRec.byTransType == VOID && srTransRec.byOffline == CN_TRUE && srTransRec.byUploaded == CN_TRUE)
    {
        vdDebug_LogPrintf("Create Advice for offline VOID");
        inMyFile_AdviceSave(&srTransRec, OFFLINE_VOID);
    }
    else if((srTransRec.byTransType == SALE || srTransRec.byTransType == CASH_OUT) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
    {
       if((srTransRec.byTCuploaded == 0) ||
          (!memcmp(srTransRec.szAuthCode, "Y1",2)) || 
          (!memcmp(srTransRec.szAuthCode, "Y3",2)) 
         )
       {
           if((memcmp(srTransRec.szAuthCode, "Y1",2)) && 
              (memcmp(srTransRec.szAuthCode, "Y3",2)) 
             )
           {
               srTransRec.byPackType = TC_UPLOAD;
               inMyFile_TCUploadFileSave(&srTransRec, srTransRec.byPackType);
           }
           else
           {
               srTransRec.byPackType = SEND_ADVICE;
               inMyFile_AdviceSave(&srTransRec, srTransRec.byPackType);
           }
       }
    }
    else if((/*srTransRec.byTransType == REFUND || */srTransRec.byTransType == PRE_AUTH) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
    {
       
        srTransRec.byPackType = TC_UPLOAD;
        inMyFile_TCUploadFileSave(&srTransRec, srTransRec.byPackType);
           
    }

    return d_OK;
}

typedef struct
{	
	char sztranstype[24+1];	
	char szIssuerLabel[24+1];	
	char szPAN[24+1];	
	char szAmount[24+1];	
	char szInvoiceNumber[6+1];
	char szDateTime[24+1];
	char szOKDRefNo[16+1];//[32+1];
}Transaction;

#define COUNT 128

void vdPopulateTrans(Transaction *, TRANS_DATA_TABLE *);

void CreateTransXML(FILE *, Transaction *);
void CreateTransData(FILE *, Transaction *);


int inCTOS_BatchReviewFlow(void)
{
    int inResult;
    int inReadRecResult;
 	int inCount = 0;
	int inBatchRecordNum = 0;
    BYTE key;
	char szPanDisplay[24+1];
    BYTE szTemp1[30+1];
	int  *pinTransDataid = NULL;
  
    Transaction trans[COUNT];
    int i;
    FILE *pFile;

	BYTE szDisplayBuf[128+1];
	
	usCTOSS_LCDDisplay(" ");

    vdDebug_LogPrintf("1.inCTOS_BatchReviewFlow");
	
	inBatchRecordNum = inBatchNumRecord();

	pinTransDataid = (int*)malloc(inBatchRecordNum * sizeof(int));
	
	inBatchByMerchandHost(inBatchRecordNum, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);

    printf("Input data:\n");
	memset(&trans, 0, sizeof(trans));

	
    vdDebug_LogPrintf("2.inCTOS_BatchReviewFlow");

    for(inCount=0; inCount < inBatchRecordNum; inCount++)
    {
		inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        vdPopulateTrans(&trans[inCount], &srTransRec);
    }

	
    vdDebug_LogPrintf("3.inCTOS_BatchReviewFlow");

    printf("\nData was saved into xml file\n");
    //pFile = fopen("/data/user/0/com.Source.S1_MPU.MPU/files/batch_review.xml","w");
    pFile = fopen("/home/ap/pub/batch_review.dat","w");
	
	vdDebug_LogPrintf("3.inCTOS_BatchReviewFlow pFile:%d: ", pFile);
	
    if(pFile==NULL){
        printf("Error");
		vdDebug_LogPrintf("3.1.inCTOS_BatchReviewFlow");
        return 0;
    }

	vdDebug_LogPrintf("3.5.inCTOS_BatchReviewFlow");
	
    //fprintf (pFile,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    //fprintf (pFile,"<batch>\n");
    for(inCount=0; inCount < inBatchRecordNum; inCount++)
    {
        //CreateTransXML(pFile,&trans[inCount]);
        CreateTransData(pFile,&trans[inCount]);
    }
    
    //fprintf (pFile,"</batch>\n");
    fclose(pFile);
	
    vdDebug_LogPrintf("4.inCTOS_BatchReviewFlow");

    ///data/user/0/com.Source.S1_MPU.MPU/files
    //chmod((char *)"/data/user/0/com.Source.S1_MPU.MPU/files/batch_review.xml", 0777);
    chmod((char *)"/home/ap/pub/batch_review.dar", 0777);
	
    //key = inCTOS_DisplayBatchRecordDetail(BATCH_REVIEW);
    
	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	strcpy(szDisplayBuf, "BATCH REVIEW");	
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, "0"); //szDisplayBuf2
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, "0"); //szPANBuf
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, "0"); //szDisplayAmountBuf
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, "0"); //szINVBuf
	strcat(szDisplayBuf, "|");
	memset(szTemp1, 0, sizeof(szTemp1));
	sprintf(szTemp1, "%d", inBatchRecordNum);
	strcat(szDisplayBuf, szTemp1); //transction count
	//inKey=usCTOSS_Confirm(szDisplayBuf);
	usCTOSS_BatchReviewUI(szDisplayBuf);

	
    vdDebug_LogPrintf("5.inCTOS_BatchReviewFlow");
	
	free(pinTransDataid);
    return d_OK;
}


void vdPopulateTrans(Transaction *trans, TRANS_DATA_TABLE *transData)
{
    BYTE szTotalAmount[12+1];
    BYTE szPreTotalAmount[12+1];
	
    char szTempBuf[64 + 1];
    char szTempBuf2[64 + 1];	
	int inPosLen=0;
	


	vdDebug_LogPrintf("vdPopulateTrans: transData->IITid:%d, transData->HDTid:%d", transData->IITid, transData->HDTid);

	inIITRead(transData->IITid);

	//CBB_OK$_MPU_Test Result_09172021 - 10032.1
	//if(transData->HDTid == 17 || transData->HDTid == 19 || transData->HDTid == CBPAY_HOST_INDEX || transData->HDTid == OK_DOLLAR_HOST_INDEX)
	inCSTRead(1);

    szGetTransTitle(transData->byTransType, trans->sztranstype);

	//http://118.201.48.210:8080/redmine/issues/2528.31.3
	#if 1
	    memcpy(trans->szIssuerLabel, "DEBIT", strlen("DEBIT"));
	#else
	if(srTransRec.IITid != 10 && srTransRec.IITid != 11)
    	memcpy(trans->szIssuerLabel, strIIT.szIssuerLabel, strlen(strIIT.szIssuerLabel));
    #endif
    
	vdCTOS_FormatPAN(strIIT.szPANFormat, transData->szPAN, trans->szPAN, strIIT.fMaskPan);

    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
    memset(szPreTotalAmount, 0x00, sizeof(szPreTotalAmount));

    wub_hex_2_str(transData->szTotalAmount, szPreTotalAmount, 6);

	vdDebug_LogPrintf("vdPopulateTrans: szTotalAmount1: [%s]", szTotalAmount);

	//CBB_OK$_MPU_Test Result_09172021 - 10032.1
	if(strcmp(strCST.szCurSymbol, "MMK") == 0)			
		sprintf(szTotalAmount, "%012.0f", atof(szPreTotalAmount)/100);
	else
		strcpy(szTotalAmount, szPreTotalAmount);
	
	vdDebug_LogPrintf("vdPopulateTrans: szTotalAmount2: [%s]", szTotalAmount);

	//CBB_OK$_MPU_Test Result_09172021 - 10032.1
	if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, szTotalAmount);
	else
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, szTotalAmount);

	

	vdDebug_LogPrintf("vdPopulateTrans: szTotalAmount3: [%s]", szTotalAmount);

	//Trans AMOUNT
	sprintf(trans->szAmount, "%s %s", strCST.szCurSymbol, szTotalAmount);

	vdDebug_LogPrintf("vdPopulateTrans: szTotalAmount4: [%s]", trans->szAmount);
	
	
    wub_hex_2_str(transData->szInvoiceNo, trans->szInvoiceNumber, 3);
	vdDebug_LogPrintf("vdPopulateTrans: trans->szInvoiceNumber:%s", trans->szInvoiceNumber);
    //memcpy(&trans->szInvoiceNumber, "000014", 6);

    /*date*/
    memset(szTempBuf, 0x00, sizeof(szTempBuf));
    wub_hex_2_str(srTransRec.szDate, szTempBuf, DATE_BCD_SIZE);
	vdDebug_LogPrintf("vdPopulateTrans: szTempBuf: DATE %s", szTempBuf);
    
    memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
    memcpy(szTempBuf2, szTempBuf+2, 2);                                    
	inPosLen += 2;
    strcat(szTempBuf2, "/");                                                
	inPosLen += 1;
    memcpy(szTempBuf2+inPosLen, szTempBuf, 2);                             
	inPosLen += 2;
    strcat(szTempBuf2, "/");                                                
	inPosLen += 1;

    memset(szTempBuf, 0x00, sizeof(szTempBuf));

	//fix for issue 10032 - internal issue
	// also for 10040 - Incorrect year displayed for cobntactless transaction on Visa&Master Detail report
    //wub_hex_2_str(srTransRec.szYear, szTempBuf, 2);
	//vdDebug_LogPrintf("vdPopulateTrans: wub_hex_2_str: szTempBuf: YEAR %s", szTempBuf);

#if 0
	if(transData->HDTid == 14 || transData->HDTid == 21)
    	memcpy(szTempBuf, srTransRec.szYear, 2);
	else{
		wub_hex_2_str(&srTransRec.szYear[1], szTempBuf, 1);		
	}
#else
	sprintf(szTempBuf, "%02lu", atol(srTransRec.szYear));

	vdDebug_LogPrintf("vdPopulateTrans: szTempBuf: YEAR1 [%s] [%d]", szTempBuf, atol(szTempBuf));		

	if(atol(szTempBuf) <= 0){		
		wub_hex_2_str(&srTransRec.szYear[1], szTempBuf, 1); 	

		vdDebug_LogPrintf("vdPopulateTrans: szTempBuf: YEAR2 [%s]", szTempBuf);		
	}
	
#endif

    //memcpy(szTempBuf2+inPosLen, szTempBuf+2, 2);                           
    memcpy(szTempBuf2+inPosLen, szTempBuf, 2);                           
	inPosLen += 2;
    strcat(szTempBuf2, " ");                                                
	inPosLen += 1;

	vdDebug_LogPrintf("vdPopulateTrans: szTempBuf2: DATE + YEAR %s", szTempBuf2);

	
	/*Time*/
    memset(szTempBuf, 0x00, sizeof(szTempBuf));
    wub_hex_2_str(srTransRec.szTime, szTempBuf, 3);
	vdDebug_LogPrintf("vdPopulateTrans: szTempBuf: TIME %s", szTempBuf);

	
    memcpy(szTempBuf2+inPosLen, szTempBuf, 2);                             
	inPosLen += 2;
    strcat(szTempBuf2, ":");                                                
	inPosLen += 1;
    memcpy(szTempBuf2+inPosLen, szTempBuf+2, 2);                           
	inPosLen += 2;
    strcat(szTempBuf2, ":");                                                
	inPosLen += 1;
    memcpy(szTempBuf2+inPosLen, szTempBuf+4, 2);                            
	inPosLen += 2;

	memcpy(trans->szDateTime, szTempBuf2, strlen(szTempBuf2));

vdDebug_LogPrintf("vdPopulateTrans END: trans->szDateTime: DATE/TIME %s", trans->szDateTime);
	
}

void CreateTransXML(FILE *f, Transaction *trans)
{
	fprintf(f,"<transaction>\n");
    fprintf(f,"<transtype>%s</transtype>\n",trans->sztranstype);
    fprintf(f,"<szIssuerLabel>%s</szIssuerLabel>\n",trans->szIssuerLabel);
    fprintf(f,"<szPAN>%s</szPAN>\n",trans->szPAN);
    fprintf(f,"<szAmount>%s</szAmount>\n",trans->szAmount);
    fprintf(f,"<szInvoiceNumber>%s</szInvoiceNumber>\n",trans->szInvoiceNumber);
	fprintf(f,"<szDateTime>%s</szDateTime>\n",trans->szDateTime);
    fprintf(f,"</transaction>\n");

	vdDebug_LogPrintf("trans->sztranstype:%s", trans->sztranstype);
	vdDebug_LogPrintf("trans->szIssuerLabel:%s", trans->szIssuerLabel);
	vdDebug_LogPrintf("trans->szPAN:%s", trans->szPAN);
	vdDebug_LogPrintf("trans->szAmount:%s", trans->szAmount);
	vdDebug_LogPrintf("trans->szInvoiceNumber:%s", trans->szInvoiceNumber);
}

void CreateTransData(FILE *f, Transaction *trans)
{
    fprintf(f, "%s|%s|%s|%s|%s|%s\n", trans->sztranstype, trans->szIssuerLabel, trans->szPAN, trans->szAmount, trans->szInvoiceNumber, trans->szDateTime);
	
	vdDebug_LogPrintf("trans->sztranstype:%s", trans->sztranstype);
	vdDebug_LogPrintf("trans->szIssuerLabel:%s", trans->szIssuerLabel);
	vdDebug_LogPrintf("trans->szPAN:%s", trans->szPAN);
	vdDebug_LogPrintf("trans->szAmount:%s", trans->szAmount);
	vdDebug_LogPrintf("trans->szInvoiceNumber:%s", trans->szInvoiceNumber);
}



