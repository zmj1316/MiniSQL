#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H
#include"MiniSQL.h"
#include"global.h"

bool Recordmanager_insertRecord(
	table* tab,
	record* rcd
	);

bool Recordmanager_insertRecordwithIndex(
	table *tab,
	record *rcd,
	vector<const char *> idxname,
	vector<u32> idx
	);

int Recordmanager_deleteRecord(
	table* tab,
	Filter *flt
	);

int Recordmanager_deleteRecordwithIndex(
	table* tab,
	Filter *flt,
	vector<const char *> idxname,
	Rule *rule,
	vector<u32> idx
	);

vector<record> Recordmanager_selectRecord(
	table* tab,
	Filter* flt
	);

vector<record> Recordmanager_selectRecordwithIndex(table* tab,
	Filter *flt,
	const char *idxname,
	Rule *rule
	);

#endif