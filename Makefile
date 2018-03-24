CC = gcc
CFLAGS = -Wall -Werror $(INCLUDE_PARAMS)
INCLUDES := . shell-context execute-cmd
INCLUDE_PARAMS := $(foreach d,$(INCLUDES),-I$d)
SRCS := $(foreach d,$(INCLUDES),$(wildcard $d/*.c))
OBJS := $(patsubst %.c, %.o, $(SRCS))

20131604.out: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

20131604.o: 20131604.c
	$(CC) -c -g -o $@ $< $(CFLAGS)

%.o: %.c
	$(CC) -c -g -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o **/*.o 20131604.out
