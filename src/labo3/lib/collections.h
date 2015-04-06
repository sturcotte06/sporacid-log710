#ifndef LIB_COLLECTIONS_H
#define LIB_COLLECTIONS_H

// Useless define. Only there to clearly specify that collections from this header file
// must not be used on multiple threads.
#define IS_THREAD_SAFE 0

// Constant for a successful execution.
extern const int SUCCESSFUL_EXEC;

// Constant for the error number when trying to dequeue an empty queue.
extern const int EMPTY_QUEUE_ERRNO;

// Constant for the error number when trying to access or modify an out of bound index.
extern const int OUT_OF_BOUNDS_ERRNO;

// Constant for the error number when trying to modify a null linked list.
extern const int NULL_LINKED_LIST_ERRNO;

// Constant for the error number when trying to modify a null queue.
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

/// <summary>
/// Initializes an linked list.
/// </summary>
/// <param name="linkedlist">The linked list to initialize. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_init(linkedlist_t* linkedlist);

/// <summary>
/// Adds an element to a linked list at the specified index.
/// </summary>
/// <param name="linkedlist">The linked list in which to add. This cannot be null.</param>
/// <param name="index">The index at which to add the element. This cannot exceed the length of the linked list.</param>
/// <param name="element">The element to add.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_add(linkedlist_t* linkedlist, unsigned int index, void* element);

/// <summary>
/// Removes the element at the specified index from a linked list.
/// </summary>
/// <param name="linkedlist">The linked list in which to remove. This cannot be null.</param>
/// <param name="index">The index at which to remove the element. This cannot exceed the length of the linked list.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_remove(linkedlist_t* linkedlist, unsigned int index);

/// <summary>
/// Gets an element from a linked list.
/// </summary>
/// <param name="linkedlist">The linked list in which to get. This cannot be null.</param>
/// <param name="index">The index at which to get the element. This cannot exceed the length of the linked list.</param>
/// <param name="element">The out parameter for the element. This should (not checked) be null when called.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_get(linkedlist_t* linkedlist, unsigned int index, void** element);

/// <summary>
/// Destroys a linked list and all used memory. The linked list structure does not belong to this module;
/// The callee should deal with the structure memory itself.
/// </summary>
/// <param name="linkedlist">The linked list to destroy. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_destroy(linkedlist_t* linkedlist);

/// <summary>
/// Initializes a queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int queue_init(queue_t* queue);

/// <summary>
/// Enqueues an element in the queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <param name="element">The element to enqueue.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int queue_enqueue(queue_t* queue, void* element);

/// <summary>
/// Dequeues an element from the queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <param name="element">The out parameter for the element. This should (not checked) be null when called.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int queue_dequeue(queue_t* queue, void** element);

/// <summary>
/// Returns whether the queue is empty.
/// </summary>
/// <param name="queue">The queue to poll.</param>
/// <param name="empty">The out parameter.</param>
/// <returns>True if empty, false otherwise.</returns>
int queue_isempty(queue_t* queue, int* empty);

/// <summary>
/// Destroys a queue and all used memory. The queue structure does not belong to this module;
/// the callee has to deal with the structure memory itself.
/// </summary>
/// <param name="queue">The queue to destroy. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int queue_destroy(queue_t* queue);

#endif