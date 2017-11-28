#ifndef __ASTAR_H__
#define __ASTAR_H__

#include "astardata.h"

typedef struct  __ASTAR_POS
{
	unsigned int x;
	unsigned int y;
}AstarPos;

typedef struct __ASTAR_PATH_NODE
{
	AstarCell *					cell;
	struct __ASTAR_PATH_NODE *	next;
} AstarPathNode;

typedef struct __ASTAR_PATH
{
	AstarPathNode *				head;
	float						cost;
} AstarPath;

void astar_path_delete(AstarPath * _path);

bool astar_find(AstarCellMap * _map, AstarPos _begin, AstarPos _end, AstarPath * _path);

#endif

