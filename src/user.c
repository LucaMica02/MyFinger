#include "finger.h"

// init all the matched users
void init_users(int num_users, int search_user) {
    char line[BUFFER_SIZE]; // buffer for the line
    FILE *file;
    users = malloc(sizeof(USER*) * num_users); // memory allocation
    if (users == NULL) {
        perror("Malloc error for users");
        exit(EXIT_FAILURE);
    } // manage malloc error

    // for every user matched search the correspond line of /etc/passwd and init the struct USER
    for (int i = 0; i < num_users; i++) {
        file = fopen("/etc/passwd", "r"); // open the file 
        while (fgets(line, sizeof(line), file) != NULL) { // get the line
            if (strncmp(line, users_username[i], strlen(users_username[i])) == 0) { // if username matched
                USER *new_user = malloc(sizeof(USER)); // memory allocation for the new user
                if (new_user == NULL) {
                    perror("Malloc error for new_user");
                    exit(EXIT_FAILURE);
                } // manage malloc error
                init_user(line, new_user); // init the user
                users[i] = new_user; 
                break;
            }
        }
        fclose(file); // close the file
    }

    // call the right print format
    for (int i = 0; i < num_users; i++) {
        if (options_flags[0] == 1) { 
            print_l(users[i], i); // where -l
        } else if (options_flags[1] == 1) { 
            print_s(users[i], i); // else if -s
        } else if (search_user == 1) {
            print_l(users[i], i); // if not option format but search user
        } else {
            print_s(users[i], i); // if not option format and not search user
        }
    } 

    // free memory for every USER
    for (int i = 0; i < num_users; i++) {
        free(users[i]);
    }
    free(users);

}

// init the struct USER for a specified user matched
void init_user(char *line, USER *user) {
    // declaration of the field
    char *username = NULL;
    char *real_name = NULL;
    char *office = NULL;
    char *office_phone = NULL;
    char *home_phone = NULL;
    char *token = NULL;
    int count = 0;
    int count_info = 0;

    // declaration of pointer to struct 
    struct passwd *pw;
    struct utmpx *ut;

    token = strtok(line, ":"); // split the line based on ':'
    while (token != NULL) {
        if (count == 0) {
            username = strdup(token); // init the username
        } else if (count == 4) {
            if (token[0] != '/') {
                real_name = strdup(token); // take the realname field
            }
        }
        token = strtok(NULL, ":"); // continues looking for tokens in the original string
        count++;
    }

    if (real_name != NULL) {
        char *real_name_copy = strdup(real_name); // copy the string
        token = strtok(real_name_copy, ","); // split the real name field based on ','
        while (token != NULL) {
            if (count_info == 0) {
                real_name = strdup(token); 
            } else if (count_info == 1) {
                office = strdup(token); 
            } else if (count_info == 2) {
                office_phone = strdup(token); 
            } else if (count_info == 3) {
                home_phone = strdup(token); 
            }
            token = strtok(NULL, ","); // continues looking for tokens in the original string
            count_info++;
        }
    }

    // init the username field
    if (username != NULL) {
        user->username = username;
    } else {
        user->username = "";
    }
    // init the real_name field
    if (real_name != NULL) {
        user->real_name = real_name;
    } else {
        user->real_name = "";
    }
    // init the office field
    if (office != NULL) {
        user->office = office;
    } else {
        user->office = "";
    }
    // init the office_phone field
    if (office_phone != NULL) {
        user->office_phone = office_phone;
    } else {
        user->office_phone = "";
    }
    // init the home_phone field
    if (home_phone != NULL) {
        user->home_phone = home_phone;
    } else {
        user->home_phone = "";
    }

    pw = getpwnam(user->username);
    if (pw == NULL){
        printf("Error: User not found\n");
        exit(EXIT_FAILURE);
    } // manage error not found user

    user->home_dir = strdup(pw->pw_dir); // init the home_dir field
    user->shell = strdup(pw->pw_shell); // init the shell field
    user->access = 0; // init the access field

    setutxent(); // open the file utmpx

    // iterate throught the file utmpx and get the user's login info for each access
    while ((ut = getutxent()) != NULL) {
        if (strcmp(ut->ut_user, user->username) == 0) {
            login_info(user, ut); // get the user's login info
        }
    }

    endutxent(); // close the file utmpx
}

//get the user's login info 
void login_info(USER *user, struct utmpx *ut) {
    time_t last_login = 0;
    int access = user->access;
    size_t len_user_terminal = strlen(ut->ut_line);
    size_t len_terminal = strlen("/dev/");

    // create the terminal path for the user
    char *terminal = (char *)malloc(len_terminal + len_user_terminal + 1);
    snprintf(terminal, len_terminal + len_user_terminal + 1, "/dev/%s", ut->ut_line);

    time_t idle_min = calculate_idle_time(terminal); // get the idle_time for the terminal
    last_login = ut->ut_tv.tv_sec;
    free(terminal);

    // init the time string
    char time_str[26];
    ctime_r(&last_login, time_str);

    // if is past more than six months remove hours, minutes, and seconds
    if (is_more_than_six_months(last_login) == 1) {
        char year[5];
        for (int i = 20; i < 24; i++) {
            year[i - 20] = time_str[i];
        }
        year[4] = '\0';
        for (int i = 11; i < 15; i++) {
            time_str[i] = year[i - 11];
        }
        time_str[15] = '\0';
    // else remove newline, year and seconds
    } else {  
        time_str[strlen(time_str) - 9] = '\0'; 
    }

    // init the field terminal for that access
    if (access == 0) {user->terminal = NULL;}
    user->terminal = custom_realloc(user->terminal, (access + 1)); // adjust memory size
    user->terminal[access] = strdup(ut->ut_line);
   
    // init the field login_time for that access
    if (access == 0) {user->login_time = NULL;}
    user->login_time = custom_realloc(user->login_time, (access + 1)); // adjust memory size
    user->login_time[access] = strdup(time_str);

    // init the field idle for that access
    if (access == 0) {user->idle = NULL;}
    time_t **temp = realloc(user->idle, (access + 1) * sizeof(time_t *)); // adjust memory allocation
    if (temp == NULL) {
        perror("Realloc error");
        exit(EXIT_FAILURE);
    } // manage realloc error
    user->idle = temp;
    user->idle[access] = idle_min;

    // init the field host for that access
    if (access == 0) {user->host = NULL;}
    user->host = custom_realloc(user->host, (access + 1)); // adjust memory size
    user->host[access] = strdup(ut->ut_host);

    user->access = access + 1;
}

// calculate idle time for a terminal
time_t calculate_idle_time(const char *terminal_device) {
    struct stat sb;
    time_t current_time, idle_time;

    time(&current_time); // get current time

    // get information about the terminal device
    if (stat(terminal_device, &sb) == -1) {
        perror("Terminal error:");
        exit(EXIT_FAILURE);
    }

    // calculate idle time (difference between current time and access time of terminal device)
    idle_time = current_time - sb.st_atime;

    return idle_time;
}