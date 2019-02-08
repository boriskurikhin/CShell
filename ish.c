#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "adder.h"

#define BUFFER_LEN 1024
#define DEBUG 1

typedef enum shit /* lmao */ { BG, NO_BG } ShellInput;
long find_gcd(long a, long b);

/* Helper function that helps us parse the file */
int check(int i, char * input, char c) {
    if ( i < 0 ) return 0;
    if ( i >= strlen(input)) return 0;
    if (c == '_') return 1; /* Don't check character */
    return input[i] == c ? 1 : 0;
}
char ** parse_input(char * input, int * size, ShellInput * _file, ShellInput * _bg, char ** in_file, char ** out_file ) {
    /* Quick Checks */
    if (input == NULL || _file == NULL || _bg == NULL || !strlen(input)) {
        /* Error occured, might as well return exit(1) later. */
        return NULL;
    }

    /* Basic counter shit */
    int i = 0;
    int len = strlen(input);
    
    /* Setting up some variables, for parsing */
    char arg[1000] = "\0";
    char ** result = NULL;
    int numArgs = 0;
    int openQuotes = 0;

    /* Some more stuff to check later */
    int tofile = 0;
    int fromfile = 0;
    int background = 0;

    int k = 0;

    /* Main parsing loop */
    while (check(i, input, '_')) {
        /* Found a space */
        if (input[i] == ' ' && !openQuotes) {
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
                    strcpy(arg, "");
                    continue;
                } else {
                    /* Leading spaces */
                    i++;
                    continue;
                }
            }
        /* Check for arguments */
        } else if (input[i] == '>' && !openQuotes) {
            if (check(i-1,input,' ') && check(i+1,input, ' ')) {
                if ( tofile || (i + 2 >= strlen(input))) {
                    /* Can't have a from file and to file */
                    for (int j = 0; j < numArgs; j++) free(result[j]);
                    free(result);
                    return NULL;
                }
                tofile = 1;
                /* Allocate some memory */
                *out_file = (char *) calloc(1, 1000);
                int j = i + 2;
                int fi = 0;
                /* Skip whitespace */
                while (input[j] == ' ') j++;
                /* Read file into it's pointer */
                while (j < len && input[j] != ' ') (*out_file)[fi++] = input[j++];
                /* Add null terminator */
                strcat((*out_file), "\0");
                
                if (!strlen(*out_file)) {
                    free(*out_file);
                    for (int j = 0; j < numArgs; j++) free(result[j]);
                    free(result);
                    return NULL;
                }
                /* Gonna need to add file later */
                i = j;
                continue;
            }
        } else if(input[i] == '<' && !openQuotes) {
            if (check(i-1,input,' ') && check(i+1,input, ' ')) {
                if (tofile || (i + 2 >= strlen(input))) {
                    /* Can't have a from file and to file */
                    for (int j = 0; j < numArgs; j++) free(result[j]);
                    free(result);
                    return NULL;
                }
                fromfile = 1;
                int fi = 0;
                /* Allocate some memory */
                *in_file = (char *) calloc(1, 1000);
                int j = i + 2;
                /* Skip whitespace */
                while (input[j] == ' ') j++;
                /* Read file into it's pointer */
                while (j < len && input[j] != ' ') (*in_file)[fi++] = input[j++];
                /* Add null terminator */
                strcat((*in_file), "\0");
                
                if (!strlen(*in_file)) {
                    free(*in_file);
                    for (int j = 0; j < numArgs; j++) free(result[j]);
                    free(result);
                    return NULL;
                }
                /* Gonna need to add file later */
                i = j;
                continue;
            }
        } else if (input[i] == '&' && !openQuotes) {
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
        if (a[0] == '0' && a[1] == 'x') {
            for (int j = 2; j < strlen(a); j++) {
                if (!( (a[j] >= '0' && a[j] <= '9') || (a[j] >= 'A' && a[j] <= 'F') || (a[j] >= 'a' && a[j] <= 'f')) ) {
                    return -1;
                }
            }
            num1 = strtol(a, NULL, 16);
        }
        else num1 = strtol(a, NULL, 10);
    }
    if (strlen(b) < 3) num2 = strtol(b, NULL, 10);
    else {
        if (b[0] == '0' && b[1] == 'x') {
            for (int j = 2; j < strlen(b); j++) {
                if (!( (b[j] >= '0' && b[j] <= '9') || (b[j] >= 'A' && b[j] <= 'F') || (b[j] >= 'a' && b[j] <= 'f')) ) {
                    return -1;
                }
            }
            num2 = strtol(b, NULL, 16);
        }
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
        case BG: printf("BG_PROCESS: TRUE\n"); break;
        case NO_BG: printf("BG_PROCESS: FALSE\n"); break;
        default: printf("ERROR CHECK PARAMS\n");
    }
}
int cd(char * path) {
    return chdir(path);
}

int main() {
    /* Setting up */
    char hostname[BUFFER_LEN];
    char * username = get_username(getuid());
    char input[2048 + 1 + 1]; /* 2^11 + 1 for new line, 1 for \0 */
    gethostname(hostname, BUFFER_LEN - 1);
    FILE * fpi = NULL;
    FILE * fpo = NULL;
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
            ShellInput _file, _bg;
            /* Some variables for execution */
            pid_t fork_id;
            int executeStatus;
            int result;
            /* If we're outputting to a file */
            char * in_file = NULL;
            char * out_file = NULL;

            /* Read in some input */
            char ** parsed_input = parse_input(input, &numArgs, &_file, &_bg, &in_file, &out_file);
            if (parsed_input == NULL) continue;

            /* If it's cd we can just deal with it ourselves */
            if (!strcmp(parsed_input[0], "cd")) {
                if (cd(parsed_input[1]) < 0)
                    perror(parsed_input[1]);
                continue;
            }

            fork_id = fork();
            
            if (fork_id < 0) {
                perror("Fork failed!\n");
                break;
            }

            if (fork_id == 0) {
                /* Child process has been spawned */
                /* This is an error! Handle it later! */
                
                #if DEBUG
                    for (int j = 0; j < numArgs; j++) printf("[%s],", parsed_input[j]);
                    printf("\n");
                    print_shellInput(&_file);
                    print_shellInput(&_bg);
                #endif

                /* Input stream */
                if (in_file != NULL) {
                    #if DEBUG
                        printf("Input file: %s\n", in_file);
                    #endif
                    fpi = freopen(in_file, "r" , stdin );
                    if (fpi == NULL) {
                        perror("File could not be opened!\n");
                        continue;
                    }
                }
                
                /* Output stream */
                if (out_file != NULL) {
                    #if DEBUG
                        printf("Output file: %s\n", out_file);
                    #endif
                    fpo = freopen(out_file, "w+", stdout );
                    if (fpo == NULL) {
                        perror("File could not be opened!\n");
                        continue;
                    }
                }
                
                /* Check for our built in functions */
                if (!strcmp(parsed_input[0], "gcd")) {
                    long answer = gcd(parsed_input[1], parsed_input[2]);
                    if (answer == -1) printf("No GCD was found!\n");
                    else printf("GCD(%s, %s) = %ld\n", parsed_input[1], parsed_input[2], answer);
                    exit(0);
                } else if (!strcmp(parsed_input[0], "args")) {
                    printf("argc = %d, args = ", numArgs - 1);
                    for (int j = 1; j < numArgs; j++)
                        printf("%s%s", parsed_input[j], (j < numArgs - 1) ? ", " : "\n");
                    exit(0);
                } else if(!strcmp(parsed_input[0], "adder")) {
                    run(parsed_input[1], parsed_input[2]);
                    exit(0);
                } else {
                /* We must add a NULL as the last element apparently*/
                    parsed_input = realloc(parsed_input, (numArgs + 1) * sizeof(char *));
                    parsed_input[numArgs++] = NULL;
                    /* No we can properly run this */
                    if (execvp(*parsed_input, parsed_input) < 0) {
                        /* We can now check PATH, and see if there exists another command */
                        printf("No command '%s' found.\n", parsed_input[0]);
                        exit(1);
                    }
                }
                /* if we opened up a file, close it back up  */
            } else {
                if (fpi) fclose(fpi);
                if (fpo) fclose(fpo);

                result = waitpid(fork_id, &executeStatus, WUNTRACED);
                #if DEBUG
                    printf("%s!\n", !executeStatus ? "Sucessfully executed" : "Failed");
                #endif
            }
            
            /* Free memory once we're done using it */
            for (int j = 0; j < numArgs; j++) free(parsed_input[j]);
            free(parsed_input);
        } else {
            
        }
        //end while loop
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