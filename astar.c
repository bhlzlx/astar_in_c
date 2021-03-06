#include "astar.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>


void DebugTrace( AstarCellMap * _cellmap )
{
    printf("DebugTrace:\n");
    astar_print_cellmap( _cellmap );
}


void astar_calc_valuecose( AstarCell * _this, AstarCell * _target )
{
    _this->value_guess  = astar_value_manhattan( _this, _target );
    _this->value_total = _this->value_guess + _this->value_known;
}

void astar_path_delete(AstarPath * _path)
{
	AstarPathNode * node = _path->head;
	while (node)
	{
		AstarPathNode * next = node->next;
		free(node);
		node = next;
	}
	free(_path);
}

bool 
astar_find(AstarCellMap * _map, AstarPos _begin, AstarPos _end, AstarPath * _path)
{
	assert(_begin.x >= 0 && _begin.x < _map->width);
	assert(_end.x >= 0 && _end.x < _map->width);
	assert(_begin.y >= 0 && _begin.y < _map->height);
	assert(_end.y >= 0 && _end.y < _map->height);

	AstarCell * _cellBegin = &_map->table[_begin.x + _begin.y * _map->width];
	AstarCell * _cellEnd = &_map->table[_end.x + _end.y * _map->width];

	AstarOpenList * openList = astar_openlist_create();
	AstarCloseList * closeList = astar_closelist_create(_map->width * _map->height / 4);

	// setup first init state
	for (int i = 0; i<4; ++i)
	{
		if (_cellBegin->simblings[i] && _cellBegin->simblings[i]->value != AstarCellUnwalkable )
		{
			_cellBegin->value = AstarCellCloseList;
			_cellBegin->simblings[i]->value_known = 1;

			_cellBegin->simblings[i]->previndex = InvertSimbling[i];

			astar_calc_valuecose(_cellBegin->simblings[i], _cellEnd);
			AstarOpenListNode * opennode = astar_openlist_create_node();
			opennode->cell = _cellBegin->simblings[i];
			astar_openlist_insert(openList, opennode);
		}
	}
	astar_closelist_insert(closeList, _cellBegin);
	//
	// DebugTrace( _astarmap );
	while (!astar_openlist_empty(openList))
	{
		// 取代码最小的那个openlist项
		AstarOpenListNode * lessnode = astar_openlist_front(openList);
		astar_openlist_popfront(openList);
		if (lessnode->cell->value_guess == 0)
		{
			// 找到了结点！
			printf("succeed!\n");

			astar_openlist_delete_node(lessnode);
			astar_openlist_delete(openList);
			astar_closelist_delete(closeList);

			// 生成结果
			AstarCell * cell = _cellEnd;
			AstarPathNode * prevnode = NULL;
			while (cell->previndex != 0xff)
			{
				AstarPathNode * pnode = (AstarPathNode*)malloc(sizeof(AstarPathNode));
				pnode->next = prevnode;
				pnode->cell = cell;
				prevnode = pnode;
				_map->table[cell->y * _map->width + cell->x].value = AstarCellPathResult;
				cell = cell->simblings[cell->previndex];
			}

			AstarPathNode * hnode = (AstarPathNode*)malloc(sizeof(AstarPathNode));
			hnode->next = prevnode;
			hnode->cell = cell;

			_path->cost = _cellEnd->value_total;
			_path->head = hnode;

			cell->value = AstarCellPathBegin;
			_cellEnd->value = AstarCellPathEnd;

			return true;
		}
		// 代价最少的不是目标结点，openlist代价最小的这个点，周围的点(如果结果已经在closelist内，不加入openlist )加入openlist
		for (int i = 0; i < 4; ++i)
		{
			AstarCell * scell = lessnode->cell->simblings[i];
			if (scell)
			{
				if (scell->value != AstarCellUnwalkable)
				{
					// in closelist?
					if (!astar_closelist_find(closeList, scell))
					{
						// in openlist?
						if (!astar_openlist_exist(openList, scell))
						{
							// 计算代价
							scell->value_known = lessnode->cell->value_known + 1;
							scell->previndex = InvertSimbling[i];
							astar_calc_valuecose(scell, _cellEnd);

						/*  // 他 else 那块功能重复,但好像另一个更好
							for (int dir = 0; dir < 4; ++dir)
							{
								AstarCell * dircell = scell->simblings[dir];
								if (dircell && dircell != lessnode->cell && dircell->previndex!=0xff && (dircell->value_known < lessnode->cell->value_known ) )
								{
									scell->previndex = dir;
									scell->value_known = dircell->value_known + 1;
								}
							}		
						*/
						}
						else
						{
							// 计算代价
							// 如果这个结点就是原本的上个节点就不做处理
							// 获取它的代价
							float scellcost = scell->value_known;
							if (scellcost > lessnode->cell->value_known + 1)
							{
								scell->previndex = InvertSimbling[i];
								scell->value_known = lessnode->cell->value_known + 1;
								astar_calc_valuecose(scell, _cellEnd);
							}							
						}
						AstarOpenListNode * opennode = astar_openlist_create_node();
						opennode->cell = scell;
						astar_openlist_insert(openList, opennode);
					}
				}
			}
		}
		// lessnode 自身加入closelist
		astar_closelist_insert(closeList, lessnode->cell);
		//
		astar_openlist_delete_node(lessnode);
		//DebugTrace(_map);
	}
	astar_openlist_delete(openList);
	astar_closelist_delete(closeList);
	_path->head = NULL;
	_path->cost = 0;
	return false;
}
