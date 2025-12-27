#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include <sstream>
#include "utils.h"
using namespace std;


void setColor(int i)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
}

string trim(string s) 
{
    int start = 0, end = s.size() - 1;
    while (start < (int)s.size() && isspace(s[start]))
        start++;
    while (end >= 0 && isspace(s[end]))
        end--;
    if (start > end)
        return "";
    return s.substr(start, end - start + 1);
}
string trim_Middle(string s)
{
    string result;
    bool inSpace = false;
    for (char c : s) {
        if (isspace(c)) {
            if (!inSpace) {
                result += ' ';
                inSpace = true;
            }
        }
        else {
            result += c;
            inSpace = false;
        }
    }
	return result;
}
void toUpper(string& s)
{
    transform(s.begin(), s.end(), s.begin(), ::toupper);
}

vector<string> split( string s, char delimiter)
{
    vector<string> tokens;
    string token;
    stringstream ss(s);

    while (getline(ss, token, delimiter))
        tokens.push_back(trim(token));

    return tokens;
}

TableData readTable(string tableName)
{
    TableData t;
    ifstream f(tableName + ".csv");
    if (!f.good()) {
        setColor(12); cout << "Error: Table not found.\n"; setColor(15);
        return t;
    }

    string line;
    if (!getline(f, line)) return t;

	vector<string> colDefs = split(line, ',');//ID:INT:PK , NAME:VARCHAR(50):NN ,   AGE:INT 
    t.primaryKeyIndex = -1;
    for (int i = 0; i <(int) colDefs.size(); ++i) {
        string d = trim(colDefs[i]);

        vector<string> parts = split(d, ':');
        if (parts.size() < 2) continue;
        string name = trim(parts[0]);
        string typeDef = trim(parts[1]);

        string baseType = typeDef;
        int length = -1;
        size_t p = typeDef.find('(');
        if (p != string::npos) {
            size_t q = typeDef.find(')', p);
            if (q != string::npos) {
                baseType = typeDef.substr(0, p);
                string lenStr = typeDef.substr(p + 1, q - p - 1);
                try { length = stoi(lenStr); }
                catch (...) { length = -1; }
            }
        }
        for (auto& ch : baseType) ch = toupper(ch);

        bool isNN = false;
        bool isPK = false;
        for (size_t k = 2; k < parts.size(); ++k) {
            string tok = parts[k];
            string up = tok; transform(up.begin(), up.end(), up.begin(), ::toupper);
            if (up == "NN" || up == "NOTNULL" || up == "NOT NULL") isNN = true;
            if (up == "PK" || up == "PRIMARY" || up == "PRIMARYKEY" || up == "PRIMARY KEY") isPK = true;
            if (up == "PK") isPK = true;
            if (up == "NN") isNN = true;
        }
        t.columns.push_back(name);
        t.types.push_back(baseType);
        t.lengths.push_back(length);
        t.notNull.push_back(isNN);
        if (isPK) t.primaryKeyIndex = (int)i;
    }


    while (getline(f, line)) {
        if (trim(line).empty()) continue;
        vector<string> vals = split(line, ',');
        for (auto& v : vals) v = trim(v);
        t.rows.push_back(vals);
    }
    return t;
}

bool writeTable(string tableName, TableData data)
{
    ofstream f(tableName + ".csv", ios::trunc);
    if (!f.is_open()) {
        setColor(12); cout << "Error: Cannot write to table '" << tableName << "'.\n"; setColor(15);
        return false;
    }


    for (int i = 0; i <(int) data.columns.size(); ++i) {
        f << data.columns[i] << ":" << data.types[i];
        if (data.lengths.size() > i && data.lengths[i] != -1) {
            f << "(" << data.lengths[i] << ")";
        }

        if (data.primaryKeyIndex == (int)i) f << ":PK";
        if (data.notNull.size() > i && data.notNull[i]) f << ":NN";
        if (i + 1 < data.columns.size()) f << ",";
    }
    f << "\n";


    for (auto& row : data.rows) {
        for (int j = 0; j < row.size(); ++j) {
            f << trim(row[j]);
            if (j + 1 < row.size()) f << ",";
        }
        f << "\n";
    }
    f.flush();
    f.close();
    return true;
}