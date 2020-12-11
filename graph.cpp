
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


#include "node.h"
#include "graph.h"


using namespace std;

bool Graph::has_euler_circle()
{

	// Recur for all the vertices adjacent to this vertex 
	for (list<NodeHead_t *>::iterator it = m_headnode_list.begin(); 
		it != m_headnode_list.end(); ++it){

		NodeHead_t *nh = *it;

		if ( (nh->degree() % 2))
			return false;
	}

	return true;
}




char* Graph::cJSON_readFile(char *fname)
{
        int fd;
        char *data;
        struct stat buf;

        if (stat(fname, &buf) < 0){
                printf("Failed to stat %s %s\n",fname,strerror(errno));
                return NULL;
        }

        fd = open(fname,O_RDONLY);
        if (fd < 0){
       		printf("Failed to open %s %s\n",fname,strerror(errno));
       		return NULL;
        }

        data = (char *)malloc(buf.st_size);
        memset(data,0x00,buf.st_size);
        if (read(fd,data,buf.st_size) < buf.st_size){
       		printf("Failed to read %s %s\n",fname,strerror(errno));
                return NULL;
        }
        close(fd);
        return data;
}

int Graph::parseCoordStr(char *p,int Coords[])
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


int  Graph::init(char *json_name)
{

	char* fileData = cJSON_readFile(json_name);
	
	cJSON* tree = cJSON_Parse(fileData);
	if (!tree) {
		printf("%s failed parsing\n",json_name);
		return 1;
	}

	cJSON* polys = cJSON_GetObjectItem(tree, "polygon");
	if (!polys) {
		printf("Failed to retrieve polyong from json\n");
		return -1;
	}

	int nr_nodes = cJSON_GetArraySize(polys);

	for (int i = 0 ; i < nr_nodes; i++){
		
		int temp_neigh[MAX_NEIGH] = {0};

		cJSON* NodeItem = cJSON_GetArrayItem(polys, i);
		if (!NodeItem) {
			printf("Failed to retrieve node %d from json\n",i);
			return -1;
		}
		
		cJSON *Node  =  cJSON_GetObjectItem(NodeItem, "node");
		cJSON *Neighbours  =  cJSON_GetObjectItem(NodeItem, "neighbours");

		int nr_neigh = parseCoordStr(Neighbours->valuestring, temp_neigh);
		NodeHead_t *n = new NodeHead_t(Node->valueint, nr_neigh);
		n->set_neighbours(temp_neigh);
		m_headnode_list.push_back(n);
		update_graph_size(nr_neigh);

	}
	cout << endl;
	
	free(fileData);
	cJSON_Delete(tree);

	return 0;
}


void Graph::dump()
{
	for (list<NodeHead_t *>::iterator it = m_headnode_list.begin(); 
		it != m_headnode_list.end(); ++it){

		NodeHead_t *n = (*it);
		cout << "Node " << n->id() << " vertices:" << endl;
		n->dump();
		
	}

	cout << endl;
}

bool Graph::remove_vertex_from_row(int row,int val)
{
	list<NodeHead_t *>::iterator it = m_headnode_list.begin();
	NodeHead_t* rowHead;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->id() == row){
			return rowHead->remove_vertex(val);
		}
	}
	return false;
}

void Graph::push_edge_back(int row,int val)
{
	list<NodeHead_t *>::iterator it = m_headnode_list.begin();
	NodeHead_t* rowHead = NULL;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->id() == row) {
			rowHead->push_back(val);
			return;	
		}
	}
}


int Graph::row_degree(int row)
{
	list<NodeHead_t *>::iterator it = m_headnode_list.begin();
	NodeHead_t* rowHead = NULL;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->id() == row) {
			return rowHead->degree();	
		}
	}
	cout << __func__ << " insane" << endl;
	return 0;
}


int Graph::DoDFS(int v1)
{
	list<NodeHead_t *>::iterator it = m_headnode_list.begin();
	NodeHead_t* rowHead = NULL;

	for (;it != m_headnode_list.end(); ++it){
		rowHead = (*it);
		if (rowHead->id() == v1)
			break;	
	}

	if (rowHead->empty()) {

		cout << "empty row left with " 
				<<  m_circle_path.size()  << 
				" vertices of total "  << graph_size()  << endl;

		if (m_circle_path.size()  == graph_size()) {
		//	cout << "Reached an empty row left with " 
		//		<<  m_circle_path.size()  << 
		//		" vertices of total "  << graph_size()  << endl;
			return 0;
		}

		/*
		* Reached a circle , revert circle path
		* up to the vertex that has an alternative path
		*	v1 - last vertex
		*	v2 - head of circle path	 
		*/
		int v2;
		do {
			v2 = m_circle_path.back();		
			m_circle_path.pop_back();
			cout << "revert path " << v1 << " " <<  v2 << endl;	
			/* put back edges into graph */
			push_edge_back(v1, v2);
			push_edge_back(v2, v1);
			
			dump();
			v1 = v2;
		} while (row_degree(v2) == 1);
		return DoDFS(v2);
	}
	
	if (it == m_headnode_list.end()) {
		cout << "insane" << endl;
		return -1;
	}

	m_circle_path.push_back(v1);

	/* Remove second vertex from neightbours row */
	int v2 = rowHead->erase_front();
	/* Remove first vertex from the next row */
	if (!remove_vertex_from_row(v2, v1)){
		cout << "insane ,failed to find adjacent vertex" << endl;	
	}

	cout << "Edge (" << v1 << ","  << v2  << ")"   << endl;
	sleep(1);
	int rc = DoDFS(v2);
	if (rc < 0)
		return -1;
	return 0;
}

int Graph::dfs()
{
	NodeHead_t* rowHead = m_headnode_list.front();
	DoDFS(rowHead->id());

}


void Graph::print_path()
{
	list<int>::iterator it = m_circle_path.begin();

	int t = m_circle_path.front();

	cout << endl <<  "[";
	for (;it != m_circle_path.end(); ++it){
		cout <<  (*it) << "," ;
	}
	cout << t << "]" << endl;
}
