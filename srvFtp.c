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
#include "commonFunctions.h"

//#define PORT "21" // puerto FTP

#define BACKLOG 10 

//mensajes de respuesta
#define A220 "220 successful connection"
#define A221 "221 bye bye!"
#define A230 "230 login correct"
#define A530 "530 login incorrect"
#define A331 "331 the specified user requieres password"
#define A503 "503 the command is not implemented"
#define MAXDATASIZE 50

int parser(char *command){ //Aca con el comando del cliente extraemos el codigo de 4 letras para ver que comando es
    char code[5];
    strncpy(code, command, 4);
    code[4] = '\0';
    if(strcmp(code, "QUIT") == 0){
        return 0;//Terminar la conexion
    }
    if(strcmp(code, "USER") == 0){
        return 1;//Enviar nombre de usuariol
    }
    if(strcmp(code, "PASS") == 0){
        return 2;//Enviar contraseña
    }
    return -1;//No reconocido
}


int main(int argc, char *argv[]){
    const char s[] = " ";
    int loginStatus, sockfd, new_fd, ext = 1, commandCode; // listen on sock_fd, new connection on new_fd
    struct addrinfo *comSocket;
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

    sockfd = createSocket(NULL, &comSocket, port);
    /*memset(&hints, 0, sizeof hints); //Capaz conviene hacer una funcion que reconsiga el addrinfo?
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    hints.ai_protocol = IPPROTO_TCP; 

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) { //Hacer una funcion de bind que reciba p
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }*/

    if (bind(sockfd, comSocket->ai_addr, comSocket->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
    }
    if (comSocket == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    freeaddrinfo(comSocket);

    printf("Server: waiting for connections...\n");
    while(1){ // Loop principal para aceptar conexiones

        sin_size = sizeof their_addr;
        if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
            perror("accept");
            continue;
        }

        if(send(new_fd, A220, strlen(A220), 0) == -1)
                perror("send");
        loginStatus = 0;
        while(ext){ // Al establecer la conexion se entra en el while del cliente (Rehacer con el uso de un parser)

            receive(new_fd, buf, MAXDATASIZE);

            commandCode = parser(buf);
            
            switch (commandCode) //Despues de conseguir el codigo del comando, el switch ejecuta la accion acorde
            {
                case 0:
                    if(send(new_fd, A221, strlen(A221), 0) == -1)
                        perror("send");
                    ext = 0;
                break;

                case 1:
                    strcpy(user, &buf[5]);
                    user[strlen(user) - 1] = '\0';
                    if(send(new_fd, A331, strlen(A331), 0) == -1)
                        perror("send");
                break;
                
                case 2:
                    token = strtok(buf, s);
                    token = strtok(NULL, s);
                    token[strlen(token) - 2] = '\0';
                    if(strcmp(token, "1234") == 0 && strcmp(user, "lucas") == 0){
                        loginStatus = 1;
                        if(send(new_fd, A230, strlen(A230), 0) == -1)
                            perror("send");
                    }else{
                        if(send(new_fd, A530, strlen(A530), 0) == -1)
                            perror("send");
                        }
                break;

                default:
                    fflush(stdout);
                    if(send(new_fd, A503, strlen(A503), 0) == -1)
                        perror("send");
                    continue;
            }
        }
        close(new_fd);
    }

    close(sockfd);
}

