#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility> //std::pair

#include "LogicalExpression.h"

using std::cout;
using std::endl;

using std::string;
using std::vector;
using std::pair;

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
      Relation renameColumn(const string&, const string&);

      Relation makeUnion(const Relation&) const;
      Relation makeIntersect(const Relation&) const;

      Relation innerJoin(const Relation&) const;
      Relation leftOuterJoin(const Relation&) const;
      Relation rightOuterJoin(const Relation&) const;
      Relation fullOuterJoin(const Relation&) const;

      bool addRow(const vector<string>&);

      void print() const;

  private:
    vector<string> colnames;
    vector<vector<string>> table;

    Relation join(const Relation&, bool, bool) const;

    vector<string> nullPad(const vector<string>&, const vector<string>&, const vector<string>&) const;
    vector<pair<int,int>> findMatchingCols(const Relation&) const;
    vector<string> joinRows(const vector<string>&,
                            const vector<string>&,
                            const vector<pair<int,int>>&) const;
};
