#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include "scheduler.h"
#include <stdarg.h>

// Buffer for all resources.
resource_t* resources;

// The number of resources.
int resxcnt;

// Buffer for all processes.
process_t* processes;

// The number of processes.
int proccnt;

// 1: lire fichier list process (done)
// 2: process temps r�el
// 3: process usager
// 4: ressources

// Main method that will execute the scheduler.
void main(int argc, char* argv[]) {
	// We only receive 1 argument: the name of the process list file.
	if (argc != 2) {
		exit(ILLEGAL_ARGUMENTS_ERRNO);
	}
	
	// Get the only argument (The process list file name).
	char* procfile = argv[1];
    
    // Initialize the scheduler.
    init_scheduler(procfile);
    exit(SUCCESSFUL_EXEC);
}

// Initialize the scheduler data.
// After this function call, the scheduler has to be ready to be ran.
int init_scheduler(char* procfile) {
    logf(INFO_LVL, "Initializing scheduler...");
    init_resources();
	init_processes(procfile);
    
    logf(INFO_LVL, "Scheduler initialized.");
    return SUCCESSFUL_EXEC;
}

// Initialize the list of processes to schedule.
int init_processes(char* procfile) {
    logf(INFO_LVL, "Initializing processes of scheduler...");

    // Allocate memory for processes.
    processes = malloc(BUFF_SIZE * sizeof(process_t));
    proccnt = 0;
    
    FILE* file;
    char* line;
    size_t len = 0;
    ssize_t read;
    
    // Open the file in read mode.
    logf(INFO_LVL, "Opening process list file %s...", procfile);
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
            logf(ERROR_LVL, "Process could not be deserialized. The process will not be run.");
        } else if (result == POSSIBLE_DEADLOCK_PROCESS_ERRNO) {
            logf(ERROR_LVL, "Process definition could lead to a deadlock. The process will not be run.");
        } else {
            proccnt++;        
        }
    }
    
    logf(INFO_LVL, "Processes of scheduler initialized.");
    return SUCCESSFUL_EXEC;
}

// Parse a single process string representation.
int parse_process(char* unparsedproc, process_t* proc) {
    logf(INFO_LVL, "Parsing serialized process [%s]...", unparsedproc);
        
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
    
    logf(INFO_LVL, "Serialized process parsed. Its priority is %d.", (int) proc->priority);
    return SUCCESSFUL_EXEC;
}

// Initialize available resources for this scheduler.
int init_resources() {
    logf(INFO_LVL, "Initializing resources of scheduler...");
    
    // Allocate memory for resources.
    resources = malloc(4 * sizeof(resource_t));
	resxcnt = 4;
    
    // Hardcode resources, because pdf says so.
    // Printers.
    logf(INFO_LVL, "Initializing printer resources...");
    resources[0].type = printer;
    sem_init(&resources[0].semaphore, 0, PRINTER_CNT);
    
    // Scanners.
    logf(INFO_LVL, "Initializing scanner resources...");
    resources[1].type = scanner;
    sem_init(&resources[1].semaphore, 0, SCANNER_CNT);
    
    // Modems.
    logf(INFO_LVL, "Initializing modem resources...");
    resources[2].type = modem;
    sem_init(&resources[2].semaphore, 0, MODEM_CNT);
    
    // Cds.
    logf(INFO_LVL, "Initializing cd resources...");
    resources[3].type = cd;
    sem_init(&resources[3].semaphore, 0, CD_CNT);
    
    logf(INFO_LVL, "Resources of scheduler initialized.");
    return SUCCESSFUL_EXEC;
}

// Logs an event to standard output.
void logf(const char* level, const char* format, ...) {
    // Preformat the current timestamp.
    time_t now = time(NULL);
    char* nowstr = ctime(&now);
    nowstr[strlen(nowstr) - 1] = '\0';
        
    // Preformat the message.
    va_list args;
    va_start(args, format);
            
    // Print the final log message.
    printf(" [%s] [%s] ", level, /*pthread_self(), */nowstr);
    vfprintf(stdout, format, args);
    printf("\n");
    va_end(args);
}