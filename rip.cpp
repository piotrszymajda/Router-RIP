// Autor: Piotr Szymajda - 273 023
#include "rip.h"

#include <iostream>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include "socket_op.h"

#define WAIT_TIME 30000000LL // 30 sec in microsec

#define microsec_to_sec 1000000LL
#define microsec_to_milisec 1000LL

using namespace std;

void prepare_rib_msg(vector<nets*>& rib, u_int8_t* msg)
{
	msg[0]='A';
	msg[1]='B';
	msg[2]='C';
}

void rip (vector<nets*>& neighbor)
{    
	// Schemat:	
	// 1. skonstruj dane z połączenia obu wektorów
	// 2. wyślij dane w pakietach do wszystkich sąsaiadów ( sieci, po broadcast'cie), zgodnie z spec.txt
	// 3. czekaj na jakieś info przez TIMEOUT czasu, ciągłymi select'ami - DONE
	//    3a. jeśli coś przyjdzie sprawdzić poprawność i sprawdzić czy nie ma tego rib'ie
	//    3b. jeśli jest to zaktualizowć, wpp dodać do rib'a
	//	4. wypisz info - DONE
	
	// Uwaga dodatkowa neighbor - do użycia przy wysyłaniu, rib - jako tablica routingu, tzn sąsiedzi też do rib'a
	int sockfd = Socket (AF_INET, SOCK_DGRAM, 0);
	int broadcast_perm = 1;
    setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&broadcast_perm, sizeof(broadcast_perm));
  

	fd_set read_fd;
	FD_ZERO (&read_fd);
	FD_SET (sockfd, &read_fd);
	
	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons (PORT);
	server_address.sin_addr.s_addr = htonl (INADDR_ANY);
	
	Bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
	
	struct timeval wait_time;
	
	vector<nets*> rib; //routing information base
    
	for( auto&& i: neighbor )
		rib.push_back (new nets(*i));
    
	u_int8_t rib_msg[IP_MAXPACKET];
	
    struct sockaddr_in 	sender;	
    socklen_t 			sender_len = sizeof(sender);
    u_int8_t 			buffer[IP_MAXPACKET+1];
    
	while( true )
	{
		bzero (&rib_msg, sizeof(rib_msg));
		prepare_rib_msg(rib, rib_msg);
		for( auto&& i: neighbor )
			i->send (rib_msg, sockfd);
	
		// increment the value of the variable containing the last round in which we received a reply
		for( auto&& i: rib )
			++(*i);
		
		
		// Set wait time
		wait_time.tv_sec = WAIT_TIME / microsec_to_sec;
		wait_time.tv_usec = WAIT_TIME % microsec_to_sec;
		
		while( wait_time.tv_usec != 0 || wait_time.tv_sec != 0 )
		{
			cout << "Waiting...\n";
			int rc = Select (sockfd+1, &read_fd, &wait_time); // w pierwszym argumencie najwyższy (numer) deskryptor+1
		
			if( rc == 0 )
			{
	        	break;
	    	}
		
			cout << "Reciving...\n";
			
            bzero (&buffer, sizeof(buffer));
			// Recivefrom ...
            ssize_t rec_bytes = Recvfrom(sockfd, buffer, 0, sender);
            cout << buffer << '\n';
			// Sprawdż poprawność
			// Sprawdź czy to coś nowego czy aktualizacja
			// Sprawdzić czy od kogoś nic nie otrzymaliśmy
		}
		
		for( auto&& i: rib )
			if( i->check_status() < 0 )
				delete i;
		
		
		cout << "Routing table:\n";
		
		for( auto&& i: rib )
			cout << *i << '\n';

		cout << '\n';		

	}
	

	for( auto&& i: rib )
		delete i;
		
	close (sockfd);
}
