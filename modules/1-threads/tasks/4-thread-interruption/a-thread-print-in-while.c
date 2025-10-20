#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *thread_function(void *arg) {
	while (1) {
		printf("\t[THREAD]: hello!\n"); // print - это точка отмены, поэтому цикл работает 1 секунду, после чего поток отменяется
	}
	return NULL;
}

int main() {
	pthread_t thread_id;

	int result = pthread_create(&thread_id, NULL, thread_function, NULL);
	if (result != 0) {
		printf("[ERROR]: creating thread\n");
		return 1;
	}

	sleep(1);
	
	pthread_cancel(thread_id);

	pthread_join(thread_id, NULL);
	
	return 0;
}
