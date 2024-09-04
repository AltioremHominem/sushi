#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>

static void showHelp() {
    printf("Help Message");


}

static void showHistory() {
    FILE* fd = NULL;
    const char *HOME_ENV_VAR = getenv("HOME");
    char filePath[PATH_MAX];

    if (HOME_ENV_VAR == NULL) {
        perror("getenv");
        exit(EXIT_FAILURE);
    }

    snprintf(filePath, sizeof(filePath), "%s/.sushi_history", HOME_ENV_VAR);

    fd = fopen(filePath, "r+");

    if (fd == NULL) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    fgets(filePath,sizeof(filePath),fd);

    printf("%s\n",filePath);

    fclose(fd);

}

/// Writes on a file that contains history
static void writeOnHistory(char *line){
    FILE* fd = NULL;   
    const char *HOME_ENV_VAR = getenv("HOME");
    char filePath[PATH_MAX];

    if (HOME_ENV_VAR == NULL) {
        perror("getenv");
        exit(EXIT_FAILURE);
    }

    snprintf(filePath, sizeof(filePath), "%s/.sushi_history", HOME_ENV_VAR);

    fd = fopen(filePath, "a+");

    if (fd == NULL) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    fprintf(fd,"%s\n ", line);

    fclose(fd);
}

/// Change Directory Builtin
static void changeDirectory( char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

/// Creates a child process to execute the command
static bool executeCommand( char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("fork");
    } else {
        waitpid(pid, &status, WUNTRACED);
        while (!WIFEXITED(status) && !WIFSIGNALED(status)) {
            waitpid(pid, &status, WUNTRACED);
        }
    }

    return true;
}


/// Launch a builtin or a command + args
static bool launchBuiltIns(char **args) {
    if (args[0] == NULL) {
        return true; // Empty command
    }

    if (strcmp(args[0], "exit") == 0) { // Exit Builtin
        return false;
    }

    if (strcmp(args[0], "cd") == 0) {
        changeDirectory(args);
        return true;
    }

    if (strcmp(args[0], "history") == 0) {
        showHistory();
        return true;
    }

    if (strcmp(args[0], "help") == 0) {
        showHelp();
        return true;
    }

    return executeCommand(args);
}

/// Split the line into multiple lines (command + args)
static char **splitLines(char *line) {
    const int TOK_BUFSIZE = 64;
    const char *TOK_DELIM = " \t\r\n\a";
    int bufsize = TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) {
                fprintf(stderr, "Allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}



static void sushiLoop() {
    char hostname[256];
    char *username;

    username = getlogin();
    if (username == NULL) {
        perror("getlogin");
        exit( EXIT_FAILURE);
    }

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        exit (EXIT_FAILURE);
    }

    char *line = NULL;
    char **args;
    bool shStatus = true;

    while (shStatus) {
        printf("\033[1;31m%s\033[1;0m@\033[1;31m%s\033[1;0m: ", username, hostname);


        // Read Input
        line = readline("> ");
        if (line == NULL) {
            perror("readline");
            exit(EXIT_FAILURE);
        }
        if (strlen(line) == 0) { 
            free(line);
            line = NULL;
            continue;
        }

        writeOnHistory(line);

        args = splitLines(line);
        shStatus = launchBuiltIns(args);
        free(args);

        free(line); 
        line = NULL; 
    }
}

int main(int argc, char** argv) {


    sushiLoop();

    return EXIT_SUCCESS;
}