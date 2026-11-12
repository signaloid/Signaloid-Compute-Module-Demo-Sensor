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

# Use these flags to enable (1) or disable (0) sensor conversion routines.
#
# The C0-microSD runs the application from its 128 KiB of RAM, which cannot
# hold all of them. So its build leaves out the four sensors set to
# $(INCLUDE_UNLESS_C0_MICROSD) below. To include one of them, set its flag to 1
# and set another sensor's flag to $(INCLUDE_UNLESS_C0_MICROSD), so that only
# the C0-microSD build leaves it out. If you change which sensors the
# C0-microSD build leaves out, update the `run-all` target in ../Makefile too.
ifeq ($(DEVICE_TYPE),SIGNALOID_C0_MICROSD)
        INCLUDE_UNLESS_C0_MICROSD   = 0
else
        INCLUDE_UNLESS_C0_MICROSD   = 1
endif

INCLUDE_FLIRAx5                     = $(INCLUDE_UNLESS_C0_MICROSD)
INCLUDE_FlussoFLS110                = $(INCLUDE_UNLESS_C0_MICROSD)
INCLUDE_NXPMPX4100A                 = 1
INCLUDE_NXPMPXx6250A                = 1
INCLUDE_SensirionSDP3x              = $(INCLUDE_UNLESS_C0_MICROSD)
INCLUDE_SensirionSDP8xx             = $(INCLUDE_UNLESS_C0_MICROSD)
INCLUDE_SensirionSFM3100            = 1
INCLUDE_SensirionSHT3xARP           = 1
INCLUDE_SensirionSHT4xI             = 1
INCLUDE_TexasInstrumentsTMAG5253    = 1
INCLUDE_TexasInstrumentsTMCS112x    = 1


ifeq ($(INCLUDE_FLIRAx5),1)
SOURCES += $(wildcard conversionRoutines/FLIRAx5/*.c)
CFLAGS += -DINCLUDE_FLIRAx5
endif

ifeq ($(INCLUDE_FlussoFLS110),1)
SOURCES += $(wildcard conversionRoutines/FlussoFLS110/*.c)
CFLAGS += -DINCLUDE_FlussoFLS110
endif

ifeq ($(INCLUDE_NXPMPX4100A),1)
SOURCES += $(wildcard conversionRoutines/NXPMPX4100A/*.c)
CFLAGS += -DINCLUDE_NXPMPX4100A
endif

ifeq ($(INCLUDE_NXPMPXx6250A),1)
SOURCES += $(wildcard conversionRoutines/NXPMPXx6250A/*.c)
CFLAGS += -DINCLUDE_NXPMPXx6250A
endif

ifeq ($(INCLUDE_SensirionSDP3x),1)
SOURCES += $(wildcard conversionRoutines/SensirionSDP3x/*.c)
CFLAGS += -DINCLUDE_SensirionSDP3x
endif

ifeq ($(INCLUDE_SensirionSDP8xx),1)
SOURCES += $(wildcard conversionRoutines/SensirionSDP8xx/*.c)
CFLAGS += -DINCLUDE_SensirionSDP8xx
endif

ifeq ($(INCLUDE_SensirionSFM3100),1)
SOURCES += $(wildcard conversionRoutines/SensirionSFM3100/*.c)
CFLAGS += -DINCLUDE_SensirionSFM3100
endif

ifeq ($(INCLUDE_SensirionSHT3xARP),1)
SOURCES += $(wildcard conversionRoutines/SensirionSHT3xARP/*.c)
CFLAGS += -DINCLUDE_SensirionSHT3xARP
endif

ifeq ($(INCLUDE_SensirionSHT4xI),1)
SOURCES += $(wildcard conversionRoutines/SensirionSHT4xI/*.c)
CFLAGS += -DINCLUDE_SensirionSHT4xI
endif

ifeq ($(INCLUDE_TexasInstrumentsTMAG5253),1)
SOURCES += $(wildcard conversionRoutines/TexasInstrumentsTMAG5253/*.c)
CFLAGS += -DINCLUDE_TexasInstrumentsTMAG5253
endif

ifeq ($(INCLUDE_TexasInstrumentsTMCS112x),1)
SOURCES += $(wildcard conversionRoutines/TexasInstrumentsTMCS112x/*.c)
CFLAGS += -DINCLUDE_TexasInstrumentsTMCS112x
endif

