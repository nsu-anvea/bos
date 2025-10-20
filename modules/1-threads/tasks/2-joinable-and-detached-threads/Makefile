SRC = src/

all: abc d e f

abc: $(SRC)abc-joinable-thread-returns-num-and-str.c
	gcc $(SRC)abc-joinable-thread-returns-num-and-str.c -o abc-jtrnas -lpthread

d: $(SRC)d-joinable-or-detached-threads-in-while.c
	gcc $(SRC)d-joinable-or-detached-threads-in-while.c -o d-jodtiw -lpthread

e: $(SRC)e-pthread-detach.c
	gcc $(SRC)e-pthread-detach.c -o e-pd -lpthread

f: $(SRC)f-detached-thread-using-attr.c
	gcc $(SRC)f-detached-thread-using-attr.c -o f-dtua -lpthread
