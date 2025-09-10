#include "Database.h"
#include "LogicalExpression.h"

#include <algorithm>

using std::find; //find in vector

vector<string> splitIntoTokens(string& command);
string nextToken(string& command);
bool isKeyword(const string&);

Relation parseSigma(vector<string> tokens);
Relation parsePi(vector<string> tokens);

const vector<string> keywords = { 
    "sigma", "select", 
    "pi", "project", 
    "join", 
    "union", 
    "intersect", 
    "minus", 
    "less", "greater", "equal", "nequal", 
    "=",  
    "{", "}", 
    "(", ")", "<", ">", ",", "\"", ";" 
};

const Relation& Database::getRelation(const string &name) const {
    return this->relations.at(name);
}

Relation& Database::execute(string& command) {
    cout << "Parsing Command: " << command << endl;

    //test relation
    vector<string> colnames = {"EID", "Cookies", "Age"};
    vector<string> r1 = {"E1", "12", "32"};
    vector<string> r2 = {"E2", "15", "12"};
    vector<string> r3 = {"E3", "22", "22"};
    Relation rel(colnames);
    rel.addRow(r1);
    rel.addRow(r2);
    rel.addRow(r3);
    rel.print();
    
    LogicalExpression expr("Cookies", DataOperator::notEqualTo, "Age");
    cout << "r1: " << expr.eval(colnames, r1) << endl;
    cout << "r2: " << expr.eval(colnames, r2) << endl;
    cout << "r3: " << expr.eval(colnames, r3) << endl;

    //parse the command
    vector<string> tokens = splitIntoTokens(command);

    int currentTokenNum = 0;
    string token = tokens.at(currentTokenNum);
    vector<string> remainingTokens(tokens.begin()+currentTokenNum, tokens.end());

    if (token == ";") { //nothing command
        exit(0);
    } else if (token == "sigma" || token == "select") {
        //parseSigma(remainingTokens);

    } else if (token == "pi" || token == "project") {
        //parsePi(remainingTokens);

    } 

    exit(1);
}

vector<string> splitIntoTokens(string& command) {
    vector<string> tokens;

    string token = "";
    while (token != ";") {
        //get next token and add if not just a space
        token = nextToken(command);
        if (token == " ") { continue; }
        tokens.push_back(token);
    }

    return tokens;
}

string nextToken(string& command) {
    if (command.length() == 0) { return ";"; }

    //iterate until token split
    int i=0;
    string tokenSplitChars = ",=<>\"{}(); ";
    while (tokenSplitChars.find( command.at(i) ) == string::npos 
            && i < command.length()) {
        i++;
    }

    if (i == 0) { i = 1; } //first char is a token on its own

    string result = command.substr(0,i);
    command = command.substr(i, command.length()-i);
    return result;
}

bool isKeyword(const string& str) {
    return (find(keywords.begin(), keywords.end(), str) != keywords.end());
}
