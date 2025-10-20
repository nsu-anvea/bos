SRC = src/

all: a b i ii

a: $(SRC)a-struct-reader-thread.c
	gcc $(SRC)a-struct-reader-thread.c -o a-srt -lpthread

b: $(SRC)b-detached-thread.c
	gcc $(SRC)b-detached-thread.c -o b-dt -lpthread

i: $(SRC)i.c
	gcc $(SRC)i.c -o i -lpthread

ii: $(SRC)ii.c
	gcc $(SRC)ii.c -o ii -lpthread -fsanitize=address

# -fsanitize=address show if there is leak memory
