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
	unsigned short T[256] = {49, 118,  63, 252,  13, 155, 114, 130, 137,  40, 210,  62, 219, 246, 136, 221,
           174, 106,  37, 227, 166,  25, 139,  19, 204, 212,  64, 176,  70,  11, 170,  58,
           146,  24, 123,  77, 184, 248, 108, 251,  43, 171,  12, 141, 126,  41,  95, 142,
           167,  46, 178, 235,  30,  75,  45, 208, 110, 230, 226,  50,  32, 112, 156, 180,
           205,  68, 202, 203,  82,   7, 247, 217, 223,  71, 116,  76,   6,  31, 194, 183,
            15, 102,  97, 215, 234, 240,  53, 119,  52,  47, 179,  99, 199,   8, 101,  35,
            65, 132, 154, 239, 148,  51, 216,  74,  93, 192,  42,  86, 165, 113,  89,  48,
           100, 195,  29, 211, 169,  38,  57, 214, 127, 117,  59,  39, 209,  88,   1, 134,
            92, 163,   0,  66, 237,  22, 164, 200,  85,   9, 190, 129, 111, 172, 231,  14,
           181, 206, 128,  23, 187,  73, 149, 193, 241, 236, 197, 159,  55, 125, 196,  60,
           161, 238, 245,  94,  87, 157, 122, 158, 115, 207,  17,  20, 145, 232, 107,  16,
            21, 185,  33, 225, 175, 253,  81, 182,  67, 243,  69, 220, 153,   5, 143,   3,
            26, 213, 147, 222, 105, 188, 229, 191,  72, 177, 250, 135, 152, 121, 218,  44,
           120, 140, 138,  28,  84, 186, 198, 131,  54,   2,  56,  78, 173, 151,  83,  27,
           255, 144, 249, 189, 104,   4, 168,  98, 162, 150, 254, 242, 109,  34, 133, 224,
           228,  79, 103, 201, 160,  90,  18,  61,  10, 233,  91,  80, 124,  96, 244,  36}
	string nodeIP=getIP();
	char* s=nodeIP.c_str();
	unsigned h;
	for (int c = *s++; c; c = *s++) 
	{
        h = T[h ^ (65535 & c)];
    }
	return h;
    /*
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
	*/	
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



