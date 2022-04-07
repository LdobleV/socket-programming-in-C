
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "commonFunctions.h"

int createSocket(const char *host, struct addrinfo **sock, char *port){
    int sockfd, rv;
    struct addrinfo hints, *p;
    
    int yes = 1;

    memset(&hints, 0, sizeof hints); //Capaz conviene hacer una funcion que reconsiga el addrinfo?
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; 
    if(host == NULL)
        hints.ai_flags = AI_PASSIVE; // use my IP

    if (rv = (getaddrinfo(host, port, &hints, sock)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = *sock; p != NULL; p = p->ai_next) { //Hacer una funcion de bind que reciba p
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
    
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
    }

    return sockfd;
}

int receive(int sd, char *buf, size_t buffSize){//Funcion para recibir datos
	return recv(sd, &buf[0], buffSize, 0);
}

