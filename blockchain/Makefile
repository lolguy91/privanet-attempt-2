C_FILES := $(wildcard *.c)
OBJ := $(C_FILES:%.c=$(BUILD_DIR)/blockchain/%.o)

CFLAGS := -Wall -Wextra -Wpedantic
ifeq ($(DEBUG),y)
CFLAGS := -O0 -g -D__DEBUG__
else
CFLAGS := -O3
endif

all: $(BIN_DIR)/blockchain.a

$(BIN_DIR)/blockchain.a: $(OBJ)
	@mkdir -p $(@D)
	@ar -r $(BIN_DIR)/blockchain.a $(OBJ)

$(BUILD_DIR)/blockchain/%.o: %.c
	@mkdir -p $(@D)
	@clang $(CFLAGS) -c $< -o $@
