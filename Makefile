CURRENT_DIR := $(shell pwd)
export FAS_HEADERS := $(CURRENT_DIR)/include
export FAS_LIBS    := $(CURRENT_DIR)/lib
export TEST_BIN    := $(CURRENT_DIR)/test_bin

all:
	make -C src
	make -C app

.PHONY : clean
clean:
	make clean -C src
	make clean -C app
	rm lib/* -rf
	rm test_bin/* -rf

