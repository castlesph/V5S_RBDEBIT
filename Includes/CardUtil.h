/* 
 * File:   CardUtil.h
 * Author: Administrator
 *
 * Created on 2012年8月21日, 下午 4:25
 */

#ifndef CARDUTIL_H
#define	CARDUTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

extern void vdGetCardHolderName(BYTE *szTrack1, USHORT usTRack1Len);
extern void vdAnalysisTrack2(BYTE *szTrack2, USHORT usTRack2Len);
extern void vdAnalysisTrack1(BYTE *szTrack1, USHORT usTRack2Len);
extern void vdRemoveCard(void);

short chk_luhn(BYTE *szPAN);
short shChk_ExpireDate(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CARDUTIL_H */

