# Export default variables
export BUILD_DIR := $(PWD)/build
export BIN_DIR := $(PWD)/bin

.PHONY: all
all: privanet blockchain

.PHONY: rebuild
rebuild: clean all

.PHONY: blockchain
blockchain:
	@$(MAKE) -C blockchain

.PHONY: privanet
privanet:
	@$(MAKE) -C privanet

.PHONY: clean
clean:
	@rm -rf build bin