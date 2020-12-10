
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

bool Graph::hasEulerCircle()
{

	// Recur for all the vertices adjacent to this vertex 
	list<NodeHead_t*> m_headnode_list;

	for (list<NodeHead_t *>::iterator it = m_headnode_list.begin(); 
		it != m_headnode_list.end(); ++it){
		
		NodeHead_t *x = *it;
		if ( (x->get_neigh_nr() % 2))
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
		n->set_neigh(temp_neigh);
		m_headnode_list.push_back(n);
		update_graph_size(nr_neigh);

	}
	cout << endl;
	
	free(fileData);
	cJSON_Delete(tree);

	return 0;
}


void Graph::Dump()
{
	for (list<NodeHead_t *>::iterator it = m_headnode_list.begin(); 
		it != m_headnode_list.end(); ++it){

		NodeHead_t *n = (*it);
		cout << "Node " << n->id() << " vertices:" << endl;
		n->dump();
		
	}

	cout << endl;
}

bool Graph::RemoveVertexFromRow(int row,int val)
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

// Scan all edges
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
		cout << "Reached an empty row left with " 
			<<  m_circle_path.size()  << 
			" vertices of total "  << graph_size()  << endl;
		Dump();
		return 0;
	}
	
	if (it == m_headnode_list.end()) {
		cout << "insane" << endl;
		return -1;
	}

	// Remove second vertex from neightbours roww
	int v2 = rowHead->erase_front();
	// Remove first vertex from the next row
	if (!RemoveVertexFromRow(v2, v1)){
		cout << "insane ,failed to find adjacent vertex" << endl;	
	}
	m_circle_path.push_back(v1);
	m_circle_path.push_back(v2);
	cout << "Edge (" << v1 << ","  << v2  << ")"   << endl;
	DoDFS(v2);
}

int Graph::DFS()
{
	NodeHead_t* rowHead = m_headnode_list.front();
	DoDFS(rowHead->id());

}
