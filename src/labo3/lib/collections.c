#include <stdio.h>
#include <stdlib.h>
#include "logging.h"
#include "collections.h"

/// <summary>
/// Initializes an linked list.
/// </summary>
/// <param name="linkedlist">The linked list to initialize. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_init(linkedlist_t* linkedlist) {
    if (linkedlist == NULL) {
        return NULL_LINKED_LIST_ERRNO;
    }
    
    linkedlist->head = NULL;
    linkedlist->tail = NULL;
    linkedlist->length = 0;
    
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Adds an element to a linked list at the specified index.
/// </summary>
/// <param name="linkedlist">The linked list in which to add. This cannot be null.</param>
/// <param name="index">The index at which to add the element. This cannot exceed the length of the linked list.</param>
/// <param name="element">The element to add.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_add(linkedlist_t* linkedlist, unsigned int index, void* element) {
    log_debug("Entering linkedlist_add() at index %d. Length: %d.", index, linkedlist->length);    
    if (linkedlist == NULL) {
        // Linked list cannot be null.
        return NULL_LINKED_LIST_ERRNO;
    }

    if (index > linkedlist->length + 1) {
        // Index must be within the bounds of the list.
        return OUT_OF_BOUNDS_ERRNO;
    }

    // Create a new node that wraps the element.
    node_t *cnode = malloc(sizeof(node_t));
    cnode->element = element;

    int i;
    node_t *pnode = NULL, *nnode = NULL;
    if (index <= (linkedlist->length / 2)) {
        // Inserting into first half. Start from head.
        // Find the node before which we need to insert.
        nnode = linkedlist->head;
        for (i = 0; i < index; i++) {
            // Keep trace of current and previous nodes.
            pnode = nnode;
            nnode = nnode->next;
        }
    } else {
        // Inserting into second half. Start from tail.
        // Find the node after which we need to insert.
        // Keep in mind we're iterating from tail to head.
        index = linkedlist->length - index;
        pnode = linkedlist->tail;
        for (i = 0; i < index; i++) {
            // Keep trace of current and next nodes.
            nnode = pnode;
            pnode = pnode->previous;
        }
    }
        
    // Set the next and previous of the new node.
    cnode->next = nnode;
    cnode->previous = pnode;
    
    // Update all links. Set new head and tail if applicable.
    if (pnode != NULL) {
        pnode->next = cnode;
		log_trace("Previous node is not NULL.");
    } else {
        linkedlist->head = cnode;
		log_trace("Previous node is NULL, current node is new head.");
    }
    
    if (nnode != NULL) {
        // Next node exists, current node is not the last node.
        nnode->previous = cnode;
		log_trace("Next node is not NULL.");
    } else {
        // Next node does not exist, current node was the last node.
        linkedlist->tail = cnode;
		log_trace("Next node is NULL, current node is new tail.");
    }
    
    linkedlist->length++;    
    log_debug("Exiting linkedlist_add().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Removes the element at the specified index from a linked list.
/// </summary>
/// <param name="linkedlist">The linked list in which to remove. This cannot be null.</param>
/// <param name="index">The index at which to remove the element. This cannot exceed the length of the linked list.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_remove(linkedlist_t* linkedlist, unsigned int index) {
    log_debug("Entering linkedlist_remove() at index %d.", index);
    if (linkedlist == NULL) {
        // Linked list cannot be null.
        return NULL_LINKED_LIST_ERRNO;
    }

    if (index >= linkedlist->length) {
        // Index must be within the bounds of the list.
        return OUT_OF_BOUNDS_ERRNO;
    }
    
    int i;
    node_t *pnode = NULL, *cnode = NULL, *nnode = NULL;
    if (index < (linkedlist->length / 2)) {
        // Inserting into first half. Start from head.
        // Find the node before which we need to insert.
        cnode = linkedlist->head;
        for (i = 0; i < index; i++) {
            // Keep trace of current and previous nodes.
            pnode = cnode;
            cnode = cnode->next;
        }
        
        nnode = cnode->next;
    } else {
        // Inserting into second half. Start from tail.
        // Find the node after which we need to insert.
        // Keep in mind we're iterating from tail to head.
        index = linkedlist->length - index - 1;
        cnode = linkedlist->tail;
        for (i = 0; i < index; i++) {
            // Keep trace of current and next nodes.
            nnode = cnode;
            cnode = cnode->previous;
        }
        
        pnode = cnode->previous;
    }
        
    // Update all links. Set new head and tail if applicable.
    if (nnode != NULL) {
        // Next node exists, current node is not the last node.
        nnode->previous = pnode;
    } else {
        // Next node does not exist, current node was the last node.
        linkedlist->tail = pnode;
    }
    
    if (pnode != NULL) {
        // Previous node exists, current node is not the first node.
        pnode->next = nnode;
    } else {
        // Previous node does not exist, current node was the first node.
        linkedlist->head = nnode;
    }
    
    // Free memory of removed node.
    free(cnode);
    
    linkedlist->length--;    
    log_debug("Exiting linkedlist_remove().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Gets an element from a linked list.
/// </summary>
/// <param name="linkedlist">The linked list in which to get. This cannot be null.</param>
/// <param name="index">The index at which to get the element. This cannot exceed the length of the linked list.</param>
/// <param name="element">The out parameter for the element. This should (not checked) be null when called.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_get(linkedlist_t* linkedlist, unsigned int index, void** element) {
    log_debug("Entering linkedlist_get() at index %d.", index);
    if (linkedlist == NULL) {
        // Linked list cannot be null.
        return NULL_LINKED_LIST_ERRNO;
    }

    if (index >= linkedlist->length) {
        // Index must be within the bounds of the list.
        return OUT_OF_BOUNDS_ERRNO;
    }
    
    int i;
    node_t *cnode = NULL;
    if (index < (linkedlist->length / 2)) {
        // Inserting into first half. Start from head.
        // Find the node before which we need to insert.
        cnode = linkedlist->head;
        for (i = 0; i < index; i++) {
            // Keep trace of current and previous nodes.
            cnode = cnode->next;
        }
    } else {
        // Inserting into second half. Start from tail.
        // Find the node after which we need to insert.
        // Keep in mind we're iterating from tail to head.
        index = linkedlist->length - index - 1;
        cnode = linkedlist->tail;
        for (i = 0; i < index; i++) {
            // Keep trace of current and next nodes.
            cnode = cnode->previous;
        }
    }
    
    *element = cnode->element;    
    log_debug("Exiting linkedlist_get().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Destroys a linked list and all used memory. The linked list structure does not belong to this module;
/// The callee should deal with the structure memory itself.
/// </summary>
/// <param name="linkedlist">The linked list to destroy. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int linkedlist_destroy(linkedlist_t* linkedlist) {
    log_debug("Entering linkedlist_destroy().");
    if (linkedlist == NULL) {
        // Linked list cannot be null.
        return NULL_LINKED_LIST_ERRNO;
    }

    // Free memory of all nodes of the linked list.
    node_t *cnode = linkedlist->head, *tnode = NULL;
    while (cnode != NULL) {
        tnode = cnode;
        cnode = cnode->next;
        free(tnode);
    }
    
    // Reset the structure to initial values.
    linkedlist->head = NULL;
    linkedlist->tail = NULL;
    linkedlist->length = 0;
    
    log_debug("Exiting linkedlist_destroy().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Initializes a queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int queue_init(queue_t* queue) {
    log_debug("Entering queue_init().");
    if (queue == NULL) {
        // Queue cannot be null.
        return NULL_QUEUE_ERRNO;
    }

    // A queue is simply a linked list that is managed differently.
    linkedlist_t* llist = malloc(sizeof(linkedlist_t));
    linkedlist_init(llist);
    queue->llist = llist;
    
    log_debug("Exiting queue_init().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Enqueues an element in the queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <param name="element">The element to enqueue.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int queue_enqueue(queue_t* queue, void* element) {    
    log_debug("Entering queue_enqueue().");
    if (queue == NULL) {
        // Queue cannot be null.
        return NULL_QUEUE_ERRNO;
    }
    
    int result;
    if ((result = linkedlist_add(queue->llist, queue->llist->length, element)) != SUCCESSFUL_EXEC) {
        return result;
    }
    
    log_debug("Exiting queue_enqueue().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Dequeues an element from the queue.
/// </summary>
/// <param name="queue">The queue to initialize. This cannot be null.</param>
/// <param name="element">The out parameter for the element. This should (not checked) be null when called.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int queue_dequeue(queue_t* queue, void** element) {
    log_debug("Entering queue_dequeue().");
    if (queue == NULL) {
        // Queue cannot be null.
        return NULL_QUEUE_ERRNO;
    }

    if (queue->llist->length == 0) {
        // Special case. 
        // Queue is empty, just set null into element and return.
        *element = NULL;
        return SUCCESSFUL_EXEC;
    }
    
    int result;
    // Get the first element in the linked list.
    if ((result = linkedlist_get(queue->llist, 0, element)) != SUCCESSFUL_EXEC) {
        *element = NULL;
        return result;
    }
    
    // Remove the element from the linked list. The element is now considered consumed.
    if ((result = linkedlist_remove(queue->llist, 0)) != SUCCESSFUL_EXEC) {
        *element = NULL;
        return result;
    }
    
    log_debug("Exiting queue_dequeue().");
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Returns whether the queue is empty.
/// </summary>
/// <param name="queue">The queue to poll.</param>
/// <param name="empty">The out parameter.</param>
/// <returns>True if empty, false otherwise.</returns>
int queue_isempty(queue_t* queue, int* empty) {
    log_debug("Entering queue_dequeue().");
    if (queue == NULL) {
        // Queue cannot be null.
        return NULL_QUEUE_ERRNO;
    }
    
    *empty = (queue->llist->length == 0);
    return SUCCESSFUL_EXEC;
}

/// <summary>
/// Destroys a queue and all used memory. The queue structure does not belong to this module;
/// the callee has to deal with the structure memory itself.
/// </summary>
/// <param name="queue">The queue to destroy. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
int queue_destroy(queue_t* queue) {
    log_debug("Entering queue_destroy().");
    if (queue == NULL) {
        // Queue cannot be null.
        return NULL_QUEUE_ERRNO;
    }
    
    // Destroy the inner list.
    linkedlist_destroy(queue->llist);
    queue->llist = NULL;
    
    log_debug("Exiting queue_destroy().");
    return SUCCESSFUL_EXEC;
}