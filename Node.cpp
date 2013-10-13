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

using namespace std;

Node::Node()
    :myHostName(""),myIP(""),
    myBroadcastPort(10000),myReceivePort(10001),myClientPort(9999),myID(0)
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
	cout<<"My IP is: "<<myIP<<endl;
	myID = buildHashID();
	cout<<"My Hash ID is: "<<myID<<endl;
}

Node::~Node()
{
}

//One-at-a-Time hash
uint32_t Node::buildHashID()
{
	string nodeIP=getIP();
	//const char* p = nodeIP.c_str();
	uint32_t h = 0;
	int i;
    for ( i = 0; i < nodeIP.length(); i++ ) 
	{
		h += nodeIP[i];
        h += ( h << 10 );
        h ^= ( h >> 6 );
	}

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );

	h=h>>16;
	
	return h;
		
	/*
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
    */
}



