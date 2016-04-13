// Autor: Piotr Szymajda - 273 023
#ifndef NETS_H
#define NETS_H

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <ostream>

#define MAX_DIST 32
#define MAX_WAITING_TIME 5
#define TIME_TO_DELETE 5

#define PORT 8888

class nets
{
    //ip adress
    struct 	sockaddr_in recp, broadcast;
    struct	in_addr netmask_addr;
    struct	in_addr broadcast_addr;
    
    int 	netadress;
    short 	netmask;
    short 	distance;
    bool 	neighbor;
    
    int		last_recv;
    int		to_delete;

public:
    nets (const char* ip_address, short mask, short dist, bool is_neighbor = false);
    
    int 	&operator ++();
    friend std::ostream& operator<< (std::ostream & os, const nets &net);
    
    int 	check_status();
    void	send(u_int8_t * msg, int msg_length, int socket);
    void	confirm_connection();
    
    short   get_distance() { return distance; }
    int     get_network_ip() { return netadress; }
    short   get_network_mask() { return netmask; }
};

std::ostream& 
operator << (std::ostream & os, const nets & net);

#endif // NETS_H
