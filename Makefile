CC      = gcc
CFLAGS  = -Wall -Wextra -O2
LDFLAGS = -lm
TARGET  = sim
OBJS    = dispersion.o main.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

dispersion.o: dispersion.c dispersion.h
	$(CC) $(CFLAGS) -c dispersion.c

main.o: main.c dispersion.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
