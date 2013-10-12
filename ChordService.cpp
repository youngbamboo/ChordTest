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





int main(int argc, char* argv[])
{
	ChordService* myService = new ChordService();
	
	//1. Send the broadcast first with my HashID
    
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
    if ((numbytes=sendto(sockfd, myService->getLocalNode()->getHashID().c_str(), 
					 myService->getLocalNode()->getHashID().length(), 0,
                    (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
    {
        perror("sendto");
        exit(1);
    }
    printf("sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));
    close(sockfd);

	//2.  Wait for the response.
	//3.  First Node in the ring. There is no preNode. 
	//myService->setPreNode(NULL);
	//3.1 Build the finger table.
	
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

