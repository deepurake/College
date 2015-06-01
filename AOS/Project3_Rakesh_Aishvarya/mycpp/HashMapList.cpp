
#include "HashMapList.hpp"

int min_counter = -1;
int curr_counter = 0;
int hash(const char *str)
{
    int h = 0;
    while (*str)
       h = h << 1 ^ *str++;
    return h%hash_size;
}
int get_index(string url)
{
	return hash(url.c_str());

}
HashMapList::HashMapList()
{
	this->my_scheme = RANDOM;
	for(int i = 0; i<hash_size;i++)
	{
		this->age_array[i] = -1;
		this->count[i] = 0;
	}
	this->cache_size = 0;
	this->total_count = 0;
}
HashMapList::HashMapList(scheme my_scheme,int max_size)
{
	this->my_scheme = my_scheme;
	this->max_size = max_size;
	for(int i = 0; i<hash_size;i++)
	{
		this->age_array[i] = -1;
		this->count[i] = 0;
	}
	this->cache_size = 0;
	this->total_count = 0;
}

void HashMapList::insert(char* val, string url)
{
	if(this->my_scheme == NONE)
	{
		return;
	}
	this->cache_size += strlen(val);
#ifdef debug
	cout << "cache size" << cache_size << endl;
#endif
	while(this->cache_size > this->max_size)
	{
#ifdef debug
		cout << "cache filled up" << endl;
#endif
		this->free();
	}
	int index = get_index(url);
#ifdef debug
	cout << "Storing in "<<index << endl;
#endif
	data[index].insert(url,val,curr_counter);
	this->total_count++;
	this->count[index]++;
	if(min_counter == -1)
	{
		min_counter = curr_counter;
	}
	curr_counter++;
}
void HashMapList::free()
{
#ifdef debug
	cout << "freeing something " << min_counter<<endl;
#endif
	if(this->my_scheme == NONE)
	{
		return;
	}
	switch	(this->my_scheme)
	{
		case RANDOM:
		{
			int idx = rand()%this->total_count;
			int i;
			for(i = 0;i<hash_size;i++)
			{
				if(idx < this->count[i]) break;
				idx -= this->count[i];
			}
			int temp = this->data[i].remove(idx);
			this->cache_size -= temp;
#ifdef debug
			cout << "freed " << temp << endl;
#endif
			this->total_count--;
			this->count[i]--;
			break;
		}
		case FIFO:
		case LRU:
		{
			int diff = -1;
			bool flag = true;
			int i;
			int count = 0;
			for(i = 0; i<hash_size;i++)
			{
				if(flag && (this->data[i].get_min_counter() == min_counter))
				{
					this->cache_size -= this->data[i].remove_head();
					flag = false;
				}
				if(this->data[i].get_min_counter() != -1)
				{
					count ++;
					int mydiff = this->data[i].get_min_counter() - min_counter;
					if(diff  == -1)
					{
						diff = mydiff;
					}
					else if(mydiff < diff)
					{
						diff = mydiff;
					}
				}
			}
			if(count == 0)
			{
				min_counter = -1;
			}
			min_counter = min_counter+diff;
			break;
		}
		default:
			cout << "Error, unkown cachng policy " << this->my_scheme << endl;
			return;
			//do nothing
	}
}
bool HashMapList::fetch(string& _return, string url)
{
	if(this->my_scheme == NONE)
	{
		return false;
	}
	int index = get_index(url);
#ifdef debug
	cout << "getting from " <<index << endl;
#endif
	char* val;
	if(this->my_scheme == LRU)
	{
		val = this->data[index].get_val_lru(url,curr_counter++);
	}
	else
	{
		val = this->data[index].get_val(url);
	}
	if (val == NULL)
	{
		return false;
	}
	_return = string(val);
	return true;
}