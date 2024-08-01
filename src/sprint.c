#include "finger.h"

// print the user info following the short format
void print_s(USER *user, int count) {
    if (count == 0) { // if is the first user
        printf("Login     Name             Tty      Idle  Login Time   Office     Office Phone\n");
    } 

    // truncate the real_name
    if (strlen(user->real_name) > 16) {
        user->real_name[16] = '\0';
    }

    if (user->access == 0) { // if the user isn't logged into the system
        printf("%-9s %-16s %-8c %-5c %-12s %-10s %s\n", user->username, user->real_name, '*', '*', "No logins", user->office, user->office_phone);
    } else {
        // for each user access
        for (int i = 0; i < user->access; i++) {
            size_t len_user_terminal = strlen(user->terminal[i]);
            size_t len_terminal = strlen("/dev/");
            char time_str[13];
            char idle_str[6];

            // create the terminal_path
            char *terminal_path = (char *)malloc(len_terminal + len_user_terminal + 1);
            snprintf(terminal_path, len_terminal + len_user_terminal + 1, "/dev/%s", user->terminal[i]);

            char terminal_perm = check_terminal_perm(terminal_path); // take the terminal permission
            time_t idle = user->idle[i];
            int hours = idle / 3600; // hours of idle
            int mins = (idle % 3600) / 60; // minutes of idle

            if (hours >= 24) { // if more than one day
                strncpy(idle_str, "+1 Day", sizeof(idle_str));
                idle_str[sizeof(idle_str) - 1] = '\0'; // ensuring null termination
            } else {
                snprintf(idle_str, sizeof(idle_str), "%d:%02d", hours, mins); // corrected format for hours and minutes
            }
            
            // custom the time string
            for (int j = 0; j < strlen(user->login_time[i]) - 4; j++){
                time_str[j] = user->login_time[i][j+4];
            }

            if (strcmp(user->host[i], "") == 0) { // if the host is not present print office and office phone
                printf("%-9s %-15s %c%-8s %-5s %s %-10s %s\n", user->username, user->real_name, terminal_perm, user->terminal[i], idle_str, time_str, user->office, user->office_phone);
            } else { // else print the host
                printf("%-9s %-15s %c%-8s %-5s %s %c%s%c\n", user->username, user->real_name, terminal_perm, user->terminal[i], idle_str, time_str, '(', user->host[i], ')');
            }
        }
    }
}