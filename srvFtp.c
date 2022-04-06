#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h> 


//#define PORT "21" // puerto FTP

#define BACKLOG 10 

//mensajes de respuesta
#define A220 "220 successful connection"
#define A221 "221 bye bye!"
#define A230 "230 login correct"
#define A530 "530 login incorrect"
#define A331 "331 the user requires password:"
#define MAXDATASIZE 50

int main(int argc, char *argv[]){
    const char s[] = " ";
    int sockfd, new_fd, ext = 1; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char *token, *buf, *user, *port = argv[1];
    int rv;

    if (argc != 2) {
	    fprintf(stderr,"error: wrong number of inputs\n");
	    exit(1);
	}

    user = malloc(sizeof(char) * MAXDATASIZE);
    buf = malloc(sizeof(char) * MAXDATASIZE);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    hints.ai_protocol = IPPROTO_TCP; 

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
        continue;
    }

    break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Server: waiting for connections...\n");
    while(1){ // main accept() loop

        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        if(send(new_fd, A220, strlen(A220), 0) == -1)
                perror("send");
        
        while(ext){

            if(recv(new_fd, buf, MAXDATASIZE, 0) == -1)
                perror("recv");

            strcpy(user, buf);
            
            token = strtok(user, s);

            if(strcmp(token, "USER") == 0){
                if(send(new_fd, A331, strlen(A331), 0) == -1)
                    perror("send");
            }

            token = strtok(NULL, s);

            user = token;
            user[strlen(user) - 1] = '\0';
            

            if(recv(new_fd, buf, MAXDATASIZE, 0) == -1)
                perror("recv");


            token = strtok(buf, s);

            if(strcmp(token, "PASS") == 0){
                token = strtok(NULL, s);
                token[strlen(token) - 2] = '\0';
                if(strcmp(token, "1234") == 0 && strcmp(user, "lucas") == 0){
                    if(send(new_fd, A230, strlen(A230), 0) == -1)
                        perror("send");
                }else{
                    if(send(new_fd, A530, strlen(A530), 0) == -1){
                        perror("send");
                    }
                }
            }
        }
        close(new_fd);
    }

    close(sockfd);
}