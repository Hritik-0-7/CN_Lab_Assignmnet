#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

int main(void) {
    int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(s < 0) { perror("Socket Error"); return 1; }

    char datagram[4096];
    char *data;
    struct iphdr *iph = (struct iphdr *) datagram;
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof(struct ip));
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(80); // Destination Port
    sin.sin_addr.s_addr = inet_addr("10.0.0.2"); // Target IP

    memset(datagram, 0, 4096);

    // 1. Data Part (Payload) - Manual Requirement: Your Roll Number
    data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
    strcpy(data, "Roll No: CSM24041"); 

    // 2. IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data);
    iph->id = htonl(54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0; // Kernel fills this
    iph->saddr = inet_addr("10.0.0.1"); // Source IP
    iph->daddr = sin.sin_addr.s_addr;

    // 3. TCP Header
    tcph->source = htons(12345); // Source Port
    tcph->dest = htons(80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5; // Header size
    tcph->fin = 0;
    tcph->syn = 1; // SYN flag set
    tcph->rst = 0;
    tcph->psh = 1; // Push data immediately
    tcph->ack = 0;
    tcph->urg = 0;
    tcph->window = htons(5840);
    tcph->check = 0; // Checksum (Left 0 for demo purposes)
    tcph->urg_ptr = 0;

    // Send the packet
    if (sendto(s, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("Send failed");
    } else {
        printf("TCP Packet with Roll Number sent.\n");
    }
    return 0;
}
