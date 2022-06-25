/**
 * This file implements functionality
 * for recursively determining the file size
 * of a directory or file
 *
 * @file disk_usage.c
 * @author William Sandström
 */
#include "disk_usage.h"

/**
 * Is this dir a dot directory,
 * ie does it match "." or ".."
 */
bool is_dot_dir(char* dir) {
    return (dir[0] != '\0' && dir[0] == '.') && // starts with .
           // string ends, must be '.'
           (dir[1] == '\0' ||
            // or string continues with '..' and ends
            (dir[1] == '.' && dir[2] == '\0'));
}

/**
 * Return the disk usage of a file 
 * Note: This is different from apparent file size
 * 
 * @param path path of directory
 * 
 * @return disk usage in bytes
 */
size_t get_file_disk_usage(char* path, bool* file_is_dir) {
    struct stat st_info;
    if (lstat(path, &st_info) != 0) {
        perror(path);
        return 0;
    }
    *file_is_dir = S_ISDIR(st_info.st_mode);
    return st_info.st_blocks * ST_NBLOCKSIZE;
}

/**
 * Return the disk usage of a file relative to an open dir fd
 * Note: This is different from apparent file size
 * 
 * @param dirfd directory file descriptor
 * @param path relative path of file
 * 
 * @return disk usage in bytes
 */
size_t get_file_disk_usage_fd(int dirfd, char* path, bool* file_is_dir) {
    struct stat st_info;
    if (fstatat(dirfd, path, &st_info, AT_SYMLINK_NOFOLLOW) != 0) {
        perror(path);
        *file_is_dir = false;
        return 0;
    }
    *file_is_dir = S_ISDIR(st_info.st_mode);
    return st_info.st_blocks * ST_NBLOCKSIZE;
}

/**
 * Determine the disk usage of the files in directory recursively
 * Single-threaded solution using only file dirs.
 * 
 * @param path path of directory
 * 
 * @return disk usage in bytes
 */
size_t total_disk_usage_task_st(int dir_fd) {
    char dirent_buffer[DIRENT_BUFFER_SIZE];
    size_t disk_usage_size = 0;
    bool file_is_dir = false;
    long nread;
    do { // Instead of using opendir and readdir, we manually get the directory contents using
        // the getdents syscall, which doesn't perform any unnecessary allocations.
        nread = syscall(SYS_getdents64, dir_fd, dirent_buffer, DIRENT_BUFFER_SIZE);
        if (nread == -1) {
            perror("getdents");
            exit(EXIT_FAILURE);
        }

        for (long bpos = 0; bpos < nread;) {
            ldirent* dir_entry = (ldirent*) (dirent_buffer + bpos);
            if (!is_dot_dir(dir_entry->d_name)) {
                disk_usage_size += get_file_disk_usage_fd(dir_fd, dir_entry->d_name,
                                                          &file_is_dir);
                if (file_is_dir) {
                    int new_dir_fd = openat(dir_fd, dir_entry->d_name,
                                            O_RDONLY | O_DIRECTORY);
                    if (new_dir_fd == -1) {
                        perror(dir_entry->d_name);
                    }
                    else {
                        disk_usage_size += total_disk_usage_task_st(new_dir_fd);
                    }
                }
            }
            bpos += dir_entry->d_reclen;
        }
    } while (nread > 0);

    close(dir_fd);
    // Determine actual filesize, not apparant filesize in st_info.st_size
    return disk_usage_size;
}

// Every entry in the stack is the directory, then the corresponding file tree node. Add
// children once encountered

/**
 * Determine the disk usage of the files in directory
 * If a containing file is a directory, add the path to new_tasks
 * Sum up the time spent in the task
 * 
 * @param path path of directory
 * @param new_tasks stack of new files to be checked
 * @param time_spent sum of time of all tasks run on this thread
 * 
 * @return disk usage in bytes
 */
size_t total_disk_usage_task_time(char* path, Stack* new_tasks, long int* time_spent) {
    size_t disk_usage_size = 0;
    StackEntry stack_entry;

    struct timespec before, after;
    clock_gettime(CLOCK_REALTIME, &before);

    int path_length;
    bool first_dir = true;

    bool file_is_dir = false;
    int dir_fd = open(path, O_RDONLY | O_DIRECTORY);
    if (dir_fd == -1) {
        return 0;
    }
    char* new_path;

    char dirent_buffer[DIRENT_BUFFER_SIZE];
    long nread;
    do {
        nread = syscall(SYS_getdents64, dir_fd, dirent_buffer, DIRENT_BUFFER_SIZE);

        for (long bpos = 0; bpos < nread;) {
            ldirent* dir_entry = (ldirent*) (dirent_buffer + bpos);
            if (!is_dot_dir(dir_entry->d_name)) {
                disk_usage_size += get_file_disk_usage_fd(dir_fd, dir_entry->d_name,
                                                          &file_is_dir);
                if (file_is_dir) {
                    if (first_dir) {
                        // Add path to start of path_buffer
                        path_length = strlen(path);
                        // Reuse path allocation
                        new_path = path;
                        first_dir = false;
                    }
                    else {
                        new_path = malloc(512);
                        memcpy(new_path, path, path_length);
                    }
                    new_path[path_length] = '/';
                    strcpy(new_path + path_length + 1, dir_entry->d_name);
                    stack_entry.path = new_path;
                    stack_push(new_tasks, stack_entry);
                }
            }
            bpos += dir_entry->d_reclen;
        }
    } while (nread > 0);

    close(dir_fd);

    if (first_dir) { // Found no directories, free path
        free(path);
    }

    clock_gettime(CLOCK_REALTIME, &after);

    *time_spent += (after.tv_sec - before.tv_sec) * 1000000000 +
                   (after.tv_nsec - before.tv_nsec);
    // Determine actual filesize, not apparant filesize in st_info.st_size
    return disk_usage_size;
}

/**
 * Determine the disk usage of the files in directory
 * If a containing file is a directory, add the path to new_tasks
 * 
 * @param path path of directory
 * @param new_tasks stack of new files to be checked
 * 
 * @return disk usage in bytes
 */
size_t total_disk_usage_task(char* path, Stack* new_tasks) {
    size_t disk_usage_size = 0;
    StackEntry stack_entry;

    int path_length;
    bool first_dir = true;

    bool file_is_dir = false;
    int dir_fd = open(path, O_RDONLY | O_DIRECTORY);
    if (dir_fd == -1) {
        return 0;
    }
    char* new_path;

    char dirent_buffer[DIRENT_BUFFER_SIZE];
    long nread;
    do { // Instead of using opendir and readdir, we manually get the directory contents using
        // the getdents syscall, which doesn't perform any unnecessary allocations.
        nread = syscall(SYS_getdents64, dir_fd, dirent_buffer, DIRENT_BUFFER_SIZE);

        for (long bpos = 0; bpos < nread;) {
            ldirent* dir_entry = (ldirent*) (dirent_buffer + bpos);
            if (!is_dot_dir(dir_entry->d_name)) {
                disk_usage_size += get_file_disk_usage_fd(dir_fd, dir_entry->d_name,
                                                          &file_is_dir);
                if (file_is_dir) {
                    if (first_dir) {
                        // Add path to start of path_buffer
                        path_length = strlen(path);
                        // Reuse path allocation
                        new_path = path;
                        first_dir = false;
                    }
                    else {
                        new_path = malloc(512);
                        memcpy(new_path, path, path_length);
                    }
                    new_path[path_length] = '/';
                    strcpy(new_path + path_length + 1, dir_entry->d_name);
                    stack_entry.path = new_path;
                    stack_push(new_tasks, stack_entry);
                }
            }
            bpos += dir_entry->d_reclen;
        }
    } while (nread > 0);

    close(dir_fd);

    if (first_dir) { // Found no directories, free path
        free(path);
    }

    // Determine actual filesize, not apparant filesize in st_info.st_size
    return disk_usage_size;
}

/**
 * Thread function which takes disk usage
 * tasks and analyzes the disk usage
 * in a threadsafe manner
 * 
 * @param arg_ptr void pointer to ThreadArgs
 * @return none, but the disk usage found per
 * thread can be found in the ThreadArgs structure
 */
void* run_disk_usage_thread(void* arg_ptr) {
    // Start timing
#ifdef PROFILE_TIME
    struct timespec before, after;
    long elapsed_nsecs;
    clock_gettime(CLOCK_REALTIME, &before);
#endif

    ThreadArgs* thread_args = (ThreadArgs*) arg_ptr;

    Stack new_tasks = stack_new(64);

    // Pop a task. We do this by blocking task_pool with a mutex
    pthread_mutex_lock(thread_args->tasks_mutex);
    while (!(*thread_args->all_threads_complete)) {
        if (stack_is_empty(thread_args->tasks)) {
            // This thread is now idle
            if (*thread_args->idle_threads == (thread_args->thread_count - 1)) {
                // All threads are idle, all tasks are complete
                *(thread_args->all_threads_complete) = true;
                while (*thread_args->idle_threads) {
                    (*thread_args->idle_threads)--;
                    sem_post(thread_args->idle_sem);
                }
            }
            else {
                *thread_args->idle_threads = *thread_args->idle_threads + 1;
                // Thread is now idle, wait until semaphore is posted,
                // which is when there are more tasks
                pthread_mutex_unlock(thread_args->tasks_mutex);
                sem_wait(thread_args->idle_sem);
                pthread_mutex_lock(thread_args->tasks_mutex);
            }
            //pthread_mutex_unlock(thread_args->tasks_mutex);
        }
        else {
            StackEntry task = stack_pop(thread_args->tasks);
            pthread_mutex_unlock(thread_args->tasks_mutex);

#ifdef PROFILE_TIME // Time the threads
            thread_args->total_size_bytes += total_disk_usage_task_time(
                task_path, &new_tasks, &thread_args->time_spent_in_task);

    #ifdef SINGLE_TASK_OPTIMIZATION
            while (new_tasks.size == 1) {
                task_path = stack_pop(&new_tasks);
                thread_args->total_size_bytes += total_disk_usage_task_time(
                    task_path, &new_tasks, &thread_args->time_spent_in_task);
            }
    #endif
#else // No timing
            thread_args->total_size_bytes += total_disk_usage_task(task.path, &new_tasks);
    #ifdef SINGLE_TASK_OPTIMIZATION
            while (new_tasks.size == 1) {
                task = stack_pop(&new_tasks);
                thread_args->total_size_bytes += total_disk_usage_task(task.path,
                                                                       &new_tasks);
            }
    #endif
#endif

            pthread_mutex_lock(thread_args->tasks_mutex);

            int new_task_count = new_tasks.size - 1;
            stack_append(thread_args->tasks, &new_tasks);
            new_tasks.size = 0;
            // Resume the idle semaphore here (if there are enough new tasks)
            while (*thread_args->idle_threads && new_task_count) {
                (*thread_args->idle_threads)--;
                new_task_count--;
                sem_post(thread_args->idle_sem);
            }
        }
    }
    pthread_mutex_unlock(thread_args->tasks_mutex);
    stack_free(&new_tasks);

#ifdef PROFILE_TIME
    // Get end time
    clock_gettime(CLOCK_REALTIME, &after);

    elapsed_nsecs = (after.tv_sec - before.tv_sec) * 1000000000 +
                    (after.tv_nsec - before.tv_nsec);
    printf("Thread took %ld ms, task total task time %ld ms (%.5f %%)\n",
           elapsed_nsecs / 1000000, thread_args->time_spent_in_task / 1000000,
           (thread_args->time_spent_in_task / (double) elapsed_nsecs) * 100);
#endif
    return 0;
}

/**
 * Analyze the total disk usage of
 * the files provided in options
 * using the threadcount in options
 * 
 * @param options options file from cmd args
 */
void disk_usage(Options options) {
#ifdef PROFILE_TIME
    struct timespec before, after;
    long elapsed_nsecs;
    clock_gettime(CLOCK_REALTIME, &before);
#endif
    Stack tasks = stack_new(64);
    pthread_mutex_t tasks_mutex;
    pthread_mutex_t idle_mutex;
    sem_t idle_sem;

    char default_working_dir[512];
    if (getcwd(default_working_dir, 512) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&tasks_mutex, NULL);
    pthread_mutex_init(&idle_mutex, NULL);
    sem_init(&idle_sem, 0, 0);
    char** current_file = options.files;
    while (*current_file != NULL) {
        bool all_threads_complete = false;
        size_t idle_threads = 0;
        //size_t total_size = total_disk_usage(*current_file);

        pthread_t tid[options.thread_count];
        ThreadArgs thread_args[options.thread_count];

        for (size_t i = 0; i < options.thread_count; i++) {
            thread_args[i].tasks = &tasks;
            thread_args[i].tasks_mutex = &tasks_mutex;
            thread_args[i].idle_mutex = &idle_mutex;
            thread_args[i].idle_sem = &idle_sem;
            thread_args[i].all_threads_complete = &all_threads_complete;
            thread_args[i].idle_threads = &idle_threads;
            thread_args[i].thread_count = options.thread_count;
            thread_args[i].total_size_bytes = 0;
            thread_args[i].time_spent_in_task = 0;
        }

        // Handle first file manually
        bool current_file_is_dir = false;
        size_t total_size = get_file_disk_usage(*current_file, &current_file_is_dir);

        if (current_file_is_dir) { // Single file, handle manually
            // Change into the dir to save on path length
            if (chdir(*current_file) == -1) {
                perror("chrdir");
            }
            if (options.thread_count == 1) { // Custom solution for one thread
                int dir_fd = open("./", O_RDONLY | O_DIRECTORY);
                total_size += total_disk_usage_task_st(dir_fd);
                //total_size += total_disk_usage_task_st(dir_fd);
            }
            else {
                char* current_file_path = malloc(512);
                strcpy(current_file_path, "./");
                StackEntry stack_task;
                stack_task.path = current_file_path;
                stack_push(&tasks, stack_task);

                for (size_t i = 0; i < options.thread_count; i++) {
                    pthread_create(&tid[i], NULL, run_disk_usage_thread,
                                   (void*) &thread_args[i]);
                }

                for (size_t i = 0; i < options.thread_count; i++) {
                    pthread_join(tid[i], NULL);
                }

                for (size_t i = 0; i < options.thread_count; i++) {
                    total_size += thread_args[i].total_size_bytes;
                }
            }
            // Change back into previous working dir to allow for more relative paths
            if (chdir(default_working_dir) == -1) {
                perror("chrdir");
            }
        }

        printf("%zu    %s\n", total_size / options.block_size, *current_file);
        current_file++;
    }

    pthread_mutex_destroy(&tasks_mutex);
    pthread_mutex_destroy(&idle_mutex);
    sem_destroy(&idle_sem);

    stack_free(&tasks);
#ifdef PROFILE_TIME
    // Get end time
    clock_gettime(CLOCK_REALTIME, &after);

    elapsed_nsecs = (after.tv_sec - before.tv_sec) * 1000000000 +
                    (after.tv_nsec - before.tv_nsec);
    printf("Complete program took %ld ms \n", elapsed_nsecs / 1000000);
#endif
}