#
MAKEFILE_FILE := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_PWD := $(dir $(MAKEFILE_FILE))

all: libutil tests

libutil: libutil-clean
	make -C $(MAKEFILE_PWD)/libutil/

tests: tests-clean
	make -C $(MAKEFILE_PWD)/tests/

clean: libutil-clean tests-clean

libutil-clean: 
	make -C $(MAKEFILE_PWD)/libutil/ clean

tests-clean:
	make -C $(MAKEFILE_PWD)/tests/ clean

install: libutil-install tests-install

libutil-install: 
	make -C $(MAKEFILE_PWD)/libutil/ install

tests-install:
	make -C $(MAKEFILE_PWD)/tests/ install

uninstall: libutil-uninstall tests-uninstall

libutil-uninstall: 
	make -C $(MAKEFILE_PWD)/libutil/ uninstall

tests-uninstall:
	make -C $(MAKEFILE_PWD)/tests/ uninstall

help: libutil-help

libutil-help:
	make -C $(MAKEFILE_PWD)/libutil/ help
