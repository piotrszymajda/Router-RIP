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
    struct 	sockaddr_in via; 
	struct 	sockaddr_in broadcast;
    struct	in_addr broadcast_addr;
    
    int 	netadress;
    short 	netmask;
    short 	distance;
    bool 	neighbor;
    
    int		last_recv;
    int		to_delete;

public:
    nets (const char* ip_address, short mask, short dist, bool is_neighbor = false);
    nets (u_int32_t network_address, const struct sockaddr_in & sender, short mask, short dist, bool is_neighbor = false);
 
    int 	&operator ++();
    bool 	operator == (const struct sockaddr_in &);
    bool 	operator == (const int & netaddr);
    friend std::ostream& operator<< (std::ostream & os, const nets &net);
    
    int 	check_status();
    int	send(u_int8_t * msg, int msg_length, int socket);
    void	confirm_connection();
    bool	same_network(const struct sockaddr_in &);
    
    short	get_distance() { return distance; }
    int		get_network_ip() { return netadress; }
    short	get_network_mask() { return netmask; }
    struct 	sockaddr_in get_via_ip() { return via; } 
	
    void	set_distance(short dst) { distance = dst; }
	void 	set_neighbor(bool is) { neighbor = is; }
    void	set_via(const struct sockaddr_in sender) { via = sender; }
	
    bool	is_neighbor() { return neighbor; }
};

std::ostream& 
operator << (std::ostream & os, const nets & net);

#endif // NETS_H
