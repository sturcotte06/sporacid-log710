#define false 0
#define true 1

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

// Default buffer size. If input exceeds, expect the program to return an error code.
extern const int BUFF_SIZE;

// Constants for the available resources count.
extern const int PRINTER_CNT;
extern const int SCANNER_CNT;
extern const int MODEM_CNT;
extern const int CD_CNT;

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
	sem_t semaphore;
};

// Structure a process.
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
int init_scheduler(char* procfile);

// Initialize the list of processes to schedule.
int init_processes(char* procfile);

// Parse a single process string representation.
int parse_process(char* unparsedproc, process_t* proc);

// Initialize available resources for this scheduler.
int init_resources();

// Starts the scheduler.
int start_scheduler(process_t* processes, int proccnt, resource_t* resources, int rexcnt);