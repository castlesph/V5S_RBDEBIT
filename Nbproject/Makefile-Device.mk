#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=arm-brcm-linux-gnueabi-gcc
CCC=arm-brcm-linux-gnueabi-g++
CXX=arm-brcm-linux-gnueabi-g++
FC=g77.exe
AS=as

# Macros
CND_PLATFORM=Gnueabi-Windows
CND_DLIB_EXT=dll
CND_CONF=Device
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include NbMakefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Main/POSMain.o \
	${OBJECTDIR}/Trans/POSSale.o \
	${OBJECTDIR}/PCI100/COMMS.o \
	${OBJECTDIR}/Database/gpt.o \
	${OBJECTDIR}/Database/dct.o \
	${OBJECTDIR}/Ctls/POSWave.o \
	${OBJECTDIR}/Functionslist/POSFunctionKey.o \
	${OBJECTDIR}/Ctls/POSCtls.o \
	${OBJECTDIR}/Aptrans/MultiShareECR.o \
	${OBJECTDIR}/Trans/POSOffline.o \
	${OBJECTDIR}/Debug/debug.o \
	${OBJECTDIR}/Ui/Display.o \
	${OBJECTDIR}/Powrfail/POSPOWRFAIL.o \
	${OBJECTDIR}/Trans/TPSec.o \
	${OBJECTDIR}/Iso8583/pack.o \
	${OBJECTDIR}/Trans/POSTipAdjust.o \
	${OBJECTDIR}/Iso8583/V5IsoFunc.o \
	${OBJECTDIR}/Database/DatabaseFunc.o \
	${OBJECTDIR}/Print/Print.o \
	${OBJECTDIR}/Batch/POSbatch.o \
	${OBJECTDIR}/TMS/TMS.o \
	${OBJECTDIR}/Trans/POSVoid.o \
	${OBJECTDIR}/Trans/LocalFunc.o \
	${OBJECTDIR}/Aptrans/MultiAptrans.o \
	${OBJECTDIR}/Trans/POSRefund.o \
	${OBJECTDIR}/Database/prm.o \
	${OBJECTDIR}/Aptrans/MultiShareEMV.o \
	${OBJECTDIR}/Trans/POSInstallment.o \
	${OBJECTDIR}/Utils/MOLmd5.o \
	${OBJECTDIR}/Trans/POSDebit.o \
	${OBJECTDIR}/Utils/myEZLib.o \
	${OBJECTDIR}/PCI100/PCI100.o \
	${OBJECTDIR}/Trans/CardUtil.o \
	${OBJECTDIR}/Functionslist/POSFunctionsList.o \
	${OBJECTDIR}/Trans/POSTrans.o \
	${OBJECTDIR}/Trans/TLESec.o \
	${OBJECTDIR}/Database/ins.o \
	${OBJECTDIR}/PinPad/pinpad.o \
	${OBJECTDIR}/Utils/wub_lib.o \
	${OBJECTDIR}/Trans/POSHost.o \
	${OBJECTDIR}/Accum/accum.o \
	${OBJECTDIR}/Aptrans/MultiShareCOM.o \
	${OBJECTDIR}/Trans/LocalAptrans.o \
	${OBJECTDIR}/PCI100/PCI100des.o \
	${OBJECTDIR}/Trans/POSSettlement.o \
	${OBJECTDIR}/Filemodule/myFileFunc.o \
	${OBJECTDIR}/Erm/PosErm.o \
	${OBJECTDIR}/Trans/Encryption.o \
	${OBJECTDIR}/Ui/showbmp.o \
	${OBJECTDIR}/Comm/V5Comm.o \
	${OBJECTDIR}/Trans/POSAuth.o \
	${OBJECTDIR}/Setting/POSSetting.o \
	${OBJECTDIR}/Trans/POSLoyalty.o \
	${OBJECTDIR}/Trans/POSCardVer.o


# C Compiler Flags
CFLAGS="-I${SDKV5SINC}" -fsigned-char -Wundef -Wstrict-prototypes -Wno-trigraphs -Wimplicit -Wformat 

# CC Compiler Flags
CCFLAGS="-I${SDKV5SINC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 
CXXFLAGS="-I${SDKV5SINC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lcafont -lcafs -lcakms -lcalcd -lcamodem -lcapmodem -lcaprt -lcartc -lcauart -lcauldpm -lcausbh -lcagsm -lcabarcode -lpthread -ldl -lcaclvw -lcatls -lctosapi -lz -lssl -lcrypto -lcurl -lfreetype -lxml2 -lcaethernet -lv5smultiap -lv5sinput -lv5scfgexpress -lcasqlite -lcaxml -lv5sISOEnginee -lv5sEFT ../VEGA_Library/NETBEANS/DMenu/Ver0001/Lib/V5S_LibDMenu.a -lv5sEFTNAC -lv3_libepadso -lv5smultiap -lbluetooth -lcaclentry -lcaclmdl -lbmp -lbluetooth -lbmp -lcaclentry -lcaclmdl -lv5spbm -lcaqrcode

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/V5S/RBDEBIT/10V5S_App/V5S_RBDEBIT.exe

dist/V5S/RBDEBIT/10V5S_App/V5S_RBDEBIT.exe: ../VEGA_Library/NETBEANS/DMenu/Ver0001/Lib/V5S_LibDMenu.a

dist/V5S/RBDEBIT/10V5S_App/V5S_RBDEBIT.exe: ${OBJECTFILES}
	${MKDIR} -p dist/V5S/RBDEBIT/10V5S_App
	arm-brcm-linux-gnueabi-g++ -L . "-L${SDKV5SLIB}" "-L${SDKV5SLIBN}" -o dist/V5S/RBDEBIT/10V5S_App/V5S_RBDEBIT  ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main/POSMain.o: Main/POSMain.c 
	${MKDIR} -p ${OBJECTDIR}/Main
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Main/POSMain.o Main/POSMain.c

${OBJECTDIR}/Trans/POSSale.o: Trans/POSSale.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSSale.o Trans/POSSale.c

${OBJECTDIR}/PCI100/COMMS.o: PCI100/COMMS.c 
	${MKDIR} -p ${OBJECTDIR}/PCI100
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/PCI100/COMMS.o PCI100/COMMS.c

${OBJECTDIR}/Database/gpt.o: Database/gpt.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/gpt.o Database/gpt.c

${OBJECTDIR}/Database/dct.o: Database/dct.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/dct.o Database/dct.c

${OBJECTDIR}/Ctls/POSWave.o: Ctls/POSWave.c 
	${MKDIR} -p ${OBJECTDIR}/Ctls
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Ctls/POSWave.o Ctls/POSWave.c

${OBJECTDIR}/Functionslist/POSFunctionKey.o: Functionslist/POSFunctionKey.c 
	${MKDIR} -p ${OBJECTDIR}/Functionslist
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Functionslist/POSFunctionKey.o Functionslist/POSFunctionKey.c

${OBJECTDIR}/Ctls/POSCtls.o: Ctls/POSCtls.c 
	${MKDIR} -p ${OBJECTDIR}/Ctls
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Ctls/POSCtls.o Ctls/POSCtls.c

${OBJECTDIR}/Aptrans/MultiShareECR.o: Aptrans/MultiShareECR.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Aptrans/MultiShareECR.o Aptrans/MultiShareECR.c

${OBJECTDIR}/Trans/POSOffline.o: Trans/POSOffline.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSOffline.o Trans/POSOffline.c

${OBJECTDIR}/Debug/debug.o: Debug/debug.c 
	${MKDIR} -p ${OBJECTDIR}/Debug
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Debug/debug.o Debug/debug.c

${OBJECTDIR}/Ui/Display.o: Ui/Display.c 
	${MKDIR} -p ${OBJECTDIR}/Ui
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Ui/Display.o Ui/Display.c

${OBJECTDIR}/Powrfail/POSPOWRFAIL.o: Powrfail/POSPOWRFAIL.c 
	${MKDIR} -p ${OBJECTDIR}/Powrfail
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Powrfail/POSPOWRFAIL.o Powrfail/POSPOWRFAIL.c

${OBJECTDIR}/Trans/TPSec.o: Trans/TPSec.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/TPSec.o Trans/TPSec.c

${OBJECTDIR}/Iso8583/pack.o: Iso8583/pack.c 
	${MKDIR} -p ${OBJECTDIR}/Iso8583
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Iso8583/pack.o Iso8583/pack.c

${OBJECTDIR}/Trans/POSTipAdjust.o: Trans/POSTipAdjust.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSTipAdjust.o Trans/POSTipAdjust.c

${OBJECTDIR}/Iso8583/V5IsoFunc.o: Iso8583/V5IsoFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Iso8583
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Iso8583/V5IsoFunc.o Iso8583/V5IsoFunc.c

${OBJECTDIR}/Database/DatabaseFunc.o: Database/DatabaseFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/DatabaseFunc.o Database/DatabaseFunc.c

${OBJECTDIR}/Print/Print.o: Print/Print.c 
	${MKDIR} -p ${OBJECTDIR}/Print
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Print/Print.o Print/Print.c

${OBJECTDIR}/Batch/POSbatch.o: Batch/POSbatch.c 
	${MKDIR} -p ${OBJECTDIR}/Batch
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Batch/POSbatch.o Batch/POSbatch.c

${OBJECTDIR}/TMS/TMS.o: TMS/TMS.c 
	${MKDIR} -p ${OBJECTDIR}/TMS
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/TMS/TMS.o TMS/TMS.c

${OBJECTDIR}/Trans/POSVoid.o: Trans/POSVoid.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSVoid.o Trans/POSVoid.c

${OBJECTDIR}/Trans/LocalFunc.o: Trans/LocalFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/LocalFunc.o Trans/LocalFunc.c

${OBJECTDIR}/Aptrans/MultiAptrans.o: Aptrans/MultiAptrans.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Aptrans/MultiAptrans.o Aptrans/MultiAptrans.c

${OBJECTDIR}/Trans/POSRefund.o: Trans/POSRefund.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSRefund.o Trans/POSRefund.c

${OBJECTDIR}/Database/prm.o: Database/prm.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/prm.o Database/prm.c

${OBJECTDIR}/Aptrans/MultiShareEMV.o: Aptrans/MultiShareEMV.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Aptrans/MultiShareEMV.o Aptrans/MultiShareEMV.c

${OBJECTDIR}/Trans/POSInstallment.o: Trans/POSInstallment.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSInstallment.o Trans/POSInstallment.c

${OBJECTDIR}/Utils/MOLmd5.o: Utils/MOLmd5.cpp 
	${MKDIR} -p ${OBJECTDIR}/Utils
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Utils/MOLmd5.o Utils/MOLmd5.cpp

${OBJECTDIR}/Trans/POSDebit.o: Trans/POSDebit.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSDebit.o Trans/POSDebit.c

${OBJECTDIR}/Utils/myEZLib.o: Utils/myEZLib.c 
	${MKDIR} -p ${OBJECTDIR}/Utils
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Utils/myEZLib.o Utils/myEZLib.c

${OBJECTDIR}/PCI100/PCI100.o: PCI100/PCI100.c 
	${MKDIR} -p ${OBJECTDIR}/PCI100
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/PCI100/PCI100.o PCI100/PCI100.c

${OBJECTDIR}/Trans/CardUtil.o: Trans/CardUtil.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/CardUtil.o Trans/CardUtil.c

${OBJECTDIR}/Functionslist/POSFunctionsList.o: Functionslist/POSFunctionsList.c 
	${MKDIR} -p ${OBJECTDIR}/Functionslist
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Functionslist/POSFunctionsList.o Functionslist/POSFunctionsList.c

${OBJECTDIR}/Trans/POSTrans.o: Trans/POSTrans.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSTrans.o Trans/POSTrans.c

${OBJECTDIR}/Trans/TLESec.o: Trans/TLESec.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/TLESec.o Trans/TLESec.c

${OBJECTDIR}/Database/ins.o: Database/ins.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/ins.o Database/ins.c

${OBJECTDIR}/PinPad/pinpad.o: PinPad/pinpad.c 
	${MKDIR} -p ${OBJECTDIR}/PinPad
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/PinPad/pinpad.o PinPad/pinpad.c

${OBJECTDIR}/Utils/wub_lib.o: Utils/wub_lib.c 
	${MKDIR} -p ${OBJECTDIR}/Utils
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Utils/wub_lib.o Utils/wub_lib.c

${OBJECTDIR}/Trans/POSHost.o: Trans/POSHost.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSHost.o Trans/POSHost.c

${OBJECTDIR}/Accum/accum.o: Accum/accum.c 
	${MKDIR} -p ${OBJECTDIR}/Accum
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Accum/accum.o Accum/accum.c

${OBJECTDIR}/Aptrans/MultiShareCOM.o: Aptrans/MultiShareCOM.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Aptrans/MultiShareCOM.o Aptrans/MultiShareCOM.c

${OBJECTDIR}/Trans/LocalAptrans.o: Trans/LocalAptrans.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/LocalAptrans.o Trans/LocalAptrans.c

${OBJECTDIR}/PCI100/PCI100des.o: PCI100/PCI100des.c 
	${MKDIR} -p ${OBJECTDIR}/PCI100
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/PCI100/PCI100des.o PCI100/PCI100des.c

${OBJECTDIR}/Trans/POSSettlement.o: Trans/POSSettlement.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSSettlement.o Trans/POSSettlement.c

${OBJECTDIR}/Filemodule/myFileFunc.o: Filemodule/myFileFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Filemodule
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Filemodule/myFileFunc.o Filemodule/myFileFunc.c

${OBJECTDIR}/Erm/PosErm.o: Erm/PosErm.c 
	${MKDIR} -p ${OBJECTDIR}/Erm
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Erm/PosErm.o Erm/PosErm.c

${OBJECTDIR}/Trans/Encryption.o: Trans/Encryption.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/Encryption.o Trans/Encryption.c

${OBJECTDIR}/Ui/showbmp.o: Ui/showbmp.c 
	${MKDIR} -p ${OBJECTDIR}/Ui
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Ui/showbmp.o Ui/showbmp.c

${OBJECTDIR}/Comm/V5Comm.o: Comm/V5Comm.c 
	${MKDIR} -p ${OBJECTDIR}/Comm
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Comm/V5Comm.o Comm/V5Comm.c

${OBJECTDIR}/Trans/POSAuth.o: Trans/POSAuth.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSAuth.o Trans/POSAuth.c

${OBJECTDIR}/Setting/POSSetting.o: Setting/POSSetting.c 
	${MKDIR} -p ${OBJECTDIR}/Setting
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Setting/POSSetting.o Setting/POSSetting.c

${OBJECTDIR}/Trans/POSLoyalty.o: Trans/POSLoyalty.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSLoyalty.o Trans/POSLoyalty.c

${OBJECTDIR}/Trans/POSCardVer.o: Trans/POSCardVer.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSCardVer.o Trans/POSCardVer.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} dist/V5S/RBDEBIT/10V5S_App/V5S_RBDEBIT.exe

# Subprojects
.clean-subprojects:
