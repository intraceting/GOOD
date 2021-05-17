#!/bin/bash
#
function CheckSystemName()
# $1 System Name
{
	SYS_NAME=$1
#	lsb_release -is | grep -iE "${SYS_NAME}" |wc -l
	if [ -s /etc/redhat-release ]
	then
		cat /etc/redhat-release | grep -iE "${SYS_NAME}" |wc -l
	else
		cat /etc/issue | grep -iE "${SYS_NAME}" |wc -l
	fi
}

#
function CheckPackageKitName()
{
	if [ $(CheckSystemName "Ubuntu|Debian") -ge 1 ]
	then
		echo "DEB"
	elif [ $(CheckSystemName "CentOS|Red Hat|RedHat|Amazon|Oracle") -ge 1 ]
	then
		echo "RPM"
	else
		echo ""
	fi
}

#
function CheckHavePackage()
# $1 name(aaaa|bbbb|ccccc|ddddd)
{
	#
	SYS_CONF=$(CheckPackageKitName)
	#用“|”分割包名字
	OLD_IFS="$IFS"
	IFS="|"
	DEP_NAMES=($1)
	IFS="$OLD_IFS"
	#
	DEP_HAVE=""
    HAVE_NUM=0
	#
	for DEP_NAME in ${DEP_NAMES[@]}
	do
		#
		if [ ${SYS_CONF} == "RPM" ]
		then 
			DEP_HAVE=$(rpm -qa |grep ${DEP_NAME} | wc -l )
		elif [ ${SYS_CONF} == "DEB" ]
		then
			DEP_HAVE=$(dpkg -l |grep ${DEP_NAME} | wc -l )
		else
			exit 22;
		fi
		# 
		if [ ${DEP_HAVE} -ge 1 ]
		then
			HAVE_NUM=$(expr ${HAVE_NUM} + 1)
		fi
	done
	#
	echo "${HAVE_NUM}"
}

##
SHELL_PWD=$(cd `dirname $0`; pwd)
DATE_TIME=$(date +"%Y-%m-%dT%H:%M:%S")

#
HOST_PLATFORM=$(uname -m)
TARGET_PLATFORM=$(uname -m)
BUILD_PATH=$(realpath "${SHELL_PWD}/build/")
PKG_PATH=${BUILD_PATH}/pkgconfig/

#
function PrintUsage()
{
    echo "usage: [ < -p ARGS > < -b ARGS > < -c ARGS > ]"
    echo "  -p 目标平台(x86_64 | aarch64)。默认：${TARGET_PLATFORM}"
    echo "  -b 编译路径。默认：${BUILD_PATH}"
}

#
while getopts "p:b:?" ARGKEY 
do
    case $ARGKEY in
    b)
        BUILD_PATH=$(realpath "${OPTARG}/")
    ;;
    p)
        TARGET_PLATFORM="$OPTARG"
    ;;
    \?)
        PrintUsage
        exit 22
    ;;
    esac
done
#
if [ ! -d ${BUILD_PATH} ];then
echo "'${BUILD_PATH}' must be an existing directory."
exit 22
fi 

#
echo "HOST_PLATFORM=${HOST_PLATFORM}"
echo "TARGET_PLATFORM=${TARGET_PLATFORM}"
echo "BUILD_PATH=${BUILD_PATH}"
echo "PKG_PATH=${PKG_PATH}"

#
mkdir -p "${PKG_PATH}"

#
LIBUTIL_DEPEND_PC=${PKG_PATH}/libutil-depend.pc
LIBUTIL_PC=${PKG_PATH}/libutil.pc

#
PKG_FLAGS=" ${PKG_FLAGS} -fopenmp"
PKG_FLAGS="-D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 ${PKG_FLAGS}"
#
PKG_LIBS=" ${PKG_LIBS} -fopenmp"
PKG_LIBS="-ldl -pthread -lrt -lc -lm ${PKG_LIBS}"

#
HAVE_UNIXODBC=$(CheckHavePackage "unixodbc-dev|unixODBC-devel")
if [ ${HAVE_UNIXODBC} -ge 1 ];then
PKG_LIBS=" -lodbc ${PKG_LIBS}"
fi

#
HAVE_SQLITE=$(CheckHavePackage "libsqlite3-dev|sqlite-devel")
if [ ${HAVE_SQLITE} -ge 1 ];then
PKG_FLAGS=" $(pkg-config --cflags sqlite3) ${PKG_FLAGS}"
PKG_LIBS=" $(pkg-config --libs sqlite3) ${PKG_LIBS}"
fi

#
HAVE_OPENSSL=$(CheckHavePackage "libssl-dev|openssl-devel")
if [ ${HAVE_OPENSSL} -ge 1 ];then
PKG_FLAGS=" $(pkg-config --cflags openssl) ${PKG_FLAGS}"
PKG_LIBS=" $(pkg-config --libs openssl) ${PKG_LIBS}"
fi

#
echo "HAVE_UNIXODBC=${HAVE_UNIXODBC}"
echo "HAVE_SQLITE=${HAVE_SQLITE}"
echo "HAVE_OPENSSL=${HAVE_OPENSSL}"

#
echo "Name: libutil dependent item" > ${LIBUTIL_DEPEND_PC}
echo "Description: HOST_PLATFORM=${HOST_PLATFORM} TARGET_PLATFORM=${TARGET_PLATFORM}" >> ${LIBUTIL_DEPEND_PC}
echo "Version: ${DATE_TIME}" >> ${LIBUTIL_DEPEND_PC}
echo "Cflags: ${PKG_FLAGS}" >> ${LIBUTIL_DEPEND_PC}
echo "Libs: ${PKG_LIBS}" >> ${LIBUTIL_DEPEND_PC}

#
PKG_FLAGS=" -I${SHELL_PWD} ${PKG_FLAGS}"
PKG_LIBS=" -lgood_util ${PKG_LIBS}"
PKG_LIBS=" -L${BUILD_PATH}/ -Wl,-rpath-link=${BUILD_PATH}/ ${PKG_LIBS}"

#
echo "Name: libutil" > ${LIBUTIL_PC}
echo "Description: HOST_PLATFORM=${HOST_PLATFORM} TARGET_PLATFORM=${TARGET_PLATFORM}" >> ${LIBUTIL_PC}
echo "Version: ${DATE_TIME}" >> ${LIBUTIL_PC}
echo "Cflags: ${PKG_FLAGS}" >> ${LIBUTIL_PC}
echo "Libs: ${PKG_LIBS}" >> ${LIBUTIL_PC}