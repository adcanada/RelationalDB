#include "LogicalExpression.h"
#include <algorithm> //searching vectors

LogicalExpression::LogicalExpression(
        const string& colname1,
        const DataOperator& op,
        const string& colname2) :
    lhsCol(colname1),
    rhsCol(colname2),
    dataOp(op),
    isFirstOrderExpr(true) { }

LogicalExpression::LogicalExpression(
        const LogicalExpression& colname1,
        const LogicalOperator& op,
        const LogicalExpression& colname2) :
    recurseLhs(&colname1),
    recurseRhs(&colname2),
    recurseOp(op),
    isFirstOrderExpr(false) { }

bool LogicalExpression::eval(const vector<string>& colnames, const vector<string>& row) const {
    if (isFirstOrderExpr) {
        //find cols and save data
        string dataLhs, dataRhs;
        bool foundLhs = false;
        bool foundRhs = false;
        for (int i=0; i<colnames.size(); i++) {
            if (colnames.at(i) == lhsCol) { dataLhs = row.at(i); foundLhs = true; }
            if (colnames.at(i) == rhsCol) { dataRhs = row.at(i); foundRhs = true; }
        }

        if (!foundLhs || !foundRhs) {
            //colums don't exist
            throw new std::runtime_error("Selected column(s) could not be found");
        }

        //do the operation
        switch (dataOp) {
        case DataOperator::lessThan:
            return stoi(dataLhs) < stoi(dataRhs);
            break;
        case DataOperator::greaterThan:
            return stoi(dataLhs) > stoi(dataRhs);
            break;
        case DataOperator::equalTo:
            return dataLhs == dataRhs;
            break;
        case DataOperator::notEqualTo:
            return dataLhs != dataRhs;
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
