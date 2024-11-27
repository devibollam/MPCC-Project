void register_client(char *username, char *password) {
    pthread_mutex_lock(&mutex);
    logger_init(); 
    logger("DEBUG", "register_client.c", LINE_NUMBER, "Register client", "register_client", "start");
    FILE *file = fopen(REGISTRATION_FILE, "a");
    if (file == NULL) {
    	logger("FATAL", "register_client.c", LINE_NUMBER, "Error opening registration.txt file", "register_client", "end");
        perror("Error opening registration file");
        fclose(log_file);
        exit(EXIT_FAILURE);
    }

    encryption(password);

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
    logger("INFO", "register_client.c", LINE_NUMBER, "Successfully registered the client", "register_client", "end");
    pthread_mutex_unlock(&mutex);
}
