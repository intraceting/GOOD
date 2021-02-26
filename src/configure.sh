#!/bin/bash -x

#
function CheckPackageToolName()
{
    #
    SYSNAME=$(lsb_release -is)
    #
    if [ ${SYSNAME} == "CentOS" ] || [ ${SYSNAME} == "RedHat" ] || [ ${SYSNAME} == "Red Hat" ] || [ ${SYSNAME} == "Amazon" ]
    then
        echo "RPM"
    elif [ ${SYSNAME} == "Ubuntu" ] || [ ${SYSNAME} == "Debian" ]
    then
        echo "DEB"
    else 
        echo ""
    fi
}

#
function CheckHavePackage()
# $1 name(aaaa|bbbb|ccccc|ddddd)
{
	#用“|”分割包名字
	OLD_IFS="$IFS"
	IFS="|"
	DEP_NAMES=($1)
	IFS="$OLD_IFS"
	#
	SYS_CONF=$(CheckPackageToolName)
	DEP_HAVE=""
    HAVE_NUM=0
	#
	for DEP_NAME in ${DEP_NAMES[@]}
	do
		#
		if [ ${SYS_CONF} == "RPM" ]
		then 
			DEP_HAVE=$(rpm -qa |grep -i ${DEP_NAME} | wc -l )
		elif [ ${SYS_CONF} == "DEB" ]
		then
			DEP_HAVE=$(dpkg -l |grep -i ${DEP_NAME} | wc -l )
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

#
while getopts "b:p:?" ARGKEY 
do
    case $ARGKEY in
    b)
        BUILD_PATH=$(realpath "${OPTARG}/")
    ;;
    p)
        TARGET_PLATFORM="$OPTARG"
    ;;
    \?)
        echo "usage: [ -b BUILD-PATH ] [ -p PLATFORM ]"
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


#
LIBCWUTIL_DEPEND_PC=${BUILD_PATH}/libcwutil-depend.pc
LIBCWUTIL_PC=${BUILD_PATH}/libcwutil.pc

#
PKG_LIBS="-ldl -pthread -lrt -lc -lm"

#
HAVE_UNIXODBC=$(CheckHavePackage "unixodbc-dev|unixODBC-devel")
if [ ${HAVE_UNIXODBC} -ge 1 ];then
PKG_FLAGS="${PKG_FLAGS} -DHAVE_UNIXODBC"
fi

#
HAVE_UNIXODBC=$(CheckHavePackage "libsqlite3-dev|sqlite-devel")
if [ ${HAVE_UNIXODBC} -ge 1 ];then
PKG_FLAGS="${PKG_FLAGS} -DHAVE_SQLITE $(pkg-config --cflags sqlite3)"
PKG_LIBS="${PKG_LIBS} $(pkg-config --libs sqlite3)"
fi

#
HAVE_UNIXODBC=$(CheckHavePackage "libssl-dev|openssl-devel")
if [ ${HAVE_UNIXODBC} -ge 1 ];then
PKG_FLAGS="${PKG_FLAGS} -DHAVE_OPENSSL $(pkg-config --cflags openssl)"
PKG_LIBS="${PKG_LIBS}  $(pkg-config --libs openssl)"
fi

#
echo "Name: libcwutil dependent item" > ${LIBCWUTIL_DEPEND_PC}
echo "Description: HOST_PLATFORM=${HOST_PLATFORM} TARGET_PLATFORM=${TARGET_PLATFORM}" >> ${LIBCWUTIL_DEPEND_PC}
echo "Version: ${DATE_TIME}" >> ${LIBCWUTIL_DEPEND_PC}
echo "Cflags: ${PKG_FLAGS}" >> ${LIBCWUTIL_DEPEND_PC}
echo "Libs: ${PKG_LIBS}" >> ${LIBCWUTIL_DEPEND_PC}

#
PKG_FLAGS="${PKG_FLAGS} -I${SHELL_PWD}"
PKG_LIBS="${PKG_LIBS} -lcwutil"
PKG_LIBS="${PKG_LIBS} -L${BUILD_PATH}/ -Wl,-rpath-link=${BUILD_PATH}/"

#
echo "Name: libcwutil" > ${LIBCWUTIL_PC}
echo "Description: HOST_PLATFORM=${HOST_PLATFORM} TARGET_PLATFORM=${TARGET_PLATFORM}" >> ${LIBCWUTIL_PC}
echo "Version: ${DATE_TIME}" >> ${LIBCWUTIL_PC}
echo "Cflags: ${PKG_FLAGS}" >> ${LIBCWUTIL_PC}
echo "Libs: ${PKG_LIBS}" >> ${LIBCWUTIL_PC}