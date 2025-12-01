#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

int main() {
    int sock;
    struct sockaddr_in dest_addr;
    char packet[4096];
    
    // Create Raw Socket
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sock < 0) { perror("Socket Error"); return 1; }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr("10.0.0.2");

    memset(packet, 0, 4096);
    
    // Create ICMP Header
    struct icmphdr *icmp = (struct icmphdr *) packet;
    
    // Manual Requirement: ICMP Timestamp (Type 13)
    icmp->type = 13; 
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->un.echo.id = 0;
    icmp->un.echo.sequence = 0;

    // Send
    if(sendto(sock, packet, sizeof(struct icmphdr) + 12, 0, 
              (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Send failed");
    } else {
        printf("ICMP Timestamp Request sent.\n");
    }
    return 0;
}
