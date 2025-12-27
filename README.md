# ITI-SQL Database Engine

A lightweight, file-based relational database management system (DBMS) written in C++ with a modern WPF GUI interface. This project demonstrates fundamental database concepts including SQL parsing, query execution, and data persistence using CSV file storage.

---

## 📋 Project Overview

**ITI-SQL Database Engine** is an educational database system that implements core RDBMS functionality from scratch. The project consists of two main components:

### 🔧 C++ Database Engine
The core engine is a command-line application written in C++ that:
- Parses and executes SQL-like commands
- Manages table creation, data manipulation, and queries
- Persists data to disk using CSV file format
- Provides a colorized console interface for direct interaction

### 🖥️ WPF GUI Application
A modern graphical user interface built with Windows Presentation Foundation (WPF) that:
- Provides a Visual Studio-inspired dark theme interface
- Launches and manages the C++ database engine as a subprocess
- Sends SQL commands to the engine via standard input
- Displays query results and engine output in real-time
- Includes keyboard shortcuts for improved productivity

---

## ✨ Features

### Database Operations
- **CREATE TABLE** - Define new tables with typed columns and constraints
- **INSERT INTO** - Add new records to tables
- **SELECT** - Query data with support for column projection and WHERE clauses
- **UPDATE** - Modify existing records based on conditions
- **DELETE** - Remove records from tables
- **DROP TABLE** - Delete entire tables and their data

### Data Types & Constraints
- **INT** - Integer values
- **VARCHAR(n)** - Variable-length strings with maximum length
- **PRIMARY KEY** - Unique identifier constraint
- **NOT NULL** - Non-nullable column constraint

### Storage & Persistence
- **CSV-based file storage** - Each table stored as a separate `.csv` file
- **Schema metadata** - Column names, types, and constraints stored in file headers
- **Automatic file management** - Tables created/deleted as files on disk

### GUI Features
- **Engine lifecycle management** - Start/stop the database engine
- **Auto-start option** - Automatically launch engine on application startup
- **Command history** - Navigate previous commands with arrow keys
- **Syntax highlighting** - Monospace font for SQL readability
- **Output saving** - Export console output to text files
- **Keyboard shortcuts** - F5 (start), Shift+F5 (stop), Ctrl+Enter (execute), Ctrl+S (save output)

---

## 🏗️ System Architecture

### Database Engine Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Main Application                      │
│                  (Db engine.cpp)                         │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
         ┌───────────────────────┐
         │   Command Parser      │
         │   (main loop)         │
         └───────┬───────────────┘
                 │
    ┌────────────┼────────────┬──────────┬──────────┐
    ▼            ▼            ▼          ▼          ▼
┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐
│CREATE  │  │SELECT  │  │INSERT  │  │UPDATE  │  │DELETE  │
│Module  │  │Module  │  │Module  │  │Module  │  │Module  │
└───┬────┘  └───┬────┘  └───┬────┘  └───┬────┘  └───┬────┘
    │           │           │           │           │
    └───────────┴───────────┴───────────┴───────────┘
                            │
                            ▼
                   ┌─────────────────┐
                   │  Utils Module   │
                   │  - File I/O     │
                   │  - Data Parsing │
                   │  - Validation   │
                   └────────┬────────┘
                            │
                            ▼
                   ┌─────────────────┐
                   │   File System   │
                   │   (.csv files)  │
                   └─────────────────┘
```

### GUI-Engine Communication

```
┌──────────────────────────────────────────────────────────┐
│                   WPF GUI Application                     │
│                   (MainWindow.xaml.cs)                    │
│                                                           │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │ Input Panel  │  │ Output Panel │  │ Control Bar  │  │
│  │ (SQL Editor) │  │ (Console)    │  │ (Start/Stop) │  │
│  └──────┬───────┘  └──────▲───────┘  └──────┬───────┘  │
│         │                  │                  │          │
└─────────┼──────────────────┼──────────────────┼──────────┘
          │                  │                  │
          │  SQL Commands    │  Output Stream   │  Process Control
          │  (stdin)         │  (stdout/stderr) │  (Start/Kill)
          │                  │                  │
          ▼                  │                  ▼
┌─────────────────────────────────────────────────────────┐
│              C++ Database Engine Process                 │
│              (Db engine.exe)                             │
└─────────────────────────────────────────────────────────┘
```

**Communication Flow:**
1. User types SQL command in GUI input box
2. GUI sends command to engine via `StandardInput` stream
3. Engine parses and executes the command
4. Engine writes results to `StandardOutput`
5. GUI captures output and displays in console panel
6. Process repeats for each command

---

## 📁 Project Structure

```
Db-engine/
├── Db engine/                  # C++ Database Engine Source
│   ├── Db engine.cpp          # Main entry point and command loop
│   ├── Create.cpp/h           # CREATE TABLE implementation
│   ├── Select.cpp/h           # SELECT query implementation
│   ├── Insert.cpp/h           # INSERT INTO implementation
│   ├── Update.cpp/h           # UPDATE statement implementation
│   ├── Delete.cpp/h           # DELETE statement implementation
│   ├── Drop.cpp/h             # DROP TABLE implementation
│   ├── Helper.cpp/h           # Help command and documentation
│   ├── Utils.cpp/h            # Utility functions and data structures
│   └── Db engine.vcxproj      # Visual Studio C++ project file
│
├── GuiApp/                     # WPF GUI Application
│   ├── MainWindow.xaml        # GUI layout and styling
│   ├── MainWindow.xaml.cs     # GUI logic and engine communication
│   ├── App.xaml               # Application resources
│   ├── App.xaml.cs            # Application entry point
│   ├── GuiApp.csproj          # Visual Studio C# project file
│   └── Properties/            # Assembly metadata
│
├── x64/Debug/                  # Build output directory
│   ├── Db engine.exe          # Compiled database engine
│   └── *.csv                  # Database table files
│
└── Db engine.sln              # Visual Studio solution file
```

### Key Files Explained

| File | Purpose |
|------|---------|
| `Db engine.cpp` | Main application loop, command routing, and user interface |
| `Utils.h/cpp` | Core data structures (`Column`, `Row`, `Table`, `TableData`), file I/O, string utilities |
| `Create.h/cpp` | Parses CREATE TABLE syntax, validates schema, creates CSV files |
| `Select.h/cpp` | Implements SELECT queries with column projection and WHERE filtering |
| `Insert.h/cpp` | Handles INSERT INTO statements, validates data types |
| `Update.h/cpp` | Processes UPDATE statements with WHERE conditions |
| `Delete.h/cpp` | Executes DELETE statements with conditional filtering |
| `Drop.h/cpp` | Removes table files from disk |
| `Helper.h/cpp` | Displays available commands and syntax examples |
| `MainWindow.xaml` | WPF UI layout with modern dark theme styling |
| `MainWindow.xaml.cs` | Process management, I/O redirection, event handlers |

---

## 🛠️ Technologies Used

### C++ Database Engine
- **Language:** C++17
- **Compiler:** MSVC (Visual Studio 2019/2022)
- **Standard Library:** STL (iostream, fstream, string, vector)
- **Platform:** Windows (uses Windows Console API for colors)

### GUI Application
- **Framework:** .NET Framework 4.7.2+
- **UI Technology:** Windows Presentation Foundation (WPF)
- **Language:** C# 7.0+
- **Design Pattern:** MVVM-inspired architecture

### Development Tools
- **IDE:** Visual Studio 2019/2022
- **Build System:** MSBuild
- **Version Control:** Git

---

## 🔍 How the Database Engine Works

### Data Storage Format

Tables are stored as CSV files with a special header format:

```csv
ID:INT,NAME:VARCHAR,AGE:INT
1,John Doe,30
2,Jane Smith,25
```

**Header Format:** `ColumnName:DataType[,ColumnName:DataType...]`

### Parsing and Execution Flow

1. **Input Reading**
   - User enters SQL command
   - Command converted to uppercase for case-insensitive parsing
   - Whitespace normalized

2. **Command Recognition**
   - Pattern matching against known SQL keywords
   - Route to appropriate handler module

3. **Query Parsing**
   - Extract table names, column names, values
   - Validate syntax and semantics
   - Build internal data structures

4. **Execution**
   - Read table metadata from CSV header
   - Load/modify data rows as needed
   - Apply WHERE clause filtering
   - Validate constraints (PRIMARY KEY, NOT NULL)

5. **Result Output**
   - Format results in tabular format
   - Display to console with color coding
   - Write changes back to CSV files

### Example: SELECT Query Flow

```
SELECT NAME, AGE FROM Employee WHERE ID=1
         ↓
    Parse Query
         ↓
    Read Employee.csv
         ↓
    Load all rows into memory
         ↓
    Filter rows where ID=1
         ↓
    Project NAME and AGE columns
         ↓
    Display formatted table
```

### Indexing

**Current Implementation:** No indexing (full table scans)

**Note:** This is an educational project. Production databases use B-trees, hash indexes, and other data structures for efficient lookups.

---

## 🖥️ GUI (WPF Application)

### Main Interface Components

#### 1. **Control Toolbar** (Top)
- **Start Engine Button** - Launches `Db engine.exe` as subprocess
- **Stop Engine Button** - Terminates the running engine process
- **Auto-start Checkbox** - Enable automatic engine startup
- **Status Indicator** - Visual feedback (green=running, red=stopped)

#### 2. **Output Console** (Top Panel)
- Displays all engine output in real-time
- Read-only monospace text area
- Supports scrolling and text selection
- Save button to export output to file

#### 3. **SQL Input Editor** (Bottom Panel)
- Multi-line text editor for SQL commands
- Syntax-friendly monospace font
- Command history navigation (↑/↓ arrows)
- Execute button sends command to engine

#### 4. **Status Bar** (Bottom)
- Shows path to `Db engine.exe`
- Displays current application state

### User Actions

| Action | Keyboard Shortcut | Description |
|--------|------------------|-------------|
| Start Engine | `F5` | Launch database engine process |
| Stop Engine | `Shift+F5` | Terminate engine process |
| Execute Command | `Ctrl+Enter` | Send SQL command to engine |
| Clear Input | `Ctrl+L` | Clear the input text box |
| Save Output | `Ctrl+S` | Export console output to file |
| Previous Command | `↑` | Navigate to previous command in history |
| Next Command | `↓` | Navigate to next command in history |

### How Queries are Sent and Results Displayed

1. **User Input**
   - User types SQL command in input box
   - Presses Execute button or `Ctrl+Enter`

2. **Command Transmission**
   - GUI writes command to engine's `StandardInput` stream
   - Command appended with newline character

3. **Engine Processing**
   - Engine reads from stdin
   - Executes command
   - Writes results to stdout

4. **Result Display**
   - GUI asynchronously reads from engine's `StandardOutput`
   - Text appended to output console in real-time
   - Auto-scrolls to show latest output

5. **Error Handling**
   - Errors written to stderr by engine
   - GUI captures and displays errors in red (if colorized)

---

## 📦 Installation & Build Instructions

### Prerequisites

- **Operating System:** Windows 10/11
- **Visual Studio:** 2019 or 2022
- **Workloads Required:**
  - Desktop development with C++
  - .NET desktop development

### Build Steps

#### Option 1: Build via Visual Studio GUI

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd Db-engine
   ```

2. **Open the solution**
   - Double-click `Db engine.sln`
   - Visual Studio will load both projects

3. **Build the C++ Engine**
   - Right-click `Db engine` project → Build
   - Output: `x64/Debug/Db engine.exe`

4. **Build the GUI Application**
   - Right-click `GuiApp` project → Build
   - Output: `GuiApp/bin/Debug/GuiApp.exe`

5. **Set startup project** (optional)
   - Right-click `GuiApp` → Set as Startup Project
   - Press `F5` to run

#### Option 2: Build via Command Line

```powershell
# Navigate to project directory
cd "C:\ITI\Db-engine"

# Build C++ engine
msbuild "Db engine\Db engine.vcxproj" /p:Configuration=Debug /p:Platform=x64

# Build GUI application
msbuild "GuiApp\GuiApp.csproj" /p:Configuration=Debug
```

### Running the Project

#### Method 1: Run GUI Application
```powershell
cd GuiApp\bin\Debug
.\GuiApp.exe
```
- Click "Start Engine" button
- GUI will automatically locate and launch `Db engine.exe`

#### Method 2: Run Engine Directly (CLI Mode)
```powershell
cd x64\Debug
.\Db engine.exe
```
- Provides direct command-line interface
- No GUI required

---

## 💡 Usage Examples

### Example 1: Create a Table

```sql
CREATE TABLE Employee(ID INT Primary Key, NAME VARCHAR(50) NOT NULL, AGE INT)
```

**Output:**
```
Table 'Employee' created successfully.
```

**File Created:** `Employee.csv`
```csv
ID:INT,NAME:VARCHAR,AGE:INT
```

### Example 2: Insert Data

```sql
INSERT INTO Employee VALUES (1, 'John Doe', 30)
INSERT INTO Employee VALUES (2, 'Jane Smith', 25)
INSERT INTO Employee VALUES (3, 'Bob Johnson', 35)
```

**Output:**
```
1 row inserted successfully.
1 row inserted successfully.
1 row inserted successfully.
```

### Example 3: Select All Records

```sql
SELECT * FROM Employee
```

**Output:**
```
+----+-------------+-----+
| ID | NAME        | AGE |
+----+-------------+-----+
| 1  | John Doe    | 30  |
| 2  | Jane Smith  | 25  |
| 3  | Bob Johnson | 35  |
+----+-------------+-----+
3 rows selected.
```

### Example 4: Select Specific Columns

```sql
SELECT NAME, AGE FROM Employee
```

**Output:**
```
+-------------+-----+
| NAME        | AGE |
+-------------+-----+
| John Doe    | 30  |
| Jane Smith  | 25  |
| Bob Johnson | 35  |
+-------------+-----+
```

### Example 5: Select with WHERE Clause

```sql
SELECT * FROM Employee WHERE ID=2
```

**Output:**
```
+----+------------+-----+
| ID | NAME       | AGE |
+----+------------+-----+
| 2  | Jane Smith | 25  |
+----+------------+-----+
1 row selected.
```

### Example 6: Update Records

```sql
UPDATE Employee SET AGE=26 WHERE ID=2
```

**Output:**
```
1 row updated successfully.
```

### Example 7: Delete Records

```sql
DELETE FROM Employee WHERE ID=3
```

**Output:**
```
1 row deleted successfully.
```

### Example 8: Drop Table

```sql
DROP TABLE Employee
```

**Output:**
```
Table 'Employee' dropped successfully.
```

**File Deleted:** `Employee.csv` removed from disk

### Example 9: Get Help

```sql
HELP
```

**Output:**
```
Available Commands:
===================

1. CREATE TABLE
   Syntax: CREATE TABLE table_name (col1 type, col2 type, ...)
   Example: CREATE TABLE Employee(ID INT Primary Key, NAME VARCHAR(50) NOT NULL)

2. SELECT
   Syntax: SELECT * FROM table_name
   Syntax: SELECT col1, col2 FROM table_name
   Syntax: SELECT * FROM table_name WHERE col=val

3. UPDATE
   Syntax: UPDATE table_name SET col=val WHERE col=val

4. DROP TABLE
   Syntax: DROP TABLE table_name

5. HELP
   Syntax: HELP
   Description: Shows this help message.

6. EXIT / QUIT
   Description: Exits the application.
```

---

## ⚠️ Limitations

### Current Limitations

1. **No Transaction Support**
   - No ACID guarantees
   - No rollback capability
   - Changes are immediately persisted

2. **Limited Data Types**
   - Only INT and VARCHAR supported
   - No DATE, FLOAT, BOOLEAN, or BLOB types

3. **No Indexing**
   - All queries perform full table scans
   - Performance degrades with large datasets

4. **Simple WHERE Clauses**
   - Only equality comparisons (`col=val`)
   - No AND/OR operators
   - No comparison operators (`<`, `>`, `<=`, `>=`, `!=`)

5. **No JOIN Operations**
   - Cannot query multiple tables
   - No foreign key relationships

6. **No Aggregation Functions**
   - No COUNT, SUM, AVG, MIN, MAX
   - No GROUP BY or HAVING clauses

7. **No Subqueries**
   - Cannot nest SELECT statements

8. **Case Sensitivity**
   - Table and column names are case-sensitive in file system
   - SQL keywords are case-insensitive

9. **Windows Only**
   - Uses Windows Console API for colors
   - WPF GUI requires Windows OS

10. **Single User**
    - No concurrent access control
    - No network/client-server architecture

### Assumptions Made

- **File Encoding:** UTF-8 for CSV files
- **CSV Delimiter:** Comma (`,`)
- **Line Endings:** Windows CRLF (`\r\n`)
- **Engine Location:** GUI searches for `Db engine.exe` in `../x64/Debug/` relative to GUI executable
- **Working Directory:** CSV files created in engine's working directory

---

## 🚀 Future Improvements

### Planned Enhancements

1. **Advanced Query Features**
   - [ ] JOIN operations (INNER, LEFT, RIGHT, FULL)
   - [ ] Aggregation functions (COUNT, SUM, AVG, MIN, MAX)
   - [ ] GROUP BY and HAVING clauses
   - [ ] ORDER BY sorting
   - [ ] LIMIT and OFFSET pagination
   - [ ] Subqueries and nested SELECT

2. **Data Types & Constraints**
   - [ ] Additional data types (FLOAT, DATE, BOOLEAN, TEXT)
   - [ ] FOREIGN KEY constraints
   - [ ] UNIQUE constraints
   - [ ] CHECK constraints
   - [ ] DEFAULT values
   - [ ] AUTO_INCREMENT for primary keys

3. **Performance Optimizations**
   - [ ] B-tree indexing for faster lookups
   - [ ] Query optimization and execution planning
   - [ ] Caching frequently accessed tables
   - [ ] Lazy loading for large datasets

4. **Transaction Support**
   - [ ] BEGIN TRANSACTION, COMMIT, ROLLBACK
   - [ ] ACID compliance
   - [ ] Write-ahead logging (WAL)
   - [ ] Crash recovery

5. **Advanced WHERE Clauses**
   - [ ] Comparison operators (`<`, `>`, `<=`, `>=`, `!=`)
   - [ ] Logical operators (AND, OR, NOT)
   - [ ] LIKE pattern matching
   - [ ] IN and BETWEEN operators
   - [ ] NULL checks (IS NULL, IS NOT NULL)

6. **Storage Improvements**
   - [ ] Binary storage format for efficiency
   - [ ] Compression for large tables
   - [ ] Page-based storage management

7. **GUI Enhancements**
   - [ ] Syntax highlighting for SQL
   - [ ] Auto-completion for table/column names
   - [ ] Visual query builder
   - [ ] Table schema viewer
   - [ ] Export results to CSV/JSON
   - [ ] Dark/light theme toggle

8. **Cross-Platform Support**
   - [ ] Linux and macOS compatibility
   - [ ] Cross-platform GUI (Qt or Electron)

9. **Multi-User Support**
   - [ ] Client-server architecture
   - [ ] Network protocol (TCP/IP)
   - [ ] User authentication and authorization
   - [ ] Concurrent access control (locking)

10. **Developer Tools**
    - [ ] Unit tests for all modules
    - [ ] Integration tests for SQL queries
    - [ ] Benchmarking suite
    - [ ] Documentation generator

---

## 🤝 Contribution Guidelines

We welcome contributions from the community! Here's how you can help:

### How to Contribute

1. **Fork the repository**
   ```bash
   git clone https://github.com/yourusername/Db-engine.git
   cd Db-engine
   ```

2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make your changes**
   - Follow existing code style and conventions
   - Add comments for complex logic
   - Update documentation as needed

4. **Test your changes**
   - Build both C++ and C# projects
   - Test all affected SQL commands
   - Verify GUI functionality

5. **Commit your changes**
   ```bash
   git add .
   git commit -m "Add: Brief description of your changes"
   ```

6. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```

7. **Create a Pull Request**
   - Describe your changes in detail
   - Reference any related issues
   - Wait for code review

### Code Style Guidelines

#### C++ Code
- Use **camelCase** for function names
- Use **PascalCase** for class/struct names
- Use **snake_case** for variables (optional)
- Indent with **4 spaces** (no tabs)
- Add header guards to all `.h` files
- Include comments for non-obvious logic

#### C# Code
- Follow **Microsoft C# Coding Conventions**
- Use **PascalCase** for public members
- Use **camelCase** for private fields (prefix with `_`)
- Use **async/await** for asynchronous operations
- Add XML documentation comments for public APIs

### Areas Needing Help

- 🐛 **Bug Fixes** - Report or fix issues
- ✨ **New Features** - Implement items from Future Improvements
- 📝 **Documentation** - Improve README, add code comments
- 🧪 **Testing** - Write unit/integration tests
- 🎨 **UI/UX** - Enhance GUI design and usability
- ⚡ **Performance** - Optimize query execution

---

## 📄 License

This project is licensed under the **MIT License**.

```
MIT License

Copyright (c) 2025 ITI-SQL Database Engine Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 📞 Contact & Support

- **Issues:** Report bugs or request features via [GitHub Issues](https://github.com/yourusername/Db-engine/issues)
- **Discussions:** Join conversations in [GitHub Discussions](https://github.com/yourusername/Db-engine/discussions)
- **Email:** your.email@example.com

---

## 🙏 Acknowledgments

- Inspired by educational database projects and SQL tutorials
- Built as a learning project to understand DBMS internals
- Thanks to the open-source community for tools and libraries

---

## 📚 Additional Resources

### Learning Resources
- [Database System Concepts](https://www.db-book.com/) - Comprehensive DBMS textbook
- [SQLite Architecture](https://www.sqlite.org/arch.html) - Real-world database design
- [How Does a Database Work?](https://cstack.github.io/db_tutorial/) - Build a database from scratch

### Related Projects
- [SQLite](https://www.sqlite.org/) - Lightweight SQL database engine
- [TinySQL](https://github.com/talent-plan/tinysql) - Educational SQL database in Go
- [SimpleDB](http://www.cs.bc.edu/~sciore/simpledb/) - Java-based educational DBMS

---

**Made with ❤️ for learning and education**
