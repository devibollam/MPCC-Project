#include "../../inc/server_utility.h"
#include "server_logger.c"
#include "signal_handler.c"
#include "encrypt_decrypt.c"
#include "register_client.c"
#include "authenticate.c"
#include "handle_client.c"

int main() {
   
    logger_init();

    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    logger("DEBUG", "server.c", LINE_NUMBER, "Create server socket", "server_socket_creation", "start");
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
	logger("FATAL", "server.c", LINE_NUMBER, "Error in server socket", "server_socket_creation", "end");
        perror("Error in socket");
        fclose(log_file);
        exit(EXIT_FAILURE);
    }
    logger("INFO", "server.c", LINE_NUMBER, "Successfully created server socket", "server_socket_creation", "end");

    // Set server address
    logger("INFO", "server.c", LINE_NUMBER, "Set server address", "set_server_address", "start");
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(11112); // Port number
    logger("INFO", "server.c", LINE_NUMBER, "Set server address", "set_server_address", "end");

    // Bind socket to address
    logger("DEBUG", "server.c", LINE_NUMBER, "Bind server socket to address", "bind_socket_address", "start");
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {	
        logger("FATAL", "server.c", LINE_NUMBER, "Error bind server socket to address", "bind_socket_address", "end");
        perror("Error in bind");
        fclose(log_file);
        exit(EXIT_FAILURE);
    }
    logger("INFO", "server.c", LINE_NUMBER, "Successfully bind server socket to address", "bind_socket_address", "end");

    // Listen for connections
    logger("DEBUG", "server.c", LINE_NUMBER, "Listen for connections", "conections_listen", "start");
    if (listen(server_socket, 5) < 0) {
    	logger("FATAL", "server.c", LINE_NUMBER, "Error listen for connections", "conections_listen", "end");
        perror("Error in listen");
        fclose(log_file);
        exit(EXIT_FAILURE);
    }
    logger("INFO", "server.c", LINE_NUMBER, "Successfully listen for connections", "conections_listen", "end");

    printf("Server is listening...\n");
    logger("INFO", "server.c", LINE_NUMBER, "Server is active and started listening", "server_listen", "start");
fclose(log_file);
    while (1) {
        signal(SIGINT, signalHandler);
	
        // Accept connection
        logger("DEBUG", "server.c", LINE_NUMBER, "Accept connection", "accept_conection", "start");
	client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
    	    logger("FATAL", "server.c", LINE_NUMBER, "Error accept connection", "accept_conection", "end");
            perror("Error in accept");
            fclose(log_file);
            exit(EXIT_FAILURE);
        }
    	logger("INFO", "server.c", LINE_NUMBER, "Successfully accept connection", "accept_conection", "end");

        printf("[::]: New client connected\n");

        char buffer[MAX_MSG_LEN]="::|New client connected";
        encryption(buffer);
        for (int i = 0; i < num_clients; i++) {
            if (clients[i].socket != client_socket) {
                send(clients[i].socket,buffer, sizeof(buffer), 0);
            }
        }

        // Handle client in a separate thread
        logger("DEBUG", "server.c", LINE_NUMBER, "Handle client in a separate thread", "handle_client_thread", "start");
        pthread_t tid;
        int *client_sock = malloc(1);
        *client_sock = client_socket;
        if (pthread_create(&tid, NULL, handle_client, (void *)client_sock) < 0) {
            perror("Error creating thread");
            logger("FATAL", "server.c", LINE_NUMBER, "Error Handle client in a separate thread", "handle_client_thread", "end");
            close(client_socket);
            fclose(log_file);
            exit(EXIT_FAILURE);
        }
        pthread_detach(tid);
        logger("INFO", "server.c", LINE_NUMBER, "Successfully Handle client in a separate thread", "handle_client_thread", "end");
    }

    return 0;
}
