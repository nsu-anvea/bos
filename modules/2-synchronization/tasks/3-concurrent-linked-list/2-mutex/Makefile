TARGET = test
SRCS = storage.c test.c

CC = gcc
RM = rm
CFLAGS = -g -Wall -pthread
INCLUDE_DIR = .

all: ${TARGET}

${TARGET}: storage.h ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} -o ${TARGET}

clean:
	${RM} -f *.o ${TARGET}

.PHONY: all clean