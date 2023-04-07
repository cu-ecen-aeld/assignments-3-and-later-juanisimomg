#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    /*
     * DONE: wait, obtain mutex, wait, release mutex as described by thread_data structure
     * hint: use a cast like the one below to obtain thread arguments from your parameter
     * struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    */
    // Extract input parameters.
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    // Wait for the input-provided ms before locking.
    DEBUG_LOG("Waiting before obtaining mutex...");
    usleep(thread_func_args->wait_to_obtain_ms * 1000);
    // Obtain mutex.
    DEBUG_LOG("Obtaining mutex...");
    int mutex_lock_status = pthread_mutex_lock(thread_func_args->mutex);
    if (mutex_lock_status != 0){
        // Mutex lock failed.
        DEBUG_LOG("Mutex lock failed.");
        thread_func_args->thread_complete_success = false;
    } else {
        // Wait for the input-provided ms before release.
        DEBUG_LOG("Waiting before releasing mutex...");
        usleep(thread_func_args->wait_to_release_ms * 1000);
        // Release mutex.
        DEBUG_LOG("Releasing mutex...");
        int mutex_unlock_status = pthread_mutex_unlock(thread_func_args->mutex);
        if (mutex_unlock_status != 0){
            // Mutex unlock failed.
            DEBUG_LOG("Mutex unlock failed.");
            thread_func_args->thread_complete_success = false;
        }
    }
    // Report successful completion of thread.
    thread_func_args->thread_complete_success = true;
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    /*
     * DONE: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
    */

    // Start by creating a thread_data variable, which depends on the types of its fields.
    struct thread_data *data_input = malloc(sizeof *data_input);
    // Initialize the fields in thread_data.
    data_input->mutex = mutex;
    data_input->wait_to_obtain_ms = wait_to_obtain_ms;
    data_input->wait_to_release_ms = wait_to_release_ms;
    data_input->thread_complete_success = false;
    // Create thread with thread_data as sole input.
    int thread_status = pthread_create(thread, NULL, threadfunc, data_input);
    // Check for errors creating thread.
    if (thread_status != 0){
        // Thread creation failed.
        DEBUG_LOG("Thread creation failed.");
        return false;
    }
    // Thread creation succeded.
    DEBUG_LOG("Thread creation succeded.");
    return true;
}

