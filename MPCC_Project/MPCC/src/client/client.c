#include "../../inc/client_utility.h"
#include "client_logger.c"
#include "signal_handler.c"
#include "encrypt_decrypt.c"
#include "receive_messages.c"

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    
    logger_init();

    // Create socket
    logger("INFO", "client.c", LINE_NUMBER, "Create client socket", "create_clent_socket", "start");
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
    	logger("ERROR", "client.c", LINE_NUMBER, "Failed client socket creation", "create_clent_socket", "end");
        perror("Error in socket");
        fclose(log_file);
        exit(EXIT_FAILURE);
    }
    logger("INFO", "client.c", LINE_NUMBER, "Successful client socket creation", "create_clent_socket", "end");

    // Set server address
    logger("INFO", "client.c", LINE_NUMBER, "Set server address", "set_server_address", "start");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    logger("INFO", "client.c", LINE_NUMBER, "Successfully set server address", "set_server_address", "end");

    // Connect to server
    logger("INFO", "client.c", LINE_NUMBER, "Connect to server", "connect_to_server", "start");
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    	logger("FATAL", "client.c", LINE_NUMBER, "Error while setting server address", "set_server_address", "end");
        perror("Error in connect");
        fclose(log_file);
        exit(EXIT_FAILURE);
    }
    logger("INFO", "client.c", LINE_NUMBER, "Successfully set server address", "set_server_address", "end");

    printf("Connected to server\n");

    char choice[MAX_MSG_LEN];
start:
    printf("Do you want to register a new account? (y/n): ");
    signal(SIGINT, signalHandler);
    scanf(" %s", choice);

    if (strlen(choice) > 1) {
    	printf("Enter a valid single character (y/n)\n");
    	logger("ERROR", "client.c", LINE_NUMBER, "Error aunthentication choice to server, choice has multiple characters", "send_auth_choice", "end");
	goto start;
    }

    if (choice[0] == 'y' || choice[0] == 'Y') {
        char new_username[MAX_USERNAME_LEN];
        char new_password[MAX_PASSWORD_LEN];

        // Get new username and password
        printf("Enter new username: ");
        signal(SIGINT, signalHandler);
        scanf("%s", new_username);
	
	encryption(new_username);
        send(client_socket, new_username, sizeof(new_username), 0);
    	logger("INFO", "client.c", LINE_NUMBER, "Send client encrypted username to server", "send_username", "start");

        printf("Enter new password: ");
        signal(SIGINT, signalHandler);
        scanf("%s", new_password);
	encryption(new_password);

        // Send registration request to the server
        send(client_socket, new_password, sizeof(new_password), 0);
    	logger("INFO", "client.c", LINE_NUMBER, "Send client encrypted password to server", "send_password", "start");
        send(client_socket, "r", 1, 0); // Send 'r' to indicate registration
    	logger("INFO", "client.c", LINE_NUMBER, "Send aunthentication choice (register) to server", "send_auth_choice", "start");

        char response[50];
    	logger("DEBUG", "client.c", LINE_NUMBER, "Handle server response", "handle_response", "start");
        recv(client_socket, response, 50, 0);
        if(response[0] == '1' && response[1] == '1') {
            printf("        Username already exist\n======Kindly Register or use other account======\n");
    	    logger("ERROR", "client.c", LINE_NUMBER, "Username already exists", "handle_response", "end");
            fclose(log_file);

        } else{
            printf("    %s=====Kindly login with new credentials=====\n", response);
    	    logger("INFO", "client.c", LINE_NUMBER, "Successfully register as new client", "handle_response", "end");
    	}
    } else if (choice[0] == 'n' || choice[0] == 'N') {
        char username[MAX_USERNAME_LEN];
        char password[MAX_PASSWORD_LEN];

login:
        printf("Enter username: ");
        signal(SIGINT, signalHandler);
        scanf("%s", username);
        encryption(username);
	send(client_socket,username, sizeof(username), 0);
    	logger("INFO", "client.c", LINE_NUMBER, "Send client encrypted username to server", "send_username", "start");
        
	printf("Enter password: ");
        signal(SIGINT, signalHandler);
        scanf("%s", password);
	encryption(password);

        // Send username
        // Send password
        send(client_socket, password, sizeof(password), 0);
    	logger("INFO", "client.c", LINE_NUMBER, "Send client encrypted password to server", "send_password", "start");
        send(client_socket, "a", 1, 0); // Send 'a' to indicate authentication
    	logger("INFO", "client.c", LINE_NUMBER, "Send aunthentication choice (login) to server", "send_auth_choice", "start");

        char response[50];
    	logger("DEBUG", "client.c", LINE_NUMBER, "Handle server response", "handle_response", "start");
        recv(client_socket, response, sizeof(response), 0);
        if(strcmp(response,"00") == 0) {
            printf("       Invalid username\n======Kindly Register======\n");
    	    logger("ERROR", "client.c", LINE_NUMBER, "Username doesn't exist", "handle_response", "end");
            goto start;
        }
        if(strcmp(response,"10") == 0) {
            printf("       Incorrect password\n======Kindly Retry======\n");
    	    logger("ERROR", "client.c", LINE_NUMBER, "Incorrect password", "handle_response", "end");
            goto login;
        }
        if(strcmp(response,"01") == 0) {
            printf("       User [%s] already exist\n======Kindly Register or login with different username======\n",username);
    	    logger("ERROR", "client.c", LINE_NUMBER, "Username already exists", "handle_response", "end");
            goto start;
        }
        printf("%s\n", response);
        logger("INFO", "client.c", LINE_NUMBER, "Successfully authenticated", "handle_response", "end");

        // Create a thread to receive messages from the server
        logger("DEBUG", "client.c", LINE_NUMBER, "Create a thread to receive messages from the server", "receive_messages", "start");
        pthread_t tid;
        int *sock = malloc(1);
        *sock = client_socket;
        if (pthread_create(&tid, NULL, receive_messages, (void *)sock) < 0) {
            perror("Error creating thread");
            logger("FATAL", "client.c", LINE_NUMBER, "Error creating a thread to receive messages from the server", "receive_messages", "end");
            close(client_socket);
            fclose(log_file);
	    exit(EXIT_FAILURE);
        }
        logger("INFO", "client.c", LINE_NUMBER, "Successfully created a thread to receive messages from the server", "receive_messages", "end");

        // Send messages to the server
        logger("DEBUG", "client.c", LINE_NUMBER, "Send messages to the server", "send_messages", "start");
        while (1) {
            char message[MAX_MSG_LEN];

            signal(SIGINT, signalHandler);
            fgets(message,sizeof(message),stdin);
	    int message_len = strcspn(message,"\n");
	    
	    encryption(message);
	    message[message_len] = '\0';
            send(client_socket, message, message_len, 0);
            logger("INFO", "client.c", LINE_NUMBER, "Successfully send messages to the server", "send_messages", "end");
        }
    } else {
	printf("Enter a valid choice (y/n)\n");
    	logger("ERROR", "client.c", LINE_NUMBER, "Error aunthentication choice to server, invalid choice", "send_auth_choice", "end");
	goto start;
    }

    // Close socket
    close(client_socket);

    return 0;
}

