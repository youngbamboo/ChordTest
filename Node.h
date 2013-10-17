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
#include <map>
#include <list>

using namespace std;

class Node {
	public:
		Node();
		~Node();
		inline int getBroadcastPort() {return myBroadcastPort;};
		inline int getReceivePort() {return myReceivePort;};
		inline int getClientPort() {return myClientPort;};
		inline int getClienGetPort() {return myClientGetPort;};
		inline int getClientDeletePort() {return myClientDeletePort;};
		inline string getIP() {return myIP;};
		inline string getHostName() {return myHostName;};
		int buildHashID(const string theStr);
		inline int getHashID() {return myID;};
		void setReplyList(list<int>* theList);
		int storeData(const string theKey, const string theValue);
		void getData(const string theKey, list<string>& result);
		int deleteData(const string theKey);
	private:
		string myHostName;
		string myIP;
		int myID;
		const int myBroadcastPort;
		const int myReceivePort;
		const int myClientPort;
		const int myClientGetPort;
		const int myClientDeletePort;
		std::multimap<string,string> myCacheMape;
		const string myFile;

		
		
	
			
			
};

#endif

