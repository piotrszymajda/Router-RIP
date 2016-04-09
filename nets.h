// Autor: Piotr Szymajda - 273 023
#ifndef NETS_H
#define NETS_H

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <ostream>

#define MAX_DIST 32

class nets
{
//protected:
    //ip adress
    struct 	sockaddr_in recp;
    
    int 	netadress;
    short 	netmask;
    short 	distance;
    bool 	neighbor;
    int		last_recv;

public:
    nets (const char* ip_address, short mask, short dist, bool is_neighbor = false);
    
    int &operator ++();
    friend std::ostream& operator<< (std::ostream & os, const nets &net);
};

std::ostream& 
operator << (std::ostream & os, const nets & net);

#endif // NETS_H
