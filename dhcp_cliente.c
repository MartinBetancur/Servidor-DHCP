#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define DHCP_SERVER_PORT 67  // Puerto en el que escucha el servidor DHCP
#define DHCP_CLIENT_PORT 5000 // Cambiado a 5000

// Estructura de datos para almacenar la oferta de DHCP
typedef struct {
    char offered_ip[16];
    char subnet_mask[16];
    char server_ip[16];
    unsigned int lease_time;
} dhcp_offer_t;

void send_dhcp_discover(int sockfd, struct sockaddr_in *server_addr) {
    // Crear paquete DHCP Discover (simplificado)
    char discover_msg[256];
    memset(discover_msg, 0, sizeof(discover_msg));
    strcpy(discover_msg, "DHCPDISCOVER");

    // Enviar mensaje DHCP Discover
    if (sendto(sockfd, discover_msg, sizeof(discover_msg), 0, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("Error al enviar DHCP Discover");
        exit(EXIT_FAILURE);
    }

    printf("Mensaje DHCP Discover enviado al servidor.\n");
}

void receive_dhcp_offer(int sockfd, struct sockaddr_in *server_addr, dhcp_offer_t *offer) {
    char buffer[512];
    socklen_t addr_len = sizeof(*server_addr);

    // Recibir paquete DHCP Offer del servidor
    if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)server_addr, &addr_len) < 0) {
        perror("Error al recibir DHCP Offer");
        exit(EXIT_FAILURE);
    }

    // Parsear el mensaje DHCP Offer (simplificado)
    sscanf(buffer, "%15s %15s %15s %u", offer->offered_ip, offer->server_ip, offer->subnet_mask, &offer->lease_time);
    printf("Oferta DHCP recibida: IP %s, Máscara %s, Servidor %s, Tiempo de concesión %u\n",
           offer->offered_ip, offer->subnet_mask, offer->server_ip, offer->lease_time);
}

void send_dhcp_request(int sockfd, struct sockaddr_in *server_addr, dhcp_offer_t *offer) {
    // Crear paquete DHCP Request
    char request_msg[256];
    memset(request_msg, 0, sizeof(request_msg));
    sprintf(request_msg, "DHCPREQUEST %s", offer->offered_ip);

    // Enviar mensaje DHCP Request al servidor
    if (sendto(sockfd, request_msg, sizeof(request_msg), 0, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("Error al enviar DHCP Request");
        exit(EXIT_FAILURE);
    }

    printf("Solicitud DHCP Request enviada para la IP %s.\n", offer->offered_ip);
}

void receive_dhcp_ack(int sockfd, struct sockaddr_in *server_addr, dhcp_offer_t *offer) {
    char buffer[512];
    socklen_t addr_len = sizeof(*server_addr);

    // Recibir el DHCP ACK confirmando la asignación de IP
    if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)server_addr, &addr_len) < 0) {
        perror("Error al recibir DHCP ACK");
        exit(EXIT_FAILURE);
    }

    printf("IP %s confirmada y asignada correctamente por el servidor DHCP.\n", offer->offered_ip);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    dhcp_offer_t offer;

    // Crear socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Error al crear el socket UDP");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del cliente
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(DHCP_CLIENT_PORT);  // Cambiado a 5000
    client_addr.sin_addr.s_addr = INADDR_ANY;  // Escuchar en cualquier dirección local

    // Asignar la dirección al socket
    if (bind(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
        perror("Error al asignar el puerto del cliente");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor DHCP
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DHCP_SERVER_PORT);  // Puerto del servidor DHCP
    inet_pton(AF_INET, "192.168.1.1", &server_addr.sin_addr);  // IP del servidor DHCP

    // Enviar mensaje DHCP Discover
    send_dhcp_discover(sockfd, &server_addr);

    // Recibir oferta DHCP (DHCP Offer)
    receive_dhcp_offer(sockfd, &server_addr, &offer);

    // Enviar solicitud DHCP Request
    send_dhcp_request(sockfd, &server_addr, &offer);

    // Recibir confirmación de asignación de IP (DHCP ACK)
    receive_dhcp_ack(sockfd, &server_addr, &offer);

    // Cerrar socket
    close(sockfd);
    printf("Cliente DHCP finalizado.\n");

    return 0;
}
