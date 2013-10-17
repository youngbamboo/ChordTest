
/* Client is used to ping server for a file name or put, get, ls , exists
 * Usage: ./Client
 */

/* TODO: When client receives the output of the ls command then it will
 * look into all the file names and remove the duplicate one.
 */

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <list>

using namespace std;

const int CLIENT_PORT = 9999;
const int CLIENT_GETPORT = 8888;
const int CLIENT_DELETEPORT = 7777;
const int MAX_MSG_SIZE = 1024;
const int SUCCESS = 1;
const int FAILED_DUPLICATED = 2;
const int FAILED = 0;

int client_sockfd,clientget_sockfd,clientdelete_sockfd;




//Simple resone format
//1: put successful
//0: key-value pair is existed
//>2 means the numbers inserted form the file...
//Data:xxx means get the resone by get operation
int recieveMessageFromServer()
{
	time_t startTime;
	time(&startTime);
	
    while(1)
    {
        char* maxMessage = new char[MAX_MSG_SIZE];
        struct sockaddr_in senderProcAddrUDP;

        memset((char*)&senderProcAddrUDP, 0, sizeof(senderProcAddrUDP));
        socklen_t senderLenUDP = sizeof(senderProcAddrUDP);

        int recvRet = 0;      

        recvRet = recvfrom(client_sockfd, maxMessage, MAX_MSG_SIZE,
                MSG_DONTWAIT, (struct sockaddr*) &senderProcAddrUDP, &senderLenUDP);

        if(recvRet > 0)
        {
            string tmp = maxMessage;
			if (tmp.length()==1)
			{
				if (tmp[0]=='1')
				{
					return SUCCESS;
				}
				else if (tmp[0]=='2')
				{
					return FAILED_DUPLICATED;
				}
			}
			else
			{
				if (tmp.substr(0,5)=="Data:")
				{
                    //Needs to be taken care...
					return SUCCESS;
				}
				else
				{
					return atoi(tmp.c_str());
				}
			}
        }
		
		time_t currentTime;
	    time(&currentTime);
		if (difftime(currentTime,startTime)>5)
	    {
	       cout<<"Timer expired. Please Retry " << endl;
		   return FAILED;
	    }
    }
}

int recieveGetMessageFromServer(list<string>& result)
{
	time_t startTime;
	time(&startTime);
	
    while(1)
    {
        char* maxMessage = new char[MAX_MSG_SIZE];
        struct sockaddr_in senderProcAddrUDP;

        memset((char*)&senderProcAddrUDP, 0, sizeof(senderProcAddrUDP));
        socklen_t senderLenUDP = sizeof(senderProcAddrUDP);

        int recvRet = 0;      

        recvRet = recvfrom(clientget_sockfd, maxMessage, MAX_MSG_SIZE,
                MSG_DONTWAIT, (struct sockaddr*) &senderProcAddrUDP, &senderLenUDP);

        if(recvRet > 0)
        {
			string data=maxMessage;
			
			int found=data.find('|');//ugly method... but no choice now
			while (found!=std::string::npos)
			{
				int length = atoi(data.substr(0,found).c_str());
				string value = data.substr(0,length);
				result.push_back(value);
				data.erase(0,length+1);
			}
            
			return SUCCESS;
        }
		else
		{
			return FAILED;
		}
		
		time_t currentTime;
	    time(&currentTime);
		if (difftime(currentTime,startTime)>5)
	    {
	       cout<<"Timer expired. Please Retry " << endl;
		   return FAILED;
	    }
    }
}

int recieveDeleteMessageFromServer()
{
	time_t startTime;
	time(&startTime);
	
    while(1)
    {
        char* maxMessage = new char[MAX_MSG_SIZE];
        struct sockaddr_in senderProcAddrUDP;

        memset((char*)&senderProcAddrUDP, 0, sizeof(senderProcAddrUDP));
        socklen_t senderLenUDP = sizeof(senderProcAddrUDP);

        int recvRet = 0;      

        recvRet = recvfrom(clientdelete_sockfd, maxMessage, MAX_MSG_SIZE,
                MSG_DONTWAIT, (struct sockaddr*) &senderProcAddrUDP, &senderLenUDP);

        if(recvRet > 0)
        {
            string tmp = maxMessage;
			if (tmp.length()==1)
			{
				if (tmp[0]=='1')
				{
					return SUCCESS;
				}
			}
			else
			{
				cout<<"Unknown message: "<<maxMessage<<endl;
				return FAILED;
			}
        }
		
		time_t currentTime;
	    time(&currentTime);
		if (difftime(currentTime,startTime)>5)
	    {
	       cout<<"Timer expired. Please Retry " << endl;
		   return FAILED;
	    }
    }
}


void setSystemParam()
{
     //Socket variables for client
    int n;
    struct sockaddr_in servaddr,servaddr_get,servaddr_delete;
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

	bzero(&servaddr,sizeof(servaddr_get));
    servaddr_get.sin_family = AF_INET;
    servaddr_get.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr_get.sin_port=htons(CLIENT_GETPORT);

	bzero(&servaddr,sizeof(servaddr_delete));
    servaddr_delete.sin_family = AF_INET;
    servaddr_delete.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr_delete.sin_port=htons(CLIENT_DELETEPORT);

    int bval = bind(client_sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    if(bval==-1)
    {
        printf("Bind failed for put socket\n");
        exit(EXIT_FAILURE);
    }

	bval = bind(clientget_sockfd,(struct sockaddr *)&servaddr_get,sizeof(servaddr_get));

    if(bval==-1)
    {
        printf("Bind failed for get socket\n");
        exit(EXIT_FAILURE);
    }

	bval = bind(clientdelete_sockfd,(struct sockaddr *)&servaddr_delete,sizeof(servaddr_delete));

    if(bval==-1)
    {
        printf("Bind failed for delete socket\n");
        exit(EXIT_FAILURE);
    }

	
    
}


void sendRequestToServer(string receiverIP,string key, string value, string clientIP,string operation)
{
    cout<<"sendRequestToServer "<<receiverIP<<" "<<key<<" "<<value<<" "<<clientIP<<" "<<operation<<endl;
	if (operation=="PUT")
	{
	    //ClientRequest req;
		string initialNode_fake="65536";
		
	    string lengthReport = std::to_string(initialNode_fake.length())+","+to_string(key.length())+","
	        +to_string(value.length())+","+to_string(clientIP.length())+",";
		
		string msgBuffer=lengthReport+initialNode_fake+key+value+clientIP;
		struct sockaddr_in receiverAddr;

	    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
	    receiverAddr.sin_family = AF_INET;
	    receiverAddr.sin_port = htons(CLIENT_PORT);
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
	}
	else if (operation=="GET")
	{
		string initialNode_fake="65536";
		string lengthReport = std::to_string(initialNode_fake.length())+","+to_string(key.length())+","+to_string(clientIP.length())+",";
		string msgBuffer=lengthReport+initialNode_fake+key+clientIP;
		struct sockaddr_in receiverAddr;

	    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
	    receiverAddr.sin_family = AF_INET;
	    receiverAddr.sin_port = htons(CLIENT_GETPORT);
	    cout<<"msgBuffer "<<msgBuffer<<endl;

		if(inet_aton(receiverIP.c_str(), &receiverAddr.sin_addr) == 0)
		{
	        cerr<<"INET_ATON Failed\n"<<endl;
	        exit(1);
	    }
	    if(sendto(clientget_sockfd, msgBuffer.c_str(), msgBuffer.length(), 0,
	                (struct sockaddr*) &receiverAddr, sizeof(receiverAddr)) == -1)
	    {

	        cerr<<"Failed to send to "<<receiverIP<<" with data "<<msgBuffer<<endl;
	    }
	    else
		{
	        cerr<<"Successfully send to "<<receiverIP<<" with data "<<msgBuffer<<endl;
	    }
	}
	else if (operation=="DELETE")
	{
		string initialNode_fake="65536";
		string lengthReport = std::to_string(initialNode_fake.length())+","+to_string(key.length())+","+to_string(clientIP.length())+",";
		string msgBuffer=lengthReport+initialNode_fake+key+clientIP;
		
		struct sockaddr_in receiverAddr;

	    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
	    receiverAddr.sin_family = AF_INET;
	    receiverAddr.sin_port = htons(CLIENT_DELETEPORT);
	    cout<<"msgBuffer "<<msgBuffer<<endl;

		if(inet_aton(receiverIP.c_str(), &receiverAddr.sin_addr) == 0)
		{
	        cerr<<"INET_ATON Failed\n"<<endl;
	        exit(1);
	    }
	    if(sendto(clientdelete_sockfd, msgBuffer.c_str(), msgBuffer.length(), 0,
	                (struct sockaddr*) &receiverAddr, sizeof(receiverAddr)) == -1)
	    {

	        cerr<<"Failed to send to "<<receiverIP<<" with data "<<msgBuffer<<endl;
	    }
	    else
		{
	        cerr<<"Successfully send to "<<receiverIP<<" with data "<<msgBuffer<<endl;
	    }
	}
	else
	{
		cout<<"unknown operation"<<endl;
	}
    
}




int main(int argc,char **argv)
{

    // Get self hostname
    char myName[100];
    gethostname(myName, 100);
    string selfName;
    struct hostent* he;

    selfName = myName;

    if((he = gethostbyname(selfName.c_str())) == NULL){
        fprintf(stderr, "Unable to get the ip address of the host: %s\n",
                selfName.c_str());
        exit(1);
    }

    struct in_addr **ipAddr;

    string selfIP;

    //Store the ip address
    ipAddr = (struct in_addr**)he->h_addr_list;
    selfIP = inet_ntoa(*ipAddr[0]);

    cout<<"Self IP is: "<<selfIP<<endl;

    string command;

    setSystemParam();
    
    while(1)
    {
        cout<<"Please input: "<<endl;
        cout<<"0 - put key-value one by one"<<endl;
        cout<<"1 - put by file"<<endl;
        cout<<"2 - get by key"<<endl;
        cout<<"3 - delete by key"<<endl;
        
        getline(cin,command);

		if(command=="0")
		{
            cout<< "Please enter the key"<<endl;

            string key;
            getline(cin,key);

			cout<< "Please enter the value"<<endl;

            string value;
            getline(cin,value);

            cout<< "Please enter the server ip"<<endl;

            string serverIP;                    
			getline(cin,serverIP);
                    

			string operation  = "PUT";                                    	
            sendRequestToServer(serverIP,key,value,selfIP,operation);
            int result = recieveMessageFromServer();
                    
            if(result == 1)
            {
            	cout<<"Successfully put" << endl;
            }
			else
			{
				cout<<"Failed" << endl;
			}
		}
		else if(command=="2")
		{
			cout<< "Please enter the key"<<endl;
			string key;
            getline(cin,key);

			cout<< "Please enter the server ip"<<endl;
            string serverIP;                    
			getline(cin,serverIP);
			
			string value = " ";
			string operation = "GET";
			
			sendRequestToServer(serverIP,key,value,selfIP,operation);
			list<string> result;
            int result = recieveGetMessageFromServer(result);
			if (result==SUCCESS)
			{
				cout<<"The value is:"<<endl;
				std::list<string>::iterator it;
				for (std::liststring::iterator it=result.begin(); it != mylist.end(); ++it)
    				cout<<*it<<endl;;
			}
			
		}
		else if(command=="3")
		{
			cout<< "Please enter the key"<<endl;
			string key;
            getline(cin,key);

			cout<< "Please enter the server ip"<<endl;
            string serverIP;                    
			getline(cin,serverIP);

			string value = " ";
			string operation = "DELETE";
			
			sendRequestToServer(serverIP,key,value,selfIP,operation);
			int result = recieveDeleteMessageFromServer();
			if(result == 1)
            {
            	cout<<"Successfully deleted" << endl;
            }
			else
			{
				cout<<"Failed" << endl;
			}
		}        
    }

}

