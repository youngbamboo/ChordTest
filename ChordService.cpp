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
#include <fstream> 
#include <dirent.h>
#include <vector>
#include <sys/stat.h>
#include <mutex>


#include "Node.h"
#include "ChordService.h"
#include "ClientMessage.h"


using namespace std;
std::mutex mtx; 

list<int> ChordService::fingerNodeList;
list<int> ChordService::fingerSuccessorList;
list<string> ChordService::successorIPList;


ChordService::ChordService():localNode(NULL),preNode(NULL)
{
	localNode = new Node();
	cout<<"Initialize finger table"<<endl;
	int aID = this->getLocalNode()->getHashID();
	string aIP = this->getLocalNode()->getIP();
	for (int i=1;i<=16;i++)
    {
		int key = (int)fmod(aID+pow(2,i-1),65535);
        ChordService::fingerNodeList.push_back(key);
        ChordService::fingerSuccessorList.push_back(aID);
        ChordService::successorIPList.push_back(aIP);
    }
	//setSystemParam();
	printFingerTable();
}

ChordService::~ChordService()
{
	delete localNode;
	localNode = NULL;

	delete preNode;
	preNode = NULL;
}

int ChordService::mkDirectory(const string s)
{
    /*
    size_t pre=0,pos;
    std::string dir;
    int mdret;

    if(s[s.size()-1]!='/'){
        // force trailing / so we can handle everything in loop
        s+='/';
    }

    while((pos=s.find_first_of('/',pre))!=std::string::npos){
        dir=s.substr(0,pos++);
        pre=pos;
        if(dir.size()==0) continue; // if leading / first time is 0 length
        if((mdret=mkdir(dir.c_str(),mode)) && errno!=EEXIST){
            return mdret;
        }
    }
    return mdret;
    */
    return 1;
}

int ChordService::receiveReply(std::map<int,string>* themap)
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
    				    int aID = atoi(buf);
    				    cout<<"Recieve the id: "<< aID <<endl;
    				    char ipstr[INET6_ADDRSTRLEN];
    				   // string aIP = inet_ntop(cliaddr.ai_family,get_in_addr(cliaddr.ai_family,ipstr, sizeof ipstr);
    			        string aIP=inet_ntoa(cliaddr.sin_addr);
    				    cout<<"From IP: "<<aIP<<endl;
    			        ((std::map<int,string>*)themap)->insert(std::pair<int,string>(aID,aIP));
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
    //char buf[1024] = {0};
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
	   char buf[1024] = {0};
       n =recvfrom(fd, buf, 1024, MSG_DONTWAIT, (struct sockaddr *)&cliaddr, &cli_addr_len);
       if (n>0)
       {
           cout<<buf<<endl;
	       int aID = atoi(buf);
	       cout<<"Recieve the id: "<< aID <<"   "<<buf<<endl;
	       char ipstr[INET6_ADDRSTRLEN];
	      // string aIP = inet_ntop(cliaddr.ai_family,get_in_addr(cliaddr.ai_family,ipstr, sizeof ipstr);
           string aIP=inet_ntoa(cliaddr.sin_addr);
	       cout<<"From IP: "<<aIP<<endl;
           ((std::map<int,string>*)themap)->insert(std::pair<int,string>(aID,aIP));
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

void ChordService::buildFingerTable(std::map<int,string>* themap)
{
	mtx.lock();
    cout<<"Enter build finger table"<<endl;
	int aID = this->getLocalNode()->getHashID();
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
		//printFingerTable();
		//Something in the ring.

		std::list<int>::iterator fingerNodeit;
		std::list<int>::iterator fingerSuccessorit;
		std::list<string>::iterator successorIPListit;
			
		std::map<int,string>::iterator it = themap->begin();
		
		for (it=themap->begin(); it!=themap->end(); ++it)
		{
			int tmpID = it->first;
			string tmpIP = it->second;
    		cout << tmpID << " => " << tmpIP << endl;
			
			fingerNodeit = ChordService::fingerNodeList.begin();
			fingerSuccessorit = ChordService::fingerSuccessorList.begin();
			successorIPListit = ChordService::successorIPList.begin();
			//int i=0;
			for(;fingerNodeit!=ChordService::fingerNodeList.end()&&fingerSuccessorit!=ChordService::fingerSuccessorList.end()
				&&successorIPListit!=ChordService::successorIPList.end();
			++fingerNodeit,++fingerSuccessorit,++successorIPListit)
			{
				if (tmpID<(*fingerNodeit))
				{
					cout<<"tmpID<(*fingerNodeit)"<<endl;
					cout<<tmpID<<","<<(*fingerNodeit)<<","<<(*fingerSuccessorit)<<endl;
					if ((tmpID<(*fingerSuccessorit))&&(*fingerNodeit)>(*fingerSuccessorit))
					{
						cout<<"tmpID<(*fingerSuccessorit))&&(*fingerNodeit)>(*fingerSuccessorit)"<<endl;
						*fingerSuccessorit=tmpID;
						*successorIPListit=tmpIP;
						//fingerSuccessorList.erase(fingerSuccessorit);
						//successorIPList.erase(successorIPListit);
						
						//fingerSuccessorList.insert(fingerSuccessorit,tmpID);
						//successorIPList.insert(successorIPListit,tmpIP);
					}
				}
				else
				{
					cout<<tmpID<<","<<&(*fingerNodeit)<<":"<<(*fingerNodeit)<<","<<&(*fingerSuccessorit)<<":"<<(*fingerSuccessorit)<<endl;
                    if ((*fingerNodeit)>(*fingerSuccessorit))
                    {
						cout<<"fingerNodeit)>(*fingerSuccessorit"<<endl;
						*fingerSuccessorit=tmpID;
						*successorIPListit=tmpIP;
						//fingerSuccessorList.erase(fingerSuccessorit);
						//successorIPList.erase(successorIPListit);
						
						//fingerSuccessorList.insert(fingerSuccessorit,tmpID);
						//successorIPList.insert(successorIPListit,tmpIP);
                    }
					else 
					{
						cout<<"else~~~"<<endl;
						if(tmpID<(*fingerSuccessorit))
						{
							cout<<tmpID<<","<<(*fingerSuccessorit)<<endl;
							cout<<"final change here??"<<endl;
							*fingerSuccessorit=tmpID;
							*successorIPListit=tmpIP;
							//fingerSuccessorList.erase(fingerSuccessorit);
							//successorIPList.erase(successorIPListit);
							
							//fingerSuccessorList.insert(fingerSuccessorit,tmpID);
							//successorIPList.insert(successorIPListit,tmpIP);
						}
					}
					//It is bigger than the (i-1) column
					//But if it's the first node...
				}
				//i++;
			}
			
			//printFingerTable();
			
		}
	}
	 mtx.unlock();
}

void ChordService::printFingerTable()
{
	mtx.lock();
	cout<<"Finger Table: "<<endl;
	cout<<"Node: "<<ChordService::fingerNodeList.size()<<endl;
	cout<<"Successor: "<<ChordService::fingerSuccessorList.size()<<endl;
	cout<<"IP: "<<ChordService::successorIPList.size()<<endl;
	
	std::list<int>::iterator fingerNodeit = ChordService::fingerNodeList.begin();
	std::list<int>::iterator fingerSuccessorit = ChordService::fingerSuccessorList.begin();
	std::list<string>::iterator successorIPListit = ChordService::successorIPList.begin();

	for (;fingerNodeit!=ChordService::fingerNodeList.end()&&fingerSuccessorit!=ChordService::fingerSuccessorList.end()
		&&successorIPListit!=ChordService::successorIPList.end();
			++fingerNodeit,++fingerSuccessorit,++successorIPListit)
	{
		cout<<&(*fingerNodeit)<<":"<<*fingerNodeit<<" "
			<<&(*fingerSuccessorit)<<":"<<*fingerSuccessorit<<" "
			<<&(*successorIPListit)<<":"<<*successorIPListit<<endl;
	}
	mtx.unlock();
	
}

void ChordService::setSystemParam()
{
	/*
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
    servaddr.sin_port=htons(9999);

    int bval = bind(client_sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    if(bval==-1)
    {
        printf("Bind failed\n");
        exit(EXIT_FAILURE);
    }
    */
}

void ChordService::sendRequestToServer(string receiverIP,string key, string value, string clientIP, string initNode,string operation)
{
	cout<<"sendRequestToServer "<<receiverIP<<" "<<key<<" "<<value<<" "<<clientIP<<" "<<initNode<<endl;
	
    string lengthReport = std::to_string(operation.length())+","+std::to_string(initNode.length())+","+to_string(key.length())+","
        +to_string(value.length())+","+to_string(clientIP.length())+",";

	string msgBuffer=lengthReport+operation+initNode+key+value+clientIP;

    int client_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(client_sockfd == -1)
    {
        printf("Could not create socket \n");
        exit(EXIT_FAILURE);
    }

	struct sockaddr_in receiverAddr;

    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(9999);
    cout<<"msgBuffer "<<msgBuffer<<endl;

    if(inet_aton(receiverIP.c_str(), &receiverAddr.sin_addr) == 0)
	{
        cerr<<"INET_ATON Failed\n"<<endl;
        exit(1);
    }
    if(sendto(client_sockfd, msgBuffer.c_str(), msgBuffer.length(), 0,
                (struct sockaddr*) &receiverAddr, sizeof(receiverAddr)) == -1)
    {

        cerr<<"Failed to send to "<<receiverIP<<" with data "<<msgBuffer<<endl;
    }
    else
	{
        cerr<<"Successfully send to "<<receiverIP<<" with data "<<msgBuffer<<endl;
    }
	close(client_sockfd);
	

}

int ChordService::calculateLength(int node,int key,int initNode)
{
	int length;
	/*
	if(node > key)
	{
		length=(node-key)<(65535-node+key)?(node-key):(65535-node+key);
	}
	else
	{
		length=(key-node)<(65535-key+node)?(key-node):(65535-key+node);
	}
	*/

	//Just clockwise, or else it will be a mess...

	cout<<"node "<<node<<" key "<<key<<endl;;

	if(node>key)
	{
		length=node-key;	
	}
	else
	{
		length=key-node;
	}
	cout<<"length: "<<length<<endl;
	return length;
}
//request should be sent here
//Return 0 failed, 1 success
int ChordService::lookupFingerTable(int thekey, string& theIP, int initNode)
{
	cout<<"Begin to find finger table "<<thekey<<" "<<initNode<<endl;
	std::list<int>::iterator fingerSuccessorit=ChordService::fingerSuccessorList.begin();
    std::list<string>::iterator successorIPListit=ChordService::successorIPList.begin();

	int localID = getLocalNode()->getHashID();
	if(localID==thekey)
	{
		return 1;
	}

	if(localID>thekey && initNode<thekey)
	{
		return 1;
	}

	if(localID>thekey && localID<initNode)
	{
		return 1;
	}
	
	int myLength;
	//First node should only go clockwise.
	if(localID>thekey)
	{
		myLength=65535-localID+thekey;
	}
	else
	{
		myLength=thekey-localID;
	}
	
	//int myLength = calculateLength(localID,thekey,initNode);
	cout<<"myLength:"<<myLength<<endl;
	
	int tmpID;
	
	int successorLength;

	int smallestLength=myLength;
	
	while(fingerSuccessorit!=ChordService::fingerSuccessorList.end()
		&&successorIPListit!=ChordService::successorIPList.end())
	{
		successorLength=calculateLength((*fingerSuccessorit),thekey,initNode);
		if(smallestLength>=successorLength)
		{
			smallestLength=successorLength;
			theIP=*successorIPListit;
			tmpID=*fingerSuccessorit;
		}
		if((*fingerSuccessorit)>initNode)
		{
			//One cycle... The IP with smallest length has been recorded
			//return 0;
		}
		fingerSuccessorit++;
		successorIPListit++;
	}

	if(myLength<=smallestLength)
	{
		cout<<"It's myself ~~~~~~"<<myLength<<" "<<smallestLength<<endl;
		return 1;
	}
	
	cout<<"For this node:"<<endl;
	cout<<"FingerTable: "<<endl;
	printFingerTable();
	cout<<"Own ID: "<<localID<<endl;
	cout<<"The shortest is "<<tmpID<<" "<<theIP<<endl;
	if(localID==tmpID)
	{
		return 1;
	}
	
	return 0;
}
/*
void ChordService::getFileList(std::list<string> &out)
{
	DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(myDirectory);
    while ((ent = readdir(dir)) != NULL) {
    	const string file_name = ent->d_name;
    	const string full_file_name = directory + "/" + file_name;

    	if (file_name[0] == '.')
    		continue;

    	if (stat(full_file_name.c_str(), &st) == -1)
    		continue;

    	const bool is_directory = (st.st_mode & S_IFDIR) != 0;

    	if (!is_directory)
    		continue;

    	out.push_back(full_file_name);
    }
    closedir(dir);
}
*/

void ChordService::setupCache()
{
	/*
	list<string> aFileList;
	getFileList(aFileList);
	for (std::list<int>::iterator it = mylist.begin() ; it != mylist.end(); ++it)
	{
		readFile((*it).c_str());
	}
	*/
	
}

void ChordService::readFile(const char* fileName)
{
	/*
	cout<<"Begin to read file: "<<readFile<<endl;
	ofstream o_file;  
	ifstream i_file;  
	string out_text;  
	
	o_file.open(fileName);  
	for (int i = 1; i < = 10; i++)  
	{  
	
	}  
	o_file.close();  
	//read
	i_file.open(filename);  
	if (i_file.is_open())  
	{  
	while (i_file.good())  
	{  
	i_file >> out_text; //  
	cout < <  out_text < <  endl;
	 
	}  
	}  
	else  
	
	i_file.close();  
	system("PAUSE");  
	return 0;
	*/
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
	std::map<int,string> mymap;
	//pthread_t thread;
	//int rc = pthread_create(&thread, NULL, receiveReply, mymap);
    //cout<<"Begin to sleep"<<endl;
	//sleep(5);
    //cout<<"After sleep"<<endl;
	//pthread_exit(NULL);
    //cout<<"Kill the thread"<<endl;

	myService->receiveReply(&mymap);

	cout<<"Before ~~~~~"<<endl;
	myService->printFingerTable();
	
	myService->buildFingerTable(&mymap);

	cout<<"After ~~~~~"<<endl;
	myService->printFingerTable();

	mymap.clear();


	//Initialization is finished.

	//Define the socket
	//ChordSocket: Listen the 10000 port for broadcast.
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

	
	//master = read_fds;

	int activity;
	
	struct timeval selectWaitTimer;
	

	while (1)
	{
		//This machine listens the port 10000, where broadcast of the new node will be received.
		//Then send its id and update its own fingertable.
		//It's based on the asumption that each node is added one by one.
	
		//master=read_fds;
		memcpy(&master, &read_fds, sizeof(read_fds));
		activity = select( fdmax + 1 , &master , NULL , NULL , NULL);
		if ((activity < 0) && (errno!=EINTR)) 
        {
            cerr<<"select error "<<activity<<endl;
        }
		else
		{
			for(int i = 0; i<=fdmax; i++)
			{
				if(FD_ISSET(i, &master))
				{
					if(i == chordSocket)
					{
						
						list<int> tmpList=ChordService::fingerSuccessorList;
						cout<<"copy~~~~~~~~~~~~"<<endl;
						for (std::list<int>::iterator it=tmpList.begin(); it != tmpList.end(); ++it)
    								cout << ' ' << *it<<" ";
						cout<<endl;
						
						cout<<"Received broadcast message"<<endl;
						struct sockaddr_in cliaddr;
						socklen_t cli_addr_len;
						cli_addr_len = sizeof(cliaddr);
	                    char buf[1024] = {0};
	                    int newfd =recvfrom(chordSocket, buf, 1024, 0, (struct sockaddr *)&cliaddr, &cli_addr_len);              
	                    cout<<buf<<endl;
	               	   	int aID = atoi(buf);
	               	    cout<<"Recieve the broadcast from id: "<< aID <<endl;
	               	    char ipstr[INET6_ADDRSTRLEN];

	               	    string aIP=inet_ntoa(cliaddr.sin_addr);
	                    cout<<"From IP: "<<aIP<<endl;
						
						std::map<int,string> tmpMap;
						tmpMap.insert(std::pair<int,string>(aID,aIP));

						cout<<"Before ~~~~~"<<endl;
						myService->printFingerTable();

						myService->buildFingerTable(&tmpMap);
						tmpMap.clear();

						cout<<"After ~~~~~"<<endl;
						myService->printFingerTable();
						

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
						//close(newfd);
						
					}
					else if (i == clientSocket)
					{
	                    
						cout<<"Receive request from client"<<endl;
						
	                    char* maxMessage = new char[1024];
	                    struct sockaddr_in cliaddr;
						socklen_t cli_addr_len;
						cli_addr_len = sizeof(cliaddr);
	                        
	                    int recvRet = 0;

	                    recvRet = recvfrom(clientSocket, maxMessage, 1024,
	                                0, (struct sockaddr*) &cliaddr, &cli_addr_len);

	                    if(recvRet > 0)
						{
							 string data = maxMessage;
							 cout<<"The raw data received is: "<<data<<endl;
							 
                             //Find the length report first
                             int comma=data.find(',');
                             int operation_length = atoi(data.substr(0,comma).c_str());
                             data.erase(0,comma+1);
							 
                             comma=data.find(',');
                             int initNode_length = atoi(data.substr(0,comma).c_str());
                             data.erase(0,comma+1);

                             comma=data.find(',');
                             int key_length=atoi(data.substr(0,comma).c_str());
                             data.erase(0,comma+1);

                             comma=data.find(',');
                             int value_length=atoi(data.substr(0,comma).c_str());
                             data.erase(0,comma+1);

                             comma=data.find(',');
                             int clientIP_length=atoi(data.substr(0,comma).c_str());
                             data.erase(0,comma+1);

                             cout<<"The pure buffer is: "<<data<<endl;
						 
							 //long len=sizeof(ClientRequest);
                             int len=0;
							 string operation = data.substr(len,operation_length);

							 len+=operation_length;
							 string initNode = data.substr(len,initNode_length);
							 
							 len += initNode_length;
							 string key = data.substr(len,key_length);

							 len += key_length;
							 string value = data.substr(len,value_length);

							 len +=value_length;
							 string clientIP = data.substr(len,clientIP_length);
							 
							 int theHash = myService->getLocalNode()->buildHashID(key);
							 
							 cout<<"Message from client:"<<endl;
							 cout<<"Operation is: "<<operation<<endl;
							 cout<<"Key: "<<key<<endl;
							 cout<<"Key hash: "<<theHash<<endl;
							 cout<<"Value: "<<value<<endl;							 
							 cout<<"Client IP: "<<clientIP<<endl;

							 string theNextNodeIP;
							 
							 if (initNode=="65536")
							 {
								initNode = std::to_string(myService->getLocalNode()->getHashID());
							 }
							 int result = myService->lookupFingerTable(theHash,theNextNodeIP,atoi(initNode.c_str()));
							 if(result==1)
							 {
							 	if (operation == "0")
								{
								 	//store data
								 	string aKey=std::to_string(theHash);
								 	myService->getLocalNode()->storeData(aKey,value);
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
										if(inet_aton(clientIP.c_str(), &cliaddr.sin_addr) == 0)
										{
									        cerr<<"INET_ATON Failed\n"<<endl;
									        exit(1);
									    }
				                        if ((numbytes=sendto(sendfd, result.c_str(), result.length(), 0,
				                                                (struct sockaddr *)&cliaddr, sizeof cliaddr)) == -1) 
				                        {
				                            cerr<<"send error"<<endl;
				                            exit(1);
				                        }
				                        else
				                        {

				                        }
										close(sendfd);
									}
									close(recvRet);
								}
								else if (operation=="1")
								{
								 	//Get data
								 	string aKey=std::to_string(theHash);
									list<string> valueResult;
								 	myService->getLocalNode()->getData(aKey,valueResult);
									//Get the result, seperate them with 'I'. A bad solution.
									string result="Data:";
									for (list<string>::iterator it=valueResult.begin(); it != valueResult.end(); ++it)
									{
										result=result+(*it)+"|";
									}
									cout<<"Get raw result is "<<result<<endl;
									
								 	//send back to client 	
									cout<<"Send successful response message"<<endl;
									if (!fork()) 
									{ // this is the child process
										//close(sockfd); // child doesn't need the listener
					                    int numbytes;
					                    int sendfd;
					                    if ((sendfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
					                    {
					                         cerr<<"socket error"<<endl;
					                         exit(1);
					                     }
					                    cliaddr.sin_port = htons(9999);
										if(inet_aton(clientIP.c_str(), &cliaddr.sin_addr) == 0)
										{
									        cerr<<"INET_ATON Failed\n"<<endl;
									        exit(1);
									    }
					                       if ((numbytes=sendto(sendfd, result.c_str(), result.length(), 0,
					                                               (struct sockaddr *)&cliaddr, sizeof cliaddr)) == -1) 
					                       {
					                           cerr<<"send error"<<endl;
					                           exit(1);
					                       }
					                       else
					                       {
					                        }
										close(sendfd);
									}
									close(recvRet);
								 }
								else if (operation=="2")
								{
								 	//delete data
								 	string aKey=std::to_string(theHash);
									list<string> valueResult;
								 	int res = myService->getLocalNode()->deleteData(aKey);
									cout<<"Delete result is "<<res<<endl;
									
									//send back to client 	
									cout<<"Send successful response message"<<endl;
									string result = to_string(res);
									if (!fork()) 
									{ // this is the child process
										//close(sockfd); // child doesn't need the listener
					                    int numbytes;
					                    int sendfd;
					                    if ((sendfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
					                    {
					                         cerr<<"socket error"<<endl;
					                         exit(1);
					                     }
					                    cliaddr.sin_port = htons(9999);
										if(inet_aton(clientIP.c_str(), &cliaddr.sin_addr) == 0)
										{
									        cerr<<"INET_ATON Failed\n"<<endl;
									        exit(1);
									    }
					                    if ((numbytes=sendto(sendfd, result.c_str(), result.length(), 0,
					                             (struct sockaddr *)&cliaddr, sizeof cliaddr)) == -1) 
					                    {
					                        cerr<<"send error"<<endl;
					                        exit(1);
					                    }
					                    else
					                    {
					                    }
										close(sendfd);
									}
									close(recvRet);
								 }
							 }
							 else
							 {
							 	//send to the next node.
							 	cout<<"Needs to send to find again: "<<theNextNodeIP<<endl;
							 	myService->sendRequestToServer(theNextNodeIP,key, value, clientIP, initNode,operation);
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

