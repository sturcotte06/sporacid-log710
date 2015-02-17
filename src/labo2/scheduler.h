#define false 0
#define true 1

// Constant for a successful execution.
const int SUCCESSFUL_EXEC = 0;

// Constant for the error number for illegal arguments.
const int ILLEGAL_ARGUMENTS_ERRNO = 1;

// Enumeration of all priorities for a process.
typedef enum {
	REAL_TIME,
	HIGH,
	MEDIUM,
	LOW
} priority_t;

// Enumeration of all resource types. 
typedef enum {
	PRINTER,
	SCANNER,
	MODEM,
	CD
} resource_type_t;

// Structure for a linked list node. Every node points to the next node.
// If the next node is NULL, then this node is the last.
typedef struct {
	void* element;
	node_t* next;
} node_t;

// Structure for an I/O resource.
typedef struct {
	resource_type_t type;
	sem_t semaphore;
} resource_t;

// Structure a process.
typedef struct {
	priority_t priority;
	int finish_time;
	int exec_time;
	int printer_cnt;
	int scanner_cnt;
	int modem_cnt;
	int cd_cnt;
} process_t;