#include "finger.h"

// definition of global variables
int options_flags[4] = {0, 0, 0, 0}; // [-l,-s,-m,-p] 
char **users_username = NULL; // usernames of matched users
USER **users = NULL; // array of struct USER for the matched users

int main(int argc, char *argv[]){
    int flag_search_user = 0;

    // parse every input
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; j < strlen(argv[i]); j++) {
                if (argv[i][j] == 'l') { 
                    options_flags[0] = 1; // long format
                } else if (argv[i][j] == 's') {
                    options_flags[1] = 1; // short format
                } else if (argv[i][j] == 'm') {
                    options_flags[2] = 1; // avoid realnames match
                } else if (argv[i][j] == 'p') {
                    options_flags[3] = 1; // don't show .plan/.project/.pgpkey files
                } else {
                    error_option(argv[i][j]); // option not allowed
                }
            }
        } else {
            flag_search_user = 1;
        }
    }

    if (flag_search_user == 0) {
        get_users(); // get the users currently logged into the system
    } else { 
        search_users(argc, argv); // get the users that matches the inputs
    }
    return 0;
}

// get the users currently logged into the system
void get_users(){
    struct utmpx *ut;
    int num_users = 0;  
    int num_unique_users = 1; // number of che currently logged in users

    // take the username of the currently logged in users from the utmpx files
    setutxent(); // open the file and start the iterator
    while ((ut = getutxent()) != NULL){
        if (ut->ut_type == USER_PROCESS) {
            users_username = custom_realloc(users_username, (num_users + 1)); // adjust memory size
            users_username[num_users] = strdup(ut->ut_user); // copy the string
            num_users++;
        }    
    }
    endutxent(); // close the file

    qsort(users_username, num_users, sizeof(char *), compare_string); // sort the array users

    // delete duplicates from the array users
    for (int i = 1; i < num_users; i++) {
        if (strcmp(users_username[i], users_username[i-1]) != 0) {
            users_username[num_unique_users++] = users_username[i];
        }
    }

    init_users(num_unique_users, 0);
    free(users_username);
}

// get the users that matches the inputs
void search_users(int argc, char *argv[]) {
    int num_total_users = 0; // number of all the users from /etc/passwd
    UserInfo **users = get_all_users(&num_total_users); // get all the user from /etc/passwd

    int num_curr_users = 0; // number of matched users
    int num_not_found_users = 0; // number of not matched users
    char **not_found_users = NULL; // usernames of not matched users

    // iterate through every input for search a match
    for (int i = 1; i < argc; i++) { 
        // only if is not a option  
        if (argv[i][0] != '-') {
            int flag = 0; // 1 if match found else 0
            for (int j = 0; j < num_total_users; j++) { // iterate through every user
                if (users[j]->took == 0 && strcasecmp(argv[i], users[j]->username) == 0) { // match found
                    users_username = custom_realloc(users_username, (num_curr_users + 1)); // adjust memory size
                    users_username[num_curr_users] = strdup(users[j]->username); // copy the string
                    num_curr_users++;
                    users[j]->took = 1;
                    flag = 1;
                } else if (options_flags[2] == 0) { // if not -m search also for real_name match
                    char *token;
                    char *username_copy = strdup(users[j]->real_name);  // create a copy for strtok
                    token = strtok(username_copy, " "); // split the real_name 
                    while (token != NULL) {
                        if (users[j]->took == 0 && strcasecmp(argv[i], token) == 0) { // match found
                            users_username = custom_realloc(users_username, (num_curr_users + 1)); // adjust memory size
                            users_username[num_curr_users] = strdup(users[j]->username); // copy the string
                            num_curr_users++;
                            users[j]->took = 1;
                            flag = 1;
                            break;
                        }
                        token = strtok(NULL, " "); // continues looking for tokens in the original string
                    }
                    free(username_copy);  // free the copy after use
                } 
            }
            if (flag == 0) {
                not_found_users = custom_realloc(not_found_users, (num_not_found_users + 1)); // adjust memory size
                not_found_users[num_not_found_users] = strdup(argv[i]); // copy the string
                num_not_found_users++;
            }
        }
    }

    print_users_not_found(not_found_users, num_not_found_users); // print the users not found
    init_users(num_curr_users, 1); // init the users found

    // free memory for every username
    for (int i = 0; i < num_curr_users; i++) {
        free(users_username[i]);
    }
    free(users_username);
    
    // free memory for every UserInfo
    for (int i = 0; i < num_total_users; i++) {
        free(users[i]);
    }
    free(users);
}

// get all the users from the file /etc/passwd
UserInfo** get_all_users(int *num_users) {
    char line[BUFFER_SIZE]; // buffer for the line
    FILE *file;
    file = fopen("/etc/passwd", "r"); // open the file
    UserInfo **users = NULL; // user info for every user in the system from /etc/passwd
    *num_users = 0;

    if (file == NULL) {
        perror("Open file error");
        exit(EXIT_FAILURE);
    } // manage error opening the file

    while (fgets(line, sizeof(line), file) != NULL) { // get the line and put it into the buffer
        UserInfo **temp = realloc(users, (*num_users + 1) * sizeof(UserInfo *)); // adjust memory allocation
        if (temp == NULL) {
            perror("Realloc error for users");
            exit(EXIT_FAILURE);
        } // manage realloc error
        users = temp;
        users[*num_users] = parse_entry(line); // parse the line and init the UserInfo
        (*num_users)++;
    }

    fclose(file); // close the file 
    return users;
}

// parse the line and init the UserInfo
UserInfo* parse_entry(char *line) {
    char *username;
    char *real_name;
    char *token; // for strtok
    int count = 0;
    username = NULL;
    real_name = NULL;

    token = strtok(line, ":"); // split the line based on ':'
    while (token != NULL) {
        if (count == 0) {
            username = strdup(token); // take the username
        } else if (count == 4) {
            if (token[0] == '/') {
                real_name = strdup("");
            } else {
                real_name = strdup(token); // take the realname if exists
            }
        }
        token = strtok(NULL, ":"); // continues looking for tokens in the original string
        count++;
    }

    // remove the final commas if present
    if (real_name != NULL) {
        for (int i = 0; i < strlen(real_name); i++) {
            if (real_name[i] == ',') {
                real_name[i] = '\0';
                break;
            }
        }
    }

    UserInfo *user = malloc(sizeof(UserInfo)); // allocate memory for the UserInfo
    if (user == NULL) {
        perror("Malloc error for UserInfo");
        exit(EXIT_FAILURE);
    } // manage malloc error

    // init the fields of UserInfo
    user->username = username;
    user->real_name = real_name;
    user->took = 0;

    return user;
}