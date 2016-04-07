#include "nets.h"

#include <assert.h>
#include <iostream>
#include <strings.h>
#include <climits>

nets::nets(const char* ip_adress, short mask, short dist)
    :netmask(mask)
    ,distance(dist)
{
     assert(mask >= 0 && mask <= 32);
     assert(distance >= 0 && distance <= MAX_DIST);
     
     bzero (&recp, sizeof(recp));
     recp.sin_family = AF_INET;
        
     if(inet_pton(AF_INET, ip_adress, &(recp.sin_addr)) != 1)
     {
        // error
        exit(1);
     }
     
     netadress = (1 << (mask-1)) -1;//INT_MIN;
	 //netadress >>= mask-1;
	 netadress &= recp.sin_addr.s_addr;
     
}

std::ostream& 
operator << ( std::ostream & os, const nets & net)
{
    char host_ip [20]; 
    inet_ntop(AF_INET, &(net.recp.sin_addr), host_ip, sizeof(host_ip));
    
	struct in_addr addr;
    addr.s_addr = net.netadress;
    char *net_ip = inet_ntoa(addr);
    
    if(net.distance==2)
    {
        os << net_ip << '/' << net.netmask << " distance 2 connected directly";
    }
    else if(net.distance >= MAX_DIST)
    {
        os << net_ip << '/' << net.netmask << " unreachable";
    }
    else
    {
        os << net_ip << '/' << net.netmask << " distance " << net.distance << " via " << host_ip;
    }
    
    return os;
}
