
#include <iostream>
#include <list>
#include "node.h"

using namespace std;

void  nodehead_t::set_neighbours(int _neigh[])
{
	for (int i = 0; i < m_degree; i++) {
		node_t *neigh = new node_t(_neigh[i]);
		m_neigh_list.push_back(neigh);
	}
}
	
bool  nodehead_t::remove_vertex(int val)
{
	list<node_t *>::iterator it = m_neigh_list.begin();

	for (; it != m_neigh_list.end(); it++) {
			if (val == (*it)->id()){
			m_neigh_list.erase(it);
			// should free memory here.
			return true;
		}
	}
	cout << __func__ << " " << __LINE__ << endl;
	return false;
}

int  nodehead_t::erase_front()
{
		node_t* n = m_neigh_list.front();
		int val = n->id();
		m_neigh_list.pop_front();
		return val;
}


void  nodehead_t::dump() 
{
	for (list<node_t *>::iterator it = m_neigh_list.begin(); 
			it !=  m_neigh_list.end(); ++it) {
		node_t* node = *it;
		cout <<  "\t" << node->id() << endl;		
	}
}


void   nodehead_t::push_back(int val) 
{
	node_t *n = new node_t(val);
	m_neigh_list.push_back(n);
}
