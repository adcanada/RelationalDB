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


vector<pair<int,int>> Relation::findMatchingCols(const Relation& other) const {
    vector<pair<int,int>> matches;

    //loop over all pairs of columns
    for (int thisIndex=0; thisIndex<this->colnames.size(); thisIndex++) {
        for (int otherIndex=0; otherIndex<other.getColNames().size(); otherIndex++) {
            //if matching, add to list of matches
            if (this->colnames.at(thisIndex) == other.getColNames().at(otherIndex)) {
                pair<int,int> match(thisIndex, otherIndex);
                matches.push_back(match);
                break; //no duplicate colnames, so only 1 match possible
            }
        }
    }

    return matches;
}


vector<string> Relation::joinRows(const vector<string>& row1, const vector<string>& row2, 
                                  const vector<pair<int,int>>& matchingCols) const {
    //joins row1 and row2, skipping over the indices in 
    //row2 specified by matchesInRow2
    
    vector<string> joinedRow = row1;

    //start with row1 and adds unique data from row2
    for (int col=0; col<row2.size(); col++) {

        //if col is in the list of matches for 'other', skip it
        bool keepData = true;
        for (pair<int,int> matchingIndices : matchingCols) {
            if (col == matchingIndices.second) {
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


vector<string> Relation::nullPad(const vector<string>& rowData, const vector<string>& rowColnames,
                                 const vector<string>& allUniqueColnames) const {
    vector<string> result;

    for (int i=0; i<allUniqueColnames.size(); i++) {

        //check if we have the data we need
        bool foundData = false;
        for (int dataCol=0; dataCol<rowColnames.size(); dataCol++) {

            if (rowColnames.at(dataCol) == allUniqueColnames.at(i)) {
                //we have it, add the data to result
                result.push_back(rowData.at(dataCol));
                foundData = true;
                break;
            }

        }

        //do null padding if neccecary
        if (!foundData) { result.push_back(""); }
    }

    return result;
}


Relation Relation::join(const Relation& other, bool keepLhs, bool keepRhs) const {

    vector<pair<int,int>> matchingCols = findMatchingCols(other);

    vector<string> uniqueCols = this->colnames; //will add non-duplicate names from other

    for (const string& otherColname : other.getColNames()) {
        //check if dupe
        bool duplicate = false;
        for (pair<int,int> matchingIndices : matchingCols) {
            if (otherColname == other.getColNames().at(matchingIndices.second)) {
                //is duplicate
                duplicate = true;
                break;
            }
        }

        if (!duplicate) { uniqueCols.push_back(otherColname); }
    }
    
    Relation joinedRelation(uniqueCols);

    //loop over all pairs of rows, and if they match
    //on the above columns, add to output
    
    //keep track of which rows have been joined (use for outer joins)
    vector<int> joinedRightRows;

    for (int thisRow=0; thisRow<this->table.size(); thisRow++) {
        //try find a matching row
        bool foundJoinablePair = false;

        for (int otherRow=0; otherRow<other.numRows(); otherRow++) {
            //check if this specific pair is joinable
            bool rowsJoinable = true;

            //loop over common columns and check if data matches
            for (int i=0; i<matchingCols.size(); i++) {
                
                if (this->table.at(thisRow).at(matchingCols.at(i).first) != 
                        other.getRow(otherRow).at(matchingCols.at(i).second)) {
                    rowsJoinable = false;
                    break;
                }
            }

            if (rowsJoinable) {
                //join the rows and add to output
                joinedRelation.addRow( 
                    joinRows(this->table.at(thisRow), other.getRow(otherRow), matchingCols)
                );
                joinedRightRows.push_back(otherRow);
                foundJoinablePair = true;
            }
            
        }

        //deal with outer joins if no match was found
        if (!foundJoinablePair) { 
            if (keepLhs) {
                joinedRelation.addRow( nullPad(this->table.at(thisRow), this->colnames, uniqueCols) );
            }
        }
    }

    if (keepRhs) {
        //add the unjoined rows from rhs
        for (int row=0; row<other.numRows(); row++) {
            
            //check if its never been joined
            bool neverJoined = true;
            for (int checkRow : joinedRightRows) {
                if (row == checkRow) {
                    neverJoined = false;
                    break;
                }
            }

            if (neverJoined) {
                joinedRelation.addRow( nullPad(other.getRow(row), other.getColNames(), uniqueCols) );
            }
        }
    }

    return joinedRelation;
}

Relation Relation::innerJoin(const Relation& other) const {
    return this->join(other, false, false);
}

Relation Relation::leftOuterJoin(const Relation& other) const {
    return this->join(other, true, false);
}

Relation Relation::rightOuterJoin(const Relation& other) const {
    return this->join(other, false, true);
}

Relation Relation::fullOuterJoin(const Relation& other) const {
    return this->join(other, true, true);
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

    cout << endl;
}
