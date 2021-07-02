#!/bin/bash
#
# This file is part of ABCDK.
#  
# MIT License
##


# Functions
checkReturnCode()
{
    rc=$?
    if [ $rc != 0 ];then
        exit $rc
    fi
}

#
CheckSystemName()
# $1 System Name
{
    if [ -f /etc/os-release ];then 
	    grep '^ID=' /etc/os-release |cut -d = -f 2 |sed 's/\"//g' | grep -iE "${1}" |wc -l
    elif [ -f /usr/lib/os-release ];then 
        grep '^ID=' /usr/lib/os-release |cut -d = -f 2 |sed 's/\"//g' | grep -iE "${1}" |wc -l
    else 
        echo "0"
    fi 
}

#
CheckPackageKitName()
{
	if [ $(CheckSystemName "Ubuntu|Debian") -ge 1 ];then
		echo "DEB"
	elif [ $(CheckSystemName "CentOS|Red Hat|RedHat|RHEL|fedora|Amazon|Oracle") -ge 1 ];then
		echo "RPM"
	else
		echo ""
	fi
}

#
CheckHavePackage()
# $1 KIT_NAME
# $2 DEB_NAME
# $3 RPM_NAME
{
    #
    HAVE_NUM="0"
    #
	if [ "DEB" == "$1" ];then 
		HAVE_NUM=$(dpkg -l |grep $2 | wc -l)
	elif [ "RPM" == "$1" ];then
		HAVE_NUM=$(rpm -qa |grep $3 | wc -l)
    fi 
	#
	echo "${HAVE_NUM}"
}

#
checkKeyword()
# $1 keywords
# $2 word
{
	NUM=$(echo "$1" |grep -wi "$2" | wc -l)
    echo ${NUM}
}

#
SOLUTION_NAME=abcdk

#
SHELL_PWD=$(cd `dirname $0`; pwd)
KIT_NAME=$(CheckPackageKitName)

#
BUILD_PATH=$(realpath "${SHELL_PWD}/build/")
MAKE_CONF=${BUILD_PATH}/makefile.conf

#
HOST_PLATFORM=$(uname -m)
TARGET_PLATFORM=${HOST_PLATFORM}

#
VERSION_MAJOR="1"
VERSION_MINOR="1"
VERSION_DATETIME=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

#
DEPEND_FUNC="Nothing"
BUILD_TYPE="release"
INSTALL_PREFIX="/usr/local/${SOLUTION_NAME}/"

#
PrintUsage()
{
    echo "usage: [ OPTIONS ]"
    echo -e "\n\t-d < KEY,KEY,... >"
    echo -e "\t\t依赖项目。关键字：have-openmp,have-unixodbc,have-sqlite,have-openssl,have-ffmpeg,have-freeimage"
    echo -e "\n\t-g"
    echo -e "\t\t生成调试符号。默认：关闭。"
    echo -e "\n\t-i < PATH >"
    echo -e "\t\t安装路径。默认：${INSTALL_PREFIX}"
}

#
while getopts "?d:gi:" ARGKEY 
do
    case $ARGKEY in
    \?)
        PrintUsage
        exit 22
    ;;
    d)
        DEPEND_FUNC="$OPTARG"
    ;;
    g)
        BUILD_TYPE="debug"
    ;;
    i)
        INSTALL_PREFIX=$(realpath "${OPTARG}/")
    ;;
    esac
done

#
mkdir -p ${BUILD_PATH}

#
if [ ! -d ${BUILD_PATH} ];then
echo "'${BUILD_PATH}' must be an existing directory."
exit 22
fi 


#
DEPEND_FLAGS="-D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 ${DEPEND_FLAGS}"

#
DEPEND_LIBS="-ldl -pthread -lrt -lc -lm ${DEPEND_LIBS}"

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-openmp") -eq 1 ];then
{
    OPENMP_EXIST=$(CheckHavePackage ${KIT_NAME} libomp-dev libopm-devel)
    if [ ${OPENMP_EXIST} -ge 1 ];then
    {
        HAVE_OPENMP="Yes"
        DEPEND_FLAGS=" -DHAVE_OPENMP -fopenmp ${DEPEND_FLAGS}"
        DEPEND_LIBS=" -fopenmp ${DEPEND_LIBS}"
    }
    else
    {
        echo "libomp-dev or libopm-devel not find."
        exit 22
    }
    fi
}
fi

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-unixodbc") -eq 1 ];then
{
    UNIXODBC_EXIST=$(CheckHavePackage ${KIT_NAME} unixodbc-dev unixODBC-devel)
    if [ ${UNIXODBC_EXIST} -ge 1 ];then
    {
        HAVE_UNIXODBC="Yes"
        DEPEND_FLAGS=" -DHAVE_UNIXODBC ${DEPEND_FLAGS}"
        DEPEND_LIBS=" -lodbc ${DEPEND_LIBS}"
    }
    else
    {
        echo "unixodbc-dev or unixODBC-devel not find."
        exit 22
    }
    fi
}
fi

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-sqlite") -eq 1 ];then
{
    SQLITE_PCS="sqlite3"
    SQLITE_EXIST=$(pkg-config --exists ${SQLITE_PCS} --short-errors ; echo $?)
    if [ ${SQLITE_EXIST} -eq 0 ];then
    {
        HAVE_SQLITE="Yes"
        DEPEND_FLAGS=" -DHAVE_SQLITE ${DEPEND_FLAGS}"
        DEPEND_FLAGS=" $(pkg-config --cflags ${SQLITE_PCS}) ${DEPEND_FLAGS}"
        DEPEND_LIBS=" $(pkg-config --libs ${SQLITE_PCS}) ${DEPEND_LIBS}"
    }
    else
    {
        echo "libsqlite3-dev or sqlite-devel not find."
        exit 22
    }
    fi
}
fi

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-openssl") -eq 1 ];then
{
    OPENSSL_PCS="openssl"
    OPENSSL_EXIST=$(pkg-config --exists ${OPENSSL_PCS} --short-errors ; echo $?)
    if [ ${OPENSSL_EXIST} -eq 0 ];then
    {
        HAVE_OPENSSL="Yes"
        DEPEND_FLAGS=" -DHAVE_OPENSSL ${DEPEND_FLAGS}"
        DEPEND_FLAGS=" $(pkg-config --cflags ${OPENSSL_PCS}) ${DEPEND_FLAGS}"
        DEPEND_LIBS=" $(pkg-config --libs ${OPENSSL_PCS}) ${DEPEND_LIBS}"
    }
    else
    {
        echo "libssl-dev or openssl-devel not find."
        exit 22
    }
    fi
}
fi

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-ffmpeg") -eq 1 ];then
{
    FFMPEG_PCS="libswscale libavutil"
    FFMPEG_EXIST=$(pkg-config --exists ${FFMPEG_PCS} --short-errors ; echo $?)
    if [ ${FFMPEG_EXIST} -eq 0 ];then
    {
        HAVE_FFMPEG="Yes"
        DEPEND_FLAGS=" -DHAVE_FFMPEG ${DEPEND_FLAGS}"
        DEPEND_FLAGS=" $(pkg-config --cflags ${FFMPEG_PCS}) ${DEPEND_FLAGS}"
        DEPEND_LIBS=" $(pkg-config --libs ${FFMPEG_PCS}) ${DEPEND_LIBS}"
    }
    else
    {
        echo "libswscale-dev or libavutil-dev or ffmpeg-devel not find."
        exit 22
    }
    fi
}
fi

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-freeimage") -eq 1 ];then
{
    FREEIMAGE_EXIST=$(CheckHavePackage ${KIT_NAME} libfreeimage-dev libfreeimage-devel)
    if [ ${FREEIMAGE_EXIST} -ge 1 ];then
    {
        HAVE_FREEIMAGE="Yes"
        DEPEND_FLAGS=" -DHAVE_FREEIMAGE ${DEPEND_FLAGS}"
        DEPEND_LIBS=" -lfreeimage ${DEPEND_LIBS}"
    }
    else
    {
        echo "libfreeimage-dev or libfreeimage-devel not find."
        exit 22
    }
    fi
}
fi

#
echo "SOLUTION_NAME=${SOLUTION_NAME}"

#
echo "BUILD_PATH=${BUILD_PATH}"
echo "MAKE_CONF=${MAKE_CONF}"

#
echo "HOST_PLATFORM=${HOST_PLATFORM}"
echo "TARGET_PLATFORM=${TARGET_PLATFORM}"

#
echo "VERSION_MAJOR=${VERSION_MAJOR}"
echo "VERSION_MINOR=${VERSION_MINOR}"
echo "VERSION_DATETIME=${VERSION_DATETIME}"

#
echo "HAVE_OPENMP=${HAVE_OPENMP}"
echo "HAVE_UNIXODBC=${HAVE_UNIXODBC}"
echo "HAVE_SQLITE=${HAVE_SQLITE}"
echo "HAVE_OPENSSL=${HAVE_OPENSSL}"
echo "HAVE_FFMPEG=${HAVE_FFMPEG}"
echo "HAVE_FREEIMAGE=${HAVE_FREEIMAGE}"

#
echo "BUILD_TYPE=${BUILD_TYPE}"
echo "INSTALL_PREFIX=${INSTALL_PREFIX}"
echo "ROOT_PATH?=/"

#
echo "#" > ${MAKE_CONF}
echo "# ABCDK is abcdk!" >> ${MAKE_CONF}
echo "#" >> ${MAKE_CONF}
echo "" >> ${MAKE_CONF}

#
echo "SOLUTION_NAME = ${SOLUTION_NAME}" >> ${MAKE_CONF}
echo "BUILD_PATH = ${BUILD_PATH}" >> ${MAKE_CONF}
echo "HOST_PLATFORM = ${HOST_PLATFORM}" >> ${MAKE_CONF}
echo "TARGET_PLATFORM = ${TARGET_PLATFORM}" >> ${MAKE_CONF}
echo "VERSION_MAJOR = ${VERSION_MAJOR}" >> ${MAKE_CONF}
echo "VERSION_MINOR = ${VERSION_MINOR}" >> ${MAKE_CONF}
echo "VERSION_DATETIME = ${VERSION_DATETIME}" >> ${MAKE_CONF}
echo "DEPEND_FLAGS = ${DEPEND_FLAGS}" >> ${MAKE_CONF}
echo "DEPEND_LIBS = ${DEPEND_LIBS}" >> ${MAKE_CONF}
echo "BUILD_TYPE = ${BUILD_TYPE}" >> ${MAKE_CONF}
echo "INSTALL_PREFIX = ${INSTALL_PREFIX}" >> ${MAKE_CONF}
echo "ROOT_PATH ?= /" >> ${MAKE_CONF}