PWD_DIR = "$(shell basename $$(pwd))"
BUILD_DIR := build
SRC_DIR := mmc/src
LIB_DIR := mmc/lib
BUILD_UNIT_TEST_DIR := build-unit-test
RESOURCES_DIR := resources

RCUTILS_LIBRARY := mmc/lib/librcutils
RCUTILS_SUBMODULE := $(RCUTILS_LIBRARY)/rcutils-sel4cp

RCL_LIBRARY := $(LIB_DIR)/librcl
RCL_SUBMODULE := $(RCL_LIBRARY)/rcl-sel4cp

RMW_LIBRARY := $(LIB_DIR)/librmw
RMW_SUBMODULE := $(RMW_LIBRARY)/rmw-sel4cp

MICROCDR_LIBRARY := $(LIB_DIR)/libmicrocdr
MICROCDR_SUBMODULE := $(MICROCDR_LIBRARY)/Micro-CDR-sel4cp

MICROXRCEDDS_CLIENT_LIBRARY := $(LIB_DIR)/libmicroxrcedds_client
MICROXRCEDDS_CLIENT_SUBMODULE := $(MICROXRCEDDS_CLIENT_LIBRARY)/Micro-XRCE-DDS-Client-sel4cp

ROSIDL_TYPESUPPORT_MICROXRCEDDS_LIBRARY := $(LIB_DIR)/librosidl_typesupport_microxrcedds_c
ROSIDL_TYPESUPPORT_MICROXRCEDDS_SUBMODULE := $(ROSIDL_TYPESUPPORT_MICROXRCEDDS_LIBRARY)/rosidl_typesupport_microxrcedds-sel4cp
ROSIDL_TYPESUPPORT_MICROXRCEDDS_HEADERS := $(SRC_DIR)/lib_rosidl_typesupport_microxrcedds_c/

ROSIDL_RUNTIME_C_LIBRARY := $(LIB_DIR)/librosidl_runtime_c
ROSIDL_RUNTIME_C_SUBMODULE := $(ROSIDL_RUNTIME_C_LIBRARY)/rosidl-sel4cp
ROSIDL_RUNTIME_C_HEADERS := $(SRC_DIR)/lib_rosidl_runtime_c/

ROSIDL_TYPESUPPORT_INTERFACE_HEADERS := $(SRC_DIR)/lib_rosidl_typesupport_interface/

RMW_MICROXRCEDDS_LIBRARY := $(LIB_DIR)/librmw_microxrcedds
RMW_MICROXRCEDDS_SUBMODULE := $(RMW_MICROXRCEDDS_LIBRARY)/rmw_microxrcedds-sel4cp
RMW_MICROXRCEDDS_HEADERS := $(SRC_DIR)/lib_rmw_microxrcedds/

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
.PHONY: build
build: \
	build-rcutils \
	build-rcl \
	build-rmw \
	build-microcdr \
	build-microxrcedds-client \
	build-rosidl-typesupport-microxrcedds-c \
	build-rosidl-runtime-c \
	build-rosidl-typesupport-interface \
	build-rmw-microxrcedds

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: clean-rcutils
clean-rcutils:
	# Remotely run the build command in rcutils submodule.
	$(MAKE) -C $(RCUTILS_SUBMODULE) remote MAKE_CMD="clean"

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: build-rcutils
build-rcutils: clean-rcutils
	# Copy ff.h and ffconf.h from `lib_fatfs` to the submodule to make sure
	# FatFs header files stay in sync.
	cp $(SRC_DIR)/lib_fatfs/ff.h $(RCUTILS_SUBMODULE)/include/fatfs
	cp $(SRC_DIR)/lib_fatfs/ffconf.h $(RCUTILS_SUBMODULE)/include/fatfs
	# Remotely run the build command in rcutils submodule.
	$(MAKE) -C $(RCUTILS_SUBMODULE) remote MAKE_CMD="build"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rcutils-sel4cp/build/librcutils.a \
		$(RCUTILS_LIBRARY)/librcutils.a
	# Copy the files in the submodule's include directory into our include directory.
	mkdir -p $(SRC_DIR)/lib_rcutils/
	cp -r $(RCUTILS_SUBMODULE)/include/rcutils $(SRC_DIR)/lib_rcutils/
	# Some header files are built in the remote build directory so we copy them
	# over.
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rcutils-sel4cp/build/include/rcutils/* \
		$(SRC_DIR)/lib_rcutils/rcutils/

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: clean-rcl
clean-rcl:
	# Remotely run the clean command in submodule.
	$(MAKE) -C $(RCL_SUBMODULE) remote MAKE_CMD="clean"

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: build-rcl
build-rcl: clean-rcl
	# Remotely run the build command in rcl submodule.
	$(MAKE) -C $(RCL_SUBMODULE) remote MAKE_CMD="build"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rcl-sel4cp/rcl/build/librcl.a \
		$(RCL_LIBRARY)/librcl.a
	mkdir -p $(SRC_DIR)/lib_rcl/rcl
	cp -r $(RCL_SUBMODULE)/rcl/include/rcl $(SRC_DIR)/lib_rcl/

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: clean-rmw
clean-rmw:
	# Remotely run the build command in rmw submodule.
	$(MAKE) -C $(RMW_SUBMODULE) remote MAKE_CMD="clean-rmw"

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: build-rmw
build-rmw: clean-rmw
	# Remotely run the build command in rmw submodule.
	$(MAKE) -C $(RMW_SUBMODULE) remote MAKE_CMD="build-rmw"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rmw-sel4cp/rmw/build/librmw.a \
		$(RMW_LIBRARY)
	mkdir -p $(SRC_DIR)/lib_rmw/rmw
	cp -r $(RMW_SUBMODULE)/rmw/include/rmw $(SRC_DIR)/lib_rmw/

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: clean-microcdr
clean-microcdr:
	# Remotely run the build command in submodule.
	$(MAKE) -C $(MICROCDR_SUBMODULE) remote MAKE_CMD="clean"

.PHONY: build-microcdr
build-microcdr: clean-microcdr
	# Remotely run the build command in submodule.
	$(MAKE) -C $(MICROCDR_SUBMODULE) remote MAKE_CMD="build"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/Micro-CDR-sel4cp/build/libmicrocdr.a \
		$(MICROCDR_LIBRARY)
	mkdir -p $(SRC_DIR)/lib_microcdr/ucdr
	cp -r $(MICROCDR_SUBMODULE)/include/ucdr $(SRC_DIR)/lib_microcdr/
	# Copy over header files that are built in the remote build directory.
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/Micro-CDR-sel4cp/build/include/ucdr/config.h \
		$(SRC_DIR)/lib_microcdr/ucdr/config.h

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: clean-microxrcedds-client
clean-microxrcedds-client:
	# Remotely run the build command in submodule.
	$(MAKE) -C $(MICROXRCEDDS_CLIENT_SUBMODULE) remote MAKE_CMD="clean"

.PHONY: build-microxrcedds-client
build-microxrcedds-client: clean-microxrcedds-client
	# Remotely run the build command in submodule.
	$(MAKE) -C $(MICROXRCEDDS_CLIENT_SUBMODULE) remote MAKE_CMD="build"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/Micro-XRCE-DDS-Client-sel4cp/build/libmicroxrcedds_client.a \
		$(MICROXRCEDDS_CLIENT_LIBRARY)
	mkdir -p $(SRC_DIR)/lib_microxrcedds_client/uxr
	cp -r $(MICROXRCEDDS_CLIENT_SUBMODULE)/include/uxr $(SRC_DIR)/lib_microxrcedds_client/
	# Copy over header files that are built in the remote build directory.
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/Micro-XRCE-DDS-Client-sel4cp/build/include/uxr/client/config.h \
		$(SRC_DIR)/lib_microxrcedds_client/uxr/client/config.h

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: clean-rosidl-typesupport-microxrcedds-c
clean-rosidl-typesupport-microxrcedds-c:
	# Remotely run the build command in submodule.
	$(MAKE) -C $(ROSIDL_TYPESUPPORT_MICROXRCEDDS_SUBMODULE) remote MAKE_CMD="clean"

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: build-rosidl-typesupport-microxrcedds-c
build-rosidl-typesupport-microxrcedds-c: clean-rosidl-typesupport-microxrcedds-c
	# Remotely run the build command in submodule.
	$(MAKE) -C $(ROSIDL_TYPESUPPORT_MICROXRCEDDS_SUBMODULE) remote MAKE_CMD="build"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rosidl_typesupport_microxrcedds-sel4cp/rosidl_typesupport_microxrcedds_c/build/librosidl_typesupport_microxrcedds_c.a \
		$(ROSIDL_TYPESUPPORT_MICROXRCEDDS_LIBRARY)
	mkdir -p $(ROSIDL_TYPESUPPORT_MICROXRCEDDS_HEADERS)
	cp -r \
		$(ROSIDL_TYPESUPPORT_MICROXRCEDDS_SUBMODULE)/rosidl_typesupport_microxrcedds_c/include/rosidl_typesupport_microxrcedds_c \
		$(ROSIDL_TYPESUPPORT_MICROXRCEDDS_HEADERS)

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: clean-rosidl-runtime-c
clean-rosidl-runtime-c:
	# Remotely run the build command in submodule.
	$(MAKE) -C $(ROSIDL_RUNTIME_C_SUBMODULE) remote MAKE_CMD="clean"

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: build-rosidl-runtime-c
build-rosidl-runtime-c: clean-rosidl-runtime-c
	# Remotely run the build command in submodule.
	$(MAKE) -C $(ROSIDL_RUNTIME_C_SUBMODULE) remote MAKE_CMD="build"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rosidl-sel4cp/rosidl_runtime_c/build/librosidl_runtime_c.a \
		$(ROSIDL_RUNTIME_C_LIBRARY)
	mkdir -p $(ROSIDL_RUNTIME_C_HEADERS)
	cp -r \
		$(ROSIDL_RUNTIME_C_SUBMODULE)/rosidl_runtime_c/include/rosidl_runtime_c \
		$(ROSIDL_RUNTIME_C_HEADERS)

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: build-rosidl-typesupport-interface
build-rosidl-typesupport-interface:
	mkdir -p $(ROSIDL_TYPESUPPORT_INTERFACE_HEADERS)
	cp -r \
		$(ROSIDL_RUNTIME_C_SUBMODULE)/rosidl_typesupport_interface/include/rosidl_typesupport_interface \
		$(ROSIDL_TYPESUPPORT_INTERFACE_HEADERS)

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: clean-rmw-microxrcedds
clean-rmw-microxrcedds:
	# Remotely run the build command in submodule.
	$(MAKE) -C $(RMW_MICROXRCEDDS_SUBMODULE) remote MAKE_CMD="clean"

# Do NOT run this command remotely because this command runs a remote command.
.PHONY: build-rmw-microxrcedds
build-rmw-microxrcedds: clean-rmw-microxrcedds
	# Remotely run the build command in submodule.
	$(MAKE) -C $(RMW_MICROXRCEDDS_SUBMODULE) remote MAKE_CMD="build"
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rmw_microxrcedds-sel4cp/rmw_microxrcedds_c/build/librmw_microxrcedds.a \
		$(RMW_MICROXRCEDDS_LIBRARY)
	mkdir -p $(RMW_MICROXRCEDDS_HEADERS)
	cp -r \
		$(RMW_MICROXRCEDDS_SUBMODULE)/rmw_microxrcedds_c/include/* \
		$(RMW_MICROXRCEDDS_HEADERS)
	# Copy over header files that are built in the remote build directory.
	scp \
		$(REMOTE_USER_HOST):$(REMOTE_DEST_DIR)/rmw_microxrcedds-sel4cp/rmw_microxrcedds_c/build/include/rmw_microxrcedds_c/config.h \
		$(RMW_MICROXRCEDDS_HEADERS)/rmw_microxrcedds_c/config.h

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


