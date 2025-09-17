## Relational Algebra Query Processor
Adam Dapoz (CUID 101302979)
2025/09/10 to 2025/09/17

# Files:
- main.cpp                      - Handles the main input loop for interactive mode, and file I/O for file input mode.
- Database.cpp/.h               - Maintains the collection of relations. Also parses and runs commands provided by main.
- Relation.cpp/.h               - Defines the storage of relations and the operations that can be performed on them.
- LogicalExpression.cpp/.h      - Essentially a boolean evaluator with support for nested expressions.

- commands.txt                  - A template file used as an example of file input mode.
- Makefile                      - Used to compile and link the project into a single executable

- RelationalDBSyntax.jpg        - A diagram showing all valid combinations of tokens in this program's syntax.
- RelationalDBClassDiagram.jpg  - A class diagram showing a brief overview of the program's sturcture.

# Execution Instructions (Linux)
1. Run 'make' from the project directory.
2. To run in interactive mode, use './run'
2. To run in file input mode, use './run [filename]'

# Execution Instructions (Windows)
Untested, but should work using a compatibility layer such as Cygwin (https://cygwin.com)


Developed independently by Adam Dapoz
