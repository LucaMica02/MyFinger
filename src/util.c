#include "finger.h"

// manages the error if has options not allowed
void error_option(char error) {
    printf("finger: invalid option -- '%c'\n", error);
    printf("usage: finger [-lmps] [login ...]\n");
    exit(EXIT_FAILURE);
}

// comparator for strings
int compare_string(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// print the user not found
void print_users_not_found(char **users, int count) {
    for (int i = 0; i < count; i++) {
        printf("finger: %s: no such user.\n", users[i]);
    }
}

// return 1 if are past more than 6 months from the given date
int is_more_than_six_months(time_t login_time) {
    // get the current time
    time_t current_time;
    time(&current_time);
    
    // check for the time difference
    if (difftime(current_time, login_time) > SIX_MONTHS) {
        return 1; // are past more that six month
    } else {
        return 0; // aren't past more that six month
    }
}

// check if the terminal has the writing permission
char check_terminal_perm(char *terminal_path) {
    if (access(terminal_path, W_OK) == 0) {
        return ' '; // if has return ' '
    } else {
        return '*'; // else return '*'
    }
}

// custom realloc function
char **custom_realloc(char **array, int count) {
    char **temp = realloc(array, count * sizeof(char *)); // adjust memory allocation
    if (temp == NULL) {
        perror("Realloc error for users_username");
        exit(EXIT_FAILURE);
    } // manage realloc error
    return temp;
}