#pragma once
#ifndef CREATE_H
#define CREATE_H
#include <string>
#include "utils.h"

using namespace std;


bool createTable(string name, vector<Column> cols);

void parseCreateQuery(string cmd);


#endif