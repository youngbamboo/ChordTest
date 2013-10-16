#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <stdlib.h>

#define CLIENT_REQ  0
#define CLIENT_RESP 1
#define SERVER_REQ  2
#define SERVER_RES  3
#define LEAVE_ENTRIES_MSG   4
#define PING_REQ_TYPE 5
#define PING_RES_TYPE 6

typedef struct{

    int type; // 2/3 for server req/resp message
    char senderID[20];
    int numParameters; // Number of parameters for the command
    int* paramLenArray; // Length of each parameter
    char* parameters; // First parameter is the command name(or function name), second
    // parameter is IP address and then any other function parameters.
    // It might so happen that my message is forwarded to third node. So
    // now that node knows my IP and can directly send the response message
    // to me.
}command;

typedef struct{
    int type; // Always 1
    int numParameters; // Number of parameters
    int result; // 0 for failure and 1 for success
    int* paramLenArray;
    char* parameters;
}ClientResponse;

typedef struct{
	int initialNode_length;
	int key_length;
	int value_length;
	int clientIP_length;
	char* initialNode;
	char* key;
	char* value;
	char* client_IP;
}ClientRequest;

typedef struct{

    int type;

    //number of entries to be stored at the successor before leaving
    int nEntries;

    //length of predecessor's IP
    int lengthPredIP;

    //array of length of each key
    int* lengthKeys;

    //array of length of each value
    int* lengthValues;

    //array of keys
    char** keys;

    //array of values
    char** values;

    char* predIP;

}LeaveMsg;

typedef struct{

    int type;

    //successors IP length
    
    int lengthSuccIP;

    //successors IP
    char* succIP;

    //no need to send successors ID as it can be computed at other end
    
}LeaveMsgForPredecessor;

typedef struct{
    int type; // PING TYPE
}pingMsg;


#endif

