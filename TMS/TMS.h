#ifndef __CTOSS_TMS_H__
#define __CTOSS_TMS_H__

int inCTOSS_SettlementCheckTMSDownloadRequest(void);

int inCTOSS_CheckIfPendingTMSDownload(void);
int inCTOSS_TMSDownloadRequest(void);
int inCTOSS_TMSChkBatchEmptyProcess(void);
int inCTOSS_TMSChkBatchEmpty(void);
int inCTOSS_TMSDownloadRequest(void);
void inCTOSS_TMS_USBUpgrade(void);
int inCTOSS_TMSPreConfig2(int inComType);


#endif //end __CTOSS_TMS_H__
