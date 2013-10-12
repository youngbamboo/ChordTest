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

using namespace std;

class ChordService {
	public:
		ChordService();
		~ChordService();
	private:
		string myHostName;
		string myIP;
		int myID;
	
			
			
};

#endif

