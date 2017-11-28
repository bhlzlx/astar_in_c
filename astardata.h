#ifndef __ASTAR_DATA_H__
#define __ASTAR_DATA_H__

#include "hashtable/hashtable.h"
#include <stdbool.h>

enum Simblings
{
	top = 0, 
	bottom, 
	left, 
	right
};

int InvertSimbling[4];

enum AstarCellType
{
	AstarCellUnwalkable =	0,
	AstarCellWalkable =		1,
	AstarCellOpenList =		2,
	AstarCellCloseList =	3,
	AstarCellPathResult =	4,
	AstarCellPathBegin =	5,
	AstarCellPathEnd =		6
};

unsigned char AstarCellTypeIndicator[];
unsigned char AstarCellTypeParseIndicator[];

void astar_set_indicator(const unsigned char * _str);

typedef struct __ASTAR_CELL
{
	struct __ASTAR_CELL * simblings[4];
	unsigned int 		x;
	unsigned int		y;
	unsigned char 		value;
	//
	float   			value_known;
	float				value_guess;
	float 				value_total;
    unsigned char       previndex;
} AstarCell;

typedef struct __ASTAR_CELL_MAP
{
	AstarCell*			table;
	unsigned int 		width;
	unsigned int 		height;
} AstarCellMap;

typedef struct __ASTAR_OPENLIST_NODE
{
	AstarCell *						cell;
	struct __ASTAR_OPENLIST_NODE * 	next;
} AstarOpenListNode;

typedef struct __ASTAR_OPENLIST
{
	AstarOpenListNode * 					head;
} AstarOpenList;

typedef struct __ASTAR_CLOSELIST
{
    struct hashtable * htable;
    unsigned int (* hashfunc )(void *);
    int (* cmpfunc )( void *, void *);
} AstarCloseList;

//// Astar Cell /////////
AstarCell * astar_newcell();

/// Astar Map //////////
AstarCellMap * 
astar_generate_cellmap( const unsigned char * _ascii, int _width, int _height );

void
astar_print_cellmap( AstarCellMap * _cellmap );

void astar_delete_cellmap( AstarCellMap * _map );

//////  OpenList ////////
AstarOpenListNode *
astar_openlist_create_node();

void
astar_openlist_delete_node(AstarOpenListNode * _node);

AstarOpenList * 
astar_openlist_create();

void
astar_openlist_delete(AstarOpenList * _openlist);

void
astar_openlist_insert( AstarOpenList * _list, AstarOpenListNode* _node );

AstarOpenListNode *
astar_openlist_front( AstarOpenList * _list );

void
astar_openlist_popfront( AstarOpenList * _list );

bool
astar_openlist_exist( AstarOpenList * _list, AstarCell * _cell );

bool
astar_openlist_empty( AstarOpenList * _list );

///////// CloseList /////////////
AstarCloseList *
astar_closelist_create( unsigned int _minsize );

void
astar_closelist_delete( AstarCloseList * _closelist );

bool 
astar_closelist_insert(AstarCloseList * _closelist, AstarCell * _cell );

bool
astar_closelist_find( AstarCloseList * _closelist, AstarCell * _cell );


/// value function ///
typedef float ( * astar_value_func )( AstarCell *, AstarCell * );

float astar_value_manhattan( AstarCell * _cella, AstarCell * _cellb );

void
astar_trace_openlist( AstarOpenList * _openlist );

void
astar_trace_closelist( AstarCloseList * _closelist );
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
#endif