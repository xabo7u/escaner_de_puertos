#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

typedef struct {
    char *ip;
    int port;
} ThreadData;

DWORD WINAPI ScanPort(LPVOID param) {
    ThreadData *data = (ThreadData *)param;
    SOCKET sock;
    struct sockaddr_in target;

    //Creacion de socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET){
        printf("Error al crear socket: %d\n", WSAGetLastError());
        return 1;
    }

    //Configurar direccion
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port = htons(data->port);
    if (inet_pton(AF_INET, data->ip, &target.sin_addr) <= 0){
        printf("Direccion IP invalida: %s\n", data->ip);
        close(sock);
        return 1;
    }

    //Intentar Conectar
    if (connect(sock, (struct sockaddr *)&target, sizeof(target)) == 0) {
        printf("Puerto %d esta ABIERTO.\n", data->port);
    }

    closesocket(sock);  
    return 0;
}

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


    HANDLE *threads = malloc((end_port - start_port + 1) * sizeof(HANDLE));
    ThreadData *threadData = malloc((end_port - start_port + 1) * sizeof(ThreadData));
    
    int thread_count = 0;

    // Crear hilos para cada puerto
    for (int port = start_port; port <= end_port; port++) {
        threadData[thread_count].ip = ip;
        threadData[thread_count].port = port;
        threads[thread_count] = CreateThread(NULL, 0, ScanPort, &threadData[thread_count], 0, NULL);
        if (threads[thread_count] == NULL) {
            printf("Error al crear hilo para el puerto %d\n", port);
        }
        thread_count++;
    }

    WaitForMultipleObjects(thread_count, threads, TRUE, INFINITE);

    free(threads);
    free(threadData);


    WSACleanup();
    return 0;
}

//-lws2_32