# Build configuration for the Signaloid SoC application.
#
# This file is included during the compilation of the Signaloid SoC application
# and configures the build process.


CONFIG_MK_DIR   := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT_DIR        := $(abspath $(CONFIG_MK_DIR)/..)
UTILITIES_DIR   := ../submodules/Signaloid-Compute-Module-Utilities


# Execution mode to build for. It sets the memory where the program will be
# executed from.
#
# Modes available:
#   - lram  : Whole application in the on-die SRAM (fastest, bounded by the
#             LRAM size). Suitable for small firmware binaries that require
#             the highest performance.
#   - psram : Whole application in external HyperRAM (larger main memory,
#             leaves the LRAM free). Suitable for larger firmware binaries
#             with minimal to no performance hit compared to lram.
#   - xip   : Code executes in place from flash, only writable data live in
#             SRAM. Suitable for very large firmware binaries, though
#             performance is bounded by the flash latency and bandwidth.
#
# Compute module support:
#   - C0-microSD  : lram (no other mode available, so this variable is ignored)
#   - C0-microSD+ : lram, xip
#   - C0-SD       : lram, psram, xip
MODE := lram


# Set variables based on DEVICE_TYPE.
#
# The DEVICE_TYPE variable is set based on the selected Core ID you are
# building for, with the Signaloid CLI.
# To learn more on the available Core IDs see:
# https://docs.signaloid.io/docs/api/guides/execution-cores/#default-cores
#
# Possible DEVICE_TYPE values are:
#   - SIGNALOID_C0_MICROSD
#   - SIGNALOID_C0_MICROSD_PLUS
#   - SIGNALOID_C0_SD


# Add all your sources here.
# Note that ordering matters. Α later module only gets linked if the
# symbols it provides are referenced in an earlier file in the objects/sources
# positional list
PROGRAM     := main
SOURCES     += main.c


# Include the Hardware Abstraction Layer library and register maps for the
# selected Signaloid compute module
ifeq ($(DEVICE_TYPE),SIGNALOID_C0_MICROSD)
        SOURCES     += $(UTILITIES_DIR)/src/c/src/C0microSD/HAL.c
else ifeq ($(DEVICE_TYPE),SIGNALOID_C0_MICROSD_PLUS)
        INC_DIRS    += $(UTILITIES_DIR)/src/c/regmaps/C0microSDPlus
        SOURCES     += $(wildcard $(UTILITIES_DIR)/src/c/regmaps/C0microSDPlus/*.c)
        SOURCES     += $(UTILITIES_DIR)/src/c/src/C0microSDPlus/HAL.c
else ifeq ($(DEVICE_TYPE),SIGNALOID_C0_SD)
        INC_DIRS    += $(UTILITIES_DIR)/src/c/regmaps/C0SD
        SOURCES     += $(wildcard $(UTILITIES_DIR)/src/c/regmaps/C0SD/*.c)
        SOURCES     += $(UTILITIES_DIR)/src/c/src/C0SD/HAL.c
else
        $(error "Invalid DEVICE_TYPE specified. Please set DEVICE_TYPE to one of: SIGNALOID_C0_MICROSD, SIGNALOID_C0_MICROSD_PLUS, SIGNALOID_C0_SD")
endif


# Include helper headers from the Signaloid Compute Module Utilities package
INC_DIRS    += $(UTILITIES_DIR)/src/c/include

# Include your application headers
INC_DIRS    += .

# Uncomment if you want to use the C0Logger utility in your application.
# SOURCES     += $(UTILITIES_DIR)/src/c/src/C0Logger.c

# Use this variable to add your own build flags. It will be appended to the
# CFLAGS and CXXFLAGS.
#
# Examples:
# BUILD_FLAGS += -DMY_CUSTOM_BUILD_FLAG=\""This is a test"\"
# BUILD_FLAGS += -DENABLE_DEBUG_LOGGING=0


# Additional sensor conversion routine sources

# Use these flags to enable or disable sensor conversion routines
INCLUDE_FLIRAx5                     = 1
INCLUDE_FlussoFLS110                = 1
INCLUDE_NXPMPX4100A                 = 1
INCLUDE_NXPMPXx6250A                = 1
INCLUDE_SensirionSDP3x              = 1
INCLUDE_SensirionSDP8xx             = 1
INCLUDE_SensirionSFM3100            = 1
INCLUDE_SensirionSHT3xARP           = 1
INCLUDE_SensirionSHT4xI             = 1
INCLUDE_TexasInstrumentsTMAG5253    = 1
INCLUDE_TexasInstrumentsTMCS112x    = 1


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

