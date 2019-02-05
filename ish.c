#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

#define BUFFER_LEN 1024
#define true 1
#define false 0
#define DEBUG 1

typedef enum shit /* lmao */ {ARGS, NO_ARGS, BG, NO_BG, FROM_FILE, TO_FILE, NO_FILE } ShellInput;
long find_gcd(long a, long b);

/* Helper function that helps us parse the file */
int check(int i, char * input, char c) {
    if ( i < 0 ) return 0;
    if ( i >= strlen(input)) return 0;
    if (c == '_') return 1; /* Don't check character */
    return input[i] == c ? 1 : 0;
}
char ** parse_input(char * input, int * size, ShellInput * _args, ShellInput * _file, ShellInput * _bg) {
    /* Quick Checks */
    if (input == NULL || _args == NULL || _file == NULL || _bg == NULL || !strlen(input)) {
        /* Error occured, might as well return exit(1) later. */
        return NULL;
    }

    /* Basic counter shit */
    int i = 0;
    int len = strlen(input);
    
    /* Setting up some variables, for parsing */
    char arg[1000] = "";
    char ** result = NULL;
    int numArgs = 0;
    int hasArguments = 0;
    int openQuotes = 0;

    /* Some more stuff to check later */
    int tofile = 0;
    int fromfile = 0;
    int background = 0;

    char filename[256] = "";
    int k = 0;

    /* Main parsing loop */
    while (check(i, input, '_')) {
        /* Found a space */
        if (input[i] == ' ') {
            /* We can skip spaces */
            if (check(i + 1, input, ' ') || openQuotes) {
                i++;
                continue;
            } else {
                if (strlen(arg)) {
                    /* Create some room */
                    if (result == NULL) {
                        result = calloc(1, sizeof(char *));
                    } else {
                        result = realloc(result, sizeof(char *) * (numArgs + 1));
                    }
                    /* Allocate some space for the argument */
                    result[numArgs] = (char *) calloc(1, strlen(arg) + 1);
                    /* Copy the argument into the result */
                    strcpy(result[numArgs], arg);
                    /* Free it, increment count, continue */
                    numArgs++;
                    i++;
                    k = 0;
                    strcpy(arg, "\0");
                    continue;
                } else {
                    /* Leading spaces */
                    i++;
                    continue;
                }
            }
        /* Check for arguments */
        } else if (input[i] == '-') {
            if (check(i-1, input, ' ') && check(i+1, input, 'l') && check(i+2, input, ' ') ) {
                hasArguments = 1;
                i += 3;
                continue;
            } else {
                i++;
                continue;
            }
        } else if (input[i] == '>') {
            if (check(i-1,input,' ') && check(i+1,input, ' ')) {
                if (fromfile || tofile || (i + 2 >= strlen(input))) {
                    /* Can't have a from file and to file */
                    for (int j = 0; j < numArgs; j++) free(result[j]);
                    free(result);
                    return NULL;
                }
                tofile = 1;
                int fi = 0;
                for (int j = i + 2; j < len; j++) filename[fi++] = input[j];
                strcat(filename, "\0");
                /* Gonna need to add file later */
                break;
            }
        } else if(input[i] == '<') {
            if (check(i-1,input,' ') && check(i+1,input, ' ')) {
                if (fromfile || tofile || (i + 2 >= strlen(input))) {
                    /* Can't have a from file and to file */
                    for (int j = 0; j < numArgs; j++) free(result[j]);
                    free(result);
                    return NULL;
                }
                fromfile = 1;
                int fi = 0;
                for (int j = i + 2; j < len; j++) filename[fi++] = input[j];
                strcat(filename, "\0");
                /* Gonna need to add file later */
                break;
            }
        } else if (input[i] == '&') {
            /* Check if it's not part of input, and that it's the last thing on the line */
            if ( check(i-1,input, ' ') && (i + 1) == strlen(input)) {
                background = 1;
                break;
            }
        } 
        if (input[i] == '"') openQuotes ^= 1;
        arg[k++] = input[i];
        arg[k] = '\0';
        i++;
    }

    /* Last (or first & only) param, no space */
    if (strlen(arg)) {
        if (result == NULL) result = calloc(1, sizeof(char *) );
        else result = realloc(result, sizeof(char *) * (numArgs + 1));
        result[numArgs] = (char *) calloc(1, strlen(arg) + 1);
        strcpy(result[numArgs++], arg);
    }
    /* Check arguments */
    *_args = (hasArguments ? ARGS : NO_ARGS);
    /* Check for file */
    if (fromfile) {
        *_file = FROM_FILE;
        /* If we specified a file, but no file has been passed */
        /* Or somehow to file was also passed */
        if (strlen(filename) == 0 || tofile ) {
            for (int j = 0; j < numArgs; j++) free(result[j]);
            free(result);
            return NULL;
        }
        result[numArgs++] = (char *) calloc(1, strlen(filename) + 1);
        strcpy(result[numArgs - 1], filename);
    } else if (tofile) {
        *_file = TO_FILE;
        /* If we specified a file, but no file was passed */
        if (strlen(filename) == 0) {
            for (int j = 0; j < numArgs; j++) free(result[j]);
            free(result);
            return NULL;
        }
        result[numArgs++] = (char *) calloc(1, strlen(filename) + 1);
        strcpy(result[numArgs - 1], filename);
    } else *_file = NO_FILE;
    /* Check for bg */
    *_bg = (background ? BG : NO_BG);
    *size = numArgs;
    /* Finally return result */

    return result;
}

long gcd(char * a, char * b) {
    if (!strlen(a) || !strlen(b)) return -1;
    long num1 = 0, num2 = 0;
    if (strlen(a) < 3) num1 = strtol(a, NULL, 10);
    else {
        if (a[0] == '0' && a[1] == 'x') num1 = strtol(a, NULL, 16);
        else num1 = strtol(a, NULL, 10);
    }
    if (strlen(b) < 3) num2 = strtol(b, NULL, 10);
    else {
        if (b[0] == '0' && b[1] == 'x') num2 = strtol(b, NULL, 16);
        else num2 = strtol(b, NULL, 10);
    }
    /* check */
    if (num1 < num2) {
        int temp = num2;
        num2 = num1;
        num1 = temp;
    }
    return find_gcd(num1, num2);
}

long find_gcd(long a, long b) {
    if (b == 0) return a;
    return find_gcd(b, a % b);
}

char * get_username(uid_t id) {
    struct passwd * username = getpwuid(id);
    char * value = (char *) calloc ( 1, strlen(username->pw_name) + 1);
    strcpy(value, username->pw_name);
    return value;
}

void print_shellInput(ShellInput * input) {
    if (input == NULL) return;
    switch (*input) {
        case ARGS: printf("ARGS: TRUE\n"); break;
        case NO_ARGS: printf("ARGS: FALSE\n"); break;
        case BG: printf("BG_PROCESS: TRUE\n"); break;
        case NO_BG: printf("BG_PROCESS: FALSE\n"); break;
        case FROM_FILE: printf("FILE: FROM\n"); break;
        case TO_FILE: printf("FILE: TO\n"); break;
        case NO_FILE: printf("FILE: NO FILE\n"); break;
        default: printf("ERROR CHECK PARAMS\n");
    }
}

int main() {
    /* Setting up */
    char hostname[BUFFER_LEN];
    char * username = get_username(getuid());
    char input[2048 + 1 + 1]; /* 2^11 + 1 for new line, 1 for \0 */
    gethostname(hostname, BUFFER_LEN - 1);
    /* Getting the username, hostname out of the way */

    /* Prompt */
    while (1) {
        printf("[%s@%s]%c ", username, hostname, (!geteuid() ? '#' : '$'));
        fflush(stdout);

        if (fgets(input, 2048, stdin) != NULL) {
            /* Remove the appended \n */
            input[strlen(input) - 1] = '\0';
            
            /* Empty, just re-display the shell */
            if (!strlen(input)) continue;
            /* Exit command has been passed in */
            if (!strcmp(input, "exit")) break;

            int numArgs = 0;
            ShellInput _args, _file, _bg;
            /* Some variables for execution */
            pid_t fork_id;
            int executeStatus;

            if ((fork_id = fork()) < 0) {
                /* The fork failed, need to do something */
                printf("The fork failed!\n");
                break;
            } else if (fork_id == 0) {
                /* Child process has been spawned */
                char ** parsed_input = parse_input(input, &numArgs, &_args, &_file, &_bg);
                /* This is an error! Handle it later! */
                if (parsed_input == NULL) break;
                
                #if DEBUG
                    for (int j = 0; j < numArgs; j++) printf("[%s],", parsed_input[j]);
                    printf("\n");
                    print_shellInput(&_args);
                    print_shellInput(&_file);
                    print_shellInput(&_bg);
                #endif

                if (!strcmp(parsed_input[0], "gcd")) {
                    long answer = gcd(parsed_input[1], parsed_input[2]);
                    if (answer == -1) printf("No GCD was found!\n");
                    else printf("GCD(%s, %s) = %ld\n", parsed_input[1], parsed_input[2], answer);
                    break;
                } else if (!strcmp(parsed_input[0], "args")) {
                    printf("argc = %d, args = ", numArgs - 1);
                    for (int j = 1; j < numArgs; j++) {
                        printf("%s%s", parsed_input[j], (j < numArgs - 1) ? ", " : "\n");
                    }
                    break;
                }
                
                /* Check if execution gets gg'ed */
                if (execvp(parsed_input[0], parsed_input) < 0) {
                    /* We can now check PATH, and see if there exists another command */

                    printf("Execution failed\n");
                    break;
                }
            }

            waitpid(fork_id, &executeStatus, 0);
            /* Need to handle execution status */

        } else {
            /* Something bad happened, free all memory & exit */
            free(username);
            exit(1);
        }
    }
    free(username);
    return 0;
}

/*
const char *mypath[] = { "./",
"/usr/bin/", "/bin/", NULL
};

while (...)
{

printf ("prompt> ");
fgets (...);

while (( ... = strsep (...)) != NULL)
{
...
}



if (fork () == 0) {
 
} else {

} // end of if

}

*/