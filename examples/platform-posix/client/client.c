/*
 * Purpose:
 * POSIX compliant example of client TCP socket connection.
 * The program attempts to connect to google and request root site information using HTTP 1.1 request.
 * Programmer: Matas Noreika 14/01/26 16:15:42
*/

//LIBC header for I/O
#include <stdio.h>
//LIBC header for error & dynamic memory handling
#include <stdlib.h>
//POSIX header for core socket functions and data structures
#include <sys/socket.h>
//POSIX header for network <-> conversion functions - inet_pton()
#include <arpa/inet.h>
//POSIX header for file control - close()
#include <unistd.h>
//google server ip address (obtained using: 'host google.com')
#define SERVER "209.85.202.139"
//Port to connect to on server
#define PORT 80
//Standard http version 1.1 request
#define HTTP_REQUEST "GET / HTTP/1.1\r\n\r\n"

int main(int argc, char** argv){//start of main method

  //define the type of address the server is formatted in
  //https://man7.org/linux/man-pages/man3/sockaddr.3type.html
  struct sockaddr_in server_addr = {
    .sin_family = AF_INET, //Sets the address to IPV4 format
    .sin_port = htons(PORT), //Sets the port from host to network short (network big-endian formatting)
  };
  //convert IPV4 string to network format (long)
  if(inet_pton(AF_INET, SERVER, &server_addr.sin_addr) <= 0){
    fprintf(stderr,"Error: failed to convert server ipv4 address from string\n");
    return EXIT_FAILURE;
  }

  //open a TCP socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0){
    fprintf(stderr,"Error: failed to create socket\n");
    return EXIT_FAILURE;
  }

  //write buffer for tcp connection
  char writeBuffer[100];
  snprintf(writeBuffer, sizeof(writeBuffer),"%s",HTTP_REQUEST);

  //buffer to read content from tcp connection
  char readBuffer[4096];

  //attempt connection to google
  if(!connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))){
    printf("Connected to google.\n");

    //send the http request
    if(send(sockfd,writeBuffer, sizeof(writeBuffer), 0) < 0){
      fprintf(stderr, "Error: send failed\n");
    }else {
      printf("HTTP request sent.\n");
    }

    //read reply content from server
    size_t bytes;
    while( (bytes = recv(sockfd, readBuffer, sizeof(readBuffer), 0)) > 0){
      printf("%s",readBuffer);
    }

    //error occured with read (bytes non positive integer)
     if(bytes < 0){
      fprintf(stderr,"\nError: recv() failed\n");
    }

  }else {
    fprintf(stderr,"Error: failed to connect to google\n");
  }

  //close the socket
  close(sockfd);
  return 0;
}//end of main method
