#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#define SIZE 1024

void write_file(int sockfd) {
    int n;
    FILE *fp;
    char *filename = "client_received.txt";
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

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    clock_t start, end;
    double cpu_time_used;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "10.0.0.1", &server_addr.sin_addr);

    // --- PHASE 1: DOWNLOAD (Receive from Server) ---
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection Error"); return 1;
    }
    printf("[Client] Connected. Downloading file...\n");

    start = clock();
    write_file(sockfd);
    end = clock();

    printf("[Client] Download complete.\n");
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[Client] Download Time: %f seconds\n", cpu_time_used);
    close(sockfd);

    sleep(1); // Brief pause to ensure server is ready

    // --- PHASE 2: UPLOAD (Send to Server) ---
    printf("[Client] Reconnecting to Upload file...\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    FILE *fp = fopen("client_file.txt", "r");
    if (fp == NULL) { perror("Error reading file"); return 1; }

    start = clock();
    send_file(fp, sockfd);
    end = clock();

    printf("[Client] Upload complete.\n");
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[Client] Upload Time: %f seconds\n", cpu_time_used);
    
    fclose(fp);
    close(sockfd);
    return 0;
}
