#pragma once

#include <unordered_map>
#include <string>
#include "Relation.h"
//#include "Parser.h"

using std::string;

class Database {
  public:
    const Relation& getRelation(const string&) const;
    void execute(const string&);

  private:
    std::unordered_map<string,Relation> relations;
    //Parser parser;
};
