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
		cout << argv[0] <<  "  <polygons.json>" << endl;
		return -1;
	}

	G.init(argv[1]);
	G.Dump();

	G.DFS();
	if (!G.hasEulerCircle()){
		cout << "No Euler Circle for me" << endl;
	}
}

