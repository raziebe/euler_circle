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




class NodeHead_t{

	int m_id;
	int  m_nr_neigh;

	list<node_t *> m_neigh_list;
public:

	list<node_t *> get_neigh_list() {
		return m_neigh_list;
	}

	NodeHead_t(int id,int nr_neigh){
		m_id = id;
		m_nr_neigh = nr_neigh;
	}
	
	int id() { return m_id;}
	int get_neigh_nr() { return m_neigh_list.size(); }

	void set_neigh(int _neigh[]){

		for (int i = 0; i < m_nr_neigh; i++) {
			node_t *neigh = new node_t(_neigh[i]);
			m_neigh_list.push_back(neigh);
		}
	}
	
	bool remove_vertex(int val){

		list<node_t *>::iterator it = m_neigh_list.begin();

		for (; it != m_neigh_list.end(); it++) {
			if (val == (*it)->id()){
				m_neigh_list.erase(it);
				return true;
			}
		}
		cout << __func__ << " " << __LINE__ << endl;
		return false;
	}

	int erase_front(){
		node_t* n = m_neigh_list.front();
		int val = n->id();
		m_neigh_list.pop_front();
		return val;
	}

	bool empty() {
		return m_neigh_list.empty();
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

