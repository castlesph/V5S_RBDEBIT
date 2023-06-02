/** 
**    A Template for developing new terminal shared application
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <sqlite3.h>

//#include "dct.h"
#include "../Database/DatabaseFunc.h"
#include "../Includes/POSTypedef.h"
#include "../FileModule/myFileFunc.h"
#include "..\Debug\Debug.h"


extern sqlite3 * db;
extern sqlite3_stmt *stmt;
extern int inStmtSeq;
#define DB_TERMINAL "/home/ap/pub/TERMINAL.S3DB"

int inDCTRead(int inHDTid, int inMITid)
{     
	int result;
	//char *sql = "SELECT HDTid, inMinPINDigit, inMaxPINDigit, szPINKey, szMACKey, usKeySet, usKeyIndex, szDisplayLine1, szDisplayLine2, szDisplayProcessing FROM DCT WHERE HDTid = ?";
	char *sql = "SELECT HDTid, inMinPINDigit, inMaxPINDigit, szPINKey, szMACKey, usKeySet, usKeyIndex, szDisplayLine1, szDisplayLine2, szDisplayProcessing, inMMTid FROM DCT WHERE HDTid = ? AND inMMTid = ?";	
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	vdDebug_LogPrintf("AAA - inDCTRead START]");
	vdDebug_LogPrintf("AAA - 2 inDCTRead HDTid[%d], inMITid[%d]", inHDTid, inMITid);
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMITid);
	//sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

             /* HDTid */
             strDCT.HDTid = sqlite3_column_int(stmt, inStmtSeq);   
             /* inMinPINDigit*/
             strDCT.inMinPINDigit = sqlite3_column_int(stmt, inStmtSeq+=1);
             /* inMaxPINDigit*/
             strDCT.inMaxPINDigit = sqlite3_column_int(stmt, inStmtSeq+=1);
             /*szPINKey*/
             memcpy(strDCT.szPINKey, sqlite3_column_blob(stmt,inStmtSeq +=1 ), IPP_TDES_KEY_SIZE);					
             /*szMACKey*/
             memcpy(strDCT.szMACKey, sqlite3_column_blob(stmt,inStmtSeq +=1 ), IPP_TDES_KEY_SIZE);
             /*usKeySet*/
             memcpy((BYTE *)&strDCT.usKeySet, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);
             /*usKeyIndex*/
             memcpy((BYTE *)&strDCT.usKeyIndex, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);                                                 
             /*szDisplayLine1*/
             strcpy((char*)strDCT.szDisplayLine1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
             /*szDisplayLine2*/
             strcpy((char*)strDCT.szDisplayLine2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
             /*szDisplayProcessing*/
             strcpy((char*)strDCT.szDisplayProcessing, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
             /*inMMTid*/
             strDCT.inMMTid = sqlite3_column_int(stmt, inStmtSeq+=1);
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	vdDebug_LogPrintf("AAA - inDCTRead END]");
	return(d_OK);
}

int inDCTSave(int inHDTid, int inMITid)
{
	int result;
	//char *sql = "UPDATE DCT SET inMinPINDigit=?, inMaxPINDigit=?, szPINKey=?, szMACKey=?, usKeySet=?, usKeyIndex=?, szDisplayLine1=?, szDisplayLine2=?, szDisplayProcessing=? WHERE HDTid = ?";
	  char *sql = "UPDATE DCT SET inMinPINDigit=?, inMaxPINDigit=?, szPINKey=?, szMACKey=?, usKeySet=?, usKeyIndex=?, szDisplayLine1=?, szDisplayLine2=?, szDisplayProcessing=? WHERE HDTid = ?  AND inMMTid = ?";

 	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	vdDebug_LogPrintf("AAA - inDCTSave START]");
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	inStmtSeq = 0;
	/* inMinPINDigit*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strDCT.inMinPINDigit);
	/* inMaxPINDigit*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strDCT.inMaxPINDigit);
	/* szPINKey*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strDCT.szPINKey, IPP_TDES_KEY_SIZE, SQLITE_STATIC);
	/* szMACKey*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strDCT.szMACKey, IPP_TDES_KEY_SIZE, SQLITE_STATIC);
	/* usKeySet*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, (BYTE *)&strDCT.usKeySet, 2, SQLITE_STATIC);
	/* usKeyIndex*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, (BYTE *)&strDCT.usKeyIndex, 2, SQLITE_STATIC);	
	/* szDisplayLine1*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strDCT.szDisplayLine1, strlen((char*)strDCT.szDisplayLine1), SQLITE_STATIC);
	/* szDisplayLine2*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strDCT.szDisplayLine2, strlen((char*)strDCT.szDisplayLine2), SQLITE_STATIC);
	/* szDisplayProcessing*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strDCT.szDisplayProcessing, strlen((char*)strDCT.szDisplayLine2), SQLITE_STATIC);
	/* inHDTid*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inHDTid);
	/* inMITid*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inMITid);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}  
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	vdDebug_LogPrintf("AAA - inDCTSave END]");
	return(d_OK);
}

int inDCTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM DCT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
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
