TARGET=ls
LIBS=
CC=cc
CFLAGS= -pedantic -std=c99

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean: 
	-rm -f *.o
	-rm -f *.tmp
	-rm -f $(TARGET)
