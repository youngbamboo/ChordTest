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
#include <list>


using namespace std;

class Node {
	public:
		Node();
		~Node();
		inline int getBroadcastPort() {return myBroadcastPort;};
		inline int getReceivePort() {return myReceivePort;};
		inline int getClientPort() {return myClientPort;};
		inline string getIP() {return myIP;};
		inline string getHostName() {return myHostName;};
		uint32_t buildHashID(const string theStr);
		inline uint32_t getHashID() {return myID;};
		void setReplyList(list<uint32_t>* theList);
	private:
		string myHostName;
		string myIP;
		uint32_t myID;
		const int myBroadcastPort;
		const int myReceivePort;
		const int myClientPort;

		
		
	
			
			
};

#endif

