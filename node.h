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




class nodehead_t {

	int m_id;
	int  m_degree;
	list<node_t *> m_neigh_list;

public:

	nodehead_t(int id,int nr_neigh){
		m_id = id;
		m_degree = nr_neigh;
	}
	
	list<node_t *> get_neigh_list() {
		return m_neigh_list;
	}

	int id() { return m_id;}
	int degree() { return m_neigh_list.size(); }
	void push_back(int val);
	void set_neighbours(int _neigh[]);
	bool remove_vertex(int val);
	int erase_front();

	bool empty() {
		return m_neigh_list.empty();
	}


	void dump();

};

#endif

