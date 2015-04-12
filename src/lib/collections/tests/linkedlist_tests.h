#ifndef LIB_COLLECTIONS_TESTS_LINKEDLIST_TESTS_H
#define LIB_COLLECTIONS_TESTS_LINKEDLIST_TESTS_H

// Unit test methods for the linked list collection.
void test_linkedlist_init(testresults_t* results, linkedlist_t* linkedlist);
void test_linkedlist_addall(testresults_t* results, linkedlist_t* linkedlist);
void test_linkedlist_addone(testresults_t* results, linkedlist_t* linkedlist, int index);
void test_linkedlist_getall(testresults_t* results, linkedlist_t* linkedlist);
void test_linkedlist_removeone(testresults_t* results, linkedlist_t* linkedlist, int index);
void test_linkedlist_removeall(testresults_t* results, linkedlist_t* linkedlist);
void test_linkedlist_destroy(testresults_t* results, linkedlist_t* linkedlist);

// Utility methods relative to tests.
void test_linkedlist_print(linkedlist_t* linkedlist);
void test_linkedlist_reverse_print(linkedlist_t* linkedlist);

#endif