CC = gcc
LD = gcc

CFLAGS  = -O2 -ggdb -Wall -ansi -pedantic
LDFLAGS =

rm      = rm


TARGET = calc



all: $(TARGET)

clean:
	$(rm) -f *~ *.o $(TARGET)

.c.o: .c.o
	$(CC) $(CFLAGS) -c $<


$(TARGET): calc.o stk.o precedence.o
	$(LD) $(LDFLAGS) -o $@ $+
