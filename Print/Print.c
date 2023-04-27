#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <ctype.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <vwdleapi.h>
#include <emv_cl.h>


#include "../Includes/wub_lib.h"
#include "../Includes/myEZLib.h"
#include "../Includes/msg.h"

#include "Print.h"
#include "../FileModule/myFileFunc.h"
#include "../UI/Display.h"
#include "../Includes/POSTypedef.h"
#include "..\Includes\CTOSInput.h"
#include "../accum/accum.h"
#include "../DataBase/DataBaseFunc.h"
#include "..\debug\debug.h"
#include "..\Includes\Showbmp.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Includes\epad.h"
#include "..\Erm\PosErm.h"

extern BOOL fAUTOManualSettle;

const BYTE baPrinterBufferLogo_Single[]={ //Width=60, Height=49
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                         
                                                                                       
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0x18,0x18,0x8C,0xC4,0xC6,      
    0x66,0x66,0x22,0x22,0x22,0x22,0x22,0x62,0x66,0x46,0xC4,0x84,0x8C,0x18,0x10,0x30,      
    0x60,0xC0,0xC0,0xE0,0x38,0x0E,0x07,0x1E,0x38,0xE0,0x80,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                         
                                                                                       
    0x00,0x00,0x00,0x00,0xC0,0xF8,0x0E,0x03,0x81,0xF0,0x1C,0x0E,0x03,0xC1,0xF0,0xF8,      
    0x7C,0x3C,0x1C,0x1E,0x1E,0x1E,0xFE,0xFC,0xFC,0xFC,0xF8,0xF0,0xC1,0x83,0xCE,0x7C,      
    0x1C,0x07,0x81,0xE0,0x70,0x1C,0x0E,0x1C,0x70,0xC0,0x83,0x07,0x1C,0x70,0xE0,0x80,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                         
                                                                                      
    0x00,0x00,0x00,0x00,0x0F,0x7F,0xC0,0x00,0x03,0x3F,0xF0,0x80,0x00,0x0F,0x3F,0x7F,  
    0xFF,0xFC,0xFC,0xFC,0xFC,0xFE,0xF3,0xFB,0xFF,0xFF,0x7F,0x3F,0x0F,0x03,0x81,0xE0,  
    0x38,0x0E,0x03,0xC0,0xF0,0x1C,0x1C,0x38,0xE0,0x81,0x03,0x0E,0x38,0x70,0xC0,0x03,  
    0x0E,0x1C,0x70,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,  
    0xF0,0x78,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x08,0x18,0x38,0x30,0x00,0x00,0x00,0x00,  
    0x80,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x80,0xC0,  
    0xC0,0xC0,0xC0,0x80,0x00,0x00,0xC0,0xFC,0xFC,0xFC,0xC0,0xC0,0x00,0xFC,0xFC,0xFC,  
    0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,  
    0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,  
    0x0C,0xFC,0xFC,0xFC,0x0C,0x0C,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,  
    0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,  
    0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,  
    0xC0,0xC0,0xC0,0xC0,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,  
    0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,  
    0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,  
    0xC0,0xC0,0xC0,0x80,0x80,0xC0,0xC0,0xC0,0x00,0x00,0x40,0xC0,0xC0,0x80,0x00,0x00,  
    0x00,0x00,0x80,0xC0,0xC0,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,  
    0xF0,0x78,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x38,0x30,0x00,0x00,0x00,0x00,  
    0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0xC0,0xC0,0xFC,0xFC,0xFC,0xC0,0x00,0x00,  
    0x00,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                     
                                                                                      
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x0E,0x18,0x30,0x61,0x63,0xC6,0x8C,0x88,  
    0x98,0x98,0x11,0x11,0x11,0x11,0x11,0x11,0x90,0x98,0x88,0x8C,0xC6,0x63,0x61,0x30,  
    0x18,0x0C,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x1C,0x70,0xE0,0x81,0x07,  
    0x0E,0x38,0xE0,0xC1,0x07,0x0E,0x38,0x70,0xC0,0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,  
    0x7F,0xF0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0x60,0x00,0x00,0x00,0x3F,  
    0xFF,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x03,0xCF,0x8F,  
    0x9E,0x9C,0xFC,0xF8,0x20,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,  
    0x00,0x00,0x3E,0x7F,0xFF,0xCD,0x8C,0x8C,0x8C,0x8C,0xCF,0xCF,0x0E,0x08,0x00,0x03,  
    0xC7,0x8F,0x8E,0x9C,0xFC,0xF9,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3E,0x7F,0xFF,0xCD,0xCC,0x8C,0x8C,0x8C,  
    0xCF,0xCF,0x0F,0x0C,0x00,0x00,0x1C,0x7F,0xFF,0xE3,0xC1,0x80,0x80,0x80,0xC1,0xC1,  
    0x41,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x01,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,  
    0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x01,0xFF,0xFF,0xFF,0x00,0x00,0x08,0x3E,0x7F,0xFF,  
    0xC1,0x80,0x80,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x3E,  
    0x7F,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xE3,0x7F,0x3F,0x08,0x00,0x0C,0x7F,0xFF,0xE3,  
    0xC1,0x80,0x80,0x80,0xC1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x03,0x0F,0x3F,0xFC,0xF0,  
    0xF8,0x7E,0x1F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,  
    0x7F,0xF0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0x60,0x00,0x00,0x08,0x3E,  
    0x7F,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0x80,0xC0,0xC0,0x00,  
    0x00,0x00,0x00,0x00,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0xFF,0xFF,0xFF,0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x1C,  
    0x7F,0xFF,0xE1,0xC0,0x80,0x80,0xC0,0xE1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xC0,0xC0,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                     
                                                                                      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,  
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,  
    0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,  
    0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,  
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0E,0x0E,  
    0x0D,0x09,0x09,0x08,0x0C,0x0F,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x0C,0x0F,0x0F,  
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,  
    0x00,0x00,0x00,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                     
};

TRANS_TOTAL stBankTotal;
BOOL	fRePrintFlag = FALSE; 
BOOL 	fERMTransaction = FALSE;

static CTOS_FONT_ATTRIB stgFONT_ATTRIB;

void vdSetGolbFontAttrib(USHORT FontSize, USHORT X_Zoom, USHORT Y_Zoom, USHORT X_Space, USHORT Y_Space)
{
    memset(&stgFONT_ATTRIB, 0x00, sizeof(stgFONT_ATTRIB));
    
    stgFONT_ATTRIB.FontSize = FontSize;      // Font Size = 12x24
	stgFONT_ATTRIB.X_Zoom = X_Zoom;		    // The width magnifies X_Zoom diameters
	stgFONT_ATTRIB.Y_Zoom = Y_Zoom;		    // The height magnifies Y_Zoom diameters

    stgFONT_ATTRIB.X_Space = X_Space;      // The width of the space between the font with next font
    stgFONT_ATTRIB.Y_Space = Y_Space;      // The Height of the space between the font with next font      
    
}

short printCheckPaper(void)
{
	unsigned short inRet;
	unsigned char key;
	
	while(1)
	{
		inRet = CTOS_PrinterStatus();
		if (inRet==d_OK)
			return 0;
		else if(inRet==d_PRINTER_PAPER_OUT)
		{
			vduiClearBelow(3);
			vduiWarningSound();
			#if 0
			vduiDisplayStringCenter(3,"PRINTER OUT OF");
			vduiDisplayStringCenter(4,"PAPER, INSERT");
			vduiDisplayStringCenter(5,"PAPER AND PRESS");
			vduiDisplayStringCenter(6,"ANY KEY TO PRINT.");
			#else
			vduiDisplayStringCenter(4,"OUT OF PAPER");
			#endif


			CTOS_KBDGet(&key);
			//if(key==d_KBD_CANCEL)
			//	return -1;	
		}		
	}	
}


void cardMasking(char *szPan, int style)
{
    int num;
    int i;
	char szTemp[30];
    if (style == PRINT_CARD_MASKING_1)
    {
        num = strlen(szPan) - 10;
        if (num > 0)
        {
            for (i = 0; i < num; i++)
            {
                szPan[6+i] = '*';
            }
        }
    }
    else if (style == PRINT_CARD_MASKING_2)
    {
        num = strlen(szPan) - 12;
        if (num > 0)
        {
            for (i = 0; i < num; i++)
            {
                szPan[12+i] = '*';
            }
        }
    }

	else if (style == PRINT_CARD_MASKING_3)
	{
	    memset(szTemp, 0x00, sizeof(szTemp));

		strcpy(szTemp,"**** **** **** ");
		num = strlen(szPan) - 4;

		//for (i = 0; i < num; i++)
		//{
		//	szTemp[i] = '*';
		//}

		memcpy(&szTemp[num+3],&szPan[num],4);
		strcpy(szPan,szTemp);
	}

else if (style == PRINT_CARD_MASKING_4){
	
	memset(szTemp, 0x00, sizeof(szTemp));
	num = strlen(szPan) - 4;

	
	for (i = 0; i < num; i++)
	{
		szTemp[i] = '*';
	}

	
	memcpy(&szTemp[num],&szPan[num],4);
	strcpy(szPan,szTemp);


}

else if (style == PRINT_CARD_MASKING_5)
{
	//strcpy(szPan, "1234567890123456789");
	//strcpy(szPan, "123456789012345678");
	
	memset(szTemp, 0x00, sizeof(szTemp));
	num = strlen(szPan) - 4;
	if(strlen(szPan) > 16)
	{
		memset(szTemp, 0x2a, num);				
		memcpy(szTemp, szPan, 6);
		memcpy(&szTemp[num],&szPan[num],4);
	}
	else	
	{
		memcpy(szTemp, szPan, 4);
		memcpy(&szTemp[4]," ",1);
		memcpy(&szTemp[5],&szPan[4],2);
		memcpy(&szTemp[7],"** **** ",8);
		memcpy(&szTemp[num+3],&szPan[num],4);
	}
	//strcpy(szPan,szTemp);
	memcpy(szPan, szTemp, strlen(szTemp));
}



}

USHORT printDateTime(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
  	CTOS_RTC SetRTC;
	char szYear[3];
//1102
	char sMonth[4];
	char szTempMonth[3];
	char szMonthNames[40];

	memset(sMonth,0x00,4);
	memset(szMonthNames,0x00,40);
	strcpy(szMonthNames,"JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC");
	memset(szTempMonth, 0x00, 3);

//1102
	
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);


	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	vdDebug_LogPrintf("year[%s],date[%02x][%02x]",srTransRec.szYear,srTransRec.szDate[0],srTransRec.szDate[1]);
    wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
    wub_hex_2_str(srTransRec.szTime, szTemp1,TIME_BCD_SIZE);
	vdDebug_LogPrintf("date[%s],time[%s]atol(szTemp)=[%d](atol(szTemp1)=[%d]",szTemp,szTemp1,atol(szTemp),atol(szTemp1) );

	if(atol(szTemp) == 0)
	{
		//sprintf(szTemp ,"%02d%02d",SetRTC.bDay,SetRTC.bMonth);
		sprintf(szTemp ,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);	
		memset(srTransRec.szDate,0x00,sizeof(srTransRec.szDate));
		wub_str_2_hex(szTemp,srTransRec.szDate,DATE_ASC_SIZE);
	}
	
	if(atol(szTemp1) == 0)
	{
		sprintf(szTemp1 ,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);	
		memset(srTransRec.szTime,0x00,sizeof(srTransRec.szTime));
		wub_str_2_hex(szTemp1,srTransRec.szTime,TIME_ASC_SIZE);
	}
	vdDebug_LogPrintf("date[%s],time[%s]",szTemp,szTemp1);
	memcpy(szTempMonth,&szTemp[0],2);
	memcpy(sMonth, &szMonthNames[(atoi(szTempMonth) - 1)* 3], 3);
// Removed strTCT.fSMReceipt implementation
#if 0

	if (strTCT.fSMReceipt== 1)
		sprintf(szStr,"Date/Time    : %02lu/%02lu/%04lu         %02lu:%02lu:%02lu",atol(szTemp)/100,atol(szTemp)%100,atol(srTransRec.szYear)+2000,atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	else
#endif
		sprintf(szStr,"DATE/TIME    : %s %02lu, %02lu    %02lu:%02lu:%02lu",sMonth,atol(szTemp)%100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
//1102    
    memset (baTemp, 0x00, sizeof(baTemp));		
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
     inPrint(szStr);
	
    return d_OK;
     
}

// v10.0 change - new function
USHORT printDateTimeAndOther(unsigned char *strOther, int inMode)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
  	CTOS_RTC SetRTC;
	char szYear[3];
//1102
	char sMonth[4];
	char szTempMonth[3];
	char szMonthNames[40];

	memset(sMonth,0x00,4);
	memset(szMonthNames,0x00,40);
	strcpy(szMonthNames,"JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC");
	memset(szTempMonth, 0x00, 3);

//1102
	
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);


	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	vdDebug_LogPrintf("year[%s],date[%02x][%02x]",srTransRec.szYear,srTransRec.szDate[0],srTransRec.szDate[1]);
    wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
    wub_hex_2_str(srTransRec.szTime, szTemp1,TIME_BCD_SIZE);
	vdDebug_LogPrintf("date[%s],time[%s]atol(szTemp)=[%d](atol(szTemp1)=[%d]",szTemp,szTemp1,atol(szTemp),atol(szTemp1) );

	if(atol(szTemp) == 0)
	{
		//sprintf(szTemp ,"%02d%02d",SetRTC.bDay,SetRTC.bMonth);
		sprintf(szTemp ,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);	

	}
	
	if(atol(szTemp1) == 0)
	{
		sprintf(szTemp1 ,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);	
	}
	vdDebug_LogPrintf("date[%s],time[%s]",szTemp,szTemp1);
//1102	
    //sprintf(szStr,"DATE/TIME : %02lu/%02lu/%02lu    %02lu:%02lu:%02lu",atol(szTemp)%100,atol(szTemp)/100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	memcpy(szTempMonth,&szTemp[0],2);
	memcpy(sMonth, &szMonthNames[(atoi(szTempMonth) - 1)* 3], 3);
	//sprintf(szStr,"DATE/TIME : %s %02lu, %02lu    %02lu:%02lu:%02lu",sMonth,atol(szTemp)%100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	sprintf(szStr,"%s %02lu, %02lu %02lu:%02lu:%02lu",sMonth,atol(szTemp)%100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	inPrintLeftRight(szStr, strOther, inMode);
//1102    
    //memset (baTemp, 0x00, sizeof(baTemp));		

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
    
    return d_OK;
     
}

USHORT printTIDMID(void)
{
    char szStr[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
	
	memset(szStr, ' ', d_LINE_SIZE);
	//if (strTCT.inSMACMode == 0)
    //	sprintf(szStr, "TERM# %s       MER# %s", srTransRec.szTID,srTransRec.szMID);
	//else
		sprintf(szStr, "TID: %-16.16sMID: %s", srTransRec.szTID,srTransRec.szMID);
    memset (baTemp, 0x00, sizeof(baTemp));		 
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
    inPrint(szStr);
	
//    memset(szStr, ' ', d_LINE_SIZE);
//    sprintf(szStr, "MER# %s", srTransRec.szMID);
//    memset (baTemp, 0x00, sizeof(baTemp));		 
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 

    return d_OK;

}

USHORT printBatchInvoiceNO(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
        
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
    memset(szTemp2, ' ', d_LINE_SIZE);
	
	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
    wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
    vdMyEZLib_LogPrintf("invoice no: %s",szTemp2);
	//if (strTCT.inSMACMode == 0)
    //	sprintf(szStr, "BATCH NO. %s         TRACE NO. %s", szTemp1, szTemp2);
	//else
    sprintf(szStr, "BATCH: %-14.14sINVOICE: %s", szTemp1, szTemp2);	// v10.0 change
    memset (baTemp, 0x00, sizeof(baTemp));		

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
    inPrint(szStr);
	
    return d_OK;
	
}


USHORT printBatchNO(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
	char szBuff[46];
 
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
	memset(szBuff,0x00,sizeof(szBuff));
	wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
    sprintf(szStr, "BATCH %s", szTemp);
	sprintf(szBuff,"HOST             %s",srTransRec.szHostLabel);
	
	inPrintLeftRight(szStr,szBuff,42);
}


USHORT printHostLabel(void)
{
    char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;

    memset(szStr, 0x00, sizeof(szStr));
    sprintf(szStr, "HOST: %s", srTransRec.szHostLabel);
    memset (baTemp, 0x00, sizeof(baTemp));		

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
    inPrint(szStr);
	return d_OK;
}


void vdPrintCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 32;
    BYTE baTemp[PAPER_X_SIZE * 64];
    int inSpacing=3;
	
    if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 32;
    else if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
         usCharPerLine = 16;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 42;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 24;
    else
        usCharPerLine = 32;

    if(stgFONT_ATTRIB.X_Zoom == DOUBLE_SIZE)
         inSpacing=4;
	
    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;
				
	memset(tucPrint,0x20,55);
	memcpy(tucPrint+spacestring,strIn,usCharPerLine);	
	
	tucPrint[i+spacestring]=0;
    memset (baTemp, 0x00, sizeof(baTemp)); 
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, inSpacing); 
    //inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB,fERMTransaction);
}

void vdPrintCenterReverseMode(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 32;
    BYTE baTemp[PAPER_X_SIZE * 64];
    int inSpacing=3;
		
    if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 32;
    else if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 16;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 42;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 24;
    else
        usCharPerLine = 32;
        
    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;
				
	memset(tucPrint,0x20,55);
	memcpy(tucPrint+spacestring,strIn,usCharPerLine);	
	
	tucPrint[i+spacestring]=0;
    memset (baTemp, 0x00, sizeof(baTemp));
    strcpy(baTemp, "\fr");
	strcat(baTemp, tucPrint);
	strcpy(tucPrint, baTemp);
	vdCTOS_Pad_String(tucPrint, 42, ' ', POSITION_RIGHT);
	memset(baTemp, 0x00, sizeof(baTemp));
    //CTOS_PrinterPutString(baTemp);

    if(stgFONT_ATTRIB.X_Zoom == DOUBLE_SIZE)
         inSpacing=4;
      
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //if((d_FONT_24x24 == stgFONT_ATTRIB.FontSize) || (d_FONT_12x24 == stgFONT_ATTRIB.FontSize)
		//&& DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4); 
	//else
	    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, inSpacing); 
    //inCCTOS_PrinterBufferOutput(tucPrint, &stgFONT_ATTRIB, 1);
	inCCTOS_PrinterBufferOutput(tucPrint, &stgFONT_ATTRIB, fERMTransaction);
	
}


void vdPrintTitleCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 16;
    BYTE baTemp[PAPER_X_SIZE * 64];

    vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
    
    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;
				
	memset(tucPrint,0x20,30);
	
	memcpy(tucPrint+spacestring,strIn,usCharPerLine);	
	
	tucPrint[i+spacestring]=0;
        
    memset (baTemp, 0x00, sizeof(baTemp));	
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
    //inCCTOS_PrinterBufferOutput(tucPrint, &stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(tucPrint, &stgFONT_ATTRIB,fERMTransaction);

    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
}

USHORT printDividingLine(int style)
{
    if (style == DIVIDING_LINE_DOUBLE)
       // return(CTOS_PrinterPutString("==========================================="));
		//return(inCCTOS_PrinterBufferOutput("===========================================",&stgFONT_ATTRIB,1));
		return(inCCTOS_PrinterBufferOutput("===========================================",&stgFONT_ATTRIB,fERMTransaction));	
    else
        //return(CTOS_PrinterPutString("-------------------------------------------"));
		//return(inCCTOS_PrinterBufferOutput("-------------------------------------------",&stgFONT_ATTRIB,1));
		return(inCCTOS_PrinterBufferOutput("-------------------------------------------",&stgFONT_ATTRIB,fERMTransaction));
}

void vdCTOSS_GetEMVTagsDescription(USHORT usTag, char *szValue)
{
    switch(usTag)
    {
        case TAG_50:
            strcpy(szValue, "Appl Label");
            break;

        case TAG_57:
            strcpy(szValue, "Track2 Data");
            break;

        case TAG_5F2A_TRANS_CURRENCY_CODE:
            strcpy(szValue, "Txn CurrCode");
            break;

        case TAG_5F34_PAN_IDENTFY_NO:
            strcpy(szValue, "PAN Seq Num");
            break;

        case TAG_82_AIP:
            strcpy(szValue, "AIP");
            break;

        case TAG_8A_AUTH_CODE:
            strcpy(szValue, "Auth Code");
            break;

        case TAG_8F:
            strcpy(szValue, "CAPK Index");
            break;

        case TAG_91_ARPC:
            strcpy(szValue, "ARPC");
            break;

        case TAG_95:
            strcpy(szValue, "TVR");
            break;

        case TAG_9A_TRANS_DATE:
            strcpy(szValue, "Txn Date");
            break;

        case TAG_9B:
            strcpy(szValue, "TSI");
            break;

        case TAG_9C_TRANS_TYPE:
            strcpy(szValue, "Txn Type");
            break;

        case TAG_9F02_AUTH_AMOUNT:
            strcpy(szValue, "Txn Amt");
            break;

        case TAG_9F03_OTHER_AMOUNT:
            strcpy(szValue, "Other Amt");
            break;

        case TAG_9F10_IAP:
            strcpy(szValue, "IAP");
            break;

        case TAG_9F12:
            strcpy(szValue, "Appl Pref Name");
            break;

        case TAG_9F1A_TERM_COUNTRY_CODE:
            strcpy(szValue, "Term CurrCode");
            break;

        case TAG_9F1B_TERM_FLOOR_LIMIT:
            strcpy(szValue, "FloorLimit");
            break;

        case TAG_9F26_EMV_AC:
            strcpy(szValue, "AC");
            break;

        case TAG_9F27:
            strcpy(szValue, "CID");
            break;

        case TAG_9F33_TERM_CAB:
            strcpy(szValue, "Term Cab");
            break;

        case TAG_9F34_CVM:
            strcpy(szValue, "CVM");
            break;

        case TAG_9F36_ATC:
            strcpy(szValue, "ATC");
            break;

        case TAG_9F37_UNPREDICT_NUM:
            strcpy(szValue, "Unpredict Num");
            break;

        case TAG_9F5B:
            strcpy(szValue, "Script Result");
            break;

        case TAG_71:
            strcpy(szValue, "Issuer Script1");
            break;

        case TAG_72:
            strcpy(szValue, "Issuer Script2");
            break;

        default :
            szValue[0] = 0x00;
            break;
    }
}

void vdPrintEMVTags(void)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    USHORT usPrintTag;
    USHORT usPrintTagLen;
    USHORT usOffset;
    char szStr[d_LINE_SIZE + 1];
    char szTagDesp[50];
    char szEMVTagList[512];
    char szEMVTagListHex[256];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;

	vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
    if( printCheckPaper()==-1)
    	return ;
    CTOS_LanguagePrinterFontSize(d_FONT_16x16, 0, TRUE);	
    vdPrintTitleCenter("EMV TAGS DATA");

    memset(szOutEMVData,0x00,sizeof(szOutEMVData));

    usCTOSS_EMV_MultiDataGet(PRINT_EMV_TAGS_LIST, &inTagLen, szOutEMVData);
    DebugAddHEX("PRINT_EMV_TAGS_LIST",szOutEMVData,inTagLen);

    memset(szEMVTagList,0x00,sizeof(szEMVTagList));
    memset(szEMVTagListHex,0x00,sizeof(szEMVTagListHex));
    strcpy(szEMVTagList, PRINT_EMV_TAGS_LIST);
    wub_str_2_hex(szEMVTagList, szEMVTagListHex, strlen(szEMVTagList));

    usOffset = 0;
    while(szEMVTagListHex[usOffset] != 0x00)
    {
        //CTOS_PrinterFline(d_LINE_DOT * 1);
        vdCTOS_PrinterFline(1);
        
        memset(szStr, 0x00, sizeof(szStr));
        if ((szEMVTagListHex[usOffset] & 0x1F) == 0x1F)	// If the least 5 bits of the first byte are set, it is a two byte Tag
    	{
    		usPrintTag = szEMVTagListHex[usOffset];
    		usPrintTag = ((usPrintTag << 8) | szEMVTagListHex[usOffset+1]);
    		usPrintTagLen = 2;		
    	}
    	else
    	{
    		usPrintTag = szEMVTagListHex[usOffset];
            usPrintTag = usPrintTag & 0x00FF;
            usPrintTagLen = 1;
    	}
        memcpy(szStr, &szEMVTagList[usOffset*2], usPrintTagLen*2);
        usOffset += usPrintTagLen;

        memset(szTagDesp,0x00,sizeof(szTagDesp));
        vdCTOSS_GetEMVTagsDescription(usPrintTag, szTagDesp);
        vdDebug_LogPrintf("szStr[%s] usPrintTag[%X] szTagDesp[%s]", szStr, usPrintTag, szTagDesp);  
        vdMyEZLib_Printf("%s (%s)", szStr,szTagDesp);
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;
        usCTOSS_FindTagFromDataPackage(usPrintTag, EMVtagVal, &EMVtagLen, szOutEMVData, inTagLen);
        memset(szStr, ' ', d_LINE_SIZE);
        if(EMVtagLen == 0)
            memcpy(szStr,"No DATA" , 7);
        else    
            wub_hex_2_str(EMVtagVal, szStr, EMVtagLen);
        vdMyEZLib_Printf("Len:%d val:%s", EMVtagLen, szStr);
    }
        
    //CTOS_PrinterFline(d_LINE_DOT * 1);
    vdCTOS_PrinterFline(1);
    vdPrintTitleCenter("*** END OF REPORT ***");
    vdCTOS_PrinterFline(10);
	//CTOS_PrinterFline(d_LINE_DOT * 10); 
	vdCTOSS_PrinterEnd();
  
}

void vdPrintTerminalConfig(void)
{
    int inResult;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ;
    BYTE szStr[50];
    BYTE szBuf[50];
    BYTE baTemp[PAPER_X_SIZE * 64];

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);  

	ushCTOS_PrintHeader(0);

    //CTOS_PrinterFline(d_LINE_DOT * 1);
    vdCTOS_PrinterFline(1);
	
    memset(szStr, 0x00, sizeof(szStr));
    memset(szBuf, 0x00, sizeof(szBuf));
    wub_hex_2_str(strTCT.szInvoiceNo, szBuf, 3);
    sprintf(szStr, "INVOICE NO: %s", szBuf);                        
    memset (baTemp, 0x00, sizeof(baTemp));
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    inPrint(szStr);

    memset(szStr, 0x00, sizeof(szStr));
    sprintf(szStr, "PABX: %s", strTCT.szPabx);                        
    memset (baTemp, 0x00, sizeof(baTemp));
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    inPrint(szStr);

    memset(szStr, 0x00, sizeof(szStr));
    if(strTCT.fTipAllowFlag)
        sprintf(szStr, "TIP ALLOWED: %s", "YES");      
    else
        sprintf(szStr, "TIP ALLOWED: %s", "NO");
	
    memset (baTemp, 0x00, sizeof(baTemp));
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    //inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, fERMTransaction);
    //check host num
    inNumOfHost = inHDTNumRecord();
    
    vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {                
            inCPTRead(inNum);

            //CTOS_PrinterFline(d_LINE_DOT * 1);
            vdCTOS_PrinterFline(1);  
			
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "HOST: %s", strHDT.szHostLabel);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
           inPrint(szStr);

            memset(szStr, 0x00, sizeof(szStr));
            memset(szBuf, 0x00, sizeof(szBuf));
            wub_hex_2_str(strHDT.szTPDU, szBuf, 5);
            sprintf(szStr, "TPDU: %s", szBuf);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);

            memset(szStr, 0x00, sizeof(szStr));
            if(strCPT.inCommunicationMode == ETHERNET_MODE)
            {   
                sprintf(szStr, "COMM TYPE: %s", "ETHERNET");           
            }
            else if(strCPT.inCommunicationMode == DIAL_UP_MODE)
            {
                sprintf(szStr, "COMM TYPE: %s", "DIAL_UP");         
            }       
            else if(strCPT.inCommunicationMode == GPRS_MODE)
            {
                sprintf(szStr, "COMM TYPE: %s", "GPRS");                 
            }
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI NUM : %s", strCPT.szPriTxnPhoneNumber);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC NUM : %s", strCPT.szSecTxnPhoneNumber);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
            inPrint(szStr);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI IP  : %s", strCPT.szPriTxnHostIP);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);
    
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC IP  : %s", strCPT.szSecTxnHostIP);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);

            inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
        
            vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
            for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
            {
                if((inResult = inMMTReadRecord(strHDT.inHostIndex,inLoop)) !=d_OK)
                {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", inLoop,strHDT.inHostIndex,inResult);
                    continue;
                    //break;
                }
                else 
                {
                    if(strMMT[0].fMMTEnable)
                    {                        
                        memset(szStr, 0x00, sizeof(szStr));
                        #ifdef SINGE_HEADER_FOOTER_TEXT
						    inNMTReadRecord(inLoop);
                            sprintf(szStr, "MERCHANT:%s", strSingleNMT.szMerchName);
                        #else
                            sprintf(szStr, "MERCHANT:%s", strMMT[0].szMerchantName);   
                        #endif
						
                        memset (baTemp, 0x00, sizeof(baTemp));
                        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        inPrint(szStr);

                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "TID : %s", strMMT[0].szTID);                        
                        memset (baTemp, 0x00, sizeof(baTemp));
                        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        inPrint(szStr);

                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "MID : %s", strMMT[0].szMID);                        
                        memset (baTemp, 0x00, sizeof(baTemp));
                        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        inPrint(szStr);  

                        memset(szStr, 0x00, sizeof(szStr));
                        memset(szBuf, 0x00, sizeof(szBuf));
                        wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
                        sprintf(szStr, "BATCH NO: %s", szBuf);                        
                        memset (baTemp, 0x00, sizeof(baTemp));
                        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        inPrint(szStr);

                    }                    

                }
            }
        }
        else
            continue;

    }

    //CTOS_PrinterFline(d_LINE_DOT * 5);
    vdCTOS_PrinterFline(5);
	
    return ;
}

void printCardHolderName(void)
{
    USHORT shLen;
    char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];

	vdTrimSpaces(srTransRec.szCardholderName);
    
    shLen = strlen(srTransRec.szCardholderName);

    if(shLen > 0)
    {
        memset(szStr, 0x00, d_LINE_SIZE);
		if (shLen > 30) 	
			memcpy(szStr, srTransRec.szCardholderName, CARD_HOLD_NAME_DIGITS); 
		else		
		    memcpy(szStr, srTransRec.szCardholderName, shLen); 
			//strcpy(szStr, srTransRec.szCardholderName);

        //while(0x20 == szStr[--shLen] && shLen >= 0)
        //    szStr[shLen] = 0x00;

    	//vdPrintCenter(szStr);	
    	
		memset (baTemp, 0x00, sizeof(baTemp));  
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
		inPrint(szStr);
    }
}

void vdTrimSpaces(char* pchString) {
    int inIndex;

    while (1) {
        inIndex = strlen(pchString);
        if (inIndex) {
            if (pchString[inIndex - 1] == IS_SPACE) {
                pchString[inIndex - 1] = IS_NULL_CH;
                continue;
            }
        }
        break;
    }
}


USHORT ushCTOS_PrintDemo(void)
{
    BYTE strIn[40];
    unsigned char tucPrint [24*4+1];
    short i,spacestring;
    USHORT usCharPerLine = 16;
    BYTE baTemp[PAPER_X_SIZE * 64];

    vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);

    memset(strIn, 0x00, sizeof(strIn));
    strcpy(strIn, "DEMO");
    i = strlen(strIn);
    spacestring=(usCharPerLine-i)/2;
                
    memset(tucPrint,0x20,30);
    
    memcpy(tucPrint+spacestring,strIn,usCharPerLine);   
    
    tucPrint[i+spacestring]=0;
// Removed strTCT.fSMReceipt implementation
#if 0

	if (strTCT.fSMReceipt == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	else
#endif
	    vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
	//CTOS_PrinterFline(d_LINE_DOT * 1);
    vdCTOS_PrinterFline(1);
	
    memset (baTemp, 0x00, sizeof(baTemp));  
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    if((d_FONT_24x24 == stgFONT_ATTRIB.FontSize) || (d_FONT_12x24 == stgFONT_ATTRIB.FontSize)
		&& DOUBLE_SIZE == stgFONT_ATTRIB.Y_Zoom)
		//inCCTOS_PrinterBufferOutput(tucPrint, &stgFONT_ATTRIB, 1);
		inCCTOS_PrinterBufferOutput(tucPrint, &stgFONT_ATTRIB, fERMTransaction);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4); 
	else
		//inCCTOS_PrinterBufferOutput(tucPrint, &stgFONT_ATTRIB, 1);
		inCCTOS_PrinterBufferOutput(tucPrint, &stgFONT_ATTRIB, fERMTransaction);
	    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 

	//CTOS_PrinterFline(d_LINE_DOT * 1);
	vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    return d_OK;
    
}



USHORT ushCTOS_PrintHeader_V9(int page)
{	
    char szBmpfile[50+1];
	memset(szBmpfile, 0x00, sizeof(szBmpfile));
	
    
	//print Logo	
	if(strTCT.fSMLogo == TRUE)
	{
		sprintf(szBmpfile,"%s%s", LOCAL_PATH, strTCT.szSMReceiptLogo);
		vdCTOSS_PrinterBMPPicEx(0,0,szBmpfile);
		//vdCTOSS_PrinterBMPPic(0, 0, strTCT.szSMReceiptLogo);
	}
	else	
	{
		sprintf(szBmpfile,"%s%s", LOCAL_PATH, strTCT.szBDOReceiptLogo);
		vdCTOSS_PrinterBMPPicEx(0,0,szBmpfile); 
	    //vdCTOSS_PrinterBMPPic(0, 0, strTCT.szBDOReceiptLogo);
	}
	if(fRePrintFlag == TRUE)
    {   
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
		if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
        {
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctHdr1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctHdr1);
    }
    
	//merhcant name and address
	if(strlen(strMMT[0].szRctHdr1) > 0)
		vdPrintCenter(strMMT[0].szRctHdr1);
	if(strlen(strMMT[0].szRctHdr2) > 0)
		vdPrintCenter(strMMT[0].szRctHdr2);
	if(strlen(strMMT[0].szRctHdr3) > 0)
    	vdPrintCenter(strMMT[0].szRctHdr3);
	if(strlen(strMMT[0].szRctHdr4) > 0)
    	vdPrintCenter(strMMT[0].szRctHdr4);
	if(strlen(strMMT[0].szRctHdr5) > 0)
    	vdPrintCenter(strMMT[0].szRctHdr5);

	//issue-00420
	if (strlen(strTCT.szAppVersionHeader) > 0)
		vdPrintCenter(strTCT.szAppVersionHeader);


    if(VS_TRUE == strTCT.fDemo)
    {
        ushCTOS_PrintDemo();
    }

    if(fRePrintFlag == TRUE)
    {
        vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
        vdPrintCenter("DUPLICATE");
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
    
    
	return d_OK;
	
}

USHORT ushCTOS_PrintHeader(int page)
{	
	char szBmpfile[50+1];
	memset(szBmpfile, 0x00, sizeof(szBmpfile));
    
	//print Logo	
    if(strTCT.fSMLogo == TRUE)
    {
		sprintf(szBmpfile,"%s%s", LOCAL_PATH, strTCT.szSMReceiptLogo);
		//vdCTOSS_PrinterBMPPicEx(0,0,szBmpfile);
        vdCTOSS_PrinterBMPPic(0, 0, szBmpfile);
    }
	else
	{
		sprintf(szBmpfile,"%s%s", LOCAL_PATH,  strTCT.szBDOReceiptLogo);
		//vdCTOSS_PrinterBMPPicEx(0,0,szBmpfile);
        vdCTOSS_PrinterBMPPic(0, 0, szBmpfile);
	}
	if(fRePrintFlag == TRUE)
    {   
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
		if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
        {
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctHdr1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctHdr1);
    }
  //aaronnino for BDOCLG ver 9.0 fix on issue #00085 All Header lines are not printing on the first host info report start
	if(strlen(strMMT[0].szRctHdr1) <= 0)
		 inMMTReadRecord(1,1);
	//aaronnino for BDOCLG ver 9.0 fix on issue #00085 All Header lines are not printing on the first host info report end
	
	//merhcant name and address
    vdPrintHeaderText();

	//issue-00420
	if (strlen(strTCT.szAppVersionHeader) > 0)
		vdPrintCenter(strTCT.szAppVersionHeader);

    if(fRePrintFlag == TRUE)
    {
        ///vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        //vdPrintCenterReverseMode("DUPLICATE");
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        vdPrintCenter("DUPLICATE");
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    }

    if(VS_TRUE == strTCT.fDemo)
    {
        ushCTOS_PrintDemo();
    }

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    
    
	return d_OK;
	
}


USHORT ushCTOS_PrintBody_V9(int page)
{	
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;

	char szAmountBuff[20], szTemp6[47];
	int inPADSize,x;
	char szSPACE[40];

	char szPOSEntry[21] = {0}; /* BDOCLG-00134: Reformat cardtype line -- jzg */


	if(d_FIRST_PAGE == page)
	{
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
        printTIDMID(); 

        //card type
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		memset(szStr, ' ', d_LINE_SIZE);
		vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);

		/* BDOCLG-00134: Reformat cardtype line - start -- jzg */
		switch(srTransRec.byEntryMode)
		{
			case CARD_ENTRY_MSR:
				strcat(szPOSEntry, "SWIPE");
				break;
			case CARD_ENTRY_MANUAL:
				strcat(szPOSEntry, "MANUAL");
				break;
			case CARD_ENTRY_ICC:
				strcat(szPOSEntry, "CHIP");	 
				break;
			case CARD_ENTRY_FALLBACK:
				strcat(szPOSEntry, "FALLBACK");	 
			//add ctls for paypass
			case CARD_ENTRY_WAVE:
				strcat(szPOSEntry, "CONTACTLESS");	 
				break;
		}
		//issue-255- remove entry mode in Card label line
		//vdStrCat2(srTransRec.szCardLable, szPOSEntry, 21, szStr);
		strcpy(szStr,srTransRec.szCardLable);
		/* BDOCLG-00134: Reformat cardtype line - start -- jzg */
		
		vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);

		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
        inPrint(szStr);

		szGetTransTitle(srTransRec.byTransType, szStr);     

		//pan
		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		memset(szTemp5, 0x00, sizeof(szTemp5));

		//BDO: Enable PAN Masking for each receipt - start -- jzg
#if 0
		strcpy(szTemp5, srTransRec.szPAN);
		cardMasking(szTemp5, PRINT_CARD_MASKING_3);
#else
		//aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 4 of 12
		if(strTCT.inCustomerCopy == 1)
		   vdCTOS_FormatPAN2(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp5);
		if(strTCT.inMerchantCopy == 1)
		   vdCTOS_FormatPAN2(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp5);
		if(strTCT.inBankCopy == 1)
		   vdCTOS_FormatPAN2(strIIT.szMaskBankCopy, srTransRec.szPAN, szTemp5);
		//aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 4 of 12
#endif
		//BDO: Enable PAN Masking for each receipt - end -- jzg

		memset(szTemp6, 0x00, sizeof(szTemp6));
		memset(szTemp4, 0x00, sizeof(szTemp4));
		
		if (strlen(srTransRec.szPAN) > 16)
			vdRemoveSpace(szTemp6, szTemp5);		
		else
			strcpy(szTemp6, szTemp5);
				
		strcpy(szTemp4, szTemp6);
					
		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);	
		//issue-00255 - Align PAN and Entry mode - normal Font size 
		
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
		inPrintLeftRight(szTemp4,szPOSEntry,41);
		//end

        //card holder name
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
		printCardHolderName();

		//Trans type
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		
        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
			vdPrintCenter(szStr);
		else{
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inPrint(szStr);
		}

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
        
		printBatchInvoiceNO();

		printDateTime();


		//BDO: Moved and reformatted approval code line - start -- jzg
		//Reference num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));
		memset(szStr, ' ', d_LINE_SIZE);


		//Approval Code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);

		//aaronnino for BDOCLG ver 9.0 fix on issue #00066 "APPR.CODE" single width printed on transaction receipt 6 of 8 start
		if (strTCT.fDutyFreeMode == TRUE){
		
		   //Reference num
		   sprintf(szStr, "REF. NO. %s", srTransRec.szRRN);
		   memset (baTemp, 0x00, sizeof(baTemp));	   
		   //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		   //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
           inPrint(szStr);
		   
		   vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);  
		   sprintf(szStr, "APPR. CODE: %s", srTransRec.szAuthCode);
		   vdCTOS_Pad_String(szStr, 23, ' ', POSITION_LEFT);
		   memset (baTemp, 0x00, sizeof(baTemp));	   
		   //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		   //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
           inPrint(szStr); 
		   vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
		}else{
			 vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
			 if(strlen(srTransRec.szRRN) <= 0)
				 sprintf(szStr, "REF. NO.				APPR. CODE: %s",srTransRec.szAuthCode);
			 else
				 sprintf(szStr, "REF. NO. %s  APPR. CODE: %s", srTransRec.szRRN,srTransRec.szAuthCode);
			 memset (baTemp, 0x00, sizeof(baTemp)); 	 
			 //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			 //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			 inPrint(szStr);
		
		}
		//BDO: Moved and reformatted approval code line - end -- jzg


		
//gcitra
		if (srTransRec.byTransType == RELOAD){
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));					
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "DEST. CARD : %s", srTransRec.szDestCardNo);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inPrint(szStr);
		}
//gcitra

		
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_OFFLINE))
		{
			//Base amount
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szAmountBuff,0x00,20);
			memset(szTemp4, 0x00, sizeof(szTemp4));
			memset(szTemp6,0x00, sizeof(szTemp6));
			strcpy(szTemp6,"AMOUNT:");
			
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp,szTemp4);
			
			sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
			inPrintLeftRight(szTemp6,szAmountBuff,23);
			
		}
		else if (srTransRec.byTransType == SALE_TIP)
		{

			//Base amount
			memset(szStr, 0x00, d_LINE_SIZE);
			memset(szAmountBuff,0x00,20);
			memset(szTemp4, 0x00, sizeof(szTemp4));
			memset(szTemp6,0x00, sizeof(szTemp6));
			strcpy(szTemp6,"BASE:");	
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp4);
			sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
			inPrintLeftRight(szTemp6,szAmountBuff,23);
            /*********************************************************/
            /*TIP*/
			memset(szStr, 0x00, d_LINE_SIZE);
			memset(szAmountBuff,0x00,20);
			memset(szTemp4, 0x00, sizeof(szTemp4));
			memset(szTemp6,0x00, sizeof(szTemp6));
			strcpy(szTemp6,"TIP:");
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3,szTemp4);
			sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
			inPrintLeftRight(szTemp6,szAmountBuff,23); 
	 		/*********************************************************/
			memset(szStr, 0x00, d_LINE_SIZE);
			memset(szAmountBuff,0x00,20);
			memset(szTemp4, 0x00, sizeof(szTemp4));
			memset(szTemp6,0x00, sizeof(szTemp6));
			strcpy(szTemp6,"TOTAL:");
			
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp,szTemp4);
			sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
			inPrintLeftRight(szTemp6,szAmountBuff,23); 
		
		}
		else
		{
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szAmountBuff,0x00,20);
			memset(szTemp4, 0x00, sizeof(szTemp4));
			memset(szTemp6,0x00, sizeof(szTemp6));
			strcpy(szTemp6,"TOTAL");
			
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp4);
			if (srTransRec.byTransType == VOID)
				sprintf(szAmountBuff,"-%s%s",strCST.szCurSymbol,szTemp4);
			else
				sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
			inPrintLeftRight(szTemp6,szAmountBuff,23); 
			
		}

        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
		
	}
	else if(d_SECOND_PAGE == page)
	{
				vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				printTIDMID(); 
		
				//card type
				memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
				EMVtagLen = 0;
				memset(szStr, ' ', d_LINE_SIZE);
				vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
		
				/* BDOCLG-00134: Reformat cardtype line - start -- jzg */
				switch(srTransRec.byEntryMode)
				{
					case CARD_ENTRY_MSR:
						strcat(szPOSEntry, "SWIPE");
						break;
					case CARD_ENTRY_MANUAL:
						strcat(szPOSEntry, "MANUAL");
						break;
					case CARD_ENTRY_ICC:
						strcat(szPOSEntry, "CHIP");	 
						break;
					case CARD_ENTRY_FALLBACK:
						strcat(szPOSEntry, "FALLBACK");	 						
					//add ctls for paypass
					case CARD_ENTRY_WAVE:
						strcat(szPOSEntry, "CONTACTLESS");	 
					break;
				}
				//issue-255- remove entry mode in Card label line
				//vdStrCat2(srTransRec.szCardLable, szPOSEntry, 21, szStr);
				strcpy(szStr,srTransRec.szCardLable);
				/* BDOCLG-00134: Reformat cardtype line - start -- jzg */
		
				vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
				memset (baTemp, 0x00, sizeof(baTemp));
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
		        inPrint(szStr);
		
				szGetTransTitle(srTransRec.byTransType, szStr); 	
		
				//pan
				vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
				memset(szTemp5, 0x00, sizeof(szTemp5));
		
				//BDO: Enable PAN Masking for each receipt - start -- jzg
#if 0
				strcpy(szTemp5, srTransRec.szPAN);
				cardMasking(szTemp5, PRINT_CARD_MASKING_3);
#else
                //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 5 of 12
                if(strTCT.inMerchantCopy == 2)
                     vdCTOS_FormatPAN2(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp5);
                if(strTCT.inBankCopy == 2)
                     vdCTOS_FormatPAN2(strIIT.szMaskBankCopy, srTransRec.szPAN, szTemp5);
                if(strTCT.inCustomerCopy == 2)
                     vdCTOS_FormatPAN2(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp5);
                //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 5 of 12
#endif
				//BDO: Enable PAN Masking for each receipt - end -- jzg
		
				memset(szTemp6, 0x00, sizeof(szTemp6));
				memset(szTemp4, 0x00, sizeof(szTemp4));
				
				if (strlen(srTransRec.szPAN) > 16)
					vdRemoveSpace(szTemp6, szTemp5);		
				else
					strcpy(szTemp6, szTemp5);
						
				strcpy(szTemp4, szTemp6);
					
				
				//memset (baTemp, 0x00, sizeof(baTemp));
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);	
				//issue-00255 - Align PAN and Entry mode - normal Font size 
				
				vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				inPrintLeftRight(szTemp4,szPOSEntry,41);
		
				//card holder name
				vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				printCardHolderName();
		
				//Trans type
				vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
				
				if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
					vdPrintCenter(szStr);
				else{
					memset (baTemp, 0x00, sizeof(baTemp));
					//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					inPrint(szStr);
				}
		
				vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				
				printBatchInvoiceNO();
		
				printDateTime();
		
				//BDO: Moved and reformatted approval code line - start -- jzg
				//Reference num
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				memset(szStr, ' ', d_LINE_SIZE);


				//Approval Code
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));					
				memset(szStr, ' ', d_LINE_SIZE);

				//aaronnino for BDOCLG ver 9.0 fix on issue #00066 "APPR.CODE" single width printed on transaction receipt 6 of 8 start
				if (strTCT.fDutyFreeMode == TRUE){
				
				   //Reference num
				   sprintf(szStr, "REF. NO. %s", srTransRec.szRRN);
				   memset (baTemp, 0x00, sizeof(baTemp));	   
				   //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				   //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                   inPrint(szStr);
				   
				   vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);  
				   sprintf(szStr, "APPR. CODE: %s", srTransRec.szAuthCode);
				   vdCTOS_Pad_String(szStr, 23, ' ', POSITION_LEFT);
				   memset (baTemp, 0x00, sizeof(baTemp));	   
				   //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				   //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                   inPrint(szStr);  
				   vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				}else{
					 vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
					 if(strlen(srTransRec.szRRN) <= 0)
						 sprintf(szStr, "REF. NO.				APPR. CODE: %s",srTransRec.szAuthCode);
					 else
						 sprintf(szStr, "REF. NO. %s  APPR. CODE: %s", srTransRec.szRRN,srTransRec.szAuthCode);
					 memset (baTemp, 0x00, sizeof(baTemp)); 	 
					 //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					 //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					 inPrint(szStr);
				
				}
				//BDO: Moved and reformatted approval code line - end -- jzg
		
				
		//gcitra
				if (srTransRec.byTransType == RELOAD){
					memset(szStr, ' ', d_LINE_SIZE);
					memset (baTemp, 0x00, sizeof(baTemp));					
					memset(szStr, ' ', d_LINE_SIZE);
					sprintf(szStr, "DEST. CARD : %s", srTransRec.szDestCardNo);
					memset (baTemp, 0x00, sizeof(baTemp));		
					//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					inPrint(szStr);
				}
		//gcitra
		
				
				
				memset(szStr, ' ', d_LINE_SIZE);
				memset(szTemp, ' ', d_LINE_SIZE);
				memset(szTemp1, ' ', d_LINE_SIZE);
				memset(szTemp3, ' ', d_LINE_SIZE);
				wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
				wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
		
				vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
				if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_OFFLINE))
				{
					//Base amount
					memset(szStr, ' ', d_LINE_SIZE);
					memset(szAmountBuff,0x00,20);
					memset(szTemp4, 0x00, sizeof(szTemp4));
					memset(szTemp6,0x00, sizeof(szTemp6));
					strcpy(szTemp6,"AMOUNT:");
					
					vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp,szTemp4);
					
					sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
					inPrintLeftRight(szTemp6,szAmountBuff,23);
						
					}
					else if (srTransRec.byTransType == SALE_TIP)
					{
					
						//Base amount
						memset(szStr, 0x00, d_LINE_SIZE);
						memset(szAmountBuff,0x00,20);
						memset(szTemp4, 0x00, sizeof(szTemp4));
						memset(szTemp6,0x00, sizeof(szTemp6));
						strcpy(szTemp6,"BASE:");	
						vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp4);
						sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
						inPrintLeftRight(szTemp6,szAmountBuff,23);
						/*********************************************************/
						/*TIP*/
						memset(szStr, 0x00, d_LINE_SIZE);
						memset(szAmountBuff,0x00,20);
						memset(szTemp4, 0x00, sizeof(szTemp4));
						memset(szTemp6,0x00, sizeof(szTemp6));
						strcpy(szTemp6,"TIP:");
						vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3,szTemp4);
						sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
						inPrintLeftRight(szTemp6,szAmountBuff,23); 
						/*********************************************************/
						memset(szStr, 0x00, d_LINE_SIZE);
						memset(szAmountBuff,0x00,20);
						memset(szTemp4, 0x00, sizeof(szTemp4));
						memset(szTemp6,0x00, sizeof(szTemp6));
						strcpy(szTemp6,"TOTAL:");
						
						vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp,szTemp4);
						sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
						inPrintLeftRight(szTemp6,szAmountBuff,23); 
				
				}
				else
				{
					memset(szStr, ' ', d_LINE_SIZE);
					memset(szAmountBuff,0x00,20);
					memset(szTemp4, 0x00, sizeof(szTemp4));
					memset(szTemp6,0x00, sizeof(szTemp6));
					strcpy(szTemp6,"TOTAL");
					
					vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp4);
					if (srTransRec.byTransType == VOID)
						sprintf(szAmountBuff,"-%s%s",strCST.szCurSymbol,szTemp4);
					else
						sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
					inPrintLeftRight(szTemp6,szAmountBuff,23); 
					
				}
		
				vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
	}

	else if(d_THIRD_PAGE == page)
	{
				vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				printTIDMID(); 
		
				//card type
				memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
				EMVtagLen = 0;
				memset(szStr, ' ', d_LINE_SIZE);
				vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
		
				/* BDOCLG-00134: Reformat cardtype line - start -- jzg */
				switch(srTransRec.byEntryMode)
				{
					case CARD_ENTRY_MSR:
						strcat(szPOSEntry, "SWIPE");
						break;
					case CARD_ENTRY_MANUAL:
						strcat(szPOSEntry, "MANUAL");
						break;
					case CARD_ENTRY_ICC:
						strcat(szPOSEntry, "CHIP");	 
						break;
					case CARD_ENTRY_FALLBACK:
						strcat(szPOSEntry, "FALLBACK");
					//add ctls for paypass
					case CARD_ENTRY_WAVE:
						strcat(szPOSEntry, "CONTACTLESS");	 
						break;
				}
				//issue-255- remove entry mode in Card label line
				//vdStrCat2(srTransRec.szCardLable, szPOSEntry, 21, szStr);
				strcpy(szStr,srTransRec.szCardLable);
				/* BDOCLG-00134: Reformat cardtype line - start -- jzg */
		
				vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
				memset (baTemp, 0x00, sizeof(baTemp));
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
		        inPrint(szStr);
		
				szGetTransTitle(srTransRec.byTransType, szStr); 	
		
				//pan
				vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
				memset(szTemp5, 0x00, sizeof(szTemp5));
			
					//BDO: Enable PAN Masking for each receipt - start -- jzg
#if 0
					strcpy(szTemp5, srTransRec.szPAN);
					cardMasking(szTemp5, PRINT_CARD_MASKING_3);
#else
					//aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 6 of 12
                 if(strTCT.inCustomerCopy == 3)
				 	  vdCTOS_FormatPAN2(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp5);
				 if(strTCT.inBankCopy == 3)
				 	  vdCTOS_FormatPAN2(strIIT.szMaskBankCopy, srTransRec.szPAN, szTemp5);
				 if(strTCT.inMerchantCopy == 3)
					  vdCTOS_FormatPAN2(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp5);
				  //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 6 of 12
#endif
					//BDO: Enable PAN Masking for each receipt - end -- jzg
			
				memset(szTemp6, 0x00, sizeof(szTemp6));
				memset(szTemp4, 0x00, sizeof(szTemp4));
				
				if (strlen(srTransRec.szPAN) > 16)
					vdRemoveSpace(szTemp6, szTemp5);		
				else
					strcpy(szTemp6, szTemp5);
						
				strcpy(szTemp4, szTemp6);
					
				
				//memset (baTemp, 0x00, sizeof(baTemp));
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);	
				//issue-00255 - Align PAN and Entry mode - normal Font size 
				vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				inPrintLeftRight(szTemp4,szPOSEntry,41);
		
				//card holder name
				vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				printCardHolderName();
		
				//Trans type
				vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
				
				if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
					vdPrintCenter(szStr);
				else{
					memset (baTemp, 0x00, sizeof(baTemp));
					//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					inPrint(szStr);
				}
		
				vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				
				printBatchInvoiceNO();
		
				printDateTime();
		
				//BDO: Moved and reformatted approval code line - start -- jzg
				//Reference num
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				memset(szStr, ' ', d_LINE_SIZE);

				//Approval Code
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));					
				memset(szStr, ' ', d_LINE_SIZE);

				//aaronnino for BDOCLG ver 9.0 fix on issue #00066 "APPR.CODE" single width printed on transaction receipt 6 of 8 start
				if (strTCT.fDutyFreeMode == TRUE){
				
				   //Reference num
				   sprintf(szStr, "REF. NO. %s", srTransRec.szRRN);
				   memset (baTemp, 0x00, sizeof(baTemp));	   
				   //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				   //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                   inPrint(szStr); 
				   
				   vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);  
				   sprintf(szStr, "APPR. CODE: %s", srTransRec.szAuthCode);
				   vdCTOS_Pad_String(szStr, 23, ' ', POSITION_LEFT);
				   memset (baTemp, 0x00, sizeof(baTemp));	   
				   //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				   //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                   inPrint(szStr);  
				   vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
				}else{
					 vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
					 if(strlen(srTransRec.szRRN) <= 0)
						 sprintf(szStr, "REF. NO.				APPR. CODE: %s",srTransRec.szAuthCode);
					 else
						 sprintf(szStr, "REF. NO. %s  APPR. CODE: %s", srTransRec.szRRN,srTransRec.szAuthCode);
					 memset (baTemp, 0x00, sizeof(baTemp)); 	 
					 //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					 //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				     inPrint(szStr);

				}
				//BDO: Moved and reformatted approval code line - end -- jzg
		
				
		//gcitra
				if (srTransRec.byTransType == RELOAD){
					memset(szStr, ' ', d_LINE_SIZE);
					memset (baTemp, 0x00, sizeof(baTemp));					
					memset(szStr, ' ', d_LINE_SIZE);
					sprintf(szStr, "DEST. CARD : %s", srTransRec.szDestCardNo);
					memset (baTemp, 0x00, sizeof(baTemp));		
					//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    inPrint(szStr);
				}
		//gcitra
		
				
				
				memset(szStr, ' ', d_LINE_SIZE);
				memset(szTemp, ' ', d_LINE_SIZE);
				memset(szTemp1, ' ', d_LINE_SIZE);
				memset(szTemp3, ' ', d_LINE_SIZE);
				wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
				wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
		
				vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
				if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_OFFLINE))
				{
					//Base amount
					memset(szStr, ' ', d_LINE_SIZE);
					memset(szAmountBuff,0x00,20);
					memset(szTemp4, 0x00, sizeof(szTemp4));
					memset(szTemp6,0x00, sizeof(szTemp6));
					strcpy(szTemp6,"AMOUNT:");
					
					vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp,szTemp4);
					
					sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
					inPrintLeftRight(szTemp6,szAmountBuff,23);
						
					}
					else if (srTransRec.byTransType == SALE_TIP)
					{
					
						//Base amount
						memset(szStr, 0x00, d_LINE_SIZE);
						memset(szAmountBuff,0x00,20);
						memset(szTemp4, 0x00, sizeof(szTemp4));
						memset(szTemp6,0x00, sizeof(szTemp6));
						strcpy(szTemp6,"BASE:");	
						vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp4);
						sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
						inPrintLeftRight(szTemp6,szAmountBuff,23);
						/*********************************************************/
						/*TIP*/
						memset(szStr, 0x00, d_LINE_SIZE);
						memset(szAmountBuff,0x00,20);
						memset(szTemp4, 0x00, sizeof(szTemp4));
						memset(szTemp6,0x00, sizeof(szTemp6));
						strcpy(szTemp6,"TIP:");
						vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3,szTemp4);
						sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
						inPrintLeftRight(szTemp6,szAmountBuff,23); 
						/*********************************************************/
						memset(szStr, 0x00, d_LINE_SIZE);
						memset(szAmountBuff,0x00,20);
						memset(szTemp4, 0x00, sizeof(szTemp4));
						memset(szTemp6,0x00, sizeof(szTemp6));
						strcpy(szTemp6,"TOTAL:");
						
						vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp,szTemp4);
						sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
						inPrintLeftRight(szTemp6,szAmountBuff,23); 
				
				}
				else
				{
					memset(szStr, ' ', d_LINE_SIZE);
					memset(szAmountBuff,0x00,20);
					memset(szTemp4, 0x00, sizeof(szTemp4));
					memset(szTemp6,0x00, sizeof(szTemp6));
					strcpy(szTemp6,"TOTAL");
					
					vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp4);
					if (srTransRec.byTransType == VOID)
						sprintf(szAmountBuff,"-%s%s",strCST.szCurSymbol,szTemp4);
					else
						sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
					inPrintLeftRight(szTemp6,szAmountBuff,23); 
					
				}
		
				vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
	}

	return d_OK;;	
	
}

USHORT ushCTOS_PrintBody(int page)
{	
    char szStr[d_LINE_SIZE + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szAmountBuff[20], szTemp6[47];
    int inPADSize,x;
    char szSPACE[40];
    char szPOSEntry[21] = {0}; /* BDOCLG-00134: Reformat cardtype line -- jzg */

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    /* tid mid */
    printTIDMID(); 
    
    /* batch invoice no. */
	
    if(strFLG.fLargeInvoice)
   		printBatchInvoiceNOEx();
	else
    	printBatchInvoiceNO();
    
    /* card no. */
    memset(szTemp5, 0x00, sizeof(szTemp5));
    //BDO: Enable PAN Masking for each receipt - start -- jzg
    //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 4 of 21

    if(page == BANK_COPY_RECEIPT) /*bank copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskBankCopy, srTransRec.szPAN, szTemp5);
    else if(page == CUSTOMER_COPY_RECEIPT) /*customer copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp5);	
    else if(page == MERCHANT_COPY_RECEIPT) /*merchant copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp5);	
    //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 4 of 21
    //BDO: Enable PAN Masking for each receipt - end -- jzg
    
    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 1, 0);
    memset(szTemp6, 0x00, sizeof(szTemp6));
    memset(szStr, 0x00, sizeof(szStr));
    
    if (strlen(srTransRec.szPAN) > 16)
    vdRemoveSpace(szStr, szTemp5);		
    else
    strcpy(szStr, szTemp5);
    
	inPrint(szStr);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

	if(strTCT.fSMMode && strTCT.fPrintIssuerID)
	{
		memset(szStr, 0x00, d_LINE_SIZE);
		sprintf(szStr, "ISSUER ID: %02d", strHDT.inIssuerID);
		inPrint(szStr);
	}
		
	if(srTransRec.fBINVer)
	{
		memset(szStr, 0x00, d_LINE_SIZE);
		sprintf(szStr, "BIN CHECK    : %06ld", atol(srTransRec.szBINVerSTAN));
		inPrint(szStr);
	}
    
    /* card holder name */
    printCardHolderName();
    
    /*Trans type*/
    memset(szStr, 0x00, sizeof(szStr));
    if (srTransRec.byTransType == PRE_AUTH)
        strcpy(szStr,"CARD VERIFY");
    else if (srTransRec.byTransType == CASH_ADVANCE)
        strcpy(szStr,"CASH ADVANCE");
    else if (srTransRec.byTransType == SALE_OFFLINE)
        strcpy(szStr,"COMPLETION");
    else if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType == SALE_OFFLINE)
        strcpy(szStr,"VOID COMPLETION");
    else if (srTransRec.byTransType == SALE)
        strcpy(szStr,"DEBIT");
    else
        szGetTransTitle(srTransRec.byTransType, szStr); 
    
    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    if ((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_WAVE))
        vdPrintCenter("CONTACTLESS SALE");
    else if ((srTransRec.byTransType == VOID) && (srTransRec.byEntryMode == CARD_ENTRY_WAVE))
        vdPrintCenter("CONTACTLESS VOID");
    else
    {
       /* memset (baTemp, 0x00, sizeof(baTemp));
        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);*/
     vdPrintCenter(szStr);   
    }
    
    /* date time entry mode */
    switch(srTransRec.byEntryMode)
    {
        case CARD_ENTRY_MSR:
            strcat(szPOSEntry, "S");
        break;
		
        case CARD_ENTRY_MANUAL:
            strcat(szPOSEntry, "M");
        break;
		
        case CARD_ENTRY_ICC:
            strcat(szPOSEntry, "C");   
        break;
		
        case CARD_ENTRY_FALLBACK:			
			if((srTransRec.szServiceCode[0] == '2') || (srTransRec.szServiceCode[0] == '6'))
            	strcat(szPOSEntry, "F");  
			else			
            	strcat(szPOSEntry, "S");
        break;
    }
    printDateTimeAndOther(szPOSEntry, 23);
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    
    /* card type approval code */
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp3, 0x00, d_LINE_SIZE);
/*
	if( srTransRec.byEntryMode == CARD_ENTRY_ICC || strcmp("SMGUARNTR",srTransRec.szCardLable) == 0 || 
		strcmp("SMADVNTGE",srTransRec.szCardLable) == 0 || strcmp("SHOP CARD",srTransRec.szCardLable) == 0|| 
		strcmp("FLEET",srTransRec.szCardLable) == 0 || strcmp("CASHCARD",srTransRec.szCardLable) == 0 || 
		strcmp("BDOREWARDS",srTransRec.szCardLable) == 0 )
		strcpy(szTemp3,srTransRec.szCardLable);
	else if(srTransRec.inCardType == DEBIT_CARD)
		strcpy(szTemp3,"DEBIT");
	else if (srTransRec.inCardType == CREDIT_CARD)	
		strcpy(szTemp3,"CREDIT");
	else
		strcpy(szTemp3,srTransRec.szCardLable);
*/	

	vdDebug_LogPrintf("TEST %s", srTransRec.szCardLable);


    strcpy(szTemp3,srTransRec.szCardLable);

    if(strTCT.fDutyFreeMode)
    {
		//sprintf(szTemp,"CARD TYPE: DEBIT");
		
		sprintf(szTemp,"CARD TYPE: %s", szTemp3);

		if(strTCT.fSMMode == TRUE)
	    	sprintf(szTemp1,"APP CODE: %s", "APPRVD");
		else
	    	sprintf(szTemp1,"APP CODE: %s", strlen(srTransRec.szAuthCode)?srTransRec.szAuthCode:"APPRVD");
		
		inPrint(szTemp);
		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		//inPrintLeftRight("",szTemp1,23);
		inPrint(szTemp1);
	}
	else
	{
		if(strTCT.fSMMode == TRUE)
			sprintf(szTemp,"CARD TYPE:%-12.12sAPP CODE: %s", szTemp3, "APPRVD");
	    else
			sprintf(szTemp,"CARD TYPE:%-12.12sAPP CODE: %s", szTemp3,strlen(srTransRec.szAuthCode)?srTransRec.szAuthCode:"APPRVD");
		
		inPrint(szTemp);
	}
	
    /*Reference num*/
	memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szTemp1, 0x00, d_LINE_SIZE);
	
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
    /* EMV: Revised EMV details printing - start -- jzg */
    (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
    (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
    (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
    (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_2) ||
    (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC))
    /* EMV: Revised EMV details printing - end -- jzg */
    {
		sprintf(szTemp,"RRN: %-17.17sAPP:%s", srTransRec.szRRN,srTransRec.stEMVinfo.szChipLabel);
		//sprintf(szTemp,"RRN: %-17.17sAPP:%s", srTransRec.szRRN,srTransRec.szCardLable);
        //sprintf(szTemp1,"APP: %s", srTransRec.szCardLable);
		//inPrintLeftRight(szTemp,szTemp1,42);
    }
    else	// v10.0 change - depends on srTransRec.byEntryMode
    {
        sprintf(szTemp, "RRN: %-17.17s", srTransRec.szRRN);
		//inPrint(szStr);
    }

    inPrint(szTemp);
    
    /* AID TC */
    memset(szStr, ' ', sizeof(szStr));
    memset(szTemp, ' ', sizeof(szTemp));
    memset(szTemp1, ' ', sizeof(szTemp1));
    memset(szTemp3, ' ', sizeof(szTemp3));
    memset(szTemp4, ' ', sizeof(szTemp4));
    if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
    /* EMV: Revised EMV details printing - start -- jzg */
    (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
    (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
    (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
    (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_2) ||
    (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC))
    /* EMV: Revised EMV details printing - end -- jzg */
    {
        //AID
        EMVtagLen = srTransRec.stEMVinfo.T84_len;
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
        wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
        sprintf(szTemp1, "AID: %s", szTemp);
        
        //TC
        if (srTransRec.byTransType != SALE_OFFLINE && srTransRec.byOrgTransType!= SALE_OFFLINE)
            wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp3, 8);
		
		sprintf(szStr, "AID: %-16.16s TC:%-16.16s", szTemp, szTemp3);
				
		memset (baTemp, 0x00, sizeof(baTemp));		
        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inPrint(szStr);
    }		  

	if (strTCT.fSMReceipt == 0)//version 11 - do not put space for SM receipts
		vdCTOS_PrinterFline(1); // Change Request. Added new line before printing amount.
	
    /* Amount */
    memset(szTemp1, 0x00, d_LINE_SIZE);
    wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    
    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    
    if(srTransRec.byTransType == SALE)
    {
        //Base amount
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szAmountBuff,0x00,20);
        memset(szTemp4, 0x00, sizeof(szTemp4));
        
        vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp4);// patrick add code 20141216		
        sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
        inPrintLeftRight("TOTAL:",szAmountBuff,23);		
    }
	
    return d_OK;	
}

USHORT ushCTOS_PrintAgreement()
{
// Removed strTCT.fSMReceipt implementation
#if 0

	if (strTCT.fSMReceipt == 0)
	{
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
		vdPrintCenter("I AGREE TO PAY THE ABOVE TOTAL AMOUNT");
		vdPrintCenter("ACCORDING TO THE CARD ISSUER AGREEMENT");
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	}
#else
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	vdPrintCenter("I AGREE TO PAY THE ABOVE TOTAL AMOUNT");
	vdPrintCenter("ACCORDING TO THE CARD ISSUER AGREEMENT");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
#endif

}

USHORT ushCTOS_PrintFooter_V9(int page)
{		
	BYTE EMVtagVal[64] = {0};
	USHORT EMVtagLen = 0; 
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 3];



	if(page == d_FIRST_PAGE)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;

		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  

		vdDebug_LogPrintf("CVM [%02X %02X %02X]",
			srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
		vdDebug_LogPrintf("srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);

		if ((srTransRec.byEntryMode == CARD_ENTRY_ICC))
		{
			EMVtagLen = 3;
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

			//EMV: Fix for noCVM during above floor limit scenario - start -- jzg
			vdDebug_LogPrintf("EMVtagVal [%02X %02X %02X]", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
			
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				(EMVtagVal[0] != 0x1E) && (EMVtagVal[0] != 0x5E)) || 
				(EMVtagVal[0] == 0x3F))
			//EMV: Fix for noCVM during above floor limit scenario - end -- jzg
			{
				//CTOS_PrinterFline(d_LINE_DOT * 1);
                vdCTOS_PrinterFline(1);
				CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
					CTOS_PrinterPutString("      (PIN VERIFY SUCCESS)");
			}
			else
			{
				//CTOS_PrinterFline(d_LINE_DOT );
				vdCTOS_PrinterFline(1);
			}
		}
		else if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		{
			vdDebug_LogPrintf("srTransRec.bWaveSCVMAnalysis = [%d]", srTransRec.bWaveSCVMAnalysis);
			//if(stRCDataAnalyze.bCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
			if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
			{
				if (srTransRec.bWaveSID == d_VW_SID_AE_EMV && srTransRec.bWaveSCVMAnalysis == d_CVM_REQUIRED_NONE)
				{
					if ((strTCT.inMerchantCopy == 1) || (strTCT.inBankCopy == 1))
				 	{
					  //CTOS_PrinterFline(d_LINE_DOT * 1);
					  vdCTOS_PrinterFline(1);
					  CTOS_PrinterPutString("SIGN:__________________________");
				 	}
				 	else
						vdCTOS_PrinterFline(1);
				 	  	//CTOS_PrinterFline(d_LINE_DOT);
				}
				else
				{
					vdCTOS_PrinterFline(1); 
                    //CTOS_PrinterFline(d_LINE_DOT * 1);
                    CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
                    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
                        CTOS_PrinterPutString("      (PIN VERIFY SUCCESS)");
				}
			}
			else
			{
			   //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 8 of 21
				if ((strTCT.inMerchantCopy == 1) || (strTCT.inBankCopy == 1))
			 	{
                   vdCTOS_PrinterFline(1);  
				   //CTOS_PrinterFline(d_LINE_DOT * 1);
				   CTOS_PrinterPutString("SIGN:__________________________");
			 	}
			 else
			 	    vdCTOS_PrinterFline(1); 
			 	  	//CTOS_PrinterFline(d_LINE_DOT);
			  //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 8 of 21
			}
		}	
		else
		{
            vdCTOS_PrinterFline(1); 
			//CTOS_PrinterFline(d_LINE_DOT );
		}

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

		//CTOS_PrinterPutString("SIGN:_______________________");
		
		//printCardHolderName();

		vdCTOS_PrinterFline(1);
		//CTOS_PrinterFline(d_LINE_DOT * 1);

		//ushCTOS_PrintAgreement();

		if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
			vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
			vdPrintCenter(strMMT[0].szRctFoot3);

        vdCTOS_PrinterFline(1);
		//CTOS_PrinterFline(d_LINE_DOT * 1); 
		//CTOS_PrinterPutString("   ***** BANK COPY *****  ");
		
		//aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 7 of 12
		if (strTCT.inMerchantCopy == 1)
		   vdPrintCenter("   ***** MERCHANT COPY *****  ");
		if (strTCT.inCustomerCopy == 1)
		   vdPrintCenter("   ***** CUSTOMER COPY *****  ");
		if (strTCT.inBankCopy== 1)
		   vdPrintCenter("   ***** BANK COPY *****  ");
		 //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 7 of 12
	}
//1027
	else if(page == d_SECOND_PAGE)
	{
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			EMVtagLen = 0;
		
			DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		
			vdDebug_LogPrintf("CVM [%02X %02X %02X]",
				srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
			vdDebug_LogPrintf("srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);
		
		if ((srTransRec.byEntryMode == CARD_ENTRY_ICC))
		{
			EMVtagLen = 3;
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

			//EMV: Fix for noCVM during above floor limit scenario - start -- jzg
			vdDebug_LogPrintf("EMVtagVal [%02X %02X %02X]", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
			
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				(EMVtagVal[0] != 0x1E) && (EMVtagVal[0] != 0x5E)) || 
				(EMVtagVal[0] == 0x3F))
			//EMV: Fix for noCVM during above floor limit scenario - end -- jzg
			{
				vdCTOS_PrinterFline(1);
				//CTOS_PrinterFline(d_LINE_DOT * 1);
				CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
					CTOS_PrinterPutString("     (PIN VERIFY SUCCESS)");
			}
			else
			{
                vdCTOS_PrinterFline(1);  
				//CTOS_PrinterFline(d_LINE_DOT );
			}
		}
		else if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		{
			//if(stRCDataAnalyze.bCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)				
			if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
			{
				if (srTransRec.bWaveSID == d_VW_SID_AE_EMV && srTransRec.bWaveSCVMAnalysis == d_CVM_REQUIRED_NONE)
				{
					if ((strTCT.inMerchantCopy == 2) || (strTCT.inBankCopy == 2))
				 	{
						vdCTOS_PrinterFline(1); 
					    //CTOS_PrinterFline(d_LINE_DOT * 1);
					    CTOS_PrinterPutString("SIGN:__________________________");
				 	}
				 	else
                        vdCTOS_PrinterFline(1);    
					    //CTOS_PrinterFline(d_LINE_DOT);
				}
				else
                {
                   vdCTOS_PrinterFline(1); 
				   //CTOS_PrinterFline(d_LINE_DOT * 1);
                   CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
                   if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
                        CTOS_PrinterPutString("      (PIN VERIFY SUCCESS)");
                }
			}
			else
			{
			   //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 12 of 21
			  if ((strTCT.inMerchantCopy == 2) || (strTCT.inBankCopy == 2))
			 	{
                   vdCTOS_PrinterFline(1); 
				   //CTOS_PrinterFline(d_LINE_DOT * 1);
				   CTOS_PrinterPutString("SIGN:__________________________");
			 	}
			 else
			 	    vdCTOS_PrinterFline(1); 
			 	  	//CTOS_PrinterFline(d_LINE_DOT);
			  //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 12 of 21
			
			}
		}
		else
		{
            vdCTOS_PrinterFline(1);
			//CTOS_PrinterFline(d_LINE_DOT );
		}
		
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
		
			//CTOS_PrinterPutString("SIGN:________________________________________");
			//printCardHolderName();
		
			vdCTOS_PrinterFline(1);
			//CTOS_PrinterFline(d_LINE_DOT * 1);
		
			//ushCTOS_PrintAgreement();
		
			if(strlen(strMMT[0].szRctFoot1) > 0)
				vdPrintCenter(strMMT[0].szRctFoot1);
			if(strlen(strMMT[0].szRctFoot2) > 0)
				vdPrintCenter(strMMT[0].szRctFoot2);
			if(strlen(strMMT[0].szRctFoot3) > 0)
				vdPrintCenter(strMMT[0].szRctFoot3);

		    vdCTOS_PrinterFline(1);
			//CTOS_PrinterFline(d_LINE_DOT * 1); 
			//CTOS_PrinterPutString("	***** BANK COPY *****  ");
			
           //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 8 of 12
           if (strTCT.inMerchantCopy == 2)
                vdPrintCenter("   ***** MERCHANT COPY *****  ");
           if (strTCT.inBankCopy == 2)
                vdPrintCenter("   ***** BANK COPY *****  ");
           if (strTCT.inCustomerCopy == 2)
                vdPrintCenter("   ***** CUSTOMER COPY *****  ");
           //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end	 8 of 12
		}


//1027
	else if(page == d_THIRD_PAGE)
	{
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			EMVtagLen = 0;
		
			DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		
			vdDebug_LogPrintf("CVM [%02X %02X %02X]",
				srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
			vdDebug_LogPrintf("srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);
		
		if ((srTransRec.byEntryMode == CARD_ENTRY_ICC))
		{
			EMVtagLen = 3;
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

			//EMV: Fix for noCVM during above floor limit scenario - start -- jzg
			vdDebug_LogPrintf("EMVtagVal [%02X %02X %02X]", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
			
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				(EMVtagVal[0] != 0x1E) && (EMVtagVal[0] != 0x5E)) || 
				(EMVtagVal[0] == 0x3F))
			//EMV: Fix for noCVM during above floor limit scenario - end -- jzg
			{
                vdCTOS_PrinterFline(1);
				//CTOS_PrinterFline(d_LINE_DOT * 1);
				CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
					CTOS_PrinterPutString("      (PIN VERIFY SUCCESS)");
			}
			else
			{
                vdCTOS_PrinterFline(1);   
				//CTOS_PrinterFline(d_LINE_DOT );
			}
		}
		else if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		{
			//if(stRCDataAnalyze.bCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
			if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
			{
				if (srTransRec.bWaveSID == d_VW_SID_AE_EMV && srTransRec.bWaveSCVMAnalysis == d_CVM_REQUIRED_NONE)
				{
					if ((strTCT.inMerchantCopy == 3) || (strTCT.inBankCopy == 3))
				 	{
                      vdCTOS_PrinterFline(1);  
					  //CTOS_PrinterFline(d_LINE_DOT * 1);
					  CTOS_PrinterPutString("SIGN:__________________________");
				 	}
				 	else
						vdCTOS_PrinterFline(1);
				 	  	//CTOS_PrinterFline(d_LINE_DOT);
				}
				else
                {
                   vdCTOS_PrinterFline(1); 
				   //CTOS_PrinterFline(d_LINE_DOT * 1);
                   CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
                   if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
                        CTOS_PrinterPutString(" 	(PIN VERIFY SUCCESS)");
                }
			}
			else
			{
			   //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 16 of 21
				if ((strTCT.inMerchantCopy == 3) || (strTCT.inBankCopy == 3))
			 	{
                    vdCTOS_PrinterFline(1);  
					//CTOS_PrinterFline(d_LINE_DOT * 1);
				    CTOS_PrinterPutString("SIGN:__________________________");
			 	}
			 else
                  vdCTOS_PrinterFline(1); 
			      //CTOS_PrinterFline(d_LINE_DOT);
			  //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 16 of 21
		
			}
		}
		else
		{
            vdCTOS_PrinterFline(1); 
			//CTOS_PrinterFline(d_LINE_DOT );
		}
		
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
		
			//printCardHolderName();
		
			vdCTOS_PrinterFline(1);
			//CTOS_PrinterFline(d_LINE_DOT * 1);
		
			//ushCTOS_PrintAgreement();
		
			if(strlen(strMMT[0].szRctFoot1) > 0)
				vdPrintCenter(strMMT[0].szRctFoot1);
			if(strlen(strMMT[0].szRctFoot2) > 0)
				vdPrintCenter(strMMT[0].szRctFoot2);
			if(strlen(strMMT[0].szRctFoot3) > 0)
				vdPrintCenter(strMMT[0].szRctFoot3);
		
            vdCTOS_PrinterFline(1);
			//CTOS_PrinterFline(d_LINE_DOT * 1); 
			//CTOS_PrinterPutString("	***** BANK COPY *****  ");
			
			//aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 9 of 12
			if(strTCT.inCustomerCopy == 3)
				vdPrintCenter("   ***** CUSTOMER COPY *****  ");
			if(strTCT.inBankCopy== 3)
				vdPrintCenter("   ***** BANK COPY *****  ");
			if (strTCT.inMerchantCopy == 3)
			   vdPrintCenter("   ***** MERCHANT COPY *****  ");
			 //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 9 of 12
		}

	/***************************************************/
	//sidumili: Issue#000135 [Print Footer Logo]
	if (srTransRec.fFooterLogo){
		vdCTOS_PrinterFline(2);
	//CTOS_PrinterFline(d_LINE_DOT * 2); 
	//vdCTOSS_PrintFooterLogoBMPPic(0, 0, "footer.bmp");
    vdCTOSS_PrinterBMPPicEx(0,0,"/home/ap/pub/footer.bmp");  
	}
	/***************************************************/
	
    vdCTOS_PrinterFline(6); 
	//CTOS_PrinterFline(d_LINE_DOT * 6); 

	return d_OK;	
}

USHORT ushCTOS_PrintFooter(int page)
{		
    BYTE EMVtagVal[64] = {0};
    USHORT EMVtagLen = 0; 
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 3];
    
    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    EMVtagLen = 0;


#if 0	
    DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
    
    vdDebug_LogPrintf("CVM [%02X %02X %02X]",
    srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
    vdDebug_LogPrintf("srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    
    if ((srTransRec.byEntryMode == CARD_ENTRY_ICC))
    {
        EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
        
        //EMV: Fix for noCVM during above floor limit scenario - start -- jzg
        vdDebug_LogPrintf("EMVtagVal [%02X %02X %02X]", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
        
        if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
        (EMVtagVal[0] != 0x1E) && (EMVtagVal[0] != 0x5E)) || 
        (EMVtagVal[0] == 0x3F))
        //EMV: Fix for noCVM during above floor limit scenario - end -- jzg
        {
            CTOS_PrinterFline(d_LINE_DOT);
            vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
            if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
                vdPrintCenter("(PIN VERIFY SUCCESS)");
        }
        else
        {
            CTOS_PrinterFline(d_LINE_DOT);
        }
    }
    else if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
    {
        vdDebug_LogPrintf("srTransRec.bWaveSCVMAnalysis = [%d]", srTransRec.bWaveSCVMAnalysis);
        //if(stRCDataAnalyze.bCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
        if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
        {
            if (srTransRec.bWaveSID == d_VW_SID_AE_EMV && srTransRec.bWaveSCVMAnalysis == d_CVM_REQUIRED_NONE)
            {
            if((page == BANK_COPY_RECEIPT) || (page == MERCHANT_COPY_RECEIPT)) /*bank and merchant copy*/
                {
                    CTOS_PrinterFline(d_LINE_DOT * 2);
                    CTOS_PrinterPutString("SIGN:__________________________");
                }
                else
                    CTOS_PrinterFline(d_LINE_DOT);
            }
            else
            {
                CTOS_PrinterFline(d_LINE_DOT);
                vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
                    vdPrintCenter("(PIN VERIFY SUCCESS)");
            }
        }
        else
        {
            //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 8 of 21
        if((page == BANK_COPY_RECEIPT) || (page == MERCHANT_COPY_RECEIPT)) /*bank and merchant copy*/
            {
                CTOS_PrinterFline(d_LINE_DOT * 2);
                CTOS_PrinterPutString("SIGN:__________________________");
            }
            else
                CTOS_PrinterFline(d_LINE_DOT);
            //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 8 of 21
            }
    }	
    else
    {
        CTOS_PrinterFline(d_LINE_DOT);
    }

#endif
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    //CTOS_PrinterFline(d_LINE_DOT); 

	
    vdPrintFooterText();

    //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 8 of 13
    if(page == BANK_COPY_RECEIPT)
        vdPrintCenter("--- BANK COPY ---");
    else if(page == CUSTOMER_COPY_RECEIPT)
        vdPrintCenter("--- CUSTOMER COPY ---");
    else if(page = MERCHANT_COPY_RECEIPT)
        vdPrintCenter("--- MERCHANT COPY ---");
    //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 8 of 13
    /***************************************************/
    //sidumili: Issue#000135 [Print Footer Logo]
    if (srTransRec.fFooterLogo)
    {
        //CTOS_PrinterFline(d_LINE_DOT * 2); 
        //vdCTOSS_PrintFooterLogoBMPPic(0, 0, "footer.bmp");
        //vdCTOSS_PrinterBMPPicEx(0,0,"/home/ap/pub/footer.bmp");
		vdCTOSS_PrinterBMPPic(0, 0, "/home/ap/pub/footer.bmp");
    }
    /***************************************************/

	vdLineFeed(srTransRec.fFooterLogo);
	
	return d_OK;	
}



USHORT ushCTOS_printAll(int page)
{	
	int inResult;

	if(strTCT.byERMMode)
       fERMTransaction=TRUE;
	else
	   fERMTransaction=FALSE;
		
		//sidumili: check printer status
		if( printCheckPaper()==-1)
    	return -1;
    
    //vdSetPrintThreadStatus(1);
    
	//vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
	vdCTOSS_PrinterStart(100);

	if(strTCT.fDisplayAPPROVED == FALSE)//Sound BEEP before printing if APPROVED message is not displayed
		CTOS_Beep();

	vdPrintReportDisplayBMP();
/*	
    if(VS_FALSE == fRePrintFlag) // Moved to vdSaveLastInv() inside inCTOS_SaveBatchTxn
	    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
		vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }
*/	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	
    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);  

	//gcitra
	if (srTransRec.byTransType == CASH_LOYALTY)
  		ushCTOS_PrintCashLoyaltyBody();	
	else
	{
	//gcitra
// Removed strTCT.fSMReceipt implementation
#if 0
		if (strTCT.fSMReceipt)	// SMAC application
		{
			ushCTOS_PrintSMHeader(page);
			ushCTOS_PrintSMBody(page);
			ushCTOS_PrintSMFooter(page);
		}
		else
		{
			ushCTOS_PrintHeader(page);
		  	ushCTOS_PrintBody(page);	
			ushCTOS_PrintFooter(page);
		}
#else
		ushCTOS_PrintHeader(page);
		ushCTOS_PrintBody(page);	
		ushCTOS_PrintFooter(page);
#endif
	}
    //vdSetPrintThreadStatus(0);
    vdCTOSS_PrinterEnd();
    CTOS_LCDTClearDisplay();  
    
    return d_OK;
 
}

int inCTOS_rePrintReceipt(void)
{
	int   inResult;
	BYTE  szInvNoAsc[6+1];
	BYTE  szInvNoBcd[3];	
	BOOL   needSecond = TRUE;	
	//1027
	BOOL   needThird = TRUE;	
	BYTE   key;

	/*BDO: Parameterized receipt printing - start*/
	BOOL BankCopy, CustCopy, MercCopy;
	BOOL fFirstReceiptPrinted = VS_FALSE;
    /*BDO: Parameterized receipt printing - end*/

    BOOL fFirstReceipt=TRUE; 
    int inReceiptCtr=0;
    BYTE szPrintOption[3+1];
    BYTE chPrintOption=0x00;

	//for MP200 no need print
	if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    	return (d_OK);
	
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
	if(printCheckPaper()==-1)
		return d_NO;

	inResult = inCTOS_GeneralGetInvoice();
	if(d_OK != inResult)
	{
		return inResult;
	}

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOS_MultiAPBatchSearch(d_IPC_CMD_REPRINT_ANY);
		if(d_OK != inResult)
			return inResult;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inResult = inCTOS_MultiAPGetVoid();
			if(d_OK != inResult)
				return inResult;
		}		
		inResult = inCTOS_BatchSearch();
		if(d_OK != inResult)
			return inResult;
	}

	if(d_OK != inResult)
	{
		return inResult;
	}

	/*BDO: Parameterized receipt printing - start*/
	inHDTRead(srTransRec.HDTid);

	if (srTransRec.byTransType == VOID){
		BankCopy = strHDT.fPrintVoidBankReceipt;
		CustCopy = strHDT.fPrintVoidCustReceipt;
		MercCopy = strHDT.fPrintVoidMercReceipt;
	}else{
		BankCopy = strHDT.fPrintBankReceipt;
		CustCopy = strHDT.fPrintCustReceipt;
		MercCopy = strHDT.fPrintMercReceipt;
	}
	/*BDO: Parameterized receipt printing - end*/

	
	//1026
	inCSTRead(srTransRec.HDTid); // sidumili: commented
	//1026

	//BDO: Reload IIT to properly load PAN masking during reprint - start -- jzg
	vdDebug_LogPrintf("JEFF::IITid [%d]", srTransRec.IITid);
	inIITRead(srTransRec.IITid);
	//BDO: Reload IIT to properly load PAN masking during reprint - end -- jzg

    vdCTOS_PrintReceiptCopies(BankCopy, CustCopy, MercCopy, FALSE);

	return d_OK;
}

void vdCTOS_PrintSummaryReport()
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult,inRet;
	int inTranCardType;
	int inReportType;
	int inIITNum , i;
	char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];

	int inPADSize,x;
	char szSPACE[40];
	char szBuff[46];
	char szBuff2[46];

	//for MP200 no need print
	if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    	return (d_OK);
	
	if( printCheckPaper()==-1)
		return;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;
	strHDT.inHostIndex = shHostIndex;
    
	DebugAddINT("summary host Index",shHostIndex);
	inCSTRead(strHDT.inCurrencyIdx);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SUMMARY_REPORT);
		if(d_OK != inRet)
			return ;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return ;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return ;
		}
	}

    //inRet = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inRet)
    //    return inRet;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return;
	}

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return;    
    }

	//vdSetPrintThreadStatus(1); /*BDO: Display printing image during report -- sidumili*/
	//vdThreadDisplayPrintBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
	vdPrintReportDisplayBMP();
	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    
    vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
    
	ushCTOS_PrintHeader(0);	
    
    //vdPrintTitleCenter("SUMMARY REPORT");
    vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    //printHostLabel();
    
	printTIDMID();
    
	printDateTime();
    
	printBatchNO();

    //vdCTOS_PrinterFline(1);
    //CTOS_PrinterFline(d_LINE_DOT * 1);
    
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	vdPrintCenter("SUMMARY REPORT");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	vdPrintCenter("*** TRANSACTION TOTALS BY ISSUER ***");
    vdCTOS_PrinterFline(1);
	CTOS_PrinterFline(d_LINE_DOT * 1);

    vdCTOS_PrinterFline(1);
    //CTOS_PrinterFline(d_LINE_DOT * 1);
	for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
	{
		inReportType = PRINT_CARD_TOTAL;
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <20; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount == 0)			
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount == 0))
					continue;
				
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				//strcpy(szStr,strIIT.szIssuerLabel);

		        //sprintf(szBuff,"HOST%s%s",szSPACE,srTransRec.szHostLabel);
				sprintf(szStr,"ISSUER: %s",strIIT.szIssuerLabel);

                //sprintf(szBuff,"HOST:      %s",srTransRec.szHostLabel);
				
				
				//inPrintLeftRight2(szStr,szBuff,41);
				inPrint(szStr);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulRefundTotalAmount), 
												
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulVoidSaleTotalAmount), 

												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
//1010
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulReloadCCTotalAmount)
//1010
												);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"GRAND TOTAL");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inPrint(szStr);
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
													
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
													
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 

													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
//1010
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount)
//1010

													);		
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inPrint(szStr);
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 

												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
//1010
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount)
//1010

												);		
		}
	}
    
	//print space one line
    vdCTOS_PrinterFline(6);
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);

	//vdSetPrintThreadStatus(0); /*BDO: Display printing image during report -- sidumili*/
	vdCTOSS_PrinterEnd();
	return;	
}

void vdCTOS_PrintDetailReport()
{
	unsigned char chkey;
	short shHostIndex;
	int inResult,i,inCount,inRet;
	int inTranCardType;
	int inReportType;
	int inBatchNumOfRecord;
	int *pinTransDataid;

	//0720
	ACCUM_REC srAccumRec;
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szStr[d_LINE_SIZE + 1];
	//0720

	//for MP200 no need print
	if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    	return (d_OK);
	
	if( printCheckPaper()==-1)
		return;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DETAIL_REPORT);
		if(d_OK != inRet)
			return ;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return ;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return ;
		}
	}

    //inResult = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inResult)
    //    return inResult;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

    inResult = inCTOS_ChkBatchEmpty();
    if(d_OK != inResult)
        return;
	
	inBatchNumOfRecord = inBatchNumRecord();
	
	DebugAddINT("batch record",inBatchNumOfRecord);
	if(inBatchNumOfRecord <= 0)
	{
		vdDisplayErrorMsg(1, 8, "EMPTY BATCH");
		return;
	}

	//vdSetPrintThreadStatus(1); /*BDO: Display printing image during report -- sidumili*/
	//vdThreadDisplayPrintBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
	vdPrintReportDisplayBMP();

	vdCTOSS_PrinterStart(100);
	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
	ushCTOS_printBatchRecordHeader();
	
	pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
	inCount = 0;		
	inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
	for(i=0; i<inBatchNumOfRecord; i++)
	{
		inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        inCount ++;
		ushCTOS_printBatchRecordFooter();	
	}

     free(pinTransDataid);

	//0720
		memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
		if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
		{
				vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
				return; 	 
		}
	

	    vdCTOS_PrinterFline(2);
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//CTOS_PrinterFline(d_LINE_DOT * 1);

	    memset(baTemp, 0x00, sizeof(baTemp));
		//sprintf(szStr,"CARD TYPE        COUNT         AMT");
		sprintf(szStr,"CARD TYPE        COUNT                 AMT");
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        inPrint(szStr);
		
		for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
		{
				inReportType = DETAIL_REPORT_TOTAL;
		
				for(i= 0; i <20; i ++ )
				{
					vdDebug_LogPrintf("--Count[%d]", i);
					if((srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount == 0)
					&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount == 0)
					&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount == 0)
					&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount == 0)
					&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount == 0))
						continue;
				
					vdDebug_LogPrintf("Count[%d]", i); 
					inIITRead(i);
					memset(szStr, ' ', d_LINE_SIZE);
					memset (baTemp, 0x00, sizeof(baTemp));
	
				
					vdCTOS_PrintAccumeByHostAndCard (inReportType, 
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount,	(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
																
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulRefundTotalAmount), 
																
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulVoidSaleTotalAmount), 
				
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
				
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulReloadCCTotalAmount));	
				}
	
				
				printDividingLine(DIVIDING_LINE);
				
				inReportType = DETAIL_REPORT_GRANDTOTAL;
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
																		
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
																		
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 
					
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
					//1010
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount));
					//1010
	}
	
	
	//0720

    vdCTOS_PrinterFline(10);
	vdCTOSS_PrinterEnd();
	vdSetPrintThreadStatus(0); /*BDO: Display printing image during report -- sidumili*/

	//vdSetPrintThreadStatus(0); /*BDO: Display printing image during report -- sidumili*/
	
	return;	
}


void vdCTOS_PrintDetailTotals()
{
	//unsigned char chkey;
	//short shHostIndex;
	int inResult,i,inCount,inRet;
	int inTranCardType;
	int inReportType;
	//int inBatchNumOfRecord;
	//int *pinTransDataid;

//0720
	ACCUM_REC srAccumRec;
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szStr[d_LINE_SIZE + 1];
//0720



//0720
	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
	{
			vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
			return; 	 
	}


    vdCTOS_PrinterFline(2);
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	//CTOS_PrinterFline(d_LINE_DOT * 1);

	memset(baTemp, 0x00, sizeof(baTemp));
	sprintf(szStr,"CARD TYPE        COUNT         AMT");
	
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    inPrint(szStr);

	for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
	{
			inReportType = DETAIL_REPORT_TOTAL;
	
			for(i= 0; i <20; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount == 0))
					continue;
			
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));

			
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
				srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount,	(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
															
				srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulRefundTotalAmount), 
															
				srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulVoidSaleTotalAmount), 
			
				srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
			
				srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulReloadCCTotalAmount));	
			}

			
			printDividingLine(DIVIDING_LINE);
			
			inReportType = DETAIL_REPORT_GRANDTOTAL;
			
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
			srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
																	
			srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
																	
			srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 
				
			srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
				//1010
			srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount));
				//1010
}


//0720


         vdCTOS_PrinterFline(5);  
    	//CTOS_PrinterFline(d_LINE_DOT * 5);

	
	return;	
}




int inCTOS_rePrintLastReceipt(void)
{
	int   inResult;
	BYTE  szInvNoAsc[6+1];
	BYTE  szInvNoBcd[3];	
    BOOL   needSecond = TRUE;	
    BYTE   key;
	//1027
	BOOL   needThird = TRUE;	

	/*BDO: Parameterized receipt printing - start*/
	BOOL BankCopy, CustCopy, MercCopy;
	BOOL fFirstReceiptPrinted = VS_FALSE;
	/*BDO: Parameterized receipt printing - end*/

    BOOL fFirstReceipt=TRUE; 
	int inReceiptCtr=0;
	BYTE szPrintOption[3+1];
	BYTE chPrintOption=0x00;
	
	//for MP200 no need print
	if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    	return (d_OK);
	
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
	if( printCheckPaper()==-1)
		return d_NO;


	memcpy(srTransRec.szInvoiceNo,strTCT.szLastInvoiceNo,INVOICE_BCD_SIZE);
	DebugAddHEX("last invoice", strTCT.szLastInvoiceNo,3 );

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOS_MultiAPBatchSearch(d_IPC_CMD_REPRINT_LAST);
		if(d_OK != inResult)
			return inResult;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inResult = inCTOS_MultiAPGetVoid();
			if(d_OK != inResult)
				return inResult;
		}		
		inResult = inCTOS_BatchSearch();
		if(d_OK != inResult)
			return inResult;
	}

	/*BDO: Parameterized receipt printing - start*/
	inHDTRead(srTransRec.HDTid);

	if (srTransRec.byTransType == VOID){
		BankCopy = strHDT.fPrintVoidBankReceipt;
		CustCopy = strHDT.fPrintVoidCustReceipt;
		MercCopy = strHDT.fPrintVoidMercReceipt;
	}else{
		BankCopy = strHDT.fPrintBankReceipt;
		CustCopy = strHDT.fPrintCustReceipt;
		MercCopy = strHDT.fPrintMercReceipt;
	}

	//remove later
	vdDebug_LogPrintf("JEFF inCTOS_rePrintLastReceipt::srTransRec.HDTid = [%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("JEFF inCTOS_rePrintLastReceipt::BankCopy = [%d]", BankCopy);
	vdDebug_LogPrintf("JEFF inCTOS_rePrintLastReceipt::CustCopy = [%d]", CustCopy);
	vdDebug_LogPrintf("JEFF inCTOS_rePrintLastReceipt::MercCopy = [%d]", MercCopy);
	/*BDO: Parameterized receipt printing - end*/

	//1026
	inCSTRead(srTransRec.HDTid); // sidumili: commented
	//1026


	//BDO: Reload IIT to properly load PAN masking during reprint - start -- jzg
	vdDebug_LogPrintf("JEFF::IITid [%d]", srTransRec.IITid);
	inIITRead(srTransRec.IITid);
	//BDO: Reload IIT to properly load PAN masking during reprint - end -- jzg

    vdCTOS_PrintReceiptCopies(BankCopy, CustCopy, MercCopy, FALSE);
	
	return d_OK;
}

/*BDO: Parameterized receipt printing - start*/
USHORT ushCTOS_printReceipt(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
		//1027
		BOOL   needThird = TRUE;	

    BOOL BankCopy, CustCopy, MercCopy;
		BOOL fFirstReceiptPrinted = VS_FALSE;
	int fNSRflag = 0;
    BOOL fFirstReceipt=TRUE; 
    int inReceiptCtr=0;
    BYTE szPrintOption[3+1];
    BYTE chPrintOption=0x00;
		
     
		if (srTransRec.byTransType == VOID){
			BankCopy = strHDT.fPrintVoidBankReceipt;
			CustCopy = strHDT.fPrintVoidCustReceipt;
			MercCopy = strHDT.fPrintVoidMercReceipt;
		}else{
			BankCopy = strHDT.fPrintBankReceipt;
			CustCopy = strHDT.fPrintCustReceipt;
			MercCopy = strHDT.fPrintMercReceipt;
		}

	//for MP200 no need print
	if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    	return (d_OK);
    
    if( printCheckPaper()==-1)
    	return -1;

	#if 0
	vdDisplayAnimateBmp(0,0, "Printer1(320240).bmp", "Printer2(320240).bmp", "Printer3(320240).bmp", NULL, NULL);
//	#else
	if (strlen(srTransRec.szRespCode) > 0){
		CTOS_LCDTClearDisplay();
		if(strTCT.fDisplayAPPROVED == TRUE && srTransRec.byTransType != BALANCE_INQUIRY) // Terminal will display the SMAC balance instead of the "APPROVED" message. 
		{
			vdDispTransTitle(srTransRec.byTransType);
			  vdDisplayErrorMsgResp2(" ", " ","APPROVED");
			//vduiDisplayStringCenter(8,"TRANSACTION APPROVED"); /*BDO : Request -- sidumili*/
			//WaitKey(1);
		}
		else
			CTOS_Beep();
	}
	#endif
	
    //gcitra
	if (srTransRec.byTransType == CASH_LOYALTY){
		result = ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
		CTOS_KBDBufFlush();//cleare key buffer
	    	return (d_OK);
	}
    //gcitra

    vdCTOS_PrintReceiptCopies(BankCopy, CustCopy, MercCopy, TRUE);

	return (d_OK);
}
/*BDO: Parameterized receipt printing - end*/

USHORT ushCTOS_printErrorReceipt(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
	
	char szStr[d_LINE_SIZE + 1];
   char szTemp[d_LINE_SIZE + 1];
   char szTemp1[d_LINE_SIZE + 1];
   char szTemp3[d_LINE_SIZE + 1];
   char szTemp4[d_LINE_SIZE + 1];
   char szTemp5[d_LINE_SIZE + 1];
   char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
   int inFmtPANSize;
   BYTE baTemp[PAPER_X_SIZE * 64];
   CTOS_FONT_ATTRIB stFONT_ATTRIB;
   int num,i,inResult;
   unsigned char tucPrint [24*4+1];    
   BYTE   EMVtagVal[64];
   USHORT EMVtagLen; 
   short spacestring;


	//for MP200 no need print
	if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    	return (d_OK);
	
    if( printCheckPaper()==-1)
    	return -1;

	memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
		vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }
	
	DebugAddHEX("LastInvoiceNum", strTCT.szLastInvoiceNo,3);

	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);  
	DebugAddSTR("ushCTOS_printAll","print...",20);
		

	ushCTOS_PrintHeader(d_FIRST_PAGE);

	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
	vdPrintCenter(szTemp1);
	
	printDateTime();

    printTIDMID(); 
	
	printBatchInvoiceNO();
	
	szGetTransTitle(srTransRec.byTransType, szStr);     
		vdPrintTitleCenter(szStr);    
    if(srTransRec.byTransType == SETTLE)
    {
        DebugAddSTR("settle","print...",20);
    }
    else
    {
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    	memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    	EMVtagLen = 0;
    	memset(szStr, ' ', d_LINE_SIZE);
    	vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);

    	sprintf(szStr, "%s", srTransRec.szCardLable);
    	vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
    	
    	memset (baTemp, 0x00, sizeof(baTemp));
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
        inPrint(szStr);

    	cardMasking(srTransRec.szPAN, PRINT_CARD_MASKING_1);
    	strcpy(szTemp4, srTransRec.szPAN);
    	memset (baTemp, 0x00, sizeof(baTemp));
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);		
        inPrint(szStr);

        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        
    	//Exp date and Entry mode
    	memset(szStr, ' ', d_LINE_SIZE);
    	memset(szTemp, 0, sizeof(szTemp));
    	memset(szTemp1, 0, sizeof(szTemp1));
    	memset(szTemp4, 0, sizeof(szTemp4));
    	memset(szTemp5, 0, sizeof(szTemp5));
    	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
    	DebugAddSTR("EXP",szTemp,12);  
    	
    	for (i =0; i<4;i++)
    		szTemp[i] = '*';
    	memcpy(szTemp4,&szTemp[0],2);
    	memcpy(szTemp5,&szTemp[2],2);

    	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
    		memcpy(szTemp1,"Chip",4);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
    		memcpy(szTemp1,"Manual",6);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
			memcpy(szTemp1,"Swipe",5);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
    		memcpy(szTemp1,"Fallback",8);
    	
    	memset (baTemp, 0x00, sizeof(baTemp));
    	sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
    	
    	DebugAddSTR("ENT:",baTemp,12);  
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        inPrint(szTemp);

    		
     	memset (baTemp, 0x00, sizeof(baTemp));
    	stFONT_ATTRIB.FontSize = 0x1010;
        stFONT_ATTRIB.X_Zoom = DOUBLE_SIZE;       // The width magnifies X_Zoom diameters
        stFONT_ATTRIB.Y_Zoom = DOUBLE_SIZE;       // The height magnifies Y_Zoom diameters

        stFONT_ATTRIB.X_Space = 0;      // The width of the space between the font with next font

    }

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szStr, "%s", "TRANS NOT SUC.");
	
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
    inPrint(szStr);

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	
	sprintf(szStr, "%s", "PLS TRY AGAIN");
	
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
    inPrint(szStr);
     vdCTOS_PrinterFline(6);
	//CTOS_PrinterFline(d_LINE_DOT * 6); 
	
	//inSetTextMode();
    return (d_OK);
}


int inCTOS_REPRINT_ANY()
{
    char szErrMsg[30+1];
    int   inResult;
	
	fRePrintFlag = TRUE;
    vdCTOS_SetTransType(REPRINT_ANY);
	
    CTOS_LCDTSelectFontSize(d_FONT_16x30);
	inResult = inCTOS_rePrintReceipt();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
	fRePrintFlag = FALSE;
	return inResult;
}

int inCTOS_REPRINT_LAST()
{
    char szErrMsg[30+1];
	int   inResult;
    
	fRePrintFlag = TRUE;
	
    CTOS_LCDTSelectFontSize(d_FONT_16x30);
	inResult = inCTOS_rePrintLastReceipt();	

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
	fRePrintFlag = FALSE;
	
	return inResult;
}

int inCTOS_PRINTF_SUMMARY()
{
    char szErrMsg[30+1];
    
	vdCTOS_PrintSummaryReport();	

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
    
	return d_OK;
	
}

int inCTOS_PRINTF_DETAIL()
{
    char szErrMsg[30+1];
    
	vdCTOS_PrintDetailReport();	

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
    
	return d_OK;
	
}

int inCTOS_REPRINTF_LAST_SETTLEMENT()
{
    char szErrMsg[30+1];
    
	ushCTOS_ReprintLastSettleReport();
	
	fRePrintFlag=FALSE;
    
    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
    
	return d_OK;
}

void vdCTOS_PrintAccumeByHostAndCard (int inReportType,
	USHORT usSaleCount, 
	double ulSaleTotalAmount,    
	USHORT usRefundCount,
	double  ulRefundTotalAmount,    
	USHORT usVoidSaleCount,
	double  ulVoidSaleTotalAmount,
	USHORT usOffSaleCount,
	double  ulOffSaleTotalAmount,
	//10101
	USHORT usReloadCount, 
	double  ulReloadTotalAmount)
	//10101
{
	char szStr[d_LINE_SIZE + 3] = {0};
	char szTemp[d_LINE_SIZE + 3] = {0}, 
	szTemp1[d_LINE_SIZE + 3] = {0}, 
	szTemp2[d_LINE_SIZE + 3] = {0};
	USHORT usTotalCount = 0;
	double  ulTotalAmount = 0.00;
	BYTE baTemp[PAPER_X_SIZE * 64] = {0};

	char szBuff[47] = {0};
	int inPADSize = 0,
	x = 0;
	char szSPACE[40] = {0};

	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);  
	memset (baTemp, 0x00, sizeof(baTemp));		


	//0720
	if((inReportType == DETAIL_REPORT_TOTAL) || (inReportType == DETAIL_REPORT_GRANDTOTAL)){
	    
		usTotalCount = usSaleCount + usOffSaleCount + usRefundCount + usReloadCount;
		memset(baTemp, 0x00, sizeof(baTemp));
			
		ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulReloadTotalAmount) - ulRefundTotalAmount;
		memset(szTemp1, 0x00, sizeof(szTemp1)); 
		memset(szTemp2, 0x00, sizeof(szTemp2));
			
		sprintf(szTemp1,"%.0f", ulTotalAmount); 	
		vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);// patrick add code 20141216		
		sprintf(szTemp,"%s%s",strCST.szCurSymbol,szTemp2);

		memset(szTemp1, 0x00, sizeof(szTemp1));
		szStr[47]=0x00;
	
		sprintf(szTemp1,"%d",usTotalCount);
		memset(szStr, 0x20, sizeof(szStr));
				
		if (inReportType == DETAIL_REPORT_TOTAL)
			memcpy(&szStr[0],strIIT.szIssuerLabel, strlen(strIIT.szIssuerLabel));
		else
			memcpy(&szStr[0],"TOTAL", 5);
				
		memcpy(&szStr[18],szTemp1, strlen(szTemp1));
		//memcpy(&szStr[24],szTemp, strlen(szTemp));
		memcpy(&szStr[42-strlen(szTemp)],szTemp, strlen(szTemp));

		memset(baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        inPrint(szStr);
		
	    return;
	}
	//0720

	//types
	/*		memset(szStr, ' ', d_LINE_SIZE);
	strcpy(szStr,"TYPES  COUNT  CUR            AMT");
	memset (baTemp, 0x00, sizeof(baTemp));
	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	CTOS_PrinterFline(d_LINE_DOT * 1);
	*/		

	//sale
	memset(szStr, 0x00, sizeof(szStr));
	memset(szBuff, 0x00, sizeof(szBuff));
	memset(szTemp1, 0x00, sizeof(szTemp1));	
	memset(szTemp2, 0x00, sizeof(szTemp2));
	sprintf(szTemp1,"%.0f", ulSaleTotalAmount+ulOffSaleTotalAmount);		
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);
	sprintf(szBuff,"%s%s",strCST.szCurSymbol,szTemp2);
	sprintf(szStr,"  SALE     %04d",usSaleCount+usOffSaleCount);
	inPrintLeftRight2(szStr,szBuff,41);


	//reload
	memset(szStr, 0x00, sizeof(szStr));
	memset(szBuff, 0x00, sizeof(szBuff));	
	memset(szTemp1, 0x00, sizeof(szTemp1));	
	memset(szTemp2, 0x00, sizeof(szTemp2));
	sprintf(szTemp1,"%.0f", ulReloadTotalAmount);		
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);
	sprintf(szBuff,"%s%s",strCST.szCurSymbol,szTemp2);
	sprintf(szStr,"  RELOAD   %04d",usReloadCount);
	inPrintLeftRight2(szStr,szBuff,41);


	//offline sale
	//memset(szStr, ' ', d_LINE_SIZE);
	//sprintf(szStr,"OFFLINE  %3d  %s  %10lu.%02lu", usOffSaleCount, strCST.szCurSymbol,ulOffSaleTotalAmount/100, ulOffSaleTotalAmount%100);
	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);		

	//void
	memset(szStr, 0x00, sizeof(szStr));
	memset(szBuff, 0x00, sizeof(szBuff));
	memset(szTemp1, 0x00, sizeof(szTemp1));	
	memset(szTemp2, 0x00, sizeof(szTemp2));
	sprintf(szTemp1,"%.0f", ulVoidSaleTotalAmount);		
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);
	sprintf(szBuff,"%s%s",strCST.szCurSymbol,szTemp2);
	sprintf(szStr,"  VOID     %04d",usVoidSaleCount);
	inPrintLeftRight2(szStr,szBuff,41);


	//refund
	memset(szStr, 0x00, sizeof(szStr));
	memset(szBuff, 0x00, sizeof(szBuff));
	memset(szTemp1, 0x00, sizeof(szTemp1));	
	memset(szTemp2, 0x00, sizeof(szTemp2));
	sprintf(szTemp1,"%.0f", ulRefundTotalAmount);		
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);
	sprintf(szBuff,"%s%s",strCST.szCurSymbol,szTemp2);
	sprintf(szStr,"  REF.     %04d",usRefundCount);
	inPrintLeftRight2(szStr,szBuff,41);

	//CTOS_PrinterPutString("----------------------------------------------");

	usTotalCount = usSaleCount + usOffSaleCount + usRefundCount + usReloadCount;
	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	if (inReportType == PRINT_HOST_TOTAL)
	{
		if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulReloadTotalAmount))
		{
			ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulReloadTotalAmount);
			memset(szTemp1, 0x00, sizeof(szTemp1)); 
			memset(szTemp2, 0x00, sizeof(szTemp2));
			sprintf(szTemp1,"%.0f", ulTotalAmount);		
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);
			sprintf(szTemp,"-%s%s",strCST.szCurSymbol,szTemp2);
			sprintf(szStr,"  TOT.     %04d", usTotalCount);	
			inPrintLeftRight2(szStr,szTemp,41);
		}
		else
		{
			ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulReloadTotalAmount) - ulRefundTotalAmount;
			memset(szTemp1, 0x00, sizeof(szTemp1)); 
			memset(szTemp2, 0x00, sizeof(szTemp2));
			sprintf(szTemp1,"%.0f", ulTotalAmount);		
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);
			sprintf(szTemp,"%s%s",strCST.szCurSymbol,szTemp2);
			sprintf(szStr,"  TOT.     %04d", usTotalCount);							
			inPrintLeftRight2(szStr,szTemp,41);
		}
	}
	else if(inReportType == PRINT_CARD_TOTAL)
	{
		if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulReloadTotalAmount))
		{
			ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulReloadTotalAmount);
			memset(szTemp1, 0x00, sizeof(szTemp1)); 
			memset(szTemp2, 0x00, sizeof(szTemp2));
			sprintf(szTemp1,"%.0f", ulTotalAmount);		
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);
			sprintf(szTemp,"-%s%s",strCST.szCurSymbol,szTemp2);
			sprintf(szStr,"  TOT.     %04d", usTotalCount);	
			inPrintLeftRight2(szStr,szTemp,41);
		}
		else
		{
			ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulReloadTotalAmount) - ulRefundTotalAmount;
			memset(szTemp1, 0x00, sizeof(szTemp1)); 
			memset(szTemp2, 0x00, sizeof(szTemp2));
			sprintf(szTemp1,"%.0f", ulTotalAmount);		
			vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp2);
			sprintf(szTemp,"%s%s",strCST.szCurSymbol,szTemp2);
			sprintf(szStr,"  TOT.     %04d", usTotalCount);							
			inPrintLeftRight2(szStr,szTemp,41);
		}	 
	}

	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 

    vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1);
}

int inCTOS_SelectFont(int inFontMode,int inFontSize ,int inFontStyle,char * szFontName)
{
	if(inFontMode == d_FONT_TTF_MODE)
	{
		CTOS_PrinterFontSelectMode(d_FONT_TTF_MODE);	//set the printer with TTF Mode
		CTOS_PrinterTTFSelect("times.ttf", inFontStyle);
	}
	else
	{
	
		CTOS_PrinterFontSelectMode(d_FONT_FNT_MODE);	//set the printer with default Mode
		CTOS_LanguagePrinterFontSize(inFontSize, 0, TRUE);		
	}
	return d_OK;
	
}

int inCTOS_PrintSettleReport(BOOL fManualSettlement)
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult;
	int inTranCardType;
	int inReportType;
	int i;		
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];	
	int inTempTransType;

    BYTE        szTempDate[DATE_BCD_SIZE+1];     
    BYTE        szTempTime[TIME_BCD_SIZE+1];     

	if( printCheckPaper()==-1)
		return;

	srTransRec.fManualSettlement=fManualSettlement; 

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return;
	}

    if (fManualSettlement != TRUE)
		inCTLOS_Updatepowrfail(PFR_BATCH_SETTLEMENT_PRINT);//1204

	//save Orig Trans type before detail report
	inTempTransType = srTransRec.byTransType;

	vdCTOSS_PrinterStart(100);

	SetReportTime();
	//save date and time for retrieval later-after printing of details report
    memset(szTempDate, 0x00, sizeof(szTempDate));
    memset(szTempTime, 0x00, sizeof(szTempTime));
	memcpy(szTempDate, srTransRec.szDate, DATE_BCD_SIZE);
	memcpy(szTempTime, srTransRec.szTime, TIME_BCD_SIZE);

	/* BDO: Include detailed report to settlement receipt - start -- jzg */
    if(srTransRec.byTransType == MANUAL_SETTLE || fAUTOManualSettle == TRUE)
		vdCTOS_SettlementPrintDetailReport(TRUE, FALSE, FALSE);
    else if(strTCT.fPrintSettleDetailReport == TRUE)
		vdCTOS_SettlementPrintDetailReport(TRUE, FALSE, FALSE);

	//Reload back Original Trans Type
	srTransRec.byTransType = inTempTransType;
	
	memcpy(srTransRec.szDate, szTempDate, DATE_BCD_SIZE);
	memcpy(srTransRec.szTime, szTempTime,  TIME_BCD_SIZE);
	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;    
    }

   	//vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL); /*BDO: Display printing image during report -- sidumili*/
	vdPrintReportDisplayBMP();
	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);

	//SetReportTime();    
	ushCTOS_PrintHeader(0);	
    
    //vdPrintTitleCenter("SETTLEMENT");
    vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1);

    //printHostLabel();
    
	printTIDMID();
    
	printDateTime();
    
	printBatchNO();

	/* BDO: if settlement title should be "SETTLEMENT CLOSED", if manual posting "SUMMARY REPORT" */
		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		if ((srTransRec.byTransType == SETTLE) && (fAUTOManualSettle == FALSE)&& (fManualSettlement==FALSE))	
	  	{
			vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
			vdPrintCenter("SETTLEMENT CLOSED");
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
			vdPrintCenter("    *** TRANSACTION TOTALS BY ISSUER ***");
	  	}
		else
			vdPrintCenter("SUMMARY REPORT");
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);



    vdCTOS_PrinterFline(1); 
    //CTOS_PrinterFline(d_LINE_DOT * 1);
	for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
	{
		inReportType = PRINT_CARD_TOTAL;
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <20; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount == 0))
				
					continue;
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				//strcpy(szStr,strIIT.szIssuerLabel);
				sprintf(szStr,"ISSUER: %s",strIIT.szIssuerLabel);
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                inPrint(szStr);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulRefundTotalAmount), 
												
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulVoidSaleTotalAmount), 

												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
//1010
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulReloadCCTotalAmount)
//1010

												);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"GRAND TOTALS");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                inPrint(szStr);
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
													
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
													
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 

													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
//1010
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount)
//1010

													);		
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 

												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
//1010
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount)
//1010
												);		
		}
	}
    
	//print space one line
    vdCTOS_PrinterFline(6);
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	vdCTOSS_PrinterEnd();
	memset(&strIIT,0x00, sizeof(STRUCT_IIT));//memset as to not carry over IIT values on next host if Settle all is performed.
	return d_OK;	
}

USHORT ushCTOS_printBatchRecordHeader(void)
{
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];


	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    ushCTOS_PrintHeader(0);	

    //printHostLabel();
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
  
	printTIDMID();
    
	printDateTime();
    
	printBatchNO();
    
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    vdPrintCenter("DETAIL REPORT");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    
    //CTOS_PrinterFline(d_LINE_DOT * 1);
    //if(strTCT.fSMMode == TRUE)
    //{
        vdCTOS_PrinterFline(1);
        memset (baTemp, 0x00, sizeof(baTemp));
        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "CARD NAME           CARD NUMBER", &stgFONT_ATTRIB);
        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        inCCTOS_PrinterBufferOutput("CARD NAME              CARD NUMBER",&stgFONT_ATTRIB,1);
        memset (baTemp, 0x00, sizeof(baTemp));
        
        
        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "EXP DATE            TRACE NUMBER", &stgFONT_ATTRIB);
        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        inCCTOS_PrinterBufferOutput("EXP DATE               TRACE NUMBER",&stgFONT_ATTRIB,1);
        memset (baTemp, 0x00, sizeof(baTemp));
    /*}
	else
	{
        vdCTOS_PrinterFline(1);
        memset (baTemp, 0x00, sizeof(baTemp));
        inCCTOS_PrinterBufferOutput("CARD NAME              TRACE NUMBER",&stgFONT_ATTRIB,1);
        memset (baTemp, 0x00, sizeof(baTemp));
	}*/
	
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "TRANSACTION         AMOUNT", &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	inCCTOS_PrinterBufferOutput("TRANSACTION            AMOUNT",&stgFONT_ATTRIB,1);
	memset (baTemp, 0x00, sizeof(baTemp));

	
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "APPROVAL CODE", &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	inCCTOS_PrinterBufferOutput("APPROVAL CODE",&stgFONT_ATTRIB,1);

#if 0	
	memset (baTemp, 0x00, sizeof(baTemp));
	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "CARD NAME                    CARD NUMBER", &stgFONT_ATTRIB);
	CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	memset (baTemp, 0x00, sizeof(baTemp));

	
	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "EXP DATE                     TRACE NUMBER", &stgFONT_ATTRIB);
	CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	memset (baTemp, 0x00, sizeof(baTemp));

	
	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "TRANSACTION                  AMOUNT", &stgFONT_ATTRIB);
	CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	memset (baTemp, 0x00, sizeof(baTemp));

	
	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "APPROVAL CODE", &stgFONT_ATTRIB);
	CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
#endif
	printDividingLine(DIVIDING_LINE);
	return d_OK;
}



USHORT ushCTOS_printBatchRecordFooter(void)
{
	BYTE baTemp[PAPER_X_SIZE * 64];		
	char szStr[d_LINE_SIZE + 1];
	char szPrintBuf[d_LINE_SIZE + 1];
	char szTempBuf1[d_LINE_SIZE + 1];
	char szTempBuf2[d_LINE_SIZE + 1];
	char szTempBuf3[d_LINE_SIZE + 1];
    char szTempBuf4[d_LINE_SIZE + 1];

	int i;

	char szTemp[d_LINE_SIZE + 3];


	DebugAddSTR("ushCTOS_printBatchRecordFooter", srTransRec.szPAN, 10);
	memset(szTempBuf1,0,sizeof(szTempBuf1));
	memset(szTempBuf2,0,sizeof(szTempBuf2));
	memset(szTempBuf4, 0, sizeof(szTempBuf4));
	strcpy(szTempBuf1,srTransRec.szPAN);    
    inIITRead(srTransRec.IITid);
	strcpy(szTempBuf4,strIIT.szIssuerLabel);
#if 0
	if (strlen(szTempBuf1) > 16)	
		cardMasking(szTempBuf1, PRINT_CARD_MASKING_4);
	else
		cardMasking(szTempBuf1, PRINT_CARD_MASKING_3);
#else
	vdCTOS_FormatPAN2(strHDT.szDetailReportMask, srTransRec.szPAN, szTempBuf1);
	if (strlen(srTransRec.szPAN) > 16)
		vdRemoveSpace(szTempBuf2, szTempBuf1);		
	else
		strcpy(szTempBuf2, szTempBuf1);
#endif


    memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
    strcpy(szPrintBuf, szTempBuf4);
    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 23-strlen(szTempBuf4));
    strcat(szPrintBuf, szTempBuf2);
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,szPrintBuf , &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//inPrint(szPrintBuf);
	//if(strTCT.fSMMode == TRUE)
	    inCCTOS_PrinterBufferOutput(szPrintBuf, &stgFONT_ATTRIB, 1);
	
	//Exp date and inv num
	memset(szTempBuf1, 0, sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	memset(szTempBuf3, 0, sizeof(szTempBuf3));
	
	wub_hex_2_str(srTransRec.szExpireDate, szTempBuf1,EXPIRY_DATE_BCD_SIZE);
	DebugAddSTR("detail--EXP",szTempBuf1,12);  
	
	for (i =0; i<4;i++)
		szTempBuf1[i] = '*';
	memcpy(szTempBuf2,&szTempBuf1[0],2);
	memcpy(szTempBuf3,&szTempBuf1[2],2);

	memset(szStr, 0,sizeof(szStr));
	wub_hex_2_str(srTransRec.szInvoiceNo, szStr, INVOICE_BCD_SIZE);
		
	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
    strcpy(szPrintBuf, szTempBuf2);
    strcat(szPrintBuf, szTempBuf3);
    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 23-strlen(szTempBuf2)-strlen(szTempBuf3));
    strcat(szPrintBuf, szStr);
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,szPrintBuf , &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    //if(strTCT.fSMMode == TRUE)
        inPrint(szPrintBuf);
    /*else
    {
        memset(szTempBuf1, 0,sizeof(szTempBuf1));
        sprintf(szTempBuf1, "%-23.23s%s",szTempBuf4, szStr);
        //inPrintLeftRight(szTempBuf4, szStr, 42);
        inPrint(szTempBuf1);
    }*/


	memset(szStr, 0x00,sizeof(szStr));
	memset(szTempBuf1, 0,sizeof(szTempBuf1));
	memset(szTempBuf2, 0,sizeof(szTempBuf2));
	szGetTransTitle(srTransRec.byTransType, szStr);
	wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf1, AMT_BCD_SIZE);
	
	memset(szTemp, 0x00, sizeof(szTemp)); 
	sprintf(szTemp,"%s", szTempBuf1);		

	memset(szTempBuf1, 0, sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTempBuf1);// patrick add code 20141216		
    sprintf(szTempBuf2,"%s%s",strCST.szCurSymbol, szTempBuf1);
    
	/* BDO: Add negative sign to amount on detail report - start -- jzg */
	if((srTransRec.byTransType == VOID) ||
		(srTransRec.byTransType == OFFLINE_VOID))
	{
		memset(szTempBuf3, 0, sizeof(szTempBuf3));
		sprintf(szTempBuf3, "-%s", szTempBuf2);
		memset(szTempBuf2, 0, sizeof(szTempBuf2));
		strcpy(szTempBuf2, szTempBuf3);
	}
	/* BDO: Add negative sign to amount on detail report - end -- jzg */
	
	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	strcpy(szPrintBuf, szStr);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 23-strlen(szStr));
	strcat(szPrintBuf, szTempBuf2);
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,szPrintBuf , &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	inPrint(szPrintBuf);


	
/*	memset(szTempBuf2, 0,sizeof(szTempBuf2));
	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		memcpy(szTempBuf2,"Chip",4);
	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
		memcpy(szTempBuf2,"Manual",6);
	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
		memcpy(szTempBuf2,"Swipe",5);
	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
		memcpy(szTempBuf2,"Fallback",8);
*/

    memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
    strcpy(szPrintBuf, srTransRec.szAuthCode);
    //memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 29-strlen(srTransRec.szAuthCode));
    //strcat(szPrintBuf, szTempBuf2);
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,szPrintBuf , &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	inPrint(szPrintBuf);
	
	//CTOS_PrinterFline(d_LINE_DOT * 1);
    vdCTOS_PrinterFline(1); 
	//CTOS_PrinterFline(d_LINE_DOT * 1);

	
}

USHORT ushCTOS_GetFontInfor(void)
{
	USHORT usASCIIFontID;
	USHORT usFontSize;
	USHORT usFontStyle;

	CTOS_LanguagePrinterGetFontInfo( &usASCIIFontID, &usFontSize, &usFontStyle );   
	vdDebug_LogPrintf("usASCIIFontID[%d]usFontSize[%d]usFontStyle[%d]",usFontSize,usFontStyle );
}

USHORT ushCTOS_ReprintLastSettleReport(void)
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult,inRet;
	int inTranCardType;
	int inReportType;
	int inIITNum , i;
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];


	fRePrintFlag=TRUE;

	//for MP200 no need print
	if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    	return (d_OK);
	
	if( printCheckPaper()==-1)
		return;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;
	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);
	inCSTRead(strHDT.inCurrencyIdx);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_REPRINT_LAST_SETT);
		if(d_OK != inRet)
			return inRet;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return inRet;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return inRet;
		}
	}

    //inResult = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inResult)
    //    return inResult;

	inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	inResult = inCTOS_ReadBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);
	if(inResult == ST_ERROR)
	{
	    //vdSetErrorMessage("NO RECORD FOUND");
	    vdDisplayErrorMsgResp2("","TRANSACTION","NOT FOUND");
		vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		return ST_ERROR;	
	}		 
	else if(inResult == RC_FILE_READ_OUT_NO_DATA)
	{
	    //vdSetErrorMessage("NO RECORD FOUND");
	    vdDisplayErrorMsgResp2("","TRANSACTION","NOT FOUND");
		return;
	}	 
    /*albert - start - 20161202 - Reprint of Detail Report for Last Settlement Report*/   

	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);

	fRePrintFlag=TRUE;
    if(strTCT.fPrintSettleDetailReport == TRUE || srAccumRec.fManualSettlement == TRUE)
        vdCTOS_SettlementPrintDetailReport(TRUE, TRUE, TRUE); //Added 3rd parameter as fix for incorrect batch on reprint last settle
	/*albert - end - 20161202 - Reprint of Detail Report for Last Settlement Report*/
	
    strcpy(srTransRec.szTID, srAccumRec.szTID);
    strcpy(srTransRec.szMID, srAccumRec.szMID);
    memcpy(srTransRec.szYear, srAccumRec.szYear, DATE_BCD_SIZE);
    memcpy(srTransRec.szDate, srAccumRec.szDate, DATE_BCD_SIZE);
    memcpy(srTransRec.szTime, srAccumRec.szTime, TIME_BCD_SIZE);
    //memcpy(srTransRec.szBatchNo, srAccumRec.szBatchNo, BATCH_NO_BCD_SIZE);

	//vdSetPrintThreadStatus(1); /*BDO: Display printing image during report -- sidumili*/
	//vdThreadDisplayPrintBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
	vdPrintReportDisplayBMP();
	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    
	//vdCTOSS_PrinterStart(100);
	//CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
    
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	ushCTOS_PrintHeader(0);	
    
    //vdPrintTitleCenter("LAST SETTLE");
    //vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1);

	//vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);

	//vdPrintCenter("DUPLICATE");
        //vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    
	printTIDMID();
    
	//printDateTime();
    printLastSettleDateTime();
    
	memcpy(srTransRec.szBatchNo, srAccumRec.szBatchNo, BATCH_NO_BCD_SIZE);
	printBatchNO();

	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);

/* BDO PHASE2: [Print Settlement Closed for last settle receipt] -- sidumili*/	
#if 0
	if (srTransRec.byTransType == SETTLE)
		vdPrintCenter("SETTLEMENT CLOSED");
	else
		vdPrintCenter("SUMMARY REPORT");
#else
		 if(srAccumRec.fManualSettlement == TRUE)
            vdPrintCenter("SUMMARY REPORT");
         else
    {
		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		vdPrintCenter("SETTLEMENT CLOSED");		
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		vdPrintCenter("    *** TRANSACTION TOTALS BY ISSUER ***");
    }
#endif
/* BDO PHASE2: [Print Settlement Closed for last settle receipt] -- sidumili*/	

	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    vdCTOS_PrinterFline(1); 
    //CTOS_PrinterFline(d_LINE_DOT * 1);
	for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
	{
		inReportType = PRINT_CARD_TOTAL;
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <20; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount == 0))
					continue;
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				//strcpy(szStr,strIIT.szIssuerLabel);
				sprintf(szStr,"ISSUER: %s",strIIT.szIssuerLabel);
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inPrint(szStr);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount,	 (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulRefundTotalAmount), 
												
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulVoidSaleTotalAmount), 

												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
//1010
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulReloadCCTotalAmount)
//1010

												);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"GRAND TOTALS");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inPrint(szStr);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
													
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
													
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 

													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
//1010
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount)
//1010

													);		
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);
			
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 

												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount)

												);		
		}
	}
	
	//print space one line
    vdCTOS_PrinterFline(6);
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);

	//vdSetPrintThreadStatus(0); /*BDO: Display printing image during report -- sidumili*/
	vdCTOSS_PrinterEnd();

    if(srAccumRec.fManualSettlement == TRUE)
         vdCTOS_PrintManualPosting();
	
	return d_OK;	
}

void vdCTOSS_PrinterBMPPic(unsigned int usX,unsigned int y, const char *path)
{
	char szBmpfile[50+1];
	
	memset(szBmpfile, 0x00, sizeof(szBmpfile));
	//sprintf(szBmpfile,"%s%s", LOCAL_PATH, path);
	sprintf(szBmpfile,"%s", path);
	
	//CTOS_PrinterBMPPic(usX, szBmpfile);

	if(strTCT.byERMMode)
        CTOS_PrinterBMPPic(usX, szBmpfile);
    else		
        vdCTOSS_PrinterBMPPicEx(0, 0, szBmpfile);
	
	return ;
}


//gcitra
extern char szField63[999];

USHORT ushCTOS_PrintCashLoyaltyBody(void)
{	
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;


    	char szTranType[d_LINE_SIZE + 1];


	char temp[30];
	char szRewardText[1000+1];
	char szRewardSize[4+1];
	int inRewardSize;
	char szPrintBuff[21+1];
	int inOffset;
	int inSizeofField63;
	
	char szCI[2+1];
	
	long lnInvNum;
	char temp2[21+1];


	ushCTOS_PrintHeader(d_FIRST_PAGE);

    memset(szTemp1, ' ', d_LINE_SIZE);
    sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    vdPrintCenter(szTemp1);
		
	printDateTime();

    printTIDMID(); 
		
	//printBatchInvoiceNO();
    memset(szTemp1,0x00,sizeof(szTemp1));
    wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
    sprintf(szStr, "INV#: %s", szTemp1);
    memset (baTemp, 0x00, sizeof(baTemp));		
   
   //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
   //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
   inPrint(szStr);

	//Reference num
	memset(szStr, ' ', d_LINE_SIZE);
	memset (baTemp, 0x00, sizeof(baTemp));					
	memset(szStr, ' ', d_LINE_SIZE);
	sprintf(szStr, "RREF NUM  : %s", srTransRec.szRRN);
	memset (baTemp, 0x00, sizeof(baTemp));		
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    inPrint(szStr);


	sprintf(&szRewardSize[0], "%x ", szField63[0]);
	sprintf(&szRewardSize[1], "%02x ", szField63[1]);

	inSizeofField63=atoi(szRewardSize) +2;
	inRewardSize = atoi(szRewardSize) -2;

	inOffset = 0;

	memset(szRewardText, 0x00, sizeof(szRewardText));

	memcpy(szRewardText, (char*) &szField63[4], inRewardSize);

	memset(szCI, 0x00, sizeof(szCI));

	strncpy(szCI, (char*) &szField63[2], 2);

       memset(szTranType,0x00, sizeof(szTranType));
	if ((strcmp(szCI, "PZ") == 0) || (strcmp(szCI, "CI") == 0))
		strcpy(szTranType, "CUSTOMER REWARD");
	else if ((strcmp(szCI, "RN") == 0) || (strcmp(szCI, "CR") == 0))
		strcpy(szTranType, "CASHIER REWARD");
			
    vdCTOS_PrinterFline(1); 
	//CTOS_PrinterFline(d_LINE_DOT * 1); 
  	vdPrintTitleCenter(szTranType);    

       vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        
    vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1); 

	while(1){
		memset(szPrintBuff, 0x00, sizeof(szPrintBuff));

		for(i=0 ; (i < 21) && (inOffset < inRewardSize); i++){
			szPrintBuff[i] = szRewardText[inOffset];
			inOffset++;
		}
	

		memset (baTemp, 0x00, sizeof(baTemp));
		vdPrintCenter(szPrintBuff);

		if (inOffset >= inRewardSize)
			break;
	}

    vdCTOS_PrinterFline(1); 
	//CTOS_PrinterFline(d_LINE_DOT * 1); 
	vdPrintCenter("THANK YOU");

        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOS_PrinterFline(6); 
	//CTOS_PrinterFline(d_LINE_DOT * 6); 

/*second reward*/


	if ((szField63[inRewardSize+4] != (char) 0) || (szField63[inRewardSize+5] != (char) 0)) {


   		CTOS_LCDTClearDisplay();  
		CTOS_LCDTPrintXY(1,4,"Press any key to");
		CTOS_LCDTPrintXY(1,5,"Print");
		sprintf(temp2,"%s",szTranType);
		CTOS_LCDTPrintXY(1,6, temp2);


		WaitKey(60);

      	memset(szTranType, 0x00, sizeof(szTranType));
		if ((strcmp(szCI, "PZ") == 0) || (strcmp(szCI, "CI") == 0))
			strcpy(szTranType, "CUSTOMER REWARD");
		else if ((strcmp(szCI, "RN") == 0) || (strcmp(szCI, "CR") == 0))
			strcpy(szTranType, "CASHIER REWARD");

		ushCTOS_PrintHeader(d_FIRST_PAGE);

    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    	vdPrintCenter(szTemp1);
		
		printDateTime();

       	printTIDMID(); 
		
		//printBatchInvoiceNO();
        memset(szTemp1,0x00,sizeof(szTemp1));
        wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
        sprintf(szStr, "INV#: %s", szTemp1);
        memset (baTemp, 0x00, sizeof(baTemp));		

        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
		inPrint(szStr);


		//Reference num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szStr, "RREF NUM  : %s", srTransRec.szRRN);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        inPrint(szStr);


		sprintf(&szRewardSize[0], "%x ", szField63[0]);
		sprintf(&szRewardSize[1], "%02x ", szField63[1]);

		inSizeofField63=atoi(szRewardSize) +2;
		inRewardSize = atoi(szRewardSize) -2;

		inOffset = 0;

		memset(szRewardText, 0x00, sizeof(szRewardText));

		memcpy(szRewardText, (char*) &szField63[4], inRewardSize);

		memset(szCI, 0x00, sizeof(szCI));

		strncpy(szCI, (char*) &szField63[2], 2);

			
        vdCTOS_PrinterFline(1);
		//CTOS_PrinterFline(d_LINE_DOT * 1); 
  		vdPrintTitleCenter(szTranType);    

       	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        
        vdCTOS_PrinterFline(1);
		//CTOS_PrinterFline(d_LINE_DOT * 1); 


		memset(szRewardSize, 0x00, sizeof(szRewardSize));

		sprintf(&szRewardSize[0], "%x ",szField63[inRewardSize+4]);
		sprintf(&szRewardSize[1], "%02x ", szField63[inRewardSize+5]);

		inOffset = inRewardSize+8;

		inRewardSize = atoi(szRewardSize) - 2;


		memset(szRewardText, 0x00, sizeof(szRewardText));


		memcpy(szRewardText, (char*) &szField63[inOffset], inRewardSize);


       vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        
    vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1); 


	inOffset = 0;

	while(1){
		memset(szPrintBuff, 0x00, sizeof(szPrintBuff));

		for(i=0 ; (i < 21) && (inOffset < inRewardSize); i++){
			szPrintBuff[i] = szRewardText[inOffset];
			inOffset++;
		}
	

		memset (baTemp, 0x00, sizeof(baTemp));
		vdPrintCenter(szPrintBuff);

		if (inOffset >= inRewardSize)
			break;
	}

    vdCTOS_PrinterFline(1); 
	//CTOS_PrinterFline(d_LINE_DOT * 1); 
	vdPrintCenter("THANK YOU");

	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOS_PrinterFline(6);
	//CTOS_PrinterFline(d_LINE_DOT * 6); 


		}
		
	return d_OK;;	
	
}

int inPrintISOPacket(unsigned char *pucTitle,unsigned char *pucMessage, int inLen)
{
    char ucLineBuffer[d_LINE_SIZE+4];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 4];
	

    if (inLen <= 0)
        return(ST_SUCCESS);

	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetWorkTime(50000,1000);
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_16x16,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    
    memset(szStr, 0x00, sizeof(szStr));
	memset(baTemp, 0x00, sizeof(baTemp));
    sprintf(szStr,"[%s] [%d] \n", pucTitle, inLen);
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    //inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB,fERMTransaction);
	vdCTOS_PrinterFline(1);	
    //CTOS_PrinterFline(d_LINE_DOT); 
    
    
    pucBuff = pucMessage + inLen;
    while (pucBuff > pucMessage)
    {
    memset(ucLineBuffer,0x00, sizeof(ucLineBuffer));
    for (inBuffPtr = 0; (inBuffPtr < 44) && (pucBuff > pucMessage); inBuffPtr += 3)
    {
    sprintf(&ucLineBuffer[inBuffPtr], "%02X ", *pucMessage);
    pucMessage++;
    }
    ucLineBuffer[44] = '\n';
    memset (baTemp, 0x00, sizeof(baTemp));		
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, ucLineBuffer, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    //inCCTOS_PrinterBufferOutput(ucLineBuffer, &stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ucLineBuffer, &stgFONT_ATTRIB,fERMTransaction);
    } 
	vdCTOS_PrinterFline(2);
    //CTOS_PrinterFline(d_LINE_DOT * 2); 
    vdCTOSS_PrinterEnd();
    
    return (ST_SUCCESS);
}

/*albert - start - August2014 - manual settlement*/
void vdCTOS_PrintManualPosting(void)
{
	vdCTOSS_PrinterStart(100);

     vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
     vdPrintCenter("FOR MANUAL POSTING");
     vdPrintCenter("PLEASE SUBMIT TO BDO");
     vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
     vdCTOS_PrinterFline(2); 
	 //CTOS_PrinterFline(d_LINE_DOT * 2);
	 
	 //print space one line
     vdCTOS_PrinterFline(6); 
	 //CTOS_PrinterFline(d_LINE_DOT * 2); 	 
	 //CTOS_PrinterFline(d_LINE_DOT * 2);
	 //CTOS_PrinterFline(d_LINE_DOT * 2);
	 
	 vdCTOSS_PrinterEnd();
	 
}
/*albert - end - August2014 - manual settlement*/
int inPrint(unsigned char *strPrint) 
{
	char szStr[42] = {0}; 
	BYTE baTemp[PAPER_X_SIZE * 64] = {0};
    int inSpacing=3;
	
    if(stgFONT_ATTRIB.X_Zoom == DOUBLE_SIZE)
         inSpacing=4;
	
	memset(szStr, 0x00, sizeof(szStr));
	memcpy(szStr, strPrint, strlen(strPrint));

	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//return CTOS_PrinterBufferOutput((BYTE *)baTemp, inSpacing);  
	//inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, 1);
	inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, fERMTransaction);
    return d_OK;
}

int inPrintLeftRight(unsigned char *strLeft, unsigned char *strRight, int inMode) 
{
	char szStr[48+1]; 
    int inLength=0, inSize=0;
    BYTE baTemp[PAPER_X_SIZE * 64];
	int inSpacing=3;
	
    if(stgFONT_ATTRIB.X_Zoom == DOUBLE_SIZE)
         inSpacing=4;

	inLength=inMode;
	
	memset(szStr, 0x20, sizeof(szStr));
	inSize=strlen(strRight);
    memcpy(&szStr[inLength-inSize], strRight, inSize);
	inSize=strlen(strLeft);
    memcpy(szStr, strLeft, strlen(strLeft));
	
    memset (baTemp, 0x00, sizeof(baTemp));
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //return CTOS_PrinterBufferOutput((BYTE *)baTemp, inSpacing);
    //inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB,fERMTransaction);
    return d_OK;
}


int inPrintLeftRight2(unsigned char *strLeft, unsigned char *strRight, int inMode) 
{
	char szStr[48+1]; 
    int inLength=0, inSize=0;
    BYTE baTemp[PAPER_X_SIZE * 64];
	
	//if(inMode == LENGTH_24)
        //inLength=LENGTH_24;
	//else if(inMode == LENGTH_32)
		//inLength=LENGTH_32;
	//else
	    //inLength=LENGTH_41;
	inLength=inMode;
	
	memset(szStr, 0x20, sizeof(szStr));
	inSize=strlen(strRight);
    memcpy(&szStr[inLength-inSize], strRight, inSize);
	inSize=strlen(strLeft);
    memcpy(szStr, strLeft, strlen(strLeft));
	
    memset (baTemp, 0x00, sizeof(baTemp));
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //return CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    //inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB,fERMTransaction);
    return d_OK;
	
}

/***********************************************************************/
//sidumili: Fix on Issue#: 000149 [PRINT RECEIPT ON BALANCE INQ]
/***********************************************************************/
USHORT inCTOS_PrintBalanceInq(void)
{	
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp3[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	char szTemp5[d_LINE_SIZE + 1];
	char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
	int inFmtPANSize;
	BYTE baTemp[PAPER_X_SIZE * 64];
	USHORT result;
	int num,i,inResult;
	unsigned char tucPrint [24*4+1];	
	BYTE   EMVtagVal[64];
	USHORT EMVtagLen; 
	short spacestring;
	BYTE   key;


		char szTranType[d_LINE_SIZE + 1];


	char temp[30];
	char szRewardText[1000+1];
	char szRewardSize[4+1];
	int inRewardSize;
	char szPrintBuff[21+1];
	int inOffset;
	int inSizeofField63;

	char szCI[2+1];

	long lnInvNum;
	char temp2[21+1];

	char szOutAmount[12+1];
	BYTE szDisplayBuf[40];
	BYTE szAmtBuf[40];
	BYTE szBuf[40];
	ULONG  ulAmount;

	if (srTransRec.byTransType != BALANCE_INQUIRY)
		return d_OK;

	if (strTCT.fEnablePrintBalanceInq == FALSE){
		return d_OK;
	}

	//CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
	
	if( printCheckPaper()==-1)
		return d_NO;

	//vdSetPrintThreadStatus(1);
	//vdThreadDisplayPrintBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
	vdPrintReportDisplayBMP();
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
	{
	vdDisplayErrorMsg(1, 8, "Update TCT fail");
	}

	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);  

	
	ushCTOS_PrintHeader(d_FIRST_PAGE);

	printTIDMID(); 

	//card type
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	sprintf(szStr, "CARD TYPE %s", srTransRec.szCardLable);
	vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
	inPrint(szStr);

	//card number
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	memset(szDisplayBuf,0,sizeof(szDisplayBuf));
	vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szDisplayBuf);
	inPrint(szDisplayBuf);
	
	//card holder name
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	printCardHolderName();

	//Trans type
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	szGetTransTitle(srTransRec.byTransType, szStr);     
	inPrint(szStr);    

	#if 0
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
	EMVtagLen = 0;
	memset(szStr, ' ', d_LINE_SIZE);
	vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	//Exp date and Entry mode
	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, 0, sizeof(szTemp));
	memset(szTemp1, 0, sizeof(szTemp1));
	memset(szTemp4, 0, sizeof(szTemp4));
	memset(szTemp5, 0, sizeof(szTemp5));
	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
	DebugAddSTR("EXP",szTemp,12);  

	for (i =0; i<4;i++)
		szTemp[i] = '*';
	memcpy(szTemp4,&szTemp[0],2);
	memcpy(szTemp5,&szTemp[2],2);

	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		memcpy(szTemp1,"Chip",4);
	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
		memcpy(szTemp1,"Manual",6);
	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
		memcpy(szTemp1,"MSR",3);
	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
		memcpy(szTemp1,"Fallback",8);
	else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
	{
		if ('4' ==srTransRec.szPAN[0])
			memcpy(szTemp1,"PayWave",7);
		if ('5' ==srTransRec.szPAN[0])
			memcpy(szTemp1,"PayPass",7);
	}
	#endif

	//date/time
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	printDateTime();

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	memset(szTemp3, ' ', d_LINE_SIZE);

	// Amount
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	memset(szStr, ' ', d_LINE_SIZE);
	memset(szOutAmount, 0x00, sizeof(szOutAmount));
	memset(szBuf, 0x00, sizeof(szBuf));
	memset(szAmtBuf, 0x00, sizeof(szAmtBuf));
	
	memset(szBuf, 0x00, sizeof(szBuf));
	sprintf(szBuf, "%02X%02X%02X%02X%02X%02X", (unsigned char)srTransRec.szBalInqAmount[0], (unsigned char)srTransRec.szBalInqAmount[1], (unsigned char)srTransRec.szBalInqAmount[2], 
																						 (unsigned char)srTransRec.szBalInqAmount[3], (unsigned char)srTransRec.szBalInqAmount[4], (unsigned char)srTransRec.szBalInqAmount[5]);

	if (strTCT.fDemo){
		strcpy(szBuf, "1234567890");
	}
	
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);
	sprintf(szDisplayBuf, "BAL: %s", szAmtBuf);
	inPrint(szDisplayBuf);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    vdCTOS_PrinterFline(6);
	//CTOS_PrinterFline(d_LINE_DOT * 6);
	vdCTOSS_PrinterEnd();
	return d_OK;
	
}
/***********************************************************************/

//sidumili: Issue#000135 [Print Footer Logo]
void vdCTOSS_PrintFooterLogoBMPPic(unsigned int usX,unsigned int y, const char *path)
{
char szBmpfile[50+1];
memset(szBmpfile, 0x00, sizeof(szBmpfile));
sprintf(szBmpfile,"%s%s", PUBLIC_PATH, path);
CTOS_PrinterBMPPic(usX, szBmpfile);
return ;
}

int inCopyLeftRight(unsigned char *strLeft, unsigned char *strRight, int inMode, unsigned char *strDest) 
{
	char szStr[48+1]; 
    int inLength=0, inSize=0;
    BYTE baTemp[PAPER_X_SIZE * 64];
	
	//if(inMode == LENGTH_24)
        //inLength=LENGTH_24;
	//else if(inMode == LENGTH_32)
		//inLength=LENGTH_32;
	//else
	    //inLength=LENGTH_41;
	inLength=inMode;
	
	memset(szStr, 0x20, sizeof(szStr));
	inSize=strlen(strRight);
    memcpy(&szStr[inLength-inSize], strRight, inSize);
	inSize=strlen(strLeft);
    memcpy(szStr, strLeft, strlen(strLeft));

		memcpy(strDest, szStr, sizeof(szStr));
	
}


void vdRemoveSpace(char* szDestination, char* szSource)
{
	int inLen;
	int i = 0;
	int j = 0;

	inLen = strlen(szSource);

	while(i < inLen)
	{
		if(szSource[i] != ' ')
		{
			szDestination[j] = szSource[i];
			j++;
		}
		i++;
	}

}

/* BDOCLG-00134: Reformat cardtype line - start -- jzg */
void vdStrCat2(char szStr1[40], char szStr2[21], short shStr2Pos, char *szOutPut)
{
	char szDisp[41] = {0};
	short shXPos = 0;

	if(shStr2Pos < 19)
		shXPos = 19;
	else
		shXPos = shStr2Pos;

	memset(szDisp, ' ', 40);
	memcpy(szDisp, szStr1, strlen(szStr1));
	memcpy(&szDisp[shStr2Pos], szStr2, strlen(szStr2));

	memcpy(szOutPut, szDisp, strlen(szDisp));
}
/* BDOCLG-00134: Reformat cardtype line - start -- jzg */
void vdCTOS_SettlementPrintDetailReport(BOOL fAddtoSettleReport, BOOL fLastSettleReport, BOOL fReprintLastSettleHeader)
{
	unsigned char chkey;
	short shHostIndex;
	int inResult,i,inCount,inRet;
	int inTranCardType;
	int inReportType;
	int inBatchNumOfRecord;
	int *pinTransDataid;

	//0720
	ACCUM_REC srAccumRec;
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szStr[d_LINE_SIZE + 1];
	//0720

	//for MP200 no need print
		if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
			return (d_OK);

	if( printCheckPaper()==-1)
		return;

	inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	//strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);


    //inResult = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inResult)
    //    return inResult;
    inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);

    if(fLastSettleReport == TRUE)
    {
        inBatchNumOfRecord = inBackupDetailReportNumRecord();
    }
    else
    {
		inResult = inCTOS_ChkBatchEmpty();
		if(d_OK != inResult)
			return;
        inBatchNumOfRecord = inBatchNumRecord();
    }
	
	DebugAddINT("batch record",inBatchNumOfRecord);
	if(inBatchNumOfRecord <= 0)
	{
		vdDisplayErrorMsg(1, 8, "EMPTY BATCH");
		return;
	}

    //vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
    vdPrintReportDisplayBMP();

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
	if(fReprintLastSettleHeader == TRUE)
		ushCTOS_ReprintBatchRecordHeader();// Get ACCUMS data to get correct batch number and date
	else
		ushCTOS_printBatchRecordHeader();

	
	pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
	inCount = 0;		
    if(fLastSettleReport == TRUE)
    {
		inBatchByDetailReport(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, pinTransDataid);
		for(i=0; i<inBatchNumOfRecord; i++)
		{
			inResult = inDatabase_ReadDetailReport(&srTransRec, pinTransDataid[inCount]);
			inCount ++;
			ushCTOS_printBatchRecordFooter();	
		}		
    }
    else
    {
        inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
        for(i=0; i<inBatchNumOfRecord; i++)
        {
            inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
            inCount ++;
            ushCTOS_printBatchRecordFooter();	
        }
    }

    free(pinTransDataid);

	//0720
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if(fLastSettleReport == TRUE)
    {
        inResult = inCTOS_ReadBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);
    }
    else
        inResult = inCTOS_ReadAccumTotal(&srAccumRec);
        
	if(inResult == ST_ERROR)
	{
		vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		return; 	 
	}
	

	    vdCTOS_PrinterFline(2);
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//CTOS_PrinterFline(d_LINE_DOT * 1);

	    memset(baTemp, 0x00, sizeof(baTemp));
		//sprintf(szStr,"CARD TYPE        COUNT         AMT");
		sprintf(szStr,"CARD TYPE        COUNT                 AMT");
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        inPrint(szStr);
		
		for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
		{
				inReportType = DETAIL_REPORT_TOTAL;
		
				for(i= 0; i <20; i ++ )
				{
					vdDebug_LogPrintf("--Count[%d]", i);
					if((srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount == 0)
					&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount == 0)
					&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount == 0)
					&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount == 0)
					&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount == 0))
						continue;
				
					vdDebug_LogPrintf("Count[%d]", i); 
					inIITRead(i);
					memset(szStr, ' ', d_LINE_SIZE);
					memset (baTemp, 0x00, sizeof(baTemp));
	
				
					vdCTOS_PrintAccumeByHostAndCard (inReportType, 
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount,	(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
																
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulRefundTotalAmount), 
																
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulVoidSaleTotalAmount), 
				
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
				
					srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulReloadCCTotalAmount));	
				}
	
				
				printDividingLine(DIVIDING_LINE);
				
				inReportType = DETAIL_REPORT_GRANDTOTAL;
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
																		
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
																		
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 
					
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
					//1010
				srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount));
					//1010
	}
	
	
	//0720


    vdCTOS_PrinterFline(10);
    //CTOS_PrinterFline(d_LINE_DOT * 10);

	
	return;	
}

int inCTOS_PRINT_DETAIL_ALL(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
		
    vdCTOS_TxnsBeginInit();
    inRet = inCTOS_PrintMerchantDetailAllOperation();
    vdCTOS_TransEndReset();

		CTOS_LCDTClearDisplay();
		
    return inRet;
}

int inCTOS_PrintMerchantDetailAllOperation(void)
{
  	int inRet = d_NO;
  	int inNumOfHost = 0,inNum;
  	char szBcd[INVOICE_BCD_SIZE+1];
  	char szErrMsg[30+1];
  	char szAPName[25];
  	int inAPPID;

    inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
    if(d_OK != inRet)
    return inRet;
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        vdCTOS_PrintMerchantDetailReportAll();
        inCTOS_MultiAPALLAppEventID(d_IPC_CMD_DETAIL_REPORT_ALL);
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;
            
            //inRet = inCTOS_MultiAPReloadTable();
            //if(d_OK != inRet)
                //return inRet;
            
            vdCTOS_PrintMerchantDetailReportAll();
        }    
    }
    
    CTOS_LCDTClearDisplay();
    return ST_SUCCESS;
}


void vdCTOS_PrintMerchantDetailReportAll(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,i,inCount,inRet;
    int inTranCardType;
    int inReportType;
    int inBatchNumOfRecord;
    int *pinTransDataid;
    
    ACCUM_REC srAccumRec;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];

		//int inRet = d_NO;
		int inNumOfHost = 0, inNum = 0;
		char szBcd[INVOICE_BCD_SIZE+1] = {0};
		char szErrMsg[31] = {0};
		char szAPName[25] = {0};
		int inAPPID = 0;
    BOOL fPrintDetail=FALSE;
	char szDisplayMsg[20];

		CTOS_LCDTClearDisplay();
		vdDispTitleString("DETAIL REPORT");
			
    if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
        return;
    
    if( printCheckPaper()==-1)
        return;
	else
		vduiClearBelow(2);

   	memset(szAPName,0x00,sizeof(szAPName));
   	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    inNumOfHost = inHDTNumRecord();
    vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            if(strHDT.fHostEnable != TRUE)
						     continue;
			//display-host
			//memset(szDisplayMsg, 0x00, sizeof(szDisplayMsg));
			//sprintf(szDisplayMsg,"%s",strHDT.szHostLabel); 
			//vdCTOS_DispStatusMessage(szDisplayMsg);
			//end
						
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
            if (strcmp(szAPName, strHDT.szAPName)!=0)
                continue;
						else
						{
						
							//SIT-start
							memset(szDisplayMsg, 0x00, sizeof(szDisplayMsg));
							sprintf(szDisplayMsg,"%s",strHDT.szHostLabel); 
							vdCTOS_DispStatusMessage(szDisplayMsg);
							CTOS_Delay(500); 
							//end
							srTransRec.HDTid=strHDT.inHostIndex;
							
							inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);
							memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
							
							inBatchNumOfRecord = inBatchNumRecord();
							
							DebugAddINT("batch record",inBatchNumOfRecord);
							if(inBatchNumOfRecord <= 0)
							{
								vdDisplayErrorMsg(1, 8, "EMPTY BATCH, SKIP");
								continue;
							}

							vdCTOS_PrintMerchantDetailReport();
							
							CTOS_LCDTClearDisplay();
							vdDispTitleString("DETAIL REPORT");
						}
        }
    }
    return;	
}

void vdCTOS_PrintMerchantDetailReport(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,i,inCount,inRet;
    int inTranCardType;
    int inReportType;
    int inBatchNumOfRecord;
    int *pinTransDataid;
    
    ACCUM_REC srAccumRec;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];

		//int inRet = d_NO;
		int inNumOfHost = 0, inNum = 0;
		char szBcd[INVOICE_BCD_SIZE+1] = {0};
		char szErrMsg[31] = {0};
		char szAPName[25] = {0};
		int inAPPID = 0;
    BOOL fPrintDetail=FALSE;

    //vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
    vdPrintReportDisplayBMP();

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
    ushCTOS_printBatchRecordHeader();

    inBatchNumOfRecord = inBatchNumRecord();
		
    pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
    inCount = 0;		
    inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
    for(i=0; i<inBatchNumOfRecord; i++)
    {
        inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        inCount ++;
        ushCTOS_printBatchRecordFooter();	
    }

	free(pinTransDataid);

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return; 	 
    }


    vdCTOS_PrinterFline(2);
    //CTOS_PrinterFline(d_LINE_DOT * 1);
    //CTOS_PrinterFline(d_LINE_DOT * 1);
    
    memset(baTemp, 0x00, sizeof(baTemp));
    //sprintf(szStr,"CARD TYPE        COUNT         AMT");
    sprintf(szStr,"CARD TYPE        COUNT              AMT");
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    inPrint(szStr);

    for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
    {
        inReportType = DETAIL_REPORT_TOTAL;
        
        for(i= 0; i <30; i ++ )
        {
            vdDebug_LogPrintf("--Count[%d]", i);
            if((srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount == 0)
            &&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount == 0)
            &&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount == 0)
            &&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount == 0))
                continue;
            
            vdDebug_LogPrintf("Count[%d]", i); 
            inIITRead(i);
            memset(szStr, ' ', d_LINE_SIZE);
            memset (baTemp, 0x00, sizeof(baTemp));
            
            inCSTRead(strHDT.inCurrencyIdx);
            vdCTOS_PrintAccumeByHostAndCard (inReportType, 
            srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount,	(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
            srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulRefundTotalAmount), 
            srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulVoidSaleTotalAmount), 
            srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
            srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulReloadCCTotalAmount));	
        }
			
        printDividingLine(DIVIDING_LINE);
        
        inReportType = DETAIL_REPORT_GRANDTOTAL;
        
        vdCTOS_PrintAccumeByHostAndCard (inReportType, 
        srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
        srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
        srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 
        srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
        srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount));
    }
		
        vdCTOS_PrinterFline(10);    
		//CTOS_PrinterFline(d_LINE_DOT * 10);
			
    return;	
}

int inCTOS_PRINT_SUMMARY_ALL(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
		
    vdCTOS_TxnsBeginInit();
    inRet = inCTOS_PrintMerchantSummaryAllOperation();
    vdCTOS_TransEndReset();

		CTOS_LCDTClearDisplay();
		
    return inRet;
}

int inCTOS_PrintMerchantSummaryAllOperation(void)
{
  	int inRet = d_NO;
  	int inNumOfHost = 0,inNum;
  	char szBcd[INVOICE_BCD_SIZE+1];
  	char szErrMsg[30+1];
  	char szAPName[25];
  	int inAPPID;

    inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        vdCTOS_PrintMerchantSummaryReportAll();
        inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SUMMARY_REPORT_ALL);
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;
            
            //inRet = inCTOS_MultiAPReloadTable();
            //if(d_OK != inRet)
                //return inRet;
            
            vdCTOS_PrintMerchantSummaryReportAll();
        }    
    }
    
    CTOS_LCDTClearDisplay();
    return ST_SUCCESS;
}


void vdCTOS_PrintMerchantSummaryReportAll(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,i,inCount,inRet;
    int inTranCardType;
    int inReportType;
    int inBatchNumOfRecord;
    int *pinTransDataid;
    
    ACCUM_REC srAccumRec;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];

		//int inRet = d_NO;
		int inNumOfHost = 0, inNum = 0;
		char szBcd[INVOICE_BCD_SIZE+1] = {0};
		char szErrMsg[31] = {0};
		char szAPName[25] = {0};
		int inAPPID = 0;
    BOOL fPrintDetail=FALSE;
	char szDisplayMsg[20];

		CTOS_LCDTClearDisplay();
		vdDispTitleString("SUMMARY REPORT");
			
    if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
        return;
    
    if( printCheckPaper()==-1)
        return;
	else
		vduiClearBelow(2);

   	memset(szAPName,0x00,sizeof(szAPName));
   	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    inNumOfHost = inHDTNumRecord();
    vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            if(strHDT.fHostEnable != TRUE)
						     continue;

			//display-host
			//memset(szDisplayMsg, 0x00, sizeof(szDisplayMsg));
			//sprintf(szDisplayMsg,"%s",strHDT.szHostLabel); 
			//vdCTOS_DispStatusMessage(szDisplayMsg);
			//end
						
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
            if (strcmp(szAPName, strHDT.szAPName)!=0)
                continue;
						else
						{
						
							//SIT-start
							memset(szDisplayMsg, 0x00, sizeof(szDisplayMsg));
							sprintf(szDisplayMsg,"%s",strHDT.szHostLabel); 
							vdCTOS_DispStatusMessage(szDisplayMsg);
							CTOS_Delay(500); 
							//end
							srTransRec.HDTid=strHDT.inHostIndex;
							
							inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);
							memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
							
							inBatchNumOfRecord = inBatchNumRecord();
							
							DebugAddINT("batch record",inBatchNumOfRecord);
							if(inBatchNumOfRecord <= 0)
							{
							
								vdDisplayErrorMsg(1, 8, "EMPTY BATCH, SKIP");
								continue;
							}

							vdCTOS_PrintMerchantSummaryReport();
							
							CTOS_LCDTClearDisplay();
							vdDispTitleString("SUMMARY REPORT");
						}
        }
    }
    return;	
}

void vdCTOS_PrintMerchantSummaryReport(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,i,inCount,inRet;
    int inTranCardType;
    int inReportType;
    int inBatchNumOfRecord;
    int *pinTransDataid;
    
    ACCUM_REC srAccumRec;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];
    
    //int inRet = d_NO;
    int inNumOfHost = 0, inNum = 0;
    char szBcd[INVOICE_BCD_SIZE+1] = {0};
    char szErrMsg[31] = {0};
    char szAPName[25] = {0};
    int inAPPID = 0;
    BOOL fPrintDetail=FALSE;

		int inPADSize,x;
		char szSPACE[40];
		char szBuff[46];
		char szBuff2[46];

    //vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
    vdPrintReportDisplayBMP();

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
	{
		vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		return;    
	}

	//vdSetPrintThreadStatus(1); /*BDO: Display printing image during report -- sidumili*/
	//vdThreadDisplayPrintBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
	vdPrintReportDisplayBMP();
	
	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
	
	ushCTOS_PrintHeader(0); 
	
	//vdPrintTitleCenter("SUMMARY REPORT");
    vdCTOS_PrinterFline(1); 
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

	//printHostLabel();
	
	printTIDMID();
	
	printDateTime();
	
	printBatchNO();

    vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	vdPrintCenter("    *** TRANSACTION TOTALS BY ISSUER ***");
    vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1);

    vdCTOS_PrinterFline(1);
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
	{
		inReportType = PRINT_CARD_TOTAL;
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <20; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount == 0)			
				&&(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount == 0))
					continue;
				
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				//strcpy(szStr,strIIT.szIssuerLabel);

				//sprintf(szBuff,"HOST%s%s",szSPACE,srTransRec.szHostLabel);
				sprintf(szStr,"ISSUER: %s",strIIT.szIssuerLabel);

				sprintf(szBuff,"HOST             %s",srTransRec.szHostLabel);
				
				
				inPrintLeftRight2(szStr,szBuff,41);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount,	(srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulRefundTotalAmount), 
												
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulVoidSaleTotalAmount), 

												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulOffSaleTotalAmount),
//1010
												srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stCardTotal[i].ulReloadCCTotalAmount)
//1010
												);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"TOTAL:");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                inPrint(szStr);

				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
													
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
													
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 

													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
//1010
													srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount)
//1010

													);		
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            inPrint(szStr);
			
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulRefundTotalAmount), 
												
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulVoidSaleTotalAmount), 

												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulOffSaleTotalAmount),
//1010
												srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.usReloadCCCount, (srAccumRec.stBankTotal[DEBIT_CARD].stHOSTTotal.ulReloadCCTotalAmount)
//1010

												);		
		}
	}
	
	//print space one line
    vdCTOS_PrinterFline(6);
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);

	//vdSetPrintThreadStatus(0); /*BDO: Display printing image during report -- sidumili*/
	
	return; 

}

void SetReportTime(void)
{
	
  	BYTE szCurrTime[7] = {0};
  	CTOS_RTC rtcClock;
  	CTOS_RTC SetRTC;


	CTOS_RTCGet(&SetRTC);

    sprintf(szCurrTime,"%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
	wub_str_2_hex(szCurrTime,srTransRec.szTime,6);

}

void printCardHolderNameV10(void)	// v10.0 change
{
    USHORT shLen;
    char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];

	vdTrimSpaces(srTransRec.szCardholderName);
	
    shLen = strlen(srTransRec.szCardholderName);

    if(shLen > 0)
    {
        memset(szStr, 0x00, d_LINE_SIZE);
		if (shLen > 30) 	
			memcpy(szStr, srTransRec.szCardholderName, CARD_HOLD_NAME_DIGITS); 
		else
			memcpy(szStr, srTransRec.szCardholderName, shLen); 

        //while(0x20 == szStr[--shLen] && shLen >= 0)
        //    szStr[shLen] = 0x00;

        //if (shLen > 20){	
			//vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
			vdPrintCenter(szStr);
			//vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
			
		//}else{
		//	vdPrintCenter(szStr);
		//}
    }
}

USHORT ushCTOS_PrintSMHeader(int page)
{	
	char szBmpfile[50+1];
	memset(szBmpfile, 0x00, sizeof(szBmpfile));

	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel); 
	//print Logo	
	sprintf(szBmpfile,"%s%s", LOCAL_PATH, strTCT.szSMReceiptLogo);
    vdCTOSS_PrinterBMPPicEx(0,0,szBmpfile);
	//vdCTOSS_PrinterBMPPic(0, 0, strTCT.szSMReceiptLogo);

	if(fRePrintFlag == TRUE)
    {   
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
		if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
        {
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctHdr1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctHdr1);
    }
  //aaronnino for BDOCLG ver 9.0 fix on issue #00085 All Header lines are not printing on the first host info report start
	if(strlen(strMMT[0].szRctHdr1) <= 0)
		 inMMTReadRecord(1,1);
	//aaronnino for BDOCLG ver 9.0 fix on issue #00085 All Header lines are not printing on the first host info report end
	
    vdPrintHeaderText();

	//issue-00420
	if (strlen(strTCT.szAppVersionHeader) > 0)
		vdPrintCenter(strTCT.szAppVersionHeader);

    if(fRePrintFlag == TRUE)
    {
        vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
        vdPrintCenterReverseMode("DUPLICATE");
    }

    if(VS_TRUE == strTCT.fDemo)
    {
        ushCTOS_PrintDemo();
    }

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    vdCTOSS_PrinterEnd();
	
	return d_OK;
}

USHORT ushCTOS_PrintSMBody(int page)
{	
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];	
	char szTemp3[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	char szTemp5[d_LINE_SIZE + 1];
	char szTempDate[d_LINE_SIZE + 1];
	char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
	int inFmtPANSize;
	BYTE baTemp[PAPER_X_SIZE * 64];
	USHORT result;
	int num,i,inResult;
	unsigned char tucPrint [24*4+1];	
	BYTE   EMVtagVal[64+1];
	USHORT EMVtagLen; 
	short spacestring;
	BYTE   key;

	char szAmountBuff[20+1], szTemp6[47+1];

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	
    printDateTime();
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "MERCHANT ID  : %s", srTransRec.szMID);
	inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "TERMINAL ID  : %s", srTransRec.szTID);
	inPrint(szStr);
	
	memset(szTemp,0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "BATCH NUM    : %s", szTemp);
	inPrint(szStr);
		
	memset(szTemp,0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szInvoiceNo,szTemp,3);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "INVOICE NUM  : %s", szTemp);
	inPrint(szStr);
	
	szGetTransTitle(srTransRec.byTransType, szStr);  
	//gcitra-012315
	if(srTransRec.byTransType == SALE){
		memset(szStr, 0x00, sizeof(szStr));
		if(srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		    strcpy(szStr,"Contactless Sale");
		else
		    strcpy(szStr,"Sale");
	}else if(srTransRec.byTransType == VOID){
		memset(szStr, 0x00, sizeof(szStr));
		if(srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		    strcpy(szStr,"Contactless Void");
		else
			strcpy(szStr,"Void Sale");
	}
	
    //vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	if(srTransRec.byTransType != VOID)
        //vdPrintTitleCenter(szStr);
        vdPrintCenter(szStr);
	else
	    vdPrintCenterReverseMode(szStr);
	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
	EMVtagLen = 0;
	memset(szStr, ' ', d_LINE_SIZE);
	vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
	
    memset(szTemp, 0x00, sizeof(szTemp)); 
    if(page == BANK_COPY_RECEIPT) /*bank copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskBankCopy, srTransRec.szPAN, szTemp);
    else if(page == CUSTOMER_COPY_RECEIPT) /*customer copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp);		
    else if(page == MERCHANT_COPY_RECEIPT) /*merchant copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp);

	memset(szTemp2, 0x00, sizeof(szTemp2)); 
	sprintf(szTemp2, "CARD NUM     : %s", szTemp);
	
	memset(szTemp3, 0x00, sizeof(szTemp3));
	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		strcpy(szTemp3,"C");
	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
		strcpy(szTemp3,"M");
	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
		strcpy(szTemp3,"S");
	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK){
		if((srTransRec.szServiceCode[0] == '2') || (srTransRec.szServiceCode[0] == '6'))
			strcat(szTemp3, "F");  
		else			
			strcat(szTemp3, "S");
	}
	inPrintLeftRight(szTemp2, szTemp3, 42);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	memset(szTemp, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);

	sprintf(szStr, "CARD TYPE    : DEBIT");
	//inPrint(szStr);
	
	if(srTransRec.inAccountType == SAVINGS_ACCOUNT)
		sprintf(szTemp2, "*SAVINGS*");
	else if(srTransRec.inAccountType == CURRENT_ACCOUNT)
		sprintf(szTemp2, "*CURRENT*");
	
	inPrintLeftRight(szStr, szTemp2, 42);
	
	
	//aaronnino for BDOCLG ver 9.0 fix on issue #00066 "APPR.CODE" single width printed on transaction receipt 6 of 8 start
    memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr, "APPR CODE    : %s", srTransRec.szAuthCode); 
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "REF NUM      : %s", srTransRec.szRRN);
	inPrint(szStr);

	memset(szTemp, 0x00, d_LINE_SIZE);
	memset(szTemp1, 0x00, d_LINE_SIZE);
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
	
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	memset(szAmountBuff,0x00,sizeof(szAmountBuff));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	
	memset(szStr,0x00, sizeof(szStr));
	strcpy(szStr,"AMOUNT");
	
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp2);		
	if (srTransRec.byTransType == VOID)
		sprintf(szAmountBuff,"-%s %s", strCST.szCurSymbol, szTemp2);
	else
		sprintf(szAmountBuff,"%s %s", strCST.szCurSymbol, szTemp2);
	
	inPrintLeftRight(szStr,szAmountBuff,24);
	//CTOS_PrinterFline(d_LINE_DOT );

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
	/* EMV: Revised EMV details printing - start -- jzg */
	(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
	(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
	(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
	(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC))
	/* EMV: Revised EMV details printing - end -- jzg */
	{
        vdCTOS_PrinterFline(1);
		//CTOS_PrinterFline(d_LINE_DOT);
		//APP
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szStr, "APP.         : %s", srTransRec.stEMVinfo.szChipLabel);
		inPrint(szStr);
		
		//AC
		wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
		sprintf(szStr, "TC           : %s", szTemp);
		inPrint(szStr);
		
		//AID
		memset(szStr, ' ', d_LINE_SIZE);
		EMVtagLen = srTransRec.stEMVinfo.T84_len;
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
		memset(szTemp, ' ', d_LINE_SIZE);
		wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
		sprintf(szStr, "AID          : %s",szTemp);
		inPrint(szStr);			  
	}
	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    vdCTOS_PrinterFline(1); 
	//CTOS_PrinterFline(d_LINE_DOT );

	return d_OK;
}

USHORT ushCTOS_PrintSMFooter(int page)
{		
	BYTE EMVtagVal[64] = {0};
	USHORT EMVtagLen = 0; 
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 3];

    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    EMVtagLen = 0;
    
    DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
    
    vdDebug_LogPrintf("CVM [%02X %02X %02X]",
    srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
    vdDebug_LogPrintf("srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);
    
#if 0    
    if ((srTransRec.byEntryMode == CARD_ENTRY_ICC)||(srTransRec.byEntryMode == CARD_ENTRY_WAVE))
    {
        EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
        
        //EMV: Fix for noCVM during above floor limit scenario - start -- jzg
        vdDebug_LogPrintf("EMVtagVal [%02X %02X %02X]", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
        
        // patrick add code 20141208
        if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
        (EMVtagVal[0] != 0x1E) &&
        (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
        //EMV: Fix for noCVM during above floor limit scenario - end -- jzg
        {
            if (memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x03\x33",5) == 0)
            {
                //for CUP must signature
                if((page == BANK_COPY_RECEIPT) || (page == MERCHANT_COPY_RECEIPT)) /*bank and merchant copy*/
                {
                    CTOS_PrinterFline(d_LINE_DOT * 2);
                    #if 0	// v10.0 change
                    CTOS_PrinterPutString("SIGN:__________________________");
                    #else
                    CTOS_PrinterPutString("_______________________________");
                    #endif
                    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                    printCardHolderNameV10();//printCardHolderName();// v10.0 change
                }
                else
                    CTOS_PrinterFline(d_LINE_DOT );
            }
            else
            {
                CTOS_PrinterFline(d_LINE_DOT);
                CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
                if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
                    CTOS_PrinterPutString(" 	 (PIN VERIFY SUCCESS)");
            }
        }
        else
        {
            //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 7 of 13
            if((page == BANK_COPY_RECEIPT) || (page == MERCHANT_COPY_RECEIPT)) /*bank and merchant copy*/
            {
                CTOS_PrinterFline(d_LINE_DOT * 2);
                #if 0	// v10.0 change
                CTOS_PrinterPutString("SIGN:__________________________");
                #else
                CTOS_PrinterPutString("_______________________________");
                #endif
                vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                printCardHolderNameV10();//printCardHolderName();// v10.0 change
            }
            else
               CTOS_PrinterFline(d_LINE_DOT );
            //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 7 of 13
        }
    }
    else
    {
        if((page == BANK_COPY_RECEIPT) || (page == MERCHANT_COPY_RECEIPT)) /*bank and merchant copy*/
        {
            CTOS_PrinterFline(d_LINE_DOT * 2);
            #if 0	// v10.0 change
            CTOS_PrinterPutString("SIGN:__________________________");
            #else
            CTOS_PrinterPutString("_______________________________");
            #endif
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            printCardHolderNameV10();//printCardHolderName();// v10.0 change
        }
        else
            CTOS_PrinterFline(d_LINE_DOT );
    }
#endif
    
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    //CTOS_PrinterFline(d_LINE_DOT); 
    //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 8 of 13
    if(page == BANK_COPY_RECEIPT)
        vdPrintCenter("--- BANK COPY ---");
    else if(page == CUSTOMER_COPY_RECEIPT)
        vdPrintCenter("--- CUSTOMER COPY ---");
    else if(page = MERCHANT_COPY_RECEIPT)
        vdPrintCenter("--- MERCHANT COPY ---");
    //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 8 of 13

#if 0
    if(page != CUSTOMER_COPY_RECEIPT)
    {
        if(srTransRec.byTransType != VOID && srTransRec.byTransType != SALE_TIP)   
        {
            ushCTOS_PrintAgreement();
        }
    }
#endif

    vdPrintFooterText();

    /***************************************************/
    //sidumili: Issue#000135 [Print Footer Logo]
    if (srTransRec.fFooterLogo)
    {
		vdCTOSS_PrinterBMPPicEx(0,0,"/home/ap/pub/footer.bmp");
        //vdCTOSS_PrintFooterLogoBMPPic(0, 0, "footer.bmp");
    }
    /***************************************************/

	vdLineFeed(srTransRec.fFooterLogo);
	
	return d_OK;	
}

void vdCTOS_PrintReceiptCopies(BOOL BankCopy, BOOL CustCopy, BOOL MercCopy, BOOL fERM)

{
    BOOL fFirstReceipt=TRUE; 
    int inReceiptCtr=0;
    BYTE szPrintOption[3+1];
    BYTE chPrintOption=0x00;
	BYTE  key;
	ULONG ulFileSize;
	
    memset(szPrintOption, 0x00, sizeof(szPrintOption)); 
    memcpy(szPrintOption, strIIT.szPrintOption, 3); /*get sequence from IIT*/
    
	inReceiptCtr=0;
	fFirstReceipt=TRUE;
    do
    {
        chPrintOption=(unsigned char)szPrintOption[inReceiptCtr];
        if((chPrintOption == '1') && (BankCopy == TRUE)) /*bank copy*/
        {
			if(fFirstReceipt == TRUE)
			{
				ushCTOS_printAll(BANK_COPY_RECEIPT);
				if(fERM == TRUE)
					inCTOSS_ERM_Form_Receipt(0);
				CTOS_KBDBufFlush();	
				fFirstReceipt=FALSE;
			}
			else
			{
				CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
				if(strTCT.fPrintReceiptPrompt == TRUE)
				{
					vduiDisplayStringCenter(7,"PRINT BANK COPY?");
					vduiDisplayStringCenter(8,"NO[X] YES[OK]");
				}
				else
				{
					vduiDisplayStringCenter(7,"PRINT BANK COPY");
					vduiDisplayStringCenter(8,"PRESS ENTER TO PRINT");
				}
	                while(1)
	                { 
	                    if (strTCT.fPrintWarningSound == TRUE)
	                    vduiWarningSound();
	                    
	                    CTOS_KBDHit(&key);
	                    if(key == d_KBD_ENTER)
	                    {
	                        ushCTOS_printAll(BANK_COPY_RECEIPT);
                            if(fERM == TRUE)
                                inCTOSS_ERM_Form_Receipt(0);
	                        CTOS_KBDBufFlush();//cleare key buffer
	                        break;
	                    }
	                    else if(key == d_KBD_CANCEL && strTCT.fPrintReceiptPrompt == TRUE)
	                        break;
	                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
	                    {
							ushCTOS_printAll(BANK_COPY_RECEIPT);
	                        break;
	                    }
	                }
			}
        }
        else if((chPrintOption == '2') && (CustCopy == TRUE)) /*customer copy*/
        {
			if(fFirstReceipt == TRUE)
			{
				ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
                if(fERM == TRUE)
                    inCTOSS_ERM_Form_Receipt(0);
				CTOS_KBDBufFlush();//cleare key buffer	
				fFirstReceipt=FALSE;
			}
			else
			{
				CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
				if(strTCT.fPrintReceiptPrompt == FALSE && strIIT.fNSRCustCopy == FALSE)
				{
					vduiDisplayStringCenter(7,"PRINT CUSTOMER COPY");
					vduiDisplayStringCenter(8,"PRESS ENTER TO PRINT");
				}
				else
				{
					vduiDisplayStringCenter(7,"PRINT CUSTOMER COPY?");
					vduiDisplayStringCenter(8,"NO[X] YES[OK]");
				}
                while(1)
                { 
                    if (strTCT.fPrintWarningSound == TRUE)
                    vduiWarningSound();
                    
                    CTOS_KBDHit(&key);
                    if(key == d_KBD_ENTER)
                    {
                        ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
                        if(fERM == TRUE)
                            inCTOSS_ERM_Form_Receipt(0);						
                        CTOS_KBDBufFlush();//cleare key buffer
                        break;
                    }
                    else if(key == d_KBD_CANCEL)
                    {
						if(strTCT.fPrintReceiptPrompt == TRUE || (strTCT.fPrintReceiptPrompt == FALSE && strIIT.fNSRCustCopy))
	                        break;
                    }	
                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                    {
						ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
                        break;
                    }
                }
			}
			
        }  	
        else if((chPrintOption == '3') && (MercCopy== TRUE)) /*merchant copy*/
        {
			if(fFirstReceipt == TRUE)
			{
				ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
                if(fERM == TRUE)
                    inCTOSS_ERM_Form_Receipt(0);
				CTOS_KBDBufFlush();//cleare key buffer	
				fFirstReceipt=FALSE;
			}
			else
			{
				CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
				if(strTCT.fPrintReceiptPrompt == TRUE)
				{
					vduiDisplayStringCenter(7,"PRINT MERCHANT COPY?");
					vduiDisplayStringCenter(8,"NO[X] YES[OK]");
				}
				else
				{
					vduiDisplayStringCenter(7,"PRINT MERCHANT COPY");
					vduiDisplayStringCenter(8,"PRESS ENTER TO PRINT");
				}
                while(1)
                { 
                    if (strTCT.fPrintWarningSound == TRUE) /*BDO PHASE 2: [Warning sound for printing flag] -- sidumili*/
                    vduiWarningSound();
                    
                    CTOS_KBDHit(&key);
                    if(key == d_KBD_ENTER)
                    {
                        ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
                        if(fERM == TRUE)
                            inCTOSS_ERM_Form_Receipt(0);						
                        CTOS_KBDBufFlush();//cleare key buffer
                        break;
                    }
                    else if(key == d_KBD_CANCEL && strTCT.fPrintReceiptPrompt == TRUE)
                        break;
                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                    {
						ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
                        break;
                    }
                }
			}
        }  		
		inReceiptCtr++; 
    }while(inReceiptCtr < 3);	

    if(fERM == TRUE)
    {
        if(strTCT.fSignatureFlag == TRUE)
        {
            ulFileSize=lnGetFileSize(DB_SIGN_BMP);
            if(ulFileSize > 0)
                vdLinuxCommandFileCopy();
        }
    }
	
}

void vdLineFeed(BOOL fFooterLogo)
{
    int inLineFeed;
    
	
	if(strTCT.byTerminalModel == V3_DESKTOP)
        inLineFeed=((fFooterLogo)?6:7);
    else if(strTCT.byTerminalModel == V3_MOBILE)
		inLineFeed=((fFooterLogo)?2:3);
	else
		inLineFeed=((fFooterLogo)?4:5);//for V5
    
    vdCTOS_PrinterFline(inLineFeed); 
	//CTOS_PrinterFline(d_LINE_DOT * inLineFeed); 
}

void vdPrintHeaderText(void)
{

    char szPOSTID[40];
	char szPOSTIDHeader[40];

	vdDebug_LogPrintf("--vdPrintHeaderText--");
	vdDebug_LogPrintf("strNMT[0].NMTID[%d] srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctHdr1[%s] strMMT[0].szRctHdr2[%s] strMMT[0].szRctHdr3[%s] strMMT[0].szRctHdr4[%s] strMMT[0].szRctHdr5[%s]",strNMT[0].NMTID, srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctHdr1, strMMT[0].szRctHdr2, strMMT[0].szRctHdr3, strMMT[0].szRctHdr4, strMMT[0].szRctHdr5);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	
	memset(szPOSTID, 0x00, 40);
	
	memset(szPOSTIDHeader, 0x00, 40);
    #ifdef SINGE_HEADER_FOOTER_TEXT
        //inNMTReadRecord(srTransRec.MITid);

		if (srTransRec.MITid > 0)
			inNMTReadRecord(srTransRec.MITid);		
		else	
			inNMTReadRecord(strNMT[0].NMTID);

            vdPrintCenter(strSingleNMT.szMerchName);
        if(strlen(strSingleNMT.szRctHdr1) > 0)
            vdPrintCenter(strSingleNMT.szRctHdr1);
        if(strlen(strSingleNMT.szRctHdr2) > 0)
            vdPrintCenter(strSingleNMT.szRctHdr2);
        if(strlen(strSingleNMT.szRctHdr3) > 0)
            vdPrintCenter(strSingleNMT.szRctHdr3);
        if(strlen(strSingleNMT.szRctHdr4) > 0)
            vdPrintCenter(strSingleNMT.szRctHdr4);
        if(strlen(strSingleNMT.szRctHdr5) > 0)
            vdPrintCenter(strSingleNMT.szRctHdr5);

		if (srTransRec.fECRTriggerTran == TRUE	&& strTCT.fSMMode == TRUE){
			//inCTOSS_GetEnvDB("PTID", szPOSTID);
	
			if (strlen(srTransRec.szPOSTID)> 0){
				sprintf(szPOSTIDHeader,"POS TID %s", srTransRec.szPOSTID);
				vdPrintCenter(szPOSTIDHeader);
			}
		}
		else if(strTCT.fSMMode == TRUE)
		{
			memset(szPOSTID,0x00,sizeof(szPOSTID));
			inCTOSS_GetEnvDB("PTID", szPOSTID);
			if (strlen(szPOSTID)> 0)
			{
				sprintf(szPOSTIDHeader,"POS TID %s", szPOSTID);
				vdPrintCenter(szPOSTIDHeader);
			}
		}

    #else
        vdPrintCenter(strMMT[0].szMerchantName);
        if(strlen(strMMT[0].szRctHdr1) > 0)
            vdPrintCenter(strMMT[0].szRctHdr1);
        if(strlen(strMMT[0].szRctHdr2) > 0)
            vdPrintCenter(strMMT[0].szRctHdr2);
        if(strlen(strMMT[0].szRctHdr3) > 0)
            vdPrintCenter(strMMT[0].szRctHdr3);
        if(strlen(strMMT[0].szRctHdr4) > 0)
            vdPrintCenter(strMMT[0].szRctHdr4);
        if(strlen(strMMT[0].szRctHdr5) > 0)
            vdPrintCenter(strMMT[0].szRctHdr5);

		if (srTransRec.fECRTriggerTran == TRUE	&& strTCT.fSMMode == TRUE){
			//inCTOSS_GetEnvDB("PTID", szPOSTID);
	
			if (strlen(srTransRec.szPOSTID)> 0){
				sprintf(szPOSTIDHeader,"POS TID %s", srTransRec.szPOSTID);
				vdPrintCenter(szPOSTIDHeader);
			}
		}
		else if(strTCT.fSMMode == TRUE)
		{
			memset(szPOSTID,0x00,sizeof(szPOSTID));
			inCTOSS_GetEnvDB("PTID", szPOSTID);
			if (strlen(szPOSTID)> 0)
			{
				sprintf(szPOSTIDHeader,"POS TID %s", szPOSTID);
				vdPrintCenter(szPOSTIDHeader);
			}
		}		

    #endif	

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	
}

void vdPrintFooterText(void)
{
    #ifdef SINGE_HEADER_FOOTER_TEXT
        if(strlen(strSingleNMT.szRctFoot1) > 0)
            vdPrintCenter(strSingleNMT.szRctFoot1);
        if(strlen(strSingleNMT.szRctFoot2) > 0)
            vdPrintCenter(strSingleNMT.szRctFoot2);
        if(strlen(strSingleNMT.szRctFoot3) > 0)
            vdPrintCenter(strSingleNMT.szRctFoot3); 
    #else
        if(strlen(strMMT[0].szRctFoot1) > 0)
            vdPrintCenter(strMMT[0].szRctFoot1);
        if(strlen(strMMT[0].szRctFoot2) > 0)
            vdPrintCenter(strMMT[0].szRctFoot2);
        if(strlen(strMMT[0].szRctFoot3) > 0)
            vdPrintCenter(strMMT[0].szRctFoot3);
    #endif	
}


USHORT ushCTOS_PrintSMBodyError(int page)
{	
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];	
	char szTemp3[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	char szTemp5[d_LINE_SIZE + 1];
	char szTempDate[d_LINE_SIZE + 1];
	char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
	int inFmtPANSize;
	BYTE baTemp[PAPER_X_SIZE * 64];
	USHORT result;
	int num,i,inResult;
	unsigned char tucPrint [24*4+1];	
	BYTE   EMVtagVal[64+1];
	USHORT EMVtagLen; 
	short spacestring;
	BYTE   key;

	char szAmountBuff[20+1], szTemp6[47+1];

    vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel); 
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	
    printDateTime();
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "MERCHANT ID  : %s", srTransRec.szMID);
	inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "TERMINAL ID  : %s", srTransRec.szTID);
	inPrint(szStr);
	
	memset(szTemp,0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "BATCH NUM    : %s", szTemp);
	inPrint(szStr);
		
	memset(szTemp,0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szInvoiceNo,szTemp,3);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "INVOICE NUM  : %s", szTemp);
	inPrint(szStr);
	
	szGetTransTitle(srTransRec.byTransType, szStr);  
	//gcitra-012315
	if(srTransRec.byTransType == SALE){
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"Sale");
		
	}else if(srTransRec.byTransType == VOID){
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"Void Sale");
	}
    vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
	if(srTransRec.byTransType != VOID)
        vdPrintTitleCenter(szStr);
	else
	    vdPrintCenterReverseMode(szStr);
	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
	EMVtagLen = 0;
	memset(szStr, ' ', d_LINE_SIZE);
	vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
	
    memset(szTemp, 0x00, sizeof(szTemp)); 
    if(page == BANK_COPY_RECEIPT) /*bank copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskBankCopy, srTransRec.szPAN, szTemp);
    else if(page == CUSTOMER_COPY_RECEIPT) /*customer copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp);		
    else if(page == MERCHANT_COPY_RECEIPT) /*merchant copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp);

	memset(szTemp2, 0x00, sizeof(szTemp2)); 
	sprintf(szTemp2, "CARD NUM     : %s", szTemp);
	
	memset(szTemp3, 0x00, sizeof(szTemp3));
	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		strcpy(szTemp3,"C");
	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
		strcpy(szTemp3,"M");
	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
		strcpy(szTemp3,"S");
	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK){
		if((srTransRec.szServiceCode[0] == '2') || (srTransRec.szServiceCode[0] == '6'))
			strcat(szTemp3, "F");  
		else			
			strcat(szTemp3, "S");
	}else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
	{
		if(srTransRec.szPAN[0] == '4')
			strcpy(szTemp3,"Paywave");
		else if(srTransRec.szPAN[0] == '5')
			strcpy(szTemp3,"Paypass");
	}
	inPrintLeftRight(szTemp2, szTemp3, 42);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	memset(szTemp, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);

	vdDebug_LogPrintf("srTransRec.szCardLable [%s]",srTransRec.szCardLable);
	if( srTransRec.byEntryMode == CARD_ENTRY_ICC || strcmp("CASHCARD",srTransRec.szCardLable) == 0 )
		strcpy(szTemp,srTransRec.szCardLable);
	else if(srTransRec.inCardType == DEBIT_CARD)
		strcpy(szTemp,"DEBIT");
	else
		strcpy(szTemp,srTransRec.szCardLable);
	
	sprintf(szStr, "CARD TYPE    : %s", szTemp);
	//inPrint(szStr);
	
	if(srTransRec.inAccountType == SAVINGS_ACCOUNT)
		sprintf(szTemp2, "*SAVINGS*");
	else if(srTransRec.inAccountType == CURRENT_ACCOUNT)
		sprintf(szTemp2, "*CURRENT*");
	
	inPrintLeftRight(szStr, szTemp2, 42);
	
	
	memset(szTemp, 0x00, d_LINE_SIZE);
	memset(szTemp1, 0x00, d_LINE_SIZE);
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
	
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	memset(szAmountBuff,0x00,sizeof(szAmountBuff));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	
	memset(szStr,0x00, sizeof(szStr));
	strcpy(szStr,"AMOUNT");
	
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp2);		
	if (srTransRec.byTransType == VOID)
		sprintf(szAmountBuff,"-%s %s", strCST.szCurSymbol, szTemp2);
	else
		sprintf(szAmountBuff,"%s %s", strCST.szCurSymbol, szTemp2);
	
	inPrintLeftRight(szStr,szAmountBuff,24);

	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	vdPrintCenter("TEMPORARILY UNABLE");
	vdPrintCenter("TO PROCESS. PLEASE");
	vdPrintCenter("DO BALANCE INQUIRY");
	vdLineFeed(srTransRec.fFooterLogo);
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	vdCTOSS_PrinterEnd();
}

void vdCTOS_PrinterFline(USHORT usLines)
{
	while(usLines > 0)
	{
		//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,fERMTransaction);
		usLines--;
	}
}

USHORT ushCTOS_PrintBodyError(int page)
{	
    char szStr[d_LINE_SIZE + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szAmountBuff[20], szTemp6[47];
    int inPADSize,x;
    char szSPACE[40];
    char szPOSEntry[21] = {0}; /* BDOCLG-00134: Reformat cardtype line -- jzg */

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    /* tid mid */
    printTIDMID(); 
    
    /* batch invoice no. */
    printBatchInvoiceNO();
    
    /* card no. */
    memset(szTemp5, 0x00, sizeof(szTemp5));
    //BDO: Enable PAN Masking for each receipt - start -- jzg
    //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable start 4 of 21

    if(page == BANK_COPY_RECEIPT) /*bank copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskBankCopy, srTransRec.szPAN, szTemp5);
    else if(page == CUSTOMER_COPY_RECEIPT) /*customer copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp5);	
    else if(page == MERCHANT_COPY_RECEIPT) /*merchant copy*/
        vdCTOS_FormatPAN2(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp5);	
    //aaronnino for BDOCLG ver 9.0 fix on issue #00078 Printing of copies should be configurable end 4 of 21
    //BDO: Enable PAN Masking for each receipt - end -- jzg
    
    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 1, 0);
    memset(szTemp6, 0x00, sizeof(szTemp6));
    memset(szStr, 0x00, sizeof(szStr));
    
    if (strlen(srTransRec.szPAN) > 16)
    vdRemoveSpace(szStr, szTemp5);		
    else
    strcpy(szStr, szTemp5);
    
	inPrint(szStr);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

	if(srTransRec.fBINVer)
	{
		memset(szStr, 0x00, d_LINE_SIZE);
		sprintf(szStr, "BIN CHECK    : %06ld", atol(srTransRec.szBINVerSTAN));
		inPrint(szStr);
	}
    
    /* card holder name */
    printCardHolderName();
    
    /*Trans type*/
    memset(szStr, 0x00, sizeof(szStr));
    if (srTransRec.byTransType == PRE_AUTH)
        strcpy(szStr,"CARD VERIFY");
    else if (srTransRec.byTransType == CASH_ADVANCE)
        strcpy(szStr,"CASH ADVANCE");
    else if (srTransRec.byTransType == SALE_OFFLINE)
        strcpy(szStr,"COMPLETION");
    else if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType == SALE_OFFLINE)
        strcpy(szStr,"VOID COMPLETION");
    else if (srTransRec.byTransType == SALE)
        strcpy(szStr,"DEBIT");
    else
        szGetTransTitle(srTransRec.byTransType, szStr); 
    
    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    if ((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_WAVE))
        vdPrintCenter("CONTACTLESS SALE");
    else if ((srTransRec.byTransType == VOID) && (srTransRec.byEntryMode == CARD_ENTRY_WAVE))
        vdPrintCenter("CONTACTLESS VOID");
    else
    {
       /* memset (baTemp, 0x00, sizeof(baTemp));
        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);*/
     vdPrintCenter(szStr);   
    }
    
    /* date time entry mode */
    switch(srTransRec.byEntryMode)
    {
        case CARD_ENTRY_MSR:
            strcat(szPOSEntry, "S");
        break;
		
        case CARD_ENTRY_MANUAL:
            strcat(szPOSEntry, "M");
        break;
		
        case CARD_ENTRY_ICC:
            strcat(szPOSEntry, "C");   
        break;
		
        case CARD_ENTRY_FALLBACK:			
			if((srTransRec.szServiceCode[0] == '2') || (srTransRec.szServiceCode[0] == '6'))
            	strcat(szPOSEntry, "F");  
			else			
            	strcat(szPOSEntry, "S");
        break;
    }
    printDateTimeAndOther(szPOSEntry, 23);
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    
    /* card type approval code */
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp3, 0x00, d_LINE_SIZE);
/*
	if( srTransRec.byEntryMode == CARD_ENTRY_ICC || strcmp("SMGUARNTR",srTransRec.szCardLable) == 0 || 
		strcmp("SMADVNTGE",srTransRec.szCardLable) == 0 || strcmp("SHOP CARD",srTransRec.szCardLable) == 0|| 
		strcmp("FLEET",srTransRec.szCardLable) == 0 || strcmp("CASHCARD",srTransRec.szCardLable) == 0 || 
		strcmp("BDOREWARDS",srTransRec.szCardLable) == 0 )
		strcpy(szTemp3,srTransRec.szCardLable);
	else if(srTransRec.inCardType == DEBIT_CARD)
		strcpy(szTemp3,"DEBIT");
	else if (srTransRec.inCardType == CREDIT_CARD)	
		strcpy(szTemp3,"CREDIT");
	else
		strcpy(szTemp3,srTransRec.szCardLable);
*/	

    /*Reference num*/
	memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szTemp1, 0x00, d_LINE_SIZE);
	
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
   
    /* AID TC */
    memset(szStr, ' ', sizeof(szStr));
    memset(szTemp, ' ', sizeof(szTemp));
    memset(szTemp1, ' ', sizeof(szTemp1));
    memset(szTemp3, ' ', sizeof(szTemp3));
    memset(szTemp4, ' ', sizeof(szTemp4));
    if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
    /* EMV: Revised EMV details printing - start -- jzg */
    (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
    (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
    (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
    (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_2) ||
    (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC))
    /* EMV: Revised EMV details printing - end -- jzg */
    {
        //AID
        EMVtagLen = srTransRec.stEMVinfo.T84_len;
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
        wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
        sprintf(szTemp1, "AID: %s", szTemp);
        
        //TC
        if (srTransRec.byTransType != SALE_OFFLINE && srTransRec.byOrgTransType!= SALE_OFFLINE)
            wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp3, 8);
		
		sprintf(szStr, "AID: %-16.16sTC: %-16.16s", szTemp, szTemp3);
				
		memset (baTemp, 0x00, sizeof(baTemp));		
        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inPrint(szStr);
    }		  
    
    /* Amount */
    memset(szTemp1, 0x00, d_LINE_SIZE);
    wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    
    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    
    if(srTransRec.byTransType == SALE)
    {
        //Base amount
        memset(szStr, ' ', d_LINE_SIZE);
        memset(szAmountBuff,0x00,20);
        memset(szTemp4, 0x00, sizeof(szTemp4));
        
        vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1,szTemp4);// patrick add code 20141216		
        sprintf(szAmountBuff,"%s%s",strCST.szCurSymbol,szTemp4);
        inPrintLeftRight("TOTAL:",szAmountBuff,23);		
    }

	
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	vdPrintCenter("TEMPORARILY UNABLE");
	vdPrintCenter("TO PROCESS. PLEASE");
	vdPrintCenter("DO BALANCE INQUIRY");
	vdLineFeed(srTransRec.fFooterLogo);
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
	vdCTOSS_PrinterEnd();
    return d_OK;	
}

void vdGetSignatureFilename(char *ptrNewFilename)
{
	char szNewFileName[24+1];
    char szInvoiceNo[6+1];
    
    memset(szInvoiceNo, 0x00, sizeof(szInvoiceNo));
    wub_hex_2_str(srTransRec.szInvoiceNo, szInvoiceNo, INVOICE_BCD_SIZE);
    
    memset(szNewFileName, 0x00, sizeof(szNewFileName));
    sprintf(szNewFileName, "signERM%06ld.bmp", atol(szInvoiceNo));
	memcpy(ptrNewFilename, szNewFileName, strlen(szNewFileName));
}

int ushCTOS_ePadSignature(void)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    ULONG ulFileSize;
	char szNewFileName[24+1];
	int fNSRflag = 0;
	
	if (FALSE ==strTCT.fSignatureFlag)
		return d_OK;

    //if(strIIT.fNSR == 1)
    //{
	//	if(inNSRFlag() == 1) /*VEPS below floorlimit*/
          //  return d_OK;			
    //}
	
    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    EMVtagLen = 0;
    
    if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
    {
        EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
        if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
        (EMVtagVal[0] != 0x1E) &&
        (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
        {
            return d_OK;	   	
        }
    }
	else if( srTransRec.byEntryMode == CARD_ENTRY_WAVE)
	{
		if(srTransRec.bWaveSCVMAnalysis != d_CVM_REQUIRED_SIGNATURE)
		{
            return d_OK;
		}
	}
	
	ePad_SignatureCaptureLibEx();
	return d_OK;
}

int ushCTOS_ePadPrintSignature(void)
{
    char szNewFileName[24+1];
    ULONG ulFileSize;
	
	if (FALSE ==strTCT.fSignatureFlag)
		return d_NO;

	if(srTransRec.byTransType == VOID 
		|| srTransRec.byTransType == SALE_TIP)
		return d_NO;

    vdDebug_LogPrintf("ushCTOS_ePadPrintSignature");

    if(fRePrintFlag == TRUE /*&& fRePrintPwrFailFlag == FALSE*/)
    {
        memset(szNewFileName, 0x00, sizeof(szNewFileName));	
        vdGetSignatureFilename(&szNewFileName);
        ulFileSize=lnGetFileSize(szNewFileName);
        vdDebug_LogPrintf("szNewFileName:[%s], ulFileSize:[%ld]",szNewFileName, ulFileSize);
        if(ulFileSize > 0)
        {
            vdCTOS_PrinterFline(1);
            vdCTOSS_PrinterBMPPicEx(0, 0, szNewFileName);
            inCTOSS_ReadERMSignatureFile(VFBMPONAME);
            return d_OK;
        }
    }
	else
	{
		ulFileSize=lnGetFileSize(DB_SIGN_BMP);
		vdDebug_LogPrintf("DB_SIGN_BMP:[%s], ulFileSize:[%ld]",DB_SIGN_BMP, ulFileSize);
		if(ulFileSize > 0)
		{
			vdCTOS_PrinterFline(1);
			vdCTOSS_PrinterBMPPicEx(0, 0, DB_SIGN_BMP);
			inCTOSS_ReadERMSignatureFile(VFBMPONAME);
			return d_OK;
		}		
	}
	
	return d_NO;
}

int ushCTOS_ClearePadSignature(void)
{
	if (FALSE ==strTCT.fSignatureFlag)
		return d_OK;
	
	ePad_ClearSignatureCaptureLib();
	return d_OK;
}

void vdPrintISOOption(unsigned char *pucTitle,unsigned char *uszData, int inLen)
{
	//inTCTRead(1);
	if(strTCT.inPrintISOOption == 1)
	{
		if(memcmp(pucTitle,"RX",2) == 0 && strCPT.inCommunicationMode == DIAL_UP_MODE)
			inPrintISOPacket(pucTitle , uszData, inLen-2);
		else
			inPrintISOPacket(pucTitle , uszData, inLen);
	}
	else if(strTCT.inPrintISOOption == 2)
	{
		vdPrintParseISO(srTransRec.byTransType,pucTitle ,uszData+5);
	}
	else if(strTCT.inPrintISOOption == 3)
	{
		if(memcmp(pucTitle,"RX",2) == 0 && strCPT.inCommunicationMode == DIAL_UP_MODE)
			inPrintISOPacket(pucTitle , uszData, inLen-2);
		else
			inPrintISOPacket(pucTitle , uszData, inLen);
		
		vdPrintParseISO(srTransRec.byTransType,pucTitle ,uszData+5);
	}

}

USHORT ushCTOS_ReprintBatchRecordHeader(void)
{
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];
	ACCUM_REC srAccumRec;
	int inResult;

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);

    ushCTOS_PrintHeader(0);	

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	inResult = inCTOS_ReadBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);
	if(inResult == ST_ERROR)
	{
	    //vdSetErrorMessage("NO RECORD FOUND");
	    vdDisplayErrorMsgResp2("","TRANSACTION","NOT FOUND");
		vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		return ST_ERROR;	
	}		 
	else if(inResult == RC_FILE_READ_OUT_NO_DATA)
	{
	    //vdSetErrorMessage("NO RECORD FOUND");
	    vdDisplayErrorMsgResp2("","TRANSACTION","NOT FOUND");
		return;
	}
	
	memcpy(srTransRec.szYear, srAccumRec.szYear, DATE_BCD_SIZE);
    memcpy(srTransRec.szDate, srAccumRec.szDate, DATE_BCD_SIZE);
    memcpy(srTransRec.szTime, srAccumRec.szTime, TIME_BCD_SIZE);
	memcpy(srTransRec.szBatchNo, srAccumRec.szBatchNo, BATCH_NO_BCD_SIZE);	
	
	printTIDMID();   

	//printDateTime();
    printLastSettleDateTime();
    
	memcpy(srTransRec.szBatchNo, srAccumRec.szBatchNo, BATCH_NO_BCD_SIZE);	
	printBatchNO();

	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    vdPrintCenter("DETAIL REPORT");
    //printHostLabel();
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0);
    
    //CTOS_PrinterFline(d_LINE_DOT * 1);
	vdCTOS_PrinterFline(1);
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "CARD NAME           CARD NUMBER", &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//inCCTOS_PrinterBufferOutput("CARD NAME           CARD NUMBER",&stgFONT_ATTRIB,1);
	if(strTCT.fSMMode == TRUE)
	{
        inCCTOS_PrinterBufferOutput("CARD NAME              CARD NUMBER",&stgFONT_ATTRIB,fERMTransaction);
        inCCTOS_PrinterBufferOutput("EXP DATE               TRACE NUMBER",&stgFONT_ATTRIB,fERMTransaction);
		inCCTOS_PrinterBufferOutput("TRANSACTION            AMOUNT",&stgFONT_ATTRIB,fERMTransaction);
	}
	else
	{
		inCCTOS_PrinterBufferOutput("CARD NAME              TRACE NUMBER",&stgFONT_ATTRIB,fERMTransaction);
		inCCTOS_PrinterBufferOutput("TRANSACTION            AMOUNT",&stgFONT_ATTRIB,fERMTransaction);
	}

	//inCCTOS_PrinterBufferOutput("APPROVAL CODE",&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput("APPROVAL CODE",&stgFONT_ATTRIB,fERMTransaction);
	printDividingLine(DIVIDING_LINE);
	return d_OK;
}



USHORT printLastSettleDateTime(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
  	CTOS_RTC SetRTC;
	char szYear[3];
//1102
	char sMonth[4];
	char szTempMonth[3];
	char szMonthNames[40];

	memset(sMonth,0x00,4);
	memset(szMonthNames,0x00,40);
	strcpy(szMonthNames,"JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC");
	memset(szTempMonth, 0x00, 3);

//1102
	
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);

	vdDebug_LogPrintf("year[%s],date[%02x][%02x]",srTransRec.szYear,srTransRec.szDate[0],srTransRec.szDate[1]);
    wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
    wub_hex_2_str(srTransRec.szTime, szTemp1,TIME_BCD_SIZE);
	vdDebug_LogPrintf("date[%s],time[%s]atol(szTemp)=[%d](atol(szTemp1)=[%d]",szTemp,szTemp1,atol(szTemp),atol(szTemp1) );
	vdDebug_LogPrintf("date[%s],time[%s]",szTemp,szTemp1);
	
	memcpy(szTempMonth,&szTemp[0],2);
	memcpy(sMonth, &szMonthNames[(atoi(szTempMonth) - 1)* 3], 3);
// Removed strTCT.fSMReceipt implementation
#if 0
	if (strTCT.fSMReceipt== 1)
		sprintf(szStr,"Date/Time    : %02lu/%02lu/%04lu         %02lu:%02lu:%02lu",atol(szTemp)/100,atol(szTemp)%100,atol(srTransRec.szYear)+2000,atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	else
#endif
		sprintf(szStr,"DATE/TIME    : %s %02lu, %02lu    %02lu:%02lu:%02lu",sMonth,atol(szTemp)%100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);

    //inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,fERMTransaction);
	inPrint(szStr);
	
    return(result);
     
}

USHORT printBatchInvoiceNOEx(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
        
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
    memset(szTemp2, ' ', d_LINE_SIZE);
	
	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
    wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
    vdMyEZLib_LogPrintf("invoice no: %s",szTemp2);

    //sprintf(szStr, "BATCH: %-14.14sINVOICE: %s", szTemp1, szTemp2);	// v10.0 change
    sprintf(szStr, "BATCH: %s", szTemp1);	// v10.0 change
	inPrint(szStr);
	
	memset(szStr, ' ', d_LINE_SIZE);
	sprintf(szStr, "INVOICE: %s", szTemp2);	// v10.0 change
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	inPrint(szStr);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    return(d_OK);
}

