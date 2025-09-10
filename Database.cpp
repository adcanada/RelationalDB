#include "Database.h"
#include "LogicalExpression.h"

const Relation& Database::getRelation(const string &name) const {
    return this->relations.at(name);
}

void Database::execute(const string& command) {
    cout << "Parsing Command: " << command << endl;

    //test relation
    vector<string> colnames = {"EID", "Cookies", "Age"};
    vector<string> r1 = {"E1", "12", "32"};
    vector<string> r2 = {"E2", "15", "12"};
    vector<string> r3 = {"E3", "22", "22"};
    Relation rel(colnames);
    rel.addRow(r1);
    rel.addRow(r2);
    rel.addRow(r3);
    rel.print();
    
    LogicalExpression expr("Cookies", DataOperator::lessThan, "Age");
    cout << "r1: " << expr.eval(colnames, r1) << endl;
    cout << "r2: " << expr.eval(colnames, r2) << endl;
    cout << "r3: " << expr.eval(colnames, r3) << endl;
}
