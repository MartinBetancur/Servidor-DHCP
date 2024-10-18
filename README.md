# Servidor-DHCP
## 1. Introducción
El objetivo de este proyecto es desarrollar las competencias necesarias en el diseño y desarrollo de aplicaciones concurrentes en red utilizando la API Sockets. En este caso, se implementa un servidor DHCP funcional en lenguaje C que permite la asignación dinámica de direcciones IP a clientes conectados a una red local o remota. El servidor maneja solicitudes de los clientes utilizando el protocolo DHCP, proporcionando parámetros esenciales como dirección IP, máscara de subred, puerta de enlace y servidor DNS.

## 2. Desarrollo
### Cliente DHCP
El cliente fue programado en lenguaje C y utiliza la API de sockets de Berkeley. Este cliente envía una solicitud DHCPDISCOVER al servidor y espera recibir la configuración de red correspondiente. El código del cliente incluye las siguientes características:
- Solicitud de dirección IP.
- Renovación y liberación de la dirección IP.
- Gestión de errores y reconexiones automáticas en caso de fallo de red.

### Servidor DHCP
El servidor fue implementado en lenguaje C y permite la asignación dinámica de direcciones IP a los clientes conectados. Este servidor es capaz de manejar múltiples solicitudes simultáneamente gracias al uso de hilos para concurrencia. Algunas características clave incluyen:
- Gestión de un rango de direcciones IP.
- Registro de concesiones (leases) de IPs.
- Soporte para subredes distintas mediante un DHCP relay.

El servidor fue desplegado en una máquina virtual CentOS en AWS, mientras que el cliente se ejecutó en una máquina virtual Ubuntu.

### Herramientas utilizadas
- Lenguaje: C.
- API de Sockets: Berkeley.
- Plataforma: AWS para servidor, Ubuntu para cliente.
- Manejo de concurrencia: Hilos (threads) en C.

## 3. Aspectos Logrados y No logrados
### Aspectos logrados
- Implementación correcta de la API sockets para cliente y servidor.
- El servidor puede manejar múltiples clientes simultáneamente y asignar direcciones IP de manera eficiente.
- Despliegue exitoso del servidor en un entorno en la nube (AWS).
- Soporte para el protocolo DHCP, incluyendo los mensajes DISCOVER, OFFER, REQUEST, y ACK.
- Renovación y liberación de IPs correctamente manejadas por el servidor.

### Aspectos no logrados
- **Problemas de conexión**: Durante las pruebas finales, se encontraron errores al intentar conectar ambas máquinas virtuales. Se observó que la máquina cliente no recibía correctamente la dirección IP asignada. Después de investigar, se determinó que el problema estaba relacionado con la configuración de red de la máquina virtual en CentOS.
- **Limitaciones en el manejo de subredes**: Aunque el servidor soporta subredes, hubo dificultades en la configuración de DHCP relay para subredes diferentes a la del servidor principal.
  
## 4. Conclusiones
El desarrollo de este servidor DHCP en C fue una experiencia valiosa para el aprendizaje de aplicaciones concurrentes en red. Se logró implementar un servidor funcional que gestiona correctamente direcciones IP para los clientes conectados. Sin embargo, el proyecto mostró algunos desafíos técnicos, especialmente en la configuración de redes virtuales, que impidieron la ejecución final sin errores. El uso de hilos para la concurrencia fue clave en el manejo eficiente de múltiples solicitudes DHCP de manera simultánea. 

A futuro, se recomienda mejorar la integración del servidor con diferentes configuraciones de red y corregir los errores en la configuración de subredes.

## 5. Referencias
- RFC 2131: Dynamic Host Configuration Protocol (DHCP).
- API Sockets Berkeley: [Enlace a la documentación](https://man7.org/linux/man-pages/man7/socket.7.html).
- AWS Academy: Plataforma utilizada para el despliegue del servidor.
- Linux Manual Pages para `sockets` y `threads` en C: [Enlace a la documentación](https://man7.org/linux/man-pages/).
