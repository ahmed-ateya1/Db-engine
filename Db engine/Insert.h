#pragma once
#include <string>
using namespace std;

struct InsertQuery
{
    string tableName;
    vector<string> columns;            
    vector<vector<string>> valuesList;  
    bool hasColumns;                  
};

InsertQuery parseInsertQuery(const string& query);
bool executeInsert(const InsertQuery& iq);
void handleInsert(const string& query);
