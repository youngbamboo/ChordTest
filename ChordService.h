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
		inline int getBroadcastPort() {return myPort;};
		inline string getIP() {return myIP;};
		string buildHashID();
		inline string getHashID() {return myID;};
	private:
		string myHostName;
		string myIP;
		string myID;
		const int myBroadcastPort;
	
			
			
};

#endif

