
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
#include "ClientMessage.h"

#define XINU 0
#define VM 1
#define debug 1
#define FILE_DNOT_EXIST " File does not exist on the cloud "
#define FILE_EXISTS  " File Exists on the cloud "
#define FILE_STORED  " File was stored on the cloud "
#define FILE_NOT_STORED " File could not be stored on the cloud. Try again as the ring is not formed yet. " 

#define ERROR " Unknown error "

#define FILE_DELETED " File deleted from cloud"
#define FILE_NOT_DELETED " File could not be deleted"
#define DATA_START " DATA : "
#define NO_FILES " No Files: "
#define LIST " LIST: "
#define ERROR_NO_FILES " No File: Ring is not Stabilized. Try again "
#define ERROR_SOME_FILES " Could retrieve some files. Cannot traverse entire ring. In stabilize phase "
#define CLIENT_WAIT_TIMER 2000

using namespace std;

const int CLIENT_PORT = 9999;
const int MAX_MSG_SIZE = 1024;
const int SUCCESS = 1;
const int FAILED_DUPLICATED = 2;
const int FAILED = 0;

int client_sockfd;




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

void setSystemParam()
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


void sendRequestToServer(string receiverIP,string key, string value, string clientIP)
{
    ClientRequest* req = new ClientRequest;
	string initialNode_fake="65536";
	
	req->key_length=key.length();
	req->value_length=value.length();
	req->initialNode_length=initialNode_fake.length();
	req->clientIP_length=clientIP.length();
	
	char* msgBuffer = NULL;
	long messageLen = 0;
	messageLen = sizeof(ClientRequest) +  key.length() + value.length()+ (sizeof uint_32) + clientIP.length();
	
	msgBuffer = new char[messageLen];
	memcpy(msgBuffer,req,sizeof(ClientRequest));
	memcpy(msgBuffer+sizeof(ClientRequest),initialNode_fake.c_str(),initialNode_fake.length());
	memcpy(msgBuffer+sizeof(ClientRequest)+initialNode_fake.length(),key.c_str(),key.length());
	memcpy(msgBuffer+sizeof(ClientRequest)+initialNode_fake.length()+key.length(),value,value.length());
	memcpy(msgBuffer+sizeof(ClientRequest)+key.length()+value.length()+initialNode_fake.length(),clientIP,clientIP.length());
	
	struct sockaddr_in receiverAddr;

    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(CLIENT_PORT);

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

    int command;

    setSystemParam();
    
    while(1)
    {
        cout<<"Please input: "<<endl;
        cout<<"0 - put key-value one by one"<<endl;
        cout<<"1 - put by file"<<endl;
        cout<<"2 - get by key"<<endl;
        cout<<"3 - delete by key"<<endl;
        
        cin >> command;

        switch(command)
        {
            case 0:
                {
                    cout<< "You selected put key-value one by one"<<endl;
                    cout<< "Please enter the key"<<endl;

                    string key;
                    cin>>key;

					cout<< "Please enter the value"<<endl;

                    string value;
                    cin>>value;

                    cout<< "Please enter the server ip"<<endl;

                    string serverIP;
                    cin>>serverIP;

                                    	
                    sendRequestToServer(serverIP,key,value,selfIP);
                    int result = recieveMessageFromServer();
                    
                    if(result == 1)
                    {
                        cout<<"Successfully put" << endl;
                    }
					else
					{
						cout<<"Failed" << endl;
					}
                    break;
                }             
            default:
                {
                    cout<<"Please select an option from the available choices"<<endl;
                }
        }
    }

}

