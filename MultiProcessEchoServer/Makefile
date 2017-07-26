#Makefile

EXEC = MultiProcessEchoServer
# Add source code
SRCS = MultiProcessEchoServer.c

OBJECTS = $(SRCS:.c=.o)
CC = gcc
CFLAG = -Wall

revproxy: ${OBJECTS}
	${CC} -o ${EXEC} ${OBJECTS} ${CFLAG}

%.o : %.c Makefile
	${CC} -c $< -o $@ ${CFLAG}

clean : 
	rm ${OBJECTS} ${EXEC}

