#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include "select.h"
#include <unordered_map>
#include "utils.h"
using namespace std;

void selectAll(string tableName)
{
    TableData t = readTable(tableName);

    if (t.columns.empty())
        return;

    setColor(11);
    for (auto c : t.columns)
        cout << c << "\t";
    cout << "\n";
    setColor(15);

    for (auto row : t.rows)
    {
        for (auto val : row)
            cout << val << "\t";
        cout << "\n";
    }
}

void selectColumns(string tableName, vector<string> wanted)
{
    TableData t = readTable(tableName);
    if (t.columns.empty())
        return;

    unordered_map<string, int> colIndex;
    for (int i = 0; i < t.columns.size(); i++)
        colIndex[t.columns[i]] = i;

    setColor(11);
    for (auto c : wanted)
        cout << c << "\t";
    cout << endl;
    setColor(15);

    for (auto row : t.rows)
    {
        for (auto c : wanted)
            cout << row[colIndex[c]] << "\t";
        cout << "\n";
    }
}

bool compareValues(const string& leftVal, const string& rightVal, const string& op)
{
    bool isNumeric = true;
    double leftNum = 0, rightNum = 0;

    try
    {
        size_t pos1, pos2;
        leftNum = stod(leftVal, &pos1);
        rightNum = stod(rightVal, &pos2);

        if (pos1 != leftVal.length() || pos2 != rightVal.length())
            isNumeric = false;
    }
    catch (...)
    {
        isNumeric = false;
    }

    if (op == "=")
    {
        return leftVal == rightVal;
    }
    else if (op == "!=")
    {
        return leftVal != rightVal;
    }
    else if (op == ">")
    {
        if (isNumeric)
            return leftNum > rightNum;
        else
            return leftVal > rightVal;
    }
    else if (op == "<")
    {
        if (isNumeric)
            return leftNum < rightNum;
        else
            return leftVal < rightVal;
    }
    else if (op == ">=")
    {
        if (isNumeric)
            return leftNum >= rightNum;
        else
            return leftVal >= rightVal;
    }
    else if (op == "<=")
    {
        if (isNumeric)
            return leftNum <= rightNum;
        else
            return leftVal <= rightVal;
    }

    return false;
}

void selectColumnsWhere(string tablename, vector<string> wanted, string whereColCond, string whereVal, string op)
{
    TableData t = readTable(tablename);

    if (t.columns.empty())
    {
        return;
    }

    unordered_map<string, int> colIndex;
    for (int i = 0; i < t.columns.size(); i++)
        colIndex[t.columns[i]] = i;

    if (colIndex.find(whereColCond) == colIndex.end())
    {
        setColor(12);
        cout << "Error: Column '" << whereColCond << "' not found.\n";
        setColor(15);
        return;
    }

    int whereIdx = colIndex[whereColCond];

    setColor(11);
    for (auto c : wanted)
        cout << c << "\t";
    cout << endl;
    setColor(15);

    for (auto row : t.rows)
    {
        if (compareValues(row[whereIdx], whereVal, op))
        {
            for (auto& c : wanted)
                cout << row[colIndex[c]] << "\t";
            cout << "\n";
        }
    }
}

string extractOperator(const string& cond, int& opPos, int& opLen)
{
    if (cond.find(">=") != string::npos)
    {
        opPos = cond.find(">=");
        opLen = 2;
        return ">=";
    }
    if (cond.find("<=") != string::npos)
    {
        opPos = cond.find("<=");
        opLen = 2;
        return "<=";
    }
    if (cond.find("!=") != string::npos)
    {
        opPos = cond.find("!=");
        opLen = 2;
        return "!=";
    }

    if (cond.find("=") != string::npos)
    {
        opPos = cond.find("=");
        opLen = 1;
        return "=";
    }
    if (cond.find(">") != string::npos)
    {
        opPos = cond.find(">");
        opLen = 1;
        return ">";
    }
    if (cond.find("<") != string::npos)
    {
        opPos = cond.find("<");
        opLen = 1;
        return "<";
    }

    return "";
}

void parseSelect(string cmd)
{
    if (!cmd.empty() && cmd.back() == ';')
        cmd.pop_back();

    string upper = cmd;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    upper = trim_Middle(upper);

    if (upper.find("SELECT") != 0)
    {
        setColor(12);
        cout << "Error: SELECT syntax.\n";
        setColor(15);
        return;
    }

    int fromPos = upper.find("FROM");
    if (fromPos == -1)
    {
        setColor(12);
        cout << "Error: Missing FROM.\n";
        setColor(15);
        return;
    }

    string colsPart = trim(cmd.substr(6, fromPos - 6));
    vector<string> wanted;

    if (colsPart == "*")
    {
        wanted = { "*" };
    }
    else
    {
        wanted = split(colsPart, ',');
    }

    int wherePos = upper.find("WHERE");
    string tableName;

    if (wherePos == -1)
    {
        tableName = trim(cmd.substr(fromPos + 4));

        if (wanted[0] == "*")
            selectAll(tableName);
        else
            selectColumns(tableName, wanted);

        return;
    }

    tableName = trim(cmd.substr(fromPos + 4, wherePos - (fromPos + 4)));

    string cond = trim(cmd.substr(wherePos + 5));

    int opPos = -1;
    int opLen = 0;
    string op = extractOperator(cond, opPos, opLen);

    if (op.empty())
    {
        setColor(12);
        cout << "Error: Invalid WHERE clause. Expected operator (=, !=, >, <, >=, <=).\n";
        setColor(15);
        return;
    }

    string whereCol = trim(cond.substr(0, opPos));
    string whereVal = trim(cond.substr(opPos + opLen));

    if (whereCol.empty() || whereVal.empty())
    {
        setColor(12);
        cout << "Error: Invalid WHERE clause format.\n";
        setColor(15);
        return;
    }

    if (wanted[0] == "*")
    {
        TableData t = readTable(tableName);
        wanted = t.columns;
    }

    selectColumnsWhere(tableName, wanted, whereCol, whereVal, op);
}