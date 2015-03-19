// Constant for a successful execution.
extern const int SUCCESSFUL_EXEC;

// Constant for the error number for illegal arguments.
extern const int ILLEGAL_ARGUMENTS_ERRNO;

// Constant for the error number when a process' serialization is invalid.
extern const int INVALID_PROCESS_SERIALIZATION_ERRNO;

// Constant for the error number when a process' definition might lead to a deadlock.
extern const int POSSIBLE_DEADLOCK_PROCESS_ERRNO;

// Constant for the error number if a required file does not exist.
extern const int FILE_UNEXISTING_ERRNO;

// Enumeration of all priorities for a process.
typedef enum priority_t priority_t;
enum priority_t {
	realtime, high, medium, low
};

// Enumeration of all resource types.
typedef enum resource_type_t resource_type_t;
enum resource_type_t {
	printer, scanner, modem, cd
};

// Structure for an I/O resource.
typedef struct resource_t resource_t;
struct resource_t {
	resource_type_t type;
	sem_t* semaphore;
};

// Structure for a process.
typedef struct process_t {
    pid_t pid;
    unsigned int has_executed_once;
	unsigned int input_time;
	priority_t priority;
	unsigned int exec_time;
	unsigned int resx_cnt[4];
} process_t;

// Initialize the scheduler data.
// After this function call, the scheduler has to be ready to be ran.
int init_scheduler(char* procfile, queue_t* process_queues[], resource_t* resources[]);

// Initialize the list of processes to schedule.
int init_processes(char* procfile, queue_t* process_queues[], resource_t* resources[]);

// Parse a single process string representation.
int parse_process(char* unparsedproc, process_t* proc);

// Initialize available resources for this scheduler.
int init_resources(resource_t* resources[]);

// Starts the scheduler.
int start_scheduler(queue_t* process_queues[], resource_t* resources[]);

// Runs a process without cpu requisition. The process can still timeout.
int run_nonpremptive_process(process_t* process);

// Runs a process with cpu requisition.
int run_premptive_process(process_t* process);

// Executes the child process.
void execute_process(process_t* process);

// Try to acquire all resources for a process.
int try_acquire_resources(process_t* process, resource_t* resources[], int* success);

// Releases all resources for a process.
int release_resources(process_t* process, resource_t* resources[]);

// Returns a string representing the process.
char* process_to_string(process_t* process);

// Handles a process that times out.
static void handle_timeout(int signo);

// Handles a process that expired its time quantum.
static void handle_quantum_expiration(int signo);

// Sorts a process buffer by input time.
void sort_by_input_time(process_t* input_processes[], int length);