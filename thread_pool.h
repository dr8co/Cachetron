#pragma once

#include <pthread.h>
#include "data_structures/deque_c.h"
#include "data_structures/vector_c.h"

struct Worker {
    void (*function)(void *); ///< The function to execute.
    void *arg; ///< The argument to pass to the function.
};

typedef struct Worker Worker;

struct ThreadPool {
    ptr_vector *threads; ///< A vector of threads.
    Deque *queue; ///< A deque to hold tasks.
    pthread_mutex_t mutex; ///< A mutex to protect the queue.
    pthread_cond_t condition; ///< A condition variable to signal the threads.
};

typedef struct ThreadPool ThreadPool;

bool thread_pool_init(ThreadPool *pool, size_t num_threads);

void thread_pool_destroy(ThreadPool *pool);

bool thread_pool_push(ThreadPool *pool, void (*function)(void *), void *arg);

void thread_pool_queue(ThreadPool *pool, void (*function)(void *), void *arg);
