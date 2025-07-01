#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdbool.h>


typedef struct Task {
    void *(*function)(void *);
    void *arg;
    struct Task *next;
} Task;

typedef struct ThreadPool {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    Task *task_head;
    Task *task_tail;
    pthread_t *threads;
    int thread_count;
    int task_queue_size;
    bool shutdown;
} ThreadPool;

int threadpool_init(ThreadPool *pool, int num_threads);
int threadpool_add_task(ThreadPool *pool, void *(*function)(void *), void *arg);
void threadpool_destroy(ThreadPool *pool);

#endif //THREADPOOL_H
