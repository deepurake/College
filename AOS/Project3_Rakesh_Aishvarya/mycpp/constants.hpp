
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <curl/curl.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>

using namespace std;



enum scheme {RANDOM,FIFO,LRU,NONE};

//#define debug
#define hash_size 10