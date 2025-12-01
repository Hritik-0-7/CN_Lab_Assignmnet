#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char fruit[20];
    char buffer[1024] = {0};

    printf("Enter fruit to buy: ");
    scanf("%s", fruit);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "10.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    send(sock, fruit, strlen(fruit), 0);
    read(sock, buffer, 1024);
    printf("Shop Response: %s\n", buffer);
    return 0;
}
