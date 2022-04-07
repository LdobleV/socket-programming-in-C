#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "commonFunctions.h"

//#define PORT "21" //puerto FTP

int codeToInt(char *str){
    char code[4];
    strncpy(code, str, 3);

    return (code[0] - '0') * 100 + (code[1] - '0') * 10 + (code[2] - '0');
}

#define MAXDATASIZE 100 // max number of bytes we can get at once 


int main(int argc, char *argv[]) {
	int sockfd, numbytes, ext = 1;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
    char *ip_address = argv[1], *port = argv[2]; // puerto e ip ingresables
    size_t bytesRec;

	if (argc != 3) {
	    fprintf(stderr,"error: wrong number of inputs\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip_address, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) { 
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) { 
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	fprintf(stdout,"\n%s\n", buf);


	while(ext){ //Una vez conectado, se entrae en el while de recibir y enviar
		fgets(buf, MAXDATASIZE, stdin); 
		if(send(sockfd, buf, strlen(buf), 0) == -1)
			perror("send");

		bytesRec = receive(sockfd, buf, MAXDATASIZE);
        buf[(int)bytesRec] = '\0';

		fprintf(stdout, "%s\n", buf);

        if(codeToInt(buf) == 221)
            ext = 0;
	}

	close(sockfd);

	return 0;
}