#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void *give_42(void *arg) {
	printf("\t\t[THREAD=%lu] start its work!\n", pthread_self());

    for (int i = 0; i < 3; i++) {
		printf("\t\t[THREAD=%lu] on %d iteration.\n", pthread_self(), i);
        sleep(1);
    }

	printf("\t\t[THREAD=%lu] finish its work!\n", pthread_self());

	int *num = (int *)malloc(sizeof(int));
	*num = 42;
	
	return num;
}

void *give_str(void *arg) {
	printf("\t\t[THREAD=%lu] start its work!\n", pthread_self());

    for (int i = 0; i < 3; i++) {
		printf("\t\t[THREAD=%lu] on %d iteration.\n", pthread_self(), i);
        sleep(1);
    }

	printf("\t\t[THREAD=%lu] finish its work!\n", pthread_self());
	
	char message[] = "Hello, World!";
	char *str = (char *)malloc(sizeof(char) * (strlen(message) + 1));
	strcpy(str, message);

	return (void *)str;
}

int main() {
    pthread_t tid1, tid2;
    
	printf("\t[MAIN T] have started its work!\n");
    
    if (pthread_create(&tid1, NULL, give_42, NULL)) {
        perror("Error: creating thread\n");
        return EXIT_FAILURE;
    }
	printf("[INFO] created thread %lu\n", tid1);
	
	if (pthread_create(&tid2, NULL, give_str, NULL)) {
		perror("Error: creating thread\n");
		return EXIT_FAILURE;
	}
	printf("[INFO] created thread %lu\n", tid2);
    
	int  *thread_gived_num;
	char *thread_gived_str;
    printf("\t[MAIN T] waiting for joining threads\n");
    if (pthread_join(tid1, (void**)&thread_gived_num)) {
        perror("[ERROR] join thread\n");
        return EXIT_FAILURE;
    }
    if (pthread_join(tid2, (void**)&thread_gived_str)) {
        perror("[ERROR] join thread\n");
        return EXIT_FAILURE;
    }

    printf("\t\t[THREAD=%lu] give to Main Thread: %d\n", tid1, *thread_gived_num);
	printf("\t\t[THREAD=%lu] give to Main Thread: %s\n", tid2, thread_gived_str);
	free(thread_gived_num);
	free(thread_gived_str);
    
	printf("[MAIN T] finish its work!\n");

    return EXIT_SUCCESS;
}
