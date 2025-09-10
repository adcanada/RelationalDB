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

    cout << "Use '|' to signal end of command." << endl;
    while (true) {
        cout << "> "; //prompt
        
        string query;
        while (true) { //multi-line input loop
            string input;
            getline(cin, input);

            query += input;
            query += " ";

            if (input.length() > 0 && input.at(input.length()-1) == '|') {
                //remove 'end of command' symbol and leave input loop
                query = query.substr( 0, query.length()-2 );
                break; 
            } else {
                cout << "+ "; //continuing command
            }
        }

        //run command
        DB.execute(query);
    }


    return 0;
}
