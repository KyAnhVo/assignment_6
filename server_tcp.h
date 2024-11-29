#ifndef SERVER_TCP_H
#define SERVER_TCP_H

/**
 * Used to multithread for TCP side
 * @param argv: {ushort *} port number address
 */
void *serverTCP(void*);

/**
 * Used to handle each tcp accepted communication in a seperate thread
 * @param argv: file descriptor of communicating socket
 */
void *serverTcpConnection(void*);

#endif