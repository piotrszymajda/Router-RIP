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

void rip (vector<nets*>& neighbor, vector<nets*>& rib)
{
	// Schemat:	
	// 1. skonstruj dane z połączenia obu wektorów
	// 2. wyślij dane w pakietach do wszystkich sąsaiadów ( sieci, po broadcast'cie), zgodnie z spec.txt
	// 3. czekaj na jakieś info przez TIMEOUT czasu, ciągłymi select'ami - DONE
	//    3a. jeśli coś przyjdzie sprawdzić poprawność i sprawdzić czy nie ma tego rib'ie
	//    3b. jeśli jest to zaktualizowć, wpp dodać do rib'a
	//	4. wypisz info - DONE
	
    int sockfd = Socket (AF_INET, SOCK_DGRAM, 0);
	
	fd_set read_fd;
	FD_ZERO (&read_fd);
	FD_SET (sockfd, &read_fd);
		
	struct timeval wait_time;

	while( true )
	{
		// increment the value of the variable containing the last round in which we received a reply
		for( auto&& i: neighbor )
			++(*i);
		
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
			
			// Recivefrom ...
			// Sprawdż poprawność
			// Sprawdź czy to coś nowego czy aktualizacja
			// Sprawdzić czy od kogoś nic nie otrzymaliśmy
		}
		
		/*
		for( auto&& i: neighbor )
			i->sprawdzenie() ?
		
		for( auto&& i: rib )
			i->sprawdzenie() ?
		*/
		
		cout << "Routing table:\n";
		for( auto&& i: neighbor )
			cout << *i << '\n';
		
		for( auto&& i: rib )
			cout << *i << '\n';

		cout << '\n';

	}
	
	close (sockfd);
}
