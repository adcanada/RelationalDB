#include "Database.h"
#include "LogicalExpression.h"

#include <algorithm>

using std::find; //find in vector

vector<string> splitIntoTokens(string& command);
string nextToken(string& command);
bool isKeyword(const string&);

//Relation parseSigma(vector<string>& tokens);
Relation parsePi(vector<string>& tokens);

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

Relation Database::execute(string& command) {
    cout << "Parsing Command: " << command << endl;
    vector<string> tokens = splitIntoTokens(command);
    return executeTokens(tokens);
}

Relation Database::executeTokens(vector<string>& tokens) {

    //test relation
    vector<string> colnames = {"EID", "Cookies", "Age"};
    vector<string> r1 = {"E1", "12", "32"};
    vector<string> r2 = {"E2", "15", "12"};
    vector<string> r3 = {"E3", "22", "22"};
    Relation rel(colnames);
    rel.addRow(r1);
    rel.addRow(r2);
    rel.addRow(r3);
    //rel.print();
    
    /*LogicalExpression expr("Cookies", DataOperator::notEqualTo, "Age");
    cout << "r1: " << expr.eval(colnames, r1) << endl;
    cout << "r2: " << expr.eval(colnames, r2) << endl;
    cout << "r3: " << expr.eval(colnames, r3) << endl;*/

    std::pair<string,Relation> sample("Table1",rel);
    relations.insert(sample);

    //start processing the tokens from the start
    
    auto tokenIter = tokens.begin();
    vector<string> remainingTokens(tokenIter+1, tokens.end());

    if (*tokenIter == ";") { //nothing command
        return Relation();
    } else if (*tokenIter == "sigma" || *tokenIter == "select") {
        //parseSigma(remainingTokens);

    } else if (*tokenIter == "pi" || *tokenIter == "project") {
        return parsePi(remainingTokens);

    } else { //relation name
        string name = *tokenIter;

        tokenIter++;
        remainingTokens = vector<string>(tokenIter+1, tokens.end());

        if (*tokenIter == "=") { //assigning to a relation
            Relation rval = executeTokens(remainingTokens);
            std::pair<string,Relation> newRelation(name, rval);
            relations.insert(newRelation);
            return rval;

        } else if (*tokenIter == ";") { //just return this relation
            if (relations.count(name) > 0) { //check existence
                return relations.at(name);
            } else {
                return Relation();
            }
        }
    }

}

Relation Database::parsePi(vector<string>& tokens) {
    //format:
    //pi { colname , ... , colname } { relation }
    int tokenNum = 0;
    if (tokens.at(tokenNum) != "{") {
        throw new std::runtime_error("Invalid token \"" + tokens.at(tokenNum) + "\", \"{\" expected");
    }

    vector<string> selectedColumns;

    do {
        //read a column name
        tokenNum++;
        if (isKeyword(tokens.at(tokenNum))) {
            throw new std::runtime_error("Invalid keyword \"" + tokens.at(tokenNum) + "\", column name expected");
        } else {
            selectedColumns.push_back(tokens.at(tokenNum));
        }

        tokenNum++;
    } while (tokens.at(tokenNum) == ",");

    //should have ended with }
    if (tokens.at(tokenNum) != "}") {
        throw new std::runtime_error("Invalid token \"" + tokens.at(tokenNum) + "\", \"}\" expected");
    }

    tokenNum++; //next should be { for the relation name
    if (tokens.at(tokenNum) != "{") {
        throw new std::runtime_error("Invalid token \"" + tokens.at(tokenNum) + "\", \"{\" expected");
    }

    tokenNum++; //now the relation name
    if (isKeyword(tokens.at(tokenNum))) {
        throw new std::runtime_error("Invalid keyword \"" + tokens.at(tokenNum) + "\", relation name expected");
    }
    string relationName = tokens.at(tokenNum);

    tokenNum++; //now the closing }
    if (tokens.at(tokenNum) != "}") {
        throw new std::runtime_error("Invalid token \"" + tokens.at(tokenNum) + "\", \"}\" expected");
    }

    tokenNum++; //and a ; to end
    if (tokens.at(tokenNum) != ";") {
        throw new std::runtime_error("Invalid token \"" + tokens.at(tokenNum) + "\", \";\" expected");
    }

    //now we have a list of columns and a relation name
    //so ask the relation to filter itself
    Relation& rel = relations.at(relationName);
    return rel.project(selectedColumns);
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
