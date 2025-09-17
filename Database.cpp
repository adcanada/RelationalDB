#include "Database.h"
#include "LogicalExpression.h"

#include <algorithm>

using std::find; //find in vector

vector<string> splitIntoTokens(string& command);
string nextToken(string& command);
bool isKeyword(const string&);
void invalidToken(const string&, const string&);

const vector<string> keywords = { 
    "sigma", "select", 
    "pi", "project", 
    "rho", "rename",
    "join", "loutjoin", "routjoin", "foutjoin",
    "union", 
    "intersect", 
    "minus", 
    "!", "=", "<", ">",
    "{", "}", 
    "(", ")", ",", "\"", ";" 
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
        Relation rel = executeParentheses(tokenIter, tokens.end());

        if (*++tokenIter == ";") { return rel; }
        else if (*tokenIter == "union"     ||
                 *tokenIter == "intersect" ||
                 *tokenIter == "minus"     ||
                 *tokenIter == "join"      ||
                 *tokenIter == "loutjoin"  ||
                 *tokenIter == "routjoin"  ||
                 *tokenIter == "foutjoin") {
            return parseBinaryOperator(tokens);
        } else {
            invalidToken(*tokenIter, ";/=/join/union/intersect");
        }

    } else if (*tokenIter == "{") { //start relation definition
        return createRelation(tokens); //pass the { too

    } else if (*tokenIter == "sigma" || *tokenIter == "select") {
        return parseSigma(remainingTokens);

    } else if (*tokenIter == "pi" || *tokenIter == "project") {
        return parsePi(remainingTokens);

    } else if (*tokenIter == "rho" || *tokenIter == "rename") {
        return parseRho(remainingTokens);

    } else { //relation name
        string name = *tokenIter;

        tokenIter++;
        remainingTokens = vector<string>(tokenIter+1, tokens.end());

        if (*tokenIter == "=") { //assigning to a relation
            Relation rval = executeTokens(remainingTokens);
            std::pair<string,Relation> newRelation(name, rval);

            relations.erase(name);
            relations.insert(newRelation);
            return rval;

        } else if (*tokenIter == ";") { //just return this relation
            if (relations.find(name) == relations.end()) { //check if relation exists
                throw new std::runtime_error("Relation \""+name+"\" not found");
            }
            return relations.at(name);

        } else if (*tokenIter == "union"     ||
                   *tokenIter == "intersect" ||
                   *tokenIter == "minus"     ||
                   *tokenIter == "join"      ||
                   *tokenIter == "loutjoin"  ||
                   *tokenIter == "routjoin"  ||
                   *tokenIter == "foutjoin") {
            return parseBinaryOperator(tokens);

        } else {
            invalidToken(*tokenIter, ";/=/join/union/intersect");
        }
    }

    return Relation();
}

Relation Database::executeParentheses(vector<string>::iterator& iter, const vector<string>::iterator& end) {
    //first char should be (
    if (*iter != "(") { 
        invalidToken(*iter, "(");
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
    //{ colnames , ... , colnames } { data , ... , data } ;
    //where # of data is divisible by # columns

    auto tokenIter = tokens.begin();

    //starts with {
    if (*tokenIter != "{") {
        invalidToken(*tokenIter, "{");
    }

    //read the colnames now
    vector<string> colnames;

    do {
        //read a column name
        if (isKeyword(*++tokenIter)) {
            invalidToken(*tokenIter, "column name");
        } else {
            //check if name is taken
            for (string colname : colnames) {
                if (colname == *tokenIter) {
                    throw new std::runtime_error("Duplicate column name \""+colname+"\"");
                }
            }
            colnames.push_back(*tokenIter);
        }

    } while (*++tokenIter == ",");

    //should end with }
    if (*tokenIter != "}") {
        invalidToken(*tokenIter, "}");
    }

    //next should be { for the data
    if (*++tokenIter != "{") {
        invalidToken(*tokenIter, "{");
    }

    //now read the data
    vector<vector<string>> data;
    do {
        vector<string> row;

        //read n data points, where n is # cols
        for (int i=0; i<colnames.size(); i++) {

            //check and store data entry
            if (isKeyword(*++tokenIter)) {
                invalidToken(*tokenIter, "data value");
            }
            row.push_back(*tokenIter);

            if (*++tokenIter != "," && *tokenIter != "}") {
                //neither comma nor } -> error
                invalidToken(*tokenIter, ",\" or \"}");
            
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
        invalidToken(*tokenIter, ";");
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

Relation Database::parseSigma(vector<string>& tokens) {
    //format:
    //sigma { logic } relation ;
    auto tokenIter = tokens.begin();

    //start with {
    if (*tokenIter != "{") {
        invalidToken(*tokenIter, "{");
    }

    //find the closing } and parse the logical expression
    auto endBrace = std::find(++tokenIter, tokens.end(), "}");
    if (endBrace == tokens.end()) {
        throw new std::runtime_error("Missing \"}\" in logical expression");
    }
    //this returns leaving iter on "}"
    LogicalExpression *logicExpr = parseLogicalExpr(tokenIter, endBrace);

    //should be }
    if (*tokenIter != "}") {
        invalidToken(*tokenIter, "}");
    }

    //now get the relation by name or brackets
    Relation rel;
    if (*++tokenIter == "(") {
        rel = executeParentheses(tokenIter, tokens.end());

    } else if (isKeyword(*tokenIter)) {
        invalidToken(*tokenIter, "relation name");
    } else { //get relation by name
        if (relations.find(*tokenIter) == relations.end()) { //check if relation exists
            throw new std::runtime_error("Relation \""+*tokenIter+"\" not found");
        }
        rel = relations.at(*tokenIter);
    }

    //and a ; at the end
    if (*++tokenIter != ";" && ++tokenIter == tokens.end()) {
        invalidToken(*tokenIter, ";");
    }

    //no more tokens after
    if (++tokenIter != tokens.end()) {
        throw new std::runtime_error("Invalid token \"" + *tokenIter + "\" after end of command");
    }

    //now we have a logical expression and a relation
    //so ask the relation to filter itself
    return rel.select(*logicExpr);
}

//will leave iter on the closing brace/parenthesis
LogicalExpression* Database::parseLogicalExpr(vector<string>::iterator& iter,
                                             const vector<string>::iterator& end) {
    //format:
    // {colname} {>/</=/!=} {constant}
    // OR
    // ({logicExpr}) {and/or} ({logicExpr})
    
    if (*iter == "(") { //trying to combine two expressions
        //find closing ) and parse recursively
        auto endParenthesis = std::find(++iter, end, ")");
        if (endParenthesis == end) {
            throw new std::runtime_error("Missing \")\" in logical expression");
        }
        LogicalExpression *lhs = parseLogicalExpr(iter, endParenthesis);

        //read operator
        LogicalOperator logicalOp;
        if (*++iter == "and") {
            logicalOp = LogicalOperator::AND;
        } else if (*iter == "or") {
            logicalOp = LogicalOperator::OR;
        } else {
            invalidToken(*iter, "and/or");
        }

        //read rhs, should also be bracketed
        if (*++iter != "(") {
            invalidToken(*iter, "{");
        }
        endParenthesis = std::find(++iter, end, ")");
        if (endParenthesis == end) {
            throw new std::runtime_error("Missing \")\" in logical expression");
        }
        LogicalExpression *rhs = parseLogicalExpr(iter, endParenthesis);

        //should be at end
        if (++iter != end) {
            invalidToken(*iter, "end of logical expression");
        }

        //combine
        return new LogicalExpression(lhs, logicalOp, rhs);

    } else { //base case, two colnames and an operator between
        string lhs = *iter;
        //should be a column name
        if (isKeyword(lhs)) {
            invalidToken(*iter, "column name");
        }

        DataOperator dataOp;
        if (*++iter == "<")    { dataOp = DataOperator::lessThan; }
        else if (*iter == ">") { dataOp = DataOperator::greaterThan; }
        else if (*iter == "=") { dataOp = DataOperator::equalTo; }
        else if (*iter == "!") {  
            // != is read as two tokens, if not connected then error
            if (*++iter == "=") { dataOp = DataOperator::notEqualTo; }
            else { invalidToken(*iter, "="); }
        } else {
            //invalid operator
            invalidToken(*iter, ">/</=/!=");
        }

        //get rhs
        string rhs = *++iter;
        if (isKeyword(rhs)) {
            invalidToken(*iter, "a constant");
        }

        //should be at end now
        if (++iter != end) {
            invalidToken(*iter, "end of logical expression");
        }

        //combine
        return new LogicalExpression(lhs, dataOp, rhs);
    }
}


Relation Database::parsePi(vector<string>& tokens) {
    //format:
    //pi { colname , ... , colname } relation ;
    auto tokenIter = tokens.begin();

    if (*tokenIter != "{") {
        invalidToken(*tokenIter, "{");
    }

    vector<string> selectedColumns;

    do {
        //read a column name
        if (isKeyword(*++tokenIter)) {
            invalidToken(*tokenIter, "column name");
        } else {
            selectedColumns.push_back(*tokenIter);
        }

    } while (*++tokenIter == ",");

    //should have ended with }
    if (*tokenIter != "}") {
        invalidToken(*tokenIter, ",\" or \"}");
    }

    //now get the relation by name or brackets
    Relation rel;
    if (*++tokenIter == "(") {
        rel = executeParentheses(tokenIter, tokens.end());

    } else if (isKeyword(*tokenIter)) {
        invalidToken(*tokenIter, "relation name");
    } else { //get relation by name
        if (relations.find(*tokenIter) == relations.end()) { //check if relation exists
            throw new std::runtime_error("Relation \""+*tokenIter+"\" not found");
        }
        rel = relations.at(*tokenIter);
    }

    //and a ; at the end
    if (*++tokenIter != ";" && ++tokenIter == tokens.end()) {
        invalidToken(*tokenIter, ";");
    }

    //now we have a list of columns and a relation
    //so ask the relation to filter itself
    return rel.project(selectedColumns);
}


Relation Database::parseRho(vector<string>& tokens) {
    //format:
    //rho { oldcolnames , newcolname } relation ;
    auto tokenIter = tokens.begin();

    if (*tokenIter != "{") {
        invalidToken(*tokenIter, "{");
    }

    string oldcolname;
    if (isKeyword(*++tokenIter)) {
        invalidToken(*tokenIter, "column name");
    } else {
        oldcolname = *tokenIter;
    }

    //comma separato
    if (*++tokenIter != ",") {
        invalidToken(*tokenIter, ",");
    }

    string newcolname;
    if (isKeyword(*++tokenIter)) {
        invalidToken(*tokenIter, "column name");
    } else {
        newcolname = *tokenIter;
    }


    //should have ended with }
    if (*++tokenIter != "}") {
        invalidToken(*tokenIter, ",\" or \"}");
    }

    //now get the relation by name or brackets
    Relation rel;
    if (*++tokenIter == "(") {
        rel = executeParentheses(tokenIter, tokens.end());

    } else if (isKeyword(*tokenIter)) {
        invalidToken(*tokenIter, "relation name");
    } else { //get relation by name
        if (relations.find(*tokenIter) == relations.end()) { //check if relation exists
            throw new std::runtime_error("Relation \""+*tokenIter+"\" not found");
        }
        rel = relations.at(*tokenIter);
    }

    //and a ; at the end
    if (*++tokenIter != ";" && ++tokenIter == tokens.end()) {
        invalidToken(*tokenIter, ";");
    }

    //now we have a list of columns and a relation
    //so ask the relation to filter itself
    return rel.renameColumn(oldcolname, newcolname);
}   


Relation Database::parseBinaryOperator(vector<string>& tokens) {
    auto tokenIter = tokens.begin();

    //get first relation (potentially evaulate bracketed)
    Relation lhs;
    if (*tokenIter == "(") {
        lhs = executeParentheses(tokenIter, tokens.end());

    } else if (isKeyword(*tokenIter)) {
        invalidToken(*tokenIter, "relation name");
    } else { //get relation by name
        if (relations.find(*tokenIter) == relations.end()) { //check if relation exists
            throw new std::runtime_error("Relation \""+*tokenIter+"\" not found");
        }
        lhs = relations.at(*tokenIter);
    }

    //store operator token for now
    string operatorToken = *++tokenIter;

    //get second relation (potentially evaulate brackted)
    Relation rhs;
    if (*++tokenIter == "(") {
        rhs = executeParentheses(tokenIter, tokens.end());

    } else if (isKeyword(*tokenIter)) {
        invalidToken(*tokenIter, "relation name");
    } else { //get relation by name
        if (relations.find(*tokenIter) == relations.end()) { //check if relation exists
            throw new std::runtime_error("Relation \""+*tokenIter+"\" not found");
        }
        rhs = relations.at(*tokenIter);
    }

    //create new relation based on operator
    Relation newRel;
    if (operatorToken == "union") {
        newRel = lhs.makeUnion(rhs);
    } else if (operatorToken == "intersect") {
        newRel = lhs.makeIntersect(rhs);
    } else if (operatorToken == "minus") {
        newRel = lhs.minusRelation(rhs);
    } else if (operatorToken == "join") {
        newRel = lhs.innerJoin(rhs);
    } else if (operatorToken == "loutjoin") {
        newRel = lhs.leftOuterJoin(rhs);
    } else if (operatorToken == "routjoin") {
        newRel = lhs.rightOuterJoin(rhs);
    } else if (operatorToken == "foutjoin") {
        newRel = lhs.fullOuterJoin(rhs);
    } else {
        invalidToken(operatorToken, "union/intsrsect/join/loutjoin/routjoin/foutjoin");
    }

    //should be ; next
    if (*++tokenIter != ";" && ++tokenIter == tokens.end()) {
        invalidToken(*tokenIter, ";");
    }

    return newRel;
}


vector<string> splitIntoTokens(string& command) {
    vector<string> tokens;

    string token = "";
    while (token != ";") {
        //get next token and add if not just a space
        token = nextToken(command);
        if (token == " "  || token == "\t") { continue; }
        tokens.push_back(token);
    }

    return tokens;
}

string nextToken(string& command) {
    if (command.length() == 0) { return ";"; }

    //iterate until token split
    int i=0;
    string tokenSplitChars = ",!=<>\"{}(); ";
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

void invalidToken(const string& token, const string& expected) {
    throw new std::runtime_error("Invalid token \"" + token + "\": \"" + expected + "\" expected");
}
