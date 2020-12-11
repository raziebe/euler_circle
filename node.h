#ifndef __NODE_H__
#define __NODE_H__


using namespace std;
#define MAX_NEIGH	10
#define MAX_VERTICES	100

#include <list>

class node_t {

	int m_id;

public:
	node_t(int id){
		m_id = id;
	}

	int id() { return m_id;}
};




class NodeHead_t {

	int m_id;
	int  m_degree;
	list<node_t *> m_neigh_list;
public:

	list<node_t *> get_neigh_list() {
		return m_neigh_list;
	}

	NodeHead_t(int id,int nr_neigh){
		m_id = id;
		m_degree = nr_neigh;
	}
	
	int id() { return m_id;}
	int degree() { return m_neigh_list.size(); }

	void set_neighbours(int _neigh[]){

		for (int i = 0; i < m_degree; i++) {
			node_t *neigh = new node_t(_neigh[i]);
			m_neigh_list.push_back(neigh);
		}
	}
	
	bool remove_vertex(int val){

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

	int erase_front()
	{
		node_t* n = m_neigh_list.front();
		int val = n->id();
		m_neigh_list.pop_front();
		return val;
	}

	bool empty() {
		return m_neigh_list.empty();
	}

	void push_back(int val){
		node_t *n = new node_t(val);
		m_neigh_list.push_back(n);
	}

	void dump() {

		for(list<node_t *>::iterator it = m_neigh_list.begin(); 
				it !=  m_neigh_list.end(); ++it) {

			node_t* node = *it;
			cout <<  "\t" << node->id() << endl;		
		}
	}


};

#endif

