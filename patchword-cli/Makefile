CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99

OBJS = patchword.o dictionary.o util.o patchword_starter.o

patchword: $(OBJS)
	$(CC) $(CFLAGS) -o patchword $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o patchword
