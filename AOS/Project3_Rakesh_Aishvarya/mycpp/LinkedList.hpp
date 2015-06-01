#include "constants.hpp"

class List;
class LinkedList
{
	List* head;
	int min_counter;
public:
	LinkedList(string url, char * val,int counter);
	LinkedList();
	void insert(string url, char * val,int counter);
	bool search(string url);
	char * get_val(string url);
	char * get_val_lru(string url,int counter);
	void remove(string url);
	void remove_node(string url);
	int remove(int idx);
	int remove_head();
	int get_min_counter();
};