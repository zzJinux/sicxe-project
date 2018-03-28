CFLAGS = -Wall -Werror $(INCLUDE_PARAMS)
C_DEBUG_FLAGS =
SRCS := $(wildcard *.c)
OBJ_DIR = objs
OBJS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: debug
debug: C_DEBUG_FLAGS+=-g
debug: build

.PHONY: release
release: build

.PHONY: dir
.PHONY: build
build: mkdir 20131604.out

mkdir:
	mkdir -p $(OBJ_DIR)

20131604.out: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $(C_DEBUG_FLAGS) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) 20131604.out
