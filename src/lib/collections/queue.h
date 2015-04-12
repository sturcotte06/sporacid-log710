#ifndef LIB_COLLECTIONS_QUEUE_H
#define LIB_COLLECTIONS_QUEUE_H

#include "linkedlist.h"

// Enum for the queue possible function states.
typedef enum queue_state_t {
	queue_success,
	queue_invalid_args,
	queue_out_of_bounds,
	queue_empty
} queue_state_t;

// Structure for a  queue.
typedef struct queue_t {
	linkedlist_t inner_list;
} queue_t;

/// <summary>
/// Initializes a queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <returns>The state enum value.</returns>
queue_state_t queue_init(queue_t* queue);

/// <summary>
/// Enqueues an element in the queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <param name="element">The element to enqueue.</param>
/// <returns>The state enum value.</returns>
queue_state_t queue_enqueue(queue_t* queue, void* element);

/// <summary>
/// Dequeues an element from the queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <param name="element">The out parameter for the element. This should (not checked) be null when called.</param>
/// <returns>The state enum value.</returns>
queue_state_t queue_dequeue(queue_t* queue, void** element);

/// <summary>
/// Destroys a queue and all used memory. The queue structure does not belong to this module;
/// the callee has to deal with the structure memory itself.
/// </summary>
/// <param name="queue">The queue to destroy. This cannot be null.</param>
/// <returns>The state enum value.</returns>
queue_state_t queue_destroy(queue_t* queue);

#endif