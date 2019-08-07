CC=gcc
RM=rm
MV=mv
CD=cd
CP=cp
AR=ar
STRIP=strip

ifeq (,$(ROOT_DIR))
ROOT_DIR =$(shell pwd)
endif

BUILD_DIR=$(ROOT_DIR)/build
all: release debug

release:
	@make -C libx clean_obj BUILD_DIR=$(BUILD_DIR)
	@make -C comm clean_obj BUILD_DIR=$(BUILD_DIR)
	@make -C libx clean BUILD_DIR=$(BUILD_DIR)
	@make -C comm clean BUILD_DIR=$(BUILD_DIR)
	@make -C libx all BUILD_DIR=$(BUILD_DIR)
	@make -C comm all BUILD_DIR=$(BUILD_DIR)
debug:
	@make -C libx clean_obj DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
	@make -C comm clean_obj DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
	@make -C libx clean DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
	@make -C comm clean DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
	@make -C libx all DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
	@make -C comm all DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
clean:
	@make -C libx clean BUILD_DIR=$(BUILD_DIR)
	@make -C comm clean BUILD_DIR=$(BUILD_DIR)
	@make -C libx clean DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
	@make -C comm clean DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)

clean_obj:
	@make -C libx clean_obj BUILD_DIR=$(BUILD_DIR)
	@make -C comm clean_obj BUILD_DIR=$(BUILD_DIR)
	@make -C libx clean_obj DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
	@make -C comm clean_obj DEBUG="DEBUG" BUILD_DIR=$(BUILD_DIR)
