#pragma once

#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;

using std::string;
using std::vector;

class Relation {
  public:
      Relation(const vector<string>& colnames) : 
          colnames(colnames) { }


      const vector<string>& getColNames() const;
      const vector<string>& getRow(int) const;

      bool addRow(const vector<string>&);

      void print() const;
      void a();

      //operators
      //Relation& select(...);
      //...

  private:
    vector<string> colnames;
    vector<vector<string>> table;
};
