PWD_DIR = "$(shell basename $$(pwd))"
BUILD_DIR := build
BUILD_UNIT_TEST_DIR := build-unit-test
RESOURCES_DIR := resources

RCUTILS_LIBRARY := mmc/lib/librcutils
RCUTILS_SUBMODULE := mmc/lib/librcutils/rcutils-sel4cp

REMOTE_USER_HOST = "patrick@vm_comp4961_ubuntu2204"
REMOTE_DEST_DIR = "~/remote/$(shell hostname -s)/"

.PHONY: directories
directories:
	mkdir -p $(BUILD_UNIT_TEST_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_UNIT_TEST_DIR)

# =================================
# Resources
# =================================

# Command used to decompile the Raspberry Pi 3B+'s device tree blob.
.PHONY: resources-decompile-dtb-rpi3bp
resources-decompile-dtb-rpi3bp:
	dtc \
		-I dtb \
		-O dts \
		$(RESOURCES_DIR)/bcm2710-rpi-3-b-plus.dtb \
		-o $(RESOURCES_DIR)/bcm2710-rpi-3-b-plus.dts

# =================================
# Build
# =================================

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: build-rcutils
build-rcutils:
	# Remotely run the build command in rcutils submodule.
	$(MAKE) -C $(RCUTILS_SUBMODULE) remote MAKE_CMD="build"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rcutils-sel4cp/build/librcutils.a \
		$(RCUTILS_LIBRARY)/librcutils.a
	cp -r $(RCUTILS_SUBMODULE)/include/rcutils mmc/src/lib_rcutils/rcutils
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rcutils-sel4cp/build/include/rcutils/* \
		mmc/src/lib_rcutils/rcutils/

# =================================
# Testing
# =================================

.PHONY: test-build
test-build: directories
	cmake -S . -B ./$(BUILD_UNIT_TEST_DIR) \
		-DMMC_RPI3B_UNIT_TEST=1
	cmake --build ./$(BUILD_UNIT_TEST_DIR) --parallel "$(nproc)"

.PHONY: test-unit
test-unit: test-build
	cmake --build ./$(BUILD_UNIT_TEST_DIR) --parallel "$(nproc)" --target test

.PHONY: test-unit-valgrind
test-unit-valgrind: test-build
	cd ./$(BUILD_UNIT_TEST_DIR) && ctest -T memcheck


