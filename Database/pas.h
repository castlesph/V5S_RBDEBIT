

typedef struct
{
	UINT PASid;
	int inRetryTime;
	BOOL fEnable;
	BOOL fOnGoing;
	char szSTLTime1[8];
	char szSTLTime2[8];
	char szSTLTime3[8];
	char szWaitTime[8];
} STRUCT_PAS;

STRUCT_PAS strPAS;

int inPASRead(int inSeekCnt);
int inPASSave(int inSeekCnt);
int inPASNumRecord(void);


