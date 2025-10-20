SRC = src/

a: $(SRC)a-struct-reader-thread.c
	gcc $(SRC)a-struct-reader-thread.c -o asrt -lpthread

b: $(SRC)b-detached-thread.c
	gcc $(SRC)b-detached-thread.c -o bdt -lpthread

i: $(SRC)i.c
	gcc $(SRC)i.c -o i -lpthread

ii: $(SRC)ii.c
	gcc $(SRC)ii.c -o ii -lpthread -fsanitize=address
