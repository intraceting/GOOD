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
all: goodutil goodswitch tests

#
goodutil: goodutil-clean
	make -C $(CURDIR)/goodutil/

#
goodswitch: goodswitch-clean
	make -C $(CURDIR)/goodswitch/

#
tests: tests-clean
	make -C $(CURDIR)/tests/

#
clean: goodutil-clean goodswitch-clean tests-clean

#
goodutil-clean: 
	make -C $(CURDIR)/goodutil/ clean

#
goodswitch-clean: 
	make -C $(CURDIR)/goodswitch/ clean

#
tests-clean:
	make -C $(CURDIR)/tests/ clean

#
install: goodutil-install goodswitch-install

#
goodutil-install: 
	make -C $(CURDIR)/goodutil/ install

#
goodswitch-install: 
	make -C $(CURDIR)/goodswitch/ install

#
uninstall: goodutil-uninstall goodswitch-uninstall

#
goodutil-uninstall: 
	make -C $(CURDIR)/goodutil/ uninstall

#
goodswitch-uninstall: 
	make -C $(CURDIR)/goodswitch/ uninstall



