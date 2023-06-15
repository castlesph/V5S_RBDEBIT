#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <ctype.h>
#include <EMVAPLib.h>
#include <EMVLib.h>

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
#include "..\Includes\epad.h"
#include "..\Includes\PosLoyalty.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Includes\epad.h"
#include "..\Erm\PosErm.h"




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

BYTE szgTempDate[DATE_BCD_SIZE+1];
BYTE szgTempTime[TIME_BCD_SIZE+1];

BYTE szgERMDate[DATE_BCD_SIZE+1];
BYTE szgERMTime[TIME_BCD_SIZE+1];

int printcopies_cntr;

BOOL fPrinterStart;

BOOL fReprintLastSettleFlag = 0;

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
	int inLine = 3;

	BYTE szTitle[20+1];
	BYTE szDisMsg[256];
	int inKey=0;

	if( (strTCT.byTerminalType%2) == 0 )
		inLine=2;

	//CTOS_LCDTClearDisplay();

	while(1)
	{
		inRet = CTOS_PrinterStatus();
		vdDebug_LogPrintf("saturn  - printCheckPaper inRet[%02x]",inRet );
		if (inRet==d_OK)
			return d_OK;
		else if(inRet==d_PRINTER_PAPER_OUT)
		{
		          #if 0
			vduiClearBelow(inLine);
			vduiWarningSound();
			vduiDisplayStringCenter(3,"PRINTER OUT OF");
			vduiDisplayStringCenter(4,"PAPER, INSERT");
			vduiDisplayStringCenter(5,"PAPER AND PRESS");
			vduiDisplayStringCenter(6,"ANY TO PRINT.");
			//vduiDisplayStringCenter(8,"[X] CANCEL PRINT");

			CTOS_KBDGet(&key);
			/*
			if(key==d_KBD_CANCEL)
			{
				CTOS_LCDTClearDisplay();
				return -1;
			}
			*/
			#else
			//CTOS_LCDTClearDisplay();
			memset(szTitle, 0x00, sizeof(szTitle));
			szGetTransTitle(srTransRec.byTransType, szTitle);
			
			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "PRINTER OUT OF PAPER");	
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "INSERT PAPER AND PRESS OK");	
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "TO PRINT");	
              			
			CTOS_Beep();
			//usCTOSS_ConfirmOK(szDisMsg);
			inDisplayMessageBoxWithButton(1,8,"PRINTER OUT OF PAPER","INSERT PAPER AND PRESS OK","TO PRINT", MSG_TYPE_WARNING); // sidumili: added

			#endif
		}
		if(isCheckTerminalMP200() == d_OK)
			return d_OK;
	}
}




void cardMasking(char *szPan, int style)
{
    int num;
    int i;
	char szTemp[30+1];
	
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
        strcpy(szTemp,"****************");
        num = strlen(szPan) - 4;
        memcpy(&szTemp[num],&szPan[num],4);
		szTemp[strlen(szPan)]=0x00;
        strcpy(szPan,szTemp);
    }
}

void vdCTOS_FormatDate(char *pchDate)
 {
	char szLocal[6 + 1] = {0};
	char szMonthNames[36 + 1] = {0};
	short Month = 0;

	strcpy(szLocal, pchDate);
	Month = (szLocal[2] - '0') * 10 + (szLocal[3] - '0') - 1;
	strcpy(szMonthNames, "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC");
	sprintf(pchDate, "%.3s.%.2s,20%.2s", &(szMonthNames[Month * 3]), &(szLocal[4]), szLocal);

	pchDate[13] = ((char) 0);
}

USHORT printDateTime(char *ptrDate, char *ptrTime, BOOL fTrans)
{
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];
	CTOS_RTC SetRTC;
	char szYear[3];
	char szTempDate[d_LINE_SIZE + 1];

	vdDebug_LogPrintf("--printDateTime--");
	vdDebug_LogPrintf("szTranYear[%s]", srTransRec.szTranYear);
	vdDebug_LogPrintf("fTrans[%d]", fTrans);
	
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
		sprintf(szTemp ,"%02d%02d",SetRTC.bMonth,SetRTC.bDay); //aaronnino BANCNET bancnet fix on issue #0002 Incomplete Date in Settlement  and Last settlement	
	}
	if(atol(szTemp1) == 0)
	{
		sprintf(szTemp1 ,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);	

	}
	vdDebug_LogPrintf("date[%s],time[%s]",szTemp,szTemp1);
	
	memset(szTempDate, 0x00, sizeof(szTempDate));

	if (fTrans)
	{
		if (strlen(srTransRec.szTranYear) > 0)
			sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szTranYear), atol(szTemp)/100, atol(szTemp)%100);
		else
			sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
	}		
	else
	{
		sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
	}
	
	vdCTOS_FormatDate(szTempDate);
	//if(strCDT.inType == DEBIT_CARD)
		//sprintf(szStr,"DATE %s	  TIME %02lu:%02lu:%02lu",szTempDate,atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	//else	
		//sprintf(szStr,"DATE/TIME: %s %02lu:%02lu:%02lu",szTempDate,atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);

	//inPrint(szStr);
	memcpy(ptrDate, szTempDate, strlen(szTempDate));
	memcpy(ptrTime, szTemp1, strlen(szTemp1));

	vdDebug_LogPrintf("ptrDate[%s].ptrTime[%s]", ptrDate, ptrTime);
	
	return(result);
	 
}



USHORT printTIDMID(void)
{
	char szStr[d_LINE_SIZE + 1];
	USHORT result;
	
	memset(szStr, ' ', d_LINE_SIZE);
	sprintf(szStr, "TID: %s", srTransRec.szTID);
	inPrint(szStr);
	
	memset(szStr, ' ', d_LINE_SIZE);
	sprintf(szStr, "MID: %s", srTransRec.szMID);
	inPrint(szStr);
	
	return (result);

}



USHORT printBatchInvoiceNO(void)
{
	char szStr[d_LINE_SIZE + 1];
	char szStr1[d_LINE_SIZE + 1];
	
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
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "BATCH NUM: %s", szTemp1);
	//result=inPrint(szStr);

	memset(szStr1, 0x00, d_LINE_SIZE);
	sprintf(szStr1, "TRACE NO.: %s", szTemp2);
	//result=inPrint(szStr);

	inPrintLeftRight(szStr, szStr1, 46);
	
	return d_OK;
}

USHORT printBatchNO(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
	
	wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
    sprintf(szStr, "BATCH NO: %s", szTemp);

    result = inPrint(szStr);
    return(result);
}

USHORT printHostLabel(void)
{
    char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;

    memset(szStr, 0x00, sizeof(szStr));
    sprintf(szStr, "HOST: %s", srTransRec.szHostLabel);

    result = inPrint(szStr);
    return(result);
}


void vdPrintCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 32;
    BYTE baTemp[PAPER_X_SIZE * 64];

	//vdDebug_LogPrintf("vdPrintCenter | fERMTransaction[%d]", fERMTransaction);
	
    if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 32;
    else if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 16;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 48;
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
      
	inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB, fERMTransaction);
}

void vdPrintTitleCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 24;
    //BYTE baTemp[PAPER_X_SIZE * 64];

	//vdDebug_LogPrintf("vdPrintTitleCenter | fERMTransaction[%d]", fERMTransaction);
	
    if ((isCheckTerminalMP200() == d_OK) && (strTCT.byERMMode == 2))
	{
		usCharPerLine = 46;
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
    	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    
    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;
				
	memset(tucPrint,0x20,30);
	
	memcpy(tucPrint+spacestring,strIn,usCharPerLine);	
	
	tucPrint[i+spacestring]=0;
        
	inPrint(tucPrint);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
}

USHORT printDividingLine(int style)
{
    if (style == DIVIDING_LINE_DOUBLE)
        return(inPrint("============================================="));
    else
        return(inPrint("---------------------------------------------"));
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
    int shHostIndex, inRet=0;
	
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

	CTOS_LCDTClearDisplay();

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRINT_EMV_TAGS);
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
	
    if( printCheckPaper()==-1)
    	return ;

    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);  

	if (isCheckTerminalMP200() != d_OK)
   	 	inCTOS_DisplayPrintBMP();
	
    CTOS_LanguagePrinterFontSize(d_FONT_16x16, 0, TRUE);	
	
	vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");
	
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
        
    vdCTOS_PrinterFline(1);
    vdPrintTitleCenter("*** END OF REPORT ***");
	vdCTOS_PrinterFline(2);
	
    vdLineFeed(FALSE);
		
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

	vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);  

	ushCTOS_PrintHeader(0);

    vdCTOS_PrinterFline(1);
            
    memset(szStr, 0x00, sizeof(szStr));
    memset(szBuf, 0x00, sizeof(szBuf));
    wub_hex_2_str(strTCT.szInvoiceNo, szBuf, 3);
    sprintf(szStr, "INVOICE NO: %s", szBuf);                        
    inPrint(szStr);
	
    memset(szStr, 0x00, sizeof(szStr));
    sprintf(szStr, "PABX: %s", strTCT.szPabx);                        
    
    inPrint(szStr);
	
    //check host num
    inNumOfHost = inHDTNumRecord();
    
    vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {                
            inCPTRead(inNum);

            vdCTOS_PrinterFline(1);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "HOST: %s", strHDT.szHostLabel);                        
            inPrint(szStr);
			
            memset(szStr, 0x00, sizeof(szStr));
            memset(szBuf, 0x00, sizeof(szBuf));
            wub_hex_2_str(strHDT.szTPDU, szBuf, 5);
            sprintf(szStr, "TPDU: %s", szBuf);                        
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
            inPrint(szStr);
			
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI NUM : %s", strCPT.szPriTxnPhoneNumber);                        
            inPrint(szStr);
			
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC NUM : %s", strCPT.szSecTxnPhoneNumber);                        
            inPrint(szStr);
			
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI IP  : %s", strCPT.szPriTxnHostIP);                        
            inPrint(szStr);
			
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum);                        
            inPrint(szStr);
			
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC IP  : %s", strCPT.szSecTxnHostIP);                        
            inPrint(szStr);
			
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum);                        
            inPrint(szStr);
			
            inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
        
            vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
            for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
            {
                if((inResult = inMMTReadRecord(strHDT.inHostIndex,strMMT[inLoop-1].MITid)) !=d_OK)
                {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex,inResult);
                    continue;
                    //break;
                }
                else 
                {
                    if(strMMT[0].fMMTEnable)
                    {                        
                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "MERCHANT:%s", strMMT[0].szMerchantName);                        
                        inPrint(szStr);
						
                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "TID : %s", strMMT[0].szTID);                        
                        inPrint(szStr);
						
                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "MID : %s", strMMT[0].szMID);                        
                        inPrint(szStr);
						
                        memset(szStr, 0x00, sizeof(szStr));
                        memset(szBuf, 0x00, sizeof(szBuf));
                        wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
                        sprintf(szStr, "BATCH NO: %s", szBuf);                        
                        inPrint(szStr);
                    }                    

                }
            }
        }
        else
            continue;

    }

    vdLineFeed(FALSE);
	
    vdCTOSS_PrinterEnd();
    return ;
}

void printCardHolderName(void)
{
    USHORT shLen;
    char szStr[d_LINE_SIZE + 1];
    
    shLen = strlen(srTransRec.szCardholderName);

    if(shLen > 0)
    {
        memset(szStr, 0x00, d_LINE_SIZE);
        strcpy(szStr, srTransRec.szCardholderName);
        szStr[31]=0x00;
        //while(0x20 == szStr[--shLen] && shLen >= 0)
            //szStr[shLen] = 0x00;

    	//vdPrintCenter(szStr);	
    	inPrint(szStr);
    }
}

void vdPrintFormattedAmount(unsigned char *prtDisplay, unsigned char *prtAmount, int inWidth)
{
	char szAmtBuf[24+1] = { 0 };
	char szStr[24+1] = { 0 };
	char szStr2[24+1] = { 0 };

	memset(szStr, 0, sizeof(szStr));
	memset(szStr2, 0, sizeof(szStr2));
	memset(szAmtBuf, 0, sizeof(szAmtBuf));
	
	if(srTransRec.byVoided == TRUE)
	{
		szAmtBuf[0]='-';
	    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", prtAmount, &szAmtBuf[1]);
	}
	else
	    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", prtAmount, szAmtBuf);
	
	//vdCTOS_Pad_String(szAmtBuf, 12, 0x20, POSITION_LEFT);
	//strcpy(szStr, prtDisplay);
	//sprintf(szStr2, "%s %s", strCST.szCurSymbol, szAmtBuf);

	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%s%s", prtDisplay, strCST.szCurSymbol);
	inPrintLeftRight(szStr, szAmtBuf, inWidth);
}

USHORT ushCTOS_PrintDemo(void)
{
    BYTE strIn[40];
    unsigned char tucPrint [24*4+1];
    short i,spacestring;
    USHORT usCharPerLine = 24;
    BYTE baTemp[PAPER_X_SIZE * 64];

	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);

    memset(strIn, 0x00, sizeof(strIn));
    strcpy(strIn, "DEMO");
    i = strlen(strIn);
    spacestring=(usCharPerLine-i)/2;
                
    memset(tucPrint,0x20,30);
    
    memcpy(tucPrint+spacestring,strIn,usCharPerLine);   
    
    tucPrint[i+spacestring]=0;

    vdCTOS_PrinterFline(1);
        
    inPrint(tucPrint);
    
    vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    return d_OK;
    
}



USHORT ushCTOS_PrintHeader(int page)
{	
    vdDebug_LogPrintf("saturn ushCTOS_PrintHeader-1");
	vdDebug_LogPrintf("strHDT.szHeaderLogoName[%s]", strHDT.szHeaderLogoName);
	vdDebug_LogPrintf("fRePrintFlag[%d] fReprintLastSettleFlag[%d]", fRePrintFlag, fReprintLastSettleFlag);
		//print Logo	
	if(strlen(strHDT.szHeaderLogoName) > 0)
		vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
	else
		vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");

	vdDebug_LogPrintf("ushCTOS_PrintHeader-2");
        
	//print Logo	
	//if(strlen(strHDT.szHeaderLogoName) > 0)
	//	vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
	//else
	//	vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");

	//if(page == 0)
	//    vdCTOSS_PrinterBMPPicEx(0, 0, "/data/data/com.Source.S1_BANCNET.BANCNET/bancnet.bmp");	//TINE:  for faster printing, logo header to be handled on ushCTOS_printAll

	if (fReprintLastSettleFlag==1)
		vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);

	vdDebug_LogPrintf("saturn ushCTOS_PrintHeader-2");
	
	if(fRePrintFlag == TRUE)
    {   
        vdDebug_LogPrintf("saturn srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
		if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
        {
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctHdr1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctHdr1);
    }

	vdDebug_LogPrintf("saturn ushCTOS_PrintHeader-3");
	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
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

    //vdCTOS_PrinterFline(1);

	vdDebug_LogPrintf("strMMT[0].szRctHdr1[%s]", strMMT[0].szRctHdr1);
	vdDebug_LogPrintf("strMMT[0].szRctHdr2[%s]", strMMT[0].szRctHdr2);
	vdDebug_LogPrintf("strMMT[0].szRctHdr3[%s]", strMMT[0].szRctHdr3);
	vdDebug_LogPrintf("strMMT[0].szRctHdr4[%s]", strMMT[0].szRctHdr4);
	vdDebug_LogPrintf("strMMT[0].szRctHdr5[%s]", strMMT[0].szRctHdr5);
	
    if(VS_TRUE == strTCT.fDemo)
    {
        ushCTOS_PrintDemo();
    }

    if(fRePrintFlag == TRUE)
    {
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        vdPrintCenter("DUPLICATE");
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
    
	vdCTOS_PrinterFline(1);

	vdDebug_LogPrintf("ushCTOS_PrintHeader-end"); 
	
    return d_OK;
}

USHORT ushCTOS_PrintInstallmentDetails(void)
{	
	char szTemp[d_LINE_SIZE+1];
	char szStr[d_LINE_SIZE+1];
    char szTempAmount[12+1];
	char szTempAmountHex[6+1];
	
	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	memset(szTemp, 0x00, d_LINE_SIZE);
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	vdPrintFormattedAmount("TOTAL :", szTemp, 24);
		
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

    //inPrintLeftRight("PROMO               :", srTransRec.szHostLabel, 46);
	
    inPrintLeftRight("TERMS/MOS           :", srTransRec.szTerms, 46);
	
	memset(szTemp, 0x00, d_LINE_SIZE);
	sprintf(szTemp, "%012ld", atol(srTransRec.szTotalInstAmnt));
	vdPrintFormattedAmount("GROSS AMT           :", szTemp, 46);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "%.1s.%s", srTransRec.szFactorRate, &srTransRec.szFactorRate[1]);
    inPrintLeftRight("RATE                :", szStr, 46);

	memset(szTemp, 0x00, d_LINE_SIZE);
	sprintf(szTemp, "%012ld", atol(srTransRec.szMoAmort));
	vdPrintFormattedAmount("MONTHLY AMORTIZATION:", szTemp, 46);	
}

USHORT ushCTOS_PrintBody(int page)
{	
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    char szRootFS[d_LINE_SIZE + 1];
    int inFmtPANSize;
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    char szTotalAmount[d_LINE_SIZE + 1];
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    ULONG ulHH=0L;
    char szAMPM[2+1];
    int inPrintAcquirerFee=0, inPrintServiceFee=0;
    long lnTotalAmount=0L;
	
	inPrintAcquirerFee=get_env_int("PRINT_ACQ_FEE");
	inPrintServiceFee=get_env_int("PRINT_SVC_FEE");	
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	//if(CUSTOMER_COPY_RECEIPT == page)
	if(page > 0)
	{
		vdPrintTIDMIDVersion();
		
        vdCTOS_PrinterFline(1);

		vdPrintTrxnType();

		vdPrintCardTypeBankName();
		
		vdPrintAcntType();

		vdPrintEntryMode();
			
        vdPrintBillsPaymentDetails(); /*Print payment details*/
		
        vdCTOS_PrinterFline(1);
		
		vdPrintBatchNoInvoiceNo();
		
		vdPrintDateTime();
		
		vdPrintRRefNoTraceNo();
		
        vdCTOS_PrinterFline(1);
		
		vdPrintECRMessage(); // Mercury Requirements -- sidumili

		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
        if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
        {
            if(srTransRec.byTransType != REFUND && srTransRec.byOrgTransType != REFUND)
            {
                //AC
                wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
                //sprintf(szStr, "APP CRYPT : %s", szTemp);
                inPrintLeftRight("APP CRYPT :", szTemp, 46);
            }
			
			//AID
			memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
			memset(szTemp, ' ', d_LINE_SIZE);
			wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
			//sprintf(szStr, "AID       : %s",szTemp);
			inPrintLeftRight("AID       :", szTemp, 46);
			
			memset(szStr, ' ', d_LINE_SIZE);
			//sprintf(szStr, "APP LABEL : %s", srTransRec.stEMVinfo.szChipLabel);
			inPrintLeftRight("APP LABEL :", srTransRec.stEMVinfo.szChipLabel, 46);

			vdCTOS_PrinterFline(1); // New Line
		}
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, 0x00, d_LINE_SIZE); //acquirer fee
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
//		wub_hex_2_str(srTransRec.szServiceFee, szTemp2, AMT_BCD_SIZE);

        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);

		#if 1
		vdPrintFormattedAmount("AMT:", szTemp, 24);
                
                lnTotalAmount=atol(szTemp);

		if(inPrintServiceFee == 1)
		{
		    if(atol(srTransRec.szServiceFee) > 0)
		    {
				vdPrintFormattedAmount("SERVICE FEE:", srTransRec.szServiceFee, 24);
				lnTotalAmount=lnTotalAmount+atol(srTransRec.szServiceFee);
		    }
			else
				inPrintServiceFee=0;
		}
		
		if(inPrintAcquirerFee == 1)
		{
		    if(atol(srTransRec.szAcquirerFee) > 0)
		    {
				vdPrintFormattedAmount("ACQUIRER FEE:", srTransRec.szAcquirerFee, 24);
				lnTotalAmount=lnTotalAmount+atol(srTransRec.szAcquirerFee);
		    }
			else
				inPrintAcquirerFee=0;
		}

		if(inPrintAcquirerFee == 1 || inPrintServiceFee == 1)
		{
		    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
		    sprintf(szTotalAmount, "%012ld", lnTotalAmount);
			vdPrintFormattedAmount("TOTAL:", szTotalAmount, 24);
		}
		#else
		if (srTransRec.byTransType == SALE || srTransRec.byTransType == SALE_OFFLINE || srTransRec.byTransType == VOID || srTransRec.byTransType == CASH_OUT
			|| srTransRec.byTransType == CASH_IN || srTransRec.byTransType == BILLS_PAYMENT || srTransRec.byTransType == FUND_TRANSFER)
		{
            if(srTransRec.fInstallment)
            {
                ushCTOS_PrintInstallmentDetails();				
            }
			else
			{
                //Tip amount
                memset(szStr, ' ', d_LINE_SIZE);
                sprintf(szStr,"%s    %s","TIP",strCST.szCurSymbol);
                memset (baTemp, 0x00, sizeof(baTemp));
                memset (szTemp4, 0x00, sizeof(szTemp4));
                wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);
                
                DebugAddSTR("TIP:",szTemp4,12);
                if (TRUE ==strHDT.fTipAllowFlag)
                {
                    if(atol(szTemp4) > 0)
                    {
						//Base amount
						vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);

                        vdPrintFormattedAmount("TIP   :", szTemp4, 24);
                        
                        //Total amount
                        vdPrintFormattedAmount("TOTAL :", szTemp, 24);
                    }
                    else
                    {
						if(srTransRec.byTransType == VOID)
						{
                            vdPrintFormattedAmount("TOTAL :", szTemp1, 24);							
						}
						else
						{
							//Base amount
							vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);

                            // Tip
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp1, ' ', d_LINE_SIZE);
							strcpy(szTemp1, "TIP:");
							sprintf(szStr,"%s ____________", strCST.szCurSymbol);
							inPrintLeftRight(szTemp1, szStr, 24);
							vdCTOS_PrinterFline(1);
							
							// Total
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp1, ' ', d_LINE_SIZE);
							strcpy(szTemp1, "TOTAL:");
							sprintf(szStr,"%s ____________", strCST.szCurSymbol);
							inPrintLeftRight(szTemp1, szStr, 24);
						}
                    }
                }
				else
				{
					if(srTransRec.byTransType == CASH_OUT || srTransRec.byTransType == CASH_IN 
					|| srTransRec.byTransType == BILLS_PAYMENT || srTransRec.byTransType == FUND_TRANSFER)
					{
						#if 0
						vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
						vdPrintFormattedAmount("WITHDRAWAL AMOUNT :", szTemp1, 46);
						vdPrintFormattedAmount("SERVICE FEE :", szTemp2, 46);
                        #endif
						vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
						vdPrintFormattedAmount("TOTAL :", szTemp, 24);
					}
					else
					{
						vdPrintFormattedAmount("TOTAL :", szTemp1, 24);
					}
				}
                    			
			}
			
		}
		else if (srTransRec.byTransType == SALE_TIP)
		{
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
			vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);
			vdPrintFormattedAmount("TIPS  :", szTemp3, 24);
			vdPrintFormattedAmount("TOTAL :", szTemp, 24);
			
		}
		else
		{			
            vdPrintFormattedAmount("AMOUNT:", szTemp, 24);
		}
        #endif
		
        //vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	#if 0
	else if(MERCHANT_COPY_RECEIPT == page || BANK_COPY_RECEIPT == page)
	{
		vdPrintTIDMIDVersion();
		
        vdCTOS_PrinterFline(1);

		vdPrintTrxnType();

		vdPrintCardTypeBankName();

		vdPrintAcntType();

		vdPrintEntryMode();
		
		vdPrintBillsPaymentDetails(); /*Print payment details*/
		
        vdCTOS_PrinterFline(1);

		vdPrintBatchNoInvoiceNo();

		vdPrintDateTime();

		vdPrintRRefNoTraceNo();

        vdCTOS_PrinterFline(1);

		vdPrintECRMessage(); // Mercury Requirements -- sidumili

		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
        if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
        {
            if(srTransRec.byTransType != REFUND && srTransRec.byOrgTransType != REFUND)
            {
                //AC
                wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
                sprintf(szStr, "APP CRYPT : %s", szTemp);
                inPrint(szStr);

				/*
                //TVR
                EMVtagLen = 5;
                memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
                memset(szStr, ' ', d_LINE_SIZE);
                sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
                inPrint(szStr);
                */
            }
			
            //AID
            memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
            memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
            memset(szTemp, ' ', d_LINE_SIZE);
            wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
            sprintf(szStr, "AID       : %s",szTemp);
            inPrint(szStr);
			
            /* EMV: Get Application Label - start -- jzg */
            memset(szStr, ' ', d_LINE_SIZE);
            sprintf(szStr, "APP LABEL : %s", srTransRec.stEMVinfo.szChipLabel);
            inPrint(szStr);
            /* EMV: Get Application Label - end -- jzg */

			vdCTOS_PrinterFline(1); // New Line
		}
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp2, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szServiceFee, szTemp2, AMT_BCD_SIZE);

        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
        
		if (srTransRec.byTransType == SALE || srTransRec.byTransType == SALE_OFFLINE || srTransRec.byTransType == VOID || srTransRec.byTransType == CASH_OUT
		|| srTransRec.byTransType == CASH_IN || srTransRec.byTransType == BILLS_PAYMENT || srTransRec.byTransType == FUND_TRANSFER)
		{
			//Base amount
            if(srTransRec.fInstallment)
            {
                ushCTOS_PrintInstallmentDetails();				
            }
			else
			{
    			//Tip amount
    			memset(szStr, ' ', d_LINE_SIZE);
    			sprintf(szStr,"%s    %s","TIP",strCST.szCurSymbol);
    			memset (baTemp, 0x00, sizeof(baTemp));
    			memset (szTemp4, 0x00, sizeof(szTemp4));
    			wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);
    
    			DebugAddSTR("TIP:",szTemp4,12);
                if (TRUE ==strHDT.fTipAllowFlag)
                {
                    if(atol(szTemp4) > 0)
                    {
						//Base amount
						vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);

                        vdPrintFormattedAmount("TIP   :", szTemp4, 24);
                        
                        //Total amount
                        vdPrintFormattedAmount("TOTAL :", szTemp, 24);
                    }
                    else
                    {
						if(srTransRec.byTransType == VOID)
						{
                            vdPrintFormattedAmount("TOTAL :", szTemp1, 24);							
						}
						else
						{
							//Base amount
							vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);

                            // Tip
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp1, ' ', d_LINE_SIZE);
							strcpy(szTemp1, "TIP:");
							sprintf(szStr,"%s ____________", strCST.szCurSymbol);
							inPrintLeftRight(szTemp1, szStr, 24);
							vdCTOS_PrinterFline(1);
							
							// Total
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp1, ' ', d_LINE_SIZE);
							strcpy(szTemp1, "TOTAL:");
							sprintf(szStr,"%s ____________", strCST.szCurSymbol);
							inPrintLeftRight(szTemp1, szStr, 24);
						}
                    }
                }
				else
				{
							
					if(srTransRec.byTransType == CASH_OUT || srTransRec.byTransType == CASH_IN 
					|| srTransRec.byTransType == BILLS_PAYMENT || srTransRec.byTransType == FUND_TRANSFER)
					{
						#if 0
						vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
						vdPrintFormattedAmount("WITHDRAWAL AMOUNT :", szTemp1, 46);
 						vdPrintFormattedAmount("SERVICE FEE :", szTemp2, 46);
 		                #endif
						vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
						vdPrintFormattedAmount("TOTAL :", szTemp, 24);
					}
					else
					{
						vdPrintFormattedAmount("TOTAL :", szTemp1, 24);
					}
				}
				
			}
		}
		else if (srTransRec.byTransType == SALE_TIP)
		{
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
			vdPrintFormattedAmount("AMOUNT:", szTemp1, 24);
			vdPrintFormattedAmount("TIPS  :", szTemp3, 24);
			vdPrintFormattedAmount("TOTAL :", szTemp, 24);
			
		}
		else
		{			
            vdPrintFormattedAmount("AMOUNT:", szTemp, 24);
		}

        //vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	#endif
	return d_OK;	
	
}
USHORT ushCTOS_PrintAgreement()
{
	if (srTransRec.byTransType == BAL_INQ)
		return d_OK;
		
    if(strCDT.inType == DEBIT_CARD)
    {
		/*Coreware*/
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		
        if (srTransRec.byTransType == CASH_OUT)
		{
			vdPrintCenter("I ACKNOWLEDGE RECEIPT OF THE WITHDRAWAL AMOUNT");
			vdPrintCenter("STATED ABOVE");
		}        	
		else
		{
			vdPrintCenter("**THANK YOU FOR USING YOUR ATM CARD TO PAY!**");
		}
		
		vdCTOS_PrinterFline(1);
		
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
    else
    {
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		
        vdPrintCenter("I AGREE TO PAY THE ABOVE TOTAL AMOUNT");
        vdPrintCenter("ACCORDING TO THE CARD ISSUER AGREEMENT");
        
		vdCTOS_PrinterFline(1);
		
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
}

USHORT ushCTOS_PrintFooter(int page)
{		
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	if(page == CUSTOMER_COPY_RECEIPT)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		{
		    #if 0
		    EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			if((EMVtagVal[2]& 0x0F) == 2)
            {   
                vdCTOS_PrinterFline(2);
                vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
                    vdPrintCenter("(PIN VERIFY SUCCESS)");
            }
            else
            {
                //CTOS_PrinterFline(d_LINE_DOT * 3);
            }
			#endif

			if (srTransRec.byTransType == CASH_OUT)
			{
				//vdCTOS_PrinterFline(1);
				//vdPrintCenter("USUAL WITHDRAWAL FEE APPLY");
			}
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
        }

		//ushCTOS_ePadPrintSignature();
		
		//CTOS_PrinterPutString("SIGN:_______________________________________");
		//printCardHolderName();
		vdCTOS_PrinterFline(1);
		
		ushCTOS_PrintAgreement();

		vdPrintFooterAndLogo();
		
		vdCTOS_PrinterFline(1); 
		
		vdPrintCenter("***** CUSTOMER COPY *****");
		vdCTOS_PrinterFline(2);
	}
	else if(page == MERCHANT_COPY_RECEIPT)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		vdDebug_LogPrintf("MERCH:CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
		vdDebug_LogPrintf("MERCH:srTransRec.byEntryMode = [%d] | bWaveSCVMAnalysis = [%d] = bWaveSID = [%d]", srTransRec.byEntryMode, srTransRec.bWaveSCVMAnalysis, srTransRec.bWaveSID);

		EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

		vdDebug_LogPrintf("MERCH:EMVtagVal [%02X %02X %02X]",EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
		
		if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		{
			 if (srTransRec.byTransType == CASH_OUT)
			 {
				//vdCTOS_PrinterFline(1);
				//vdPrintCenter("USUAL WITHDRAWAL FEE APPLY");

				vdCTOS_PrinterFline(2);
				if(ushCTOS_ePadPrintSignature() != d_OK)
					vdCTOS_PrinterFline(1);

				inPrint("SIGN:_______________________________________");
			 }
			 else
			 {
			 	 if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
	             (EMVtagVal[0] != 0x1E) &&
	             (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
				 {
		            vdCTOS_PrinterFline(2);
		            vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
		            if (EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42 || srTransRec.bWaveSCVMAnalysis == 2)
		            {
		            	vdCTOS_PrinterFline(1);
		                vdPrintCenter("(PIN VERIFY SUCCESS)");
		            }
				 }
			 }			
		}
        else
        {
        	if (srTransRec.byTransType == CASH_OUT)
			{
				//vdCTOS_PrinterFline(1);
				//vdPrintCenter("USUAL WITHDRAWAL FEE APPLY");

				vdCTOS_PrinterFline(2);
				if(ushCTOS_ePadPrintSignature() != d_OK)
					vdCTOS_PrinterFline(1);

				inPrint("SIGN:_______________________________________");
			}
			else
			{
				if(strCDT.inType != DEBIT_CARD)
				{
					vdCTOS_PrinterFline(2);
					inPrint("SIGN:_______________________________________");
				}
			}			
        }

        //ushCTOS_ePadPrintSignature();
        
        //if(strCDT.inType != DEBIT_CARD)
            //CTOS_PrinterPutString("SIGN:_______________________________________");
		//printCardHolderName();
		vdCTOS_PrinterFline(1);
		
		ushCTOS_PrintAgreement();
        
		vdPrintFooterAndLogo();
		
		vdCTOS_PrinterFline(1);
		
		vdPrintCenter("***** MERCHANT COPY *****");
		vdCTOS_PrinterFline(2);
	}
	else if(page == BANK_COPY_RECEIPT)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		vdDebug_LogPrintf("BANK:CVM [%02X %02X %02X]",srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
		vdDebug_LogPrintf("BANK:srTransRec.byEntryMode = [%d] | bWaveSCVMAnalysis = [%d] = bWaveSID = [%d]", srTransRec.byEntryMode, srTransRec.bWaveSCVMAnalysis, srTransRec.bWaveSID);

		EMVtagLen = 3;
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

		vdDebug_LogPrintf("BANK:EMVtagVal [%02X %02X %02X]",EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
		
		if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		{
			 if (srTransRec.byTransType == CASH_OUT)
			 {
				//vdCTOS_PrinterFline(1);
				//vdPrintCenter("USUAL WITHDRAWAL FEE APPLY");

				vdCTOS_PrinterFline(2);
				if(ushCTOS_ePadPrintSignature() != d_OK)
					vdCTOS_PrinterFline(1);

				inPrint("SIGN:_______________________________________");
			 }
			 else
			 {
			 	 if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
	             (EMVtagVal[0] != 0x1E) &&
	             (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
				 {
		            vdCTOS_PrinterFline(2);
		            vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
		            if (EMVtagVal[0] == 0x01 || EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42 || srTransRec.bWaveSCVMAnalysis == 2)
		            {
		            	vdCTOS_PrinterFline(1);
		                vdPrintCenter("(PIN VERIFY SUCCESS)");
		            }
				 }
			 }            
		}
        else
        {
        	if (srTransRec.byTransType == CASH_OUT)
			{
				//vdCTOS_PrinterFline(1);
				//vdPrintCenter("USUAL WITHDRAWAL FEE APPLY");

				vdCTOS_PrinterFline(2);
				if(ushCTOS_ePadPrintSignature() != d_OK)
					vdCTOS_PrinterFline(1);

				inPrint("SIGN:_______________________________________");
			}
			else
			{
				if(strCDT.inType != DEBIT_CARD)
				{
	               vdCTOS_PrinterFline(2);
	               inPrint("SIGN:_______________________________________");
				}
			}			
        }

        //ushCTOS_ePadPrintSignature();
        
        //if(strCDT.inType != DEBIT_CARD)
            //CTOS_PrinterPutString("SIGN:_______________________________________");
		//printCardHolderName();
		vdCTOS_PrinterFline(1);
		
		ushCTOS_PrintAgreement();
		
		vdPrintFooterAndLogo();
		
		vdCTOS_PrinterFline(1); 

		vdPrintCenter("***** BANK COPY *****");
		vdCTOS_PrinterFline(2);
	}

	vdLineFeed((strHDT.fFooterLogo == TRUE)?TRUE:FALSE); // added -- sidumili

return d_OK;	

}


#if 1

USHORT ushCTOS_printAll(int page) {	
	int inResult;

	vdDebug_LogPrintf("--ushCTOS_printAll--");
	//vdDebug_LogPrintf("fERMTransaction[%d]", fERMTransaction);
	
    if( printCheckPaper()==-1)
        return -1;

	//if (isCheckTerminalMP200() != d_OK)
   	//	 vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);

    #if 0 /*albert (ecr tct update fail) - save last invoice number, moved to vdSaveLastInvoiceNo*/
    if(VS_FALSE == fRePrintFlag)
	    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
		vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }
	#endif

    if (isCheckTerminalMP200() != d_OK)
		vdCTOSS_PrinterStart(100);
	
    //CTOS_PrinterSetHeatLevel(4);  

	vdDebug_LogPrintf("ushCTOS_printAll-2");
	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
	ushCTOS_PrintHeader(page);

	vdDebug_LogPrintf("ushCTOS_printAll-3");

	if(srTransRec.fLoyalty==TRUE && (srTransRec.byTransType == LOY_BAL_INQ || srTransRec.byTransType == LOY_REDEEM_5050
	|| srTransRec.byTransType == LOY_REDEEM_VARIABLE || srTransRec.byTransType == VOID))
	{
 	    ushCTOS_PrintLoyaltyBody(page);	
	    ushCTOS_PrintLoyaltyFooter(page);
	}
	else
	{	
  	    ushCTOS_PrintBody(page);	
	    ushCTOS_PrintFooter(page);
	}
	
	//vdCTOSS_PrinterEnd();
    //vdSetPrintThreadStatus(0);

	if (isCheckTerminalMP200() != d_OK)
	    CTOS_LCDTClearDisplay();  
    
    return d_OK;
 
}
#else
USHORT ushCTOS_printAll(int page)
{	
	int inResult, print_counter = 0;
    int inKey = d_NO;
    BYTE szLogoPath[256 + 1];
    BYTE szMsg[256];
    BYTE szPrintMsg[512];
	BYTE szTitle[20+1];
	BYTE szDisMsg[100];
	BYTE szCopy[256];

	vdDebug_LogPrintf("--ushCTOS_printAll,page[%d]--", page);
	//vdDebug_LogPrintf("fERMTransaction[%d]", fERMTransaction);
	
    if( printCheckPaper()==-1)
        return -1;

	//tine/sidumili: android - set ui thread to display nothing
	DisplayStatusLine(" ");
	
	//if (isCheckTerminalMP200() != d_OK)
   	//	 vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);

    #if 0 /*albert (ecr tct update fail) - save last invoice number, moved to vdSaveLastInvoiceNo*/
    if(VS_FALSE == fRePrintFlag)
	    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
		vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }
	#endif

    if (isCheckTerminalMP200() != d_OK)
		vdCTOSS_PrinterStart(100);
	
    CTOS_PrinterSetHeatLevel(4);  

	vdDebug_LogPrintf("ushCTOS_printAll-2");
	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
	ushCTOS_PrintHeader(page);

	vdDebug_LogPrintf("ushCTOS_printAll-3");

	if(srTransRec.fLoyalty==TRUE && (srTransRec.byTransType == LOY_BAL_INQ || srTransRec.byTransType == LOY_REDEEM_5050
	|| srTransRec.byTransType == LOY_REDEEM_VARIABLE || srTransRec.byTransType == VOID))
	{
 	    ushCTOS_PrintLoyaltyBody(page);	
	    ushCTOS_PrintLoyaltyFooter(page);
	}
	else
	{	
  	    ushCTOS_PrintBody(page);	
	    //ushCTOS_PrintFooter(page);
	    ushCTOS_PrintFooterCashMo(page);
	}
	
	vdCTOSS_PrinterEnd();
    //vdSetPrintThreadStatus(0);

	//if (isCheckTerminalMP200() != d_OK)
	//    CTOS_LCDTClearDisplay(); 
	
	//TINE:  19AUG2019 modified for android terminal to display receipt on screen
    memset(szLogoPath, 0x00, sizeof(szLogoPath));
    memset(szMsg, 0x00, sizeof(szMsg));
	memset(szCopy, 0x00, sizeof(szCopy));
    memset(szPrintMsg, 0x00, sizeof(szPrintMsg));

    sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);

    if (page == BANK_COPY_RECEIPT)
    	{
    		strcpy(szMsg, "Print Bank Copy?");
			strcpy(szCopy, "Bank Copy");
    	}        
    else if (page == CUSTOMER_COPY_RECEIPT)
    	{
    		strcpy(szMsg, "Print Customer Copy?");
			strcpy(szCopy, "Customer Copy");
    	}        
    else if (page == MERCHANT_COPY_RECEIPT)
    	{	
    		strcpy(szMsg, "Print Merchant Copy?");
			strcpy(szCopy, "Merchant Copy");
    	}
    
    vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
        CTOS_KBDBufFlush();
        inCTOSS_ERM_Form_Receipt(0);

vdDebug_LogPrintf("strTCT.feReceiptEnable=%d, byTerminalModel[%d],printcopies_cntr=%d", strTCT.feReceiptEnable, strTCT.byTerminalModel, printcopies_cntr);
	if (strTCT.feReceiptEnable == 1) {

        strcpy(szPrintMsg, szLogoPath);
        strcat(szPrintMsg, "|");
        strcat(szPrintMsg, szMsg);

        if (strTCT.byTerminalModel == 1) {
            //vdCTOSS_PrinterStart(200);
            //ushCTOS_printAll(BANK_COPY_RECEIPT);
            
            if (printcopies_cntr == 0) {
                //inKey = PrintFirstReceiptUI(szLogoPath);
                memset(szTitle, 0x00, sizeof(szTitle));
				szGetTransTitle(srTransRec.byTransType, szTitle);
				strcpy(szDisMsg, szTitle);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "PRINTING...");
				usCTOSS_LCDDisplay(szDisMsg);
				//vdDisplayMessageBox(1, 8, "PRINTING", szCopy, MSG_PLS_WAIT, MSG_TYPE_PRINT);
	
                vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
                vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
                vdCTOSS_PrinterEnd();
                return d_OK;
            } else {
            	inKey = PrintReceiptUI(szPrintMsg);
			vdDebug_LogPrintf("PrintReceiptUI() - inKey=%d", inKey);	
				if (inKey == d_OK || inKey == 0xFF) 
                                {
	                vdDebug_LogPrintf("vdCTOSS_PrinterEnd() - START");
					memset(szTitle, 0x00, sizeof(szTitle));
					szGetTransTitle(srTransRec.byTransType, szTitle);
					strcpy(szDisMsg, szTitle);
					strcat(szDisMsg, "|");
					strcat(szDisMsg, "PRINTING...");
					usCTOSS_LCDDisplay(szDisMsg);
					//vdDisplayMessageBox(1, 8, "PRINTING", szCopy, MSG_PLS_WAIT, MSG_TYPE_PRINT);
				
	                vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
	                vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
	                vdCTOSS_PrinterEnd();
	                return d_OK;
	                //print_counter = 1;
	                //goto INPRINTRECEIPT;
	            }
	            if (inKey == d_USER_CANCEL)
	            {
					return d_OK;		
	            }
	                		
            }            
                          
        } else {
            vdDebug_LogPrintf("strTCT.byTerminalModel = [%d]", strTCT.byTerminalModel);
            inKey = EliteReceiptUI(szLogoPath);
        }
    } else {
        //sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);  
        memset(szTitle, 0x00, sizeof(szTitle));
		szGetTransTitle(srTransRec.byTransType, szTitle);
		strcpy(szDisMsg, szTitle);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "PRINTING...");
		usCTOSS_LCDDisplay(szDisMsg);
		//vdDisplayMessageBox(1, 8, "PRINTING", szCopy, MSG_PLS_WAIT, MSG_TYPE_PRINT);
					
        vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
		vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
        vdCTOSS_PrinterEnd();
        //vdSetPrintThreadStatus(0);

        //if (isCheckTerminalMP200() != d_OK)
        //    CTOS_LCDTClearDisplay();

        return d_OK;

    }

    return inKey;
 
}

#endif

int inCTOS_rePrintReceipt(void)
{
	int   inResult;
	BYTE  szInvNoAsc[6+1];
	BYTE  szInvNoBcd[3];	
	BOOL   needSecond = TRUE;	
	BYTE   key;

	vdDebug_LogPrintf("--inCTOS_rePrintReceipt--");
	
	CTOS_PrinterSetHeatLevel(4);
	if( printCheckPaper()==-1)
		return d_NO;

	#if 0
	inResult = inCTOS_GeneralGetInvoice();
	if(d_OK != inResult)
	{
		return inResult;
	}
    #endif
	
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

	vdDebug_LogPrintf("srTransRec.HDTid[%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("srTransRec.IITid[%d]", srTransRec.IITid);
	vdDebug_LogPrintf("srTransRec.CDTid[%d]", srTransRec.CDTid);
	
	inHDTRead(srTransRec.HDTid); // Fix for print the correct "szHeaderLogoName" define -- sidumili
    inIITRead(srTransRec.IITid);
    inCDTRead(srTransRec.CDTid);
	
	// Revise for printer start mp200 -- sidumili
	if (!fPrinterStart)
		vdCTOSS_PrinterStart(100);

	vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, TRUE, strTCT.fPrintMerchCopy);
	
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
	char szTemp1[d_LINE_SIZE + 1], szTemp2[d_LINE_SIZE + 1];
    int result;
	char szRootFS[d_LINE_SIZE + 1];
	int inIITCount=0;

	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

	BYTE szLogoPath[256 + 1];
	BYTE szDisMsg[100];

	vdSetGlobalVariable();
	
	if( printCheckPaper()==-1)
		return;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	// Commented to support print confirmation -- sidumili
	#if 0
	strcpy(szDisMsg, "SUMMARY  REPORT");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "PRINTING...");
	usCTOSS_LCDDisplay(szDisMsg);
	#endif
		  
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

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("summary MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return;

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return;
	}

    inResult = inChkAccumEmpty();
    if(d_OK != inResult)
        return;

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return;    
    }

    vdCTOSS_PrinterStart(1000);
    CTOS_PrinterSetHeatLevel(4);  

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

	if (isCheckTerminalMP200() != d_OK)
	    inCTOS_DisplayPrintBMP();

	memset(szLogoPath, 0x00, sizeof(szLogoPath));	
	
    sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
//	vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
	
	ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("SUMMARY REPORT");
	
	vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	/*
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "MERCHANT              %s", srTransRec.szMID);
	result=inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "TERMINAL ID           %s", srTransRec.szTID);
	result=inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	vdCTOS_GetROOTFS(szRootFS);
	szRootFS[6]=0x00;
	sprintf(szStr, "OS VERSION            %s", szRootFS);
    result=inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "APPLICATION VERSION:  %s", APPL_VERION);
	result=inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "MODEL VERSION         %s", MODEL_VERSION);
	result=inPrint(szStr);
	
    vdCTOS_PrinterFline(1);
	
	vdCTOS_SetDateTime();

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "HOST:                 %s", srTransRec.szHostLabel);
	result=inPrint(szStr);
	
	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	printDateTime(szTemp1, szTemp2);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr,"DATE:                 %s",szTemp1);
	result=inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr,"TIME:                 %02lu:%02lu:%02lu",atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	result=inPrint(szStr);

	memset(szTemp1, 0x00, d_LINE_SIZE);
	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "BATCH:                %s", szTemp1);
	result=inPrint(szStr);
	*/

	vdPrintDateTimeAlignLeftRight();
	
	vdPrintTIDMIDVersion();
	
	vdPrintHostNameAndBatchNo();

	vdCTOS_PrinterFline(1);
	
	//for(inTranCardType = 1; inTranCardType < 2 ;inTranCardType ++)
	
	//inTranCardType = 1;
	inIITCount=inIITNumRecord()+1;
    for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
	{
		BOOL fPrintTotal=FALSE;
		inReportType = PRINT_CARD_TOTAL;
		inTranCardType = 1; // Debit
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <inIITCount; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRedeemCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashOutCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashInCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBillsPaymentCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBalanceInquiryCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usFundTransferCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBalanceInquiryCount== 0))
					continue;
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				strcpy(szStr,strIIT.szIssuerLabel);
				inPrint(szStr);
				//vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);
				vdCTOS_PrintAccumeByHostAndCardCashMo(inReportType, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);
				fPrintTotal=TRUE;
			}
			//after print issuer total, then print host toal
			if(fPrintTotal == TRUE)
			{
				memset(szStr, ' ', d_LINE_SIZE);
				strcpy(szStr,"TOTAL:");
				inPrint(szStr);
				//vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 
				vdCTOS_PrintAccumeByHostAndCardCashMo(inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 

				// For BancNet CashOut -- sidumili
				//vdCTOS_ServiceFeePrintAccumByHostAndCard(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
			}
		}
		else
		{
			memset(szStr, ' ', d_LINE_SIZE);
			strcpy(szStr,srTransRec.szHostLabel);
			inPrint(szStr);
            //vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 
			vdCTOS_PrintAccumeByHostAndCardCashMo(inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 
		}
	}
    
	//print space one line
	vdLineFeed(FALSE);
	
	vdCTOSS_PrinterEnd(); // Commented to support print confirmation -- sidumili

	// Capture report -- sidumili
	vdDebug_LogPrintf("Capturing erm report...");
	vdDeleteBMP();
	vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
	CTOS_KBDBufFlush();
	inCTOSS_ERM_Form_Receipt(0);

	/* tine:  no screen display for summary report due to indeterminate report length
	if (strGBLVar.fGBLvConfirmPrint)
	{
		// Print confirmation -- sidumili
		if (inPrintConfirmation("SUMMARY REPORT", FALSE) == d_OK)
		{
			vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
			vdCTOSS_PrinterEnd();
		}
		else
		{			
			vdCTOSS_PrinterEnd();
		}
	}
	else	*/
	{
		//usCTOSS_LCDDisplay("SUMMARY REPORT|PRINTING...");
//		vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);
//		vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
//		vdCTOSS_PrinterEnd();
	}
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
	ACCUM_REC srAccumRec;
	char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];

	//test
	BYTE szTitle[20+1];
	BYTE szDisMsg[100];
	//test

	vdDebug_LogPrintf("saturn print detail report");

	vdSetGlobalVariable();

	if( printCheckPaper()==-1)
		return;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	// Commented to support print confirmation -- sidumili
	#if 0
	strcpy(szDisMsg, "DETAIL  REPORT");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "PRINTING...");
	usCTOSS_LCDDisplay(szDisMsg);
	#endif

	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("detail host Index",shHostIndex);

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

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("detail MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return;

    inResult = inCTOS_ChkBatchEmpty();
    if(d_OK != inResult)
        return;

    inResult = inChkAccumEmpty();
    if(d_OK != inResult)
        return;

	inBatchNumOfRecord = inBatchNumRecord();
	
	DebugAddINT("batch record",inBatchNumOfRecord);
	if(inBatchNumOfRecord <= 0)
	{
		vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
		return;
	}


//test
/*
memset(szTitle, 0x00, sizeof(szTitle));

szGetTransTitle(srTransRec.byTransType, szTitle);
strcpy(szDisMsg, szTitle);
strcat(szDisMsg, "|");
strcat(szDisMsg, "PRINTING...");
usCTOSS_LCDDisplay(szDisMsg);
*/
//test

    vdCTOSS_PrinterStart(1000);
    CTOS_PrinterSetHeatLevel(4);  
	
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

	if (isCheckTerminalMP200() != d_OK)
	    inCTOS_DisplayPrintBMP();
	
	//ushCTOS_printBatchRecordHeader();
	ushCTOS_printBatchRecordHeaderCashMo();

	if (!strGBLVar.fGBLvConfirmPrint)
		vdCTOSS_PrinterEnd();
	
	pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
	inCount = 0;		
	inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
	for(i=0; i<inBatchNumOfRecord; i++)
	{
		inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        inCount ++;
		if(srTransRec.byTransType == PRE_AUTH)
            continue;

			if (!strGBLVar.fGBLvConfirmPrint)
			{
				vdCTOSS_PrinterStart(100);
				CTOS_PrinterSetHeatLevel(4);
			}
			
		    //ushCTOS_printBatchRecordFooter();
            ushCTOS_printBatchRecordFooterCashMo();
			if (!strGBLVar.fGBLvConfirmPrint)
				vdCTOSS_PrinterEnd();
	}


    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return;    
    }

	if (!strGBLVar.fGBLvConfirmPrint)
	{
		vdCTOSS_PrinterStart(100);
		CTOS_PrinterSetHeatLevel(4);
	}
		
    vdCTOS_PrinterFline(1);

    for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
    {
    	inTranCardType = 1; // Debit
		
        if((srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRedeemCount == 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashOutCount== 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashInCount== 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usBillsPaymentCount== 0)
        && (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usFundTransferCount== 0)
        &&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBalanceInquiryCount== 0))
            continue;	
		
        memset(szStr, ' ', d_LINE_SIZE);
        strcpy(szStr,"TOTAL:");
        inPrint(szStr);
        inReportType = PRINT_CARD_TOTAL;
        vdCTOS_PrintAccumeByHostAndCardCashMo(inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 

		// For BancNet CashOut -- sidumili
		//vdCTOS_ServiceFeePrintAccumByHostAndCard(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
    }

    vdLineFeed(FALSE);
	
    //vdCTOSS_PrinterEnd(); // Commented to support print confirmation -- sidumili
	
	// Capture report -- sidumili		
	vdDebug_LogPrintf("Capturing erm report...");
	vdDeleteBMP();
	vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
	CTOS_KBDBufFlush();
	inCTOSS_ERM_Form_Receipt(0);

	/*  tine:  no screen display for detail report due to indeterminate report length
	if (strGBLVar.fGBLvConfirmPrint)
	{
		// Print confirmation -- sidumili
		if (inPrintConfirmation("DETAIL REPORT", FALSE) == d_OK)
		{
			vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
			vdCTOSS_PrinterEnd();
		}
		else
		{			
			vdCTOSS_PrinterEnd();
		}
	}
	else	*/
	{
		//usCTOSS_LCDDisplay("DETAIL REPORT|PRINTING...");
		vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);
		vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
		vdCTOSS_PrinterEnd();
	}
	
	//return;	
}




int inCTOS_rePrintLastReceipt(void)
{
	int   inResult;
	BYTE  szInvNoAsc[6+1];
	BYTE  szInvNoBcd[3];	
    BOOL   needSecond = TRUE;	
    BYTE   key;

	vdDebug_LogPrintf("--inCTOS_rePrintLastReceipt--");
	
	CTOS_PrinterSetHeatLevel(4);
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

	vdDebug_LogPrintf("srTransRec.HDTid[%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("srTransRec.IITid[%d]", srTransRec.IITid);
	vdDebug_LogPrintf("srTransRec.CDTid[%d]", srTransRec.CDTid);
	
	inHDTRead(srTransRec.HDTid); // Fix for print the correct "szHeaderLogoName" define -- sidumili
    inIITRead(srTransRec.IITid);
    inCDTRead(srTransRec.CDTid);
	
	// Revise for printer start mp200 -- sidumili
	if (!fPrinterStart)
		vdCTOSS_PrinterStart(100);
	
	vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, TRUE, strTCT.fPrintMerchCopy);
	
	return d_OK;
}

USHORT ushCTOS_printReceipt(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
    vdDebug_LogPrintf("ushCTOS_printReceipt starttt...");
    if( printCheckPaper()==-1)
    	return -1;
	
	// Revise for printer start mp200 -- sidumili
	if (!fPrinterStart)
		vdCTOSS_PrinterStart(100);

	vdCTOS_PrintReceiptCopies(strTCT.fPrintBankCopy, TRUE, strTCT.fPrintMerchCopy);
	vdDebug_LogPrintf("ushCTOS_printReceipt enddd...");
    return (d_OK);
}



USHORT ushCTOS_printErrorReceipt(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
	
	char szStr[d_LINE_SIZE + 1];
   char szTemp[d_LINE_SIZE + 1];
   char szTemp1[d_LINE_SIZE + 1];
   char szTemp2[d_LINE_SIZE + 1];
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
    
    CTOS_PrinterSetHeatLevel(4);  
	DebugAddSTR("ushCTOS_printAll","print...",20);
		

	ushCTOS_PrintHeader(d_FIRST_PAGE);

	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
	vdPrintCenter(szTemp1);

	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	printDateTime(szTemp1, szTemp2, TRUE);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr,"DATE/TIME: %s %02lu:%02lu:%02lu",szTemp1,atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	result=inPrint(szStr);
		
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
    	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    	CTOS_PrinterBufferOutput((BYTE *)baTemp, 4); 

    	cardMasking(srTransRec.szPAN, PRINT_CARD_MASKING_1);
    	strcpy(szTemp4, srTransRec.szPAN);
    	memset (baTemp, 0x00, sizeof(baTemp));
    	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
    	CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);		

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
    		memcpy(szTemp1,"MSR",3);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
    		memcpy(szTemp1,"Fallback",8);
    	
    	memset (baTemp, 0x00, sizeof(baTemp));
    	sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
    	
    	DebugAddSTR("ENT:",baTemp,12);  
    	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
    	CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);

    		
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
	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
	CTOS_PrinterBufferOutput((BYTE *)baTemp, 4); 

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	
	sprintf(szStr, "%s", "PLS TRY AGAIN");
	
	memset (baTemp, 0x00, sizeof(baTemp));
	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
	CTOS_PrinterBufferOutput((BYTE *)baTemp, 4); 

	vdLineFeed(FALSE);
	
	vdCTOSS_PrinterEnd();
	//inSetTextMode();
    return (d_OK);
}


int inCTOS_REPRINT_ANY()
{
    char szErrMsg[30+1];
    int   inResult;
	
	fRePrintFlag = TRUE;
    vdCTOS_SetTransType(REPRINT_ANY);
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

	fReprintLastSettleFlag = 0;
    
    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
		CTOS_LCDTClearDisplay();
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();

    return d_OK;
}

void vdCTOS_PrintAccumeByHostAndCard (int inReportType, STRUCT_TOTAL Totals)
{
	USHORT usSaleCount = 0;
	ULONG ulSaleTotalAmount = 0;
	USHORT usRefundCount = 0;
	ULONG  ulRefundTotalAmount = 0;	 
	USHORT usVoidSaleCount = 0;
	ULONG  ulVoidSaleTotalAmount = 0;
	USHORT usOffSaleCount = 0;
	ULONG  ulOffSaleTotalAmount = 0;
	USHORT usTipCount = 0;
	ULONG  ulTipAmount = 0;
	USHORT usRedeemCount = 0;
	ULONG  ulRedeemTotalAmount = 0;

	// For BancNet CashOut -- sidumili
	USHORT usCashOutCount = 0;
	ULONG ulCashOutTotalAmount = 0;
	ULONG ulServiceFeeTotalAmount = 0;
		
		char szStr[d_LINE_SIZE + 1];
		char szTemp[d_LINE_SIZE + 1];
		char szFormattedAmount[d_LINE_SIZE + 1];
		USHORT usTotalCount;
		ULONG  ulTotalAmount;
		BYTE baTemp[PAPER_X_SIZE * 64];
		BOOL isCashOutFlag = (BOOL)get_env_int("CASHOUTFLAG"); // For BancNet CashOut -- sidumili

		// Set variable
		usSaleCount=Totals.usSaleCount-Totals.usOffSaleCount;
		ulSaleTotalAmount=Totals.ulSaleTotalAmount-Totals.ulOffSaleTotalAmount;
		//usRefundCount=Totals.usRefundCount;
		//ulRefundTotalAmount=Totals.ulRefundTotalAmount;	 
		usVoidSaleCount=Totals.usVoidSaleCount;
		ulVoidSaleTotalAmount=Totals.ulVoidSaleTotalAmount;
		//usOffSaleCount=Totals.usOffSaleCount;
		//ulOffSaleTotalAmount=Totals.ulOffSaleTotalAmount;
		//usTipCount=Totals.usTipCount;
		//ulTipAmount=Totals.ulTipTotalAmount;
		//usRedeemCount=Totals.usRedeemCount;
		//ulRedeemTotalAmount=Totals.ulRedeemTotalAmount;

		// For BancNet CashOut -- sidumili
		usCashOutCount=Totals.usCashOutCount;
		ulCashOutTotalAmount=Totals.ulCashOutTotalAmount;
		ulServiceFeeTotalAmount=Totals.ulServiceFeeTotalAmount;
		
		CTOS_PrinterSetHeatLevel(4);  
		memset (baTemp, 0x00, sizeof(baTemp));		
		
		//types
		memset(szStr, 0x00, d_LINE_SIZE);
		strcpy(szStr,"TYPES           COUNT     CUR             AMT");
		inPrint(szStr);
		
		//sale
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0x00, d_LINE_SIZE);
		memset(szFormattedAmount, 0x00, d_LINE_SIZE);
		sprintf(szTemp,"%ld", ulSaleTotalAmount);
		vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
		sprintf(szStr,"SALE            %3d       %s",usSaleCount, strCST.szCurSymbol);
		inPrintLeftRight(szStr, szFormattedAmount, 46);

		if (isCashOutFlag)
		{
			//CashOut
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp, 0x00, d_LINE_SIZE);
			memset(szFormattedAmount, 0x00, d_LINE_SIZE);
			sprintf(szTemp,"%ld", ulCashOutTotalAmount);
			vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
			sprintf(szStr,"CASHOUT         %3d       %s",usCashOutCount, strCST.szCurSymbol);
			inPrintLeftRight(szStr, szFormattedAmount, 46);
			
		}
		
		//void
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0x00, d_LINE_SIZE);
		memset(szFormattedAmount, 0x00, d_LINE_SIZE);
		sprintf(szTemp,"%ld", ulVoidSaleTotalAmount);
		vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
		sprintf(szStr,"VOID            %3d       %s", usVoidSaleCount,strCST.szCurSymbol);
		inPrintLeftRight(szStr, szFormattedAmount, 46);

		/*
		//refund
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0x00, d_LINE_SIZE);
		memset(szFormattedAmount, 0x00, d_LINE_SIZE);
		sprintf(szTemp,"%ld", ulRefundTotalAmount);
		vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
		sprintf(szStr,"REFUNDS		   %3d		 %s", usRefundCount,strCST.szCurSymbol);
		inPrintLeftRight(szStr, szFormattedAmount, 46);

		//tips sale
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0x00, d_LINE_SIZE);
		memset(szFormattedAmount, 0x00, d_LINE_SIZE);
		sprintf(szTemp,"%ld", ulTipAmount);
		vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
		sprintf(szStr,"TIPS 					 %s", strCST.szCurSymbol);
		inPrintLeftRight(szStr, szFormattedAmount, 46);
		
		//offline sale
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0x00, d_LINE_SIZE);
		memset(szFormattedAmount, 0x00, d_LINE_SIZE);
		sprintf(szTemp,"%ld", ulOffSaleTotalAmount);
		vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
		sprintf(szStr,"OFFLINE		   %3d		 %s", usOffSaleCount,strCST.szCurSymbol);
		inPrintLeftRight(szStr, szFormattedAmount, 46);
		*/
		
		inPrint("---------------------------------------------");
		
		usTotalCount = usSaleCount + usOffSaleCount + usRefundCount + usCashOutCount;
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0x00, d_LINE_SIZE);
		memset(szFormattedAmount, 0x00, d_LINE_SIZE);
		if (inReportType == PRINT_HOST_TOTAL)
		{
			if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount))
			{
				ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount);
				
				sprintf(szTemp,"%ld", ulTotalAmount);
				vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
				szFormattedAmount[0]='-';
			}
			else
			{
				ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount) - ulRefundTotalAmount;
				sprintf(szTemp,"%ld", ulTotalAmount);
				vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szFormattedAmount);
			}
		}
		else if(inReportType == PRINT_CARD_TOTAL)
		{
			if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount))
			{
				ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount);
				sprintf(szTemp,"%ld", ulTotalAmount);
				vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
				szFormattedAmount[0]='-';
			}
			else
			{
				ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount) - ulRefundTotalAmount;
				sprintf(szTemp,"%ld", ulTotalAmount);
				vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szFormattedAmount);
			}	 
		}
		sprintf(szStr,"TOTALS          %3d       %s", usTotalCount,strCST.szCurSymbol);
		inPrintLeftRight(szStr, szFormattedAmount, 46);

		vdCTOS_PrinterFline(1);
}



int inCTOS_SelectFont(int inFontMode,int inFontSize ,int inFontStyle,char * szFontName)
{
    vdDebug_LogPrintf("inFontMode=%d", inFontMode);
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

int inCTOS_PrintSettleReport(void)
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
    int *pinTransDataid;
	int inBatchNumOfRecord;
	int inCount;
    TRANS_DATA_TABLE srTransRecTemp;
	char szTemp1[d_LINE_SIZE + 1],szTemp2[d_LINE_SIZE + 1];
    int result;
    int inIITCount=0;
	BOOL fPrintSettleDetailReport = (VS_BOOL)strTCT.fPrintSettleDetailReport;
	int iOrientation = get_env_int("#ORIENTATION");
	
	BYTE szLogoPath[256 + 1];
	
	vdDebug_LogPrintf("--inCTOS_PrintSettleReport--");
	vdDebug_LogPrintf("fAutoSettleFlag=[%d]", srTransRec.fAutoSettleFlag);

	if (iOrientation > 0 || (isCheckTerminalMP200() == d_OK))
		fPrintSettleDetailReport = FALSE;
	
	vdDebug_LogPrintf("fPrintSettleDetailReport[%d]", fPrintSettleDetailReport);
	
	if( printCheckPaper()==-1)
		return d_NO;

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return d_NO;
	}

	inCTLOS_Updatepowrfail(PFR_BATCH_SETTLEMENT_PRINT);//1204
	
	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;    
    }
    
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	
    vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);

	if (isCheckTerminalMP200() != d_OK)
   		inCTOS_DisplayPrintBMP();

	memset(szLogoPath, 0x00, sizeof(szLogoPath));	
	sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
//	vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
		
	ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("SETTLEMENT REPORT");
	
	CTOS_PrinterFline(d_LINE_DOT * 1);
    
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	vdPrintDateTimeAlignLeftRight();
	
	vdPrintTIDMIDVersion();

	vdPrintHostNameAndBatchNo();
	
	//memset(szTemp1, 0x00, d_LINE_SIZE);
	//memset(szTemp2, 0x00, d_LINE_SIZE);
	//printDateTime(szTemp1, szTemp2);
	//memset(szStr, 0x00, d_LINE_SIZE);
	//sprintf(szStr,"DATE/TIME: %s %02lu:%02lu:%02lu",szTemp1,atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	//result=inPrint(szStr);
	//printBatchNO();    

	// Flag to print detail report during settlement
	if (fPrintSettleDetailReport)
	{
	    vdCTOS_PrinterFline(1);
	    vdPrintCenter("*** DETAIL REPORT ***");
	    vdCTOS_PrinterFline(1);

		// Print Detail Report Header Details
		vdPrintDetailReportHeader();
	
		printDividingLine(DIVIDING_LINE_DOUBLE);
		
		if (isCheckTerminalMP200() != d_OK)
			vdCTOSS_PrinterEnd();
		
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		inBatchNumOfRecord = inBatchNumRecord();

		pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
		inCount = 0;		

		//memset(&srTransRecTemp, 0x00, sizeof(srTransRecTemp));
		//memcpy(&srTransRecTemp, &srTransRec, sizeof(srTransRec));
		
		inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
		for(i=0; i<inBatchNumOfRecord; i++)
		{
			inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
	        inCount ++;

			if (isCheckTerminalMP200() != d_OK)
			{
				vdCTOSS_PrinterStart(100);
				CTOS_PrinterSetHeatLevel(4);
			}
			
			ushCTOS_printBatchRecordFooter();

			if (isCheckTerminalMP200() != d_OK)
				vdCTOSS_PrinterEnd();
		}
	}

	vdRestoreReportTime(); // Fix for time on last settle receipt -- sidumili
	
	//memcpy(&srTransRec, &srTransRecTemp, sizeof(srTransRecTemp));

    // for investigation.. need for mp200
	/*
	if (isCheckTerminalMP200() != d_OK)
	{
		vdCTOSS_PrinterStart(100);
		CTOS_PrinterSetHeatLevel(4);
	}
	*/
	
    vdCTOS_PrinterFline(1);

    inIITCount=inIITNumRecord()+1;
	for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
	{
		BOOL fPrintTotal=FALSE;
		inReportType = PRINT_CARD_TOTAL;
		inTranCardType = 1; // Debit
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <inIITCount; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashOutCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashInCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBillsPaymentCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usFundTransferCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBalanceInquiryCount== 0))
					continue;
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				strcpy(szStr,strIIT.szIssuerLabel);
				inPrint(szStr);
                vdCTOS_PrintAccumeByHostAndCardCashMo(inReportType, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);
				fPrintTotal=TRUE;
			}
			//after print issuer total, then print host toal
			if(fPrintTotal == TRUE)
			{
				memset(szStr, ' ', d_LINE_SIZE);
				strcpy(szStr,"TOTAL:");
				inPrint(szStr);
                vdCTOS_PrintAccumeByHostAndCardCashMo(inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 

				// For BancNet CashOut -- sidumili
				//vdCTOS_ServiceFeePrintAccumByHostAndCard(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
			}
		}
		else
		{
			memset(szStr, ' ', d_LINE_SIZE);
			strcpy(szStr,srTransRec.szHostLabel);
			inPrint(szStr);
            vdCTOS_PrintAccumeByHostAndCard (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 
		}
	}
	
    vdPrintTitleCenter("SETTLEMENT CONFIRMED");	
	vdCTOS_PrinterFline(2);
	//print space one line
	vdLineFeed(FALSE);

	vdDisplayMessageBox(1, 8, "", "PRINTING...", "", MSG_TYPE_PRINT);
	vdCTOSS_PrinterEnd();

	if(strTCT.byERMMode != 0);
	    inCTOSS_ERM_Form_Receipt(1);
		
	return d_OK;	
}

USHORT ushCTOS_printBatchRecordHeader(void)
{
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[DISPLAY_LINE_SIZE + 1], szTemp1[DISPLAY_LINE_SIZE + 1], szTemp2[DISPLAY_LINE_SIZE + 1];
	char szRootFS[d_LINE_SIZE + 1];
    BYTE szLogoPath[256 + 1];

	memset(szLogoPath, 0x00, sizeof(szLogoPath));	

    sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
//	vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
	
    ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("DETAIL REPORT");
	
	vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	/*
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "MERCHANT              %s", srTransRec.szMID);
	result=inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "TERMINAL ID           %s", srTransRec.szTID);
	result=inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	vdCTOS_GetROOTFS(szRootFS);
	szRootFS[6]=0x00;
	sprintf(szStr, "OS VERSION            %s", szRootFS);
	result=inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "APPLICATION VERSION:  %s", APPL_VERION);
	result=inPrint(szStr);
	
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "MODEL VERSION         %s", MODEL_VERSION);
	result=inPrint(szStr);
	
    vdCTOS_PrinterFline(1);
	
	vdCTOS_SetDateTime();

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "HOST:                 %s", srTransRec.szHostLabel);
	result=inPrint(szStr);
	
	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	printDateTime(szTemp1, szTemp2);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr,"DATE:                 %s",szTemp1);
	result=inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr,"TIME:                 %02lu:%02lu:%02lu",atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	result=inPrint(szStr);

	memset(szTemp1, 0x00, d_LINE_SIZE);
	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "BATCH:                %s", szTemp1);
	result=inPrint(szStr);

	*/
	
	vdPrintDateTimeAlignLeftRight();

	vdPrintTIDMIDVersion();

	vdPrintHostNameAndBatchNo();

	vdCTOS_PrinterFline(1);

	#if 0
    inPrint("TRANSACTION         INVOICE");
    inPrint("AMOUNT              CARD NUMBER");
	inPrint("APPR CODE           RREF NUM");
    inPrint("DATE                TIME");
	#endif
	
	inPrintLeftRight("TRANSACTION", "INVOICE", 46);
	inPrintLeftRight("AMOUNT", "CARD NUMBER", 46);
	inPrintLeftRight("APPR CODE", "RREF NUM", 46);
	inPrintLeftRight("DATE", "TIME", 46);
	
	printDividingLine(DIVIDING_LINE_DOUBLE);
	
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
	char szDate[d_LINE_SIZE + 1];
	char szTime[d_LINE_SIZE + 1];
	char szAcntType[d_LINE_SIZE + 1];
	char szTranName[d_LINE_SIZE + 1];
	int i;

	DebugAddSTR("ushCTOS_printBatchRecordFooter", srTransRec.szPAN, 10);

    /*trans type*/
    memset(szTempBuf1,0,sizeof(szTempBuf1));
    memset(szTempBuf2, 0, sizeof(szTempBuf2));
    if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
        strcpy(szTempBuf1, "REDEEM");
    else
        szGetTransTitle(srTransRec.byTransType, szTempBuf1);
    memset (szTempBuf2, 0x00, sizeof(szTempBuf2));

	memset(szTranName, 0x00, sizeof(szTranName));
	#if 1
	sprintf(szTranName, "%s", szTempBuf1);
	#else
	if (srTransRec.byTransType == CASH_OUT)
	{
		memset(szAcntType, 0x00, sizeof(szAcntType));
		sprintf(szAcntType, (srTransRec.inAccountType == SAVINGS_ACCOUNT ? "SA" : "CA"));
		sprintf(szTranName, "%s %s", szTempBuf1, szAcntType);
	}		
	else
	{
		sprintf(szTranName, "%s", szTempBuf1);
	}
	#endif
	
    /*invoice no.*/
    wub_hex_2_str(srTransRec.szInvoiceNo, szTempBuf2, INVOICE_BCD_SIZE);
	
	// Print Trxn Name, Invoince Number
	inPrintLeftRight(szTranName, szTempBuf2, 46);

	#if 0
    memset(szPrintBuf, 0x20, sizeof(szPrintBuf));
	memcpy(szPrintBuf, szTempBuf1, strlen(szTempBuf1));
	memcpy(&szPrintBuf[20], szTempBuf2, strlen(szTempBuf2));
	szPrintBuf[46]=0x00;
	inPrintNoErm(szPrintBuf);
	#endif
	
    /*total amount*/ 
    memset(szTempBuf1, 0, sizeof(szTempBuf1));
    memset(szTempBuf2, 0, sizeof(szTempBuf2));
    memset(szTempBuf3,0,sizeof(szTempBuf3));	
	wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf3, AMT_BCD_SIZE);
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTempBuf3, szTempBuf1);
    /*PAN*/ 
	strcpy(szTempBuf2,srTransRec.szPAN);    
	if(strHDT.fMaskDetail == TRUE)
	    cardMasking(szTempBuf2, PRINT_CARD_MASKING_3);

	// Print Amount, Card Number
	inPrintLeftRight(szTempBuf1, szTempBuf2, 46);
	
	#if 0
    memset(szPrintBuf, 0x20, sizeof(szPrintBuf));
	memcpy(szPrintBuf, szTempBuf1, strlen(szTempBuf1));
	memcpy(&szPrintBuf[20], szTempBuf2, strlen(szTempBuf2));
	szPrintBuf[46]=0x00;
	inPrintNoErm(szPrintBuf);
	#endif

	// Print Approval Code, RRef Num
	memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
	memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
	sprintf(szTempBuf1, "%s", srTransRec.szAuthCode);
	sprintf(szTempBuf2, "%s", srTransRec.szRRN);
	vdCTOS_Pad_String(szTempBuf1, 6, 0x30, POSITION_LEFT);
	vdCTOS_Pad_String(szTempBuf2, 12, 0x30, POSITION_LEFT);
	inPrintLeftRight(szTempBuf1, szTempBuf2, 46);
	
	/*date and time*/ 
    memset(szTempBuf1,0,sizeof(szTempBuf1));
    memset(szTempBuf2, 0, sizeof(szTempBuf2));
    printDateTime(szTempBuf1,szTempBuf2, TRUE);

	// Print Date, Time
	memset(szDate, 0x00, sizeof(szDate));
	memset(szTime, 0x00, sizeof(szTime));
	sprintf(szDate, "%s", szTempBuf1);
	sprintf(szTime, "%02lu:%02lu:%02lu", atol(szTempBuf2)/10000,atol(szTempBuf2)%10000/100, atol(szTempBuf2)%100);
	inPrintLeftRight(szDate, szTime, 46);
	
	#if 0
    memset(szPrintBuf, 0x20, sizeof(szPrintBuf));
    sprintf(szPrintBuf,"%s         %02lu:%02lu:%02lu",szTempBuf1,atol(szTempBuf2)/10000,atol(szTempBuf2)%10000/100, atol(szTempBuf2)%100);	
 	inPrintNoErm(szPrintBuf);
	#endif

	vdCTOS_PrinterFline(1);
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
	char szTemp1[d_LINE_SIZE + 1], szTemp2[d_LINE_SIZE + 1];
          int result;
          int inIITCount=0;
	BYTE szDisMsg[100];

	fReprintLastSettleFlag = 1;
		
	if( printCheckPaper()==-1)
		return d_NO;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return d_NO;
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

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("summary MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return d_NO;

	 strcpy(szDisMsg, "PRINT LAST SETTLE");
          strcat(szDisMsg, "|");
          strcat(szDisMsg, "PRINTING...");
          usCTOSS_LCDDisplay(szDisMsg);

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	inResult = inCTOS_ReadBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);
	if(inResult == ST_ERROR)
	{
	    vdSetErrorMessage("NO RECORD FOUND");
		vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		return ST_ERROR;	
	}		 
	else if(inResult == RC_FILE_READ_OUT_NO_DATA)
	{
	    vdSetErrorMessage("NO RECORD FOUND");
		return d_NO;
	}	 
    strcpy(srTransRec.szTID, srAccumRec.szTID);
    strcpy(srTransRec.szMID, srAccumRec.szMID);
    memcpy(srTransRec.szYear, srAccumRec.szYear, DATE_BCD_SIZE);
    memcpy(srTransRec.szDate, srAccumRec.szDate, DATE_BCD_SIZE);
    memcpy(srTransRec.szTime, srAccumRec.szTime, TIME_BCD_SIZE);
    memcpy(srTransRec.szBatchNo, srAccumRec.szBatchNo, BATCH_NO_BCD_SIZE);

    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);

	if (isCheckTerminalMP200() != d_OK)
	    inCTOS_DisplayPrintBMP();
	
	ushCTOS_PrintHeader(0);	

    vdPrintTitleCenter("LAST SETTLEMENT REPORT");
	
	vdCTOS_PrinterFline(1);

    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	/*	
    printHostLabel();
    
	printTIDMID();
    
	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	printDateTime(szTemp1, szTemp2);
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr,"DATE/TIME: %s %02lu:%02lu:%02lu",szTemp1,atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	result=inPrint(szStr);
	printBatchNO();
	*/

	vdPrintDateTimeAlignLeftRight();

	vdPrintTIDMIDVersion();

	vdPrintHostNameAndBatchNo();

    vdCTOS_PrinterFline(1);

	inIITCount=inIITNumRecord()+1;
	for(inTranCardType = 0; inTranCardType < 2 ;inTranCardType ++)
	{
		BOOL fPrintTotal=FALSE;
		inReportType = PRINT_CARD_TOTAL;
		inTranCardType = 1; // Debit
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <inIITCount; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashOutCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashInCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBillsPaymentCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usFundTransferCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usBalanceInquiryCount== 0))
					continue;
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				strcpy(szStr,strIIT.szIssuerLabel);
				inPrint(szStr);
                vdCTOS_PrintAccumeByHostAndCardCashMo(inReportType, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);
				fPrintTotal=TRUE;
			}
			//after print issuer total, then print host toal
			if(fPrintTotal == TRUE)
			{
				memset(szStr, ' ', d_LINE_SIZE);
				strcpy(szStr,"TOTAL:");
				inPrint(szStr);
                vdCTOS_PrintAccumeByHostAndCardCashMo (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 

				// For BancNet CashOut -- sidumili
				//vdCTOS_ServiceFeePrintAccumByHostAndCard(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);
			}
		}
		else
		{
			memset(szStr, ' ', d_LINE_SIZE);
			strcpy(szStr,srTransRec.szHostLabel);
			inPrint(szStr);
            vdCTOS_PrintAccumeByHostAndCardCashMo (inReportType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal); 
		}
	}
	
	vdPrintTitleCenter("SETTLEMENT CONFIRMED");
	
	vdCTOS_PrinterFline(2);
	
	vdLineFeed(FALSE);
	
	vdCTOSS_PrinterEnd();
	return d_OK;	
}

void vdCTOSS_PrinterBMPPic(unsigned int usX,unsigned int y, const char *path)
{
	char szBmpfile[100+1];

	vdDebug_LogPrintf("--vdCTOSS_PrinterBMPPic--");

	// No need to print logo when terminal is mp200 -- sidumili
	if (isCheckTerminalMP200() == d_OK) 
		return;
	
	memset(szBmpfile, 0x00, sizeof(szBmpfile));
	sprintf(szBmpfile,"%s%s", LOCAL_PATH, path);
	vdDebug_LogPrintf("Len[%d], szBmpfile[%s]", strlen(szBmpfile), szBmpfile);
	
	vdCTOSS_PrinterBMPPicEx(0, 0, szBmpfile);
	return ;
}

int inPrint(unsigned char *strPrint) 
{
	char szStr[46+1] = {0}; 

	//vdDebug_LogPrintf("inPrint | fERMTransaction[%d]", fERMTransaction);
	
	memset(szStr, 0x00, sizeof(szStr));
	memcpy(szStr, strPrint, strlen(strPrint));

	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB, fERMTransaction);
	return d_OK;
}

int inPrintNoErm(unsigned char *strPrint) 
{
	char szStr[46+1] = {0}; 
	//BYTE baTemp[PAPER_X_SIZE * 64] = {0};

	//vdDebug_LogPrintf("inPrint | fERMTransaction[%d]", fERMTransaction);
	
	memset(szStr, 0x00, sizeof(szStr));
	memcpy(szStr, strPrint, strlen(strPrint));

	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	//return CTOS_PrinterBufferOutput((BYTE *)baTemp, 4);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB, fERMTransaction);
	return d_OK;
}

int inPrintLeftRight(unsigned char *strLeft, unsigned char *strRight, int inWidth) 
{
	char szStr[64+1] = {0}; 
	BYTE baTemp[PAPER_X_SIZE * 64] = {0};

	//vdDebug_LogPrintf("inPrintLeftRight | fERMTransaction[%d]", fERMTransaction);
	
	if ((isCheckTerminalMP200() == d_OK) && (strTCT.byERMMode == 2))
	{
		inWidth = 46;
		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	
    inWidth--;
	
	memset(szStr, 0x20, sizeof(szStr));
    memcpy(szStr, strLeft, strlen(strLeft));
    memcpy(&szStr[inWidth-strlen(strRight)], strRight, strlen(strRight));
	szStr[inWidth]=0x00;
	
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB, fERMTransaction);
	return d_OK;
}

/*albert - start - Aug2014 - compute crc*/
void vdCTOS_PrintCRC(void)
{
    int shHostIndex = 1, inNumOfMerchant=0;
    int inResult,inRet;
    unsigned char szCRC[8+1];
    char ucLineBuffer[d_LINE_SIZE], szTemp1[d_LINE_SIZE+1], szTemp2[d_LINE_SIZE+1], szStr[d_LINE_SIZE+1];
    BYTE baTemp[PAPER_X_SIZE * 64];
	
    vduiLightOn();                

    //fix for issue #00092
    //Must display "Out of Paper..."
    if( printCheckPaper()==-1)
      return;
	
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    CTOS_LCDTClearDisplay();

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRINT_CRC);
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
		
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);

	if (isCheckTerminalMP200() != d_OK)
	    inCTOS_DisplayPrintBMP();

    fRePrintFlag = FALSE;
	
	//print Logo	
	if(strlen(strHDT.szHeaderLogoName) > 0)
		vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
	else
		vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");

    vdPrintTitleCenter("CRC REPORT");
	vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	vdCTOS_SetDateTime();
	
    //printDateTime();
	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	printDateTime(szTemp1, szTemp2, FALSE);
	memset(szStr, 0x00, d_LINE_SIZE);
    sprintf(szStr,"DATE/TIME: %s %2.2s:%2.2s:%2.2s",szTemp1, szTemp2, &szTemp2[2], &szTemp2[4]);
    inPrint(szStr);
	
	vdCTOS_PrinterFline(1);

    memset(baTemp, 0x00, sizeof(baTemp));
    sprintf(baTemp, "HOST: %s", strHDT.szHostLabel);
	inPrint(baTemp);

	// Print CRC
	vdPrintCRC();
	
    vdCTOS_PrinterFline(7);
	vdCTOSS_PrinterEnd();

}
/*albert - end - Aug2014 - compute crc*/

void vdCTOS_HostInfo(void){
int shHostIndex = 1, inNumOfMerchant=0;
int inResult,inRet;
unsigned char szCRC[8+1];
char ucLineBuffer[d_LINE_SIZE];
BYTE baTemp[PAPER_X_SIZE * 64];
int inLoop = 0;
char szBillerName[1024] = {0};
char szBillerCode[1024] = {0};
char szStr[d_LINE_SIZE + 1];
BYTE szTempBuf[12+1];
BYTE szBuf[50];
char szTemp1[d_LINE_SIZE + 1], szTemp2[d_LINE_SIZE + 1];
int result;
char szSerialNum[16 + 1];
char szStrLeft[d_LINE_SIZE + 1];
char szStrRight[d_LINE_SIZE + 1];
BYTE szDisMsg[100];
	
vdDebug_LogPrintf("-->>vdCTOS_HostInfo[START]");

vdSetGlobalVariable();

vduiLightOn();								
    if( printCheckPaper()==-1)
        return;

shHostIndex = inCTOS_SelectHostSetting();
if (shHostIndex == -1)
	return;

CTOS_LCDTClearDisplay();

// Commented to support print confirmation -- sidumili
#if 0
strcpy(szDisMsg, "HOST INFO  REPORT");
strcat(szDisMsg, "|");
strcat(szDisMsg, "PRINTING...");
usCTOSS_LCDDisplay(szDisMsg);
#endif

if (inMultiAP_CheckMainAPStatus() == d_OK)
{
	inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRINT_HOST_INFO);
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

//inRet = inCTOS_MultiAPReloadHost();
//if(d_OK != inRet)
	//return ;

//inCTOS_CheckAndSelectMutipleMID();
			
inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4); 

	if (isCheckTerminalMP200() != d_OK)
	    inCTOS_DisplayPrintBMP();
    
    //print Logo	
    if(strlen(strHDT.szHeaderLogoName) > 0)
        vdCTOSS_PrinterBMPPic(0, 0, strHDT.szHeaderLogoName);
    else
        vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");

    ushCTOS_PrintHeader(0);	
    
vdPrintTitleCenter("HOST INFO REPORT");
    vdCTOS_PrinterFline(1);

vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

#if 0
memset(szTemp1, 0x00, d_LINE_SIZE);
memset(szTemp2, 0x00, d_LINE_SIZE);
printDateTime(szTemp1, szTemp2);
memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr,"DATE/TIME: %s %02lu:%02lu:%02lu",szTemp1,atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
result=inPrint(szStr);
#endif

vdPrintDateTimeAlignLeftRight();

printDividingLine(DIVIDING_LINE);

memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr, "HOST        : %s", (char *)strHDT.szHostLabel);
inPrint(szStr);

memset(szTempBuf, 0x00, sizeof(szTempBuf));
wub_hex_2_str(strHDT.szTPDU,szTempBuf,5);
sprintf(szStr, "TPDU        : %s", szTempBuf);
inPrint(szStr);

memset(szTempBuf, 0x00, sizeof(szTempBuf));
wub_hex_2_str(strHDT.szNII,szTempBuf,2);
sprintf(szStr, "NII         : %s", szTempBuf);
inPrint(szStr);

memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr, "PRI TEL NUM : %s", (char *)strCPT.szPriTxnPhoneNumber);
inPrint(szStr);

memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr, "SEC TEL NUM : %s", (char *)strCPT.szSecTxnPhoneNumber);
inPrint(szStr);

memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr, "PRI IP      : %s", (char *)strCPT.szPriTxnHostIP);
inPrint(szStr);

memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr, "PRI IP PORT : %04ld", strCPT.inPriTxnHostPortNum);
inPrint(szStr);

memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr, "SEC IP      : %s", (char *)strCPT.szSecTxnHostIP);
inPrint(szStr);

memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr, "SEC IP PORT : %04ld", strCPT.inSecTxnHostPortNum);
inPrint(szStr);

memset(szSerialNum, 0, sizeof(szSerialNum));
//CTOS_GetFactorySN(szSerialNum);
usGetSerialNumber(szSerialNum);

szSerialNum[15]=0;
memset(szStr, 0x00, d_LINE_SIZE);
sprintf(szStr, "SERIAL NO   : %s", szSerialNum);
inPrint(szStr);

// Print CRC
vdPrintCRC();

printDividingLine(DIVIDING_LINE);

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
					 sprintf(szStr, "  MERCHANT : %s", strMMT[0].szMerchantName); 											 
					 inPrint(szStr);

					 memset(szStr, 0x00, sizeof(szStr));
					 sprintf(szStr, "  TID      : %s", strMMT[0].szTID); 											 
					 inPrint(szStr);

					 memset(szStr, 0x00, sizeof(szStr));
					 sprintf(szStr, "  MID      : %s", strMMT[0].szMID); 											 
					 inPrint(szStr);

					 memset(szStr, 0x00, sizeof(szStr));
					 memset(szBuf, 0x00, sizeof(szBuf));
					 wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
					 sprintf(szStr, "  BATCH NO : %s", szBuf); 											 
					 inPrint(szStr);
					 vdCTOS_PrinterFline(1);

			 }										

	 }
}

    vdLineFeed(FALSE);
	
	//vdCTOSS_PrinterEnd(); // Commented to support print confirmation -- sidumili
	
	// Capture report -- sidumili		
	vdDebug_LogPrintf("Capturing erm report...");
	vdDeleteBMP();
	vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
	CTOS_KBDBufFlush();
	inCTOSS_ERM_Form_Receipt(0);
			
	if (strGBLVar.fGBLvConfirmPrint)
	{
		// Print confirmation -- sidumili
		if (inPrintConfirmation("HOST REPORT", FALSE) == d_OK)
		{
			vdCTOSS_PrinterBMPPicEx(0, 0, "/home/ap/pub/Print_BMP.bmp");
			vdCTOSS_PrinterEnd();
		}
		else
		{			
			vdCTOSS_PrinterEnd();
		}
	}
	else
	{
		usCTOSS_LCDDisplay("HOST REPORT|PRINTING...");
		vdCTOSS_PrinterEnd();
	}
	
    vdDebug_LogPrintf("-->>vdCTOS_HostInfo[END]");
}

int ushCTOS_ePadSignature(void)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    ULONG ulFileSize;
	char szNewFileName[24+1];
    BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];
    DISPLAY_REC szDisplayRec;
	BYTE szDisMsg[50];
	BYTE szFormattedAmount[45];
	BYTE szTitle[25+1];

    //tine: android - set ui thread to display nothing
	DisplayStatusLine(" ");
	
	if (FALSE ==strTCT.fSignatureFlag)
		return d_OK;

	memset(&szDisplayRec,0x00,sizeof(DISPLAY_REC));

	memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
	
	memset(szStr, 0x00, sizeof(szStr));
	memset(szFormattedAmount, 0x00, sizeof(szFormattedAmount));

    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTotalAmt, szStr);
    sprintf(szFormattedAmount, "%s %s", strCST.szCurSymbol,szStr);

	vdDebug_LogPrintf("Before epad signature...");

	//TINE:  android
	memset(&szDisplayRec,0x00,sizeof(DISPLAY_REC));
	//strcpy(szDisplayRec.szDisplay1, "SIGN ON SCREEN 		");

	// sidumili: include total label and amount in signature panel display.
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "Total Amount:");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, szFormattedAmount);

	strcpy(szDisplayRec.szDisplay1, szDisMsg);
		  
	ePad_SignatureCaptureLibEex(&szDisplayRec);

	vdDebug_LogPrintf("After epad signature...");
	
	//ePad_SignatureCaptureLib();
	return d_OK;
}

int ushCTOS_ePadPrintSignature(void)
{
    char szNewFileName[24+1];
    ULONG ulFileSize;


	if (FALSE ==strTCT.fSignatureFlag)
		return d_NO;

	/*if(srTransRec.byTransType == VOID
		|| srTransRec.byTransType == SALE_TIP)
		return d_NO; */

    vdDebug_LogPrintf("ushCTOS_ePadPrintSignature");

    if(fRePrintFlag == TRUE)
    {
        memset(szNewFileName, 0x00, sizeof(szNewFileName));
        vdGetSignatureFilename(&szNewFileName);
        ulFileSize=lnGetFileSize(szNewFileName);
        vdDebug_LogPrintf("AAA szNewFileName:[%s], AAA ulFileSize:[%ld]",szNewFileName, ulFileSize);
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
		vdDebug_LogPrintf("AAA DB_SIGN_BMP:[%s], AAA ulFileSize:[%ld]",DB_SIGN_BMP, ulFileSize);
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

USHORT ushCTOS_PrintLoyaltyBody(int page)
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
    struct LOYALTY_DATA stLoyData;
    int inIndex=0;

    if(strTCT.fDemo == 1)
    {
        memset(srTransRec.szLoyaltyData, 0x00, sizeof(srTransRec.szLoyaltyData));
        memcpy(srTransRec.szLoyaltyData, "0000000011110000000022220000000033330000000044400000000555000000006660000000000770000000088800000000999 R", 105);
	}

    memset(&stLoyData, 0x00, sizeof(struct LOYALTY_DATA));
    vdExtractLoyaltyData(&stLoyData, &srTransRec.szLoyaltyData);

	if(CUSTOMER_COPY_RECEIPT == page)
	{
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0); 
		if(srTransRec.byTransType == LOY_BAL_INQ)
			vdPrintCenter("POINTS INQUIRY");
		else if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
			vdPrintCenter("REAL TIME REWARDS REDEMPT.");
		else if(srTransRec.byTransType == VOID)
			vdPrintCenter("VOID REAL TIME REWARDS REDEMPT.");

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);	

		vdCTOS_PrinterFline(1);

	    memset (szTemp, 0x00, sizeof(szTemp));
        vdCTOS_FormatPAN(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szTemp, strIIT.fMaskCustCopy);
         inPrintLeftRight("CARD NUMBER", szTemp, 46);
		
 	    TrimTrail(srTransRec.szCardholderName);
        inPrintLeftRight("CARDHOLDER NAME", srTransRec.szCardholderName, 46);
		
		vdCTOS_PrinterFline(1);

		if(srTransRec.byTransType == LOY_BAL_INQ)
		{
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szPointBalance, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szSaleAmount)/100), (atol(stLoyData.szSaleAmount)%100)); 
            inPrintLeftRight("POINTS BALANCE", szTemp, 46);
    		
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szEPurseBalance, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemAmountAmount)/100), (atol(stLoyData.szRedeemAmountAmount)%100));
            inPrintLeftRight("PESO VALUE OF POINTS", szTemp, 46);			
		}
		else if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
		{
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szSaleAmount, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szSaleAmount)/100), (atol(stLoyData.szSaleAmount)%100)); 
            inPrintLeftRight("TOTAL AMOUNT DUE", szTemp, 46);
    		
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szRedeemAmountAmount, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemAmountAmount)/100), (atol(stLoyData.szRedeemAmountAmount)%100));
            inPrintLeftRight("AMOUNT CHARGE TO POINTS", szTemp, 46);
    
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szNetSalesAmount, szTemp);
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szNetSalesAmount)/100), (atol(stLoyData.szNetSalesAmount)%100));
            inPrintLeftRight("AMOUNT CHARGE TO C. CARD", szTemp, 46);
     
    		vdCTOS_PrinterFline(1);
    
    		memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szBeginPointBalance, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szBeginPointBalance)/100), (atol(stLoyData.szBeginPointBalance)%100));
     		inPrintLeftRight("BEGINNING BALANCE POINTS", szTemp, 46);
    
    
     		memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szRedeemPoints, szTemp);
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemPoints)/100), (atol(stLoyData.szRedeemPoints)%100));
      		inPrintLeftRight("POINTS USED FOR PAYMENT", szTemp, 46);
    
     		memset (szTemp, 0x00, sizeof(szTemp));
            //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szPointBalance, szTemp);	
    		sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szPointBalance)/100), (atol(stLoyData.szPointBalance)%100));
       		inPrintLeftRight("ENDING BALANCE: POINTS", szTemp, 46);
		}
		else if(srTransRec.byTransType == VOID)
		{
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szSaleAmount, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szSaleAmount)/100), (atol(stLoyData.szSaleAmount)%100)); 
            inPrintLeftRight("AMOUNT REVERSED", szTemp, 46);
    		
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szRedeemAmountAmount, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemAmountAmount)/100), (atol(stLoyData.szRedeemAmountAmount)%100));
            inPrintLeftRight("AMOUNT REVERSED TO POINTS", szTemp, 46);
    
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szNetSalesAmount, szTemp);
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szNetSalesAmount)/100), (atol(stLoyData.szNetSalesAmount)%100));
            inPrintLeftRight("AMOUNT REVERSED TO CARD", szTemp, 46);
     
    		vdCTOS_PrinterFline(1);
    
    		memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szBeginPointBalance, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szBeginPointBalance)/100), (atol(stLoyData.szBeginPointBalance)%100));
     		inPrintLeftRight("BEGINNING BALANCE POINTS", szTemp, 46);
    
    
     		memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szRedeemPoints, szTemp);
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemPoints)/100), (atol(stLoyData.szRedeemPoints)%100));
      		inPrintLeftRight("POINTS REVERSED", szTemp, 46);
    
     		memset (szTemp, 0x00, sizeof(szTemp));
            //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szPointBalance, szTemp);	
    		sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szPointBalance)/100), (atol(stLoyData.szPointBalance)%100));
       		inPrintLeftRight("ENDING BALANCE: POINTS", szTemp, 46);	
		}
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else if(MERCHANT_COPY_RECEIPT == page)
	{
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0); 
		if(srTransRec.byTransType == LOY_BAL_INQ)
			vdPrintCenter("POINTS INQUIRY");
		else if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
			vdPrintCenter("REAL TIME REWARDS REDEMPT.");
		else if(srTransRec.byTransType == VOID)
			vdPrintCenter("VOID REAL TIME REWARDS REDEMPT.");

        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);	

		vdCTOS_PrinterFline(1);

	    memset (szTemp, 0x00, sizeof(szTemp));
        vdCTOS_FormatPAN(strIIT.szMaskMerchantCopy, srTransRec.szPAN, szTemp, strIIT.fMaskMerchCopy);
         inPrintLeftRight("CARD NUMBER", szTemp, 46);
		
 	    TrimTrail(srTransRec.szCardholderName);
        inPrintLeftRight("CARDHOLDER NAME", srTransRec.szCardholderName, 46);
		
        vdCTOS_PrinterFline(1);

		if(srTransRec.byTransType == LOY_BAL_INQ)
		{
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szPointBalance, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szSaleAmount)/100), (atol(stLoyData.szSaleAmount)%100)); 
            inPrintLeftRight("POINTS BALANCE", szTemp, 46);
    		
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szEPurseBalance, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemAmountAmount)/100), (atol(stLoyData.szRedeemAmountAmount)%100));
            inPrintLeftRight("PESO VALUE OF POINTS", szTemp, 46);			
		}
		else if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
		{
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szSaleAmount, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szSaleAmount)/100), (atol(stLoyData.szSaleAmount)%100)); 
            inPrintLeftRight("TOTAL AMOUNT DUE", szTemp, 46);
    		
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szRedeemAmountAmount, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemAmountAmount)/100), (atol(stLoyData.szRedeemAmountAmount)%100));
            inPrintLeftRight("AMOUNT CHARGE TO POINTS", szTemp, 46);
    
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szNetSalesAmount, szTemp);
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szNetSalesAmount)/100), (atol(stLoyData.szNetSalesAmount)%100));
            inPrintLeftRight("AMOUNT CHARGE TO C. CARD", szTemp, 46);
     
    		vdCTOS_PrinterFline(1);
    
    		memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szBeginPointBalance, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szBeginPointBalance)/100), (atol(stLoyData.szBeginPointBalance)%100));
     		inPrintLeftRight("BEGINNING BALANCE POINTS", szTemp, 46);
    
    
     		memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szRedeemPoints, szTemp);
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemPoints)/100), (atol(stLoyData.szRedeemPoints)%100));
      		inPrintLeftRight("POINTS USED FOR PAYMENT", szTemp, 46);
    
     		memset (szTemp, 0x00, sizeof(szTemp));
            //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szPointBalance, szTemp);	
    		sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szPointBalance)/100), (atol(stLoyData.szPointBalance)%100));
       		inPrintLeftRight("ENDING BALANCE: POINTS", szTemp, 46);
		}
		else if(srTransRec.byTransType == VOID)
		{
            memset(szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szSaleAmount, szTemp);
            //sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szSaleAmount)/100), (atol(stLoyData.szSaleAmount)%100)); 
            inPrintLeftRight("AMOUNT REVERSED", szTemp, 46);
    		
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szRedeemAmountAmount, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemAmountAmount)/100), (atol(stLoyData.szRedeemAmountAmount)%100));
            inPrintLeftRight("AMOUNT REVERSED TO POINTS", szTemp, 46);
    
            memset(szTemp, 0x00, sizeof(szTemp));        
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szNetSalesAmount, szTemp);
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szNetSalesAmount)/100), (atol(stLoyData.szNetSalesAmount)%100));
            inPrintLeftRight("AMOUNT REVERSED TO CARD", szTemp, 46);
     
           vdCTOS_PrinterFline(1);
    
    		memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szBeginPointBalance, szTemp);	
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szBeginPointBalance)/100), (atol(stLoyData.szBeginPointBalance)%100));
     		inPrintLeftRight("BEGINNING BALANCE POINTS", szTemp, 46);
    
    
     		memset (szTemp, 0x00, sizeof(szTemp));
            vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szRedeemPoints, szTemp);
    		//sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szRedeemPoints)/100), (atol(stLoyData.szRedeemPoints)%100));
      		inPrintLeftRight("POINTS REVERSED", szTemp, 46);
    
     		memset (szTemp, 0x00, sizeof(szTemp));
            //vdCTOS_FormatAmount("NNN,NNN,NNn.nn", stLoyData.szPointBalance, szTemp);	
    		sprintf(szTemp, "%10lu.%02lu", (atol(stLoyData.szPointBalance)/100), (atol(stLoyData.szPointBalance)%100));
       		inPrintLeftRight("ENDING BALANCE: POINTS", szTemp, 46);	
		}
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);	
	}
	return d_OK;	
	
}

USHORT ushCTOS_PrintLoyaltyFooter(int page)
{		
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
	int result;
    char szStr[d_LINE_SIZE + 1];
  	CTOS_RTC SetRTC;
	char szYear[3];
	char szTempDate[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];

	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	
	if(page == CUSTOMER_COPY_RECEIPT)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		{
		    EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			if((EMVtagVal[2]& 0x0F) == 2)
            {   
                vdCTOS_PrinterFline(2);
                vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
                    vdPrintCenter("(PIN VERIFY SUCCESS)");
            }
            else
            {
                //CTOS_PrinterFline(d_LINE_DOT * 3);
            }
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
        }

		ushCTOS_ePadPrintSignature();
		
		//CTOS_PrinterPutString("SIGN:_______________________________________");
		//printCardHolderName();
		vdCTOS_PrinterFline(1);
		
		//ushCTOS_PrintAgreement();
		
		if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot3);

        vdCTOS_PrinterFline(1);

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);	
		
	    memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
	    memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
        wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
        wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);
        memset(szTempDate, 0x00, sizeof(szTempDate));
        sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
        vdCTOS_FormatDate(szTempDate);
        sprintf(szTemp3, "DATE: %s", szTempDate);
		sprintf(szTemp4, "TIME: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
		inPrintLeftRight(szTemp3, szTemp4, 46);
 
        memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        sprintf(szTemp, "TID: %s", srTransRec.szTID);
        sprintf(szTemp2, "MID: %s", srTransRec.szMID);
	    inPrintLeftRight(szTemp, szTemp2, 46);
			
	    memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
	    memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
		wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
		wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
		sprintf(szTemp3, "BATCH NUM: %s", szTemp);
		sprintf(szTemp4, "TRACE NO.: %s", szTemp2);
	    inPrintLeftRight(szTemp3, szTemp4, 46);
		
	    memset(szStr, 0x00, d_LINE_SIZE);
        sprintf(szStr, "REF NO.   : %s", srTransRec.szRRN);
	    result=inPrint(szStr);
	
	    memset(szStr, 0x00, d_LINE_SIZE);
        sprintf(szStr, "APPR. CODE: %s", srTransRec.szAuthCode);
	    result=inPrint(szStr);

        if(strTCT.fProductCode == TRUE)
        {
            memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "PRODUCT CODE: %s", srTransRec.szProductCode);
            result=inPrint(szStr);
        }

        if(strTCT.fCustomerNo == TRUE)
        {
	        memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "CUSTOMER NUMBER: %s", srTransRec.szCustomerNo);
	        result=inPrint(szStr);
        }
		
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

		if(strHDT.fFooterLogo == TRUE)	
            vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");
			
		vdCTOS_PrinterFline(1); 
		vdPrintCenter("***** CUSTOMER COPY *****");
	}
	else
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
		{
			EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			if((EMVtagVal[2]& 0x0F) == 2)
            {         
                vdCTOS_PrinterFline(2);
                vdPrintCenter("*****NO SIGNATURE REQUIRED*****");
                if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
                    vdPrintCenter("(PIN VERIFY SUCCESS)");
            }
            else
            {
				if(strCDT.inType != DEBIT_CARD)
				{
					vdCTOS_PrinterFline(2);
					inPrint("SIGN:_______________________________________");
					TrimTrail(srTransRec.szCardholderName);
					vdPrintCenter(srTransRec.szCardholderName); 
				}
            }
		}
        else
        {
			if(strCDT.inType != DEBIT_CARD)
			{
				vdCTOS_PrinterFline(2);
				inPrint("SIGN:_______________________________________");
				TrimTrail(srTransRec.szCardholderName);
				vdPrintCenter(srTransRec.szCardholderName); 
			}
        }

        ushCTOS_ePadPrintSignature();
        
        //CTOS_PrinterPutString("SIGN:_______________________________________");
		//printCardHolderName();

	
		vdCTOS_PrinterFline(1);
		
		//ushCTOS_PrintAgreement();

        if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot3);

        vdCTOS_PrinterFline(1);

		vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);	
		
	    memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
	    memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
        wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
        wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);
        memset(szTempDate, 0x00, sizeof(szTempDate));
        sprintf(szTempDate, "%02lu%02lu%02lu", atol(srTransRec.szYear), atol(szTemp)/100, atol(szTemp)%100);
        vdCTOS_FormatDate(szTempDate);
        sprintf(szTemp3, "DATE: %s", szTempDate);
		sprintf(szTemp4, "TIME: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
		inPrintLeftRight(szTemp3, szTemp4, 46);
 
        memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        sprintf(szTemp, "TID: %s", srTransRec.szTID);
        sprintf(szTemp2, "MID: %s", srTransRec.szMID);
	    inPrintLeftRight(szTemp, szTemp2, 46);
			
	    memset(szTemp, 0x00, sizeof(szTemp));
        memset(szTemp2, 0x00, sizeof(szTemp2));
	    memset(szTemp3, 0x00, sizeof(szTemp3));
        memset(szTemp4, 0x00, sizeof(szTemp4));
		wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
		wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
		sprintf(szTemp3, "BATCH NUM: %s", szTemp);
		sprintf(szTemp4, "TRACE NO.: %s", szTemp2);
	    inPrintLeftRight(szTemp3, szTemp4, 46);
		
	    memset(szStr, 0x00, d_LINE_SIZE);
        sprintf(szStr, "REF NO.   : %s", srTransRec.szRRN);
	    result=inPrint(szStr);
	
	    memset(szStr, 0x00, d_LINE_SIZE);
        sprintf(szStr, "APPR. CODE: %s", srTransRec.szAuthCode);
	    result=inPrint(szStr);

        if(strTCT.fProductCode == TRUE)
        {
            memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "PRODUCT CODE: %s", srTransRec.szProductCode);
            result=inPrint(szStr);
        }

        if(strTCT.fCustomerNo == TRUE)
        {
	        memset(szStr, 0x00, d_LINE_SIZE);
            sprintf(szStr, "CUSTOMER NUMBER: %s", srTransRec.szCustomerNo);
	        result=inPrint(szStr);
        }
		
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

		if(strHDT.fFooterLogo == TRUE)	
            vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");

		vdCTOS_PrinterFline(1); 
		vdPrintCenter("***** MERCHANT COPY *****");
	}

	vdLineFeed(FALSE);
	
return d_OK;	

}

void vdCTOS_GetROOTFS(char* ucBuffer){
int inResult;

memset(ucBuffer, 0x00, sizeof(ucBuffer));
//inResult = CTOS_GetSystemInfo (ID_ROOTFS, ucBuffer);

return;
}

void vdCTOS_PrinterFline(USHORT usLines)
{
	while(usLines > 0)
	{
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		usLines--;
	}
}

#if 1
void vdCTOS_PrintReceiptCopies(BOOL BankCopy, BOOL CustCopy, BOOL MercCopy)
{
    BOOL fFirstReceipt=TRUE; 
    int inReceiptCtr=0;
    BYTE szPrintOption[3+1];
    BYTE chPrintOption=0x00;
	BYTE  key;
	int inKey;
	
	BYTE szTitle[20+1];
	BYTE szPrintDisplay[100];
	BOOL fOrientation = (BOOL)get_env_int("#ORIENTATION");
	BOOL fNextCopy = TRUE;

	ULONG ulFileSize;
	BOOL isCashOutFlag = (BOOL)get_env_int("CASHOUTFLAG"); 

	vdDebug_LogPrintf("TINE: S1_BANCNET --vdCTOS_PrintReceiptCopies--");
	vdDebug_LogPrintf("--vdCTOS_PrintReceiptCopies--");
	vdDebug_LogPrintf("BankCopy[%d], CustCopy[%d], MercCopy[%d]", BankCopy, CustCopy, MercCopy);
	vdDebug_LogPrintf("byTerminalType[%d], byTerminalModel[%d]", strTCT.byTerminalType, strTCT.byTerminalModel);
	vdDebug_LogPrintf("byERMMode[%d]", strTCT.byERMMode);
	vdDebug_LogPrintf("fOrientation[%d]", fOrientation);

	memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
	
    memset(szPrintOption, 0x00, sizeof(szPrintOption)); 
    memcpy(szPrintOption, strTCT.szPrintOption, 3); 
	inReceiptCtr=0;
	fFirstReceipt=TRUE;
	
	if (isCheckTerminalMP200() == d_OK)
	{
		// Minimize the white screend display -- sidumili
		vdCTOS_DispStatusMessage("PROCESSING...");
		
		ushCTOS_printAll(BANK_COPY_RECEIPT);
		vdCTOSS_PrinterEnd(); // For fast printing																	
    	CTOS_KBDBufFlush();//cleare key buffer
		inCTOSS_ERM_Form_Receipt(0);	
	}

	if (isCheckTerminalMP200() != d_OK && fOrientation == TRUE)
	{
		vdCTOS_DispStatusMessage("PROCESSING...");
//		vdDisplayMessageStatusBox(1, 8, "PROCESSING", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
		
		fERMTransaction = TRUE;
		vdCTOSS_PrinterStart(500);
		ushCTOS_printAll(BANK_COPY_RECEIPT);
		vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
                CTOS_KBDBufFlush();
		inCTOSS_ERM_Form_Receipt(0);
	}	
	//else
{
	    do
		    {
		        chPrintOption=(unsigned char)szPrintOption[inReceiptCtr];
		        if((chPrintOption == '1') && (BankCopy == TRUE)) /*bank copy*/
		        {
					if(fFirstReceipt == TRUE)
					{
						ushCTOS_printAll(BANK_COPY_RECEIPT);
						CTOS_KBDBufFlush();	
						fFirstReceipt=FALSE;
						if(strTCT.fPrintReceipt == TRUE)
						{
							inCTOS_DisplayPrintBMP();
							vdCTOSS_PrinterEnd();
						}
					}
					else
					{
						CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
						//clearLine(7);
						CTOS_LCDTClearDisplay();
						setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT BANK COPY?");
						setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");
		                while(1)
		                { 
		                	if(strTCT.fPrintReceipt != TRUE)
	                        break;
							
		                    //if (strTCT.fPrintWarningSound == TRUE)
		                    vduiWarningSound();
		                    
		                    CTOS_KBDHit(&key);
		                    if(key == d_KBD_ENTER)
		                    {
		                        ushCTOS_printAll(BANK_COPY_RECEIPT);
		                        CTOS_KBDBufFlush();//cleare key buffer
		                        if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
		                        break;
		                    }
		                    else if((key == d_KBD_CANCEL))
		                    {
		                        CTOS_LCDTClearDisplay();
		                        break;
		                    }
		                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		                    {
								ushCTOS_printAll(BANK_COPY_RECEIPT);
								if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
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
						CTOS_KBDBufFlush();//cleare key buffer	
						fFirstReceipt=FALSE;
						if(strTCT.fPrintReceipt == TRUE)
						{
							inCTOS_DisplayPrintBMP();
							vdCTOSS_PrinterEnd();
						}
					}
					else
					{
						CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
						//clearLine(7);
						CTOS_LCDTClearDisplay();
						setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT CUSTOMER COPY?");
						setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");
		                while(1)
		                { 
		                	if(strTCT.fPrintReceipt != TRUE)
	                        break;	
							
		                    //if (strTCT.fPrintWarningSound == TRUE)
		                    vduiWarningSound();
		                    
		                    CTOS_KBDHit(&key);
		                    if(key == d_KBD_ENTER)
		                    {
		                        ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
		                        CTOS_KBDBufFlush();//cleare key buffer
		                        if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
		                        break;
		                    }
		                    else if((key == d_KBD_CANCEL))
		                    {
		                        CTOS_LCDTClearDisplay();
		                        break;
		                    }
		                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		                    {
								ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
								if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
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
						CTOS_KBDBufFlush();//cleare key buffer	
						fFirstReceipt=FALSE;
						if(strTCT.fPrintReceipt == TRUE)
                        {
                            inCTOS_DisplayPrintBMP();
                            vdCTOSS_PrinterEnd();
                        }
					}
					else
					{
						CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
						//clearLine(7);				
						CTOS_LCDTClearDisplay();
						setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT MERCHANT COPY?");
						setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");
		                while(1)
		                { 
		                	if(strTCT.fPrintReceipt != TRUE)
	                        break;	
							
		                    //if (strTCT.fPrintWarningSound == TRUE) /*BDO PHASE 2: [Warning sound for printing flag] -- sidumili*/
		                    vduiWarningSound();
		                    
		                    CTOS_KBDHit(&key);
		                    if(key == d_KBD_ENTER)
		                    {
		                        ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
		                        CTOS_KBDBufFlush();//cleare key buffer
		                        if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
		                        break;
		                    }
		                    else if((key == d_KBD_CANCEL))
		                    {
		                        CTOS_LCDTClearDisplay();
		                        break;
		                    }
		                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		                    {
								ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
								if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
		                        break;
		                    }
		                }
					}
		        }  		
				inReceiptCtr++; 
		    }while(inReceiptCtr < 3);
	}
    if (isCashOutFlag)
    {
         if(strTCT.fSignatureFlag == TRUE)
         {
              ulFileSize=lnGetFileSize(DB_SIGN_BMP);
              if(ulFileSize > 0)
                   vdLinuxCommandFileCopy();
         }
    }
	vdCheckERMMode(); // Enhancement for printing speed -- sidumili
	
}
#else

void vdCTOS_PrintReceiptCopies(BOOL BankCopy, BOOL CustCopy, BOOL MercCopy)
{
    BOOL fFirstReceipt=TRUE; 
    int inReceiptCtr=0;
    BYTE szPrintOption[3+1];
    BYTE chPrintOption=0x00;
	BYTE  key;
	int inKey;
	
	BYTE szTitle[20+1];
	BYTE szPrintDisplay[48+1];
	BOOL fOrientation = (BOOL)get_env_int("#ORIENTATION");
	
	vdDebug_LogPrintf("--vdCTOS_PrintReceiptCopies--");
	vdDebug_LogPrintf("BankCopy[%d], CustCopy[%d], MercCopy[%d]", BankCopy, CustCopy, MercCopy);
	vdDebug_LogPrintf("byTerminalType[%d], byTerminalModel[%d]", strTCT.byTerminalType, strTCT.byTerminalModel);
	vdDebug_LogPrintf("byERMMode[%d]", strTCT.byERMMode);
	vdDebug_LogPrintf("fOrientation[%d]", fOrientation);

	memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
	
    memset(szPrintOption, 0x00, sizeof(szPrintOption)); 
    memcpy(szPrintOption, strTCT.szPrintOption, 3); 
	inReceiptCtr=0;
	fFirstReceipt=TRUE;
	
	if (isCheckTerminalMP200() == d_OK)
	{
		// Minimize the white screend display -- sidumili
		vdCTOS_DispStatusMessage("PROCESSING...");
		
		ushCTOS_printAll(BANK_COPY_RECEIPT);
		vdCTOSS_PrinterEnd(); // For fast printing																	
    	CTOS_KBDBufFlush();//cleare key buffer
		inCTOSS_ERM_Form_Receipt(0);	
	}

	if (isCheckTerminalMP200() != d_OK && fOrientation == TRUE)
	{
		vdCTOS_DispStatusMessage("PROCESSING...");
		
		fERMTransaction = TRUE;
		vdCTOSS_PrinterStart(500);
		ushCTOS_printAll(BANK_COPY_RECEIPT);
		vdCTOSS_PrinterEnd_CConvert2BMP("/home/ap/pub/Print_BMP.bmp");
    	CTOS_KBDBufFlush();
		inCTOSS_ERM_Form_Receipt(0);
	}	
	//else
	{
	    do
		    {
		        chPrintOption=(unsigned char)szPrintOption[inReceiptCtr];
		        if((chPrintOption == '1') && (BankCopy == TRUE)) /*bank copy*/
		        {
					if(fFirstReceipt == TRUE)
					{
					    vdDebug_LogPrintf("saturn print bank");
						ushCTOS_printAll(BANK_COPY_RECEIPT);
						CTOS_KBDBufFlush();	
						fFirstReceipt=FALSE;
						if(strTCT.fPrintReceipt == TRUE)
						{
							inCTOS_DisplayPrintBMP();
							vdCTOSS_PrinterEnd();
						}
					}
					else
					{
						CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
						//clearLine(7);
						vdDebug_LogPrintf("saturn print bank copy");
						memset(szPrintDisplay,0x00,sizeof(szPrintDisplay));
						strcpy(szPrintDisplay, szTitle);
						strcat(szPrintDisplay, "|");
						strcat(szPrintDisplay, "PRINT BANK COPY?");
						inKey=usCTOSS_Confirm(szPrintDisplay);
						
		                while(1)
		                { 
		                	if(strTCT.fPrintReceipt != TRUE)
	                        break;
							
		                    //if (strTCT.fPrintWarningSound == TRUE)
		                    vduiWarningSound();
		                    
		                    //CTOS_KBDHit(&key);
		                    if(inKey == d_OK)
		                    {
		                        ushCTOS_printAll(BANK_COPY_RECEIPT);
		                        CTOS_KBDBufFlush();//cleare key buffer
		                        if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
		                        break;
		                    }
		                    else if((inKey == d_USER_CANCEL))
		                    {
		                        CTOS_LCDTClearDisplay();
		                        break;
		                    }
							if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
							{
								ushCTOS_printAll(BANK_COPY_RECEIPT);
								if(strTCT.fPrintReceipt == TRUE)
								{
									inCTOS_DisplayPrintBMP();
									vdCTOSS_PrinterEnd();
								}
							
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
						CTOS_KBDBufFlush();//cleare key buffer	
						fFirstReceipt=FALSE;
						if(strTCT.fPrintReceipt == TRUE)
						{
							inCTOS_DisplayPrintBMP();
							vdCTOSS_PrinterEnd();
						}
					}
					else
					{
						CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
						vdDebug_LogPrintf("saturn print customer");
						memset(szPrintDisplay,0x00,sizeof(szPrintDisplay));
						strcpy(szPrintDisplay, szTitle);
						strcat(szPrintDisplay, "|");
						strcat(szPrintDisplay, "PRINT CUSTOMER COPY?");
						inKey=usCTOSS_Confirm(szPrintDisplay);
						
		                while(1)
		                { 
		                	if(strTCT.fPrintReceipt != TRUE)
	                        break;	
							
		                    //if (strTCT.fPrintWarningSound == TRUE)
		                    vduiWarningSound();
		                    
		                    CTOS_KBDHit(&key);
		                    if(inKey == d_OK)
		                    {
		                        ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
		                        CTOS_KBDBufFlush();//cleare key buffer
		                        if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
		                        break;
		                    }
		                    else if((inKey == d_USER_CANCEL))
		                    {
		                        CTOS_LCDTClearDisplay();
		                        break;
		                    }
							if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
							{
								ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
								if(strTCT.fPrintReceipt == TRUE)
								{
									inCTOS_DisplayPrintBMP();
									vdCTOSS_PrinterEnd();
								}
							
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
						CTOS_KBDBufFlush();//cleare key buffer	
						fFirstReceipt=FALSE;
						if(strTCT.fPrintReceipt == TRUE)
                        {
                            inCTOS_DisplayPrintBMP();
                            vdCTOSS_PrinterEnd();
                        }
					}
					else
					{
					  	CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
					    vdDebug_LogPrintf("saturn print merchant copy");
						memset(szPrintDisplay,0x00,sizeof(szPrintDisplay));
						strcpy(szPrintDisplay, szTitle);
						strcat(szPrintDisplay, "|");
						strcat(szPrintDisplay, "PRINT MERCHANT COPY?");
						vdDebug_LogPrintf("before user confirm");
						inKey=usCTOSS_Confirm(szPrintDisplay);
						vdDebug_LogPrintf("saturn szPrintDisplay %s",szPrintDisplay);
						vdDebug_LogPrintf("saturn print merchant copy %d", inKey);
						
		                while(1)
		                { 
		                	if(strTCT.fPrintReceipt != TRUE)
	                        break;	
							
		                    //if (strTCT.fPrintWarningSound == TRUE) /*BDO PHASE 2: [Warning sound for printing flag] -- sidumili*/
		                    vduiWarningSound();
		                    
		                    //CTOS_KBDHit(&key);
		                    if(inKey == d_OK)
		                    {
		                        ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
		                        CTOS_KBDBufFlush();//cleare key buffer
		                        if(strTCT.fPrintReceipt == TRUE)
		                        {
		                            inCTOS_DisplayPrintBMP();
		                            vdCTOSS_PrinterEnd();
		                        }
		                        break;
		                    }
		                    else if((inKey == d_USER_CANCEL))
		                    {
		                        CTOS_LCDTClearDisplay();
		                        break;
		                    }
							if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
							{
								ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
								if(strTCT.fPrintReceipt == TRUE)
								{
									inCTOS_DisplayPrintBMP();
									vdCTOSS_PrinterEnd();
								}
							
								break;
							}
		                }
					}
		        }  		
				inReceiptCtr++; 
		    }while(inReceiptCtr < 3);
	}

	vdCheckERMMode(); // Enhancement for printing speed -- sidumili
	
}

#endif

void vdPrintISOOption(unsigned char *pucTitle,unsigned char *uszData, int inLen)
{
	BYTE szTitle[36 + 1] = {0};

	memset(szTitle, 0, sizeof(szTitle));
	if(strTCT.inPrintISOOption == 1)
	{
		if(memcmp(pucTitle,"RX",2) == 0 && strCPT.inCommunicationMode == DIAL_UP_MODE)
			inPrintISOPacket(pucTitle , uszData, inLen-2);
		else
			inPrintISOPacket(pucTitle , uszData, inLen);
	}
	else if(strTCT.inPrintISOOption == 2)
	{
		szGetTransTitle(srTransRec.byTransType, szTitle);
		vdPrintParseISO(szTitle,pucTitle ,uszData+5);
	}
	else if(strTCT.inPrintISOOption == 3)
	{
		if(memcmp(pucTitle,"RX",2) == 0 && strCPT.inCommunicationMode == DIAL_UP_MODE)
			inPrintISOPacket(pucTitle , uszData, inLen-2);
		else
			inPrintISOPacket(pucTitle , uszData, inLen);

		szGetTransTitle(srTransRec.byTransType, szTitle);
		vdPrintParseISO(szTitle,pucTitle ,uszData+5);
		
	}

}

int inPrintISOPacket(unsigned char *pucTitle,unsigned char *pucMessage, int inLen)
{
    char ucLineBuffer[d_LINE_SIZE+4];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 4];
	
	//vdDebug_LogPrintf("inPrintISOPacket | fERMTransaction[%d]", fERMTransaction);
	
    if (inLen <= 0)
        return(ST_SUCCESS);

	//if (isCheckTerminalMP200() != d_OK)
	//	vdCTOSS_PrinterStart(100);
	
    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
	//adnroid-removed
	//CTOS_PrinterSetWorkTime(50000,1000);
	//end
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_16x16,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    
    memset(szStr, 0x00, sizeof(szStr));
	memset(baTemp, 0x00, sizeof(baTemp));
    sprintf(szStr,"[%s] [%d] \n", pucTitle, inLen);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB, fERMTransaction);

	vdCTOS_PrinterFline(1);
    
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
		inCCTOS_PrinterBufferOutput(ucLineBuffer,&stgFONT_ATTRIB, fERMTransaction);
    
    } 

	vdCTOS_PrinterFline(1);
    
    if (isCheckTerminalMP200() != d_OK)
	    vdCTOSS_PrinterEnd();
	
    return (ST_SUCCESS);
}

void vdRestoreReportTime(void)
{
	vdDebug_LogPrintf("--vdRestoreReportTime--");
		
	memcpy(srTransRec.szDate, szgERMDate, DATE_BCD_SIZE);
	memcpy(srTransRec.szTime, szgERMTime,  TIME_BCD_SIZE);

	vdDebug_LogPrintf("szgERMDate[%s]", szgERMDate);
	vdDebug_LogPrintf("szgERMTime[%s]", szgERMTime);
	vdDebug_LogPrintf("szDate[%s]", srTransRec.szDate);
	vdDebug_LogPrintf("szTime[%s]", srTransRec.szTime);
	
}

int inPrintBalanceInqReceipt(void)
{	
	int inResult;
	int shHostIndex;
	int inRet;

	if (isCheckTerminalMP200() != d_OK)
	{
		if (srTransRec.byTransType == BAL_INQ)
		return(d_OK);
	}
	
	vdDebug_LogPrintf("**vdPrintBalanceInqReceipt** - start");

	vdDebug_LogPrintf("fPrintBalInqCopy[%d]", strTCT.fPrintBalInqCopy);
	
	if (strTCT.fPrintBalInqCopy == FALSE)
		return d_OK;
	
    if( printCheckPaper()==-1)
        return d_NO;
	
	vdCTOSS_PrinterStart(100); // For fast printing
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);

	if (isCheckTerminalMP200() != d_OK)
		inCTOS_DisplayPrintBMP();

    fRePrintFlag = FALSE;
			
	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0); // small font

	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);  
	
	ushCTOS_PrintHeader(0);
		
	vdPrintTIDMIDVersion();
	
	vdCTOS_PrinterFline(1); // New Line
	
	vdPrintTrxnType();
			
	vdPrintCardTypeBankName();

	vdPrintAcntType();
	
	vdPrintDateTime();
	
	vdCTOS_PrinterFline(1); // New Line

	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	
	vdPrintBalanceAmount();

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 1, 0); // small font
		
	ushCTOS_PrintFooter(2); // Customer Copy
		
	vdCTOSS_PrinterEnd();

	vdDebug_LogPrintf("**vdPrintBalanceInqReceipt** - end");
	
    return d_OK;
 
}

void vdPrintTIDMIDVersion(void)
{
	char szStr[d_LINE_SIZE + 1];
    char szRootFS[d_LINE_SIZE + 1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

	#if 0
	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "MERCHANT              %s", srTransRec.szMID);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "TERMINAL ID           %s", srTransRec.szTID);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	vdCTOS_GetROOTFS(szRootFS);
	szRootFS[6]=0x00;
	sprintf(szStr, "OS VERSION            %s", szRootFS);
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "APPLICATION VERSION:  %s", "VERSION 1.0");
	inPrint(szStr);

	memset(szStr, 0x00, d_LINE_SIZE);
	sprintf(szStr, "MODEL VERSION         %s", "V3");
	inPrint(szStr);
	#endif

	//-->> TID / MID
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "TID: %s", srTransRec.szTID);
	memset(szStrRight, 0x00, d_LINE_SIZE);
	sprintf(szStrRight, "MID: %s", srTransRec.szMID);
	inPrintLeftRight(szStrLeft, szStrRight, 46);

	//-->>  OS VERSION / MODEL VERSION
	vdCTOS_GetROOTFS(szRootFS);
//	usGetRootFS(szRootFS);
	szRootFS[6]=0x00;
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "OS VERSION: %s", szRootFS);
	memset(szStrRight, 0x00, d_LINE_SIZE);
	sprintf(szStrRight, "MODEL VERSION: %s", MODEL_VERSION);
	inPrintLeftRight(szStrLeft, szStrRight, 46);

	//-->> APPLICATION VERSION
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "%s", "APPLICATION VERSION:");
	memset(szStrRight, 0x00, d_LINE_SIZE);
	sprintf(szStrRight, "%s", strTCT.szAppVersion); 
	inPrintLeftRight(szStrLeft, szStrRight, 46);

	if(strlen(srTransRec.szUserName) > 0)
        inPrintLeftRight("USER NAME:", srTransRec.szUserName, 46);
	else
		inPrintLeftRight("USER NAME:", strTCT.szUserName, 46);
}

void vdPrintTrxnType(void)
{
	char szTemp[d_LINE_SIZE + 1];
	char szStr[d_LINE_SIZE + 1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];
	char szAcntType[d_LINE_SIZE + 1];
	char szTranName[d_LINE_SIZE + 1];
	
	memset(szTemp, ' ', d_LINE_SIZE);
	if(srTransRec.fInstallment)
		strcpy(szTemp, srTransRec.szHostLabel);
	else
		szGetTransTitle(srTransRec.byTransType, szTemp);     

	memset(szTranName, 0x00, sizeof(szTranName));
	#if 1
	sprintf(szTranName, "%s", szTemp);
	#else
	if (srTransRec.byTransType == CASH_OUT)
	{
		memset(szAcntType, 0x00, sizeof(szAcntType));
		sprintf(szAcntType, (srTransRec.inAccountType == SAVINGS_ACCOUNT ? "SA" : "CA"));
		sprintf(szTranName, "%s %s", szTemp, szAcntType);
	}		
	else
	{
		sprintf(szTranName, "%s", szTemp);
	}
    #endif
	
	//-->> TRAN TYPE
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "%s", "TRAN TYPE:");
	memset(szStrRight, 0x00, d_LINE_SIZE);
	sprintf(szStrRight, "%s", szTranName);
	inPrintLeftRight(szStrLeft, szStrRight, 46);
}

void vdPrintCardTypeBankName(void)
{
	char szTemp[d_LINE_SIZE + 1];
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

	//-->> CARD#
	memset (baTemp, 0x00, sizeof(baTemp));
	vdCTOS_FormatPAN(strIIT.szMaskCustomerCopy, srTransRec.szPAN, baTemp, strIIT.fMaskCustCopy);
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "%s", "CARD NUM:");
	memset(szStrRight, 0x00, d_LINE_SIZE);
	sprintf(szStrRight, "%s", baTemp);
	inPrintLeftRight(szStrLeft, szStrRight, 46);

	//vdPrintExpDateEntryMode(); // Print Expiry Date / Entry Mode
	
	// BANK NAME
	memset(szTemp, 0x00, sizeof(szTemp));
	sprintf(szTemp, "%s", strCDT.szBank_Mnemonic);
	strcpy(szTemp,strCDT.szBank_Mnemonic);	
	//inCTOSS_GetEnvDB("BANKNAME", szTemp);

	vdDebug_LogPrintf("strCDT.szBank_Mnemonic[%s]", strCDT.szBank_Mnemonic);
	vdDebug_LogPrintf("szTemp[%s]", szTemp);
	
	memset(szStrLeft, 0x00, sizeof(szStrLeft));
	sprintf(szStrLeft, "%s", "BANK NAME:");
	memset(szStrRight, 0x00, sizeof(szStrRight));
	sprintf(szStrRight, "%s", szTemp);
	inPrintLeftRight(szStrLeft, szStrRight, 46);
}

void vdPrintBatchNoInvoiceNo(void)
{
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

	#if 0
	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	//wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3); /*Coreware*/
	srTransRec.szBancnetBatchNo[6]=0x00; /*Coreware*/
	wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
	memset(szStr, 0x00, d_LINE_SIZE);
	//sprintf(szStr, "BATCH # %s        INVOICE # %s", szTemp1, szTemp2); /*Coreware*/
	sprintf(szStr, "BATCH # %s        INVOICE # %s", srTransRec.szBancnetBatchNo, szTemp2); /*Coreware*/
	inPrint(szStr);
	#endif

	// -->> BATCH NUM / INVOICE NUM
	//srTransRec.szBancnetBatchNo[6]=0x00;
	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3); // sidumili: revert to its original function
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "BATCH NUM: %s", szTemp1);

	memset(szTemp2, 0x00, d_LINE_SIZE);
	wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
	memset(szStrRight, 0x00, d_LINE_SIZE);
	sprintf(szStrRight, "INVOICE NUM: %s", szTemp2);
	inPrintLeftRight(szStrLeft, szStrRight, 46);
	
}
#if 0
void vdPrintDateTime(void)
{
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szStr[d_LINE_SIZE + 1];
	ULONG ulHH=0L;
    char szAMPM[2+1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];
	
	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	printDateTime(szTemp1, szTemp2, TRUE);
	memset(szStr, 0x00, d_LINE_SIZE);

	ulHH=atol(szTemp2)/10000;
	memset(szAMPM, 0, sizeof(szAMPM));
	if(ulHH > 12)
	{
		ulHH-=12;
		strcpy(szAMPM,"PM"); 
	}
	else
		strcpy(szAMPM,"AM");			

	#if 0
	sprintf(szStr,"DATE %s      TIME %02lu:%02lu:%02lu%s",szTemp1,ulHH,atol(szTemp2)%10000/100, atol(szTemp2)%100, szAMPM);
	inPrint(szStr);	
	#endif

	//-->> DATE/TIME
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "DATE: %s", szTemp1);
	memset(szStrRight, 0x00, d_LINE_SIZE);
	sprintf(szStrRight, "TIME: %02lu:%02lu:%02lu%s", ulHH,atol(szTemp2)%10000/100, atol(szTemp2)%100, szAMPM);
	inPrintLeftRight(szStrLeft, szStrRight, 46);
}
#else
void vdPrintDateTime(void)
{
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szStr[d_LINE_SIZE + 1];
	ULONG ulHH=0L;
    char szAMPM[2+1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];
	
	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	printDateTime(szTemp1, szTemp2, TRUE);
	memset(szStr, 0x00, d_LINE_SIZE);

	vdDebug_LogPrintf("--vdPrintDateTime--");
	vdDebug_LogPrintf("szTemp1=[%s], szTemp2=[%s]", szTemp1, szTemp2);
	
	ulHH=atol(szTemp2)/10000;
	vdDebug_LogPrintf("ulHH=[%ld]", ulHH);
	memset(szAMPM, 0, sizeof(szAMPM));

	if (ulHH==0&& ulHH!=12)
	{
		ulHH=12;
		strcpy(szAMPM,"AM"); 
	}
	else if (ulHH==12 && ulHH!=0)
	{
		ulHH=12;
		strcpy(szAMPM,"PM"); 
	}
	else if(ulHH<12 && ulHH!=0)
	{
		strcpy(szAMPM,"AM"); 
	}
	else if(ulHH>12 && ulHH!=0)
	{
		ulHH-=12;
		strcpy(szAMPM,"PM"); 
	}
	
	#if 0
	sprintf(szStr,"DATE %s      TIME %02lu:%02lu:%02lu%s",szTemp1,ulHH,atol(szTemp2)%10000/100, atol(szTemp2)%100, szAMPM);
	inPrint(szStr);	
	#endif

	//-->> DATE/TIME
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "DATE: %s", szTemp1);
	memset(szStrRight, 0x00, d_LINE_SIZE);
	sprintf(szStrRight, "TIME: %02lu:%02lu:%02lu%s", ulHH,atol(szTemp2)%10000/100, atol(szTemp2)%100, szAMPM);
	inPrintLeftRight(szStrLeft, szStrRight, 46);
}

#endif
void vdPrintRRefNoTraceNo(void)
{
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szStr[d_LINE_SIZE + 1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

	#if 0
	memset(szTemp1, 0x00, d_LINE_SIZE);
	memset(szTemp2, 0x00, d_LINE_SIZE);
	sprintf(szTemp1, "REF # %s", srTransRec.szRRN); 
	szTemp1[18]=0;
	//sprintf(szTemp2, "TRACE # %06ld", srTransRec.ulTraceNum); /*Coreware*/
	sprintf(szTemp2, "TRACE # %s", srTransRec.szBancnetTraceNo); /*Coreware*/   
	szTemp2[14]=0;
	memset(szStr, 0x20, d_LINE_SIZE);
	memcpy(szStr,szTemp1,strlen(szTemp1));
	memcpy(&szStr[22],szTemp2,strlen(szTemp2)); szStr[d_LINE_SIZE]=0;
	inPrint(szStr);
	#endif

	vdDebug_LogPrintf("AAA - srTransRec.szBancnetBatchNo[%s]", srTransRec.szBancnetBatchNo);
	
	memset(szStrLeft, 0x00, d_LINE_SIZE);
	sprintf(szStrLeft, "REF NO: %s", srTransRec.szRRN);
	memset(szStrRight, 0x00, d_LINE_SIZE);
	memset(szTemp1, 0x00, sizeof(szTemp1));

	if (srTransRec.byTransType == CASH_OUT)
	{
		sprintf(szTemp1, "%06ld", srTransRec.ulTraceNum);
	}
	else
	{
		//sprintf(szTemp1, "%s", srTransRec.szBancnetTraceNo);
		sprintf(szTemp1, "%s", srTransRec.szBancnetBatchNo);
	}
	
	vdCTOS_Pad_String(szTemp1, 6, 0x30, POSITION_LEFT);
	sprintf(szStrRight, "TRACE NO: %s", szTemp1);
	inPrintLeftRight(szStrLeft, szStrRight, 46);
}

void vdPrintBalanceAmount(void)
{
    char szAmount[12+1];
        
    wub_hex_2_str(srTransRec.szTotalAmount, szAmount, AMT_BCD_SIZE);      
	vdPrintFormattedAmount("AMOUNT:", szAmount, 24);
}

int isCheckTerminalMP200(void) 
{
    if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
        return d_OK;
    return d_NO;
}

void vdPrintExpDateEntryMode(void)
{
	int i;
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	char szTemp5[d_LINE_SIZE + 1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

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
    
	// Print Expiry Date / Entry Mode
	memset(szStrLeft, 0x00, sizeof(szStrLeft));
	memset(szStrRight, 0x00, sizeof(szStrRight));
	sprintf(szStrLeft, "EXP: %s/%s", szTemp4, szTemp5);
	sprintf(szStrRight, "ENT: %s", szTemp1);		
	inPrintLeftRight(szStrLeft, szStrRight, 46);
}

void vdPrintDateTimeAlignLeftRight(void)
{
	char szTemp1[d_LINE_SIZE + 1] = {0};
	char szTemp2[d_LINE_SIZE + 1] = {0};
	char szStrLeft[d_LINE_SIZE + 1] = {0};
	char szStrRight[d_LINE_SIZE + 1] = {0};
	
	vdCTOS_SetDateTime();
	
	printDateTime(szTemp1, szTemp2, FALSE); // Get Date/Time

	// For Last Settlement
	memset(szgTempDate, 0x00, sizeof(szgTempDate));
    memset(szgTempTime, 0x00, sizeof(szgTempTime));
	memcpy(szgTempDate, szTemp1, DATE_BCD_SIZE);
	memcpy(szgTempTime, szTemp2, TIME_BCD_SIZE);

	// For ERM Settlement
	memset(szgERMDate, 0x00, sizeof(szgERMDate));
	memset(szgERMTime, 0x00, sizeof(szgERMTime));
	memcpy(srTransRec.szDate, szgERMDate, DATE_BCD_SIZE);
	memcpy(srTransRec.szTime, szgERMTime, TIME_BCD_SIZE);
	
	memset(szStrLeft, 0x00, sizeof(szStrLeft));
	sprintf(szStrLeft, "DATE: %s", szTemp1);
	memset(szStrRight, 0x00, sizeof(szStrRight));
	sprintf(szStrRight, "TIME: %02lu:%02lu:%02lu", atol(szTemp2)/10000,atol(szTemp2)%10000/100, atol(szTemp2)%100);
	inPrintLeftRight(szStrLeft, szStrRight, 46);
}

void vdPrintHostNameAndBatchNo(void)
{
	char szStrLeft[d_LINE_SIZE + 1] = {0};
	char szStrRight[d_LINE_SIZE + 1] = {0};
	char szTemp1[d_LINE_SIZE + 1] = {0};
	
	memset(szStrLeft, 0x00, sizeof(szStrLeft));
	memset(szStrRight, 0x00, sizeof(szStrRight));
	memset(szTemp1, 0x00, sizeof(szTemp1));
	sprintf(szStrLeft, "HOST: %s", srTransRec.szHostLabel);
	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
	sprintf(szStrRight, "BATCH NO: %s", szTemp1);
	inPrintLeftRight(szStrLeft, szStrRight, 46);
}

void vdPrintAcntType(void)
{
	char szStrLeft[d_LINE_SIZE + 1] = {0};
	char szStrRight[d_LINE_SIZE + 1] = {0};

	vdDebug_LogPrintf("--vdPrintAcntType--");
	vdDebug_LogPrintf("inAccountType=%d", srTransRec.inAccountType);		

	switch (srTransRec.inAccountType)
	{
		case CURRENT_ACCOUNT:
			strcpy(szStrRight, "CURRENT");
		break;
		case SAVINGS_ACCOUNT:
			strcpy(szStrRight, "SAVINGS");
		break;
		default:
			strcpy(szStrRight, "SAVINGS");
		break;	
	}

	inPrintLeftRight("ACCOUNT TYPE:", szStrRight, 46);
}

void vdPrintFooterAndLogo(void)
{
	// Do not print footer -- sidumili
	if (srTransRec.byTransType == BAL_INQ)
		return;
	
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	if(strlen(strMMT[0].szRctFoot1) > 0)
		vdPrintCenter(strMMT[0].szRctFoot1);
	if(strlen(strMMT[0].szRctFoot2) > 0)
		vdPrintCenter(strMMT[0].szRctFoot2);
	if(strlen(strMMT[0].szRctFoot3) > 0)
		vdPrintCenter(strMMT[0].szRctFoot3);

	if(strHDT.fFooterLogo == TRUE)	
	{
		if(strlen(strHDT.szFooterLogoName))
			vdCTOSS_PrinterBMPPic(0, 0, strHDT.szFooterLogoName);
		else	
	        vdCTOSS_PrinterBMPPic(0, 0, "footer.bmp");
	}
}

// Mercury Requirements -- sidumili
void vdPrintECRMessage(void)
{
	BOOL fMsg1 = FALSE;
	BOOL fMsg2 = FALSE;
	BOOL fMsg3 = FALSE;
	BOOL fMsg4 = FALSE;

	TrimTrail(srTransRec.szECRMessage1);
	TrimTrail(srTransRec.szECRMessage2);
	TrimTrail(srTransRec.szECRMessage3);
	TrimTrail(srTransRec.szECRMessage4);

	vdDebug_LogPrintf("szECRMessage1[%s]", srTransRec.szECRMessage1);
	vdDebug_LogPrintf("szECRMessage2[%s]", srTransRec.szECRMessage2);
	vdDebug_LogPrintf("szECRMessage3[%s]", srTransRec.szECRMessage3);
	vdDebug_LogPrintf("szECRMessage4[%s]", srTransRec.szECRMessage4);
		
	if (strlen(srTransRec.szECRMessage1) > 1)
	{
		fMsg1 = TRUE;
		inPrint(srTransRec.szECRMessage1);
	}
	
	if (strlen(srTransRec.szECRMessage2) > 1)
	{
		fMsg2 = TRUE;
		inPrint(srTransRec.szECRMessage2);
	}
	
	if (strlen(srTransRec.szECRMessage3) > 1)
	{
		fMsg3 = TRUE;
		inPrint(srTransRec.szECRMessage3);
	}
	
	if (strlen(srTransRec.szECRMessage4) > 1)
	{	
		fMsg4 = TRUE;
		inPrint(srTransRec.szECRMessage4);
	}

	if ((fMsg1) || (fMsg2) || (fMsg3) || (fMsg4))
		vdCTOS_PrinterFline(1);
}

void vdGetSignatureFilename(char *ptrNewFilename)
{
	char szNewFileName[24+1];
    char szInvoiceNo[6+1];

	vdDebug_LogPrintf("--vdGetSignatureFilename--");

    memset(szInvoiceNo, 0x00, sizeof(szInvoiceNo));
    wub_hex_2_str(srTransRec.szInvoiceNo, szInvoiceNo, INVOICE_BCD_SIZE);

    memset(szNewFileName, 0x00, sizeof(szNewFileName));
    sprintf(szNewFileName, "signERM%06ld.bmp", atol(szInvoiceNo));
	memcpy(ptrNewFilename, szNewFileName, strlen(szNewFileName));

	vdDebug_LogPrintf("AAA szInvoiceNo[%s]", szInvoiceNo);
	vdDebug_LogPrintf("AAA szNewFileName[%s]", szNewFileName);
}


void vdComputeGetAppCRC(char *szAppName, char *ptrCRC, BOOL fMP200)
{
	char szStr[40 + 1];	
	FILE* file;
	int nLen;
	unsigned char chBuffer[1024];
	BOOL fAppExist = FALSE;

	vdDebug_LogPrintf("--vdComputeGetAppCRC--");
	
	memset(szStr, 0x00, sizeof(szStr));
	memset(chBuffer, 0, 1024);

	strcpy(szAppName, "V5S_BANCNET");
	
	if ((file = fopen (szAppName, "rb")) != NULL)
	{
		fAppExist = TRUE;
		
	    while (nLen = fread (chBuffer, 1, 256, file))
	    {
	    }

	    sprintf(szStr, "%02x%02x%02x%02x", wub_lrc((unsigned char *) &chBuffer[0], 64),
	    wub_lrc((unsigned char *) &chBuffer[63], 64),
	    wub_lrc((unsigned char *) &chBuffer[127], 64),
	    wub_lrc((unsigned char *) &chBuffer[191], 64));	            
	    fclose (file);
	}

	#if 0
	// hardcode crc
	if (isCheckTerminalMP200() == d_OK)
		strcpy(szStr, "7D1C736E");
	else	
		strcpy(szStr, "7771314C");
	#endif
	
	// Put CRC to ENV
	put_env_char(szAppName, szStr);

	if (fAppExist)
	{
		if (fMP200)
			sprintf(ptrCRC, "%s", szStr);
		else	
			sprintf(ptrCRC, "CRC: %s", szStr);
	}
	else
		strcpy(ptrCRC, "N/A");

	vdDebug_LogPrintf("szAppName[%s] | ptrCRC[%s] | fAppExist[%d] | fMP200[%d]", szAppName, ptrCRC, fAppExist, fMP200);
	
}

// Put CRC value to ENV -- sidumili
void vdPutEnvCRC(void)
{
    unsigned char szCRC[8+1];	

	memset(szCRC, 0x00, sizeof(szCRC));
	
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
    	vdGetCRC();
        inCTOS_MultiAPALLAppEventID(d_IPC_CMD_PUT_ENV_CRC);       
    }
    else
    {
        vdGetCRC();
    }
}

void vdPrintDetailReportHeader(void)
{
	char szPrintBuf[d_LINE_SIZE + 1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

	/*
	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "CARD NAME");
	strcpy(szStrRight, "CARD NUMBER");	
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);	

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "EXP DATE");
	strcpy(szStrRight, "TRACE NO.");	
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "TRANSACTION");
	strcpy(szStrRight, "AMOUNT");	
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);

	memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
	memset (szStrLeft, 0x00, sizeof(szStrLeft));
	memset (szStrRight, 0x00, sizeof(szStrRight));
	strcpy(szStrLeft, "APPR. CODE");
	strcpy(szStrRight, "ENTRY MODE");	
	strcpy(szPrintBuf, szStrLeft);
	memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 20-strlen(szStrLeft));
	strcat(szPrintBuf, szStrRight);
	inPrint(szPrintBuf);
	*/

	inPrintLeftRight("TRANSACTION", "INVOICE", 46);
	inPrintLeftRight("AMOUNT", "CARD NUMBER", 46);
	inPrintLeftRight("APPR. CODE", "RREF NUM", 46);
	inPrintLeftRight("DATE", "TIME", 46);

}

void vdLogISOOption(unsigned char *pucTitle,unsigned char *uszData, int inLen)
{
	//vdDebug_LogPrintf2("============================================");
	
	if(memcmp(pucTitle,"RX",2) == 0 && strCPT.inCommunicationMode == DIAL_UP_MODE)
		inLogISOPacket(pucTitle , uszData, inLen-2);
	else
		inLogISOPacket(pucTitle , uszData, inLen);

	vdLogParseISO(srTransRec.byTransType,pucTitle ,uszData+5);

	//vdDebug_LogPrintf2("============================================");

}

int inLogISOPacket(unsigned char *pucTitle,unsigned char *pucMessage, int inLen)
{
    char ucLineBuffer[d_LINE_SIZE+4];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 4];

    if (inLen <= 0)
        return(ST_SUCCESS);

	//if (isCheckTerminalMP200() != d_OK)
	//	vdCTOSS_PrinterStart(100); // For fast printing
		
    //CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
	
	//CTOS_PrinterSetWorkTime(50000,1000);
    //inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_16x16,0," ");
	//vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    
    memset(szStr, 0x00, sizeof(szStr));
	memset(baTemp, 0x00, sizeof(baTemp));
    sprintf(szStr,"[%s] [%d] \n", pucTitle, inLen);
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	//if (isCheckTerminalMP200() == d_OK)		
	//	vdDebug_LogPrintf2(szStr);

		
    //CTOS_PrinterFline(d_LINE_DOT); 
	  //vdCTOS_PrinterFline(1);
    
    
    pucBuff = pucMessage + inLen;
    while (pucBuff > pucMessage)
    {
    memset(ucLineBuffer,0x00, sizeof(ucLineBuffer));
    for (inBuffPtr = 0; (inBuffPtr < 44) && (pucBuff > pucMessage); inBuffPtr += 3)
    {
    sprintf(&ucLineBuffer[inBuffPtr], "%02X ", *pucMessage);
    pucMessage++;
    }

	//if (isCheckTerminalMP200() == d_OK) 	
	//	vdDebug_LogPrintf2(ucLineBuffer);

    ucLineBuffer[44] = '\n';
    memset (baTemp, 0x00, sizeof(baTemp));		
    //inCCTOS_PrinterBufferOutput(ucLineBuffer, &stgFONT_ATTRIB,1);
    }
	//vdCTOS_PrinterFline(2);	
    //CTOS_PrinterFline(d_LINE_DOT * 2); 
   	//if (isCheckTerminalMP200() != d_OK)
    //	vdCTOSS_PrinterEnd();
    return (ST_SUCCESS);
}


USHORT GetDateTimeForERM(void)
{
	BYTE   EMVtagVal[64];
	USHORT EMVtagLen; 
	int result;
	char szStr[35 + 1];
	CTOS_RTC SetRTC;
	char szYear[3];
	char szTempDate[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
	char szTemp3[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	char szTermSerialNum[15+1]; // print terminal serial number on all txn receipt - mfl
	BYTE baTemp[PAPER_X_SIZE * 64];

	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);

	memset(szTemp, 0x00, sizeof(szTemp));
	memset(szTemp2, 0x00, sizeof(szTemp2));
	memset(szTemp3, 0x00, sizeof(szTemp3));
	memset(szTemp4, 0x00, sizeof(szTemp4));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	wub_hex_2_str(srTransRec.szTime, szTemp2,TIME_BCD_SIZE);
	
	return(ST_SUCCESS);
	
}

void vdLineFeed(BOOL fFooterLogo)
{
    int inLineFeed;
/*
	if(strTCT.byTerminalModel == V3_DESKTOP)
        inLineFeed=((fFooterLogo)?6:7);
    else if(strTCT.byTerminalModel == V3_MOBILE)
		inLineFeed=((fFooterLogo)?2:3);
	else
		inLineFeed=((fFooterLogo)?4:5);//for V5
*/	
    inLineFeed=((fFooterLogo)?3:4);
	vdCTOS_PrinterFline(inLineFeed);
}

int inCTOS_DisplayPrintBMP(void)
{
	vdDebug_LogPrintf("--inCTOS_DisplayPrintBMP--");
	vdDebug_LogPrintf("fImageAnimated[%d]", strTCT.fImageAnimated);
	vdDebug_LogPrintf("byTerminalType[%d]", strTCT.byTerminalType);
	
	if (strTCT.fImageAnimated == TRUE)
	{
		if ((strTCT.byTerminalType % 2) == 0)
			vdDisplayAnimateBmp(0, 0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
		else
			vdDisplayAnimateBmp(0, 0, "Print4.bmp", "Print5.bmp", "Print6.bmp", NULL, NULL);
	}
	else
	{
		if ((strTCT.byTerminalType % 2) == 0)
			displayAppbmpDataEx(0, 0, "Print1.bmp");
		else
			displayAppbmpDataEx(0, 0, "Print4.bmp");
	}
		
}

void vdPrintCRC(void)
{
	BYTE szCRC[50];
	BYTE szStr[50];
	BYTE szAPName[50];
	BYTE szPrintCRC[50] = {0};
	 
	memset(szCRC, 0x00, sizeof(szCRC));
	memset(szStr, 0x00, sizeof(szStr));
	memset(szPrintCRC, 0x00, sizeof(szPrintCRC));
	memset(szAPName, 0x00, sizeof(szAPName));

	//TINE:  android
	//inCTOSS_GetEnvDB("V5S_BANCNET_CRC", szCRC);
	getAppPackageInfo("com.Source.S1_BANCNET.BANCNET", szCRC);
	vdDebug_LogPrintf("szCRC[%s]", szCRC);

	strcpy(szAPName, "BANCNET     ");
	memset(szPrintCRC, 0x00, sizeof(szPrintCRC));
	//sprintf(szPrintCRC, "%s: %s", szAPName, "2139D3B09");
	sprintf(szPrintCRC, "%s: %s", szAPName, szCRC);
	inPrint(szPrintCRC);

}

USHORT ushCTOS_printReceipt_Diag(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
	ULONG ulFileSize;
	
    if( printCheckPaper()==-1)
    	return -1;

	if (isCheckTerminalMP200() == d_OK)
	{
		if (srTransRec.byTransType == LOY_BAL_INQ)
		return (d_OK);
	}

	// Revise for printer start mp200 -- sidumili
	if (!fPrinterStart)
		vdCTOSS_PrinterStart(100);
	
	vdCTOS_PrintReceiptCopies_Diag(TRUE, 0, 0, TRUE);
	
	//inCTOSS_ERM_Form_Receipt(0);
	
    return (d_OK);
}

void vdCTOS_PrintReceiptCopies_Diag(BOOL BankCopy, BOOL CustCopy, BOOL MercCopy, BOOL fERM)
{
    BOOL fFirstReceipt=TRUE; 
    int inReceiptCtr=0;
    BYTE szPrintOption[3+1];
    BYTE chPrintOption=0x00;
    BYTE  key;
    ULONG ulFileSize;

    //if (strTCT.byTerminalType == 5)
        //return;
	
    memset(szPrintOption, 0x00, sizeof(szPrintOption)); 
    memcpy(szPrintOption, strTCT.szPrintOption, 3); 
	inReceiptCtr=0;
	fFirstReceipt=TRUE;

	if (isCheckTerminalMP200() == d_OK)
	{	
		// Minimize the white screend display -- sidumili
		vdCTOS_DispStatusMessage("PROCESSING...");
		
		ushCTOS_printAll(BANK_COPY_RECEIPT);
		vdCTOSS_PrinterEnd(); // For fast printing																	
    	CTOS_KBDBufFlush();//cleare key buffer
		inCTOSS_ERM_Form_Receipt(0);	
	}
	else
	{
	    do
	    {
	        chPrintOption=(unsigned char)szPrintOption[inReceiptCtr];
	        if((chPrintOption == '1') && (BankCopy == TRUE)) /*bank copy*/
	        {
				if(fFirstReceipt == TRUE)
				{
					ushCTOS_printAll(BANK_COPY_RECEIPT);
					CTOS_KBDBufFlush();	
					fFirstReceipt=FALSE;
					if(strTCT.fPrintReceipt == TRUE)
					{
						inCTOS_DisplayPrintBMP();
						vdCTOSS_PrinterEnd();
					}
	                if(fERM == TRUE)
	                    inCTOSS_ERM_Form_Receipt(0);
				}
				else
				{
					CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
					//clearLine(7);
					CTOS_LCDTClearDisplay();
	                while(1)
	                { 
	                    if(strTCT.fPrintReceipt != TRUE)
	                        break;				
	                    
						setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT BANK COPY?");
						setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");
						
	                    //if (strTCT.fPrintWarningSound == TRUE)
	                    vduiWarningSound();
	                    
	                    CTOS_KBDHit(&key);
	                    if(key == d_KBD_ENTER)
	                    {
							ushCTOS_printAll(BANK_COPY_RECEIPT);
	                        CTOS_KBDBufFlush();//cleare key buffer
	                        if(strTCT.fPrintReceipt == TRUE)
	                        {
	                            inCTOS_DisplayPrintBMP();
	                            vdCTOSS_PrinterEnd();
	                        }
							if(fERM == TRUE)
								inCTOSS_ERM_Form_Receipt(0);
	                        break;
	                    }
	                    else if((key == d_KBD_CANCEL))
	                    {
	                        CTOS_LCDTClearDisplay();
	                        break;
	                    }
	                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
	                    {
							ushCTOS_printAll(BANK_COPY_RECEIPT);
	                        if(strTCT.fPrintReceipt == TRUE)
	                        {
	                            inCTOS_DisplayPrintBMP();
	                            vdCTOSS_PrinterEnd();
	                        }
							if(fERM == TRUE)
								inCTOSS_ERM_Form_Receipt(0);
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
					CTOS_KBDBufFlush();//cleare key buffer	
					fFirstReceipt=FALSE;
					if(strTCT.fPrintReceipt == TRUE)
					{
						inCTOS_DisplayPrintBMP();
						vdCTOSS_PrinterEnd();
					}				
				}
				else
				{
					CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
					//clearLine(7);
					CTOS_LCDTClearDisplay();
	                while(1)
	                { 
	                    if(strTCT.fPrintReceipt != TRUE)
	                        break;						

						setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT CUSTOMER COPY?");
						setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");
						
	                    vduiWarningSound();
	                    
	                    CTOS_KBDHit(&key);
	                    if(key == d_KBD_ENTER)
	                    {
							ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
	                        if(strTCT.fPrintReceipt == TRUE)
	                        {
	                            inCTOS_DisplayPrintBMP();
	                            vdCTOSS_PrinterEnd();
	                        }
	                        CTOS_KBDBufFlush();//cleare key buffer
	                        break;
	                    }
	                    else if((key == d_KBD_CANCEL))
	                    {
	                        CTOS_LCDTClearDisplay();
	                        break;
	                    }
	                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
	                    {
							ushCTOS_printAll(CUSTOMER_COPY_RECEIPT);
	                        if(strTCT.fPrintReceipt == TRUE)
	                        {
	                            inCTOS_DisplayPrintBMP();
	                            vdCTOSS_PrinterEnd();
	                        }
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
					CTOS_KBDBufFlush();//cleare key buffer	
					fFirstReceipt=FALSE;
					if(strTCT.fPrintReceipt == TRUE)
					{
						inCTOS_DisplayPrintBMP();
						vdCTOSS_PrinterEnd();
					}				
				}
				else
				{
					CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
					//clearLine(7);
					CTOS_LCDTClearDisplay();
	                while(1)
	                {	
	                    if(strTCT.fPrintReceipt != TRUE)
	                        break;						

						setLCDPrint(7, DISPLAY_POSITION_CENTER, "PRINT MERCHANT COPY?");
						setLCDPrint(8, DISPLAY_POSITION_CENTER, "NO[X] YES[OK]");

	                    vduiWarningSound();
	                    
	                    CTOS_KBDHit(&key);
	                    if(key == d_KBD_ENTER)
	                    {
							ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
	                        CTOS_KBDBufFlush();//cleare key buffer
	                        if(strTCT.fPrintReceipt == TRUE)
	                        {
	                            inCTOS_DisplayPrintBMP();
	                            vdCTOSS_PrinterEnd();
	                        }
	                        break;
	                    }
	                    else if((key == d_KBD_CANCEL))
	                    {
	                        CTOS_LCDTClearDisplay();
	                        break;
	                    }
	                    if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
	                    {
							ushCTOS_printAll(MERCHANT_COPY_RECEIPT);
	                        if(strTCT.fPrintReceipt == TRUE)
	                        {
	                            inCTOS_DisplayPrintBMP();
	                            vdCTOSS_PrinterEnd();
	                        }
	                        break;
	                    }
	                }
				}
	        }  		
			inReceiptCtr++; 
	    }while(inReceiptCtr < 3);
	}

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

int inPrintConfirmation(char* szTitle, BOOL fIncludeLogoHeader)
{
	int inKey = d_NO;
	BYTE szLogoPath[512];
    BYTE szMsg[512];
    BYTE szPrintMsg[512];
	BYTE szDisMsg[512];

	vdDebug_LogPrintf("--inPrintConfirmation--");
	vdDebug_LogPrintf("feReceiptEnable=[%d]", strTCT.feReceiptEnable);
	vdDebug_LogPrintf("szHeaderLogoName=[%s]", strHDT.szHeaderLogoName);
	
	if (strTCT.feReceiptEnable != 1)
		return d_NO;

	memset(szMsg, 0x00, sizeof(szMsg));
    memset(szPrintMsg, 0x00, sizeof(szPrintMsg));
	memset(szLogoPath, 0x00, sizeof(szLogoPath));	
	
	if (fIncludeLogoHeader)
	{
		if (strlen(strHDT.szHeaderLogoName) > 0)
			sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
		else
			sprintf(szLogoPath, "%s%s", LOCAL_PATH, "logo.bmp");
	}
	else
	{
		sprintf(szLogoPath, "%s%s", LOCAL_PATH, "blank.bmp");
	}

	vdDebug_LogPrintf("szLogoPath=[%s]", szLogoPath);
	
    strcpy(szMsg, "Print Confirmation");

	strcpy(szPrintMsg, szLogoPath);
	strcat(szPrintMsg, "|");
	strcat(szPrintMsg, szMsg);
		
	inKey = PrintReceiptUI(szPrintMsg);
	vdDebug_LogPrintf("inKey=[%d]", inKey);
				
	if (inKey == d_OK) 
	{
		// sidumili: check printer when selecting "PRINT"
		if( printCheckPaper()==-1)
    		return d_NO;
		
		memset(szDisMsg, 0x00, sizeof(szDisMsg));
		strcpy(szDisMsg, szTitle);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "PRINTING...");
		usCTOSS_LCDDisplay(szDisMsg);
	        
        return d_OK;
    }
	
    if (inKey == 0xFF) // timeout
    {
		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
		CTOS_Delay(1000);
		
		return d_NO;		
    }
	
    if (inKey == d_USER_CANCEL)
    {
		vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		CTOS_Beep();
		CTOS_Delay(1000);
		return d_NO;		
    }
}

// Delete bmp file -- sidumili
void vdDeleteBMP(void)
{
	char szSystemCmdPath[128+1];    
    char szNewFileName[24+1];

	vdDebug_LogPrintf("--vdDeleteBMP--");
	
    memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
    sprintf(szSystemCmdPath, "rm %s", "/home/ap/pub/Print_BMP.bmp");
    system(szSystemCmdPath);

	vdDebug_LogPrintf("szSystemCmdPath[%s]", szSystemCmdPath);
}

void vdCTOS_ServiceFeePrintAccumByHostAndCard(STRUCT_TOTAL Totals)
{
	USHORT usSaleCount = 0;
	ULONG ulSaleTotalAmount = 0;
	USHORT usRefundCount = 0;
	ULONG  ulRefundTotalAmount = 0;	 
	USHORT usVoidSaleCount = 0;
	ULONG  ulVoidSaleTotalAmount = 0;
	USHORT usOffSaleCount = 0;
	ULONG  ulOffSaleTotalAmount = 0;
	USHORT usTipCount = 0;
	ULONG  ulTipAmount = 0;
	USHORT usRedeemCount = 0;
	ULONG  ulRedeemTotalAmount = 0;

	// For BancNet CashOut -- sidumili
	USHORT usCashOutCount = 0;
	ULONG ulCashOutTotalAmount = 0;
	ULONG ulServiceFeeTotalAmount = 0;
		
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szFormattedAmount[d_LINE_SIZE + 1];
	USHORT usTotalCount;
	ULONG  ulTotalAmount;
	BYTE baTemp[PAPER_X_SIZE * 64];
	BOOL isCashOutFlag = (BOOL)get_env_int("CASHOUTFLAG"); // For BancNet CashOut -- sidumili

	if (!isCashOutFlag)
		return;
	
	vdDebug_LogPrintf("--vdCTOS_ServiceFeePrintAccumByHostAndCard--");
	
	// For BancNet CashOut -- sidumili
	usCashOutCount=Totals.usCashOutCount;
	ulCashOutTotalAmount=Totals.ulCashOutTotalAmount;
	ulServiceFeeTotalAmount=Totals.ulServiceFeeTotalAmount;
	ulTotalAmount=Totals.ulSaleTotalAmount + Totals.ulCashOutTotalAmount + Totals.ulServiceFeeTotalAmount;

	vdDebug_LogPrintf("usCashOutCount[%d] ulCashOutTotalAmount[%ld] ulServiceFeeTotalAmount[%ld] ulTotalAmount[%ld]", usCashOutCount, ulCashOutTotalAmount, ulServiceFeeTotalAmount, ulTotalAmount);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	printDividingLine(DIVIDING_LINE_DOUBLE);
	// Total Count
	memset(szStr, 0x00, sizeof(szStr));
	sprintf(szStr,"%d", usCashOutCount);
	inPrintLeftRight("CASHOUT COUNT:", szStr, 46);

	// Total CashOut Amount
	memset(szStr, 0x00, sizeof(szStr));
	memset(szFormattedAmount, 0x00, sizeof(szFormattedAmount));
	sprintf(szStr,"%ld", ulCashOutTotalAmount);
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szStr, szFormattedAmount);
	memset(szStr, 0x00, sizeof(szStr));
	sprintf(szStr, "%s %s", strCST.szCurSymbol, szFormattedAmount);
	inPrintLeftRight("TOTAL WITHDRAWAL AMOUNT:", szFormattedAmount, 46);

	// Total Service Fee
	memset(szStr, 0x00, sizeof(szStr));
	memset(szFormattedAmount, 0x00, sizeof(szFormattedAmount));
	sprintf(szStr,"%ld", ulServiceFeeTotalAmount);
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szStr, szFormattedAmount);
	memset(szStr, 0x00, sizeof(szStr));
	sprintf(szStr, "%s %s", strCST.szCurSymbol, szFormattedAmount);
	inPrintLeftRight("TOTAL SERVICE FEE:", szFormattedAmount, 46);

	// Total Amount
	memset(szStr, 0x00, sizeof(szStr));
	memset(szFormattedAmount, 0x00, sizeof(szFormattedAmount));
	sprintf(szStr,"%ld", ulTotalAmount);
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szStr, szFormattedAmount);
	memset(szStr, 0x00, sizeof(szStr));
	sprintf(szStr, "%s %s", strCST.szCurSymbol, szFormattedAmount);
	inPrintLeftRight("TOTAL AMOUNT:", szFormattedAmount, 46);

	vdCTOS_PrinterFline(1);
		
}

void vdCTOS_PrintAccumeByHostAndCardCashMo(int inReportType, STRUCT_TOTAL Totals)
{
	USHORT usSaleCount = 0;
	ULONG ulSaleTotalAmount = 0;
	USHORT usRefundCount = 0;
	ULONG  ulRefundTotalAmount = 0;	 
	USHORT usVoidSaleCount = 0;
	ULONG  ulVoidSaleTotalAmount = 0;
	USHORT usOffSaleCount = 0;
	ULONG  ulOffSaleTotalAmount = 0;
	USHORT usTipCount = 0;
	ULONG  ulTipAmount = 0;
	USHORT usRedeemCount = 0;
	ULONG  ulRedeemTotalAmount = 0;

	// For BancNet CashOut -- sidumili
	USHORT usCashOutCount = 0;
	ULONG ulCashOutTotalAmount = 0;
	USHORT usCashInCount = 0;
	ULONG ulCashInTotalAmount = 0;
	USHORT usBillsPaymentCount = 0;
	ULONG ulBillsPaymentTotalAmount = 0;
	USHORT usBalanceInquiryCount = 0;
	USHORT usFundTransferCount = 0;
	ULONG ulFundTransferTotalAmount = 0;
    USHORT usServiceFeeCount = 0;
	ULONG ulServiceFeeTotalAmount = 0;
    USHORT usAcquirerFeeCount = 0;
	ULONG ulAcquirerFeeTotalAmount = 0;
    USHORT usBillerFeeCount = 0;
	ULONG ulBillerFeeTotalAmount = 0;

    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szFormattedAmount[d_LINE_SIZE + 1];
    USHORT usTotalCount;
    ULONG  ulTotalAmount;
    BYTE baTemp[PAPER_X_SIZE * 64];
    BOOL isCashOutFlag = (BOOL)get_env_int("CASHOUTFLAG"); // For BancNet CashOut -- sidumili
    
    // For BancNet CashOut -- sidumili
    usCashOutCount=Totals.usCashOutCount;
    ulCashOutTotalAmount=Totals.ulCashOutTotalAmount;
	
    usCashInCount=Totals.usCashInCount;
    ulCashInTotalAmount=Totals.ulCashInTotalAmount;
    usBillsPaymentCount=Totals.usBillsPaymentCount;
    ulBillsPaymentTotalAmount=Totals.ulBillsPaymentTotalAmount;
    usBalanceInquiryCount=Totals.usBalanceInquiryCount;
    usFundTransferCount=Totals.usFundTransferCount;
    ulFundTransferTotalAmount=Totals.ulFundTransferTotalAmount;
    usServiceFeeCount=Totals.usServiceFeeCount;
    ulServiceFeeTotalAmount=Totals.ulServiceFeeTotalAmount;
    usAcquirerFeeCount=Totals.usAcquirerFeeCount;
    ulAcquirerFeeTotalAmount=Totals.ulAcquirerFeeTotalAmount;
    usBillerFeeCount=Totals.usBillerFeeCount;
    ulBillerFeeTotalAmount=Totals.ulBillerFeeTotalAmount;

    CTOS_PrinterSetHeatLevel(4);  
    memset (baTemp, 0x00, sizeof(baTemp));		
    
    //types
    memset(szStr, 0x00, d_LINE_SIZE);
    strcpy(szStr,"TYPES           COUNT     CUR             AMT");
    inPrint(szStr);

    //CashOut
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulCashOutTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"CASH OUT        %3d       %s",usCashOutCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //CashIn
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulCashInTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"CASH IN         %3d       %s",usCashInCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //Bills Payment
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulBillsPaymentTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"BILLS PAYMENT   %3d       %s",usBillsPaymentCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //Balance Inquiry
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", 0L);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"BAL INQUIRY     %3d       %s",usBalanceInquiryCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

	//Fund Transfer
	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, 0x00, d_LINE_SIZE);
	memset(szFormattedAmount, 0x00, d_LINE_SIZE);
	sprintf(szTemp,"%ld", ulFundTransferTotalAmount);
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
	sprintf(szStr,"FUND TRANSFER   %3d       %s",usFundTransferCount, strCST.szCurSymbol);
	inPrintLeftRight(szStr, szFormattedAmount, 46);

    //Service Fee
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulServiceFeeTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
	sprintf(szStr,"SERVICE FEE     %3d       %s",usServiceFeeCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //Service Fee
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulAcquirerFeeTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
	sprintf(szStr,"ACQUIRER FEE    %3d       %s",usAcquirerFeeCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

    //Service Fee
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, 0x00, d_LINE_SIZE);
    memset(szFormattedAmount, 0x00, d_LINE_SIZE);
    sprintf(szTemp,"%ld", ulBillerFeeTotalAmount);
    vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,szFormattedAmount);
    sprintf(szStr,"BILLER FEE      %3d       %s",usBillerFeeCount, strCST.szCurSymbol);
    inPrintLeftRight(szStr, szFormattedAmount, 46);

		
		inPrint("---------------------------------------------");
		
		usTotalCount = usCashOutCount + usCashInCount + usBillsPaymentCount + usFundTransferCount + usBalanceInquiryCount;
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0x00, d_LINE_SIZE);
		memset(szFormattedAmount, 0x00, d_LINE_SIZE);
		if (inReportType == PRINT_HOST_TOTAL)
		{
			#if 0
			if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount))
			{
				ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount);
				
				sprintf(szTemp,"%ld", ulTotalAmount);
				vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
				szFormattedAmount[0]='-';
			}
			else
			{
				ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount) - ulRefundTotalAmount;
				sprintf(szTemp,"%ld", ulTotalAmount);
				vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szFormattedAmount);
			}
			#endif
		}
		else if(inReportType == PRINT_CARD_TOTAL)
		{
			if(ulRefundTotalAmount > (ulCashOutTotalAmount+ulCashInTotalAmount+usBillsPaymentCount+ulFundTransferTotalAmount))
			{
				ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashOutTotalAmount);
				sprintf(szTemp,"%ld", ulTotalAmount);
				vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp,&szFormattedAmount[1]);
				szFormattedAmount[0]='-';
			}
			else
			{
				ulTotalAmount = (ulCashOutTotalAmount+ulCashInTotalAmount+ulBillsPaymentTotalAmount+ulFundTransferTotalAmount) - ulRefundTotalAmount;
				sprintf(szTemp,"%ld", ulTotalAmount);
				vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTemp, szFormattedAmount);
			}	 
		}
		sprintf(szStr,"TOTALS          %3d       %s", usTotalCount,strCST.szCurSymbol);
		inPrintLeftRight(szStr, szFormattedAmount, 46);

		vdCTOS_PrinterFline(1);
}

USHORT ushCTOS_printBatchRecordHeaderCashMo(void)
{
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1], szTemp1[DISPLAY_LINE_SIZE + 1], szTemp2[DISPLAY_LINE_SIZE + 1];
	char szRootFS[d_LINE_SIZE + 1];
    BYTE szLogoPath[256 + 1];

	memset(szLogoPath, 0x00, sizeof(szLogoPath));	

    sprintf(szLogoPath, "%s%s", LOCAL_PATH, strHDT.szHeaderLogoName);
//	vdCTOSS_PrinterBMPPicEx(0, 0, szLogoPath);
	
    ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("DETAIL REPORT");
	
	vdCTOS_PrinterFline(1);

	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	
	vdPrintDateTimeAlignLeftRight();

	vdPrintTIDMIDVersion();

	vdPrintHostNameAndBatchNo();

	vdCTOS_PrinterFline(1);

	memset(szStr, 0, sizeof(szStr));
	strcpy(szStr,"TRANSACTION     INVOICE");
	inPrintLeftRight(szStr, "TOTAL", 46);
	
	memset(szStr, 0, sizeof(szStr));
	strcpy(szStr,"CARD NUMBER");
	inPrintLeftRight(szStr, "DATE TIME", 46);

    inPrint("USER NAME");
	
	printDividingLine(DIVIDING_LINE_DOUBLE);
	
	return d_OK;
}

USHORT ushCTOS_printBatchRecordFooterCashMo(void)
{
	BYTE baTemp[PAPER_X_SIZE * 64];		
	char szStr[d_LINE_SIZE + 1];
	char szPrintBuf[d_LINE_SIZE + 1];
	char szTempBuf1[d_LINE_SIZE + 1];
	char szTempBuf2[d_LINE_SIZE + 1];
	char szTempBuf3[d_LINE_SIZE + 1];
	char szTempBuf4[d_LINE_SIZE + 1];
	char szDate[d_LINE_SIZE + 1];
	char szTime[d_LINE_SIZE + 1];
	char szAcntType[d_LINE_SIZE + 1];
	char szTranName[d_LINE_SIZE + 1];
	int i;

	DebugAddSTR("ushCTOS_printBatchRecordFooter", srTransRec.szPAN, 10);

    /*trans type*/
    memset(szTempBuf1,0,sizeof(szTempBuf1));
    memset(szTempBuf2, 0, sizeof(szTempBuf2));
    if(srTransRec.byTransType == LOY_REDEEM_5050 || srTransRec.byTransType == LOY_REDEEM_VARIABLE)
        strcpy(szTempBuf1, "REDEEM");
    else
        szGetTransTitle(srTransRec.byTransType, szTempBuf1);
    memset (szTempBuf2, 0x00, sizeof(szTempBuf2));

	memset(szTranName, 0x00, sizeof(szTranName));
	#if 1
	sprintf(szTranName, "%s", szTempBuf1);
	#else
	if (srTransRec.byTransType == CASH_OUT)
	{
		memset(szAcntType, 0x00, sizeof(szAcntType));
		sprintf(szAcntType, (srTransRec.inAccountType == SAVINGS_ACCOUNT ? "SA" : "CA"));
		sprintf(szTranName, "%s %s", szTempBuf1, szAcntType);
	}		
	else
	{
		sprintf(szTranName, "%s", szTempBuf1);
	}
	#endif
	
    /*invoice no.*/
	memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
    wub_hex_2_str(srTransRec.szInvoiceNo, szTempBuf1, INVOICE_BCD_SIZE);

    memset(szTempBuf2,0,sizeof(szTempBuf2));
	memset(szTempBuf3,0,sizeof(szTempBuf3));
	wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf2, AMT_BCD_SIZE);
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", szTempBuf2, szTempBuf3);

	memset(szTempBuf4, 0, sizeof(szTempBuf3));
	sprintf(szTempBuf4, "%s %s", strCST.szCurSymbol, szTempBuf3);
	
	// Print Trxn Name, Invoince Number, amount
	memset(szStr, 0, sizeof(szStr));
	sprintf(szStr, "%-16.16s%s", szTranName, szTempBuf1);
	inPrintLeftRight(szStr, szTempBuf4, 46);


	/*date and time*/ 
    memset(szTempBuf1,0,sizeof(szTempBuf1));
    memset(szTempBuf2, 0, sizeof(szTempBuf2));
    printDateTime(szTempBuf1,szTempBuf2, TRUE);

	// Print Date, Time
	memset(szDate, 0x00, sizeof(szDate));
	memset(szTime, 0x00, sizeof(szTime));
	sprintf(szDate, "%s", szTempBuf1);
	sprintf(szTime, "%02lu:%02lu:%02lu", atol(szTempBuf2)/10000,atol(szTempBuf2)%10000/100, atol(szTempBuf2)%100);
	//inPrintLeftRight(szDate, szTime, 46);
	
    /*PAN*/
	memset (szStr, 0x00, sizeof(szStr));
	strcpy(szStr,srTransRec.szPAN);    
	if(strHDT.fMaskDetail == TRUE)
	    cardMasking(szStr, PRINT_CARD_MASKING_3);

	memset(szTempBuf3,0,sizeof(szTempBuf3));
    sprintf(szTempBuf3, "%s %s", szDate, szTime);
	
    inPrintLeftRight(szStr, szTempBuf3, 46);

	//inPrint("ADMIN");
		memset(szStr, 0, sizeof(szStr));
	sprintf(szStr,"%s", srTransRec.szUserName);
	inPrint(szStr);
	vdCTOS_PrinterFline(1);
}

void vdPrintBillsPaymentDetails(void)
{
	char szTempBuf1[d_LINE_SIZE + 1];
	
    if(srTransRec.byTransType != BILLS_PAYMENT)
		return;

    vdDebug_LogPrintf("vdPrintBillsPaymentDetails");
		
    vdCTOS_PrinterFline(1);
	
	memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
	strcpy(szTempBuf1, srTransRec.szBillerName);
	inPrintLeftRight("PAYMENT TO:", szTempBuf1, 46);

	memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
	memcpy(szTempBuf1, srTransRec.szBillerCode, 4);
	inPrintLeftRight("BILLER CODE:", szTempBuf1, 46);

	memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
	strcpy(szTempBuf1, srTransRec.szSubscriberNo);
	inPrintLeftRight("SUBSCRIBER NO:", szTempBuf1, 46);

	vdDebug_LogPrintf("srTransRec.szBillerFee:%s", srTransRec.szBillerFee);
	memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn", srTransRec.szBillerFee, szTempBuf1);
	inPrintLeftRight("BILLER FEE:", szTempBuf1, 46);
}

void vdPrintEntryMode(void)
{
	int i;
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	char szTemp4[d_LINE_SIZE + 1];
	char szTemp5[d_LINE_SIZE + 1];
	char szStrLeft[d_LINE_SIZE + 1];
	char szStrRight[d_LINE_SIZE + 1];

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

#if 0
	if(srTransRec.byTransType == BILLS_PAYMENT && srTransRec.fBillsPaymentCash == VS_TRUE)
	{
		memset(szTemp1, 0, sizeof(szTemp1));
		memcpy(szTemp1,"Manual",6);
	}
#endif

	// Print Expiry Date / Entry Mode
	memset(szStrLeft, 0x00, sizeof(szStrLeft));
	memset(szStrRight, 0x00, sizeof(szStrRight));
	//sprintf(szStrLeft, "EXP: %s/%s", szTemp4, szTemp5);
	//sprintf(szStrRight, "ENT: %s", szTemp1);		
	inPrintLeftRight("ENT: ", szTemp1, 46);
}

 USHORT ushCTOS_PrintFooterCashMo(int page)
 {		 
	 BYTE	EMVtagVal[64];
	 USHORT EMVtagLen; 
	 
	 vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	 if(page == CUSTOMER_COPY_RECEIPT)
	 {
		 vdCTOS_PrinterFline(1);
		 
		 ushCTOS_PrintAgreement();
 
		 vdPrintFooterAndLogo();
		 
		 vdCTOS_PrinterFline(1); 
		 
		 vdPrintCenter("***** CUSTOMER COPY *****");
		 vdCTOS_PrinterFline(2);
	 }
	 else if(page == MERCHANT_COPY_RECEIPT || page == BANK_COPY_RECEIPT)
	 {
		vdCTOS_PrinterFline(2);
		vdPrintCenter("*****NO SIGNATURE REQUIRED*****");

         vdCTOS_PrinterFline(1);
         vdPrintCenter("(PIN VERIFY SUCCESS)");
		 vdCTOS_PrinterFline(1);
		 
		 ushCTOS_PrintAgreement();
 
		 vdPrintFooterAndLogo();
		 
		 vdCTOS_PrinterFline(1); 

		 if(page == MERCHANT_COPY_RECEIPT)
		     vdPrintCenter("***** MERCHANT COPY *****");
		 else
	         vdPrintCenter("***** BANK COPY *****");
		 	
		 vdCTOS_PrinterFline(2);
	 }
  
	 vdLineFeed((strHDT.fFooterLogo == TRUE)?TRUE:FALSE); // added -- sidumili
 
     return d_OK;	 
 }


