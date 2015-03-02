#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "lib/logging.h"
#include "lib/collections.h"
#include "scheduler.h"

// Define useful macros.
#define RESOURCE_TYPE_COUNT 4
#define PROCESS_PRIORITY_COUNT 4
#define SMALL_BUFFER_SIZE 64
#define BIG_BUFFER_SIZE 1024

// Set the logging level to whatever we need for debugging purposes.
unsigned int loglevel = DEBUG_LVL;

// Constant for a successful execution.
const int SUCCESSFUL_EXEC = 0;

// Constant for the error number when trying to dequeue an empty queue.
const int EMPTY_QUEUE_ERRNO = 100;

// Constant for the error number when trying to access or modify an out of bound index.
const int OUT_OF_BOUNDS_ERRNO = 101;

// Constant for the error number when trying to modify a null linked list.
const int NULL_LINKED_LIST_ERRNO = 102;

// Constant for the error number when trying to modify a null queue.
const int NULL_QUEUE_ERRNO = 103;

// Constant for the error number for illegal arguments.
const int ILLEGAL_ARGUMENTS_ERRNO = 10;

// Constant for the error number when a process' serialization is invalid.
const int INVALID_PROCESS_SERIALIZATION_ERRNO = 11;

// Constant for the error number when a process' definition might lead to a deadlock.
const int POSSIBLE_DEADLOCK_PROCESS_ERRNO = 12;

// Constant for the error number if a required file does not exist.
const int FILE_UNEXISTING_ERRNO = 13;

// Constants for the available resources count.
const int RESOURCE_COUNTS[RESOURCE_TYPE_COUNT] = { 2, 1, 1, 2 };

// Array of all queues for each priorities.
queue_t* process_queues[PROCESS_PRIORITY_COUNT];

// Array of resources by resource type.
resource_t* resources[RESOURCE_TYPE_COUNT];

// 1: lire fichier list process (done)
// 2: process temps réel
// 3: process usager
// 4: ressources

// Main method that will execute the scheduler.
int main(int argc, char* argv[]) {
	// We only receive 1 argument: the name of the process list file.
	if (argc != 2) {
        log_fatal("Illegal number of arguments. The application takes a single argument, which is the path to a process file.");
		exit(ILLEGAL_ARGUMENTS_ERRNO);
	} 
    
	// Get the only argument (The process list file name).
	char* procfile = argv[1];
    
    // Initialize the scheduler.
    int result;
    if ((result = init_scheduler(procfile, process_queues, resources)) != SUCCESSFUL_EXEC) {
        log_fatal("Could not initialize the scheduler. The process cannot continue.");
        exit(result);
    }
    
    // Start the scheduler.
    if ((result = start_scheduler(process_queues, resources)) != SUCCESSFUL_EXEC) {
        log_fatal("Could not start the scheduler. The process cannot continue.");
        exit(result);
    }

    // Done.
    exit(SUCCESSFUL_EXEC);
}

// Initialize the scheduler data.
// After this function call, the scheduler has to be ready to be ran.
int init_scheduler(char* procfile, queue_t* process_queues[], resource_t* resources[]) {
    log_info("Entering init_scheduler().");
    
    // First initialize resources. 
    // We need to know what resources are available to detect possible deadlocks.
    int result;
    if ((result = init_resources(resources)) != SUCCESSFUL_EXEC) {
        log_error("init_scheduler(): unable to initialize resources.");
        return result;
    }
    
    // Then, initiatlize processes.
    if ((result = init_processes(procfile, process_queues, resources)) != SUCCESSFUL_EXEC) {
        log_error("init_scheduler(): unable to initialize processes.");
        return result;
    }
    
    log_info("Exiting init_scheduler().");
    return SUCCESSFUL_EXEC;
}

// Initialize the list of processes to schedule.
int init_processes(char* procfile, queue_t* process_queues[], resource_t* resources[]) {
    log_info("Entering init_processes().");

    int i; for (i = 0; i < PROCESS_PRIORITY_COUNT; i++) {
        // Allocate memory for the process queue.
        process_queues[i] = malloc(sizeof(queue_t));
        // Initialize the queue.
        queue_init(process_queues[i]);
    }    
    
    // Open the file in read mode.
    log_info("Opening process list file \"%s\".", procfile);
    FILE* file;
    if ((file = fopen("liste-taches", "r")) == NULL) {
        return FILE_UNEXISTING_ERRNO;
    }
    
    // Read the file line by line. Every line is a process.
    char* line; size_t len; ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
       // Truncate last newline character.
       line[strlen(line) - 1] = '\0';
       
       // Parse the line into a process struct.
       process_t* process = malloc(sizeof(process_t));
       int result = parse_process(line, process);        
       if (result == INVALID_PROCESS_SERIALIZATION_ERRNO) {
           // Deserialization error, just skip this process.
           free(process);
           log_warn("Process could not be deserialized. It will be skipped.");
       } else if (result == POSSIBLE_DEADLOCK_PROCESS_ERRNO) {
           // Process definition requires more resources than available, just skip the process.
           free(process);
           log_warn("Process definition could lead to a deadlock. The process will not be run.");
       } else {
           // Queue the process in the right priority queue.
           queue_enqueue(process_queues[process->priority], process);
           log_info("Process was queued and is ready to be executed.");
       }
    }
    
    fclose(file);
    log_info("Exiting init_processes().");
    return SUCCESSFUL_EXEC;
}

// Parse a single process string representation.
int parse_process(char* unparsedproc, process_t* proc) {
    log_info("Entering parse_process() with process \"%s\".", unparsedproc);
        
    // Keep the size of a process struct, in integers, so we never
    // get segfaults (lol never, this is Cparta smartass).
    const int procsz = sizeof(process_t) / sizeof(int);
    const char* delimiter = ", ";
    
    // Since a process is only integers, assume the proc is a int*.
    unsigned int* values = (unsigned int *) proc;

    int tokcnt = 0;
    char* tok = strtok(unparsedproc, delimiter);
    while (tok != NULL && tokcnt < procsz) {        
        // Interpret token as an integer.
        values[tokcnt++] = (unsigned) atoi(tok);
        // Fetch next token.
        tok = strtok(NULL, delimiter);
    }
    
    // Validate process data.
    if (proc->finish_time == 0 || 
        proc->priority > low || 
        proc->exec_time == 0) {
        return INVALID_PROCESS_SERIALIZATION_ERRNO;
    }
    
    // Validate process' resource usage.
    if (proc->printer_cnt > RESOURCE_COUNTS[printer] ||
        proc->scanner_cnt > RESOURCE_COUNTS[scanner] || 
        proc->modem_cnt > RESOURCE_COUNTS[modem] ||
        proc->cd_cnt > RESOURCE_COUNTS[cd]) {
        return POSSIBLE_DEADLOCK_PROCESS_ERRNO;
    }
    
    log_info("Exiting parse_process().");
    return SUCCESSFUL_EXEC;
}

// Initialize available resources for this scheduler.
int init_resources(resource_t* resources[]) {
    log_info("Entering init_resources().");
    
    int i; for (i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        resources[i] = malloc(sizeof(resource_t));
        resources[i]->semaphore = malloc(sizeof(sem_t));
        resources[i]->type = (resource_type_t) i;
        sem_init(resources[i]->semaphore, 0, RESOURCE_COUNTS[i]);
    }
    
    log_info("Exiting init_resources().");
    return SUCCESSFUL_EXEC;
}

// Starts the scheduler.
int start_scheduler(queue_t* process_queues[], resource_t* resources[]) {
    return SUCCESSFUL_EXEC;
}