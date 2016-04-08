// Autor: Piotr Szymajda - 273 023
#ifndef NETS_H
#define NETS_H

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <ostream>

#define MAX_DIST 16

class nets
{
//protected:
    //ip adress
    struct 	sockaddr_in recp;
    
    int 	netadress;
    short 	netmask;
    short 	distance;
    bool 	neighbor;

public:
    nets (const char* ip_adress, short mask, short dist, bool is_neighbor = false);
    friend std::ostream& operator<< (std::ostream & os, const nets &net);
};

std::ostream& 
operator << (std::ostream & os, const nets & net);

#endif // NETS_H
