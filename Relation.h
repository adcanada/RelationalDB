#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "LogicalExpression.h"

using std::cout;
using std::endl;

using std::string;
using std::vector;

class Relation {
  public:
      Relation() {}
      Relation(const vector<string>& colnames) : 
          colnames(colnames) { }


      const vector<string>& getColNames() const;
      const vector<string>& getRow(int) const;
      bool isColPresent(const string&) const;

      Relation select(const LogicalExpression&) const;
      Relation project(vector<string>&) const;

      bool addRow(const vector<string>&);

      void print() const;

      //operators
      //Relation& select(...);
      //...

  private:
    vector<string> colnames;
    vector<vector<string>> table;
};
