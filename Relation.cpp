#include "Relation.h"
#include <algorithm> //std::find, std::unique

const vector<string>& Relation::getColNames() const {
    return colnames;
}

const vector<string>& Relation::getRow(int rownum) const {
    return table.at(rownum);
}

bool Relation::isColPresent(const string& colName) const {
    //if found, then it is present (helpful i know)
    return std::find(colnames.begin(), colnames.end(), colName) != colnames.end();
}

Relation Relation::project(vector<string>& columns) const {
    //remove potential duplicates in the column list 
    //do this using std::unique (required sorted)
    std::sort( columns.begin(), columns.end() );
    columns.erase( std::unique(columns.begin(), columns.end()), columns.end() );

    //check that all selected columns are actually in this relation
    //and get the indices while doing this
    vector<int> columnIndices;
    for (string colName : columns) {
        if (!isColPresent(colName)) {
            throw new std::runtime_error("Column \"" + colName + "\" not found");
        }

        //calculate column index with iteration arithmetic
        auto it = std::find( colnames.begin(), colnames.end(), colName);
        columnIndices.push_back( it - colnames.begin() );
    }

    //create new relation
    Relation newRel( columns );
    for (const vector<string>& row : this->table) {
        //add the appropriate data from this row
        vector<string> newRow;
        for (int index : columnIndices) {
            newRow.push_back(row.at(index));
        }

        newRel.addRow(newRow);
    }

    //done
    return newRel;
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
        int textWidth = std::max( 10, int(colname.length()) );
        printf("%*s ", textWidth, colname.c_str());
    }
    cout << endl;

    //print data
    for (vector<string> row : table) {
        for (int colNum=0; colNum<colnames.size(); colNum++) {

            int textWidth = std::max( 10, int(colnames.at(colNum).length()) );
            //cout << row.at(colNum);
            printf("%*s ", textWidth, row.at(colNum).c_str());
        }
        cout << endl;
    }
}
