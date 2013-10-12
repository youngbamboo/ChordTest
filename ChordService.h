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

using namespace std;

class ChordService {
	public:
		ChordService();
		~ChordService();
		inline Node* getLocalNode() {return localNode;};
		inline Node* getPreNode() {return preNode;};
		inline void setPreNode(Node* theNode) {this->preNode=theNode;};
	private:
		Node* localNode;
		Node* preNode;
		list<Node*> successorList;
	
			
			
};

#endif

