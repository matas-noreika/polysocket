/*
 * Purpose:
 * Port scanner program similar to nc(netcat). This is a implementation from issue #7.
 * Programmer(s): Niki Mardari, Matas Noreika
*/

// LIBC header for I/O fprintf()
#include <stdio.h>
// LIBC header for memory control and error handling
#include <stdlib.h>
// LIBC header for string manipulation functionality memset()
#include <string.h>
// POSIX header for protocol definitions for getaddrinfo(), struct addrinfo, gai_strerror()
#include <netdb.h>
// POSIX header for handling numerical IP addresses for inet_ntop()
#include <arpa/inet.h>
// POSIX header for socket definitions for struct sockaddr_in
#include <sys/socket.h>
// POSIX header for miscellaneous symbolic constants and types like NULL
#include <unistd.h>
// POSIX header for error number definitions for errno
#include <errno.h>
// POSIX header for fixed width integer types for uint16_t
#include <stdint.h>
//POSIX header for file control - fcntl()
#include <fcntl.h>
//Asynchronous file multiplexing - poll()
//needed for handling the socket when set into none blocking
#include <poll.h>
//LIBC header for time functionality - struct timeval
#include <time.h>

//https://stackoverflow.com/questions/2597608/c-socket-connection-timeout
//Function obtained from the above stackoverflow discussion (needed because sockets dont have a portable timeout option🙄)
int connect_timeout(int socketfd, const struct sockaddr *addr, socklen_t addrlen, unsigned int timeout_ms);


int main(int argc, char **argv) {//start of main method

    // check if an argument was passed for host
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <host domain>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * hint: hints of the type of address or format we expect
     * res: linked list iterator
     * res0: linked list head of all found responses
     */
    struct addrinfo hint, *res, *res0;
    int error; // return code holder for getaddrinfo()

    memset(&hint, 0, sizeof(hint));

    // set our hints:
    hint.ai_family = AF_INET;        // IPv4
    hint.ai_protocol = IPPROTO_TCP;  // TCP protocol

    // attempt to retrieve linked list of resolutions
    error = getaddrinfo(argv[1], NULL, &hint, &res0);
    if (error) {
        fprintf(stderr, "Error: %s\n", gai_strerror(error));
        return EXIT_FAILURE;
    }

    // iterate through linked list
    for (res = res0; res; res = res->ai_next) {

        char ip_str[INET_ADDRSTRLEN];

        // cast to sockaddr_in
        struct sockaddr_in *psockaddr = (struct sockaddr_in *)res->ai_addr;

        if (inet_ntop(psockaddr->sin_family,
                      &(psockaddr->sin_addr),
                      ip_str,
                      sizeof(ip_str)) == NULL) {
            fprintf(stderr, "Failed to convert IP to string!\n");
            continue;
        }

        // Cycle through ports (ports are 16 bits in value so max is 2^16=65535 (unsigned))
        for (uint16_t port = 1; port <= 65535; port++) {
            // Set port
            psockaddr->sin_port = htons(port);

            // Create socket
            int fd = socket(res->ai_family,
                            res->ai_socktype,
                            res->ai_protocol);
            if (fd == -1) continue;

            // Attempt connection (will use the timeout function)
            int rc = connect_timeout(fd, res->ai_addr, res->ai_addrlen, 2500);

            if (rc > 0) {
                printf("Connection open  %s:%u\n", ip_str, port);
            } else {
                int saved_errno = errno;
                if (saved_errno == ECONNREFUSED) {
                    printf("Connection closed %s:%u (refused)\n",
                           ip_str, port);
                } else {
                    printf("Connection failed %s:%u (errno=%d)\n",
                           ip_str, port, saved_errno);
                }
            }

            close(fd);
        }
    }

    // free dynamically allocated memory
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
