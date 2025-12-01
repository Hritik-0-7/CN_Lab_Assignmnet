#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

int main() {
    int raw_sock;
    unsigned char buffer[65536];
    
    // Create Raw Socket to listen to TCP
    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(raw_sock < 0) { perror("Socket Error"); return 1; }

    printf("Sniffer running...\n");
    while(1) {
        int data_size = recvfrom(raw_sock, buffer, 65536, 0, NULL, NULL);
        if(data_size < 0) { perror("Recv Error"); return 1; }
        
        struct iphdr *iph = (struct iphdr *)buffer;
        printf("Packet Captured! Protocol: %d | Src: %s\n", 
               iph->protocol, inet_ntoa(*(struct in_addr *)&iph->saddr));
    }
    return 0;
}
