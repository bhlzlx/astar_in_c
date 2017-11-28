#include "astardata.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "hashtable/hashtable_itr.h"

int InvertSimbling[4] = { 
    bottom,
    top,
    right,
    left
 };

unsigned char AstarCellTypeIndicator[] = "# ?x.oO";
unsigned char AstarCellTypeParseIndicator[] = "#`";


void astar_set_indicator(const unsigned char * _str)
{
	assert( strlen(( const char*)_str) >= strlen( AstarCellTypeIndicator ));
	memcpy(&AstarCellTypeIndicator, _str, sizeof(AstarCellTypeIndicator));
}

AstarCell *
astar_newcell()
{
	AstarCell * cell = (AstarCell * )malloc( (size_t)sizeof( AstarCell ));
	memset( cell, 0, sizeof(AstarCell) );
    cell->previndex = 0xff;
	return cell;
}

AstarCellMap * 
astar_generate_cellmap( const unsigned char * _ascii, int _width, int _height )
{
	AstarCellMap * cellmap = (AstarCellMap *)malloc( sizeof( AstarCellMap ) );
	AstarCell * ptrcache = malloc( sizeof( AstarCell) * _width * _height );
	memset( ptrcache, 0, sizeof( AstarCell*) * _width * _height );
	
	for(int h = 0; h < _height; ++h )
	{
		for( int w = 0; w < _width; ++w )
		{
			AstarCell* cell = &ptrcache[h * _width + w];
			memset(cell, 0, sizeof(AstarCell) );
			cell->previndex = 0xff;
			// 处理相lin关系
			{
				int toph = h - 1;
				int topw = w;
				if( toph >= 0 && topw >= 0 )
				{
					AstarCell * topcell = &ptrcache[toph * _width + topw ];
					topcell->simblings[bottom] = cell;
					cell->simblings[top] = topcell;
				}
			}
			{
				int lefth = h;
				int leftw = w - 1;
				if( lefth >= 0 && leftw >= 0 )
				{
					AstarCell * leftcell = &ptrcache[lefth * _width + leftw ];
					leftcell->simblings[right] = cell;
					cell->simblings[left] = leftcell;
				}
			}
			cell->x = w;
			cell->y = h;
			unsigned char indicator = _ascii[h * _width + w];
			cell->value = AstarCellUnwalkable;
			for (int i = 0; i < 2; ++i)
			{
				if ( indicator == AstarCellTypeParseIndicator[i])
				{
					cell->value = i;
				}
			}
		}
	}
	cellmap->table = ptrcache;
	cellmap->height = _height;
	cellmap->width = _width;
	return cellmap;
}

void
astar_print_cellmap( AstarCellMap * _cellmap )
{
	
	for( unsigned int i = 0; i<_cellmap->height; ++i )
	{
		for( unsigned int j = 0; j<_cellmap->width; ++j )
		{
			unsigned char value = _cellmap->table[i * _cellmap->width + j].value;
			printf("%c", AstarCellTypeIndicator[ value ] );
		}
		printf("\n");
	}
	printf("\n");
}

void astar_delete_cellmap(AstarCellMap * _map)
{
	//_map->table
}

float
astar_value_manhattan( AstarCell * _cella, AstarCell * _cellb )
{
	return (float)abs(_cellb->y - _cella->y)+abs(_cellb->x - _cella->x );
}

/// openlist operations

AstarOpenListNode *
astar_openlist_create_node()
{
	AstarOpenListNode * node = (AstarOpenListNode *)malloc( sizeof(AstarOpenListNode) );
	node->cell = NULL;
	node->next = NULL;
    return node;
}

void astar_openlist_delete_node(AstarOpenListNode * _node)
{
	_node->cell = NULL;
	_node->next = NULL;
	free(_node);
}

AstarOpenList * 
astar_openlist_create()
{
	AstarOpenList * list = (AstarOpenList *)malloc( sizeof(AstarOpenList) );
	list->head = astar_openlist_create_node();
    return list;
}

void astar_openlist_delete( AstarOpenList * _openlist )
{
	AstarOpenListNode * node = _openlist->head;
	while (node)
	{
		AstarOpenListNode * next = node->next;
		free(node);
		node = next;
	}
	free( _openlist );
}

void
astar_openlist_insert( AstarOpenList * _list, AstarOpenListNode* _node )
{
    _node->cell->value = AstarCellOpenList;
    //
	AstarOpenListNode * curr = _list->head->next;
	AstarOpenListNode * last = _list->head;
	while( curr )
	{
		if( curr->cell->value_total >= _node->cell->value_total )
		{
			last->next = _node;
			_node->next = curr;
            return;
		}
		last = curr;
		curr = curr->next;
	}
	last->next = _node;
	_node->next = NULL;
}

AstarOpenListNode *
astar_openlist_front( AstarOpenList * _list )
{
	return _list->head->next;
}

void
astar_openlist_popfront( AstarOpenList * _list )
{
	AstarOpenListNode * node = _list->head->next;
    node->cell->value= AstarCellCloseList;
    if(!node)
    {
        return;
    }
	_list->head->next = _list->head->next->next;
}

bool
astar_openlist_exist( AstarOpenList * _list, AstarCell * _cell )
{
    AstarOpenListNode * curr = _list->head->next;
    while(curr)
    {
        if( curr->cell == _cell )
        {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

bool
astar_openlist_empty( AstarOpenList * _list )
{
    if(  _list->head->next )
    {
        return false;
    }
    return true;
}

/////////////// close list hash table //////////////////

unsigned int 
astar_closelist_hash( void * _key )
{
    unsigned int hash = 5381;
    for( int i = 0; i<sizeof( _key ); ++i )
    {
        hash = ((hash << 5) + hash) + ((unsigned char *)&_key)[i];        
    }
    return hash;
}

int 
astar_closelist_hash_keyequal( void * _key1, void * _key2 )
{
    return _key1 == _key2;
}

AstarCloseList *
astar_closelist_create( unsigned int _minsize )
{
    AstarCloseList * closelist = (AstarCloseList *)malloc( sizeof( AstarCloseList ) );
    closelist->cmpfunc = astar_closelist_hash_keyequal;
    closelist->hashfunc = astar_closelist_hash;
    closelist->htable = create_hashtable( _minsize, astar_closelist_hash, astar_closelist_hash_keyequal );
    return closelist;
}

void astar_closelist_delete(AstarCloseList * _closelist)
{
	// 销毁啥希表
	hashtable_destroy(_closelist->htable, 0);
	free(_closelist);
}

bool 
astar_closelist_insert(AstarCloseList * _closelist, AstarCell * _cell )
{
    if ( !hashtable_insert( _closelist->htable, _cell, (void*)0x1 ) ) 
     {
        return false;
     }
     else
     {
         return true;
     }
}

bool astar_closelist_find( AstarCloseList * _closelist, AstarCell * _cell )
{
    void * result = hashtable_search( _closelist->htable, _cell );
    return !!result;
}

void
astar_trace_openlist( AstarOpenList * _openlist )
{
    printf("openlist : ");
    AstarOpenListNode * curr = _openlist->head->next;
    while( curr )
    {
        printf( "(%d,%d)-", curr->cell->x, curr->cell->y );
        curr = curr->next;
    }
    printf("\n");
}

void
astar_trace_closelist( AstarCloseList * _closelist )
{
    printf("closelist : ");
    struct hashtable_itr *itr = hashtable_iterator( _closelist->htable );
    int i = 0;
    if (hashtable_count( _closelist->htable ) > 0)
    {
        do {
            AstarCell * cell = (AstarCell *)hashtable_iterator_key(itr);
            printf( "(%d,%d)-", cell->x, cell->y );
            i++;
        } while (hashtable_iterator_advance(itr));
    }
    printf("\n");
}