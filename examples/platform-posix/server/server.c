/*
 * Purpose:
 * Sample program on how to open a server that will server a hello world message when client connects and close the connection.
 * Programmer: Matas Noreika 26/01/19 22:57:18
*/

//LIBC header for I/O
#include <stdio.h>
//LIBC header for error and dynamic memory handling
#include <stdlib.h>
//POSIX header for socket data structure and methods
#include <sys/socket.h>
//POSIX header for IPv4 & IPv6 conversions
#include <arpa/inet.h>
//POSIX header for more socket data structure
#include <netinet/in.h>
//POSIX header for OS level interactions
#include <unistd.h>
//POSIX header for errno
#include <errno.h>

#define SERVER_MSG "Hello World!\n"
#define PORT 1500 //Using none standard port
#define CLIENT_LIMIT 5 //the limit is capped to 128
int main(int argc, char** argv){//start of main method
  //define the address of where our server application exists
  struct sockaddr_in serveraddr = {
    .sin_family = AF_INET, //set address family to an IPv4 address
    .sin_port = htons(PORT), //assign our desired port
    .sin_addr = INADDR_ANY //localhost address 127.0.0.1 defined in netinet/in.h
  };

  //create a socket
  int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(socketfd < 0){
    fprintf(stderr,"Error: socket() failed\n");
    return EXIT_FAILURE;
  }

  //bind our application to the socket
  if(bind(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))){
    fprintf(stderr,"Error: bind() failed, code %d\n", errno);
    close(socketfd);
    return EXIT_FAILURE;
  }

  //listen for connections
  if(listen(socketfd, CLIENT_LIMIT)){
    fprintf(stderr, "Error: listen() failed, code %d\n", errno);
    close(socketfd);
    return EXIT_FAILURE;
  }

  struct sockaddr clientaddr;
  socklen_t clientaddr_size;
  //accept client - function yields until the backlog queue has at least one entry from which it will grab the top
  int client_socket = accept(socketfd, &clientaddr, &clientaddr_size);
  if(client_socket < 0){
    fprintf(stderr,"Error: accept() failed, code %d\n", errno);
    close(socketfd);
    return EXIT_FAILURE;
  }

  char writeBuffer[100] = {0};
  snprintf(writeBuffer, sizeof(writeBuffer),"%s",SERVER_MSG);
  if(send(client_socket, writeBuffer, sizeof(writeBuffer), 0) < 0){
    fprintf(stderr,"Error: send() failed, code %d\n", errno);
    close(client_socket);
    close(socketfd);
    return EXIT_FAILURE;
  }

  close(client_socket);
  close(socketfd);

  return EXIT_SUCCESS;
}//end of main method
