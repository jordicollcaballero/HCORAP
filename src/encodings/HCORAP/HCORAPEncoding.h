//
// Created by jordic on 06/02/24.
//

#ifndef RLSAT_HCORAPENCODING_H
#define RLSAT_HCORAPENCODING_H

#include "encoding.h"
#include "hcorap.h"

class HCORAPEncoding : public Encoding {
    HCORAP * instance;
    bool strat;
    vector<vector<vector<literal>>> x;
    vector<vector<literal>> y;
    vector<vector<literal>> s;
    vector<vector<literal>> su;
    int totalSoft, totalSoftStrat, konstantrevenue;

    vector<clause> softClauses;
    vector<int> weights;
    vector<bool> model;

    int checkAllSoftClauses();


public:
    HCORAPEncoding(HCORAP * instance, bool strat);

    SMTFormula *  encode(int lb = INT_MIN, int ub = INT_MAX);

    void checkSolution(const string & filename);
};


#endif //RLSAT_HCORAPENCODING_H
