#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "scheduler.h"

// Buffer for all resources.
void resource_t[];

// 1: lire fichier list process
// 2: process temps réel
// 3: process usager
// 4: ressources

// Main method that will execute the scheduler.
void main(int argc, char* argv[]) {
	// We only receive 1 argument: the name of the process list file.
	if (argc != 2) {
		exit(ILLEGAL_ARGUMENTS_ERRNO);
	}
	
	// Get the only argument (The process list file name).
	char* processListFile = argv[1];
}

int init_scheduler() {
	
}

int init_resources(void* resources) {
	
}