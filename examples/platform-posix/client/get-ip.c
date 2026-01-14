/*
 * Purpose:
 * Example program that creates a utility program to resolve domain name string to server address.
 * Similar shell/bash command: host <domain name>
 * The program will take in first argument and translate it (if possible) to an IPV4 address.
 * For greater detail into the functionality refer to the man pages for getaddrinfo(). I.E. 'man getaddrinfo'
 * Programmer: Matas Noreika 14/01/26 16:49:17
*/
//LIBC header for I/O -> fprintf()
#include <stdio.h>
//LIBC header for memory control and error handling -> (EXIT CODES)
#include <stdlib.h>
//LIBC header for string manipulation functionality -> memset()
#include <string.h>
//POSIX header for protocol definitions -> getaddrinfo(), struct addrinfo, gai_strerror()
#include <netdb.h>
//POSIX header for handling numerical IP addresses -> inet_ntop()
#include <arpa/inet.h>

int main(int argc, char** argv){//start of main method

  //check if an argument was passed for host
  if(argc != 2){
    fprintf(stderr,"Usage: %s <host domain>\n", argv[0]);
    return EXIT_FAILURE;
  }

  /*
   * hint: hints of the type of address or format we expect
   * res: linked list iterator
   * res0: linked list head of all found responses
  */
  struct addrinfo hint, *res, *res0;
  int error; //return code holder for getaddrinfo()

  memset(&hint, 0, sizeof(hint)); //set all memeory of hint to 0 (incase there is garbage)
  //set our hints🧐
  hint.ai_family = AF_INET; //hint we are looking for IPV4 address

  //attempt to retrieve linked list of resolutions (otherwise print error reason and exit)
  error = getaddrinfo(argv[1],NULL,&hint,&res0);
  if(error){
    fprintf(stderr,"Error: %s\n",gai_strerror(error));
    return EXIT_FAILURE;
  }

  //linked list iteration
  //Basically a for loop that as long as res != NULL,
  //print the details and set the res to the next item listed in linked list
  for(res = res0; res; res = res->ai_next){
    //variable to hold ip string
    char ip_str[INET_ADDRSTRLEN];
    //obtain the address data and cast to sockaddr_in type
    struct sockaddr_in *psockaddr = (struct sockaddr_in*) res->ai_addr;
    //obtain the string hostname (otherwise print error reason and continue to nexr entry) 
    /*
     * family format IPV4 or IPV6
     * pointer to address data bytes
     * string buffer to write to
     * size of string buffer
    */
    if(inet_ntop(psockaddr->sin_family, &(psockaddr->sin_addr), ip_str, sizeof(ip_str)) != NULL){
      //print the aquired information
      printf("Hostname: %s\n",ip_str);
    }else {
      fprintf(stderr, "Failed to convert IP to string!\n");
    }
  }

  //have to free dynamically allocated memeory😁
  freeaddrinfo(res0);

  return 0;
}//end of main method
