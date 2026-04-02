/* 
* Purpose: 
* Windows Server application to listen for incoming HTTP requests and respond with a simple message 
*
*/

//LIBC header for I/O
#include <stdio.h>
//LIBC header for error and dynamic memory handling
#include <stdlib.h>
//POSIX header for socket data structure and methods
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
//Preprocessor directive to link dynamic library into the application
//Can be done using make just added here for the purpose of the tutorial
#pragma comment(lib, "Ws2_32.lib")

#define SERVER_MSG "Hi Bro!\n"
#define SERVER "127.0.0.1"
#define PORT 1500 //Using none standard port
#define CLIENT_LIMIT 5 //the limit is capped to 128

int main(int argc, char** argv){//start of main method
  //define the address of where our server application exists
struct sockaddr_in serveraddr = {
    .sin_family = AF_INET,
    .sin_port = htons(PORT),
    .sin_addr.s_addr = htonl(INADDR_ANY)
};

// Winsock startup
WSADATA wsaData;
// Winsock version 2.2 is requested
int result = WSAStartup(MAKEWORD(2,2), &wsaData);
if(result){
    fprintf(stderr, "WSAStartup() failed %d\n", result);
    return EXIT_FAILURE;
}

  //create a socket
SOCKET socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if(socketfd == INVALID_SOCKET){
    fprintf(stderr, "Error: socket() %ld\n", WSAGetLastError());
    WSACleanup();
    return EXIT_FAILURE;
}

  //bind our application to the socket
  if(bind(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))){
    fprintf(stderr, "Error: bind() failed, code %ld\n", WSAGetLastError()); // WSAGetLastError() is the winsock equivalent of errno
    closesocket(socketfd);
    WSACleanup();
    return EXIT_FAILURE;
}

  //listen for connections
  if(listen(socketfd, CLIENT_LIMIT)){
    fprintf(stderr, "Error: listen() failed, code %ld\n", WSAGetLastError());
    closesocket(socketfd);
    WSACleanup();
    return EXIT_FAILURE;
  }

  struct sockaddr clientaddr;
  int clientaddr_size = sizeof(clientaddr);
  //accept client - function yields until the backlog queue has at least one entry from which it will grab the top
  SOCKET client_socket = accept(socketfd, &clientaddr, &clientaddr_size);
  if(client_socket == INVALID_SOCKET){
    fprintf(stderr, "Error: accept() failed, code %ld\n", WSAGetLastError());
    closesocket(socketfd);
    WSACleanup();
    return EXIT_FAILURE;
}

  char writeBuffer[100] = {0};
  snprintf(writeBuffer, sizeof(writeBuffer),"%s",SERVER_MSG);
  // Send only what the string actually contains, not the entire buffer
  result = send(client_socket, writeBuffer, (int)strlen(writeBuffer), 0);
  if(result == SOCKET_ERROR){
    fprintf(stderr, "Error: send() failed, code %ld\n", WSAGetLastError());
    closesocket(client_socket);
    closesocket(socketfd);
    WSACleanup();
    return EXIT_FAILURE;
}

  closesocket(client_socket);
  closesocket(socketfd);
  WSACleanup();

  return EXIT_SUCCESS;
}//end of main method

/*
// Sources:
https://medium.com/@adityakumarbgs6/network-programming-part-1-using-winsock-programming-sockets-1978e4de94a2
*/
