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



int main(int argc, char* argv[])
{
	Graph G;

	if (argc < 1){
		cout << argv[0] <<  "  <graph.json>" << endl;
		return -1;
	}

	if (!G.init(argv[1])){
		cout << "failed ot parse json" << endl;
		return -1;
	}
	cout << "Graph has " << G.graph_size() << " edges." << endl ;
	if (!G.has_euler_circle()){
		cout << "No Euler Circle for me" << endl;
		return 1;
	}
//	G.dump();
	G.dfs();
}

