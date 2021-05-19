#!/bin/bash

##
#
# This file is part of GOOD.
#  
# MIT License
##

# Functions
function checkReturnCode()
{
    rc=$?
    if [ $rc != 0 ];then
        exit $rc
    fi
}
##
# Functions
function copyToDir()
# $1 from-*
# $2 to-path
{
    mkdir -p "$2"
    cp -rf "$1" "$2"
    checkReturnCode
}

##
SHELL_PWD=$(cd `dirname $0`; pwd)
DATE_TIME=$(date +"%Y.%m.%d.%H%M%S")

#
VERSION_MAJOR="1"
VERSION_MINOR="0"

#
HOST_PLATFORM=$(uname -m)
TARGET_PLATFORM=${HOST_PLATFORM}
PACKAGE_PATH=${SHELL_PWD}/package/
BUILD_PATH=${SHELL_PWD}/src/build/
PKG_PATH=${BUILD_PATH}/pkgconfig/
DEPEND_FUNC="Nothing"

#
function PrintUsage()
{
    echo "usage: [ < -t ARGS > < -b ARGS > < -p ARGS > < -d ARGS > ]"
    echo "  -b 编译路径。默认：${BUILD_PATH}"
    echo "  -t 目标平台(x86_64 | aarch64)。默认：${TARGET_PLATFORM}"
    echo "  -p 发行路径。默认：${PACKAGE_PATH}"
    echo "  -d 依赖包。关键字：have-openmp,have-unixodbc,have-sqlite,have-openssl"
}

#
while getopts "b:t:p:d:?" ARGKEY
do
    case $ARGKEY in
    b)
        BUILD_PATH=$(realpath "${OPTARG}/")
    ;;
    t)
        TARGET_PLATFORM="$OPTARG"
    ;;
    p)
        PACKAGE_PATH="$OPTARG"
    ;;
    d)
        DEPEND_FUNC="$OPTARG"
    ;;
    \?)
        PrintUsage
        exit 22
    ;;
    esac
done

#
echo "SHELL_PWD=${SHELL_PWD}"
echo "DATE_TIME=${DATE_TIME}"

#
echo "VERSION_MAJOR=${VERSION_MAJOR}"
echo "VERSION_MINOR=${VERSION_MINOR}"

#
echo "HOST_PLATFORM=${HOST_PLATFORM}"
echo "TARGET_PLATFORM=${TARGET_PLATFORM}"
echo "BUILD_PATH=${BUILD_PATH}"
echo "PACKAGE_PATH=${PACKAGE_PATH}"
echo "DEPEND_FUNC=${DEPEND_FUNC}"

#
export PKG_CONFIG_PATH=${PKG_PATH}

#
TMP_PATH=/tmp/good-publish/good/

#
${SHELL_PWD}/src/configure.sh ${DEPEND_FUNC}
checkReturnCode

#
make -C ${SHELL_PWD}/src/  "VERSION_MAJOR=${VERSION_MAJOR}" "VERSION_MINOR=${VERSION_MINOR}"
checkReturnCode

#
make -C ${SHELL_PWD}/src/ install "INSTALL_PATH=${TMP_PATH}"
checkReturnCode

#
mkdir -p ${PACKAGE_PATH}
checkReturnCode

#
tar -czv -f "${PACKAGE_PATH}/good-${VERSION_MAJOR}.${VERSION_MINOR}-${TARGET_PLATFORM}.tar.gz" -C "${TMP_PATH}/../" "good"
checkReturnCode

#
if [ ${TMP_PATH:0:4} == "/tmp" ];then
rm -rf ${TMP_PATH}
fi