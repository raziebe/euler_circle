#ifndef __GRAPH_H__
#define  __GRAPH_H__

#include <list>

using namespace std;

class Graph {

	int m_nr_nodes;
	list<NodeHead_t*> m_headnode_list;
	list<int> m_circle_path;
	int m_directed_edges;

private:
	void  update_graph_size(int sz) { m_directed_edges += sz;}
	char* cJSON_readFile(char *fname);
	int   parseCoordStr(char *p,int Coords[]);
	int   DoDFS(int v);
	bool  remove_vertex_from_row(int row,int val);
	void  push_edge_back(int row,int val);
	int   row_degree(int row);
	int   handle_circle();
public:
	Graph() { m_directed_edges = 0 ;}
	void  print_path();
	int   graph_size() {return m_directed_edges/2;}
	bool   init(char *json_name);
	void  add_HeadNode(NodeHead_t *hd);
	void  dump();
	bool  has_euler_circle();
	int   dfs();
};

#endif

