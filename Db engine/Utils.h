#pragma once
#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector> 
using namespace std;

struct Column
{
    string type;
    string name;
    int length = -1;
};
struct Row
{
    vector<string> data;
};
class Table
{
public:
    string name;
    vector<Row> Rows;
    vector<Column> Columns;

    Table(string n, vector<Column> c) : name(n), Columns(c) {}

    void saveToFile()
    {
        ofstream f(name + ".csv");
        for (size_t i = 0; i < Columns.size(); i++)
        {
            f << Columns[i].name << ":" << Columns[i].type;
            if (i < Columns.size() - 1)
                f << ",";
        }
        f << "\n";
    }
};

struct TableData {
    vector<string> columns;
    vector<string> types;
    vector<int> lengths;
    vector<bool> notNull;
    int primaryKeyIndex = -1;
    vector<vector<string>> rows;
};

void setColor(int colorCode);

string trim(string s);
string trim_Middle(string s);

void toUpper(string& s);

vector<string> split(string s, char delimiter);

TableData readTable(string tableName);

bool writeTable(string tableName, TableData data);

#endif