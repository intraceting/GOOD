#
MAKEFILE_FILE := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_PWD := $(dir $(MAKEFILE_FILE))

#
all: libutil tests

#
libutil: libutil-clean
	make -C $(MAKEFILE_PWD)/libutil/

#
tests: tests-clean
	make -C $(MAKEFILE_PWD)/tests/

#
clean: libutil-clean tests-clean

#
libutil-clean: 
	make -C $(MAKEFILE_PWD)/libutil/ clean

#
tests-clean:
	make -C $(MAKEFILE_PWD)/tests/ clean

#
install: libutil-install

#
libutil-install: 
	make -C $(MAKEFILE_PWD)/libutil/ install

#
uninstall: libutil-uninstall

#
libutil-uninstall: 
	make -C $(MAKEFILE_PWD)/libutil/ uninstall

#
help: libutil-help

#
libutil-help:
	make -C $(MAKEFILE_PWD)/libutil/ help
