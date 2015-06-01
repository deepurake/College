#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "../gen-cpp/page.h"
#include <fstream>

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;

using namespace  ::page;

void getBody(string url,shared_ptr<TTransport> transport,pageClient client)
{
  string body = "";
  try
  {
    transport->open();
    client.getPageBody(body, url);
    if(url.compare("terminate") == 0)
      cout << "terminating ..........." << body << endl;
    transport->close();
  }
  catch (TException& tx)
  {
    cout << "ERROR: " << tx.what() << endl;
  }
}

int main(int argc, char **argv) {
  int port = 9090;
  const char* IP = "localhost";
  if(argc == 3)
  {
    IP = argv[1];
    port = atoi(argv[2]);
  }
  string url_list[100];
  string url = "http://www.google.com";

  ifstream infile("url_list.txt");


  shared_ptr<TTransport> socket(new TSocket(IP, port));
  shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  pageClient client(protocol);
  int count = 0;
  while(getline(infile,url))
  {
    url_list[count++] = url;
  }
  count = 100;
  int iter = 5*count;
  for(int i = 0; i<iter; i++)
  {
    getBody(url_list[rand()%(count)],transport,client);
  }
  getBody("terminate",transport,client);
}


