#include "ChordService.h"

ChordService::ChordService():myHostName(""),myIP("")
{
	myHostName=NULL;
	struct hostent* he;
	gethostname(myHostName.c_str(), 100);
	cout<<"My host name is: "<<myHostName<<endl;
	if((he = gethostbyname(selfName.c_str())) == NULL){
        cerr<<"Unable to get the ip address of the host:"<<myHostName.c_str()<<endl;;
        exit(-1);
    }
	struct in_addr **ipAddr = (struct in_addr**)he->h_addr_list;
	myIP = inet_ntoa(*ipAddr[0]);
}

ChordService::~ChordService()
{
}


int main(int argc, char* argv[])
{
	
	
	int n, fd;
    socklen_t cli_addr_len;
    char buf[1024] = {0};
    struct sockaddr_in servaddr, cliaddr;
 
    if((fd = socket(af_inet, sock_dgram, 0)) == -1)
    {
        printf("socket error!\n");
        exit(0);
    }
 
    servaddr.sin_family = af_inet;
    servaddr.sin_addr.s_addr = htonl(inaddr_any);
    servaddr.sin_port = htons(2345);
 
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



    return 0;
}

