#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "run.h"

// Main method of this program.
void main (int argc, char *argv[]) {
	if (argc < 2) {
		// Since argv[0] is the current program name (run), and at least
		// one argument is required (the command to execute), the minimum number of
		// argument for this program is 2.
		printf("The minimum number of argument is 2.\n");
		exit(ILLEGAL_ARGUMENTS_ERRNO);
	}
	
	// Create a new process to execute the command in.
	pid_t pid = fork();
	if (pid == 0) {
		// Child process. Execute the command.
        char ** newArgv = argv + 1;
		execvp(newArgv[0], newArgv);
		
		// If exec succeeds, this won't execute.
		exit(COMMAND_EXEC_FAIL_ERRNO);
	} else {
		struct timeval time1, time2;
		gettimeofday(&time1, NULL);
		
		// Parent process. Wait on the child process.
		wait(pid);
        
        // Get all stats of the process.
		gettimeofday(&time2, NULL);
        struct rusage usage;
        getrusage(RUSAGE_CHILDREN, &usage);
		
		// Display stats of execution.
		printf("------------------------\n");
		printf("| pid: %d\n", (int) pid);
		printf("| execution time: %lims\n", getMilliseconds(time2) - getMilliseconds(time1));
        printf("| cpu time: %lims\n", getMilliseconds(usage.ru_stime));
        printf("| involuntary interruption count: %li\n", usage.ru_nivcsw);
        printf("| voluntary interruption count: %li\n", usage.ru_nvcsw);
        printf("| soft page failure count: %li\n", usage.ru_minflt);
        printf("| hard page failure count: %li\n", usage.ru_majflt);
		printf("------------------------\n");
	}
	
	// Success.
	exit(SUCCESSFUL_EXEC);
}

// Get milliseconds from a timeval struct.
long getMilliseconds(struct timeval time) {
	return (time.tv_sec * (long)1000) + (time.tv_usec / (long)1000);
}