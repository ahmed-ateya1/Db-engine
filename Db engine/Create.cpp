#include "utils.h"
#include "create.h"
#include <string>
#include<vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
using namespace std;



struct ColumnDef { string name; string type; int length; bool isPK; bool isNN; };

static bool writeEmptyTableFile(string& tableName, vector<ColumnDef>& cols)
{

    TableData t;
    t.primaryKeyIndex = -1;
    for (int i = 0; i < cols.size(); ++i) {
        t.columns.push_back(cols[i].name);
        t.types.push_back(cols[i].type);
        t.lengths.push_back(cols[i].length);
        t.notNull.push_back(cols[i].isNN);
        if (cols[i].isPK) t.primaryKeyIndex = (int)i;
    }

    return writeTable(tableName, t);
}

bool createTable(string name, vector<ColumnDef> cols)
{
    ifstream check(name + ".csv");
    if (check.good()) {
        setColor(12); cout << "Error: Table already exists.\n"; setColor(15);
        return false;
    }
    if (cols.empty()) {
        setColor(12); cout << "Error: Table must have at least one column.\n"; setColor(15);
        return false;
    }


    int pkCount = 0;
    for (auto& c : cols)
        if (c.isPK)
            pkCount++;

    if (pkCount > 1) {
        setColor(12); cout << "Error: More than one PRIMARY KEY defined.\n"; setColor(15);
        return false;
    }

    bool ok = writeEmptyTableFile(name, cols);
    if (ok) {
        setColor(10); cout << "Table '" << name << "' created successfully.\n"; setColor(15);
    }
    else {
        setColor(12); cout << "Error: Failed to create table file.\n"; setColor(15);
    }
    return ok;
}

void parseCreateQuery(string cmd)
{
    if (!trim(cmd).empty() && trim(cmd).back() == ';')
        cmd.pop_back();
    string upper = cmd;

    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    upper = trim_Middle(upper);
    if (upper.find("CREATE TABLE") != 0) {
        setColor(12); cout << "Error: Command must start with CREATE TABLE.\n"; setColor(15);
        return;
    }

    int tablePos = upper.find("TABLE") + 5;
    int parenPos = upper.find("(");

    if (parenPos == string::npos) {
        setColor(12); cout << "Error: Missing '('.\n"; setColor(15);
        return;
    }
    string tableName = trim(cmd.substr(tablePos, parenPos - tablePos));

    if (tableName.empty()) {
        setColor(12); cout << "Error: Missing table name.\n"; setColor(15);
        return;
    }
    int endParen = upper.rfind(")");
    if (endParen == string::npos || endParen < parenPos) {
        setColor(12); cout << "Error: Missing ')'.\n"; setColor(15);
        return;
    }

    string colSection = trim(cmd.substr(parenPos + 1, endParen - parenPos - 1));
    if (colSection.empty()) {
        setColor(12); cout << "Error: No columns defined.\n"; setColor(15);
        return;
    }

    vector<string> rawCols = split(colSection, ',');
    vector<ColumnDef> cols;

    for (string colDef : rawCols)
    {
        colDef = trim(colDef);

        stringstream ss(colDef);
        string colName;
        if (!(ss >> colName)) {
            setColor(12); cout << "Error: Invalid column definition: " << colDef << "\n"; setColor(15);
            return;
        }

        string typeToken;
        if (!(ss >> typeToken)) {
            setColor(12); cout << "Error: Missing type for column: " << colName << "\n"; setColor(15);
            return;
        }


        string baseType = typeToken;
        int length = -1;
        int p = typeToken.find('(');
        if (p != string::npos) {
            int q = typeToken.find(')', p);
            if (q == string::npos) {
                setColor(12); cout << "Error: Invalid type size syntax in " << typeToken << "\n"; setColor(15);
                return;
            }
            string lenStr = typeToken.substr(p + 1, q - p - 1);
            try { length = stoi(lenStr); }
            catch (...) {
                setColor(12); cout << "Error: Invalid length '" << lenStr << "'\n"; setColor(15); return;
            }
            baseType = typeToken.substr(0, p);
        }

        for (auto& ch : baseType) ch = toupper(ch);


        vector<string> allowed = { "INT","FLOAT","DOUBLE","VARCHAR","CHAR" };
        if (find(allowed.begin(), allowed.end(), baseType) == allowed.end()) {
            setColor(12); cout << "Error: Unsupported type '" << baseType << "'\n"; setColor(15);
            return;
        }

        if ((baseType == "VARCHAR" || baseType == "CHAR") && length == -1) {
            setColor(12); cout << "Error: Type '" << baseType << "' requires size, e.g. " << baseType << "(20)\n"; setColor(15);
            return;
        }


        bool isPK = false, isNN = false;
        string token;
        while (ss >> token)
        {
            string up = token;
            transform(up.begin(), up.end(), up.begin(), ::toupper);
            if (up == "PRIMARY") {

                string next;
                if (ss >> next) {
                    string upn = next; transform(upn.begin(), upn.end(), upn.begin(), ::toupper);
                    if (upn == "KEY") isPK = true;
                    else {
                        setColor(12); cout << "Error: Expected KEY after PRIMARY.\n"; setColor(15); return;
                    }
                }
                else {
                    setColor(12); cout << "Error: Expected KEY after PRIMARY.\n"; setColor(15); return;
                }
            }
            else if (up == "NOT") {

                string next; if (ss >> next) {
                    string upn = next; transform(upn.begin(), upn.end(), upn.begin(), ::toupper);
                    if (upn == "NULL") isNN = true;
                    else { setColor(12); cout << "Error: Expected NULL after NOT.\n"; setColor(15); return; }
                }
                else { setColor(12); cout << "Error: Expected NULL after NOT.\n"; setColor(15); return; }
            }
            else {

                setColor(12); cout << "Warning: Unknown token '" << token << "' in column def\n"; setColor(15);
            }
        }

        cols.push_back({ colName, baseType, length, isPK, isNN });
    }

    vector<ColumnDef> coldefs = cols;
    createTable(tableName, coldefs);
}
