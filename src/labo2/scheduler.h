#define false 0
#define true 1

// Constant for a successful execution.
const int SUCCESSFUL_EXEC = 0;

// Constant for the error number for illegal arguments.
const int ILLEGAL_ARGUMENTS_ERRNO = 1;

// Constant for the error number when a process' serialization is invalid.
const int INVALID_PROCESS_SERIALIZATION_ERRNO = 40;

// Constant for the error number when a process' definition might lead to a deadlock.
const int POSSIBLE_DEADLOCK_PROCESS_ERRNO = 41;

// Constant for the error number if a required file does not exist.
const int FILE_UNEXISTING_ERRNO = 50;

// Default buffer size. If input exceeds, expect the program to return an error code.
const int BUFF_SIZE = 1024;

// Constants for the available resources count.
const int PRINTER_CNT = 2;
const int SCANNER_CNT = 1;
const int MODEM_CNT = 1;
const int CD_CNT = 2;

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

// Structure for a linked list node. Every node points to the next node.
// If the next node is NULL, then this node is the last.
typedef struct node_t node_t;
struct node_t {
	void* element;
	node_t* next;
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