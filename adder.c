#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* DISCLAIMER */
/*
    This code was 100% written by Boris Skurikhin
*/

/* Compares two strings, (as numbers to see which is smaller) */
int smaller(char * str1, char * str2) {
    int l1 = strlen(str1);
    int l2 = strlen(str2);
    if (l1 < l2) return 1;
    if (l1 > l2) return 0;
    for (int i = 0; i < l1; i++) {
        if (str1[i] < str2[i]) return 1;
        if (str1[i] > str2[i]) return 0;
    }
    return 0;
}

void swap (char **a, char **b) {
    char *tmp = *a;
    *a = *b;
    *b = tmp;
}

char * add (char * str1, char * str2) {
    if (strlen(str1) > strlen(str2)) swap(&str1, &str2);
    int l1 = strlen(str1);
    int l2 = strlen(str2);
    int diff = l2 - l1;
    int carry = 0;
    /* set up all variables */
    char * result = (char *) calloc(1, 100001);
    char * output = (char *) calloc(1, 100001);
    result[0] = '\0';
    int ri = 0;
    int oi = 0;
    /* now we're done with the setup */
    for (int i = l1 - 1; i >= 0; i--) {
        int col = ( (str1[i] - '0') + (str2[i + diff] - '0') + carry );
        result[ri++] = ((col % 10) + '0');
        carry = col / 10;
    }
    for (int i = diff - 1; i >= 0; i--) {
        int col = (str2[i] - '0') + carry;
        result[ri++] = ((col % 10) + '0');
        carry = col / 10;
    }
    if (carry) result[ri++] = (carry + '0');
    for (int i = ri - 1; i >= 0; i--) output[oi++] = result[i];
    free(result);

    return output;
}

char * leading_zero(char * str) {
    int j = 0;
    char * result;
    while ( j < strlen(str)) { if (str[j] == '0') j++; else break; }
    if (j == strlen(str)) {
        result = (char *) calloc(1, 2);
        strcpy(result, "0");
        
        free(str);
        return result;
    } else {
        result = (char *) calloc(1, strlen(str) - j);
        strncpy(result, str + j, strlen(str) - j);
        strcat(result, "\0");
        free(str);
        return result;
    }
}

void run(char * str1, char * str2) {
    if (str1 == NULL || str2 == NULL) return;
    if (strlen(str1) == 0 || strlen(str2) == 0) return;
    if (str1[0] == '-' && str2[0] == '-') {
        char * nstr1 = (char *) calloc(1, strlen(str1) + 1);
        char * nstr2 = (char *) calloc(1, strlen(str2) + 1);
        strncpy(nstr1, str1 + 1, strlen(str1) - 1);
        strncpy(nstr2, str2 + 1, strlen(str2) - 1);
        strcat(nstr1, "\0");
        strcat(nstr2, "\0");
        /* Took out the negative signs */
        char * answer = leading_zero(add(nstr1, nstr2));
        printf("%s%s", ( (!strcmp(answer, "0")) ? "" : "-") , answer);
        /* free memory */
        free(nstr1);
        free(nstr2);
        free(answer);
    } else if (str1[0] != '-' && str2[0] != '-') {
        char * answer = leading_zero(add(str1, str2));
        printf("%s", answer);
        free(answer);
    } else {
       int which = 0;
       /* Get substrings */
        if (str1[0] == '-') {
            char * temp = (char *) calloc(1, strlen(str1));
            strncpy(temp, str1 + 1, strlen(str1) - 1);
            swap(&temp, &str1);
        } else {
            char * temp = (char *) calloc(1, strlen(str2));
            strncpy(temp, str2 + 1, strlen(str2) - 1);
            swap(&temp, &str2);
            which = 1;
        }
        /* Done */
        if (!strcmp(str1, str2)) {
            printf("0\n");
            return;
        }
        int neg = 0;
        if (smaller(str1, str2)) {
            neg = 1;
            swap(&str1, &str2);
        }
        
        char * secondComp = (char *) calloc(1, 100001);
        secondComp[0] = '\0';
        int si = 0;
        /* pad with 9's */
        for (int i = 0; i < strlen(str1) - strlen(secondComp); i++) {
            secondComp[si++] = '9';
            secondComp[si] = '\0';
        }
        for (int i = 0; i < strlen(str2); i++) {
            secondComp[si++] = (9 - (str2[i] - '0') + '0');
            secondComp[si] = '\0';
        }
        char * result = add(str1, secondComp);
        free(secondComp);

        if (strlen(result) > strlen(str1)) {
            char * carry = (char *) calloc(1, 2);
            carry[0] = result[0];
            carry[1] = '\0';
            char * new_result = (char *) calloc(1, strlen(result) );
            strncpy(new_result, result + 1, strlen(result) - 1);
            strcat(new_result, "\0");
            free(result);
            result = add(new_result, carry);
            free(new_result);
            free(carry);
        } else {
            for (int i = 0; i < strlen(result); i++) {
                result[i] = (9 - (result[i] - '0') + '0');
            }
        }
        char * final = leading_zero(result);
        printf("%s%s", (neg ? (which ? "-" : "") : (which ? "" : "-") ), final);
        free(final);
    }
    printf("\n");
}