/*
 * Purpose:
 * Implementation of polysocket library.
 * Programmers: Matas Noreika 26/01/20 03:10:09
*/

//polysocket API
#include <polysocket.h>

//domain - address family type IPv4, IPv6, etc
//type - Connection type of socket, SOCK_STREAM, SOCK_DATAGRAM, etc
//protocol - The protocol that the socket will use i.e. TCP, UDP, etc
//Returns - non-negative file descriptor of socket, or -1 for error and sets errno to the error code
int socket(int domain, int type, int protocol);
int socket(int af, int type, int protocol){
  return 0;
}
