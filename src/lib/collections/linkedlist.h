#ifndef LIB_COLLECTIONS_LINKEDLIST_H
#define LIB_COLLECTIONS_LINKEDLIST_H

// Enum for the linked list possible function states.
typedef enum linkedlist_state_t {
	linkedlist_success,
	linkedlist_invalid_args,
	linkedlist_out_of_bounds
} linkedlist_state_t;

// Structure for a double linked list node. Every node points to the previous and next node.
// If the next node is NULL, then this node is the last.
typedef struct node_t node_t;
struct node_t {
	void* element;
    node_t* previous;
	node_t* next;
};

// Structure for a head-tail linked list.
typedef struct linkedlist_t {
	node_t* head;
	node_t* tail;
    unsigned int length;
} linkedlist_t;

/// <summary>
/// Initializes an linked list.
/// </summary>
/// <param name="linkedlist">The linked list to initialize. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
linkedlist_state_t linkedlist_init(linkedlist_t* linkedlist);

/// <summary>
/// Destroys a linked list and all used memory. The linked list structure does not belong to this module;
/// The callee should deal with the structure memory itself.
/// </summary>
/// <param name="linkedlist">The linked list to destroy. This cannot be null.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
linkedlist_state_t linkedlist_destroy(linkedlist_t* linkedlist);

/// <summary>
/// Adds an element to a linked list at the specified index.
/// </summary>
/// <param name="linkedlist">The linked list in which to add. This cannot be null.</param>
/// <param name="index">The index at which to add the element. This cannot exceed the length of the linked list.</param>
/// <param name="element">The element to add.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
linkedlist_state_t linkedlist_add(linkedlist_t* linkedlist, unsigned int index, void* element);

/// <summary>
/// Removes the element at the specified index from a linked list.
/// </summary>
/// <param name="linkedlist">The linked list in which to remove. This cannot be null.</param>
/// <param name="index">The index at which to remove the element. This cannot exceed the length of the linked list.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
linkedlist_state_t linkedlist_remove(linkedlist_t* linkedlist, unsigned int index);

/// <summary>
/// Gets an element from a linked list.
/// </summary>
/// <param name="linkedlist">The linked list in which to get. This cannot be null.</param>
/// <param name="index">The index at which to get the element. This cannot exceed the length of the linked list.</param>
/// <param name="element">The out parameter for the element. This should (not checked) be null when called.</param>
/// <returns>Value of SUCCESSFUL_EXEC if successful.</returns>
linkedlist_state_t linkedlist_get(linkedlist_t* linkedlist, unsigned int index, void** element);

#endif