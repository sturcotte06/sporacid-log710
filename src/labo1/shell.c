#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "shell.h"
#include <sys/stat.h>

// Prompt for this shell.
char* currentDirectory;

void main (int argc, char *argv[]) {
    if (argc != 1) {
        // Since argv[0] is the current program name (run), only one argument is required.
        printf("The shell must be called without parameter.\n");
        exit(ILLEGAL_ARGUMENTS_ERRNO);
    }
    
    // Make sure the run executable exists.
    if (!fopen(RUN_COMMAND_LOCATION, "r")) {
        printf("Could not find all required executable dependencies.\n");
        exit(EXEC_UNEXISTING_ERRNO);
    }

    // Initialize current directory.
    if (changeDirectory(DEFAULT_DIRECTORY) != SUCCESSFUL_EXEC) {
        printf("Unable to initialize shell to default path.\n");
        exit(DIRECTORY_UNEXISTING_ERRNO);
    }

    // Loop until the user enters the "exit" command.
    while (true) {
        // Print the prompt.
        printf("$ %s ", currentDirectory);

        // Wait for user input.
        char* command = malloc(BUFF_SIZE * sizeof(char));
        if (fgets(command, BUFF_SIZE, stdin) == NULL) {
            printf("Unable to read from standard input.\n");
            exit(STDIN_ERROR_ERRNO);
        }

        // Split the command into an arg vector.
        int argc;
        char** execArgv = malloc(BUFF_SIZE * sizeof(char*));
        char** argv = execArgv + 1;
        splitCommand(command, &argc, argv);
        
        if (strcmp(argv[0], "exit") == 0) {
            // Exit command.
            break;
        } else if (strcmp(argv[0], "cd") == 0) {
            // Change directory command.
            const char* path = argv[1] != NULL ? argv[1] : DEFAULT_DIRECTORY;
            changeDirectory(path);
        } else {
            // Execute command using run command.
            int isBackground = false;
            if (argv[argc - 1][0] == '&'/* && strlen(argv[argc - 1]) == 1*/) {
                // Run as background process.
                isBackground = true;
                // Consume the "&" argument.
                argv[argc - 1] = NULL;
            } 

            pid_t pid = fork();
            if (pid == 0) {
                // Child process.
                execArgv[0] = RUN_COMMAND_LOCATION;
                printf("arg1 %s\n", execArgv[0]);
                printf("arg2 %s\n", execArgv[1]);
                printf("arg3 %s\n", execArgv[2]);
                
                execvp(RUN_COMMAND_LOCATION, execArgv);
                // If exec succeeds, this won't execute.
                exit(COMMAND_EXEC_FAIL_ERRNO);
            } else if (!isBackground) {
                // Only wait if set to be ran synchronously.
                wait(pid);
            }
        }
        
        // Free all memory.
        free(command);
        
        // int i;
        // for (i = 0; i < argc; i++) {
        //     free(argv[i]);
        // }
        // free(argv);
    }

    // Exited successfully.
    exit(SUCCESSFUL_EXEC);
}

// Splits a command into argc and argv.
void splitCommand(char *command, int* argc, char** argv) {
    char* arg = strtok(command, " \n");
    
    int iArg = 0;
    while (arg != NULL) {
        // Copy the arg into argv.
        argv[iArg] = malloc((strlen(arg) + 1) * sizeof(char));
        strcat(argv[iArg], arg);
        strcat(argv[iArg], "\0");
        
        // Find next token.
        iArg++;
        arg = strtok(NULL, " \n");
    }
        
    argc[0] = iArg;
}

// Change current directory of the shell.
int changeDirectory(const char* path) {
    char* newDirectory;
    int newDirectoryLen;
    
    // If the first char is a dot, special case.
    if (path[0] != '/') {
        newDirectoryLen = strlen(currentDirectory) + strlen(path) + 1;
        newDirectory = malloc(newDirectoryLen * sizeof(char));
        
        strcat(newDirectory, currentDirectory);
        strcat(newDirectory, path);
    } else {
        newDirectoryLen = strlen(path) + 1;
        newDirectory = malloc(newDirectoryLen * sizeof(char));
        
        strcat(newDirectory, path);
    }

    // Append null char.
    newDirectory[newDirectoryLen - 1] = '\0';

    // Try to change working directory of process.
    if (chdir(newDirectory) != 0) {
        printf("Path %s is not a directory.\n", newDirectory);
        return DIRECTORY_UNEXISTING_ERRNO;
    }
    
    // struct stat pathInfo;
    // if (stat(newDirectory, &pathInfo) == -1) {
    //     printf("Unable to fetch information of path %s (code: %d)\n", newDirectory, errno);
    //     return UNKNOWN_ERROR_ERRNO;
    // } else if(!S_ISDIR(pathInfo.st_mode)) {
    //     printf("Path %s is not a directory.\n", newDirectory);
    //     return DIRECTORY_UNEXISTING_ERRNO;
    // }
    
    // Release memory of old directory string.
    if (currentDirectory != NULL) {
        free(currentDirectory);
    }
    
    // Change the cached version of the working directory.
    currentDirectory = newDirectory;    
    return SUCCESSFUL_EXEC;
}