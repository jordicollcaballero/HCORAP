//
// Created by jordic on 06/02/24.
//

#ifndef RLSAT_RLSATENCODING_H
#define RLSAT_RLSATENCODING_H

#include "encoding.h"
#include "rlsat.h"

class RLSATEncoding : public Encoding {
    RLSAT * instance;
    bool strat;
    vector<vector<vector<literal>>> x;
    vector<vector<literal>> y;
    vector<vector<literal>> s;
    vector<vector<literal>> su;
    int totalSoft, konstantrevenue;
public:
    RLSATEncoding(RLSAT * instance, bool strat);

    SMTFormula *  encode(int lb = INT_MIN, int ub = INT_MAX);

    void checkSolution(const string & filename);
};


#endif //RLSAT_RLSATENCODING_H
