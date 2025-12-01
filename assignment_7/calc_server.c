#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[1024];
    socklen_t len = sizeof(cliaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);
    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Calc Server Ready...\n");
    while(1) {
        int n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        
        double val, res = 0.0;
        char op[10];
        sscanf(buffer, "%s %lf", op, &val); // Parse "sin 1.57"
        
        if(strcmp(op, "sin") == 0) res = sin(val);
        else if(strcmp(op, "cos") == 0) res = cos(val);
        else if(strcmp(op, "tan") == 0) res = tan(val);

        sprintf(buffer, "Result: %.2f", res);
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);
    }
}
