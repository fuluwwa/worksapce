CC = cc

INCLUDE = -I.
LIB=-lpthread

OBJ += $(patsubst %.c, %.o, $(wildcard *.c))

CFLAGS = -O0 -g3 -Wall
target = EasyLoggerLinuxDemo

all:$(OBJ)
	$(CC) *.o -o $(target) $(LIB)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)
clean:
	rm -rf *.o $(target) 
