#include <iostream>
#include <string>

#include "Database.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

int main() {
    Database DB;

    cout << "Adam Dapoz 101302979 --- Relational Algebra Processor" << endl;
    cout << "COMP 3005 Bonus Assignment 1" << endl << endl;

    cout << "Use ';' to signal end of command." << endl;
    while (true) {
        cout << "> "; //prompt
        
        string query;
        while (true) { //multi-line input loop
            string input;
            getline(cin, input);

            query += input;
            query += " ";

            if (input.length() > 0 && input.at(input.length()-1) == ';') {
                break; //ended command
            } else {
                cout << "+ "; //continuing command
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
