# Multi-Party Conference Chat Application
This project is a simple multi-party conference chat application built in C using socket programming. The application consists of a server and multiple clients. Clients can register, authenticate, and communicate with the server and other clients in a chat environment.

# Features
# Client Registration and Authentication

Clients can register a new account with a username and password.
Registered users can log in using their credentials.
Secure Communication
# Project Structure
├── inc/
│   ├── client_utility.h      # Header file for client utilities
│   ├── server_utility.h      # Header file for server utilities
├── client/
│   ├── client.c              # Main client application
│   ├── client_logger.c       # Logging utility for client
│   ├── encrypt_decrypt.c     # Encryption and decryption functions for client
│   ├── receive_messages.c    # Threaded message receiving function for client
│   ├── signal_handler.c      # Signal handler for client
├── server/
│   ├── server.c              # Main server application
│   ├── server_logger.c       # Logging utility for server
│   ├── encrypt_decrypt.c     # Encryption and decryption functions for server
│   ├── register_client.c     # Handles client registration
│   ├── authenticate.c        # Handles client authentication
│   ├── handle_client.c       # Handles communication with clients
│   ├── signal_handler.c      # Signal handler for server

# Prerequisites
Operating System: Linux/Unix-based system (preferred) or Windows with POSIX compatibility.
Compiler: GCC (for compiling C programs).
Libraries:
pthread (for multi-threading).
socket (for network communication).
Compilation and Execution
Server
Navigate to the server/ directory:
