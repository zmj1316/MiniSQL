#include"btree.h"
#include"MiniSQL.h"
#include"buffer.h"
#include"global.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h> 
#include <stdio.h>
#include<vector>
#include<set>
#include <algorithm>
#include <iterator>
using namespace std;

void *mallocZero(int n) {
    void *p = malloc(n);
    if (p)
    {
        memset(p, 0, n);
    }
    return p;
}

u8 Rule_cmp(dataType type, Data* s, Data* dst, Compare c)
{
    float cmp = 0;
    switch (type)
    {
    case INT:
        cmp = s->i - dst->i;
        break;
    case CHAR:
        cmp = strcmp(s->str, dst->str);
        break;
    case FLOAT:
        cmp = s->f - dst->f;
        break;
    default:
        break;
    }
    switch (c)
    {
    default:
        return -1;
    case LT:
        return cmp < 0 ? 1 : 0;
    case LE:
        return cmp <= 0 ? 1 : 0;
    case EQ:
        return cmp == 0 ? 1 : 0;
    case NE:
        return cmp != 0 ? 1 : 0;
    case GE:
        return cmp >= 0 ? 1 : 0;
    case GT:
        return cmp > 0 ? 1 : 0;
    }
}

static record binary2record(table* tab, u8* bin)
{
    record result;
    if ((*bin) == 0x0)
    {
        result.valid = false;
        return result;
    }
    bin++;
    result.valid = true;
    item it;
    for (u32 i = 0; i < tab->colNum_u64; i++)
    {
        
        column *col = &(tab->col[i]);
        it.type = col->type;
        switch (col->type)
        {
        case INT:
            it.data.i = *(int *)bin;
            break;
        case FLOAT:
            it.data.f = *(float*)bin;
            break;
        case CHAR:
            it.data.str = (char *)mallocZero(col->size_u8);
            memcpy(it.data.str, (char *)bin, col->size_u8);/* Mem leap! */
            break;
        default:
            break;
        }
        result.i.push_back(it);
        bin += col->size_u8;
    }
    return result;
}

static bool record2binary(table *tab, u8 *bin, record *entry)
{
    *bin++ = 0xFF;/* valid = true */
    for (u32 i = 0; i < tab->colNum_u64; i++)
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
        bin += col->size_u8;
    }
    return true;
}

bool Recordmanager_insertRecord(table* tab, record* rcd)
{
    bool rc = true;
    vector<u32> vct;
    for (u32 k = 0; k < tab->colNum_u64; k++)
    {
        if (tab->col[k].unique_u8)
        {
            // use index to check unique
            if (tab->col[k].idxname[0] != 0)
            {
                Rule r;
                r.target = rcd->i[k].data;
                r.cmp = EQ;
                r.colNo = k;
                if (btree_select(tab->col[k].idxname, &r).size() != 0)
                {
                    fprintf(stderr, "Unique check failure at index %s.\n", tab->col[k].idxname);
                    return false;
                }
            }
            else
            {
                vct.push_back(k);
            }
        }
    }
    const u32 capacity = BLOCKSIZE / (tab->recordSize + 1);
    if (vct.size()!=0)  // check unique
    {
        u32 recordcount = 0;
        for (size_t block = 0; recordcount < tab->recordNum; block++)
        {
            move_window(&tab->buf, block);
            for (size_t i = 0; i < capacity; i++)
            {
                u8 *bin = tab->buf.win + i * (tab->recordSize + 1);
                record r = binary2record(tab, bin);
                if (r.valid == false)
                {
                    continue;
                }
                else
                {
                    recordcount++;
                }
                for (vector<u32>::iterator it = vct.begin(); it != vct.end(); ++it)
                {
                    if (Rule_cmp(tab->col[*it].type, &r.i[*it].data, &rcd->i[*it].data, EQ) == 1)
                    {
                        fprintf(stderr, "Unique check failure at %s.\n",tab->col[*it].name_str);
                        return false;
                    }
                }
            }
        }
    }
    
    // alloc new block
    if (tab->recordNum % capacity == 0)
    {
        newBlock(&(tab->buf));
    }
    // insert
    move_window(&(tab->buf), (++tab->recordNum - 1) / capacity);
    record2binary(tab, tab->buf.win + (tab->recordNum - 1) % capacity * (tab->recordSize + 1), rcd);
    tab->buf.dirty = true;
    sync_window(&tab->buf);
    // check index
    vector<u32> colNums;
    vector<const char*> idxnames;
    for (size_t i = 0; i < tab->colNum_u64; i++)
    {
        if (tab->col[i].idxname[0] != 0)
        {
            colNums.push_back(i);
            idxnames.push_back(tab->col[i].idxname);
        }
    }
    if (colNums.size() != 0)      // index exists update index
    {
        vector<const char *>::iterator idxnameit;
        vector<u32>::iterator idxit;
        for (idxnameit = idxnames.begin(), idxit = colNums.begin(); idxnameit != idxnames.end() && idxit != colNums.end(); ++idxnameit, ++idxit)
        {
            rc &= btree_insert(*idxnameit, &rcd->i[*idxit].data, (tab->recordNum - 1) / capacity);
        }
    }
    return rc;
}
//
//bool Recordmanager_insertRecordwithIndex(table *tab, record *rcd, vector<const char *> idxname, vector<u32> idx)
//{
//    bool rc = true;
//    vector<u32> vct;
//    for (u32 k = 0; k < tab->colNum_u64; k++)
//        if (tab->col[k].unique_u8)
//            vct.push_back(k);
//
//    u32 recordcount = 0;
//    u32 capacity = BLOCKSIZE / (tab->recordSize + 1);
//
//    for (size_t block = 0; recordcount < tab->recordNum; block++)
//    {
//        move_window(&tab->buf, block);
//        for (size_t i = 0; i < capacity; i++)
//        {
//            u8 *bin = tab->buf.win + i * (tab->recordSize + 1);
//            record *r = binary2record(tab, bin);
//            if (r->valid == false)
//            {
//                continue;
//            }
//            else
//            {
//                recordcount++;
//            }
//            for (vector<u32>::iterator it = vct.begin(); it != vct.end(); ++it)
//            {
//                if (Rule_cmp(tab->col[*it].type, &r->i[*it].data, &rcd->i[*it].data, EQ))
//                {
//                    fprintf(stderr, "Unique check failure.");
//                    return false;
//                }
//            }
//        }
//    }
//    if (tab->recordNum % capacity == 0)
//    {
//        newBlock(&(tab->buf));
//    }
//    move_window(&(tab->buf), (++tab->recordNum - 1) / capacity);
//    record2binary(tab, tab->buf.win + (tab->recordNum - 1) % capacity, rcd);
//    vector<const char *>::iterator idxnameit;
//    vector<u32>::iterator idxit;
//    for (idxnameit = idxname.begin(), idxit = idx.begin(); idxnameit != idxname.end() && idxit != idx.end(); ++idxnameit, ++idxit)
//    {
//        rc &= btree_insert(*idxnameit, &rcd->i[*idxit].data, (tab->recordNum - 1) / capacity);
//    }
//    tab->buf.dirty = true;
//    sync_window(&tab->buf);
//    return rc;
//}

/*1*/
int Recordmanager_deleteRecord(table *tab, Filter *filter)
{
    bool rc = true;
    int num = 0;
    const char * idxname = NULL;
    u32 colidx = -1;
    Rule *r = NULL;

    for (vector<Rule>::iterator it = filter->rules.begin(); it != filter->rules.end(); ++it)
    {
        if (tab->col[(*it).colNo].idxname[0] != 0 && (*it).cmp != NE) // index exists and is not NE condition
        {
            idxname = tab->col[(*it).colNo].idxname;
            colidx = (*it).colNo;
            r = &(*it);
            break;
        }
    }
    // check index
    vector<u32> colNums;
    vector<const char*> idxnames;
    for (size_t i = 0; i < tab->colNum_u64; i++)
    {
        if (tab->col[i].idxname[0] != 0)
        {
            colNums.push_back(i);
            idxnames.push_back(tab->col[i].idxname);
        }
    }
    u32 capacity = BLOCKSIZE / (tab->recordSize + 1);
    if (r != NULL) // delete with index
    {
        set<u32> blockset = btree_select(idxname, r);
        for (set<u32>::iterator it = blockset.begin(); it != blockset.end(); ++it)
        {
            move_window(&tab->buf, *it);
            for (size_t i = 0; i < capacity; i++)
            {
                u8 *bin = tab->buf.win + i * (tab->recordSize + 1);
                record r = binary2record(tab, bin);
                if (r.valid == false)
                {
                    continue;
                }
                bool res = true;
                for (vector<Rule>::iterator it = filter->rules.begin(); it != filter->rules.end(); ++it)
                {
                    Rule rule = (*it);
                    if (0 == Rule_cmp(r.i[rule.colNo].type, &r.i[rule.colNo].data, &rule.target, rule.cmp))
                    {
                        res = false;
                        break;
                    }
                }
                if (res)// match 
                {
                    *bin = 0;// valid = false
                    tab->buf.dirty = true;
                    num++;
                    if (colNums.size() != 0)      // index exists
                    {
                        vector<const char *>::iterator idxnameit;
                        vector<u32>::iterator idxit;
                        for (idxnameit = idxnames.begin(), idxit = colNums.begin(); idxnameit != idxnames.end() && idxit != colNums.end(); ++idxnameit, ++idxit)
                        {
                            btree_delete_node(*idxnameit, &r.i[*idxit].data);
                        }
                    }
                }
            }


        }
        tab->buf.dirty = true;
        sync_window(&tab->buf);
        return num;
    }
    for (size_t i = 0; i < tab->recordNum; i++)
    {
        move_window(&tab->buf, i / capacity);
        u8 *bin = tab->buf.win + (i % capacity) * (tab->recordSize + 1);
        record r = binary2record(tab, bin);
        if (r.valid == false)
        {
            continue;
        }
        bool res = true;
        for (vector<Rule>::iterator it = filter->rules.begin(); it != filter->rules.end(); ++it)
        {
            Rule rule = (*it);
            if (0 == Rule_cmp(r.i[rule.colNo].type, &r.i[rule.colNo].data, &rule.target, rule.cmp))
            {
                res = false;
                break;
            }
        }
        if (res)// match 
        {
            *bin = 0;// valid = false
            tab->buf.dirty = true;
            num++;
            if (colNums.size() != 0)      // index exists
            {
                vector<const char *>::iterator idxnameit;
                vector<u32>::iterator idxit;
                for (idxnameit = idxnames.begin(), idxit = colNums.begin(); idxnameit != idxnames.end() && idxit != colNums.end(); ++idxnameit, ++idxit)
                {
                    btree_delete_node(*idxnameit, &r.i[*idxit].data);
                }
            }
        }
    }
    tab->buf.dirty = true;
    sync_window(&tab->buf);

    return num;
}

vector<record> Recordmanager_selectRecord(table* tab, Filter* flt)
{
    // check index
    const char * idxname = NULL;
    u32 colidx = -1;
    Rule *r = NULL;
    const char * idxname2 = NULL;
    Rule *r2 = NULL;
    vector<record> result;
    u32 capacity = BLOCKSIZE / (tab->recordSize + 1);

    for (vector<Rule>::iterator it = flt->rules.begin(); it != flt->rules.end(); ++it)
    {
        if (tab->col[(*it).colNo].idxname[0] != 0 && (*it).cmp != NE) // index exists and is not NE condition
        {
            if (idxname!=NULL)
            {
                idxname2 = tab->col[(*it).colNo].idxname;
                r2 = &(*it);
                break;
            }
            idxname = tab->col[(*it).colNo].idxname;
            colidx = (*it).colNo;
            r = &(*it);
            //break;
        }
    }
    if (r != NULL) // select with index
    {
        set<u32> blockset1 = btree_select(idxname, r);
        // the code commented is for multi-index optimize 
        // but it seems not to improve performance as btree costs more time ToT
        //set<u32> blockset;
        //if (r2 != NULL)
        //{
        //    set<u32> blockset2 = btree_select(idxname2, r2);
        //    set_intersection(blockset1.begin(), blockset1.end(), blockset2.begin(), blockset2.end(), inserter(blockset,blockset.begin()));
        //}
        //else
        //{
        //    blockset = blockset1;
        //}
        // search in blocks
        for (set<u32>::iterator it = blockset1.begin(); it != blockset1.end(); ++it)
        {
            move_window(&tab->buf, *it);
            for (size_t i = 0; i < capacity; i++)
            {
                u8 *bin = tab->buf.win + i * (tab->recordSize + 1);
                record r = binary2record(tab, bin);
                if (r.valid == false)
                {
                    continue;
                }
                bool res = true;
                for (vector<Rule>::iterator it = flt->rules.begin(); it != flt->rules.end(); ++it)
                {
                    Rule rule = (*it);
                    if (0 == Rule_cmp(r.i[rule.colNo].type, &r.i[rule.colNo].data, &rule.target, rule.cmp))
                    {
                        res = false;
                        break;
                    }
                }
                if (res)// match 
                {
                    result.push_back(r);
                }
            }
        }
    }
    else    // without index
    {
        for (size_t i = 0; i < tab->recordNum; i++)
        {
            move_window(&tab->buf, i / capacity);
            u8 *bin = tab->buf.win + (i % capacity) * (tab->recordSize + 1);
            record r = binary2record(tab, bin);
            if (r.valid == false)
            {
                continue;
            }
            bool res = true;
            for (vector<Rule>::iterator it = flt->rules.begin(); it != flt->rules.end(); ++it)
            {
                Rule rule = (*it);
                if (0 == Rule_cmp(r.i[rule.colNo].type, &r.i[rule.colNo].data, &rule.target, rule.cmp))
                {
                    res = false;
                    break;
                }
            }
            if (res)// match 
            {
                result.push_back(r);
            }
        }
    }
    return result;
}

bool Recordmanager_addindex(table* tb,const char * idxname,u32 colidx)
{
    bool rc = true;
    u32 capacity = BLOCKSIZE / (tb->recordSize + 1);
    for (size_t i = 0; i < tb->recordNum; i++)
    {
        move_window(&tb->buf, i / capacity);
        u8 *bin = tb->buf.win + (i % capacity) * (tb->recordSize + 1);
        record r = binary2record(tb, bin);
        if (r.valid == false)
        {
            continue;
        }
        rc &= btree_insert(idxname, &r.i[colidx].data, i / capacity);
    }
    return rc;
}
