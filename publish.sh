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
PACKAGE_PATH=${SHELL_PWD}/package/
BUILD_PATH=${SHELL_PWD}/src/build/
DEPEND_FUNC="Nothing"

#
function PrintUsage()
{
    echo "usage: [ < -b ARGS > < -p ARGS > < -d ARGS > ]"
    echo -e "\n  -p 发行路径。默认：${PACKAGE_PATH}"
    echo -e "\n  -b 编译路径。默认：${BUILD_PATH}"
    echo -e "\n  -d 依赖组件。关键字：have-openmp,have-unixodbc,have-sqlite,have-openssl"
}

#
while getopts "b:t:p:d:?" ARGKEY
do
    case $ARGKEY in
    b)
        BUILD_PATH=$(realpath "${OPTARG}/")
    ;;
    p)
        PACKAGE_PATH=$(realpath "${OPTARG}/")
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
if [ ! -d ${BUILD_PATH} ];then
echo "'${BUILD_PATH}' must be an existing directory."
exit 22
fi 

#
if [ ! -d ${PACKAGE_PATH} ];then
echo "'${PACKAGE_PATH}' must be an existing directory."
exit 22
fi 

#
echo "SHELL_PWD=${SHELL_PWD}"
echo "DATE_TIME=${DATE_TIME}"

#
echo "VERSION_MAJOR=${VERSION_MAJOR}"
echo "VERSION_MINOR=${VERSION_MINOR}"

#
echo "HOST_PLATFORM=${HOST_PLATFORM}"
echo "BUILD_PATH=${BUILD_PATH}"
echo "PACKAGE_PATH=${PACKAGE_PATH}"
echo "DEPEND_FUNC=${DEPEND_FUNC}"

#
export PKG_CONFIG_PATH="${BUILD_PATH}/pkgconfig/"

#
${SHELL_PWD}/src/configure.sh -b ${BUILD_PATH} -d ${DEPEND_FUNC}
checkReturnCode

#
make -C ${SHELL_PWD}/src/ "VERSION_MAJOR=${VERSION_MAJOR}" "VERSION_MINOR=${VERSION_MINOR}" "BUILD_PATH=${BUILD_PATH}"
checkReturnCode

#
TMP_PATH=/tmp/good-publish/good/

#
make -C ${SHELL_PWD}/src/ install "INSTALL_PATH=${TMP_PATH}"
checkReturnCode

#
mkdir -p ${PACKAGE_PATH}
checkReturnCode

#
tar -czv -f "${PACKAGE_PATH}/good-${VERSION_MAJOR}.${VERSION_MINOR}-${HOST_PLATFORM}.tar.gz" -C "${TMP_PATH}/../" "good"
checkReturnCode

#
if [ ${TMP_PATH:0:4} == "/tmp" ];then
rm -rf ${TMP_PATH}
fi