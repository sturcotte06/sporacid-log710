// Unit test methods for the linked list collection.
void test_linkedlist_init(testresults_t* results, linkedlist_t* linkedlist);
void test_linkedlist_addall(testresults_t* results, linkedlist_t* linkedlist);
void test_linkedlist_getall(testresults_t* results, linkedlist_t* linkedlist);
void test_linkedlist_removeone(testresults_t* results, linkedlist_t* linkedlist, int index);
void test_linkedlist_removeall(testresults_t* results, linkedlist_t* linkedlist);
void test_linkedlist_destroy(testresults_t* results, linkedlist_t* linkedlist);

// Unit test methods for the queue collection.
void test_queue_init(testresults_t* results, queue_t* queue);
void test_queue_queueall(testresults_t* results, queue_t* queue);
void test_queue_dequeueall(testresults_t* results, queue_t* queue);
void test_queue_destroy(testresults_t* results, queue_t* queue);

// Utility methods relative to tests.
void test_linkedlist_printlist(linkedlist_t* linkedlist);
void test_linkedlist_rprintlist(linkedlist_t* linkedlist);