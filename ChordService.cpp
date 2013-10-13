#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map>

#include "Node.h"
#include "ChordService.h"

using namespace std;

ChordService::ChordService():localNode(NULL),preNode(NULL)
{
	localNode = new Node();
}

ChordService::~ChordService()
{
	delete localNode;
	localNode = NULL;

	delete preNode;
	preNode = NULL;
}

int ChordService::receiveReply(map<uint32_t,string>* mymap)
{
	int n, fd;
    socklen_t cli_addr_len;
    char buf[1024] = {0};
    struct sockaddr_in servaddr, cliaddr;

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket error!\n");
        exit(0);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(this->getLocalNode()->getBroadcastPort());

    if (bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        perror("bind");
        exit(-1);
    }
	time_t startTime;
	time(&startTime);
    while(1)
    {
       cli_addr_len = sizeof(cliaddr);
       n =recvfrom(fd, buf, 1024, 0, (struct sockaddr *)&cliaddr, &cli_addr_len);
       printf("%d\n", n);
       cout<<buf<<endl;
	   uint32_t aID = aoti(buf);
	   cout<<"Recieve the id: "<< aID <<endl;
	   char ipstr[INET6_ADDRSTRLEN];
	   string aIP = inet_ntop(addr.ss_family,addr.ss_family == AF_INET?
								((struct sockadd_in *)&cliaddr)->sin_addr:
								((struct sockadd_in6 *)&cliaddr)->sin6_addr,
								ipstr, sizeof ipstr);
	   cout<<"From IP: "<<aIP<<endl;;

	   mymap->insert(std::pair<uint32_t,string>(aID,aIP));
	   
	   time_t currentTime;
	   time(&currentTime);
	   if (difftime(currentTime,startTime)>5)
	   {
	       cout<<"sleep 5s is over"<<endl;
		   break;
	   }
    }
}

void ChordService::buildFingerTable(std::map<uint32_t,string>* themap)
{
	uint32_t aID = this->getLocalNode()->getHashID();
	string aIP = this->getLocalNode()->getIP();
	for (int i=1;i++;i<=16)
	{
		fingerNodeList.push_back(aID+pow(2,i-1));
		fingerSuccessorList.push_back(aID);
		successorIPList.push_back(aIP);
	}
	if (themap==NULL || themap.size()==0)
	{
		//It's the first node
		cout<<"It looks like I'm the first node"<<endl;
		setPreNode(NULL);	
		//std::list<uint32_t>::iterator fingerNodeit = fingerNodeList.begin();
		//std::list<uint32_t>::iterator fingerSuccessorit = fingerSuccessorList.begin();
		printFingerTable();
	}
	else
	{
		cout<<"There are nodes in the ring"<<endl;
		//Something in the ring.
		
		std::map<uint32_t,string>::iterator it = themap->begin();
	
		for (it=themap.begin(); it!=themap.end(); ++it)
		{
			uint32_t tmpID = it->first;
			string tmpIP = it->second;
    		cout << tmpID << " => " << tmpID << '\n';
			int i=0;
			while (i<16)
			{
				if (tmpID>fingerNodeList[i])
				{
					if (i==15)
					{
						//Last colunme
						fingerSuccessorList[i]=tmpID;
						successorIPList[i]=tmpIP;
					}
					i++;
					continue;
				}
				else
				{
					//update (i-1) colume
					fingerSuccessorList[i-1]=tmpID;
					successorIPList[i-1]=tmpIP;
				}
			}
			printFingerTable();
			
		}
	}
	
}

void ChordService::printFingerTable()
{
	cout<<"Finger Size: "<<endl;
	cout<<"Node: "<<fingerNodeList.size()<<endl;
	cout<<"Successor: "<<fingerSuccessorList.size()<<endl;
	cout<<"IP: "<<successorIPList.size()<<endl;
	
	std::list<uint32_t>::iterator fingerNodeit = fingerNodeList.begin();
	std::list<uint32_t>::iterator fingerSuccessorit = fingerSuccessorList.begin();
	std::list<string>::iterator successorIPListit = successorIPList.begin();
	for (;fingerNodeit!=fingerNodeList.end()&&fingerSuccessorit!=fingerNodeList.end()&&successorIPListit!=successorIPList.end();
			fingerNodeit++,fingerSuccessorit,successorIPListit)
	{
		cout<<*fingerNodeit<<" "<<*fingerSuccessorit<<" "<<*successorIPListit<<endl;
	}
}

int main(int argc, char* argv[])
{
	ChordService* myService = new ChordService();
	
	//1. Send the broadcast first with my HashID
	//pthread_t listen_reply_thread;
	//int rc= pthread_create(&listen_reply_thread, NULL, receiveReply, NULL);
    
    string broadcastName="255.255.255.255";
    int sockfd;
    struct sockaddr_in their_addr; // connector's address information
    struct hostent *he;
    int numbytes;
    int broadcast = 1;
    if ((he=gethostbyname(broadcastName.c_str())) == NULL)
    {
        cerr<<"gethostbyname"<<endl;
        exit(-1);
    }
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        cerr<<"socket error"<<endl;
        exit(-1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1) 
	{
        cerr<<"setsockopt (SO_BROADCAST)"<<endl;
        exit(-1);
    }

    their_addr.sin_family = AF_INET; // host byte order
    their_addr.sin_port = htons(myService->getLocalNode()->getBroadcastPort()); // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

	string aBuf = std::to_string(myService->getLocalNode()->getHashID());
	cout<<"braodcast will be: "<<aBuf<<endl;
    if ((numbytes=sendto(sockfd, aBuf.c_str(), aBuf.length(), 0,
                    (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
    {
        perror("sendto");
        exit(1);
    }
    printf("sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));
    close(sockfd);
	std::map<uint32_t,string>* mymap = NULL;

	myService->receiveReply(mymap);
	
	myService->buildFingerTable(mymap);

	delete mymap;
	mymap=NULL;

	//Initialization is finished.

	//Define the socket
	//ChordSocket: Listen the 10000 port for broadcast. Send the reply (to 10001) and update its own finger table.
	//ClientSocket: Listen the 9999 to receive the key-value data. 

	int clientSocket, chordSocket;
	
	if((clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        cerr<<"Error when initializing client socket"<<endl;
        exit(1);
    }

    if((chordSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        cerr<<"Error when initializing chord socket"<<endl;
        exit(1);
    }

	struct sockaddr_in clientUDP,chordUDP;

    memset((char*)&clientUDP, 0, sizeof(clientUDP));
    clientUDP.sin_family = AF_INET;
    clientUDP.sin_port = htons(this->getLocalNode()->getClientPort());
    clientUDP.sin_addr.s_addr = htonl(INADDR_ANY);

    memset((char*)&chordUDP, 0, sizeof(chordUDP));
    chordUDP.sin_family = AF_INET;
    chordUDP.sin_port = htons(this->getLocalNode()->getBroadcastPort());
    chordUDP.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(clientSocket, (struct sockaddr*) &clientUDP, sizeof(clientUDP)) == -1){
        generalInfoLog("Bind failed for server socket \n");
        exit(1);
    }
    
    if(bind(chordSocket, (struct sockaddr*) &chordUDP, sizeof(chordUDP)) == -1){
        generalInfoLog("Bind failed for client socket\n");
        exit(1);
    }

	fd_set read_fds, master; // set of read fds.

    FD_ZERO(&read_fds); // clear the read fd set
    FD_ZERO(&master); // clear the read fd set

	FD_SET(chordSocket, &read_fds);
    FD_SET(clientSocket, &read_fds);

	int fdmax = (chordSocket > clientSocket ? chordSocket : clientSocket);

	master = read_fds;

	int activity;

	while (1)
	{
		//This machine listens the port 10000, where broadcast of the new node will be received.
		//Then send its id and update its own fingertable.
		//It's based on the asumption that each node is added one by one.
		read_fds = master;
		activity = select( fdmax + 1 , &read_fds , NULL , NULL , NULL);
		if ((activity < 0) && (errno!=EINTR)) 
        {
            cerr<<"select error"<<endl;
        }
		for(int i = 0; i<=fdmax; i++)
		{
			if(FD_ISSET(i, &read_fds))
			{
				if(i == chordSocket)
				{
					cout<<"Received broadcast message"<<endl;
					struct sockaddr_in cliaddr;
					socklen_t cli_addr_len;
					cli_addr_len = sizeof(cliaddr);
                    int newfd =recvfrom(fd, buf, 1024, 0, (struct sockaddr *)&cliaddr, &cli_addr_len);              
                    cout<<buf<<endl;
               	   	uint32_t aID = aoti(buf);
               	    cout<<"Recieve the broadcast from id: "<< aID <<endl;
               	    char ipstr[INET6_ADDRSTRLEN];
               	    string aIP = inet_ntop(addr.ss_family,addr.ss_family == AF_INET?
               								((struct sockadd_in *)&cliaddr)->sin_addr:
               								((struct sockadd_in6 *)&cliaddr)->sin6_addr,
               								ipstr, sizeof ipstr);
               	    cout<<"From IP: "<<aIP<<endl;

					std::map<uint32_t,string>* mymap=NULL;
					mymap->insert(std::pair<uint32_t,string>(aID,aIP));
	
					myService->buildFingerTable(mymap);

					delete mymap;
					mymap=NULL;

					if (!fork()) 
					{ // this is the child process
						close(sockfd); // child doesn't need the listener
						string myID = std::to_string(myService->getLocalNode()->getHashID());
						if (send(newfd, myID.c_str(), myID.legnth(), 0) == -1)
						{
								cout<<"send back with my id: "<<myID<<endl;
						}
						close(newfd);
					}
					close(newfd);
					
				}
				else if (i == clientSocket)
				{
				}
			}
		}
	}

	
	
	/*int n, fd;
    socklen_t cli_addr_len;
    char buf[1024] = {0};
    struct sockaddr_in servaddr, cliaddr;
 
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket error!\n");
        exit(0);
    }
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(myService->getPort());
 
    if (bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        perror("bind");
        exit(-1);
    }
    while(1)
    {
       cli_addr_len = sizeof(cliaddr);
       n =recvfrom(fd, buf, 1024, 0, (struct sockaddr *)&cliaddr, &cli_addr_len);
       cout<<buf<<endl;
    }

*/

    return 0;
}

