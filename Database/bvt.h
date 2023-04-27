

typedef struct
{
	UINT 	BVTid;
	BYTE	szBINVerTPDU[5+1];
	BYTE	szBINVerNII[4+1] ;
    BYTE	szBINVerTID[10] ;
    BYTE	szBINVerMID[20] ;
	/*BDO: Separate dialing number for BIN Ver - start -- jzg */
	BYTE	szBINVerPriNum[30];
	BYTE	szBINVerSecNum[30];
	/*BDO: Separate dialing number for BIN Ver - end -- jzg */

}STRUCT_BVT;

STRUCT_BVT strBVT;

int inBVTRead(int inSeekCnt);
int inBVTSave(int inSeekCnt);
int inBVTNumRecord(void);


