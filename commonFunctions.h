
#ifndef __COMMONFUNCTIONS_H__
#define __COMMONFUNCTIONS_H__


#include <stddef.h>


/*
 * Crea un socket y devuelve el descriptor de socket
 */
int createSocket(const char *host, struct addrinfo **sock, char *port);

/**
 * Utiliza recv() para enviar un mensaje al cliente/servidor
 */
int receive(int sd, char *buf, size_t buffSize);


#endif /* __COMMONFUNCTIONS_H__*/