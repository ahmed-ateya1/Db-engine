#pragma once
#ifndef UPDATE_H
#define UPDATE_H
#include <string>
#include <unordered_map>
#include "utils.h"


struct UpdateQuery
{
    string tableName;
    unordered_map<string, string> setValues;
    string whereColumn;
    string whereOperator;
    string whereValue;
    bool hasWhere;
};

// Main handler
UpdateQuery parseUpdateQuery(const string& query);
bool evaluateCondition(const string& cellValue, const string& op, const string& targetValue);
bool executeUpdate(const UpdateQuery& uq);
void handleUpdate(const string& query);
int getColumnIndex(const TableData& table, const string& columnName);

#endif