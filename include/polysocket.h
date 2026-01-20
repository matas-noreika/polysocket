/*
 * purpose:
 * API for cross-platform socket programming.
 * Programmers: Matas Noreika 26/01/20 02:28:28
*/

//HEADER GUARD
#ifndef __POLYSOCKET_H__
#define __POLYSOCKET_H__
//LIBC header for error codes
#include <errno.h>
//WINDOWS platform
#ifdef _WIN32
//Core header for socket functions and data structure on windows
#include <winsock2.h>
#endif
//LINUX & UNIX platforms
#ifdef __linux__ || __unix__
//Core header for socket functions and data structures on linux/unix
#include <sys/socket.h>
//header for network <-> host data conversions
#include <arpa/inet.h>
//header for IP family macro definitions
#include <netinet/in.h>
#endif

//function prototypes

//domain - address family type IPv4, IPv6, etc
//type - Connection type of socket, SOCK_STREAM, SOCK_DATAGRAM, etc
//protocol - The protocol that the socket will use i.e. TCP, UDP, etc
//Returns - non-negative file descriptor of socket, or -1 for error and sets errno to the error code
int socket(int domain, int type, int protocol);

#endif
//END OF HEADER GUARD
