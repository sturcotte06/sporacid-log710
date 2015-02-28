#include <stdio.h>
#include <stdlib.h>
#include "logging.h"
#include "collections.h"

const int TEST_SIZE = 10;

int main(void) {
    linkedlist_t linkedlist;
    queue_t queue;
    
    // Tests linked list.
    test_linkedlist_init(&linkedlist);
    test_linkedlist_addall(&linkedlist);
    test_linkedlist_printlist(&linkedlist);
    test_linkedlist_rprintlist(&linkedlist);
    
    test_linkedlist_getall(&linkedlist);
    
    test_linkedlist_removeone(&linkedlist, 3);
    test_linkedlist_removeone(&linkedlist, 7);
    test_linkedlist_printlist(&linkedlist);
    test_linkedlist_rprintlist(&linkedlist);
    
    test_linkedlist_removeall(&linkedlist);
    test_linkedlist_printlist(&linkedlist);
    test_linkedlist_rprintlist(&linkedlist);
    
    test_linkedlist_destroy(&linkedlist);
}

void test_linkedlist_init(linkedlist_t* llist) {
    if(linkedlist_init(&linkedlist) != SUCCESSFUL_EXEC) {
        logft(ERROR_LVL, "test_linkedlist_init(): Initialization of linked list returned wrong value.");
    }
}

void test_linkedlist_addall(linkedlist_t* llist) {
    int i;
    node_t *cnode;
    
    logft(DEBUG_LVL, "test_linkedlist_addall(): Beginning add all...");
    for (i = 0; i < TEST_SIZE; i++) {
        char* buff = malloc(256 * sizeof(char));
        sprintf(buff, "Element #%d", i + 1);
    
        if (linkedlist_add(&linkedlist, i, buff) != SUCCESSFUL_EXEC) {
            logft(ERROR_LVL, "test_linkedlist_addall(): Add of linked list element returned wrong value.");
        }
    }
    logft(DEBUG_LVL, "test_linkedlist_addall(): Add all succeeded.");
}

void test_linkedlist_getall(linkedlist_t* llist) {
    logft(DEBUG_LVL, "test_linkedlist_getall(): Beginning get all...");
    for (i = 0; i < llist->length; i++) {
        void* element = NULL;
        if (linkedlist_get(&linkedlist, i, &element) != SUCCESSFUL_EXEC) {
            logft(ERROR_LVL, "test_linkedlist_getall():  Get of linked list element returned wrong value.");
        }
        
        logft(DEBUG_LVL, "Element found: %s", (char*) element);
    }
    logft(DEBUG_LVL, "test_linkedlist_getall(): Get all succeeded.");
}

void test_linkedlist_removeone(linkedlist_t* llist, int index) {
    logft(DEBUG_LVL, "test_linkedlist_removeone(): Beginning removal of one element #%d...", index);
    if(linkedlist_remove(&linkedlist, index) != SUCCESSFUL_EXEC) {
        logft(ERROR_LVL, "test_linkedlist_removeone(): Removal of linked list element #%d returned wrong value.", index);
    }
    
    logft(DEBUG_LVL, "test_linkedlist_removeone(): Remove one succeeded.");
}

void test_linkedlist_removeall(linkedlist_t* llist) {
    logft(DEBUG_LVL, "test_linkedlist_removeall(): Beginning remove all...");
    for (i = 0; i < llist->length;) {
        if (linkedlist_remove(&linkedlist, 0) != SUCCESSFUL_EXEC) {
            logft(ERROR_LVL, "test_linkedlist_removeall(): Get of linked list element returned wrong value.");
        }
    }
    logft(DEBUG_LVL, "test_linkedlist_removeall(): Remove all succeeded...");
}

void test_linkedlist_destroy(linkedlist_t* llist) {
    logft(DEBUG_LVL, "test_linkedlist_destroy(): Beginning destroy...");
    if(linkedlist_destroy(&linkedlist) != SUCCESSFUL_EXEC) {
        logft(ERROR_LVL, "test_linkedlist_destroy(): Destroy of linked list returned wrong value.");
    }
    logft(DEBUG_LVL, "test_linkedlist_destroy(): Destroy succeeded...");
}

void test_linkedlist_printlist(linkedlist_t* llist) {
    logft(DEBUG_LVL, "test_linkedlist_printlist(): Printing list of elements.");
    cnode = linkedlist.head;
    while (cnode != NULL) {
        logft(DEBUG_LVL, "%s", (char*) cnode->element);
        cnode = cnode->next;
    }
}

void test_linkedlist_rprintlist(linkedlist_t* llist) {
    logft(DEBUG_LVL, "test_linkedlist_rprintlist(): Printing reverse list of elements.");
    cnode = linkedlist.tail;
    while (cnode != NULL) {
        logft(DEBUG_LVL, "%s", (char*) cnode->element);
        cnode = cnode->previous;
    }
}

// Unit tests for the queue.
void test_queue() {
    
}