// Autor: Piotr Szymajda - 273 023
#include "socket_op.h"

#include <errno.h>

void print_error (std::string fname, int exit_no)
{
    std::cout << fname << "(...) failed. \033[1;31mError:\033[0m " << errno << " - "<< strerror (errno) << "\n";
    exit (exit_no); 
}

int Socket(int family, int type, int protocol)
{
	int	fd = socket (family, type, protocol);
	if( fd == -1 )
        print_error ("socket", -1);
    
	return fd;
}

void Sendto(int fd, const void *buff, int buff_length, int flags, const struct sockaddr_in *saddr)
{
	int sent_bytes = sendto (fd, buff, buff_length, flags, (struct sockaddr*)saddr, sizeof(*saddr));
	
	if( sent_bytes == -1 || sent_bytes != buff_length )
        print_error ("sendto", -2);
}

void Setsockopt(int sockfd, int level, int name, const void *val, socklen_t len)
{
	if( setsockopt(sockfd, level, name, val, len) == -1 )
        print_error ("setsockopt", -3);
}

ssize_t Recvfrom(int fd, void *ptr, int flags, struct sockaddr_in &saddr)
{   
	socklen_t saddr_len = sizeof(saddr);
	ssize_t	rec_bytes = recvfrom (fd, ptr, IP_MAXPACKET, flags, (struct sockaddr*)&saddr, &saddr_len);
	if( rec_bytes == -1 )
		if( errno != EAGAIN )
            print_error ("recvfrom", -4);
			
	return rec_bytes;
}

int Select (int nfds, fd_set *readfds, struct timeval *timeout)
{
	int rc = select (nfds, readfds, NULL, NULL, timeout); // w pierwszym argumencie najwyższy (numer) deskryptor+1
    if( rc < 0 )
        print_error ("select", -1);
        
    return rc;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	if( bind (sockfd, addr, addrlen) < 0 ) 
	{
        print_error ("bind", -2);
	}

}

