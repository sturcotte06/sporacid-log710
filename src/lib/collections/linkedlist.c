#include <stdio.h>
#include <stdlib.h>
#include "../logging/logging.h"
#include "linkedlist.h"

/// <summary>
/// Initializes an linked list.
/// </summary>
/// <param name="linkedlist">The linked list to initialize. This cannot be null.</param>
/// <returns>The state enum value.</returns>
linkedlist_state_t linkedlist_init(linkedlist_t* linkedlist) {
    if (linkedlist == NULL) {
        return linkedlist_invalid_args;
    }
    
	// Initialize at default empty linked list.
    linkedlist->head = NULL;
    linkedlist->tail = NULL;
    linkedlist->length = 0;
    
    return linkedlist_success;
}

/// <summary>
/// Destroys a linked list and all used memory. The linked list structure does not belong to this module;
/// The callee should deal with the structure memory itself.
/// </summary>
/// <param name="linkedlist">The linked list to destroy. This cannot be null.</param>
/// <returns>The state enum value.</returns>
linkedlist_state_t linkedlist_destroy(linkedlist_t* linkedlist) {
    log_debug("Entering linkedlist_destroy().");
    if (linkedlist == NULL) {
        return linkedlist_invalid_args;
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
    return linkedlist_success;
}

/// <summary>
/// Adds an element to a linked list at the specified index.
/// </summary>
/// <param name="linkedlist">The linked list in which to add. This cannot be null.</param>
/// <param name="index">The index at which to add the element. This cannot exceed the length of the linked list.</param>
/// <param name="element">The element to add.</param>
/// <returns>The state enum value.</returns>
linkedlist_state_t linkedlist_add(linkedlist_t* linkedlist, unsigned int index, void* element) {
    log_debug("Entering linkedlist_add() at index %d. Length: %d.", index, linkedlist->length);
    if (linkedlist == NULL) {
        return linkedlist_success;
    }

    if (index > linkedlist->length + 1) {
        // Index must be within the bounds of the list.
        return linkedlist_out_of_bounds;
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
    return linkedlist_success;
}

/// <summary>
/// Removes the element at the specified index from a linked list.
/// </summary>
/// <param name="linkedlist">The linked list in which to remove. This cannot be null.</param>
/// <param name="index">The index at which to remove the element. This cannot exceed the length of the linked list.</param>
/// <returns>The state enum value.</returns>
linkedlist_state_t linkedlist_remove(linkedlist_t* linkedlist, unsigned int index) {
    log_debug("Entering linkedlist_remove() at index %d.", index);
    if (linkedlist == NULL) {
        return linkedlist_invalid_args;
    }

    if (index >= linkedlist->length) {
        // Index must be within the bounds of the list.
        return linkedlist_out_of_bounds;
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
    return linkedlist_success;
}

/// <summary>
/// Gets an element from a linked list.
/// </summary>
/// <param name="linkedlist">The linked list in which to get. This cannot be null.</param>
/// <param name="index">The index at which to get the element. This cannot exceed the length of the linked list.</param>
/// <param name="element">The out parameter for the element. This should (not checked) be null when called.</param>
/// <returns>The state enum value.</returns>
linkedlist_state_t linkedlist_get(linkedlist_t* linkedlist, unsigned int index, void** element) {
    log_debug("Entering linkedlist_get() at index %d.", index);
    if (linkedlist == NULL) {
        return linkedlist_invalid_args;
    }

    if (index >= linkedlist->length) {
        // Index must be within the bounds of the list.
        return linkedlist_out_of_bounds;
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
    return linkedlist_success;
}