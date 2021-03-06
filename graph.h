#ifndef __GRAPH_H__
#define  __GRAPH_H__

#include <list>
#include <vector>

using namespace std;

class graph_t {

	int m_nr_nodes;
	list<nodehead_t*> m_headnode_list;
	vector<int> m_circle_path;
	list < vector<int> > sub_circles; // nested list of circles
	int m_directed_edges;

private:
	void  update_graph_t_size(int sz) { m_directed_edges += sz;}
	char* cjson_readFile(char *fname);
	int   parseCoordStr(char *p,int Coords[]);
	int   do_dfs(int v);
	bool  remove_vertex_from_row(int row,int val);
	void  push_edge_back(int row,int val);
	int   row_degree(int row);
	int   handle_circle();
	void  construct_full_circle();
	void  dump_vector(vector<int>& vec);
	int   find_some_circle_vertex();
	void  strip_doubles();

public:
	graph_t() { m_directed_edges = 0 ;}
	void  print_sub_path(const char *);

	void  print_euler_vector();
	void  print_full_circle();
	int   graph_t_size() {return m_directed_edges/2;}
	bool  init(char *json_name);
	void  add_HeadNode(nodehead_t *hd);
	void  dump();
	bool  has_euler_circle();
	int   dfs();
};

#endif

