void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);
    logger_init(); 
    logger("DEBUG", "handle_client.c", LINE_NUMBER, "Handle client", "handle_client", "start");

    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];

top:
    // Receive username
    logger("DEBUG", "handle_client.c", LINE_NUMBER, "receiving username", "receive_username", "start");
    if (recv(client_socket, username, sizeof(username), 0) <= 0) {
	perror("Error receiving username");
        close(client_socket);
    	logger("ERROR", "handle_client.c", LINE_NUMBER, "Error while receiving username", "receive_username", "end");
        return NULL;
    }
    decryption(username);
    logger("INFO", "handle_client.c", LINE_NUMBER, "Successfully received username", "receive_username", "end");
    
    // Receive password
    logger("DEBUG", "handle_client.c", LINE_NUMBER, "receiving password", "receive_password", "start");
    if (recv(client_socket, password, sizeof(password), 0) <= 0) {
        perror("Error receiving password");
	close(client_socket);
        logger("ERROR", "handle_client.c", LINE_NUMBER, "Error while receiving password", "receive_password", "end");
        return NULL;
    }
    decryption(password);
    logger("INFO", "handle_client.c", LINE_NUMBER, "Successfully received password", "receive_password", "end");
    
    // Check if the client wants to register or authenticate
    char choice;
    logger("DEBUG", "handle_client.c", LINE_NUMBER, "receiving choice", "receive_choice", "start");
    if (recv(client_socket, &choice, sizeof(choice), 0) <= 0) {
        perror("Error receiving choice");
        close(client_socket);
        logger("ERROR", "handle_client.c", LINE_NUMBER, "Error while receiving choice", "receive_choice", "end");
        return NULL;
    }
    logger("INFO", "handle_client.c", LINE_NUMBER, "Successfully received choice", "receive_choice", "end");

    if (choice == 'r') { // Register new user

        pthread_mutex_lock(&mutex);
        FILE *file; 
	file = fopen(REGISTRATION_FILE, "r");
        if (file == NULL) {
		file = fopen(REGISTRATION_FILE, "w+");
		if (file == NULL) {
    		    logger("FATAL", "handle_client.c", LINE_NUMBER, "Error opening registration.txt", "handle_client", "end");
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

	    if(strcmp(saved_username,username) == 0) {

    		logger("ERROR", "handle_client.c", LINE_NUMBER, "Error, username is already registered", "handle_client", "end");
                fclose(file);
                pthread_mutex_unlock(&mutex);
                send(client_socket,"11",2,0);

                close(client_socket);
                return NULL;
            }
        }

        fclose(file);
        pthread_mutex_unlock(&mutex);
        register_client(username, password);
        logger("INFO", "handle_client.c", LINE_NUMBER, "Registration successful", "handle_client", "end");
        send(client_socket, "Registration successful.\n", 26, 0);
        close(client_socket);
        return NULL;

    } else if (choice == 'a') { // Authenticate user
        int auth_code = authenticate(username,password);
        if (auth_code == 0) {
            send(client_socket,"00",2,0);
            logger("ERROR", "handle_client.c", LINE_NUMBER, "Error, Invalid username", "handle_client", "end");
            goto top;
        }
        if (auth_code == 1) {
            send(client_socket,"10",2,0);
            logger("ERROR", "handle_client.c", LINE_NUMBER, "Error, Invalid password", "handle_client", "end");
            goto top;
        }
        if (auth_code == 3) {
            send(client_socket,"01",2,0);
            logger("ERROR", "handle_client.c", LINE_NUMBER, "Error, username already exist", "handle_client", "end");
            goto top;
        }
        if (auth_code == 2) {
            send(client_socket, "Authentication successful. You are now in the chat.\n", 50, 0);
            logger("INFO", "handle_client.c", LINE_NUMBER, "Authentication successful", "handle_client", "end");
            // Add client to the list
            pthread_mutex_lock(&mutex);
            struct Client new_client;
            new_client.socket = client_socket;
            strncpy(new_client.username, username, MAX_USERNAME_LEN);
            clients[num_clients++] = new_client;
            pthread_mutex_unlock(&mutex);

            printf("[%s]: == Logged into chat ==\n",username);

            char buffer[MAX_MSG_LEN+MAX_USERNAME_LEN+1];

            sprintf(buffer,"%s|%s",username,"== Logged into chat ==");
            logger("INFO", "handle_client.c", LINE_NUMBER, buffer, "handle_client", "end");

            encryption(buffer);
            // Broadcast message to all other clients
            logger("INFO", "handle_client.c", LINE_NUMBER, "Broadcast message to all other clients", "handle_client", "end");
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < num_clients; i++) {
                if (clients[i].socket != client_socket) {
                    send(clients[i].socket,buffer, sizeof(buffer), 0);
                }
            }
            pthread_mutex_unlock(&mutex);

        }
    }

    char message[MAX_MSG_LEN];
    while (1) {
        // Receive message from client
//      char message[MAX_MSG_LEN];
        memset(message,0,sizeof(message));
        int bytes_received = recv(client_socket,message,sizeof(message),0);
	decryption(message);
	if (bytes_received <= 0) {
            printf("[%s]: == Disconnected ==\n",username);
            pthread_mutex_lock(&mutex);
            // Remove client from the list
            for (int i = 0; i < num_clients; i++) {
                if (clients[i].socket == client_socket) {
                    for (int j = i; j < num_clients - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    num_clients--;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);


            char buffer[MAX_MSG_LEN+MAX_USERNAME_LEN+1];

            sprintf(buffer,"%s|%s",username,"== Disconnected ==");
            logger("INFO", "handle_client.c", LINE_NUMBER, buffer, "handle_client", "end");
            encryption(buffer);
	    // Broadcast message to all other clients
            logger("INFO", "handle_client.c", LINE_NUMBER, "Broadcast message to all other clients", "handle_client", "end");
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < num_clients; i++) {
                if (clients[i].socket != client_socket) {
                    send(clients[i].socket, buffer, sizeof(buffer), 0);
                }
            }
            pthread_mutex_unlock(&mutex);

            close(client_socket);
            return NULL;
        }

        message[bytes_received] = '\0';

        // Print the message on the server
        printf("[%s]: %s\n",username, message);

        char buffer[MAX_MSG_LEN+MAX_USERNAME_LEN+1];

        sprintf(buffer,"%s|%s",username,message);
        logger("INFO", "handle_client.c", LINE_NUMBER, buffer, "handle_client", "end");
        encryption(buffer);
        // Broadcast message to all other clients
        logger("INFO", "handle_client.c", LINE_NUMBER, "Broadcast message to all other clients", "handle_client", "end");
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < num_clients; i++) {
            if (clients[i].socket != client_socket) {
                send(clients[i].socket, buffer, sizeof(buffer), 0);
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
