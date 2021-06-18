#
# This file is part of GOOD.
#
# MIT License
#
#

#
MAKE_CONF ?= $(abspath $(CURDIR)/build/makefile.conf)

#加载配置项。
include ${MAKE_CONF}

#
all: goodutil goodswitch goodtools tests

#
goodutil: goodutil-clean
	make -C $(CURDIR)/goodutil/

#
goodswitch: goodswitch-clean
	make -C $(CURDIR)/goodswitch/

#
goodtools: goodtools-clean
	make -C $(CURDIR)/goodtools/

#
tests: tests-clean
	make -C $(CURDIR)/tests/

#
clean: goodutil-clean goodswitch-clean goodtools-clean tests-clean

#
goodutil-clean: 
	make -C $(CURDIR)/goodutil/ clean

#
goodswitch-clean: 
	make -C $(CURDIR)/goodswitch/ clean

#
goodtools-clean: 
	make -C $(CURDIR)/goodtools/ clean

#
tests-clean:
	make -C $(CURDIR)/tests/ clean

#
install: goodutil-install goodswitch-install goodtools-install

#
goodutil-install: 
	make -C $(CURDIR)/goodutil/ install

#
goodswitch-install: 
	make -C $(CURDIR)/goodswitch/ install

#
goodtools-install: 
	make -C $(CURDIR)/goodtools/ install

#
uninstall: goodutil-uninstall goodswitch-uninstall goodtools-uninstall

#
goodutil-uninstall: 
	make -C $(CURDIR)/goodutil/ uninstall

#
goodswitch-uninstall: 
	make -C $(CURDIR)/goodswitch/ uninstall

#
goodtools-uninstall: 
	make -C $(CURDIR)/goodtools/ uninstall

#
pack: pack-tar

#
ROOT_PATH=/tmp/good-build-installer.tmp/

#
pack-tar: all
	make -C $(CURDIR) install ROOT_PATH=${ROOT_PATH}
	tar -czv -f "${BUILD_PATH}/good-${VERSION_MAJOR}.${VERSION_MINOR}-${TARGET_PLATFORM}.tar.gz" -C "${ROOT_PATH}/${INSTALL_PREFIX}/../" "good"
	make -C $(CURDIR) uninstall ROOT_PATH=${ROOT_PATH}
