#include "Database.h"
#include "LogicalExpression.h"

#include <algorithm>

using std::find; //find in vector

vector<string> splitIntoTokens(string& command);
string nextToken(string& command);
bool isKeyword(const string&);

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

    } else if (*tokenIter == "(") {
        return executeParentheses(tokenIter, tokens.end());

    } else if (*tokenIter == "{") { //start relation definition
        return createRelation(tokens); //pass the { too

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

        } else {
            throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \";/=/join/union/intersect\" expected");
        }
    }

    return Relation();
}

Relation Database::executeParentheses(vector<string>::iterator& iter, const vector<string>::iterator& end) {
    //first char should be (
    if (*iter != "(") { 
        throw new std::runtime_error("Invalid token \"" + *iter + "\", \"(\" expected");
    }

    //make new vector of tokens until we match all ()
    vector<string> subCommand;
    int bracketDepth = 1;

    while (++iter != end) {
        if (*iter == "(") { bracketDepth++; }
        if (*iter == ")") { bracketDepth--; }

        if (bracketDepth == 0) {
            subCommand.push_back(";"); //add ; to run command in bracket
            return executeTokens(subCommand);
        }

        subCommand.push_back(*iter);
    }

    //reached end without pairing ()
    throw new std::runtime_error("Unmatched parentheses, ) expected");
}

Relation Database::createRelation(vector<string>& tokens) {
    //format:
    //{ colnames , ... , colnames } { data , ... , data}
    //where # of data is divisible by # columns

    auto tokenIter = tokens.begin();

    //starts with {
    if (*tokenIter != "{") {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \"{\" expected");
    }

    //read the colnames now
    vector<string> colnames;

    do {
        //read a column name
        if (isKeyword(*++tokenIter)) {
            throw new std::runtime_error("Invalid keyword \"" + *tokenIter + "\", column name expected");
        } else {
            colnames.push_back(*tokenIter);
        }

    } while (*++tokenIter == ",");

    //should end with }
    if (*tokenIter != "}") {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \"}\" expected");
    }

    //next should be { for the data
    if (*++tokenIter != "{") {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \"{\" expected");
    }

    //now read the data
    vector<vector<string>> data;
    do {
        vector<string> row;

        //read n data points, where n is # cols
        for (int i=0; i<colnames.size(); i++) {

            //check and store data entry
            if (isKeyword(*++tokenIter)) {
                throw new std::runtime_error("Invalid keyword \"" + *tokenIter + "\", data value expected");
            }
            row.push_back(*tokenIter);

            if (*++tokenIter != "," && *tokenIter != "}") {
                //neither comma nor } -> error
                throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \",\" or \"}\" expected");
            
            } else if (i != colnames.size()-1 && *tokenIter == "}") { 
                //read } when not divisible by # cols -> error
                throw new std::runtime_error("Invalid token \"}\", not enough data given");
            }
        }

        //add row to new relation
        data.push_back(row);

    } while (*tokenIter != "}");

    //should be the end
    if (*++tokenIter != ";") {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \";\" expected");
    }
    
    //no more tokens after
    if (++tokenIter != tokens.end()) {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\" after end of command");
    }

    //make and return relation
    Relation newRel(colnames);
    for (vector<string>& row : data) {
        newRel.addRow(row);
    }
    return newRel;
}

Relation Database::parsePi(vector<string>& tokens) {
    //format:
    //pi { colname , ... , colname } { relation }
    auto tokenIter = tokens.begin();

    if (*tokenIter != "{") {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \"{\" expected");
    }

    vector<string> selectedColumns;

    do {
        //read a column name
        if (isKeyword(*++tokenIter)) {
            throw new std::runtime_error("Invalid keyword \"" + *tokenIter + "\", column name expected");
        } else {
            selectedColumns.push_back(*tokenIter);
        }

    } while (*++tokenIter == ",");

    //should have ended with }
    if (*tokenIter != "}") {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \",\" or \"}\" expected");
    }

    //next should be { for the relation name
    if (*++tokenIter != "{") {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \"{\" expected");
    }

    //now get the relation by name or brackets
    Relation rel;
    if (*++tokenIter == "(") {
        rel = executeParentheses(tokenIter, tokens.end());

    } else if (isKeyword(*tokenIter)) {
        throw new std::runtime_error("Invalid keyword \"" + *tokenIter + "\", relation name expected");
    } else { //get relation by name
        rel = relations.at(*tokenIter);
    }

    //now the closing }
    if (*++tokenIter != "}") {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \"}\" expected");
    }

    //and a ; at the end
    if (*++tokenIter != ";" && ++tokenIter == tokens.end()) {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\", \";\" expected");
    }

    //now we have a list of columns and a relation
    //so ask the relation to filter itself
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
