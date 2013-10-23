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
#include <fstream> 
#include <dirent.h>
#include <mutex>
#include <sys/stat.h>


#include "Node.h"

using namespace std;
std::mutex cachemtx; 


Node::Node()
    :myHostName(""),myIP(""),
    myBroadcastPort(10000),myReceivePort(10001),myClientPort(9999),
    myID(0),myFile("/tmp/zyang/zyang.db")
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
	myID = buildHashID(myIP);
	cout<<"My Hash ID is: "<<myID<<endl;
/*
	if (mkdir("/tmp/zyang/",666)!=0)
	{
		perror("mkdir() error");
		exit(1);
	}
	*/
	//system("mkdir /tmp/zyang");
	int status = mkdir("/tmp/zyang", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	//Remove all files.
	system("exec rm /tmp/zyang/*");

	fstream f;
	f.open( myFile.c_str(), ios::out );
	f << flush;
	f.close();
	
}

Node::~Node()
{
}


int Node::buildHashID(const string theStr)
{

	int len = theStr.length();
	int hash = (int)(theStr[len-1]);
	for(int i = 0; i < len; i ++)
	{
		hash = (hash << 5) - hash + theStr[i];
	}
	hash = hash % (65536);
	if(hash < 0)
	{
		hash = hash *(-1);
	}
	return hash;
	
	
	//string nodeIP=getIP();
	//const char* p = nodeIP.c_str();
	/*
	int h = 0;
	int i;
    for ( i = 0; i < theStr.length(); i++ ) 
	{
		h += theStr[i];
        h += ( h << 10 );
        h ^= ( h >> 6 );
	}

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );

	h=h>>16;
	
	return (int)h;
*/
	/*
	string nodeIP=getIP();
	
    string digest;
    EVP_MD_CTX mdctx;
    const EVP_MD *md;
    int char md_value[EVP_MAX_MD_SIZE];
    int md_len;
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

int Node::storeData(const string theKey, const string theValue)
{	 
	cout<<"Node:Store Data: "<<theKey<<" "<<theValue<<endl;
	cachemtx.lock();
	//Write them to the file.
	string content = theKey+","+theValue;
	std::ofstream fs;
	fs.open(myFile.c_str(), ios::out | ios::app);
	if (fs.is_open())
	{
		fs <<content<<'\n';
		
		
	}
	else
	{
		cout<<"Open file error"<<endl;
		return 0;		
	}
	//Cache it!
	myCacheMape.insert( std::pair<string,string>(theKey,theValue) );
    cout<<"After insert: size is: "<<myCacheMape.size()<<endl;
    for (std::multimap<string,string>::iterator it=myCacheMape.begin(); it!=myCacheMape.end(); ++it)
            std::cout << (*it).first << " => " << (*it).second << '\n';
	fs.close();
	cachemtx.unlock();
	return 1;
}

void Node::getData(const string theKey, list<string>& result)
{
	cachemtx.lock();
	/*
	std::pair <std::multimap<string,string>::iterator, std::multimap<string,string>::iterator> ret;
	ret = myCacheMape.equal_range(theKey);
	for (std::multimap<string,string>::iterator it=ret.first; it!=ret.second; ++it)
      result.push_back(it->second);
      */
    cout<<"Get: "<<myCacheMape.size()<<endl;
    for (std::multimap<string,string>::iterator it=myCacheMape.begin(); it!=myCacheMape.end(); ++it)
       std::cout << (*it).first << " => " << (*it).second << '\n';
    std::multimap<string,string>::iterator it;
	for (it=myCacheMape.begin(); it!=myCacheMape.end(); it++)
	{
		cout<<it->first<<endl;
		if(it->first==theKey)
		{
			 result.push_back(it->second);
		}
	}
	cachemtx.unlock();
}

int Node::deleteData(const string theKey)
{
	cachemtx.lock();
	//Delete in cache and rewrite the file

	system("exec rm /tmp/zyang/*");

	fstream f;
	f.open( myFile.c_str(), ios::out );
	f << flush;
	f.close();
	
	std::fstream fs;
	fs.open(myFile.c_str());
	if (fs.is_open())
	{
		myCacheMape.erase(theKey);
		std::multimap<string,string>::iterator it;
		for (it=myCacheMape.begin(); it!=myCacheMape.end(); ++it)
    		fs << (*it).first << "," << (*it).second << '\n';
	}
	else
	{
		cout<<"Open file error"<<endl;
		return 0;		
	}
	fs.close();
	cachemtx.unlock();
	return 1;
}





