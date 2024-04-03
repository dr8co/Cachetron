#include "thread_pool.h"
#include <stdlib.h>

static void *worker(void *arg) {
    ThreadPool *pool = arg;
    while (true) {
        pthread_mutex_lock(&pool->mutex);
        while (deque_empty(pool->queue)) {
            pthread_cond_wait(&pool->condition, &pool->mutex);
        }
        const Worker task = *(Worker *) deque_front(pool->queue);
        deque_pop_front(pool->queue);
        pthread_mutex_unlock(&pool->mutex);

        task.function(task.arg);
    }
    return nullptr;
}

bool thread_pool_init(ThreadPool *pool, const size_t num_threads) {
    if (num_threads > 0) {
        pool->threads = ptr_vector_new();
        pool->queue = create_deque();
        if (pthread_mutex_init(&pool->mutex, nullptr) == 0) {
            if (pthread_cond_init(&pool->condition, nullptr) == 0) {
                for (size_t i = 0; i < num_threads; ++i) {
                    pthread_t *thread = malloc(sizeof(pthread_t));
                    if (thread && pthread_create(thread, nullptr, &worker, pool) != 0) {
                        free(thread);
                        return false;
                    }
                    ptr_vector_push_back(pool->threads, thread);
                }
                return true;
            }
        }
    }
    return false;
}

void thread_pool_destroy(ThreadPool *pool) {
    for (size_t i = 0; i < ptr_vector_size(pool->threads); ++i) {
        pthread_t *thread = ptr_vector_at(pool->threads, i);
        pthread_cancel(*thread);
        free(thread);
    }
    ptr_vector_free(pool->threads);
    destroy_deque(pool->queue);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->condition);
}

bool thread_pool_push(ThreadPool *pool, void (*function)(void *), void *arg) {
    Worker task = {function, arg};
    pthread_mutex_lock(&pool->mutex);
    const bool success = deque_push_back(pool->queue, &task);
    pthread_mutex_unlock(&pool->mutex);
    if (success) {
        pthread_cond_signal(&pool->condition);
    }
    return success;
}

void thread_pool_queue(ThreadPool *pool, void (*function)(void *), void *arg) {
    Worker task = {function, arg};

    pthread_mutex_lock(&pool->mutex);
    deque_push_back(pool->queue, &task);
    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_signal(&pool->condition);
}
