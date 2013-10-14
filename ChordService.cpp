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
#include <math.h>
#include <sys/time.h>


#include "Node.h"
#include "ChordService.h"

using namespace std;

ChordService::ChordService():localNode(NULL),preNode(NULL)
{
	localNode = new Node();
	cout<<"Initialize finger table"<<endl;
	uint32_t aID = this->getLocalNode()->getHashID();
	string aIP = this->getLocalNode()->getIP();
	for (int i=1;i<=16;i++)
    {
		uint32_t key = (uint32_t)fmod(aID+pow(2,i-1),65535);
        fingerNodeList.push_back(key);
        fingerSuccessorList.push_back(aID);
        successorIPList.push_back(aIP);
    }
	printFingerTable();
}

ChordService::~ChordService()
{
	delete localNode;
	localNode = NULL;

	delete preNode;
	preNode = NULL;
}

int ChordService::receiveReply(std::map<uint32_t,string>* themap)
{
    /*
	cout<<"receiveReply: Waiting for reply..."<<endl;
	int n, fd;
    socklen_t cli_addr_len;
    char buf[1024] = {0};
	
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        cerr<<"socket error!"<<endl;
        exit(1);
    } 

	struct sockaddr_in servaddr, cliaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(10000);

	fd_set read_fds, master; // set of read fds.

    FD_ZERO(&read_fds); // clear the read fd set
    FD_ZERO(&master); // clear the read fd set

	FD_SET(fd, &read_fds);
    

	int fdmax = fd;

	master = read_fds;

	int activity;
	time_t startTime;
	time(&startTime);
	while (1)
	{
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
				if(i == fd)
				{
					cli_addr_len = sizeof(cliaddr);
    			    n =recvfrom(fd, buf, 1024, 0, (struct sockaddr *)&cliaddr, &cli_addr_len);
    			    if (n>0)
    			    {
    			        cout<<buf<<endl;
    				    uint32_t aID = atoi(buf);
    				    cout<<"Recieve the id: "<< aID <<endl;
    				    char ipstr[INET6_ADDRSTRLEN];
    				   // string aIP = inet_ntop(cliaddr.ai_family,get_in_addr(cliaddr.ai_family,ipstr, sizeof ipstr);
    			        string aIP=inet_ntoa(cliaddr.sin_addr);
    				    cout<<"From IP: "<<aIP<<endl;
    			        ((std::map<uint32_t,string>*)themap)->insert(std::pair<uint32_t,string>(aID,aIP));
    			    }
    			    else
    			    {
    			        //cout<<"Nothing received"<<endl;
    			    }
				}
				
			}
            time_t currentTime;
            time(&currentTime);
            if (difftime(currentTime,startTime)>5)
            {
                cout<<"sleep 5s is over"<<endl;
                close(fd);
                break;
            }
		}
	}*/
    
	cout<<"receiveReply: Waiting for reply..."<<endl;
	int n, fd;
    socklen_t cli_addr_len;
    char buf[1024] = {0};
    struct sockaddr_in servaddr, cliaddr;

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        cerr<<"socket error!"<<endl;
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(10000);

    if (bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        cerr<<"bind"<<endl;
        exit(1);
    }
	time_t startTime;
	time(&startTime);
    while(1)
    {
       cli_addr_len = sizeof(cliaddr);
		/*
	   struct timeval tv;
       tv.tv_sec = 0;
       tv.tv_usec = 500000;//500ms
       if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) 
       {
          perror("Error");
       }
	   */
       n =recvfrom(fd, buf, 1024, MSG_DONTWAIT, (struct sockaddr *)&cliaddr, &cli_addr_len);
       if (n>0)
       {
           cout<<buf<<endl;
	       uint32_t aID = atoi(buf);
	       cout<<"Recieve the id: "<< aID <<endl;
	       char ipstr[INET6_ADDRSTRLEN];
	      // string aIP = inet_ntop(cliaddr.ai_family,get_in_addr(cliaddr.ai_family,ipstr, sizeof ipstr);
           string aIP=inet_ntoa(cliaddr.sin_addr);
	       cout<<"From IP: "<<aIP<<endl;
           ((std::map<uint32_t,string>*)themap)->insert(std::pair<uint32_t,string>(aID,aIP));
       }
       else
       {
           //cout<<"Nothing received"<<endl;
       }
	   time_t currentTime;
	   time(&currentTime);
	   if (difftime(currentTime,startTime)>5)
	   {
	       cout<<"sleep 5s is over"<<endl;
           close(fd);
		   break;
	   }
	   
    }
   
}

void ChordService::buildFingerTable(std::map<uint32_t,string>* themap)
{
    cout<<"Enter build finger table"<<endl;
	uint32_t aID = this->getLocalNode()->getHashID();
	string aIP = this->getLocalNode()->getIP();
	if (themap==NULL || themap->size()==0)
	{
		//It's the first node
		cout<<"It looks like I'm the first node"<<endl;
		setPreNode(NULL);	
	}
	else
	{
		cout<<"There are nodes in the ring"<<endl;
		//Something in the ring.

		std::list<uint32_t>::iterator fingerNodeit;
		std::list<uint32_t>::iterator fingerSuccessorit;
		std::list<string>::iterator successorIPListit;
			
		std::map<uint32_t,string>::iterator it = themap->begin();
	
		for (it=themap->begin(); it!=themap->end(); ++it)
		{
			uint32_t tmpID = it->first;
			string tmpIP = it->second;
    		cout << tmpID << " => " << tmpIP << endl;
			fingerNodeit = fingerNodeList.begin();
			fingerSuccessorit = fingerSuccessorList.begin();
			successorIPListit = successorIPList.begin();
			int i=0;
			for(;fingerNodeit!=fingerNodeList.end()&&fingerSuccessorit!=fingerSuccessorList.end()&&successorIPListit!=successorIPList.end();
			fingerNodeit++,fingerSuccessorit++,successorIPListit++)
			{
				if (tmpID<(*fingerNodeit))
				{
					if (tmpID<(*fingerSuccessorit))
					{
						*fingerSuccessorit=tmpID;
						*successorIPListit=tmpIP;
					}
            		/*
					if (i==15)
					{
                      //  cerr<"3"<<endl;
						//Update Last column.
						//Needs to make sure the original successor bigger than tmpID. Then replace it...
						if(*fingerSuccessorit>tmpID)
						{
							*fingerSuccessorit=tmpID;
							*successorIPListit=tmpIP;
						}
					}*/
				}
				else
				{
                    if ((*fingerNodeit)>(*fingerSuccessorit))
                    {
						*fingerSuccessorit=tmpID;
						*successorIPListit=tmpIP;
                    }
					else if (tmpID<(*fingerSuccessorit))
					{
						*fingerSuccessorit=tmpID;
						*successorIPListit=tmpIP;
					}
					//It is bigger than the (i-1) column
					//But if it's the first node...
					/*if (i!=0)
					{
                      //  cerr<"5"<<endl;
						//Needs to make sure the original successor bigger than tmpID. Then replace it...
                        fingerSuccessorit--;
                        successorIPListit--;
						if (*fingerSuccessorit==0 || *fingerSuccessorit>tmpID)
						{
							*fingerSuccessorit=tmpID;
							*successorIPListit=tmpIP;
						}
						else
						{
							//No else...
						}
                        fingerSuccessorit++;
                        successorIPListit++;
					}*/
				}
				i++;
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
			fingerNodeit++,fingerSuccessorit++,successorIPListit++)
	{
		cout<<*fingerNodeit<<" "<<*fingerSuccessorit<<" "<<*successorIPListit<<endl;
	}
}

// Format: "key//value" Simple only for test usage
void sendRequestToServer(string receiverIP,string data)
{
    
    struct sockaddr_in receiverAddr;

    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(CLIENT_PORT);

    if(inet_aton(receiverIP.c_str(), &receiverAddr.sin_addr) == 0)
	{
        cerr<<"INET_ATON Failed\n"<<endl;
        exit(1);
    }

    if(sendto(client_sockfd, data.c_str(), data.length(), 0,
                (struct sockaddr*) &receiverAddr, sizeof(receiverAddr)) == -1)
    {

        cerr<<"Failed to send to "<<receiverIP<<" with data "<<data<<endl;
    }
    else
	{
        cerr<<"Successfully send to "<<receiverIP<<" with data "<<data<<endl;
    }

}

//request should be sent here
int lookupFingerTable()
{
		
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
	cout<<"broadcast will be: "<<aBuf<<endl;
    if ((numbytes=sendto(sockfd, aBuf.c_str(), aBuf.length(), 0,
                    (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
    {
        perror("sendto");
        exit(1);
    }
    printf("sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));
    close(sockfd);
	std::map<uint32_t,string> mymap;
	//pthread_t thread;
	//int rc = pthread_create(&thread, NULL, receiveReply, mymap);
    //cout<<"Begin to sleep"<<endl;
	//sleep(5);
    //cout<<"After sleep"<<endl;
	//pthread_exit(NULL);
    //cout<<"Kill the thread"<<endl;

	myService->receiveReply(&mymap);
	
	myService->buildFingerTable(&mymap);



	//Initialization is finished.

	//Define the socket
	//ChordSocket: Listen the 10000 port for broadcast. Send the reply (to 10001) and update its own finger table.
	//ClientSocket: Listen the 9999 to receive the key-value data. 

	int clientSocket, chordSocket, clientGetSocket;
	
	if((clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        cerr<<"Error when initializing client socket"<<endl;
        exit(1);
    }

    if((chordSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        cerr<<"Error when initializing chord socket"<<endl;
        exit(1);
    }

	struct sockaddr_in clientUDP,chordUDP, clietGetUDP;

    memset((char*)&clientUDP, 0, sizeof(clientUDP));
    clientUDP.sin_family = AF_INET;
    clientUDP.sin_port = htons(myService->getLocalNode()->getClientPort());
    clientUDP.sin_addr.s_addr = htonl(INADDR_ANY);

    memset((char*)&chordUDP, 0, sizeof(chordUDP));
    chordUDP.sin_family = AF_INET;
    chordUDP.sin_port = htons(myService->getLocalNode()->getBroadcastPort());
    chordUDP.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(clientSocket, (struct sockaddr*) &clientUDP, sizeof(clientUDP)) == -1)
    {
        cerr<<"Bind failed for client socket"<<endl;
        exit(1);
    }
    
    if(bind(chordSocket, (struct sockaddr*) &chordUDP, sizeof(chordUDP)) == -1)
    {
        cerr<<"Bind failed for chord socket"<<endl;
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
		else
		{
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
	                    char buf[1024] = {0};
	                    int newfd =recvfrom(chordSocket, buf, 1024, 0, (struct sockaddr *)&cliaddr, &cli_addr_len);              
	                    cout<<buf<<endl;
	               	   	uint32_t aID = atoi(buf);
	               	    cout<<"Recieve the broadcast from id: "<< aID <<endl;
	               	    char ipstr[INET6_ADDRSTRLEN];

	               	    string aIP=inet_ntoa(cliaddr.sin_addr);
	                    cout<<"From IP: "<<aIP<<endl;

						std::map<uint32_t,string> tmpMap;
						tmpMap.insert(std::pair<uint32_t,string>(aID,aIP));

						myService->buildFingerTable(&tmpMap);

						if (!fork()) 
						{ // this is the child process
							//close(sockfd); // child doesn't need the listener
							string myID = std::to_string(myService->getLocalNode()->getHashID());
	                        int numbytes;
	                        int sendfd;
	                        if ((sendfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	                        {
	                            cerr<<"socket error"<<endl;
	                            exit(1);
	                        }
	                        cliaddr.sin_port = htons(10000);
	                        if ((numbytes=sendto(sendfd, myID.c_str(), myID.length(), 0,
	                                                (struct sockaddr *)&cliaddr, sizeof cliaddr)) == -1) 
	                        {
	                            cerr<<"send error"<<endl;
	                            exit(1);
	                        }
	                        else
	                        {
	                            cout<<"send back with my id: "<<myID<<endl;

	                        }
							close(sendfd);
						}
						close(newfd);
						
					}
					else if (i == clientSocket)
					{
	                    
						cout<<"Receive store request"<<endl;
						
	                    char* maxMessage = new char[1024];
	                    struct sockaddr_in senderProcAddrUDP;

	                    // To store the address of the process from whom a message is received
	                    memset((char*)&senderProcAddrUDP, 0, sizeof(senderProcAddrUDP));
	                    socklen_t senderLenUDP = sizeof(senderProcAddrUDP);
	                        
	                    int recvRet = 0;

	                    recvRet = recvfrom(clientSocket, maxMessage, 1024,
	                                0, (struct sockaddr*) &senderProcAddrUDP, &senderLenUDP);

	                    if(recvRet > 0)
						{
	                         // It's a put key-value message
	                         //Format: "key//value"
	                         string key;
							 string value;
							 string tmpResult = maxMessage;

							 int found = tmpResult.find("//");
							 if (found!=std::string::npos)
							 {
							 	key = tmpResult.substr(0,found);
								value = tmpResult.substr(found+2,tmpResult.length()-found-2);
								uint32_t theHash = myService->getLocalNode()->buildHashID(key);
								cout<<"Message from client:"<<endl;
								cout<<"Key: "<<key<<endl;
								cout<<"Key hash: "<<theHash<<endl;
								cout<<"Value: "<<value<<endl;
							 }
							 else
							 {
							 	cerr<<"Put message from client is not right"<<endl;
							 }
							 

	                         if(type == CLIENT_REQ){		
	                              myChordInstance->handleRequestFromClient(maxMessage, recvRet);
	                         }
	                         else{
	                              cout << "SERVICE: Invalid message received: " << type << endl;
	                         }
	                     }
	                     else{
	                          cout << "Error " << errno << " while receiving message at clientsocket\n" << endl;
	                     }

	                     delete[] maxMessage;
	                     
					}
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

