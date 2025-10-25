#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>


void signal_handler(int sig) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
}


void *thread_function(void *arg) {
	int *i = (int *) arg;
	while (1) {
		(*i)++;
	}
	return NULL;
}


int main() {
	pthread_t thread_id;

	int i = 0;

	signal(SIGUSR1, signal_handler);
	
	int result = pthread_create(&thread_id, NULL, thread_function, (void *) &i);
	if (result != 0) {
		printf("\n[ERROR]: creating thread\n\n");
		return 1;
	}

	sleep(1);
	
	pthread_cancel(thread_id);
	pthread_kill(thread_id, SIGUSR1);

	void *ret_value;
	pthread_join(thread_id, &ret_value);

	if (ret_value == PTHREAD_CANCELED) {
		printf("\n[INFO]: thread was cancelled\n\n");
	}

	printf("\t[MAIN]: i=%d\n", i);
	
	return 0;
}
