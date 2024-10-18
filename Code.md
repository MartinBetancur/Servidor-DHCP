# Código del Servidor DHCP

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define DHCP_SERVER_PORT 67  // Puerto en el que escucha el servidor DHCP
#define DHCP_CLIENT_PORT 68  // Puerto del cliente DHCP

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
    client_addr.sin_port = htons(DHCP_CLIENT_PORT);  // Puerto de cliente DHCP
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

Instalación de paquetes para Ubuntu
sudo apt-get install build-essential
sudo apt-get install gcc

Instalación de paquetes para CentOS
sudo yum groupinstall "Development Tools"
sudo yum install gcc

Comando para verificar conexión
ip a

Asignar IP en una red interna
sudo ip addr add 192.168.1.2/24 dev enp0s3
sudo ip link set enp0s3 up

Verificar que los puertos 67 y 68 estén abiertos (Ubuntu)
sudo ufw allow 67/udp
sudo ufw allow 68/udp

(CentOS)
sudo firewall-cmd --zone=public --add-port=67/udp --permanent
sudo firewall-cmd --zone=public --add-port=68/udp --permanent
sudo firewall-cmd --reload

Configuración para red interna (Ubuntu)
sudo nano /etc/netplan/01-netcfg.yaml

network:
  version: 2
  ethernets:
    enp0s8:   # El nombre del segundo adaptador, puede ser diferente (verifícalo con ip a)
      dhcp4: no
      addresses: [192.168.1.2/24]

sudo netplan apply

Configuración para red interna (CentOS)
sudo nano /etc/sysconfig/network-scripts/ifcfg-enp0s8

DEVICE=enp0s8  # Verifica que sea el nombre correcto del segundo adaptador
BOOTPROTO=none
ONBOOT=yes
IPADDR=192.168.1.3
PREFIX=24

sudo systemctl restart network

Solución de conexión para CentOS
nmcli device status

nmcli connection modify enp0s8 ipv4.addresses 192.168.1.3/24
nmcli connection modify enp0s8 ipv4.method manual
nmcli connection up enp0s8

Se creó una conexión desde "0"
sudo nmcli connection add type ethernet ifname enp0s8 con-name enp0s8

sudo nmcli connection modify enp0s8 ipv4.addresses 192.168.1.3/24
sudo nmcli connection modify enp0s8 ipv4.method manual

sudo nmcli connection up enp0s8

ping 192.168.1.2

Configuración de la máquina para correr el servidor
sudo nano /etc/netplan/50-cloud-init.yaml

network:
  version: 2
  renderer: networkd
  ethernets:
    enp0s8:
      dhcp4: no
      addresses:
        - 192.168.1.1/24
      gateway4: 192.168.1.1

sudo systemctl restart NetworkManager

Ejecutar el servidor
gcc -o dhcp_server server.c
sudo ./dhcp_server

Ejecutar cliente
gcc -o dhcp_client dhcp_client.c
sudo ./dhcp_client

Comprobar la asignación IP bash
ip addr show enp0s8

Crear archivo de C en el terminal
nano server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define DHCP_SERVER_PORT 67  // Puerto en el que escucha el servidor DHCP
#define DHCP_CLIENT_PORT 68  // Puerto del cliente DHCP

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
    client_addr.sin_port = htons(DHCP_CLIENT_PORT);  // Puerto de cliente DHCP
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

Instalación de paquetes para Ubuntu:
sudo apt-get install build-essential
sudo apt-get install gcc

Instalación de paquetes para CentOS:
sudo yum groupinstall "Development Tools"
sudo yum install gcc


Comando para verificar conexión:
ip a

Asignar IP en una red interna:
sudo ip addr add 192.168.1.2/24 dev enp0s3
sudo ip link set enp0s3 up

Verificar que los puertos 67 y 68 estén abiertos (Ubuntu):
sudo ufw allow 67/udp
sudo ufw allow 68/udp

Verificar que los puertos 67 y 68 estén abiertos (CentOS):
sudo firewall-cmd --zone=public --add-port=67/udp --permanent
sudo firewall-cmd --zone=public --add-port=68/udp --permanent
sudo firewall-cmd --reload

Configuración para red interna (Ubuntu):
sudo nano /etc/netplan/01-netcfg.yaml

network:
  version: 2
  ethernets:
    enp0s8:   # El nombre del segundo adaptador, puede ser diferente (verifícalo con ip a)
      dhcp4: no
      addresses: [192.168.1.2/24]

sudo netplan apply

Configuración para red interna (CentOS):
sudo nano /etc/sysconfig/network-scripts/ifcfg-enp0s8
DEVICE=enp0s8  # Verifica que sea el nombre correcto del segundo adaptador
BOOTPROTO=none
ONBOOT=yes
IPADDR=192.168.1.3
PREFIX=24
sudo systemctl restart network

Solución de conexión para CentOS:
nmcli device status
nmcli connection modify enp0s8 ipv4.addresses 192.168.1.3/24
nmcli connection modify enp0s8 ipv4.method manual
nmcli connection up enp0s8

Crear conexión desde "0":
sudo nmcli connection add type ethernet ifname enp0s8 con-name enp0s8
sudo nmcli connection modify enp0s8 ipv4.addresses 192.168.1.3/24
sudo nmcli connection modify enp0s8 ipv4.method manual
sudo nmcli connection up enp0s8
ping 192.168.1.2

Configuración de la máquina para correr el servidor:
sudo nano /etc/netplan/50-cloud-init.yaml
network:
  version: 2
  renderer: networkd
  ethernets:
    enp0s8:
      dhcp4: no
      addresses:
        - 192.168.1.1/24
      gateway4: 192.168.1.1
sudo systemctl restart NetworkManager

Ejecutar el servidor:
gcc -o dhcp_server server.c
sudo ./dhcp_server

Ejecutar cliente:
gcc -o dhcp_client dhcp_client.c
sudo ./dhcp_client

Comprobar la asignación de IP:
ip addr show enp0s8

Crear archivo de C en el terminal:
nano server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define DHCP_SERVER_IP "192.168.1.2"
#define DHCP_SUBNET_MASK "255.255.255.0"
#define DHCP_IP_RANGE_START "192.168.1.100"
#define DHCP_IP_RANGE_END "192

.168.1.200"
#define DHCP_LEASE_TIME 86400  // Tiempo de concesión en segundos

void send_dhcp_offer(int sockfd, struct sockaddr_in *client_addr) {
    char offer_msg[512];
    memset(offer_msg, 0, sizeof(offer_msg));
    snprintf(offer_msg, sizeof(offer_msg),
             "DHCPOFFER %s %s %s %d", 
             DHCP_IP_RANGE_START, 
             DHCP_SERVER_IP, 
             DHCP_SUBNET_MASK, 
             DHCP_LEASE_TIME);
    
    // Enviar el mensaje de oferta al cliente
    if (sendto(sockfd, offer_msg, sizeof(offer_msg), 0, 
               (struct sockaddr *)client_addr, sizeof(*client_addr)) < 0) {
        perror("Error al enviar DHCP Offer");
        exit(EXIT_FAILURE);
    }

    printf("Oferta DHCP enviada a %s.\n", inet_ntoa(client_addr->sin_addr));
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Crear el socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(DHCP_SERVER_IP);
    server_addr.sin_port = htons(DHCP_SERVER_PORT);

    // Asignar dirección al socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al asignar el puerto del servidor");
        exit(EXIT_FAILURE);
    }

    printf("Servidor DHCP iniciado y escuchando en %s:%d...\n", DHCP_SERVER_IP, DHCP_SERVER_PORT);

    while (1) {
        char buffer[512];
        memset(buffer, 0, sizeof(buffer));

        // Recibir mensaje de DHCP Discover
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, 
                     (struct sockaddr*)&client_addr, &addr_len) < 0) {
            perror("Error al recibir DHCP Discover");
            continue;
        }

        printf("Mensaje recibido: %s de %s\n", buffer, inet_ntoa(client_addr.sin_addr));

        // Enviar oferta DHCP al cliente
        send_dhcp_offer(sockfd, &client_addr);
    }

    // Cerrar el socket
    close(sockfd);
    return 0;
}
