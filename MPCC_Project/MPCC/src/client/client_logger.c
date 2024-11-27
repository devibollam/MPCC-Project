void logger_init() {                                                                                                                                                  
    t = time(NULL);
    struct tm *tm = localtime(&t);
    
    char log_filename[100];
    sprintf(log_filename, "../../log/client_log/%d-%02d-%02d.log", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

    log_file = fopen(log_filename, "a+");
    if (log_file == NULL) {
        printf("Error opening file.\n");
    }   
}

void logger(char *level, char *filename, int line_number, char *message, char *activity, char *event) {
    char* timeString = asctime(localtime(&t));

    timeString[strcspn(timeString, "\n")] = '\0';

    fprintf(log_file,"{\"asctime\": \"%s\", \"level\": \"%s\", \"file\": \"%s\", \"line\": \"%d\", \"message\": \"%s\", \"activity\": \"%s\", \"event\": \"%s\"}\n",timeString, level, filename, line_number, message, activity, event); 
}
