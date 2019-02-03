#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_LEN 1024
#define true 1
#define false 0
#define DEBUG 1

typedef enum shi {ARGS, NO_ARGS, BG, NO_BG, FROM_FILE, TO_FILE, NO_FILE } ShellInput;

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
            if (check(i + 1, input, ' ')) {
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
        arg[k++] = input[i];
        arg[k] = '\0';
        i++;
    }

    /* Last param, no space */
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
        result[numArgs++] = (char *) calloc(1, strlen(filename) + 1);
        strcpy(result[numArgs - 1], filename);
    }
    else if (tofile) {
        *_file = TO_FILE;
        result[numArgs++] = (char *) calloc(1, strlen(filename) + 1);
        strcpy(result[numArgs - 1], filename);
    }
    else *_file = NO_FILE;
    /* Check for bg */
    *_bg = (background ? BG : NO_BG);
    *size = numArgs;
    /* Finally return result */

    return result;
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
        if (fgets(input, 2048, stdin) != NULL) {
            /* Remove the appended \n */
            input[strlen(input) - 1] = '\0';
            int numArgs = 0;
            ShellInput _args, _file, _bg;
            char ** parsed_input = parse_input(input, &numArgs, &_args, &_file, &_bg);
            if (parsed_input == NULL) {
                printf("Error occured might have to exit...\n");
            } else {
                #if DEBUG
                    for (int j = 0; j < numArgs; j++) printf("[%s],", parsed_input[j]);
                    printf("\n");
                    print_shellInput(&_args);
                    print_shellInput(&_file);
                    print_shellInput(&_bg);
                #endif
            }
            break;
        } else {
            /* Something bad happened, free all memory & exit */
            free(username);
            exit(1);
        }
        break;
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