#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "update.h"
#include "utils.h"
using namespace std;

UpdateQuery parseUpdateQuery(const string& query)
{
    UpdateQuery uq;
    uq.hasWhere = false;

    string cmd = query;
    if (!cmd.empty() && cmd.back() == ';')
        cmd.pop_back();

    string upper = cmd;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    upper = trim_Middle(upper);
    size_t updatePos = upper.find("UPDATE");
    size_t setPos = upper.find("SET");

    if (updatePos == string::npos || setPos == string::npos)
    {
        setColor(12);
        cout << "Error: Invalid UPDATE syntax.\n";
        setColor(15);
        return uq;
    }

    uq.tableName = trim(cmd.substr(updatePos + 6, setPos - updatePos - 6));

    size_t wherePos = upper.find("WHERE");
    size_t setEnd = (wherePos != string::npos) ? wherePos : cmd.length();

    string setClause = trim(cmd.substr(setPos + 3, setEnd - setPos - 3));

    vector<string> assignments = split(setClause, ',');

    for (const string& assignment : assignments)
    {
        size_t eqPos = assignment.find('=');
        if (eqPos == string::npos)
        {
            setColor(12);
            cout << "Error: Invalid SET clause.\n";
            setColor(15);
            return uq;
        }

        string colName = trim(assignment.substr(0, eqPos));
        string colValue = trim(assignment.substr(eqPos + 1));

        if (colValue.size() >= 2 && colValue.front() == '\'' && colValue.back() == '\'')
            colValue = colValue.substr(1, colValue.length() - 2);

        uq.setValues[colName] = colValue;
    }

    if (wherePos != string::npos)
    {
        uq.hasWhere = true;
        string whereClause = trim(cmd.substr(wherePos + 5));

        size_t opPos = string::npos;
        string ops[] = { ">=", "<=", "!=", "=", ">", "<" };

        for (const string& op : ops)
        {
            opPos = whereClause.find(op);
            if (opPos != string::npos)
            {
                uq.whereOperator = op;
                break;
            }
        }

        if (opPos == string::npos)
        {
            setColor(12);
            cout << "Error: Invalid WHERE clause.\n";
            setColor(15);
            uq.hasWhere = false;
            return uq;
        }

        uq.whereColumn = trim(whereClause.substr(0, opPos));
        uq.whereValue = trim(whereClause.substr(opPos + uq.whereOperator.length()));

        if (uq.whereValue.size() >= 2 && uq.whereValue.front() == '\'' && uq.whereValue.back() == '\'')
            uq.whereValue = uq.whereValue.substr(1, uq.whereValue.length() - 2);
    }

    return uq;
}

bool evaluateCondition(const string& cellValue, const string& op, const string& targetValue)
{
    if (op == "=")
        return cellValue == targetValue;
    else if (op == "!=")
        return cellValue != targetValue;
    else if (op == ">")
    {
        try { return stod(cellValue) > stod(targetValue); }
        catch (...) { return false; }
    }
    else if (op == "<")
    {
        try { return stod(cellValue) < stod(targetValue); }
        catch (...) { return false; }
    }
    else if (op == ">=")
    {
        try { return stod(cellValue) >= stod(targetValue); }
        catch (...) { return false; }
    }
    else if (op == "<=")
    {
        try { return stod(cellValue) <= stod(targetValue); }
        catch (...) { return false; }
    }

    return false;
}

bool executeUpdate(const UpdateQuery& uq)
{
    ifstream check(uq.tableName + ".csv");
    if (!check.good())
    {
        setColor(12);
        cout << "Error: Table doesn't exists.\n";
        setColor(15);
        return false;
    }

    TableData table = readTable(uq.tableName);

    if (table.columns.empty())
        return false;

    unordered_map<string, int> colIndex;
    for (size_t i = 0; i < table.columns.size(); i++)
    {
        string upperCol = table.columns[i];
        transform(upperCol.begin(), upperCol.end(), upperCol.begin(), ::toupper);
        colIndex[upperCol] = i;
    }

    if (table.primaryKeyIndex != -1)
    {
        string pkColName = table.columns[table.primaryKeyIndex];
        string upperPK = pkColName;
        transform(upperPK.begin(), upperPK.end(), upperPK.begin(), ::toupper);

        for (const auto& pair : uq.setValues)
        {
            string upperSetCol = pair.first;
            transform(upperSetCol.begin(), upperSetCol.end(), upperSetCol.begin(), ::toupper);

            if (upperSetCol == upperPK)
            {
                setColor(12);
                cout << "Error: Cannot update PRIMARY KEY column '" << pkColName << "'.\n";
                setColor(15);
                return false;
            }
        }
    }

    for (const auto& pair : uq.setValues)
    {
        string upperSetCol = pair.first;
        transform(upperSetCol.begin(), upperSetCol.end(), upperSetCol.begin(), ::toupper);

        if (colIndex.find(upperSetCol) == colIndex.end())
        {
            setColor(12);
            cout << "Error: Column '" << pair.first << "' does not exist.\n";
            setColor(15);
            return false;
        }
    }

    int whereColIndex = -1;
    if (uq.hasWhere)
    {
        whereColIndex = getColumnIndex(table, uq.whereColumn);
        if (whereColIndex == -1)
        {
            setColor(12);
            cout << "Error: WHERE column '" << uq.whereColumn << "' not found.\n";
            setColor(15);
            return false;
        }
    }

    int updatedCount = 0;

    for (auto& row : table.rows)
    {
        bool shouldUpdate = true;

        if (uq.hasWhere)
        {
            if (whereColIndex >= (int)row.size())
            {
                shouldUpdate = false;
            }
            else
            {
                shouldUpdate = evaluateCondition(
                    row[whereColIndex],
                    uq.whereOperator,
                    uq.whereValue);
            }
        }

        if (shouldUpdate)
        {
            for (const auto& pair : uq.setValues)
            {
                string upperSetCol = pair.first;
                transform(upperSetCol.begin(), upperSetCol.end(), upperSetCol.begin(), ::toupper);

                int colIdx = colIndex[upperSetCol];
                if (colIdx < (int)row.size())
                {
                    if (colIdx < (int)table.notNull.size() && table.notNull[colIdx])
                    {
                        if (trim(pair.second).empty())
                        {
                            setColor(12);
                            cout << "Error: Column '" << pair.first << "' cannot be set to NULL.\n";
                            setColor(15);
                            return false;
                        }
                    }

                    if (colIdx < (int)table.lengths.size() && table.lengths[colIdx] != -1)
                    {
                        if ((int)pair.second.length() > table.lengths[colIdx])
                        {
                            setColor(12);
                            cout << "Error: Value '" << pair.second << "' exceeds maximum length "
                                << table.lengths[colIdx] << " for column '" << pair.first << "'.\n";
                            setColor(15);
                            return false;
                        }
                    }

                    row[colIdx] = pair.second;
                }
            }
            updatedCount++;
        }
    }

    if (!writeTable(uq.tableName, table))
        return false;

    setColor(10);
    cout << updatedCount << " row(s) updated successfully.\n";
    setColor(15);

    return true;
}

int getColumnIndex(const TableData& table, const string& columnName)
{
    string upperCol = columnName;
    transform(upperCol.begin(), upperCol.end(), upperCol.begin(), ::toupper);

    for (size_t i = 0; i < table.columns.size(); i++)
    {
        string upperTableCol = table.columns[i];
        transform(upperTableCol.begin(), upperTableCol.end(), upperTableCol.begin(), ::toupper);

        if (upperTableCol == upperCol)
            return i;
    }
    return -1;
}

void handleUpdate(const string& query)
{
    UpdateQuery uq = parseUpdateQuery(query);

    if (uq.tableName.empty())
        return;

    executeUpdate(uq);
}