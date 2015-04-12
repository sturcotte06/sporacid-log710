#include <stdio.h>
#include <stdlib.h>
#include "../logging/logging.h"
#include "linkedlist.h"
#include "queue.h"

/// <summary>
/// Initializes a queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <returns>The state enum value.</returns>
queue_state_t queue_init(queue_t* queue) {
    log_debug("Entering queue_init().");
    if (queue == NULL) {
        return queue_invalid_args;
    }

    // A queue is simply a linked list that is managed differently.
	linkedlist_state_t state = linkedlist_init(&queue->inner_list);
	if (state != linkedlist_success) return (queue_state_t) state;
    
    log_debug("Exiting queue_init().");
    return queue_success;
}

/// <summary>
/// Destroys a queue and all used memory. The queue structure does not belong to this module;
/// the callee has to deal with the structure memory itself.
/// </summary>
/// <param name="queue">The queue to destroy. This cannot be null.</param>
/// <returns>The state enum value.</returns>
queue_state_t queue_destroy(queue_t* queue) {
    log_debug("Entering queue_destroy().");
    if (queue == NULL) {
        return queue_invalid_args;
    }
    
    // Destroy the inner list.
    linkedlist_state_t state = linkedlist_destroy(&queue->inner_list);
	if (state != linkedlist_success) return (queue_state_t) state;
    
    log_debug("Exiting queue_destroy().");
    return queue_success;
}

/// <summary>
/// Enqueues an element in the queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <param name="element">The element to enqueue.</param>
/// <returns>The state enum value.</returns>
queue_state_t queue_enqueue(queue_t* queue, void* element) {    
    log_debug("Entering queue_enqueue().");
    if (queue == NULL) {
        return queue_invalid_args;
    }
    
    linkedlist_state_t state = linkedlist_add(&queue->inner_list, queue->inner_list.length, element);
    if (state != linkedlist_success) return (queue_state_t) state;
    
    log_debug("Exiting queue_enqueue().");
    return queue_success;
}

/// <summary>
/// Dequeues an element from the queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <param name="element">The out parameter for the element. This should (not checked) be null when called.</param>
/// <returns>The state enum value.</returns>
queue_state_t queue_dequeue(queue_t* queue, void** element) {
    log_debug("Entering queue_dequeue().");
    if (queue == NULL) {
        return queue_invalid_args;
    }

    if (queue->inner_list.length == 0) {
        // Special case. 
        // Queue is empty, just set null into element and return.
        *element = NULL;
        return queue_empty;
    }
    
    // Get the first element in the linked list.
    linkedlist_state_t state = linkedlist_get(&queue->inner_list, 0, element);
    if (state != linkedlist_success) {
        *element = NULL;
        return (queue_state_t) state;
    }
    
    // Remove the element from the linked list. The element is now considered consumed.
	state = linkedlist_remove(&queue->inner_list, 0);
    if (state != linkedlist_success) {
        *element = NULL;
        return (queue_state_t) state;
    }
    
    log_debug("Exiting queue_dequeue().");
    return queue_success;
}