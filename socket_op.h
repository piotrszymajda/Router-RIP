// Autor: Piotr Szymajda - 273 023
#ifndef SOCKET_OP_H
#define SOCKET_OP_H

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>

#include <string.h>
#include <iostream>

int 	Socket (int family, int type, int protocol);
void 	Sendto(int fd, const void *buff, int buff_length, int flags, const struct sockaddr_in *saddr);  
void 	Setsockopt (int sockfd, int level, int name, const void *val, socklen_t len);  
ssize_t Recvfrom (int fd, void *ptr, int flags, struct sockaddr_in &saddr);
int 	Select (int nfds, fd_set *readfds, struct timeval *timeout);
void 	Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

void 	print_error (std::string fname, int exit_no);

#endif // SOCKET_OP_H

