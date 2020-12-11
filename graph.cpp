
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mount.h>
#include <semaphore.h>
#include <stdint.h>
#include <iostream>
#include "json/cJSON.h"

#include<vector>
#include<iostream>
#include<algorithm>

#include "node.h"
#include "graph.h"


using namespace std;

bool graph_t::has_euler_circle()
{

	// Recur for all the vertices adjacent to this vertex 
	for (list<nodehead_t *>::iterator it = m_headnode_list.begin(); 
		it != m_headnode_list.end(); ++it){

		nodehead_t *nh = *it;

		if ( (nh->degree() % 2))
			return false;
	}

	return true;
}




char* graph_t::cjson_readFile(char *fname)
{
        int fd;
        char *data;
        struct stat buf;

        if (stat(fname, &buf) < 0){
       		cout << "Failed to stat" << fname << " " << strerror(errno);
                return NULL;
        }

        fd = open(fname,O_RDONLY);
        if (fd < 0){
       		cout << "Failed to open" << fname << " " << strerror(errno);
       		return NULL;
        }

        data = (char *)malloc(buf.st_size);
        memset(data,0x00,buf.st_size);
        if (read(fd,data,buf.st_size) < buf.st_size){
       		cout << "Failed to read" << fname << " " << strerror(errno);
                return NULL;
        }
        close(fd);
        return data;
}

int graph_t::parseCoordStr(char *p,int Coords[])
{
        int i = 0;
        char *s,*e;

        e = p + strlen(p);
        for (s = p; s != NULL && s < e ; s++){
                sscanf(s,"%d",&Coords[i++]);
                s = strstr(s,",");
                if(!s) break;
        }
        return i;
}


bool  graph_t::init(char *json_name)
{

	char* fileData = cjson_readFile(json_name);
	
	cJSON* tree = cJSON_Parse(fileData);
	if (!tree) {
		cout << " failed parsing " << json_name << endl;
		return false;
	}

	cJSON* polys = cJSON_GetObjectItem(tree, "polygon");
	if (!polys) {
		cout << "Failed to retrieve polyong from json" << endl;
		return false;
	}

	int nr_nodes = cJSON_GetArraySize(polys);

	for (int i = 0 ; i < nr_nodes; i++){
		
		int temp_neigh[MAX_NEIGH] = {0};

		cJSON* NodeItem = cJSON_GetArrayItem(polys, i);
		if (!NodeItem) {
			cout << "Failed to retrieve node" << i << "from json" << endl;
			return false;
		}
		
		cJSON *Node  =  cJSON_GetObjectItem(NodeItem, "node");
		cJSON *Neighbours  =  cJSON_GetObjectItem(NodeItem, "neighbours");

		int nr_neigh = parseCoordStr(Neighbours->valuestring, temp_neigh);
		nodehead_t *n = new nodehead_t(Node->valueint, nr_neigh);
		n->set_neighbours(temp_neigh);
		m_headnode_list.push_back(n);
		update_graph_t_size(nr_neigh);

	}
	cout << endl;
	
	free(fileData);
	cJSON_Delete(tree);

	return true;
}


void graph_t::dump()
{
	cout << "Total edges " << graph_t_size() << endl;
	for (list<nodehead_t *>::iterator it = m_headnode_list.begin(); 
		it != m_headnode_list.end(); ++it){

		nodehead_t *n = (*it);
		cout << "Node " << n->id() << " vertices:" << endl;
		n->dump();
	}
	cout << endl;
}

void graph_t::dump_vector(vector<int>& vec)
{
	vector<int>::iterator it;
 		
	// solder two circles to one.
       	for (it = vec.begin(); it != vec.end(); it++) { 
		cout << *it << ":";		
	}
	cout << endl;
}	


bool graph_t::remove_vertex_from_row(int row,int val)
{
	list<nodehead_t *>::iterator it = m_headnode_list.begin();
	nodehead_t* rowHead;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->id() == row){
			return rowHead->remove_vertex(val);
		}
	}
	return false;
}

void graph_t::push_edge_back(int row,int val)
{
	list<nodehead_t *>::iterator it = m_headnode_list.begin();
	nodehead_t* rowHead = NULL;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->id() == row) {
			rowHead->push_back(val);
			return;	
		}
	}
}


int graph_t::row_degree(int row)
{
	list<nodehead_t *>::iterator it = m_headnode_list.begin();
	nodehead_t* rowHead = NULL;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->id() == row) {
			return rowHead->degree();	
		}
	}
	cout << __func__ << " insane " << endl;
	return 0;
}

/*
 * walk on the sub circle, find 
 * any vetex with edges and create a new circle
*/
int graph_t::handle_circle()
{
	vector<int>::iterator it = m_circle_path.begin();

	for (;it != m_circle_path.end(); ++it){
		if (row_degree((*it)) > 0)
			return *it;
	}
	return 0;
}

int graph_t::find_some_circle_vertex()
{
	list<nodehead_t *>::iterator it = m_headnode_list.begin();
	nodehead_t* rowHead = NULL;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->degree() > 0)
			return rowHead->id();
	}
	return -1;
}


int graph_t::do_dfs(int v1)
{
	list<nodehead_t *>::iterator it = m_headnode_list.begin();
	nodehead_t* rowHead = NULL;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->id() == v1){
			break;	
		}
	}
	if (it == m_headnode_list.end() ){
		// We did not find any connecting vertex
		cout << "reached an empty" << endl;
		return -1;
	}

	if (rowHead->empty()) {
		/*
		 * We have a circle
		*/
		print_sub_path("Euler sub-circle ");
		sub_circles.push_back(m_circle_path);
		v1 = handle_circle();
		if (v1 == 0) {  
			// End of scan. graph_t is empty from edges
			m_circle_path.clear();
			return 0;
		}
		
		m_circle_path.clear();
		if (!do_dfs(v1))
			return 0;
		 /*
		  * We have disconnected circles. find them 
		 */
		return do_dfs(find_some_circle_vertex());
	}


	m_circle_path.push_back(v1);

	/* Remove second vertex from neightbours row */
	int v2 = rowHead->erase_front();
	/* Remove first vertex from the next row */
	if (!remove_vertex_from_row(v2, v1)){
		cout << "insane ,failed to find adjacent vertex" << endl;	
	}

//	cout << "Edge (" << v1 << ","  << v2  << ")"   << endl;

	return do_dfs(v2);
}

void graph_t::strip_doubles()
{
	vector<int>::iterator it = m_circle_path.begin();

	for (;it != m_circle_path.end(); ){
		vector<int>::iterator itn = it;
		itn++;
		if ((*itn) == (*it))
			it = m_circle_path.erase(it);
		else
			++it;
	}
}

int graph_t::dfs()
{
	nodehead_t* rowHead = m_headnode_list.front();
	int v;

	while ( (v = find_some_circle_vertex()) > 0) {	
		do_dfs(v);
	}
	
	construct_full_circle();

	m_circle_path.clear();
	m_circle_path = sub_circles.front();
	strip_doubles();		
	sub_circles.pop_front();
	print_sub_path("Full eu circle");
}

void graph_t::print_sub_path(const char *prefix)
{
	vector<int>::iterator it = m_circle_path.begin();

	cout << endl <<  prefix <<  " [ ";
	for (;it != m_circle_path.end(); ++it){
		cout <<  (*it) << " " ;
	}
	cout << "]" << endl;
}

/*
 * find a commin vertex on each subcircle 
 * and unify
*/
void graph_t::construct_full_circle()
{
	list<vector<int> >::iterator vec1_itr; 
	vec1_itr = sub_circles.begin(); 

	if (vec1_itr == sub_circles.end())
		return;
	
	vector<int>& vec1 = *vec1_itr;
	list<vector<int> >::iterator vec2_itr = vec1_itr;
	vec2_itr++;

	if (vec2_itr == sub_circles.end())
		return;

	vector<int>& vec2 = *vec2_itr;

	vector<int>::iterator it1;
	vector<int>::iterator it2;
	/*
	* solder two sub circles to one.
	*/
       	for (it1 = vec1.begin(); it1 != vec1.end(); it1++) { 
			
		for (it2 = vec2.begin(); it2 != vec2.end() ; it2++){

			if ((*it2) == (*it1)){

				vector<int>* circle = new vector<int>;
				/*		
				* So first we must assign the same vertex
				*  of any two sub circles
				*/
				int val = (*it1);
				int dist1 = it1 - vec1.begin();
				rotate(vec1.begin(), vec1.begin() + dist1, vec1.end() );			
			

				int dist2 = it2 - vec2.begin();
				rotate(vec2.begin(), vec2.begin() + dist2, vec2.end() );			
		
				
				// Merge 					
				circle->insert(circle->end(), vec1.begin(), vec1.end());	
				circle->insert(circle->end(), vec2.begin(), vec2.end());	
				circle->push_back(val);
				// Prep for Next Iter
				sub_circles.erase(vec1_itr);
				sub_circles.erase(vec2_itr);
				sub_circles.push_back(*circle);
				construct_full_circle();
				// Do not iterate here. vector are garbled now
				return;
			}
		}
	}
}

