#include "Relation.h"
#include <algorithm> //std::find, std::unique

const vector<string>& Relation::getColNames() const {
    return colnames;
}

const vector<string>& Relation::getRow(int rownum) const {
    return table.at(rownum);
}

int Relation::numRows() const {
    return table.size();
}

bool Relation::isColPresent(const string& colName) const {
    //if found, then it is present (helpful i know)
    return std::find(colnames.begin(), colnames.end(), colName) != colnames.end();
}

Relation Relation::select(const LogicalExpression& logicExpr) const {
    //make a new relation, evaluate each row and add if true
    Relation newRel(this->colnames);

    for (const vector<string>& row : this->table) {
        if (logicExpr.eval(this->colnames, row)) {
            newRel.addRow(row);
        }
    }

    return newRel;
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


Relation Relation::makeUnion(const Relation& other) const {
    //check same # cols
    if (this->colnames.size() != other.getColNames().size()) {
        throw new std::runtime_error("Cannot union incompatible relations");
    }

    //check all col names match
    for (int i=0; i<colnames.size(); i++) {
        if (this->colnames.at(i) != other.getColNames().at(i)) {
            throw new std::runtime_error("Cannot union incompatible relations");
        }
    }

    Relation newRel(this->colnames);

    //add this table
    for (vector<string> row : this->table) {
        newRel.addRow(row);
    }

    //add other table
    for (int i=0; i<other.numRows(); i++) {
        newRel.addRow( other.getRow(i) );
    }

    return newRel;
}

Relation Relation::makeIntersect(const Relation& other) const {
    //check same # cols
    if (this->colnames.size() != other.getColNames().size()) {
        throw new std::runtime_error("Cannot union incompatible relations");
    }

    //check all col names match
    for (int i=0; i<colnames.size(); i++) {
        if (this->colnames.at(i) != other.getColNames().at(i)) {
            throw new std::runtime_error("Cannot union incompatible relations");
        }
    }
    
    Relation newRel(this->colnames);

    for (vector<string> row : this->table) {
        //check if a matching row is in the other table
        for (int i=0; i<other.numRows(); i++) {
            vector<string> otherRow = other.getRow(i);

            //check each element
            bool rowsEqual = true;
            for (int ele=0; ele<row.size(); ele++) {
                if (row.at(ele) != otherRow.at(ele)) {
                    rowsEqual = false;
                    break;
                }
            }

            if (rowsEqual) {
                newRel.addRow(row);
                break;
            }
        }
    }

    return newRel;
}


Relation Relation::innerJoin(const Relation& other) const {
    //make list of matching cols
    vector<int> thisColIndices;
    vector<int> otherColIndices;

    //also make a list of unique col names
    //(add to this in the loop)
    vector<string> uniqueCols = other.getColNames();

    //find all pairs of col indices connecting the relations
    for (int i=0; i<this->colnames.size(); i++) {
        bool uniqueCol = true;

        for (int j=0; j<other.getColNames().size(); j++) {
            //check if colnames match
            if (this->colnames.at(i) == other.getColNames().at(j)) { 
                uniqueCol = false;
                thisColIndices.push_back(i);
                otherColIndices.push_back(j);
                break;
            }
        }
        
        if (uniqueCol) { uniqueCols.push_back(colnames.at(i)); }
    }

    
    Relation joinedRelation(uniqueCols);

    //loop over all pairs of rows, and if they match
    //on the above columns, add to output

    for (vector<string> thisRow : table) {
        for (int row=0; row<other.numRows(); row++) {
            //check if joinable
            bool rowsJoinable = true;

            //loop over common columns and check if data matches
            for (int i=0; i<thisColIndices.size(); i++) {
                if (thisRow.at(thisColIndices.at(i)) != 
                        other.getRow(row).at(otherColIndices.at(i))) {
                    rowsJoinable = false;
                    break;
                }
            }

            if (rowsJoinable) {
                //join the rows and add to output
                joinedRelation.addRow( 
                    joinRows(thisRow, other.getRow(row), otherColIndices)
                );
            }
        }
    }

    return joinedRelation;
}


vector<string> Relation::joinRows(const vector<string>& row1, const vector<string>& row2, 
                                  const vector<int>& matchesInRow2) const {
    //joins row1 and row2, skipping over the indices in 
    //row2 specified by matchesInRow2
    
    vector<string> joinedRow = row1;

    //start with row1 and adds unique data from row2
    for (int col=0; col<row2.size(); col++) {

        //if i is in the list of matches, skip it
        bool keepData = true;
        for (int index : matchesInRow2) {
            if (col == index) { 
                keepData = false;
                break;
            }
        }
        
        if (keepData) {
            //add data to joined row
            joinedRow.push_back(row2.at(col));
        }
    }

    return joinedRow;
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
