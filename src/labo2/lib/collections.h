#define IS_THREAD_SAFE 0

// Constant for a successful execution.
extern const int SUCCESSFUL_EXEC;

// Constant for the error number when trying to dequeue an empty queue.
extern const int EMPTY_QUEUE_ERRNO;

// Constant for the error number when trying to access or modify an out of bound index.
extern const int OUT_OF_BOUNDS_ERRNO;

// Constant for the error number when trying to modify a null linked list.
extern const int NULL_LINKED_LIST_ERRNO;

// Constant for the error number when trying to modify a null linked list.
extern const int NULL_QUEUE_ERRNO;

// Structure for a double linked list node. Every node points to the previous and next node.
// If the next node is NULL, then this node is the last.
typedef struct node_t node_t;
struct node_t {
	void* element;
    node_t* previous;
	node_t* next;
};

// Structure for a head-tail linked list.
typedef struct linkedlist_t linkedlist_t;
struct linkedlist_t {
	node_t* head;
	node_t* tail;
    int length;
};

// Structure for a head-tail queue.
typedef struct queue_t queue_t;
struct queue_t {
	linkedlist_t* llist;
};

// Unit tests for the linked list.
void linkedlist_utest();

// Unit tests for the queue.
void queue_utest();

// Initializes an empty queue.
int linkedlist_init(linkedlist_t* linkedlist);

// Adds an element to a linked list at the specified index.
int linkedlist_add(linkedlist_t* linkedlist, unsigned int index, void* element);

// Removes an element from a linked list.
int linkedlist_remove(linkedlist_t* linkedlist, unsigned int index);

// Gets an element from a linked list.
int linkedlist_get(linkedlist_t* linkedlist, unsigned int index, void** element);

// Destroys a linked list and all used memory. The head node will *not* be destroyed however.
int linkedlist_destroy(linkedlist_t* linkedlist);

// Initializes an empty queue.
int queue_init(queue_t* queue);

// Dequeues an element from the queue.
int queue_dequeue(queue_t* queue, void** element);

// Enqueues an element in the queue.
int queue_enqueue(queue_t* queue, void* element);

// Destroys a queue and all used memory.
int queue_destroy(queue_t* queue);