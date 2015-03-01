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
typedef struct process_t process_t;
struct process_t {
	unsigned int finish_time;
	priority_t priority;
	unsigned int exec_time;
	unsigned int printer_cnt;
	unsigned int scanner_cnt;
	unsigned int modem_cnt;
	unsigned int cd_cnt;
};

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