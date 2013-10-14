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
#include "ClientMessage.h"


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
		uint32_t preID =it->first;
		for (it=themap->begin(); it!=themap->end(); ++it)
		{
			uint32_t tmpID = it->first;
			string tmpIP = it->second;
    		cout << tmpID << " => " << tmpIP << endl;
			if (tmpID<aID)
			{
				if (tmpID>preID)
				{
					preID=tmpID;
				}
			}
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
				}
				i++;
			}
			if (preID<aID)
			{
				//Find the PreNode.
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

void ChordService::setSystemParam()
{
     //Socket variables for client
    int n;
    struct sockaddr_in servaddr;
    socklen_t lensock;

    client_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(client_sockfd == -1)
    {
        printf("Could not create socket \n");
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(CLIENT_PORT);

    int bval = bind(client_sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    if(bval==-1)
    {
        printf("Bind failed\n");
        exit(EXIT_FAILURE);
    }
}

void ChordService::sendRequestToServer(string receiverIP,string key, string value, string clientIP, string initNode)
{
    ClientRequest* req = new ClientRequest;
	
	req->key_length=key.length();
	req->value_length=value.length();
	req->initialNode_length=initNode.length();
	req->clientIP_length=clientIP.length();
	
	char* msgBuffer = NULL;
	long messageLen = 0;
	messageLen = sizeof(ClientRequest) + key.length() + value.length()+ initNode.length() + clientIP.length();
	
	msgBuffer = new char[messageLen];
	memcpy(msgBuffer,req,sizeof(ClientRequest));
	memcpy(msgBuffer+sizeof(ClientRequest),initNode.c_str(),initNode.length());
	memcpy(msgBuffer+sizeof(ClientRequest)+initNode.length(),key.c_str(),key.length());
	memcpy(msgBuffer+sizeof(ClientRequest)+initNode.length()+key.length(),value.c_str(),value.length());
	memcpy(msgBuffer+sizeof(ClientRequest)+key.length()+value.length()+initNode.length(),clientIP.c_str(),clientIP.length());
	
	struct sockaddr_in receiverAddr;

    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(9999);

    if(inet_aton(receiverIP.c_str(), &receiverAddr.sin_addr) == 0)
	{
        cerr<<"INET_ATON Failed\n"<<endl;
        exit(1);
    }
    
    if(sendto(client_sockfd, msgBuffer, messageLen, 0,
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
//Return 0 failed, 1 success
int ChordService::lookupFingerTable(uint32_t thekey, string& theIP, uint32_t initNode)
{
	//First check my node...
	std::list<uint32_t>::iterator fingerSuccessorit = fingerSuccessorList.begin();
	std::list<string>::iterator successorIPListit = successorIPList.begin();
	uint32_t compare = 65535;
	uint32_t tmpNode;
	string tmpIP;
	uint32_t localID = myService->getLocalNode()->getHashID();
	

	if (thekey==localID)
	{ 
		return 1;
	}
	else if (initNode!=65536)
	{
		if (thekey>initNode)
		{
			if (localID>thekey)
			{
				return 1;
			}
		}
		if (thekey<initNode)
		{
			if (localID>thekey&&localID<initNode)
			{
				return 1;
			}
		}
		
	}
	if (theKey>initNode)
	{
		for (;fingerSuccessorit!=fingerNodeList.end()&&successorIPListit!=successorIPList.end();
			fingerSuccessorit++,successorIPListit++)
		{
			if((*fingerSuccessorit)==thekey)
			{
				theIP=*successorIPListit;
				return 0;
			}
			else if ((*fingerSuccessorit)>thekey)
			{
				if((*fingerSuccessorit)==successorIPList.front())
				{
					//My first successor is bigger than key, choose between local and successor
					uint32_t firstSuccessor = *fingerSuccessorit;
					if ((thekey-localID)<=(firstSuccessor-thekey))
					{
						return 1;
					}
					else
					{
						theIP=*successorIPListit;
						return 0;
					}
				}
				uint32_t thisNode = *fingerSuccessorit;
				uint32_t lastNode = *(--fingerSuccessorit);
				if ((thekey-lastnode)>(thisnode-thekey))
				{
					theIP = *successorIPListit;
					return 0;
				}
				else
				{
					theIP = *(--successorIPListit);
					return 0;
				}
			}
			else 
			{
				theIP=*successorIPListit;
			}
		}
		return 0;
	}
	if (theKey<initNode)
	{		
		for (;fingerSuccessorit!=fingerNodeList.end()&&successorIPListit!=successorIPList.end();
			fingerSuccessorit++,successorIPListit++)
		{
			if((*fingerSuccessorit)==localNode)
			{
				//only myself in the ring
				return 1;
			}
			if((*fingerSuccessorit)==thekey)
			{
				theIP=*successorIPListit;
				return 0;
			}
			else if (localNode<=65535&&(*fingerSuccessorit)<localNode)
			{
				else if ((*fingerSuccessorit)<=initNode)
				{
					if ((*fingerSuccessorit)>=thekey 
						||(*fingerSuccessorit)>=initNode )
					{
						if((*fingerSuccessorit)==successorIPList.front())
						{
							//My first successor is bigger than key, choose between local and successor
							uint32_t firstSuccessor = *fingerSuccessorit;
							if (localID>initNode)
							{
								if((65535-localID+thekey)<=(firstSuccessor-thekey))
								{
									return 1;
								}
								else
								{
									theIP=*successorIPListit;
									return 0;
								}
							}
							else
							{
								if((thekey-localID)<=(firstSuccessor-thekey))
								{
									return 1;
								}
								else
								{
									theIP=*successorIPListit;
									return 0;
								}
							}
							
						}
						uint32_t thisNode = *fingerSuccessorit;
						uint32_t lastNode = *(--fingerSuccessorit);
						if (lastNode>initNode)
						{
							if((65535-lastNode+thekey)<=(thisNode-thekey))
							{
								return 1;
							}
							else
							{
								theIP=*successorIPListit;
								return 0;
							}
						}
						else
						{
							if((thekey-lastNode)<=(thisNode-thekey))
							{
								return 1;
							}
							else
							{
								theIP=*successorIPListit;
								return 0;
							}
						}
						
						if ((thekey-localNode)>(thisNode-thekey))
						{
							theIP = *successorIPListit;
							return 0;
						}
						else
						{
							return 1;
						}
					}
				}
			}
			else
			{
				// one loop now. Needs to stop now...  
				theIP=*successorIPListit;
			}

		}
		return 0;
	}
	
	//Not find correct node here. "compare" should be the most nearest one now. do something......

	//We need to send this request with key and value to tmpIP.
	theIP=tmpIP;
	return 0;
	
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
	                    struct sockaddr_in cliaddr;
						socklen_t cli_addr_len;
						cli_addr_len = sizeof(cliaddr);
	                        
	                    int recvRet = 0;

	                    recvRet = recvfrom(clientSocket, maxMessage, 1024,
	                                0, (struct sockaddr*) &cliaddr, &cli_addr_len);

	                    if(recvRet > 0)
						{
	                         // It's a put key-value message
	                         //Format: "key//value"
	                         ClientRequest* clientMsg = new ClientRequest;
							 memcpy(clientMsg, maxMessage, sizeof(ClientRequest));
							 
							 string data = maxMessage;
						 
							 long len=sizeof(ClientRequest);
							 string initNode = data.substr(0+len,clientMsg->initialNode_length);
							 
							 len += clientMsg->initialNode_length;
							 string key = data.substr(len,clientMsg->key_length);
							 
							 len += clientMsg->key_length;
							 string value = data.substr(len,clientMsg->value_length);

							 len +=clientMsg->value_length;
							 string clientIP = data.substr(len,clientMsg->clientIP_length);

							 
							 uint32_t theHash = myService->getLocalNode()->buildHashID(key);
							
							 
							 cout<<"Message from client:"<<endl;
							 cout<<"Key: "<<key<<endl;
							 cout<<"Key hash: "<<theHash<<endl;
							 cout<<"Value: "<<value<<endl;							 
							 cout<<"Client IP: "<<aClientip<<endl;

							 string theNextNodeIP;
							 
							 if (initNode=="65536")
							 {
								string initNode = std::to_string(theHash);
							 }
							 int result = myService->lookupFingerTable(theHash,theNextNodeIP,atoi(initNode));
							 if(result==1)
							 {
							 	//store data
							 	//send back to client
									 	
								cout<<"Send successful response message"<<endl;
								
								              

								

								if (!fork()) 
								{ // this is the child process
									//close(sockfd); // child doesn't need the listener
									string result="1";
			                        int numbytes;
			                        int sendfd;
			                        if ((sendfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			                        {
			                            cerr<<"socket error"<<endl;
			                            exit(1);
			                        }
			                        cliaddr.sin_port = htons(9999);
			                        if ((numbytes=sendto(sendfd, result.c_str(), result.length(), 0,
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
								close(recvRet);
							 }
							 else
							 {
							 	//send to the next node.
							 	sendRequestToServer(theNextNodeIP,key, value, clientIP, initNode);
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

