#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "shell.h"

// Current working directory.
char* currentDirectory;

// Buffer of all background tasks.
bgtask *tasks;

// Count of tasks buffer.
int iTasks;

// Main method that will execute the shell.
void main (void) {    
    // Make sure the run executable exists.
    if (!fopen(RUN_COMMAND_PATH, "r")) {
        printf("Could not find all required executable dependencies.\n");
        exit(EXEC_UNEXISTING_ERRNO);
    }

    // Initialize current directory.
    if (changeDirectory(DEFAULT_DIRECTORY_PATH) != SUCCESSFUL_EXEC) {
        printf("Unable to initialize shell to default path.\n");
        exit(DIRECTORY_UNEXISTING_ERRNO);
    }
    
    // Initialize background processes buffer.
    tasks = malloc(BUFF_SIZE * sizeof(bgtask));
    iTasks = 0;
    
    // Loop until the user enters the "exit" command.
    while (true) {
        // Print the prompt.
        printf("$ %s ", currentDirectory);

        // Wait for user input.
        char* command = calloc(BUFF_SIZE, sizeof(char));
        if (fgets(command, BUFF_SIZE, stdin) == NULL) {
            printf("Unable to read from standard input.\n");
            exit(STDIN_ERROR_ERRNO);
        } else if(command[0] == '\n') {
            // If input is only a return carriage, then user inputed nothing.
            continue;
        }

        // Declare memory for command split.
        int argc;
        char** execArgv = calloc(BUFF_SIZE, sizeof(char*));
        char** argv = execArgv + 3;
        
        // Split the command into an arg vector.
        splitCommand(command, &argc, argv);
        free(command);
                        
        // Switch on command name.
        if (strcmp(argv[0], "exit") == 0) {
            // Exception case: exit command.
            break;
        } else if (strcmp(argv[0], "cd") == 0) {
            // Exception case: change directory command.
            const char* path = argv[1] != NULL ? argv[1] : DEFAULT_DIRECTORY_PATH;
            changeDirectory(path);
        } else if (strcmp(argv[0], "aptaches") == 0) {
            // Exception case: list all background tasks.
            listBackgroundTasks(tasks, iTasks);
        } else if (strcmp(argv[0], "otaches") == 0) {
            // Exception case: output to screen the stdout of given bgtask.
            if (argv[1] == NULL) {
                printf("otaches requires a task id as argument.\n");
            } else {
                int id = atoi(argv[1]);
                if (id > 0 && id < iTasks + 1) {
                    bgtask* task = &tasks[id - 1];
                    
                    // Cheap implementation: call cat on the stdout path of the bgtask.
                    argv[0] = calloc (4, sizeof(char));
                    argv[1] = calloc (strlen(task->stdout) + 1, sizeof(char));
                    strcpy(argv[0], "cat");
                    strcpy(argv[1], task->stdout);
                    
                    runTask(5, execArgv);
                } else {
                    printf("otaches requires an existing task id as argument.\n");
                }
            }
        } else {
            // Consume the background flag.
            int isBackground = argv[argc - 1][0] == '&' && strlen(argv[argc - 1]) == 1;
            if (isBackground) {
                // Consume the "&" argument.
                argv[argc - 1] = NULL;
                argc--;
            }
        
            // Exception case: ls without directory means ls for the current directory.
            if (strcmp(argv[0], "ls") == 0 && argc == 1) {
                // We have to add a new argument for the current dictionary.
                int commandLen = strlen(currentDirectory) + 3;
                argv[1] = malloc(commandLen * sizeof(char));
                
                // Copy the current directory in argv.
                strcpy(argv[1], currentDirectory);
                argv[1][commandLen - 1] = '\0';
                argc++;
            }
        
            // Default case: execute the command.
            if (isBackground) {                
                // Run asynchronously.
                runBackgroundTask(argc + 3, execArgv);
            } else {
                // Run synchronously.
                runTask(argc + 3, execArgv);
            }
        }
                
        // Free all memory used for execution of this loop iteration.
        if (execArgv != NULL) {
            int iArgv;
            for (iArgv = 0; iArgv < argc + 1; iArgv++) {
                if (argv[iArgv] != NULL) {
                    free(argv[iArgv]);
                }
            }
            
            free(execArgv);
        }
    }
    
    // Exited successfully.
    exit(SUCCESSFUL_EXEC);
}

// Splits a command into argc and argv.
void splitCommand(char *command, int* argc, char** argv) {
    // Trim all trailing characters.
    int commandLen = strlen(command);
    while (command[commandLen - 1] == '\n') {
        command[commandLen - 1] = '\0';
        commandLen--;
    }
    
    // Tokenize the command.
    int iArg = 0;
    char* arg = strtok(command, " ");
    while (arg != NULL) {
        // Copy the arg into argv.
        argv[iArg] = malloc((strlen(arg) + 1) * sizeof(char));
        strcpy(argv[iArg], arg);
        
        // Find next token.
        iArg++;
        arg = strtok(NULL, " ");
    }
        
    argc[0] = iArg;
}

// Change current directory of the shell.
int changeDirectory(const char* path) {
    char* newDirectory;
    int pathLen = strlen(path);
    
    // If the first char is a dot, special case.
    if (path[0] != '/') {
        newDirectory = combinePath(currentDirectory, strlen(currentDirectory), path, pathLen);
    } else {
        newDirectory = calloc(pathLen + 1, sizeof(char));
        strcat(newDirectory, path);
    }

    // Try to change working directory of process.
    if (chdir(newDirectory) != 0) {
        printf("Path %s is not a directory.\n", newDirectory);
        return DIRECTORY_UNEXISTING_ERRNO;
    }
        
    // Release memory of old directory string.
    if (currentDirectory != NULL) {
        free(currentDirectory);
    }
    
    // Change the cached version of the working directory.
    currentDirectory = newDirectory;
    return SUCCESSFUL_EXEC;
}

// Runs a task synchronously.
void runTask(int argc, char** argv) {
    argv[0] = RUN_COMMAND_PATH;
    // Redirect stdout of child process to screen.
    argv[1] = "-o";
    argv[2] = "/dev/tty";
    
    // Create a new process to run the task in.
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process.           
        execvp(RUN_COMMAND_PATH, argv);
        // If exec succeeds, this won't execute.
        exit(COMMAND_EXEC_FAIL_ERRNO);
    } 
        
    // Parent process.
    wait(pid);
}

// Runs a task asynchronously.
void runBackgroundTask(int argc, char** argv) {
    argv[0] = RUN_COMMAND_PATH;
    
    // Create a temp file with arbitrary name.
    char* filename = calloc(BUFF_SIZE, sizeof(char));
    sprintf(filename, "%d.out", (unsigned)time(NULL));
    char* filepath = combinePath(TEMP_PATH, strlen(TEMP_PATH), filename, strlen(filename));
    free(filename);
    
    // Redirect stdout of child process to temp file.
    argv[1] = "-o";
    argv[2] = filepath;

    // Create a new process to run the task in.
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process. Run the task.
        execvp(RUN_COMMAND_PATH, argv);
        
        // If exec succeeds, this won't execute.
        exit(COMMAND_EXEC_FAIL_ERRNO);
    }
    
    // Make a copy of task name and stdout, because we will free argv later on.
    char* taskName = calloc(strlen(argv[3]) + 1, sizeof(char));
    strcpy(taskName, argv[3]);

    // Create a new background task struct.
    bgtask task;
    task.id = iTasks + 1;
    task.pid = pid;
    task.name = taskName;
    task.stdout = filepath;
    task.finished = false;
    
    // Create a mutex to synchronize the main thread and the newly created thread.
    pthread_mutex_init(&(task.mutex), NULL);
    
    // Add the background task to the buffer.
    // Don't mind buffer overflow. I don't think this shell will live up to see that day.
    tasks[iTasks++] = task;
    
    // Create a thread to turn the finished flag on when process is done.    
    int id = task.id;
    pthread_t thread;
    pthread_create(&thread, NULL, switchFinishedFlag, (void*)(id));
    
    // Show the task to the user.
    printBackgroundTask(&task);
}

// List all background processes currently active.
int listBackgroundTasks() {
    int i;
    for (i = 0; i < iTasks; i++) {
        printBackgroundTask(&tasks[i]);
    }
}

// Prints a background task.
void printBackgroundTask(bgtask* task) {
    // Format the task.
    pthread_mutex_lock(&(task->mutex));
    printf("  [%d] %d %s %s %s\n",
        task->id,
        (int) task->pid,
        task->name,
        (task->finished ? "finished" : "running"),
        task->stdout);
    pthread_mutex_unlock(&(task->mutex));
}

// Simple method to be used asynchronously in a thread.
void *switchFinishedFlag(void *id) {
    // Get the task to wait for.
    int tid = (int)((intptr_t)(id));
    bgtask* task = &tasks[tid - 1];

    // Wait on the task process.
    wait(task->pid);
    
    // Set the flag to true.
    pthread_mutex_lock(&(task->mutex));
    task->finished = true;
    pthread_mutex_unlock(&(task->mutex));
}

// Utility method to print the bytes of a string.
void printBytes(char* str) {
    printf("\n");
    int i = 0, len = strlen(str);
    while (i < len) {
        printf("%d ", (int) str[i]);
        i++;
    }
    printf("\n");
}

// Combine two paths together.
char* combinePath(const char* left, int leftLen, const char* right, int rightLen) {
    int mustAddSlash = left[leftLen - 1] != '/' && right[0] != '/';
    char* path;
    if (mustAddSlash) {
        path = calloc(leftLen + rightLen + 2, sizeof(char));
        sprintf(path, "%s/%s", left, right);
    } else {
        
        int mustRemoveSlash = left[leftLen - 1] == '/' && right[0] == '/';  
        if (mustRemoveSlash) {
            path = calloc(leftLen + rightLen, sizeof(char));
            sprintf(path, "%s%s", left, right + 1);
        } else {
            path = calloc(leftLen + rightLen + 1, sizeof(char));
            sprintf(path, "%s%s", left, right);
        }
    }
    
    return path;
}