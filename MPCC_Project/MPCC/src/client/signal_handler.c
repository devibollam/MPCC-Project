void signalHandler(int sig_num) {
    logger_init();
    logger("INFO", "signal_handler.c", LINE_NUMBER, "Logout from client side", "client", "end");

    signal(SIGINT, signalHandler);
    if (log_file != NULL) {
        fclose(log_file);
    }
    exit(EXIT_SUCCESS);
}

