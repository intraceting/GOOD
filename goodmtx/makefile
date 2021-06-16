#
# This file is part of GOOD.
#
# MIT License
#
#


#
MAKE_CONF ?= $(abspath $(CURDIR)/../build/makefile.conf)

#
PROJECT_NAME = goodmtx

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
CCC_FLAGS += -fPIC -Wno-unused-result

#
LINK_FLAGS += -Wl,--as-needed -Wl,-rpath="./"

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
ifeq ($(HAVE_OPENMP),1)
CCC_FLAGS += -fopenmp
LINK_FLAGS += -fopenmp
endif 

#
OBJ_PATH = ${BUILD_PATH}/${PROJECT_NAME}.o.tmp/

# TEST源文件
OBJ_FILES += \
	${OBJ_PATH}/mtx.o

#
all: ${PROJECT_NAME}

#
${PROJECT_NAME}:${OBJ_FILES}
	mkdir -p $(BUILD_PATH)
	rm -f $(BUILD_PATH)/${PROJECT_NAME}.exe
	$(CCC) -o $(BUILD_PATH)/${PROJECT_NAME}.exe $^ $(LINK_FLAGS)

#
$(OBJ_PATH)/%.o: %.c
	mkdir -p $(OBJ_PATH)
	rm -f $@
	$(CCC) $(CCC_STD) $(CCC_FLAGS) -c $< -o "$@"

#
clean:
	rm -rf $(OBJ_PATH)
	rm -f $(BUILD_PATH)/${PROJECT_NAME}.exe

#
INSTALL_PATH_BIN = $(abspath ${INSTALL_PREFIX}/bin/)

#
install:
#
	mkdir -p ${INSTALL_PATH_BIN}
	cp -f -f $(BUILD_PATH)/${PROJECT_NAME}.exe ${INSTALL_PATH_BIN}/

#
uninstall:
#
	rm -f $(INSTALL_PATH_BIN)/${PROJECT_NAME}.exe
