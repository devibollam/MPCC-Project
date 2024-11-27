// Simple XOR encryption function
void encryption(char *text) {
    //char *encrypted = (char *)malloc(strlen(text));
    for (int i = 0; i < strlen(text); i++) {
        text[i] = text[i] ^ ENCRYPTION_KEY;
    }
}

// Simple XOR decryption function
void decryption(char *text) {
    //char *decrypted = (char *)malloc(strlen(text));
    for (int i = 0; i < strlen(text); i++) {
        text[i] = text[i] ^ ENCRYPTION_KEY;
    }
}
