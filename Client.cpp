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
#include <fstream>

using namespace std;

const int CLIENT_PORT = 9999;

const int MAX_MSG_SIZE = 1024;
const int SUCCESS = 1;
const int FAILED_DUPLICATED = 2;
const int FAILED = 0;

int client_sockfd;



int recieveMessageFromServer(list<string>& result)
{
	time_t startTime;
	time(&startTime);
    char* maxMessage = new char[MAX_MSG_SIZE];
	
    while(1)
    {
        //char* maxMessage = new char[MAX_MSG_SIZE];
        struct sockaddr_in senderProcAddrUDP;

        memset((char*)&senderProcAddrUDP, 0, sizeof(senderProcAddrUDP));
        socklen_t senderLenUDP = sizeof(senderProcAddrUDP);

        int recvRet = 0;      

        recvRet = recvfrom(client_sockfd, maxMessage, MAX_MSG_SIZE,
                MSG_DONTWAIT, (struct sockaddr*) &senderProcAddrUDP, &senderLenUDP);
		

        if(recvRet > 0)
        {
            string data = maxMessage;
			if (data.length()==1)
			{
				if (data[0]=='1')
				{
					return SUCCESS;
				}
			}
			else
			{
				if (data.substr(0,5)=="Data:")
				{
                    data.erase(0,5);
					string value;
					
					for (string::iterator it=data.begin(); it!=data.end(); ++it)
					{
						if ((*it)!='|')
						{
							value.append(1,(*it));
						}
						else
						{
							result.push_back(value);
							value.clear();
						}
					}
    
					/*
					 int found=data.find('|');//ugly method... but no choice now
					while (found!=std::string::npos)
					{
						string value = data.substr(0,found);
						result.push_back(value);
						data.erase(0,found+1);
					}
					*/
					return SUCCESS;
				}
				else
				{
					return atoi(data.c_str());
				}
			}
			
		}
		else
		{
	        
	    }
        memset(maxMessage, 0, 1024);
		
		
		time_t currentTime;
	    time(&currentTime);
		if (difftime(currentTime,startTime)>10)
	    {
	       cout<<"Timer expired. Please Retry " << endl;
		   return FAILED;
	    }
    }
    delete maxMessage;
    maxMessage=NULL;
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
        printf("Bind failed for put socket\n");
        exit(EXIT_FAILURE);
    }
    
}

//0:out 1:get 2:delete
void sendRequestToServer(string receiverIP,string key, string value, string clientIP,string operation)
{
    //cout<<"sendRequestToServer "<<receiverIP<<" "<<key<<" "<<value<<" "<<clientIP<<" "<<operation<<endl;
	string initialNode_fake="65536";
	if (operation=="PUT")
	{
	    //ClientRequest req;
		
		string operation="0";
		
	    string lengthReport = std::to_string(operation.length())+","+std::to_string(initialNode_fake.length())+","+to_string(key.length())+","
	        +to_string(value.length())+","+to_string(clientIP.length())+",";
		
		string msgBuffer=lengthReport+operation+initialNode_fake+key+value+clientIP;
		struct sockaddr_in receiverAddr;

	    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
	    receiverAddr.sin_family = AF_INET;
	    receiverAddr.sin_port = htons(CLIENT_PORT);
	    //cout<<"msgBuffer "<<msgBuffer<<endl;

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
		string operation="1";
		string value = "N/A";
		string lengthReport = std::to_string(operation.length())+","+std::to_string(initialNode_fake.length())+","+to_string(key.length())+","
	        +to_string(value.length())+","+to_string(clientIP.length())+",";
		string msgBuffer=lengthReport+operation+initialNode_fake+key+value+clientIP;
		struct sockaddr_in receiverAddr;

	    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
	    receiverAddr.sin_family = AF_INET;
	    receiverAddr.sin_port = htons(CLIENT_PORT);
	    //cout<<"msgBuffer "<<msgBuffer<<endl;

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
	else if (operation=="DELETE")
	{
		string operation="2";
		string value = "N/A";
		string lengthReport = std::to_string(operation.length())+","+std::to_string(initialNode_fake.length())+","+to_string(key.length())+","
	        +to_string(value.length())+","+to_string(clientIP.length())+",";
		string msgBuffer=lengthReport+operation+initialNode_fake+key+value+clientIP;
		
		struct sockaddr_in receiverAddr;

	    memset((char*)&receiverAddr, 0, sizeof(receiverAddr));
	    receiverAddr.sin_family = AF_INET;
	    receiverAddr.sin_port = htons(CLIENT_PORT);
	    //cout<<"msgBuffer "<<msgBuffer<<endl;

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
			list<string> resultList;
            int result = recieveMessageFromServer(resultList);
                    
            if(result == 1)
            {
            	cout<<"Successfully put" << endl;
            }
			else
			{
				cout<<"Failed" << endl;
			}
		}
		else if(command=="1")
		{
			//Here I only use 98 world cup file
			//File fomat
			//0.0.0.0 - - [30/Apr/1998:22:00:02 +0000] "GET /images/home_intro.anim.gif HTTP/1.0" 200 60349
			//Here uses IP address as key, because it's the record from different address.
			cout<<"The file is under ~zyang/CS581/assignment_1/ChordTest/recreate_wc_day6_1.out"<<endl;
			cout<<"YOu have no choice to choose files in this version..."<<endl;
			cout<< "Please enter the server ip"<<endl;

            string serverIP;                    
			getline(cin,serverIP);

			int success=0;
			int failed=0;
			
			std::ifstream myfile("./recreate_wc_day6_1.out");
			string line;
			int i=0;
			if (myfile.is_open())
			{
				while ( getline (myfile,line)&&i<10000)
				{
					int pos=line.find("- -");
					string key = line.substr(0,pos-1);
					cout<<key<<endl;
					line.erase(0,pos+4);
					cout<<line<<endl;
					i++;
					string operation  = "PUT";                                    	
		            sendRequestToServer(serverIP,key,line,selfIP,operation);
					list<string> resultList;
		            int result = recieveMessageFromServer(resultList);
                    //int result=1;
		                    
		            if(result == 1)
		            {
		            	success++;
		            }
					else
					{
						failed++;
					}
				}
				myfile.close();
			}
			cout<<"Final result for recreate_wc_day6_1.out is:"<<endl;
			cout<<"Successful: "<<success<<endl;
			cout<<"Failed: "<<failed<<endl;
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
			list<string> resultList;
            int result = recieveMessageFromServer(resultList);
			if (result==SUCCESS)
			{
				cout<<"The value is:"<<endl;
				list<string>::iterator it;
				for (list<string>::iterator it=resultList.begin(); it != resultList.end(); ++it)
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
			list<string> resultList;
			int result = recieveMessageFromServer(resultList);
			if(result == SUCCESS)
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

