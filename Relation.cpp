#include "Relation.h"

const vector<string>& Relation::getColNames() const {
    return colnames;
}

const vector<string>& Relation::getRow(int rownum) const {
    return table.at(rownum);
}

bool Relation::addRow(const vector<string> &row) {
    if (row.size() != colnames.size()) {
        return false;
    }

    table.push_back(row);
    return true;
}

void Relation::print() const {
    //print headers
    for (string colname : colnames) {
        cout << colname << " ";
    }
    cout << endl;

    //print data
    for (vector<string> row : table) {
        for (int colNum=0; colNum<colnames.size(); colNum++) {

            int textWidth = colnames.at(colNum).length();
            //cout << row.at(colNum);
            printf("%*s", textWidth, (row.at(colNum)+' ').c_str());
        }
        cout << endl;
    }
}

void Relation::a() { cout << "ah" << endl; }
