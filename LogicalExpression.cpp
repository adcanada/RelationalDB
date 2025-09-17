#include "LogicalExpression.h"
#include <algorithm> //searching vectors
#include <iostream>
using std::cout;

LogicalExpression::LogicalExpression(
        const string& colname1,
        const DataOperator& op,
        const string& value) :
    lhsCol(colname1),
    compareValue(value),
    dataOp(op),
    isFirstOrderExpr(true) { }

LogicalExpression::LogicalExpression(
        const LogicalExpression *lhsExpr,
        const LogicalOperator& op,
        const LogicalExpression *rhsExpr) :
    recurseLhs(lhsExpr),
    recurseRhs(rhsExpr),
    recurseOp(op),
    isFirstOrderExpr(false) { }

bool LogicalExpression::eval(const vector<string>& colnames, const vector<string>& row) const {
    if (isFirstOrderExpr) {
        //find col and save data
        string dataLhs;
        bool foundLhs = false;
        for (int i=0; i<colnames.size(); i++) {
            if (colnames.at(i) == lhsCol) { dataLhs = row.at(i); foundLhs = true; }
        }

        if (!foundLhs) {
            //column doesn't exist
            cout <<"asdads";
            //throw new std::runtime_error("Selected column \""+lhsCol+"\" could not be found");
            throw new std::runtime_error("Selected column could not be found");
        }

        //do the operation
        switch (dataOp) {
        case DataOperator::lessThan:
            return stoi(dataLhs) < stoi(compareValue);
            break;
        case DataOperator::lessOrEqualTo:
            return stoi(dataLhs) <= stoi(compareValue);
            break;
        case DataOperator::greaterThan:
            return stoi(dataLhs) > stoi(compareValue);
            break;
        case DataOperator::greaterOrEqualTo:
            return stoi(dataLhs) >= stoi(compareValue);
            break;
        case DataOperator::equalTo:
            return dataLhs == compareValue;
            break;
        case DataOperator::notEqualTo:
            return dataLhs != compareValue;
            break;
        default:
            throw new std::runtime_error("Invalid operator in logical expression");
        }
    }

    //second+ order expression, must evaluate subclauses
    bool lhsTruthVal = recurseLhs->eval(colnames, row);
    bool rhsTruthVal = recurseRhs->eval(colnames, row);

    //now combine them
    if (recurseOp == LogicalOperator::AND) {
        return lhsTruthVal && rhsTruthVal;
    } else if (recurseOp == LogicalOperator::OR) {
        return lhsTruthVal || rhsTruthVal;
    } else {
        throw new std::logic_error("Invalid operator in logical expression");
    }
}
