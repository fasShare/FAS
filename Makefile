CURRENT_DIR := $(shell pwd)
export FAS_HEADERS := $(CURRENT_DIR)/include
export FAS_LIBS    := $(CURRENT_DIR)/lib
export TEST_BIN    := $(CURRENT_DIR)/test_bin


all:
ifneq ($(FAS_LIBS), $(wildcard $(FAS_LIBS)))
	mkdir $(FAS_LIBS)
endif

ifneq ($(TEST_BIN), $(wildcard $(TEST_BIN)))	
	mkdir $(TEST_BIN)
endif
	make -C src
	make -C app

.PHONY : clean
clean:
	make clean -C src
	make clean -C app
	rm lib/* -rf
	rm test_bin/* -rf

