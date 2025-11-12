CC = gcc
CFLAGS = -Wall -g -s
SRCS = a7.c avl.c
OBJS = $(SRCS:.c=.o)
TARGET = a7
	
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)