CFLAGS = -Wall -Werror $(INCLUDE_PARAMS)
C_DEBUG_FLAGS =
INCLUDES := . shell-context execute-cmd
INCLUDE_PARAMS := $(foreach d,$(INCLUDES),-I$d)
SRCS := $(foreach d,$(INCLUDES),$(wildcard $d/*.c))
OBJS := $(patsubst %.c, %.o, $(SRCS))

.PHONY: debug
debug: C_DEBUG_FLAGS+=-g
debug: build

.PHONY: release
release: build

.PHONY: build
build: 20131604.out

20131604.out: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c $(C_DEBUG_FLAGS) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o **/*.o 20131604.out
