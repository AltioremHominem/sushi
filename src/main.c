#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

static void cd(char **args){

    if (args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"cd\"\n");

    } else {

        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

static bool execute(char **args){

    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {

        if (execvp(args[0], args) == -1) {

            fprintf(stderr, "execvp:  error\n");

        }
        exit(0);
    } else if (pid < 0) {

        fprintf(stderr, "execvp:  error\n");

    } else {
                
        wpid = waitpid(pid, &status, WUNTRACED);

        while (!WIFEXITED(status) && !WIFSIGNALED(status)){

            wpid = waitpid(pid, &status, WUNTRACED);

        }
    }

  return 1;
}

// Execute a program or Shell Builtins
static bool launch(char **args){

    if (strcmp(args[0] , "exit" )== 0) {
        return 0;
    }

    if (strcmp(args[0] , "cd" )== 0) {
        cd(args);
        return 1;
    }

    return execute(args);
}

static char **splitLines(char *line){

    const int TOK_BUFSIZE = 64;
    const char *TOK_DELIM = " \t\r\n\a";
    int bufsize = TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(0);
    }

    token = strtok(line, TOK_DELIM);

    while (token != NULL){
        tokens[position] = token;
        position++;

        if (position >= bufsize) {

            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) {
                fprintf(stderr, "malloc: allocation error\n");
                exit(0);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

static char *readLine(void) {

    char *line = NULL;
    size_t bufsize = 0; 

    if (getline(&line, &bufsize, stdin) == -1){
        if (feof(stdin)) {
            exit(1);  
        } else  {
            fprintf(stderr, "readline:  error\n");
            exit(0);
        }
    }

    return line;

}

static void sushiLoop(){
    char *line;
    char **args;
    bool shStatus = 1;

    while (shStatus){
        printf(">");

        line = readLine();
        args = splitLines(line);
        shStatus = launch(args);

        free(line);
        free(args);

    }
    
}


int main(int argc, char** argv) {

    sushiLoop();

    return 0;
}