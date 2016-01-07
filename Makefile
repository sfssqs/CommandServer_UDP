EXEC = listener
OBJS = listener.o
SRC  = listener.c

# CC = arm-none-linux-gnueabi-gcc
# CFLAGS += -O2 -Wall
# LDFLAGS += 

all:$(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -pthread

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -pthread

clean:
	@rm -vf $(EXEC) *.o *~
