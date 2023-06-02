
typedef struct
{
	int PRMid;
	int HDTid;
	char szPromoLabel[21];
	char szPromoCode[3];
	int inGroup;
} STRUCT_PRM;

STRUCT_PRM strPRM;
STRUCT_PRM strMultiPRM[20];

int inPRMReadbyHDTid(int inHDTid, int *inNumRecs);
int inPRMReadbyPRMid(int inPRMid);
int inPRMSave(int inSeekCnt);
int inPRMNumRecord(void);
int inPRMReadbyinInstGroup(int inGroup, int *inNumRecs);

