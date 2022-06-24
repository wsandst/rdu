/**
 * This file implements functionality
 * for recursively determining the file size
 * of a directory or file
 *
 * @file disk_usage.h
 * @author William Sandström
 */
#define _GNU_SOURCE
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h> // stat
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <linux/fs.h>

#include "util/helpers.h"
#include "args.h"
#include "util/stack.h"

#define ST_NBLOCKSIZE 512 // Always 512 on linux

#define SINGLE_TASK_OPTIMIZATION
#define DIRENT_BUFFER_SIZE 4096
//#define PROFILE_TIME

struct ThreadArgs {
    Stack* tasks;
    size_t total_size_bytes;
    size_t thread_count;
    pthread_mutex_t* tasks_mutex;
    pthread_mutex_t* idle_mutex;
    sem_t* idle_sem;
    size_t* idle_threads;
    bool* all_threads_complete;
    long int time_spent_in_task;
    //bool error_encountered;
};

struct linux_dirent64 {
    unsigned long d_ino; /* 64-bit inode number */
    long d_off; /* 64-bit offset to next structure */
    unsigned short d_reclen; /* Size of this dirent */
    unsigned char d_type; /* File type */
    char d_name[]; /* Filename (null-terminated) */
};

typedef struct linux_dirent64 ldirent;

typedef struct ThreadArgs ThreadArgs;

/**
 * Is this dir a dot directory,
 * ie does it match "." or ".."
 */
bool is_dot_dir(char* dir);

/**
 * Determine the total disk usage of a file or 
 * directory, recursively counting files and directories
 * inside
 * 
 * @param path path of file/dir
 * @return disk usage in bytes
 */
size_t total_disk_usage(char* path);

/**
 * Analyze the total disk usage of
 * the files provided in options
 * using the threadcount in options
 * 
 * @param options options file from cmd args
 */
void disk_usage(Options options);

/**
 * Thread function which takes disk usage
 * tasks and analyzes the disk usage
 * in a threadsafe manner
 * 
 * @param arg_ptr void pointer to ThreadArgs
 * @return none, but the disk usage found per
 * thread can be found in the ThreadArgs structure
 */
void* run_disk_usage_thread(void* arg_ptr);

/**
 * Determine the disk usage of the files in directory
 * If a containing file is a directory, add the path to new_tasks
 * 
 * @param path path of directory
 * @param new_tasks stack of new files to be checked
 * 
 * @return disk usage in bytes
 */
size_t total_disk_usage_task(char* path, Stack* new_tasks);

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
size_t total_disk_usage_task_time(char* path, Stack* new_tasks, long int* time_spent);

/**
 * Return the disk usage of a file 
 * Note: This is different from apparent file size
 * 
 * @param path path of directory
 * 
 * @return disk usage in bytes
 */
size_t get_file_disk_usage(char* path, bool* file_is_dir);

/**
 * Return the disk usage of a file relative to an open dir fd
 * Note: This is different from apparent file size
 * 
 * @param dirfd directory file descriptor
 * @param path relative path of file
 * 
 * @return disk usage in bytes
 */
size_t get_file_disk_usage_fd(int dirfd, char* path, bool* file_is_dir);

/**
 * Determine the disk usage of the files in directory recursively
 * Single-threaded solution using file dirs, this is only faster single threaded
 * for whatever reason
 * 
 * @param path path of directory
 * 
 * @return disk usage in bytes
 */
size_t total_disk_usage_task_st(int dir_fd);

size_t total_disk_usage_task_st_getdents(int dir_fd);