#ifndef _CHORDSERVICE_H
#define _CHORDSERVICE_H

#include <stdio.h>
#include <string>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <map>
#include <list>

using namespace std;

class ChordService {
	public:
		ChordService();
		~ChordService();
		inline Node* getLocalNode() {return this->localNode;};
		inline Node* getPreNode() {return this->preNode;};
		inline void setPreNode(Node* theNode) {this->preNode=theNode;};
		void buildFingerTable(std::map<int,string>* themap);
		int receiveReply(std::map<int,string>* themap);
        void sendRequestToServer(string receiverIP,string key, string value, string clientIP, string initNode,string operation);
        int lookupFingerTable(int thekey, string& theIP, int initNode);
		
		void getFileList();
		
		int mkDirectory(const string s);
		void printFingerTable();
		int calculateLength(int node,int key, int initNode);
		
		static list<int> fingerNodeList;
		static list<int> fingerSuccessorList;
		static list<string> successorIPList;
		
	private:
		Node* localNode;
		Node* preNode;
		
		//cache the data in this machine
		//map<string,string>* themap;
		//used for when this node is taken as a client
		//int client_sockfd;
		//void* receiveReply(std::map<int,string>* themap);

		
 
		
};

#endif

