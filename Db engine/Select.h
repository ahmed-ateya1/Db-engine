#pragma once
#include <string>
#include <vector>
using namespace std;


void selectAll(string tableName);

void selectColumns(string tableName, vector<string> wanted);

void selectColumnsWhere(string tablename, vector<string>wanted, string whereColCond, string whereVal);

void parseSelect(string cmd);
