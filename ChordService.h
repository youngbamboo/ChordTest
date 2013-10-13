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
		void buildFingerTable(std::map<uint32_t,string>* themap);
		//int receiveReply(std::map<uint32_t,string>* themap);
	private:
		Node* localNode;
		Node* preNode;
		list<uint32_t> fingerNodeList;
		list<uint32_t> fingerSuccessorList;
		list<string> successorIPList;
		void* receiveReply(std::map<uint32_t,string>* themap);
 
		void printFingerTable();
};

#endif

