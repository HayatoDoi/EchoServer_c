#Makefile

EXEC = MulthiThreadEchoServer

# Add source code
SRCS = MulthiThreadEchoServer.c

OBJECTS = $(SRCS:.c=.o)
CC = gcc
CFLAG = -Wall -lpthread

revproxy: ${OBJECTS}
	${CC} -o ${EXEC} ${OBJECTS} ${CFLAG}

%.o : %.c Makefile
	${CC} -c $< -o $@ ${CFLAG}

clean : 
	rm ${OBJECTS} ${EXEC}

