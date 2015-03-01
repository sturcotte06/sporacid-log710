#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdarg.h>
#include "lib/logging.h"
#include "lib/collections.h"
#include "scheduler.h"

// Set the logging level to whatever we need for debugging purposes.
int loglevel = INFO_LVL;

// Constant for a successful execution.
const int SUCCESSFUL_EXEC = 0;

// Constant for the error number when trying to dequeue an empty queue.
const int EMPTY_QUEUE_ERRNO = 100;

// Constant for the error number when trying to access or modify an out of bound index.
const int OUT_OF_BOUNDS_ERRNO = 101;

// Constant for the error number when trying to modify a null linked list.
const int NULL_LINKED_LIST_ERRNO = 102;

// Constant for the error number for illegal arguments.
const int ILLEGAL_ARGUMENTS_ERRNO = 10;

// Constant for the error number when a process' serialization is invalid.
const int INVALID_PROCESS_SERIALIZATION_ERRNO = 11;

// Constant for the error number when a process' definition might lead to a deadlock.
const int POSSIBLE_DEADLOCK_PROCESS_ERRNO = 12;

// Constant for the error number if a required file does not exist.
const int FILE_UNEXISTING_ERRNO = 13;

// Default buffer size. If input exceeds, expect the program to return an error code.
const int BUFF_SIZE = 1024;

// Constants for the available resources count.
const int PRINTER_CNT = 2;
const int SCANNER_CNT = 1;
const int MODEM_CNT = 1;
const int CD_CNT = 2;

// Buffer for all resources.
resource_t* resources;

// The number of resources.
int resxcnt;

// Buffer for all processes.
process_t* processes;

// The number of processes.
int proccnt;

// 1: lire fichier list process (done)
// 2: process temps réel
// 3: process usager
// 4: ressources

// Main method that will execute the scheduler.
int main(int argc, char* argv[]) {
	// We only receive 1 argument: the name of the process list file.
	if (argc != 2) {
		exit(ILLEGAL_ARGUMENTS_ERRNO);
	}
    
	// Get the only argument (The process list file name).
	char* procfile = argv[1];
    
    // Initialize the scheduler.
    init_scheduler(procfile);
    
    // Start the scheduler.
    start_scheduler(processes, proccnt, resources, resxcnt);

    // Done.
    exit(SUCCESSFUL_EXEC);
}

// Initialize the scheduler data.
// After this function call, the scheduler has to be ready to be ran.
int init_scheduler(char* procfile) {
    logft(INFO_LVL, "Initializing scheduler...");
    init_resources();
	init_processes(procfile);
    
    logft(INFO_LVL, "Scheduler initialized.");
    return SUCCESSFUL_EXEC;
}

// Initialize the list of processes to schedule.
int init_processes(char* procfile) {
    logft(INFO_LVL, "Initializing processes of scheduler...");

    // Allocate memory for processes.
    processes = malloc(BUFF_SIZE * sizeof(process_t));
    proccnt = 0;
    
    FILE* file;
    char* line;
    size_t len = 0;
    ssize_t read;
    
    // Open the file in read mode.
    logft(INFO_LVL, "Opening process list file %s...", procfile);
    if ((file = fopen(procfile, "r")) == NULL) {
        return FILE_UNEXISTING_ERRNO;
    }
    
    // Read the file line by line. Every line is a process.
    while ((read = getline(&line, &len, file)) != -1) {
        // Truncate last newline character.
        line[strlen(line) - 1] = '\0';
    
        // Parse the line into a process struct.
        int result = parse_process(line, &processes[proccnt]);
        if (result == INVALID_PROCESS_SERIALIZATION_ERRNO) {
            logft(ERROR_LVL, "Process could not be deserialized. The process will not be run.");
        } else if (result == POSSIBLE_DEADLOCK_PROCESS_ERRNO) {
            logft(ERROR_LVL, "Process definition could lead to a deadlock. The process will not be run.");
        } else {
            proccnt++;        
        }
    }
    
    logft(INFO_LVL, "Processes of scheduler initialized.");
    return SUCCESSFUL_EXEC;
}

// Parse a single process string representation.
int parse_process(char* unparsedproc, process_t* proc) {
    logft(INFO_LVL, "Parsing serialized process [%s]...", unparsedproc);
        
    // Keep the size of a process struct, in integers, so we never
    // get segfaults.
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
    if (proc->finish_time == 0) 
        return INVALID_PROCESS_SERIALIZATION_ERRNO;
    if (proc->priority > low) 
        return INVALID_PROCESS_SERIALIZATION_ERRNO;
    if (proc->exec_time == 0) 
        return INVALID_PROCESS_SERIALIZATION_ERRNO;
    if (proc->printer_cnt > PRINTER_CNT) 
        return POSSIBLE_DEADLOCK_PROCESS_ERRNO;
    if (proc->scanner_cnt > SCANNER_CNT) 
        return POSSIBLE_DEADLOCK_PROCESS_ERRNO;
    if (proc->modem_cnt > MODEM_CNT) 
        return POSSIBLE_DEADLOCK_PROCESS_ERRNO;
    if (proc->cd_cnt > CD_CNT) 
        return POSSIBLE_DEADLOCK_PROCESS_ERRNO;
    
    logft(INFO_LVL, "Serialized process parsed. Its priority is %d.", (int) proc->priority);
    return SUCCESSFUL_EXEC;
}

// Initialize available resources for this scheduler.
int init_resources() {
    logft(INFO_LVL, "Initializing resources of scheduler...");
    
    // Allocate memory for resources.
    resources = malloc(4 * sizeof(resource_t));
	resxcnt = 4;
    
    // Hardcode resources, because pdf says so.
    // Printers.
    logft(INFO_LVL, "Initializing printer resources...");
    resources[0].type = printer;
    sem_init(&resources[0].semaphore, 0, PRINTER_CNT);
    
    // Scanners.
    logft(INFO_LVL, "Initializing scanner resources...");
    resources[1].type = scanner;
    sem_init(&resources[1].semaphore, 0, SCANNER_CNT);
    
    // Modems.
    logft(INFO_LVL, "Initializing modem resources...");
    resources[2].type = modem;
    sem_init(&resources[2].semaphore, 0, MODEM_CNT);
    
    // Cds.
    logft(INFO_LVL, "Initializing cd resources...");
    resources[3].type = cd;
    sem_init(&resources[3].semaphore, 0, CD_CNT);
    
    logft(INFO_LVL, "Resources of scheduler initialized.");
    return SUCCESSFUL_EXEC;
}

// Starts the scheduler.
int start_scheduler(process_t* processes, int proccnt, resource_t* resources, int rexcnt) {
    // Create queues for all priority levels.
    // queue_t queues[4];
    return SUCCESSFUL_EXEC;
}