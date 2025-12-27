#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <set>
#include "insert.h"
#include "utils.h"

vector<string> parseValueSet(const string& valueSet)
{
    vector<string> values;
    string current = "";
    bool inQuotes = false;
    int parenDepth = 0;

    for (size_t i = 0; i < valueSet.size(); i++)
    {
        char c = valueSet[i];

        if (c == '\'')
        {
            inQuotes = !inQuotes;
            continue;
        }

        if (!inQuotes)
        {
            if (c == '(')
            {
                parenDepth++;
                continue;
            }
            else if (c == ')')
            {
                parenDepth--;
                if (parenDepth == 0)
                {
                    if (!current.empty())
                    {
                        values.push_back(trim(current));
                        current = "";
                    }
                    break;
                }
                continue;
            }
            else if (c == ',' && parenDepth == 1)
            {
                values.push_back(trim(current));
                current = "";
                continue;
            }
        }

        current += c;
    }

    if (!current.empty())
    {
        values.push_back(trim(current));
    }

    return values;
}

InsertQuery parseInsertQuery(const string& query)
{
    InsertQuery iq;
    iq.hasColumns = false;

    string cmd = query;
    if (!cmd.empty() && cmd.back() == ';')
        cmd.pop_back();

    string upper = cmd;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    upper = trim_Middle(upper);
    size_t insertPos = upper.find("INSERT INTO");
    if (insertPos == string::npos)
    {
        setColor(12);
        cout << "Error: Invalid INSERT syntax. Use: INSERT INTO table VALUES (...)\n";
        setColor(15);
        return iq;
    }

    size_t tableStart = insertPos + 11;
    size_t valuesPos = upper.find("VALUES");
    size_t parenPos = upper.find("(", tableStart);

    if (valuesPos == string::npos)
    {
        setColor(12);
        cout << "Error: Missing VALUES clause.\n";
        setColor(15);
        return iq;
    }

    if (parenPos != string::npos && parenPos < valuesPos)
    {
        iq.hasColumns = true;
        iq.tableName = trim(cmd.substr(tableStart, parenPos - tableStart));

        size_t endParen = upper.find(")", parenPos);
        if (endParen == string::npos)
        {
            setColor(12);
            cout << "Error: Missing ')' after column names.\n";
            setColor(15);
            return iq;
        }

        string colSection = trim(cmd.substr(parenPos + 1, endParen - parenPos - 1));
        iq.columns = split(colSection, ',');
    }
    else
    {
        iq.tableName = trim(cmd.substr(tableStart, valuesPos - tableStart));
    }

    if (iq.tableName.empty())
    {
        setColor(12);
        cout << "Error: Missing table name.\n";
        setColor(15);
        return iq;
    }

    string valuesSection = trim(cmd.substr(valuesPos + 6));

    if (valuesSection.empty())
    {
        setColor(12);
        cout << "Error: No values provided.\n";
        setColor(15);
        return iq;
    }

    size_t pos = 0;
    while (pos < valuesSection.size())
    {
        size_t startParen = valuesSection.find("(", pos);
        if (startParen == string::npos)
            break;

        size_t endParen = startParen + 1;
        int depth = 1;
        bool inQuotes = false;

        while (endParen < valuesSection.size() && depth > 0)
        {
            if (valuesSection[endParen] == '\'' && (endParen == 0 || valuesSection[endParen - 1] != '\\'))
            {
                inQuotes = !inQuotes;
            }
            else if (!inQuotes)
            {
                if (valuesSection[endParen] == '(')
                    depth++;
                else if (valuesSection[endParen] == ')')
                    depth--;
            }
            endParen++;
        }

        if (depth != 0)
        {
            setColor(12);
            cout << "Error: Unmatched parentheses in VALUES.\n";
            setColor(15);
            return iq;
        }

        string valueSet = valuesSection.substr(startParen, endParen - startParen);
        vector<string> values = parseValueSet(valueSet);

        if (!values.empty())
        {
            iq.valuesList.push_back(values);
        }

        pos = endParen;
    }

    if (iq.valuesList.empty())
    {
        setColor(12);
        cout << "Error: No valid value sets found.\n";
        setColor(15);
        return iq;
    }

    return iq;
}

bool executeInsert(const InsertQuery& iq)
{
    ifstream check(iq.tableName + ".csv");
    if (!check.good())
    {
        setColor(12);
        cout << "Error: Table '" << iq.tableName << "' does not exist.\n";
        setColor(15);
        return false;
    }
    check.close();

    TableData table = readTable(iq.tableName);

    if (table.columns.empty())
    {
        setColor(12);
        cout << "Error: Cannot read table structure.\n";
        setColor(15);
        return false;
    }

    unordered_map<string, int> colIndex;
    for (size_t i = 0; i < table.columns.size(); i++)
    {
        string upperCol = table.columns[i];
        transform(upperCol.begin(), upperCol.end(), upperCol.begin(), ::toupper);
        colIndex[upperCol] = i;
    }

    if (iq.hasColumns)
    {
        for (const string& col : iq.columns)
        {
            string upperInputCol = col;
            transform(upperInputCol.begin(), upperInputCol.end(), upperInputCol.begin(), ::toupper);

            if (colIndex.find(upperInputCol) == colIndex.end())
            {
                setColor(12);
                cout << "Error: Column '" << col << "' does not exist.\n";
                setColor(15);
                return false;
            }
        }
    }

    set<string> existingPKValues;
    if (table.primaryKeyIndex != -1)
    {
        for (const auto& row : table.rows)
        {
            if (table.primaryKeyIndex < (int)row.size())
            {
                existingPKValues.insert(trim(row[table.primaryKeyIndex]));
            }
        }
    }

    int insertedCount = 0;

    for (const vector<string>& values : iq.valuesList)
    {
        vector<string> newRow;

        if (!iq.hasColumns)
        {
            if (values.size() != table.columns.size())
            {
                setColor(12);
                cout << "Error: Column count doesn't match value count in row " << (insertedCount + 1) << ".\n";
                cout << "Expected " << table.columns.size() << " values, got " << values.size() << ".\n";
                setColor(15);
                return false;
            }

            newRow = values;
        }
        else
        {
            if (iq.columns.size() != values.size())
            {
                setColor(12);
                cout << "Error: Column count doesn't match value count in row " << (insertedCount + 1) << ".\n";
                setColor(15);
                return false;
            }

            newRow.resize(table.columns.size(), "");

            for (size_t i = 0; i < iq.columns.size(); i++)
            {
                string upperInputCol = iq.columns[i];
                transform(upperInputCol.begin(), upperInputCol.end(), upperInputCol.begin(), ::toupper);

                int idx = colIndex[upperInputCol];
                newRow[idx] = values[i];
            }
        }

        for (size_t i = 0; i < table.columns.size(); i++)
        {
            if (i < table.notNull.size() && table.notNull[i])
            {
                if (i >= newRow.size() || trim(newRow[i]).empty())
                {
                    setColor(12);
                    cout << "Error: Column '" << table.columns[i] << "' cannot be NULL (row " << (insertedCount + 1) << ").\n";
                    setColor(15);
                    return false;
                }
            }
        }

        for (size_t i = 0; i < table.columns.size(); i++)
        {
            if (i >= newRow.size() || trim(newRow[i]).empty())
            {
                string typeUpper = table.types[i];
                transform(typeUpper.begin(), typeUpper.end(), typeUpper.begin(), ::toupper);

                if (typeUpper == "INT" || typeUpper == "FLOAT")
                {
                    newRow[i] = "0";  
                }
                else if (typeUpper == "STRING")
                {
                    newRow[i] = "NULL"; 
                }
                else
                {
                    newRow[i] = "NULL";
                }
            }
        }

        if (table.primaryKeyIndex != -1)
        {
            if (table.primaryKeyIndex >= (int)newRow.size() || trim(newRow[table.primaryKeyIndex]).empty())
            {
                setColor(12);
                cout << "Error: PRIMARY KEY column '" << table.columns[table.primaryKeyIndex]
                    << "' cannot be NULL (row " << (insertedCount + 1) << ").\n";
                setColor(15);
                return false;
            }

            string pkValue = trim(newRow[table.primaryKeyIndex]);
            if (existingPKValues.find(pkValue) != existingPKValues.end())
            {
                setColor(12);
                cout << "Error: Duplicate PRIMARY KEY value '" << pkValue << "' in column '"
                    << table.columns[table.primaryKeyIndex] << "' (row " << (insertedCount + 1) << ").\n";
                setColor(15);
                return false;
            }

            existingPKValues.insert(pkValue);
        }

        for (size_t i = 0; i < table.columns.size(); i++)
        {
            if (i < table.lengths.size() && table.lengths[i] != -1)
            {
                if (i < newRow.size() && (int)newRow[i].length() > table.lengths[i])
                {
                    setColor(12);
                    cout << "Error: Value '" << newRow[i] << "' exceeds maximum length "
                        << table.lengths[i] << " for column '" << table.columns[i]
                        << "' (row " << (insertedCount + 1) << ").\n";
                    setColor(15);
                    return false;
                }
            }
        }

        table.rows.push_back(newRow);
        insertedCount++;
    }

    if (!writeTable(iq.tableName, table))
        return false;

    setColor(10);
    cout << insertedCount << " row(s) inserted successfully.\n";
    setColor(15);

    return true;
}

void handleInsert(const string& query)
{
    InsertQuery iq = parseInsertQuery(query);

    if (iq.tableName.empty())
        return;

    executeInsert(iq);
}