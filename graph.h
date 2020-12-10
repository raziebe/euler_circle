#ifndef __GRAPH_H__
#define  __GRAPH_H__

#include <list>

using namespace std;

class Graph {

	int m_nr_nodes;
	list<NodeHead_t*> m_headnode_list;
	list<int> m_circle_path;
	int m_graph_size;

private:
	void  update_graph_size(int sz) { m_graph_size += sz;}
	char* cJSON_readFile(char *fname);
	int   parseCoordStr(char *p,int Coords[]);
	int   DoDFS(int v);
	bool  RemoveVertexFromRow(int row,int val);

public:
	Graph() { m_graph_size = 0 ;}
	
	int   graph_size() { return m_graph_size;}
	int   init(char *json_name);
	void  add_HeadNode(NodeHead_t *hd);
	void  Dump();
	bool  hasEulerCircle();
	int   DFS();
};

#endif

