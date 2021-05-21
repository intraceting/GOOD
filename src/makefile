#
MAKEFILE_FILE := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_PWD := $(dir $(MAKEFILE_FILE))
#

#
all: goodutil tests

#
goodutil: goodutil-clean
	make -C $(MAKEFILE_PWD)/goodutil/

#
tests: tests-clean
	make -C $(MAKEFILE_PWD)/tests/

#
clean: goodutil-clean tests-clean

#
goodutil-clean: 
	make -C $(MAKEFILE_PWD)/goodutil/ clean

#
tests-clean:
	make -C $(MAKEFILE_PWD)/tests/ clean

#
install: goodutil-install

#
goodutil-install: 
	make -C $(MAKEFILE_PWD)/goodutil/ install

#
uninstall: goodutil-uninstall

#
goodutil-uninstall: 
	make -C $(MAKEFILE_PWD)/goodutil/ uninstall

