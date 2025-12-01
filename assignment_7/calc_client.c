#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024];
    char input[50];
    socklen_t len = sizeof(servaddr);

    printf("Enter op (e.g., 'sin 1.57'): ");
    fgets(input, 50, stdin);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    inet_pton(AF_INET, "10.0.0.1", &servaddr.sin_addr);

    sendto(sockfd, input, strlen(input), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    int n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';
    printf("%s\n", buffer);
}
