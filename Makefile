CFLAGS = -std=gnu11 -Wall -Werror $(INCLUDE_PARAMS)
C_DEBUG_FLAGS =
SRCS := $(wildcard *.c)
OBJ_DIR = objs
OBJS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: debug
debug: C_DEBUG_FLAGS+=-g
debug: build

.PHONY: release
release: build

.PHONY: build
build: mkdir 20131604.out

.PHONY: mkdir
mkdir:
	mkdir -p $(OBJ_DIR)

20131604.out: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/assemble.o: assemble_pass1.c assemble_pass2.c

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $(C_DEBUG_FLAGS) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) 20131604.out **/*.lst **/*.obj
