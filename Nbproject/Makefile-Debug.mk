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
CND_CONF=Debug
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
	${OBJECTDIR}/Trans/POSAutoReport.o \
	${OBJECTDIR}/Database/eft.o \
	${OBJECTDIR}/Trans/POSCashAdvance.o \
	${OBJECTDIR}/Database/pas.o \
	${OBJECTDIR}/Database/dct.o \
	${OBJECTDIR}/Aptrans/MultiShareECR.o \
	${OBJECTDIR}/Ctls/POSWave.o \
	${OBJECTDIR}/Trans/POSReload.o \
	${OBJECTDIR}/Ctls/POSCtls.o \
	${OBJECTDIR}/Trans/POSLogon.o \
	${OBJECTDIR}/Trans/POSBinVer.o \
	${OBJECTDIR}/POWRFAIL/POSPOWRFAIL.o \
	${OBJECTDIR}/Trans/POSOffline.o \
	${OBJECTDIR}/UTILS/wub_lib.o \
	${OBJECTDIR}/Trans/POSTipAdjust.o \
	${OBJECTDIR}/EFTSec/EFTSec.o \
	${OBJECTDIR}/Iso8583/V5IsoFunc.o \
	${OBJECTDIR}/Trans/POSCashierLoyalty.o \
	${OBJECTDIR}/Database/bvt.o \
	${OBJECTDIR}/Database/DatabaseFunc.o \
	${OBJECTDIR}/Trans/date.o \
	${OBJECTDIR}/UTILS/myEZLib.o \
	${OBJECTDIR}/UI/Display.o \
	${OBJECTDIR}/ACCUM/accum.o \
	${OBJECTDIR}/Iso8583/Iso.o \
	${OBJECTDIR}/Database/clt.o \
	${OBJECTDIR}/TMS/TMS.o \
	${OBJECTDIR}/Trans/POSVoid.o \
	${OBJECTDIR}/Trans/LocalFunc.o \
	${OBJECTDIR}/Aptrans/MultiAptrans.o \
	${OBJECTDIR}/Trans/POSRefund.o \
	${OBJECTDIR}/Aptrans/MultiShareEMV.o \
	${OBJECTDIR}/Trans/CardUtil.o \
	${OBJECTDIR}/Functionslist/POSFunctionsList.o \
	${OBJECTDIR}/Trans/POSTrans.o \
	${OBJECTDIR}/batch/POSbatch.o \
	${OBJECTDIR}/print/Print.o \
	${OBJECTDIR}/PinPad/pinpad.o \
	${OBJECTDIR}/Trans/POSHost.o \
	${OBJECTDIR}/Aptrans/MultiShareCOM.o \
	${OBJECTDIR}/Trans/LocalAptrans.o \
	${OBJECTDIR}/Trans/POSSettlement.o \
	${OBJECTDIR}/Erm/PosErm.o \
	${OBJECTDIR}/Trans/Encryption.o \
	${OBJECTDIR}/FileModule/myFileFunc.o \
	${OBJECTDIR}/Comm/V5Comm.o \
	${OBJECTDIR}/Trans/POSAuth.o \
	${OBJECTDIR}/Setting/POSSetting.o \
	${OBJECTDIR}/UI/showbmp.o \
	${OBJECTDIR}/Trans/POSBalanceInq.o \
	${OBJECTDIR}/Database/par.o \
	${OBJECTDIR}/DEBUG/debug.o


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
LDLIBSOPTIONS=-lcafont -lcafs -lcakms -lcalcd -lcamodem -lcapmodem -lcaprt -lcartc -lcauart -lcauldpm -lcausbh -lcagsm -lcabarcode -lpthread -ldl -lcaclvw -lcatls -lctosapi -lz -lssl -lcrypto -lcurl -lfreetype -lxml2 -lcaethernet -lv5smultiap -lv5sinput -lv5scfgexpress -lcasqlite -lcaxml -lv5sISOEnginee -lv5sEFT ../VEGA_Library/NETBEANS/DMenu/Ver0001/Lib/V5S_LibDMenu.a -lv5sEFTNAC -lv3_libepadso -lv5smultiap

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/V5S/BDODEBIT/10V5S_App/V5S_RBDEBIT.exe

dist/V5S/BDODEBIT/10V5S_App/V5S_RBDEBIT.exe: ../VEGA_Library/NETBEANS/DMenu/Ver0001/Lib/V5S_LibDMenu.a

dist/V5S/BDODEBIT/10V5S_App/V5S_RBDEBIT.exe: ${OBJECTFILES}
	${MKDIR} -p dist/V5S/BDODEBIT/10V5S_App
	arm-brcm-linux-gnueabi-g++ -L . "-L${SDKV5SLIB}" "-L${SDKV5SLIBN}" -o dist/V5S/BDODEBIT/10V5S_App/V5S_RBDEBIT  ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main/POSMain.o: Main/POSMain.c 
	${MKDIR} -p ${OBJECTDIR}/Main
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Main/POSMain.o Main/POSMain.c

${OBJECTDIR}/Trans/POSSale.o: Trans/POSSale.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSSale.o Trans/POSSale.c

${OBJECTDIR}/Trans/POSAutoReport.o: Trans/POSAutoReport.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSAutoReport.o Trans/POSAutoReport.c

${OBJECTDIR}/Database/eft.o: Database/eft.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/eft.o Database/eft.c

${OBJECTDIR}/Trans/POSCashAdvance.o: Trans/POSCashAdvance.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSCashAdvance.o Trans/POSCashAdvance.c

${OBJECTDIR}/Database/pas.o: Database/pas.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/pas.o Database/pas.c

${OBJECTDIR}/Database/dct.o: Database/dct.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/dct.o Database/dct.c

${OBJECTDIR}/Aptrans/MultiShareECR.o: Aptrans/MultiShareECR.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Aptrans/MultiShareECR.o Aptrans/MultiShareECR.c

${OBJECTDIR}/Ctls/POSWave.o: Ctls/POSWave.c 
	${MKDIR} -p ${OBJECTDIR}/Ctls
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Ctls/POSWave.o Ctls/POSWave.c

${OBJECTDIR}/Trans/POSReload.o: Trans/POSReload.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSReload.o Trans/POSReload.c

${OBJECTDIR}/Ctls/POSCtls.o: Ctls/POSCtls.c 
	${MKDIR} -p ${OBJECTDIR}/Ctls
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Ctls/POSCtls.o Ctls/POSCtls.c

${OBJECTDIR}/Trans/POSLogon.o: Trans/POSLogon.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSLogon.o Trans/POSLogon.c

${OBJECTDIR}/Trans/POSBinVer.o: Trans/POSBinVer.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSBinVer.o Trans/POSBinVer.c

${OBJECTDIR}/POWRFAIL/POSPOWRFAIL.o: POWRFAIL/POSPOWRFAIL.c 
	${MKDIR} -p ${OBJECTDIR}/POWRFAIL
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/POWRFAIL/POSPOWRFAIL.o POWRFAIL/POSPOWRFAIL.c

${OBJECTDIR}/Trans/POSOffline.o: Trans/POSOffline.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSOffline.o Trans/POSOffline.c

${OBJECTDIR}/UTILS/wub_lib.o: UTILS/wub_lib.c 
	${MKDIR} -p ${OBJECTDIR}/UTILS
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/UTILS/wub_lib.o UTILS/wub_lib.c

${OBJECTDIR}/Trans/POSTipAdjust.o: Trans/POSTipAdjust.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSTipAdjust.o Trans/POSTipAdjust.c

${OBJECTDIR}/EFTSec/EFTSec.o: EFTSec/EFTSec.c 
	${MKDIR} -p ${OBJECTDIR}/EFTSec
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/EFTSec/EFTSec.o EFTSec/EFTSec.c

${OBJECTDIR}/Iso8583/V5IsoFunc.o: Iso8583/V5IsoFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Iso8583
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Iso8583/V5IsoFunc.o Iso8583/V5IsoFunc.c

${OBJECTDIR}/Trans/POSCashierLoyalty.o: Trans/POSCashierLoyalty.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSCashierLoyalty.o Trans/POSCashierLoyalty.c

${OBJECTDIR}/Database/bvt.o: Database/bvt.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/bvt.o Database/bvt.c

${OBJECTDIR}/Database/DatabaseFunc.o: Database/DatabaseFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/DatabaseFunc.o Database/DatabaseFunc.c

${OBJECTDIR}/Trans/date.o: Trans/date.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/date.o Trans/date.c

${OBJECTDIR}/UTILS/myEZLib.o: UTILS/myEZLib.c 
	${MKDIR} -p ${OBJECTDIR}/UTILS
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/UTILS/myEZLib.o UTILS/myEZLib.c

${OBJECTDIR}/UI/Display.o: UI/Display.c 
	${MKDIR} -p ${OBJECTDIR}/UI
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/UI/Display.o UI/Display.c

${OBJECTDIR}/ACCUM/accum.o: ACCUM/accum.c 
	${MKDIR} -p ${OBJECTDIR}/ACCUM
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/ACCUM/accum.o ACCUM/accum.c

${OBJECTDIR}/Iso8583/Iso.o: Iso8583/Iso.c 
	${MKDIR} -p ${OBJECTDIR}/Iso8583
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Iso8583/Iso.o Iso8583/Iso.c

${OBJECTDIR}/Database/clt.o: Database/clt.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/clt.o Database/clt.c

${OBJECTDIR}/TMS/TMS.o: TMS/TMS.c 
	${MKDIR} -p ${OBJECTDIR}/TMS
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/TMS/TMS.o TMS/TMS.c

${OBJECTDIR}/Trans/POSVoid.o: Trans/POSVoid.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSVoid.o Trans/POSVoid.c

${OBJECTDIR}/Trans/LocalFunc.o: Trans/LocalFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/LocalFunc.o Trans/LocalFunc.c

${OBJECTDIR}/Aptrans/MultiAptrans.o: Aptrans/MultiAptrans.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Aptrans/MultiAptrans.o Aptrans/MultiAptrans.c

${OBJECTDIR}/Trans/POSRefund.o: Trans/POSRefund.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSRefund.o Trans/POSRefund.c

${OBJECTDIR}/Aptrans/MultiShareEMV.o: Aptrans/MultiShareEMV.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Aptrans/MultiShareEMV.o Aptrans/MultiShareEMV.c

${OBJECTDIR}/Trans/CardUtil.o: Trans/CardUtil.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/CardUtil.o Trans/CardUtil.c

${OBJECTDIR}/Functionslist/POSFunctionsList.o: Functionslist/POSFunctionsList.c 
	${MKDIR} -p ${OBJECTDIR}/Functionslist
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Functionslist/POSFunctionsList.o Functionslist/POSFunctionsList.c

${OBJECTDIR}/Trans/POSTrans.o: Trans/POSTrans.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSTrans.o Trans/POSTrans.c

${OBJECTDIR}/batch/POSbatch.o: batch/POSbatch.c 
	${MKDIR} -p ${OBJECTDIR}/batch
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/batch/POSbatch.o batch/POSbatch.c

${OBJECTDIR}/print/Print.o: print/Print.c 
	${MKDIR} -p ${OBJECTDIR}/print
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/print/Print.o print/Print.c

${OBJECTDIR}/PinPad/pinpad.o: PinPad/pinpad.c 
	${MKDIR} -p ${OBJECTDIR}/PinPad
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/PinPad/pinpad.o PinPad/pinpad.c

${OBJECTDIR}/Trans/POSHost.o: Trans/POSHost.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSHost.o Trans/POSHost.c

${OBJECTDIR}/Aptrans/MultiShareCOM.o: Aptrans/MultiShareCOM.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Aptrans/MultiShareCOM.o Aptrans/MultiShareCOM.c

${OBJECTDIR}/Trans/LocalAptrans.o: Trans/LocalAptrans.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/LocalAptrans.o Trans/LocalAptrans.c

${OBJECTDIR}/Trans/POSSettlement.o: Trans/POSSettlement.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSSettlement.o Trans/POSSettlement.c

${OBJECTDIR}/Erm/PosErm.o: Erm/PosErm.c 
	${MKDIR} -p ${OBJECTDIR}/Erm
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Erm/PosErm.o Erm/PosErm.c

${OBJECTDIR}/Trans/Encryption.o: Trans/Encryption.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/Encryption.o Trans/Encryption.c

${OBJECTDIR}/FileModule/myFileFunc.o: FileModule/myFileFunc.c 
	${MKDIR} -p ${OBJECTDIR}/FileModule
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/FileModule/myFileFunc.o FileModule/myFileFunc.c

${OBJECTDIR}/Comm/V5Comm.o: Comm/V5Comm.c 
	${MKDIR} -p ${OBJECTDIR}/Comm
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Comm/V5Comm.o Comm/V5Comm.c

${OBJECTDIR}/Trans/POSAuth.o: Trans/POSAuth.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSAuth.o Trans/POSAuth.c

${OBJECTDIR}/Setting/POSSetting.o: Setting/POSSetting.c 
	${MKDIR} -p ${OBJECTDIR}/Setting
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Setting/POSSetting.o Setting/POSSetting.c

${OBJECTDIR}/UI/showbmp.o: UI/showbmp.c 
	${MKDIR} -p ${OBJECTDIR}/UI
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/UI/showbmp.o UI/showbmp.c

${OBJECTDIR}/Trans/POSBalanceInq.o: Trans/POSBalanceInq.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Trans/POSBalanceInq.o Trans/POSBalanceInq.c

${OBJECTDIR}/Database/par.o: Database/par.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/Database/par.o Database/par.c

${OBJECTDIR}/DEBUG/debug.o: DEBUG/debug.c 
	${MKDIR} -p ${OBJECTDIR}/DEBUG
	$(COMPILE.c) -g -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -o ${OBJECTDIR}/DEBUG/debug.o DEBUG/debug.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} dist/V5S/BDODEBIT/10V5S_App/V5S_RBDEBIT.exe

# Subprojects
.clean-subprojects:
