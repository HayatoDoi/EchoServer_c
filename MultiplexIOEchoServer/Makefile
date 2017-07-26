#Makefile

EXEC = MultiplexIOEchoServer
# Add source code
SRCS = MultiplexIOEchoServer.c

OBJECTS = $(SRCS:.c=.o)
CC = gcc
CFLAG = -Wall

revproxy: ${OBJECTS}
	${CC} -o ${EXEC} ${OBJECTS} ${CFLAG}

%.o : %.c Makefile
	${CC} -c $< -o $@ ${CFLAG}

clean : 
	rm ${OBJECTS} ${EXEC}

