#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct some_struct {
    int some_int;
    char *some_char_ptr;
};

void init_struct(struct some_struct *st, int i, char c) {
	st->some_int = i;
	st->some_char_ptr = (char *) malloc(sizeof(char));
	*st->some_char_ptr = c;
}

void fini_struct(struct some_struct *st) {
	free(st->some_char_ptr);
}

void *thread_function(void *arg) {
    printf("[INFO]: thread have started!\n");
	struct some_struct *st = (struct some_struct *) arg;

	printf("\t[THREAD]: some_int=%d\n", st->some_int);
	printf("\t[THREAD]: some_char=%c\n", *st->some_char_ptr);

    printf("[INFO]: thread have finished!\n");
    return NULL;
}

int main() {
    pthread_t thread_id;

	struct some_struct st;
	init_struct(&st, 1, '@');

    int result = pthread_create(&thread_id, NULL, thread_function, (void *) &st);
    if (result != 0) {
        printf("[ERROR]: creating thread\n");
        return 1;
    }

    pthread_join(thread_id, NULL);

    return 0;
}
