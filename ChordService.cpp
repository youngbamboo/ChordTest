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

#include "ChordService.h"

using namespace std;

ChordService::ChordService():myHostName(""),myIP(""),myBroadcastPort(2345)
{
	struct hostent* he;
    char aName[100];
    gethostname(aName, 100);
    myHostName=aName;
	cout<<"My host name is: "<<myHostName<<endl;
	if((he = gethostbyname(myHostName.c_str())) == NULL){
        cerr<<"Unable to get the ip address of the host:"<<myHostName.c_str()<<endl;;
        exit(-1);
    }
	struct in_addr **ipAddr = (struct in_addr**)he->h_addr_list;
	myIP = inet_ntoa(*ipAddr[0]);
	myID = buildHashID();
}

ChordService::~ChordService()
{
}

string ChordService::buildHashID()
{
	string nodeIP=getIP();
	
    string digest;
    EVP_MD_CTX mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    int i;

    OpenSSL_add_all_digests();

    md = EVP_get_digestbyname("SHA1");

    if(!md){
        cerr<<"Unknown message digest"<<endl;
		exit(-1);
    }

    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, md, NULL);
    EVP_DigestUpdate(&mdctx, nodeIP.c_str(), nodeIP.length());
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);

    digest = reinterpret_cast<const char*>(md_value);
	cout<<"hash value length is: "<<digest.length()<<endl;

    return digest;
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
    their_addr.sin_port = htons(myService->getBroadcastPort()); // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
    if ((numbytes=sendto(sockfd, myService->getHashID().c_str(), myService->getHashID().length(), 0,
                    (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
    {
        perror("sendto");
        exit(1);
    }
    printf("sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));
    close(sockfd);

	//2.  Wait for the response.

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

