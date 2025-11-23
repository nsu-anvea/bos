#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <stddef.h>

/* Непрозрачная структура стека - детали скрыты от пользователя */
typedef struct mystack_t mystack_t;

/* Структура потока - доступна пользователю */
typedef struct mythread_t {
    int         pid;      /* PID клонированного процесса */
    mystack_t * stack;    /* Указатель на стек */
    void *      retv;     /* Возвращаемое значение потока */
} mythread_t;

/* 
 * Создаёт новый поток
 * 
 * Параметры:
 *   thread        - указатель на структуру mythread_t (будет заполнена)
 *   start_routine - функция, которую выполнит поток
 *   arg           - аргумент для функции потока
 * 
 * Возвращает:
 *   0 при успехе
 *   -1 при ошибке (errno будет установлен)
 */
int mythread_create(mythread_t *thread, 
                    void *(*start_routine)(void *), 
                    void *arg);

/* 
 * Ожидает завершения потока
 * 
 * Параметры:
 *   thread - указатель на структуру потока
 *   retv   - указатель для сохранения возвращаемого значения (может быть NULL)
 * 
 * Возвращает:
 *   0 при успехе
 *   -1 при ошибке
 */
int mythread_join(mythread_t *thread, void **retv);

#endif /* MYTHREAD_H */