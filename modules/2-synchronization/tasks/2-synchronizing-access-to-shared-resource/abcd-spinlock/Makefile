CC=gcc
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."
BUILD_DIR=build

TARGET_1 = ${BUILD_DIR}/queue-example
SRCS_1 = queue.c queue-example.c

TARGET_2 = ${BUILD_DIR}/queue-threads
SRCS_2 = queue.c queue-threads.c


all: ${TARGET_1} ${TARGET_2}

${TARGET_1}: queue.h ${SRCS_1} build
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

${TARGET_2}: queue.h ${SRCS_2} build
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_2} ${LIBS} -o ${TARGET_2}

test1: ${TARGET_1}
	./$<

test2: ${TARGET_2}
	./$<

build:
	mkdir -p $@

clean:
	rm -rf build