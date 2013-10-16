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
		int buildHashID(const string theStr);
		inline int getHashID() {return myID;};
		void setReplyList(list<int>* theList);
		/*
		inline void getFingerTable(list<int>& nodeList, list<int>& succList, list<string>& succIPList) 
		{
			nodeList=fingerNodeList;
			succList=fingerSuccessorList;
			succIPList=successorIPList;
		};
		*/
	private:
		string myHostName;
		string myIP;
		int myID;
		const int myBroadcastPort;
		const int myReceivePort;
		const int myClientPort;
		//list<int> fingerNodeList;
		//list<int> fingerSuccessorList;
		//list<string> successorIPList;

		
		
	
			
			
};

#endif

