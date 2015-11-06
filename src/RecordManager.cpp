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
                    fprintf(stderr, "Unique check failure at index %s\n.", tab->col[k].idxname);
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
                        fprintf(stderr, "%s || %s \n", r.i[*it].data.str, rcd->i[*it].data.str);
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
    for (size_t i = 0; i < tab->recordNum; i++)
    {
        move_window(&tab->buf, i / capacity);
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
    tab->buf.dirty = true;
    sync_window(&tab->buf);

    return num;
}

///*3*/
//int Recordmanager_deleteRecordwithIndex(
//    table *tab, Filter *flt, vector<const char *> idxname, vector<u32> idx)
//{
//    int num = 0;
//    record *rcd;
//    int capacity = BLOCKSIZE / (tab->recordSize + 1);
//    u32 recordcount = 0;
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
//            bool res = true;
//            for (vector<Rule>::iterator it = filter->rules.begin(); it != filter->rules.end(); ++it)
//            {
//                Rule rule = (*it);
//                if (0 == Rule_cmp(r->i[rule.colNo].type, &r->i[rule.colNo].data, &rule.target, rule.cmp))
//                {
//                    res = false;
//                    break;
//                }
//            }
//            if (res)
//            {
//                r->valid = false;
//                record2binary(tab, bin, r);
//                tab->buf.dirty = true;
//            }
//        }
//    }
//    tab->buf.dirty = true;
//    sync_window(&tab->buf);
//    return num;
//}
///*2*/
//int Recordmanager_deleteRecordwithIndex(
//    table *tab, Filter *flt, vector<const char *> idxname, Rule rule)
//{
//    int num = 0;
//    record *rcd;
//    vector<Rule>::iterator fiter;
//    vector<const char *>::iterator iter;
//    vector<u32>::iterator iter2;
//    int c = BLOCKSIZE / (tab->recordSize + 1);
//    for (int i = 1; i <= (tab->recordNum - 1) / c + 1; i++)
//    {
//        move_window(&(tab->buf), i);
//        for (int j = 0; j < c; j++)
//        {
//            rcd = binary2record(tab, tab->buf.win + j * tab->recordSize);
//            if (rcd->valid == true)
//            {
//                for (fiter = flt->rules.begin(); fiter != flt->rules.end(); fiter++)
//                {
//                    if (Rule_cmp(rcd->i[fiter->colNo].type, &rcd->i[fiter->colNo].data, &fiter->target, fiter->cmp))
//                    {
//                        rcd->valid = false;
//                        record2binary(tab, tab->buf.win + j * tab->recordSize, rcd);
//                        num++;
//                    }
//                }
//            }
//        }
//    }
//    for (iter = idxname.begin(), iter2 = idx.begin(); iter != idxname.end(); iter++, iter2++)
//        btree_delete_node(*iter, &rcd->i[*iter2].data);
//
//    return num;
//}

vector<record> Recordmanager_selectRecord(table* tab, Filter* flt)
{
    // check index
    const char * idxname = NULL;
    u32 colidx = -1;
    Rule *r = NULL;
    vector<record> result;
    u32 capacity = BLOCKSIZE / (tab->recordSize + 1);

    for (vector<Rule>::iterator it = flt->rules.begin(); it != flt->rules.end(); ++it)
    {
        if (tab->col[(*it).colNo].idxname[0] != 0) // index exists
        {
            idxname = tab->col[(*it).colNo].idxname;
            colidx = (*it).colNo;
            r = &flt->rules[colidx];
        }
    }
    if (r!=NULL) // select with index
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
    return result;
}
//
//vector<record> Recordmanager_selectRecordwithIndex(table* tab, Filter *flt, const char *idxname, Rule *rule)
//{
//    record *rcd;
//    vector<record> result;
//    vector<Rule>::iterator iter;
//    set<u32> blockset = btree_select(idxname, rule);
//    set<u32>::iterator siter;
//    int c = BLOCKSIZE / (tab->recordSize + 1);
//    for (siter = blockset.begin(); siter != blockset.end(); siter++)
//    {
//        move_window(&(tab->buf), *siter);
//        for (int j = 0; j < c; j++)
//        {
//            rcd = binary2record(tab, tab->buf.win + j * tab->recordSize);
//            if (flt->rules.size() != 0)
//                for (iter = flt->rules.begin(); iter != flt->rules.end(); iter++)
//                {
//                    if (Rule_cmp(rcd->i[iter->colNo].type, &rcd->i[iter->colNo].data, &iter->target, iter->cmp))
//                        result.push_back(*rcd);
//                }
//        }
//    }
//    tab->buf.dirty = true;
//    return result;
//}
bool Recordmanager_addindex(table* tb,const char * idxname,u32 colidx)
{
    bool rc = true;
    u32 capacity = BLOCKSIZE / (tb->recordSize + 1);
    for (size_t i = 0; i < tb->recordNum; i++)
    {
        move_window(&tb->buf, i / capacity);
        u8 *bin = tb->buf.win + i * (tb->recordSize + 1);
        record r = binary2record(tb, bin);
        if (r.valid == false)
        {
            continue;
        }
        rc &= btree_insert(idxname, &r.i[colidx].data, i / capacity);
    }
    return rc;
}