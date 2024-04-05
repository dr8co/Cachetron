#include "thread_pool.h"
#include <stdlib.h>

/**
 * @brief Worker function for the thread pool.
 *
 * This function is executed by each thread in the thread pool.
 *
 * It continuously checks the task queue for tasks to execute.\n
 * If the task queue is empty and the thread pool is not stopped, it waits for a condition signal.\n
 * If the thread pool is stopped, it exits the thread.\n
 * If there is a task in the queue, it pops the task from the queue, executes the task,
 * and then checks the queue again.
 *
 * @param arg Pointer to the thread pool structure.
 * This is passed to the function when the thread is created.
 * @return This function always returns nullptr.
 */
static void *worker(void *arg) {
    ThreadPool *pool = arg;
    // Enable thread cancellation
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);

    // Worker loop
    while (true) {
        pthread_mutex_lock(&pool->mutex);
        while (deque_empty(pool->queue) && !pool->stop) {
            pthread_cond_wait(&pool->condition, &pool->mutex);
        }
        // Check if the thread needs to stop
        if (pool->stop) {
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(nullptr);
        }
        // Get the task from the queue
        const Worker task = *(Worker *) deque_front(pool->queue);
        deque_pop_front(pool->queue);
        pthread_mutex_unlock(&pool->mutex);

        // Execute the task
        task.function(task.arg);
    }
    return nullptr;
}

/**
 * @brief Initializes the thread pool.
 *
 * This function is used to initialize the thread pool with a specified number of threads.
 *
 * @param pool Pointer to the thread pool structure to be initialized.
 * @param num_threads The number of threads to be created in the thread pool.
 * @return true if the initialization is successful, false otherwise.
 */
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
                pthread_mutex_lock(&pool->mutex);
                pool->stop = false;
                pthread_mutex_unlock(&pool->mutex);

                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Destroys the thread pool.
 *
 * This function is used to stop all threads in the thread pool
 * and free the resources used by the thread pool.
 *
 * @param pool Pointer to the thread pool structure to be destroyed.
 */
void thread_pool_destroy(ThreadPool *pool) {
    pthread_mutex_lock(&pool->mutex);
    pool->stop = true;
    pthread_cond_broadcast(&pool->condition); // Wake up all threads to stop
    pthread_mutex_unlock(&pool->mutex);

    // Join all threads
    for (size_t i = 0; i < ptr_vector_size(pool->threads); ++i) {
        pthread_t *thread = ptr_vector_at(pool->threads, i);
        pthread_join(*thread, nullptr);
        free(thread);
    }
    ptr_vector_free(pool->threads);
    destroy_deque(pool->queue);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->condition);
}

/**
 * @brief Pushes a task to the thread pool.
 *
 * @param pool Pointer to the thread pool structure.
 * @param function Pointer to the function to be executed by the task.
 * @param arg Pointer to the argument to be passed to the function.
 * @return true if the push operation is successful, false otherwise.
 */
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

/**
 * @brief Queues a task in the thread pool.
 *
 * @param pool Pointer to the thread pool structure.
 * @param function Pointer to the function to be executed by the task.
 * @param arg Pointer to the argument to be passed to the function.
 */
void thread_pool_queue(ThreadPool *pool, void (*function)(void *), void *arg) {
    Worker task = {function, arg};
    pthread_mutex_lock(&pool->mutex);
    deque_push_back(pool->queue, &task);
    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_signal(&pool->condition);
}
