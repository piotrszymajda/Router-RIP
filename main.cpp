/*
Autor: Piotr Szymajda - 273 023

Program typu "Router" - implementacja algorytmu wektora odległości
i tworzenia tablic przekazywania - drugie zad. programistyczne na SK
*/
#include <iostream>
#include <cstring>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include "nets.h"
#include "rip.h"

#define VERSION 1
#define BUILD 4

using namespace std;

void man ();

int main (int argc, char *argv[])
{
    if (argc >= 2)
    {   
        if( strcmp (argv [1], "--help") == 0 || strcmp (argv [1], "-h") == 0 )
        {
            man();
        }
        else if( strcmp (argv [1], "--version") == 0 || strcmp (argv [1], "-v") == 0 )
        {
            cout << "Version: " << VERSION << "." << BUILD << '\n' ;
        }
        else
        {
            cout << "Error: Wrong argument\n";
            man ();
        }
        return 1;
    }
    
    vector<nets*> neighbor;
 
    short interfaces;
    cout << "Enter the number of interfaces: ";
    cin >> interfaces;
    
    if( interfaces <= 0 )
   	{
   		cout << "I feel so lonely ;( \n";
   		return 0;
   	}
   	
    cout << "Enter interfaces configuration (" << interfaces << " times):\n";
 
    while( interfaces-- )
    {
        string ip, tmp;
        short mask, dst;
        cin >> ip;
        cin.ignore (100, '/');
        cin >> mask >> tmp >> dst;
        
        neighbor.push_back (new nets(ip.c_str(), mask, dst, true));
    }
    
    cout << "------------------------------------------\n";
    cout << "\033[1mStarting RIP with following configuration: \033[0m\n";
    for( auto&& i: neighbor ) // access by reference
        cout << "> " << *i << '\n';
    cout << "------------------------------------------\n";
    
    // start RIP
    rip (neighbor);
    
	for( auto&& i: neighbor )
		delete i;

    return 0;
}

/*
Function displays manual
*/
void man()
{
    cout << "Router - program that implement algorithm of distance vector\n";
    cout << "\t and creating forwarding table.\n";
    cout << "\nUsage:\n";
    cout << "\tsudo ./router - for start program\n";
    cout << "\t./router -h - for display this help\n";
    cout << "\t./router -v - for display version\n";
    cout << "\n";
    cout << "After launched, the program waits for the initial setup.\n";
    cout << "At the beginning, specify the number N that indicating the amount of\n";
    cout << "IP addresses contained in the next N lines. Each entry is as follows:\n";
    cout << "\t<IP address> netmask /M distance D\n";
    cout << "Where M is from 0 to 32, and D is from 0 to " << MAX_DIST << "\n";
    cout << "\nFor example:\n";
    cout << "\t2\n";
    cout << "\t10.0.1.1 netmask /8 distance 3\n";
    cout << "\t192.168.5.43 netmask /24 distance 2\n";
    
}
