#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=adc_sampler.c classifier.c features.c lcd.c newmain.c uart.c "../final project/final project/firmware2/word_templates.c" "../final project/final project/firmware2/sram.c" audio_test_with_sram_main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/adc_sampler.o ${OBJECTDIR}/classifier.o ${OBJECTDIR}/features.o ${OBJECTDIR}/lcd.o ${OBJECTDIR}/newmain.o ${OBJECTDIR}/uart.o ${OBJECTDIR}/_ext/188906888/word_templates.o ${OBJECTDIR}/_ext/188906888/sram.o ${OBJECTDIR}/audio_test_with_sram_main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/adc_sampler.o.d ${OBJECTDIR}/classifier.o.d ${OBJECTDIR}/features.o.d ${OBJECTDIR}/lcd.o.d ${OBJECTDIR}/newmain.o.d ${OBJECTDIR}/uart.o.d ${OBJECTDIR}/_ext/188906888/word_templates.o.d ${OBJECTDIR}/_ext/188906888/sram.o.d ${OBJECTDIR}/audio_test_with_sram_main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/adc_sampler.o ${OBJECTDIR}/classifier.o ${OBJECTDIR}/features.o ${OBJECTDIR}/lcd.o ${OBJECTDIR}/newmain.o ${OBJECTDIR}/uart.o ${OBJECTDIR}/_ext/188906888/word_templates.o ${OBJECTDIR}/_ext/188906888/sram.o ${OBJECTDIR}/audio_test_with_sram_main.o

# Source Files
SOURCEFILES=adc_sampler.c classifier.c features.c lcd.c newmain.c uart.c ../final project/final project/firmware2/word_templates.c ../final project/final project/firmware2/sram.c audio_test_with_sram_main.c

# Pack Options 
PACK_COMPILER_OPTIONS=-I "${DFP_DIR}/include"
PACK_COMMON_OPTIONS=-B "${DFP_DIR}/gcc/dev/atmega32a"



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=ATmega32A
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/adc_sampler.o: adc_sampler.c  .generated_files/flags/default/b3edb90415d42710aa1751d75e462789590c18d0 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/adc_sampler.o.d 
	@${RM} ${OBJECTDIR}/adc_sampler.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/adc_sampler.o.d" -MT "${OBJECTDIR}/adc_sampler.o.d" -MT ${OBJECTDIR}/adc_sampler.o  -o ${OBJECTDIR}/adc_sampler.o adc_sampler.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/classifier.o: classifier.c  .generated_files/flags/default/f9929f15424ac7fdbe1c339a8a46eb80f7ce7ab1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/classifier.o.d 
	@${RM} ${OBJECTDIR}/classifier.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/classifier.o.d" -MT "${OBJECTDIR}/classifier.o.d" -MT ${OBJECTDIR}/classifier.o  -o ${OBJECTDIR}/classifier.o classifier.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/features.o: features.c  .generated_files/flags/default/e695b0432cac72e06cfdc85562dac8c74717c31b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/features.o.d 
	@${RM} ${OBJECTDIR}/features.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/features.o.d" -MT "${OBJECTDIR}/features.o.d" -MT ${OBJECTDIR}/features.o  -o ${OBJECTDIR}/features.o features.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/lcd.o: lcd.c  .generated_files/flags/default/eb1f5d6c5a424895fb156966b6e09eb5230f9144 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/lcd.o.d 
	@${RM} ${OBJECTDIR}/lcd.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/lcd.o.d" -MT "${OBJECTDIR}/lcd.o.d" -MT ${OBJECTDIR}/lcd.o  -o ${OBJECTDIR}/lcd.o lcd.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/newmain.o: newmain.c  .generated_files/flags/default/6005727fd6b5734205edcaa73fb5b1d0f5a9913 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/newmain.o.d 
	@${RM} ${OBJECTDIR}/newmain.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/newmain.o.d" -MT "${OBJECTDIR}/newmain.o.d" -MT ${OBJECTDIR}/newmain.o  -o ${OBJECTDIR}/newmain.o newmain.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/uart.o: uart.c  .generated_files/flags/default/6f23e92ebfc3257d78c8a3086015adbf634c29 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/uart.o.d" -MT "${OBJECTDIR}/uart.o.d" -MT ${OBJECTDIR}/uart.o  -o ${OBJECTDIR}/uart.o uart.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/188906888/word_templates.o: ../final\ project/final\ project/firmware2/word_templates.c  .generated_files/flags/default/20fb56a81016774300858768c9c9a248e98494a2 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/188906888" 
	@${RM} ${OBJECTDIR}/_ext/188906888/word_templates.o.d 
	@${RM} ${OBJECTDIR}/_ext/188906888/word_templates.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/_ext/188906888/word_templates.o.d" -MT "${OBJECTDIR}/_ext/188906888/word_templates.o.d" -MT ${OBJECTDIR}/_ext/188906888/word_templates.o  -o ${OBJECTDIR}/_ext/188906888/word_templates.o "../final project/final project/firmware2/word_templates.c"  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/188906888/sram.o: ../final\ project/final\ project/firmware2/sram.c  .generated_files/flags/default/f2a2136e493f518fe81d7ca9314b548d2b3d348b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/188906888" 
	@${RM} ${OBJECTDIR}/_ext/188906888/sram.o.d 
	@${RM} ${OBJECTDIR}/_ext/188906888/sram.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/_ext/188906888/sram.o.d" -MT "${OBJECTDIR}/_ext/188906888/sram.o.d" -MT ${OBJECTDIR}/_ext/188906888/sram.o  -o ${OBJECTDIR}/_ext/188906888/sram.o "../final project/final project/firmware2/sram.c"  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/audio_test_with_sram_main.o: audio_test_with_sram_main.c  .generated_files/flags/default/9c65952e14f877f02828ec2946a1d29b62eac5ff .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/audio_test_with_sram_main.o.d 
	@${RM} ${OBJECTDIR}/audio_test_with_sram_main.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/audio_test_with_sram_main.o.d" -MT "${OBJECTDIR}/audio_test_with_sram_main.o.d" -MT ${OBJECTDIR}/audio_test_with_sram_main.o  -o ${OBJECTDIR}/audio_test_with_sram_main.o audio_test_with_sram_main.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/adc_sampler.o: adc_sampler.c  .generated_files/flags/default/2b510bfb10e9c882b74e0c26507fc08ba7be0571 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/adc_sampler.o.d 
	@${RM} ${OBJECTDIR}/adc_sampler.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/adc_sampler.o.d" -MT "${OBJECTDIR}/adc_sampler.o.d" -MT ${OBJECTDIR}/adc_sampler.o  -o ${OBJECTDIR}/adc_sampler.o adc_sampler.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/classifier.o: classifier.c  .generated_files/flags/default/f9f97ea427ef9afc0636fa457e8876586a9891c0 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/classifier.o.d 
	@${RM} ${OBJECTDIR}/classifier.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/classifier.o.d" -MT "${OBJECTDIR}/classifier.o.d" -MT ${OBJECTDIR}/classifier.o  -o ${OBJECTDIR}/classifier.o classifier.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/features.o: features.c  .generated_files/flags/default/8010dd71e4d3eb7134d21576dc1cb075a13eeb09 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/features.o.d 
	@${RM} ${OBJECTDIR}/features.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/features.o.d" -MT "${OBJECTDIR}/features.o.d" -MT ${OBJECTDIR}/features.o  -o ${OBJECTDIR}/features.o features.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/lcd.o: lcd.c  .generated_files/flags/default/abe330dc55e4c421ce83876b196b6f986ed5ca81 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/lcd.o.d 
	@${RM} ${OBJECTDIR}/lcd.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/lcd.o.d" -MT "${OBJECTDIR}/lcd.o.d" -MT ${OBJECTDIR}/lcd.o  -o ${OBJECTDIR}/lcd.o lcd.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/newmain.o: newmain.c  .generated_files/flags/default/77609245921d4225fac2723269cfcf5e47bc073 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/newmain.o.d 
	@${RM} ${OBJECTDIR}/newmain.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/newmain.o.d" -MT "${OBJECTDIR}/newmain.o.d" -MT ${OBJECTDIR}/newmain.o  -o ${OBJECTDIR}/newmain.o newmain.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/uart.o: uart.c  .generated_files/flags/default/a11398571509f67a43483d1278f4d4a6a2599e4a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart.o.d 
	@${RM} ${OBJECTDIR}/uart.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/uart.o.d" -MT "${OBJECTDIR}/uart.o.d" -MT ${OBJECTDIR}/uart.o  -o ${OBJECTDIR}/uart.o uart.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/188906888/word_templates.o: ../final\ project/final\ project/firmware2/word_templates.c  .generated_files/flags/default/41a3d259a00a48a4cb34660c2fb6a7bcec32eafb .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/188906888" 
	@${RM} ${OBJECTDIR}/_ext/188906888/word_templates.o.d 
	@${RM} ${OBJECTDIR}/_ext/188906888/word_templates.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/_ext/188906888/word_templates.o.d" -MT "${OBJECTDIR}/_ext/188906888/word_templates.o.d" -MT ${OBJECTDIR}/_ext/188906888/word_templates.o  -o ${OBJECTDIR}/_ext/188906888/word_templates.o "../final project/final project/firmware2/word_templates.c"  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/188906888/sram.o: ../final\ project/final\ project/firmware2/sram.c  .generated_files/flags/default/d913fb22fdc8d711936a21f8c1df7bffa93ef7e .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/188906888" 
	@${RM} ${OBJECTDIR}/_ext/188906888/sram.o.d 
	@${RM} ${OBJECTDIR}/_ext/188906888/sram.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/_ext/188906888/sram.o.d" -MT "${OBJECTDIR}/_ext/188906888/sram.o.d" -MT ${OBJECTDIR}/_ext/188906888/sram.o  -o ${OBJECTDIR}/_ext/188906888/sram.o "../final project/final project/firmware2/sram.c"  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/audio_test_with_sram_main.o: audio_test_with_sram_main.c  .generated_files/flags/default/973190d010720c9b6b0406abe8175da73945198e .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/audio_test_with_sram_main.o.d 
	@${RM} ${OBJECTDIR}/audio_test_with_sram_main.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega32a ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/audio_test_with_sram_main.o.d" -MT "${OBJECTDIR}/audio_test_with_sram_main.o.d" -MT ${OBJECTDIR}/audio_test_with_sram_main.o  -o ${OBJECTDIR}/audio_test_with_sram_main.o audio_test_with_sram_main.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=atmega32a ${PACK_COMMON_OPTIONS}   -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\_5-11_FINALPROJECT.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	
	
	
	
	
	
else
${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=atmega32a ${PACK_COMMON_OPTIONS}  -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\_5-11_FINALPROJECT.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	${MP_CC_DIR}\\avr-objcopy -O ihex "${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/_5-11_FINALPROJECT.X.${IMAGE_TYPE}.hex"
	
	
	
	
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
