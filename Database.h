#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "Relation.h"

using std::string;
using std::vector;

class Database {
  public:
    const Relation& getRelation(const string&) const;
    Relation execute(string&);

  private:
    Relation executeTokens(vector<string>&);
    Relation executeParentheses(vector<string>::iterator&, 
                                const vector<string>::iterator&);
    LogicalExpression parseLogicalExpr(vector<string>::iterator&,
                                       const vector<string>::iterator&);

    Relation createRelation(vector<string>&);
    Relation parseSigma(vector<string>&);
    Relation parsePi(vector<string>&);

    std::unordered_map<string,Relation> relations;
};
