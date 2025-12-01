#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define SIZE 1024

void send_file(FILE *fp, int sockfd) {
    char data[SIZE] = {0};
    while (fgets(data, SIZE, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("Error sending file.");
            exit(1);
        }
        bzero(data, SIZE);
    }
}

void write_file(int sockfd) {
    int n;
    FILE *fp;
    char *filename = "server_received.txt";
    char buffer[SIZE];

    fp = fopen(filename, "w");
    while (1) {
        n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0) {
            break;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    fclose(fp);
}

int main() {
    int server_fd, new_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    clock_t start, end;
    double cpu_time_used;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    printf("[Server] Starting...\n");

    // 1. Accept Client
    new_sock = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    
    // --- PHASE 1: DOWNLOAD (Send File to Client) ---
    printf("[Server] Phase 1: Sending server_file.txt...\n");
    FILE *fp = fopen("server_file.txt", "r");
    if (fp == NULL) { perror("Error reading file"); return 1; }
    
    start = clock();
    send_file(fp, new_sock);
    end = clock();
    
    printf("[Server] File sent successfully.\n");
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[Server] Time taken to send: %f seconds\n", cpu_time_used);
    fclose(fp);

    // Close connection and wait for Client to reconnect for Upload
    // (This is the simplest way to separate streams for beginners)
    close(new_sock); 

    // --- PHASE 2: UPLOAD (Receive File from Client) ---
    printf("[Server] Phase 2: Waiting for Client Upload...\n");
    new_sock = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    
    start = clock();
    write_file(new_sock);
    end = clock();
    
    printf("[Server] File received from client.\n");
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[Server] Time taken to receive: %f seconds\n", cpu_time_used);

    close(new_sock);
    return 0;
}
