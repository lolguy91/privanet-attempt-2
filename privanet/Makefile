C_FILES := $(wildcard *.c)
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
	@ld $(OBJ) -o $(BIN_DIR)/privanet

$(BUILD_DIR)/privanet/%.o: %.c
	@mkdir -p $(@D)
	@gcc $(CFLAGS) -c $< -o $@
