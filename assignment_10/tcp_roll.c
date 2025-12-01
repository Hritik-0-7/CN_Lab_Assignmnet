#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

int main (void) {
    // 1. Create Raw Socket
    int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(s < 0) { perror("Socket failed"); exit(1); }

    char datagram[4096];
    char *data;
    struct iphdr *iph = (struct iphdr *) datagram;
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr ("10.0.0.2");

    memset (datagram, 0, 4096);

    // --- PAYLOAD (Manual Requirement) ---
    data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
    strcpy(data , "Roll No: CSM24041"); 

    // --- IP HEADER ---
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr) + strlen(data);
    iph->id = htonl (54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;      
    iph->saddr = inet_addr ("10.0.0.1");
    iph->daddr = sin.sin_addr.s_addr;

    // --- TCP HEADER ---
    tcph->source = htons (12345);
    tcph->dest = htons (80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;  
    tcph->fin=0;
    tcph->syn=1; // SYN Flag
    tcph->rst=0;
    tcph->psh=1; // PSH Flag (Push data)
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840);
    tcph->check = 0; 
    tcph->urg_ptr = 0;

    // Send packet
    if (sendto (s, datagram, iph->tot_len , 0, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
        perror("Send failed");
    } else {
        printf("TCP Packet sent.\n");
    }
    return 0;
}
