#include "LinkedList.hpp"


class HashMapList
{
	int max_size;
	int cache_size;
	int age_array[1024];
	int count[1024];
	int total_count;
	scheme my_scheme;
	LinkedList data[1024];
public:
	HashMapList();
	HashMapList(scheme my_scheme,int max_size);
	void free();
	void insert(char* val, string url);
	bool fetch(string& _return, string url);
};