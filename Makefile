CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra
# CFLAGS += -fsanitize=address -Og -Werror # extra flags

OBJS = traceroute.o sender.o receiver.o

all: traceroute

traceroute: $(OBJS)
	$(CC) $(CFLAGS) -o traceroute $(OBJS)

traceroute.o: traceroute.c traceroute.h
sender.o: sender.c traceroute.h
receiver.o: receiver.c traceroute.h

clean:
	rm -f *.o

distclean:
	rm -f *.o traceroute

.PHONY: all clean distclean