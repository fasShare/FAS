CURRENT_DIR	:= $(shell pwd)
export FAS_BASE	:= $(CURRENT_DIR)/base
export FAS_UTILS := $(CURRENT_DIR)/utils
export FAS_HTTP := $(CURRENT_DIR)/http
export FAS_TEST := $(CURRENT_DIR)/test
export FAS_LIBS_DIR := $(CURRENT_DIR)/lib
export FAS_TEST_BIN := $(CURRENT_DIR)/test_bin

export STATIC_LIB_CMD := ar rcs
export CFLAG := -std=c++11 -Wall -static
export TEST_CFLAG := -std=c++11 -Wall -static 
export CC := g++

export UTILS_HEADERS := $(wildcard $(FAS_UTILS)/*.h)
export BASE_HEADERS := $(wildcard $(FAS_BASE)/*.h) $(UTILS_HEADERS)
export HTTP_HEADERS := $(wildcard $(FAS_HTTP)/*.h) $(BASE_HEADERS)
export TEST_HEADERS := $(wildcard $(FAS_TEST)/*.h) $(HTTP_HEADERS)

export UTILS_INCLUDE_DIR := -I$(FAS_UTILS)
export BASE_INCLUDE_DIR := -I$(FAS_BASE) -I$(FAS_UTILS)
export HTTP_INCLUDE_DIR := -I$(FAS_BASE) -I$(FAS_UTILS) -I$(FAS_HTTP)
export TEST_INCLUDE_DIR := $(HTTP_INCLUDE_DIR)

export UTILS_LIBS := -lpthread
export BASE_LIBS := -lglog -lpthread -lfasutils -ljsoncpp 
export HTTP_LIBS := -lfas -lglog -lpthread -lfasutils
export TEST_BIN_LIBS := -lfashttp -lfas -lglog -lpthread -lfasutils -ljsoncpp 

all:
	mkdir -p $(FAS_LIBS_DIR)
	mkdir -p $(FAS_TEST_BIN)

	make -C $(FAS_UTILS)
	make -C $(FAS_BASE)
	make -C $(FAS_HTTP)
	make -C $(FAS_TEST)

.PHONY : clean
clean:
	make clean -C $(FAS_BASE)
	make clean -C $(FAS_UTILS)
	make clean -C $(FAS_HTTP)
	make clean -C $(FAS_TEST)
	rm $(FAS_LIBS) -rf
	rm $(FAS_TEST_BIN) -rf
