#pragma once

#if __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "data_structures/queue/deque_c.h"
#include "data_structures/vector/vector_c.h"

/**
 * @brief A structure representing a worker in the thread pool.
 *
 */
struct Worker {
    void (*function)(void *); ///< The function to execute.
    void *arg;                ///< The argument to pass to the function.
};

typedef struct Worker Worker;

/**
 * @brief A structure representing a thread pool.
 */
struct ThreadPool {
    pthread_cond_t condition; ///< A condition variable to signal the threads.
    pthread_mutex_t mutex;    ///< A mutex to protect the queue.
    ptr_vector *threads;      ///< A vector of threads.
    Deque *queue;             ///< A deque to hold tasks.
    bool stop;                ///< A flag to signal the threads to stop.
};

typedef struct ThreadPool ThreadPool;

bool thread_pool_init(ThreadPool *pool, size_t num_threads);

void thread_pool_destroy(ThreadPool *pool);

bool thread_pool_push(ThreadPool *pool, void (*function)(void *), void *arg);

void thread_pool_queue(ThreadPool *pool, void (*function)(void *), void *arg);

#if __cplusplus
}
#endif
