#include "Helper.h"
#include "utils.h"

void printHelp() {
    setColor(14);
    cout << "\nAvailable Commands:\n";
    cout << "===================\n\n";
    setColor(15);

    cout << "1. CREATE TABLE\n";
    cout << "   Syntax: CREATE TABLE table_name (col1 type, col2 type, ...)\n";
    cout << "   Example: CREATE TABLE Employee(ID INT Primary Key , NAME VARCHAR(50) NOT NULL)\n\n";

    cout << "2. SELECT\n";
    cout << "   Syntax: SELECT * FROM table_name\n";
    cout << "   Syntax: SELECT col1, col2 FROM table_name\n";
    cout << "   Syntax: SELECT * FROM table_name WHERE col=val\n\n";

    cout << "3. UPDATE\n";
    cout << "   Syntax: UPDATE table_name SET col=val WHERE col=val\n\n";

    cout << "4. DROP TABLE\n";
    cout << "   Syntax: DROP TABLE table_name\n\n";

    cout << "5. HELP\n";
    cout << "   Syntax: HELP\n";
    cout << "   Description: Shows this help message.\n\n";

    cout << "6. EXIT / QUIT\n";
    cout << "   Description: Exits the application.\n\n";
}
