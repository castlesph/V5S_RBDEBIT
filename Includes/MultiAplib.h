

#ifndef _MULTIAPLIB_H
#define	_MULTIAPLIB_H

#ifdef	__cplusplus
extern "C" {
#endif



BYTE inMultiAP_IPCGetParent(BYTE *str , int *len);
BYTE inMultiAP_IPCSendParent(BYTE *str, int len);
USHORT inMultiAP_IPCCmdParent(BYTE *in, BYTE *out, int inprocessID);
void vdMultiAP_getPID(char* processName, char* processID);
int inMultiAP_CheckSubAPStatus(void);
int inMultiAP_ForkTask(void);
int inMultiAP_ForkSharlsAp(void);
int inMultiAP_CheckMainAPStatus(void);
BYTE inMultiAP_IPCGetChild(BYTE *str , int *len);
BYTE inMultiAP_IPCSendChild(BYTE *str, int len);
int inMultiAP_CurrentAPNamePID(char *szAPName, int *inAPPID);
int inCTOS_KillASubAP(char *Appname);
int inCTOS_ReForkSubAP(char *Appname);
int inCTOS_KillALLSubAP(void);
int inCTOSS_MultiCheckUnForkAPName(char *Appname);

void vdCTOSS_RestoreMemory(int limitRAM);
USHORT usCTOSS_SystemMemoryStatus(ULONG* ulUsedDiskSize ,ULONG* ulTotalDiskSize ,ULONG* ulUsedRamSize ,ULONG* ulTotalRamSize);

#ifdef	__cplusplus
}
#endif

#endif	/* _MULTIAPLIB_H */

