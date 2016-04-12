// Autor: Piotr Szymajda - 273 023
#include "nets.h"

#include <assert.h>
#include <iostream>
#include <strings.h>
#include <climits>

#include "socket_op.h"

nets::nets (const char* ip_address, short mask, short dist, bool is_neighbor)
    :netmask(mask)
    ,distance(dist)
    ,neighbor(is_neighbor)
    ,last_recv(0)
    ,to_delete(-1)
{
     assert (mask >= 0 && mask <= 32);
     assert (distance >= 0 && distance <= MAX_DIST);
     
     bzero (&recp, sizeof(recp));
     recp.sin_family 	= AF_INET;
     recp.sin_port		= htons(PORT);
        
     if( inet_pton(AF_INET, ip_address, &(recp.sin_addr)) != 1 )
     {
		std::cout << "\033[1;31mError: \033[0m" << ip_address<< " is NOT IP address\n";
        exit(1);
     }
     
     netadress = (1 << (mask-1)) -1;//INT_MIN;
	 //netadress >>= mask-1;
	 netadress &= recp.sin_addr.s_addr;
     
}


int & nets::operator ++()
{
	++last_recv;
	if( to_delete > 0 ) 
	{ 
		--to_delete; 
	}
	return last_recv;
}

std::ostream& 
operator << (std::ostream & os, const nets & net)
{
    
	struct in_addr addr;
    addr.s_addr = net.netadress;
    char *net_ip = inet_ntoa (addr);
    
    if( net.distance >= MAX_DIST )
    {
        os << net_ip << '/' << net.netmask << " unreachable";
    }
    else if( net.neighbor )
    {
        os << net_ip << '/' << net.netmask << " distance "<< net.distance <<" connected directly";
    }
    else
    {
		char host_ip [20]; 
		inet_ntop (AF_INET, &(net.recp.sin_addr), host_ip, sizeof(host_ip));
    
        os << net_ip << '/' << net.netmask << " distance " << net.distance << " via " << host_ip;
    }
    
    return os;
}


int nets::check_status()
{
	if( neighbor )
	{
		return 2;
	}
	
	if( to_delete == 0 )
	{
		return -1;
	}
	
	if( last_recv > MAX_WAITING_TIME)
	{
		distance = MAX_DIST + 1;
		to_delete = TIME_TO_DELETE;
		return 0;
	}
	
	return 1;
}

void nets::send(u_int8_t * msg, int socket)
{
	Sendto(socket, msg, strlen((char*)msg), 0, &recp);	
}
