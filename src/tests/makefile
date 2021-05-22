#
# This file is part of GOOD.
#
# MIT License
#
#


#
MAKE_CONF ?= $(abspath $(CURDIR)/../build/makefile.conf)

#
PROJECT_NAME = tests

#加载配置项。
include ${MAKE_CONF}

#
ifeq (${BUILD_PATH},)
BUILD_PATH = $(abspath $(CURDIR)/../build/)
else ifeq (${BUILD_PATH},"")
BUILD_PATH = $(abspath $(CURDIR)/../build/)
endif

#
#Compiler
CCC = gcc

# 
CCC_STD = -std=c11

#-Werror -Wall -Wextra
CCC_FLAGS = -fPIC -Wno-unused-result -MMD -MP -MF "$@.mdep"

#
LINK_FLAGS = -Wl,--as-needed -Wl,-rpath="./"

#
ifeq ($(BUILD_TYPE),debug)
CCC_FLAGS += -g
else 
CCC_FLAGS += -s -O2
endif

#
CCC_FLAGS += -I$(abspath $(CURDIR)/../) ${DEPEND_FLAGS}

#
LINK_FLAGS += -lgoodutil -L${BUILD_PATH} -Wl,-rpath-link=${BUILD_PATH} ${DEPEND_LIBS}

#
OBJ_PATH = ${BUILD_PATH}/${PROJECT_NAME}.o.tmp/

#
all: ${PROJECT_NAME}

#
${PROJECT_NAME}: \
	$(BUILD_PATH)/thread_test.exe \
	$(BUILD_PATH)/buffer_test.exe \
	$(BUILD_PATH)/tree_test.exe \
	$(BUILD_PATH)/map_test.exe \
	$(BUILD_PATH)/geometry_test.exe \
	$(BUILD_PATH)/log_test.exe \
	$(BUILD_PATH)/bloom_test.exe \
	$(BUILD_PATH)/file_test.exe \
	$(BUILD_PATH)/process_test.exe \
	$(BUILD_PATH)/popen_test.exe \
	$(BUILD_PATH)/socket_test.exe \
	$(BUILD_PATH)/option_test.exe \
	$(BUILD_PATH)/scsi_test.exe

#
$(BUILD_PATH)/%.exe: ${OBJ_PATH}/%.o
	mkdir -p $(BUILD_PATH)
	rm -f $@
	$(CCC) -o "$@" $^ $(LINK_FLAGS)

#
$(OBJ_PATH)/%.o: %.c
	mkdir -p $(OBJ_PATH)
	rm -f $@
	$(CCC) $(CCC_STD) $(CCC_FLAGS) -c $< -o "$@"

#
clean:
	rm -rf $(OBJ_PATH)
	rm -f $(BUILD_PATH)/thread_test.exe
	rm -f $(BUILD_PATH)/buffer_test.exe
	rm -f $(BUILD_PATH)/tree_test.exe
	rm -f $(BUILD_PATH)/map_test.exe
	rm -f $(BUILD_PATH)/geometry_test.exe
	rm -f $(BUILD_PATH)/log_test.exe
	rm -f $(BUILD_PATH)/bloom_test.exe
	rm -f $(BUILD_PATH)/file_test.exe
	rm -f $(BUILD_PATH)/process_test.exe
	rm -f $(BUILD_PATH)/popen_test.exe
	rm -f $(BUILD_PATH)/socket_test.exe
	rm -f $(BUILD_PATH)/option_test.exe
	rm -f $(BUILD_PATH)/scsi_test.exe


