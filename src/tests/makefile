#
MAKEFILE_FILE := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_PWD := $(dir $(MAKEFILE_FILE))

#
PROJECT_NAME = tests
PROJECT_VERSION ?= devel
BUILD_TYPE ?= release
BUILD_PATH ?= ${MAKEFILE_PWD}/../build/

#
INSTALL_PATH ?= /usr/local/
INSTALL_PATH_INC ?= ${INSTALL_PATH}/include/
INSTALL_PATH_LIB ?= ${INSTALL_PATH}/lib/
INSTALL_PATH_BIN ?= ${INSTALL_PATH}/bin/

#
# Common definitions
CCC = gcc

# 
CCC_STD = -std=c11

#-Werror -Wall -Wextra
CCC_FLAGS = -fPIC -Wno-unused-result -MMD -MP -MF "$@.mdep"

#
LINK_FLAGS = -Wl,--as-needed

#
ifeq ($(BUILD_TYPE),debug)
CCC_FLAGS += -g
else 
CCC_FLAGS += -s -O2
endif

#
CCC_FLAGS += -D_GNU_SOURCE

#
CCC_FLAGS += `pkg-config --cflags libcwutil`

#
LINK_FLAGS += `pkg-config --libs libcwutil`

#
OBJ_PATH = ${BUILD_PATH}/${PROJECT_NAME}.o.tmp/
#
OBJ_FILES = \
	${OBJ_PATH}/thread_test.o

#
all: ${PROJECT_NAME}
#
${PROJECT_NAME}: $(BUILD_PATH)/thread_test.exe

#
$(BUILD_PATH)/thread_test.exe: $(OBJ_FILES)
	mkdir -p $(BUILD_PATH)
	rm -f $@
	$(CCC) $^ $(LINK_FLAGS) -o "$@"

#
$(OBJ_PATH)/%.o: %.c
	mkdir -p $(OBJ_PATH)
	rm -f $@
	$(CCC) $(CCC_STD) $(CCC_FLAGS) -c $< -o "$@"

#
clean:
	rm -rf $(OBJ_PATH)
	rm -f $(BUILD_PATH)/thread_test.exe


