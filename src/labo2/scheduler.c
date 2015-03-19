#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <wait.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include "lib/logging.h"
#include "lib/collections.h"
#include "scheduler.h"

// Define useful macros.
#define RESOURCE_TYPE_COUNT 4
#define PROCESS_PRIORITY_COUNT 4
#define SMALL_BUFFER_SIZE 64
#define BIG_BUFFER_SIZE 1024
#define EXECUTABLE "./log710h15process"
#define QUANTUM 1
#define true 1
#define false 0

// Set the logging level to whatever we need for debugging purposes.
unsigned int loglevel = INFO_LVL;

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

// The input processes before they are queued.
process_t* input_processes[SMALL_BUFFER_SIZE];

// The input processes length.
int input_processes_length;

// Array of all queues for each priorities.
queue_t* process_queues[PROCESS_PRIORITY_COUNT];

// Array of resources by resource type.
resource_t* resources[RESOURCE_TYPE_COUNT];

// The process that currently has the cpu.
process_t* current_running_process;

// Clock to know which processes arrives when.
int global_clock;

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
// After this function call, the scheduler has to be ready to be started.
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
           // Add the process to the input processes.
           input_processes[input_processes_length++] = process;
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
    // get segfaults (lol never, this is Cparta).
    const int procsz = sizeof(process_t) / sizeof(int);
    const char* delimiter = ", ";
    
    // Since a process is only integers, assume the proc is a int*.
    // Skip the first value tough.
    unsigned int* values = ((unsigned int *) (((void*) proc) + sizeof(pid_t))) + 1;
    
    int tokcnt = 0;
    char* tok = strtok(unparsedproc, delimiter);
    while (tok != NULL && tokcnt < procsz) {        
        // Interpret token as an integer.
        values[tokcnt++] = (unsigned) atoi(tok);
        // Fetch next token.
        tok = strtok(NULL, delimiter);
    }
    
    // Validate process data.
    if (proc->priority > low || 
        proc->exec_time == 0) {
        return INVALID_PROCESS_SERIALIZATION_ERRNO;
    }
    
    // Validate process' resource usage.
    if (proc->resx_cnt[printer] > RESOURCE_COUNTS[printer] ||
        proc->resx_cnt[scanner] > RESOURCE_COUNTS[scanner] || 
        proc->resx_cnt[modem] > RESOURCE_COUNTS[modem] ||
        proc->resx_cnt[cd] > RESOURCE_COUNTS[cd]) {
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
    log_info("Entering start_scheduler().");
    
    // It is critical that input processes are in the right order.
    int iproc = 0;
    sort_by_input_time(input_processes, input_processes_length);
    
    while (true) {
        log_info("Global clock tick %d.", global_clock);
        
        // Queue all processes that arrived at this tick of the global clock.
        while (iproc < input_processes_length && input_processes[iproc]->input_time == global_clock) {
            log_info("Process (%s) is entering the scheduler.", process_to_string(input_processes[iproc]));
            priority_t priority = input_processes[iproc]->priority;
            queue_enqueue(process_queues[priority], input_processes[iproc]);
            iproc++;
        }
        
        // In order of priority, run the first process that can be dequeued.
        int proc_exists_in_queues = false;
        int i; for (i = 0; i < PROCESS_PRIORITY_COUNT && !proc_exists_in_queues; i++) {
            // Dequeue one element from the current queue.
            void* element;
            queue_dequeue(process_queues[i], &element);
            
            if (element != NULL) {
                // Process existed in the queue. Run it.
                switch (i) {
                    case realtime:
                        run_nonpremptive_process(element);
                        break;
                    default:
                        run_premptive_process(element);
                        break;
                }
                
                // One process ran. Break this loop.
                proc_exists_in_queues = true;
            }
        }
        
        if (!proc_exists_in_queues && iproc >= input_processes_length) {
            // No more input and no more processes to be ran.
            log_info("No more processes exist in the input or in the queues. Scheduler will now shutdown.");
            break;
        } else {
            // Keep ticking.
            global_clock++;
            // sleep(1);
        }
    }
    
    log_info("Exiting start_scheduler().");
    return SUCCESSFUL_EXEC;
}

// Runs a process with cpu requisition.
int run_premptive_process(process_t* process) {
    log_info("Entering run_premptive_process().");

    // Set an handler for the quantum expiration.
    if (signal(SIGALRM, handle_quantum_expiration) == SIG_ERR) {
        log_error("Could not set the signal handler for quantum expirations.");
        return -1;
    }
    
    // Set the currently running process.
    current_running_process = process;
    
    // Acquire all resources the process needs.
    acquire_resources(current_running_process, resources);
                
    // Execute the process.
    execute_process(current_running_process);
    alarm(QUANTUM);
    waitpid(current_running_process->pid, NULL, WUNTRACED);

    // Releases all resources the process has.
    release_resources(current_running_process, resources);
        
    // Remove quantum expiration handler.
    signal(SIGALRM, SIG_DFL);
    log_info("Exiting run_premptive_process().");
    return SUCCESSFUL_EXEC;
}

// Runs a process without cpu requisition. The process can still timeout.
int run_nonpremptive_process(process_t* process) {
    log_info("Entering run_nonpremptive_process().");
    
    // Set an handler for the timeout.
    if (signal(SIGALRM, handle_timeout) == SIG_ERR) {
        log_error("Could not set the signal handler for timeouts.");
        return -1;
    }
    
    // Set the currently running process.
    current_running_process = process;
    
    // Execute the process.
    execute_process(current_running_process);

    // Non preemptive. Set a timeout, and wait for either timeout or process end.
    alarm(current_running_process->exec_time);
    waitpid(current_running_process->pid, NULL, 0);
    
    // Remove timeout handler.
    signal(SIGALRM, SIG_DFL);
    log_info("Exiting run_nonpremptive_process().");
    return SUCCESSFUL_EXEC;
}

// Executes the child process.
void execute_process(process_t* process) {
    log_info("Entering execute_process()");

    if (process->has_executed_once) {
        // Has already executed at least once.
        log_info("Cpu given to process %s", process_to_string(current_running_process));
        kill(process->pid, SIGCONT);
    } else {
        // Create a pipe to redirect outputs.
        // pipe(process->pipe);
        
        process->has_executed_once = true;
        process->pid = fork();
        if (process->pid == 0) {
            // Redirect stdout to pipe.
            // close(process->pipe[0]);
            // dup2(1, process->pipe[1]);
            
            // Child process. Start the default executable.
            char buffer[SMALL_BUFFER_SIZE];
            sprintf(buffer, "%d", process->exec_time);
            char* argv[3] = { EXECUTABLE, buffer, NULL };
            execvp(argv[0], argv);
            exit(errno);
        } 
        
        // close(process->pipe[1]);
        log_info("Cpu given to process %s", process_to_string(current_running_process));
    }
        
    log_info("Exiting execute_process().");
}

// Try to acquire all resources for a process.
int acquire_resources(process_t* process, resource_t* resources[]) {
    log_info("Entering acquire_resources().");

    int i; for (i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        int j; for (j = 0; j < process->resx_cnt[i]; j++) {
            log_info("Waiting for resource of type %d.", i);
            sem_wait(resources[i]->semaphore);
        }
    }
    
    log_info("Exiting acquire_resources().");
    return SUCCESSFUL_EXEC;
}

// Releases all resources for a process.
int release_resources(process_t* process, resource_t* resources[]) {
    log_info("Entering release_resources().");

        int i; for (i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        int j; for (j = 0; j < process->resx_cnt[i]; j++) {
            log_info("Releasing resource of type %d.", i);
            sem_post(resources[i]->semaphore);
        }
    }
    
    log_info("Exiting release_resources().");
    return SUCCESSFUL_EXEC;
}

// Returns a string representing the process.
char* process_to_string(process_t* process) {
    log_info("Entering process_to_string()");
    
    char* buffer = malloc(BIG_BUFFER_SIZE * sizeof(char)); // TODO free it.
    sprintf(buffer, "Process %d Priority: %d Execution time: %d",
        process->pid, process->priority, process->exec_time);
    
    log_info("Exiting process_to_string()");
    return buffer;
}

// Handles a process that times out.
static void handle_timeout(int signo) {
    log_info("Entering handle_timeout()");
    
    // Reached timeout. Kill currently running process.
    kill(current_running_process->pid, SIGINT);
    waitpid(current_running_process->pid, NULL, 0);
    log_info("Process with pid %d timed out.", current_running_process->pid);
    
    log_info("Exiting handle_timeout().");
}

// Handles a process that expired its time quantum.
static void handle_quantum_expiration(int signo) {
    log_info("Entering handle_quantum_expiration()");
    
    // Decrement the execution time
    current_running_process->exec_time = current_running_process->exec_time - QUANTUM;
    
    // Reached timeout. Kill currently running process.
    if (current_running_process->exec_time > 0) {
        log_info("Process with pid %d expired its quantum.", current_running_process->pid);
        kill(current_running_process->pid, SIGTSTP);
    
        // Reenqueue in a lower priority.
        if (current_running_process->priority != low) {
            // Lowest priorities have a greater value. Increment the priority.
            current_running_process->priority = current_running_process->priority + 1;
        }
        
        queue_enqueue(process_queues[current_running_process->priority], current_running_process);
    } else {
        log_info("Process with pid %d timed out.", current_running_process->pid);
        kill(current_running_process->pid, SIGINT);
    }
    
    log_info("Exiting handle_quantum_expiration().");
}

// Sorts a process buffer by input time.
void sort_by_input_time(process_t* input_processes[], int length) {
    // Bubble sort, because I don't care.
    int i; for (i = length - 1; i > 0; i--) {
        int j; for (j = 0; j < i; j++) {
            if (input_processes[j] > input_processes[j + 1]) {
                process_t* temp = input_processes[j];
                input_processes[j] = input_processes[j + 1];
                input_processes[j + 1] = temp;
            }
        }
    }
    
    for (i = 0; i > length; i++) {
        log_info("%s", process_to_string(input_processes[i]));
    }
}