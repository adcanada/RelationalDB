#include <iostream>
#include <fstream>
#include <string>

#include "Database.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

int main(int argc, char *argv[]) {
    Database DB;

    cout << "Adam Dapoz 101302979 --- Relational Algebra Processor" << endl;
    cout << "COMP 3005 Bonus Assignment 1" << endl << endl;


    //setup input stream
    std::ifstream fileStream;
    
    if (argc > 2) {
        cout << "Invaid number of arguments, 0 or 1 expected";
        return 1;

    } else if (argc == 2) { //run command from file
        //try opening provided file
        try {
            fileStream.open(argv[1]);
            cout << "Running in file input mode." << endl;
            cout << "Use './run' to run in interactive mode." << endl;
        } catch (...) {
            cout << "Error opening file \"" << argv[1] << "\"";
            return 1;
        }
    } else {
        cout << "Running in interactive mode." << endl;
        cout << "Use './run <filename>' to execute a list of commands from a file." << endl;
        cout << "';' signals the end of a command." << endl;
    }

    //main input loop: read -> execute -> read -> ...
    //(break if using file mode and hit eof)
    while (!fileStream.is_open() || !fileStream.eof()) {
        cout << "> "; //prompt
        
        string query;
        while (true) { //multi-line input loop
            string input;

            //read a line from file or cin depending
            if (fileStream.is_open()) {
                getline(fileStream, input);
                if (fileStream.eof()) { break; } //we read past the end
                cout << input;
            } else {
                getline(cin, input);
                if (input == "quit" || input == "exit") {
                    cout << "Exiting...";
                    return 0;
                }
            }

            query += input;
            query += " ";

            if (input.length() > 0 && input.at(input.length()-1) == ';') {
                break; //ended command
            } else if (!fileStream.is_open()) {
                cout << "+ "; //continuing command (interactive mode)
            }
        }

        cout << endl;

        //run command
        try {
            //split into commands separated by ';' and run them
            size_t pos;
            while ((pos = query.find(';')) != string::npos) {
                string command = query.substr(0, pos+1);
                query.erase(0, pos+1);

                Relation result = DB.execute(command);
                result.print();
            }
        } catch (std::runtime_error *e) {
            cout << "\nException: " << e->what() << endl;
        }
    }

    return 0;
}
