# Root directory variables
MAKEFILE_PATH                  := $(abspath $(firstword $(MAKEFILE_LIST)))
MAKEFILE_DIR                   := $(abspath $(dir $(MAKEFILE_PATH)))
ROOT_DIR                       := $(abspath $(MAKEFILE_DIR))


DEVICE                         ?= /usr/local/signaloid/C0microSDPlus0
# DEVICE_TYPE                    ?= SIGNALOID_C0_MICROSD
DEVICE_TYPE                    ?= SIGNALOID_C0_MICROSD_PLUS

PYTHON                         := python3
SIGNALOID_CLI                  := signaloid-cli-internal


# Set variables based on DEVICE_TYPE
ifeq ($(DEVICE_TYPE),SIGNALOID_C0_MICROSD)
	# Signaloid C0-microSD core IDs
	CORE_ID_C0_microSD_N            := cor_271d544c73a8544d9026252652342972
	CORE_ID_C0_microSD_N_plus       := cor_c1cde893b0d75bb6a8941e9caf90f2a6
	CORE_ID_C0_microSD_XS           := cor_808bbbb9932c5d29a58370a1ec9a859f
	CORE_ID_C0_microSD_XS_plus      := cor_3d8dfc5d4f305e16b867716fe6aba1e9

	# Set the core ID to use for the build
	CORE_ID=$(CORE_ID_C0_microSD_N)

	FLASH_TARGET    := flash-C0-microSD
        DEVICE_VARIANT  := C0-microSD
else ifeq ($(DEVICE_TYPE),SIGNALOID_C0_MICROSD_PLUS)
	# Signaloid C0-microSD+ core IDs
	CORE_ID_C0_microSD_plus_N       := cor_1faf6bb2d7d5522ea7fa8d0abb5f8287
	CORE_ID_C0_microSD_plus_N_plus  := cor_47178d2437f95276961d2b1311f6efb7
	CORE_ID_C0_microSD_plus_XS      := cor_fec16af93c525850a49abe6ddbe9a434
	CORE_ID_C0_microSD_plus_XS_plus := cor_28cfadb7a9535ddf9dffbdeaa41b0f20
	CORE_ID_C0_microSD_plus_S       := cor_b4bca7fa91c95e17bba1c210d2485eb1
	CORE_ID_C0_microSD_plus_S_plus  := cor_b3d7e24ecca45da7b3752304e1230f02

	# Set the core ID to use for the build
	CORE_ID=$(CORE_ID_C0_microSD_plus_N)

	FLASH_TARGET    := flash-C0-microSD-Plus
        DEVICE_VARIANT  := C0-microSD+
else
	$(error "Invalid DEVICE_TYPE specified. Please set DEVICE_TYPE to one of: SIGNALOID_C0_MICROSD, SIGNALOID_C0_MICROSD_PLUS")
endif


# Repo URL and branch to build
REPO_URL := $(shell git remote get-url origin | sed s,git@github.com:,https://github.com/,)
REPO_BRANCH := $(shell git branch --show-current)

# C0-microSD-utilities submodule directory
UTILITIES_DIR := $(ROOT_DIR)/submodules/C0-microSD-utilities

# Build directory
BUILD_DIR := signaloid-soc-application

REPO_ID_FILE := $(ROOT_DIR)/.repo_id
REPO_ID=$(shell cat $(REPO_ID_FILE))

BUILD_ID_FILE := $(ROOT_DIR)/.build_id
BUILD_ID=$(shell cat $(BUILD_ID_FILE))

BINARY_FILENAME := main.bin
BINARY_FILE := $(ROOT_DIR)/$(BUILD_DIR)/$(BINARY_FILENAME)

# Enable Global "Exit on Error" for shell commands
.SHELLFLAGS = -ec

all: download

print-%  : ; @echo $* = $($*)

# Search for the repo in Signaloid Cloud Developer Platform.
# If it doesn't exist, connect it.
connect $(REPO_ID_FILE):
	@RESPONSE=$$($(SIGNALOID_CLI) repos lookup \
		--url $(REPO_URL) \
		--branch $(REPO_BRANCH)); \
	REPO_ID=$$(echo $$RESPONSE | jq -r '.RepositoryID'); \
	if [ "$$REPO_ID" = "null" ]; then \
		RESPONSE=$$($(SIGNALOID_CLI) repos connect \
			--url $(REPO_URL) \
			--branch $(REPO_BRANCH) \
			--dir $(BUILD_DIR) \
			--core-id $(CORE_ID)); \
		REPO_ID=$$(echo $$RESPONSE | jq -r '.RepositoryID'); \
	fi; \
	echo $$REPO_ID > $(REPO_ID_FILE)

update: $(REPO_ID_FILE)
	@RESPONSE=$$($(SIGNALOID_CLI) repos update \
		--repo-id $(REPO_ID) \
		--branch $(REPO_BRANCH) \
		--dir $(BUILD_DIR) \
		--core-id $(CORE_ID));

# Create a build for the repo
build $(BUILD_ID_FILE): $(REPO_ID_FILE)
	@RESPONSE=$$($(SIGNALOID_CLI) builds create:repo --repo-id $(REPO_ID)); \
	BUILD_ID=$$(echo $$RESPONSE | jq -r '.BuildID'); \
	echo $$BUILD_ID > $(BUILD_ID_FILE); \
	$(SIGNALOID_CLI) builds watch --build-id $$BUILD_ID; \
	$(SIGNALOID_CLI) builds output --build-id $$BUILD_ID;

# Download the build binary
download $(BINARY_FILE): $(BUILD_ID_FILE)
	@cd $(BUILD_DIR) \
	&& $(SIGNALOID_CLI) builds binary \
		--build-id $(BUILD_ID) \
		--out builds \
		--filename $(BUILD_ID).tar.gz \
	&& mkdir -p builds/$(BUILD_ID) \
	&& tar -xzf builds/$(BUILD_ID).tar.gz -C builds/$(BUILD_ID) \
	&& cp builds/$(BUILD_ID)/$(BUILD_DIR)/main.bin .;

clean:
	@$(RM) $(BINARY_FILE)
	@$(RM) $(BUILD_ID_FILE)

clean-all: clean
	@$(RM) $(REPO_ID_FILE)
	@$(RM) -r $(BUILD_DIR)/builds

# Flashing targets
flash: $(FLASH_TARGET)

flash-C0-microSD: $(BINARY_FILE)
	@echo "\n- Flashing: Signaloid C0-microSD [$(DEVICE)]"
	@file_size=$$(ls -l $(BINARY_FILE) | awk '{print $$5}'); \
	if [ "$${file_size}" -gt 131072 ]; then \
		echo "Error: Binary file is too large ($$file_size bytes)."; \
		exit 1; \
	fi
	@$(PYTHON) $(UTILITIES_DIR)/C0_microSD_toolkit.py -t $(DEVICE) -b $(BINARY_FILE) -U -p 128K

flash-C0-microSD-Plus: $(BINARY_FILE) stop
	@echo "\n- Flashing: Signaloid C0-microSD+ [$(DEVICE)]"
	@file_size=$$(ls -l $(BINARY_FILE) | awk '{print $$5}'); \
	if [ "$${file_size}" -gt 327680 ]; then \
		echo "Error: Binary file is too large ($$file_size bytes)."; \
		exit 1; \
	fi
	@$(PYTHON) $(UTILITIES_DIR)/C0_SD_toolkit.py --variant=$(DEVICE_VARIANT) $(DEVICE) flash-application $(BINARY_FILE)

switch:
	@echo "\n- Switching: Signaloid C0-microSD [$(DEVICE)]"
	@$(TOOLKIT) -t $(DEVICE) -s

start:
	@$(PYTHON) $(UTILITIES_DIR)/C0_SD_toolkit.py --variant=$(DEVICE_VARIANT) $(DEVICE) config core-start

stop:
	@$(PYTHON) $(UTILITIES_DIR)/C0_SD_toolkit.py --variant=$(DEVICE_VARIANT) $(DEVICE) config core-stop

reset: stop start

log:
	@$(PYTHON) $(UTILITIES_DIR)/C0_debug_logger.py --variant=$(DEVICE_VARIANT) $(DEVICE)


VENV_DIR = $(ROOT_DIR)/.venv

$(VENV_DIR):
	@echo "\n- Creating virtual environment in $(VENV_DIR)"
	@python3 -m venv $(VENV_DIR)
	@$(VENV_DIR)/bin/pip install --upgrade pip
	@$(VENV_DIR)/bin/pip install -r $(ROOT_DIR)/python-host-application/requirements.txt

ITERATIONS ?= 1
RUN_CMD=$(VENV_DIR)/bin/python3 $(ROOT_DIR)/python-host-application/host_application.py $(DEVICE) --variant $(DEVICE_VARIANT) --benchmark --iterations "${ITERATIONS}"

run-all: $(VENV_DIR)
	@$(RUN_CMD) FLIRAx5 "30050(50)"
	@$(RUN_CMD) FlussoFLS110 "0.03(2)" "293.5(5)" "273.25(25)" "422500(2500)" "402500(2500)"
	@$(RUN_CMD) NXPMPX4100A "2.5(2)" "5.1(3)"
	@$(RUN_CMD) NXPMPXx6250A "2.5(2)" "5.1(3)"
	@$(RUN_CMD) SensirionSDP3x "1.5(2)" "3.6(3)"
	@$(RUN_CMD) SensirionSDP8xx "1.5(2)" "3.6(3)"
	@$(RUN_CMD) SensirionSFM3100 "0.75(5)"
	@$(RUN_CMD) SensirionSHT3xARP "2.5(2)" "2.5(2)" "5.1(3)"
	@$(RUN_CMD) SensirionSHT4xI "2.5(2)" "2.5(2)" "5.1(3)"
	@$(RUN_CMD) TexasInstrumentsTMAG5253 "2.7(1)" "3.3(1)"
	@$(RUN_CMD) TexasInstrumentsTMCS112x "3.3(1)" "2.5(1)"
