/*
 * Purpose:
 * Example program that creates a utility program to resolve domain name string to server address (Only TCP socket type).
 * Similar shell/bash command: host <domain name>
 * The program will take in first argument and translate it (if possible) to an IPV4 address.
 * For greater detail into the functionality refer to the man pages for getaddrinfo(). I.E. 'man getaddrinfo'
 * Programmer: Matas Noreika 14/01/26 16:49:17
*/
//LIBC header for I/O -> fprintf()
#include <stdio.h>
//LIBC header for memory control and error handling -> (EXIT CODES)
#include <stdlib.h>
//POSIX OS API - close()
#include <unistd.h>
//POSIX header for file control - fcntl()
#include <fcntl.h>
//POSIX header for DNS -> getaddrinfo(), struct addrinfo, gai_strerror()
#include <netdb.h>
//POSIX header for handling numerical IP addresses -> inet_ntop()
#include <arpa/inet.h>
//Asynchronous file multiplexing - poll()
//needed for handling the socket when set into none blocking
#include <poll.h>
//LIBC header for time functionality - struct timeval
#include <time.h>
//posix error handlling header - errno global variable
#include <errno.h>

//https://stackoverflow.com/questions/2597608/c-socket-connection-timeout
//Function obtained from the above stackoverflow discussion (needed because sockets dont have a portable timeout option🙄)
int connect_timeout(int socketfd, const struct sockaddr *addr, socklen_t addrlen, unsigned int timeout_ms);

int main(int argc, char** argv){//start of main method

  //check if an argument was passed for host
  if(argc != 3){
    fprintf(stderr,"Usage: %s <host domain> <port>\n", argv[0]);
    fprintf(stderr,"Example: %s google.com http\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct addrinfo hint = {0}, *res, *res0;
  int error; //return code holder for getaddrinfo()
  //variable to hold ip string
  //Moved definition out of local scope to loop to prevent unecessary 
  char ip_str[INET_ADDRSTRLEN];

  //obtain a socket file descriptor
  /*
   * specify that its a IPV4 connection
   * specify the connection type as stream (TCP)
   * additional protocol value for TCP/IP its 0
  */
  //set our hints🧐
  hint.ai_family = AF_INET; //hint we are looking for IPV4 address
  hint.ai_protocol = IPPROTO_TCP; //hint we are looking for TCP socket connection

  //attempt to retrieve linked list of resolutions (otherwise print error reason and exit)
  error = getaddrinfo(argv[1],argv[2],&hint,&res0);
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
    //obtain a socket file descriptor
    /*
    * specify that its a IPV4 connection
    * specify the connection type as stream (TCP)
    * additional protocol value for TCP/IP its 0
    */
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(!socketfd){
      fprintf(stderr,"Error: Failed to create socket\n");
      continue;
    }
    //if the socket was able to connect on port (we can print the address)
    if(connect_timeout(socketfd, res->ai_addr, res->ai_addrlen, 2500) > 0){
      printf("Connected\n");
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
        printf("Hostname: %s:%d\n",ip_str,ntohs(psockaddr->sin_port));
      }else {
        fprintf(stderr, "Failed to convert IP to string!\n");
      }
    }else{
      //fprintf(stderr,"Error: connect failed\n");
      //continue;
    }
    close(socketfd);
  }

  //have to free dynamically allocated memeory😁
  freeaddrinfo(res0);

  return 0;
}//end of main method

//https://stackoverflow.com/questions/2597608/c-socket-connection-timeout
//Function obtained from the above stackoverflow discussion (needed because sockets dont have a portable timeout option🙄)
int connect_timeout(int socketfd, const struct sockaddr *addr, socklen_t addrlen, unsigned int timeout_ms){
  //return code variable
  int rc = 0;
  //set socket in non-blocking mode (Asynchronous)
  int flags_before;
  //retrieve current operation flags
  if( (flags_before = fcntl(socketfd, F_GETFL,0) < 0) ){return -1;}
  //set the non-blocking flag property
  if(fcntl(socketfd,F_SETFL, flags_before | O_NONBLOCK) <0 ){return -1;}
  //start connecting Asynchronously
  //wrapped in loop to allow reduce the need of repeating restoring of socket blocking flag
  do{
    if(connect(socketfd, addr, addrlen)<0){
      //check reason for connect error not to be Asynchronous operation reasons
      if((errno != EWOULDBLOCK) && (errno != EINPROGRESS) ){
        rc = -1;
      }else{ //wait for operation to complete
        //set our current timestamp (used to compare later because poll can be interrupted)
        struct timespec now;
        //get the current time fails we set the return code and dont continue rest of loop
        if(clock_gettime(CLOCK_MONOTONIC, &now) <0 ){rc=-1; break;}
        //set our deadline time
        struct timespec deadline = { .tv_sec = now.tv_sec, .tv_nsec = now.tv_nsec + (timeout_ms*1000000l) };
        do {
          //calculate if we reached deadline
          if(clock_gettime(CLOCK_MONOTONIC, &now) <0 ){rc=-1; break;}
          int time_to_delay_ms = (int)((deadline.tv_sec - now.tv_sec)*1000l + (deadline.tv_nsec - now.tv_nsec)/1000000l);
          if(time_to_delay_ms < 0){rc = 0; break;}
          //set the poll parameters (POLLOUT - file is ready to read/write to in non-blocking mode)
          struct pollfd pfds[] = {{.fd = socketfd, .events = POLLOUT}};
          //poll returns number of fds ready, 0 if timedout or negative error code
          rc = poll(pfds, 1, time_to_delay_ms);
          //check if poll succeeded, we check for proper success
          if(rc>0){
            int error = 0; socklen_t len = sizeof(error);
            int retval = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &error, &len);
            //if reading sock errors doesn't return an error
            if(retval == 0){errno = error;}
            //read the new error (or old either case the operation failed)
            if(errno!=0){rc=-1;}
          }
        }while(rc==-1 && errno==EINTR); //continue looping while the operations fails or interrupted
        //did we timeout?
        if(rc==0){
          errno = ETIMEDOUT;
          rc=-1;
        }
      }
    }
  }while(0);

  //restore original settings
  if(fcntl(socketfd, F_SETFL, flags_before) < 0 ){ return -1;}
  //success otherwise
  return rc;
}
