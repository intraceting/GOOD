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
all: goodutil goodswitch goodmtx tests

#
goodutil: goodutil-clean
	make -C $(CURDIR)/goodutil/

#
goodswitch: goodswitch-clean
	make -C $(CURDIR)/goodswitch/

#
goodmtx: goodmtx-clean
	make -C $(CURDIR)/goodmtx/

#
tests: tests-clean
	make -C $(CURDIR)/tests/

#
clean: goodutil-clean goodswitch-clean goodmtx-clean tests-clean

#
goodutil-clean: 
	make -C $(CURDIR)/goodutil/ clean

#
goodswitch-clean: 
	make -C $(CURDIR)/goodswitch/ clean

#
goodmtx-clean: 
	make -C $(CURDIR)/goodmtx/ clean

#
tests-clean:
	make -C $(CURDIR)/tests/ clean

#
install: goodutil-install goodswitch-install goodmtx-install

#
goodutil-install: 
	make -C $(CURDIR)/goodutil/ install

#
goodswitch-install: 
	make -C $(CURDIR)/goodswitch/ install

#
goodmtx-install: 
	make -C $(CURDIR)/goodmtx/ install

#
uninstall: goodutil-uninstall goodswitch-uninstall goodmtx-uninstall

#
goodutil-uninstall: 
	make -C $(CURDIR)/goodutil/ uninstall

#
goodswitch-uninstall: 
	make -C $(CURDIR)/goodswitch/ uninstall

#
goodmtx-uninstall: 
	make -C $(CURDIR)/goodmtx/ uninstall



