#include <iostream>
#include <string>
#include "utils.h"
#include "helper.h"
#include "create.h"
#include "select.h"
#include "update.h"
#include "insert.h" 
#include "delete.h"
#include "drop.h"
using namespace std;

void printPrompt()
{
    setColor(11);
    cout << "iti-sql>: ";
    setColor(15);
}

int main()
{
    setColor(10);
    cout << "Welcome to ITI-SQL Database Engine\n";
    cout << "Type 'HELP' for a list of commands.\n\n";
    setColor(15);

    while (true)
    {
        string cmd;
        printPrompt();
        getline(cin, cmd);

        if (cmd.empty())
            continue;

        string upper = cmd;
        toUpper(upper);

		upper = trim_Middle(upper);


        if (upper == "EXIT" || upper == "QUIT")
        {
            break;
        }
        else if (upper == "HELP")
        {
            printHelp();
        }
        else if (upper.find("CREATE TABLE") == 0)
        {
            parseCreateQuery(cmd);
        }
        else if (upper.find("SELECT") == 0)
        {
            parseSelect(cmd);
        }
        else if (upper.find("INSERT") == 0)
        {
            handleInsert(cmd);
        }
        else if (upper.find("UPDATE") == 0)
        {
            handleUpdate(cmd);
        }
        else if (upper.find("DELETE") == 0)
        {
            parseDeleteQuery(cmd);
        }
        else if (upper.find("DROP TABLE") == 0)
        {
            parseDrop(cmd);
        }
        else
        {
            setColor(12);
            cout << "Unknown command. Type 'HELP' for available commands.\n";
            setColor(15);
        }
    }

    return 0;
}