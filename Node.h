#ifndef _NODE_H
#define _NODE_H

#include <stdio.h>
#include <string>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>

using namespace std;

class Node {
	public:
		Node();
		~Node();
		inline int getBroadcastPort() {return myBroadcastPort;};
		inline string getIP() {return myIP;};
		uint32_t buildHashID();
		inline uint32_t getHashID() {return myID;};
	private:
		string myHostName;
		string myIP;
		uint32_t myID;
		const int myBroadcastPort;
	
			
			
};

#endif

