#ifndef FINGER_H
#define FINGER_H

/* INCLUDE */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <string.h>
#include <utmpx.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

/* STRUCT DECLARATION*/
typedef struct {
    char *username; 
    char *real_name; 
    char *office; 
    char *office_phone; 
    char *home_phone; 
    char **terminal; 
    char *home_dir; 
    char *shell; 
    char **login_time; 
    time_t *idle; 
    int *access;
    char **host; 
} USER;

typedef struct {
    char *username;
    char *real_name;
    int took;
} UserInfo;

/* GLOBAL VARIABLES DECLARATION */
extern int options_flags[4]; // [-l,-s,-m,-p] 
extern char **users_username; // usernames of matched users
extern USER **users; // array of struct USER for the matched users

/* CONSTANTS DECLARATION*/
#define ONE_HOUR (60 * 60) // one hour in seconds
#define ONE_DAY (60 * 60 * 24) // one day in seconds
#define SIX_MONTHS (60 * 60 * 24 * 180) // six months in seconds
#define ONE_YEAR (60 * 60 * 24 * 365) // one year in seconds
#define BUFFER_SIZE 1024

/* FUNCTIONS DECLARATION */
void error_option(char error); // manages the error if has options not allowed
void get_users(); // get the users currently logged into the system
int compare_string(const void *a, const void *b); // comparator for strings
void search_users(int argc, char *argv[]);  // get the users that matches the inputs
UserInfo** get_all_users(int *num_users); // get all the users from the file /etc/passwd
UserInfo* parse_entry(char *line); // parse the entry of the user from the file /etc/passwd
void print_users_not_found(char **users, int count); // print the user not found
void init_users(int num_users, int search_user); // // init all the matched users
void init_user(char *line, USER *user); // init the struct USER for a specified user matched
void login_info(USER *user, struct utmpx *ut); //get the user's login info 
time_t calculate_idle_time(const char *terminal_device); // calculate idle time for a terminal
int is_more_than_six_months(time_t login_time); // return 1 if are past more than 6 months from the given date
char check_terminal_perm(char *terminal_path); // check if the terminal has the writing permission
char **custom_realloc(char **array, int count); // custom realloc function
void print_s(USER *user, int count); // print the user info following the short format
void print_l(USER *user, int count); // print the user info following the long format
void print_timezone(); // print the current timezone
int show_mail(const char *filepath, const char *path_forward); // show the mail information
int show_file(const char *filepath, const char *out); // shows the content of the file

#endif /* FINGER_H */