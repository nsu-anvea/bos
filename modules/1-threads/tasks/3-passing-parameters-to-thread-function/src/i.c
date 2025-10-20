#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

void *another_thread(void *arg) {
    sleep(1);

    struct some_struct *st = (struct some_struct *) arg;
    st->some_int = 5;
    //*st->some_char_ptr = '$';

    sleep(3);
    return NULL;
}

void *thread_function(void *arg) {
    printf("\n[INFO]: thread have started!\n\n");
    sleep(5);
	struct some_struct *st = (struct some_struct *) arg;

    printf("\t\t[THREAD]: reading from %p\n", arg);
	printf("\t\t[THREAD]: some_int=%d\n", st->some_int);
	printf("\t\t[THREAD]: some_char=%c\n", *st->some_char_ptr);

    fini_struct(st);

    sleep(2);
    printf("\t\t[THREAD]: *AGAIN* some_int=%d\n", st->some_int);

    printf("\n[INFO]: thread have finished!\n\n");
    return NULL;
}

int main() {
    pthread_t thread_id;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	struct some_struct st;
	init_struct(&st, 1, '@');

    printf("\t[MAIN]: struct address=%p\n", &st);
    printf("\t[MAIN]: some_int=%d\n", st.some_int);

    int result = pthread_create(&thread_id, &attr, thread_function, (void *) &st);
    if (result != 0) {
        printf("\n[ERROR]: creating thread\n\n");
        return 1;
    }
    printf("\t[MAIN]: I have created a thread\n");

    pthread_t another_id;
    result = pthread_create(&another_id, NULL, another_thread, (void *) &st);
    if (result != 0) {
        printf("\n[ERROR]: creating thread\n\n");
        return 1;
    }
    pthread_detach(another_id);

    pthread_exit(NULL);

    return 0;
}
