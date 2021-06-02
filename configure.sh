#!/bin/bash
#
# This file is part of GOOD.
#  
# MIT License
##

#
LC_ALL=C
export LC_ALL

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
	SYS_NAME=$1
#	lsb_release -is | grep -iE "${SYS_NAME}" |wc -l
	if [ -s /etc/redhat-release ];then
		cat /etc/redhat-release | grep -iE "${SYS_NAME}" |wc -l
	else
		cat /etc/issue | grep -iE "${SYS_NAME}" |wc -l
	fi
}

#
CheckPackageKitName()
{
	if [ $(CheckSystemName "Ubuntu|Debian") -ge 1 ];then
		echo "DEB"
	elif [ $(CheckSystemName "CentOS|Red Hat|RedHat|Amazon|Oracle") -ge 1 ];then
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
	NUM=$(echo "$1" |grep -i "$2" | wc -l)
    echo ${NUM}
}

#
SHELL_PWD=$(cd `dirname $0`; pwd)
KIT_NAME=$(CheckPackageKitName)

#
BUILD_PATH=$(realpath "${SHELL_PWD}/build/")
MAKE_CONF=${BUILD_PATH}/makefile.conf

#
VERSION_MAJOR="1"
VERSION_MINOR="0"

#
DEPEND_FUNC="Nothing"
BUILD_TYPE="release"
INSTALL_PREFIX="/usr/local/good/"

#
PrintUsage()
{
    echo "usage: [ OPTIONS ]"
    echo -e "\n\t-d < KEY,KEY,... >"
    echo -e "\t\t依赖项目。关键字：have-openmp,have-unixodbc,have-sqlite,have-openssl"
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
echo "BUILD_PATH=${BUILD_PATH}"
echo "MAKE_CONF=${MAKE_CONF}"

#
DEPEND_FLAGS="-D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 ${DEPEND_FLAGS}"

#
DEPEND_LIBS="-ldl -pthread -lrt -lc -lm ${DEPEND_LIBS}"

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-openmp") -eq 1 ];then
{
    HAVE_OPENMP=1
    DEPEND_FLAGS=" -DHAVE_OPENMP -fopenmp ${DEPEND_FLAGS}"
    DEPEND_LIBS="  -fopenmp ${DEPEND_LIBS}"
}
fi

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-unixodbc") -eq 1 ];then
{
    HAVE_UNIXODBC=$(CheckHavePackage ${KIT_NAME} unixodbc-dev unixODBC-devel)
    if [ ${HAVE_UNIXODBC} -ge 1 ];then
    {
        DEPEND_FLAGS=" -DHAVE_UNIXODBC ${DEPEND_FLAGS}"
        DEPEND_LIBS=" -lodbc ${DEPEND_LIBS}"
    }
    fi
}
fi

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-sqlite") -eq 1 ];then
{
    HAVE_SQLITE=$(CheckHavePackage ${KIT_NAME} libsqlite3-dev sqlite-devel)
    if [ ${HAVE_SQLITE} -ge 1 ];then
    {
        DEPEND_FLAGS=" -DHAVE_SQLITE ${DEPEND_FLAGS}"
        DEPEND_FLAGS=" $(pkg-config --cflags sqlite3) ${DEPEND_FLAGS}"
        DEPEND_LIBS=" $(pkg-config --libs sqlite3) ${DEPEND_LIBS}"
    }
    fi
}
fi

#
if [ $(checkKeyword ${DEPEND_FUNC} "have-openssl") -eq 1 ];then
{
    HAVE_OPENSSL=$(CheckHavePackage ${KIT_NAME} libssl-dev openssl-devel)
    if [ ${HAVE_OPENSSL} -ge 1 ];then
    {
        DEPEND_FLAGS=" -DHAVE_OPENSSL ${DEPEND_FLAGS}"
        DEPEND_FLAGS=" $(pkg-config --cflags openssl) ${DEPEND_FLAGS}"
        DEPEND_LIBS=" $(pkg-config --libs openssl) ${DEPEND_LIBS}"
    }
    fi
}
fi

#
echo "VERSION_MAJOR=${VERSION_MAJOR}"
echo "VERSION_MINOR=${VERSION_MINOR}"

#
echo "HAVE_OPENMP=${HAVE_OPENMP}"
echo "HAVE_UNIXODBC=${HAVE_UNIXODBC}"
echo "HAVE_SQLITE=${HAVE_SQLITE}"
echo "HAVE_OPENSSL=${HAVE_OPENSSL}"

#
echo "BUILD_TYPE=${BUILD_TYPE}"
echo "INSTALL_PREFIX?=${INSTALL_PREFIX}"

#
echo "#" > ${MAKE_CONF}
echo "# GOOD is good!" >> ${MAKE_CONF}
echo "#" >> ${MAKE_CONF}
echo "" >> ${MAKE_CONF}

#
echo "BUILD_PATH = ${BUILD_PATH}" >> ${MAKE_CONF}
echo "VERSION_MAJOR = ${VERSION_MAJOR}" >> ${MAKE_CONF}
echo "VERSION_MINOR = ${VERSION_MINOR}" >> ${MAKE_CONF}
echo "DEPEND_FLAGS = ${DEPEND_FLAGS}" >> ${MAKE_CONF}
echo "DEPEND_LIBS = ${DEPEND_LIBS}" >> ${MAKE_CONF}
echo "BUILD_TYPE = ${BUILD_TYPE}" >> ${MAKE_CONF}
echo "INSTALL_PREFIX ?= ${INSTALL_PREFIX}" >> ${MAKE_CONF}