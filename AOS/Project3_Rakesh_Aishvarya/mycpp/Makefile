GPP = g++
CURL_LIB = -lcurl
FLAGS = -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -Wall -g
INCLUDE_DIR = -I/usr/local/include/thrift -I../gen-cpp
LD_DIR = -L/usr/local/lib

PAGE_CLIENT_SRC = page_client.cpp
PAGE_CLIENT_SRC_UNIFORM = page_client_uniform.cpp
PAGE_SERVER_SRC = page_server.cpp

THRIFT_LIB = -lthrift
GEN_SRC = ../gen-cpp/page_constants.cpp ../gen-cpp/page_types.cpp ../gen-cpp/page.cpp

HELPERS = LinkedList.cpp HashMapList.cpp

PAGE_CLIENT_TARGET = pageclient
PAGE_CLIENT_TARGET_UNIFORM = pageclient_uniform
PAGE_SERVER_TARGET = pageserver

all: $(PAGE_CLIENT_TARGET) $(PAGE_SERVER_TARGET) $(PAGE_CLIENT_TARGET_UNIFORM)

clean:
	rm -f $(PAGE_CLIENT_TARGET) $(PAGE_CLIENT_TARGET_UNIFORM) $(PAGE_SERVER_TARGET) 

$(PAGE_CLIENT_TARGET): $(PAGE_CLIENT_SRC) $(GEN_SRC) 
	$(GPP) $(FLAGS) $(INCLUDE_DIR) $(LD_DIR) $^ -o $@ $(THRIFT_LIB)


$(PAGE_CLIENT_TARGET_UNIFORM): $(PAGE_CLIENT_SRC_UNIFORM) $(GEN_SRC) 
	$(GPP) $(FLAGS) $(INCLUDE_DIR) $(LD_DIR) $^ -o $@ $(THRIFT_LIB)

$(PAGE_SERVER_TARGET): $(HELPERS) $(PAGE_SERVER_SRC) $(GEN_SRC)
	$(GPP) $(FLAGS) $(INCLUDE_DIR) $(LD_DIR) $^ -o $@ $(THRIFT_LIB) $(CURL_LIB)

