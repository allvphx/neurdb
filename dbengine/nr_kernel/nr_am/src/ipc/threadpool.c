#include "threadpool.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void *threadpool_worker(void *arg);


int threadpool_init(ThreadPool *pool, int num_threads) {
    memset(pool, 0, sizeof(ThreadPool));
    pool->thread_count = num_threads;
    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    pool->task_head = NULL;
    pool->task_tail = NULL;
    pool->task_queue_size = 0;
    pool->shutdown = false;

    if (pthread_mutex_init(&pool->lock, NULL) != 0 ||
        pthread_cond_init(&pool->notify, NULL) != 0) {
        free(pool->threads);
        return -1;
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&pool->threads[i], NULL, threadpool_worker, pool) !=
            0) {
            return -1;
        }
    }
    return 0;
}

int threadpool_add_task(ThreadPool *pool, void *(*function)(void *),
                        void *arg) {
    Task *task = malloc(sizeof(Task));
    task->function = function;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&pool->lock);
    if (pool->shutdown) {
        free(task);
        pthread_mutex_unlock(&pool->lock);
        return -1;
    }

    if (pool->task_tail) {
        pool->task_tail->next = task;
    } else {
        pool->task_head = task;
    }
    pool->task_tail = task;
    pool->task_queue_size++;
    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);
    return 0;
}

void threadpool_destroy(ThreadPool *pool) {
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    free(pool->threads);

    while (pool->task_head) {
        Task *tmp = pool->task_head;
        pool->task_head = pool->task_head->next;
        free(tmp);
    }
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
}

static void *threadpool_worker(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;

    while (true) {
        pthread_mutex_lock(&pool->lock);

        while (!pool->shutdown && pool->task_queue_size == 0)
            pthread_cond_wait(&pool->notify, &pool->lock);

        if (pool->shutdown && pool->task_queue_size == 0) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        Task *task = pool->task_head;
        if (task) {
            pool->task_head = task->next;
            if (!pool->task_head) {
                pool->task_tail = NULL;
            }
            pool->task_queue_size--;
        }

        pthread_mutex_unlock(&pool->lock);

        if (task) {
            task->function(task->arg);
            free(task);
        }
    }
    return NULL;
}
