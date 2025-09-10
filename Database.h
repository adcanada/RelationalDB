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
    Relation& execute(string&);

  private:
    std::unordered_map<string,Relation> relations;
    //Parser parser;
};
