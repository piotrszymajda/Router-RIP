// Autor: Piotr Szymajda - 273 023
#include "rip.h"

#include <iostream>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include "socket_op.h"

#define WAIT_TIME 15000000LL // 30 sec in microsec

#define microsec_to_sec 1000000LL
#define microsec_to_milisec 1000LL

using namespace std;

void int32_to_char(u_int32_t x, u_int8_t* tab, int offset)
{
	tab[offset+3] = (x>>24) & 0xFF;
	tab[offset+2] = (x>>16) & 0xFF;
	tab[offset+1] = (x>>8) & 0xFF;
	tab[offset] = x & 0xFF;
}

u_int32_t char_to_int32(u_int8_t* tab, int offset)
{
	return (tab[offset+3] << 24) | (tab[offset+2] << 16) | (tab[offset+1] << 8) | tab[offset];
}

#define BYTE_BY_RECORD 6
#define MSG_WHEN_RIB_IS_EMPTY "I feel so lonely"
int prepare_rib_msg(vector<nets*>& rib, u_int8_t* msg)
{	
	int size = rib.size();
	assert( size < 10000 );
	if( size == 0 )
	{
		string lonely = MSG_WHEN_RIB_IS_EMPTY;
		int length = lonely.copy((char*)msg,16);
		return length;
	}

	int32_to_char (size, msg, 0);
	
	int cs = 0, i, ip, mask, dst;
	for( i=0; i<size; ++i )
	{
		ip = rib[i]->get_network_ip();
		mask = rib[i]->get_network_mask();
		dst = rib[i]->get_distance();

		assert( dst <= 255 );

		int32_to_char (ip, msg, BYTE_BY_RECORD*(i+1)-2);
		msg[8+i*6] = mask;
		msg[9+i*6] = dst;
	}
	
	for( int j=0; j<BYTE_BY_RECORD*(i+1)-2; ++j )
	{
		cs += (int)msg[j];
	}
	
	int32_to_char (cs, msg, BYTE_BY_RECORD*(i+1)-2);
	
#ifdef DEBUG 
	cout << "Msg: ";
	for( int j=0; j<BYTE_BY_RECORD*(i+2)-4; ++j )
		cout << msg[j];
	cout << '\n';
#endif

	return BYTE_BY_RECORD*(i+2)-4;
}

int netaddress(int ip, short netmask )
{
	int	netaddr = (1 << (netmask)) -1;
	
	return netaddr & ip;
}

void rip (vector<nets*>& interfaces)
{	
	int sockfd = Socket (AF_INET, SOCK_DGRAM, 0);
	int broadcast_perm = 1;
	setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&broadcast_perm, sizeof(broadcast_perm));
  
	fd_set read_fd;
	
	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family		= AF_INET;
	server_address.sin_port			= htons (PORT);
	server_address.sin_addr.s_addr	= htonl (INADDR_ANY);
	
	Bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
	
	struct timeval		wait_time;
	
	vector<nets*>		rib; //routing information base
	
	for( auto&& i: interfaces )
		rib.push_back (new nets(*i));

	u_int8_t			rib_msg[IP_MAXPACKET];
	
	struct sockaddr_in	sender;	
	u_int8_t			buffer[IP_MAXPACKET+1];
	
	while( true )
	{
		bzero (&rib_msg, sizeof(rib_msg));
		int msg_length = prepare_rib_msg(rib, rib_msg);
		for( auto&& i: interfaces )
		{
			if( i->send (rib_msg, msg_length, sockfd) != 0 )
			{
				//cout << "Send error to " << (*i) << "\n"; 
				//found in rib and set inf distance
				for( auto&& j: rib )
				{
					if( j->is_neighbor() && i->same_network( j->get_via_ip() ) )
					{
						//cout << "Find " << (*j) << "\n";
						j->set_distance( MAX_DIST + 1 );
					}
				}
			}
		}
		
		// increment the value of the variable containing the last round in which we received a reply
		for( auto&& i: rib )
			++(*i);
		
		// Set wait time
		wait_time.tv_sec = WAIT_TIME / microsec_to_sec;
		wait_time.tv_usec = WAIT_TIME % microsec_to_sec;
		
		FD_ZERO (&read_fd);
		FD_SET (sockfd, &read_fd);
	
		while( wait_time.tv_usec != 0 || wait_time.tv_sec != 0 )
		{
		
			#ifdef DEBUG 
				cout << "Waiting...\n";
			#endif
		
			int rc = Select (sockfd+1, &read_fd, &wait_time); // w pierwszym argumencie najwyższy (numer) deskryptor+1
		
			if( rc == 0 )
			{
				break;
			}
		
			#ifdef DEBUG 
				cout << "Reciving...\n";
			#endif
			
			bzero (&buffer, sizeof(buffer));
			// Recivefrom ...
			ssize_t rec_bytes = Recvfrom(sockfd, buffer, 0, sender); //MSG_DONTWAIT
			
			bool my_packet = false;
			for( auto&& i: interfaces )
			{
				if( (*i) == sender )
				{
					 my_packet = true;// it's mean that it's packet from us
				}
			}
			
			if( my_packet )
			{
				#ifdef DEBUG 
					cout << "I recived my own packet\n";
				#endif
				continue;
			}
			
			#ifdef DEBUG 
				cout << "Recived msg: ";
				for( int j=0; j<rec_bytes; ++j )
					cout << (int)buffer[j] << ' ';
				cout << '\n';
			#endif	
			
			// check checksum
			int cs = 0;
			for( int j=0; j< rec_bytes-4; ++j )
			{
				cs += buffer[j];
			}
			
			int cs_from_msg = char_to_int32(buffer, rec_bytes-4);
			if( cs != cs_from_msg )
			{
				#ifdef DEBUG 
					cout << "Wrong checksum: " << cs << " <> " << cs_from_msg << '\n';
				#endif
				continue;
			}
			
			bool nothing_intresting = false;
			
			if( strcmp((char*)buffer, MSG_WHEN_RIB_IS_EMPTY) == 0 )
			{
				#ifdef DEBUG
					char sender_ip [20]; 
					inet_ntop (AF_INET, &(sender.sin_addr), sender_ip, sizeof(sender_ip));
					
					cout << sender_ip << " feel lonely\n";
				#endif
				nothing_intresting = true;
			}

			bool known_interface = false;
			int distance_to_network = 0;
			nets * from_interface;
			for( auto&& i: interfaces ) // znajdź dystans przez ten interfejs
			{
				if( i->same_network( sender ) )
				{
					assert( !known_interface );
					distance_to_network = i->get_distance();
					from_interface = i;
					known_interface = true;
				}
			}
			if( !known_interface )
			{
				cout << "What? How could we get packet like this?\n";
				assert( false );
			}
			
			if( !nothing_intresting )
			{
				// parse message:
				int size = char_to_int32(buffer, 0);
				for( int i = 0; i<size; ++i )
				{
					int network_addr	= char_to_int32(buffer, 4+6*i);
					short mask 			= buffer[8+6*i];
					short dst 			= buffer[9+6*i];
					
					#ifdef DEBUG
						struct in_addr addr;
						addr.s_addr = network_addr;
						char *net_ip = inet_ntoa (addr);
						
						cout << net_ip << '/' << mask << " dst: " << dst << '\n';
					#endif 
				
					bool known = false;
					for( auto&& i: rib )
					{
						if( (*i) == network_addr )
						{	
							assert( !known );

							if ( (*i) == sender || i->same_network( sender ) )
							{
								int tmp_dst = dst;
								if( ! i->same_network( sender ) )
									tmp_dst += distance_to_network;
								
								#ifdef DEBUG
									if( i->get_distance() != tmp_dst )
										cout << "Change " << i->get_distance() << " to " << tmp_dst << '\n';
								#endif
								
								i->set_distance( tmp_dst );
								i->confirm_connection();
							}
							else
							{
								if( i->get_distance() > distance_to_network + dst)
								{
									i->set_distance( distance_to_network + dst );
									i->set_via( sender );
									if( from_interface->same_network( sender ) )
									{
										i->set_neighbor( false );
									}
									else
									{
										i->set_neighbor( true );
									}
									i->confirm_connection();
								}
								
							}
							known = true;
						}
					}
					if( !known )
					{
						if( dst > MAX_DIST ) //if this is unreachable net
							continue;
							
						// check if this is old (deleted from rib) interface
						for( auto&& i: interfaces )
						{
							if( (*i) == network_addr )
							{
								assert( !known ); 
								known = true;
								rib.push_back( new nets(*i) );
							}
						}
						if( !known )
						{
							rib.push_back( new nets(network_addr, sender, mask, dst + distance_to_network) );
						}
					}
		
				}
				// Check if we have sender in RIB
				bool have = false;
				for( auto&& i: rib )
				{
					if( i->is_neighbor() && ((*i) == sender || i->same_network( sender )) )
					{	
						assert( !have );
						i->confirm_connection();
						have = true;
					}
				}
				if( !have )
				{
					rib.push_back( new nets(*from_interface) );
				}
			}
			else
			{
				// someone feel lonely, it must be my neighbor
				// check that I have him in rib
				// if yes -> confirm_connection()
				// if not -> add to rib
				bool known = false;
				for( auto&& i: rib )
				{
					if( i->is_neighbor() && (*i) == sender )
					{	
						assert( !known );
						i->confirm_connection();
						known = true;
					}
				}
				if( !known )
				{
					rib.push_back( new nets(*from_interface) );
				}
			}
		}
		
		for( auto iter = rib.begin(); iter != rib.end(); )
		{
			if( (*iter)->check_status() < 0 )
			{
				delete * iter;
				iter = rib.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		
		cout << "\033[1mRouting table: \033[0m\n";
		for( auto&& i: rib )
			cout << *i << '\n';

		cout << '\n';		
	}

	for( auto&& i: rib )
		delete i;
	rib.clear();
	
	close (sockfd);
}
