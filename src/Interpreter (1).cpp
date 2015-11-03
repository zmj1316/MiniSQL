#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include"MiniSQL.h"
//#define INT 1
//#define CHAR 2
//#define FLOAT 3

using namespace std;

string sql[110];
int op;

int num(string str)
{
    op = 0;
    if (str == "create")
    {
        op = 1;
    }else
    if (str == "drop")
    {
        op = 2;
    }else
    if (str == "select")
    {
        op = 3;
    }else
    if (str == "insert")
    {
        op = 4;
    }else
    if (str == "delete")
    {
        op = 5;
    }else
        if (str == "quit;")
    {
        op = 6;
    }else
    if (str == "execfile")
    {
        op = 7;
    }

    return op;
}

bool Init()
{
    int i = 0;
    string str;
    cin >> str;
    if(num(str) == 0) return false;
    int j = str.find_first_of(";");
    while(j < 0)
    {
        sql[i++] = str;
        cin >> str;
        j = str.find_first_of(";");
    }
    sql[i] = str;
    cout << "finish read" << endl;
    return true;
}

int todigit(string str)
{
    int k = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        k = k * 10 + str[i] - '0';
        i++;
    }
    return k;
}

void transfer(char *p, string str)
{
    size_t i;
    for (i = 0; i < str.length(); i++)
    {
        *(p + i) = str.at(i);
    }
    *(p + i) = '\0';
}

string rtransfer(char *p)
{
    string str = "";
    int i = 0;
    while (*(p + i) != '\0')
    {
        str = str + *(p + i);
        i++;
    }
    return str;
}

void create_table()
{
    if (miniSQL_connectTable(sql[2].c_str()) == NULL)
    {
        if (sql[3] != "(")
        {
            cout << "Wrong Instruction Format!" << endl;
            return;
        }

        int k = 4, i = 0;
        table tab;
        transfer(tab.name_str, sql[2]);
        tab.recordSize = 0;
        tab.recordNum = 0;
        while (sql[k] != "primary")
        {
            transfer(tab.col[i].name_str, sql[k]);
            if (sql[k + 1].at(sql[k + 1].length() - 1) == ',')
            {
                tab.col[i].unique_u8 = false;
                sql[k + 1].erase(sql[k + 1].length() - 1, 1);
            }
            else
            {
                if (sql[k + 2] == "unique,")
                    tab.col[i].unique_u8 = true;
            }

            if (sql[k + 1].substr(0, 3) == "int")
            {
                tab.col[i].type = INT;
                tab.col[i].size_u8 = 4;
            }
            else
            if (sql[k + 1].substr(0, 5) == "float")
            {
                tab.col[i].type = FLOAT;
                tab.col[i].size_u8 = 4;
            }
            else
            if (sql[k + 1].substr(0, 4) == "char")
            {
                tab.col[i].type = CHAR;
                tab.col[i].size_u8 = todigit(sql[k + 1].substr(6, sql[k + 1].length() - 6));
            }

            if (tab.col[i].unique_u8 == true) k = k + 3;
            else k = k + 2;
            tab.recordSize += tab.col[i].size_u8;
            i++;
        }
        if (sql[k] == "primary")
        {
            if (sql[k + 1] == "key")
                if (sql[k + 2] == "(")
                    if (sql[k + 4] == ")")
                    {
                        for (int j = 0; j < i; j++)
                        {
                            string name = rtransfer(tab.col[j].name_str);
                            if (name == sql[k + 3])
                            {
                                tab.primarykey_u8 = j;
                                //miniSQL_createIndex(tab.col[j].name_str + 'idx', tab.name_str, tab.col[j].name_str);
                                tab.colNum_u64 = i;
                                if (miniSQL_createTable(&tab) == true)
                                {
                                    cout << "Create Table Successful!" << endl;
                                    return;
                                }
                            }
                        }
                    }
        }
    }
    else
    {
        cout << "Table" << sql[2] << "is Exist!" << endl;
        return;
    }
    cout << "Wrong Instruction Format!" << endl;
}

void create_index()
{
    table *p;

    if (sql[4] != "")
    {
        if ((p = miniSQL_connectTable(sql[4].c_str())) != NULL)
        {
            if (sql[3] == "on")
                if (sql[5] == "(")
                    if (sql[7] == ");")
                    {
                        //cout << "1" << endl;
                        if (miniSQL_createIndex(p, sql[4].c_str(), sql[6].c_str()) == true)
                        {
                            cout << "Create Index Successful!" << endl;
                            return;
                        }
                    }
            miniSQL_disconnectTable(p);
        }
        else
        {
            cout << "Table " << sql[4] << " is not exist!" << endl;
            return;
        }
    }
    
    cout << "Wrong Instruction Format!" << endl;
}

void create()
{
    if (sql[1] == "table") create_table(); else
    if (sql[1] == "index") create_index(); else
        cout << "Wrong Instruction Format!" << endl;
}

void drop_table()
{
    table *p;
    sql[2].erase(sql[2].length() - 1, 1);
    if (sql[3] == "")
    {
        if ((p = miniSQL_connectTable(sql[2].c_str())) != NULL)
        {
            if (miniSQL_dropTable(p) == true)
            {
                cout << "Drop Table Successful!" << endl;
                return;
            }
        }
        else
        {
            cout << "Table" << sql[2] << "is Exist!" << endl;
            return;
        }
    }

    cout << "Wrong Instruction Format!" << endl;
}

void drop_index()
{
    sql[2].erase(sql[2].length() - 1, 1);
    if (sql[3] == "")
    {
        if (miniSQL_dropIndex(sql[2].c_str()) == true)
        {
            cout << "Drop Index Successful!" << endl;
            return;
        }
        else
        {
            cout << "Index" << sql[2] << "is not Exist!" << endl;
            return;
        }
    }
    cout << "Wrong Insruction Format!" << endl;
}

void drop()
{
    if (sql[1] == "table") drop_table(); else
    if (sql[1] == "index") drop_index(); else
        cout << "Wrong Instruction Format!" << endl;
}

void select()
{
    if (sql[1] == "*")
        if (sql[2] == "from")
        {
            table *p;
            if (sql[3].find_first_of(';') != -1)
            {
                sql[3] = sql[3].substr(0, sql[3].find_first_of(';'));
                if ((p = miniSQL_connectTable(sql[3].c_str())) != NULL)
                {
                    Filter f;
                    auto r = miniSQL_select(p, &f);
                    if (r.size() == 0)
                    {
                        cout << "No Such Record!" << endl;
                        miniSQL_disconnectTable(p);
                        return;
                    }
                    else
                    {
                        //							int i;
                        //							for (i = 0; i<)
                        miniSQL_disconnectTable(p);
                        return;
                    }
                }
                else
                {
                    cout << "Table " << sql[3] << " is not exist!" << endl;
                    return;
                }
            }
            else
            {
                if ((p = miniSQL_connectTable(sql[3].c_str())) != NULL)
                {
                    if (sql[4] == "where")
                    {
                        Filter f;
                        int i = 0, j = 5;
                        while ((sql[j + 2].at(sql[j + 2].length() - 1) != ';') && (sql[j + 3] == "and"))
                        {
                            size_t k = 0;
                            for (k = 0; k < p->colNum_u64; k++)
                            {
                                string name;
                                name = rtransfer(p->col[k].name_str);
                                if (name == sql[j])
                                {
                                    f.rules[i].colNo = k;
                                    break;
                                }
                            }
                            if (sql[j + 1] == "=") f.rules[i].cmp = EQ; else
                                if (sql[j + 1] == "<>") f.rules[i].cmp = NE; else
                                    if (sql[j + 1] == "<") f.rules[i].cmp = LT; else
                                        if (sql[j + 1] == ">") f.rules[i].cmp = GT; else
                                            if (sql[j + 1] == "<=") f.rules[i].cmp = LE; else
                                                if (sql[j + 1] == ">=") f.rules[i].cmp = GE;
                            if (sql[j + 3].at(1) == '\'')
                            {
                                sql[j + 3].erase(0, 1);
                                sql[j + 3].erase(sql[j + 3].length() - 1, 1);
                            }
                            cout << sql[j + 3] << "*" << endl;
                            switch (p->col[k].type)
                            {
                            case INT:
                            {
                                f.rules[k].target.i = todigit(sql[j + 3]);
                                //cout << f.rules[k].target.i << endl;
                                break;
                            }
                            case CHAR:
                            {
                                transfer(f.rules[k].target.str, sql[j + 3]);
                                //cout << f.rules[k].target.str << endl;
                                break;
                            }
                            case FLOAT:
                            {
                                char a[100], **p = NULL;
                                transfer(a, sql[j + 3]);
                                f.rules[k].target.f = strtod(a, p);
                                break;
                            }
                            }
                            Filter f;
                            auto r = miniSQL_select(p, &f);
                            if (r.size() == 0)
                            {
                                cout << "No Such Record!" << endl;
                                miniSQL_disconnectTable(p);
                                return;
                            }
                            else
                            {
                                //							int i;
                                //							for (i = 0; i<)
                                miniSQL_disconnectTable(p);
                                return;
                            }
                        }
                    }
                    
                }
                else
                {
                    cout << "Table " << sql[2] << " is not exist!" << endl;
                    return;
                }
            }				
        }
            
    cout << "Wrong Instruction Format!" << endl;
}

void insert()
{
    if (sql[1] == "into")
    {
        table *p;
        if ((p = miniSQL_connectTable(sql[2].c_str())) != NULL)
        {
            if (sql[3] == "values")
            {
                int k = 0;
                record rcd;
                while (sql[4].find_first_of(",") != -1)
                {
                    rcd.i[k].type = p->col[k].type;
                    string sub = sql[4].substr(1, sql[4].find_first_of(",") - 1);
                    if (sub.at(1) == '\'')
                    {
                        sub.erase(0, 1);
                        sub.erase(sub.length() - 1, 1);
                    }
                    cout << sub << "*" << endl;
                    switch (p->col[k].type)
                    {
                        case INT:
                        {
                            rcd.i[k].data.i = todigit(sub);
                            break;
                        }
                        case CHAR:
                        {
                            transfer(rcd.i[k].data.str, sub);
                            break;
                        }
                        case FLOAT:
                        {
                            char a[100], **p = NULL;
                            transfer(a, sub);
                            rcd.i[k].data.f = strtod(a, p);
                            break;
                        }
                    }
                    sql[4].erase(1, sql[4].find_first_of(","));
                    k++;
                }
                rcd.i[k].type = p->col[k].type;
                string sub = sql[4].substr(1, sql[4].length() - 3);
                if (sub.at(1) == '\'')
                {
                    sub.erase(0, 1);
                    sub.erase(sub.length() - 1, 1);
                }
                switch (p->col[k].type)
                {
                case INT:
                {
                    rcd.i[k].data.i = todigit(sub);
                    break;
                }
                case CHAR:
                {
                    transfer(rcd.i[k].data.str, sub);
                    break;
                }
                case FLOAT:
                {
                    char a[100], **p = NULL;
                    transfer(a, sub);
                    rcd.i[k].data.f = strtod(a, p);
                    break;
                }
                }
                if (miniSQL_insert(p, &rcd))
                {
                    cout << "Insert Successful!" << endl;
                    miniSQL_disconnectTable(p);
                    return;
                }
            }
            miniSQL_disconnectTable(p);
        }
    }

    cout << "Wrong Instruction Format!" << endl;
}

void delet()
{
    if (sql[1] == "from")
    {
        table *p;
        if (sql[2].find_first_of(';') > -1)
        {
            sql[2] = sql[2].substr(0, sql[2].find_first_of(';'));
            if ((p = miniSQL_connectTable(sql[2].c_str())) != NULL)
            {
                cout << "1" << endl;
                Filter f, *r;
                int num;
                if ((num = miniSQL_delete(p, &f)) != 0)
                {
                    cout << "Delete " << num << " Record!" << endl;
                    miniSQL_disconnectTable(p);
                    return;
                }
                else
                {
                    cout << "No record is deleted!" << endl;
                    miniSQL_disconnectTable(p);
                    return;
                }
            }
            else
            {
                cout << "Table " << sql[2] << " is not exist!" << endl;
                return;
            }
        }
        else
        {
            if ((p = miniSQL_connectTable(sql[2].c_str())) != NULL)
            {
//				cout << "1" << endl;
                if (sql[3] == "where")
                {
                    Filter f;
                    int i = 0, j = 4;
                    while ((sql[j + 2].at(sql[j + 2].length() - 1) != ';') && (sql[j + 3] == "and"))
                    {
                        size_t k = 0;
                        for (k = 0; k < p->colNum_u64; k++)
                        {
                            string name;
                            name = rtransfer(p->col[k].name_str);
                            if (name == sql[j])
                            {
                                f.rules[i].colNo = k;
                                break;
                            }
                        }
                        if (sql[j + 1] == "=") f.rules[i].cmp = EQ; else
                            if (sql[j + 1] == "<>") f.rules[i].cmp = NE; else
                                if (sql[j + 1] == "<") f.rules[i].cmp = LT; else
                                    if (sql[j + 1] == ">") f.rules[i].cmp = GT; else
                                        if (sql[j + 1] == "<=") f.rules[i].cmp = LE; else
                                            if (sql[j + 1] == ">=") f.rules[i].cmp = GE;
                        if (sql[j + 3].at(1) == '\'')
                        {
                            sql[j + 3].erase(0, 1);
                            sql[j + 3].erase(sql[j + 3].length() - 1, 1);
                        }
                        cout << sql[j + 3] << "*" << endl;
                        switch (p->col[k].type)
                        {
                        case INT:
                        {
                            f.rules[k].target.i = todigit(sql[j + 3]);
                            //cout << f.rules[k].target.i << endl;
                            break;
                        }
                        case CHAR:
                        {
                            transfer(f.rules[k].target.str, sql[j + 3]);
                            //cout << f.rules[k].target.str << endl;
                            break;
                        }
                        case FLOAT:
                        {
                            char a[100], **p = NULL;
                            transfer(a, sql[j + 3]);
                            f.rules[k].target.f = strtod(a, p);
                            break;
                        }
                        }
                        int num;
                        if ((num = miniSQL_delete(p, &f)) != 0)
                        {
                            cout << "Delete " << num << " Record!" << endl;
                            miniSQL_disconnectTable(p);
                            return;
                        }
                        else
                        {
                            cout << "No record is deleted!" << endl;
                            miniSQL_disconnectTable(p);
                            return;
                        }
                    }
                }
                miniSQL_disconnectTable(p);
            }
            else
            {
                cout << "Table " << sql[2] << " is not exist!" << endl;
                return;
            }
        }


    }
    cout << "Wrong Instruction Format!" << endl;
}

void quit()
{
    cout << "Thanks for Using MiniSQL!" << endl;
    getchar(); getchar();
    exit(0);
}

void execfile()
{
    string filename = sql[2].substr(0, sql[2].find_first_of(';'));
    ifstream in(filename);
    string str;
    int i = 0;

    in >> str;
//	if (num(str) == 0) return false;
    int j = str.find_first_of(";");
    while (j < 0)
    {
        sql[i++] = str;
        cin >> str;
        j = str.find_first_of(";");
    }
    sql[i] = str;
//	cout << "finish read" << endl;
    switch (op)
    {
    case 1:
    {
        create();
        break;
    }
    case 2:
    {
        drop();
        break;
    }
    case 3:
    {
        select();
        break;
    }
    case 4:
    {
        insert();
        break;
    }
    case 5:
    {
        delet();
        break;
    }
    case 6:
    {
        quit();
        break;
    }
    case 7:
    {
        execfile();
        break;
    }
    }
}

int main()
{
    while (1)
    {
        for (int i = 0; i < 100; i++)
        {
            sql[i] = "";
        }
        cout << "Pleast input SQL:" << endl;
        if (!Init())
        {
            cout << "Wrong Instruction Format!" << endl;
            continue;
        }

        switch (op)
        {
            case 1: 
            {
                create();
                break;
            }
            case 2:
            {
                drop();
                break;
            }
            case 3:
            {
                select();
                break;
            }
            case 4:
            {
                insert();
                break;
            }
            case 5:
            {
                delet();
                break;
            }
            case 6:
            {
                quit();
                break;
            }
            case 7:
            {
                execfile();
                break;
            }
        }
    }
}
