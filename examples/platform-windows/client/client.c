/*
 * Purpose: 
 * Client application to connect to google.com and request root page using HTTP/1.1
 * Programmer: Matas Noreika 26/01/19 17:36:42
*/

//LIBC I/O header
#include <stdio.h>
//LIBC header for error & dynamic memory handling
#include <stdlib.h>
//WIN32 header for socket functions and data structures
#include <winsock2.h>

//Preprocessor directive to link dynamic library into the application
//Can be done using make just added here for the purpose of the tutorial
#pragma comment(lib, "Ws2_32.lib")

//google server ip address (obtained using Powershell: 'Resolve-DnsName -Name google.com')
#define SERVER "209.85.202.139"
//Port for which HTTP service is run on
#define PORT 80
//HTTP request string
#define HTTP_REQUEST "GET / HTTP/1.1\r\n\r\n"

int main(int argc, char** argv){//start of main method

	WSADATA wsaData;//object to hold information about the windows socket implementation
	
	//intitalise winsock dynamic library
	//MAKEWORD() - macro function to generate a word object, requests version 2.2
	int result = WSAStartup(MAKEWORD(2,2), &wsaData);
	//result is a non-zero value (error)
	if(result){
		fprintf(stderr,"WSAStarup() failed %d\n",result);
		return EXIT_FAILURE;
	}

	//create a TCP socket
	//AF_INET: ipv4 address family
	//SOCK_STREAM: TCP presistent socket
	//IPPROTO_TCP: uses the TCP/IP stack for the socket protocol
	SOCKET socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socketfd == INVALID_SOCKET){
		fprintf(stderr,"Error: socket() %ld\n", WSAGetLastError());
		//deinitialises the WS2_32.lib dynamic library
		WSACleanup();
		return EXIT_FAILURE;
	}
	
	printf("Created socket\n");

	//define the server data details
	struct sockaddr_in serveraddr = {
		.sin_family = AF_INET, //set address family as IPV4
		.sin_port = htons(PORT), //convert port int as network format short
		.sin_addr.s_addr = inet_addr(SERVER) //convert string address to network long format
	};
	//check if server address was converted sucessfully
	if(serveraddr.sin_addr.s_addr == INADDR_NONE || serveraddr.sin_addr.s_addr == INADDR_ANY){
		fprintf(stderr, "Error: inet_addr() failed to convert string!\n");
		closesocket(socketfd);
		WSACleanup();
		return EXIT_FAILURE;
	}

	//attempt to connect to server
	result = connect(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(result == SOCKET_ERROR){
		fprintf(stderr,"Error: connect() %ld\n", WSAGetLastError());
		closesocket(socketfd); //close the socket
		socketfd = INVALID_SOCKET; //set the referance of socket to invalid
		WSACleanup();
		return EXIT_FAILURE;
	}
	
	printf("Connected to google.com\n");

	//buffers for communication over the socket
	char writeBuffer[100];
	//copy request into buffer, memcpy() can be used too
	snprintf(writeBuffer, sizeof(writeBuffer), HTTP_REQUEST);
	char readBuffer[4096];

	//send the HTTP request
	result = send(socketfd, writeBuffer, sizeof(writeBuffer), 0);
	if(result == SOCKET_ERROR){
		fprintf(stderr,"Error send() %ld\n", WSAGetLastError());
		closesocket(socketfd);//close the socket
		socketfd = INVALID_SOCKET;//set the socket as invalid
		WSACleanup();
		return EXIT_FAILURE;
	}

	//recieve data
	do{
		result = recv(socketfd, readBuffer, sizeof(readBuffer), 0);
		//print/dump buffer content
		printf("%s",readBuffer);
	}while(result > 0);

	//close socket
	closesocket(socketfd);
	WSACleanup();

	return EXIT_SUCCESS;
}//end of main method
