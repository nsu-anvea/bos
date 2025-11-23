#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

// обработчик сигналов можно установить только для процесса, но не для каждого потока по отдельности
void handler(int sig) {
	pthread_t tid = pthread_self();
	printf("\n[HANDLER][THREAD=%lu]: Получен сигнал SIGINT (%d)\n\n", tid, sig);
}

void *thread_func1(void *arg) {
	pthread_t tid = pthread_self();

	while (1) {
		printf("\t[THREAD=1]: I am working...\n");
		sleep(2);
	}
	return NULL;
}

void *thread_func2(void *arg) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
	printf("\t[THREAD=2]: SIGINT разблокирован, жду сигналы...");

	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGQUIT);
	pthread_sigmask(SIG_BLOCK, &block_set, NULL); // для ясности, а так SIGQUIT уже заблокирован (унаследованно из main)

	int i = 0;

	while (1) {
		printf("\t[THREAD=2]: работаю и жду сигнал SIGINT\n");
		if (i % 2 == 0) {
			pthread_sigmask(SIG_BLOCK, &set, NULL);
			printf("\t[THREAD=2]: заблокировал сигнал SIGINT\n");
		}
		else {
			pthread_sigmask(SIG_UNBLOCK, &set, NULL);
			printf("\t[THREAD=2]: разблокировал сигнал SIGINT\n");
		}
		sleep(3);
		i++;
	}
	return NULL;
}

void *thread_func3(void *arg) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);

	sleep(10);
	printf("\t\t[THREAD=3]: проснулся\n");
	
	while (1) {
		int sig;
		sigwait(&set, &sig);
		printf("\t\t[THREAD=3]: Получил сигнал %d (SIGQUIT)\n", sig);
	}
	return NULL;
}

void *thread_func4(void *arg) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);

	while (1) {
		int sig;
		sigwait(&set, &sig);
		printf("\t\t[THREAD=4]: Получил сигнал %d (SIGQUIT)\n", sig);
		printf("\t\t[THREAD=4]: Засыпает...\n");
		sleep(30);
		printf("\t\t[THREAD=4]: Просыпается...\n");
		
	}
	return NULL;
}

int main() {
	printf("\n[INFO]: PID=%d\n\n", getpid());

	sigset_t set;

	sigfillset(&set);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	signal(SIGINT, handler);

	pthread_t tids[4];
	void *(*thread_funcs[4])(void *) = {
		thread_func1,
		thread_func2,
		thread_func3,
		thread_func4
	};

	for (int i = 0; i < 4; i++) {
		if (pthread_create(&tids[i], NULL, thread_funcs[i], NULL) != 0) {
			printf("\n[ERROR]: creating a thread!\n\n");
		}
	}

	for (int i = 0; i < 4; i++) {
		pthread_join(tids[i], NULL);
	}

	return 0;
}
