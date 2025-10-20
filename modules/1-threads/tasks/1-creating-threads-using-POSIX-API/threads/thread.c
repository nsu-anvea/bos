#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global_var = 100; //
const int global_const = 200;
static int global_static = 300; 

void *mythread(void *arg) {
	int thread_num = *(int *)arg;
	pthread_t self_tid = pthread_self(); // тип POSIX для идентификации потоков в рамках процесса; просто функция для получения id процесса в POSIX
	pid_t system_tid = gettid();		 // системный идентификатор потока

	int local_var = 400 + thread_num; // у каждого потока свой стек, поэтому локальные переменные и локальные константы хранятся там
    const int local_const = 500 + thread_num; 
    static int local_static = 600; // статические переменные размещаются в общем сегменте данных процесса; располагаются в сегменте данных data/bss
	
	printf("mythread %d:\n", thread_num);
	printf("\t[PID=%d, PPID=%d, TID=%d]\n", getpid(), getppid(), system_tid);
	printf("\tpthread_self() = %lu\n", self_tid);
	printf("\tgettid()       = %d\n", system_tid);

	// Печатаем адреса переменных
    printf("\n\t=== АДРЕСА ПЕРЕМЕННЫХ ===\n");
    
    // Локальные переменные (в стеке)
    printf("\tЛокальная переменная:      %p\n", (void*)&local_var);
    printf("\tЛокальная константа:       %p\n", (void*)&local_const);
    
    // Статические переменные (в data/bss сегменте)
    printf("\tЛокальная статическая:     %p\n", (void*)&local_static);
    printf("\tГлобальная статическая:    %p\n", (void*)&global_static);
    
    // Глобальные переменные (в data сегменте)
    printf("\tГлобальная переменная:     %p\n", (void*)&global_var);
    printf("\tГлобальная константа:      %p\n", (void*)&global_const);
    
    // Увеличиваем локальную статическую для демонстрации
	global_var += 10;
	global_static += 10;

	local_var += 10;
    local_static += 10;

	printf("\t[t=%d] global_var значение:     %d\n", thread_num, global_var);
    printf("\t[t=%d] global_const значение:     %d\n", thread_num, global_const);
	printf("\t[t=%d] global_static значение:     %d\n", thread_num, global_static);

	printf("\t[t=%d] local_var значение:     %d\n", thread_num, local_var);
	printf("\t[t=%d] local_const значение:     %d\n", thread_num, local_const);
	printf("\t[t=%d] local_static значение:     %d\n", thread_num, local_static);

    
    printf("\n");
	sleep(120);
    return NULL;
}

int main() {
	pthread_t tid[5];
	int err;
	int thread_args[5];

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
	printf("main pthread_self() = %lu\n\n", pthread_self());

	// pthread_equal() нужно использоваться для переносимости программы (
	// 		на linux - это unsigned int или long
	// 		на macOS - это уже другая структура данных
	// )

	for (int i = 0; i < 5; i++) {
		thread_args[i] = i + 1;
		err = pthread_create(&tid[i], NULL, mythread, &thread_args[i]); // возвращает тоже самое, что и pthread_self()
		if (err) {
			printf("main: pthread_create() failed for thread %d: %s\n", i + 1, strerror(err));
			return -1;
		}
		printf("main: created thread %d, pthread_create() returned: %lu\n\n", i + 1, tid[i]);
	}
	printf("\n");

	for (int i = 0; i < 5; i++) {
		pthread_join(tid[i], NULL);
	}

	return 0;
}

// Проблема данного кода в том, что созданный поток не успевает напечатать сообщение
// до того, как основной поток завершит выполнение.

// Чтобы исправить это, добавим pthread_join.
