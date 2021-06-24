#
# This file is part of ABTK.
#
# MIT License
#
#

#
MAKE_CONF ?= $(abspath $(CURDIR)/build/makefile.conf)

#加载配置项。
include ${MAKE_CONF}

#
SOLUTION_NAME ?= abtk

#
all: abtkutil abtkcomm tools tests

#
abtkutil: abtkutil-clean
	make -C $(CURDIR)/abtkutil/

#
abtkcomm: abtkcomm-clean
	make -C $(CURDIR)/abtkcomm/

#
tools: tools-clean
	make -C $(CURDIR)/tools/

#
tests: tests-clean
	make -C $(CURDIR)/tests/

#
clean: abtkutil-clean abtkcomm-clean tools-clean tests-clean

#
abtkutil-clean: 
	make -C $(CURDIR)/abtkutil/ clean

#
abtkcomm-clean: 
	make -C $(CURDIR)/abtkcomm/ clean

#
tools-clean: 
	make -C $(CURDIR)/tools/ clean

#
tests-clean:
	make -C $(CURDIR)/tests/ clean

#
install: abtkutil-install abtkcomm-install tools-install

#
abtkutil-install: 
	make -C $(CURDIR)/abtkutil/ install

#
abtkcomm-install: 
	make -C $(CURDIR)/abtkcomm/ install

#
tools-install: 
	make -C $(CURDIR)/tools/ install

#
uninstall: abtkutil-uninstall abtkcomm-uninstall tools-uninstall

#
abtkutil-uninstall: 
	make -C $(CURDIR)/abtkutil/ uninstall

#
abtkcomm-uninstall: 
	make -C $(CURDIR)/abtkcomm/ uninstall

#
tools-uninstall: 
	make -C $(CURDIR)/tools/ uninstall


#
ROOT_PATH = /tmp/${SOLUTION_NAME}-build-installer.tmp/
PACK_TAR_NAME=${SOLUTION_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}-${TARGET_PLATFORM}.tar.gz
#
pack: pack-tar

#
pack-tar: 
#
	make -C $(CURDIR)
	make -C $(CURDIR) install ROOT_PATH=${ROOT_PATH}
	tar -czv -f "${BUILD_PATH}/${PACK_TAR_NAME}" -C "${ROOT_PATH}/${INSTALL_PREFIX}/../" "abtk"
	make -C $(CURDIR) uninstall ROOT_PATH=${ROOT_PATH}
	make -C $(CURDIR) clean
#
	@echo "\n"
	@echo "${BUILD_PATH}/${PACK_TAR_NAME}"

