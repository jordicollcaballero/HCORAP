//
// Created by jordic on 05/03/24.
//

#ifndef RLSAT_RLSATNSERVICESENCODING_H
#define RLSAT_RLSATNSERVICESENCODING_H

#include "encoding.h"
#include "rlsat.h"


class RLSATNServicesEncoding : public Encoding{
    RLSAT * instance;
    vector<vector<vector<literal>>> x;
    vector<vector<literal>> y;
    vector<vector<literal>> su;
    int totalSoftStrat;

    vector<clause> softClauses;
    vector<int> weights;
    vector<bool> model;

    int checkAllSoftClauses();


public:
    RLSATNServicesEncoding(RLSAT * instance);

    SMTFormula *  encode(int lb = INT_MIN, int ub = INT_MAX);

    void checkSolution(const string & filename);
};


#endif //RLSAT_RLSATNSERVICESENCODING_H
