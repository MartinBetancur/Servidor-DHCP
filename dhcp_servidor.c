#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define DHCP_SERVER_IP "192.168.1.2"
#define DHCP_SUBNET_MASK "255.255.255.0"
#define DHCP_IP_RANGE_START "192.168.1.100"
#define DHCP_IP_RANGE_END "192.168.1.150"
#define LEASE_TIME 3600

typedef struct {
    struct in_addr ip;
    struct in_addr mask;
    struct in_addr server_ip;
} dhcp_offer;

char allocated_ips[50][16];

int allocate_ip(char *allocated_ip) {
    for (int i = 100; i <= 150; ++i) {
        snprintf(allocated_ip, 16, "192.168.1.%d", i);
        int found = 0;
        for (int j = 0; j < 50; ++j) {
            if (strcmp(allocated_ips[j], allocated_ip) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            return 0;
        }
    }
    return -1;
}

void create_dhcp_offer(dhcp_offer *offer, char *offered_ip) {
    inet_pton(AF_INET, offered_ip, &offer->ip);
    inet_pton(AF_INET, DHCP_SUBNET_MASK, &offer->mask);
    inet_pton(AF_INET, DHCP_SERVER_IP, &offer->server_ip);
}

void send_dhcp_ack(struct sockaddr_in *client_addr, int sockfd, char *offered_ip) {
    sendto(sockfd, offered_ip, strlen(offered_ip), 0, (struct sockaddr *) client_addr, sizeof(*client_addr));
}

void dhcp_server() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(67);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    char buffer[1024];
    while (1) {
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, &client_addr_len);

        printf("Mensaje DHCP Discover recibido del cliente.\n"); // Mensaje para el servidor

        char offered_ip[16];
        if (allocate_ip(offered_ip) == 0) {
            strcpy(allocated_ips[client_addr_len % 50], offered_ip);
            dhcp_offer offer;
            create_dhcp_offer(&offer, offered_ip);
            send_dhcp_ack(&client_addr, sockfd, offered_ip);
            printf("Oferta DHCP enviada: IP %s\n", offered_ip); // Mensaje para el servidor
        }
    }
    close(sockfd);
}

int main() {
    dhcp_server();
    return 0;
}
