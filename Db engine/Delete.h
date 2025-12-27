#pragma once
#include <string>
#include <map>

void parseDeleteQuery(string cmd);
bool deleteFromTable(string tableName, string whereClause);
bool evaluateCondition(const map<string, string>& row, const string& condition, const vector<string>& columns);

