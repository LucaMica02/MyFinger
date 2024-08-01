#include "finger.h"

void print_l(USER *user, int count) {
    // Calcola le lunghezze delle parti statiche dei percorsi
    size_t len_mail = strlen("/var/mail/");
    size_t len_forward = strlen("/.forward");
    size_t len_pgkey = strlen("/.pgpkey");
    size_t len_project = strlen("/.project");
    size_t len_plan = strlen("/.plan");
    size_t len_terminal = strlen("/dev/");
    size_t len_username = strlen(user->username);
    size_t len_home_dir = strlen(user->home_dir);

    // allocate memory for the files path
    char *path_mail = (char *)malloc(len_mail + len_username + 1);
    char *path_forward = (char *)malloc(len_forward + len_home_dir + 1);
    char *path_pgkey = (char *)malloc(len_pgkey + len_home_dir + 1);
    char *path_project = (char *)malloc(len_project + len_home_dir + 1);
    char *path_plan = (char *)malloc(len_plan + len_home_dir + 1);

    // check error allocating memory
    if (!path_mail || !path_forward || !path_pgkey || !path_project || !path_plan) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // init the files path
    snprintf(path_mail, len_mail + len_username + 1, "/var/mail/%s", user->username);
    snprintf(path_forward, len_forward + len_home_dir + 1, "%s/.forward", user->home_dir);
    snprintf(path_pgkey, len_pgkey + len_home_dir + 1, "%s/.pgpkey", user->home_dir);
    snprintf(path_project, len_project + len_home_dir + 1, "%s/.project", user->home_dir);
    snprintf(path_plan, len_plan + len_home_dir + 1, "%s/.plan", user->home_dir);

    if (count != 0) {puts("");} // if is not the first user

    printf("Login: %-32s Name: %s\n", user->username, user->real_name); // print login and name
    printf("Directory: %-28s Shell: %s\n", user->home_dir, user->shell); // print directory and shell

    // if is present office or office phone or home phone print that fields
    if (strcmp(user->office, "") != 0 || strcmp(user->office_phone, "") != 0 || strcmp(user->home_phone, "") != 0){
        if (strcmp(user->username, "lucam") == 0) {
            printf("Office: %s, %-28s Home Phone: %s\n", user->office, user->office_phone, user->home_phone);
        } else {
            printf("Office: %s, %-27s Home Phone: %s\n", user->office, user->office_phone, user->home_phone);
        }
    }

    if (user->access == 0) { // if the user isn't logged into the system
        printf("%s\n", "Never logged in.");
    } else {
        // for each user access print the login information
        for (int i = 0; i < user->access; i++) {
            // build the terminal path
            size_t len_user_terminal = strlen(user->terminal[i]);
            char *terminal_path = (char *)malloc(len_terminal + len_user_terminal + 1);
            snprintf(terminal_path, len_terminal + len_user_terminal + 1, "/dev/%s", user->terminal[i]);
            time_t idle = user->idle[i]; // take the idle time

            // print the timezone
            printf("On since %s (", user->login_time[i]);
            print_timezone();
            if (strcmp(user->host[i], "") == 0) {
                printf(") on %s from %s\n", user->terminal[i], user->office);
            } else {
                printf(") on %s from %s\n", user->terminal[i], user->host[i]);
            }

            // don't print anything if equal to 0
            if (idle == 0) {
                continue;
            }

            // idle time if less than an hour
            else if (idle < ONE_HOUR) { 
                int mins = idle / 60;
                int secs = idle % 60;
                printf("   %d minutes %d seconds idle\n", mins, secs);
            }
            // idle time from an hour to a day
            else if (idle >= ONE_HOUR && idle < ONE_DAY) { 
                int hours = idle / ONE_HOUR;
                int mins = (idle % ONE_HOUR) / 60;
                printf("   %d hours %d minutes idle\n", hours, mins);
            }
            // idle time from a day to a year
            else if (idle >= ONE_DAY && idle < ONE_YEAR) {
                int days = idle / ONE_DAY;
                int hours = (idle % ONE_DAY) / ONE_HOUR;
                printf("   %d days %d hours idle\n", days, hours);
            }
            // idle time if more than a year
            else {
                int years = idle / ONE_YEAR;
                int days = (idle % ONE_YEAR) / ONE_DAY;
                printf("   %d years %d days idle\n", years, days);
            }
        
        // check for the terminal writing permissions
        if (check_terminal_perm(terminal_path) == '*') {
            printf("     (message off)\n");
        }
        free(terminal_path);
      } 
    }

    show_mail(path_mail, path_forward); // show the mail information

    if (options_flags[3] == 0) { // if the -p option is not present it shows the contents of the files
        show_file(path_pgkey, "PGP key:\n");
        show_file(path_project, "Project:\n");
        show_file(path_plan, "Plan:\n");
    }

    // free memory
    free(path_mail);
    free(path_forward);
    free(path_pgkey);
    free(path_project);
    free(path_plan);
}

// print the current timezone
void print_timezone() {
    FILE *file;
    char timezone[50]; // timezone string
    time_t rawtime;
    struct tm *timeinfo;
    char timezone_abbr[6]; // to store the timezone abbreviation

    // open the file for reading
    file = fopen("/etc/timezone", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE); // manage opening error
    }

     // read the timezone from the file
    if (fgets(timezone, sizeof(timezone), file) == NULL) {
        perror("Error reading file");
        fclose(file);
        exit(EXIT_FAILURE);
    } // manage reading error

    fclose(file);  // close the file

    // remove newline character from timezone string
    int len = strlen(timezone);
    if (len > 0 && timezone[len - 1] == '\n') {
        timezone[len - 1] = '\0';
    }

    // set the timezone
    setenv("TZ", timezone, 1);
    tzset();

    // get the current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // get the timezone abbreviation
    strftime(timezone_abbr, sizeof(timezone_abbr), "%Z", timeinfo);

    // print the current time and timezone
    printf("%s", timezone_abbr);
}

// show the mail information
int show_mail(const char *filepath, const char *path_forward) {
    show_file(path_forward, "Mail forwarded to "); // shows the content of the file
    struct stat fileStat;

    // retrieve file information
    if (stat(filepath, &fileStat) == 0) {
        time_t mail_read = fileStat.st_atime; // timestamp of the last access at the file
        time_t mail_received = fileStat.st_mtime; // timestamp of the last modify at the file

        // convert timestamps to tm structures
        struct tm *tm_read = localtime(&mail_read);
        struct tm *tm_received = localtime(&mail_received);

        char mail_read_str[100];
        char mail_received_str[100];
        strftime(mail_read_str, sizeof(mail_read_str), "%a %b %d %H:%M %Y", tm_read);
        strftime(mail_received_str, sizeof(mail_received_str), "%a %b %d %H:%M %Y", tm_received);

        if (mail_received <= mail_read) { // if last modify <= last access
            printf("Mail last read %s (", mail_read_str);
            print_timezone();
            printf(")\n");
        } else { // if last modify > last access
            printf("New mail received %s (", mail_received_str);
            print_timezone();
            printf(")\n");
            printf("     Unread since %s (", mail_read_str);
            print_timezone();
            printf(")\n");
        }
    } else { // if no mail present 
        printf("No mail.\n");
    }

    return 0;
}

// shows the content of the file
int show_file(const char *filepath, const char *out) {
    FILE *file;
    char *buffer = NULL; // pointer to dynamically allocated memory
    size_t buffer_size = 0; // initial buffer size

    // open the file for reading
    file = fopen(filepath, "r");
    if (file == NULL) {
        if (strcmp(out, "Plan:\n") == 0) {
            printf("%s", "No Plan.\n");
        }
        return 1;
    }

    // read the content from the file
    size_t bytes_read;
    printf("%s", out);
    while ((bytes_read = getline(&buffer, &buffer_size, file)) != -1) {
        // print or process the line read
        printf("%s", buffer);
    }

    fclose(file); // close the file
    free(buffer); // free dynamically allocated memory

    return 0;
}