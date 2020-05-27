#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_NUM_ARGS 10                                         /* The maximum number of arguments */
#define MAX_ARG_LENGTH 50                                       /* The maximum length of an argument */

// ----------------------------------------------
void firstInit(char *args[]) {
    // when this whole program first run, each memory location will be assigned to NULL

    for (int i = 0; i <= MAX_NUM_ARGS; i++) {
        args[i] = NULL;
    }
}
// ----------------------------------------------
void releaseArgsMemory(char *args[]) {
    // after every loop (should_run == 1), release memory and init *args[] for new command line

    int index = 0;
    while (args[index] != NULL) {
        free(args[index]);
        args[index] = NULL;
    }
}
// ----------------------------------------------
void allocateArgsMemory(char *args[], int number_of_args) {
    // allocate memory for every argument in *args[]

    for (int i = 0; i < number_of_args; i++) {
        args[i] = (char*) malloc(MAX_ARG_LENGTH * sizeof(char));
        for (int j = 0; j < MAX_ARG_LENGTH; j++) {
            args[i][j] = '\0';
        }
    }
}
// ----------------------------------------------
int countArgs(char *args[]) {
    // count the number of arguments in command line

    int count = 0;
    while (args[count] != NULL) {
        count ++;
    }
    return count;
}
// ----------------------------------------------
int countArgsBuf(char *buf) {
    // count the number of arguments in buffer

    int count = 0;
    int length = strlen(buf);
    for (int i = 0; i < length; i++) {
        if (i == 0) {
            count ++;
        }
        else {
            if (buf[i] != ' ' && buf[i - 1] == ' ') {
                count ++;
            }
        }
    }
    return count;
}
// ----------------------------------------------
void bufToArgs(char *buf, char *args[]) {
    /*
    *   buffer[] = 'sudo apt-get update'
    *   --> args[0] = 'sudo'
    *       args[1] = 'apt-get'
    *       args[2] = 'update'
    *       args[3] = NULL
    */

    int number_of_args = countArgsBuf(buf);
    for (int i = 0; i < number_of_args; i++) {

        while (buf[0] == ' ') {
            buf ++;
        }
        int j = 0;
        while (buf[0] != '\0' && buf[0] != '\n' && buf[0] != ' ') {
            args[i][j] = buf[0];
            buf ++;
            j ++;
        }
    }
}
// ----------------------------------------------
void input(char *args[]) {

    char buf[MAX_ARG_LENGTH * MAX_NUM_ARGS];
    fgets(buf, 200, stdin);
    
    int number_of_args = countArgsBuf(buf);
    allocateArgsMemory(args, number_of_args);
    
    bufToArgs(buf, args);
    
    for (int i = 0; i < number_of_args; i++) {
        printf("%s\n", args[i]);
    }
}
// ----------------------------------------------
int main() {

    /*
    *   args: list of argument
    *   ex: args[0] = 'python3'
    *       args[1] = '--version'
    *       args[2] = NULL
    */
    char *args[MAX_NUM_ARGS];
    firstInit(args);

    int should_run = 1;
    while (should_run == 1) {
        printf("oppa:$ ");
        fflush(stdout);
        
        /* (1) fork a child process using fork() */
        // code ...


        /* (2) child process will invoke execvp() */
        input(args);      
        execvp(args[0], args);


        /* (3) if command included &, parent will not invoke wait() */
        // code ...


        /* release memory allocation after using */
        releaseArgsMemory(args);
    }
    return 0;
}