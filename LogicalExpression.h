#pragma once

#include <string>
#include <stdexcept>
#include <vector>

using std::string;
using std::vector;

enum class DataOperator { lessThan, greaterThan, equalTo, notEqualTo };
enum class LogicalOperator { AND, OR };

class LogicalExpression {
  public:
    //first order expression: (colname1) (operator) (colname2)
    LogicalExpression(const string&, const DataOperator&, const string&);     

    //second+ order expression: (expression) (AND/OR) (expression)
    LogicalExpression(const LogicalExpression&, const LogicalOperator&, const LogicalExpression&);     

    bool eval(const vector<string>&, const vector<string>&) const; //colnames, row
    
  private:
    bool isFirstOrderExpr;
    
    //used for first order logical expressions
    const string lhsCol; //column name
    const string rhsCol; //column name
    DataOperator dataOp; //comparion

    //used for second+ order logical expressions
    const LogicalExpression* recurseLhs;
    const LogicalExpression* recurseRhs;
    LogicalOperator recurseOp;
};
