CURRENT_DIR := $(shell pwd)
export fas_base := $(CURRENT_DIR)/base
export fas_utils := $(CURRENT_DIR)/utils
export fas_http := $(CURRENT_DIR)/http
export fas_test := $(CURRENT_DIR)/test
export fas_libs    := $(CURRENT_DIR)/lib
export fas_test_bin    := $(CURRENT_DIR)/test_bin


all:
ifneq ($(fas_libs), $(wildcard $(fas_libs)))
	mkdir $(fas_libs)
endif

ifneq ($(fas_test_bin), $(wildcard $(fas_test_bin)))	
	mkdir $(fas_test_bin)
endif
	make -C $(fas_base)
	make -C $(fas_utils)
	make -C $(fas_http)
	make -C $(fas_test)

.PHONY : clean
clean:
	make clean -C $(fas_base)
	make clean -C $(fas_utils)
	make clean -C $(fas_http)
	make clean -C $(fas_test)
	rm $(fas_libs) -rf
	rm $(fas_test_bin) -rf

