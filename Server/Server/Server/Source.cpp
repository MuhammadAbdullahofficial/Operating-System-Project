

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <WS2tcpip.h>

#include <process.h>

#pragma comment (lib, "ws2_32.lib")

#define PORT 54000


void read_line(char* buf)
{
    char ch;
    while ((ch = fgetc(stdin)) != 0x0A)
    {
        *buf = ch;
        buf++;
    }
    *buf = '\0';
}


int main(int argc, char* argv[])
{
    SOCKET              sock;
    WSADATA             wsa;
    struct sockaddr_in  address;
    
    //Initalization socket
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d.\nPress a key to exit...", WSAGetLastError());
        _getch();
        return 1;
    }

    //create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d.\n", WSAGetLastError());
        WSACleanup();
        _getch();
        return 1;
    }

    //CONNECTING ...
    memset(&address, 0, sizeof(address));
    address.sin_addr.S_un.S_addr = INADDR_ANY; // ip address
    address.sin_family = AF_INET; //family
    address.sin_port = htons(PORT); // port

    //binding the socket to port
    bind(sock, (sockaddr*)&address, sizeof(address));

    //listening at the sockect
    listen(sock, SOMAXCONN);


    //wait for connection
    struct sockaddr_in client;
    int clientSize = sizeof(client);

    printf("[+] Waiting for incoming connections\n");
    SOCKET clientSock = accept(sock, (sockaddr*)&client, &clientSize);
    printf("[+] Got a connection %ld\n", client.sin_port);

    // sending commands
    char buf[4096];
    while (true)
    {
        printf(">> ");
        read_line(buf);
        int sendResult = send(clientSock, buf, strlen(buf) + 1, 0);
        if (sendResult == SOCKET_ERROR)
        {
            printf("ERROR in sending data : %d", WSAGetLastError());            
            WSACleanup();            
            break;
        }
        if (sendResult != SOCKET_ERROR)
        {
            ZeroMemory(buf, 4096);
            int bytesRecived = recv(clientSock, buf, 4096, 0);
            if (bytesRecived > 0)
            {
                fwrite(buf, 1, bytesRecived, stdout);
            }
        }
        printf("\n");
    }

    // succfully closed coonection
    closesocket(sock);
    WSACleanup();
    _getch();
    return 0;

}

