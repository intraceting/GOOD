#
MAKE_CONF ?= $(abspath $(CURDIR)/../build/makefile.conf)

#
PROJECT_NAME = goodutil

#
VERSION_MAJOR = 1
VERSION_MINOR = 0

#
SONAME = lib${PROJECT_NAME}.so
SOREALNAME = ${SONAME}.${VERSION_MAJOR}.${VERSION_MINOR}
PKGNAME = ${PROJECT_NAME}.pc

#加载配置项。
include ${MAKE_CONF}

#
ifeq (${BUILD_PATH},)
BUILD_PATH = $(abspath $(CURDIR)/../build/)
else ifeq (${BUILD_PATH},"")
BUILD_PATH = $(abspath $(CURDIR)/../build/)
endif

#Compiler
CCC = gcc

#Standard
CCC_STD = -std=c11

#-Werror -Wall -Wextra
CCC_FLAGS = -fPIC -Wno-unused-result -MMD -MP -MF "$@.mdep"

#
LINK_FLAGS = -Wl,--as-needed -Wl,-rpath="./"

#
LINK_FLAGS = -Wl,--soname,${SONAME}

#
CCC_FLAGS += -DBUILD_VERSION_MAJOR=${VERSION_MAJOR} -DBUILD_VERSION_MINOR=${VERSION_MINOR}

#
ifeq (${BUILD_TYPE},debug)
CCC_FLAGS += -g
else 
CCC_FLAGS += -s -O2
endif

#
CCC_FLAGS += ${DEPEND_FLAGS}

#
LINK_FLAGS += ${DEPEND_LIBS}

#
OBJ_PATH = ${BUILD_PATH}/${PROJECT_NAME}.o.tmp/
#
OBJ_FILES = \
	${OBJ_PATH}/general.o \
	${OBJ_PATH}/log.o \
	${OBJ_PATH}/thread.o \
	${OBJ_PATH}/signal.o \
	${OBJ_PATH}/crc32.o \
	${OBJ_PATH}/base64.o \
	${OBJ_PATH}/clock.o \
	${OBJ_PATH}/geometry.o \
	${OBJ_PATH}/allocator.o \
	${OBJ_PATH}/mman.o \
	${OBJ_PATH}/buffer.o \
	${OBJ_PATH}/pool.o \
	${OBJ_PATH}/tree.o \
	${OBJ_PATH}/map.o \
	${OBJ_PATH}/option.o \
	${OBJ_PATH}/getargs.o \
	${OBJ_PATH}/dirent.o \
	${OBJ_PATH}/notify.o \
	${OBJ_PATH}/iconv.o \
	${OBJ_PATH}/socket.o \
	${OBJ_PATH}/epoll.o \
	${OBJ_PATH}/scsi.o \
	${OBJ_PATH}/mtx.o \
	${OBJ_PATH}/mt.o \
	${OBJ_PATH}/blockio.o \
	${OBJ_PATH}/tar.o \
	${OBJ_PATH}/termios.o


#
all: ${PROJECT_NAME}

#
${PROJECT_NAME}: $(OBJ_FILES)
	mkdir -p $(BUILD_PATH)
	rm -f $@
	$(CCC) -o $(BUILD_PATH)/${SOREALNAME} $^ $(LINK_FLAGS) -shared
	ln -f -s ${SOREALNAME} $(BUILD_PATH)/${SONAME}

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

#
ifeq (${INSTALL_PREFIX},)
INSTALL_PREFIX = /usr/local/good/
else ifeq (${INSTALL_PREFIX},"")
INSTALL_PREFIX = /usr/local/good/
endif

#
INSTALL_PATH_INC = $(abspath ${INSTALL_PREFIX}/include/${PROJECT_NAME}/)
INSTALL_PATH_LIB = $(abspath ${INSTALL_PREFIX}/lib/)
INSTALL_PATH_BIN = $(abspath ${INSTALL_PREFIX}/bin/)
INSTALL_PATH_PKG = $(abspath ${INSTALL_PREFIX}/pkgconfig/)

#
install:
#
	mkdir -p ${INSTALL_PATH_LIB}
	cp -f $(BUILD_PATH)/${SOREALNAME} ${INSTALL_PATH_LIB}/
	ln -f -s ${SOREALNAME} ${INSTALL_PATH_LIB}/${SONAME}

#
	mkdir -p ${INSTALL_PATH_INC}/
	cp  *.h ${INSTALL_PATH_INC}/

#
	mkdir -p ${INSTALL_PATH_PKG}
	echo "prefix=${INSTALL_PREFIX}" > ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "libdir=${INSTALL_PATH_LIB}" >> ${INSTALL_PATH_PKG}/${PKGNAME}
	echo "incdir=${INSTALL_PATH_INC}" >> ${INSTALL_PATH_PKG}/${PKGNAME}
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
	rm -f ${INSTALL_PATH_INC}/*.h

#
	rm -f ${INSTALL_PATH_LIB}/${SOREALNAME}
	rm -f ${INSTALL_PATH_LIB}/${SONAME}
	rm -f ${INSTALL_PATH_PKG}/${PKGNAME}
