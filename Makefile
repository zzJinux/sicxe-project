CC=gcc
CFLAGS=-Wall -I.
OBJS=20131604.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

20131604.out: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o 20131604.out
