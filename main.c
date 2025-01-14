#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Uso: %s <IP> <puerto_inicial> <puerto_final>/n", argv[0]);
        return 1;
    }
    
    char *ip = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

    //Inicializacion de winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("Error al inicializar Winsock.\n");
        return 1;
    }

    for (int port = start_port; port <= end_port; port++){
        SOCKET sock;
        struct sockaddr_in target;

        //Creacion de socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == INVALID_SOCKET){
            printf("Error al crear socket: %d\n", WSAGetLastError());
            continue;
        }

        //Configurar direccion
        memset(&target, 0, sizeof(target));
        target.sin_family = AF_INET;
        target.sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &target.sin_addr) <= 0){
            printf("Direccion IP invalida: %s\n", ip);
            close(sock);
            continue;
        }

        //Intentar Conectar
        if (connect(sock, (struct sockaddr *)&target, sizeof(target)) == 0) {
            printf("Puerto %d esta ABIERTO.\n", port);
        } else {
            printf("Puerto %d esta CERRADO.\n", port);
        }

        closesocket(sock);
    }

    WSACleanup();
    return 0;
}

//-lws2_32