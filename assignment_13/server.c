#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello";

    // 1. Create Socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) { perror("socket failed"); exit(EXIT_FAILURE); }

    // 2. Bind to Port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server Listening on Port 8080...\n");

    // 4. Accept a connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    // 5. Read & Reply
    read(new_socket, buffer, 1024);
    printf("Client message: %s\n", buffer); // Should print "Hi"
    
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    
    return 0;
}
