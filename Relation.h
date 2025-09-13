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
      int numRows() const;
      bool isColPresent(const string&) const;

      //operators
      Relation select(const LogicalExpression&) const;
      Relation project(vector<string>&) const;
      Relation makeUnion(const Relation&) const;
      Relation makeIntersect(const Relation&) const;
      Relation innerJoin(const Relation&) const;

      bool addRow(const vector<string>&);

      void print() const;

  private:
    vector<string> colnames;
    vector<vector<string>> table;

    vector<string> joinRows(const vector<string>&,
                            const vector<string>&,
                            const vector<int>&) const;
};
