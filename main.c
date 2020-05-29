/*
 * HCMUS/CS333-Introduction to operating system/project02-simple shell
 * 
 * 1751078 - Nguyễn Võ Đức Lộc  - 17CTT2
 * 1751012 - Nguyễn Chấn Nam    - 17CTT2
 * 1751058 - Lê Hoàng Đạt       - 17CTT1
 * 
 */

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<wait.h>
#include <fcntl.h>

#define MAX_NUM_ARGS 10                                         /* The maximum number of arguments */
#define MAX_ARG_LENGTH 50                                       /* The maximum length of an argument */

// --------------------------------------------------------------------------------------------------------
void firstInit(char *args[]) {
    // when this whole program first run, each memory location will be assigned to NULL

    for (int i = 0; i <= MAX_NUM_ARGS; i++) {
        args[i] = NULL;
    }
}
// --------------------------------------------------------------------------------------------------------
void releaseArgsMemory(char *args[]) {

    int index = 0;
    while (args[index] != NULL) {
        free(args[index]);
        args[index] = NULL;
    }
}
// --------------------------------------------------------------------------------------------------------
void allocateArgsMemory(char *args[], int number_of_args) {
    // allocate memory for every argument in *args[]

    for (int i = 0; i < number_of_args; i++) {
        args[i] = (char*) malloc(MAX_ARG_LENGTH * sizeof(char));
        for (int j = 0; j < MAX_ARG_LENGTH; j++) {
            args[i][j] = '\0';
        }
    }
}
// --------------------------------------------------------------------------------------------------------
int countArgs(char *args[]) {
    // count the number of arguments in command line

    int count = 0;
    while (args[count] != NULL) {
        count ++;
    }
    return count;
}
// --------------------------------------------------------------------------------------------------------
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
// --------------------------------------------------------------------------------------------------------
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
// --------------------------------------------------------------------------------------------------------
int getLastCharIndex(char buf[], char chr) {
    /*
     *  buf = 'sudo apt get update'
     *  chr = ' '
     *  getLastCharIndex return 12
     */

    int len = strlen(buf);
    for (int i = len - 1; i >= 0; i--) {
        if (buf[i] == chr) {
            return i;
        }
    }
    return -1;
}
// --------------------------------------------------------------------------------------------------------
void loadFromHistory(char buf[]) {
    
    FILE *file = fopen(".history", "r");
    if (!file) {
        /* file does not exist, create it first */

        file = fopen(".history", "w");
        fclose(file);
        printf("No commands in history!\n");
        exit(0);
    }

    /* read the last line in history */
    char temp_buf[MAX_ARG_LENGTH * MAX_NUM_ARGS];
    fseek(file, -MAX_ARG_LENGTH * MAX_NUM_ARGS, SEEK_END);
    int len = read(fileno(file), temp_buf, MAX_ARG_LENGTH * MAX_NUM_ARGS);
    temp_buf[len] = '\0';
    
    if (len < 1) {
        printf("No command in history!\n");
        exit(0);
    }

    int lastLineIndex = getLastCharIndex(temp_buf, '\n') + 1;
    for (int i = lastLineIndex; i < len; i++) {
        buf[i - lastLineIndex] = temp_buf[i];
    }
    buf[len] = '\0';

    printf("%s\n", buf);
}
// --------------------------------------------------------------------------------------------------------
void saveToHistory(char buf[]) {

    /* delete last \n of buffer */
    int len = strlen(buf);
    for (int i = len - 1; i > 0; i--)
        if (buf[i] == '\n') {
            buf[i] = '\0';
            break;
        }

    FILE *file = fopen(".history", "a");
    if (!file) {
        /* if file does not exist, create it first */
        file = fopen(".history", "w");
        fclose(file);
        file = fopen(".history", "a");
    }

    fprintf(file, "\n%s", buf);
    fclose(file);
}
// --------------------------------------------------------------------------------------------------------
void input(char *args[]) {

    char buf[MAX_ARG_LENGTH * MAX_NUM_ARGS];
    fgets(buf, 200, stdin);
    
    if (buf[0] == '!' && buf[1] == '!') {
        /* load command line from .history */
        loadFromHistory(buf);
    }
    else {
        saveToHistory(buf);
    }
    
    int number_of_args = countArgsBuf(buf);
    allocateArgsMemory(args, number_of_args);
    
    bufToArgs(buf, args);
}
// --------------------------------------------------------------------------------------------------------
int getCharIndex(char *args[], char chr) {

    /*
     *  args[0] = 'ls'
     *  args[1] = '>'
     *  args[2] = 'output.txt'
     *  args[3] = NULL
     *  getCharIndex(args, '>') --> return 1
     */

    int number_of_args = countArgs(args);
    for (int i = 0; i < number_of_args; i++) {
        if (args[i][0] == chr) {
            return i;
        }
    }
    return -1;
}
// --------------------------------------------------------------------------------------------------------
int isInputRedirecting(char *args[]) {
    /*
     *  check if '>' exists in args
     *  ex: args[0] = 'ls'
     *      args[1] = '>'
     *      args[2] = 'output.txt'
     *      args[3] = NULL
     *  --> return True
     */
    int number_of_args = countArgs(args);
    for (int i = 0; i < number_of_args; i++) {
        if (args[i][0] == '>') {
            return 1;
        }
    }
    return 0;
}
// --------------------------------------------------------------------------------------------------------
int isOutputRedirecting(char *args[]) {
    /* 
        check if '<' exists in shell
        ex: args[0] = 'ls'
            args[1] = '>'
            args[2] = 'output.txt'
            args[3] = NULL
            --> return True
    */
    int number_of_args = countArgs(args);
    for (int i = 0; i < number_of_args; i++) {
        if (args[i][0] == '<') {
            return 1;
        }
    }
    return 0;
}
// --------------------------------------------------------------------------------------------------------
int isPipe(char *args[]) {
    /*
     *  check if '|' exist in shell
     */
    int number_of_args = countArgs(args);
    for (int i = 0; i < number_of_args; i++) {
        if (args[i][0] == '|') {
            return 1;
        }
    }
    return 0;
}
// --------------------------------------------------------------------------------------------------------
void inputRedirectingExec(char *args[]) {
    /* input redirection shell execution */

    int index = getCharIndex(args, '>');
    args[index] = NULL;
    
    int fd = open(args[index + 1], O_WRONLY);
    if (fd < 0) {
        // when the file does not exist, we have to create it first

        FILE *fp = fopen(args[index + 1], "w");
        fclose(fp);

        fd = open(args[index + 1], O_WRONLY);
    }
    dup2(fd, STDOUT_FILENO);
    execvp(args[0], args);

    // if execvp failed, release memory and close manually
    releaseArgsMemory(args);
    close(fd);
}
// --------------------------------------------------------------------------------------------------------
void outputRedirectingExec(char *args[]) {
    /* output redirection shell execution */

    int index = getCharIndex(args, '<');
    args[index] = NULL;

    int fd = open(args[index + 1], O_RDONLY);
    if (fd < 0) {
        // when the file does not exist, nothing to read, we exit and do nothing
        releaseArgsMemory(args);
        exit(0);
    }
    else {
        dup2(fd, STDIN_FILENO);
        execvp(args[0], args);

        // if execvp failed, release memory and close manually
        releaseArgsMemory(args);
        close(fd);
    }
}
// --------------------------------------------------------------------------------------------------------
void pipeExec(char *args[]) {

    int fd[2];
    char buf[200];
    pipe(fd);

    int index = getCharIndex(args, '|');
    args[index] = NULL;
    
    if (fork() == 0) {
        // children's children process
        close(fd[0]);

        /* copy values from stdandard output (screen) to file fd[1] */
        dup2(fd[1], STDOUT_FILENO);

        /* run command before | */
        execvp(args[0], args);

        /* if execvp failed, release memory */
        printf("left command failed!\n");
        releaseArgsMemory(args);
    }
    else {
        // chilren process
        wait(NULL);
        close(fd[1]);

        /* 
         * copy value from fd[0] to standard input (STDIN) 
         * the grep command will read input from STDIN and return result on screen
         */
        dup2(fd[0], STDIN_FILENO);

        /* run grep command */
        execvp(args[index + 1], &args[index + 1]);

        /* if execvp failed, release memory */
        printf("grep command failed!\n");
        releaseArgsMemory(args);
    }
}
// --------------------------------------------------------------------------------------------------------
void executeArgs(char *args[]) {

    if (isInputRedirecting(args)) {
        /* input redirection shell */

        inputRedirectingExec(args);
    }
    else
    if (isOutputRedirecting(args)) {
        /* output redirection shell */

        outputRedirectingExec(args);
    }
    else 
    if (isPipe(args)) {
        /* pipe shell */

        pipeExec(args);
    }
    else {
        /* simple shell */

        execvp(args[0], args);
    }
}
// --------------------------------------------------------------------------------------------------------
int main() {

    /*
    *   args: list of argument
    *   ex: args[0] = 'python3'
    *       args[1] = '--version'
    *       args[2] = NULL
    */

    int should_run = 1;
    while (should_run == 1) {

        int pid = fork();
        if (pid > 0) {
            // this is parent process
            printf("oppa:$ ");
            fflush(stdout);

            // parent will wait until child finished
            wait(NULL);
        }
        else {
            // this is child process
            char *args[MAX_NUM_ARGS];
            firstInit(args);

            input(args);
            executeArgs(args);

            /*
                if the execvp run successfully, no need to deallocate memory
                However, if the command does not exists, then execvp will return this current process
                Therefore, we have to release memory allocation manually
            */
            printf("error, command does not exist\n");
            releaseArgsMemory(args);
            exit(1);
        }
    }
    return 0;
}