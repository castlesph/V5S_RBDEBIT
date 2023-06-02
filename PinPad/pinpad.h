
#ifndef ___PINPAD_H___
#define	___PINPAD_H___

#ifdef	__cplusplus
extern "C" {
#endif

    int inInitializePinPad(void);
    void TEST_Write3DES_Plaintext(void);
    void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine);
    void OnGetPINDigit(BYTE NoDigits);
    void OnGetPINCancel(void);
    void OnGetPINBackspace(BYTE NoDigits);
    int inGetIPPPin(void);
    int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC);
    int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC);
    int inCheckKeys(USHORT ushKeySet, USHORT ushKeyIndex);
	void vdCTOSS_EFT_TestHardcodeKey(void);

	void vdSetDummyMKey(void);

	// PIN Result Define -- sidumili
	#define IPP_CANCEL    -1
	#define IPP_TIMEOUT   -2
	#define IPP_ERROR     -3
	
#ifdef	__cplusplus
}
#endif

#endif	/* ___PINPAD_H___ */

