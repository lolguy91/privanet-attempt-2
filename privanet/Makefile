C_FILES := $(shell find . -type f -name "*.c")
OBJ := $(C_FILES:%.c=$(BUILD_DIR)/privanet/%.o)

CFLAGS := -Wall -Wextra -Wpedantic
ifeq ($(DEBUG),y)
CFLAGS := -O0 -g -D__DEBUG__
else
CFLAGS := -O3
endif

all: $(BIN_DIR)/privanet

$(BIN_DIR)/privanet: $(OBJ)
	@mkdir -p $(@D)
	@clang $(OBJ) -lpthread -o $(BIN_DIR)/privanet

$(BUILD_DIR)/privanet/%.o: %.c
	@mkdir -p $(@D)
	@clang $(CFLAGS) -c $< -o $@
