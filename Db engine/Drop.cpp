#include "Drop.h"
#include "Utils.h"
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

void parseDrop(string cmd)
{

    string trimmed = trim(cmd);
    if (!trimmed.empty() && trimmed.back() == ';') {
        trimmed.pop_back();
    }

    string upper = trimmed;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    upper = trim_Middle(upper);
    if (upper.find("DROP TABLE") != 0) {
        setColor(12);
        cout << "Error: Command must start with DROP TABLE.\n";
        setColor(15);
        return;
    }

    string tableName = trim(trimmed.substr(11));

    if (tableName.empty()) {
        setColor(12);
        cout << "Error: Missing table name.\n";
        setColor(15);
        return;
    }

    string filename = tableName + ".csv";
    ifstream check(filename);
    if (!check.good()) {
        setColor(12);
        cout << "Error: Table '" << tableName << "' does not exist.\n";
        setColor(15);
        return;
    }
    check.close();


    if (remove(filename.c_str()) != 0) {
        setColor(12);
        cout << "Error: Failed to delete table '" << tableName << "'.\n";
        setColor(15);
    }
    else {
        setColor(10);
        cout << "Table '" << tableName << "' dropped successfully.\n";
        setColor(15);
    }
}
