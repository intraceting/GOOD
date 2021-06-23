#
# This file is part of GOOD.
#
# MIT License
#
#

#
MAKE_CONF ?= $(abspath $(CURDIR)/../build/makefile.conf)

#
PROJECT_NAME = goodswitch

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
ARNAME = lib${PROJECT_NAME}.a
ARREALNAME = ${ARNAME}.${VERSION_MAJOR}.${VERSION_MINOR}
#
SONAME = lib${PROJECT_NAME}.so
SOREALNAME = ${SONAME}.${VERSION_MAJOR}.${VERSION_MINOR}
#
PKGNAME = ${PROJECT_NAME}.pc

#Archive
ARC = ar

#Compiler
CCC = gcc

#Standard
CCC_STD = -std=c11

#-Werror -Wall -Wextra
CCC_FLAGS += -fPIC -Wno-unused-result

#
LINK_FLAGS += -Wl,--as-needed -Wl,-rpath="./" -Wl,-rpath="${INSTALL_PREFIX}/lib/"

#
LINK_FLAGS += -Wl,--soname,${SONAME}

#
CCC_FLAGS += -DBUILD_VERSION_MAJOR=${VERSION_MAJOR} -DBUILD_VERSION_MINOR=${VERSION_MINOR} -DBUILD_VERSION_DATETIME=\"${VERSION_DATETIME}\"

#
ifeq (${BUILD_TYPE},debug)
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
OBJ_FILES = \
	${OBJ_PATH}/mux.o


#
all: ${PROJECT_NAME}

#
${PROJECT_NAME}: $(OBJ_FILES)
	mkdir -p $(BUILD_PATH)
	rm -f $(BUILD_PATH)/${SOREALNAME}
	$(CCC) -o $(BUILD_PATH)/${SOREALNAME} $^ $(LINK_FLAGS) -shared
	ln -f -s ${SOREALNAME} $(BUILD_PATH)/${SONAME}
	rm -f $(BUILD_PATH)/${ARREALNAME}
	$(ARC) rcs $(BUILD_PATH)/${ARREALNAME} $^
	ln -f -s ${ARREALNAME} $(BUILD_PATH)/${ARNAME}
#
$(OBJ_PATH)/%.o: %.c
	mkdir -p $(OBJ_PATH)
	rm -f $@
	$(CCC) $(CCC_STD) $(CCC_FLAGS) -c $< -o "$@"

#
clean:
	rm -rf $(OBJ_PATH)
	rm -f $(BUILD_PATH)/${SOREALNAME}
	rm -f $(BUILD_PATH)/${SONAME}
	rm -f $(BUILD_PATH)/${ARREALNAME}
	rm -f $(BUILD_PATH)/${ARNAME}


#
INSTALL_PATH_INC = $(abspath ${ROOT_PATH}/${INSTALL_PREFIX}/include/${PROJECT_NAME}/)
INSTALL_PATH_LIB = $(abspath ${ROOT_PATH}/${INSTALL_PREFIX}/lib/)
INSTALL_PATH_BIN = $(abspath ${ROOT_PATH}/${INSTALL_PREFIX}/bin/)
INSTALL_PATH_PKG = $(abspath ${ROOT_PATH}/${INSTALL_PREFIX}/pkgconfig/)


#
install:
#
	mkdir -p ${INSTALL_PATH_LIB}
	cp -f $(BUILD_PATH)/${SOREALNAME} ${INSTALL_PATH_LIB}/
	ln -f -s ${SOREALNAME} ${INSTALL_PATH_LIB}/${SONAME}
	cp -f $(BUILD_PATH)/${ARREALNAME} ${INSTALL_PATH_LIB}/
	ln -f -s ${ARREALNAME} ${INSTALL_PATH_LIB}/${ARNAME}
	
#
	mkdir -p ${INSTALL_PATH_INC}/
	cp  -f $(CURDIR)/mux.h ${INSTALL_PATH_INC}/

#
	mkdir -p ${INSTALL_PATH_PKG}
	echo "prefix=${INSTALL_PREFIX}" > ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "libdir=\$${prefix}/lib/" >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "incdir=\$${prefix}/include/${PROJECT_NAME}/" >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "" >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "Name: ${PROJECT_NAME}" >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "Description: This ${PROJECT_NAME} is part of GOOD. " >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "Version: ${VERSION_MAJOR}.${VERSION_MINOR}" >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "Cflags: -I\$${incdir}" >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "Libs: -l${PROJECT_NAME} -L\$${libdir}" >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "Libs.private: ${DEPEND_LIBS}" >> ${INSTALL_PATH_PKG}/${PKGNAME}

#
uninstall:
#
	rm -f ${INSTALL_PATH_LIB}/${SOREALNAME}
	rm -f ${INSTALL_PATH_LIB}/${SONAME}
	rm -f ${INSTALL_PATH_LIB}/${ARREALNAME}
	rm -f ${INSTALL_PATH_LIB}/${ARNAME}

#
	rm -f ${INSTALL_PATH_INC}/mux.h

	#
	rm -f ${INSTALL_PATH_PKG}/${PKGNAME}
