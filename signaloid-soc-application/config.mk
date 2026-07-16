PROGRAM := main
SOURCES	:= main.c

# DEVICE_TYPE is set via the selected Core ID.
ifeq ($(DEVICE_TYPE),SIGNALOID_C0_MICROSD)
        SOURCES     += ../submodules/C0-microSD-utilities/src/c/src/C0microSD/HAL.c
else ifeq ($(DEVICE_TYPE),SIGNALOID_C0_MICROSD_PLUS)
        INC_DIRS    += ../submodules/C0-microSD-utilities/src/c/regmaps/C0microSDPlus
        SOURCES     += $(wildcard ../submodules/C0-microSD-utilities/src/c/regmaps/C0microSDPlus/*.c)
        SOURCES     += ../submodules/C0-microSD-utilities/src/c/src/C0microSDPlus/HAL.c
else ifeq ($(DEVICE_TYPE),SIGNALOID_C0_SD)
        INC_DIRS    += ../submodules/C0-microSD-utilities/src/c/regmaps/C0SD
        SOURCES     += $(wildcard ../submodules/C0-microSD-utilities/src/c/regmaps/C0SD/*.c)
        SOURCES     += ../submodules/C0-microSD-utilities/src/c/src/C0SD/HAL.c
else
        $(error Unknown device: $(DEVICE_TYPE))
endif

SOURCES     += ../submodules/C0-microSD-utilities/src/c/src/C0Logger.c
INC_DIRS    += ../submodules/C0-microSD-utilities/src/c/include

# Use this variable to add your own build flags. Examples below.
# BUILD_FLAGS += -DMY_CUSTOM_BUILD_FLAG=\""This is a test"\"
# BUILD_FLAGS += -DENABLE_DEBUG_LOGGING=0


INCLUDE_FLIRAx5 = 1
INCLUDE_FlussoFLS110 = 1
INCLUDE_NXPMPX4100A = 1
INCLUDE_NXPMPXx6250A = 1
INCLUDE_SensirionSDP3x = 1
INCLUDE_SensirionSDP8xx = 1
INCLUDE_SensirionSFM3100 = 1
INCLUDE_SensirionSHT3xARP = 1
INCLUDE_SensirionSHT4xI = 1
INCLUDE_TexasInstrumentsTMAG5253 = 1
INCLUDE_TexasInstrumentsTMCS112x = 1


ifeq ($(INCLUDE_FLIRAx5),1)
SOURCES += conversionRoutines/FLIRAx5/kernel.c
CFLAGS += -DINCLUDE_FLIRAx5
endif

ifeq ($(INCLUDE_FlussoFLS110),1)
SOURCES += conversionRoutines/FlussoFLS110/kernel.c
CFLAGS += -DINCLUDE_FlussoFLS110
endif

ifeq ($(INCLUDE_NXPMPX4100A),1)
SOURCES += conversionRoutines/NXPMPX4100A/kernel.c
CFLAGS += -DINCLUDE_NXPMPX4100A
endif

ifeq ($(INCLUDE_NXPMPXx6250A),1)
SOURCES += conversionRoutines/NXPMPXx6250A/kernel.c
CFLAGS += -DINCLUDE_NXPMPXx6250A
endif

ifeq ($(INCLUDE_SensirionSDP3x),1)
SOURCES += conversionRoutines/SensirionSDP3x/kernel.c
CFLAGS += -DINCLUDE_SensirionSDP3x
endif

ifeq ($(INCLUDE_SensirionSDP8xx),1)
SOURCES += conversionRoutines/SensirionSDP8xx/kernel.c
CFLAGS += -DINCLUDE_SensirionSDP8xx
endif

ifeq ($(INCLUDE_SensirionSFM3100),1)
SOURCES += conversionRoutines/SensirionSFM3100/kernel.c
CFLAGS += -DINCLUDE_SensirionSFM3100
endif

ifeq ($(INCLUDE_SensirionSHT3xARP),1)
SOURCES += conversionRoutines/SensirionSHT3xARP/kernel.c
CFLAGS += -DINCLUDE_SensirionSHT3xARP
endif

ifeq ($(INCLUDE_SensirionSHT4xI),1)
SOURCES += conversionRoutines/SensirionSHT4xI/kernel.c
CFLAGS += -DINCLUDE_SensirionSHT4xI
endif

ifeq ($(INCLUDE_TexasInstrumentsTMAG5253),1)
SOURCES += conversionRoutines/TexasInstrumentsTMAG5253/kernel.c
CFLAGS += -DINCLUDE_TexasInstrumentsTMAG5253
endif

ifeq ($(INCLUDE_TexasInstrumentsTMCS112x),1)
SOURCES += conversionRoutines/TexasInstrumentsTMCS112x/kernel.c
CFLAGS += -DINCLUDE_TexasInstrumentsTMCS112x
endif

