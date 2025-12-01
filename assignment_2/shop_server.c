#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

struct Fruit { char name[20]; int qty; };
struct Fruit stock[4] = {{"Apple", 5}, {"Mango", 5}, {"apple", 5}, {"mango", 5}};

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Shop Open...\n");
    while(1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        // Manual Requirement: Display Customer ID (IP and Port)
        printf("Transaction with Customer: %s:%d\n", 
               inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        read(new_socket, buffer, 1024);
        
        char response[100] = "Regret: Not Available";
        for(int i=0; i<4; i++) {
            if(strcmp(buffer, stock[i].name) == 0) {
                if(stock[i].qty > 0) {
                    stock[i].qty--;
                    sprintf(response, "Sold 1 %s. Remaining: %d", stock[i].name, stock[i].qty);
                } else {
                    strcpy(response, "Regret: Out of stock");
                }
            }
        }
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }
    return 0;
}
