#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <map>
#include "utils.h"
#include "delete.h"

void parseDeleteQuery(string cmd)
{
    if (!trim(cmd).empty() && trim(cmd).back() == ';')
        cmd.pop_back();

    cmd = trim(cmd);
    string upper = cmd;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

	upper = trim_Middle(upper);
    if (upper.find("DELETE") != 0)
    {
        setColor(12);
        cout << "Error: Command must start with DELETE.\n";
        setColor(15);
        return;
    }

    int fromPos = upper.find("FROM");
    if (fromPos == string::npos)
    {
        setColor(12);
        cout << "Error: Missing FROM clause.\n";
        setColor(15);
        return;
    }

    int wherePos = upper.find("WHERE");
    string tableName;
    string whereClause = "";

    if (wherePos != string::npos)
    {
        tableName = trim(cmd.substr(fromPos + 4, wherePos - fromPos - 4));
        whereClause = trim(cmd.substr(wherePos + 5));
    }
    else
    {
        tableName = trim(cmd.substr(fromPos + 4));
    }

    int deleteEnd = fromPos;
    string betweenDeleteFrom = trim(cmd.substr(6, deleteEnd - 6));
    if (betweenDeleteFrom == "*")
    {
    }
    else if (!betweenDeleteFrom.empty())
    {
        setColor(12);
        cout << "Error: Invalid DELETE syntax. Use 'DELETE FROM table' or 'DELETE * FROM table'.\n";
        setColor(15);
        return;
    }

    if (tableName.empty())
    {
        setColor(12);
        cout << "Error: Missing table name.\n";
        setColor(15);
        return;
    }

    deleteFromTable(tableName, whereClause);
}

bool deleteFromTable(string tableName, string whereClause)
{
    TableData tableData = readTable(tableName);
    if (tableData.columns.empty())
    {
        setColor(12);
        cout << "Error: Unable to read table structure.\n";
        setColor(15);
        return false;
    }

    ifstream inFile(tableName + ".csv");
    if (!inFile.good())
    {
        setColor(12);
        cout << "Error: Table '" << tableName << "' not found.\n";
        setColor(15);
        return false;
    }

    vector<string> lines;
    string line;

    getline(inFile, line);
    lines.push_back(line);

    int deletedCount = 0;
    int totalRows = 0;
    bool deleteAll = whereClause.empty();

    while (getline(inFile, line))
    {
        line = trim(line);
        if (line.empty())
            continue;

        totalRows++;
        bool shouldDelete = deleteAll;

        if (!deleteAll)
        {
            vector<string> values = split(line, ',');
            map<string, string> row;

            for (size_t i = 0; i < tableData.columns.size() && i < values.size(); i++)
                row[tableData.columns[i]] = trim(values[i]);

            shouldDelete = evaluateCondition(row, whereClause, tableData.columns);
        }

        if (shouldDelete)
            deletedCount++;
        else
            lines.push_back(line);
    }

    inFile.close();

    ofstream outFile(tableName + ".csv", ios::trunc);
    if (!outFile.good())
    {
        setColor(12);
        cout << "Error: Unable to write to table file.\n";
        setColor(15);
        return false;
    }

    for (size_t i = 0; i < lines.size(); i++)
    {
        outFile << lines[i];
        if (i < lines.size() - 1)
            outFile << "\n";
    }
    outFile.close();

    if (deletedCount == 0)
    {
        setColor(14);
        cout << "No rows matched the condition. 0 row(s) deleted.\n";
        setColor(15);
    }
    else
    {
        setColor(10);
        cout << deletedCount << " row(s) deleted from table '" << tableName << "'.\n";
        setColor(15);
    }

    return true;
}

bool evaluateCondition(const map<string, string>& row, const string& condition, const vector<string>& columns)
{
    string cond = trim(condition);

    string ops[] = { ">=", "<=", "!=", "=", ">", "<" };
    string foundOp = "";
    size_t opPos = string::npos;

    for (const string& op : ops)
    {
        size_t pos = cond.find(op);
        if (pos != string::npos)
        {
            foundOp = op;
            opPos = pos;
            break;
        }
    }

    if (opPos == string::npos)
    {
        setColor(12);
        cout << "Error: Invalid WHERE condition format.\n";
        setColor(15);
        return false;
    }

    string colName = trim(cond.substr(0, opPos));
    string value = trim(cond.substr(opPos + foundOp.length()));

    if (!value.empty() && (value.front() == '\'' || value.front() == '"'))
        value = value.substr(1);
    if (!value.empty() && (value.back() == '\'' || value.back() == '"'))
        value.pop_back();

    if (row.find(colName) == row.end())
    {
        setColor(12);
        cout << "Warning: Column '" << colName << "' not found in table.\n";
        setColor(15);
        return false;
    }

    string rowValue = row.at(colName);

    if (foundOp == "=")
        return rowValue == value;
    else if (foundOp == "!=")
        return rowValue != value;
    else if (foundOp == ">")
    {
        try { return stod(rowValue) > stod(value); }
        catch (...) { return rowValue > value; }
    }
    else if (foundOp == "<")
    {
        try { return stod(rowValue) < stod(value); }
        catch (...) { return rowValue < value; }
    }
    else if (foundOp == ">=")
    {
        try { return stod(rowValue) >= stod(value); }
        catch (...) { return rowValue >= value; }
    }
    else if (foundOp == "<=")
    {
        try { return stod(rowValue) <= stod(value); }
        catch (...) { return rowValue >= value; }
    }

    return true;
}
