int authenticate(char *username, char *password) {
   pthread_mutex_lock(&mutex);
   logger_init();
   logger("DEBUG", "authenticate.c", LINE_NUMBER, "Aunthenticate client", "authenticate_client", "start");
   FILE *file;
   file = fopen(REGISTRATION_FILE, "r");
    if (file == NULL) {
	file = fopen(REGISTRATION_FILE, "w+");
	if (file == NULL) {
   	    logger("DEBUG", "authenticate.c", LINE_NUMBER, "Error while opening registration.txt", "authenticate_client", "end");
            perror("Error opening registration file");
            fclose(log_file);
            exit(EXIT_FAILURE);
	}
    }

    char line[MAX_USERNAME_LEN + MAX_PASSWORD_LEN + 2];
    while (fgets(line, sizeof(line), file) != NULL) {
        char saved_username[MAX_USERNAME_LEN];
        char saved_password[MAX_PASSWORD_LEN];
        sscanf(line, "%s %s", saved_username, saved_password);

        // Decrypt the stored password for comparison
        decryption(saved_password);

        if (strcmp(username, saved_username) == 0){
            if (strcmp(password, saved_password) == 0) {
                for(int i=0;i<num_clients;i++)
                {
                    if(strcmp(clients[i].username,username) == 0)
                    {
                        fclose(file);
                        pthread_mutex_unlock(&mutex);
   			char log_buffer[MAX_MSG_LEN];
			sprintf(log_buffer,"Failed client authentication, username : [%s] already active", username);
			logger("ERROR", "authenticate.c", LINE_NUMBER, log_buffer, "authenticate_client", "end");
                        return 3;
                    }
                }
                fclose(file);
                pthread_mutex_unlock(&mutex);
		logger("INFO", "authenticate.c", LINE_NUMBER, "Successful client authentication", "authenticate_client", "end");
                return 2; // Authentication successful
            }
            else {
                fclose(file);
                pthread_mutex_unlock(&mutex);
		logger("ERROR", "authenticate.c", LINE_NUMBER, "Failed authentication, wrong password", "authenticate_client", "end");
                return 1;
            }
        }
    }

    fclose(file);
    pthread_mutex_unlock(&mutex);
    logger("ERROR", "authenticate.c", LINE_NUMBER, "Failed authentication,Invalid username", "authenticate_client", "end");
    return 0; // Authentication failed
}

