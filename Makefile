CC=gcc
CFLAGS=-Wall -I. -Iexecution
OBJS=20131604.o execution/execution.o

%.o: %.c
	$(CC) -c -g -o $@ $< $(CFLAGS)

20131604.out: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o **/*.o 20131604.out
