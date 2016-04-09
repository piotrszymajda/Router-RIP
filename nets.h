// Autor: Piotr Szymajda - 273 023
#ifndef NETS_H
#define NETS_H

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <ostream>

#define MAX_DIST 32
#define MAX_WAITING_TIME 5
#define TIME_TO_DELETE 5

class nets
{
    //ip adress
    struct 	sockaddr_in recp;
    
    int 	netadress;
    short 	netmask;
    short 	distance;
    bool 	neighbor;
    int		last_recv;
    int		to_delete;

public:
    nets (const char* ip_address, short mask, short dist, bool is_neighbor = false);
    
    int &operator ++();
    friend std::ostream& operator<< (std::ostream & os, const nets &net);
    
    int check_status();
};

std::ostream& 
operator << (std::ostream & os, const nets & net);

#endif // NETS_H
