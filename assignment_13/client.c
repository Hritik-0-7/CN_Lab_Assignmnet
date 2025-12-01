#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *hi = "Hi";
    char buffer[1024] = {0};

    // 1. Create Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // Convert IP address to binary
    if(inet_pton(AF_INET, "10.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 2. Connect to Server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // 3. Send "Hi" and Read Reply
    send(sock, hi, strlen(hi), 0);
    printf("Message sent to server: %s\n", hi);
    
    read(sock, buffer, 1024);
    printf("Server replied: %s\n", buffer);
    
    return 0;
}
