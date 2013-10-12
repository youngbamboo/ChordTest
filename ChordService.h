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
		inline int getPort() {return myPort;};
	private:
		string myHostName;
		string myIP;
		int myID;
		const int myPort = 2345;
	
			
			
};

#endif

