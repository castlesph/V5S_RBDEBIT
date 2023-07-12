/** 
**    A Template for developing new terminal shared application
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <sqlite3.h>

#include "../Includes/myEZLib.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSTypedef.h"

#include "../Includes/Trans.h"

#include "DatabaseFunc.h"
#include "../FileModule/myFileFunc.h"
#include "../Debug/Debug.h"
#include "../Database/DatabaseFunc.h"
#include "../Includes/POSSetting.h"

sqlite3 * db;
sqlite3_stmt *stmt;
int inStmtSeq = 0;

static BOOL fDBOpen = FALSE;

BOOL fGetDBOpen(void)
{
    //vdDebug_LogPrintf("--fGetDBOpen--");
    //vdDebug_LogPrintf("fDBOpen[%d]",fDBOpen);
    return fDBOpen;
}

void vdSetDBOpen(BOOL fOpen)
{
	//vdDebug_LogPrintf("--vdSetDBOpen--");
    //vdDebug_LogPrintf("fOpen[%d]",fOpen);
    fDBOpen = fOpen;
}

int inHDTReadHostName(char szHostName[][100], int inCPTID[])
{    
	int result;
	char *sql = "SELECT HDTid, szHostLabel FROM HDT WHERE fHostEnable = ?";
	int inCount = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	
	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fHostEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			/* HDTid */
			inCPTID[inCount] = sqlite3_column_int(stmt,inStmtSeq);

            /* szHostName */
			strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			inCount++;
		}
	} while (result == SQLITE_ROW);

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	vdDebug_LogPrintf("inDBResult[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

int inCPTRead(int inSeekCnt)
{     
	int result;
	//char *sql = "SELECT HDTid, szHostName,inCommunicationMode, szPriTxnPhoneNumber, szSecTxnPhoneNumber, szPriSettlePhoneNumber, szSecSettlePhoneNumber, fFastConnect, fDialMode, inDialHandShake, szATCommand, inMCarrierTimeOut, inMRespTimeOut, fPreDial, szPriTxnHostIP, szSecTxnHostIP, szPriSettlementHostIP, szSecSettlementHostIP, inPriTxnHostPortNum, inSecTxnHostPortNum, inPriSettlementHostPort, inSecSettlementHostPort, fTCPFallbackDial, fSSLEnable, inTCPConnectTimeout, inTCPResponseTimeout, inCountryCode, inHandShake, inParaMode, inIPHeader, fCommBackUpMode, inCommunicationBackUpMode FROM CPT WHERE CPTid = ?";
	char *sql = "SELECT HDTid, szHostName,inCommunicationMode, szPriTxnPhoneNumber, szSecTxnPhoneNumber, szPriSettlePhoneNumber, szSecSettlePhoneNumber, fFastConnect, fDialMode, inDialHandShake, szATCommand, inMCarrierTimeOut, inMRespTimeOut, fPreDial, szPriTxnHostIP, szSecTxnHostIP, szPriSettlementHostIP, szSecSettlementHostIP, inPriTxnHostPortNum, inSecTxnHostPortNum, inPriSettlementHostPort, inSecSettlementHostPort, fTCPFallbackDial, fSSLEnable, inTCPConnectTimeout, inTCPResponseTimeout, inCountryCode, inHandShake, inParaMode, inIPHeader, fCommBackUpMode, inCommunicationBackUpMode, szPriTxnHostIPGPRS, szSecTxnHostIPGPRS, szPriSettlementHostIPGPRS, szSecSettlementHostIPGPRS, inPriTxnHostPortNumGPRS, inSecTxnHostPortNumGPRS, inPriSettlementHostPortGPRS, inSecSettlementHostPortGPRS FROM CPT WHERE CPTid = ?";	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		vdDebug_LogPrintf("SATURN open cptrec error");
		return 1;
	}
	
	vdDebug_LogPrintf("saturn inCPTRead[%d]", inSeekCnt);
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	result = sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

                        /* HDTid */
                        strCPT.HDTid = sqlite3_column_int(stmt, inStmtSeq );   
                        
                        /* szHostName*/
			strcpy((char*)strCPT.szHostName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
           
            /* inCommunicationMode */
            strCPT.inCommunicationMode = sqlite3_column_int(stmt, inStmtSeq +=1 );   

			/* szPriTxnPhoneNumber; */
                        strcpy((char*)strCPT.szPriTxnPhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szSecTxnPhoneNumber*/
                        strcpy((char*)strCPT.szSecTxnPhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szPriSettlePhoneNumber*/			 //5
			strcpy((char*)strCPT.szPriSettlePhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        
                        
			/* szSecSettlePhoneNumber*/
			strcpy((char*)strCPT.szSecSettlePhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fFastConnect*/
                        strCPT.fFastConnect = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));   

			/* fDialMode*/
			strCPT.fDialMode = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
                        
			/* inDialHandShake*/
			strCPT.inDialHandShake = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* szATCommand*/
			strcpy((char*)strCPT.szATCommand, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inMCarrierTimeOut*/
			strCPT.inMCarrierTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* inMRespTimeOut*/
			strCPT.inMRespTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );
                        
            /* fPreDial*/
			strCPT.fPreDial = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));

			/* szPriTxnHostIP*/
			strcpy((char*)strCPT.szPriTxnHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
						
                        /* szSecTxnHostIP*/
			strcpy((char*)strCPT.szSecTxnHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
                        /* szPriSettlementHostIP*/
			strcpy((char*)strCPT.szPriSettlementHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
	                
                        /* szSecSettlementHostIP*/
			strcpy((char*)strCPT.szSecSettlementHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		        
                        /* inPriTxnHostPortNum*/
			strCPT.inPriTxnHostPortNum = sqlite3_column_int(stmt, inStmtSeq +=1 );
                          
                        /* inSecTxnHostPortNum*/
			strCPT.inSecTxnHostPortNum = sqlite3_column_int(stmt, inStmtSeq +=1 );
                        
                        /* inPriSettlementHostPort*/
			strCPT.inPriSettlementHostPort = sqlite3_column_int(stmt, inStmtSeq +=1 );
                         
                        /* inSecSettlementHostPort*/
			strCPT.inSecSettlementHostPort = sqlite3_column_int(stmt, inStmtSeq +=1 );
                        vdDebug_LogPrintf("strCPT.inPriTxnHostPortNum[%d]", strCPT.inPriTxnHostPortNum);
			/*fTCPFallbackDial*/
			strCPT.fTCPFallbackDial = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        			
                        /*fSSLEnable*/
			strCPT.fSSLEnable =fGetBoolean((BYTE *) sqlite3_column_text(stmt,inStmtSeq +=1 ));
                       
                        /*inTCPConnectTimeout*/
			strCPT.inTCPConnectTimeout = sqlite3_column_int(stmt, inStmtSeq +=1 );
                            
                        /*inTCPResponseTimeout*/
			strCPT.inTCPResponseTimeout = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/*inCountryCode*/
			strCPT.inCountryCode = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/*inHandShake*/
			strCPT.inHandShake = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/*inParaMode*/
			strCPT.inParaMode = sqlite3_column_int(stmt, inStmtSeq +=1 );

                        /*inIPHeader*/
			strCPT.inIPHeader = sqlite3_column_int(stmt, inStmtSeq +=1 );
                        vdDebug_LogPrintf("strCPT.inIPHeader[%d]", strCPT.inIPHeader);
			/*fCommBackUpMode*/
			strCPT.fCommBackUpMode =fGetBoolean((BYTE *) sqlite3_column_text(stmt,inStmtSeq +=1 ));
		
			/* inCommunicationBackUpMode */
			strCPT.inCommunicationBackUpMode = sqlite3_column_int(stmt, inStmtSeq +=1 );	 
			
			/* szPriTxnHostIPGPRS*/
			strcpy((char*)strCPT.szPriTxnHostIPGPRS, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szSecTxnHostIPGPRS*/
			strcpy((char*)strCPT.szSecTxnHostIPGPRS, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szPriSettlementHostIPGPRS*/
			strcpy((char*)strCPT.szPriSettlementHostIPGPRS, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szSecSettlementHostIPGPRS*/
			strcpy((char*)strCPT.szSecSettlementHostIPGPRS, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inPriTxnHostPortNumGPRS*/
			strCPT.inPriTxnHostPortNumGPRS = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* inSecTxnHostPortNumGPRS*/
			strCPT.inSecTxnHostPortNumGPRS = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* inPriSettlementHostPortGPRS*/
			strCPT.inPriSettlementHostPortGPRS = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* inSecSettlementHostPortGPRS*/
			strCPT.inSecSettlementHostPortGPRS = sqlite3_column_int(stmt, inStmtSeq +=1 );
                        
   
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

int inCPTSave(int inSeekCnt)
{
	int result;
	//char *sql = "UPDATE CPT SET HDTid = ?, szHostName = ?,inCommunicationMode = ?, szPriTxnPhoneNumber = ?, szSecTxnPhoneNumber = ?, szPriSettlePhoneNumber = ?, szSecSettlePhoneNumber = ?, fFastConnect = ?, fDialMode = ?, inDialHandShake = ?, szATCommand = ?, inMCarrierTimeOut = ?, inMRespTimeOut = ?, fPreDial = ?, szPriTxnHostIP = ?, szSecTxnHostIP = ?, szPriSettlementHostIP = ?, szSecSettlementHostIP = ?, inPriTxnHostPortNum = ?, inSecTxnHostPortNum = ?, inPriSettlementHostPort = ?, inSecSettlementHostPort = ?, fTCPFallbackDial = ?, fSSLEnable = ?, inTCPConnectTimeout = ?, inTCPResponseTimeout = ?, inCountryCode = ?, inHandShake = ?, inParaMode = ?, inIPHeader= ?, fCommBackUpMode=?, inCommunicationBackUpMode=? WHERE  CPTid = ?";
	char *sql = "UPDATE CPT SET HDTid = ?, szHostName = ?,inCommunicationMode = ?, szPriTxnPhoneNumber = ?, szSecTxnPhoneNumber = ?, szPriSettlePhoneNumber = ?, szSecSettlePhoneNumber = ?, fFastConnect = ?, fDialMode = ?, inDialHandShake = ?, szATCommand = ?, inMCarrierTimeOut = ?, inMRespTimeOut = ?, fPreDial = ?, szPriTxnHostIP = ?, szSecTxnHostIP = ?, szPriSettlementHostIP = ?, szSecSettlementHostIP = ?, inPriTxnHostPortNum = ?, inSecTxnHostPortNum = ?, inPriSettlementHostPort = ?, inSecSettlementHostPort = ?, fTCPFallbackDial = ?, fSSLEnable = ?, inTCPConnectTimeout = ?, inTCPResponseTimeout = ?, inCountryCode = ?, inHandShake = ?, inParaMode = ?, inIPHeader= ?, fCommBackUpMode=?, inCommunicationBackUpMode=?, szPriTxnHostIPGPRS=?, szSecTxnHostIPGPRS=?, szPriSettlementHostIPGPRS=?, szSecSettlementHostIPGPRS=?, inPriTxnHostPortNumGPRS=?, inSecTxnHostPortNumGPRS=?, inPriSettlementHostPortGPRS=?, inSecSettlementHostPortGPRS=? WHERE  CPTid = ?";
 	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	inStmtSeq = 0;
        
        /* HDTid*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.HDTid);
	/* szHostName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szHostName, strlen((char*)strCPT.szHostName), SQLITE_STATIC);
	/* inCommunicationMode*/	
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inCommunicationMode);
	/* szPriTxnPhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriTxnPhoneNumber, strlen((char*)strCPT.szPriTxnPhoneNumber), SQLITE_STATIC);
        /* szSecTxnPhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecTxnPhoneNumber, strlen((char*)strCPT.szSecTxnPhoneNumber), SQLITE_STATIC);
        /* szPriSettlePhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriSettlePhoneNumber, strlen((char*)strCPT.szPriSettlePhoneNumber), SQLITE_STATIC);
        /* szSecSettlePhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecSettlePhoneNumber, strlen((char*)strCPT.szSecSettlePhoneNumber), SQLITE_STATIC);
        
        /* fFastConnect*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fFastConnect);
        
        /* fDialMode*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fDialMode);
         /* inDialHandShake*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inDialHandShake);       
        
        /* szATCommand*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szATCommand, strlen((char*)strCPT.szATCommand), SQLITE_STATIC);   
        
        /* inMCarrierTimeOut*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inMCarrierTimeOut);       
               
        /* inMRespTimeOut*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inMRespTimeOut);    
   
        /* fPreDial*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fPreDial);
        
	/* szPriTxnHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriTxnHostIP, strlen((char*)strCPT.szPriTxnHostIP), SQLITE_STATIC);
	/* szSecTxnHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecTxnHostIP, strlen((char*)strCPT.szSecTxnHostIP), SQLITE_STATIC);
	/*szPriSettlementHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriSettlementHostIP, strlen((char*)strCPT.szPriSettlementHostIP), SQLITE_STATIC);
	/*szSecSettlementHostIP*/	
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecSettlementHostIP, strlen((char*)strCPT.szSecSettlementHostIP), SQLITE_STATIC);
	/* inPriTxnHostPortNum*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriTxnHostPortNum);
       /* inSecTxnHostPortNum*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecTxnHostPortNum); 
        /* inPriSettlementHostPort*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriSettlementHostPort);          
        /* inSecSettlementHostPort*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecSettlementHostPort);   
        /* fTCPFallbackDial*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fTCPFallbackDial);        
        /* fSSLEnable*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fSSLEnable);           
          /* inTCPConnectTimeout*/	  
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inTCPConnectTimeout);	
        /* inTCPResponseTimeout*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inTCPResponseTimeout);
		/* inCountryCode*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inCountryCode);
		/* inHandShake*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inHandShake);
		/* inParaMode*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inParaMode);
         /* inIPHeader*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inIPHeader);	 

    	/* fCommBackUpMode*/
    	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fCommBackUpMode); 		  
    
    	/* inCommunicationBackUpMode*/	 
    	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inCommunicationBackUpMode);

	/* szPriTxnHostIPGPRS*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriTxnHostIPGPRS, strlen((char*)strCPT.szPriTxnHostIPGPRS), SQLITE_STATIC);
	/* szSecTxnHostIPGPRS*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecTxnHostIPGPRS, strlen((char*)strCPT.szSecTxnHostIPGPRS), SQLITE_STATIC);
	/*szPriSettlementHostIPGPRS*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriSettlementHostIPGPRS, strlen((char*)strCPT.szPriSettlementHostIPGPRS), SQLITE_STATIC);
	/*szSecSettlementHostIPGPRS*/	
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecSettlementHostIPGPRS, strlen((char*)strCPT.szSecSettlementHostIPGPRS), SQLITE_STATIC);
	/* inPriTxnHostPortNumGPRS*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriTxnHostPortNumGPRS);
       /* inSecTxnHostPortNumGPRS*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecTxnHostPortNumGPRS); 
        /* inPriSettlementHostPortGPRS*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriSettlementHostPortGPRS);          
        /* inSecSettlementHostPortGPRS*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecSettlementHostPortGPRS);   
        
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(d_OK);
        
}



int inHDTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM HDT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inTCTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM TCT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inCDTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM CDT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inIITNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM IIT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inMMTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM MMT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inPITNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM PIT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inRDTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM RDT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inEMVNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM EMV";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_EMV,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inAIDNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM AID";
	int inCount = 0;
	
	/* open the database */
	//result = sqlite3_open(DB_TERMINAL,&db);
	result = sqlite3_open(DB_EMV,&db);
	
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inMSGNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM MSG";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inTLENumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM TLE";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inWaveAIDNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM AID";
	int inCount = 0;
	
	/* open the database */
	//result = sqlite3_open(DB_TERMINAL,&db);
	result = sqlite3_open(DB_WAVE,&db);
	
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	//�ر�sqlite����
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inWaveEMVNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM EMV";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_WAVE,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	//�ر�sqlite����
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inHDTRead(int inSeekCnt)
{
	int len;
	CTOS_RTC SetRTC;     
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inHostIndex, szHostLabel, szTPDU, szNII, fReversalEnable, fHostEnable, szTraceNo, fSignOn, ulLastTransSavedIndex, inCurrencyIdx, szAPName, inFailedREV, inDeleteREV, inNumAdv, ulCRC, szMinInstAmt, fFooterLogo, fLast4Digit, szFooterLogoName, szHeaderLogoName, fMaskDetail, fTipAllowFlag, ulTipPercent, fHostTPSec, fHostTLESec FROM HDT WHERE HDTid = ? AND fHostEnable = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);

	
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fHostEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*inHostIndex*/
			strHDT.inHostIndex = sqlite3_column_int(stmt, inStmtSeq );

            /* szHostLabel */
			strcpy((char*)strHDT.szHostLabel, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            
   
			/*szTPDU*/
			memcpy(strHDT.szTPDU, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);

			/* szNII*/
			memcpy(strHDT.szNII, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);

			/* fReversalEnable*/
			strHDT.fReversalEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fHostEnable*/
			strHDT.fHostEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

	
			/* szTraceNo*/
			memcpy(strHDT.szTraceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

			/* fSignOn */
			strHDT.fSignOn = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*ulLastTransSavedIndex*/
			strHDT.ulLastTransSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );

 			/* inCurrencyIdx */	
			strHDT.inCurrencyIdx = sqlite3_column_int(stmt, inStmtSeq +=1 );
            
			/* szAPName */			
			strcpy((char*)strHDT.szAPName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inFailedREV */
			strHDT.inFailedREV = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* inDeleteREV */
			strHDT.inDeleteREV = sqlite3_column_int(stmt,inStmtSeq +=1);

            /* inNumAdv */
			strHDT.inNumAdv = sqlite3_column_int(stmt,inStmtSeq +=1);

			
            /*strHDT.ulCRC*/
            strHDT.ulCRC = sqlite3_column_double(stmt, inStmtSeq +=1 );

			memcpy(strHDT.szMinInstAmt, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);

            /*footer logo*/  
            strHDT.fFooterLogo = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 	

            /*fLast4Digit*/  
            strHDT.fLast4Digit = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            strcpy((char*)strHDT.szFooterLogoName, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            
            strcpy((char*)strHDT.szHeaderLogoName, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            
            strHDT.fMaskDetail = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fTipAllowFlag*/
            strHDT.fTipAllowFlag= fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			strHDT.ulTipPercent = sqlite3_column_double(stmt,inStmtSeq +=1 );

			/* fHostTPSec*/
            strHDT.fHostTPSec= fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

	        /* fHostTLESec*/
            strHDT.fHostTLESec= fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inHDTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE HDT SET inHostIndex = ? , szTPDU = ? ,szNII = ? ,fHostEnable = ? ,szTraceNo = ? ,fSignOn = ?,inFailedREV = ?,inNumAdv = ?, fLast4Digit = ?, fTipAllowFlag = ? WHERE  HDTid = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	inStmtSeq = 0;
    /*inHostIndex*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.inHostIndex);
	/*szTPDU*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strHDT.szTPDU, 5, SQLITE_STATIC);
	/* szNII*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strHDT.szNII, 2, SQLITE_STATIC);
	/* fHostEnable*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.fHostEnable);

	/* szTraceNo*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strHDT.szTraceNo, 3, SQLITE_STATIC);
	/* fSignOn */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.fSignOn);

	/*inFailedREV*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.inFailedREV);

    /*inNumAdv*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.inNumAdv);

	/* fLast4Digit*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.fLast4Digit);

    /* fTipAllowFlag*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.fTipAllowFlag);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(d_OK);
}

int inCDTReadMulti(char *szPAN, int *inFindRecordNum)
{
	int result;
	int inResult = d_NO;
	//char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE ? BETWEEN cast(szPANLo as long) AND cast(szPANHi as long) AND fCDTEnable = ?";
	//char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid, fInstallmentEnable, inInstGroup, inLoyaltyGroup FROM CDT WHERE ? BETWEEN szPANLo AND szPANHi AND fCDTEnable = ?";
    char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid, fInstallmentEnable, inInstGroup, inLoyaltyGroup, szBank_Mnemonic FROM CDT WHERE ? BETWEEN szPANLo AND szPANHi AND fCDTEnable = ?";
	int inDBResult = 0;
	char szTemp[16];
	int inCount=0;
	unsigned long inPAN = 0;
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTRead,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	memcpy(szTemp, szPAN, 10);
	szTemp[10]=0;
	//inPAN = atol(szTemp);
	vdDebug_LogPrintf("inPAN=%s",szTemp);
	inStmtSeq = 0;
	//sqlite3_bind_int(stmt, inStmtSeq +=1, inPAN);
	//sqlite3_bind_double(stmt, inStmtSeq +=1, inPAN);
	sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)szTemp, strlen((char*)szTemp), SQLITE_STATIC);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inCDTRead,result[%d]SQLITE_ROW[%d]",result,SQLITE_ROW);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

		    /*szPANLo*/
		     strcpy(strMCDT[inCount].szPANLo, sqlite3_column_text(stmt,inStmtSeq));

		    /*szPANHi*/
		     strcpy(strMCDT[inCount].szPANHi, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*szCardLabel*/
		    strcpy(strMCDT[inCount].szCardLabel, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*inType*/
			strMCDT[inCount].inType = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMinPANDigit*/
		    strMCDT[inCount].inMinPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMaxPANDigit*/
		    strMCDT[inCount].inMaxPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*inCVV_II*/
		    strMCDT[inCount].inCVV_II = sqlite3_column_int(stmt,inStmtSeq +=1);

            
			/*InFloorLimitAmount*/
			strMCDT[inCount].InFloorLimitAmount = sqlite3_column_double(stmt,inStmtSeq +=1 );

			
			/*fExpDtReqd*/
		    strMCDT[inCount].fExpDtReqd = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
			
			/*fPinpadRequired*/
		    strMCDT[inCount].fPinpadRequired = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

				
			/*fManEntry*/
		    strMCDT[inCount].fManEntry = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    /*fCardPresent*/
		    strMCDT[inCount].fCardPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
		    /*fChkServiceCode*/
		    strMCDT[inCount].fChkServiceCode =fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fluhnCheck */
			strMCDT[inCount].fluhnCheck = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCDTEnable*/
			strMCDT[inCount].fCDTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

            /*IITid*/
		    strMCDT[inCount].IITid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* HDTid*/
			strMCDT[inCount].HDTid = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			strMCDT[inCount].CDTid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*fInstallmentEnable*/
			strMCDT[inCount].fInstallmentEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

			/*inInstGroup*/
			strMCDT[inCount].inInstGroup = sqlite3_column_double(stmt,inStmtSeq +=1 );

            /*inLoyaltyGroup*/
            strMCDT[inCount].inLoyaltyGroup = sqlite3_column_double(stmt,inStmtSeq +=1 );

			/*szBank_Mnemonic*/
		     strcpy(strMCDT[inCount].szBank_Mnemonic, sqlite3_column_text(stmt,inStmtSeq +=1));
			
			inCount ++;
			if(inCount >=10)
				break;
		}
	} while (result == SQLITE_ROW);
  
  *inFindRecordNum = inCount;
  vdDebug_LogPrintf("inCDTReadMulti Record=%d",*inFindRecordNum);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inCDTRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	//char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, fInstallmentEnable, inInstGroup, inLoyaltyGroup FROM CDT WHERE CDTid = ? AND fCDTEnable = ?";
	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, fInstallmentEnable, inInstGroup, inLoyaltyGroup, szBank_Mnemonic, fOnUs FROM CDT WHERE CDTid = ? AND fCDTEnable = ?";
	int inDBResult = 0;	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		vdDebug_LogPrintf("saturn cdt read return 1");
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTRead,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		vdDebug_LogPrintf("saturn cdt read return 2");
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("saturn inCDTRead,result[%d]SQLITE_ROW[%d]",result,SQLITE_ROW);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

		    /*szPANLo*/
		     strcpy(strCDT.szPANLo, sqlite3_column_text(stmt,inStmtSeq));

		    /*szPANHi*/
		     strcpy(strCDT.szPANHi, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*szCardLabel*/
		    strcpy(strCDT.szCardLabel, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*inType*/
			strCDT.inType = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMinPANDigit*/
		    strCDT.inMinPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMaxPANDigit*/
		    strCDT.inMaxPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*inCVV_II*/
		    strCDT.inCVV_II = sqlite3_column_int(stmt,inStmtSeq +=1);

            
			/*InFloorLimitAmount*/
			strCDT.InFloorLimitAmount = sqlite3_column_double(stmt,inStmtSeq +=1 );

			
			/*fExpDtReqd*/
		    strCDT.fExpDtReqd = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
			
			/*fPinpadRequired*/
		    strCDT.fPinpadRequired = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

				
			/*fManEntry*/
		    strCDT.fManEntry = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    /*fCardPresent*/
		    strCDT.fCardPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
		    /*fChkServiceCode*/
		    strCDT.fChkServiceCode =fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fluhnCheck */
			strCDT.fluhnCheck = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCDTEnable*/
			strCDT.fCDTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

            /*IITid*/
		    strCDT.IITid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* HDTid*/
			strCDT.HDTid = sqlite3_column_int(stmt,inStmtSeq +=1);	

			/*fInstallmentEnable*/
			strCDT.fInstallmentEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

			/*inInstGroup*/
			strCDT.inInstGroup = sqlite3_column_double(stmt,inStmtSeq +=1 );			

			/*inLoyaltyGroup*/
			strCDT.inLoyaltyGroup = sqlite3_column_double(stmt,inStmtSeq +=1 );	

			/*szBank_Mnemonic*/
		     strcpy(strCDT.szBank_Mnemonic, sqlite3_column_text(stmt,inStmtSeq +=1));

			/*fOnUs*/
			strCDT.fOnUs = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);
   
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	vdDebug_LogPrintf("saturn cdt read return result %d", inResult);

    return(inResult);
}

int inCDTMAX(void)
{
	int result;	
	char *sql = "SELECT MAX(CDTid) FROM CDT";
	int inMaxValue = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			inMaxValue = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);
    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("inCDTRead,inDBResult[%d]result[%d]",inDBResult,result);
	inDBResult = sqlite3_finalize(stmt);
	
	vdDebug_LogPrintf("inCDTRead,sqlite3_finalize[%d]",inDBResult);
	inDBResult = sqlite3_close(db);
	vdDebug_LogPrintf("inCDTRead,sqlite3_close[%d]",inDBResult);

	return(inMaxValue);
}

int inIITRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inIssuerNumber, szIssuerAbbrev, szIssuerLabel, szPANFormat, szMaskMerchantCopy, szMaskCustomerCopy, szMaskExpireDate, szMaskDisplay, fMerchExpDate, fCustExpDate, fMerchPANFormat, inCheckHost, ulTransSeqCounter, szIssuerLogo, fMaskCustCopy, fMaskMerchCopy, fMaskPan, inMaskDigit FROM IIT WHERE IITid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("IIT read result[%d]", result); 
		vdDebug_LogPrintf("SQLITE_ROW[%d]", SQLITE_ROW); 
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/*inIssuerNumber*/
			strIIT.inIssuerNumber = sqlite3_column_int(stmt, inStmtSeq );
			vdDebug_LogPrintf("inIssuerNumber [%d]", strIIT.inIssuerNumber);
			/* szIssuerAbbrev */			
			strcpy((char*)strIIT.szIssuerAbbrev, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
				
			/* szIssuerLabel */			
			strcpy((char*)strIIT.szIssuerLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szPANFormat*/
			strcpy((char*)strIIT.szPANFormat, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMaskMerchantCopy*/
			strcpy((char*)strIIT.szMaskMerchantCopy, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMaskCustomerCopy*/
			strcpy((char*)strIIT.szMaskCustomerCopy, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMaskExpireDate*/
			strcpy((char*)strIIT.szMaskExpireDate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMaskDisplay*/
			strcpy((char*)strIIT.szMaskDisplay, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fMerchExpDate */
			strIIT.fMerchExpDate = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCustExpDate //5*/
			strIIT.fCustExpDate = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fMerchPANFormat*/
			strIIT.fMerchPANFormat = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	

            /*inCheckHost*/
		    strIIT.inCheckHost = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*ulTransSeqCounter*/
			strIIT.ulTransSeqCounter = sqlite3_column_double(stmt,inStmtSeq +=1);
 
			//Display Issuer logo: get issuer logo filename -- jzg
			strcpy((char*)strIIT.szIssuerLogo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            strIIT.fMaskCustCopy = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
		    strIIT.fMaskMerchCopy = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strIIT.fMaskPan = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strIIT.inMaskDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
        }
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inIITSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE IIT SET szIssuerAbbrev = ?, szIssuerLabel = ?, szPANFormat = ?, szMaskMerchantCopy = ?, szMaskCustomerCopy = ?, szMaskExpireDate = ?, szMaskDisplay = ?, fMerchExpDate = ?, fCustExpDate = ?, fMerchPANFormat = ?, ulTransSeqCounter = ?, szIssuerLogo = ? WHERE  IITid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	/* szIssuerAbbrev */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szIssuerAbbrev, strlen((char*)strIIT.szIssuerAbbrev), SQLITE_STATIC);
	/* szIssuerLabel */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szIssuerLabel, strlen((char*)strIIT.szIssuerLabel), SQLITE_STATIC);
	/* szPANFormat*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szPANFormat, strlen((char*)strIIT.szPANFormat), SQLITE_STATIC);
	/* szMaskMerchantCopy*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskMerchantCopy, strlen((char*)strIIT.szMaskMerchantCopy), SQLITE_STATIC);
	/* szMaskCustomerCopy*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskCustomerCopy, strlen((char*)strIIT.szMaskCustomerCopy), SQLITE_STATIC);
	/* szMaskExpireDate*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskExpireDate, strlen((char*)strIIT.szMaskExpireDate), SQLITE_STATIC);
	/* szMaskDisplay*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskDisplay, strlen((char*)strIIT.szMaskDisplay), SQLITE_STATIC);
	/*fMerchExpDate*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fMerchExpDate);
	/* fCustExpDate */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fCustExpDate);
	/* fMerchPANFormat */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fMerchPANFormat);
	/* ulTransSeqCounter */
    if(strIIT.ulTransSeqCounter >= 0xFFFFFE)
       strIIT.ulTransSeqCounter = 1;
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, strIIT.ulTransSeqCounter);
    vdDebug_LogPrintf(" strIIT.ulTransSeqCounter[%d] result[%d]",strIIT.ulTransSeqCounter, result);

	//Display Issuer logo: save issuer logo filename -- jzg
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szIssuerLogo, strlen((char*)strIIT.szIssuerLogo), SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}


int inPCTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM PCT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inPCTRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT szRctHdr1, szRctHdr2, szRctHdr3, szRctHdr4, szRctHdr5, szRctFoot1, szRctFoot2, szRctFoot3, inPrinterType, inPrintOption, inCustCopyOption, fPrintDisclaimer, fEnablePrinter FROM PCT WHERE PCTid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	vdDebug_LogPrintf("saturn inPCTRead");
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/* szRctHdr1 */			
			strcpy((char*)strPCT.szRctHdr1, (char *)sqlite3_column_text(stmt,inStmtSeq ));
			
			/* szRctHdr2 */			
			strcpy((char*)strPCT.szRctHdr2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctHdr3*/
			strcpy((char*)strPCT.szRctHdr3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr4 */			
			strcpy((char*)strPCT.szRctHdr4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szRctHdr5*/
			strcpy((char*)strPCT.szRctHdr5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctFoot1 */			
			strcpy((char*)strPCT.szRctFoot1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szRctFoot2*/
			strcpy((char*)strPCT.szRctFoot2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctFoot3 */			
			strcpy((char*)strPCT.szRctFoot3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inPrinterType*/
			strPCT.inPrinterType = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*inPrintOption*/
			strPCT.inPrintOption = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*inCustCopyOption*/
			strPCT.inCustCopyOption = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/* fPrintDisclaimer*/
			strPCT.fPrintDisclaimer = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fEnablePrinter*/
			strPCT.fEnablePrinter = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


int inPCTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE PCT SET szRctHdr1 = ?, szRctHdr2 = ?, szRctHdr3 = ?, szRctHdr4 = ?, szRctHdr5 = ?, szRctFoot1 = ?, szRctFoot2 = ?, szRctFoot3 = ?, inPrinterType = ?, inPrintOption = ?, inCustCopyOption = ?, fPrintDisclaimer = ?, fEnablePrinter = ? WHERE  PCTid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	
	/* szRctHdr1 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPCT.szRctHdr1, strlen((char*)strPCT.szRctHdr1), SQLITE_STATIC);
	/* szRctHdr2 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPCT.szRctHdr2, strlen((char*)strPCT.szRctHdr2), SQLITE_STATIC);
	/* szRctHdr3 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPCT.szRctHdr3, strlen((char*)strPCT.szRctHdr3), SQLITE_STATIC);
	/* szRctHdr4 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPCT.szRctHdr4, strlen((char*)strPCT.szRctHdr4), SQLITE_STATIC);
	/* szRctHdr5 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPCT.szRctHdr5, strlen((char*)strPCT.szRctHdr5), SQLITE_STATIC);
	/* szRctFoot1 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPCT.szRctFoot1, strlen((char*)strPCT.szRctFoot1), SQLITE_STATIC);
	/* szRctFoot2 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPCT.szRctFoot2, strlen((char*)strPCT.szRctFoot2), SQLITE_STATIC);
	/* szRctFoot3 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPCT.szRctFoot3, strlen((char*)strPCT.szRctFoot3), SQLITE_STATIC);
	/*inPrinterType*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPCT.inPrinterType);
	/*inPrintOption*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPCT.inPrintOption);
	/*inCustCopyOption*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPCT.inCustCopyOption);
	/* fPrintDisclaimer */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPCT.fPrintDisclaimer);
	/* fEnablePrinter */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPCT.fEnablePrinter);
	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}



int inCSTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM CST";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inCSTRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inCurrencyIndex, szCurSymbol, szCurCode FROM CST WHERE CSTid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	
	vdDebug_LogPrintf("saturn inCSTRead");
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/* inCurrencyIndex */
			strCST.inCurrencyIndex = sqlite3_column_int(stmt,inStmtSeq);
				
			/* szCurSymbol */			
			strcpy((char*)strCST.szCurSymbol, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szCurCode*/
			strcpy((char*)strCST.szCurCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


int inCSTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE CST SET inCurrencyIndex = ?, szCurSymbol = ?, szCurCode = ? WHERE  CSTid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	/* inCurrencyIndex */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCST.inCurrencyIndex);
	/* szCurSymbol */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCST.szCurSymbol, strlen((char*)strCST.szCurSymbol), SQLITE_STATIC);
	/* szCurCode */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCST.szCurCode, strlen((char*)strCST.szCurCode), SQLITE_STATIC);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}



int inMMTReadRecord(int inHDTid,int inMITid)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT MMTid, szHostName, HDTid, szMerchantName, MITid, szTID, szMID, szATCMD1, szATCMD2, szATCMD3, szATCMD4, szATCMD5, fMustSettFlag, szBatchNo, szRctHdr1,szRctHdr2, szRctHdr3, szRctHdr4, szRctHdr5, szRctFoot1, szRctFoot2, szRctFoot3, fMMTEnable, fPasswordEnable, szPassword, inFailedREV FROM MMT WHERE HDTid = ? AND MITid = ? AND fMMTEnable = ? ORDER BY MMTid";
	int incount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

    vdDebug_LogPrintf("inMMTReadRecord inHDTid[%d]inMITid[%d]", inHDTid, inMITid);
    
	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMITid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);
	
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/* MMTid */
			strMMT[incount].MMTid = sqlite3_column_int(stmt, inStmtSeq);
			
			/*szHostName*/
			strcpy((char*)strMMT[incount].szHostName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
            
			/* HDTid */
			strMMT[incount].HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
				
			/* szMerchantName */			
			strcpy((char*)strMMT[incount].szMerchantName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* MITid */
			strMMT[incount].MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*szTID*/
			strcpy((char*)strMMT[incount].szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMID*/
			strcpy((char*)strMMT[incount].szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
			/*szATCMD1*/
			strcpy((char*)strMMT[incount].szATCMD1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD2*/
			strcpy((char*)strMMT[incount].szATCMD2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD3*/
			strcpy((char*)strMMT[incount].szATCMD3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD4*/
			strcpy((char*)strMMT[incount].szATCMD4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD5*/
			strcpy((char*)strMMT[incount].szATCMD5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
	
            /* fMustSettFlag */
			strMMT[incount].fMustSettFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		
			/*szBatchNo*/
			memcpy(strMMT[incount].szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

			/* szRctHdr1 */			
			strcpy((char*)strMMT[incount].szRctHdr1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr2 */			
			strcpy((char*)strMMT[incount].szRctHdr2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr3 */ 		
			strcpy((char*)strMMT[incount].szRctHdr3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr4 */ 		
			strcpy((char*)strMMT[incount].szRctHdr4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr5 */ 		
			strcpy((char*)strMMT[incount].szRctHdr5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1  ));

			/* szRctFoot1 */ 		
			strcpy((char*)strMMT[incount].szRctFoot1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1  ));

			/* szRctFoot2 */		
			strcpy((char*)strMMT[incount].szRctFoot2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctFoot3 */		
			strcpy((char*)strMMT[incount].szRctFoot3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
            /* fMMTEnable */
			strMMT[incount].fMMTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fPasswordEnable */
            strMMT[incount].fPasswordEnable= fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szPassword */ 		
            strcpy((char*)strMMT[incount].szPassword, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inFailedREV */
			strMMT[incount].inFailedREV = sqlite3_column_int(stmt,inStmtSeq +=1);
			
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
    
    return(inResult);
}


int inMMTReadNumofRecords(int inSeekCnt,int *inFindRecordNum)
{
	int result;
	char *sql = "SELECT MMTid, szHostName, HDTid, szMerchantName, MITid, szTID, szMID, szATCMD1, szATCMD2, szATCMD3, szATCMD4, szATCMD5, fMustSettFlag, szBatchNo, szRctHdr1,szRctHdr2, szRctHdr3, szRctHdr4, szRctHdr5, szRctFoot1, szRctFoot2, szRctFoot3, fMMTEnable, fPasswordEnable, szPassword, inFailedREV FROM MMT WHERE HDTid = ? AND fMMTEnable = ? ORDER BY MMTid";
	int incount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inMMTReadNumofRecords=[%d]",result);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* MMTid */
			strMMT[incount].MMTid = sqlite3_column_int(stmt, inStmtSeq);

			/*szHostName*/
			strcpy((char*)strMMT[incount].szHostName, (char *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
            
			/* HDTid */
			strMMT[incount].HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
				
			/* szMerchantName */			
			strcpy((char*)strMMT[incount].szMerchantName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* MITid */
			strMMT[incount].MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*szTID*/
			strcpy((char*)strMMT[incount].szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMID*/
			strcpy((char*)strMMT[incount].szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szATCMD1*/
			strcpy((char*)strMMT[incount].szATCMD1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD2*/
			strcpy((char*)strMMT[incount].szATCMD2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD3*/
			strcpy((char*)strMMT[incount].szATCMD3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD4*/
			strcpy((char*)strMMT[incount].szATCMD4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD5*/
			strcpy((char*)strMMT[incount].szATCMD5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
	
            /* fMustSettFlag */
			strMMT[incount].fMustSettFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
            /*szBatchNo*/
            memcpy(strMMT[incount].szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

			/*szRctHdr1*/
			strcpy((char*)strMMT[incount].szRctHdr1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			
			/*szRctHdr2*/
			strcpy((char*)strMMT[incount].szRctHdr2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctHdr3*/
			strcpy((char*)strMMT[incount].szRctHdr3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctHdr4*/
			strcpy((char*)strMMT[incount].szRctHdr4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctHdr5*/
			strcpy((char*)strMMT[incount].szRctHdr5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctFoot1*/
			strcpy((char*)strMMT[incount].szRctFoot1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctFoot2*/
			strcpy((char*)strMMT[incount].szRctFoot2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctFoot3*/
			strcpy((char*)strMMT[incount].szRctFoot3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			
            /* fMMTEnable */
			strMMT[incount].fMMTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fPasswordEnable */
            strMMT[incount].fPasswordEnable= fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szPassword */ 		
            strcpy((char*)strMMT[incount].szPassword, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inFailedREV */
			strMMT[incount].inFailedREV = sqlite3_column_int(stmt,inStmtSeq +=1);

			incount ++;

		}
	} while (result == SQLITE_ROW);

	   *inFindRecordNum = incount;

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
    
    return(d_OK);
}

int inMMTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE MMT SET MMTid = ? , szHostName = ? ,HDTid = ? ,szMerchantName = ? ,MITid = ? ,szTID = ? ,szMID = ? , szATCMD1 = ? , szATCMD2 = ? , szATCMD3 = ? , szATCMD4 = ? , szATCMD5 = ?, fMustSettFlag = ?, szBatchNo = ?, szRctHdr1 = ?, szRctHdr2 = ?, szRctHdr3 = ?, szRctHdr4 = ?, szRctHdr5 = ?, szRctFoot1 =?, szRctFoot2 =?, szRctFoot3 =?, fMMTEnable =?, inFailedREV=? WHERE  MMTid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
    /* MMMTid */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].MMTid);
    /* szHostName */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szHostName, strlen((char*)strMMT[0].szHostName), SQLITE_STATIC);
	/* HDTid */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].HDTid);
	/* szMerchantName */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szMerchantName, strlen((char*)strMMT[0].szMerchantName), SQLITE_STATIC);
    /* MITid */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].MITid);
    /* szTID*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szTID, strlen((char*)strMMT[0].szTID), SQLITE_STATIC);
	/* szMID*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szMID, strlen((char*)strMMT[0].szMID), SQLITE_STATIC);
	/* szATCMD1 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD1, strlen((char*)strMMT[0].szATCMD1), SQLITE_STATIC);
	/* szATCMD2 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD2, strlen((char*)strMMT[0].szATCMD2), SQLITE_STATIC);
	/* szATCMD3 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD3, strlen((char*)strMMT[0].szATCMD3), SQLITE_STATIC);
	/* szATCMD4 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD4, strlen((char*)strMMT[0].szATCMD4), SQLITE_STATIC);
	/* szATCMD5 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD5, strlen((char*)strMMT[0].szATCMD5), SQLITE_STATIC);   
    /* fMustSettFlag */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].fMustSettFlag);
    /* szBatchNo*/
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strMMT[0].szBatchNo, 3, SQLITE_STATIC);

	/* szRctHdr1 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr1, strlen((char*)strMMT[0].szRctHdr1), SQLITE_STATIC);   

	/* szRctHdr2 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr2, strlen((char*)strMMT[0].szRctHdr2), SQLITE_STATIC);   

	/* szRctHdr3 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr3, strlen((char*)strMMT[0].szRctHdr3), SQLITE_STATIC);   

	/* szRctHdr4 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr4, strlen((char*)strMMT[0].szRctHdr4), SQLITE_STATIC);   

	/* szRctHdr5 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr5, strlen((char*)strMMT[0].szRctHdr5), SQLITE_STATIC);   

	/* szRctFoot1 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctFoot1, strlen((char*)strMMT[0].szRctFoot1), SQLITE_STATIC);   

	/* szRctFoot2 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctFoot2, strlen((char*)strMMT[0].szRctFoot2), SQLITE_STATIC);   

	/* szRctFoot3 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctFoot3, strlen((char*)strMMT[0].szRctFoot2), SQLITE_STATIC);
	/* fMMTEnable */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].fMMTEnable);

	/* inFailedREV */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].inFailedREV);

    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}
	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);


    return(d_OK);
}

int inPITRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT szTransactionTypeName, inTxnTypeID, fTxnEnable, inPasswordLevel FROM PIT WHERE inTxnTypeID = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/*szTransactionTypeName*/
			strcpy((char*)strPIT.szTransactionTypeName, (char *)sqlite3_column_text(stmt,inStmtSeq));
            
			/* inTxnTypeID */
			strPIT.inTxnTypeID = sqlite3_column_int(stmt,inStmtSeq +=1 );
		    
            /* fTxnEnable */
			strPIT.fTxnEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* inPasswordLevel */
			strPIT.inPasswordLevel = sqlite3_column_int(stmt,inStmtSeq +=1 );
		}
	} while (result == SQLITE_ROW);
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	   
    return(inResult);
}

int inPITSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE PIT SET szTransactionTypeName = ? ,inTxnTypeID = ? ,fTxnEnable = ? ,inPasswordLevel = ? WHERE  PITid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
    /* szTransactionTypeName */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPIT.szTransactionTypeName, strlen((char*)strPIT.szTransactionTypeName), SQLITE_STATIC);
	/* inTxnTypeID */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPIT.inTxnTypeID);
	/* fTxnEnable*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPIT.fTxnEnable);
    /* inPasswordLevel */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPIT.inPasswordLevel);

    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

int inEMVRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inSchemeReference, inIssuerReference, inTRMDataPresent, lnEMVFloorLimit, lnEMVRSThreshold, inEMVTargetRSPercent, inEMVMaxTargetRSPercent, inMerchantForcedOnlineFlag, inBlackListedCardSupportFlag, szEMVTACDefault, szEMVTACDenial, szEMVTACOnline, szDefaultTDOL, szDefaultDDOL, inEMVFallbackAllowed, inNextRecord, ulEMVCounter, inEMVAutoSelectAppln, szEMVTermCountryCode, szEMVTermCurrencyCode, inEMVTermCurExp, szEMVTermCapabilities, szEMVTermAddCapabilities, szEMVTermType, szEMVMerchantCategoryCode, szEMVTerminalCategoryCode, inModifyCandListFlag, shRFU1, shRFU2, shRFU3, szRFU1, szRFU2, szRFU3 FROM EMV WHERE EMVid = ?";

	/* open the database */
	result = sqlite3_open(DB_EMV,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		
		//Debug_LogPrintf("EMV read[%d]",result);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*inSchemeReference;*/
			strEMVT.inSchemeReference = sqlite3_column_int(stmt,inStmtSeq);
    
            /*inIssuerReference*/
			strEMVT.inIssuerReference = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inTRMDataPresent*/
			strEMVT.inTRMDataPresent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                            
            /*lnEMVFloorLimit*/
			strEMVT.lnEMVFloorLimit = sqlite3_column_double(stmt,inStmtSeq +=1 );
                
            /*lnEMVRSThreshold*/            //5
			strEMVT.lnEMVRSThreshold = sqlite3_column_double(stmt,inStmtSeq +=1 );
                
            /*inEMVTargetRSPercent*/
			strEMVT.inEMVTargetRSPercent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inEMVMaxTargetRSPercent*/
			strEMVT.inEMVMaxTargetRSPercent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inMerchantForcedOnlineFlag*/
			strEMVT.inMerchantForcedOnlineFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inBlackListedCardSupportFlag*/
			strEMVT.inBlackListedCardSupportFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
            
            /* szEMVTACDefault */            //10
			memcpy(strEMVT.szEMVTACDefault, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
           
            /* szEMVTACDenial */            
			memcpy(strEMVT.szEMVTACDenial, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
            
            /* szEMVTACOnline */
			memcpy(strEMVT.szEMVTACOnline, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
            
            /* szDefaultTDOL */
			strcpy((char*)strEMVT.szDefaultTDOL, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szDefaultDDOL */
			strcpy((char*)strEMVT.szDefaultDDOL, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*inEMVFallbackAllowed*/        //15
			strEMVT.inEMVFallbackAllowed = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /*inNextRecord*/
			strEMVT.inNextRecord = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /*ulEMVCounter*/
			strEMVT.ulEMVCounter = sqlite3_column_double(stmt, inStmtSeq +=1 );

            /*inEMVAutoSelectAppln*/
			strEMVT.inEMVAutoSelectAppln = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szEMVTermCountryCode */
			memcpy(strEMVT.szEMVTermCountryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			
            /* szEMVTermCurrencyCode */        //20
			memcpy(strEMVT.szEMVTermCurrencyCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            
            /*inEMVTermCurExp*/
			strEMVT.inEMVTermCurExp = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szEMVTermCapabilities*/
			memcpy(strEMVT.szEMVTermCapabilities, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
               
            /* szEMVTermAddCapabilities */
			memcpy(strEMVT.szEMVTermAddCapabilities, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);
    
            /* szEMVTermType */
			memcpy(strEMVT.szEMVTermType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            
            /* szEMVMerchantCategoryCode */        //25
			memcpy(strEMVT.szEMVMerchantCategoryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);

            /* szEMVTerminalCategoryCode */
			memcpy(strEMVT.szEMVTerminalCategoryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

            /*inModifyCandListFlag*/
			strEMVT.inModifyCandListFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU1*/
			strEMVT.shRFU1 = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU2*/
			strEMVT.shRFU2 = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU3*/                            //30
			strEMVT.shRFU3 = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szRFU1*/
			strcpy((char*)strEMVT.szRFU1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
    
            /* szRFU2 */
			strcpy((char*)strEMVT.szRFU2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szRFU3 */
			strcpy((char*)strEMVT.szRFU3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}
int inAIDReadbyRID(int inSeekCnt, char * PRid)
{
	int result;
	int inResult = d_NO;
	char *sql = "select ulRef, bAIDLen, pbAID, bApplication_Selection_Indicator, inEMVid, inCAPKindex1, pbExpireDate1, inCAPKindex2, pbExpireDate2, inCAPKindex3, pbExpireDate3, inCAPKindex4, pbExpireDate4, inCAPKindex5, pbExpireDate5, inCAPKindex6, pbExpireDate6, inCAPKindex7, pbExpireDate7, inCAPKindex8, pbExpireDate8, inCAPKindex9, pbExpireDate9, inCAPKindex10, pbExpireDate10, pbTerminalAVN, pb2ndTerminalAVN from AID where AIDid = ?";
	BOOL fRIDMatchFlag = FALSE;


	/* open the database */
	result = sqlite3_open(DB_EMV,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		
		//vdDebug_LogPrintf("strAIDT.result[%d]AIDid[%d]",result,inSeekCnt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
			
			/*ulRef*/
			strAIDT.ulRef = sqlite3_column_double(stmt, inStmtSeq);

			/*bAIDLen*/
			strAIDT.bAIDLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			//Debug_LogPrintf("strAIDT.bAIDLen[%d]",strAIDT.bAIDLen);

			/* pbAID */
			memcpy(strAIDT.pbAID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), strAIDT.bAIDLen);
			vdDebug_LogPrintf("strAIDT.bAIDLen[%d]",strAIDT.bAIDLen);
			DebugAddHEX("read AID =",strAIDT.pbAID,strAIDT.bAIDLen);
			if((memcmp(strAIDT.pbAID, PRid, 5) == 0))
			{
				fRIDMatchFlag = TRUE;
			}
			
			/*bApplication_Selection_Indicator*/
			strAIDT.bApplication_Selection_Indicator = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*inEMVid*/
			strAIDT.inEMVid = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*inCAPKindex1*/
			strAIDT.inCAPKindex1 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate1*/
			strcpy((char*)strAIDT.pbExpireDate1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex2*/
			strAIDT.inCAPKindex2 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate2*/
			strcpy((char*)strAIDT.pbExpireDate2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex3*/
			strAIDT.inCAPKindex3 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate3*/
			strcpy((char*)strAIDT.pbExpireDate3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex4*/
			strAIDT.inCAPKindex4 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate4*/
			strcpy((char*)strAIDT.pbExpireDate4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		
			/*inCAPKindex5*/
			strAIDT.inCAPKindex5 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate5*/
			strcpy((char*)strAIDT.pbExpireDate5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/*inCAPKindex6*/
			strAIDT.inCAPKindex6 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate6*/
			strcpy((char*)strAIDT.pbExpireDate6, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

					
			/*inCAPKindex7*/
			strAIDT.inCAPKindex7 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate7*/
			strcpy((char*)strAIDT.pbExpireDate7, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex8*/
			strAIDT.inCAPKindex8 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate8*/
			strcpy((char*)strAIDT.pbExpireDate8, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex9*/
			strAIDT.inCAPKindex9 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate9*/
			strcpy((char*)strAIDT.pbExpireDate9, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex10*/
			strAIDT.inCAPKindex4 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate10*/
			strcpy((char*)strAIDT.pbExpireDate10, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* pbTerminalAVN */
			memcpy(strAIDT.pbTerminalAVN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);
			/* pb2ndTerminalAVN */
			memcpy(strAIDT.pb2ndTerminalAVN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);


		}
	} while (result == SQLITE_ROW);    

	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	if(fRIDMatchFlag == TRUE)
		return(99);
	else
		return(inResult);
}

int inAIDRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "select ulRef, bAIDLen, pbAID, bApplication_Selection_Indicator, inEMVid, inCAPKindex1, pbExpireDate1, inCAPKindex2, pbExpireDate2, inCAPKindex3, pbExpireDate3, inCAPKindex4, pbExpireDate4, inCAPKindex5, pbExpireDate5, inCAPKindex6, pbExpireDate6, inCAPKindex7, pbExpireDate7, inCAPKindex8, pbExpireDate8, inCAPKindex9, pbExpireDate9, inCAPKindex10, pbExpireDate10, pbTerminalAVN, pb2ndTerminalAVN from AID where AIDid = ?";



	/* open the database */
	result = sqlite3_open(DB_EMV,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		
		//vdDebug_LogPrintf("strAIDT.result[%d]AIDid[%d]",result,inSeekCnt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
			
            /*ulRef*/
			strAIDT.ulRef = sqlite3_column_double(stmt, inStmtSeq);

            /*bAIDLen*/
			strAIDT.bAIDLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			//Debug_LogPrintf("strAIDT.bAIDLen[%d]",strAIDT.bAIDLen);

			/* pbAID */
			memcpy(strAIDT.pbAID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), strAIDT.bAIDLen);
			vdDebug_LogPrintf("strAIDT.bAIDLen[%d]",strAIDT.bAIDLen);
            DebugAddHEX("AID =",strAIDT.pbAID,strAIDT.bAIDLen);
                 
            /*bApplication_Selection_Indicator*/
			strAIDT.bApplication_Selection_Indicator = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*inEMVid*/
			strAIDT.inEMVid = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*inCAPKindex1*/
			strAIDT.inCAPKindex1 = sqlite3_column_int(stmt,inStmtSeq +=1 );
            /* pbExpireDate1*/
			strcpy((char*)strAIDT.pbExpireDate1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex2*/
			strAIDT.inCAPKindex2 = sqlite3_column_int(stmt,inStmtSeq +=1 );
            /* pbExpireDate2*/
			strcpy((char*)strAIDT.pbExpireDate2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex3*/
			strAIDT.inCAPKindex3 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate3*/
			strcpy((char*)strAIDT.pbExpireDate3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex4*/
			strAIDT.inCAPKindex4 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate4*/
			strcpy((char*)strAIDT.pbExpireDate4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		
			/*inCAPKindex5*/
			strAIDT.inCAPKindex5 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate5*/
			strcpy((char*)strAIDT.pbExpireDate5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/*inCAPKindex6*/
			strAIDT.inCAPKindex6 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate6*/
			strcpy((char*)strAIDT.pbExpireDate6, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

					
			/*inCAPKindex7*/
			strAIDT.inCAPKindex7 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate7*/
			strcpy((char*)strAIDT.pbExpireDate7, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex8*/
			strAIDT.inCAPKindex8 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate8*/
			strcpy((char*)strAIDT.pbExpireDate8, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex9*/
			strAIDT.inCAPKindex9 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate9*/
			strcpy((char*)strAIDT.pbExpireDate9, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex10*/
			strAIDT.inCAPKindex4 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate10*/
			strcpy((char*)strAIDT.pbExpireDate10, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* pbTerminalAVN */
			memcpy(strAIDT.pbTerminalAVN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);
			/* pb2ndTerminalAVN */
			memcpy(strAIDT.pb2ndTerminalAVN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);


		}
	} while (result == SQLITE_ROW);	   

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inTCTRead(int inSeekCnt)
{
	int result;
	int len = 0;
	int inResult = -1;
 	char *sql = "SELECT inMainLine, fHandsetPresent, fDemo, fCVVEnable, szSuperPW, szSystemPW, szEngineerPW, szFunKeyPW, szPMpassword, byRS232DebugPort, byTerminalType, byPinPadType, byPinPadPort,inPrinterHeatLevel, inWaitTime, byCtlsMode, byERMMode, fTipAllowFlag, fSignatureFlag, fDebitFlag, szFallbackTime, inFallbackTimeGap, fManualEntryInv, szInvoiceNo, szPabx, szLastInvoiceNo, fECR, fDebugOpenedFlag,fShareComEnable, inReversalCount, inTipAdjustCount, fPrintISOMessage, fNSR, fNSRCustCopy, szNSRLimit, szCTLSLimit, usTMSGap, inTMSComMode, usTMSRemotePort, szTMSRemoteIP, szTMSRemotePhone, szTMSRemoteID, szTMSRemotePW, fFirstInit, inLogonHostIndex, szLogonDate, fProductCode, fCustomerNo, fPrintBankCopy, fRegUSD, fAutoDebitLogon, fPrintMerchCopy, szPrintOption, fDebitEMVCapable, fDebitTC, inPrintISOOption, szEMVDebitTermCapability, fPrintBalInqCopy, fPrintSettleDetailReport, fConfirmPAN, fPrintReceipt, byTerminalModel, fTPSec, fImageAnimated, szAppVersion, feReceiptEnable, inGPRSFallback, fTLESec, szServiceFee, szCashoutMinAmt, szCashoutMaxAmt, byDLNotFinished, inISOLogger, inISOTxnLogLimit, inISOTxnLogCnt, fCashoutAmt100, fECRFormatPan, fECRMaskExpiryDate, szRequestKeyDate, szUserName, fAtmMsg, szPOSCashInAcquirerFeeOnUs, szPOSCashOutAcquirerFeeOnUs, szBalInqAcquirerFee, szBillsPaymentServiceFee, szPOSCashOutAcquirerFeeOffUs, szATMCashOutAcquirerFeeOnUs, szATMCashOutAcquirerFeeOffUs, fSwipeEnable, fCTLSEnable, fFalllbackEnable FROM TCT WHERE TCTid = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		vdDebug_LogPrintf("saturn inTCTRead ERROR");
		return 1;
	}

	vdDebug_LogPrintf("saturn inTCTRead");
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
			
    
            /* inMainLine */
			strTCT.inMainLine = sqlite3_column_int(stmt,inStmtSeq  );

            /* fHandsetPresent */
			strTCT.fHandsetPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
    
             /* fDemo //5*/
			strTCT.fDemo = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* fCVVEnable */
			strTCT.fCVVEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        
            /* szSuperPW*/
			strcpy((char*)strTCT.szSuperPW, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szSystemPW */
			strcpy((char*)strTCT.szSystemPW, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szEngineerPW */
			strcpy((char*)strTCT.szEngineerPW, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szFunKeyPW */
			strcpy((char*)strTCT.szFunKeyPW, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szPMpassword */
			strcpy((char*)strTCT.szPMpassword, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
               
            /*byRS232DebugPort*/
			strTCT.byRS232DebugPort = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/*byTerminalType*/
			strTCT.byTerminalType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/*byPinPadType*/
			strTCT.byPinPadType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/*byPinPadPort*/
			strTCT.byPinPadPort = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/*inPrinterHeatLevel*/
			strTCT.inPrinterHeatLevel = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/*inWaitTime*/
			strTCT.inWaitTime = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/*byCtlsMode*/
			strTCT.byCtlsMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/*byERMMode*/
			strTCT.byERMMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
            
 			/* fTipAllowFlag */
			strTCT.fTipAllowFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fSignatureFlag */
			strTCT.fSignatureFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fDebitFlag */
            strTCT.fDebitFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szFallbackTime*/
			strcpy((char*)strTCT.szFallbackTime, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

             /* inFallbackTimeGap */
			strTCT.inFallbackTimeGap = sqlite3_column_int(stmt,inStmtSeq +=1 );

             /* fManualEntryInv */
            strTCT.fManualEntryInv = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			 
			/* szInvoiceNo*/
			memcpy(strTCT.szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

            /* szPabx */
			strcpy((char*)strTCT.szPabx, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			DelCharInStr(strTCT.szPabx,',',2);
			len = strlen((char*)strTCT.szPabx);
			if(len > 0)
				strTCT.szPabx[len]=',';

			/* szLastInvoiceNo */
			memcpy(strTCT.szLastInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			
			/* fECR */
			strTCT.fECR = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fDebugOpenedFlag */
            strTCT.fDebugOpenedFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fShareComEnable */
            strTCT.fShareComEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* inReversalCount */
			strTCT.inReversalCount = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* inTipAdjustCount */
			strTCT.inTipAdjustCount = sqlite3_column_int(stmt,inStmtSeq +=1 );

		/*fPrintISOMessage*/
		strTCT.fPrintISOMessage = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
	
			/*fNSR*/
			strTCT.fNSR = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fNSRCustCopy*/
			strTCT.fNSRCustCopy = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		
            /* szNSRLimit*/
			strcpy((char*)strTCT.szNSRLimit, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szCTLSLimit*/
			strcpy((char*)strTCT.szCTLSLimit, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* usTMSGap */
			strTCT.usTMSGap = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* inTMSComMode */
			strTCT.inTMSComMode = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* usTMSRemotePort */
			strTCT.usTMSRemotePort = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* szTMSRemoteIP */
            strcpy((char*)strTCT.szTMSRemoteIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
            /* szTMSRemotePhone */
            strcpy((char*)strTCT.szTMSRemotePhone, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szTMSRemoteID */
            strcpy((char*)strTCT.szTMSRemoteID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szTMSRemotePW */
            strcpy((char*)strTCT.szTMSRemotePW, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fFirstInit */
			strTCT.fFirstInit = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inLogonHostIndex*/
		    strTCT.inLogonHostIndex = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* szLogonDate */
            memcpy(strTCT.szLogonDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2); 

            /*fProductCode*/
            strTCT.fProductCode= fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /*fCustomerNo*/
            strTCT.fCustomerNo = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*fPrintBankCopy*/
            strTCT.fPrintBankCopy = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*fRegUSD*/
            strTCT.fRegUSD = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*fAutoDebitLogon*/
            strTCT.fAutoDebitLogon = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*fPrintMerchCopy*/
            strTCT.fPrintMerchCopy = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		
			
            strcpy((char*)strTCT.szPrintOption, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));			

            /*fDebit EMV Enable/Disable*/
            strTCT.fDebitEMVCapable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	

            /*Debit TC Enable/Disable*/  
            strTCT.fDebitTC = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			/*inPrintISOOption*/
			strTCT.inPrintISOOption = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*szEMVDebitTermCapability*/
			strcpy((char*)strTCT.szEMVDebitTermCapability, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fPrintBalInqCopy*/
			strTCT.fPrintBalInqCopy = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fPrintSettleDetailReport*/
			strTCT.fPrintSettleDetailReport = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fConfirmPAN*/
            strTCT.fConfirmPAN = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*Print transaction receipt*/
            strTCT.fPrintReceipt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*byTerminalModel*/
			strTCT.byTerminalModel = sqlite3_column_int(stmt,inStmtSeq +=1 );
			
            /*fTPSec*/
			strTCT.fTPSec = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fImageAnimated*/
			strTCT.fImageAnimated = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szAppVersion*/
		    strcpy((char*)strTCT.szAppVersion, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		    vdDebug_LogPrintf("strTCT.szAppVersion = [%s]", strTCT.szAppVersion);

			/*feReceiptEnable*/
			strTCT.feReceiptEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			/* inGPRSFallback*/
					strTCT.inGPRSFallback = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*fTLESec*/
	  	    strTCT.fTLESec = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szServiceFee - sidumili*/
			strcpy((char*)strTCT.szServiceFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szCashoutMinAmt - sidumili*/
       	 	strcpy((char*)strTCT.szCashoutMinAmt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szCashoutMaxAmt - sidumili*/
       	 	strcpy((char*)strTCT.szCashoutMaxAmt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* byDLNotFinished */
		    strTCT.byDLNotFinished = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/* inISOLogger */
           strTCT.inISOLogger = sqlite3_column_int(stmt,inStmtSeq +=1);
        
           /* inISOTxnLogLimit */ 
           strTCT.inISOTxnLogLimit = sqlite3_column_int(stmt,inStmtSeq +=1);

		   /*inISOTxnLogCnt*/
           strTCT.inISOTxnLogCnt = sqlite3_column_int(stmt,inStmtSeq +=1);

		   /* fCashoutAmt100 */
		   strTCT.fCashoutAmt100 = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*fECRFormatPan*/
            strTCT.fECRFormatPan = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /*fECRMaskExpiryDate*/
            strTCT.fECRMaskExpiryDate = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szLogonDate */
            memcpy(strTCT.szRequestKeyDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2); 
            
            /* szUserName */
            strcpy((char*)strTCT.szUserName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

 			/* fAtmMsg */
			strTCT.fAtmMsg = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szPOSCashInAcquirerFeeOnUs */
            strcpy((char*)strTCT.szPOSCashInAcquirerFeeOnUs, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szPOSCashOutAcquirerFeeOnUs */
            strcpy((char*)strTCT.szPOSCashOutAcquirerFeeOnUs, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szBalInqAcquirerFee */
            strcpy((char*)strTCT.szBalInqAcquirerFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szBillsPaymentServiceFee */
            strcpy((char*)strTCT.szBillsPaymentServiceFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szPOSCashOutAcquirerFeeOffUs */
            strcpy((char*)strTCT.szPOSCashOutAcquirerFeeOffUs, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szATMCashOutAcquirerFeeOnUs */
            strcpy((char*)strTCT.szATMCashOutAcquirerFeeOnUs, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szATMCashOutAcquirerFeeOffUs */
            strcpy((char*)strTCT.szATMCashOutAcquirerFeeOffUs, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			//fSwipeEnable, fCTLSEnable, fFalllbackEnable
			
            /* fSwipeEnable */
            strTCT.fSwipeEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* fCTLSEnable */
            strTCT.fCTLSEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* fFalllbackEnable */
            strTCT.fFalllbackEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
        }
	} while (result == SQLITE_ROW);	

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

	//char *sql = "UPDATE TCT SET inMainLine = ?, fHandsetPresent = ?, fDemo = ?, fCVVEnable = ?, szSuperPW = ?, szSystemPW = ?, szEngineerPW = ?, szFunKeyPW = ?, szPMpassword = ?, byRS232DebugPort = ?, inPrinterHeatLevel = ?, inWaitTime = ?, byCtlsMode = ?, fTipAllowFlag = ?, fDebitFlag = ?, inFallbackTimeGap = ?, fManualEntryInv = ?, szInvoiceNo = ?, szPabx = ?, szLastInvoiceNo = ?, fDebugOpenedFlag = ?, fShareComEnable = ?, inReversalCount = ?, inTipAdjustCount = ?, usTMSGap = ?, inTMSComMode = ?, usTMSRemotePort = ?, szTMSRemoteIP = ?, szTMSRemotePhone = ?, fPrintISOMessage = ?, fSignatureFlag = ?, fAutoLogon = ?, szLogonDate = ?, byPinPadType = ? , byPinPadPort = ?, szTMSRemoteID = ?, szTMSRemotePW = ?, fFirstInit = ? WHERE  TCTid = ?";    		
int inTCTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE TCT SET inMainLine = ?, fHandsetPresent = ?, fDemo = ?, fCVVEnable = ?, szSuperPW = ?, szSystemPW = ?, szEngineerPW = ?, szFunKeyPW = ?, szPMpassword = ?, byRS232DebugPort = ?, byTerminalType = ?, byPinPadType = ?, byPinPadPort = ?, inPrinterHeatLevel = ?, inWaitTime = ?, byCtlsMode = ?, byERMMode = ?, fTipAllowFlag = ?, fSignatureFlag = ?, fDebitFlag = ?, inFallbackTimeGap = ?, fManualEntryInv = ?, szInvoiceNo = ?, szPabx = ?, szLastInvoiceNo = ?, fDebugOpenedFlag = ?, fShareComEnable = ?, inReversalCount = ?, inTipAdjustCount = ?, fPrintISOMessage = ?, fNSR = ?, fNSRCustCopy = ?, szNSRLimit = ?, szCTLSLimit = ?, usTMSGap = ?, inTMSComMode = ?, usTMSRemotePort = ?, szTMSRemoteIP = ?, szTMSRemotePhone = ?, szTMSRemoteID = ?, szTMSRemotePW = ?, fFirstInit = ?, szLogonDate = ?, inGPRSFallback = ?, byDLNotFinished = ?, inISOLogger = ?, inISOTxnLogLimit = ?, inISOTxnLogCnt = ?, fCashoutAmt100 = ?, fECRFormatPan = ?, fECRMaskExpiryDate = ?, szRequestKeyDate = ? WHERE  TCTid = ?";    		

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
    vdDebug_LogPrintf("[inTCTSave]-fDemo[%d]",strTCT.fDemo);
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);   
    vdDebug_LogPrintf("[inTCTSave]-sqlite3_prepare_v2[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;

	/* inMainLine */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inMainLine);
	/* fHandsetPresent */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fHandsetPresent);
    /* fDemo */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fDemo);
	/* fCVVEnable */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fCVVEnable);
	/* szSuperPW */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szSuperPW, strlen((char*)strTCT.szSuperPW), SQLITE_STATIC);
	/* szSystemPW */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szSystemPW, strlen((char*)strTCT.szSystemPW), SQLITE_STATIC);
	/* szEngineerPW */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szEngineerPW, strlen((char*)strTCT.szEngineerPW), SQLITE_STATIC);
	
	/* szFunKeyPW */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szFunKeyPW, strlen((char*)strTCT.szFunKeyPW), SQLITE_STATIC);
	/* szPMpassword */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szPMpassword, strlen((char*)strTCT.szPMpassword), SQLITE_STATIC);
	/* byRS232DebugPort */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.byRS232DebugPort);
	/* byTerminalType */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.byTerminalType);
	/* byPinPadType */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.byPinPadType);
	/* byPinPadPort */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.byPinPadPort);
	/* inPrinterHeatLevel */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inPrinterHeatLevel);
	/* inWaitTime */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inWaitTime);
	/* byCtlsMode */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.byCtlsMode);
	/* byERMMode */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.byERMMode);

	/* fTipAllowFlag */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fTipAllowFlag);
	/* fSignatureFlag */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fSignatureFlag);

    /* fDebitFlag */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fDebitFlag);

    /*inFallbackTimeGap*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inFallbackTimeGap);

    /* fManualEntryInv */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fManualEntryInv);

	/* szInvoiceNo*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strTCT.szInvoiceNo, 3, SQLITE_STATIC);

    /* szPabx*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szPabx, strlen((char*)strTCT.szPabx), SQLITE_STATIC);
	/* szLastInvoiceNo*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strTCT.szLastInvoiceNo, 3, SQLITE_STATIC);

    /* fDebugOpenedFlag*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fDebugOpenedFlag);

    /* fShareComEnable*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fShareComEnable);

    /*inReversalCount*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inReversalCount);

    /*inTipAdjustCount*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inTipAdjustCount);

	/*fPrintISOMessage*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fPrintISOMessage);
	/*fNSR*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fNSR);
	/*fNSRCustCopy*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fNSRCustCopy);	
	/* szNSRLimit */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szNSRLimit, strlen((char*)strTCT.szNSRLimit), SQLITE_STATIC);
	/* szCTLSLimit */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szCTLSLimit, strlen((char*)strTCT.szCTLSLimit), SQLITE_STATIC);

    /*usTMSGap*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.usTMSGap);
	/*inTMSComMode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inTMSComMode);

    /*usTMSRemotePort*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.usTMSRemotePort);

    /* szTMSRemoteIP*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szTMSRemoteIP, strlen((char*)strTCT.szTMSRemoteIP), SQLITE_STATIC);
	
    /* szTMSRemotePhone*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szTMSRemotePhone, strlen((char*)strTCT.szTMSRemotePhone), SQLITE_STATIC);

	/* szTMSRemoteID*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szTMSRemoteID, strlen((char*)strTCT.szTMSRemoteID), SQLITE_STATIC);

	/* szTMSRemotePW*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCT.szTMSRemotePW, strlen((char*)strTCT.szTMSRemotePW), SQLITE_STATIC);

	/* fFirstInit */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fFirstInit);

    /*szLogonDate*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strTCT.szLogonDate, 2, SQLITE_STATIC);
	
	/*inGPRSFallback*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inGPRSFallback);

	/* byDLNotFinished */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.byDLNotFinished);

	/*inISOLogger*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inISOLogger);

	/*inISOTxnLogLimit*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inISOTxnLogLimit);

	/*inISOTxnLogCnt*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inISOTxnLogCnt);

	/* fCashoutAmt100 */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fCashoutAmt100);

     /*fECRFormatPan*/
     result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fECRFormatPan);
     
     /*fECRMaskExpiryDate*/
     result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.fECRMaskExpiryDate);

    /*szRequestKeyDate*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strTCT.szRequestKeyDate, 2, SQLITE_STATIC);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
    vdDebug_LogPrintf("[inTCTSave]-end ");

    return(d_OK);
}

int inTCTMenuSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE TCT SET byTerminalType = ?, inMenuid = ? WHERE  TCTid = ?";    		

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
    vdDebug_LogPrintf("[inTCTSave]-fDemo[%d]",strTCT.fDemo);
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);   
    vdDebug_LogPrintf("[inTCTSave]-sqlite3_prepare_v2[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;

	/* byTerminalType */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.byTerminalType);

	/* inMenuid */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inMenuid);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
    vdDebug_LogPrintf("[inTCTSave]-end ");

    return(d_OK);
}

int inTCPRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT szTerminalIP, szGetWay, szSubNetMask, szHostDNS1, szHostDNS2, fDHCPEnable, szAPN, szUserName, szPassword FROM TCP WHERE TCPid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}


	
	vdDebug_LogPrintf("saturn inTCPRead");

	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/*szTerminalIP*/
			strcpy((char*)strTCP.szTerminalIP, (char *)sqlite3_column_text(stmt,inStmtSeq));
            				
			/* szGetWay */			
			strcpy((char*)strTCP.szGetWay, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szSubNetMask*/
			strcpy((char*)strTCP.szSubNetMask, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szHostDNS1*/
			strcpy((char*)strTCP.szHostDNS1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szHostDNS2*/
			strcpy((char*)strTCP.szHostDNS2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fDHCPEnable */
			strTCP.fDHCPEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szAPN*/
			strcpy((char*)strTCP.szAPN, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*szUserName*/
			strcpy((char*)strTCP.szUserName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*szPassword*/
			strcpy((char*)strTCP.szPassword, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inTCPSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE TCP SET szTerminalIP = ? ,szGetWay = ? ,szSubNetMask = ? ,szHostDNS1 = ? ,szHostDNS2 = ? ,fDHCPEnable = ? ,szAPN = ? ,szUserName = ? ,szPassword = ? WHERE  TCPid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
    /* szTerminalIP */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szTerminalIP, strlen((char*)strTCP.szTerminalIP), SQLITE_STATIC);
	/* szGetWay */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szGetWay, strlen((char*)strTCP.szGetWay), SQLITE_STATIC);
    /* szSubNetMask*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szSubNetMask, strlen((char*)strTCP.szSubNetMask), SQLITE_STATIC);
	/* szHostDNS1*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szHostDNS1, strlen((char*)strTCP.szHostDNS1), SQLITE_STATIC);
	/* szHostDNS2*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szHostDNS2, strlen((char*)strTCP.szHostDNS2), SQLITE_STATIC);
    /* fDHCPEnable*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCP.fDHCPEnable);
    /* szAPN*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szAPN, strlen((char*)strTCP.szAPN), SQLITE_STATIC);
    /* szUserName*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szUserName, strlen((char*)strTCP.szUserName), SQLITE_STATIC);
    /* szPassword*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szPassword, strlen((char*)strTCP.szPassword), SQLITE_STATIC);


    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}


int inMSGRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
  	char *sql = "select usMsgIndex, szMsg, szResponseCode, inHostIndex, szHostName from MSG where MSGid = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*usMsgIndex*/
			strMSG.usMsgIndex = sqlite3_column_int(stmt,inStmtSeq);
    
            /* szMeg */
			strcpy((char*)strMSG.szMsg, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


			/* szResponseCode */
			strcpy((char*)strMSG.szResponseCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


			
			/* inHostIndex */
			strMSG.inHostIndex, sqlite3_column_int(stmt,inStmtSeq +=1 );


						
			/* szHostName */
			strcpy((char*)strMSG.szHostName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
   		}
	} while (result == SQLITE_ROW);	   

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inTLERead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
   	char *sql = "select szVERSION, szTMKRefNum, szZMK, szTMK, szTAK, szTermPinEncryuptKey, szMACKey, szLineEncryptKey from TLE where TLEid = ?";
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/* szVERSION */
			memcpy(strTLE.szVERSION, sqlite3_column_blob(stmt,inStmtSeq), 2);

			/* szTMKRefNum*/
			strcpy((char*)strTLE.szTMKRefNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* Zone Master key*/
			memcpy(strTLE.szZMK, sqlite3_column_blob(stmt,inStmtSeq +=1), 16);
				
			/* Terminal Master key*/
			memcpy(strTLE.szTMK, sqlite3_column_blob(stmt,inStmtSeq +=1), 16);

			/* Terminal A key*/
			memcpy(strTLE.szTAK, sqlite3_column_blob(stmt,inStmtSeq +=1), 16);
				
			/* szTermPinEncryuptKey*/
			memcpy(strTLE.szTermPinEncryuptKey, sqlite3_column_blob(stmt,inStmtSeq +=1), 16);

			/* szMACKey*/
			memcpy(strTLE.szMACKey, sqlite3_column_blob(stmt,inStmtSeq +=1), 16);

			/* szLineEncryptKey*/
			memcpy(strTLE.szLineEncryptKey, sqlite3_column_blob(stmt,inStmtSeq +=1), 16);
   		}
	} while (result == SQLITE_ROW);	

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inTLESave(int inSeekCnt)
{   
    return d_OK;
}


int inWaveAIDRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "select ulRef, fAIDEnable, inAIDLen, pbAID, inApplication_Selection_Indicator, EMVid, inCAPKindex1, pbExpireDate1, pbCAPKExponent1, inCAPKindex2, pbExpireDate2, pbCAPKExponent2, inCAPKindex3, pbExpireDate3, pbCAPKExponent3, inCAPKindex4, pbExpireDate4, pbCAPKExponent4, inCAPKindex5, pbExpireDate5, pbCAPKExponent5, inCAPKindex6, pbExpireDate6, pbCAPKExponent6, inCAPKindex7, pbExpireDate7, pbCAPKExponent7, inCAPKindex8, pbExpireDate8, pbCAPKExponent8, inCAPKindex9, pbExpireDate9, pbCAPKExponent9, inCAPKindex10, pbExpireDate10, pbCAPKExponent10, pbTerminalAVN, pb2ndTerminalAVN from AID where AIDid = ? AND fAIDEnable = ?";

	/* open the database */
	result = sqlite3_open(DB_WAVE,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	
	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		
		//vdDebug_LogPrintf("strAIDT.result[%d]AIDid[%d]",result,inSeekCnt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
			
            /*ulRef*/
			strWaveAIDT.ulRef = sqlite3_column_double(stmt, inStmtSeq);

            /* fAIDEnable */
			strWaveAIDT.fAIDEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*inAIDLen*/
			strWaveAIDT.inAIDLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			//Debug_LogPrintf("strAIDT.inAIDLen[%d]",strAIDT.inAIDLen);

			/* pbAID */
			memcpy(strWaveAIDT.pbAID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), strWaveAIDT.inAIDLen);
			vdDebug_LogPrintf("strAIDT.inAIDLen[%d]",strWaveAIDT.inAIDLen);
            DebugAddHEX("AID =",strWaveAIDT.pbAID,strWaveAIDT.inAIDLen);
                 
            /*inApplication_Selection_Indicator*/
			strWaveAIDT.inApplication_Selection_Indicator = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*EMVid*/
			strWaveAIDT.EMVid = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*inCAPKindex1*/
			strWaveAIDT.inCAPKindex1 = sqlite3_column_int(stmt,inStmtSeq +=1 );
            /* pbExpireDate1*/
			strcpy((char*)strWaveAIDT.pbExpireDate1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent1*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			/*inCAPKindex2*/
			strWaveAIDT.inCAPKindex2 = sqlite3_column_int(stmt,inStmtSeq +=1 );
            /* pbExpireDate2*/
			strcpy((char*)strWaveAIDT.pbExpireDate2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent2*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex3*/
			strWaveAIDT.inCAPKindex3 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate3*/
			strcpy((char*)strWaveAIDT.pbExpireDate3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent3*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex4*/
			strWaveAIDT.inCAPKindex4 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate4*/
			strcpy((char*)strWaveAIDT.pbExpireDate4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent4*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		
			/*inCAPKindex5*/
			strWaveAIDT.inCAPKindex5 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate5*/
			strcpy((char*)strWaveAIDT.pbExpireDate5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent5*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex6*/
			strWaveAIDT.inCAPKindex6 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate6*/
			strcpy((char*)strWaveAIDT.pbExpireDate6, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent6*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent6, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

					
			/*inCAPKindex7*/
			strWaveAIDT.inCAPKindex7 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate7*/
			strcpy((char*)strWaveAIDT.pbExpireDate7, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent7*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent7, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex8*/
			strWaveAIDT.inCAPKindex8 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate8*/
			strcpy((char*)strWaveAIDT.pbExpireDate8, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent8*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent8, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex9*/
			strWaveAIDT.inCAPKindex9 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate9*/
			strcpy((char*)strWaveAIDT.pbExpireDate9, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent9*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent9, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inCAPKindex10*/
			strWaveAIDT.inCAPKindex4 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			/* pbExpireDate10*/
			strcpy((char*)strWaveAIDT.pbExpireDate10, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* pbCAPKExponent10*/
			strcpy((char*)strWaveAIDT.pbCAPKExponent10, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* pbTerminalAVN */
			memcpy(strWaveAIDT.pbTerminalAVN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);
			/* pb2ndTerminalAVN */
			memcpy(strWaveAIDT.pb2ndTerminalAVN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);


		}
	} while (result == SQLITE_ROW);	   

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	//�ر�sqlite����
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


int inWaveEMVRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
 	char *sql = "SELECT szASI, szSchemeID,szActivate,szTerminalType,szEMVTermCapabilities,szEMVTermAddCapabilities,szAppVerNo,szEMVTermCountryCode,szEMVTermCurrencyCode,szTransactionType,lnEMVFloorLimit,szVisaWaveVLPIndicator,szVisaWaveTTQ,szDefaultTDOL,szDefaultDDOL,inEMVTargetRSPercent,inEMVMaxTargetRSPercent,lnEMVRSThreshold,szEMVTACDefault,szEMVTACDenial,szEMVTACOnline,szUploadDOL,szAEPara,szTransactionResult,szVisMSDCVN17Enable,szTerminalIdentification,szIFDSerialNumber,szMerchantIdentifier,inVisaMSDTrack1Enable,inVisaMSDTrack2Enable,inDisplayOfflineIndicator,szStatusCheck,szAmount0Check,szCLTranslimit,szCLCVMlimit,szCLFloorlimit, szCVN17UploadDOL, szAdditionalDataUploadDOL FROM EMV WHERE EMVid = ?";

	/* open the database */
	result = sqlite3_open(DB_WAVE,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		
		//Debug_LogPrintf("EMV read[%d]",result);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
	
			/* szASI */
		   strcpy((char*)strWaveEMVT.szASI, (char *)sqlite3_column_text(stmt,inStmtSeq));
//			vdDebug_LogPrintf("szASI=[%s]",strWaveEMVT.szASI);
	   
		    /* szSchemeID */
		   strcpy((char*)strWaveEMVT.szSchemeID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
//			vdDebug_LogPrintf("szSchemeID=[%s]",strWaveEMVT.szSchemeID);
            /*szActivate;*/
			strWaveEMVT.szActivate = sqlite3_column_int(stmt,inStmtSeq +=1 );
//			vdDebug_LogPrintf("szActivate=[%d]",strWaveEMVT.szActivate);
			/* szSchemeID */
			strcpy((char*)strWaveEMVT.szTerminalType, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
//			vdDebug_LogPrintf("szTerminalType=[%s]",strWaveEMVT.szTerminalType);
			 /* szEMVTermCapabilities*/
			memcpy(strWaveEMVT.szEMVTermCapabilities, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
            DebugAddHEX("szEMVTermCapabilities",strWaveEMVT.szEMVTermCapabilities,4);  
            /* szEMVTermAddCapabilities */
			memcpy(strWaveEMVT.szEMVTermAddCapabilities, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);
    		DebugAddHEX("szEMVTermAddCapabilities",strWaveEMVT.szEMVTermAddCapabilities,6); 
			/* szAppVerNo */
		    memcpy((char*)strWaveEMVT.szAppVerNo, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),3);
			DebugAddHEX("szAppVerNo",strWaveEMVT.szAppVerNo,3);
			/* szEMVTermCountryCode */
		   memcpy(strWaveEMVT.szEMVTermCountryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
		   DebugAddHEX("szEMVTermCountryCode",strWaveEMVT.szEMVTermCountryCode,3); 
		   /* szEMVTermCurrencyCode */		  //20
		   memcpy(strWaveEMVT.szEMVTermCurrencyCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			DebugAddHEX("szEMVTermCurrencyCode",strWaveEMVT.szEMVTermCurrencyCode,3); 
		   /* szAppVerNo */
		   memcpy((char*)strWaveEMVT.szTransactionType, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("szTransactionType=",strWaveEMVT.szTransactionType,2);
            /*lnEMVFloorLimit*/
			memcpy((char*)strWaveEMVT.lnEMVFloorLimit, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),5);
			DebugAddHEX("lnEMVFloorLimit=",strWaveEMVT.lnEMVFloorLimit,5);
			/* szVisaWaveVLPIndicator */
			memcpy((char*)strWaveEMVT.szVisaWaveVLPIndicator, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("szVisaWaveVLPIndicator=",strWaveEMVT.szVisaWaveVLPIndicator,2);
			/* szVisaWaveTTQ */
			memcpy((char*)strWaveEMVT.szVisaWaveTTQ, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),5);
			DebugAddHEX("szVisaWaveTTQ=",strWaveEMVT.szVisaWaveTTQ,5);
            /* szDefaultTDOL */
			memcpy((char*)strWaveEMVT.szDefaultTDOL, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),4);
           DebugAddHEX("szDefaultTDOL=",strWaveEMVT.szDefaultTDOL,4);
            /* szDefaultDDOL */
			memcpy((char*)strWaveEMVT.szDefaultDDOL, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),4);
			DebugAddHEX("szDefaultDDOL=",strWaveEMVT.szDefaultDDOL,4);
            /*inEMVTargetRSPercent*/
			memcpy((char*)strWaveEMVT.inEMVTargetRSPercent, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
           DebugAddHEX("inEMVTargetRSPercent=",strWaveEMVT.inEMVTargetRSPercent,2);    
            /*inEMVMaxTargetRSPercent*/
			memcpy((char*)strWaveEMVT.inEMVMaxTargetRSPercent, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("inEMVMaxTargetRSPercent=",strWaveEMVT.inEMVMaxTargetRSPercent,2);    
			/*lnEMVRSThreshold*/			//5
			memcpy((char*)strWaveEMVT.lnEMVRSThreshold, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),5);
			DebugAddHEX("lnEMVRSThreshold=",strWaveEMVT.lnEMVRSThreshold,5);
			/* szEMVTACDefault */			 //10
		   memcpy(strWaveEMVT.szEMVTACDefault, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
		  DebugAddHEX("szEMVTACDefault",strWaveEMVT.szEMVTACDefault,5); 
		   /* szEMVTACDenial */ 		   
		   memcpy(strWaveEMVT.szEMVTACDenial, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
		   DebugAddHEX("szEMVTACDenial",strWaveEMVT.szEMVTACDenial,5); 
		   /* szEMVTACOnline */
		   memcpy(strWaveEMVT.szEMVTACOnline, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
  			DebugAddHEX("szEMVTACOnline",strWaveEMVT.szEMVTACOnline,5);
		   /* szUploadDOL */
		   memcpy((char*)strWaveEMVT.szUploadDOL, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),16);
			DebugAddHEX("szUploadDOL",strWaveEMVT.szUploadDOL,16);
			/* szAEPara */
			memcpy((char*)strWaveEMVT.szAEPara, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),7);
			DebugAddHEX("szAEPara",strWaveEMVT.szAEPara,7);
			/* szTransactionResult */
		   memcpy((char*)strWaveEMVT.szTransactionResult, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("szTransactionResult=",strWaveEMVT.szTransactionResult,2);
			/* szVisMSDCVN17Enable */
			memcpy((char*)strWaveEMVT.szVisMSDCVN17Enable, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("szVisMSDCVN17Enable=",strWaveEMVT.szVisMSDCVN17Enable,2);
			/* szTerminalIdentification */
		   memcpy((char*)strWaveEMVT.szTerminalIdentification, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),9);
			DebugAddHEX("szTerminalIdentification=",strWaveEMVT.szTerminalIdentification,9);
			/* szIFDSerialNumber */
			memcpy((char*)strWaveEMVT.szIFDSerialNumber, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),9);
			DebugAddHEX("szIFDSerialNumber=",strWaveEMVT.szIFDSerialNumber,9);
			/* szMerchantIdentifier */
			memcpy((char*)strWaveEMVT.szMerchantIdentifier, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),16);
			DebugAddHEX("szMerchantIdentifier",strWaveEMVT.szMerchantIdentifier,16);
			/*inVisaMSDTrack1Enable*/
			memcpy((char*)strWaveEMVT.inVisaMSDTrack1Enable, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
           DebugAddHEX("inVisaMSDTrack1Enable=",strWaveEMVT.inVisaMSDTrack1Enable,2);    
            /*inEMVMaxTargetRSPercent*/
 			memcpy((char*)strWaveEMVT.inVisaMSDTrack2Enable, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("inVisaMSDTrack2Enable=",strWaveEMVT.inVisaMSDTrack2Enable,2);
			/*inDisplayOfflineIndicator*/
 			memcpy((char*)strWaveEMVT.inDisplayOfflineIndicator, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("inDisplayOfflineIndicator=",strWaveEMVT.inDisplayOfflineIndicator,2);

			 /* szStatusCheck */
		   memcpy((char*)strWaveEMVT.szStatusCheck, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("szStatusCheck=",strWaveEMVT.szStatusCheck,2);
			/* szAmount0Check */
			memcpy((char*)strWaveEMVT.szAmount0Check, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),2);
			DebugAddHEX("szAmount0Check=",strWaveEMVT.szAmount0Check,2);
			/* szCLTranslimit */
		   memcpy((char*)strWaveEMVT.szCLTranslimit, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),7);
			DebugAddHEX("szCLTranslimit",strWaveEMVT.szCLTranslimit,7);
			/* szCLCVMlimit */
			memcpy((char*)strWaveEMVT.szCLCVMlimit, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),7);
			DebugAddHEX("szCLCVMlimit=",strWaveEMVT.szCLCVMlimit,7);
			/* szCLFloorlimit */
		   memcpy((char*)strWaveEMVT.szCLFloorlimit, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),7);
			DebugAddHEX("szCLFloorlimit=",strWaveEMVT.szCLFloorlimit,7);
			/* szCVN17UploadDOL */
			memcpy((char*)strWaveEMVT.szCVN17UploadDOL, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),70);
			DebugAddHEX("szCVN17UploadDOL=",strWaveEMVT.szCVN17UploadDOL,70);
			/* szAdditionalDataUploadDOL */
			memcpy((char*)strWaveEMVT.szAdditionalDataUploadDOL, (char *)sqlite3_column_blob(stmt,inStmtSeq +=1 ),7);
			DebugAddHEX("szAdditionalDataUploadDOL=",strWaveEMVT.szAdditionalDataUploadDOL,7);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	//�ر�sqlite����
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


int inDatabase_BatchDeleteHDTidMITid(void)
{
	int result;
	char *sql = "DELETE FROM TransData WHERE HDTid = ? AND MITid = ?";	
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].HDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].MITid);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delete,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_BatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM TransData WHERE (szHostLabel = ? AND szBatchNo = ? AND MITid = ?)";	
	int inDBResult = 0;
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)srTransRec.szHostLabel, strlen((char*)srTransRec.szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, srTransRec.szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, srTransRec.MITid);


	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch dele,sqlite3_exec[%d]MITid[%d]",inDBResult,srTransRec.MITid);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_BatchRead(TRANS_DATA_TABLE *transData, int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	//char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szTerms, szFactorRate, szMoAmort, szTotalInstAmnt, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, szLoyaltyData, fLoyalty, szBancnetBatchNo, szBancnetTraceNo FROM TransData WHERE ulSavedIndex = ?";
	char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szTerms, szFactorRate, szMoAmort, szTotalInstAmnt, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, szLoyaltyData, fLoyalty, szBancnetBatchNo, szBancnetTraceNo, szBank_Mnemonic, inAccountType, szECRMessage1, szECRMessage2, szECRMessage3, szECRMessage4, szTranYear, szServiceFee, szBillerName, szBillerCode, szSubscriberNo, szAccountNoTo, szUserName, szAcquirerFee, szBillerFee FROM TransData WHERE ulSavedIndex = ?";
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->MITid = sqlite3_column_int(stmt,inStmtSeq);
			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
			strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            /*installment data*/
            strcpy((char*)transData->szTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)transData->szFactorRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)transData->szMoAmort, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)transData->szTotalInstAmnt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)transData->szPromo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)transData->szInstallmentData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            transData->fInstallment= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
			/*loyalty data*/
			strcpy((char*)transData->szProductCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szCustomerNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szLoyaltyData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            transData->fLoyalty= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));

            /*Coreware*/ 						
            strcpy((char*)transData->szBancnetBatchNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szBancnetTraceNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szBank_Mnemonic, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			transData->inAccountType = sqlite3_column_int(stmt,inStmtSeq +=1 );

			// Mercury Requirements -- sidumili
			strcpy((char*)transData->szECRMessage1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			// szTranYear
			strcpy((char*)transData->szTranYear, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szServiceFee*/
			strcpy((char*)transData->szServiceFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            //szServiceFee, szBillerName, szBillerCode, szSubscriberNo, szAccountNoTo, szUserName, szAcquirerFee, szServiceFee2
			strcpy((char*)transData->szBillerName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szBillerCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szSubscriberNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szAccountNoTo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szUserName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szAcquirerFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szBillerFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));			 
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	return(inResult);
}

int inDatabase_BatchReadByHostidAndMITid(TRANS_DATA_TABLE *transData,int inHDTid,int inMITid)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo FROM MMT WHERE HDTid = ? AND MITid = ?";

	int incount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	
	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMITid);
	
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {		
		result = sqlite3_step(stmt);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->MITid = sqlite3_column_int(stmt,inStmtSeq);
			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);			
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);		
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
			strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            /*installment data*/
            strcpy((char*)transData->szTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)transData->szFactorRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
            strcpy((char*)transData->szMoAmort, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szTotalInstAmnt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szPromo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szInstallmentData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			transData->fInstallment= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 )); 
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
    
    return(inResult);
}


//szServiceFee, szBillerName, szBillerCode, szSubscriberNo, szAccountNoTo, szUserName, szAcquirerFee, szServiceFee2
int inDatabase_BatchInsert(TRANS_DATA_TABLE *transData)
{
	int result;	
	char *sql1 = "SELECT MAX(TransDataid) FROM TransData";
 	//char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szTerms, szFactorRate, szMoAmort, szTotalInstAmnt, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, szLoyaltyData, fLoyalty, szBancnetBatchNo, szBancnetTraceNo) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";	
 	//char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szTerms, szFactorRate, szMoAmort, szTotalInstAmnt, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, szLoyaltyData, fLoyalty, szBancnetBatchNo, szBancnetTraceNo, szBank_Mnemonic, inAccountType, szECRMessage1, szECRMessage2, szECRMessage3, szECRMessage4, szTranYear, szServiceFee, szBillerName, szBillerCode, szSubscriberNo, szAccountNoTo, szUserName, szAcquirerFee, szBillerFee) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szFactorRate, szMoAmort, szTotalInstAmnt, szTerms, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, fLoyalty, szLoyaltyData, szBancnetBatchNo, szBancnetTraceNo, szBank_Mnemonic, inAccountType, szECRMessage1, szECRMessage2, szECRMessage3, szECRMessage4, szTranYear, szServiceFee, szBillerName, szBillerCode, szSubscriberNo, szAccountNoTo, szUserName, szAcquirerFee, szBillerFee) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"; 	


	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);

		vdDebug_LogPrintf("inDatabase_BatchInsert-1");		
		
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		vdDebug_LogPrintf("inDatabase_BatchInsert-2");		
		
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			transData->ulSavedIndex = sqlite3_column_int(stmt,inStmtSeq);
			transData->ulSavedIndex += 1;
		}
	} while (result == SQLITE_ROW);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	
	vdDebug_LogPrintf("inDatabase_BatchInsert-3");		

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		vdDebug_LogPrintf("inDatabase_BatchInsert-4");		
		
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		vdDebug_LogPrintf("inDatabase_BatchInsert-5");		
		
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC); 
    //result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
    //result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
    
	/*installment data*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szFactorRate, strlen((char*)transData->szFactorRate), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMoAmort, strlen((char*)transData->szMoAmort), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTotalInstAmnt, strlen((char*)transData->szTotalInstAmnt), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTerms, strlen((char*)transData->szTerms), SQLITE_STATIC); 	
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szPromo, strlen((char*)transData->szPromo), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szInstallmentData, strlen((char*)transData->szInstallmentData), SQLITE_STATIC); 
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fInstallment);

    /*loyalty data*/ 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szProductCode, strlen((char*)transData->szProductCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szCustomerNo, strlen((char*)transData->szCustomerNo), SQLITE_STATIC); 
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fLoyalty);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szLoyaltyData, strlen((char*)transData->szLoyaltyData), SQLITE_STATIC); 

    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBancnetBatchNo, strlen((char*)transData->szBancnetBatchNo), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBancnetTraceNo, strlen((char*)transData->szBancnetTraceNo), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBank_Mnemonic, strlen((char*)transData->szBank_Mnemonic), SQLITE_STATIC); 
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inAccountType);
	
	// Mercury Requirements -- sidumili
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMessage1, strlen((char*)transData->szECRMessage1), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMessage2, strlen((char*)transData->szECRMessage2), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMessage3, strlen((char*)transData->szECRMessage3), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMessage4, strlen((char*)transData->szECRMessage4), SQLITE_STATIC);

	// szTranYear
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTranYear, strlen((char*)transData->szTranYear), SQLITE_STATIC);

	//szServiceFee, szBillerName, szBillerCode, szSubscriberNo, szAccountNoTo, szUserName, szAcquirerFee, szBillerFee
	
	/*szServiceFee*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szServiceFee, strlen((char*)transData->szServiceFee), SQLITE_STATIC); 
	
	/*szBillerName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBillerName, strlen((char*)transData->szBillerName), SQLITE_STATIC); 

	/*szBillerCode*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBillerCode, strlen((char*)transData->szBillerCode), SQLITE_STATIC); 

	/*szSubscriberNo*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szSubscriberNo, strlen((char*)transData->szSubscriberNo), SQLITE_STATIC); 

	/*szAccountNoTo*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szAccountNoTo, strlen((char*)transData->szAccountNoTo), SQLITE_STATIC); 

	/*szUserName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szUserName, strlen((char*)transData->szUserName), SQLITE_STATIC); 

	/*szAcquirerFee*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szAcquirerFee, strlen((char*)transData->szAcquirerFee), SQLITE_STATIC); 

	/*szBillerFee*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBillerFee, strlen((char*)transData->szBillerFee), SQLITE_STATIC);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}


	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	vdDebug_LogPrintf("inDatabase_BatchInsert >>> SUCCESSFUL!!!!");
	
	return(ST_SUCCESS);
}

int inDatabase_BatchUpdate(TRANS_DATA_TABLE *transData)
{
	int result;
	//char *sql = "UPDATE TransData SET HDTid = ?, MITid = ?, CDTid = ?, IITid = ?, szHostLabel = ?, szBatchNo = ?, byTransType = ?, byPanLen = ?, szExpireDate = ?, byEntryMode = ?, szTotalAmount = ?, szBaseAmount = ?, szTipAmount = ?, byOrgTransType = ?, szMacBlock = ?, szYear = ?, szDate = ?, szTime = ?, szOrgDate = ?, szOrgTime = ?, szAuthCode = ?, szRRN = ?, szInvoiceNo = ?, szOrgInvoiceNo = ?, byPrintType = ?, byVoided = ?, byAdjusted = ?, byUploaded = ?, byTCuploaded = ?, szCardholderName = ?, szzAMEX4DBC = ?, szStoreID = ?, szRespCode = ?, szServiceCode = ?, byContinueTrans = ?, byOffline = ?, byReversal = ?, byEMVFallBack = ?, shTransResult = ?, szTpdu = ?, szIsoField03 = ?, szMassageType = ?, szPAN = ?, szCardLable = ?, usTrack1Len = ?, usTrack2Len = ?, usTrack3Len = ?, szTrack1Data = ?, szTrack2Data = ?, szTrack3Data = ?, usChipDataLen = ?, baChipData = ?, usAdditionalDataLen = ?, baAdditionalData = ?, bWaveSID = ?,usWaveSTransResult = ?,bWaveSCVMAnalysis = ?, ulTraceNum = ?, ulOrgTraceNum = ?, usTerminalCommunicationMode = ?, ulSavedIndex = ?, byPINEntryCapability = ?, byPackType = ?, szOrgAmount = ?, szCVV2 = ?, inCardType = ?, byTCFailUpCnt = ?, byCardTypeNum = ?, byEMVTransStatus = ?, T5A_len = ?, T5A = ?, T5F2A = ?, T5F30 = ?, T5F34 = ?, T5F34_len = ?, T82 = ?, T84_len = ?, T84 = ?, T8A = ?, T91 = ?, T91Len = ?, T95 = ?, T9A = ?, T9C = ?, T9F02 = ?, T9F03 = ?, T9F09 = ?, T9F10_len = ?, T9F10 = ?, T9F1A = ?, T9F26 = ?, T9F27 = ?, T9F33 = ?, T9F34 = ?, T9F35 = ?, T9F36_len = ?, T9F36 = ?, T9F37 = ?, T9F41 = ?, T9F53 = ?, ISR = ?, ISRLen = ?, T9B = ?, T5F24 = ?, T71Len = ?, T71 = ?, T72Len = ?, T72 = ?, T9F06 = ?, T9F1E = ?, T9F28 = ?, T9F29 = ?, szChipLabel = ?, szTID = ?, szMID = ?, szTerms = ?, szFactorRate = ?, szMoAmort = ?, szTotalInstAmnt = ?, szPromo = ?, szInstallmentData = ?, fInstallment = ?, szProductCode = ?, szCustomerNo = ?, szLoyaltyData = ?, fLoyalty = ?, szBancnetBatchNo = ?, szBancnetTraceNo = ? WHERE ulSavedIndex = ?";	
	char *sql = "UPDATE TransData SET HDTid = ?, MITid = ?, CDTid = ?, IITid = ?, szHostLabel = ?, szBatchNo = ?, byTransType = ?, byPanLen = ?, szExpireDate = ?, byEntryMode = ?, szTotalAmount = ?, szBaseAmount = ?, szTipAmount = ?, byOrgTransType = ?, szMacBlock = ?, szYear = ?, szDate = ?, szTime = ?, szOrgDate = ?, szOrgTime = ?, szAuthCode = ?, szRRN = ?, szInvoiceNo = ?, szOrgInvoiceNo = ?, byPrintType = ?, byVoided = ?, byAdjusted = ?, byUploaded = ?, byTCuploaded = ?, szCardholderName = ?, szzAMEX4DBC = ?, szStoreID = ?, szRespCode = ?, szServiceCode = ?, byContinueTrans = ?, byOffline = ?, byReversal = ?, byEMVFallBack = ?, shTransResult = ?, szTpdu = ?, szIsoField03 = ?, szMassageType = ?, szPAN = ?, szCardLable = ?, usTrack1Len = ?, usTrack2Len = ?, usTrack3Len = ?, szTrack1Data = ?, szTrack2Data = ?, szTrack3Data = ?, usChipDataLen = ?, baChipData = ?, usAdditionalDataLen = ?, baAdditionalData = ?, bWaveSID = ?,usWaveSTransResult = ?,bWaveSCVMAnalysis = ?, ulTraceNum = ?, ulOrgTraceNum = ?, usTerminalCommunicationMode = ?, ulSavedIndex = ?, byPINEntryCapability = ?, byPackType = ?, szOrgAmount = ?, szCVV2 = ?, inCardType = ?, byTCFailUpCnt = ?, byCardTypeNum = ?, byEMVTransStatus = ?, T5A_len = ?, T5A = ?, T5F2A = ?, T5F30 = ?, T5F34 = ?, T5F34_len = ?, T82 = ?, T84_len = ?, T84 = ?, T8A = ?, T91 = ?, T91Len = ?, T95 = ?, T9A = ?, T9C = ?, T9F02 = ?, T9F03 = ?, T9F09 = ?, T9F10_len = ?, T9F10 = ?, T9F1A = ?, T9F26 = ?, T9F27 = ?, T9F33 = ?, T9F34 = ?, T9F35 = ?, T9F36_len = ?, T9F36 = ?, T9F37 = ?, T9F41 = ?, T9F53 = ?, ISR = ?, ISRLen = ?, T9B = ?, T5F24 = ?, T71Len = ?, T71 = ?, T72Len = ?, T72 = ?, T9F06 = ?, T9F1E = ?, T9F28 = ?, T9F29 = ?, szChipLabel = ?, szTID = ?, szMID = ?, szTerms = ?, szFactorRate = ?, szMoAmort = ?, szTotalInstAmnt = ?, szPromo = ?, szInstallmentData = ?, fInstallment = ?, szProductCode = ?, szCustomerNo = ?, szLoyaltyData = ?, fLoyalty = ?, szBancnetBatchNo = ?, szBancnetTraceNo = ?, szBank_Mnemonic = ?, inAccountType = ?, szECRMessage1 = ?, szECRMessage2 = ?, szECRMessage3 = ?, szECRMessage4 = ?, szTranYear = ?, szServiceFee = ? WHERE ulSavedIndex = ?";
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC); 

	/*installment data*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTerms, strlen((char*)transData->szTerms), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szFactorRate, strlen((char*)transData->szFactorRate), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMoAmort, strlen((char*)transData->szMoAmort), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTotalInstAmnt, strlen((char*)transData->szTotalInstAmnt), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szPromo, strlen((char*)transData->szPromo), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szInstallmentData, strlen((char*)transData->szInstallmentData), SQLITE_STATIC); 
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fInstallment);

    /*loyalty data*/ 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szProductCode, strlen((char*)transData->szProductCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szCustomerNo, strlen((char*)transData->szCustomerNo), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szLoyaltyData, strlen((char*)transData->szLoyaltyData), SQLITE_STATIC); 
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fLoyalty);

    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBancnetBatchNo, strlen((char*)transData->szBancnetBatchNo), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBancnetTraceNo, strlen((char*)transData->szBancnetTraceNo), SQLITE_STATIC); 	
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, strCDT.szBank_Mnemonic, strlen((char*)transData->szBank_Mnemonic), SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inAccountType);
		
	// Mercury Requirements -- sidumili
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMessage1, strlen((char*)transData->szECRMessage1), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMessage2, strlen((char*)transData->szECRMessage2), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMessage3, strlen((char*)transData->szECRMessage3), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMessage4, strlen((char*)transData->szECRMessage4), SQLITE_STATIC); 

	// szTranYear
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTranYear, strlen((char*)transData->szTranYear), SQLITE_STATIC); 

	/*szServiceFee*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceFee, 7, SQLITE_STATIC);
	
    /*always put at the end*/ 
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->ulSavedIndex);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	return(ST_SUCCESS);
}

int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType)
{
	int inResult;
    TRANS_DATA_TABLE temptransData;


	vdDebug_LogPrintf("inDatabase_BatchSave. START");		
	vdDebug_LogPrintf("saturn inDatabase_BatchSave inStoredType= %d", inStoredType);
	

    if (inStoredType == DF_BATCH_APPEND)
    {
		inResult = inDatabase_BatchInsert(transData);

		vdDebug_LogPrintf("saturn inDatabase_BatchSave::inDatabase_BatchInsert::inResult[%d]", inResult);
		
		if (inResult != ST_SUCCESS)
		{
			return ST_ERROR;
		}
		memset(&temptransData, 0x00, sizeof(temptransData));
        memcpy(&temptransData, transData, sizeof(temptransData));
		inDatabase_InvoiceNumInsert(&temptransData);
    }
    else if(inStoredType == DF_BATCH_UPDATE)//default for tip or adjust txn
    {
		inResult = inDatabase_BatchUpdate(transData);

		vdDebug_LogPrintf("saturn inDatabase_BatchSave::inDatabase_BatchUpdate::inResult[%d]", inResult);
		
		if (inResult != ST_SUCCESS)
		{
			return ST_ERROR;
		}
    }
    else
    {
        return ST_ERROR;
    }

    strHDT.ulLastTransSavedIndex = transData->ulSavedIndex;

    
	vdDebug_LogPrintf("inDatabase_BatchSave. END");


    return ST_SUCCESS;
}

int inDatabase_BatchSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo)
{
	int result,i = 0;
	int inResult = d_NO;
	char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szTerms, szFactorRate, szMoAmort, szTotalInstAmnt, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, szLoyaltyData, fLoyalty, szBancnetBatchNo, szBancnetTraceNo, inAccountType, szECRMessage1, szECRMessage2, szECRMessage3, szECRMessage4, szTranYear, szServiceFee, szBillerName, szBillerCode, szSubscriberNo, szAccountNoTo, szUserName, szAcquirerFee, szBillerFee FROM TransData WHERE szInvoiceNo = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, hexInvoiceNo, 4, SQLITE_STATIC);

	DebugAddINT("inDatabase_BatchSearch sqlite3_bind_blob", result);
	DebugAddHEX("hexInvoiceNo", hexInvoiceNo, 3);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		i++;
		DebugAddINT("if(100)=Find", result);
		DebugAddINT("Loop Count", i);

		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq);
			transData->MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq+=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);						
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
            strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            /*installment data*/
            strcpy((char*)transData->szTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)transData->szFactorRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
            strcpy((char*)transData->szMoAmort, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            strcpy((char*)transData->szTotalInstAmnt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            strcpy((char*)transData->szPromo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 	
            strcpy((char*)transData->szInstallmentData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            transData->fInstallment= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
			/*loyalty data*/
			strcpy((char*)transData->szProductCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szCustomerNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szLoyaltyData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            transData->fLoyalty= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));			

            /*Coreware*/ 		 	 
            strcpy((char*)transData->szBancnetBatchNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szBancnetTraceNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			transData->inAccountType = sqlite3_column_int(stmt,inStmtSeq +=1 ); // sidumili: fix for last/any receipt printing of acnt type
			
			// Mercury Requirements -- sidumili
			strcpy((char*)transData->szECRMessage1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			// szTranYear
			strcpy((char*)transData->szTranYear, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szServiceFee*/
			strcpy((char*)transData->szServiceFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            //szServiceFee, szBillerName, szBillerCode, szSubscriberNo, szAccountNoTo, szUserName, szAcquirerFee, szServiceFee2
			strcpy((char*)transData->szBillerName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szBillerCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szSubscriberNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szAccountNoTo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szUserName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szAcquirerFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szBillerFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			vdDebug_LogPrintf("transData->szUserName:%s", transData->szUserName);
			vdDebug_LogPrintf("transData->szAcquirerFee:%s", transData->szAcquirerFee);
			vdDebug_LogPrintf("transData->szBillerFee:%s", transData->szBillerFee);
		}
	} while (result == SQLITE_ROW);

    DebugAddSTR("batch serPAN", transData->szPAN, 10);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	return(inResult);
}

/*bancnet*/
int inDatabase_BancnetBatchSearch(TRANS_DATA_TABLE *transData, char *szBancnetTraceNo)
{
	int result,i = 0;
	int inResult = d_NO;
	//char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szTerms, szFactorRate, szMoAmort, szTotalInstAmnt, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, szLoyaltyData, fLoyalty, szBancnetBatchNo, szBancnetTraceNo FROM TransData WHERE szBancnetTraceNo = ?";	
	char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szTerms, szFactorRate, szMoAmort, szTotalInstAmnt, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, szLoyaltyData, fLoyalty, szBancnetBatchNo, szBancnetTraceNo, szBank_Mnemonic, inAccountType, szECRMessage1, szECRMessage2, szECRMessage3, szECRMessage4, szTranYear, szServiceFee FROM TransData WHERE szBancnetTraceNo = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)szBancnetTraceNo, strlen((char*)szBancnetTraceNo), SQLITE_STATIC);

	vdDebug_LogPrintf("inDatabase_BancnetBatchSearch szBancnetTraceNo[%s]",(char*)szBancnetTraceNo);
	DebugAddINT("inDatabase_BatchSearch sqlite3_bind_blob", result);
	//DebugAddHEX("hexInvoiceNo", hexInvoiceNo, 3);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		i++;
		DebugAddINT("if(100)=Find", result);
		DebugAddINT("Loop Count", i);

		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq);
			transData->MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq+=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);						
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
            strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            /*installment data*/
            strcpy((char*)transData->szTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)transData->szFactorRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
            strcpy((char*)transData->szMoAmort, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            strcpy((char*)transData->szTotalInstAmnt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            strcpy((char*)transData->szPromo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 	
            strcpy((char*)transData->szInstallmentData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            transData->fInstallment= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
			/*loyalty data*/
			strcpy((char*)transData->szProductCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szCustomerNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szLoyaltyData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            transData->fLoyalty= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));			

            /*Coreware*/ 		 	 
            strcpy((char*)transData->szBancnetBatchNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szBancnetTraceNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szBank_Mnemonic, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			transData->inAccountType = sqlite3_column_int(stmt,inStmtSeq +=1 );

			// Mercury Requirements -- sidumili
			strcpy((char*)transData->szECRMessage1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szECRMessage4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			// szTranYear
			strcpy((char*)transData->szTranYear, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szServiceFee*/
			memcpy(transData->szServiceFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
			
		}
	} while (result == SQLITE_ROW);

	vdDebug_LogPrintf("AAA - BANCNETBATCHSEARCH[%s]",(char*)transData->szBancnetTraceNo);

    DebugAddSTR("batch serPAN", transData->szPAN, 10);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	vdDebug_LogPrintf("AAA - BANCNETBATCHSEARCH2 [%s]",(char*)transData->szBancnetTraceNo);
	
	return(inResult);
}

int inDatabase_BatchCheckDuplicateInvoice(char *hexInvoiceNo)
{
	int result;
	char *sql = "SELECT * FROM TransData WHERE szInvoiceNo = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, hexInvoiceNo, 4, SQLITE_STATIC);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			
			sqlite3_exec(db,"commit;",NULL,NULL,NULL);

			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return 0x0080;
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inMSGResponseCodeReadByHostName(char* szMsg, char *szMsg2, int inMsgIndex, char *szHostName)
{
	int result;
  	char *sql = "SELECT szMsg, szMsg2 FROM MSG WHERE usMsgIndex = ? AND szHostName = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMsgIndex);
    sqlite3_bind_text(stmt, inStmtSeq +=1, szHostName, strlen(szHostName), SQLITE_STATIC);
	
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */		
			inStmtSeq = 0;

			 /* szMsg */
			 strcpy((char*)szMsg, (char *)sqlite3_column_text(stmt,inStmtSeq ));
			 
			 /* szMsg */
			 strcpy((char*)szMsg2, (char *)sqlite3_column_text(stmt,inStmtSeq += 1));			 
   		}
	} while (result == SQLITE_ROW);	   

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}


int inMSGResponseCodeRead(char* szMsg, char *szMsg2, int inMsgIndex, int inHostIndex)
{
	int result;
  	char *sql = "SELECT szMsg, szMsg2 FROM MSG WHERE usMsgIndex = ? AND inHostIndex = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMsgIndex);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHostIndex);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */		
			inStmtSeq = 0;

			 /* szMsg */
			 strcpy((char*)szMsg, (char *)sqlite3_column_text(stmt,inStmtSeq ));
			 
			 /* szMsg */
			 strcpy((char*)szMsg2, (char *)sqlite3_column_text(stmt,inStmtSeq += 1));			 
   		}
	} while (result == SQLITE_ROW);	   

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

int inBatchNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM TransData WHERE HDTid = ? AND MITid = ? AND szBatchNo = ?";
	int inCount = 0;

	vdDebug_LogPrintf("--inBatchNumRecord--");
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].HDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].MITid);
	sqlite3_bind_blob(stmt, inStmtSeq +=1, strMMT[0].szBatchNo, 3, SQLITE_STATIC);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	vdDebug_LogPrintf("inCount[%d]",inCount);

	return(inCount);
}



int inBatchByMerchandHost(int inNumber, int inHostIndex, int inMerchIndex, char *szBatchNo, int *inTranID)
{
	int result;
  	char *sql = "SELECT TransDataid FROM TransData WHERE HDTid = ? AND MITid = ? AND szBatchNo = ?";
	int count = 0;

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHostIndex);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMerchIndex);
	sqlite3_bind_blob(stmt, inStmtSeq +=1, szBatchNo, 3, SQLITE_STATIC);

	inStmtSeq = 0;
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */		

			 inTranID[count] = sqlite3_column_int(stmt,inStmtSeq);
             DebugAddINT("inTranID", inTranID[count]);
			 count++;

   		}
	} while (result == SQLITE_ROW);	   

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

int inDatabase_BatchReadByTransId(TRANS_DATA_TABLE *transData, int inTransDataid)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szTerms, szFactorRate, szMoAmort, szTotalInstAmnt, szPromo, szInstallmentData, fInstallment, szProductCode, szCustomerNo, szLoyaltyData, fLoyalty, szTranYear, inAccountType, szUserName FROM TransData WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inTransDataid);

	inStmtSeq = 0;
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			transData->MITid = sqlite3_column_int(stmt,inStmtSeq);
			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);			
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
			strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            /*installment data*/
            strcpy((char*)transData->szTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
            strcpy((char*)transData->szFactorRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
            strcpy((char*)transData->szMoAmort, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            strcpy((char*)transData->szTotalInstAmnt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            strcpy((char*)transData->szPromo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
            strcpy((char*)transData->szInstallmentData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 			
            transData->fInstallment= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 )); 
			/*loyalty data*/
			strcpy((char*)transData->szProductCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szCustomerNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szLoyaltyData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            transData->fLoyalty= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));

			//szTranYear
			strcpy((char*)transData->szTranYear, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szServiceFee*/
			//memcpy(transData->szServiceFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);

			/*inAccountType*/
			transData->inAccountType = sqlite3_column_int(stmt,inStmtSeq +=1 );

			
			/*szServiceFee*/
			//memcpy(transData->szServiceFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);

			strcpy((char*)transData->szUserName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

int inMultiAP_Database_BatchRead(TRANS_DATA_TABLE *transData)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szPromo, fInstallment, fLoyalty, fDebit, fCredit, fECRTxnFlg, fAutoSettleFlag, szECRPANFormatted, szECRRespText, szECRMerchantName, szECRRespCode, fBillsPaymentCash FROM TransData WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->MITid = sqlite3_column_int(stmt,inStmtSeq);
			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);	
                        DebugAddHEX("szTrack2Data", transData->szTrack2Data, 42);	
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
			strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            strcpy((char*)transData->szPromo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            transData->fInstallment= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
            transData->fLoyalty= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
            transData->fDebit= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
            transData->fCredit= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
            transData->fECRTxnFlg= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
            transData->fAutoSettleFlag= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));


			strcpy((char*)transData->szECRPANFormatted, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szECRRespText, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szECRMerchantName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szECRRespCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			transData->fBillsPaymentCash= fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

int inMultiAP_Database_BatchUpdate(TRANS_DATA_TABLE *transData)
{
	int result;
	char *sql = "UPDATE TransData SET HDTid = ?, MITid = ?, CDTid = ?, IITid = ?, szHostLabel = ?, szBatchNo = ?, byTransType = ?, byPanLen = ?, szExpireDate = ?, byEntryMode = ?, szTotalAmount = ?, szBaseAmount = ?, szTipAmount = ?, byOrgTransType = ?, szMacBlock = ?, szYear = ?, szDate = ?, szTime = ?, szOrgDate = ?, szOrgTime = ?, szAuthCode = ?, szRRN = ?, szInvoiceNo = ?, szOrgInvoiceNo = ?, byPrintType = ?, byVoided = ?, byAdjusted = ?, byUploaded = ?, byTCuploaded = ?, szCardholderName = ?, szzAMEX4DBC = ?, szStoreID = ?, szRespCode = ?, szServiceCode = ?, byContinueTrans = ?, byOffline = ?, byReversal = ?, byEMVFallBack = ?, shTransResult = ?, szTpdu = ?, szIsoField03 = ?, szMassageType = ?, szPAN = ?, szCardLable = ?, usTrack1Len = ?, usTrack2Len = ?, usTrack3Len = ?, szTrack1Data = ?, szTrack2Data = ?, szTrack3Data = ?, usChipDataLen = ?, baChipData = ?, usAdditionalDataLen = ?, baAdditionalData = ?, bWaveSID = ?,usWaveSTransResult = ?,bWaveSCVMAnalysis = ?, ulTraceNum = ?, ulOrgTraceNum = ?, usTerminalCommunicationMode = ?, ulSavedIndex = ?, byPINEntryCapability = ?, byPackType = ?, szOrgAmount = ?, szCVV2 = ?, inCardType = ?, byTCFailUpCnt = ?, byCardTypeNum = ?, byEMVTransStatus = ?, T5A_len = ?, T5A = ?, T5F2A = ?, T5F30 = ?, T5F34 = ?, T5F34_len = ?, T82 = ?, T84_len = ?, T84 = ?, T8A = ?, T91 = ?, T91Len = ?, T95 = ?, T9A = ?, T9C = ?, T9F02 = ?, T9F03 = ?, T9F09 = ?, T9F10_len = ?, T9F10 = ?, T9F1A = ?, T9F26 = ?, T9F27 = ?, T9F33 = ?, T9F34 = ?, T9F35 = ?, T9F36_len = ?, T9F36 = ?, T9F37 = ?, T9F41 = ?, T9F53 = ?, ISR = ?, ISRLen = ?, T9B = ?, T5F24 = ?, T71Len = ?, T71 = ?, T72Len = ?, T72 = ?, T9F06 = ?, T9F1E = ?, T9F28 = ?, T9F29 = ?, szChipLabel = ?, szTID = ?, szMID = ?, szPromo = ?, fInstallment = ?, fLoyalty = ?, fDebit = ?, fCredit = ?, fECRTxnFlg = ?, fAutoSettleFlag = ?, szECRPANFormatted=?, szECRRespText=?, szECRMerchantName=?  szECRRespCode=? WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szPromo, strlen((char*)transData->szPromo), SQLITE_STATIC); 

    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fInstallment);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fLoyalty);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fDebit);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fCredit);

    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fECRTxnFlg);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fAutoSettleFlag);

	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRPANFormatted, strlen((char*)transData->szECRPANFormatted), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRRespText, strlen((char*)transData->szECRRespText), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMerchantName, strlen((char*)transData->szECRMerchantName), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRRespCode, strlen((char*)transData->szECRRespCode), SQLITE_STATIC); 
	

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}



int inMultiAP_Database_BatchInsert(TRANS_DATA_TABLE *transData)
{
	int result;	
	char *sql1 = "SELECT MAX(TransDataid) FROM TransData";
 	char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, szPromo, fInstallment, fLoyalty, fDebit, fCredit, szResponseText, szMerchantName, fECRTxnFlg, fAutoSettleFlag, szECRPANFormatted, szECRRespText, szECRMerchantName, szECRRespCode) VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			transData->ulSavedIndex = sqlite3_column_int(stmt,inStmtSeq);
			transData->ulSavedIndex += 1;
		}
	} while (result == SQLITE_ROW);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	   
	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	//vdCTOSS_GetMemoryStatus("bagin Database");

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szPromo, strlen((char*)transData->szPromo), SQLITE_STATIC); 

    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fInstallment);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fLoyalty);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fDebit);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fCredit);

    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szResponseText, strlen((char*)transData->szResponseText), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMerchantName, strlen((char*)transData->szMerchantName), SQLITE_STATIC); 

    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fECRTxnFlg);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fAutoSettleFlag);

	
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRPANFormatted, strlen((char*)transData->szECRPANFormatted), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRRespText, strlen((char*)transData->szECRRespText), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRMerchantName, strlen((char*)transData->szECRMerchantName), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szECRRespCode, strlen((char*)transData->szECRRespCode), SQLITE_STATIC);
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}



int inMultiAP_Database_BatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM TransData";	
	int inDBResult =0 ;

    remove(DB_MULTIAP_JOURNAL);
	
	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

//	inStmtSeq = 0;
//	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)srTransRec.szHostLabel, strlen((char*)srTransRec.szHostLabel), SQLITE_STATIC); 
//	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, srTransRec.szBatchNo, 3, SQLITE_STATIC);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}


int inMultiAP_Database_EMVTransferDataInit(void)
{
    int inSeekCnt = 1;
    USHORT usDataLen = 0;
    BYTE bEMVData[4];
    int result;
    char *sql = "UPDATE ETD SET inEMVDataLen = ? ,szEMVTransferData = ? WHERE  ETDid = ?";          
   
    /* open the database */
    result = sqlite3_open(DB_EMV,&db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
	sqlite3_exec( db, "begin", 0, 0, NULL );
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

    memset(bEMVData, 0x00, sizeof(bEMVData));
    inStmtSeq = 0;

    /* inEMVDataLen */
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, usDataLen);
    
    /* szEMVTransferData */    
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, bEMVData, 1, SQLITE_STATIC);


    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    result = sqlite3_step(stmt);
    if( result != SQLITE_DONE ){
        sqlite3_close(db);
        return 3;
    }
    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return(d_OK);
}



int inMultiAP_Database_EMVTransferDataWrite(USHORT usDataLen, BYTE *bEMVData)
{
    int inSeekCnt = 1;
    int result;
    char *sql = "UPDATE ETD SET inEMVDataLen = ? ,szEMVTransferData = ? WHERE  ETDid = ?";          
  
    /* open the database */
    result = sqlite3_open(DB_EMV,&db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
	sqlite3_exec( db, "begin", 0, 0, NULL );
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

    inStmtSeq = 0;

    /* inEMVDataLen */
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, usDataLen);
    
    /* szEMVTransferData */    
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, bEMVData, usDataLen, SQLITE_STATIC);


    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    result = sqlite3_step(stmt);
    if( result != SQLITE_DONE ){
        sqlite3_close(db);
        return 3;
    }
    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return(d_OK);
}


int inMultiAP_Database_EMVTransferDataRead(USHORT *usDataLen, BYTE *bEMVData)
{
    int inSeekCnt = 1;
    int result;
    int inResult = d_NO;
    char *sql = "SELECT inEMVDataLen, szEMVTransferData FROM ETD WHERE ETDid = ?";
        
    /* open the database */
    result = sqlite3_open(DB_EMV,&db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
	sqlite3_exec( db, "begin", 0, 0, NULL );
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

    sqlite3_bind_int(stmt, 1, inSeekCnt);

    /* loop reading each row until step returns anything other than SQLITE_ROW */
    do {
        result = sqlite3_step(stmt);
        if (result == SQLITE_ROW) { /* can read data */
            inResult = d_OK;
            inStmtSeq = 0;

            /*inEMVDataLen*/
            *usDataLen = sqlite3_column_int(stmt,inStmtSeq  );
            
            /*szEMVTransferData*/
            memcpy(bEMVData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), *usDataLen);

        }
    } while (result == SQLITE_ROW);

    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return(inResult);
}

//for Sharls_COM modem
#if 0
int inMultiAP_Database_COM_Read(void)
{
    int result;
    int inSeekCnt = 1;
    int len = 0;
    int inResult = -1;
    char *sql = "SELECT inCommMode, inHeaderFormat,bSSLFlag, szCAFileName, szClientFileName, szClientKeyFileName, szPriPhoneNum, szSecPhoneNum, szPriHostIP, ulPriHostPort, szSecHostIP, ulSecHostPort, bDialBackUp, bPredialFlag, inParaMode, inHandShake, inCountryCode, szATCMD1, szATCMD2, szATCMD3, szATCMD4, szATCMD5, inConnectionTimeOut, inReceiveTimeOut, inGPRSSingal, inSendLen, szSendData, inReceiveLen, szReceiveData FROM COM WHERE COMid = ?";

    /* open the database */
    result = sqlite3_open(DB_COM,&db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
    sqlite3_exec( db, "begin", 0, 0, NULL );
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

    sqlite3_bind_int(stmt, 1, inSeekCnt);

    /* loop reading each row until step returns anything other than SQLITE_ROW */
    do {
        result = sqlite3_step(stmt);
        if (result == SQLITE_ROW) { /* can read data */
            inResult = d_OK;
            inStmtSeq = 0;

           /* inCommMode */
           strCOM.inCommMode = sqlite3_column_int(stmt, inStmtSeq );

           
           /* inCommMode */
           strCOM.inHeaderFormat = sqlite3_column_int(stmt, inStmtSeq +=1 );

           /* fDialMode*/
           strCOM.bSSLFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));

           /* szCAFileName*/
			strcpy((char*)strCOM.szCAFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szClientFileName*/
             strcpy((char*)strCOM.szClientFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szClientKeyFileName*/
             strcpy((char*)strCOM.szClientKeyFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szPriPhoneNum*/
             strcpy((char*)strCOM.szPriPhoneNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szSecPhoneNum*/
             strcpy((char*)strCOM.szSecPhoneNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szPriHostIP*/
             strcpy((char*)strCOM.szPriHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szPriHostIP*/
			strCOM.ulPriHostPort = sqlite3_column_double(stmt,inStmtSeq +=1);

            /* szSecHostIP*/
             strcpy((char*)strCOM.szSecHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szPriHostIP*/
			strCOM.ulSecHostPort = sqlite3_column_double(stmt,inStmtSeq +=1);
            vdDebug_LogPrintf("**inMultiAP_Database_COM_Read inSendLen**");

			/* bDialBackUp*/
			strCOM.bDialBackUp = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
			/* bPredialFlag*/
			strCOM.bPredialFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
			/* inParaMode */
			strCOM.inParaMode = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inHandShake */
			strCOM.inHandShake = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inCountryCode */
			strCOM.inCountryCode = sqlite3_column_int(stmt, inStmtSeq +=1 );		
			/* szATCMD1*/
			strcpy((char*)strCOM.szATCMD1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szATCMD2*/
			strcpy((char*)strCOM.szATCMD2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szATCMD3*/
			strcpy((char*)strCOM.szATCMD3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szATCMD4*/
			strcpy((char*)strCOM.szATCMD4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szATCMD5*/
			strcpy((char*)strCOM.szATCMD5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* inConnectionTimeOut */
			strCOM.inConnectionTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inReceiveTimeOut */
			strCOM.inReceiveTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inGPRSSingal */
			strCOM.inGPRSSingal = sqlite3_column_int(stmt, inStmtSeq +=1 );

            /* inSendLen*/
            strCOM.inSendLen = sqlite3_column_int(stmt, inStmtSeq +=1 );
            vdDebug_LogPrintf("**inMultiAP_Database_COM_Read inSendLen[%d]**",strCOM.inSendLen);
            /*szSendData*/
			memcpy(strCOM.szSendData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), strCOM.inSendLen);
            vdDebug_LogPrintf("**inMultiAP_Database_COM_Read data inSendLen[%d]**",strCOM.inSendLen);

            /* inReceiveLen*/
            strCOM.inReceiveLen = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/*szReceiveData*/
			memcpy(strCOM.szReceiveData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), strCOM.inReceiveLen);

        }
    } while (result == SQLITE_ROW); 

    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);
    //�ر�sqlite����
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    vdDebug_LogPrintf((". *COM read"));

    return(inResult);
}


int inMultiAP_Database_COM_Save(void)
{
    int inSeekCnt = 1;
    int result;
    char *sql = "UPDATE COM SET inCommMode = ? ,inHeaderFormat = ? ,bSSLFlag = ? ,szCAFileName = ? ,szClientFileName = ? ,szClientKeyFileName = ? ,szPriPhoneNum = ? ,szSecPhoneNum = ? ,szPriHostIP = ? ,ulPriHostPort = ? ,szSecHostIP = ? ,ulSecHostPort = ? ,bDialBackUp = ? ,bPredialFlag = ? ,inParaMode = ? ,inHandShake = ? ,inCountryCode = ? ,szATCMD1 = ? ,szATCMD2 = ? ,szATCMD3 = ? ,szATCMD4 = ? ,szATCMD5 = ? ,inConnectionTimeOut = ? ,inReceiveTimeOut = ? ,inGPRSSingal = ? ,inSendLen = ? ,szSendData = ? ,inReceiveLen = ?,szReceiveData = ? WHERE  COMid = ?";          
 
    /* open the database */
    result = sqlite3_open(DB_COM,&db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
    vdDebug_LogPrintf("**inMultiAP_Database_COM_Save **");
    sqlite3_exec( db, "begin", 0, 0, NULL );
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

	inStmtSeq = 0;
        
    /* inCommMode*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inCommMode);
    /* inHeaderFormat*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inHeaderFormat);
    /* bSSLFlag*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bSSLFlag);
    /* szCAFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szCAFileName, strlen((char*)strCOM.szCAFileName), SQLITE_STATIC);
    /* szClientFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szClientFileName, strlen((char*)strCOM.szClientFileName), SQLITE_STATIC);
    /* szClientKeyFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szClientKeyFileName, strlen((char*)strCOM.szClientKeyFileName), SQLITE_STATIC);
    /* szPriPhoneNum*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szPriPhoneNum, strlen((char*)strCOM.szPriPhoneNum), SQLITE_STATIC);
    /* szSecPhoneNum*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szSecPhoneNum, strlen((char*)strCOM.szSecPhoneNum), SQLITE_STATIC);
    /* szPriHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szPriHostIP, strlen((char*)strCOM.szPriHostIP), SQLITE_STATIC);
    /* ulPriHostPort*/
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, strCOM.ulPriHostPort);
    /* szSecHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szSecHostIP, strlen((char*)strCOM.szSecHostIP), SQLITE_STATIC);
    /* ulSecHostPort*/
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, strCOM.ulSecHostPort);
	/* bDialBackUp*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bDialBackUp);
	/* bPredialFlag*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bPredialFlag);
	/* inParaMode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inParaMode);
	/* inHandShake*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inHandShake);
	/* inCountryCode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inCountryCode);	
    /* szATCMD1*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD1, strlen((char*)strCOM.szATCMD1), SQLITE_STATIC);	
    /* szATCMD2*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD2, strlen((char*)strCOM.szATCMD2), SQLITE_STATIC);
	/* szATCMD3*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD3, strlen((char*)strCOM.szATCMD3), SQLITE_STATIC);
	/* szATCMD4*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD4, strlen((char*)strCOM.szATCMD4), SQLITE_STATIC);
    /* szATCMD5*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD5, strlen((char*)strCOM.szATCMD5), SQLITE_STATIC);
	/* inConnectionTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inConnectionTimeOut);
	/* inReceiveTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inReceiveTimeOut);
	/* inGPRSSingal*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inGPRSSingal);


    /* inSendLen*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inSendLen);
	/* szSendData*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCOM.szSendData, strCOM.inSendLen, SQLITE_STATIC);

    /* inReceiveLen*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inReceiveLen);
	/* szReceiveData*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCOM.szReceiveData, strCOM.inReceiveLen, SQLITE_STATIC);


    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    result = sqlite3_step(stmt);
    if( result != SQLITE_DONE ){
        sqlite3_close(db);
        return 3;
    }
    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);
    //�ر�sqlite����
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return(d_OK);
}




int inMultiAP_Database_COM_Clear(void)
{
    int inSeekCnt = 1;
    int result;
    //char *sql = "UPDATE COM SET inCommMode = ? ,inHeaderFormat = ? ,bSSLFlag = ? ,szCAFileName = ? ,szClientFileName = ? ,szClientKeyFileName = ? ,szPriPhoneNum = ? ,szSecPhoneNum = ? ,szPriHostIP = ? ,ulPriHostPort = ? ,szSecHostIP = ? ,ulSecHostPort = ? ,inSendLen = ? ,szSendData = ? ,inReceiveLen = ?,szReceiveData = ? WHERE  COMid = ?";          
 	char *sql = "UPDATE COM SET inCommMode = ? ,inHeaderFormat = ? ,bSSLFlag = ? ,szCAFileName = ? ,szClientFileName = ? ,szClientKeyFileName = ? ,szPriPhoneNum = ? ,szSecPhoneNum = ? ,szPriHostIP = ? ,ulPriHostPort = ? ,szSecHostIP = ? ,ulSecHostPort = ? ,bDialBackUp = ? ,bPredialFlag = ? ,inParaMode = ? ,inHandShake = ? ,inCountryCode = ? ,szATCMD1 = ? ,szATCMD2 = ? ,szATCMD3 = ? ,szATCMD4 = ? ,szATCMD5 = ? ,inConnectionTimeOut = ? ,inReceiveTimeOut = ? ,inGPRSSingal = ? ,inSendLen = ? ,szSendData = ? ,inReceiveLen = ?,szReceiveData = ? WHERE  COMid = ?";		  

	/* open the database */
    result = sqlite3_open(DB_COM,&db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
    sqlite3_exec( db, "begin", 0, 0, NULL );
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

	inStmtSeq = 0;

    memset(&strCOM,0,sizeof(strCOM));
    
    /* inCommMode*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inCommMode);
    /* inHeaderFormat*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inHeaderFormat);
    /* bSSLFlag*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bSSLFlag);
    /* szCAFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szCAFileName, strlen((char*)strCOM.szCAFileName), SQLITE_STATIC);
    /* szClientFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szClientFileName, strlen((char*)strCOM.szClientFileName), SQLITE_STATIC);
    /* szClientKeyFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szClientKeyFileName, strlen((char*)strCOM.szClientKeyFileName), SQLITE_STATIC);
    /* szPriPhoneNum*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szPriPhoneNum, strlen((char*)strCOM.szPriPhoneNum), SQLITE_STATIC);
    /* szSecPhoneNum*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szSecPhoneNum, strlen((char*)strCOM.szSecPhoneNum), SQLITE_STATIC);
    /* szPriHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szPriHostIP, strlen((char*)strCOM.szPriHostIP), SQLITE_STATIC);
    /* ulPriHostPort*/
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, strCOM.ulPriHostPort);
    /* szSecHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szSecHostIP, strlen((char*)strCOM.szSecHostIP), SQLITE_STATIC);
    /* ulSecHostPort*/
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, strCOM.ulSecHostPort);
	/* bDialBackUp*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bDialBackUp);
	/* bPredialFlag*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bPredialFlag);
	/* inParaMode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inParaMode);
	/* inHandShake*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inHandShake);
	/* inCountryCode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inCountryCode);	
    /* szATCMD1*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD1, strlen((char*)strCOM.szATCMD1), SQLITE_STATIC);	
    /* szATCMD2*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD2, strlen((char*)strCOM.szATCMD2), SQLITE_STATIC);
	/* szATCMD3*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD3, strlen((char*)strCOM.szATCMD3), SQLITE_STATIC);
	/* szATCMD4*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD4, strlen((char*)strCOM.szATCMD4), SQLITE_STATIC);
    /* szATCMD5*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD5, strlen((char*)strCOM.szATCMD5), SQLITE_STATIC);
	/* inConnectionTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inConnectionTimeOut);
	/* inReceiveTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inReceiveTimeOut);
	/* inGPRSSingal*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inGPRSSingal);

    /* inSendLen*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inSendLen);
	/* szSendData*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCOM.szSendData, strCOM.inSendLen, SQLITE_STATIC);

    /* inReceiveLen*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inReceiveLen);
	/* szReceiveData*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCOM.szReceiveData, strCOM.inReceiveLen, SQLITE_STATIC);


    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    result = sqlite3_step(stmt);
    if( result != SQLITE_DONE ){
        sqlite3_close(db);
        return 3;
    }
    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);
    //�ر�sqlite����
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    vdDebug_LogPrintf("---usCTOSS_COMM_SAVE");

    return(d_OK);
}
//end for Sharls_COM modem
#endif

int inERMAP_Database_BatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM TransData";	
	int inDBResult =0 ;

	/* open the database */
	result = sqlite3_open(DB_ERM,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inERMAP_Database_BatchRead(ERM_TransData *strERMTransData)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT shType, szTPDU, szPAN, szAmt, szSTAN, szTime, szDate, szExpDate, szRefNum, szApprCode, szTID, szMID, szTerminalSerialNO, szBankCode, szMerchantCode, szStoreCode, szPaymentType, szPaymentMedia, szBatchNum, szReceiptImageFileName, szInvNum, szLogoFileName, szSingatureStatus, szRSAKEKVersion, szReceiptLogoIndex, fGzipReceiptImage, fSettle, fReserved1, fReserved2, fReserved3, szReserved1, szReserved2, szReserved3 FROM TransData WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_ERM,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			strERMTransData->shType = sqlite3_column_int(stmt,inStmtSeq);
			strcpy((char*)strERMTransData->szTPDU, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szPAN, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szAmt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szSTAN, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szTime, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szDate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szExpDate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szRefNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szApprCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szTerminalSerialNO, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));			
			strcpy((char*)strERMTransData->szBankCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szMerchantCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szStoreCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szPaymentType, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szPaymentMedia, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szBatchNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szReceiptImageFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szInvNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szLogoFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szSingatureStatus, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szRSAKEKVersion, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szReceiptLogoIndex, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fGzipReceiptImage = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fSettle = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fReserved1 = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fReserved2 = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fReserved3 = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));			
			strcpy((char*)strERMTransData->szReserved1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szReserved2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szReserved3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

//INSERT INTO TransData (TransDataid, shType, szTPDU, szPAN, szAmt, szSTAN, szTime, szDate, szExpDate, szRefNum, szApprCode, szTID, szMID, szTerminalSerialNO, szBankCode, szMerchantCode, szStoreCode, szPaymentType, szPaymentMedia, szBatchNum, szReceiptImageFileName, szInvNum, szLogoFileName, szSingatureStatus, szRSAKEKVersion, szReceiptLogoIndex, fGzipReceiptImage, fSettle, fReserved1, fReserved2, fReserved3, szReserved1, szReserved2, szReserved3) VALUES (1, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1)
int inERMAP_Database_BatchInsert(ERM_TransData *strERMTransData)
{
	int result;	
 	char *sql = "INSERT INTO TransData (TransDataid, shType, szTPDU, szPAN, szAmt, szSTAN, szTime, szDate, szExpDate, szRefNum, szApprCode, szTID, szMID, szTerminalSerialNO, szBankCode, szMerchantCode, szStoreCode, szPaymentType, szPaymentMedia, szBatchNum, szReceiptImageFileName, szInvNum, szLogoFileName, szSingatureStatus, szRSAKEKVersion, szReceiptLogoIndex, fGzipReceiptImage, fSettle, fReserved1, fReserved2, fReserved3, szReserved1, szReserved2, szReserved3) VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	   
	/* open the database */
	result = sqlite3_open(DB_ERM,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	//vdCTOSS_GetMemoryStatus("bagin Database");

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->shType);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szTPDU, strlen((char*)strERMTransData->szTPDU), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szPAN, strlen((char*)strERMTransData->szPAN), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szAmt, strlen((char*)strERMTransData->szAmt), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szSTAN, strlen((char*)strERMTransData->szSTAN), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szTime, strlen((char*)strERMTransData->szTime), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szDate, strlen((char*)strERMTransData->szDate), SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szExpDate, strlen((char*)strERMTransData->szExpDate), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szRefNum, strlen((char*)strERMTransData->szRefNum), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szApprCode, strlen((char*)strERMTransData->szApprCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szTID, strlen((char*)strERMTransData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szMID, strlen((char*)strERMTransData->szMID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szTerminalSerialNO, strlen((char*)strERMTransData->szTerminalSerialNO), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szBankCode, strlen((char*)strERMTransData->szBankCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szMerchantCode, strlen((char*)strERMTransData->szMerchantCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szStoreCode, strlen((char*)strERMTransData->szStoreCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szPaymentType, strlen((char*)strERMTransData->szPaymentType), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szPaymentMedia, strlen((char*)strERMTransData->szPaymentMedia), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szBatchNum, strlen((char*)strERMTransData->szBatchNum), SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReceiptImageFileName, strlen((char*)strERMTransData->szReceiptImageFileName), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szInvNum, strlen((char*)strERMTransData->szInvNum), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szLogoFileName, strlen((char*)strERMTransData->szLogoFileName), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szSingatureStatus, strlen((char*)strERMTransData->szSingatureStatus), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szRSAKEKVersion, strlen((char*)strERMTransData->szRSAKEKVersion), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReceiptLogoIndex, strlen((char*)strERMTransData->szReceiptLogoIndex), SQLITE_STATIC); 
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fGzipReceiptImage);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fSettle);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fReserved1);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fReserved3);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fReserved3);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReserved1, strlen((char*)strERMTransData->szReserved1), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReserved2, strlen((char*)strERMTransData->szReserved2), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReserved3, strlen((char*)strERMTransData->szReserved3), SQLITE_STATIC); 
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inUpdateDefaultApp(int inHostIndexNew, int inHostIndexOld)
{
	int result;
	char *sql = "UPDATE CDT set HDTid=? where HDTid=?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

    inStmtSeq = 0;
    sqlite3_bind_int(stmt, inStmtSeq +=1, inHostIndexNew);
    sqlite3_bind_int(stmt, inStmtSeq +=1, inHostIndexOld);



    result = sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}
	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}



int inDatabase_InvoiceNumInsert(TRANS_DATA_TABLE *transData)
{
	int result;	
	char *sql1 = "SELECT MAX(TransDataid) FROM TransData";

 	char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, szInvoiceNo, ulSavedIndex, byTransType) VALUES (NULL, ?, ?, ?, ?, ?)";


	/* open the database */
	result = sqlite3_open(DB_INVOICE,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			transData->ulSavedIndex = sqlite3_column_int(stmt,inStmtSeq);
			transData->ulSavedIndex += 1;
		}
	} while (result == SQLITE_ROW);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	/* open the database */
	result = sqlite3_open(DB_INVOICE,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_InvoiceNumSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo)
{
	int result,i = 0;
	int inResult = d_NO;

	/* Issue# 000096: BIN VER Checking -- jzg*/
	char *sql = "SELECT HDTid FROM TransData WHERE szInvoiceNo = ?";

	/* open the database */
	result = sqlite3_open(DB_INVOICE,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, hexInvoiceNo, 4, SQLITE_STATIC);

	DebugAddINT("inDatabase_BatchSearch sqlite3_bind_blob", result);
	DebugAddHEX("hexInvoiceNo", hexInvoiceNo, 3);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		i++;
		DebugAddINT("if(100)=Find", result);
		DebugAddINT("Loop Count", i);

		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    DebugAddSTR("batch serPAN", transData->szPAN, 10);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

int inDatabase_InvoiceNumDelete(int HDTid, int MITid)
{
	int result;
	char *sql = "DELETE FROM TransData WHERE HDTid = ? and MITid = ?";	
	int inDBResult =0 ;
	
	//1205
	//remove(DB_INVOICE);
	//1205

	/* open the database */
	result = sqlite3_open(DB_INVOICE,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
//	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)srTransRec.szHostLabel, strlen((char*)srTransRec.szHostLabel), SQLITE_STATIC); 
//	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, srTransRec.szBatchNo, 3, SQLITE_STATIC);

//    result = sqlite3_bind_int(stmt, 1, inSeekCnt);
    sqlite3_bind_int(stmt, inStmtSeq +=1, HDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, MITid);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inEMVReadData(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inSchemeReference, inIssuerReference, inTRMDataPresent, lnEMVFloorLimit, lnEMVRSThreshold, inEMVTargetRSPercent, inEMVMaxTargetRSPercent, inMerchantForcedOnlineFlag, inBlackListedCardSupportFlag, szEMVTACDefault, szEMVTACDenial, szEMVTACOnline, szDefaultTDOL, szDefaultDDOL, inEMVFallbackAllowed, inNextRecord, ulEMVCounter, inEMVAutoSelectAppln, szEMVTermCountryCode, szEMVTermCurrencyCode, inEMVTermCurExp, szEMVTermCapabilities, szEMVTermAddCapabilities, szEMVTermType, szEMVMerchantCategoryCode, szEMVTerminalCategoryCode, inModifyCandListFlag, shRFU1, shRFU2, shRFU3, szRFU1, szRFU2, szRFU3 FROM EMV WHERE EMVid = ?";

	/* open the database */
	result = sqlite3_open(DB_EMV,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		
		//Debug_LogPrintf("EMV read[%d]",result);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*inSchemeReference;*/
			strEMVT_Temp.inSchemeReference = sqlite3_column_int(stmt,inStmtSeq);
    
            /*inIssuerReference*/
			strEMVT_Temp.inIssuerReference = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inTRMDataPresent*/
			strEMVT_Temp.inTRMDataPresent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                            
            /*lnEMVFloorLimit*/
			strEMVT_Temp.lnEMVFloorLimit = sqlite3_column_double(stmt,inStmtSeq +=1 );
                
            /*lnEMVRSThreshold*/            //5
			strEMVT_Temp.lnEMVRSThreshold = sqlite3_column_double(stmt,inStmtSeq +=1 );
                
            /*inEMVTargetRSPercent*/
			strEMVT_Temp.inEMVTargetRSPercent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inEMVMaxTargetRSPercent*/
			strEMVT_Temp.inEMVMaxTargetRSPercent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inMerchantForcedOnlineFlag*/
			strEMVT_Temp.inMerchantForcedOnlineFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inBlackListedCardSupportFlag*/
			strEMVT_Temp.inBlackListedCardSupportFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
            
            /* szEMVTACDefault */            //10
			memcpy(strEMVT_Temp.szEMVTACDefault, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
           
            /* szEMVTACDenial */            
			memcpy(strEMVT_Temp.szEMVTACDenial, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
            
            /* szEMVTACOnline */
			memcpy(strEMVT_Temp.szEMVTACOnline, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
            
            /* szDefaultTDOL */
			strcpy((char*)strEMVT_Temp.szDefaultTDOL, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szDefaultDDOL */
			strcpy((char*)strEMVT_Temp.szDefaultDDOL, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*inEMVFallbackAllowed*/        //15
			strEMVT_Temp.inEMVFallbackAllowed = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /*inNextRecord*/
			strEMVT_Temp.inNextRecord = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /*ulEMVCounter*/
			strEMVT_Temp.ulEMVCounter = sqlite3_column_double(stmt, inStmtSeq +=1 );

            /*inEMVAutoSelectAppln*/
			strEMVT_Temp.inEMVAutoSelectAppln = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szEMVTermCountryCode */
			memcpy(strEMVT_Temp.szEMVTermCountryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			
            /* szEMVTermCurrencyCode */        //20
			memcpy(strEMVT_Temp.szEMVTermCurrencyCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            
            /*inEMVTermCurExp*/
			strEMVT_Temp.inEMVTermCurExp = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szEMVTermCapabilities*/
			memcpy(strEMVT_Temp.szEMVTermCapabilities, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
               
            /* szEMVTermAddCapabilities */
			memcpy(strEMVT_Temp.szEMVTermAddCapabilities, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);
    
            /* szEMVTermType */
			memcpy(strEMVT_Temp.szEMVTermType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            
            /* szEMVMerchantCategoryCode */        //25
			memcpy(strEMVT_Temp.szEMVMerchantCategoryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);

            /* szEMVTerminalCategoryCode */
			memcpy(strEMVT_Temp.szEMVTerminalCategoryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

            /*inModifyCandListFlag*/
			strEMVT_Temp.inModifyCandListFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU1*/
			strEMVT_Temp.shRFU1 = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU2*/
			strEMVT_Temp.shRFU2 = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU3*/                            //30
			strEMVT_Temp.shRFU3 = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szRFU1*/
			strcpy((char*)strEMVT_Temp.szRFU1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
    
            /* szRFU2 */
			strcpy((char*)strEMVT_Temp.szRFU2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szRFU3 */
			strcpy((char*)strEMVT_Temp.szRFU3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

// ERM
int inERMAP_Database_ERMAdviceBatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM Advice";	
	int inDBResult =0 ;

	/* open the database */
	result = sqlite3_open(DB_ERM_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

// ERM
int inERMAP_Database_ERMTransDataBatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM TransData";	
	int inDBResult =0 ;

	/* open the database */
	result = sqlite3_open(DB_ERM_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

// ERM
int inERMAP_Database_ERMTransDataBackupBatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM TransDataBackup";	
	int inDBResult =0 ;

	/* open the database */
	result = sqlite3_open(DB_ERM_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_TerminalOpenDatabase(const char *szDBName)
{
	int result;

	vdDebug_LogPrintf("--inDatabase_TerminalOpenDatabase--");	
	
	/* open the database */
	result = sqlite3_open(szDBName,&db);
	vdDebug_LogPrintf("szDBName[%s] | result[%d]", szDBName, result);
	
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		vdDebug_LogPrintf("DB Open %s Fail",szDBName);
		vdSetDBOpen(TRUE);
		return 1;
	}
	
	vdDebug_LogPrintf("DB Open %s Successful",szDBName);
	return d_OK;
}

int inDatabase_TerminalCloseDatabase(void)
{
	int result;

	vdDebug_LogPrintf("--inDatabase_TerminalCloseDatabase--");
	
	if (db != NULL) sqlite3_close(db);
	db = NULL;
	
	return d_OK;
}

int inMMTBatchNotEmptySave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE MMT SET fBatchNotEmpty = ? WHERE  MMTid = ?";

	vdDebug_LogPrintf("***inMMTBatchNotEmptySave inSeekCnt [%d]***",inSeekCnt);
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	/* fMustSettFlag */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].fBatchNotEmpty);
    
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}
	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);


    return(d_OK);
}

int inCheckBatcheNotEmtpy(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM MMT where fBatchNotEmpty = 1";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	vdDebug_LogPrintf("inCount[%d]",inCount);
	return(inCount);
}

int inTLESecRead(int inSeekCnt)
{
	int len;
	CTOS_RTC SetRTC;     
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inHostindex, szHostLabel, szTPDU, szNII, szTID FROM TLESEC WHERE inHostindex = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*inHostIndex*/
			strTLESEC.inHostindex= sqlite3_column_int(stmt, inStmtSeq );

            /* szHostLabel */
			strcpy((char*)strTLESEC.szHostLabel, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
               
			/*szTPDU*/
			memcpy(strTLESEC.szTPDU, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);

			/* szNII*/
			memcpy(strTLESEC.szNII, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);

			/*szTID*/
			memcpy(strTLESEC.szTID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);			
			
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inDatabase_ISOLogInsert(TRANS_ISOLOG_TABLE *transData)
{
	int result;	
	char *sql1 = "SELECT MAX(TransDataid) FROM TransData";
 	char *sql = "INSERT INTO TransData (TransDataid,szISOLogTransType,szISOLogDateTime,szISOLogData,ulSavedIndex) VALUES (NULL, ?, ?, ?, ?)";


	/* open the database */
	result = sqlite3_open(ISOLOG_DB,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			transData->ulSavedIndex = sqlite3_column_int(stmt,inStmtSeq);
			transData->ulSavedIndex += 1;
		}
	} while (result == SQLITE_ROW);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	/* open the database */
	result = sqlite3_open(ISOLOG_DB,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szISOLogTransType, strlen((char*)transData->szISOLogTransType), SQLITE_STATIC);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szISOLogDateTime, strlen((char*)transData->szISOLogDateTime), SQLITE_STATIC);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szISOLogData, strlen((char*)transData->szISOLogData), SQLITE_STATIC); 
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}


int inDatabase_DeleteISOLogRec(void)
{
	int result;
	char *sql = "DELETE FROM TransData WHERE TransDataid in (select TransDataid FROM TransData LIMIT 1)";	
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(ISOLOG_DB,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Delete isolog 1st record,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_DeleteISOLogAll(void)
{
	int result;
	char *sql = "DELETE FROM TransData";	
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(ISOLOG_DB,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Delete isolog all record,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inBLRRead(int inSeekCnt)
{     
	int result;
	int inCount=0;
	//char *sql = "SELECT HDTid, szHostName,inCommunicationMode, szPriTxnPhoneNumber, szSecTxnPhoneNumber, szPriSettlePhoneNumber, szSecSettlePhoneNumber, fFastConnect, fDialMode, inDialHandShake, szATCommand, inMCarrierTimeOut, inMRespTimeOut, fPreDial, szPriTxnHostIP, szSecTxnHostIP, szPriSettlementHostIP, szSecSettlementHostIP, inPriTxnHostPortNum, inSecTxnHostPortNum, inPriSettlementHostPort, inSecSettlementHostPort, fTCPFallbackDial, fSSLEnable, inTCPConnectTimeout, inTCPResponseTimeout, inCountryCode, inHandShake, inParaMode, inIPHeader, fCommBackUpMode, inCommunicationBackUpMode FROM CPT WHERE CPTid = ?";         
	char *sql = "SELECT BLRid, szBillerName, szBillerCode, szBillerFee FROM BLR WHERE fBLREnable = 1";
	
	#if 1
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	#endif
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	//result = sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* USRid */
			strMultiBLR[inCount].BLRid = sqlite3_column_int(stmt, inStmtSeq );   

			/* szUserName*/
			strcpy((char*)strMultiBLR[inCount].szBillerName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szPassword; */
			strcpy((char*)strMultiBLR[inCount].szBillerCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szBillerFee */
			strcpy((char*)strMultiBLR[inCount].szBillerFee, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			inCount++;
			if (inCount>50)
				break;
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

int inBLRNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM BLR";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}



