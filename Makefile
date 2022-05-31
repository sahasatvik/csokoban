CC=gcc
CFLAGS=-Wall
MAIN=csokoban.c
TARGET=csokoban
OBJS=maps.o \
     board.o \
     screen.o

all : $(TARGET)

$(TARGET) : $(MAIN) $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(OBJS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $<

board.o : screen.o

clean :
	rm $(TARGET) $(OBJS)
