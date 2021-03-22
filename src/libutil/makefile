#
MAKEFILE_FILE := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_PWD := $(dir $(MAKEFILE_FILE))

#
PROJECT_NAME = libutil
PROJECT_VERSION ?= devel
BUILD_TYPE ?= release
BUILD_PATH ?= ${MAKEFILE_PWD}/../build/
INSTALL_PATH ?= /usr/local/


#
# Common definitions
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
CCC_FLAGS += -DCWUTIL_VERSION_STR=\"${PROJECT_VERSION}\"

#
CCC_FLAGS += `pkg-config --cflags ${PROJECT_NAME}-depend`

#
LINK_FLAGS += `pkg-config --libs ${PROJECT_NAME}-depend`

#
OBJ_PATH = ${BUILD_PATH}/${PROJECT_NAME}.o.tmp/
#
OBJ_FILES = \
	${OBJ_PATH}/general.o \
	${OBJ_PATH}/heap.o \
	${OBJ_PATH}/hash.o \
	${OBJ_PATH}/endian.o \
	${OBJ_PATH}/buffer.o \
	${OBJ_PATH}/thread.o \
	${OBJ_PATH}/tree.o \
	${OBJ_PATH}/map.o \
	${OBJ_PATH}/geometry.o \
	${OBJ_PATH}/bloom.o \
	${OBJ_PATH}/crc32.o \
	${OBJ_PATH}/time.o \
	${OBJ_PATH}/clock.o \
	${OBJ_PATH}/log.o \
	${OBJ_PATH}/file.o


#
all: ${PROJECT_NAME}
#
${PROJECT_NAME}: $(BUILD_PATH)/libgood_util.so

#
$(BUILD_PATH)/libgood_util.so: $(OBJ_FILES)
	mkdir -p $(BUILD_PATH)
	rm -f $@
	$(CCC) $^ $(LINK_FLAGS) -shared -o "$@"

#
$(OBJ_PATH)/%.o: %.c
	mkdir -p $(OBJ_PATH)
	rm -f $@
	$(CCC) $(CCC_STD) $(CCC_FLAGS) -c $< -o "$@"

#
clean:
	rm -rf $(OBJ_PATH)
	rm -f $(BUILD_PATH)/libgood_util.so

#
INSTALL_PATH_INC = ${INSTALL_PATH}/include/${PROJECT_NAME}/
INSTALL_PATH_LIB = ${INSTALL_PATH}/lib/
INSTALL_PATH_BIN = ${INSTALL_PATH}/bin/

#
install:
#
	mkdir -p ${INSTALL_PATH_LIB}
	cp  $(BUILD_PATH)/libgood_util.so ${INSTALL_PATH_LIB}/
#
	mkdir -p ${INSTALL_PATH_INC}/
	cp  *.h ${INSTALL_PATH_INC}/
#
uninstall:
#
	rm -f ${INSTALL_PATH_INC}/*.h
#
	rm -f ${INSTALL_PATH_LIB}/libgood_util.so
