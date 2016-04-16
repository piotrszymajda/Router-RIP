// Autor: Piotr Szymajda - 273 023
#include "nets.h"

#include <assert.h>
#include <iostream>
#include <strings.h>
#include <climits>

#include "socket_op.h"

nets::nets (const char* ip_address, short mask, short dist, bool is_neighbor)
	:netmask{ mask }
	,distance{ dist }
	,neighbor{ is_neighbor }
	,last_recv{ 0 }
	,to_delete{ -1 }
{
	assert (mask >= 0 && mask <= 32);
	assert (distance >= 0 && distance <= MAX_DIST);
	 
	bzero (&via, sizeof(via));
	via.sin_family 	= AF_INET;
	via.sin_port		= htons(PORT);
		
	if( inet_pton(AF_INET, ip_address, &(via.sin_addr)) != 1 )
	{
		std::cout << "\033[1;31mError: \033[0m" << ip_address<< " is NOT IP address\n";
		exit(1);
	}

	netadress = (1 << (mask))-1;
	
	int bc = via.sin_addr.s_addr | (~ netadress);
	netadress &= via.sin_addr.s_addr;
	
	broadcast_addr.s_addr = bc;
	
	bzero (&broadcast, sizeof(broadcast));
	broadcast.sin_family 	= AF_INET;
	broadcast.sin_port		= htons(PORT);
	broadcast.sin_addr		= broadcast_addr;
	
	
	assert(to_delete == -1);
}


nets::nets (u_int32_t network_address, const struct sockaddr_in & sender, short mask, short dist, bool is_neighbor)
	:via{ sender }
	,netadress{ (int)network_address }
	,netmask{ mask }
	,distance{ dist }
	,neighbor{ is_neighbor }
	,last_recv{ 0 }
	,to_delete{ -1 }
{
	
	int bc = via.sin_addr.s_addr | (~( (1 << (mask))-1 ));
	
	broadcast_addr.s_addr 	= bc;
	
	bzero (&broadcast, sizeof(broadcast));
	broadcast.sin_family 	= AF_INET;
	broadcast.sin_port		= htons(PORT);
	broadcast.sin_addr		= broadcast_addr;
}

int & nets::operator ++()
{
	++last_recv;
	if( to_delete > 0 ) 
	{ 
		--to_delete; 
	}
		
#ifdef DEBUG 
	char host_ip [20]; 
	inet_ntop (AF_INET, &(via.sin_addr), host_ip, sizeof(host_ip));
	std::cout << "To delete: " << host_ip << " " << to_delete << " lr: " << last_recv << '\n';
#endif

	return last_recv;
}

bool nets::operator == (const struct sockaddr_in & sender)
{
	if( via.sin_addr.s_addr == sender.sin_addr.s_addr )
		return true;
	return false;
}

bool nets::operator == (const int & netaddr)
{
	if( netadress == netaddr )
		return true;
	return false;
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
	    inet_ntop (AF_INET, &(net.via.sin_addr), host_ip, sizeof(host_ip));
	
		os << net_ip << '/' << net.netmask << " distance " << net.distance << " via " << host_ip;
	}
	
	return os;
}

int nets::check_status()
{	
	if( to_delete < 0 )
	{
		if( distance > MAX_DIST )
		{
			to_delete = TIME_TO_DELETE;
			return 0;
		}
		if( last_recv > MAX_WAITING_TIME)
		{
			distance = MAX_DIST + 1;
			to_delete = TIME_TO_DELETE;
			return 0;
		}
	}
	if( to_delete == 0 )
	{
		return -1;
	}
	
	return 1;
}

void nets::send (u_int8_t * msg, int msg_length, int socket)
{	
	#ifdef DEBUG 
		std::cout << "Sending ";
		for(int i=0; i<msg_length; ++i)
			std::cout << (int)msg[i] << ' '; 
		std::cout << '\n';
	#endif 
	
	Sendto(socket, msg, msg_length, 0, &broadcast);
}

void nets::confirm_connection()
{
	last_recv = 0;
	if( distance <= MAX_DIST )
		to_delete = -1;
}

bool nets::same_network (const struct sockaddr_in & sender)
{
	#ifdef DEBUG 
		struct in_addr addr;
		addr.s_addr = netadress;
		
		char *net_ip = inet_ntoa (addr);
		std::cout << net_ip << ' ';
		
		char *sender_ip = inet_ntoa (sender.sin_addr);
		std::cout << sender_ip << ' ';
		
		char *bc_ip = inet_ntoa (broadcast_addr);
		std::cout << bc_ip << '\n';
	#endif 
	
	if( (unsigned int) ntohl(netadress) > ntohl(sender.sin_addr.s_addr) )
		return false;
	if( ntohl(sender.sin_addr.s_addr) > ntohl(broadcast_addr.s_addr) )
		return false;
	
	return true;
}
