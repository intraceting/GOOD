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
all: goodutil tests

#
goodutil: goodutil-clean
	make -C $(CURDIR)/goodutil/

#
tests: tests-clean
	make -C $(CURDIR)/tests/

#
clean: goodutil-clean tests-clean

#
goodutil-clean: 
	make -C $(CURDIR)/goodutil/ clean

#
tests-clean:
	make -C $(CURDIR)/tests/ clean

#
install: goodutil-install

#
goodutil-install: 
	make -C $(CURDIR)/goodutil/ install

#
uninstall: goodutil-uninstall

#
goodutil-uninstall: 
	make -C $(CURDIR)/goodutil/ uninstall



