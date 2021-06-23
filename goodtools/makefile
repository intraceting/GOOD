#
# This file is part of GOOD.
#
# MIT License
#
#


#
MAKE_CONF ?= $(abspath $(CURDIR)/../build/makefile.conf)

#
PROJECT_NAME = goodtools

#加载配置项。
include ${MAKE_CONF}

#
ifeq (${VERSION_MAJOR},)
VERSION_MAJOR = 1
else ifeq (${VERSION_MAJOR},"")
VERSION_MAJOR = 1
endif

#
ifeq (${VERSION_MINOR},)
VERSION_MINOR = 0
else ifeq (${VERSION_MINOR},"")
VERSION_MINOR = 0
endif

#
ifeq (${BUILD_PATH},)
BUILD_PATH = $(abspath $(CURDIR)/../build/)
else ifeq (${BUILD_PATH},"")
BUILD_PATH = $(abspath $(CURDIR)/../build/)
endif

#
ifeq (${INSTALL_PREFIX},)
INSTALL_PREFIX = /usr/local/good/
else ifeq (${INSTALL_PREFIX},"")
INSTALL_PREFIX = /usr/local/good/
endif

#
ifeq (${ROOT_PATH},)
ROOT_PATH = /
else ifeq (${ROOT_PATH},"")
ROOT_PATH = /
endif

#
#Compiler
CCC = gcc

# 
CCC_STD = -std=c11

#-Werror -Wall -Wextra
CCC_FLAGS += -fPIC -Wno-unused-result

#
LINK_FLAGS += -Wl,--as-needed -Wl,-rpath="./" -Wl,-rpath="${INSTALL_PREFIX}/lib/"

#
CCC_FLAGS += -DBUILD_VERSION_MAJOR=${VERSION_MAJOR} -DBUILD_VERSION_MINOR=${VERSION_MINOR} -DBUILD_VERSION_DATETIME=\"${VERSION_DATETIME}\"

#
ifeq ($(BUILD_TYPE),debug)
CCC_FLAGS += -g
else 
CCC_FLAGS += -s -O2
endif

#可能在交叉编译环中。
ifneq ($(TARGET_PLATFORM),$(HOST_PLATFORM))
    CCC = $(TARGET_PLATFORM)-linux-gnu-gcc
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
OBJ_FILES = \
	${OBJ_PATH}/mtx.o \
	${OBJ_PATH}/mt.o

#
${PROJECT_NAME}:${OBJ_FILES}
	mkdir -p $(BUILD_PATH)
	#
	rm -f $(BUILD_PATH)/goodmtx.exe
	$(CCC) -o $(BUILD_PATH)/goodmtx.exe ${OBJ_PATH}/mtx.o $(LINK_FLAGS)
	ln -f -s goodmtx.exe $(BUILD_PATH)/goodmtx
	#
	rm -f $(BUILD_PATH)/goodmt.exe
	$(CCC) -o $(BUILD_PATH)/goodmt.exe ${OBJ_PATH}/mt.o $(LINK_FLAGS)
	ln -f -s goodmt.exe $(BUILD_PATH)/goodmt

#
$(OBJ_PATH)/%.o: %.c
	mkdir -p $(OBJ_PATH)
	rm -f $@
	$(CCC) $(CCC_STD) $(CCC_FLAGS) -c $< -o "$@"

#
clean:
	rm -rf $(OBJ_PATH)
	rm -f $(BUILD_PATH)/goodmtx.exe
	rm -f $(BUILD_PATH)/goodmtx
	rm -f $(BUILD_PATH)/goodmt.exe
	rm -f $(BUILD_PATH)/goodmt


#
INSTALL_PATH_BIN = $(abspath ${ROOT_PATH}/${INSTALL_PREFIX}/bin/)


#
install:
#
	mkdir -p ${INSTALL_PATH_BIN}
	cp -f -f $(BUILD_PATH)/goodmtx.exe ${INSTALL_PATH_BIN}/
	ln -f -s goodmtx.exe $(INSTALL_PATH_BIN)/goodmtx
	cp -f -f $(BUILD_PATH)/goodmt.exe ${INSTALL_PATH_BIN}/
	ln -f -s goodmt.exe $(INSTALL_PATH_BIN)/goodmt
#
uninstall:
#
	rm -f $(INSTALL_PATH_BIN)/goodmtx.exe
	rm -f $(INSTALL_PATH_BIN)/goodmtx
	rm -f $(INSTALL_PATH_BIN)/goodmt.exe
	rm -f $(INSTALL_PATH_BIN)/goodmt
