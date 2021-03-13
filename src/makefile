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

