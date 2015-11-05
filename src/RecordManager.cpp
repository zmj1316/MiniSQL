#include<iostream>
#include<string>
#include<vector>
#include<set>
#include "recordmanager.h"
#include "MiniSQL.h"
#include "global.h"
#include "btree.h"

using namespace std;
static record *binary2record(table* tab, u8* bin)
{
	record *result = new record;
	if ((*bin) == 0xFF)
	{
		result->valid = false;
		return result;
	}
	bin++;
	result->valid = true;
	for (int i = 0; i < tab->colNum_u64; i++)
	{
		column *col = &(tab->col[i]);
		result->i[i].type = col->type;
		switch (col->type)
		{
		case INT:
			result->i[i].data.i = *(int *)bin;
			break;
		case FLOAT:
			result->i[i].data.f = *(float*)bin;
			break;
		case CHAR:
			result->i[i].data.str = new char[(col->size_u8)];
			memcpy(result->i[i].data.str, (char *)bin, col->size_u8);/* Mem leap! */
			break;
		default:
			break;
		}
		bin += col->size_u8;
	}
	return result;
}

static bool record2binary(table *tab, u8 *bin, record *entry)
{
	*bin++ = 1;/* valid = true */
	for (int i = 0; i < tab->colNum_u64; i++)
	{
		column *col = &(tab->col[i]);
		item *it = &(entry->i[i]);
		switch (it->type)
		{
		case INT:
			*(int*)bin = it->data.i;
			break;
		case CHAR:
			memcpy(bin, it->data.str, col->size_u8);
			break;
		case FLOAT:
			*(float*)bin = it->data.f;
			break;
		default:
			break;
		}
	}
	return true;
}

bool Recordmanager_insertRecord(table* tab, record* rcd)
{
	int c = BLOCKSIZE / (tab->recordSize + 1);
	if (tab->recordNum % c == 0)
	{
		newBlock(&(tab->buf));
	}
	move_window(&(tab->buf), (++tab->recordNum - 1) / c + 1);
	record2binary(tab, tab->buf.win + (tab->recordNum - 1) % c, rcd);
	tab->buf.dirty = true;
}

int Recordmanager_deleteRecordwithIndex(table* tab, Filter *flt, const char *idxname, Rule *rule, int idx)
{
	int num = 0;
	record *rcd;
	vector<record> result;
	vector<Rule>::iterator iter;
	set<u32> blockset = btree_select(idxname, rule);
	set<u32>::iterator siter;
	int c = BLOCKSIZE / (tab->recordSize + 1);
	for (siter = blockset.begin(); siter != blockset.end(); siter++)
	{
		move_window(&(tab->buf), *siter);
		for (int j = 0; j < c; j++)
		{
			rcd = binary2record(tab, tab->buf.win + j * tab->recordSize);
			for (iter = flt->rules.begin(); iter != flt->rules.end(); iter++)
			{
				switch (iter->cmp)
				{
				case LT:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i < iter->target.i)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str < iter->target.str)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f < iter->target.f)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					}
					break;
				}
				case LE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i <= iter->target.i)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str <= iter->target.str)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f <= iter->target.f)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					}
					break;
				}
				case EQ:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i = iter->target.i)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str = iter->target.str)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f = iter->target.f)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					}
					break;
				}
				case NE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i != iter->target.i)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str != iter->target.str)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f != iter->target.f)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					}
					break;
				}
				case GT:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i > iter->target.i)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str > iter->target.str)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f > iter->target.f)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					}
					break;
				}
				case GE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i >= iter->target.i)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str >= iter->target.str)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f >= iter->target.f)
						{
							btree_delete_node(idxname, &rcd->i[idx].data);
							rcd->valid = false;
							num++;
						}
						break;
					}
					}
					break;
				}
				}
			}
		}
	}

	tab->buf.dirty = true;
	return num;
}

int Recordmanager_deleteRecord(table* tab, Filter *flt)
{
	int num = 0;
	record *rcd;
	vector<Rule>::iterator iter;
	int c = BLOCKSIZE / (tab->recordSize + 1);
	for (int i = 1; i <= (tab->recordNum - 1) / c + 1; i++)
	{
		move_window(&(tab->buf), i);
		for (int j = 0; j < c; j++)
		{
			rcd = binary2record(tab, tab->buf.win + j * tab->recordSize);
			for (iter = flt->rules.begin(); iter != flt->rules.end(); iter++)
			{
				switch (iter->cmp)
				{
					case LT:
					{
						switch (rcd->i[iter->colNo].type)
						{
							case INT:
							{
								if (rcd->i[iter->colNo].data.i < iter->target.i)
								{
									rcd->valid = false;
									num++;
								}
								break;
							}
							case CHAR:
							{
								if (rcd->i[iter->colNo].data.str < iter->target.str)
								{
									rcd->valid = false;
									num++;
								}
								break;
							}
							case FLOAT:
							{
								if (rcd->i[iter->colNo].data.f < iter->target.f)
								{
									rcd->valid = false;
									num++;
								}
								break;
							}
						}
						break;
					}
					case LE:
					{
						switch (rcd->i[iter->colNo].type)
						{
						case INT:
						{
							if (rcd->i[iter->colNo].data.i <= iter->target.i)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case CHAR:
						{
							if (rcd->i[iter->colNo].data.str <= iter->target.str)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case FLOAT:
						{
							if (rcd->i[iter->colNo].data.f <= iter->target.f)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						}
						break;
					}
					case EQ:
					{
						switch (rcd->i[iter->colNo].type)
						{
						case INT:
						{
							if (rcd->i[iter->colNo].data.i = iter->target.i)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case CHAR:
						{
							if (rcd->i[iter->colNo].data.str = iter->target.str)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case FLOAT:
						{
							if (rcd->i[iter->colNo].data.f = iter->target.f)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						}
						break;
					}
					case NE:
					{
						switch (rcd->i[iter->colNo].type)
						{
						case INT:
						{
							if (rcd->i[iter->colNo].data.i != iter->target.i)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case CHAR:
						{
							if (rcd->i[iter->colNo].data.str != iter->target.str)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case FLOAT:
						{
							if (rcd->i[iter->colNo].data.f != iter->target.f)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						}
						break;
					}
					case GT:
					{
						switch (rcd->i[iter->colNo].type)
						{
						case INT:
						{
							if (rcd->i[iter->colNo].data.i > iter->target.i)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case CHAR:
						{
							if (rcd->i[iter->colNo].data.str > iter->target.str)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case FLOAT:
						{
							if (rcd->i[iter->colNo].data.f > iter->target.f)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						}
						break;
					}
					case GE:
					{
						switch (rcd->i[iter->colNo].type)
						{
						case INT:
						{
							if (rcd->i[iter->colNo].data.i >= iter->target.i)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case CHAR:
						{
							if (rcd->i[iter->colNo].data.str >= iter->target.str)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						case FLOAT:
						{
							if (rcd->i[iter->colNo].data.f >= iter->target.f)
							{
								rcd->valid = false;
								num++;
							}
							break;
						}
						}
						break;
					}
				}
			}
		}
	}
	tab->buf.dirty = true;
	return num;
}

vector<record> Recordmanager_selectRecord(table* tab, Filter* flt)
{
	vector<record> result;
	record *rcd;
	vector<Rule>::iterator iter;
	int c = BLOCKSIZE / (tab->recordSize + 1);
	for (int i = 1; i <= (tab->recordNum - 1) / c + 1; i++)
	{
		move_window(&(tab->buf), i);
		for (int j = 0; j < c; j++)
		{
			rcd = binary2record(tab, tab->buf.win + j * tab->recordSize);
			for (iter = flt->rules.begin(); iter != flt->rules.end(); iter++)
			{
				switch (iter->cmp)
				{
				case LT:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i < iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str < iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f < iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case LE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i <= iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str <= iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f <= iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case EQ:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i = iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str = iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f = iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case NE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i != iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str != iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f != iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case GT:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i > iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str > iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f > iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case GE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i >= iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str >= iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f >= iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				}
			}
		}
	}
	tab->buf.dirty = true;
}

vector<record> Recordmanager_selectRecordwithIndex(table* tab, Filter *flt, const char *idxname, Rule *rule)
{
	record *rcd;
	vector<record> result;
	vector<Rule>::iterator iter;
	set<u32> blockset = btree_select(idxname, rule);
	set<u32>::iterator siter;
	int c = BLOCKSIZE / (tab->recordSize + 1);
	for (siter = blockset.begin(); siter != blockset.end(); siter++)
	{
		move_window(&(tab->buf), *siter);
		for (int j = 0; j < c; j++)
		{
			rcd = binary2record(tab, tab->buf.win + j * tab->recordSize);
			for (iter = flt->rules.begin(); iter != flt->rules.end(); iter++)
			{
				switch (iter->cmp)
				{
				case LT:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i < iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str < iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f < iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case LE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i <= iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str <= iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f <= iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case EQ:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i = iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str = iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f = iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case NE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i != iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str != iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f != iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case GT:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i > iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str > iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f > iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				case GE:
				{
					switch (rcd->i[iter->colNo].type)
					{
					case INT:
					{
						if (rcd->i[iter->colNo].data.i >= iter->target.i)
							result.push_back(*rcd);
						break;
					}
					case CHAR:
					{
						if (rcd->i[iter->colNo].data.str >= iter->target.str)
							result.push_back(*rcd);
						break;
					}
					case FLOAT:
					{
						if (rcd->i[iter->colNo].data.f >= iter->target.f)
							result.push_back(*rcd);
						break;
					}
					}
					break;
				}
				}
			}
		}
	}
}
