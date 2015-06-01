#include <iostream>
#include "LinkedList.hpp"

using namespace std;

class List
{
	string url;
	char * val;
	List* next;
	int counter;
public:
	List(string url, char * val,int counter)
	{
		this->url = url;
		this->val = val;
		this->next = NULL;
		this->counter = counter;
	}
	void insert(string url,char * val,int counter)
	{
		if(this->next == NULL)
		{
			this->next = new List(url,val,counter);
		}
		else
		{
			this->next->insert(url,val,counter);
		}
	}
	bool search(string url)
	{
		if(this->url.compare(url) == 0)
		{
			return true;
		}
		else if(this->next != NULL)
		{
			return this->next->search(url);
		}
		return false;
	}
	char * get_val(string url)
	{
		if(this->url.compare(url) == 0)
		{
			return this->get_val();
		}
		else if(this->next != NULL)
		{
			return this->next->get_val(url);
		}
		return NULL;
	}
	void remove(string url)
	{
		if(this->next == NULL)
		{
			return;
		}
		if(this->next->url.compare(url) == 0)
		{
			List* temp = this->next->next;
			delete this->next->val;
			delete this->next;
			this->next = temp;
		}
		else
		{
			return this->next->remove(url);
		}
	}
	void remove_node(string url)
	{
		if(this->next == NULL)
		{
			return;
		}
		if(this->next->url.compare(url) == 0)
		{
			List* temp = this->next->next;
			delete this->next;
			this->next = temp;
		}
		else
		{
			return this->next->remove_node(url);
		}
	}
	int remove(int idx)
	{
		int return_val = 0;
		if(idx == 0)
		{
			return 0;
		}
		if(idx == 1)
		{
			List* temp = this->next->next;
			return_val = strlen(this->next->val);
#ifdef debug
			cout << "removing " << this->next->url << endl;
#endif
			delete this->next->val;
			delete this->next;
			this->next = temp;
			return return_val;
		}
		else
		{
			return this->next->remove(idx-1);
		}
	}
	List* get_next()
	{
		return this->next;
	}
	string get_url()
	{
		return this->url;
	}
	char * get_val()
	{
		return this->val;
	}
	int get_counter()
	{
		return counter;
	}
};

LinkedList::LinkedList()
{
	this->head = NULL;
	this->min_counter = -1;
	// do nothing
}
LinkedList::LinkedList(string url, char * val,int counter)
{
	this->head = new List(url,val,counter);
}
void LinkedList::insert(string url, char * val,int counter)
{
	if(this->head == NULL)
	{
		this->head = new List(url,val,counter);
		this->min_counter = counter;
	}
	else
	{
		this->head->insert(url,val,counter);
	}
}
bool LinkedList::search(string url)
{
	if(this->head == NULL)
	{
		return false;
	}
	return this->head->search(url);
}
char * LinkedList::get_val(string url)
{
	if(this->head == NULL)
	{
		return NULL;
	}
	return this->head->get_val(url);
}
char * LinkedList::get_val_lru(string url,int counter)
{
	if(this->head == NULL)
	{
		return NULL;
	}
	char* val = this->head->get_val(url);
	if(val == NULL)
		return val;
#ifdef debug
	cout << "length before " << strlen(val) << endl;
#endif
	this->remove_node(url);
#ifdef debug
	cout << "length after " << strlen(val) << endl;
#endif
	this->insert(url,val,counter);
	if(this->head!=NULL)
	{
		this->min_counter = head->get_counter();
	}
	return val;
}
void LinkedList::remove_node(string url)
{
	if(this->head == NULL)
	{
		return;
	}
	if(head->get_url().compare(url)==0)
	{
		List* temp = head->get_next();
		delete head;
		head = temp;
	}
	else
	{
		head->remove_node(url);
	}
}
void LinkedList::remove(string url)
{
	delete this->get_val(url);
	this->remove_node(url);
}
int LinkedList::remove(int idx)
{
	int return_val = 0;
	if(idx==0)
	{
		List* temp = head->get_next();
		return_val += strlen(head->get_val());
#ifdef debug
		cout << "removing " << head->get_url() << endl;
#endif
		delete head->get_val();
		delete head;
		head = temp;
	}
	else
	{
		return head->remove(idx);
	}
	return return_val;
}

int LinkedList::remove_head()
{
	List* temp = head->get_next();
	int return_val = strlen(head->get_val());
#ifdef debug
	cout << "removing " << head->get_url() << endl;
#endif
	delete head->get_val();
	delete head;
	head = temp;
	if(head!=NULL)
	{
		this->min_counter = head->get_counter();
	}
	else
	{
		this->min_counter = -1;
	}

	return return_val;
}

int LinkedList::get_min_counter()
{
	return this->min_counter;
}