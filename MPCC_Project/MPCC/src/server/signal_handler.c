void signalHandler(int sig_num) {
    signal(SIGINT, signalHandler);
    
    logger_init(); 
    logger("INFO", "signal_handler.c", LINE_NUMBER, "Server stopped listening", "server_listen", "end");
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++) {
	close(clients[i].socket);    	
    }
    pthread_mutex_unlock(&mutex);

    close(server_socket);
    if (log_file != NULL) {
        fclose(log_file);
    }
    exit(EXIT_SUCCESS);
}

