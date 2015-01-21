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
#include <math.h>
#include <time.h>
#include "shell.h"

// Prompt for this shell.
char* currentDirectory;

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

    // Seed randoms.
    srand(time(NULL));
    
    // Initialize background processes buffer.
    bgtask *bgTasks = malloc(BUFF_SIZE * sizeof(bgtask));
    int iBgTasks = 0;
    
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
        char** argv = execArgv + 1;
        
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
            listBackgroundTasks(bgTasks, iBgTasks);
        } else {
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
        
            // Default case: execute command using run command.
            int isBackground = false;
            if (argv[argc - 1][0] == '&' && strlen(argv[argc - 1]) == 1) {
                // Run as background process.
                isBackground = true;
                // Consume the "&" argument.
                argv[argc - 1] = NULL;
            } 

            // Create a new process to run the command in.
            pid_t pid = fork();
            
            if (pid == 0) {
                if (isBackground) {
                    // Redirect stdout to a temp file.
                    char* filename = calloc(BUFF_SIZE, sizeof(char));
                    sprintf(filename, "%d", rand());
                    char* filepath = combinePath(TEMP_PATH, strlen(TEMP_PATH), filename, strlen(filename));
                    // free(filename);
                    printf("%s\n", filepath);
                    freopen(filepath, "w", stdout);
                }
            
                // Child process.
                execArgv[0] = RUN_COMMAND_PATH;                
                execvp(RUN_COMMAND_PATH, execArgv);
                // If exec succeeds, this won't execute.
                exit(COMMAND_EXEC_FAIL_ERRNO);
            } else if (isBackground) {
                // Make a copy of task name, because we will free argv later on.
                char* taskName = calloc(strlen(argv[0]), sizeof(char));
                strcpy(taskName, argv[0]);         

                // Create a new background task struct.
                volatile bgtask task;
                task.id = iBgTasks + 1;
                task.pid = pid;
                task.name = taskName;
                task.finished = false;
                
                printf("%d\n", (int) &task);
                
                // Add the background task to the buffer.
                // Don't mind buffer overflow. I don't think this shell will live up to see that day.
                bgTasks[iBgTasks++] = task;
                
                // Meh, create a thread to turn the finished flag on when process is done.
                pthread_t thread;
                pthread_create(&thread, NULL, switchFinishedFlag, &task);
            } else {
                // Only wait if set to be ran synchronously.
                wait(pid);
            }
        }
                
        /*// Free all memory used for execution of this loop iteration.
        if (execArgv != NULL) {
            int iArgv;
            for (iArgv = 0; iArgv < argc + 1; iArgv++) {
                if (argv[iArgv] != NULL) {
                    free(argv[iArgv]);
                }
            }
            
            free(execArgv);
        }
        
        if (command != NULL) {
            free(command);
        }*/   
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
    int newDirectoryLen, currentDirectoryLen, pathLength = strlen(path);
    
    // If the first char is a dot, special case.
    if (path[0] != '/') {
        combinePath(currentDirectory, currentDirectoryLen, path, pathLength);
    } else {
        newDirectoryLen = pathLength + 1;
        newDirectory = calloc(newDirectoryLen, sizeof(char));
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

// List all background processes currently active.
int listBackgroundTasks(bgtask* bgTasks, int count) {
    int i;
    for (i = 0; i < count; i++) {
        bgtask task = bgTasks[i];
        
        // Format the task.
        printf("  [%d] %d %s %s\n",
            task.id,
            (int) task.pid,
            task.name,
            (task.finished ? "finished" : "running"));
    }
}

// Simple method to be used asynchronously in a thread.
void *switchFinishedFlag(void *task) {
    bgtask* bgTask = (bgtask *) task;

    // Wait on the task process.
    wait(bgTask->pid);
    
    // Set the flag to true.
    bgTask->finished = true;
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
    char* path;
    if (left[leftLen - 2] != '/' && right[0] != '/') {
        path = calloc(leftLen + rightLen + 2, sizeof(char));
        sprintf("%s/%s", left, right);
        // memcpy(path, left, leftLen);
        // strcat(path, "/");
        // memcpy(path, right, rightLen);
    } else {
        char* realRight = right;
        if (left[leftLen - 2] == '/' && right[0] == '/') {
            realRight++;
        }
    
        path = calloc(leftLen + rightLen + 1, sizeof(char));
        sprintf("%s%s", left, realRight);
        // memcpy(path, left, leftLen);
        // memcpy(path, right, rightLen);
    }
    
    return path;
}